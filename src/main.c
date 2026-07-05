/*
 * Copyright (c) 2026
 * SPDX-License-Identifier: Apache-2.0
 *
 * Aplicacao principal da campainha silenciosa.
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/zbus/zbus.h>

#include "hist.h"
#include "msgs.h"

LOG_MODULE_REGISTER(campainha, LOG_LEVEL_INF);

#define BLINK_URGENT_MS 200
#define BLINK_CALM_MS   800

/* Bits do flag de "o que salvar" no work deferido. */
#define SAVE_BIT_BELL 0
#define SAVE_BIT_DND  1
#define SAVE_BIT_HIST 2

/* Estado global. */
static bool bell_active;
static bool do_not_disturb;

/* Offset entre o relogio de parede (epoch em segundos) e o uptime local.
 * Fica em zero ate o usuario rodar `time set <epoch>` no shell.
 */
static int64_t epoch_offset_s;

/* Padrao do LED e nivel atual (piscando entre ligado/desligado). */
enum led_pattern {
	LED_PAT_OFF,
	LED_PAT_URGENT,
	LED_PAT_CALM,
};

static enum led_pattern led_pat;
static bool led_on_state;
static struct k_work_delayable blink_work;

/* Save deferido: listeners marcam bits e agendam; o handler faz o
 * settings_save_one no system workqueue, que tem pilha grande.
 * Isso evita estourar o stack do thread bt_tx_processor.
 */
static struct k_work save_work;
static atomic_t save_flags;

/* LED WS2812 declarado no devicetree via alias led-strip. */
static const struct device *const led_strip_dev = DEVICE_DT_GET(DT_ALIAS(led_strip));

/* Rotulos legiveis para logs. */
static const char *on_off_str(bool v)
{
	if (v) {
		return "ON";
	}
	return "OFF";
}

/* Padrao do LED em repouso: sempre apagado.
 * Em DND o LED permanece desligado (alertas sao ignorados);
 * fora de DND, sem alerta ativo, tambem fica apagado.
 */
static enum led_pattern idle_pattern(void)
{
	return LED_PAT_OFF;
}

/* Retorna o horario atual em segundos desde 1970 (epoch).
 * Se o usuario ainda nao ajustou o relogio, retorna apenas os
 * segundos desde o boot (o que sera mostrado como uma data de 1970).
 */
static int64_t now_epoch_s(void)
{
	return epoch_offset_s + (k_uptime_get() / 1000);
}

/* Escreve uma cor RGB no unico pixel da strip. */
static void led_hw(uint8_t r, uint8_t g, uint8_t b)
{
	struct led_rgb pixel = {
		.r = r,
		.g = g,
		.b = b,
	};

	led_strip_update_rgb(led_strip_dev, &pixel, 1);
}

/* Retorna o intervalo (em ms) do padrao ativo. */
static uint32_t pattern_interval_ms(enum led_pattern p)
{
	if (p == LED_PAT_URGENT) {
		return BLINK_URGENT_MS;
	}
	return BLINK_CALM_MS;
}

/* Alterna o LED e reagenda conforme o padrao ativo. */
static void blink_work_handler(struct k_work *work)
{
	uint8_t red;

	ARG_UNUSED(work);

	if (led_pat == LED_PAT_OFF) {
		led_hw(0, 0, 0);
		return;
	}

	led_on_state = !led_on_state;

	red = 0;
	if (led_on_state) {
		red = 255;
	}
	led_hw(red, 0, 0);

	k_work_reschedule(&blink_work, K_MSEC(pattern_interval_ms(led_pat)));
}

/* Troca o padrao do LED e dispara o handler agora. */
static void led_set_pattern(enum led_pattern p)
{
	led_pat = p;
	led_on_state = false;
	k_work_reschedule(&blink_work, K_NO_WAIT);
}

/* Handler do save deferido: consome os flags e faz as escritas NVS.
 * Roda no system workqueue (pilha CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE).
 */
static void save_work_handler(struct k_work *work)
{
	atomic_val_t flags = atomic_clear(&save_flags);

	ARG_UNUSED(work);

	if (flags & BIT(SAVE_BIT_BELL)) {
		settings_save_one("campainha/bell", &bell_active,
				  sizeof(bell_active));
	}
	if (flags & BIT(SAVE_BIT_DND)) {
		settings_save_one("campainha/dnd", &do_not_disturb,
				  sizeof(do_not_disturb));
	}
	if (flags & BIT(SAVE_BIT_HIST)) {
		settings_save_one("campainha/hist", &hist_state,
				  sizeof(hist_state));
	}
}

/* Marcam o que deve ser salvo e delegam ao system workqueue. */
static void save_bell(void)
{
	atomic_set_bit(&save_flags, SAVE_BIT_BELL);
	k_work_submit(&save_work);
}

static void save_dnd(void)
{
	atomic_set_bit(&save_flags, SAVE_BIT_DND);
	k_work_submit(&save_work);
}

static void save_hist(void)
{
	atomic_set_bit(&save_flags, SAVE_BIT_HIST);
	k_work_submit(&save_work);
}

/* Handler do settings: restaura bell, dnd e historico do storage. */
static int campainha_settings_set(const char *name, size_t len,
				  settings_read_cb read_cb, void *cb_arg)
{
	if (settings_name_steq(name, "bell", NULL) && len == sizeof(bell_active)) {
		read_cb(cb_arg, &bell_active, sizeof(bell_active));
		return 0;
	}
	if (settings_name_steq(name, "dnd", NULL) && len == sizeof(do_not_disturb)) {
		read_cb(cb_arg, &do_not_disturb, sizeof(do_not_disturb));
		return 0;
	}
	if (settings_name_steq(name, "hist", NULL) && len == sizeof(hist_state)) {
		read_cb(cb_arg, &hist_state, sizeof(hist_state));
		return 0;
	}
	return -ENOENT;
}

SETTINGS_STATIC_HANDLER_DEFINE(campainha, "campainha", NULL,
			       campainha_settings_set, NULL, NULL);

/* Canal ZBUS: toque da campainha (BLE ou shell). */
ZBUS_CHAN_DEFINE(ring_chan, struct ring_msg, NULL, NULL,
		 ZBUS_OBSERVERS_EMPTY, ZBUS_MSG_INIT(.pressed = false));

/* Policy: em DND registra ignorado; senao acende LED urgente. */
static void ring_listener_cb(const struct zbus_channel *chan)
{
	const struct ring_msg *msg = zbus_chan_const_msg(chan);

	if (!msg->pressed) {
		return;
	}

	if (do_not_disturb) {
		LOG_INF("Toque ignorado (DND ativo)");
		hist_add(now_epoch_s(), true);
		save_hist();
		return;
	}

	bell_active = true;
	LOG_INF("Campainha acionada, LED URGENTE");
	led_set_pattern(LED_PAT_URGENT);
	hist_add(now_epoch_s(), false);
	save_hist();
	save_bell();
}

ZBUS_LISTENER_DEFINE(ring_listener, ring_listener_cb);
ZBUS_CHAN_ADD_OBS(ring_chan, ring_listener, 0);

/* Toque longo alterna DND; curto silencia o alerta ativo. */
static void btn_listener_cb(const struct zbus_channel *chan)
{
	const struct btn_evt *evt = zbus_chan_const_msg(chan);

	if (evt->long_press) {
		do_not_disturb = !do_not_disturb;
		LOG_INF("DND: %s", on_off_str(do_not_disturb));
		led_set_pattern(idle_pattern());
		save_dnd();
		return;
	}

	if (bell_active) {
		LOG_INF("Silenciando alerta");
		bell_active = false;
		led_set_pattern(idle_pattern());
		save_bell();
	}
}

ZBUS_LISTENER_DEFINE(main_btn_listener, btn_listener_cb);
ZBUS_CHAN_ADD_OBS(btn_chan, main_btn_listener, 0);

/* Shell: publica um toque simulado no ring_chan. */
static int cmd_ring(const struct shell *sh, size_t argc, char **argv)
{
	struct ring_msg msg = {
		.pressed = true,
	};

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(sh, "Publicando toque...");
	return zbus_chan_pub(&ring_chan, &msg, K_MSEC(100));
}

/* Shell: apaga o LED e limpa o alerta. */
static int cmd_off(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	bell_active = false;
	led_set_pattern(idle_pattern());
	save_bell();
	shell_print(sh, "Alerta limpo");
	return 0;
}

/* Shell: "mode" mostra estado; "mode dnd" alterna DND. */
static int cmd_mode(const struct shell *sh, size_t argc, char **argv)
{
	if (argc >= 2 && strcmp(argv[1], "dnd") == 0) {
		do_not_disturb = !do_not_disturb;
		led_set_pattern(idle_pattern());
		save_dnd();
	}
	shell_print(sh, "DND=%s bell=%s",
		    on_off_str(do_not_disturb),
		    on_off_str(bell_active));
	return 0;
}

/* Shell: `time` mostra hora atual, `time set <epoch>` ajusta relogio.
 * Uso tipico no PC: date +%s (retorna o epoch em segundos UTC).
 */
static int cmd_time(const struct shell *sh, size_t argc, char **argv)
{
	time_t now;
	struct tm cal;
	char buf[32];

	if (argc >= 3 && strcmp(argv[1], "set") == 0) {
		int64_t epoch = strtoll(argv[2], NULL, 10);

		epoch_offset_s = epoch - (k_uptime_get() / 1000);
		shell_print(sh, "Horario ajustado");
		return 0;
	}

	now = (time_t)now_epoch_s();
	gmtime_r(&now, &cal);
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S UTC", &cal);
	shell_print(sh, "Agora: %s", buf);
	return 0;
}

/* Rotulo legivel para uma entrada do historico. */
static const char *hist_label(const struct hist_entry *e)
{
	if (e->ignored) {
		return "ignorado";
	}
	return "alertado";
}

/* Shell: lista o historico do mais antigo (0) para o mais recente. */
static int cmd_hist(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (hist_len() == 0) {
		shell_print(sh, "Historico vazio");
		return 0;
	}

	for (size_t i = 0; i < hist_len(); i++) {
		const struct hist_entry *e = hist_get(i);
		time_t t = (time_t)e->ms;
		struct tm cal;
		char buf[32];

		gmtime_r(&t, &cal);
		strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S UTC", &cal);
		shell_print(sh, "[%u] %s %s",
			    (unsigned int)i, buf, hist_label(e));
	}
	return 0;
}

SHELL_CMD_REGISTER(ring, NULL, "Simula toque BLE",               cmd_ring);
SHELL_CMD_REGISTER(off,  NULL, "Apaga LED e limpa alerta",       cmd_off);
SHELL_CMD_REGISTER(mode, NULL, "mode [dnd]: mostra/alterna DND", cmd_mode);
SHELL_CMD_REGISTER(hist, NULL, "Lista o historico",              cmd_hist);
SHELL_CMD_REGISTER(time, NULL, "time | time set <epoch>",        cmd_time);

/* Restaura o padrao do LED depois de carregar o estado do settings.
 * Se havia um alerta ativo (bell) antes do reset, retoma o padrao urgente.
 * Em qualquer outro caso (inclusive DND) o LED fica apagado.
 */
static void restore_led_pattern(void)
{
	if (bell_active) {
		led_set_pattern(LED_PAT_URGENT);
	}
}

/* Inicializa LED, carrega settings e restaura padrao coerente. */
int main(void)
{
	LOG_INF("Campainha silenciosa iniciada");

	if (!device_is_ready(led_strip_dev)) {
		LOG_ERR("LED strip nao esta pronto");
		return -ENODEV;
	}

	k_work_init_delayable(&blink_work, blink_work_handler);
	k_work_init(&save_work, save_work_handler);
	led_hw(0, 0, 0);

	settings_subsys_init();
	settings_load();

	LOG_INF("Estado: bell=%d dnd=%d hist=%u",
		bell_active, do_not_disturb, hist_state.count);

	restore_led_pattern();

	return 0;
}
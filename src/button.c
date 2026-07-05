/*
 * Copyright (c) 2026
 * SPDX-License-Identifier: Apache-2.0
 *
 * Driver do botao fisico da campainha.
 *
 * Detecta cliques curtos e longos com debounce por software e
 * publica os eventos no canal ZBUS btn_chan para consumo pela
 * aplicacao principal.
 */

#include <stdbool.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "msgs.h"

LOG_MODULE_REGISTER(button, LOG_LEVEL_INF);

#define LONG_PRESS_MS 800
#define DEBOUNCE_MS   50

ZBUS_CHAN_DEFINE(btn_chan,
		 struct btn_evt,
		 NULL,
		 NULL,
		 ZBUS_OBSERVERS_EMPTY,
		 ZBUS_MSG_INIT(0));

static const struct gpio_dt_spec sw = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);

static struct gpio_callback btn_cb_data;
static struct k_work_delayable debounce_work;
static int64_t press_start_ms;

/* Roda depois do sinal estabilizar: classifica o toque e publica. */
static void debounce_handler(struct k_work *work)
{
	struct btn_evt evt;
	int64_t duration;
	int val;

	ARG_UNUSED(work);

	val = gpio_pin_get_dt(&sw);

	if (val == 1) {
		/* Borda de descida confirmada: botao pressionado. */
		press_start_ms = k_uptime_get();
		return;
	}

	/* Borda de subida confirmada: botao solto. */
	if (press_start_ms == 0) {
		return;
	}

	duration = k_uptime_get() - press_start_ms;
	if (duration < 0) {
		return;
	}

	evt.ms = duration;
	evt.long_press = (duration >= LONG_PRESS_MS);

	if (evt.long_press) {
		LOG_INF("Toque LONGO (%lld ms)", duration);
	} else {
		LOG_INF("Toque CURTO (%lld ms)", duration);
	}

	zbus_chan_pub(&btn_chan, &evt, K_NO_WAIT);
	press_start_ms = 0;
}

/* Reagenda o debounce a cada borda: espera o sinal estabilizar. */
static void on_edge(const struct device *port, struct gpio_callback *cb, uint32_t pins)
{
	ARG_UNUSED(port);
	ARG_UNUSED(cb);
	ARG_UNUSED(pins);

	k_work_reschedule(&debounce_work, K_MSEC(DEBOUNCE_MS));
}

/* Configura o GPIO em interrupcao nas duas bordas e registra o callback. */
static int button_init(void)
{
	int err;

	if (!gpio_is_ready_dt(&sw)) {
		LOG_ERR("GPIO do botao nao esta pronto");
		return -ENODEV;
	}

	k_work_init_delayable(&debounce_work, debounce_handler);

	err = gpio_pin_configure_dt(&sw, GPIO_INPUT);
	if (err) {
		LOG_ERR("Falha ao configurar GPIO (err %d)", err);
		return err;
	}

	err = gpio_pin_interrupt_configure_dt(&sw, GPIO_INT_EDGE_BOTH);
	if (err) {
		LOG_ERR("Falha ao configurar interrupcao (err %d)", err);
		return err;
	}

	gpio_init_callback(&btn_cb_data, on_edge, BIT(sw.pin));

	err = gpio_add_callback(sw.port, &btn_cb_data);
	if (err) {
		LOG_ERR("Falha ao registrar callback (err %d)", err);
		return err;
	}

	LOG_INF("Botao sw1 pronto (debounce %d ms)", DEBOUNCE_MS);
	return 0;
}

SYS_INIT(button_init, APPLICATION, 90);

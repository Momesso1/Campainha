/*
 * Copyright (c) 2026
 * SPDX-License-Identifier: Apache-2.0
 *
 * Testes automatizados do modulo button.c usando ZTEST + GPIO emulado.
 * Roda 100% em simulacao no native_sim, sem hardware.
 */

#include <zephyr/ztest.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/zbus/zbus.h>

#include "msgs.h"

/* Variavel que guarda o ultimo evento recebido para inspecao. */
static struct btn_evt last;

/* Semaforo binario: o listener libera quando recebe evento. */
static K_SEM_DEFINE(got, 0, 1);

/* Copia a mensagem recebida e sinaliza o semaforo. */
static void capture(const struct zbus_channel *chan)
{
	last = *(const struct btn_evt *)zbus_chan_const_msg(chan);
	k_sem_give(&got);
}

ZBUS_LISTENER_DEFINE(test_lst, capture);
ZBUS_CHAN_ADD_OBS(btn_chan, test_lst, 0);

/* Pino do botao vindo do overlay via alias sw1. */
static const struct gpio_dt_spec sw = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);

/* Simula um aperto fisico de `ms` milissegundos (ACTIVE_LOW). */
static void simula_toque(uint32_t ms)
{
	gpio_emul_input_set(sw.port, sw.pin, 0);   /* borda de descida: aperta */
	k_msleep(ms);
	gpio_emul_input_set(sw.port, sw.pin, 1);   /* borda de subida: solta */
}

/* Executa antes de cada teste: garante estado inicial "solto".
 * Sem isso, o primeiro teste pode falhar porque o gpio_emul nasce
 * em nivel fisico 0 (que, com ACTIVE_LOW, e o estado "pressionado")
 * e o simula_toque nao gera a primeira borda de descida.
 */
static void before(void *fixture)
{
	ARG_UNUSED(fixture);

	/* Coloca o pino em nivel fisico 1 (solto para ACTIVE_LOW). */
	gpio_emul_input_set(sw.port, sw.pin, 1);

	/* Espera o debounce processar essa borda (50 ms) e sobra folga
	 * para o handler ver val=0, notar que press_start_ms==0 e sair.
	 */
	k_msleep(150);

	/* Descarta qualquer evento espurio deixado por testes anteriores. */
	k_sem_reset(&got);
}

ZTEST_SUITE(button, NULL, NULL, before, NULL, NULL);

/* Toque de 100 ms deve ser classificado como CURTO. */
ZTEST(button, test_toque_curto)
{
	simula_toque(100);

	zassert_ok(k_sem_take(&got, K_SECONDS(1)),
		   "nao recebi evento no btn_chan");
	zassert_false(last.long_press, "deveria ser curto");
	zassert_true(last.ms >= 100 && last.ms < 800,
		     "duracao fora do esperado: %lld", last.ms);
}

/* Segurar 900 ms (> 800) deve ser classificado como LONGO. */
ZTEST(button, test_toque_longo)
{
	simula_toque(900);

	zassert_ok(k_sem_take(&got, K_SECONDS(2)),
		   "nao recebi evento no btn_chan");
	zassert_true(last.long_press, "deveria ser longo");
	zassert_true(last.ms >= 800, "duracao curta demais: %lld", last.ms);
}

/* Dois toques seguidos devem gerar dois eventos distintos (nao "engasgar"). */
ZTEST(button, test_dois_toques_seguidos)
{
	simula_toque(120);
	zassert_ok(k_sem_take(&got, K_SECONDS(1)),
		   "primeiro evento nao chegou");
	zassert_false(last.long_press);

	simula_toque(150);
	zassert_ok(k_sem_take(&got, K_SECONDS(1)),
		   "segundo evento nao chegou");
	zassert_false(last.long_press);
}
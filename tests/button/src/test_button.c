/*
 * Copyright (c) 2026
 * SPDX-License-Identifier: Apache-2.0
 *
 * Testes automatizados do modulo button.c usando ZTEST + GPIO emulado.
 * Roda 100% em simulacao no native_sim, sem hardware.
 */

#include <zephyr/ztest.h>                     /* framework de testes */
#include <zephyr/kernel.h>                    /* k_msleep, k_sem */
#include <zephyr/drivers/gpio.h>              /* gpio_dt_spec */
#include <zephyr/drivers/gpio/gpio_emul.h>    /* gpio_emul_input_set */
#include <zephyr/zbus/zbus.h>                 /* zbus para receber eventos */

/* Replica da struct definida em button.c. Precisa ser identica em
 * layout porque vamos receber a mensagem pelo zbus.
 */
struct btn_evt {
	bool long_press;     /* true = toque longo, false = curto */
	int64_t ms;          /* duracao real do aperto em ms */
};

/* Declara que o canal "btn_chan" existe em outro .c (button.c).
 * O linker conecta os dois na hora de gerar o binario.
 */
ZBUS_CHAN_DECLARE(btn_chan);

/* Variavel que guarda o ULTIMO evento recebido, para o teste inspecionar. */
static struct btn_evt last;

/* Semaforo binario: o listener libera quando recebe evento;
 * o teste espera nele para nao ler 'last' antes da hora.
 */
static K_SEM_DEFINE(got, 0, 1);

/* Callback invocado automaticamente pelo zbus quando button.c publicar
 * em btn_chan. Copia a mensagem e sinaliza o semaforo.
 */
static void capture(const struct zbus_channel *chan)
{
	last = *(const struct btn_evt *)zbus_chan_const_msg(chan);
	k_sem_give(&got);
}

/* Define este teste como observador do canal btn_chan. */
ZBUS_LISTENER_DEFINE(test_lst, capture);
ZBUS_CHAN_ADD_OBS(btn_chan, test_lst, 0);

/* Pega o pino do botao a partir do alias sw1 (vem do overlay).
 * Usado para injetar bordas no driver emulado.
 */
static const struct gpio_dt_spec sw = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);

/* Helper: simula um aperto fisico de 'ms' milissegundos.
 * Nivel 0 = pressionado, nivel 1 = solto (porque o overlay e' ACTIVE_LOW).
 * O button.c vai detectar as bordas exatamente como faria com HW real.
 */
static void simula_toque(uint32_t ms)
{
	gpio_emul_input_set(sw.port, sw.pin, 0);   /* borda de descida: aperta */
	k_msleep(ms);                              /* mantem pressionado */
	gpio_emul_input_set(sw.port, sw.pin, 1);   /* borda de subida: solta */
}

/* Declara a suite de testes "button" sem hooks de setup/teardown. */
ZTEST_SUITE(button, NULL, NULL, NULL, NULL, NULL);

/* TESTE 1 ---------------------------------------------------------------
 * Verifica que um toque rapido (100 ms) e' classificado como CURTO.
 */
ZTEST(button, test_toque_curto)
{
	/* Zera o semaforo para nao pegar evento de teste anterior. */
	k_sem_reset(&got);

	/* Simula o aperto. */
	simula_toque(100);

	/* Espera ate 1 segundo o evento aparecer no zbus. */
	zassert_ok(k_sem_take(&got, K_SECONDS(1)),
		   "nao recebi evento no btn_chan");

	/* Toque de 100 ms tem que ser CURTO. */
	zassert_false(last.long_press, "deveria ser curto");

	/* Duracao medida tem que ser proxima de 100 ms, e abaixo do
	 * threshold de 800 ms definido em button.c.
	 */
	zassert_true(last.ms >= 100 && last.ms < 800,
		     "duracao fora do esperado: %lld", last.ms);
}

/* TESTE 2 ---------------------------------------------------------------
 * Verifica que segurar 900 ms (> 800) e' classificado como LONGO.
 */
ZTEST(button, test_toque_longo)
{
	k_sem_reset(&got);
	simula_toque(900);

	/* Da mais tempo aqui porque o aperto em si dura quase 1 segundo. */
	zassert_ok(k_sem_take(&got, K_SECONDS(2)),
		   "nao recebi evento no btn_chan");

	zassert_true(last.long_press, "deveria ser longo");
	zassert_true(last.ms >= 800,
		     "duracao curta demais: %lld", last.ms);
}

/* TESTE 3 ---------------------------------------------------------------
 * Verifica que dois toques seguidos geram dois eventos distintos
 * (regressao: garantir que nao "engasga" no segundo aperto).
 */
ZTEST(button, test_dois_toques_seguidos)
{
	k_sem_reset(&got);

	simula_toque(120);
	zassert_ok(k_sem_take(&got, K_SECONDS(1)));
	zassert_false(last.long_press);

	simula_toque(150);
	zassert_ok(k_sem_take(&got, K_SECONDS(1)));
	zassert_false(last.long_press);
}
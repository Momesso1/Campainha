/*
 * Copyright (c) 2026
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>             /* k_uptime_get, SYS_INIT, tipos basicos */
#include <zephyr/drivers/gpio.h>       /* API de GPIO (gpio_dt_spec, gpio_pin_*) */
#include <zephyr/logging/log.h>        /* LOG_INF e cia */
#include <zephyr/zbus/zbus.h>          /* canais zbus */

LOG_MODULE_REGISTER(button, LOG_LEVEL_INF);
/* Registra este arquivo como modulo de log "button" (nivel INFO). */

/* Estrutura da mensagem publicada quando o botao e' solto. */
struct btn_evt {
	bool long_press;     /* true se foi toque longo, false se curto */
	int64_t ms;          /* duracao em milissegundos do aperto */
};

/* Canal zbus onde o evento sera publicado. Quem quiser reagir
 * (ex.: policy da campainha) se inscreve neste canal.
 */
ZBUS_CHAN_DEFINE(btn_chan,                 /* nome do canal */
		 struct btn_evt,           /* tipo da mensagem */
		 NULL,                     /* validador (nao usado) */
		 NULL,                     /* user_data (nao usado) */
		 ZBUS_OBSERVERS_EMPTY,     /* sem observadores iniciais */
		 ZBUS_MSG_INIT(0));        /* valor inicial zerado */

/* Pega o GPIO referente ao alias "sw1" definido no devicetree overlay.
 * Trocar de placa = trocar o overlay, sem mexer neste .c.
 */
static const struct gpio_dt_spec sw = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);

/* Estrutura interna que o kernel usa para guardar o callback do GPIO. */
static struct gpio_callback cb;

/* Guarda o instante (ms desde o boot) em que o botao foi apertado,
 * para calcular a duracao quando ele for solto.
 */
static int64_t t_down;

/* Limite em ms: acima disso, conta como "toque longo". */
#define LONG_MS 800

/* Callback chamado pelo kernel a cada borda (subida OU descida) do botao.
 * 'd'    = device do GPIO (nao usamos)
 * 'c'    = ponteiro pro proprio cb (nao usamos)
 * 'pins' = mascara de pinos que dispararam (nao usamos aqui)
 */
static void on_edge(const struct device *d, struct gpio_callback *c, uint32_t pins)
{
	/* Le o estado logico atual do botao.
	 * Retorna 1 = pressionado, 0 = solto
	 * (gpio_dt ja inverte automaticamente se for ACTIVE_LOW).
	 */
	if (gpio_pin_get_dt(&sw) == 1) {
		/* Borda de apertar: marca o instante de inicio e sai. */
		t_down = k_uptime_get();
		return;
	}

	/* Chegou aqui = borda de soltar. Calcula a duracao do aperto. */
	int64_t dur = k_uptime_get() - t_down;

	/* Monta o evento a ser publicado no zbus. */
	struct btn_evt e;
	e.ms = dur;

	/* Classifica como longo ou curto comparando com o threshold. */
	if (dur >= LONG_MS) 
	{
		e.long_press = true;
	} 
	else 
	{
		e.long_press = false;
	}

	/* Imprime no log qual tipo de toque foi detectado. */
	if (e.long_press) 
	{
		LOG_INF("Toque LONGO (%lld ms)", dur);
	} 
	else 
	{
		LOG_INF("Toque CURTO (%lld ms)", dur);
	}

	/* Publica o evento no canal zbus.
	 * K_NO_WAIT pq estamos em contexto de interrupcao: nao pode bloquear.
	 */
	zbus_chan_pub(&btn_chan, &e, K_NO_WAIT);
}

/* Funcao de inicializacao chamada automaticamente pelo SYS_INIT no boot. */
static int button_init(void)
{
	/* Configura o pino como entrada digital. */
	gpio_pin_configure_dt(&sw, GPIO_INPUT);

	/* Habilita interrupcao nas DUAS bordas (subida e descida),
	 * para conseguirmos medir o tempo entre apertar e soltar.
	 */
	gpio_pin_interrupt_configure_dt(&sw, GPIO_INT_EDGE_BOTH);

	/* Associa a funcao on_edge ao pino do botao. */
	gpio_init_callback(&cb, on_edge, BIT(sw.pin));

	/* Registra o callback no driver do GPIO. */
	gpio_add_callback(sw.port, &cb);

	LOG_INF("Botao sw1 pronto");
	return 0;
}

/* Registra button_init para rodar automaticamente no boot,
 * no nivel APPLICATION (depois dos drivers), prioridade 90.
 * NAO precisa chamar a partir do main.
 */
SYS_INIT(button_init, APPLICATION, 90);
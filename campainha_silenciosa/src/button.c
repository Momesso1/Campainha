/*
 * Copyright (c) 2026
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

LOG_MODULE_REGISTER(button, LOG_LEVEL_INF);

struct btn_evt {
	bool long_press;
	int64_t ms;
};

ZBUS_CHAN_DEFINE(btn_chan,
		 struct btn_evt,
		 NULL,
		 NULL,
		 ZBUS_OBSERVERS_EMPTY,
		 ZBUS_MSG_INIT(0));

static const struct gpio_dt_spec sw = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);
static struct gpio_callback cb;

static int64_t t_down;
static struct k_work_delayable debounce_work;

#define LONG_MS      800
#define DEBOUNCE_MS  50

static void debounce_handler(struct k_work *work)
{
	/* Lê o estado estável depois do debounce */
	int val = gpio_pin_get_dt(&sw);

	if (val == 1) {
		/* Botão acabou de ser confirmado como pressionado */
		t_down = k_uptime_get();
	} else {
		/* Botão acabou de ser confirmado como solto */
		int64_t dur = k_uptime_get() - t_down;

		/* Ignora se t_down nunca foi setado ou duração absurda */
		if (t_down == 0 || dur < 0) {
			return;
		}

		struct btn_evt e;
		e.ms = dur;
		e.long_press = (dur >= LONG_MS);

		if (e.long_press) {
			LOG_INF("Toque LONGO (%lld ms)", dur);
		} else {
			LOG_INF("Toque CURTO (%lld ms)", dur);
		}

		zbus_chan_pub(&btn_chan, &e, K_NO_WAIT);
		t_down = 0;
	}
}

static void on_edge(const struct device *d, struct gpio_callback *c, uint32_t pins)
{
	/* Ao invés de reagir direto, agenda o debounce.
	 * Se outra borda chegar antes dos 50ms, o timer reseta.
	 */
	k_work_reschedule(&debounce_work, K_MSEC(DEBOUNCE_MS));
}

static int button_init(void)
{
	k_work_init_delayable(&debounce_work, debounce_handler);

	gpio_pin_configure_dt(&sw, GPIO_INPUT);
	gpio_pin_interrupt_configure_dt(&sw, GPIO_INT_EDGE_BOTH);
	gpio_init_callback(&cb, on_edge, BIT(sw.pin));
	gpio_add_callback(sw.port, &cb);

	LOG_INF("Botao sw1 pronto (debounce %d ms)", DEBOUNCE_MS);
	return 0;
}

SYS_INIT(button_init, APPLICATION, 90);

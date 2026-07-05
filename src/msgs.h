/*
 * Copyright (c) 2026
 * SPDX-License-Identifier: Apache-2.0
 *
 * Structs de mensagens e canais zbus compartilhados entre modulos.
 */

#ifndef APP_MSGS_H_
#define APP_MSGS_H_

#include <stdbool.h>
#include <stdint.h>

#include <zephyr/zbus/zbus.h>

/* Mensagem publicada quando o app BLE ou o shell dispara um toque. */
struct ring_msg {
	bool pressed;
};

/* Mensagem publicada pelo botao fisico apos debounce. */
struct btn_evt {
	bool long_press;
	int64_t ms;
};

/* Canais definidos em main.c (ring_chan) e button.c (btn_chan). */
ZBUS_CHAN_DECLARE(ring_chan);
ZBUS_CHAN_DECLARE(btn_chan);

#endif /* APP_MSGS_H_ */

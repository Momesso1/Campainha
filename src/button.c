/*
 * Driver do botao usando Input Subsystem + input-longpress.
 * A classificacao curto/longo e feita pelo driver input-longpress
 * (via devicetree). Aqui so recebemos o codigo ja classificado e
 * publicamos no zbus.
 */
#include <zephyr/device.h>
#include <zephyr/input/input.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include "msgs.h"

LOG_MODULE_REGISTER(button, LOG_LEVEL_INF);

ZBUS_CHAN_DEFINE(btn_chan, struct btn_evt, NULL, NULL,
		 ZBUS_OBSERVERS_EMPTY, ZBUS_MSG_INIT(0));

/* Node do input-longpress: e ele quem emite os codigos classificados. */
#define LONGPRESS_NODE DT_NODELABEL(longpress)

static void input_cb(struct input_event *evt, void *user_data)
{
	ARG_UNUSED(user_data);

	
	if (evt->type != INPUT_EV_KEY || evt->value != 0) {
		return;
	}

	struct btn_evt out = {0};

	switch (evt->code) 
	{
		case INPUT_KEY_A:
			out.long_press = false;
			LOG_INF("Toque CURTO");
			break;
		case INPUT_KEY_B:
			out.long_press = true;
			LOG_INF("Toque LONGO");
			break;
		default:
			return;
	}

	zbus_chan_pub(&btn_chan, &out, K_NO_WAIT);
}

INPUT_CALLBACK_DEFINE(DEVICE_DT_GET(LONGPRESS_NODE), input_cb, NULL);
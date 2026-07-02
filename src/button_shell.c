/*
 * Copyright (c) 2026
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/shell/shell.h>

/* Mesmo alias do button.c */
static const struct gpio_dt_spec sw = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);

/* Comando: button press [ms]
 * Injeta uma borda de descida, espera ms, injeta borda de subida.
 * O on_edge() do button.c detecta exatamente como se fosse hardware.
 */
static int cmd_press(const struct shell *sh, size_t argc, char **argv)
{
	uint32_t ms = 100;

	if (argc > 1) {
		ms = strtoul(argv[1], NULL, 10);
	}

	gpio_emul_input_set(sw.port, sw.pin, 0);  /* aperta (ACTIVE_LOW) */
	k_msleep(ms);
	gpio_emul_input_set(sw.port, sw.pin, 1);  /* solta */

	shell_print(sh, "toque simulado de %u ms", ms);
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_button,
	SHELL_CMD_ARG(press, NULL, "Simula toque [ms]", cmd_press, 1, 1),
	SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(button, &sub_button, "Botao da campainha", NULL);

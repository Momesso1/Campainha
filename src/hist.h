/*
 * Copyright (c) 2026
 * SPDX-License-Identifier: Apache-2.0
 *
 * Historico circular de toques da campainha. Testavel sem hardware.
 */

#ifndef APP_HIST_H_
#define APP_HIST_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define HIST_SIZE 8

struct hist_entry {
	int64_t ms;     /* uptime em ms no momento do toque */
	bool ignored;   /* true se foi ignorado por modo DND */
};

/* Buffer circular exposto para permitir persistencia via settings. */
struct hist_state {
	struct hist_entry entries[HIST_SIZE];
	uint16_t head;
	uint16_t count;
};

extern struct hist_state hist_state;

/* Insere um toque no fim do historico, sobrescrevendo o mais antigo. */
void hist_add(int64_t ms, bool ignored);

/* Reseta o buffer para zero entradas. */
void hist_clear(void);

/* Numero de entradas validas (0..HIST_SIZE). */
size_t hist_len(void);

/* Acessa a entrada i em ordem cronologica (0 = mais antiga). */
const struct hist_entry *hist_get(size_t i);

#endif /* APP_HIST_H_ */

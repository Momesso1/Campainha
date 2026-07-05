/*
 * Copyright (c) 2026
 * SPDX-License-Identifier: Apache-2.0
 *
 * Implementacao do buffer circular do historico.
 */

#include "hist.h"

/* Estado global; exposto para permitir persistir via settings. */
struct hist_state hist_state;

/* Insere um toque na proxima posicao livre e avanca o head. */
void hist_add(int64_t ms, bool ignored)
{
	hist_state.entries[hist_state.head].ms = ms;
	hist_state.entries[hist_state.head].ignored = ignored;
	hist_state.head = (hist_state.head + 1) % HIST_SIZE;
	if (hist_state.count < HIST_SIZE) {
		hist_state.count++;
	}
}

/* Zera tudo: head, count e as entradas nao precisam ser limpas
 * porque count controla o que e' valido.
 */
void hist_clear(void)
{
	hist_state.head = 0;
	hist_state.count = 0;
}

/* Retorna quantas entradas validas existem. */
size_t hist_len(void)
{
	return hist_state.count;
}

/* Traduz o indice cronologico i para a posicao real no buffer. */
const struct hist_entry *hist_get(size_t i)
{
	size_t start;

	if (i >= hist_state.count) {
		return NULL;
	}
	/* Se ainda nao encheu, o inicio e' 0. Se encheu, e' o head. */
	start = (hist_state.count == HIST_SIZE) ? hist_state.head : 0;
	return &hist_state.entries[(start + i) % HIST_SIZE];
}

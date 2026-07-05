/*
 * Copyright (c) 2026
 * SPDX-License-Identifier: Apache-2.0
 *
 * Testes do buffer circular do historico. Puro C, sem hardware.
 */

#include <zephyr/ztest.h>

#include "hist.h"

ZTEST_SUITE(hist, NULL, NULL, NULL, NULL, NULL);

/* Historico vazio: len = 0 e get(0) retorna NULL. */
ZTEST(hist, test_vazio)
{
	hist_clear();

	zassert_equal(hist_len(), 0);
	zassert_is_null(hist_get(0));
}

/* N < HIST_SIZE toques mantem a ordem cronologica. */
ZTEST(hist, test_ordem_cronologica)
{
	hist_clear();

	hist_add(100, false);
	hist_add(200, true);
	hist_add(300, false);

	zassert_equal(hist_len(), 3);
	zassert_equal(hist_get(0)->ms, 100);
	zassert_false(hist_get(0)->ignored);
	zassert_equal(hist_get(1)->ms, 200);
	zassert_true(hist_get(1)->ignored);
	zassert_equal(hist_get(2)->ms, 300);
}

/* Mais de HIST_SIZE toques sobrescrevem os mais antigos. */
ZTEST(hist, test_overflow_circular)
{
	hist_clear();

	for (int i = 0; i < HIST_SIZE + 3; i++) {
		hist_add((int64_t)i * 100, false);
	}

	zassert_equal(hist_len(), HIST_SIZE);
	/* Os 3 primeiros (0, 100, 200) foram sobrescritos: mais antigo agora e' 300. */
	zassert_equal(hist_get(0)->ms, 300);
	zassert_equal(hist_get(HIST_SIZE - 1)->ms, (int64_t)(HIST_SIZE + 2) * 100);
}

/* hist_clear zera o buffer para reuso. */
ZTEST(hist, test_clear)
{
	hist_add(42, false);
	hist_add(43, true);
	hist_clear();

	zassert_equal(hist_len(), 0);
	zassert_is_null(hist_get(0));
}

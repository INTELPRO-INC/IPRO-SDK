/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * entropy_ipro_trng (SEC_ENG TRNG): buffers fill, consecutive draws differ,
 * ISR-safe path returns bytes, and a coarse byte-distribution sanity check.
 */
#include <zephyr/ztest.h>
#include <zephyr/drivers/entropy.h>
#include <string.h>

static const struct device *const rng = DEVICE_DT_GET(DT_CHOSEN(zephyr_entropy));

ZTEST(ipro_entropy, test_get_entropy)
{
	uint8_t a[64], b[64];

	zassert_true(device_is_ready(rng));
	memset(a, 0, sizeof(a));
	memset(b, 0, sizeof(b));
	zassert_ok(entropy_get_entropy(rng, a, sizeof(a)));
	zassert_ok(entropy_get_entropy(rng, b, sizeof(b)));
	zassert_true(memcmp(a, b, sizeof(a)) != 0, "two draws identical");

	uint8_t zeros[64] = {0};

	zassert_true(memcmp(a, zeros, sizeof(a)) != 0, "all zero");
}

ZTEST(ipro_entropy, test_isr_path)
{
	uint8_t buf[32];
	int n = entropy_get_entropy_isr(rng, buf, sizeof(buf), ENTROPY_BUSYWAIT);

	zassert_equal(n, (int)sizeof(buf), "isr path returned %d", n);
}

ZTEST(ipro_entropy, test_distribution)
{
	static uint8_t buf[4096];
	uint32_t hist[256] = {0};

	zassert_ok(entropy_get_entropy(rng, buf, sizeof(buf)));
	for (size_t i = 0; i < sizeof(buf); i++) {
		hist[buf[i]]++;
	}
	/* expected 16 per bin; a stuck generator would blow far past this */
	for (int v = 0; v < 256; v++) {
		zassert_true(hist[v] < 64, "byte 0x%02x occurs %u/4096 times", v, hist[v]);
	}
}

ZTEST_SUITE(ipro_entropy, NULL, NULL, NULL, NULL, NULL);

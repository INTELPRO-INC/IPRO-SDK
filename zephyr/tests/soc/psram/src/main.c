/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * PSRAM board test (ipro7ai_evk): the X8 PSRAM is trained by
 * ipro7_psram_prep() before main(); this suite checks that the linker's
 * PSRAM region is usable (.psram_data initialised, .psram_bss cleared) and
 * that the whole 8 MB window holds data (address-pattern sweep, walking
 * bits at the corners), and reports the DQS delay the training picked.
 */
#include <zephyr/ztest.h>
#include <zephyr/devicetree.h>
#include <ipro_zephyr/ipro_hal_glue.h>
#include <string.h>

#define PSRAM_BASE DT_REG_ADDR(DT_NODELABEL(psram0))
#define PSRAM_SIZE DT_REG_SIZE(DT_NODELABEL(psram0))

/* Placed by ipro7_psram_sections.ld */
__attribute__((section(".psram_data"))) static uint32_t psram_data_word = 0xC0FFEE42u;
__attribute__((section(".psram_bss"))) static uint32_t psram_bss_buf[256];

extern volatile uint16_t g_psram_early_dqs_cfg;

ZTEST(ipro_psram, test_a_linker_sections)
{
	uintptr_t d = (uintptr_t)&psram_data_word, b = (uintptr_t)psram_bss_buf;

	printk("psram_data @%08lx psram_bss @%08lx, training dqs cfg 0x%02x%s\n",
	       (unsigned long)d, (unsigned long)b, g_psram_early_dqs_cfg,
	       g_psram_early_dqs_cfg ? "" : " (trained by boot2, efuse trim in use)");
	zassert_true(d >= PSRAM_BASE && d < PSRAM_BASE + PSRAM_SIZE, ".psram_data not in PSRAM");
	zassert_true(b >= PSRAM_BASE && b < PSRAM_BASE + PSRAM_SIZE, ".psram_bss not in PSRAM");
	zassert_equal(psram_data_word, 0xC0FFEE42u, ".psram_data not loaded");
	for (int i = 0; i < ARRAY_SIZE(psram_bss_buf); i++) {
		zassert_equal(psram_bss_buf[i], 0, ".psram_bss not cleared at %d", i);
	}
	psram_data_word = ~psram_data_word;
	zassert_equal(psram_data_word, ~0xC0FFEE42u, "PSRAM write-back");
}

/* Every 4 KB page: one word written with its own address, then all read back.
 * Reads after a full-window write defeat the 32 KB data cache, so this is a
 * real DRAM round trip. */
ZTEST(ipro_psram, test_b_address_sweep)
{
	const uint32_t stride = 4096;
	volatile uint32_t *p;
	uint32_t bad = 0, first_bad = 0;

	for (uintptr_t a = PSRAM_BASE; a < PSRAM_BASE + PSRAM_SIZE; a += stride) {
		p = (volatile uint32_t *)a;
		*p = (uint32_t)a ^ 0xA5A5A5A5u;
	}
	ipro_hal_cache_clean(PSRAM_BASE, PSRAM_SIZE);
	ipro_hal_cache_invalidate(PSRAM_BASE, PSRAM_SIZE);
	for (uintptr_t a = PSRAM_BASE; a < PSRAM_BASE + PSRAM_SIZE; a += stride) {
		p = (volatile uint32_t *)a;
		if (*p != ((uint32_t)a ^ 0xA5A5A5A5u)) {
			if (bad++ == 0) {
				first_bad = a;
			}
		}
	}
	if (bad) {
		printk("address sweep: %u pages, %u bad, first at 0x%08x\n", PSRAM_SIZE / stride,
		       bad, first_bad);
	} else {
		printk("address sweep: %u pages, 0 bad\n", PSRAM_SIZE / stride);
	}
	zassert_equal(bad, 0, "%u bad pages, first 0x%08x", bad, first_bad);
}

static void walking_bits(uintptr_t a)
{
	volatile uint32_t *p = (volatile uint32_t *)a;

	for (int bit = 0; bit < 32; bit++) {
		uint32_t v = 1u << bit;

		*p = v;
		ipro_hal_cache_clean(a, 32); ipro_hal_cache_invalidate(a, 32);
		zassert_equal(*p, v, "walking 1 bit %d at 0x%08lx", bit, (unsigned long)a);
		*p = ~v;
		ipro_hal_cache_clean(a, 32); ipro_hal_cache_invalidate(a, 32);
		zassert_equal(*p, ~v, "walking 0 bit %d at 0x%08lx", bit, (unsigned long)a);
	}
}

ZTEST(ipro_psram, test_c_walking_bits_corners)
{
	walking_bits(PSRAM_BASE);
	walking_bits(PSRAM_BASE + PSRAM_SIZE / 2);
	walking_bits(PSRAM_BASE + PSRAM_SIZE - 4);
}

/* 1 MB block fill + memcmp: throughput figure and a burst-transfer check. */
ZTEST(ipro_psram, test_d_block_copy)
{
	uint8_t *dst = (uint8_t *)(PSRAM_BASE + PSRAM_SIZE / 4);
	static uint8_t src[4096];
	const size_t total = 1024 * 1024;
	uint32_t t0, t1;

	for (int i = 0; i < sizeof(src); i++) {
		src[i] = (uint8_t)(i * 7 + 3);
	}
	t0 = k_cycle_get_32();
	for (size_t off = 0; off < total; off += sizeof(src)) {
		memcpy(dst + off, src, sizeof(src));
	}
	t1 = k_cycle_get_32();
	ipro_hal_cache_clean((uint32_t)dst, total);
	ipro_hal_cache_invalidate((uint32_t)dst, total);
	for (size_t off = 0; off < total; off += sizeof(src)) {
		zassert_equal(memcmp(dst + off, src, sizeof(src)), 0, "block mismatch at +0x%x", off);
	}
	uint32_t us = k_cyc_to_us_floor32(t1 - t0);

	printk("1 MB memcpy into PSRAM: %u us (%u KB/s)\n", us,
	       us ? (uint32_t)((uint64_t)total * 1000U / us / 1024U) : 0);
}

ZTEST_SUITE(ipro_psram, NULL, NULL, NULL, NULL, NULL);

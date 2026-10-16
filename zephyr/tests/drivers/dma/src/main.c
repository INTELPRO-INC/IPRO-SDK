/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * dma_ipro memory-to-memory: config validation, single-block copy with
 * completion callback, channel bookkeeping, and get_status.
 */
#include <zephyr/ztest.h>
#include <zephyr/drivers/dma.h>
#include <string.h>

static const struct device *const dma = DEVICE_DT_GET(DT_NODELABEL(dma0));
static K_SEM_DEFINE(done, 0, 1);
static int done_status;

static void on_done(const struct device *dev, void *ud, uint32_t ch, int status)
{
	ARG_UNUSED(dev); ARG_UNUSED(ud); ARG_UNUSED(ch);
	done_status = status;
	k_sem_give(&done);
}

static uint8_t src[1024] __aligned(4);
static uint8_t dst[1024] __aligned(4);

static struct dma_block_config blk;
static struct dma_config cfg;

static void setup_m2m(size_t len, uint32_t width)
{
	memset(&blk, 0, sizeof(blk));
	memset(&cfg, 0, sizeof(cfg));
	blk.source_address = (uint32_t)src;
	blk.dest_address = (uint32_t)dst;
	blk.block_size = len;
	cfg.channel_direction = MEMORY_TO_MEMORY;
	cfg.source_data_size = width;
	cfg.dest_data_size = width;
	cfg.source_burst_length = 1;
	cfg.dest_burst_length = 1;
	cfg.block_count = 1;
	cfg.head_block = &blk;
	cfg.dma_callback = on_done;
}

ZTEST(ipro_dma, test_ready)
{
	zassert_true(device_is_ready(dma));
}

ZTEST(ipro_dma, test_config_validation)
{
	setup_m2m(64, 4);
	zassert_equal(dma_config(dma, 99, &cfg), -EINVAL, "bad channel");
	cfg.block_count = 2;
	zassert_equal(dma_config(dma, 0, &cfg), -ENOTSUP, "multi-block not supported");
	cfg.block_count = 1;
	cfg.source_data_size = 3;
	zassert_equal(dma_config(dma, 0, &cfg), -EINVAL, "3-byte width");
	cfg.source_data_size = 4;
	cfg.head_block = NULL;
	zassert_equal(dma_config(dma, 0, &cfg), -EINVAL, "no block");
}

ZTEST(ipro_dma, test_m2m_copy)
{
	struct dma_status st;

	for (size_t i = 0; i < sizeof(src); i++) {
		src[i] = (uint8_t)(i ^ 0x5a);
	}
	memset(dst, 0, sizeof(dst));

	setup_m2m(sizeof(src), 4);
	k_sem_reset(&done);
	zassert_ok(dma_config(dma, 0, &cfg));
	/* Zephyr lets an idle channel be re-configured; -EBUSY is only for a
	 * channel with a transfer in flight (not testable on a 256-byte m2m
	 * copy that completes in microseconds). */
	zassert_ok(dma_config(dma, 0, &cfg), "re-configure of an idle channel");
	zassert_ok(dma_start(dma, 0));
	zassert_ok(k_sem_take(&done, K_MSEC(200)), "completion callback");
	zassert_equal(done_status, DMA_STATUS_COMPLETE);
	zassert_mem_equal(dst, src, sizeof(src));
	zassert_ok(dma_get_status(dma, 0, &st));
	zassert_false(st.busy);
	zassert_ok(dma_stop(dma, 0));
}

ZTEST(ipro_dma, test_byte_width_and_reload)
{
	memset(dst, 0, sizeof(dst));
	setup_m2m(100, 1);
	k_sem_reset(&done);
	zassert_ok(dma_config(dma, 1, &cfg));
	zassert_ok(dma_start(dma, 1));
	zassert_ok(k_sem_take(&done, K_MSEC(200)));
	zassert_mem_equal(dst, src, 100);
	zassert_equal(dst[100], 0, "byte 100 must be untouched");

	memset(dst, 0, sizeof(dst));
	k_sem_reset(&done);
	zassert_ok(dma_reload(dma, 1, (uint32_t)src + 100, (uint32_t)dst, 50));
	zassert_ok(dma_start(dma, 1));
	zassert_ok(k_sem_take(&done, K_MSEC(200)));
	zassert_mem_equal(dst, src + 100, 50);
	zassert_ok(dma_stop(dma, 1));
}

ZTEST_SUITE(ipro_dma, NULL, NULL, NULL, NULL, NULL);

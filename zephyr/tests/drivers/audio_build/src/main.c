/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * i2s_ipro / dmic_ipro without a codec or microphone: configuration and
 * state-machine paths that need no peer (a TX stream of zeros with DRAIN
 * completes even with nothing listening; RX/dmic configuration and
 * start/stop return the documented codes). Signal content cannot be checked
 * on this bench.
 */
#include <zephyr/ztest.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/audio/dmic.h>
#include <string.h>

#define BLOCK 256
K_MEM_SLAB_DEFINE_STATIC(slab, BLOCK, 8, 4);

static const struct device *const i2s = DEVICE_DT_GET(DT_NODELABEL(i2s0));
static const struct device *const dmic = DEVICE_DT_GET(DT_NODELABEL(pdm0));

ZTEST(ipro_audio, test_i2s_configure_and_tx_drain)
{
	struct i2s_config cfg = {
		.word_size = 16,
		.channels = 2,
		.format = I2S_FMT_DATA_FORMAT_I2S,
		.options = I2S_OPT_BIT_CLK_CONTROLLER | I2S_OPT_FRAME_CLK_CONTROLLER,
		.frame_clk_freq = 16000,
		.mem_slab = &slab,
		.block_size = BLOCK,
		.timeout = 200,
	};
	void *blk;

	zassert_true(device_is_ready(i2s));
	zassert_ok(i2s_configure(i2s, I2S_DIR_TX, &cfg));
	zassert_not_null(i2s_config_get(i2s, I2S_DIR_TX));
	zassert_equal(i2s_trigger(i2s, I2S_DIR_TX, I2S_TRIGGER_START), -EIO, "start needs a block");

	for (int i = 0; i < 3; i++) {
		zassert_ok(k_mem_slab_alloc(&slab, &blk, K_NO_WAIT));
		memset(blk, 0, BLOCK);
		zassert_ok(i2s_write(i2s, blk, BLOCK));
	}
	zassert_ok(i2s_trigger(i2s, I2S_DIR_TX, I2S_TRIGGER_START));
	zassert_ok(i2s_trigger(i2s, I2S_DIR_TX, I2S_TRIGGER_DRAIN));
	/* 3 x 64 stereo frames at 16 kHz = 12 ms; the DMA must have drained:
	 * every block is back in the slab and a second DRAIN is refused (READY). */
	k_msleep(50);
	zassert_equal(k_mem_slab_num_used_get(&slab), 0, "blocks not returned (drain incomplete)");
	zassert_equal(i2s_trigger(i2s, I2S_DIR_TX, I2S_TRIGGER_DRAIN), -EIO, "still running");

	cfg.format |= I2S_FMT_BIT_CLK_INV;
	zassert_equal(i2s_configure(i2s, I2S_DIR_TX, &cfg), -ENOTSUP);
}

ZTEST(ipro_audio, test_i2s_rx_start_stop)
{
	struct i2s_config cfg = {
		.word_size = 16,
		.channels = 2,
		.format = I2S_FMT_DATA_FORMAT_I2S,
		.options = I2S_OPT_BIT_CLK_CONTROLLER | I2S_OPT_FRAME_CLK_CONTROLLER,
		.frame_clk_freq = 16000,
		.mem_slab = &slab,
		.block_size = BLOCK,
		.timeout = 100,
	};
	void *blk;
	size_t sz;

	zassert_ok(i2s_configure(i2s, I2S_DIR_RX, &cfg));
	zassert_ok(i2s_trigger(i2s, I2S_DIR_RX, I2S_TRIGGER_START));
	/* Master mode clocks the receiver: blocks of (floating) data arrive. */
	zassert_ok(i2s_read(i2s, &blk, &sz), "no RX block");
	zassert_equal(sz, BLOCK);
	k_mem_slab_free(&slab, blk);
	zassert_ok(i2s_trigger(i2s, I2S_DIR_RX, I2S_TRIGGER_DROP));
	k_msleep(5);
	zassert_equal(k_mem_slab_num_used_get(&slab), 0, "blocks not returned");
}

ZTEST(ipro_audio, test_dmic_configure)
{
	struct pcm_stream_cfg stream = {
		.pcm_rate = 16000, .pcm_width = 16, .block_size = BLOCK, .mem_slab = &slab,
	};
	struct dmic_cfg cfg = {
		.io = {.min_pdm_clk_freq = 1000000, .max_pdm_clk_freq = 3500000},
		.streams = &stream,
		.channel = {.req_num_streams = 1, .req_num_chan = 1},
	};

	zassert_true(device_is_ready(dmic));
	stream.pcm_width = 24;
	zassert_equal(dmic_configure(dmic, &cfg), -ENOTSUP);
	stream.pcm_width = 16;
	zassert_ok(dmic_configure(dmic, &cfg));
	zassert_equal(cfg.channel.act_num_chan, 1);
	zassert_ok(dmic_trigger(dmic, DMIC_TRIGGER_START));
	k_msleep(20);
	zassert_ok(dmic_trigger(dmic, DMIC_TRIGGER_STOP));
	zassert_ok(dmic_trigger(dmic, DMIC_TRIGGER_RELEASE));
	zassert_equal(k_mem_slab_num_used_get(&slab), 0, "blocks not returned");
}

ZTEST_SUITE(ipro_audio, NULL, NULL, NULL, NULL, NULL);

/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * spi_ipro: operation-word validation, 8/16/32-bit transfers complete, and
 * loopback when MOSI/MISO are jumpered (reported, not asserted).
 */
#include <zephyr/ztest.h>
#include <zephyr/drivers/spi.h>
#include <string.h>

static const struct device *const spi = DEVICE_DT_GET(DT_NODELABEL(spi0));

#define OP8 (SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB)

static int xfer(struct spi_config *cfg, void *tx, void *rx, size_t len)
{
	const struct spi_buf txb = {.buf = tx, .len = len};
	const struct spi_buf rxb = {.buf = rx, .len = len};
	const struct spi_buf_set txs = {.buffers = &txb, .count = 1};
	const struct spi_buf_set rxs = {.buffers = &rxb, .count = 1};

	return spi_transceive(spi, cfg, &txs, &rxs);
}

ZTEST(ipro_spi, test_ready)
{
	zassert_true(device_is_ready(spi));
}

ZTEST(ipro_spi, test_rejected_configs)
{
	uint8_t b[2] = {0};
	struct spi_config cfg = {.frequency = 1000000, .operation = OP8};

	cfg.operation = SPI_OP_MODE_SLAVE | SPI_WORD_SET(8);
	zassert_equal(xfer(&cfg, b, b, 1), -ENOTSUP, "slave mode");
	cfg.operation = OP8 | SPI_HALF_DUPLEX;
	zassert_equal(xfer(&cfg, b, b, 1), -ENOTSUP, "half duplex");
	cfg.operation = OP8 | SPI_MODE_LOOP;
	zassert_equal(xfer(&cfg, b, b, 1), -ENOTSUP, "internal loop");
	cfg.operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(12);
	zassert_equal(xfer(&cfg, b, b, 1), -ENOTSUP, "12-bit words");
	cfg.operation = OP8;
	cfg.frequency = 0;
	zassert_equal(xfer(&cfg, b, b, 1), -EINVAL, "zero clock");
}

ZTEST(ipro_spi, test_transfer_8bit)
{
	uint8_t tx[8] = {0xA5, 0x5A, 1, 2, 3, 4, 5, 6}, rx[8] = {0};
	struct spi_config cfg = {.frequency = 1000000, .operation = OP8};

	zassert_ok(xfer(&cfg, tx, rx, sizeof(tx)));
	TC_PRINT("8-bit rx %02x %02x %02x: %s\n", rx[0], rx[1], rx[2],
		 memcmp(tx, rx, sizeof(tx)) == 0 ? "loopback" : "no jumper");
}

ZTEST(ipro_spi, test_transfer_16_and_32bit_modes)
{
	uint16_t tx16[4] = {0x1234, 0xABCD, 0x0001, 0x8000}, rx16[4];
	uint32_t tx32[2] = {0xDEADBEEF, 0x01020304}, rx32[2];
	struct spi_config cfg = {
		.frequency = 4000000,
		.operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(16) | SPI_MODE_CPOL | SPI_MODE_CPHA,
	};

	zassert_ok(xfer(&cfg, tx16, rx16, sizeof(tx16)));
	cfg.operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(32);
	zassert_ok(xfer(&cfg, tx32, rx32, sizeof(tx32)));
}

ZTEST(ipro_spi, test_tx_only_and_rx_only)
{
	uint8_t tx[4] = {1, 2, 3, 4}, rx[4];
	struct spi_config cfg = {.frequency = 1000000, .operation = OP8};
	const struct spi_buf txb = {.buf = tx, .len = sizeof(tx)};
	const struct spi_buf rxb = {.buf = rx, .len = sizeof(rx)};
	const struct spi_buf_set txs = {.buffers = &txb, .count = 1};
	const struct spi_buf_set rxs = {.buffers = &rxb, .count = 1};

	zassert_ok(spi_write(spi, &cfg, &txs));
	zassert_ok(spi_read(spi, &cfg, &rxs));
	zassert_ok(spi_release(spi, &cfg));
}

/* A transmit-only burst large enough for the DMA path (node with dmas); a
 * timing check tells whether it really went by DMA at the requested clock. */
ZTEST(ipro_spi, test_tx_dma_burst)
{
	static uint8_t tx[4096];
	struct spi_config cfg = {.frequency = 8000000, .operation = OP8};
	const struct spi_buf txb = {.buf = tx, .len = sizeof(tx)};
	const struct spi_buf_set txs = {.buffers = &txb, .count = 1};

	for (size_t i = 0; i < sizeof(tx); i++) {
		tx[i] = (uint8_t)i;
	}
	uint32_t t0 = k_cycle_get_32();

	zassert_ok(spi_write(spi, &cfg, &txs));
	uint32_t us = k_cyc_to_us_floor32(k_cycle_get_32() - t0);

	/* 4096 x 8 bits at 8 MHz = 4.1 ms on the wire */
	printk("4 KB tx-only at 8 MHz: %u us (wire time 4096 us)\n", us);
	zassert_true(us >= 4000 && us < 20000, "4 KB burst took %u us", us);
	zassert_ok(spi_release(spi, &cfg));
}

#ifdef CONFIG_SPI_ASYNC
static K_SEM_DEFINE(async_sem, 0, 1);
static int async_result = -1;

static void async_cb(const struct device *dev, int result, void *data)
{
	ARG_UNUSED(dev); ARG_UNUSED(data);
	async_result = result;
	k_sem_give(&async_sem);
}

ZTEST(ipro_spi, test_async_transceive)
{
	static uint8_t tx[64], rx[64];
	struct spi_config cfg = {.frequency = 1000000, .operation = OP8};
	struct spi_buf txb = {.buf = tx, .len = sizeof(tx)}, rxb = {.buf = rx, .len = sizeof(rx)};
	struct spi_buf_set txs = {.buffers = &txb, .count = 1}, rxs = {.buffers = &rxb, .count = 1};

	memset(tx, 0xA5, sizeof(tx));
	k_sem_reset(&async_sem);
	zassert_ok(spi_transceive_cb(spi, &cfg, &txs, &rxs, async_cb, NULL));
	zassert_ok(k_sem_take(&async_sem, K_MSEC(100)), "async transfer did not complete");
	zassert_equal(async_result, 0);
	/* The bus is free again for a synchronous transfer. */
	zassert_ok(spi_transceive(spi, &cfg, &txs, &rxs));
}
#endif

ZTEST_SUITE(ipro_spi, NULL, NULL, NULL, NULL, NULL);

/*
 * Copyright (c) 2025 IPRO
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT ipro_ipro7_spi

#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

#include "drv_spi.h"

LOG_MODULE_REGISTER(spi_ipro7, CONFIG_SPI_LOG_LEVEL);

struct spi_ipro7_config {
	uint32_t base;
	uint32_t frequency;
};

struct spi_ipro7_data {
	struct k_mutex mutex;
};

static int spi_ipro7_configure(const struct device *dev,
			       const struct spi_config *config)
{
	struct spi_ipro7_data *data = dev->data;
	SPI_CFG_Type spiCfg;
	
	k_mutex_lock(&data->mutex, K_FOREVER);
	
	/* Configure SPI */
	spiCfg.deglitchEnable = DISABLE;
	spiCfg.continuousEnable = DISABLE;
	spiCfg.byteSequence = SPI_BYTE_INVERSE_BYTE0_FIRST;
	spiCfg.bitSequence = SPI_BIT_INVERSE_MSB_FIRST;
	spiCfg.frameSize = SPI_FRAME_SIZE_8;
	
	if (config->operation & SPI_MODE_CPOL) {
		spiCfg.clkPolarity = SPI_CLK_POLARITY_HIGH;
	} else {
		spiCfg.clkPolarity = SPI_CLK_POLARITY_LOW;
	}
	
	if (config->operation & SPI_MODE_CPHA) {
		spiCfg.clkPhase = SPI_CLK_PHASE_1;
	} else {
		spiCfg.clkPhase = SPI_CLK_PHASE_0;
	}
	
	SPI_Init(SPI_ID_0, &spiCfg);
	SPI_Enable(SPI_ID_0, SPI_WORK_MODE_MASTER);
	
	k_mutex_unlock(&data->mutex);
	
	return 0;
}

static int spi_ipro7_transceive(const struct device *dev,
				const struct spi_config *config,
				const struct spi_buf_set *tx_bufs,
				const struct spi_buf_set *rx_bufs)
{
	struct spi_ipro7_data *data = dev->data;
	int ret = 0;
	
	k_mutex_lock(&data->mutex, K_FOREVER);
	
	/* Configure SPI for this transaction */
	ret = spi_ipro7_configure(dev, config);
	if (ret) {
		goto out;
	}
	
	/* Perform transfer */
	if (tx_bufs && rx_bufs) {
		/* Full duplex transfer */
		for (size_t i = 0; i < tx_bufs->count && i < rx_bufs->count; i++) {
			const uint8_t *tx_data = tx_bufs->buffers[i].buf;
			uint8_t *rx_data = rx_bufs->buffers[i].buf;
			size_t len = MIN(tx_bufs->buffers[i].len, rx_bufs->buffers[i].len);
			
			for (size_t j = 0; j < len; j++) {
				SPI_SendData(SPI_ID_0, tx_data ? tx_data[j] : 0);
				while (SPI_GetTxFifoCount(SPI_ID_0) != 0);
				while (SPI_GetRxFifoCount(SPI_ID_0) == 0);
				if (rx_data) {
					rx_data[j] = SPI_ReceiveData(SPI_ID_0);
				} else {
					SPI_ReceiveData(SPI_ID_0); /* Dummy read */
				}
			}
		}
	} else if (tx_bufs) {
		/* TX only */
		for (size_t i = 0; i < tx_bufs->count; i++) {
			const uint8_t *tx_data = tx_bufs->buffers[i].buf;
			size_t len = tx_bufs->buffers[i].len;
			
			for (size_t j = 0; j < len; j++) {
				SPI_SendData(SPI_ID_0, tx_data[j]);
				while (SPI_GetTxFifoCount(SPI_ID_0) != 0);
			}
		}
	} else if (rx_bufs) {
		/* RX only */
		for (size_t i = 0; i < rx_bufs->count; i++) {
			uint8_t *rx_data = rx_bufs->buffers[i].buf;
			size_t len = rx_bufs->buffers[i].len;
			
			for (size_t j = 0; j < len; j++) {
				SPI_SendData(SPI_ID_0, 0); /* Dummy write */
				while (SPI_GetTxFifoCount(SPI_ID_0) != 0);
				while (SPI_GetRxFifoCount(SPI_ID_0) == 0);
				rx_data[j] = SPI_ReceiveData(SPI_ID_0);
			}
		}
	}
	
out:
	k_mutex_unlock(&data->mutex);
	return ret;
}

static int spi_ipro7_release(const struct device *dev,
			     const struct spi_config *config)
{
	/* Nothing to do for release */
	return 0;
}

static int spi_ipro7_init(const struct device *dev)
{
	struct spi_ipro7_data *data = dev->data;
	
	LOG_DBG("Initializing SPI IPRO7 device %s", dev->name);
	
	k_mutex_init(&data->mutex);
	
	/* Initialize SPI hardware */
	GLB_Set_SPI_CLK(ENABLE, GLB_SPI_CLK_MCU_MUXPLL_160M, 0);
	
	return 0;
}

static const struct spi_driver_api spi_ipro7_driver_api = {
	.transceive = spi_ipro7_transceive,
	.release = spi_ipro7_release,
};

#define SPI_IPRO7_INIT(n)						\
	static const struct spi_ipro7_config spi_ipro7_config_##n = {	\
		.base = DT_INST_REG_ADDR(n),				\
		.frequency = DT_INST_PROP_OR(n, clock_frequency, 1000000), \
	};								\
									\
	static struct spi_ipro7_data spi_ipro7_data_##n;		\
									\
	DEVICE_DT_INST_DEFINE(n, spi_ipro7_init,			\
			      NULL,					\
			      &spi_ipro7_data_##n,			\
			      &spi_ipro7_config_##n,			\
			      POST_KERNEL,				\
			      CONFIG_SPI_INIT_PRIORITY,			\
			      &spi_ipro7_driver_api);

DT_INST_FOREACH_STATUS_OKAY(SPI_IPRO7_INIT)

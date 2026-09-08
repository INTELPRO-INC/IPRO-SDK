/*
 * Copyright (c) 2025 IPRO
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT ipro_ipro7_i2c

#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

#include "drv_i2c.h"

LOG_MODULE_REGISTER(i2c_ipro7, CONFIG_I2C_LOG_LEVEL);

struct i2c_ipro7_config {
	uint32_t base;
	uint32_t bitrate;
};

struct i2c_ipro7_data {
	struct k_mutex mutex;
};

static int i2c_ipro7_configure(const struct device *dev, uint32_t dev_config)
{
	const struct i2c_ipro7_config *config = dev->config;
	struct i2c_ipro7_data *data = dev->data;
	I2C_CFG_Type i2cCfg;
	
	k_mutex_lock(&data->mutex, K_FOREVER);
	
	/* Configure I2C */
	i2cCfg.freq = 400000; /* 400kHz default */
	i2cCfg.slaveAddr = 0;
	
	switch (I2C_SPEED_GET(dev_config)) {
	case I2C_SPEED_STANDARD:
		i2cCfg.freq = 100000;
		break;
	case I2C_SPEED_FAST:
		i2cCfg.freq = 400000;
		break;
	case I2C_SPEED_FAST_PLUS:
		i2cCfg.freq = 1000000;
		break;
	default:
		i2cCfg.freq = config->bitrate;
		break;
	}
	
	I2C_Init(I2C_ID_0, &i2cCfg);
	I2C_Enable(I2C_ID_0);
	
	k_mutex_unlock(&data->mutex);
	
	return 0;
}

static int i2c_ipro7_transfer(const struct device *dev,
			      struct i2c_msg *msgs,
			      uint8_t num_msgs,
			      uint16_t addr)
{
	struct i2c_ipro7_data *data = dev->data;
	int ret = 0;
	
	k_mutex_lock(&data->mutex, K_FOREVER);
	
	for (uint8_t i = 0; i < num_msgs; i++) {
		struct i2c_msg *msg = &msgs[i];
		
		if (msg->flags & I2C_MSG_READ) {
			/* Read operation */
			I2C_MasterReceiveData(I2C_ID_0, addr, msg->buf, msg->len);
		} else {
			/* Write operation */
			I2C_MasterSendData(I2C_ID_0, addr, msg->buf, msg->len);
		}
		
		/* Wait for completion */
		while (I2C_GetBusyStatus(I2C_ID_0) == SET) {
			k_yield();
		}
		
		/* Check for errors */
		if (I2C_GetIntStatus(I2C_ID_0, I2C_INT_NACK) == SET) {
			I2C_IntClear(I2C_ID_0, I2C_INT_NACK);
			ret = -EIO;
			break;
		}
	}
	
	k_mutex_unlock(&data->mutex);
	
	return ret;
}

static int i2c_ipro7_init(const struct device *dev)
{
	struct i2c_ipro7_data *data = dev->data;
	
	LOG_DBG("Initializing I2C IPRO7 device %s", dev->name);
	
	k_mutex_init(&data->mutex);
	
	/* Initialize I2C hardware */
	GLB_Set_I2C_CLK(ENABLE, GLB_I2C_CLK_XCLK, 0);
	
	return 0;
}

static const struct i2c_driver_api i2c_ipro7_driver_api = {
	.configure = i2c_ipro7_configure,
	.transfer = i2c_ipro7_transfer,
};

#define I2C_IPRO7_INIT(n)						\
	static const struct i2c_ipro7_config i2c_ipro7_config_##n = {	\
		.base = DT_INST_REG_ADDR(n),				\
		.bitrate = DT_INST_PROP(n, clock_frequency),		\
	};								\
									\
	static struct i2c_ipro7_data i2c_ipro7_data_##n;		\
									\
	DEVICE_DT_INST_DEFINE(n, i2c_ipro7_init,			\
			      NULL,					\
			      &i2c_ipro7_data_##n,			\
			      &i2c_ipro7_config_##n,			\
			      POST_KERNEL,				\
			      CONFIG_I2C_INIT_PRIORITY,			\
			      &i2c_ipro7_driver_api);

DT_INST_FOREACH_STATUS_OKAY(I2C_IPRO7_INIT)

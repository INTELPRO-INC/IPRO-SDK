/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 I2C controller driver.
 *
 * Zephyr i2c_driver_api -> SDK LL drv_i2c.c (I2C_Init / I2C_Enable / FIFO
 * word accessors / status flags). The hardware executes one transaction of
 * the form  START <addr+R/W> [sub-address 0..2 bytes] <data...> STOP, with
 * the sub-address always written before the data phase (a repeated START is
 * inserted by hardware for reads). Zephyr message lists are mapped onto that:
 *
 *   [W n<=2 bytes][R ... RESTART]   -> one read transaction with sub-address
 *   [W n<=2 bytes][W ...]           -> one write transaction with sub-address
 *   anything else                    -> one transaction per message
 *
 * The LL blocking helpers spin ~320k iterations before reporting a NACK as a
 * timeout, which makes bus scanning unusable, so the transfer loop here polls
 * the NACK/END flags directly with a bounded wait.
 */

#define DT_DRV_COMPAT intelpro_ipro_i2c

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/logging/log.h>
#include <soc.h>

#include "ipro_soc_regs.h"
#include "i2c_reg.h"
#include "drv_i2c.h"

LOG_MODULE_REGISTER(i2c_ipro, CONFIG_I2C_LOG_LEVEL);

struct i2c_ipro_config {
	uint8_t id;
	uint32_t base;
	uint32_t bitrate;
	const struct pinctrl_dev_config *pincfg;
	const struct device *clock_dev;
	clock_control_subsys_t clock_id;
	/* Optional scl-gpios / sda-gpios: stuck-bus detection and recovery. */
	struct gpio_dt_spec scl;
	struct gpio_dt_spec sda;
};

struct i2c_ipro_data {
	struct k_mutex lock;
	uint32_t bitrate;
};

static inline uint32_t rd(const struct i2c_ipro_config *c, uint32_t ofs)
{
	return *(volatile uint32_t *)(c->base + ofs);
}

static inline void wr(const struct i2c_ipro_config *c, uint32_t ofs, uint32_t v)
{
	*(volatile uint32_t *)(c->base + ofs) = v;
}

static inline uint32_t tx_fifo_free(const struct i2c_ipro_config *c)
{
	return reg_get_bits(rd(c, I2C_FIFO_CONFIG_1_OFFSET), I2C_TX_FIFO_CNT);
}

static inline uint32_t rx_fifo_avail(const struct i2c_ipro_config *c)
{
	return reg_get_bits(rd(c, I2C_FIFO_CONFIG_1_OFFSET), I2C_RX_FIFO_CNT);
}

/* Wait for cond() with a bounded busy-wait; abort early on NACK. */
#define I2C_IPRO_WAIT(cfg, cond)                                                                   \
	({                                                                                         \
		int __ret = 0;                                                                     \
		uint32_t __us = CONFIG_I2C_IPRO_TIMEOUT_US;                                        \
		while (!(cond)) {                                                                  \
			if (I2C_TransferNackStatus((cfg)->id) == SET) {                            \
				__ret = -EIO;                                                      \
				break;                                                             \
			}                                                                          \
			if (__us-- == 0) {                                                         \
				__ret = -ETIMEDOUT;                                                \
				break;                                                             \
			}                                                                          \
			k_busy_wait(1);                                                            \
		}                                                                                  \
		__ret;                                                                             \
	})

static int i2c_ipro_finish(const struct i2c_ipro_config *cfg, int ret)
{
	if (ret == 0) {
		ret = I2C_IPRO_WAIT(cfg, !I2C_IsBusy(cfg->id) && I2C_TransferEndStatus(cfg->id));
	}
	I2C_Disable(cfg->id);
	return ret;
}

static int i2c_ipro_xfer(const struct device *dev, uint16_t addr, uint32_t subaddr,
			 uint8_t subaddr_len, uint8_t *buf, uint16_t len, bool read)
{
	const struct i2c_ipro_config *cfg = dev->config;
	struct i2c_ipro_data *data = dev->data;
	I2C_Transfer_Cfg ll = {
		.slaveAddr = addr,
		.slaveAddr10Bit = DISABLE,
		.stopEveryByte = DISABLE,
		.subAddrSize = subaddr_len,
		.subAddr = subaddr,
		.dataSize = len,
		.data = buf,
		.clk = data->bitrate,
	};
	int ret = 0;

	if (len == 0 && subaddr_len == 0) {
		/*
		 * Address-only probe (i2c scan): the controller needs a data
		 * phase, so issue a 1-byte read and only look at the ACK.
		 */
		uint8_t dummy;

		ll.dataSize = 1;
		ll.data = &dummy;
		read = true;
		len = 1;
		buf = &dummy;
	}

	I2C_Disable(cfg->id);
	I2C_Init(cfg->id, read ? I2C_READ : I2C_WRITE, &ll);

	if (read) {
		uint16_t i = 0;

		I2C_Enable(cfg->id);
		while (i < len) {
			ret = I2C_IPRO_WAIT(cfg, rx_fifo_avail(cfg) > 0);
			if (ret != 0) {
				break;
			}
			uint32_t w = rd(cfg, I2C_FIFO_RDATA_OFFSET);

			for (int b = 0; b < 4 && i < len; b++, i++) {
				buf[i] = (uint8_t)(w >> (8 * b));
			}
		}
	} else {
		uint16_t i = 0;

		while (i < len) {
			uint32_t w = 0;
			int n = 0;

			for (; n < 4 && i + n < len; n++) {
				w |= (uint32_t)buf[i + n] << (8 * n);
			}
			ret = I2C_IPRO_WAIT(cfg, tx_fifo_free(cfg) > 0);
			if (ret != 0) {
				break;
			}
			wr(cfg, I2C_FIFO_WDATA_OFFSET, w);
			if (reg_get_bits(rd(cfg, I2C_CONFIG_OFFSET), I2C_CR_I2C_M_EN) == 0) {
				I2C_Enable(cfg->id);
			}
			i += n;
		}
		if (len == 0) {
			/* Sub-address only write (e.g. register pointer set). */
			I2C_Enable(cfg->id);
		}
	}

	return i2c_ipro_finish(cfg, ret);
}

/*
 * Sample SCL/SDA before a transaction. The GLB input register only follows
 * a pad while it is in the GPIO function (an alternate-function pad reads 0
 * regardless of its level - verified on ipro7_evb and ipro7ai_evk
 * 2026-09-21), so the pads are switched to GPIO input for the sample and
 * the I2C mux re-applied. Starting a transaction while SCL or SDA is held
 * low (unpowered target clamping the lines, a target stuck mid-byte) makes
 * the controller wedge after the NACK and, on the next enable, stall the
 * APB bridge: the CPU stops without an exception. Refuse such transfers.
 */
static int i2c_ipro_bus_stuck(const struct i2c_ipro_config *cfg)
{
	int scl, sda, ret;

	if (cfg->scl.port == NULL || cfg->sda.port == NULL) {
		return 0;
	}
	(void)gpio_pin_configure_dt(&cfg->scl, GPIO_INPUT | GPIO_PULL_UP);
	(void)gpio_pin_configure_dt(&cfg->sda, GPIO_INPUT | GPIO_PULL_UP);
	scl = gpio_pin_get_raw(cfg->scl.port, cfg->scl.pin);
	sda = gpio_pin_get_raw(cfg->sda.port, cfg->sda.pin);
	ret = pinctrl_apply_state(cfg->pincfg, PINCTRL_STATE_DEFAULT);
	if (ret != 0) {
		return ret;
	}
	return (scl == 0 || sda == 0) ? -EBUSY : 0;
}

/* Standard 9-clock recovery: take the pads back as GPIO, clock SCL until a
 * target that is holding SDA releases it, issue a STOP, re-apply the mux. */
static int i2c_ipro_recover_bus(const struct device *dev)
{
	const struct i2c_ipro_config *cfg = dev->config;
	struct i2c_ipro_data *data = dev->data;
	const uint32_t half = 5; /* us: ~100 kHz */
	int ret;

	if (cfg->scl.port == NULL || cfg->sda.port == NULL) {
		return -ENOTSUP;
	}

	k_mutex_lock(&data->lock, K_FOREVER);
	I2C_Disable(cfg->id);

	(void)gpio_pin_configure_dt(&cfg->sda, GPIO_INPUT | GPIO_PULL_UP);
	(void)gpio_pin_configure_dt(&cfg->scl, GPIO_OUTPUT_HIGH | GPIO_PULL_UP);
	for (int i = 0; i < 9 && gpio_pin_get_raw(cfg->sda.port, cfg->sda.pin) == 0; i++) {
		gpio_pin_set_raw(cfg->scl.port, cfg->scl.pin, 0);
		k_busy_wait(half);
		gpio_pin_set_raw(cfg->scl.port, cfg->scl.pin, 1);
		k_busy_wait(half);
	}
	/* STOP: SDA low -> SCL high -> SDA high */
	(void)gpio_pin_configure_dt(&cfg->sda, GPIO_OUTPUT_LOW | GPIO_PULL_UP);
	k_busy_wait(half);
	gpio_pin_set_raw(cfg->scl.port, cfg->scl.pin, 1);
	k_busy_wait(half);
	gpio_pin_set_raw(cfg->sda.port, cfg->sda.pin, 1);
	k_busy_wait(half);

	ret = pinctrl_apply_state(cfg->pincfg, PINCTRL_STATE_DEFAULT);
	if (ret == 0) {
		ret = i2c_ipro_bus_stuck(cfg);
	}
	k_mutex_unlock(&data->lock);
	return ret;
}

static int i2c_ipro_transfer(const struct device *dev, struct i2c_msg *msgs, uint8_t num_msgs,
			     uint16_t addr)
{
	const struct i2c_ipro_config *cfg = dev->config;
	struct i2c_ipro_data *data = dev->data;
	int ret = 0;

	if (addr > 0x7f) {
		return -ENOTSUP;
	}

	k_mutex_lock(&data->lock, K_FOREVER);

	ret = i2c_ipro_bus_stuck(cfg);
	if (ret != 0) {
		k_mutex_unlock(&data->lock);
		return ret;
	}

	for (uint8_t i = 0; i < num_msgs && ret == 0; i++) {
		struct i2c_msg *m = &msgs[i];
		bool is_read = (m->flags & I2C_MSG_RW_MASK) == I2C_MSG_READ;

		if (m->flags & I2C_MSG_ADDR_10_BITS) {
			ret = -ENOTSUP;
			break;
		}

		/* Merge a short write with the message that follows it. */
		if (!is_read && m->len <= 2 && i + 1 < num_msgs &&
		    !(m->flags & I2C_MSG_STOP)) {
			struct i2c_msg *n = &msgs[i + 1];
			uint32_t sub = 0;

			for (uint8_t b = 0; b < m->len; b++) {
				sub |= (uint32_t)m->buf[b] << (8 * b);
			}
			ret = i2c_ipro_xfer(dev, addr, sub, m->len, n->buf, n->len,
					    (n->flags & I2C_MSG_RW_MASK) == I2C_MSG_READ);
			i++;
			continue;
		}

		ret = i2c_ipro_xfer(dev, addr, 0, 0, m->buf, m->len, is_read);
	}

	k_mutex_unlock(&data->lock);
	return ret;
}

static int i2c_ipro_configure(const struct device *dev, uint32_t dev_config)
{
	struct i2c_ipro_data *data = dev->data;

	if (!(dev_config & I2C_MODE_CONTROLLER)) {
		return -ENOTSUP;
	}
	switch (I2C_SPEED_GET(dev_config)) {
	case I2C_SPEED_STANDARD:
		data->bitrate = 100000;
		break;
	case I2C_SPEED_FAST:
		data->bitrate = 400000;
		break;
	default:
		return -ENOTSUP;
	}
	return 0;
}

static int i2c_ipro_get_config(const struct device *dev, uint32_t *dev_config)
{
	struct i2c_ipro_data *data = dev->data;

	*dev_config = I2C_MODE_CONTROLLER |
		      (data->bitrate >= 400000 ? I2C_SPEED_SET(I2C_SPEED_FAST)
					       : I2C_SPEED_SET(I2C_SPEED_STANDARD));
	return 0;
}

static int i2c_ipro_init(const struct device *dev)
{
	const struct i2c_ipro_config *cfg = dev->config;
	struct i2c_ipro_data *data = dev->data;
	int ret;

	k_mutex_init(&data->lock);
	data->bitrate = cfg->bitrate;

	ret = clock_control_on(cfg->clock_dev, cfg->clock_id);
	if (ret != 0) {
		return ret;
	}
	ret = pinctrl_apply_state(cfg->pincfg, PINCTRL_STATE_DEFAULT);
	if (ret != 0) {
		return ret;
	}
	I2C_Disable(cfg->id);
	I2C_SetDeglitchCount(cfg->id, 0);
	return 0;
}

static DEVICE_API(i2c, i2c_ipro_api) = {
	.configure = i2c_ipro_configure,
	.get_config = i2c_ipro_get_config,
	.transfer = i2c_ipro_transfer,
	.recover_bus = i2c_ipro_recover_bus,
#ifdef CONFIG_I2C_RTIO
	.iodev_submit = i2c_iodev_submit_fallback,
#endif
};

#define I2C_IPRO_INIT(n)                                                                           \
	PINCTRL_DT_INST_DEFINE(n);                                                                 \
	static const struct i2c_ipro_config i2c_ipro_cfg_##n = {                                   \
		.id = DT_INST_REG_ADDR(n) == I2C_BASE ? 0 : 1,                                     \
		.base = DT_INST_REG_ADDR(n),                                                       \
		.bitrate = DT_INST_PROP(n, clock_frequency),                                       \
		.pincfg = PINCTRL_DT_INST_DEV_CONFIG_GET(n),                                       \
		.clock_dev = DEVICE_DT_GET(DT_INST_CLOCKS_CTLR(n)),                                \
		.clock_id = (clock_control_subsys_t)DT_INST_CLOCKS_CELL(n, id),                    \
		.scl = GPIO_DT_SPEC_INST_GET_OR(n, scl_gpios, {0}),                                \
		.sda = GPIO_DT_SPEC_INST_GET_OR(n, sda_gpios, {0}),                                \
	};                                                                                         \
	static struct i2c_ipro_data i2c_ipro_data_##n;                                             \
	I2C_DEVICE_DT_INST_DEFINE(n, i2c_ipro_init, NULL, &i2c_ipro_data_##n, &i2c_ipro_cfg_##n,   \
				  POST_KERNEL, CONFIG_I2C_INIT_PRIORITY, &i2c_ipro_api);

DT_INST_FOREACH_STATUS_OKAY(I2C_IPRO_INIT)

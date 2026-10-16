/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * components/sensor's SCCB layer (sccb.h) on the Zephyr I2C API. The SDK
 * version muxes the pads and drives hal_i2c itself; here the board's i2c0
 * node (pinctrl, stuck-bus guard) is the bus and 7-bit addresses are passed
 * straight through, as the SDK's LL does (SC101IOT = 0x68).
 */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include "sccb.h"

static const struct device *const bus = DEVICE_DT_GET(DT_NODELABEL(i2c0));

int SCCB_Init(void)
{
	return device_is_ready(bus) ? 0 : -1;
}

int SCCB_Read(uint8_t slave_addr, uint8_t reg_addr, uint8_t *rdata, uint8_t rdsize)
{
	return i2c_write_read(bus, slave_addr, &reg_addr, 1, rdata, rdsize) == 0 ? 0 : -1;
}

int SCCB_Read_Reg16(uint8_t slave_addr, uint16_t reg_addr, uint8_t *rdata, uint8_t rdsize)
{
	uint8_t reg[2] = {(uint8_t)(reg_addr >> 8), (uint8_t)reg_addr};

	return i2c_write_read(bus, slave_addr, reg, 2, rdata, rdsize) == 0 ? 0 : -1;
}

int SCCB_Write(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data)
{
	uint8_t buf[2] = {reg_addr, *data};

	return i2c_write(bus, buf, 2, slave_addr) == 0 ? 0 : -1;
}

int SCCB_Write_Reg16(uint8_t slave_addr, uint16_t reg_addr, uint8_t *data)
{
	uint8_t buf[3] = {(uint8_t)(reg_addr >> 8), (uint8_t)reg_addr, *data};

	return i2c_write(bus, buf, 3, slave_addr) == 0 ? 0 : -1;
}

/* Returns the last address in [first, last] that answered a register-0 read, 0 if none. */
int SCCB_Scan(int first, int last)
{
	int last_acked = 0;

	for (int a = first; a <= last && a < 0x80; a++) {
		uint8_t tmp;

		if (SCCB_Read((uint8_t)a, 0x0, &tmp, 1) == 0) {
			last_acked = a;
		}
	}
	return last_acked;
}

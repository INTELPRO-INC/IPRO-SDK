/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * GPIO facade over the SDK LL (drv_gpio.c / drv_glb.c). Pure SDK side: no
 * Zephyr driver header here (drv_gpio.h's GPIO_PULL_* macros collide with
 * Zephyr's gpio.h flags).
 */

#include <ipro_zephyr/ipro_hal_glue.h>

#include "ipro7.h"
#include "glb_reg.h"
#include "drv_glb.h"
#include "drv_gpio.h"

static inline uint32_t glb_read(uint32_t ofs)
{
	return *(volatile uint32_t *)(GLB_BASE + ofs);
}

static inline void glb_write(uint32_t ofs, uint32_t v)
{
	*(volatile uint32_t *)(GLB_BASE + ofs) = v;
}

int ipro_hal_gpio_configure(uint8_t pad, enum ipro_hal_gpio_dir dir, enum ipro_hal_gpio_pull pull,
			    int init_level)
{
	GLB_GPIO_Cfg_Type c = {
		.gpioPin = pad,
		.gpioFun = GPIO_FUN_GPIO,
		.gpioMode = GPIO_MODE_INPUT,
		.pullType = GPIO_PULL_NONE,
		.drive = 0,
		.smtCtrl = 1,
	};

	if (pad >= GLB_GPIO_PIN_MAX) {
		return -1;
	}

	if (dir == IPRO_HAL_GPIO_DISCONNECTED) {
		return GLB_GPIO_Set_HZ(pad) == 0 ? 0 : -1;
	}

	if (pull == IPRO_HAL_GPIO_PULL_UP) {
		c.pullType = GPIO_PULL_UP;
	} else if (pull == IPRO_HAL_GPIO_PULL_DOWN) {
		c.pullType = GPIO_PULL_DOWN;
	}
	if (dir == IPRO_HAL_GPIO_OUTPUT) {
		c.gpioMode = GPIO_MODE_OUTPUT;
	}

	if (GLB_GPIO_Init(&c) != 0) {
		return -1;
	}

	if (dir == IPRO_HAL_GPIO_OUTPUT) {
		if (init_level >= 0) {
			GLB_GPIO_Write(pad, init_level);
		}
		GLB_GPIO_OUTPUT_Enable(pad);
	}
	/* GLB_GPIO_Init clears IE for outputs; keep the input path on so the
	 * pad level reads back (gpio_pin_get on an output, Zephyr semantics). */
	GLB_GPIO_INPUT_Enable(pad);
	return 0;
}

void ipro_hal_gpio_write(uint8_t pad, int level)
{
	GLB_GPIO_Write(pad, level);
}

int ipro_hal_gpio_read(uint8_t pad)
{
	return (int)GLB_GPIO_Read(pad);
}

void ipro_hal_gpio_int_enable(uint8_t pad, enum ipro_hal_gpio_trig trig)
{
	static const GLB_GPIO_INT_TRIG_Type map[] = {
		[IPRO_HAL_GPIO_TRIG_RISING_EDGE] = GLB_GPIO_INT_TRIG_POS_PULSE,
		[IPRO_HAL_GPIO_TRIG_FALLING_EDGE] = GLB_GPIO_INT_TRIG_NEG_PULSE,
		[IPRO_HAL_GPIO_TRIG_HIGH_LEVEL] = GLB_GPIO_INT_TRIG_POS_LEVEL,
		[IPRO_HAL_GPIO_TRIG_LOW_LEVEL] = GLB_GPIO_INT_TRIG_NEG_LEVEL,
	};

	GLB_GPIO_IntMask(pad, MASK);
	GLB_Set_GPIO_IntMod(pad, GLB_GPIO_INT_CONTROL_SYNC, map[trig]);
	GLB_GPIO_IntClear(pad, SET);
	GLB_GPIO_IntClear(pad, RESET);
	GLB_GPIO_IntMask(pad, UNMASK);
}

void ipro_hal_gpio_int_disable(uint8_t pad)
{
	GLB_GPIO_IntMask(pad, MASK);
	GLB_GPIO_IntClear(pad, SET);
	GLB_GPIO_IntClear(pad, RESET);
}

bool ipro_hal_gpio_int_pending(uint8_t pad)
{
	return GLB_Get_GPIO_IntStatus(pad) == SET;
}

void ipro_hal_gpio_int_clear(uint8_t pad)
{
	/* LL idiom: assert then release the clear bit. */
	GLB_GPIO_IntClear(pad, SET);
	GLB_GPIO_IntClear(pad, RESET);
}

/* Port 0 = pads 0-31, port 1 = pads 32-41 (bits 0-9 in / bits 12-21 out). */
uint32_t ipro_hal_gpio_in_reg(uint8_t port)
{
	if (port == 0) {
		return glb_read(GLB_GPIO_CFGCTL31_OFFSET);
	}
	return glb_read(GLB_GPIO_CFGCTL32_OFFSET) & 0x3FF;
}

uint32_t ipro_hal_gpio_out_reg(uint8_t port)
{
	if (port == 0) {
		return glb_read(GLB_GPIO_CFGCTL33_OFFSET);
	}
	return (glb_read(GLB_GPIO_CFGCTL34_OFFSET) >> 12) & 0x3FF;
}

void ipro_hal_gpio_out_reg_write(uint8_t port, uint32_t value)
{
	if (port == 0) {
		glb_write(GLB_GPIO_CFGCTL33_OFFSET, value);
	} else {
		uint32_t reg = glb_read(GLB_GPIO_CFGCTL34_OFFSET);

		reg = (reg & ~(0x3FFu << 12)) | ((value & 0x3FF) << 12);
		glb_write(GLB_GPIO_CFGCTL34_OFFSET, reg);
	}
}

/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * GPIO facade over the IPRO6 LL (drv_glb_gpio.c). Same interface as
 * ipro_gpio_glue.c (IPRO7); the register model differs: one GLB_GPIO_CFGn
 * word per pad (0x8C4 + 4*pad: ie/oe/pu/pd/func, int mode/mask/clear,
 * o/set/clr, i) plus aggregate 32-pad views GLB_GPIO_CFG128/129 (input),
 * 136/137 (output level). 35 pads: port 0 = 0-31, port 1 = 32-34.
 */

/* Zephyr headers first: the NMSIS core headers pulled in by ipro6.h define
 * likely/unlikely/STRINGIFY too, and only tolerate being the second definer. */
#include <zephyr/sys/util.h>
#include <ipro_zephyr/ipro_hal_glue.h>

#include "ipro6.h"
#include "glb_reg.h"
#include "drv_glb.h"
#include "drv_gpio.h"
#include "drv_glb_gpio.h"

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
		GLB_GPIO_Output_Enable(pad);
	}
	/* Keep the input path on for outputs too so the level reads back. */
	GLB_GPIO_Input_Enable(pad);
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
	GLB_GPIO_IntClear(pad, SET);
	GLB_GPIO_IntClear(pad, RESET);
}

/*
 * Port 0 = pads 0-31, port 1 = pads 32-34. The per-pad GLB_GPIO_CFGn word is
 * the authoritative view (what GLB_GPIO_Read()/Write() use); the aggregate
 * CFG128+ mirrors did not read back the pad level on the bench.
 */
#define PAD_CFG(pad) (GLB_GPIO_CFG0_OFFSET + 4u * (pad))
#define PAD_I_BIT    28
#define PAD_O_BIT    24

static inline uint8_t port_npads(uint8_t port)
{
	return port == 0 ? 32 : (GLB_GPIO_PIN_MAX - 32);
}

uint32_t ipro_hal_gpio_in_reg(uint8_t port)
{
	uint32_t v = 0;

	for (uint8_t i = 0; i < port_npads(port); i++) {
		if (glb_read(PAD_CFG(port * 32 + i)) & BIT(PAD_I_BIT)) {
			v |= BIT(i);
		}
	}
	return v;
}

uint32_t ipro_hal_gpio_out_reg(uint8_t port)
{
	uint32_t v = 0;

	for (uint8_t i = 0; i < port_npads(port); i++) {
		if (glb_read(PAD_CFG(port * 32 + i)) & BIT(PAD_O_BIT)) {
			v |= BIT(i);
		}
	}
	return v;
}

void ipro_hal_gpio_out_reg_write(uint8_t port, uint32_t value)
{
	for (uint8_t i = 0; i < port_npads(port); i++) {
		uint32_t ofs = PAD_CFG(port * 32 + i);
		uint32_t reg = glb_read(ofs);
		uint32_t want = (value & BIT(i)) ? BIT(PAD_O_BIT) : 0;

		if ((reg & BIT(PAD_O_BIT)) != want) {
			glb_write(ofs, (reg & ~BIT(PAD_O_BIT)) | want);
		}
	}
}

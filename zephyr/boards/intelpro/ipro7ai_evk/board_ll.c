/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * SDK LL side of the EVK board init (no Zephyr GPIO header here: the SDK's
 * GPIO_PULL_* names collide with Zephyr's).
 */
#include <stdint.h>
#include "ipro7.h"
#include "drv_glb.h"
#include "drv_gpio.h"

#define CAM_MCLK_PIN 41

/* GPIO41 = CAM_MISC (DVP_REF_CLK). */
void ipro7ai_evk_cam_mclk_pinmux(void)
{
	GLB_GPIO_Cfg_Type mclk = {
		.gpioMode = GPIO_MODE_AF,
		.pullType = GPIO_PULL_NONE,
		.drive = 3,
		.smtCtrl = 1,
		.gpioPin = CAM_MCLK_PIN,
		.gpioFun = GPIO_FUN_CAM_MISC,
	};

	GLB_GPIO_Init(&mclk);
}

/* 24 MHz from PLL48M / 2 onto the CAM reference clock. */
void ipro7ai_evk_cam_mclk_clock(void)
{
	/* The CAM reference clock divider lives in the MM (ISP / camera) domain,
	 * which the board clock init leaves gated; enabling the divider with MM
	 * gated wedges the APB (the CPU never returns from the next mtime wait,
	 * ipro7ai_evk 2026-09-21). Every SDK camera path ungates MM first. */
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);
	GLB_Set_CAM_CLK(DISABLE, GLB_CAM_CLK_PLL48M, 1);
	GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_PLL48M, 1);
}

/* GPIO35 = sensor reset, active low through Q3. assert = hold in reset. */
#define CAM_RST_PIN 35

void ipro7ai_evk_cam_reset(int assert)
{
	GLB_GPIO_Cfg_Type rst = {
		.gpioMode = GPIO_MODE_OUTPUT,
		.pullType = GPIO_PULL_UP,
		.drive = 0,
		.smtCtrl = 1,
		.gpioPin = CAM_RST_PIN,
		.gpioFun = GPIO_FUN_GPIO,
	};

	GLB_GPIO_Init(&rst);
	GLB_GPIO_Write(CAM_RST_PIN, assert ? 0 : 1);
	GLB_GPIO_OUTPUT_Enable(CAM_RST_PIN);
}

/* DVP: D0..D7 = GPIO0..7, PCLK 38, VSYNC 39, HSYNC 40 (function CAM). */
void ipro7ai_evk_cam_dvp_pinmux(void)
{
	GLB_GPIO_Cfg_Type cfg = {
		.gpioMode = GPIO_MODE_AF,
		.pullType = GPIO_PULL_NONE,
		.drive = 0,
		.smtCtrl = 1,
		.gpioFun = GPIO_FUN_CAM,
	};
	static const uint8_t pins[] = {0, 1, 2, 3, 4, 5, 6, 7, 38, 39, 40};

	for (unsigned int i = 0; i < sizeof(pins); i++) {
		cfg.gpioPin = pins[i];
		GLB_GPIO_Init(&cfg);
	}
}

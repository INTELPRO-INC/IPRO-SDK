/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7AI EVK board bring-up that no Zephyr driver covers: the LCD backlight
 * enable and the camera's MCLK + reset (schematic IPRO7AI_MODULE_SUB_0812A,
 * mirrored from apps/platform/ipro7ai_evk_demo).
 */
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/drivers/gpio.h>

/* SDK GPIO names (GPIO_PULL_UP, ...) collide with Zephyr's, so the LL part
 * lives in board_ll.c which includes SDK headers only. */
void ipro7ai_evk_cam_mclk_pinmux(void);
void ipro7ai_evk_cam_mclk_clock(void);

#ifdef CONFIG_BOARD_IPRO7AI_EVK_TRACE
/* printk only fills the UART FIFO; spin (no timer involved) so the line is
 * on the wire before the next step can wedge the bus. */
#define TRACE(s) do { printk("[board] " s "\n"); \
		for (volatile int _i = 0; _i < 400000; _i++) { } } while (0)
#else
#define TRACE(s) do {} while (0)
#endif

static int ipro7ai_evk_board_init(void)
{
#ifdef CONFIG_BOARD_IPRO7AI_EVK_TRACE
	printk("\n[board] ---- init start ----\n");
	printk("[board] GLB CLK_CFG0 %08x CFG1 %08x CFG2 %08x CFG5 %08x CGEN1 %08x\n",
	       sys_read32(0x30000000), sys_read32(0x30000004), sys_read32(0x30000008),
	       sys_read32(0x30000014), sys_read32(0x30000034));
	printk("[board] CCI PLL_OUT_EN %08x DLL2 %08x\n", sys_read32(0x3000941c),
	       sys_read32(0x30009204));
	for (volatile int i = 0; i < 400000; i++) { }
#endif
#ifdef CONFIG_BOARD_IPRO7AI_EVK_LCD_BACKLIGHT
	{
		const struct gpio_dt_spec bl = GPIO_DT_SPEC_GET(DT_NODELABEL(lcd_backlight), gpios);

		if (gpio_is_ready_dt(&bl)) {
			(void)gpio_pin_configure_dt(&bl, GPIO_OUTPUT_ACTIVE);
		}
	}
#endif
#ifdef CONFIG_BOARD_IPRO7AI_EVK_CAMERA_POWER
	{
		const struct gpio_dt_spec rst = GPIO_DT_SPEC_GET(DT_NODELABEL(cam_reset), gpios);

		if (gpio_is_ready_dt(&rst)) {
			/* Hold the sensor in reset, start MCLK (24 MHz), release. */
			TRACE("cam reset low");
			(void)gpio_pin_configure_dt(&rst, GPIO_OUTPUT_ACTIVE);
#ifndef CONFIG_BOARD_IPRO7AI_EVK_CAMERA_NO_MCLK_PINMUX
			TRACE("cam mclk pinmux");
			ipro7ai_evk_cam_mclk_pinmux();
#endif
#ifndef CONFIG_BOARD_IPRO7AI_EVK_CAMERA_NO_MCLK_CLOCK
			TRACE("cam mclk clock");
			ipro7ai_evk_cam_mclk_clock();
#endif
			TRACE("cam wait 2 ms");
			k_busy_wait(2000);
			TRACE("cam reset release");
			(void)gpio_pin_set_dt(&rst, 0);
			k_busy_wait(20000); /* MCLK + sensor PLL settle */
			TRACE("cam done");
		}
	}
#endif
	TRACE("init done");
	return 0;
}

/* POST_KERNEL, after the flash driver (its boot prints are the first thing a
 * slow-to-reconnect USB console shows) and before the shell backend (90)
 * takes the console UART over. */
SYS_INIT(ipro7ai_evk_board_init, POST_KERNEL, 85);

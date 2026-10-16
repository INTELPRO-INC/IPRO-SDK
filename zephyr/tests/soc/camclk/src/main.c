/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Board probe: route the camera reference clock (CAM_MISC on GPIO41) and
 * enable it, reading mtime around every step. Reproduces the 2026-09-21
 * ipro7ai_evk hang (a pad re-muxed to a toggling alternate function raised a
 * GPIO INT0 status the driver never cleared) and stays as a regression check
 * for the GPIO driver's foreign-interrupt handling.
 */
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "ipro7.h"
#include "drv_glb.h"
#include "drv_gpio.h"
#include "drv_mm_misc.h"
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include "drv_i2c.h"

#include <soc.h>
#define MTIME (*(volatile uint32_t *)(ipro_systimer_base() + 0x000))

static void spin(void) { for (volatile int i = 0; i < 400000; i++) { } }
static void report(const char *tag)
{
	uint32_t a = MTIME; spin(); uint32_t b = MTIME;
	printk("%-24s mtime %08x -> %08x (%s) CFG5 %08x\n", tag, a, b, b != a ? "runs" : "STOPPED",
	       *(volatile uint32_t *)(GLB_BASE + 0x14));
	spin();
}

int main(void)
{
	GLB_GPIO_Cfg_Type mclk = { .gpioMode = GPIO_MODE_AF, .pullType = GPIO_PULL_NONE, .drive = 3,
				   .smtCtrl = 1, .gpioPin = 41, .gpioFun = GPIO_FUN_CAM_MISC };
	printk("camclk probe\n");
	report("start");
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM); report("MM ungated");
	printk("MM_MISC RST0 %08x\n", *(volatile uint32_t *)(MM_MISC_BASE + 0x0)); spin();
	MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP_AS, 1);
	MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP_AS, 0);
	MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP2AXI_C, 1);
	MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP2AXI_C, 0);
	MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_ISP, 1);
	MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_ISP, 0);
	report("MM misc resets done");
	GLB_GPIO_Init(&mclk); report("GPIO41 CAM_MISC");
	GLB_Set_CAM_CLK(DISABLE, GLB_CAM_CLK_XCLK, 1); report("cam XCLK/2 disabled");
	{
		volatile uint32_t *cfg5 = (volatile uint32_t *)(GLB_BASE + 0x14);
		uint32_t v = *cfg5;

		{
			volatile uint8_t *ctl = (volatile uint8_t *)(ipro_eclic_base() + 0x1000);
			printk("enabled ECLIC lines:");
			for (int irq = 0; irq < CONFIG_NUM_IRQS; irq++) {
				if (ctl[irq * 4 + 1] & 1) { printk(" %d", irq); }
			}
			printk("\n"); spin();
		}
		printk("raw: about to set bit7 (v=%08x)\n", v); spin();
		*cfg5 = v | (1U << 7);
		printk("raw: write returned\n"); spin();
		printk("raw: readback %08x\n", *cfg5); spin();
		printk("raw: mtime %08x\n", MTIME); spin();
		printk("raw: mtime %08x\n", MTIME); spin();

	}
	report("cam XCLK/2 ENABLED");
	GLB_Set_CAM_CLK(DISABLE, GLB_CAM_CLK_XCLK, 1); report("cam disabled again");
	GLB_Set_CAM_CLK(DISABLE, GLB_CAM_CLK_PLL48M, 1); report("cam PLL48M/2 disabled");
	GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_PLL48M, 1); report("cam PLL48M/2 ENABLED");
	k_busy_wait(2000); report("after k_busy_wait");
#if DT_NODE_HAS_STATUS(DT_NODELABEL(i2c0), okay) && defined(CONFIG_I2C)
	{
		/* SCCB probe like components/sensor/sc101iot.c: ID at 0xf7/0xf8 */
		const struct device *i2c = DEVICE_DT_GET(DT_NODELABEL(i2c0));
		const struct gpio_dt_spec scl = GPIO_DT_SPEC_GET(DT_NODELABEL(i2c0), scl_gpios);
		const struct gpio_dt_spec sda = GPIO_DT_SPEC_GET(DT_NODELABEL(i2c0), sda_gpios);
		const uint16_t addrs[] = {0x68, 0x34};

		/* An AF pad reads back 0 through the GPIO input register: sample the
		 * lines the way the driver does, as GPIO inputs, then re-mux. */
		gpio_pin_configure_dt(&scl, GPIO_INPUT | GPIO_PULL_UP);
		gpio_pin_configure_dt(&sda, GPIO_INPUT | GPIO_PULL_UP);
		printk("i2c0 lines: SCL=%d SDA=%d (1 = pulled up)\n",
		       gpio_pin_get_raw(scl.port, scl.pin), gpio_pin_get_raw(sda.port, sda.pin));
		i2c_recover_bus(i2c); /* re-applies the I2C mux */
		for (int round = 0; round < 3; round++) {
			for (int a = 0; a < 2; a++) {
				uint8_t id[2] = {0, 0}, reg = 0xf7;
				int r1 = i2c_write_read(i2c, addrs[a], &reg, 1, &id[0], 1);

				reg = 0xf8;
				int r2 = i2c_write_read(i2c, addrs[a], &reg, 1, &id[1], 1);

				printk("sccb 0x%02x: id 0xf7=%02x (%d) 0xf8=%02x (%d)%s\n", addrs[a],
				       id[0], r1, id[1], r2,
				       (r1 == 0 && id[0] == 0xda && id[1] == 0x4a) ? "  <- SC101IOT" : "");
				{
					uint8_t wr[2] = {0xf0, 0x00};
					int r3 = i2c_write(i2c, wr, 2, addrs[a]);
					uintptr_t b = 0x3000e000; /* I2C0 */

					printk("   plain write (%d); CONFIG %08x INT_STS %08x BUSY %08x PRD %08x/%08x/%08x FIFO0 %08x FIFO1 %08x\n",
					       r3, sys_read32(b), sys_read32(b + 4), sys_read32(b + 0xc),
					       sys_read32(b + 0x10), sys_read32(b + 0x14), sys_read32(b + 0x18),
					       sys_read32(b + 0x80), sys_read32(b + 0x84));
				}
			}
			k_msleep(300);
		}
	}
#endif
	printk("PROBE DONE\n");
	return 0;
}

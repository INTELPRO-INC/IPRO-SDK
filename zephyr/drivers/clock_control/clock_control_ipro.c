/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * clock_control adapter for the IPRO7 GLB block.
 *
 *   clock_control_on/off(dev, (clock_control_subsys_t)IPRO7_CLK_x)
 *       -> GLB_PER_Clock_UnGate / GLB_PER_Clock_Gate  (bsp std_drv)
 *   clock_control_get_rate(dev, IPRO7_CLK_x, &rate)
 *       -> Clock_Peripheral_Clock_Get / Clock_System_Clock_Get
 *
 * The functional-clock dividers themselves (GLB_Set_UART_CLK & co.) are
 * programmed once here with the SDK board defaults so peripheral drivers
 * only need the gate and the resulting rate.
 */

#define DT_DRV_COMPAT intelpro_ipro_glb_clock

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/clock_control.h>
#ifdef CONFIG_SOC_SERIES_IPRO6S
#include <zephyr/dt-bindings/clock/ipro6s-clock.h>
#else
#include <zephyr/dt-bindings/clock/ipro7-clock.h>
#endif
#include <zephyr/logging/log.h>

#include "ipro_soc_regs.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "drv_clock.h"
#include "drv_timer.h"
#ifdef CONFIG_SOC_SERIES_IPRO6S
#include "timer_reg.h"
#endif

LOG_MODULE_REGISTER(clock_control_ipro, CONFIG_CLOCK_CONTROL_LOG_LEVEL);

static inline uint32_t sub_id(clock_control_subsys_t sys)
{
	return (uint32_t)(uintptr_t)sys;
}

/*
 * IPRO7's GLB_PER_Clock_(Un)Gate() takes the module index; IPRO6's takes a
 * 64-bit mask of modules (GLB_AHB_CLOCK_* = 1ULL << GLB_AHB_CLOCK_IP_*).
 * The device-tree id is the index on both chips.
 */
#ifdef CONFIG_SOC_SERIES_IPRO6S
#define IPRO_CLK_GATE_ARG(id) (1ULL << (id))
#else
#define IPRO_CLK_GATE_ARG(id) ((GLB_AHB_CLOCK_Type)(id))
#endif

static int ipro_clk_on(const struct device *dev, clock_control_subsys_t sys)
{
	ARG_UNUSED(dev);
	return GLB_PER_Clock_UnGate(IPRO_CLK_GATE_ARG(sub_id(sys))) == 0 ? 0 : -EIO;
}

static int ipro_clk_off(const struct device *dev, clock_control_subsys_t sys)
{
	ARG_UNUSED(dev);
	return GLB_PER_Clock_Gate(IPRO_CLK_GATE_ARG(sub_id(sys))) == 0 ? 0 : -EIO;
}

static enum clock_control_status ipro_clk_status(const struct device *dev,
						 clock_control_subsys_t sys)
{
	ARG_UNUSED(dev);
	uint32_t id = sub_id(sys);
	uint32_t reg;

#ifdef CONFIG_SOC_SERIES_IPRO6S
	/* No public gate-status accessor in the IPRO6 LL. */
	ARG_UNUSED(id);
	ARG_UNUSED(reg);
	return CLOCK_CONTROL_STATUS_UNKNOWN;
#else
	/* GLB_CGEN_CFG0/1/2: bit set = clock enabled (same map as UnGate). */
	if (id < 32) {
		reg = *(volatile uint32_t *)(GLB_BASE + GLB_CGEN_CFG0_OFFSET);
	} else if (id < 64) {
		reg = *(volatile uint32_t *)(GLB_BASE + GLB_CGEN_CFG1_OFFSET);
	} else if (id < 96) {
		reg = *(volatile uint32_t *)(GLB_BASE + GLB_CGEN_CFG2_OFFSET);
	} else {
		return CLOCK_CONTROL_STATUS_UNKNOWN;
	}
	return (reg & BIT(id % 32)) ? CLOCK_CONTROL_STATUS_ON : CLOCK_CONTROL_STATUS_OFF;
#endif
}

static int ipro_clk_get_rate(const struct device *dev, clock_control_subsys_t sys, uint32_t *rate)
{
	ARG_UNUSED(dev);

#ifdef CONFIG_SOC_SERIES_IPRO6S
#define IPRO_CLK_UART0 IPRO6S_CLK_UART0
#define IPRO_CLK_UART1 IPRO6S_CLK_UART1
#define IPRO_CLK_SPI0  IPRO6S_CLK_SPI0
#define IPRO_CLK_SPI1  IPRO6S_CLK_SPI0
#define IPRO_CLK_I2C0  IPRO6S_CLK_I2C0
#define IPRO_CLK_I2C1  IPRO6S_CLK_I2C1
#define IPRO_CLK_PWM   IPRO6S_CLK_PWM
#define IPRO_CLK_TIMER IPRO6S_CLK_TIMER
#define IPRO_CLK_I2S   IPRO6S_CLK_I2S
#define IPRO_CLK_MCU   IPRO6S_CLK_CPU
#define IPRO_SYSCLK_CPU DRV_SYSTEM_CLOCK_MCU_CLK
#define IPRO_SYSCLK_BUS DRV_SYSTEM_CLOCK_MCU_PBCLK
#else
#define IPRO_CLK_UART0 IPRO7_CLK_UART0
#define IPRO_CLK_UART1 IPRO7_CLK_UART1
#define IPRO_CLK_SPI0  IPRO7_CLK_SPI0
#define IPRO_CLK_SPI1  IPRO7_CLK_SPI1
#define IPRO_CLK_I2C0  IPRO7_CLK_I2C0
#define IPRO_CLK_I2C1  IPRO7_CLK_I2C1
#define IPRO_CLK_PWM   IPRO7_CLK_PWM
#define IPRO_CLK_TIMER IPRO7_CLK_TIMER
#define IPRO_CLK_I2S   IPRO7_CLK_I2S
#define IPRO_CLK_MCU   IPRO7_CLK_MCU
#define IPRO_SYSCLK_CPU DRV_SYSTEM_CLOCK_FCLK
#define IPRO_SYSCLK_BUS DRV_SYSTEM_CLOCK_BCLK
#endif

	switch (sub_id(sys)) {
	case IPRO_CLK_UART0:
		*rate = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_UART0);
		break;
	case IPRO_CLK_UART1:
		*rate = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_UART1);
		break;
#if IPRO_CLK_SPI0 != IPRO_CLK_SPI1
	case IPRO_CLK_SPI1:
#endif
	case IPRO_CLK_SPI0:
#ifdef CONFIG_SOC_SERIES_IPRO6S
		*rate = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_SPI);
#else
		*rate = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_SPI0);
#endif
		break;
	case IPRO_CLK_I2C0:
	case IPRO_CLK_I2C1:
		*rate = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_I2C0);
		break;
	case IPRO_CLK_PWM:
#ifdef CONFIG_SOC_SERIES_IPRO6S
		/* No PWM entry in the IPRO6 clock tree API; the PWM driver derives
		 * its rate from the DT clock-source/divider (XCLK here). */
		*rate = Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK);
#else
		*rate = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_PWM);
#endif
		break;
	case IPRO_CLK_TIMER:
#ifdef CONFIG_SOC_SERIES_IPRO6S
		/* ipro_clk_init(): XTAL / (TCDR2 + 1) on both channels. */
		*rate = Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XTAL) /
			(reg_get_bits(*(volatile uint32_t *)(TIMER_BASE + TIMER_TCDR_OFFSET),
				      TIMER_TCDR2) + 1u);
#else
		*rate = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_TIMER0);
#endif
		break;
	case IPRO_CLK_I2S:
		*rate = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_I2S);
		break;
	case IPRO_CLK_MCU:
		*rate = Clock_System_Clock_Get(IPRO_SYSCLK_CPU);
		break;
	default:
		/* Plain AHB peripherals run on BCLK. */
		*rate = Clock_System_Clock_Get(IPRO_SYSCLK_BUS);
		break;
	}
	return 0;
}

static int ipro_clk_init(const struct device *dev)
{
	ARG_UNUSED(dev);

	/*
	 * Functional clock dividers, identical to BOARD_PERIPHERAL_CLOCK_INIT()
	 * in bsp/board/ipro7/ipro7_evb_board_config.h:
	 *   UART  <- XCLK (32 MHz) / 1
	 *   SPI   <- BCLK (96 MHz) / 1
	 *   TIMER0 ch0/ch1 <- XTAL / 32  (1 MHz)
	 *   WDT   <- 32 kHz / 32
	 */
#ifdef CONFIG_SOC_SERIES_IPRO6S
	/* IPRO6: XCLK is the 40 MHz crystal; TIMER0 XTAL/40 = 1 MHz. The IPRO6
	 * LL only sets the timer clock inside TIMER_Init(); program TCCR +
	 * the divider directly, the same registers TIMER_Init() writes. */
	/* The TIMER block is clock-gated at reset on IPRO6 (TIMER_Init() ungates
	 * it first); writes to a gated block are dropped. Ungate what this
	 * function programs before touching it. */
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_CPU | GLB_AHB_CLOCK_EF_CTRL | GLB_AHB_CLOCK_SF_CTRL |
			     GLB_AHB_CLOCK_TIMER);

	GLB_Set_UART_CLK(1, HBN_UART_CLK_XCLK, 0);
	GLB_Set_SPI_CLK(ENABLE, GLB_SPI_CLK_XCLK, 0);
	{
		uint32_t tccr = *(volatile uint32_t *)(TIMER_BASE + TIMER_TCCR_OFFSET);
		uint32_t tcdr;

		tccr = reg_set_bits(tccr, TIMER_CS_2, TIMER_CLKSRC_XTAL);
		tccr = reg_set_bits(tccr, TIMER_CS_3, TIMER_CLKSRC_XTAL);
		*(volatile uint32_t *)(TIMER_BASE + TIMER_TCCR_OFFSET) = tccr;
		tcdr = *(volatile uint32_t *)(TIMER_BASE + TIMER_TCDR_OFFSET);
		tcdr = reg_set_bits(tcdr, TIMER_TCDR2, 39);
		tcdr = reg_set_bits(tcdr, TIMER_TCDR3, 39);
		*(volatile uint32_t *)(TIMER_BASE + TIMER_TCDR_OFFSET) = tcdr;
	}
	WDT_Set_Clock(WDT0_ID, TIMER_CLKSRC_32K, 31);
#else
	/*
	 * Same set and order as the SDK's BOARD_PERIPHERAL_CLOCK_INIT()
	 * (bsp/board/ipro7 board config headers). GLB_Set_USB_CLK() also switches
	 * the PLL 48 MHz divider output on (PLL_48M_DIV_EN); the camera MCLK
	 * (GLB_Set_CAM_CLK PLL48M) selects that branch and a clock mux fed a
	 * dead clock wedges the bus (ipro7ai_evk 2026-09-21), so keep it even
	 * when USB itself is unused.
	 */
	GLB_Set_UART_CLK(1, HBN_UART_CLK_XCLK, 0);
	GLB_Set_I2S_CLK(1, GLB_I2S_OUT_REF_CLK_NONE);
	TIMER_Set_Clock(TIMER0_ID, TIMER_CH0, TIMER_CLKSRC_XTAL, 31);
	TIMER_Set_Clock(TIMER0_ID, TIMER_CH1, TIMER_CLKSRC_XTAL, 31);
	WDT_Set_Clock(WDT0_ID, TIMER_CLKSRC_32K, 31);
	GLB_Set_SDH_CLK(1, GLB_SDH_CLK_PLL_96M, 1);
	GLB_Set_USB_CLK(1);
	GLB_Set_SPI_CLK(ENABLE, GLB_SPI_CLK_BCLK, 0);

	/* Gates needed by the kernel itself. */
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MCU);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_EF_CTRL);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SF_CTRL);
	/*
	 * And the same baseline the SDK's BOARD_CLOCK_GATING_INIT() opens on
	 * every IPRO7 board. Drivers still ungate their own block through the
	 * clock_control API; this keeps blocks whose clock trees feed others
	 * (USB: the PLL 48 MHz divider; SDH: PLL 96 MHz) alive like the SDK.
	 * MM (ISP / camera) stays gated, as in the SDK.
	 */
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DMA_0);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_UART0);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_UART1);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SPI0);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SPI1);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_TIMER);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_USB);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SDH);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_BLE);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_M154);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SEC_ENG);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_PKA);
#endif
	return 0;
}

static DEVICE_API(clock_control, ipro_clk_api) = {
	.on = ipro_clk_on,
	.off = ipro_clk_off,
	.get_rate = ipro_clk_get_rate,
	.get_status = ipro_clk_status,
};

DEVICE_DT_INST_DEFINE(0, ipro_clk_init, NULL, NULL, NULL, PRE_KERNEL_1,
		      CONFIG_CLOCK_CONTROL_INIT_PRIORITY, &ipro_clk_api);

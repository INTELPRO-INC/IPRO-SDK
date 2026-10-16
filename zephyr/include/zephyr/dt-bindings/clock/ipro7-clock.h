/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 GLB AHB clock gate identifiers (GLB_AHB_CLOCK_Type in
 * bsp/drivers/ipro7_soc/regs/ipro7.h). Used as the single clock cell of
 * the intelpro,ipro-glb-clock controller.
 */

#ifndef ZEPHYR_INCLUDE_DT_BINDINGS_CLOCK_IPRO7_CLOCK_H_
#define ZEPHYR_INCLUDE_DT_BINDINGS_CLOCK_IPRO7_CLOCK_H_

#define IPRO7_CLK_MCU      0
#define IPRO7_CLK_SEC      1
#define IPRO7_CLK_DMA0     2
#define IPRO7_CLK_CCI      3
#define IPRO7_CLK_GPIP     4
#define IPRO7_CLK_SEC_DBG  5
#define IPRO7_CLK_SEC_ENG  6
#define IPRO7_CLK_TZ1      7
#define IPRO7_CLK_EF_CTRL  8
#define IPRO7_CLK_SF_CTRL  9
#define IPRO7_CLK_SPI1     10
#define IPRO7_CLK_UART0    11
#define IPRO7_CLK_SPI0     12
#define IPRO7_CLK_I2C0     13
#define IPRO7_CLK_PWM      14
#define IPRO7_CLK_TIMER    15
#define IPRO7_CLK_IRR      16
#define IPRO7_CLK_CHECKSUM 17
#define IPRO7_CLK_KYS      18
#define IPRO7_CLK_AUDIO    19
#define IPRO7_CLK_MM       20
#define IPRO7_CLK_UART1    21
#define IPRO7_CLK_CAN      22
#define IPRO7_CLK_I2S      23
#define IPRO7_CLK_QDEC0    24
#define IPRO7_CLK_QDEC1    25
#define IPRO7_CLK_QDEC2    26
#define IPRO7_CLK_PIO      27
#define IPRO7_CLK_USB      28
#define IPRO7_CLK_PKA      29
#define IPRO7_CLK_EMAC     30
#define IPRO7_CLK_SDH      31
#define IPRO7_CLK_I2C1     46
#define IPRO7_CLK_M154     64
#define IPRO7_CLK_BLE      68

#endif /* ZEPHYR_INCLUDE_DT_BINDINGS_CLOCK_IPRO7_CLOCK_H_ */

/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Clock ids for the IPRO6S GLB clock controller: the index of the module in
 * the SDK's GLB_AHB_CLOCK_IP_* enum (bsp/drivers/ipro6_soc/std_drv/inc/
 * drv_glb.h). The driver turns it into the 1ULL << id gate mask.
 */

#ifndef ZEPHYR_INCLUDE_DT_BINDINGS_CLOCK_IPRO6S_CLOCK_H_
#define ZEPHYR_INCLUDE_DT_BINDINGS_CLOCK_IPRO6S_CLOCK_H_

#define IPRO6S_CLK_CPU       0
#define IPRO6S_CLK_SEC       2
#define IPRO6S_CLK_DMA0      3
#define IPRO6S_CLK_CCI       6
#define IPRO6S_CLK_RF_TOP    7
#define IPRO6S_CLK_GPIP      8
#define IPRO6S_CLK_TZC       9
#define IPRO6S_CLK_EF_CTRL   10
#define IPRO6S_CLK_SF_CTRL   11
#define IPRO6S_CLK_EMAC      12
#define IPRO6S_CLK_UART0     13
#define IPRO6S_CLK_UART1     14
#define IPRO6S_CLK_SPI0      18
#define IPRO6S_CLK_I2C0      19
#define IPRO6S_CLK_PWM       20
#define IPRO6S_CLK_TIMER     21
#define IPRO6S_CLK_I2S       26
#define IPRO6S_CLK_USB       27
#define IPRO6S_CLK_BLE       30
#define IPRO6S_CLK_M154      32
#define IPRO6S_CLK_WIFI      34
#define IPRO6S_CLK_SDH       44
#define IPRO6S_CLK_I2C1      47

#endif /* ZEPHYR_INCLUDE_DT_BINDINGS_CLOCK_IPRO6S_CLOCK_H_ */

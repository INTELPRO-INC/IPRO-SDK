/* Copyright (c) 2025 IPRO
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file board_config.c
 * @brief IPRO7 board configuration for Zephyr
 * 
 * This file provides REAL implementations of board initialization functions
 * that are required for UART and other peripherals to work.
 * 
 * The initialization sequence follows the FreeRTOS SDK:
 * 1. board_em_init()      - Energy Mode (OCRAM allocation)
 * 2. board_clock_init()   - System and peripheral clocks
 * 3. board_pin_mux_init() - GPIO pin multiplexing (CRITICAL for UART!)
 */

#include <stdint.h>

/* SDK Driver includes */
#include "drv_glb.h"
#include "drv_gpio.h"
#include "drv_hbn.h"
#include "drv_clock.h"

void board_em_init(void)
{
	GLB_Set_EM_Sel(GLB_EM_0KB);
}

void board_clock_init(void)
{
	GLB_Set_System_CLK(GLB_DLL_XTAL_32M, GLB_SYS_CLK_PLL192M);
	GLB_Set_System_CLK_Div(0, 1);
	GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_XCLK, 
Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK) / 1000 / 1000 / 2 - 1);
	HBN_32K_Sel(HBN_32K_RC);
	HBN_Power_Off_Xtal_32K();
	HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_XTAL);
	GLB_Set_I2C_CLK(ENABLE, 0);
	GLB_Set_SPI_CLK(ENABLE, GLB_SPI_CLK_BCLK, 0);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_TIMER);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DMA_0);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_UART0);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_UART1);
	GLB_Set_USB_CLK(1);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_USB);
	GLB_Set_I2S_CLK(ENABLE, GLB_I2S_OUT_REF_CLK_NONE);
	GLB_Set_SDH_CLK(1, GLB_SDH_CLK_PLL_96M, 0);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SDH);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SPI0);
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SPI1);
}

static void gpio_test_init(void);
static void gpio_test_toggle(uint8_t pin);
static void gpio_test_pulse(uint8_t pin, int count);

void board_pin_mux_init(void)
{
	GLB_GPIO_Cfg_Type cfg;
	cfg.gpioFun = GPIO_FUN_UART;
	cfg.pullType = GPIO_PULL_UP;
	cfg.drive = 0;
	cfg.smtCtrl = 1;
	cfg.gpioPin = GLB_GPIO_PIN_20;
	cfg.gpioMode = GPIO_MODE_OUTPUT;
	GLB_GPIO_Init(&cfg);
	cfg.gpioPin = GLB_GPIO_PIN_21;
	cfg.gpioMode = GPIO_MODE_INPUT;
	GLB_GPIO_Init(&cfg);
	GLB_UART_Fun_Sel(GLB_UART_SIG_9, GLB_UART_SIG_FUN_UART0_RXD);
	GLB_UART_Fun_Sel(GLB_UART_SIG_8, GLB_UART_SIG_FUN_UART0_TXD);
	gpio_test_init();
	gpio_test_pulse(29, 3);
}

int board_get_debug_uart_index(void) { return 0; }
void debug_uart_id_set(uint8_t id) { (void)id; }
int uart_init(int uart_id, uint16_t caps) { (void)uart_id; (void)caps; return 0; }
int uart_set_baudrate(int uart_id, uint32_t baudrate) { (void)uart_id; (void)baudrate; return 0; }
void uart_uninit(int uart_id) { (void)uart_id; }

#include "drv_ef_ctrl.h"

void EF_Ctrl_Read_Common_Trim(char *name, Efuse_Common_Trim_Type *trim, uint8_t reload)
{
	(void)name; (void)reload;
	trim->en = 0; trim->parity = 0; trim->empty = 1; trim->len = 0; trim->value = 0;
}

uint8_t EF_Ctrl_Get_Trim_Parity(uint32_t val, uint8_t len)
{
	(void)val; (void)len; return 0;
}

static void gpio_test_init(void)
{
	GLB_GPIO_Cfg_Type cfg;
	cfg.gpioPin = GLB_GPIO_PIN_29;
	cfg.gpioFun = GPIO_FUN_GPIO;
	cfg.gpioMode = GPIO_MODE_OUTPUT;
	cfg.pullType = GPIO_PULL_NONE;
	cfg.drive = 0;
	cfg.smtCtrl = 1;
	GLB_GPIO_Init(&cfg);
	GLB_GPIO_Write(GLB_GPIO_PIN_29, 0);
	cfg.gpioPin = GLB_GPIO_PIN_30;
	GLB_GPIO_Init(&cfg);
	GLB_GPIO_Write(GLB_GPIO_PIN_30, 0);
}

static void gpio_test_toggle(uint8_t pin)
{
	static uint8_t state29 = 0, state30 = 0;
	if (pin == 29) {
		state29 = !state29;
		GLB_GPIO_Write(GLB_GPIO_PIN_29, state29);
	} else if (pin == 30) {
		state30 = !state30;
		GLB_GPIO_Write(GLB_GPIO_PIN_30, state30);
	}
}

static void gpio_test_pulse(uint8_t pin, int count)
{
	uint8_t gpio_pin = (pin == 29) ? GLB_GPIO_PIN_29 : GLB_GPIO_PIN_30;
	for (int i = 0; i < count; i++) {
		GLB_GPIO_Write(gpio_pin, 1);
		for (volatile int d = 0; d < 50000; d++);
		GLB_GPIO_Write(gpio_pin, 0);
		for (volatile int d = 0; d < 50000; d++);
	}
}

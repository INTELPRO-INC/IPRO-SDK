/*
 * esp32_wifi_hw.c - ESP32 WiFi HW abstraction (UART + GPIO)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp32_wifi_hw.h"

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "hal_uart.h"
#include "drv_glb.h"
#include "drv_gpio.h"
#include "drv_clock.h"
#include "ipro_log.h"

#define TAG "ESP32_HW"

/* Kconfig fallbacks */
#ifndef CONFIG_ESP32_WIFI_GPIO_TX
#define CONFIG_ESP32_WIFI_GPIO_TX   4
#endif
#ifndef CONFIG_ESP32_WIFI_GPIO_RX
#define CONFIG_ESP32_WIFI_GPIO_RX   3
#endif
#ifndef CONFIG_ESP32_WIFI_UART_ID
#define CONFIG_ESP32_WIFI_UART_ID   1
#endif
#ifndef CONFIG_ESP32_WIFI_UART_BAUD
#define CONFIG_ESP32_WIFI_UART_BAUD 115200
#endif
#ifndef CONFIG_ESP32_WIFI_GPIO_EN
#define CONFIG_ESP32_WIFI_GPIO_EN   (-1)
#endif

/* ------------------------------------------------------------------ */
/* UART signal helpers                                                */
/* ------------------------------------------------------------------ */

/*
 * GLB_UART_SIG_FUN_Type enum layout (per drv_glb.h):
 *   UART0_RTS=0, UART0_CTS=1, UART0_TXD=2, UART0_RXD=3,
 *   UART1_RTS=4, UART1_CTS=5, UART1_TXD=6, UART1_RXD=7,
 *   ...
 * Each UART occupies 4 slots.  TXD is at offset 2, RXD at offset 3.
 */
static inline GLB_UART_SIG_FUN_Type uart_txd_sig(int id)
{
    return (GLB_UART_SIG_FUN_Type)(GLB_UART_SIG_FUN_UART0_TXD + id * 4);
}

static inline GLB_UART_SIG_FUN_Type uart_rxd_sig(int id)
{
    return (GLB_UART_SIG_FUN_Type)(GLB_UART_SIG_FUN_UART0_RXD + id * 4);
}

/* ------------------------------------------------------------------ */
/* UART init                                                          */
/* ------------------------------------------------------------------ */

void esp32_hw_uart_init(void)
{
    GLB_GPIO_Cfg_Type uart_gpio;

    /* TX GPIO - UART function, output, pull-up */
    uart_gpio.gpioPin  = CONFIG_ESP32_WIFI_GPIO_TX;
    uart_gpio.gpioFun  = GPIO_FUN_UART;
    uart_gpio.gpioMode = GPIO_MODE_OUTPUT;
    uart_gpio.pullType = GPIO_PULL_UP;
    uart_gpio.drive    = 0;
    uart_gpio.smtCtrl  = 1;
    GLB_GPIO_Init(&uart_gpio);

    /* RX GPIO - UART function, input, pull-up */
    uart_gpio.gpioPin  = CONFIG_ESP32_WIFI_GPIO_RX;
    uart_gpio.gpioMode = GPIO_MODE_INPUT;
    GLB_GPIO_Init(&uart_gpio);

    /* UART signal routing: SIG = GPIO % 12.
     * GLB_UART_Fun_Sel() guarantees uniqueness — any stale duplicates
     * of the requested function in other sigs are cleaned up. */
    GLB_UART_Fun_Sel((GLB_UART_SIG_Type)(CONFIG_ESP32_WIFI_GPIO_TX % 12),
                     uart_txd_sig(CONFIG_ESP32_WIFI_UART_ID));
    GLB_UART_Fun_Sel((GLB_UART_SIG_Type)(CONFIG_ESP32_WIFI_GPIO_RX % 12),
                     uart_rxd_sig(CONFIG_ESP32_WIFI_UART_ID));

    /* Set baud rate before uart_init */
    uart_set_baudrate(CONFIG_ESP32_WIFI_UART_ID, CONFIG_ESP32_WIFI_UART_BAUD);

    /* Init UART peripheral */
    uart_init(CONFIG_ESP32_WIFI_UART_ID, 0);

    uint32_t uart_clk = Clock_Peripheral_Clock_Get(
        DRV_PERIPHERAL_CLOCK_UART0 + CONFIG_ESP32_WIFI_UART_ID);
    IPRO_LOGI(TAG, "UART%d init: TX=GPIO%d RX=GPIO%d baud=%lu (periph_clk=%luHz)",
              CONFIG_ESP32_WIFI_UART_ID,
              CONFIG_ESP32_WIFI_GPIO_TX,
              CONFIG_ESP32_WIFI_GPIO_RX,
              (unsigned long)CONFIG_ESP32_WIFI_UART_BAUD,
              (unsigned long)uart_clk);
}

void esp32_hw_uart_enable_rx(void)
{
    uart_ioctl(CONFIG_ESP32_WIFI_UART_ID, UART_CTRL_SET_INT,
               (void *)(uintptr_t)(UART_RX_END_IT | UART_RX_FIFO_IT | UART_RTO_IT));
}

/* ------------------------------------------------------------------ */
/* ESP32 EN (reset) pin                                               */
/* ------------------------------------------------------------------ */

void esp32_hw_reset(void)
{
#if CONFIG_ESP32_WIFI_GPIO_EN >= 0
    GLB_GPIO_Cfg_Type cfg;

    cfg.gpioPin  = CONFIG_ESP32_WIFI_GPIO_EN;
    cfg.gpioFun  = 11; /* GPIO function */
    cfg.gpioMode = GPIO_MODE_OUTPUT;
    cfg.pullType = GPIO_PULL_NONE;
    cfg.drive    = 1;
    cfg.smtCtrl  = 1;
    GLB_GPIO_Init(&cfg);
    GLB_GPIO_OUTPUT_Enable(CONFIG_ESP32_WIFI_GPIO_EN);

    IPRO_LOGI(TAG, "ESP32 EN pulse (GPIO%d, 100ms)", CONFIG_ESP32_WIFI_GPIO_EN);
    GLB_GPIO_Write(CONFIG_ESP32_WIFI_GPIO_EN, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    GLB_GPIO_Write(CONFIG_ESP32_WIFI_GPIO_EN, 1);

    /* Wait for ESP32 to boot after reset */
    vTaskDelay(pdMS_TO_TICKS(1000));
#else
    IPRO_LOGI(TAG, "ESP32 EN pin not configured - reset skipped");
#endif
}

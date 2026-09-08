/**
 * @file ipro_zephyr.h
 * @brief IPRO SDK - Zephyr RTOS Integration Header
 * 
 * This file provides the main integration interface between IPRO SDK
 * and Zephyr RTOS.
 * 
 * @copyright Copyright (c) 2025 IPRO
 */

#ifndef IPRO_ZEPHYR_H
#define IPRO_ZEPHYR_H

#ifdef CONFIG_ZEPHYR

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/i2c.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize IPRO platform for Zephyr
 * 
 * This function performs platform-specific initialization
 * required for IPRO hardware when running under Zephyr.
 * 
 * @return 0 on success, negative error code on failure
 */
int ipro_zephyr_init(void);

/**
 * @brief IPRO UART configuration structure
 */
struct ipro_uart_config {
    const struct device *uart_dev;
    uint32_t baudrate;
    uint8_t parity;
    uint8_t stop_bits;
    uint8_t data_bits;
};

/**
 * @brief Initialize IPRO UART for Zephyr
 * 
 * @param config UART configuration structure
 * @return 0 on success, negative error code on failure
 */
int ipro_zephyr_uart_init(const struct ipro_uart_config *config);

/**
 * @brief IPRO GPIO configuration structure
 */
struct ipro_gpio_config {
    const struct device *gpio_dev;
    uint32_t pin;
    gpio_flags_t flags;
};

/**
 * @brief Initialize IPRO GPIO for Zephyr
 * 
 * @param config GPIO configuration structure
 * @return 0 on success, negative error code on failure
 */
int ipro_zephyr_gpio_init(const struct ipro_gpio_config *config);

/**
 * @brief Get the main system clock frequency
 * 
 * @return System clock frequency in Hz
 */
uint32_t ipro_zephyr_get_sys_clock_freq(void);

/**
 * @brief Convert IPRO error codes to Zephyr error codes
 * 
 * @param ipro_err IPRO error code
 * @return Corresponding Zephyr error code
 */
int ipro_to_zephyr_error(int ipro_err);

/* Compatibility macros for IPRO SDK when using Zephyr */
#define IPRO_THREAD_PRIORITY_NORMAL    K_PRIO_COOP(7)
#define IPRO_THREAD_PRIORITY_HIGH      K_PRIO_COOP(5)
#define IPRO_THREAD_PRIORITY_LOW       K_PRIO_COOP(9)

#define ipro_thread_create(name, stack_size, priority, entry, arg) \
    k_thread_create(name, stack_size, entry, arg, NULL, NULL, priority, 0, K_NO_WAIT)

#define ipro_mutex_create()            ((void*)k_mutex_init)
#define ipro_mutex_lock(mutex)         k_mutex_lock((struct k_mutex*)mutex, K_FOREVER)
#define ipro_mutex_unlock(mutex)       k_mutex_unlock((struct k_mutex*)mutex)

#define ipro_sem_create(initial_count) ((void*)k_sem_init)
#define ipro_sem_take(sem)             k_sem_take((struct k_sem*)sem, K_FOREVER)
#define ipro_sem_give(sem)             k_sem_give((struct k_sem*)sem)

#define ipro_sleep_ms(ms)              k_sleep(K_MSEC(ms))
#define ipro_sleep_us(us)              k_sleep(K_USEC(us))

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_ZEPHYR */

#endif /* IPRO_ZEPHYR_H */

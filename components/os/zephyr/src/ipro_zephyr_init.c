/**
 * @file ipro_zephyr_init.c
 * @brief IPRO SDK - Zephyr RTOS Initialization
 * 
 * This file implements the initialization routines for integrating
 * IPRO hardware with Zephyr RTOS.
 * 
 * @copyright Copyright (c) 2025 IPRO
 */

#include "ipro_zephyr.h"

#ifdef CONFIG_ZEPHYR

#include <zephyr/logging/log.h>
#include <zephyr/init.h>

LOG_MODULE_REGISTER(ipro_zephyr, CONFIG_LOG_DEFAULT_LEVEL);

/**
 * @brief IPRO platform initialization for Zephyr
 */
int ipro_zephyr_init(void)
{
    LOG_INF("Initializing IPRO platform for Zephyr RTOS");

    /* Initialize system clocks */
    // Add platform-specific clock initialization here
    
    /* Initialize memory management */
    // Add platform-specific memory initialization here
    
    /* Initialize interrupt controller */
    // Add platform-specific interrupt initialization here
    
    /* Initialize power management */
    // Add platform-specific power management initialization here
    
    LOG_INF("IPRO platform initialization completed");
    return 0;
}

/**
 * @brief Get system clock frequency
 */
uint32_t ipro_zephyr_get_sys_clock_freq(void)
{
    /* Return the configured system clock frequency */
    /* This should be configured based on your IPRO7 hardware */
    return CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC;
}

/**
 * @brief Convert IPRO error codes to Zephyr error codes
 */
int ipro_to_zephyr_error(int ipro_err)
{
    switch (ipro_err) {
        case 0:
            return 0;
        case -1:
            return -EINVAL;
        case -2:
            return -ENOMEM;
        case -3:
            return -EBUSY;
        case -4:
            return -ETIMEDOUT;
        default:
            return -EIO;
    }
}

/**
 * @brief Early platform initialization (called by Zephyr)
 */
static int ipro_platform_early_init(const struct device *dev)
{
    ARG_UNUSED(dev);
    
    LOG_DBG("IPRO early platform initialization");
    
    /* Perform early hardware initialization */
    // Add critical early initialization here
    
    return 0;
}

/**
 * @brief Platform initialization (called by Zephyr)
 */
static int ipro_platform_init(const struct device *dev)
{
    ARG_UNUSED(dev);
    
    return ipro_zephyr_init();
}

/* Register initialization functions with Zephyr */
SYS_INIT(ipro_platform_early_init, EARLY, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
SYS_INIT(ipro_platform_init, POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY);

#endif /* CONFIG_ZEPHYR */

/**
 * @file ipro_zephyr_gpio.c
 * @brief IPRO SDK - Zephyr GPIO Driver Integration
 * 
 * This file implements GPIO driver integration between IPRO SDK
 * and Zephyr RTOS.
 * 
 * @copyright Copyright (c) 2025 IPRO
 */

#include "ipro_zephyr.h"

#ifdef CONFIG_ZEPHYR

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(ipro_gpio, CONFIG_LOG_DEFAULT_LEVEL);

/**
 * @brief Initialize IPRO GPIO for Zephyr
 */
int ipro_zephyr_gpio_init(const struct ipro_gpio_config *config)
{
    if (!config || !config->gpio_dev) {
        LOG_ERR("Invalid GPIO configuration");
        return -EINVAL;
    }

    if (!device_is_ready(config->gpio_dev)) {
        LOG_ERR("GPIO device not ready");
        return -ENODEV;
    }

    int ret = gpio_pin_configure(config->gpio_dev, config->pin, config->flags);
    if (ret < 0) {
        LOG_ERR("Failed to configure GPIO pin %d: %d", config->pin, ret);
        return ret;
    }

    LOG_INF("GPIO pin %d initialized successfully", config->pin);
    return 0;
}

/**
 * @brief Set GPIO pin state
 */
int ipro_gpio_pin_set(const struct device *gpio_dev, uint32_t pin, int value)
{
    if (!gpio_dev) {
        return -EINVAL;
    }

    return gpio_pin_set(gpio_dev, pin, value);
}

/**
 * @brief Get GPIO pin state
 */
int ipro_gpio_pin_get(const struct device *gpio_dev, uint32_t pin)
{
    if (!gpio_dev) {
        return -EINVAL;
    }

    return gpio_pin_get(gpio_dev, pin);
}

/**
 * @brief Toggle GPIO pin state
 */
int ipro_gpio_pin_toggle(const struct device *gpio_dev, uint32_t pin)
{
    if (!gpio_dev) {
        return -EINVAL;
    }

    return gpio_pin_toggle(gpio_dev, pin);
}

/**
 * @brief GPIO interrupt callback
 */
static void ipro_gpio_callback_handler(const struct device *dev,
                                       struct gpio_callback *cb,
                                       uint32_t pins)
{
    LOG_DBG("GPIO interrupt on pins: 0x%08x", pins);
    
    /* Handle GPIO interrupt */
    /* You can add custom interrupt handling logic here */
}

/**
 * @brief Configure GPIO interrupt
 */
int ipro_gpio_interrupt_configure(const struct device *gpio_dev, uint32_t pin,
                                  gpio_flags_t flags)
{
    if (!gpio_dev) {
        return -EINVAL;
    }

    static struct gpio_callback gpio_cb;
    
    /* Initialize callback */
    gpio_init_callback(&gpio_cb, ipro_gpio_callback_handler, BIT(pin));
    
    /* Add callback */
    int ret = gpio_add_callback(gpio_dev, &gpio_cb);
    if (ret < 0) {
        LOG_ERR("Failed to add GPIO callback: %d", ret);
        return ret;
    }

    /* Configure interrupt */
    ret = gpio_pin_interrupt_configure(gpio_dev, pin, flags);
    if (ret < 0) {
        LOG_ERR("Failed to configure GPIO interrupt: %d", ret);
        gpio_remove_callback(gpio_dev, &gpio_cb);
        return ret;
    }

    LOG_INF("GPIO interrupt configured for pin %d", pin);
    return 0;
}

/**
 * @brief Disable GPIO interrupt
 */
int ipro_gpio_interrupt_disable(const struct device *gpio_dev, uint32_t pin)
{
    if (!gpio_dev) {
        return -EINVAL;
    }

    return gpio_pin_interrupt_configure(gpio_dev, pin, GPIO_INT_DISABLE);
}

/**
 * @brief Configure multiple GPIO pins
 */
int ipro_gpio_configure_pins(const struct device *gpio_dev,
                             const struct ipro_gpio_config *configs,
                             size_t num_configs)
{
    if (!gpio_dev || !configs || num_configs == 0) {
        return -EINVAL;
    }

    for (size_t i = 0; i < num_configs; i++) {
        int ret = gpio_pin_configure(gpio_dev, configs[i].pin, configs[i].flags);
        if (ret < 0) {
            LOG_ERR("Failed to configure GPIO pin %d: %d", configs[i].pin, ret);
            return ret;
        }
    }

    LOG_INF("Configured %zu GPIO pins successfully", num_configs);
    return 0;
}

#endif /* CONFIG_ZEPHYR */

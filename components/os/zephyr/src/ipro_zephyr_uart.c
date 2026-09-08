/**
 * @file ipro_zephyr_uart.c
 * @brief IPRO SDK - Zephyr UART Driver Integration
 * 
 * This file implements UART driver integration between IPRO SDK
 * and Zephyr RTOS.
 * 
 * @copyright Copyright (c) 2025 IPRO
 */

#include "ipro_zephyr.h"

#ifdef CONFIG_ZEPHYR

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(ipro_uart, CONFIG_LOG_DEFAULT_LEVEL);

/**
 * @brief Initialize IPRO UART for Zephyr
 */
int ipro_zephyr_uart_init(const struct ipro_uart_config *config)
{
    if (!config || !config->uart_dev) {
        LOG_ERR("Invalid UART configuration");
        return -EINVAL;
    }

    if (!device_is_ready(config->uart_dev)) {
        LOG_ERR("UART device not ready");
        return -ENODEV;
    }

    /* Configure UART parameters */
    struct uart_config uart_cfg = {
        .baudrate = config->baudrate,
        .parity = config->parity,
        .stop_bits = config->stop_bits,
        .data_bits = config->data_bits,
        .flow_ctrl = UART_CFG_FLOW_CTRL_NONE,
    };

    int ret = uart_configure(config->uart_dev, &uart_cfg);
    if (ret < 0) {
        LOG_ERR("Failed to configure UART: %d", ret);
        return ret;
    }

    LOG_INF("UART initialized successfully (baudrate: %d)", config->baudrate);
    return 0;
}

/**
 * @brief UART callback for handling received data
 */
void ipro_uart_callback(const struct device *dev, struct uart_event *evt, void *user_data)
{
    ARG_UNUSED(user_data);

    switch (evt->type) {
        case UART_RX_RDY:
            LOG_DBG("UART RX ready: %d bytes", evt->data.rx.len);
            /* Handle received data */
            break;
            
        case UART_RX_BUF_REQUEST:
            LOG_DBG("UART RX buffer request");
            /* Provide new buffer for reception */
            break;
            
        case UART_RX_BUF_RELEASED:
            LOG_DBG("UART RX buffer released");
            /* Release buffer */
            break;
            
        case UART_TX_DONE:
            LOG_DBG("UART TX done");
            /* Transmission completed */
            break;
            
        case UART_TX_ABORTED:
            LOG_WRN("UART TX aborted");
            /* Transmission aborted */
            break;
            
        case UART_RX_STOPPED:
            LOG_WRN("UART RX stopped");
            /* Reception stopped due to error */
            break;
            
        case UART_RX_DISABLED:
            LOG_DBG("UART RX disabled");
            /* Reception disabled */
            break;
            
        default:
            LOG_WRN("Unknown UART event: %d", evt->type);
            break;
    }
}

/**
 * @brief Send data over UART
 */
int ipro_uart_send(const struct device *uart_dev, const uint8_t *data, size_t len)
{
    if (!uart_dev || !data || len == 0) {
        return -EINVAL;
    }

    return uart_tx(uart_dev, data, len, SYS_FOREVER_US);
}

/**
 * @brief Start UART reception
 */
int ipro_uart_receive_start(const struct device *uart_dev, uint8_t *buffer, size_t len)
{
    if (!uart_dev || !buffer || len == 0) {
        return -EINVAL;
    }

    return uart_rx_enable(uart_dev, buffer, len, SYS_FOREVER_US);
}

/**
 * @brief Stop UART reception
 */
int ipro_uart_receive_stop(const struct device *uart_dev)
{
    if (!uart_dev) {
        return -EINVAL;
    }

    return uart_rx_disable(uart_dev);
}

#endif /* CONFIG_ZEPHYR */

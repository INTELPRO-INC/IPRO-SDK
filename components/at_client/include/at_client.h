/*
 * at_client.h - Generic multi-instance AT command client engine
 *
 * Transport-agnostic AT engine with pluggable transport (UART, SPI, etc.)
 * and OS abstraction via ipro_osal. Provides command send/receive, URC
 * dispatch, and data mode support.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __AT_CLIENT_H__
#define __AT_CLIENT_H__

#include <stdint.h>
#include <stdbool.h>
#include "ipro_osal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AT_CLIENT_RESP_MAX_SIZE  512
#define AT_CLIENT_URC_MAX        12
#define AT_CLIENT_NAME_MAX       16

typedef struct at_client at_client_t;

typedef void (*at_urc_cb_t)(const char *line, void *ctx);

/* ------------------------------------------------------------------ */
/* Transport abstraction                                               */
/* ------------------------------------------------------------------ */

/**
 * @brief RX callback type — transport calls this to feed received bytes
 *        into the AT client ring buffer.
 *
 * Called from ISR or RX thread context depending on transport.
 * Implementation provided by at_client internally.
 */
typedef void (*at_client_rx_cb_t)(const uint8_t *data, uint32_t len, void *arg);

/**
 * @brief Transport operations — caller provides these to at_client.
 *
 * The transport layer owns the physical interface (UART, SPI, etc.)
 * and provides read/write/callback-registration through this struct.
 */
typedef struct {
    /**
     * @brief Write data to the transport.
     * @return number of bytes written, or negative on error
     */
    int (*write)(const void *data, uint32_t len, void *ctx);

    /**
     * @brief Register RX callback.
     *
     * Transport must call @p cb from its ISR/RX handler when data arrives.
     * After calling cb, transport must signal @p notify_sem using
     * ipro_osal_sem_signal_isr() + ipro_osal_yield_from_isr().
     *
     * @param cb         Callback to invoke with received bytes
     * @param cb_arg     Opaque argument passed to cb
     * @param notify_sem OSAL semaphore for ISR→task notification
     * @param ctx        Transport context
     */
    void (*set_rx_cb)(at_client_rx_cb_t cb, void *cb_arg,
                      ipro_osal_sem_t *notify_sem, void *ctx);

    /**
     * @brief Unregister RX callback (cleanup on deinit).
     */
    void (*clear_rx_cb)(void *ctx);

    /** Transport-private context (passed to all ops) */
    void *ctx;
} at_client_transport_t;

/* ------------------------------------------------------------------ */
/* Configuration                                                       */
/* ------------------------------------------------------------------ */

typedef struct {
    const char            *name;          /* instance name for logging */
    at_client_transport_t  transport;     /* transport ops */
    uint32_t               rx_buf_size;   /* ring buffer size (bytes) */
    uint32_t               task_stack;    /* RX task stack (bytes) */
    uint8_t                task_prio;     /* RX task priority */
    uint16_t               resp_max_size; /* response buffer size, 0=default 512 */
} at_client_cfg_t;

/* ------------------------------------------------------------------ */
/* API                                                                 */
/* ------------------------------------------------------------------ */

/**
 * @brief Create and initialize an AT client instance.
 *
 * Allocates buffers, creates OS primitives and RX task, then calls
 * transport->set_rx_cb() to register the internal RX handler.
 *
 * The caller MUST have initialized the transport hardware BEFORE
 * calling this function (e.g. GPIO pinmux, UART/SPI peripheral init).
 * The caller should enable transport RX interrupts AFTER this returns.
 *
 * @param[out] out  Receives the allocated at_client_t pointer
 * @param      cfg  Configuration
 * @return 0 on success, negative on error
 */
int  at_client_init(at_client_t **out, const at_client_cfg_t *cfg);

void at_client_deinit(at_client_t *at);

int  at_client_send_cmd(at_client_t *at, const char *cmd,
                        char *resp_buf, uint16_t resp_size,
                        uint32_t timeout_ms);

int  at_client_send_cmd_match(at_client_t *at, const char *cmd,
                              const char *prefix,
                              char *value_buf, uint16_t value_size,
                              uint32_t timeout_ms);

int  at_client_register_urc(at_client_t *at, const char *prefix,
                            at_urc_cb_t cb, void *ctx);
int  at_client_unregister_urc(at_client_t *at, const char *prefix);

void at_client_enter_data_mode(at_client_t *at,
                               void (*raw_cb)(const uint8_t *, uint32_t, void *),
                               void *ctx);
int  at_client_exit_data_mode(at_client_t *at);
bool at_client_is_data_mode(at_client_t *at);

/** Direct transport write (for escape sequences, raw data). */
void at_client_write(at_client_t *at, const void *data, uint32_t len);

/* ------------------------------------------------------------------ */
/* Built-in UART transport helper                                      */
/* ------------------------------------------------------------------ */

/**
 * @brief Fill an at_client_transport_t for UART.
 *
 * Caller must have already done GPIO pinmux and uart_init().
 * After at_client_init(), caller must enable RX interrupts.
 *
 * @param tp       Transport struct to fill
 * @param uart_id  UART peripheral index
 */
void at_client_uart_transport_init(at_client_transport_t *tp, uint8_t uart_id);

#ifdef __cplusplus
}
#endif

#endif /* __AT_CLIENT_H__ */

/*
 * at_client.c - Generic multi-instance AT command client engine
 *
 * Transport-agnostic: all I/O goes through at_client_transport_t ops.
 * OS-agnostic: all threading/sync through ipro_osal.
 * Only exception: transport ISR notification uses raw semaphore handle
 * (platform-specific, handed to transport via set_rx_cb).
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "at_client.h"
#include "ipro_osal.h"
#include "ipro_log.h"

#include <string.h>
#include <stdio.h>

/* ------------------------------------------------------------------ */
/* Internal struct                                                     */
/* ------------------------------------------------------------------ */

struct at_client {
    at_client_cfg_t cfg;
    char            name[AT_CLIENT_NAME_MAX];
    uint16_t        resp_max_size;

    /* Ring buffer */
    volatile uint32_t rx_wr;
    volatile uint32_t rx_rd;
    uint8_t          *rx_buf;

    /* Response */
    char             *resp_buf;
    uint16_t          resp_len;
    int               resp_result;

    /* Line accumulator */
    char              line_buf[256];
    uint16_t          line_pos;

    /* URC table */
    struct {
        const char  *prefix;
        at_urc_cb_t  cb;
        void        *ctx;
    } urc_table[AT_CLIENT_URC_MAX];
    int               urc_count;

    /* Data mode */
    bool              data_mode;
    void            (*raw_cb)(const uint8_t *, uint32_t, void *);
    void             *raw_ctx;

    /* OS primitives */
    ipro_osal_task_t  rx_task;
    ipro_osal_mutex_t mutex;
    ipro_osal_sem_t   resp_sem;
    ipro_osal_sem_t   rx_sem;    /* ISR → RX task notification */

    /* Debug */
    volatile uint32_t dbg_irq_bytes;
    volatile uint32_t dbg_lines;
};

/* ------------------------------------------------------------------ */
/* Ring buffer helpers                                                 */
/* ------------------------------------------------------------------ */

static inline int rxbuf_get(at_client_t *at, uint8_t *c)
{
    if (at->rx_rd == at->rx_wr)
        return -1;
    *c = at->rx_buf[at->rx_rd];
    at->rx_rd = (at->rx_rd + 1) % at->cfg.rx_buf_size;
    return 0;
}

static inline void rxbuf_push(at_client_t *at, const uint8_t *data, uint32_t len)
{
    uint32_t w = at->rx_wr;
    for (uint32_t i = 0; i < len; i++) {
        uint32_t next = (w + 1) % at->cfg.rx_buf_size;
        if (next == at->rx_rd)
            break;  /* overflow — drop */
        at->rx_buf[w] = data[i];
        w = next;
    }
    /*
     * Memory barrier: ensure all buffer writes are visible before
     * updating the write index. Required on weakly-ordered architectures.
     */
#if defined(__riscv) || defined(__RISCV)
    __asm volatile ("fence" ::: "memory");
#else
    __asm volatile ("" ::: "memory");   /* compiler barrier fallback */
#endif
    at->rx_wr = w;
}

/* ------------------------------------------------------------------ */
/* RX callback — transport calls this with received bytes              */
/* ------------------------------------------------------------------ */

static void at_rx_feed(const uint8_t *data, uint32_t len, void *arg)
{
    at_client_t *at = (at_client_t *)arg;
    if (!at || len == 0)
        return;

    at->dbg_irq_bytes += len;
    rxbuf_push(at, data, len);

    /*
     * Transport is responsible for signalling rx_sem from ISR context.
     * We don't signal here — transport's set_rx_cb was given the raw
     * semaphore handle and does the ISR-safe signal + yield itself.
     */
}

/* ------------------------------------------------------------------ */
/* Line processing                                                     */
/* ------------------------------------------------------------------ */

static void process_line(at_client_t *at, const char *line)
{
    if (line[0] == '\0')
        return;

    at->dbg_lines++;

    /* Final result codes */
    if (strcmp(line, "OK") == 0) {
        at->resp_result = 0;
        ipro_osal_sem_signal(&at->resp_sem);
        return;
    }
    if (strcmp(line, "ERROR") == 0 ||
        strncmp(line, "+CME ERROR:", 11) == 0 ||
        strncmp(line, "+CMS ERROR:", 11) == 0) {
        int remain = at->resp_max_size - at->resp_len - 1;
        if (remain > 0) {
            int n = snprintf(at->resp_buf + at->resp_len, remain, "%s\n", line);
            if (n > 0) at->resp_len += (n < remain) ? n : remain;
        }
        at->resp_result = -1;
        ipro_osal_sem_signal(&at->resp_sem);
        return;
    }

    /* CONNECT — used for PPP dial */
    if (strncmp(line, "CONNECT", 7) == 0) {
        at->resp_result = 0;
        ipro_osal_sem_signal(&at->resp_sem);
        return;
    }

    /* NO CARRIER */
    if (strcmp(line, "NO CARRIER") == 0) {
        at->resp_result = -1;
        ipro_osal_sem_signal(&at->resp_sem);
        return;
    }

    /* Check URC table */
    for (int i = 0; i < at->urc_count; i++) {
        if (strncmp(line, at->urc_table[i].prefix,
                    strlen(at->urc_table[i].prefix)) == 0) {
            at->urc_table[i].cb(line, at->urc_table[i].ctx);
            return;
        }
    }

    /* Not a URC — accumulate as AT response line */
    int remain = at->resp_max_size - at->resp_len - 1;
    if (remain > 0) {
        int n = snprintf(at->resp_buf + at->resp_len, remain, "%s\n", line);
        if (n > 0) at->resp_len += (n < remain) ? n : remain;
    }
}

/* ------------------------------------------------------------------ */
/* RX task                                                             */
/* ------------------------------------------------------------------ */

static void at_rx_task(void *arg)
{
    at_client_t *at = (at_client_t *)arg;
    uint8_t c;

    for (;;) {
        ipro_osal_sem_wait(&at->rx_sem, 100);

        while (rxbuf_get(at, &c) == 0) {
            /* Data mode: forward raw bytes */
            if (at->data_mode) {
                uint8_t chunk[128];
                uint32_t chunk_len = 0;
                chunk[chunk_len++] = c;
                while (chunk_len < sizeof(chunk) && rxbuf_get(at, &c) == 0)
                    chunk[chunk_len++] = c;
                if (at->raw_cb)
                    at->raw_cb(chunk, chunk_len, at->raw_ctx);
                continue;
            }

            /* AT mode: accumulate lines */
            if (c == '\n') {
                if (at->line_pos > 0 && at->line_buf[at->line_pos - 1] == '\r')
                    at->line_pos--;
                at->line_buf[at->line_pos] = '\0';
                process_line(at, at->line_buf);
                at->line_pos = 0;
            } else if (c != '\r' || at->line_pos > 0) {
                if (at->line_pos < sizeof(at->line_buf) - 1)
                    at->line_buf[at->line_pos++] = c;
            }
        }
    }
}

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

int at_client_init(at_client_t **out, const at_client_cfg_t *cfg)
{
    if (!out || !cfg || !cfg->transport.write || !cfg->transport.set_rx_cb)
        return -1;
    if (cfg->rx_buf_size == 0)
        return -1;

    /* Allocate instance */
    at_client_t *at = ipro_osal_calloc(1, sizeof(at_client_t));
    if (!at)
        return -1;

    at->cfg = *cfg;
    snprintf(at->name, sizeof(at->name), "%.*s",
             (int)(AT_CLIENT_NAME_MAX - 1), cfg->name ? cfg->name : "AT");
    at->cfg.name = at->name;
    at->resp_max_size = cfg->resp_max_size ? cfg->resp_max_size
                                           : AT_CLIENT_RESP_MAX_SIZE;

    /* Allocate ring buffer */
    at->rx_buf = ipro_osal_malloc(at->cfg.rx_buf_size);
    if (!at->rx_buf)
        goto fail;

    /* Allocate response buffer */
    at->resp_buf = ipro_osal_malloc(at->resp_max_size);
    if (!at->resp_buf)
        goto fail;

    /* Create OS primitives */
    if (ipro_osal_mutex_create(&at->mutex) != IPRO_OSAL_OK)
        goto fail;
    if (ipro_osal_sem_create(&at->resp_sem, 0) != IPRO_OSAL_OK)
        goto fail;
    if (ipro_osal_sem_create(&at->rx_sem, 0) != IPRO_OSAL_OK)
        goto fail;

    /* Create RX task */
    char task_name[AT_CLIENT_NAME_MAX + 4];
    snprintf(task_name, sizeof(task_name), "at_%s", at->name);
    if (ipro_osal_task_create(&at->rx_task, task_name, at_rx_task, at,
                              cfg->task_stack, cfg->task_prio) != IPRO_OSAL_OK) {
        IPRO_LOGE(at->name, "Failed to create RX task");
        goto fail;
    }

    /* Register RX callback with transport */
    at->cfg.transport.set_rx_cb(at_rx_feed, at, &at->rx_sem,
                                at->cfg.transport.ctx);

    *out = at;
    IPRO_LOGI(at->name, "AT client initialized (rxbuf=%lu)",
              (unsigned long)cfg->rx_buf_size);
    return 0;

fail:
    if (ipro_osal_sem_is_valid(&at->rx_sem))
        ipro_osal_sem_delete(&at->rx_sem);
    if (ipro_osal_sem_is_valid(&at->resp_sem))
        ipro_osal_sem_delete(&at->resp_sem);
    if (ipro_osal_mutex_is_valid(&at->mutex))
        ipro_osal_mutex_delete(&at->mutex);
    if (at->resp_buf) ipro_osal_free(at->resp_buf);
    if (at->rx_buf)   ipro_osal_free(at->rx_buf);
    ipro_osal_free(at);
    return -1;
}

void at_client_deinit(at_client_t *at)
{
    if (!at)
        return;

    /* Deregister transport RX callback */
    if (at->cfg.transport.clear_rx_cb)
        at->cfg.transport.clear_rx_cb(at->cfg.transport.ctx);

    ipro_osal_task_delete(&at->rx_task);
    ipro_osal_sem_delete(&at->rx_sem);
    ipro_osal_sem_delete(&at->resp_sem);
    ipro_osal_mutex_delete(&at->mutex);

    ipro_osal_free(at->resp_buf);
    ipro_osal_free(at->rx_buf);
    ipro_osal_free(at);
}

int at_client_send_cmd(at_client_t *at, const char *cmd,
                       char *resp_buf, uint16_t resp_size,
                       uint32_t timeout_ms)
{
    if (!at || at->data_mode)
        return -2;

    if (ipro_osal_mutex_lock(&at->mutex, timeout_ms) != IPRO_OSAL_OK)
        return -2;

    /* Clear response state */
    at->resp_len    = 0;
    at->resp_buf[0] = '\0';
    at->resp_result = -2;

    /* Clear any pending response signal */
    ipro_osal_sem_wait(&at->resp_sem, IPRO_OSAL_NO_WAIT);

    /* Send command via transport */
    at->cfg.transport.write(cmd, strlen(cmd), at->cfg.transport.ctx);
    at->cfg.transport.write("\r\n", 2, at->cfg.transport.ctx);

    /* Wait for response */
    if (ipro_osal_sem_wait(&at->resp_sem, timeout_ms) != IPRO_OSAL_OK) {
        IPRO_LOGW(at->cfg.name, "AT timeout: %s", cmd);
        at->resp_result = -2;
    }

    /* Copy response to caller */
    if (resp_buf && resp_size > 0) {
        uint16_t copy_len = at->resp_len;
        if (copy_len >= resp_size)
            copy_len = resp_size - 1;
        memcpy(resp_buf, at->resp_buf, copy_len);
        resp_buf[copy_len] = '\0';
    }

    int result = at->resp_result;
    ipro_osal_mutex_unlock(&at->mutex);
    return result;
}

int at_client_send_cmd_match(at_client_t *at, const char *cmd,
                             const char *prefix,
                             char *value_buf, uint16_t value_size,
                             uint32_t timeout_ms)
{
    if (!at || at->data_mode)
        return -2;

    if (ipro_osal_mutex_lock(&at->mutex, timeout_ms) != IPRO_OSAL_OK)
        return -2;

    /* Clear response state */
    at->resp_len    = 0;
    at->resp_buf[0] = '\0';
    at->resp_result = -2;
    ipro_osal_sem_wait(&at->resp_sem, IPRO_OSAL_NO_WAIT);

    /* Send command via transport */
    at->cfg.transport.write(cmd, strlen(cmd), at->cfg.transport.ctx);
    at->cfg.transport.write("\r\n", 2, at->cfg.transport.ctx);

    /* Wait for response */
    if (ipro_osal_sem_wait(&at->resp_sem, timeout_ms) != IPRO_OSAL_OK) {
        IPRO_LOGW(at->cfg.name, "AT timeout: %s", cmd);
        at->resp_result = -2;
    }

    int result = at->resp_result;

    /* Search for prefix in response buffer (still under mutex) */
    if (result == 0 && value_buf && value_size > 0) {
        char *p = strstr(at->resp_buf, prefix);
        if (!p) {
            value_buf[0] = '\0';
            result = -1;
        } else {
            p += strlen(prefix);
            uint16_t i = 0;
            while (*p && *p != '\n' && *p != '\r' && i < value_size - 1)
                value_buf[i++] = *p++;
            value_buf[i] = '\0';
        }
    } else if (value_buf && value_size > 0) {
        value_buf[0] = '\0';
    }

    ipro_osal_mutex_unlock(&at->mutex);
    return result;
}

int at_client_register_urc(at_client_t *at, const char *prefix,
                           at_urc_cb_t cb, void *ctx)
{
    if (!at || at->urc_count >= AT_CLIENT_URC_MAX)
        return -1;
    at->urc_table[at->urc_count].prefix = prefix;
    at->urc_table[at->urc_count].cb     = cb;
    at->urc_table[at->urc_count].ctx    = ctx;
    at->urc_count++;
    return 0;
}

int at_client_unregister_urc(at_client_t *at, const char *prefix)
{
    if (!at)
        return -1;
    for (int i = 0; i < at->urc_count; i++) {
        if (strcmp(at->urc_table[i].prefix, prefix) == 0) {
            for (int j = i; j < at->urc_count - 1; j++)
                at->urc_table[j] = at->urc_table[j + 1];
            at->urc_count--;
            return 0;
        }
    }
    return -1;
}

void at_client_enter_data_mode(at_client_t *at,
                               void (*raw_cb)(const uint8_t *, uint32_t, void *),
                               void *ctx)
{
    if (!at)
        return;
    uint32_t state = ipro_osal_critical_enter();
    at->raw_cb    = raw_cb;
    at->raw_ctx   = ctx;
    at->data_mode = true;
    ipro_osal_critical_exit(state);
    IPRO_LOGI(at->cfg.name, "Entered data mode");
}

int at_client_exit_data_mode(at_client_t *at)
{
    if (!at || !at->data_mode)
        return 0;

    uint32_t state = ipro_osal_critical_enter();
    at->data_mode = false;
    at->raw_cb    = NULL;
    ipro_osal_critical_exit(state);

    /* Guard time: 1s silence before +++ */
    ipro_osal_delay_ms(1100);

    /* Send escape sequence via transport */
    at->cfg.transport.write("+++", 3, at->cfg.transport.ctx);

    /* Guard time: 1s silence after +++ */
    ipro_osal_delay_ms(1100);

    /*
     * Manipulate response state without mutex: safe because data mode
     * flag prevents at_client_send_cmd from running (it returns -2).
     */
    at->resp_len    = 0;
    at->resp_result = -2;
    ipro_osal_sem_wait(&at->resp_sem, IPRO_OSAL_NO_WAIT);

    if (ipro_osal_sem_wait(&at->resp_sem, 3000) == IPRO_OSAL_OK &&
        at->resp_result == 0) {
        IPRO_LOGI(at->cfg.name, "Exited data mode");
        return 0;
    }

    IPRO_LOGW(at->cfg.name, "Exit data mode: no OK");
    return -1;
}

bool at_client_is_data_mode(at_client_t *at)
{
    return at ? at->data_mode : false;
}

void at_client_write(at_client_t *at, const void *data, uint32_t len)
{
    if (!at || !data || len == 0)
        return;
    at->cfg.transport.write(data, len, at->cfg.transport.ctx);
}

/* ------------------------------------------------------------------ */
/* Built-in UART transport                                             */
/* ------------------------------------------------------------------ */

#include "hal_uart.h"

typedef struct {
    uint8_t             uart_id;
    at_client_rx_cb_t   rx_cb;
    void               *rx_cb_arg;
    ipro_osal_sem_t    *notify_sem;
} at_uart_tp_t;

#define AT_UART_TP_MAX 4
static at_uart_tp_t s_uart_tp[AT_UART_TP_MAX];

static void at_uart_isr(void *dev, void *arg, uint32_t size, uint32_t event)
{
    (void)arg;
    (void)size;

    if (event != UART_EVENT_RX_FIFO && event != UART_EVENT_RTO &&
        event != UART_EVENT_RX_END)
        return;

    uart_device_t *uart_dev = (uart_device_t *)dev;
    at_uart_tp_t *tp = NULL;
    for (int i = 0; i < AT_UART_TP_MAX; i++) {
        if (s_uart_tp[i].rx_cb && s_uart_tp[i].uart_id == uart_dev->id) {
            tp = &s_uart_tp[i];
            break;
        }
    }
    if (!tp)
        return;

    uint8_t tmp[64];
    int cnt = uart_read(tp->uart_id, tmp, sizeof(tmp));
    if (cnt <= 0)
        return;

    /* Push bytes into at_client ring buffer */
    tp->rx_cb(tmp, (uint32_t)cnt, tp->rx_cb_arg);

    /* ISR-safe notification via OSAL */
    if (tp->notify_sem) {
        bool need_yield = false;
        ipro_osal_sem_signal_isr(tp->notify_sem, &need_yield);
        ipro_osal_yield_from_isr(need_yield);
    }
}

static int at_uart_write(const void *data, uint32_t len, void *ctx)
{
    at_uart_tp_t *tp = (at_uart_tp_t *)ctx;
    uart_write(tp->uart_id, data, len);
    return (int)len;
}

static void at_uart_set_rx_cb(at_client_rx_cb_t cb, void *cb_arg,
                               ipro_osal_sem_t *notify_sem, void *ctx)
{
    at_uart_tp_t *tp = (at_uart_tp_t *)ctx;
    tp->rx_cb      = cb;
    tp->rx_cb_arg  = cb_arg;
    tp->notify_sem = notify_sem;
    uart_set_callback(tp->uart_id, (void *)at_uart_isr);
}

static void at_uart_clear_rx_cb(void *ctx)
{
    at_uart_tp_t *tp = (at_uart_tp_t *)ctx;
    uart_set_callback(tp->uart_id, NULL);
    tp->rx_cb      = NULL;
    tp->rx_cb_arg  = NULL;
    tp->notify_sem = NULL;
}

void at_client_uart_transport_init(at_client_transport_t *tp, uint8_t uart_id)
{
    /* Find or allocate a slot */
    at_uart_tp_t *slot = NULL;
    for (int i = 0; i < AT_UART_TP_MAX; i++) {
        if (!s_uart_tp[i].rx_cb && s_uart_tp[i].uart_id == 0) {
            slot = &s_uart_tp[i];
            break;
        }
    }
    if (!slot) {
        /* Reuse first empty slot */
        for (int i = 0; i < AT_UART_TP_MAX; i++) {
            if (!s_uart_tp[i].rx_cb) {
                slot = &s_uart_tp[i];
                break;
            }
        }
    }
    if (!slot)
        return;

    memset(slot, 0, sizeof(*slot));
    slot->uart_id = uart_id;

    tp->write       = at_uart_write;
    tp->set_rx_cb   = at_uart_set_rx_cb;
    tp->clear_rx_cb = at_uart_clear_rx_cb;
    tp->ctx         = slot;
}

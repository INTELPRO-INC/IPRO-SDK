/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

/**
 * @file bt_shell.c
 * @brief NUS-style GATT service that runs shell commands. See bt_shell.h.
 */
#include "bt_shell.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#include "shell.h"
#include "compiler/common.h"
#include "ipro_log.h"

#define LOG_TAG "BTSH"

#define BTSH_CMD_MAX   128
#define BTSH_OUT_MAX   4096
#define BTSH_STACK     1536
#define BTSH_PRIO      (configMAX_PRIORITIES - 5)

/* Nordic UART Service, so stock phone apps work without anything custom. */
#define BT_UUID_NUS_VAL \
    BT_UUID_128_ENCODE(0x6e400001, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)
#define BT_UUID_NUS_RX_VAL \
    BT_UUID_128_ENCODE(0x6e400002, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)
#define BT_UUID_NUS_TX_VAL \
    BT_UUID_128_ENCODE(0x6e400003, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)

static struct bt_uuid_128 uuid_nus    = BT_UUID_INIT_128(BT_UUID_NUS_VAL);
static struct bt_uuid_128 uuid_nus_rx = BT_UUID_INIT_128(BT_UUID_NUS_RX_VAL);
static struct bt_uuid_128 uuid_nus_tx = BT_UUID_INIT_128(BT_UUID_NUS_TX_VAL);

/* OCRAM is over 93% full, so both buffers live in PSRAM. */
static char ATTR_PSRAM_SECTION s_cmd[BTSH_CMD_MAX];
static char ATTR_PSRAM_SECTION s_out[BTSH_OUT_MAX];

static volatile uint16_t s_cmd_len;
static volatile uint32_t s_out_len;
static volatile bool     s_notify_on;
static volatile uint32_t s_ran;

static SemaphoreHandle_t s_work;         /* RX thread -> worker */
static struct bt_conn   *s_conn;

extern const struct bt_gatt_service_static btsh_svc;

/* shell.c keeps a file-private pointer, but the object it points at is a
 * global. That is the only way to read the current print hook back, which is
 * needed because shell_set_print() cannot restore one. */
extern struct shell _shell;

/* ------------------------------------------------------------------------- */

/*
 * Collect shell output instead of printing it.
 *
 * shell_set_print() swaps the shell's output function globally, so this is
 * installed only around the command and put back immediately after - otherwise
 * the UART console would go silent for anyone watching it.
 */
static void btsh_collect(char *fmt, ...)
{
    va_list ap;
    int n;

    if (s_out_len >= BTSH_OUT_MAX - 1u) {
        return;
    }
    va_start(ap, fmt);
    n = vsnprintf(&s_out[s_out_len], BTSH_OUT_MAX - s_out_len, fmt, ap);
    va_end(ap);

    if (n > 0) {
        s_out_len += (uint32_t)n;
        if (s_out_len > BTSH_OUT_MAX - 1u) {
            s_out_len = BTSH_OUT_MAX - 1u;
        }
    }
}

static void btsh_send(const char *p, uint32_t len)
{
    uint16_t mtu;

    if (!s_conn || !s_notify_on) {
        return;
    }
    /* MTU is negotiated, so ask rather than assume the 23-byte minimum. */
    mtu = bt_gatt_get_mtu(s_conn);
    if (mtu > 3u) {
        mtu -= 3u;
    } else {
        mtu = 20u;
    }

    while (len && s_conn && s_notify_on) {
        uint16_t n = (len > mtu) ? mtu : (uint16_t)len;

        /* attrs[1] is the TX characteristic declaration; bt_gatt_notify()
         * resolves the value handle from it. */
        if (bt_gatt_notify(s_conn, &btsh_svc.attrs[1], p, n) != 0) {
            vTaskDelay(pdMS_TO_TICKS(10));   /* buffers busy; back off */
            continue;
        }
        p   += n;
        len -= n;
    }
}

static void btsh_task(void *arg)
{
    (void)arg;

    for (;;) {
        if (xSemaphoreTake(s_work, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        if (!s_cmd_len) {
            continue;
        }

        /*
         * Save and restore the print hook by hand. shell_set_print(NULL)
         * returns -1 and does nothing, so using it to restore would leave the
         * UART console permanently redirected into this buffer.
         */
        {
            void (*prev)(char *fmt, ...) = _shell.shell_printf;

            s_out_len = 0;
            shell_set_print(btsh_collect);
            shell_exe_cmd((uint8_t *)s_cmd, s_cmd_len);
            _shell.shell_printf = prev;
        }

        s_ran++;
        s_cmd_len = 0;

        if (s_out_len == 0u) {
            btsh_send("(no output)\n", 12);
        } else {
            btsh_send(s_out, s_out_len);
        }
    }
}

/* ------------------------------------------------------------------------- */

static ssize_t btsh_on_write(struct bt_conn *conn,
                             const struct bt_gatt_attr *attr,
                             const void *buf, uint16_t len,
                             uint16_t offset, uint8_t flags)
{
    const char *p = buf;
    uint16_t n;

    (void)attr; (void)offset; (void)flags;

    if (s_cmd_len) {
        return BT_GATT_ERR(BT_ATT_ERR_WRITE_REQ_REJECTED);  /* one at a time */
    }

    n = (len >= BTSH_CMD_MAX) ? (BTSH_CMD_MAX - 1u) : len;
    memcpy(s_cmd, p, n);

    /*
     * Normalise to exactly one terminator - the shell needs one, and clients
     * disagree about how many to send.
     *
     * shell_exe_cmd() does not execute anything: it writes the bytes into the
     * same input ring the UART feeds, and shell.c only runs a line when it sees
     * '\r' or '\n' (shell.c:772). Stripping the terminator, as this did at
     * first, left the characters sitting in the buffer forever - the command
     * was received and never ran, with nothing to show that it had not.
     */
    while (n && (s_cmd[n - 1u] == '\r' || s_cmd[n - 1u] == '\n')) {
        n--;
    }
    if (!n) {
        return len;
    }
    s_cmd[n++] = '\r';
    s_cmd[n] = '\0';

    s_conn    = conn;
    s_cmd_len = n;
    /*
     * Hand off and return. Commands block for seconds - `spk tone`, `vid play`
     * - and this is the host's RX thread, which must keep processing HCI.
     */
    xSemaphoreGive(s_work);
    return len;
}

static void btsh_on_ccc(const struct bt_gatt_attr *attr, uint16_t value)
{
    (void)attr;
    s_notify_on = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(LOG_TAG, "client %s output", s_notify_on ? "subscribed to"
                                                       : "unsubscribed from");
}

BT_GATT_SERVICE_DEFINE(btsh_svc,
    BT_GATT_PRIMARY_SERVICE(&uuid_nus),
    BT_GATT_CHARACTERISTIC(&uuid_nus_tx.uuid, BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_NONE, NULL, NULL, NULL),
    BT_GATT_CCC(btsh_on_ccc, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(&uuid_nus_rx.uuid,
                           BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE, NULL, btsh_on_write, NULL),
);

int bt_shell_init(void)
{
    if (s_work) {
        return 0;
    }
    s_work = xSemaphoreCreateBinary();
    if (!s_work) {
        return -1;
    }
    if (xTaskCreate(btsh_task, "btshell", BTSH_STACK, NULL, BTSH_PRIO,
                    NULL) != pdPASS) {
        return -2;
    }
    IPRO_LOGI(LOG_TAG, "shell over BLE ready (Nordic UART Service) - write a "
                       "command line to 6E400002, subscribe to 6E400003");
    return 0;
}

bool     bt_shell_subscribed(void)  { return s_notify_on; }
uint32_t bt_shell_commands_run(void) { return s_ran; }

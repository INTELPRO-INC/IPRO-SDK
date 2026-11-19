/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 *
 * IPRO6S Classic Bluetooth example: Serial Port Profile (SPP) echo server.
 *
 * A phone or computer pairs with the board (Secure Simple Pairing, no PIN),
 * opens its serial port, and every byte it sends is printed on the console
 * and sent straight back. The console command `spp send <text>` sends text
 * the other way.
 *
 * SPP is RFCOMM channel BT_RFCOMM_CHAN_SPP announced by an SDP record with
 * the Serial Port service class (UUID 0x1101), which is what an SPP
 * terminal app on the phone looks for.
 */
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "freertos_app_hooks.h"
#include "ipro_log.h"
#include "shell.h"
#include "bt_classic_common.h"

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/classic/rfcomm.h>
#include <zephyr/bluetooth/classic/sdp.h>
#include <zephyr/net_buf.h>

#define TAG "SPP"

#ifndef CONFIG_APP_SPP_MTU
#define CONFIG_APP_SPP_MTU 256
#endif
#define SPP_MTU          CONFIG_APP_SPP_MTU
#define SPP_TX_BUFFERS   4
#define SPP_PRINT_MAX    64u

NET_BUF_POOL_FIXED_DEFINE(s_spp_tx_pool, SPP_TX_BUFFERS, BT_RFCOMM_BUF_SIZE(SPP_MTU),
                          CONFIG_BT_CONN_TX_USER_DATA_SIZE, NULL);

/* SDP record: Serial Port service on RFCOMM channel BT_RFCOMM_CHAN_SPP. */
static struct bt_sdp_attribute s_spp_attrs[] = {
    BT_SDP_NEW_SERVICE,
    BT_SDP_LIST(
        BT_SDP_ATTR_SVCLASS_ID_LIST,
        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 3),
        BT_SDP_DATA_ELEM_LIST(
            {
                BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                BT_SDP_ARRAY_16(BT_SDP_SERIAL_PORT_SVCLASS)
            },
        )
    ),
    BT_SDP_LIST(
        BT_SDP_ATTR_PROTO_DESC_LIST,
        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 12),
        BT_SDP_DATA_ELEM_LIST(
            {
                BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 3),
                BT_SDP_DATA_ELEM_LIST(
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                        BT_SDP_ARRAY_16(BT_SDP_PROTO_L2CAP)
                    },
                )
            },
            {
                BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 5),
                BT_SDP_DATA_ELEM_LIST(
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                        BT_SDP_ARRAY_16(BT_SDP_PROTO_RFCOMM)
                    },
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UINT8),
                        BT_SDP_ARRAY_8(BT_RFCOMM_CHAN_SPP)
                    },
                )
            },
        )
    ),
    BT_SDP_LIST(
        BT_SDP_ATTR_PROFILE_DESC_LIST,
        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 8),
        BT_SDP_DATA_ELEM_LIST(
            {
                BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
                BT_SDP_DATA_ELEM_LIST(
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                        BT_SDP_ARRAY_16(BT_SDP_SERIAL_PORT_SVCLASS)
                    },
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UINT16),
                        BT_SDP_ARRAY_16(0x0102) /* SPP 1.2 */
                    },
                )
            },
        )
    ),
    BT_SDP_SERVICE_NAME("IPRO6S Serial Port"),
};

static struct bt_sdp_record s_spp_record = BT_SDP_RECORD(s_spp_attrs);

static volatile bool s_spp_open;
static uint32_t s_rx_bytes;
static uint32_t s_tx_bytes;

static void spp_print_rx(const uint8_t *data, uint16_t len)
{
    char text[SPP_PRINT_MAX + 1];
    uint16_t n = len < SPP_PRINT_MAX ? len : SPP_PRINT_MAX;

    for (uint16_t i = 0; i < n; i++) {
        text[i] = isprint(data[i]) ? (char)data[i] : '.';
    }
    text[n] = '\0';
    IPRO_LOGI(TAG, "rx %u bytes: %s%s", len, text, len > n ? "..." : "");
}

static int spp_send(struct bt_rfcomm_dlc *dlc, const uint8_t *data, uint16_t len)
{
    struct net_buf *buf;
    int err;

    if (len > SPP_MTU) {
        return -EMSGSIZE;
    }
    buf = bt_rfcomm_create_pdu(&s_spp_tx_pool);
    if (buf == NULL) {
        return -ENOMEM;
    }
    net_buf_add_mem(buf, data, len);
    err = bt_rfcomm_dlc_send(dlc, buf);
    if (err < 0) {
        net_buf_unref(buf);
        return err;
    }
    s_tx_bytes += len;
    return 0;
}

static void spp_connected(struct bt_rfcomm_dlc *dlc)
{
    (void)dlc;
    s_spp_open = true;
    IPRO_LOGI(TAG, "serial port open");
}

static void spp_disconnected(struct bt_rfcomm_dlc *dlc)
{
    (void)dlc;
    s_spp_open = false;
    IPRO_LOGI(TAG, "serial port closed (rx %lu bytes, tx %lu bytes)",
              (unsigned long)s_rx_bytes, (unsigned long)s_tx_bytes);
}

static void spp_recv(struct bt_rfcomm_dlc *dlc, struct net_buf *buf)
{
    int err;

    s_rx_bytes += buf->len;
    spp_print_rx(buf->data, buf->len);

    /* Echo: send the same bytes back on the same channel. */
    err = spp_send(dlc, buf->data, buf->len);
    if (err != 0) {
        IPRO_LOGW(TAG, "echo failed: %d", err);
    }
}

static struct bt_rfcomm_dlc_ops s_spp_ops = {
    .recv = spp_recv,
    .connected = spp_connected,
    .disconnected = spp_disconnected,
};

static struct bt_rfcomm_dlc s_spp_dlc = {
    .ops = &s_spp_ops,
    .mtu = SPP_MTU,
};

static int spp_accept(struct bt_conn *conn, struct bt_rfcomm_server *server,
                      struct bt_rfcomm_dlc **dlc)
{
    (void)conn;
    (void)server;
    if (s_spp_dlc.session != NULL) {
        return -ENOMEM; /* one serial port at a time */
    }
    *dlc = &s_spp_dlc;
    return 0;
}

static struct bt_rfcomm_server s_spp_server = {
    .channel = BT_RFCOMM_CHAN_SPP,
    .accept = spp_accept,
};

static void spp_register(void)
{
    int err = bt_rfcomm_server_register(&s_spp_server);

    if (err == 0) {
        err = bt_sdp_register_service(&s_spp_record);
    }
    if (err != 0) {
        IPRO_LOGE(TAG, "register failed: %d", err);
        return;
    }
    IPRO_LOGI(TAG, "SPP server on RFCOMM channel %u", s_spp_server.channel);
}

#if defined(CONFIG_SHELL)
static void cmd_spp(int argc, char **argv)
{
    char line[SPP_MTU + 1];
    size_t used = 0;
    int err;

    if (argc >= 2 && strcmp(argv[1], "status") == 0) {
        IPRO_LOGI(TAG, "link %s, serial port %s, rx %lu bytes, tx %lu bytes",
                  bt_classic_connected() ? "up" : "down",
                  s_spp_open ? "open" : "closed",
                  (unsigned long)s_rx_bytes, (unsigned long)s_tx_bytes);
        return;
    }
    if (argc < 3 || strcmp(argv[1], "send") != 0) {
        IPRO_LOGI(TAG, "usage: spp status | spp send <text>");
        return;
    }
    if (!s_spp_open) {
        IPRO_LOGW(TAG, "serial port is not open");
        return;
    }
    /* Join the words back into one line and end it with CR LF. */
    for (int i = 2; i < argc; i++) {
        int n = snprintf(&line[used], sizeof(line) - used, "%s%s",
                         i > 2 ? " " : "", argv[i]);
        if (n < 0 || (size_t)n >= sizeof(line) - used) {
            IPRO_LOGW(TAG, "text longer than %d bytes", SPP_MTU);
            return;
        }
        used += (size_t)n;
    }
    if (used + 2u > SPP_MTU) {
        IPRO_LOGW(TAG, "text longer than %d bytes", SPP_MTU);
        return;
    }
    line[used++] = '\r';
    line[used++] = '\n';
    err = spp_send(&s_spp_dlc, (const uint8_t *)line, (uint16_t)used);
    IPRO_LOGI(TAG, "tx %u bytes: %d", (unsigned int)used, err);
}
SHELL_CMD_EXPORT_ALIAS(cmd_spp, spp, SPP status / send text to the phone);
#endif

static void app_task(void *arg)
{
    (void)arg;
    if (bt_classic_host_start(spp_register) != 0) {
        IPRO_LOGE(TAG, "Bluetooth did not start");
    }
    vTaskDelete(NULL);
}

int main(void)
{
    ipro_platform_init();
    IPRO_LOGI(TAG, "IPRO6S Classic Bluetooth SPP example");

    if (bt_classic_controller_init() != 0) {
        IPRO_LOGE(TAG, "stopped: no Bluetooth controller");
    } else if (xTaskCreate(app_task, "app", 2048, NULL, 5, NULL) != pdPASS) {
        IPRO_LOGE(TAG, "stopped: cannot create the app task");
    }

    vTaskStartScheduler();
    while (1) {
    }
}

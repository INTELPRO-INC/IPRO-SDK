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

/* Opt-in board test fixture, never linked into customer Host libraries. */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/l2cap.h>
#include <zephyr/net_buf.h>
#include "ipro_log.h"
#include "shell.h"
#include "coc_pattern.h"
#define TAG "COC_TEST"
#define TEST_PSM 0x0080
NET_BUF_POOL_FIXED_DEFINE(coc_tx_pool, 4, BT_L2CAP_SDU_BUF_SIZE(COC_TEST_MTU),
                          CONFIG_BT_CONN_TX_USER_DATA_SIZE, NULL);
NET_BUF_POOL_FIXED_DEFINE(coc_rx_pool, 2, COC_TEST_MTU, sizeof(uint16_t), NULL);
static struct bt_l2cap_le_chan channel;
static atomic_t in_use, connected, hold_next, rx_count, rx_bad, sent_count;
static atomic_ptr_t held;
static uint16_t expected_seq, tx_seq;
static bool registered;

static struct net_buf *alloc_rx(struct bt_l2cap_chan *chan)
{
    (void)chan;
    return net_buf_alloc(&coc_rx_pool, K_NO_WAIT);
}
static void on_connected(struct bt_l2cap_chan *chan)
{
    (void)chan;
    atomic_set(&connected, 1);
    IPRO_LOGI(TAG, "CONNECTED tx_mtu=%u tx_mps=%u rx_mtu=%u rx_mps=%u",
              channel.tx.mtu, channel.tx.mps, channel.rx.mtu, channel.rx.mps);
}
static void on_disconnected(struct bt_l2cap_chan *chan)
{
    (void)chan;
    atomic_clear(&connected);
    struct net_buf *buf = atomic_ptr_clear(&held);
    if (buf != NULL) net_buf_unref(buf);
    atomic_clear(&hold_next);
    IPRO_LOGI(TAG, "DISCONNECTED");
}
static void on_released(struct bt_l2cap_chan *chan)
{
    (void)chan;
    atomic_clear(&in_use);
}
static int on_recv(struct bt_l2cap_chan *chan, struct net_buf *buf)
{
    (void)chan;
    bool ok = coc_pattern_check(buf->data, buf->len, expected_seq);
    atomic_inc(&rx_count);
    if (!ok) atomic_inc(&rx_bad);
    IPRO_LOGI(TAG, "RX %s seq=%u len=%u total=%ld bad=%ld",
              ok ? "PASS" : "FAIL", expected_seq, buf->len,
              (long)atomic_get(&rx_count), (long)atomic_get(&rx_bad));
    expected_seq++;
    if (atomic_cas(&hold_next, 1, 0)) {
        atomic_ptr_set(&held, buf);
        IPRO_LOGI(TAG, "HELD credit withheld");
        return -EINPROGRESS;
    }
    return 0;
}
static void on_sent(struct bt_l2cap_chan *chan)
{
    (void)chan;
    atomic_inc(&sent_count);
}
static const struct bt_l2cap_chan_ops ops = {
    .connected = on_connected, .disconnected = on_disconnected,
    .released = on_released, .alloc_buf = alloc_rx, .recv = on_recv, .sent = on_sent,
};
static bool prepare(void)
{
    if (!atomic_cas(&in_use, 0, 1)) return false;
    memset(&channel, 0, sizeof(channel));
    channel.chan.ops = &ops;
    channel.rx.mtu = COC_TEST_MTU;
    expected_seq = tx_seq = 0;
    atomic_clear(&rx_count);
    atomic_clear(&rx_bad);
    atomic_clear(&sent_count);
    return true;
}
static int accept_chan(struct bt_conn *conn, struct bt_l2cap_server *server,
                       struct bt_l2cap_chan **chan)
{
    (void)conn; (void)server;
    if (!prepare()) return -ENOMEM;
    *chan = &channel.chan;
    return 0;
}
static struct bt_l2cap_server server = {
    .psm = TEST_PSM, .sec_level = BT_SECURITY_L1, .accept = accept_chan,
};
static void pick_conn(struct bt_conn *conn, void *user)
{
    struct bt_conn **out = user;
    struct bt_conn_info info;
    if (*out == NULL && bt_conn_get_info(conn, &info) == 0 &&
        info.state == BT_CONN_STATE_CONNECTED) *out = bt_conn_ref(conn);
}
static int command(int argc, char **argv)
{
    int err = 0;
    if (argc < 2) return -EINVAL;
    if (argc == 2 && strcmp(argv[1], "listen") == 0) {
        if (registered) return -EALREADY;
        err = bt_l2cap_server_register(&server);
        if (!err) registered = true;
    } else if (argc == 2 && strcmp(argv[1], "connect") == 0) {
        struct bt_conn *conn = NULL;
        bt_conn_foreach(BT_CONN_TYPE_LE, pick_conn, &conn);
        if (conn == NULL) return -ENOTCONN;
        if (!prepare()) { bt_conn_unref(conn); return -EBUSY; }
        err = bt_l2cap_chan_connect(conn, &channel.chan, TEST_PSM);
        if (err) atomic_clear(&in_use);
        bt_conn_unref(conn);
    } else if (argc == 3 && strcmp(argv[1], "send") == 0) {
        char *end;
        errno = 0;
        unsigned long len = strtoul(argv[2], &end, 10);
        if (errno || *end || end == argv[2] || len < 4 || len > COC_TEST_MTU)
            return -EINVAL;
        if (!atomic_get(&connected)) return -ENOTCONN;
        struct net_buf *buf = net_buf_alloc(&coc_tx_pool, K_NO_WAIT);
        if (buf == NULL) return -ENOMEM;
        net_buf_reserve(buf, BT_L2CAP_SDU_CHAN_SEND_RESERVE);
        coc_pattern_fill(net_buf_add(buf, len), len, tx_seq);
        err = bt_l2cap_chan_send(&channel.chan, buf);
        IPRO_LOGI(TAG, "TX seq=%u len=%lu rc=%d", tx_seq, len, err);
        if (err < 0) net_buf_unref(buf); else tx_seq++;
    } else if (argc == 2 && strcmp(argv[1], "hold") == 0) {
        if (!atomic_get(&connected) || atomic_ptr_get(&held)) return -EBUSY;
        atomic_set(&hold_next, 1);
    } else if (argc == 2 && strcmp(argv[1], "release") == 0) {
        struct net_buf *buf = atomic_ptr_clear(&held);
        if (buf == NULL) return -ENOENT;
        err = bt_l2cap_chan_recv_complete(&channel.chan, buf);
    } else if (argc == 2 && strcmp(argv[1], "disconnect") == 0) {
        err = bt_l2cap_chan_disconnect(&channel.chan);
    } else if (argc != 2 || strcmp(argv[1], "status") != 0) {
        return -EINVAL;
    }
    IPRO_LOGI(TAG, "%s rc=%d connected=%ld rx=%ld bad=%ld sent=%ld credits=%ld held=%u",
              argv[1], err, (long)atomic_get(&connected), (long)atomic_get(&rx_count),
              (long)atomic_get(&rx_bad), (long)atomic_get(&sent_count),
              (long)atomic_get(&channel.tx.credits), atomic_ptr_get(&held) != NULL);
    return err;
}
SHELL_CMD_EXPORT_ALIAS(command, coc, LE credit based channel test);

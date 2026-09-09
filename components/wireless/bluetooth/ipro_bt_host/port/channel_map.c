/* SPDX-License-Identifier: Apache-2.0 */
#include <errno.h>
#include <string.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/hci_types.h>
#include <zephyr/net_buf.h>
#include <zephyr/sys/byteorder.h>
#include "ipro_bt_host/channel_map.h"

int ipro_bt_conn_le_read_channel_map(struct bt_conn *conn, uint8_t map[5])
{
    struct bt_hci_cp_le_read_chan_map *cp;
    struct bt_hci_rp_le_read_chan_map *rp;
    struct net_buf *buf;
    struct net_buf *rsp = NULL;
    struct bt_conn_info info;
    uint16_t handle;
    int err;

    if ((conn == NULL) || (map == NULL)) {
        return -EINVAL;
    }
    err = bt_conn_get_info(conn, &info);
    if (err != 0) {
        return err;
    }
    if (info.type != BT_CONN_TYPE_LE) {
        return -ENOTSUP;
    }

    err = bt_hci_get_conn_handle(conn, &handle);
    if (err != 0) {
        return err;
    }

    buf = bt_hci_cmd_alloc(K_FOREVER);
    if (buf == NULL) {
        return -ENOBUFS;
    }
    cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(handle);
    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_READ_CHAN_MAP, buf, &rsp);
    if (err != 0) {
        return err;
    }
    if ((rsp == NULL) || (rsp->len < sizeof(*rp))) {
        if (rsp != NULL) {
            net_buf_unref(rsp);
        }
        return -EMSGSIZE;
    }

    rp = (void *)rsp->data;
    if (rp->status != 0U) {
        err = rp->status;
    } else {
        memcpy(map, rp->ch_map, 5U);
    }
    net_buf_unref(rsp);
    return err;
}

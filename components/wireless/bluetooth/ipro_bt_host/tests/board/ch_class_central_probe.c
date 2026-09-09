/* SPDX-License-Identifier: Apache-2.0 */
/* Opt-in BLE 5.3 central channel-map observation fixture. */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/bluetooth/conn.h>
#include "ipro_bt_host/channel_map.h"
#include "ipro_log.h"
#include "shell.h"

#define TAG "CH_CLASS_TEST"

static void pick_connected_le(struct bt_conn *conn, void *user_data)
{
    struct bt_conn **result = user_data;
    struct bt_conn_info info;

    if ((*result == NULL) && (bt_conn_get_info(conn, &info) == 0) &&
        (info.state == BT_CONN_STATE_CONNECTED)) {
        *result = bt_conn_ref(conn);
    }
}

static int read_map(int expected_bad_channel)
{
    struct bt_conn *conn = NULL;
    uint8_t map[5] = {0};
    int rc;

    bt_conn_foreach(BT_CONN_TYPE_LE, pick_connected_le, &conn);
    if (conn == NULL) {
        return -ENOTCONN;
    }
    rc = ipro_bt_conn_le_read_channel_map(conn, map);
    if ((rc == 0) && (expected_bad_channel >= 0) &&
        ((map[(unsigned int)expected_bad_channel / 8U] &
          (1U << ((unsigned int)expected_bad_channel % 8U))) != 0U)) {
        rc = -EBADMSG;
    }
    IPRO_LOGI(TAG,
              "CENTRAL map=%02x%02x%02x%02x%02x expect_bad=%d %s rc=%d sec=%u",
              map[0], map[1], map[2], map[3], map[4], expected_bad_channel,
              (rc == 0) ? "PASS" : "FAIL", rc,
              (unsigned int)bt_conn_get_security(conn));
    bt_conn_unref(conn);
    return rc;
}

static int command(int argc, char **argv)
{
    if ((argc == 2) && (strcmp(argv[1], "map") == 0)) {
        return read_map(-1);
    }
    if ((argc == 3) && (strcmp(argv[1], "expect_bad") == 0)) {
        char *end = NULL;
        unsigned long channel;

        errno = 0;
        channel = strtoul(argv[2], &end, 10);
        if ((errno != 0) || (end == argv[2]) || (*end != '\0') ||
            (channel > 36U)) {
            return -EINVAL;
        }
        return read_map((int)channel);
    }
    return -EINVAL;
}

SHELL_CMD_EXPORT_ALIAS(command, chclass, BLE 5.3 central channel classification test);

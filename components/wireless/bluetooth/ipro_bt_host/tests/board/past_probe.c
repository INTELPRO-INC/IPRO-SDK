/* SPDX-License-Identifier: Apache-2.0 */
/* Opt-in Periodic Advertising Sync Transfer two-board fixture. */
#include <errno.h>
#include <string.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/sys/atomic.h>
#include "coc_pattern.h"
#include "ipro_log.h"
#include "shell.h"

#define TAG "PAST_TEST"
#define PAST_SID 7U
#define PAST_SERVICE_DATA 0x4950U
#define PAST_PAYLOAD_LEN 32U
#define PAST_INTERVAL 160U

#if defined(CONFIG_BT_PER_ADV) && \
    defined(CONFIG_BT_PER_ADV_SYNC_TRANSFER_SENDER)
static struct bt_le_ext_adv *past_adv;
#endif
#if defined(CONFIG_BT_PER_ADV_SYNC_TRANSFER_RECEIVER)
static struct bt_le_per_adv_sync *past_sync;
static bool sync_cb_registered;
#endif
static atomic_t transfers, transferred_syncs, reports, bad, lost, errors;

static void pick_connected_le(struct bt_conn *conn, void *user_data)
{
    struct bt_conn **result = user_data;
    struct bt_conn_info info;

    if ((*result == NULL) && (bt_conn_get_info(conn, &info) == 0) &&
        (info.state == BT_CONN_STATE_CONNECTED)) {
        *result = bt_conn_ref(conn);
    }
}

static struct bt_conn *connected_le(void)
{
    struct bt_conn *conn = NULL;

    bt_conn_foreach(BT_CONN_TYPE_LE, pick_connected_le, &conn);
    return conn;
}

#if defined(CONFIG_BT_PER_ADV_SYNC_TRANSFER_RECEIVER)
static bool exact_periodic_data(const struct net_buf_simple *buf)
{
    return (buf != NULL) && (buf->len == PAST_PAYLOAD_LEN + 2U) &&
           (buf->data[0] == PAST_PAYLOAD_LEN + 1U) &&
           (buf->data[1] == BT_DATA_MANUFACTURER_DATA) &&
           coc_pattern_check(buf->data + 2U, PAST_PAYLOAD_LEN,
                             PAST_SERVICE_DATA);
}

static void past_synced(struct bt_le_per_adv_sync *sync,
                        struct bt_le_per_adv_sync_synced_info *info)
{
    bool valid = (info->conn != NULL) &&
                 (info->service_data == PAST_SERVICE_DATA) &&
                 (info->sid == PAST_SID) &&
                 (info->interval == PAST_INTERVAL) && info->recv_enabled;

    if (!valid || (past_sync != NULL)) {
        atomic_inc(&bad);
        IPRO_LOGE(TAG,
                  "SYNC_BAD via_conn=%u service=%04x sid=%u interval=%u recv=%u duplicate=%u",
                  info->conn != NULL, info->service_data, info->sid,
                  info->interval, info->recv_enabled, past_sync != NULL);
        return;
    }
    past_sync = sync;
    atomic_inc(&transferred_syncs);
    IPRO_LOGI(TAG,
              "SYNC_PASS via_conn=1 service=%04x sid=%u interval=%u recv=1",
              info->service_data, info->sid, info->interval);
}

static void past_terminated(struct bt_le_per_adv_sync *sync,
                            const struct bt_le_per_adv_sync_term_info *info)
{
    if (sync == past_sync) {
        past_sync = NULL;
    }
    atomic_inc(&lost);
    IPRO_LOGI(TAG, "SYNC_TERM reason=%u", info->reason);
}

static void past_recv(struct bt_le_per_adv_sync *sync,
                      const struct bt_le_per_adv_sync_recv_info *info,
                      struct net_buf_simple *buf)
{
    (void)sync;
    if ((info->sid != PAST_SID) || !exact_periodic_data(buf)) {
        atomic_inc(&bad);
        return;
    }
    if (atomic_inc(&reports) == 0) {
        IPRO_LOGI(TAG, "REPORT_PASS sid=%u len=%u exact=1", info->sid,
                  buf->len);
    }
}

static struct bt_le_per_adv_sync_cb past_sync_cb = {
    .synced = past_synced,
    .term = past_terminated,
    .recv = past_recv,
};

static void register_sync_cb(void)
{
    if (!sync_cb_registered) {
        bt_le_per_adv_sync_cb_register(&past_sync_cb);
        sync_cb_registered = true;
    }
}

static int subscribe(bool enable)
{
    struct bt_conn *conn = connected_le();
    int rc;

    if (conn == NULL) return -ENOTCONN;
    register_sync_cb();
    if (enable) {
        const struct bt_le_per_adv_sync_transfer_param params = {
            .skip = 0U,
            .timeout = 1000U,
            .options = BT_LE_PER_ADV_SYNC_TRANSFER_OPT_NONE,
        };
        rc = bt_le_per_adv_sync_transfer_subscribe(conn, &params);
    } else {
        rc = bt_le_per_adv_sync_transfer_unsubscribe(conn);
    }
    bt_conn_unref(conn);
    return rc;
}

static int negative_params(void)
{
    struct bt_conn *conn = connected_le();
    struct bt_le_per_adv_sync_transfer_param params = {
        .skip = 0x01f4U,
        .timeout = 1000U,
    };
    int rc_skip, rc_timeout, rc_options;

    if (conn == NULL) return -ENOTCONN;
    rc_skip = bt_le_per_adv_sync_transfer_subscribe(conn, &params);
    params.skip = 0U;
    params.timeout = 9U;
    rc_timeout = bt_le_per_adv_sync_transfer_subscribe(conn, &params);
    params.timeout = 1000U;
    params.options =
        BT_LE_PER_ADV_SYNC_TRANSFER_OPT_REPORTING_INITIALLY_DISABLED |
        BT_LE_PER_ADV_SYNC_TRANSFER_OPT_FILTER_DUPLICATES;
    rc_options = bt_le_per_adv_sync_transfer_subscribe(conn, &params);
    bt_conn_unref(conn);
    IPRO_LOGI(TAG, "NEGATIVE skip=%d timeout=%d options=%d expected=%d",
              rc_skip, rc_timeout, rc_options, -EINVAL);
    return ((rc_skip == -EINVAL) && (rc_timeout == -EINVAL) &&
            (rc_options == -EINVAL)) ? 0 : -EBADMSG;
}
#endif

#if defined(CONFIG_BT_PER_ADV) && \
    defined(CONFIG_BT_PER_ADV_SYNC_TRANSFER_SENDER)
static int start_periodic_advertiser(void)
{
    const struct bt_le_adv_param ext = {
        .id = BT_ID_DEFAULT, .sid = PAST_SID,
        .options = BT_LE_ADV_OPT_EXT_ADV | BT_LE_ADV_OPT_USE_IDENTITY |
                   BT_LE_ADV_OPT_NO_2M,
        .interval_min = 160U, .interval_max = 160U,
    };
    const struct bt_le_per_adv_param periodic = {
        .interval_min = PAST_INTERVAL, .interval_max = PAST_INTERVAL,
    };
    static const uint8_t name[] = {'I','P','R','O','-','P','A','S','T'};
    struct bt_data ad = BT_DATA(BT_DATA_NAME_COMPLETE, name, sizeof(name));
    uint8_t payload[PAST_PAYLOAD_LEN];
    struct bt_data periodic_ad;
    int rc;

    if (past_adv != NULL) return -EALREADY;
    coc_pattern_fill(payload, sizeof(payload), PAST_SERVICE_DATA);
    periodic_ad = (struct bt_data)BT_DATA(BT_DATA_MANUFACTURER_DATA,
                                          payload, sizeof(payload));
    rc = bt_le_ext_adv_create(&ext, NULL, &past_adv);
    if (!rc) rc = bt_le_ext_adv_set_data(past_adv, &ad, 1U, NULL, 0U);
    if (!rc) rc = bt_le_per_adv_set_param(past_adv, &periodic);
    if (!rc) rc = bt_le_per_adv_set_data(past_adv, &periodic_ad, 1U);
    if (!rc) rc = bt_le_per_adv_start(past_adv);
    if (!rc) rc = bt_le_ext_adv_start(past_adv, BT_LE_EXT_ADV_START_DEFAULT);
    if (rc && (past_adv != NULL)) {
        (void)bt_le_per_adv_stop(past_adv);
        (void)bt_le_ext_adv_stop(past_adv);
        if (bt_le_ext_adv_delete(past_adv) == 0) past_adv = NULL;
    }
    return rc;
}

static int transfer_set(void)
{
    struct bt_conn *conn;
    int rc;

    if (past_adv == NULL) return -ENOENT;
    conn = connected_le();
    if (conn == NULL) return -ENOTCONN;
    rc = bt_le_per_adv_set_info_transfer(past_adv, conn, PAST_SERVICE_DATA);
    bt_conn_unref(conn);
    if (!rc) atomic_inc(&transfers);
    return rc;
}
#endif

static int stop_all(void)
{
    int rc = 0;

#if defined(CONFIG_BT_PER_ADV_SYNC_TRANSFER_RECEIVER)
    if (past_sync != NULL) {
        rc = bt_le_per_adv_sync_delete(past_sync);
        if (!rc) past_sync = NULL;
    }
#endif
#if defined(CONFIG_BT_PER_ADV) && \
    defined(CONFIG_BT_PER_ADV_SYNC_TRANSFER_SENDER)
    if (past_adv != NULL) {
        int err = bt_le_per_adv_stop(past_adv);
        if (!rc && err) rc = err;
        err = bt_le_ext_adv_stop(past_adv);
        if (!rc && err) rc = err;
        err = bt_le_ext_adv_delete(past_adv);
        if (!rc && err) rc = err;
        if (!err) past_adv = NULL;
    }
#endif
    return rc;
}

static int command(int argc, char **argv)
{
    int rc;

    if (argc != 2) return -EINVAL;
#if defined(CONFIG_BT_PER_ADV_SYNC_TRANSFER_RECEIVER)
    if (!strcmp(argv[1], "subscribe")) rc = subscribe(true);
    else if (!strcmp(argv[1], "unsubscribe")) rc = subscribe(false);
    else if (!strcmp(argv[1], "negative")) rc = negative_params();
#else
    if (!strcmp(argv[1], "subscribe") || !strcmp(argv[1], "unsubscribe") ||
        !strcmp(argv[1], "negative")) rc = -ENOTSUP;
#endif
#if defined(CONFIG_BT_PER_ADV) && \
    defined(CONFIG_BT_PER_ADV_SYNC_TRANSFER_SENDER)
    else if (!strcmp(argv[1], "adv")) rc = start_periodic_advertiser();
    else if (!strcmp(argv[1], "transfer")) rc = transfer_set();
#else
    else if (!strcmp(argv[1], "adv") || !strcmp(argv[1], "transfer"))
        rc = -ENOTSUP;
#endif
    else if (!strcmp(argv[1], "stop")) rc = stop_all();
    else if (!strcmp(argv[1], "status")) rc = 0;
    else return -EINVAL;
    if (rc) atomic_inc(&errors);
    IPRO_LOGI(TAG,
              "%s rc=%d transfers=%ld syncs=%ld reports=%ld bad=%ld lost=%ld err=%ld",
              argv[1], rc, (long)atomic_get(&transfers),
              (long)atomic_get(&transferred_syncs),
              (long)atomic_get(&reports), (long)atomic_get(&bad),
              (long)atomic_get(&lost), (long)atomic_get(&errors));
    return rc;
}

SHELL_CMD_EXPORT_ALIAS(command, past, PAST local-set transfer test);

/* SPDX-License-Identifier: Apache-2.0 */
/* Opt-in BLE 5.3 Periodic Advertising ADI/DID bench fixture. */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/sys/atomic.h>
#include "adi_pattern.h"
#include "ipro_log.h"
#include "shell.h"

#define TAG "ADI_TEST"
#define PAYLOAD_LEN IPRO_ADI_PATTERN_LEN

static const uint8_t marker[] = {'I', 'P', 'R', 'O', '-', 'A', 'D', 'I'};
static struct bt_le_ext_adv *advertiser;
static struct bt_le_per_adv_sync *sync_obj;
static bool callbacks_registered;
static bool seeking;
static bool include_adi;
static bool filter_duplicates;
static bool bare_advertiser;
static bool accept_any_sid5;
static uint16_t tx_sequence;
static uint16_t last_rx_sequence;
static bool have_last_rx_sequence;
static atomic_t rx_total;
static atomic_t rx_good;
static atomic_t rx_bad;
static atomic_t rx_same;
static atomic_t rx_changed;
static atomic_t lost;
static atomic_t errors;

static bool check_payload(const struct net_buf_simple *buf, uint16_t *sequence)
{
    if ((buf == NULL) || (buf->len != PAYLOAD_LEN + 2U) ||
        (buf->data[0] != PAYLOAD_LEN + 1U) ||
        (buf->data[1] != BT_DATA_MANUFACTURER_DATA)) {
        return false;
    }
    return ipro_adi_pattern_check(buf->data + 2, PAYLOAD_LEN, sequence);
}

static void reset_rx_stats(void)
{
    atomic_set(&rx_total, 0);
    atomic_set(&rx_good, 0);
    atomic_set(&rx_bad, 0);
    atomic_set(&rx_same, 0);
    atomic_set(&rx_changed, 0);
    atomic_set(&lost, 0);
    atomic_set(&errors, 0);
    last_rx_sequence = 0;
    have_last_rx_sequence = false;
}

static void synced(struct bt_le_per_adv_sync *sync,
                   struct bt_le_per_adv_sync_synced_info *info)
{
    sync_obj = sync;
    seeking = false;
    IPRO_LOGI(TAG, "SYNC interval=%u filter=%u", info->interval,
              filter_duplicates);
}

static void terminated(struct bt_le_per_adv_sync *sync,
                       const struct bt_le_per_adv_sync_term_info *info)
{
    (void)sync;
    sync_obj = NULL;
    atomic_inc(&lost);
    IPRO_LOGW(TAG, "SYNC_LOST reason=%u", info->reason);
}

static void periodic_recv(struct bt_le_per_adv_sync *sync,
                          const struct bt_le_per_adv_sync_recv_info *info,
                          struct net_buf_simple *buf)
{
    uint16_t sequence;

    (void)sync;
    atomic_inc(&rx_total);
    if (!check_payload(buf, &sequence)) {
        atomic_inc(&rx_bad);
        return;
    }
    if (have_last_rx_sequence) {
        if (sequence == last_rx_sequence) {
            atomic_inc(&rx_same);
        } else {
            atomic_inc(&rx_changed);
        }
    }
    last_rx_sequence = sequence;
    have_last_rx_sequence = true;
    atomic_inc(&rx_good);
    IPRO_LOGI(TAG, "RX seq=%u len=%u exact=1 total=%ld same=%ld changed=%ld",
              sequence, buf == NULL ? 0U : buf->len, (long)atomic_get(&rx_good),
              (long)atomic_get(&rx_same), (long)atomic_get(&rx_changed));
    (void)info;
}

static struct bt_le_per_adv_sync_cb sync_callbacks = {
    .synced = synced,
    .term = terminated,
    .recv = periodic_recv,
};

static bool find_marker(struct bt_data *data, void *user_data)
{
    bool *found = user_data;

    if ((data->type == BT_DATA_NAME_COMPLETE) &&
        (data->data_len == sizeof(marker)) &&
        (memcmp(data->data, marker, sizeof(marker)) == 0)) {
        *found = true;
    }
    return !*found;
}

static void scan_recv(const struct bt_le_scan_recv_info *info,
                      struct net_buf_simple *buf)
{
    struct bt_le_per_adv_sync_param params = {0};
    bool found = false;
    int rc;

    if (!seeking || (sync_obj != NULL) || (info->interval == 0U)) {
        return;
    }
    if (accept_any_sid5) {
        found = (info->sid == 5U);
    } else {
        bt_data_parse(buf, find_marker, &found);
    }
    if (!found) {
        return;
    }
    params.sid = info->sid;
    params.skip = 0;
    params.timeout = 1000;
    if (filter_duplicates) {
        params.options = BT_LE_PER_ADV_SYNC_OPT_FILTER_DUPLICATE;
    }
    bt_addr_le_copy(&params.addr, info->addr);
    seeking = false;
    rc = bt_le_per_adv_sync_create(&params, &sync_obj);
    if (rc != 0) {
        atomic_inc(&errors);
        seeking = true;
    }
    IPRO_LOGI(TAG, "CREATE_SYNC sid=%u interval=%u filter=%u rc=%d",
              info->sid, info->interval, filter_duplicates, rc);
    if ((rc == 0) && (bt_le_scan_stop() != 0)) {
        atomic_inc(&errors);
    }
}

static struct bt_le_scan_cb scan_callbacks = {
    .recv = scan_recv,
};

static int set_periodic_data(uint16_t sequence)
{
    uint8_t payload[PAYLOAD_LEN];
    struct bt_data data;

    if (advertiser == NULL) {
        return -ENODEV;
    }
    ipro_adi_pattern_fill(payload, sequence);
    data = (struct bt_data)BT_DATA(BT_DATA_MANUFACTURER_DATA, payload,
                                   sizeof(payload));
    return bt_le_per_adv_set_data(advertiser, &data, 1U);
}

static int start_advertiser(bool with_adi, bool without_marker)
{
    struct bt_le_adv_param ext = {
        .id = BT_ID_DEFAULT,
        .sid = 5,
        .options = BT_LE_ADV_OPT_EXT_ADV | BT_LE_ADV_OPT_USE_IDENTITY |
                   BT_LE_ADV_OPT_NO_2M,
        .interval_min = 160,
        .interval_max = 160,
    };
    struct bt_le_per_adv_param periodic = {
        .interval_min = 160,
        .interval_max = 160,
        .options = BT_LE_PER_ADV_OPT_NONE,
    };
    struct bt_data name = BT_DATA(BT_DATA_NAME_COMPLETE, marker,
                                  sizeof(marker));
    int rc;

    if (advertiser != NULL) {
        return -EALREADY;
    }
    if (with_adi) {
        periodic.options |= BT_LE_PER_ADV_OPT_INCLUDE_ADI;
    }
    include_adi = with_adi;
    bare_advertiser = without_marker;
    tx_sequence = 0;
    rc = bt_le_ext_adv_create(&ext, NULL, &advertiser);
    if ((rc == 0) && !without_marker) {
        rc = bt_le_ext_adv_set_data(advertiser, &name, 1U, NULL, 0U);
    }
    if (rc == 0) {
        rc = bt_le_per_adv_set_param(advertiser, &periodic);
    }
    if (rc == 0) {
        rc = set_periodic_data(tx_sequence);
    }
    if (rc == 0) {
        rc = bt_le_per_adv_start(advertiser);
    }
    if (rc == 0) {
        rc = bt_le_ext_adv_start(advertiser, BT_LE_EXT_ADV_START_DEFAULT);
    }
    if ((rc != 0) && (advertiser != NULL)) {
        (void)bt_le_per_adv_stop(advertiser);
        (void)bt_le_ext_adv_stop(advertiser);
        if (bt_le_ext_adv_delete(advertiser) == 0) {
            advertiser = NULL;
        }
    }
    return rc;
}

static int start_scanner(bool filter, bool any_sid5)
{
    int rc;

    if (sync_obj != NULL || seeking) {
        return -EALREADY;
    }
    if (!callbacks_registered) {
        bt_le_per_adv_sync_cb_register(&sync_callbacks);
        rc = bt_le_scan_cb_register(&scan_callbacks);
        if (rc != 0) {
            return rc;
        }
        callbacks_registered = true;
    }
    reset_rx_stats();
    filter_duplicates = filter;
    accept_any_sid5 = any_sid5;
    seeking = true;
    rc = bt_le_scan_start(BT_LE_SCAN_PASSIVE, NULL);
    /* The central test app keeps a background scan active while disconnected.
     * Registering our callback above is sufficient to share that scan.
     */
    if (rc == -EALREADY) {
        rc = 0;
    }
    if (rc != 0) {
        seeking = false;
    }
    return rc;
}

static int stop_all(void)
{
    int rc = 0;

    seeking = false;
    (void)bt_le_scan_stop();
    if (sync_obj != NULL) {
        rc = bt_le_per_adv_sync_delete(sync_obj);
        if (rc == 0) {
            sync_obj = NULL;
        }
    }
    if (advertiser != NULL) {
        rc = bt_le_per_adv_stop(advertiser);
        if (rc == 0) {
            rc = bt_le_ext_adv_stop(advertiser);
        }
        if (rc == 0) {
            rc = bt_le_ext_adv_delete(advertiser);
        }
        if (rc == 0) {
            advertiser = NULL;
        }
    }
    return rc;
}

static void log_status(const char *operation, int rc)
{
    IPRO_LOGI(TAG,
              "%s rc=%d adv=%u adi=%u seeking=%u sync=%u filter=%u tx_seq=%u "
              "rx=%ld good=%ld bad=%ld same=%ld changed=%ld last=%u lost=%ld err=%ld",
              operation, rc, advertiser != NULL, include_adi, seeking,
              sync_obj != NULL, filter_duplicates, tx_sequence,
              (long)atomic_get(&rx_total), (long)atomic_get(&rx_good),
              (long)atomic_get(&rx_bad), (long)atomic_get(&rx_same),
              (long)atomic_get(&rx_changed), last_rx_sequence,
              (long)atomic_get(&lost), (long)atomic_get(&errors));
    IPRO_LOGI(TAG, "MODE bare=%u any_sid5=%u", bare_advertiser,
              accept_any_sid5);
}

static int command(int argc, char **argv)
{
    int rc = 0;

    if ((argc == 2) && (strcmp(argv[1], "caps") == 0)) {
        struct bt_le_local_features local;

        rc = bt_le_get_local_features(&local);
        if (rc == 0) {
            IPRO_LOGI(TAG, "CAPS adi=%u periodic=%u raw4=%02x",
                      !!BT_FEAT_LE_PER_ADV_ADI_SUPP(local.features),
                      !!BT_FEAT_LE_EXT_PER_ADV(local.features),
                      local.features[4]);
        }
    } else if ((argc == 3) && (strcmp(argv[1], "adv") == 0)) {
        if (strcmp(argv[2], "on") == 0) {
            rc = start_advertiser(true, false);
        } else if (strcmp(argv[2], "off") == 0) {
            rc = start_advertiser(false, false);
        } else if (strcmp(argv[2], "bare") == 0) {
            rc = start_advertiser(true, true);
        } else {
            return -EINVAL;
        }
    } else if ((argc == 3) && (strcmp(argv[1], "scan") == 0)) {
        if (strcmp(argv[2], "filter") == 0) {
            rc = start_scanner(true, false);
        } else if (strcmp(argv[2], "all") == 0) {
            rc = start_scanner(false, false);
        } else if (strcmp(argv[2], "any") == 0) {
            rc = start_scanner(true, true);
        } else {
            return -EINVAL;
        }
    } else if ((argc == 3) && (strcmp(argv[1], "update") == 0)) {
        char *end = NULL;
        unsigned long sequence;

        errno = 0;
        sequence = strtoul(argv[2], &end, 0);
        if ((errno != 0) || (end == argv[2]) || (*end != '\0') ||
            (sequence > UINT16_MAX)) {
            return -EINVAL;
        }
        rc = set_periodic_data((uint16_t)sequence);
        if (rc == 0) {
            tx_sequence = (uint16_t)sequence;
        }
    } else if ((argc == 2) && (strcmp(argv[1], "stop") == 0)) {
        rc = stop_all();
    } else if (!((argc == 2) && (strcmp(argv[1], "status") == 0))) {
        return -EINVAL;
    }
    log_status(argv[1], rc);
    return rc;
}

SHELL_CMD_EXPORT_ALIAS(command, adi, BLE 5.3 periodic advertising ADI test);

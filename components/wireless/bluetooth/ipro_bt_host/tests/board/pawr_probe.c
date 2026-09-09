/* SPDX-License-Identifier: Apache-2.0 */
/* Opt-in bench fixture; never included in customer Host libraries. */
#include <errno.h>
#include <string.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/byteorder.h>
#include "pawr_pattern.h"
#include "pawr_gate.h"
#include "pawr_diagnose.h"
#include "pawr_ledger.h"
#include "../../../ipro_ble_controller/include/ipro_pawr_radio_trace.h"
#include "ipro_log.h"
#include "shell.h"

#define TAG "PAWR_TEST"
#define SUBEVENTS PAWR_PATTERN_SUBEVENTS
#define PAYLOAD 24
static struct bt_le_ext_adv *advertiser;
static struct bt_le_per_adv_sync *sync_obj;
static bool registered, seeking, sync_is_pawr;
static bool stop_scan_after_create;
static uint16_t sequence[SUBEVENTS];
static atomic_t requested, submitted, rx_good, rsp_good, bad, empty, errors, lost;
static atomic_t rx_sub[SUBEVENTS], rsp_sub[SUBEVENTS];
static atomic_t sync_gate = PAWR_GATE_CLOSED, adv_gate = PAWR_GATE_CLOSED;
static atomic_t cancelled, rsp_status_bad, rsp_slot_bad, rsp_payload_bad;
static atomic_t first_rsp_state;
static struct pawr_diagnosis first_rsp;
static struct pawr_ledger ledger;
static atomic_t first_rx_state;
static struct pawr_diagnosis first_rx;
static unsigned first_rx_event, first_rx_sub;
static unsigned first_rsp_sub, first_rsp_slot, first_rsp_tx_status;
static const uint8_t marker[] = {'I','P','R','O','-','P','A','W','R'};

static void data_request_active(struct bt_le_ext_adv *adv,
                         const struct bt_le_per_adv_data_request *req)
{
    atomic_inc(&requested);
    if (!req->count || req->start >= SUBEVENTS ||
        req->count > SUBEVENTS - req->start) {
        atomic_inc(&errors);
        IPRO_LOGE(TAG, "unexpected request start=%u count=%u", req->start, req->count);
        return;
    }
    /* One subevent per command: aggregate lengths plus HCI metadata exceed 255. */
    uint8_t bytes[PAWR_PATTERN_MAX];
    for (uint8_t i = 0; i < req->count; i++) {
        uint8_t sub = req->start + i;
        struct net_buf_simple buffer;
        pawr_pattern_fill(bytes, sizeof(bytes), sub, ++sequence[sub]);
        net_buf_simple_init_with_data(&buffer, bytes, pawr_pattern_size(sub));
        struct bt_le_per_adv_subevent_data_params param = {
            .subevent = sub, .response_slot_count = 1, .data = &buffer,
        };
        int rc = bt_le_per_adv_set_subevent_data(adv, 1, &param);
        pawr_ledger_add(&ledger, PAWR_LEDGER_TX, sys_get_le16(bytes + 2),
                        UINT16_MAX, sub, rc);
        if (rc) {
            if (atomic_inc(&errors) == 0) IPRO_LOGE(TAG, "set data sub=%u rc=%d", sub, rc);
        } else atomic_inc(&submitted);
    }
}

static void data_request(struct bt_le_ext_adv *adv,
                         const struct bt_le_per_adv_data_request *req)
{
    if (!pawr_gate_enter(&adv_gate)) { atomic_inc(&cancelled); return; }
    data_request_active(adv, req);
    pawr_gate_leave(&adv_gate);
}

static bool valid_payload(const struct net_buf_simple *buf, uint8_t sub)
{
    return buf && pawr_pattern_check(buf->data, buf->len, sub);
}

static void response_active(struct bt_le_ext_adv *adv, struct bt_le_per_adv_response_info *info,
                     struct net_buf_simple *buf)
{
    (void)adv;
    if (!buf) { atomic_inc(&empty); return; }
    bool status_bad = info->tx_status != 0;
    bool slot_bad = info->response_slot != 0 || info->subevent >= SUBEVENTS;
    bool payload_bad = !valid_payload(buf, info->subevent);
    if (status_bad || slot_bad || payload_bad) {
        if (status_bad) atomic_inc(&rsp_status_bad);
        if (slot_bad) atomic_inc(&rsp_slot_bad);
        if (payload_bad) atomic_inc(&rsp_payload_bad);
        if (atomic_cas(&first_rsp_state, 0, 1)) {
            first_rsp = pawr_diagnose(buf->data, buf->len, info->subevent);
            first_rsp_sub = info->subevent;
            first_rsp_slot = info->response_slot;
            first_rsp_tx_status = info->tx_status;
            atomic_set(&first_rsp_state, 2);
        }
        if (atomic_inc(&bad) == 0)
            IPRO_LOGE(TAG, "RESPONSE_BAD tx_status=%u sub=%u slot=%u len=%u payload_bad=%u",
                      info->tx_status, info->subevent, info->response_slot,
                      buf->len, payload_bad);
        return;
    }
    atomic_inc(&rsp_sub[info->subevent]);
    pawr_ledger_add(&ledger, PAWR_LEDGER_RSP, sys_get_le16(buf->data + 2),
                    UINT16_MAX, info->subevent, 0);
    if (atomic_inc(&rsp_good) == 0)
        IPRO_LOGI(TAG, "RESPONSE_PASS sub=%u slot=%u len=%u seq=%u",
                  info->subevent, info->response_slot, buf->len, sys_get_le16(buf->data + 2));
}
static void response(struct bt_le_ext_adv *adv, struct bt_le_per_adv_response_info *info,
                     struct net_buf_simple *buf)
{
    if (!pawr_gate_enter(&adv_gate)) { atomic_inc(&cancelled); return; }
    response_active(adv, info, buf);
    pawr_gate_leave(&adv_gate);
}
static const struct bt_le_ext_adv_cb adv_cb = {
    .pawr_data_request = data_request, .pawr_response = response,
};

static void synced_active(struct bt_le_per_adv_sync *obj, struct bt_le_per_adv_sync_synced_info *info)
{
    sync_obj = obj;
    sync_is_pawr = info->num_subevents != 0;
    uint8_t subs[SUBEVENTS] = {0, 1, 2, 3};
    struct bt_le_per_adv_sync_subevent_params params = {
        .num_subevents = SUBEVENTS, .subevents = subs,
    };
    int rc = !sync_is_pawr ? 0 :
        (info->num_subevents == SUBEVENTS ? bt_le_per_adv_sync_subevent(obj, &params) : -EINVAL);
    if (rc) atomic_inc(&errors);
    IPRO_LOGI(TAG, "SYNC interval=%u subevents=%u select_rc=%d", info->interval,
              info->num_subevents, rc);
}
static void synced(struct bt_le_per_adv_sync *obj, struct bt_le_per_adv_sync_synced_info *info)
{
    if (!pawr_gate_enter(&sync_gate)) { atomic_inc(&cancelled); return; }
    synced_active(obj, info);
    pawr_gate_leave(&sync_gate);
}
static void terminated(struct bt_le_per_adv_sync *obj, const struct bt_le_per_adv_sync_term_info *info)
{
    if (sync_obj == obj) {
        pawr_gate_close(&sync_gate);
        sync_obj = NULL;
    }
    atomic_inc(&lost);
    IPRO_LOGW(TAG, "SYNC_LOST reason=%u", info->reason);
}
static void periodic_recv_active(struct bt_le_per_adv_sync *obj,
                          const struct bt_le_per_adv_sync_recv_info *info,
                          struct net_buf_simple *buf)
{
    if (!buf || !buf->len) {
        if (sync_is_pawr) pawr_ledger_add(&ledger, PAWR_LEDGER_EMPTY, UINT16_MAX,
            info->periodic_event_counter, info->subevent, 0);
        atomic_inc(&empty); return;
    }
    if (!sync_is_pawr) {
        /* Ordinary periodic train carries one complete manufacturer AD. */
        if (buf->len != PAYLOAD + 2 || buf->data[0] != PAYLOAD + 1 ||
            buf->data[1] != BT_DATA_MANUFACTURER_DATA ||
            !coc_pattern_check(buf->data + 2, PAYLOAD, 0x4242)) {
            atomic_inc(&bad);
            return;
        }
        if (atomic_inc(&rx_good) == 0) IPRO_LOGI(TAG, "PERIODIC_PASS len=%u exact_content=1", buf->len);
        return;
    }
    if (!valid_payload(buf, info->subevent)) {
        if (atomic_cas(&first_rx_state, 0, 1)) {
            first_rx = pawr_diagnose(buf->data, buf->len, info->subevent);
            first_rx_event = info->periodic_event_counter;
            first_rx_sub = info->subevent;
            atomic_set(&first_rx_state, 2);
        }
        if (atomic_inc(&bad) == 0) {
            IPRO_LOGE(TAG, "RX_BAD event=%u sub=%u len=%u head=%02x/%02x/%02x/%02x",
                      info->periodic_event_counter, info->subevent, buf->len,
                      buf->len > 0 ? buf->data[0] : 0,
                      buf->len > 1 ? buf->data[1] : 0,
                      buf->len > 2 ? buf->data[2] : 0,
                      buf->len > 3 ? buf->data[3] : 0);
        }
        return;
    }
    atomic_inc(&rx_sub[info->subevent]);
    struct bt_le_per_adv_response_params params = {
        .request_event = info->periodic_event_counter,
        .request_subevent = info->subevent,
        .response_subevent = info->subevent, .response_slot = 0,
    };
    int rc = bt_le_per_adv_set_response_data(obj, &params, buf);
    pawr_ledger_add(&ledger, PAWR_LEDGER_RX, sys_get_le16(buf->data + 2),
                    info->periodic_event_counter, info->subevent, rc);
    if (rc && atomic_inc(&errors) == 0) IPRO_LOGE(TAG, "response data rc=%d", rc);
    if (atomic_inc(&rx_good) == 0)
        IPRO_LOGI(TAG, "RX_PASS event=%u sub=%u len=%u seq=%u echo_rc=%d",
                  info->periodic_event_counter, info->subevent, buf->len,
                  sys_get_le16(buf->data + 2), rc);
}
static void periodic_recv(struct bt_le_per_adv_sync *obj,
                          const struct bt_le_per_adv_sync_recv_info *info,
                          struct net_buf_simple *buf)
{
    if (!pawr_gate_enter(&sync_gate)) { atomic_inc(&cancelled); return; }
    periodic_recv_active(obj, info, buf);
    pawr_gate_leave(&sync_gate);
}
static struct bt_le_per_adv_sync_cb sync_cb = {
    .synced = synced, .term = terminated, .recv = periodic_recv,
};
static bool find_marker(struct bt_data *data, void *user)
{
    bool *found = user;
    if (data->type == BT_DATA_NAME_COMPLETE && data->data_len == sizeof(marker) &&
        !memcmp(data->data, marker, sizeof(marker))) *found = true;
    return !*found;
}
static void scan_recv_active(const struct bt_le_scan_recv_info *info, struct net_buf_simple *buf)
{
    if (!seeking || sync_obj || !info->interval) return;
    bool found = false;
    bt_data_parse(buf, find_marker, &found);
    if (!found) return;
    struct bt_le_per_adv_sync_param params = {
        .sid = info->sid, .skip = 0, .timeout = 1000,
    };
    bt_addr_le_copy(&params.addr, info->addr);
    seeking = false;
    int rc = bt_le_per_adv_sync_create(&params, &sync_obj);
    if (rc) { atomic_inc(&errors); seeking = true; }
    IPRO_LOGI(TAG, "CREATE_SYNC sid=%u interval=%u rc=%d", info->sid, info->interval, rc);
    if (!rc && stop_scan_after_create) {
        int stop_rc = bt_le_scan_stop();
        if (stop_rc) atomic_inc(&errors);
        IPRO_LOGI(TAG, "EXPLICIT_SCAN_STOP rc=%d", stop_rc);
    }
}
static void scan_recv(const struct bt_le_scan_recv_info *info, struct net_buf_simple *buf)
{
    if (!pawr_gate_enter(&sync_gate)) return;
    scan_recv_active(info, buf);
    pawr_gate_leave(&sync_gate);
}
static struct bt_le_scan_cb scan_cb = {.recv = scan_recv};

static int start_advertiser(bool pawr)
{
    if (advertiser) return -EALREADY;
    if (!pawr_gate_open(&adv_gate)) return -EBUSY;
    struct bt_le_adv_param ext = {
        .id = BT_ID_DEFAULT, .sid = 3,
        .options = BT_LE_ADV_OPT_EXT_ADV | BT_LE_ADV_OPT_USE_IDENTITY | BT_LE_ADV_OPT_NO_2M,
        .interval_min = 160, .interval_max = 160,
    };
    struct bt_le_per_adv_param periodic = {
        .interval_min = 240, .interval_max = 240, /* 300 ms */
        .num_subevents = SUBEVENTS, .subevent_interval = 40, /* 50 ms */
        .response_slot_delay = 16, /* 20 ms to leave Host callback budget */
        /* 1M 247-byte responses exceed the short fixture's 2 ms reservation. */
        .response_slot_spacing = pawr_long_payloads ? 32 : 16, .num_response_slots = 1,
    };
    struct bt_data ad = BT_DATA(BT_DATA_NAME_COMPLETE, marker, sizeof(marker));
    uint8_t plain[PAYLOAD];
    coc_pattern_fill(plain, sizeof(plain), 0x4242);
    struct bt_data periodic_ad = BT_DATA(BT_DATA_MANUFACTURER_DATA, plain, sizeof(plain));
    if (!pawr) {
        periodic.num_subevents = 0;
        periodic.subevent_interval = 0;
        periodic.response_slot_delay = 0;
        periodic.response_slot_spacing = 0;
        periodic.num_response_slots = 0;
    }
    int rc = bt_le_ext_adv_create(&ext, &adv_cb, &advertiser);
    if (!rc) rc = bt_le_ext_adv_set_data(advertiser, &ad, 1, NULL, 0);
    if (!rc) rc = bt_le_per_adv_set_param(advertiser, &periodic);
    if (!rc && !pawr) rc = bt_le_per_adv_set_data(advertiser, &periodic_ad, 1);
    if (!rc) rc = bt_le_per_adv_start(advertiser);
    if (!rc) rc = bt_le_ext_adv_start(advertiser, BT_LE_EXT_ADV_START_DEFAULT);
    if (rc && advertiser) {
        /* Keep the handle for explicit stop/drain cleanup; do not race an
         * already admitted data-request callback with immediate deletion. */
        pawr_gate_close(&adv_gate);
    }
    if (rc && !advertiser) pawr_gate_close(&adv_gate);
    return rc;
}
static int stop_fixture(void)
{
    pawr_gate_close(&sync_gate);
    pawr_gate_close(&adv_gate);
    /* Shell context only. HCI completion and Host callbacks remain runnable.
     * Timeout keeps handles and admission closed; another stop can retry. */
    unsigned wait;
    for (wait = 0; wait < 1000; wait++) {
        if (pawr_gate_drained(&sync_gate) && pawr_gate_drained(&adv_gate)) break;
        k_sleep(K_MSEC(1));
    }
    if (wait == 1000) return -EBUSY;
    seeking = false;
    int rc = bt_le_scan_stop();
    if (rc == -EALREADY) rc = 0;
    if (rc) return rc;
    if (sync_obj) {
        rc = bt_le_per_adv_sync_delete(sync_obj);
        if (rc) return rc;
    }
    if (advertiser) {
        rc = bt_le_per_adv_stop(advertiser);
        if (rc && rc != -EALREADY) return rc;
        rc = bt_le_ext_adv_stop(advertiser);
        if (rc) return rc;
        rc = bt_le_ext_adv_delete(advertiser);
        if (rc) return rc;
        advertiser = NULL;
    }
    return 0;
}
static int command(int argc, char **argv)
{
    if (argc != 2) return -EINVAL;
    int rc = 0;
    if (!strcmp(argv[1], "long") || !strcmp(argv[1], "short")) {
        if (advertiser || sync_obj || seeking) return -EBUSY;
        pawr_long_payloads = !strcmp(argv[1], "long");
    } else if (!strcmp(argv[1], "caps")) {
        struct bt_le_local_features local;
        rc = bt_le_get_local_features(&local);
        if (!rc) IPRO_LOGI(TAG, "CAPS ext=%u periodic=%u pawr_adv=%u pawr_scan=%u raw=%02x%02x%02x%02x%02x%02x%02x%02x",
            !!BT_FEAT_LE_EXT_ADV(local.features), !!BT_FEAT_LE_EXT_PER_ADV(local.features),
            !!BT_FEAT_LE_PAWR_ADVERTISER(local.features), !!BT_FEAT_LE_PAWR_SCANNER(local.features),
            local.features[0],local.features[1],local.features[2],local.features[3],
            local.features[4],local.features[5],local.features[6],local.features[7]);
        struct net_buf *rsp = NULL;
        if (!rc) rc = bt_hci_cmd_send_sync(BT_HCI_OP_READ_SUPPORTED_COMMANDS, NULL, &rsp);
        if (!rc && rsp && rsp->len == sizeof(struct bt_hci_rp_read_supported_commands)) {
            const struct bt_hci_rp_read_supported_commands *commands = (const void *)rsp->data;
            IPRO_LOGI(TAG, "COMMANDS status=%u byte46=%02x byte47=%02x",
                commands->status, commands->commands[46], commands->commands[47]);
        }
        if (rsp) net_buf_unref(rsp);
    } else if (!strcmp(argv[1], "adv")) rc = start_advertiser(true);
    else if (!strcmp(argv[1], "plain")) rc = start_advertiser(false);
    else if (!strcmp(argv[1], "scan") || !strcmp(argv[1], "scan_once")) {
        if (sync_obj || seeking) return -EALREADY;
        stop_scan_after_create = !strcmp(argv[1], "scan_once");
        if (!registered) {
            bt_le_per_adv_sync_cb_register(&sync_cb);
            rc = bt_le_scan_cb_register(&scan_cb);
            if (rc) return rc;
            registered = true;
        }
        if (!pawr_gate_open(&sync_gate)) return -EBUSY;
        seeking = true;
        rc = bt_le_scan_start(BT_LE_SCAN_PASSIVE, NULL);
        if (rc == -EALREADY) rc = 0;
        if (rc) { seeking = false; pawr_gate_close(&sync_gate); }
    } else if (!strcmp(argv[1], "stop")) {
        rc = stop_fixture();
    } else if (!strcmp(argv[1], "ledger") || !strcmp(argv[1], "dump")) {
        if (advertiser || sync_obj || seeking) return -EBUSY;
        rc = stop_fixture();
        if (rc) return rc;
        if (!strcmp(argv[1], "ledger")) {
            pawr_ledger_reset(&ledger);
            ipro_pawr_radio_trace_reset();
            ipro_pawr_advrx_trace_reset();
        }
        else for (unsigned kind = 0; kind < PAWR_LEDGER_KINDS; kind++) {
            IPRO_LOGI(TAG, "LEDGER kind=%u count=%u dropped=%u", kind,
                      ledger.count[kind], ledger.dropped[kind]);
            for (unsigned i = 0; i < ledger.count[kind]; i++) {
                const struct pawr_ledger_entry *e = &ledger.rows[kind][i];
                IPRO_LOGI(TAG, "LEDGER_ROW kind=%u seq=%u event=%u sub=%u rc=%d",
                          kind, e->seq, e->event, e->sub, e->rc);
            }
        }
        if (!strcmp(argv[1], "dump")) {
            struct ipro_pawr_trace_row row;
            unsigned index = 0, dropped;
            while (ipro_pawr_radio_trace_get(index, &row, &dropped)) {
                IPRO_LOGI(TAG, "RADIO seq=%u event=%u sub=%u phases=%u irq=%u ch=%u",
                          row.seq, row.event, row.sub, row.phases, row.irq, row.channel);
                IPRO_LOGI(TAG, "TXTIME event=%u sub=%u valid=%u hs=%lu hus=%u refhs=%lu refhus=%u",
                          row.event, row.sub, row.time_valid, (unsigned long)row.start_hs,
                          row.start_hus, (unsigned long)row.ref_hs, row.ref_hus);
                index++;
            }
            IPRO_LOGI(TAG, "RADIO_END count=%u dropped=%u", index, dropped);
            index = 0;
            while (ipro_pawr_advrx_trace_get(index, &row, &dropped)) {
                IPRO_LOGI(TAG, "ADVRX event=%u sub=%u phases=%u irq=%u status=%04x ch=%u len=%u type=%u n=%u",
                          row.event, row.sub, row.phases, row.irq, row.seq,
                          row.channel, row.length, row.type, row.packets);
                IPRO_LOGI(TAG, "RXTIME event=%u sub=%u valid=%u hs=%lu hus=%u refhs=%lu refhus=%u winhus=%lu",
                          row.event, row.sub, row.time_valid, (unsigned long)row.start_hs,
                          row.start_hus, (unsigned long)row.ref_hs, row.ref_hus,
                          (unsigned long)row.window_hus);
                index++;
            }
            IPRO_LOGI(TAG, "ADVRX_END count=%u dropped=%u", index, dropped);
        }
    } else if (strcmp(argv[1], "status")) return -EINVAL;
    IPRO_LOGI(TAG, "%s rc=%d req=%ld tx=%ld rx=%ld rsp=%ld bad=%ld empty=%ld err=%ld lost=%ld",
        argv[1], rc, (long)atomic_get(&requested), (long)atomic_get(&submitted),
        (long)atomic_get(&rx_good), (long)atomic_get(&rsp_good), (long)atomic_get(&bad),
        (long)atomic_get(&empty), (long)atomic_get(&errors), (long)atomic_get(&lost));
    /* Do not dump five more lines while starting a deadline-driven train.
     * Full counters remain available explicitly; stop drains callbacks first. */
    if (strcmp(argv[1], "status") && strcmp(argv[1], "stop")) return rc;
    if (atomic_get(&first_rx_state) == 2) {
        IPRO_LOGI(TAG, "FIRST_RX event=%u sub=%u len=%u expected_len=%u seq=%u self_valid=%u diff=%d actual=%02x expected=%02x",
            first_rx_event, first_rx_sub, first_rx.length, first_rx.expected_length,
            first_rx.sequence, first_rx.self_valid, first_rx.first_diff,
            first_rx.actual, first_rx.expected);
    }
    if (atomic_get(&first_rsp_state) == 2) {
        IPRO_LOGI(TAG, "FIRST_RSP status=%u sub=%u slot=%u len=%u expected_len=%u header_len=%u seq=%u encoded_sub=%u self_valid=%u diff=%d actual=%02x expected=%02x",
            first_rsp_tx_status, first_rsp_sub, first_rsp_slot, first_rsp.length,
            first_rsp.expected_length, first_rsp.header_length, first_rsp.sequence,
            first_rsp.encoded_sub, first_rsp.self_valid, first_rsp.first_diff,
            first_rsp.actual, first_rsp.expected);
    }
    IPRO_LOGI(TAG, "detail cancelled=%ld rsp_status_bad=%ld rsp_slot_bad=%ld rsp_payload_bad=%ld",
        (long)atomic_get(&cancelled), (long)atomic_get(&rsp_status_bad),
        (long)atomic_get(&rsp_slot_bad), (long)atomic_get(&rsp_payload_bad));
    for (unsigned sub = 0; sub < SUBEVENTS; sub++)
        IPRO_LOGI(TAG, "sub=%u expected_len=%u rx=%ld rsp=%ld", sub, pawr_pattern_size(sub),
            (long)atomic_get(&rx_sub[sub]), (long)atomic_get(&rsp_sub[sub]));
    return rc;
}
SHELL_CMD_EXPORT_ALIAS(command, pawr, PAwR bench test);

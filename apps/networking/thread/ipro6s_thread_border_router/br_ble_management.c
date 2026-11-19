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

#include <generated/autoconf.h>
#include "br_ble_management.h"
#include "br_ble_protocol.h"
#include "br_wifi_profile.h"
#include "br_wifi_storage.h"
#include "wifi_backhaul.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "ipro_log.h"
#include "btdm_sdk.h"
#include "hci_driver_ipro_ble_ctlr.h"
#include "ipro_ble_ctlr_hcitl.h"
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci_types.h>
#include <zephyr/settings/settings.h>
#include <ipro_matter_wifi.h>
#include <ipro_matter_br_thread.h>
#include <openthread/thread.h>
#include <openthread/dataset_ftd.h>
#include <openthread/ip6.h>
#include "openthread_port.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#define TAG "BR-BLE"
#define SERVICE_UUID BT_UUID_128_ENCODE(0x8b07e100, 0x9de3, 0x4a53, 0x90a5, 0xf14e2c794601)
#define REQUEST_UUID BT_UUID_128_ENCODE(0x8b07e101, 0x9de3, 0x4a53, 0x90a5, 0xf14e2c794601)
#define RESPONSE_UUID BT_UUID_128_ENCODE(0x8b07e102, 0x9de3, 0x4a53, 0x90a5, 0xf14e2c794601)

#include "br_management.h"
#include "br_factory_reset.h"
#ifdef CONFIG_THREAD_BR_LAN_MANAGEMENT
#include "br_lan_management.h"
#endif
struct queued_frame {
    struct bt_conn *connection;
    uint32_t epoch;
    uint16_t length;
    uint8_t bytes[BR_BLE_MAX_FRAME];
};
static QueueHandle_t sQueue;
static struct bt_conn *sConnection;
static uint32_t sEpoch;
static bool sReady, sBonded, sRestartAdvertising;
static bool sPairWindow;
static TickType_t sPairDeadline;
static unsigned sAttempts, sPasskey;
static bool sDisplayPasskey;
/* Worker-owned protocol/operation state. */
static struct br_ble_message sMessage;
static uint32_t sMessageEpoch;
static uint16_t sLastId;
static TickType_t sFragmentDeadline;
static TickType_t sAdvertiseRetry;

static bool pairing_open(void)
{
    bool open;
    taskENTER_CRITICAL();
#ifdef CONFIG_THREAD_BR_BLE_DEMO_PAIRING
    /* No console at a demo: always pairable; a new bond replaces the old one. */
    open = sReady;
#else
    open = sPairWindow && !sBonded && sAttempts < 3 && (int32_t)(sPairDeadline - xTaskGetTickCount()) > 0;
#endif
    taskEXIT_CRITICAL();
    return open;
}

static bool current(struct bt_conn *conn, uint32_t epoch)
{
    bool valid;
    taskENTER_CRITICAL();
    valid = conn == sConnection && epoch == sEpoch;
    taskEXIT_CRITICAL();
    return valid && bt_conn_get_security(conn) >= BT_SECURITY_L4;
}

struct bond_query { const bt_addr_le_t *peer; bool found; };
static void bonded_peer(const struct bt_bond_info *info, void *context)
{
    struct bond_query *query = context;
    if (!query->peer || bt_addr_le_eq(query->peer, &info->addr)) query->found = true;
}

static void connected(struct bt_conn *conn, uint8_t error)
{
    IPRO_LOGI(TAG, "Connected error=%u", error);
    if (error) return;
    /* Only one bond is allowed; unknown peers are rejected outside local setup. */
    struct bond_query query = {.peer = bt_conn_get_dst(conn)};
    bt_foreach_bond(BT_ID_DEFAULT, bonded_peer, &query);
    taskENTER_CRITICAL();
    bool occupied = sConnection != NULL;
    taskEXIT_CRITICAL();
    if (occupied || (!query.found && !pairing_open())) {
        bt_conn_disconnect(conn, BT_HCI_ERR_PAIRING_NOT_ALLOWED);
        return;
    }
    taskENTER_CRITICAL();
    sConnection = bt_conn_ref(conn);
    ++sEpoch;
    taskEXIT_CRITICAL();
    if (bt_conn_set_security(conn, BT_SECURITY_L4) != 0)
        bt_conn_disconnect(conn, BT_HCI_ERR_AUTH_FAIL);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    IPRO_LOGI(TAG, "Disconnected reason=0x%02x", reason);
    struct bt_conn *old = NULL;
    taskENTER_CRITICAL();
    if (conn == sConnection) {
        old = sConnection;
        sConnection = NULL;
        ++sEpoch;
        sDisplayPasskey = false;
        sPasskey = 0;
    }
    sRestartAdvertising = true;
    taskEXIT_CRITICAL();
    if (old) bt_conn_unref(old);
}

static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err error)
{
    IPRO_LOGI(TAG, "Security level=%u error=%u", (unsigned)level, (unsigned)error);
    if (error || level < BT_SECURITY_L4) bt_conn_disconnect(conn, BT_HCI_ERR_AUTH_FAIL);
}
static struct bt_conn_cb sConnectionCallbacks = {
    .connected = connected, .disconnected = disconnected, .security_changed = security_changed,
};

static enum bt_security_err pairing_accept(struct bt_conn *conn, const struct bt_conn_pairing_feat *features)
{
    (void)conn;
    if (!features || !pairing_open() || !(features->auth_req & 0x08) || features->max_enc_key_size != 16)
        return BT_SECURITY_ERR_PAIR_NOT_ALLOWED;
    taskENTER_CRITICAL();
    ++sAttempts;
    taskEXIT_CRITICAL();
    return BT_SECURITY_ERR_SUCCESS;
}
static void display_passkey(struct bt_conn *conn, unsigned passkey)
{
    (void)conn;
    taskENTER_CRITICAL();
    sPasskey = passkey;
    sDisplayPasskey = true;
    taskEXIT_CRITICAL();
}
static void auth_cancel(struct bt_conn *conn)
{
    (void)conn;
    taskENTER_CRITICAL();
    sPasskey = 0; sDisplayPasskey = false;
    taskEXIT_CRITICAL();
}
#ifdef CONFIG_THREAD_BR_BLE_DEMO_PAIRING
/* Demo only: the presenter is told the passkey instead of reading the console. */
static uint32_t demo_passkey(struct bt_conn *conn)
{
    (void)conn;
    return CONFIG_THREAD_BR_BLE_DEMO_PASSKEY;
}
#endif
static struct bt_conn_auth_cb sAuthCallbacks = {
    .pairing_accept = pairing_accept, .passkey_display = display_passkey, .cancel = auth_cancel,
#ifdef CONFIG_THREAD_BR_BLE_DEMO_PAIRING
    .app_passkey = demo_passkey,
#endif
};
static void pairing_complete(struct bt_conn *conn, bool bonded)
{
    auth_cancel(conn);
    taskENTER_CRITICAL();
    sBonded = bonded; sPairWindow = false;
    taskEXIT_CRITICAL();
    if (!bonded) bt_conn_disconnect(conn, BT_HCI_ERR_AUTH_FAIL);
    IPRO_LOGI(TAG, "Pairing complete, bonded=%u", (unsigned)bonded);
}
static struct bt_conn_auth_info_cb sAuthInfo = {.pairing_complete = pairing_complete};

static ssize_t receive(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                       const void *data, uint16_t length, uint16_t offset, uint8_t flags)
{
    (void)attr;
    if (offset || flags || length < BR_BLE_HEADER_SIZE || length > BR_BLE_MAX_FRAME)
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    if (bt_conn_get_security(conn) < BT_SECURITY_L4) return BT_GATT_ERR(BT_ATT_ERR_AUTHENTICATION);
    struct queued_frame *frame = pvPortMalloc(sizeof(*frame));
    if (!frame) return BT_GATT_ERR(BT_ATT_ERR_INSUFFICIENT_RESOURCES);
    memset(frame, 0, sizeof(*frame));
    frame->connection = bt_conn_ref(conn); frame->length = length;
    taskENTER_CRITICAL();
    frame->epoch = sEpoch;
    bool valid = sConnection == conn;
    taskEXIT_CRITICAL();
    memcpy(frame->bytes, data, length);
    bool queued = valid && sQueue && xQueueSend(sQueue, &frame, 0) == pdTRUE;
    if (!queued) {
        bt_conn_unref(frame->connection); br_wifi_profile_wipe(frame, sizeof(*frame)); vPortFree(frame);
        return BT_GATT_ERR(BT_ATT_ERR_INSUFFICIENT_RESOURCES);
    }
    return length;
}
static void subscription_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    (void)attr; (void)value;
}
BT_GATT_SERVICE_DEFINE(br_management,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_128(SERVICE_UUID)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_128(REQUEST_UUID), BT_GATT_CHRC_WRITE,
        BT_GATT_PERM_WRITE_AUTHEN, NULL, receive, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_128(RESPONSE_UUID), BT_GATT_CHRC_NOTIFY,
        BT_GATT_PERM_NONE, NULL, NULL, NULL),
    BT_GATT_CCC(subscription_changed, BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN)
);

static int advertise(void)
{
    const struct bt_data data[] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),
        BT_DATA_BYTES(BT_DATA_UUID128_ALL, SERVICE_UUID),
    };
    const struct bt_data scan[] = { BT_DATA(BT_DATA_NAME_COMPLETE, "IPRO6S Thread BR", 15) };
    return bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, data, ARRAY_SIZE(data), scan, ARRAY_SIZE(scan));
}

static int respond(struct queued_frame *request, uint8_t *payload, size_t length)
{
    uint8_t frame[BR_BLE_MAX_FRAME];
    size_t offset = 0;
    int error = 0;
    do {
        if (!current(request->connection, request->epoch)) { error = -ENOTCONN; break; }
        size_t size = br_ble_frame_encode(sMessage.opcode | 0x80, sMessage.id, payload, length, offset,
            bt_gatt_get_mtu(request->connection), frame, sizeof(frame));
        if (!size) { error = -EINVAL; break; }
        error = bt_gatt_notify(request->connection, &br_management.attrs[4], frame, size);
        if (error) break;
        offset += size - BR_BLE_HEADER_SIZE;
    } while (offset < length);
    br_wifi_profile_wipe(frame, sizeof(frame));
    return error;
}

static void execute(struct queued_frame *request)
{
    uint8_t response[BR_BLE_MAX_PAYLOAD] = {0};
    bool reboot = false;
    size_t length;
#ifdef CONFIG_THREAD_BR_LAN_MANAGEMENT
    if (sMessage.opcode == BR_BLE_LAN_CREDENTIALS && !sMessage.length) {
        /* The identity is erased by a committed factory reset; do not export it. */
        if (br_factory_reset_pending()) response[0] = BR_BLE_STATUS_BUSY;
        else response[0] = br_lan_credentials(response + 1) == 0 ? BR_BLE_STATUS_OK : BR_BLE_STATUS_NOT_READY;
        length = response[0] == BR_BLE_STATUS_OK ? 71 : 1;
    } else
#endif
        length = br_management_execute(sMessage.opcode, sMessage.payload, sMessage.length, response, &reboot);
    const int error = respond(request, response, length);
    if (error) {
        IPRO_LOGW(TAG, "Response failed epoch=%lu id=%u opcode=%u error=%d", (unsigned long)request->epoch, sMessage.id, sMessage.opcode, error);
        bt_conn_disconnect(request->connection, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    }
    br_wifi_profile_wipe(response, sizeof(response));
    br_ble_message_clear(&sMessage);
    /* A committed factory reset reboots even when its reply was lost. */
    if (reboot && (!error || br_factory_reset_pending())) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        br_ble_management_quiesce();
        extern void hal_reboot(void);
        hal_reboot();
    }
}

static void host_ready(int error)
{
    if (error) { IPRO_LOGE(TAG, "Host initialization failed: %d", error); return; }
    ipro_ble_ctlr_hcitl_host_ready();
    error = settings_load_subtree("bt");
    if (error) { IPRO_LOGE(TAG, "Bond storage failed: %d", error); return; }
    struct bond_query query = {0};
    bt_foreach_bond(BT_ID_DEFAULT, bonded_peer, &query);
    bt_set_bondable(true);
    taskENTER_CRITICAL();
    sBonded = query.found; sReady = true; sRestartAdvertising = true;
    taskEXIT_CRITICAL();
#ifdef CONFIG_THREAD_BR_BLE_DEMO_PAIRING
    IPRO_LOGI(TAG, "Management ready; demo pairing always open, bonded=%u", (unsigned)query.found);
#else
    IPRO_LOGI(TAG, "Management ready; use br_ble_open for first pairing");
#endif
}

#ifndef CONFIG_THREAD_BR_BLE_STACK_WORDS
#define CONFIG_THREAD_BR_BLE_STACK_WORDS 2048
#endif

static void worker(void *unused)
{
    (void)unused;
    struct br_wifi_boot_status boot;
    /* A saved WPA profile restores with a large temporary heap. First setup
     * (no profile) completes immediately; a saved profile finishes or fails
     * before the Host creates its RX/TX/workqueue stacks. */
    do { vTaskDelay(pdMS_TO_TICKS(100)); br_wifi_boot_status_get(&boot); }
    while (!boot.manager_ready || (boot.phase != BR_WIFI_BOOT_COMPLETE && boot.phase != BR_WIFI_BOOT_FAILED));
    IPRO_LOGI(TAG, "Starting Host after Wi-Fi boot phase=%u heap=%u", (unsigned)boot.phase, (unsigned)xPortGetFreeHeapSize());
    uint8_t mac[6];
    int error = ipro_wifi_get_station_mac(mac);
    if (!error) {
        /* BT address = Wi-Fi MAC + 1 (the factory numbering), used as a random
         * static identity: val[] is little-endian, so the +1 carries upward. */
        bt_addr_le_t identity = {.type = BT_ADDR_LE_RANDOM};
        unsigned carry = 1;
        for (unsigned i = 0; i < 6; ++i) {
            unsigned sum = mac[5 - i] + carry;
            identity.a.val[i] = (uint8_t)sum;
            carry = sum >> 8;
        }
        identity.a.val[5] |= 0xc0;
        error = bt_id_create(&identity, NULL);
        IPRO_LOGI(TAG, "BLE identity %02X:%02X:%02X:%02X:%02X:%02X (Wi-Fi MAC + 1) result=%d",
                  identity.a.val[5], identity.a.val[4], identity.a.val[3],
                  identity.a.val[2], identity.a.val[1], identity.a.val[0], error);
    }
    if (error == 0) error = bt_conn_auth_cb_register(&sAuthCallbacks);
    if (error == 0) error = bt_conn_auth_info_cb_register(&sAuthInfo);
    if (error == 0) { bt_conn_cb_register(&sConnectionCallbacks); error = hci_driver_ipro_ble_ctlr_init(); }
    if (error == 0) error = bt_enable(host_ready);
    if (error) { IPRO_LOGE(TAG, "Management start failed: %d", error); vTaskDelete(NULL); return; }
    struct queued_frame *frame;
    for (;;) {
        struct bt_conn *expired = NULL;
        taskENTER_CRITICAL();
        if (sPairWindow && (int32_t)(sPairDeadline - xTaskGetTickCount()) <= 0) {
            sPairWindow = false; sPasskey = 0; sDisplayPasskey = false;
            if (sConnection && !sBonded) expired = bt_conn_ref(sConnection);
        }
        bool advertise_now = sReady && sRestartAdvertising && !sConnection &&
            (int32_t)(xTaskGetTickCount() - sAdvertiseRetry) >= 0;
        if (advertise_now) sRestartAdvertising = false;
        bool display = sDisplayPasskey;
        uint32_t epoch = sEpoch;
        unsigned passkey = sPasskey;
        sDisplayPasskey = false; sPasskey = 0;
        taskEXIT_CRITICAL();
        if (expired) { bt_conn_disconnect(expired, BT_HCI_ERR_AUTH_FAIL); bt_conn_unref(expired); }
        if (epoch != sMessageEpoch) { br_ble_message_clear(&sMessage); sLastId = 0; sMessageEpoch = epoch; }
        if (advertise_now) {
            int result = advertise();
            IPRO_LOGI(TAG, "Advertising result=%d", result);
            if (result && result != -EALREADY) {
                taskENTER_CRITICAL();
                sRestartAdvertising = true;
                sAdvertiseRetry = xTaskGetTickCount() + pdMS_TO_TICKS(1000);
                taskEXIT_CRITICAL();
            }
        }
        if (display) {
            /* Local out-of-band display, deliberately excluded from structured telemetry.
             * Demo UART must be private; production needs a device display/label flow. */
            char line[48];
            int size = snprintf(line, sizeof(line), "BR BLE pairing code: %06u\r\n", passkey);
            /* SDK puts() reaches the console directly; fwrite() requires VFS. */
            if (size > 0 && (size_t)size < sizeof(line)) puts(line);
            br_wifi_profile_wipe(line, sizeof(line));
        }
        if (sMessage.active && (int32_t)(sFragmentDeadline - xTaskGetTickCount()) <= 0) br_ble_message_clear(&sMessage);
        if (xQueueReceive(sQueue, &frame, pdMS_TO_TICKS(100)) != pdTRUE) continue;
        if (current(frame->connection, frame->epoch)) {
            if (frame->epoch != sMessageEpoch) { br_ble_message_clear(&sMessage); sLastId = 0; sMessageEpoch = frame->epoch; }
            enum br_ble_parse_result result = br_ble_message_accept(&sMessage, frame->bytes, frame->length);
            sFragmentDeadline = xTaskGetTickCount() + pdMS_TO_TICKS(10000);
            if (result == BR_BLE_COMPLETE && sMessage.id > sLastId && sMessage.opcode < 0x80) {
                sLastId = sMessage.id; execute(frame);
            } else if (result != BR_BLE_MORE) {
                br_ble_message_clear(&sMessage); bt_conn_disconnect(frame->connection, BT_HCI_ERR_AUTH_FAIL);
            }
        } else br_ble_message_clear(&sMessage);
        bt_conn_unref(frame->connection);
        br_wifi_profile_wipe(frame, sizeof(*frame)); vPortFree(frame);
    }
}

int br_ble_management_ready(void)
{
    taskENTER_CRITICAL(); bool ready = sReady; taskEXIT_CRITICAL();
    return ready;
}
void br_ble_management_quiesce(void)
{
    /* Stop automatic advertising restarts before the Host resets the
     * controller. Warm reset changes MCU clocks; do not leave an active
     * BLE scheduler owning EM across that transition. Keep the next boot's
     * destructive EM self-test unchanged. */
    taskENTER_CRITICAL();
    bool ready = sReady;
    sReady = false; sRestartAdvertising = false; sPairWindow = false;
    taskEXIT_CRITICAL();
    if (ready) {
        int error = bt_disable();
        IPRO_LOGI(TAG, "Reboot Host shutdown result=%d", error);
    }
}
int br_ble_controller_prepare(void) { return btdm_init(NULL); }
int br_ble_management_start(void)
{
    if (sQueue || br_management_init() != 0) return -1;
    sQueue = xQueueCreate(4, sizeof(struct queued_frame *));
    if (!sQueue) return -1;
    if (xTaskCreate(worker, "br_ble", CONFIG_THREAD_BR_BLE_STACK_WORDS, NULL, 6, NULL) == pdPASS) return 0;
    vQueueDelete(sQueue); sQueue = NULL; return -1;
}

#ifdef CONFIG_SHELL
static void open_pairing(int argc, char **argv)
{
    (void)argv;
#ifdef CONFIG_THREAD_BR_BLE_DEMO_PAIRING
    /* A timed window would only close demo pairing early when it expires. */
    (void)argc;
    IPRO_LOGI(TAG, "Demo pairing is always open; no window needed");
    return;
#endif
    taskENTER_CRITICAL();
    bool allowed = argc == 1 && sReady && !sConnection && !sBonded;
    if (allowed) { sAttempts = 0; sPairWindow = true; sPairDeadline = xTaskGetTickCount() + pdMS_TO_TICKS(180000); }
    taskEXIT_CRITICAL();
    IPRO_LOGI(TAG, "First-pairing window %s", allowed ? "open for 180 seconds" : "unavailable (not ready, connected or already owned)");
}
SHELL_CMD_EXPORT_ALIAS(open_pairing, br_ble_open, Open local BLE first-pairing window);
#endif

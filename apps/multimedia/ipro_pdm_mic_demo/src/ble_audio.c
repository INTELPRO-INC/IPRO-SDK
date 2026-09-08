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
 * @file ble_audio.c
 * @brief BLE NUS service with audio command protocol
 *
 * Provides a Nordic UART Service (NUS) over BLE for phone control of the
 * PDM recorder. Commands arrive on the RX characteristic; responses and
 * audio data are sent as notifications on the TX characteristic.
 *
 * Audio transfer uses a dedicated FreeRTOS task that chunks PCM data
 * into MTU-sized BLE notifications.
 */

#include "ble_audio.h"
#include "ble_emprobe.h"
#include "ipro_log.h"
#include "shell.h"
#include <generated/autoconf.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <lc3.h>

#undef STRINGIFY
#undef likely
#undef unlikely
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/settings/settings.h>
#include <zephyr/kernel.h>

#include <hci_core.h>
#include <ble_lib_api.h>
#include <hci_driver_ipro_ble_ctlr.h>
#include <ipro_ble_controller.h>
#include "conn_internal.h"
#include "ipro_ble_ctlr_pds_snapshot.h"
#include "hal_lp.h"
#include "hal_pm.h"

#define TAG "BLE_AUDIO"

#define DEVICE_NAME         "IPRO-MIC"
#define ADV_INT_MIN         160     /* 100ms in 0.625ms units */
#define ADV_INT_MAX         320     /* 200ms */

/* Audio send task */
#define SEND_TASK_STACK     1024
#define SEND_TASK_PRIO      8

/* Stream send task */
#define STREAM_TASK_STACK   1536
#define STREAM_TASK_PRIO    9   /* higher than bulk send */
#define FRAME_SAMPLES       320
#define FRAME_BYTES         (FRAME_SAMPLES * sizeof(int16_t))  /* 640 */

/* LC3 encoder: 10ms frames, 16kHz, 40 bytes output = 32 kbps */
#define LC3_FRAME_US        10000
#define LC3_SAMPLE_RATE     16000
#define LC3_FRAME_SAMPLES   160   /* 10ms at 16kHz */
#define LC3_OCTETS          40    /* 32 kbps target */

static lc3_encoder_mem_16k_t s_enc_mem;
static lc3_encoder_t s_encoder;
static bool s_encoder_init;
static bool s_use_lc3 = true;  /* toggle via shell: pdm_lc3 on|off */

/* --- 2M 破音驗證儀表 ------------------------------------------------- */
/* 破音 = 接收端少了音框。三個可能掉幀點全部計數:
 *   producer_drop : DMIC ISR 塞 stream_queue 滿(編碼/BLE 消化不及)
 *   tx_retry_ok   : notify 首發失敗、2ms 後重試成功(壓力前兆)
 *   tx_drop       : 重試也失敗,音框確定遺失 → 每一次都是一聲破音   */
static struct {
    volatile uint32_t enc_frames;    /* LC3 編碼成功的 10ms 音框數 */
    volatile uint32_t tx_ok;
    volatile uint32_t tx_retry_ok;
    volatile uint32_t tx_drop;
    volatile uint32_t producer_drop;
    volatile uint32_t bulk_pkts;     /* get_audio 批次已送封包數 */
    volatile uint32_t bulk_retry;    /* get_audio notify 退避重送次數 */
    volatile uint8_t  tx_phy, rx_phy;   /* 最近一次 PHY 回報(0=未知) */
} s_st;

/* get_audio 批次傳輸測試旋鈕(shell 可調) */
static bool     s_bulk_seq_en = true;   /* 封包插入序號位元組(2M 亂序校正,
                                           APP v1.1+ 按序號定位寫入;pdm_seq off 可關) */
static uint8_t  s_bulk_seq;             /* 遞增序號(get_audio 開始時歸零) */
static uint16_t s_pace_bytes = 4096;    /* 每送 N bytes 讓出一次 */
static uint16_t s_pace_ms    = 2;       /* 讓出時長 ms */
static bool     s_auto_conn_param_update; /* default off: central/test controls CI */
static bool     s_auto_data_len_update = true; /* the phone cannot raise our TX octets; see DLE_REQUEST_DELAY_MS */
static bool     s_auto_phy_update;      /* default off: phone/app controls high-speed mode */

extern volatile uint32_t ipro_ble_ctlr_con_trace_reset;
extern volatile uint32_t ipro_ble_ctlr_con_trace_count;
extern volatile uint32_t ipro_ble_ctlr_con_trace_force;
extern volatile uint32_t ipro_ble_ctlr_con_trace_freeze;
extern volatile uint16_t ipro_ble_ctlr_con_trace_evt[256];
extern volatile uint8_t ipro_ble_ctlr_con_trace_ch[256];
extern volatile uint8_t ipro_ble_ctlr_con_trace_expected_ch[256];
extern volatile uint8_t ipro_ble_ctlr_con_trace_hop_ch[256];
extern volatile uint8_t ipro_ble_ctlr_con_trace_result[256];
extern volatile uint8_t ipro_ble_ctlr_con_trace_rxdesc_idx[256];
extern volatile uint16_t ipro_ble_ctlr_con_trace_rxcntl[256];
extern volatile uint16_t ipro_ble_ctlr_con_trace_rxstat[256];
extern volatile uint16_t ipro_ble_ctlr_con_trace_rxphce[256];
extern volatile uint16_t ipro_ble_ctlr_con_trace_rxchass[256];
extern volatile uint16_t ipro_ble_ctlr_con_trace_rxclkn0[256];
extern volatile uint16_t ipro_ble_ctlr_con_trace_rxclkn1[256];
extern volatile uint16_t ipro_ble_ctlr_con_trace_rxfcnt[256];
extern volatile uint32_t ipro_ble_ctlr_con_trace_target[256];
extern volatile uint32_t ipro_ble_ctlr_con_trace_start[256];
extern volatile uint16_t ipro_ble_ctlr_con_trace_txrx[256];
extern volatile uint8_t ipro_ble_ctlr_con_trace_role[256];
extern volatile uint32_t ipro_ble_ctlr_llcp_tx_try_count[];
extern volatile uint32_t ipro_ble_ctlr_llcp_tx_ack_count[];
extern volatile uint32_t ipro_ble_ctlr_llcp_rx_count[];
extern volatile uint8_t ipro_ble_ctlr_llcp_last_tx_opcode[];
extern volatile uint8_t ipro_ble_ctlr_llcp_last_tx_len[];
extern volatile uint8_t ipro_ble_ctlr_llcp_last_tx_status[];
extern volatile uint8_t ipro_ble_ctlr_llcp_last_ack_opcode[];
extern volatile uint8_t ipro_ble_ctlr_llcp_last_ack_len[];
extern volatile uint8_t ipro_ble_ctlr_llcp_last_rx_opcode[];
extern volatile uint8_t ipro_ble_ctlr_llcp_last_rx_len[];
extern volatile uint32_t ipro_ble_ctlr_lld_rx_nonempty_desc_count;
extern volatile uint32_t ipro_ble_ctlr_lld_rx_nonempty_crcok_count;
extern volatile uint32_t ipro_ble_ctlr_lld_rx_nonempty_badstat_count;
extern volatile uint32_t ipro_ble_ctlr_lld_rx_clean_pdu_count;
extern volatile uint32_t ipro_ble_ctlr_lld_rx_control_pdu_count;
extern volatile uint32_t ipro_ble_ctlr_lld_rx_acl_pdu_count;
extern volatile uint32_t ipro_ble_ctlr_lld_rx_rfu_pdu_count;
extern volatile uint32_t ipro_ble_ctlr_lld_rx_ring_wr;
extern volatile uint32_t ipro_ble_ctlr_lld_rx_last_evt;
extern volatile uint16_t ipro_ble_ctlr_lld_rx_last_rxstat;
extern volatile uint16_t ipro_ble_ctlr_lld_rx_last_rxphce;
extern volatile uint16_t ipro_ble_ctlr_lld_rx_last_dataptr;
extern volatile uint8_t ipro_ble_ctlr_lld_rx_last_link;
extern volatile uint8_t ipro_ble_ctlr_lld_rx_last_ch;
extern volatile uint8_t ipro_ble_ctlr_lld_rx_last_llid;
extern volatile uint8_t ipro_ble_ctlr_lld_rx_last_len;
extern volatile uint8_t ipro_ble_ctlr_lld_rx_last_opcode;
extern volatile uint16_t ipro_ble_ctlr_lld_rx_ring_evt[32];
extern volatile uint16_t ipro_ble_ctlr_lld_rx_ring_rxstat[32];
extern volatile uint16_t ipro_ble_ctlr_lld_rx_ring_rxphce[32];
extern volatile uint16_t ipro_ble_ctlr_lld_rx_ring_dataptr[32];
extern volatile uint16_t ipro_ble_ctlr_lld_rx_ring_data0[32];
extern volatile uint16_t ipro_ble_ctlr_lld_rx_ring_data1[32];
extern volatile uint8_t ipro_ble_ctlr_lld_rx_ring_link[32];
extern volatile uint8_t ipro_ble_ctlr_lld_rx_ring_ch[32];
extern volatile uint8_t ipro_ble_ctlr_lld_rx_ring_llid[32];
extern volatile uint8_t ipro_ble_ctlr_lld_rx_ring_len[32];
extern volatile uint8_t ipro_ble_ctlr_lld_rx_ring_opcode[32];

void ble_audio_stats_producer_drop(void)
{
    s_st.producer_drop++;
}

/* --- NUS UUIDs (same as ipro_ble_transparent) --- */
static struct bt_uuid_128 nus_svc_uuid = BT_UUID_INIT_128(
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
    0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E);

static struct bt_uuid_128 nus_rx_uuid = BT_UUID_INIT_128(
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
    0x93, 0xF3, 0xA3, 0xB5, 0x02, 0x00, 0x40, 0x6E);

static struct bt_uuid_128 nus_tx_uuid = BT_UUID_INIT_128(
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
    0x93, 0xF3, 0xA3, 0xB5, 0x03, 0x00, 0x40, 0x6E);

/* --- State --- */
static struct {
    struct bt_conn *conn;
    bool tx_notify_enabled;
    bool pds_requested;
    bool peer_was_bonded;       /* had a bond on file when it connected */
    const ble_audio_recorder_t *recorder;
} s_ble;

/* Whatever the link is actually running on, for ble_stat and the log. The
 * phone drives all three of these; the device only reports them. */
static struct {
    uint16_t interval_1p25ms;
    uint16_t latency;
    uint16_t timeout_10ms;
    uint16_t tx_max_len, rx_max_len;
} s_link;

/* Set once bt_enable() has been accepted. Deliberately outside s_ble, which
 * ble_audio_init() clears: the sleep path needs to know the stack was up even
 * while s_ble itself is being reset. */
static volatile bool s_ble_started;

bool ipro_ble_ctlr_app_pds_tickless_allowed(void)
{
    if (!s_ble.pds_requested) {
        return false;
    }
    if (!s_ble.conn) {
        return false;
    }
    if (s_ble.recorder) {
        if (s_ble.recorder->is_recording && s_ble.recorder->is_recording()) {
            return false;
        }
        if (s_ble.recorder->is_streaming && s_ble.recorder->is_streaming()) {
            return false;
        }
    }
    return true;
}

/* Audio send task */
static TaskHandle_t s_send_task_handle;
static SemaphoreHandle_t s_send_sem;

/* --- Forward declarations --- */
static int start_advertising(void);
static void adv_restart_work_handler(struct k_work *work);
static struct k_work_delayable s_adv_restart_work;
static uint8_t s_adv_restart_tries;
static void sec_req_work_handler(struct k_work *work);
static struct k_work_delayable s_sec_req_work;

/* How long an unbonded central gets to start pairing before we ask. Long
 * enough for the phone app's createBond() to land first every time; short
 * enough that a generic tool with no app still gets prompted. */
#define SEC_REQ_GRACE_MS    3000

/*
 * Deliberately NOT directed advertising.
 *
 * It was tried: on a bond, advertise ADV_DIRECT_IND at the phone (with
 * DIR_ADDR_RPA so the target is the phone's resolvable address) for 30 s
 * before opening up. On hardware the Pixel's pending background connection
 * ignored it for the whole 30 s and then connected within one second of
 * ordinary undirected advertising starting. Whatever the reason - the phone
 * not resolving TargetA for background connects, or the RPA the controller
 * generated - the measured effect was a 30 s delay on every wake, for a
 * reconnect that undirected advertising delivers in a second.
 *
 * The phone's autoConnect completes just as well on undirected advertising:
 * its controller matches AdvA against its accept list, and our AdvA is a
 * public identity address it already knows.
 */

/* DLE has to be asked for from this side. The phone's LL_LENGTH_REQ carries
 * the phone's limits and can raise only what the phone sends; our TX octets
 * stay at the controller default of 27 until we ask. With
 * CONFIG_BT_USER_DATA_LEN_UPDATE the host never asks on its own. Measured:
 * "tx 27 / rx 251" and every 244-byte notification going out as ten LL
 * packets. Deferred a little so it does not land on top of the phone's own
 * length and PHY procedures in the first few hundred milliseconds. */
#define DLE_REQUEST_DELAY_MS 300
static void dle_request_work_handler(struct k_work *work);
static struct k_work_delayable s_dle_request_work;
static void handle_command(const uint8_t *data, uint16_t len);
extern void ipro_ble_ctlr_tickless_stats_reset(void);
extern volatile uint32_t ipro_ble_ctlr_tickless_calls;
extern volatile uint32_t ipro_ble_ctlr_tickless_selected_ticks;
extern volatile uint32_t ipro_ble_ctlr_tickless_pds_grant;
extern volatile uint32_t ipro_ble_ctlr_tickless_pds_grant_reject;
extern volatile uint32_t ipro_ble_ctlr_tickless_pds_grant_sleep_us;
extern volatile uint32_t ipro_ble_ctlr_tickless_window_grant;
extern volatile uint32_t ipro_ble_ctlr_tickless_window_reject_no_budget;
extern volatile uint32_t ipro_ble_ctlr_tickless_window_reject_expired;
extern volatile uint32_t ipro_ble_ctlr_tickless_window_reject_repeat;
extern volatile uint32_t ipro_ble_ctlr_tickless_window_reject_timing;
extern volatile uint32_t ipro_ble_ctlr_tickless_window_reject_policy;
extern volatile uint32_t ipro_ble_ctlr_tickless_deferred_grant;
extern volatile uint32_t ipro_ble_ctlr_tickless_deferred_reject_policy;
extern volatile uint32_t ipro_ble_ctlr_tickless_deferred_reject_timing;
extern volatile uint32_t ipro_ble_ctlr_pds_pending_seen_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_ready_seen_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_gate_mask;
extern volatile uint32_t ipro_ble_ctlr_pds_deadline_reject_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_gate_reject_hci_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_gate_reject_guard_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_gate_reject_recovery_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_gate_reject_native_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_gate_reject_rw_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_last_slack_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_probe_run_calls;
extern volatile uint32_t ipro_ble_ctlr_pds_probe_consume_count;
extern volatile bool ipro_ble_ctlr_hcitl_host_is_ready;
extern volatile uint32_t ipro_ble_ctlr_hcitl_h2c_bytes;
extern volatile uint32_t ipro_ble_ctlr_hcitl_c2h_bytes;
extern volatile uint32_t ipro_ble_ctlr_hcitl_h2c_deferred;
extern volatile uint32_t ipro_ble_ctlr_hcitl_c2h_deferred;
extern volatile uint32_t ipro_ble_ctlr_hcitl_gate_now_tick;
extern volatile uint32_t ipro_ble_ctlr_hcitl_gate_awake_until_tick;
extern volatile uint32_t ipro_ble_ctlr_hcitl_gate_reject_not_ready;
extern volatile uint32_t ipro_ble_ctlr_hcitl_gate_reject_guard;

/* --- GATT callbacks --- */
static ssize_t nus_rx_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                             const void *buf, uint16_t len, uint16_t offset,
                             uint8_t flags)
{
    if (len == 0)
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);

    handle_command((const uint8_t *)buf, len);
    return len;
}

static void nus_tx_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    s_ble.tx_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(TAG, "TX notify %s", s_ble.tx_notify_enabled ? "enabled" : "disabled");
}

/* --- NUS Service definition --- */
BT_GATT_SERVICE_DEFINE(nus_svc,
    BT_GATT_PRIMARY_SERVICE(&nus_svc_uuid),
    /* TX characteristic (device → phone, notify) */
    BT_GATT_CHARACTERISTIC(&nus_tx_uuid.uuid,
                           BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_NONE,
                           NULL, NULL, NULL),
    BT_GATT_CCC(nus_tx_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    /* RX characteristic (phone → device, write) */
    BT_GATT_CHARACTERISTIC(&nus_rx_uuid.uuid,
                           BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE,
                           NULL, nus_rx_write, NULL),
);

/* --- Advertising data --- */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, sizeof(DEVICE_NAME) - 1),
};

static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL,
        0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
        0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E),
};

/* --- Send helper (chunked notification) --- */
static int ble_send(const uint8_t *data, uint16_t len)
{
    if (!s_ble.conn || !s_ble.tx_notify_enabled)
        return -1;

    uint16_t mtu = bt_gatt_get_mtu(s_ble.conn);
    uint16_t max_payload = (mtu > 3) ? (mtu - 3) : 20;
    uint16_t offset = 0;

    while (offset < len) {
        uint16_t chunk = len - offset;
        if (chunk > max_payload)
            chunk = max_payload;

        int ret = bt_gatt_notify(s_ble.conn, &attr_nus_svc[1], data + offset, chunk);
        if (ret)
            return ret;
        offset += chunk;
    }
    return 0;
}

/* --- Send typed response --- */
static void send_status_response(void)
{
    uint8_t buf[6];
    buf[0] = BLE_RSP_STATUS;

    if (s_ble.recorder) {
        if (s_ble.recorder->is_streaming && s_ble.recorder->is_streaming())
            buf[1] = BLE_STATUS_STREAMING;
        else if (s_ble.recorder->is_recording())
            buf[1] = BLE_STATUS_RECORDING;
        else
            buf[1] = BLE_STATUS_IDLE;
    } else {
        buf[1] = BLE_STATUS_IDLE;
    }

    /* Recorded bytes (4 bytes, little-endian) */
    uint32_t bytes = s_ble.recorder ? s_ble.recorder->get_bytes() : 0;
    memcpy(&buf[2], &bytes, 4);

    ble_send(buf, sizeof(buf));
}

static void send_error(uint8_t code)
{
    uint8_t buf[2] = { BLE_RSP_ERROR, code };
    ble_send(buf, sizeof(buf));
}

/* --- 壓力放大器:bulk 期間對 PSRAM 高強度讀寫,擠出讀取層時序邊際 ------- */
#define STRESS_BUF_SZ 32768
static uint8_t s_stress_buf[2][STRESS_BUF_SZ];   /* 落在 PSRAM bss */
static volatile bool s_stress_en;
static TaskHandle_t s_stress_task;

static void mem_stress_task(void *arg)
{
    (void)arg;
    uint32_t sum = 0;
    for (;;) {
        if (!s_stress_en) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        memcpy(s_stress_buf[1], s_stress_buf[0], STRESS_BUF_SZ);
        const uint32_t *p = (const uint32_t *)s_stress_buf[1];
        for (int i = 0; i < STRESS_BUF_SZ / 4; i += 8)
            sum += p[i];
        s_stress_buf[0][sum & (STRESS_BUF_SZ - 1)] = (uint8_t)sum;
        taskYIELD();   /* 讓出但不休眠:最大化 PSRAM 匯流排競爭 */
    }
}

static int cmd_mem_stress(int argc, char **argv)
{
    if (argc > 1) {
        s_stress_en = (strcmp(argv[1], "on") == 0);
        if (s_stress_en && !s_stress_task)
            xTaskCreate(mem_stress_task, "mstress", 512, NULL,
                        tskIDLE_PRIORITY + 1, &s_stress_task);
    }
    IPRO_LOGI(TAG, "mem_stress: %s (PSRAM 讀寫競爭壓力)", s_stress_en ? "ON" : "off");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_mem_stress, mem_stress, PSRAM bus stress on|off);

/* --- 探針 v3:bulk 來源讀取自我驗證 ------------------------------------ */
#define SRC_CRC_MAX_CHUNKS 4096   /* 30s/242B 上限 3967 */
static uint16_t s_src_crc[SRC_CRC_MAX_CHUNKS];
static uint32_t s_src_crc_chunks;
static uint16_t s_src_chunk_sz;
volatile uint32_t g_src_crc_fail;    /* 組包時讀到與真值表不符 */
volatile uint32_t g_src_crc_healed;  /* 重讀一次後恢復一致(=瞬時讀取毛病) */
volatile uint32_t g_src_crc_1st_pkt; /* 首次失敗的封包號 */

static uint16_t src_crc16(const uint8_t *p, uint32_t len)
{
    uint16_t h = 0;
    for (uint32_t i = 0; i < len; i++)
        h = (uint16_t)(h * 31u + p[i]);
    return h;
}

static void src_crc_build(const uint8_t *src, uint32_t total, uint16_t chunk)
{
    s_src_chunk_sz = chunk;
    s_src_crc_chunks = (total + chunk - 1) / chunk;
    if (s_src_crc_chunks > SRC_CRC_MAX_CHUNKS)
        s_src_crc_chunks = SRC_CRC_MAX_CHUNKS;
    for (uint32_t k = 0; k < s_src_crc_chunks; k++) {
        uint32_t off = k * chunk;
        uint32_t len = (off + chunk <= total) ? chunk : (total - off);
        s_src_crc[k] = src_crc16(src + off, len);
    }
}

/* 回傳 0=一致;1=不符但重讀後恢復;2=持續不符 */
static int src_crc_verify(const uint8_t *pkt_data, const uint8_t *src,
                          uint32_t sent, uint32_t len)
{
    uint32_t k = sent / s_src_chunk_sz;
    if (k >= s_src_crc_chunks)
        return 0;
    if (src_crc16(pkt_data, len) == s_src_crc[k])
        return 0;
    if (g_src_crc_fail == 0)
        g_src_crc_1st_pkt = k;
    g_src_crc_fail++;
    /* 直接重讀來源再驗一次:恢復=瞬時讀取毛病(且此包可重組後重send) */
    if (src_crc16(src + sent, len) == s_src_crc[k]) {
        g_src_crc_healed++;
        return 1;
    }
    return 2;
}

/* --- Audio send task --- */
static void audio_send_task(void *arg)
{
    (void)arg;

    for (;;) {
        /* Wait for GET_AUDIO trigger */
        xSemaphoreTake(s_send_sem, portMAX_DELAY);

        if (!s_ble.conn || !s_ble.tx_notify_enabled || !s_ble.recorder) {
            send_error(0x01); /* not ready */
            continue;
        }

        const int16_t *pcm = s_ble.recorder->get_buf();
        uint32_t total_bytes = s_ble.recorder->get_bytes();

        if (!pcm || total_bytes == 0) {
            send_error(0x02); /* no data */
            continue;
        }

        /* Send audio header: sample rate (16-bit LE) + total bytes (32-bit LE) */
        uint8_t hdr[7];
        hdr[0] = BLE_RSP_AUDIO_HDR;
        uint16_t sr = 16000;
        memcpy(&hdr[1], &sr, 2);
        memcpy(&hdr[3], &total_bytes, 4);
        ble_send(hdr, sizeof(hdr));

        /* Chunk audio data via notifications.
         * 註：本迴圈是無損的(失敗退避重送同一段,成功才前進),且 GATT
         * notification 在鏈路層可靠有序 — 若手機端收到的總量 < header
         * 宣告的 total_bytes,遺失只可能發生在手機協定棧/APP 消化不及。 */
        uint16_t mtu = bt_gatt_get_mtu(s_ble.conn);
        uint16_t max_payload = (mtu > 3) ? (mtu - 3) : 20;
        /* Reserve 1 byte response ID (+1 byte seq when pdm_seq on) */
        uint16_t hdr_len = s_bulk_seq_en ? 2 : 1;
        uint16_t data_chunk = max_payload - hdr_len;
        const uint8_t *src = (const uint8_t *)pcm;
        uint32_t sent = 0;

        s_bulk_seq = 0;
        s_st.bulk_pkts = 0;
        s_st.bulk_retry = 0;

        /* 探針 v3:來源讀取自我驗證 — bulk 開始時對每個 chunk 建 CRC 真值表,
         * 迴圈內每包組完覆驗。抓「PSRAM 讀出 ≠ buffer 實際內容」的讀取層
         * 毛病(快取繞不過:表建立與覆驗是兩次獨立讀取路徑+失敗即重讀)。 */
        src_crc_build(src, total_bytes, data_chunk);
        if (s_bulk_seq_en)
            ble_emprobe_expect(src, total_bytes, data_chunk, BLE_RSP_AUDIO_DATA);
        else
            ble_emprobe_expect(NULL, 0, 0, 0);

        IPRO_LOGI(TAG, "Sending %lu bytes audio (MTU=%u, chunk=%u, seq=%s, pace=%u/%ums)",
                  (unsigned long)total_bytes, mtu, data_chunk,
                  s_bulk_seq_en ? "on" : "off",
                  s_pace_bytes, s_pace_ms);

        extern volatile uint32_t ipro_ble_ctlr_hcitl_h2c_deferred;
        TickType_t t_start = xTaskGetTickCount();
        uint32_t h2c_stalls_before = ipro_ble_ctlr_hcitl_h2c_deferred;
        uint32_t fast_bytes = 0;       /* bytes sent once the interval was short */
        TickType_t t_fast = 0;          /* when it became short */

        while (sent < total_bytes) {
            if (!s_ble.conn || !s_ble.tx_notify_enabled)
                break;

            uint16_t remain = (total_bytes - sent > data_chunk)
                              ? data_chunk : (uint16_t)(total_bytes - sent);

            uint8_t pkt[252]; /* max BLE payload + hdr */
            pkt[0] = BLE_RSP_AUDIO_DATA;
            if (s_bulk_seq_en)
                pkt[1] = s_bulk_seq;   /* 遞增序號:接收端抓缺號=丟包鐵證 */
            memcpy(&pkt[hdr_len], src + sent, remain);

            /* 探針 v3:覆驗這次組包讀到的資料;瞬時讀取毛病則重組一次 */
            if (src_crc_verify(&pkt[hdr_len], src, sent, remain) == 1)
                memcpy(&pkt[hdr_len], src + sent, remain);

            int ret = bt_gatt_notify(s_ble.conn, &attr_nus_svc[1],
                                     pkt, remain + hdr_len);
            if (ret) {
                /* BLE buffer full — back off and retry (同一段,無損).
                 * One tick, not five: at an 11.25 ms interval a 5 ms wait
                 * could idle the pool for half an event. */
                s_st.bulk_retry++;
                vTaskDelay(1);
                continue;
            }
            sent += remain;
            s_st.bulk_pkts++;
            if (s_bulk_seq_en)
                s_bulk_seq++;

            /* Yield periodically — 節流可用 ble_pace 調(0=不節流) */
            if (s_pace_bytes && (sent % s_pace_bytes) < data_chunk)
                vTaskDelay(pdMS_TO_TICKS(s_pace_ms));

            /* Split the accounting at the interval switch: the phone grants
             * the short interval a second or so into the transfer, and a
             * figure that averages the slow start in says nothing about
             * the link the bulk actually ran on. */
            if (t_fast == 0 && s_link.interval_1p25ms != 0 &&
                s_link.interval_1p25ms <= 24) {           /* <= 30 ms */
                t_fast = xTaskGetTickCount();
                fast_bytes = sent;
            }
        }

        /* Send done */
        uint8_t done[5];
        done[0] = BLE_RSP_AUDIO_DONE;
        memcpy(&done[1], &sent, 4);
        ble_send(done, sizeof(done));

        {
            TickType_t t_end = xTaskGetTickCount();
            uint32_t ms = (uint32_t)(t_end - t_start);
            uint32_t stalls = ipro_ble_ctlr_hcitl_h2c_deferred - h2c_stalls_before;

            IPRO_LOGI(TAG, "Audio send complete: %lu bytes in %lu ms = %lu kB/s "
                      "(%lu pkts, notify refused %lu, H2C stalls %lu)",
                      (unsigned long)sent, (unsigned long)ms,
                      (unsigned long)(ms ? (sent / ms) : 0),
                      (unsigned long)s_st.bulk_pkts,
                      (unsigned long)s_st.bulk_retry, (unsigned long)stalls);
            if (t_fast != 0 && t_end > t_fast) {
                uint32_t fms = (uint32_t)(t_end - t_fast);
                uint32_t fb = sent - fast_bytes;

                /* Packets per connection event on the short interval: the
                 * number that says whether the phone's controller or this
                 * side's pipeline is the ceiling. Ten is the 2M/251 limit at
                 * 15 ms. */
                uint32_t evt_us = (uint32_t)s_link.interval_1p25ms * 1250U;
                uint32_t events = evt_us ? (fms * 1000U) / evt_us : 0;
                uint32_t pkts_fast = fb / data_chunk;

                IPRO_LOGI(TAG, "  on %u.%02u ms interval: %lu bytes in %lu ms = %lu kB/s, "
                          "%lu.%lu pkts/event",
                          (s_link.interval_1p25ms * 125U) / 100U,
                          (s_link.interval_1p25ms * 125U) % 100U,
                          (unsigned long)fb, (unsigned long)fms,
                          (unsigned long)(fms ? fb / fms : 0),
                          (unsigned long)(events ? pkts_fast / events : 0),
                          (unsigned long)(events ? (pkts_fast * 10U / events) % 10U : 0));
            }
        }
    }
}

/* --- Stream send task: LC3-encodes and forwards live PDM frames over BLE --- */
static void stream_send_task(void *arg)
{
    (void)arg;
    int16_t *frame;

    /* LC3 packet: 1 byte header + 40 bytes LC3 = 41 bytes — fits in one BLE notif */
    uint8_t lc3_pkt[1 + LC3_OCTETS];
    lc3_pkt[0] = BLE_RSP_AUDIO_LC3;

    /* Fallback raw PCM packet */
    uint8_t raw_pkt[1 + 243];

    /* Init LC3 encoder (once, static memory) */
    if (!s_encoder_init) {
        s_encoder = lc3_setup_encoder(LC3_FRAME_US, LC3_SAMPLE_RATE,
                                      0, &s_enc_mem);
        if (s_encoder) {
            s_encoder_init = true;
            IPRO_LOGI(TAG, "LC3 encoder ready: %dHz, %dus, %d bytes/frame",
                      LC3_SAMPLE_RATE, LC3_FRAME_US, LC3_OCTETS);
        } else {
            IPRO_LOGW(TAG, "LC3 encoder setup failed — raw PCM fallback");
        }
    }

    for (;;) {
        QueueHandle_t q = s_ble.recorder ?
            (QueueHandle_t)s_ble.recorder->stream_queue : NULL;
        if (!q) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        if (xQueueReceive(q, &frame, portMAX_DELAY) != pdTRUE)
            continue;

        if (!s_ble.conn || !s_ble.tx_notify_enabled)
            continue;

        /* Ring buffer is in PSRAM (cached). ISR wrote via CPU cache,
         * so data is already in cache — do NOT invalidate. */

        if (s_use_lc3 && s_encoder_init && s_encoder) {
            /* LC3 mode: encode 320 samples as 2 × 160-sample (10ms) LC3 frames.
             * Each 41-byte packet fits in a single BLE notification. */
            for (int half = 0; half < 2; half++) {
                if (lc3_encode(s_encoder, LC3_PCM_FORMAT_S16,
                               frame + half * LC3_FRAME_SAMPLES, 1,
                               LC3_OCTETS, &lc3_pkt[1]) != 0) {
                    continue;
                }
                s_st.enc_frames++;

                int ret = bt_gatt_notify(s_ble.conn, &attr_nus_svc[1],
                                         lc3_pkt, 1 + LC3_OCTETS);
                if (ret == 0) {
                    s_st.tx_ok++;
                    continue;
                }
                vTaskDelay(pdMS_TO_TICKS(2));
                ret = bt_gatt_notify(s_ble.conn, &attr_nus_svc[1],
                                     lc3_pkt, 1 + LC3_OCTETS);
                if (ret == 0) {
                    s_st.tx_retry_ok++;
                    continue;
                }
                s_st.tx_drop++;
                if ((s_st.tx_drop & 0x1F) == 1) {   /* rate-limited */
                    IPRO_LOGW(TAG, "audio frame DROP #%lu (err %d) <- glitch",
                              (unsigned long)s_st.tx_drop, ret);
                }
            }
        } else {
            /* Fallback: send raw PCM (chunked per notification) */
            const uint8_t *src = (const uint8_t *)frame;
            uint16_t mtu = bt_gatt_get_mtu(s_ble.conn);
            uint16_t max_payload = (mtu > 3) ? (mtu - 3) : 20;
            uint16_t data_per_notif = (max_payload - 1) & ~1;
            uint16_t offset = 0;

            while (offset < FRAME_BYTES) {
                if (!s_ble.conn || !s_ble.tx_notify_enabled)
                    break;
                uint16_t remain = FRAME_BYTES - offset;
                uint16_t chunk = (remain < data_per_notif) ? remain : data_per_notif;

                raw_pkt[0] = BLE_RSP_AUDIO_DATA;
                memcpy(&raw_pkt[1], src + offset, chunk);

                int ret = bt_gatt_notify(s_ble.conn, &attr_nus_svc[1],
                                         raw_pkt, chunk + 1);
                if (ret) {
                    vTaskDelay(pdMS_TO_TICKS(2));
                    continue;
                }
                offset += chunk;
            }
        }
    }
}

/* --- Command handler --- */
static void handle_command(const uint8_t *data, uint16_t len)
{
    if (len == 0)
        return;

    uint8_t cmd = data[0];

    switch (cmd) {
    case BLE_CMD_START_REC: {
        int seconds = 5;
        if (len >= 2)
            seconds = data[1];
        if (seconds < 1) seconds = 1;
        if (seconds > 30) seconds = 30;

        IPRO_LOGI(TAG, "CMD: START_REC %d sec", seconds);
        if (s_ble.recorder && s_ble.recorder->start_rec)
            s_ble.recorder->start_rec(seconds);

        send_status_response();
        break;
    }
    case BLE_CMD_STOP_REC:
        IPRO_LOGI(TAG, "CMD: STOP_REC");
        if (s_ble.recorder && s_ble.recorder->stop_rec)
            s_ble.recorder->stop_rec();
        send_status_response();
        break;

    case BLE_CMD_GET_AUDIO:
        IPRO_LOGI(TAG, "CMD: GET_AUDIO");
        xSemaphoreGive(s_send_sem);
        break;

    case BLE_CMD_GET_STATUS:
        send_status_response();
        break;

    case BLE_CMD_SET_GAIN:
        if (len >= 2) {
            int8_t gain = (int8_t)data[1];
            IPRO_LOGI(TAG, "CMD: SET_GAIN %d dB", gain);
            if (s_ble.recorder && s_ble.recorder->set_gain)
                s_ble.recorder->set_gain(gain);
        }
        send_status_response();
        break;

    case BLE_CMD_START_STREAM:
        IPRO_LOGI(TAG, "CMD: START_STREAM");
        if (s_ble.recorder && s_ble.recorder->start_stream)
            s_ble.recorder->start_stream();
        send_status_response();
        break;

    case BLE_CMD_STOP_STREAM:
        IPRO_LOGI(TAG, "CMD: STOP_STREAM");
        if (s_ble.recorder && s_ble.recorder->stop_stream)
            s_ble.recorder->stop_stream();
        send_status_response();
        break;

    default:
        IPRO_LOGW(TAG, "Unknown cmd 0x%02x", cmd);
        send_error(0xFF);
        break;
    }
}

/* --- Connection callbacks --- */
static void connected_cb(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        IPRO_LOGE(TAG, "Connection failed (err 0x%02x)", err);
        return;
    }

    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));
    IPRO_LOGI(TAG, "Connected: %s", addr_str);

    s_ble.conn = bt_conn_ref(conn);
    memset(&s_link, 0, sizeof(s_link));

    /* Sampled here, before any SMP traffic. Asked later - in
     * security_changed_cb or pairing_complete - the answer is useless: a
     * pairing that just completed has written a bond, so "a bond exists" is
     * true either way and the two cases become indistinguishable. */
    s_ble.peer_was_bonded = bt_le_bond_exists(BT_ID_DEFAULT, bt_conn_get_dst(conn));

    /*
     * Ask for encryption - but who asks first matters on Android.
     *
     * Bonded peer: request now. It is answered from the stored LTK with no UI
     * on either side, and this is the whole reconnect-after-PDS path.
     *
     * Unbonded peer: wait. A Security Request sent the instant the link comes
     * up races the phone app's own createBond(), and whichever side Android
     * sees first decides how the pairing is presented: app-initiated pairing
     * gets a foreground dialog, remote-initiated pairing gets a notification
     * in the shade that nobody notices. The first attempt on hardware sat in
     * DHCHECK_WAIT (SMP flags 0x1a028) for the full 30 s SMP timeout waiting
     * for a confirmation the user was never shown, then the phone gave up.
     * The retry, where the app happened to win the race, paired in 11 s.
     *
     * So the request for a new peer is deferred. If the central has started
     * pairing by then, bt_conn_set_security() returns -EBUSY and nothing
     * changes; if it has not (a generic tool, no app), the prompt still comes.
     *
     * Failing here is never fatal - the NUS characteristics carry no security
     * requirement of their own - so errors are logged, not acted on.
     */
    if (s_ble.peer_was_bonded) {
        int sec_err = bt_conn_set_security(conn, BT_SECURITY_L2);
        if (sec_err) {
            IPRO_LOGW(TAG, "bt_conn_set_security: %d", sec_err);
        }
    } else {
        (void)k_work_reschedule(&s_sec_req_work, K_MSEC(SEC_REQ_GRACE_MS));
    }

    ipro_ble_ctlr_tickless_stats_reset();
    ipro_ble_ctlr_pds_auto_connected(s_ble.pds_requested);
    ipro_ble_ctlr_tickless_connection_active_set(s_ble.pds_requested);

#if defined(CONFIG_BT_USER_DATA_LEN_UPDATE)
    if (s_auto_data_len_update) {
        (void)k_work_reschedule(&s_dle_request_work, K_MSEC(DLE_REQUEST_DELAY_MS));
    }
#endif

#if defined(CONFIG_BT_USER_PHY_UPDATE)
    if (s_auto_phy_update) {
        /* Request 2M PHY. Result is reported asynchronously via le_phy_updated_cb. */
        const struct bt_conn_le_phy_param phy = {
            .options = BT_CONN_LE_PHY_OPT_NONE,
            .pref_tx_phy = BT_GAP_LE_PHY_2M,
            .pref_rx_phy = BT_GAP_LE_PHY_2M,
        };
        (void)bt_conn_le_phy_update(conn, &phy);
    }
#endif

    /* Keep peripheral-side connection interval negotiation disabled by
     * default.  Automated tests use the central shell to issue a single,
     * explicit connection-update procedure; doing another request here creates
     * overlapping instants and obscures long-interval controller diagnostics.
     */
    if (s_auto_conn_param_update) {
        struct bt_le_conn_param param = {
            .interval_min = 200,    /* 250ms */
            .interval_max = 200,    /* 250ms */
            .latency = 0,
            .timeout = 600,         /* 6s */
        };
        bt_conn_le_param_update(conn, &param);
    }
}

static void disconnected_cb(struct bt_conn *conn, uint8_t reason)
{
    IPRO_LOGI(TAG, "Disconnected (reason 0x%02x)", reason);

    /* Stop streaming if active */
    if (s_ble.recorder && s_ble.recorder->stop_stream &&
        s_ble.recorder->is_streaming && s_ble.recorder->is_streaming()) {
        s_ble.recorder->stop_stream();
    }

    if (s_ble.conn) {
        bt_conn_unref(s_ble.conn);
        s_ble.conn = NULL;
    }
    s_ble.tx_notify_enabled = false;
    (void)k_work_cancel_delayable(&s_sec_req_work);
    (void)k_work_cancel_delayable(&s_dle_request_work);
    ipro_ble_ctlr_tickless_connection_active_set(false);
    ipro_ble_ctlr_pds_auto_connected(false);

    /*
     * Restart advertising from somewhere else, not from here.
     *
     * CONFIG_BT_MAX_CONN is 1, and undirected connectable advertising needs a
     * free connection object: le_adv_start_add_conn() calls bt_conn_add_le()
     * to represent the pending advertiser, and returns -ENOMEM when the pool
     * is empty. This callback runs while the connection that just closed is
     * still allocated - it is the object being reported - so the one slot is
     * taken for as long as we are in here.
     *
     * Sleeping in the callback made it worse rather than better: the delay ran
     * on the BT RX thread, which is the thread that would have freed the
     * object, so the wait guaranteed the very condition it was waiting on.
     * Observed as "Advertising start failed: -12" roughly 200 ms after every
     * disconnect, after which the device is unreachable until it is reset.
     */
    s_adv_restart_tries = 0U;
    (void)k_work_reschedule(&s_adv_restart_work, K_MSEC(200));
}

static void dle_request_work_handler(struct k_work *work)
{
    (void)work;

    if (s_ble.conn == NULL) {
        return;
    }
    if (s_link.tx_max_len >= 251U) {
        return;     /* already there - the phone's procedure raised both sides */
    }

    const struct bt_conn_le_data_len_param dle = {
        .tx_max_len = 251,
        .tx_max_time = 2120,
    };
    int err = bt_conn_le_data_len_update(s_ble.conn, &dle);
    if (err) {
        IPRO_LOGW(TAG, "DLE request: %d", err);
    }
}

static void sec_req_work_handler(struct k_work *work)
{
    (void)work;

    if (s_ble.conn == NULL) {
        return;
    }

    /* The usual case: the central paired inside the grace period and the
     * link is already encrypted. bt_conn_set_security() would return 0 here
     * without doing anything, which is not "we asked" - so check first. */
    if (bt_conn_get_security(s_ble.conn) >= BT_SECURITY_L2) {
        return;
    }

    int err = bt_conn_set_security(s_ble.conn, BT_SECURITY_L2);
    if (err == -EBUSY || err == -EALREADY) {
        return;     /* pairing in progress - the central got there first */
    }
    if (err) {
        IPRO_LOGW(TAG, "deferred bt_conn_set_security: %d", err);
    } else {
        IPRO_LOGI(TAG, "central did not pair within %u ms - requesting security",
                  (unsigned)SEC_REQ_GRACE_MS);
    }
}

static void adv_restart_work_handler(struct k_work *work)
{
    (void)work;

    if (s_ble.conn != NULL) {
        return;     /* reconnected already; connected_cb owns the state now */
    }

    if (start_advertising() == 0) {
        return;
    }

    /* Losing the race is still possible - the host frees the connection
     * object asynchronously and nothing tells us when. Retry rather than
     * leave the device silently off the air, which is indistinguishable from
     * a radio fault from the phone's side. */
    if (++s_adv_restart_tries < 25U) {
        (void)k_work_reschedule(&s_adv_restart_work, K_MSEC(200));
    } else {
        IPRO_LOGE(TAG, "advertising did not start after %u tries - BLE is down",
                  s_adv_restart_tries);
    }
}

static void le_phy_updated_cb(struct bt_conn *conn, struct bt_conn_le_phy_info *param)
{
    (void)conn;
    s_st.tx_phy = param ? param->tx_phy : 0U;
    s_st.rx_phy = param ? param->rx_phy : 0U;
    IPRO_LOGI(TAG, "PHY updated: TX=%uM RX=%uM (1=1M 2=2M 3=Coded)",
              s_st.tx_phy, s_st.rx_phy);
}

/*
 * The reconnect path after a PDS wake runs entirely through these two.
 *
 * There is no "reconnect" API to call: the peripheral re-advertises, the phone
 * initiates, and encryption comes back from the LTK both sides already hold.
 * What we do control is asking for it - connected_cb() raises the security
 * requirement, which on a bonded link is an SMP Security Request the central
 * answers by encrypting rather than by pairing again.  security_changed_cb()
 * is where that either happens or visibly does not.
 */
static void security_changed_cb(struct bt_conn *conn, bt_security_t level,
                                enum bt_security_err err)
{
    char addr[BT_ADDR_LE_STR_LEN];
    const bt_addr_le_t *dst = bt_conn_get_dst(conn);

    bt_addr_le_to_str(dst, addr, sizeof(addr));

    if (err != BT_SECURITY_ERR_SUCCESS) {
        IPRO_LOGE(TAG, "security failed: %s err %u", addr, err);
        return;
    }

    /* Distinguishing the two ways a link becomes encrypted matters here:
     * re-keying from a bond is the thing this app wants after every wake, and
     * a fresh pairing means the bond was lost - flash, not radio.
     *
     * bt_le_bond_exists() rather than bt_conn_info.security.flags: those flags
     * report OOB and Secure Connections, neither of which says anything about
     * whether a key is on file. bt_conn_get_dst() gives the identity address
     * on a resolved link, which is what the key store is indexed by. */
    (void)dst;
    IPRO_LOGI(TAG, "encrypted: %s level %u (%s)", addr, level,
              s_ble.peer_was_bonded ? "peer was bonded at connect"
                                    : "peer was NOT bonded -- this is a fresh pairing");
}

static void le_param_updated_cb(struct bt_conn *conn, uint16_t interval,
                                uint16_t latency, uint16_t timeout)
{
    (void)conn;
    s_link.interval_1p25ms = interval;
    s_link.latency = latency;
    s_link.timeout_10ms = timeout;
    IPRO_LOGI(TAG, "conn params: interval %u.%02u ms, latency %u, timeout %u ms",
              (interval * 125U) / 100U, (interval * 125U) % 100U,
              latency, timeout * 10U);
}

static void le_data_len_updated_cb(struct bt_conn *conn,
                                   struct bt_conn_le_data_len_info *info)
{
    (void)conn;
    s_link.tx_max_len = info->tx_max_len;
    s_link.rx_max_len = info->rx_max_len;
    /* 27 here means DLE never happened and every 244-byte notification is
     * going out as ten link-layer packets. Android initiates it after the
     * MTU exchange; this device does not, so if it is missing, look at the
     * phone. */
    IPRO_LOGI(TAG, "DLE: tx %u bytes/%u us, rx %u bytes/%u us%s",
              info->tx_max_len, info->tx_max_time,
              info->rx_max_len, info->rx_max_time,
              info->tx_max_len <= 27U ? " -- NOT negotiated" : "");
}

static struct bt_conn_cb conn_callbacks = {
    .connected = connected_cb,
    .disconnected = disconnected_cb,
    .le_param_updated = le_param_updated_cb,
    .le_phy_updated = le_phy_updated_cb,
    .le_data_len_updated = le_data_len_updated_cb,
    .security_changed = security_changed_cb,
};

static void auth_pairing_complete(struct bt_conn *conn, bool bonded)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    /* bonded=0 means the keys were not written to flash, so the next wake
     * pairs again.  Worth saying out loud - the link works either way, and the
     * difference only shows up one power cycle later. */
    IPRO_LOGI(TAG, "pairing complete: %s bonded=%d", addr, (int)bonded);

    /* A pairing that runs when the peer already had a bond means the old keys
     * were just replaced. The link works, so nothing else reports it, but it
     * says the reconnect path is not being used - either the phone dropped its
     * side, or the two disagree about the bond. */
    if (s_ble.peer_was_bonded) {
        IPRO_LOGW(TAG, "re-paired a peer that was already bonded -- the stored bond was not used");
    }
}

static void auth_pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    IPRO_LOGE(TAG, "pairing failed: %s reason=%u", addr, reason);
}

static struct bt_conn_auth_info_cb auth_info_callbacks = {
    .pairing_complete = auth_pairing_complete,
    .pairing_failed = auth_pairing_failed,
};

/* --- SMP auth (auto-accept) --- */
static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    IPRO_LOGI(TAG, "Passkey: %06u", passkey);
}

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
    bt_conn_auth_passkey_confirm(conn);
}

static void auth_pairing_confirm(struct bt_conn *conn)
{
    bt_conn_auth_pairing_confirm(conn);
}

static void auth_cancel(struct bt_conn *conn)
{
    IPRO_LOGW(TAG, "Pairing cancelled");
}

static struct bt_conn_auth_cb auth_callbacks = {
    .passkey_display = auth_passkey_display,
    .passkey_confirm = auth_passkey_confirm,
    .pairing_confirm = auth_pairing_confirm,
    .cancel = auth_cancel,
};

static void bond_log_cb(const struct bt_bond_info *info, void *user_data)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(&info->addr, addr, sizeof(addr));
    IPRO_LOGI(TAG, "  bond: %s", addr);
    (*(size_t *)user_data)++;
}

/* --- Advertising --- */
struct first_bond_arg {
    bt_addr_le_t addr;
    bool found;
};

static void first_bond_cb(const struct bt_bond_info *info, void *user_data)
{
    struct first_bond_arg *arg = user_data;

    if (!arg->found) {
        bt_addr_le_copy(&arg->addr, &info->addr);
        arg->found = true;
    }
}

static int start_advertising(void)
{
    struct bt_le_adv_param adv_param = {
        .id = BT_ID_DEFAULT,
        .options = BT_LE_ADV_OPT_CONN,
        .interval_min = ADV_INT_MIN,
        .interval_max = ADV_INT_MAX,
    };

    int err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    if (err == -EALREADY) {
        return 0;
    }
    if (err) {
        IPRO_LOGW(TAG, "advertising start: %d%s", err,
                  err == -ENOMEM ? " (connection object still held; will retry)" : "");
        return err;
    }

    struct first_bond_arg bond = { .found = false };

    bt_foreach_bond(BT_ID_DEFAULT, first_bond_cb, &bond);
    if (bond.found) {
        char addr[BT_ADDR_LE_STR_LEN];

        bt_addr_le_to_str(&bond.addr, addr, sizeof(addr));
        IPRO_LOGI(TAG, "Advertising as '%s' (bonded to %s - it reconnects on its own)",
                  DEVICE_NAME, addr);
    } else {
        IPRO_LOGI(TAG, "Advertising as '%s'", DEVICE_NAME);
    }
    return 0;
}

/* --- bt_ready callback --- */
static void bt_ready_cb(int err)
{
    if (err) {
        IPRO_LOGE(TAG, "BLE init failed (err %d)", err);
        return;
    }

    IPRO_LOGI(TAG, "BLE initialized");
    ipro_ble_ctlr_hcitl_host_ready();

    bt_set_name(DEVICE_NAME);
    bt_conn_cb_register(&conn_callbacks);
    bt_conn_auth_cb_register(&auth_callbacks);
    bt_conn_auth_info_cb_register(&auth_info_callbacks);
    bt_set_bondable(true);
    bt_passkey_set(123456);

#if defined(CONFIG_BT_SETTINGS)
    /* Bonds live in flash, and nothing reads them back without this call.
     *
     * It is what makes the button-triggered PDS cycle survivable: that path
     * powers the radio down and comes back through a reset, so keys held only
     * in RAM would be gone and every wake would ask the user to pair again.
     *
     * Must run before advertising starts - a peer that connects first would
     * be answered with an empty key store and pair from scratch, quietly
     * replacing the bond it already had. */
    int set_err = settings_load_subtree("bt");
    if (set_err) {
        /* The store itself is unusable (not "an entry was stale" - the port
         * skips those). Nothing loads and nothing will persist. */
        IPRO_LOGE(TAG, "settings_load_subtree(bt): %d -- bonds will not survive a reset",
                  set_err);
    } else {
        size_t bonds = 0;
        /* The addresses, not just how many. A count cannot answer the only
         * question that matters when a phone pairs again instead of
         * reconnecting - whether the bond on file is that phone's. */
        bt_foreach_bond(BT_ID_DEFAULT, bond_log_cb, &bonds);
        IPRO_LOGI(TAG, "bonds loaded: %u", (unsigned)bonds);
    }
#endif

    k_work_init_delayable(&s_adv_restart_work, adv_restart_work_handler);
    k_work_init_delayable(&s_sec_req_work, sec_req_work_handler);
    k_work_init_delayable(&s_dle_request_work, dle_request_work_handler);
    if (start_advertising() != 0) {
        /* -EAGAIN here means the host is not READY yet - with settings on,
         * that happens in the settings commit, and a load that stumbled can
         * leave it a little late. A first start that fails and is never
         * retried is a device that is silent until someone resets it. */
        s_adv_restart_tries = 0U;
        (void)k_work_reschedule(&s_adv_restart_work, K_MSEC(200));
    }
}

/* --- Public API --- */
int ble_audio_init(const ble_audio_recorder_t *recorder)
{
    int err;

    /* Do not rely on reset-time BSS state here.  IPRO7AI flash/software-reset
     * recovery can leave retained RAM-looking state long enough for the APP
     * PDS policy to see a stale connection pointer and allow pre-connection
     * PDS.  Start each BLE bring-up from an explicit disconnected policy
     * state; connected_cb() is the only place that arms connected tickless PDS.
     */
    memset(&s_ble, 0, sizeof(s_ble));
    s_ble.recorder = recorder;

    /* Create audio send semaphore and task */
    s_send_sem = xSemaphoreCreateBinary();

    xTaskCreate(audio_send_task, "ble_send", SEND_TASK_STACK, NULL,
                SEND_TASK_PRIO, &s_send_task_handle);

    /* Create stream send task (reads from stream_queue) */
    xTaskCreate(stream_send_task, "ble_strm", STREAM_TASK_STACK, NULL,
                STREAM_TASK_PRIO, NULL);

    /* APP-selected PDS policy: keep HAL/PM defaults unchanged for other apps. */
    pm_pds_ble_ctlr_domain_policy_set(
        PM_PDS_BLE_CTLR_DOMAIN_POLICY_CONNECTED_BASELINE);
    hal_lp_set_timing_policy(HAL_LP_TIMING_POLICY_EXACT_US_TO_RTC |
                                 HAL_LP_TIMING_POLICY_PDS_WAKE_DELAY_VALID,
                             1021U);

    /* Init new BLE controller */
    IPRO_LOGI(TAG, "Initializing BLE controller...");
    err = ipro_ble_ctlr_controller_init(configMAX_PRIORITIES - 1);
    if (err) {
        IPRO_LOGE(TAG, "controller init failed: %d", err);
        return -1;
    }
    ipro_ble_ctlr_pds_auto_connected(false);
    ipro_ble_ctlr_tickless_connection_active_set(false);
    ipro_ble_ctlr_tickless_stats_reset();
    vTaskDelay(pdMS_TO_TICKS(50));

    /* Init HCI driver */
    IPRO_LOGI(TAG, "Initializing HCI driver...");
    err = hci_driver_ipro_ble_ctlr_init();
    if (err) {
        IPRO_LOGE(TAG, "HCI driver init failed: %d", err);
        return -1;
    }
    vTaskDelay(pdMS_TO_TICKS(50));

    if (ipro_ble_ctlr_pds_enable(false) != 0) {
        IPRO_LOGW(TAG, "retained PDS disable API unavailable");
    } else if (ipro_ble_ctlr_pds_auto_config(false, 0U) != 0) {
        IPRO_LOGW(TAG, "automatic tickless PDS disable rejected");
    } else {
        IPRO_LOGI(TAG, "automatic tickless PDS disabled by default; use ble_pds on");
    }

    /* Enable BLE stack */
    IPRO_LOGI(TAG, "Enabling BLE stack...");
    err = bt_enable(bt_ready_cb);
    if (err) {
        IPRO_LOGE(TAG, "bt_enable failed: %d", err);
        return -1;
    }
    s_ble_started = true;

    return 0;
}

void ble_audio_notify_recording_done(void)
{
    send_status_response();
}

void ble_audio_set_lc3(bool enable)
{
    s_use_lc3 = enable;
    IPRO_LOGI(TAG, "Stream codec: %s", enable ? "LC3" : "raw PCM");
}

bool ble_audio_get_lc3(void)
{
    return s_use_lc3;
}

bool ble_audio_is_started(void)
{
    return s_ble_started;
}

int ble_audio_suspend_for_sleep(void)
{
    if (!s_ble_started) {
        return 0;
    }

    /*
     * Take the link down on purpose instead of letting it lapse.
     *
     * PDS31 powers the radio off, so the peer stops being answered the moment
     * we sleep either way. The difference is what the peer sees: a supervision
     * timeout leaves it waiting out the full timeout (6 s at the parameters
     * this app negotiates) before it can even begin looking for us again,
     * while a disconnect with a reason tells it immediately. On Android that
     * is the difference between reconnecting as soon as the key is pressed and
     * reconnecting several seconds later.
     *
     * REMOTE_POWER_OFF rather than REMOTE_USER_TERM: a phone treats the latter
     * as "the user turned this device's connection off" and some stacks will
     * not auto-reconnect afterwards.
     */
    if (s_ble.conn) {
        int err = bt_conn_disconnect(s_ble.conn,
                                     BT_HCI_ERR_REMOTE_POWER_OFF);
        if (err && err != -ENOTCONN) {
            IPRO_LOGW(TAG, "disconnect before sleep: %d", err);
        }
        /* Wait for disconnected_cb() to release the reference, so the LL
         * actually transmits the terminate before the radio loses power.
         * Bounded: a peer that is already out of range never acknowledges. */
        for (int i = 0; i < 100 && s_ble.conn != NULL; i++) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    /* disconnected_cb() restarts advertising, so this has to come after the
     * wait above or it stops an advertiser that is about to be re-armed. */
    (void)k_work_cancel_delayable(&s_adv_restart_work);
    int err = bt_le_adv_stop();
    if (err && err != -EALREADY) {
        IPRO_LOGW(TAG, "adv stop before sleep: %d", err);
    }
    return 0;
}

/* bond — list bonded peers, or `bond clear` to forget them all. */
static void bond_print_cb(const struct bt_bond_info *info, void *user_data)
{
    char addr[BT_ADDR_LE_STR_LEN];
    size_t *n = user_data;

    bt_addr_le_to_str(&info->addr, addr, sizeof(addr));
    printf("  [%u] %s\r\n", (unsigned)(*n)++, addr);
}

static int cmd_bond(int argc, char **argv)
{
    size_t n = 0;

    if (argc >= 2 && strcmp(argv[1], "clear") == 0) {
        int err = bt_unpair(BT_ID_DEFAULT, NULL);

        printf("bond clear: %d\r\n", err);
        return 0;
    }

    printf("bonded peers:\r\n");
    bt_foreach_bond(BT_ID_DEFAULT, bond_print_cb, &n);
    if (n == 0) {
        printf("  (none) -- the next connection will pair\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_bond, bond, List bonded peers; bond clear forgets them);

/* --- 2M 破音驗證 shell 指令 ------------------------------------------ */

/* ble_phy <1|2|3> — 對現行連線發起 PHY update(1=1M 2=2M 3=Coded) */
static int cmd_ble_phy(int argc, char **argv)
{
    if (argc < 2) {
        IPRO_LOGI(TAG, "Usage: ble_phy <1|2|3>  (1=1M, 2=2M, 3=Coded)");
        IPRO_LOGI(TAG, "Current: TX=%uM RX=%uM (0=尚未回報)",
               s_st.tx_phy, s_st.rx_phy);
        return 0;
    }
    if (!s_ble.conn) {
        IPRO_LOGW(TAG, "ble_phy: not connected");
        return -1;
    }
    int phy = atoi(argv[1]);
    if (phy < 1 || phy > 3) {
        IPRO_LOGW(TAG, "ble_phy: invalid %d", phy);
        return -1;
    }
    uint8_t pref = (uint8_t)(1u << (phy - 1));
    struct bt_conn_le_phy_param phy_param = {
        .options = BT_CONN_LE_PHY_OPT_NONE,
        .pref_tx_phy = pref,
        .pref_rx_phy = pref,
    };
    int err = bt_conn_le_phy_update(s_ble.conn, &phy_param);
    if (err) {
        IPRO_LOGE(TAG, "ble_phy: bt_conn_le_phy_update failed %d", err);
        return -1;
    }
    IPRO_LOGI(TAG, "ble_phy: requested %dM — 等 'PHY updated' log(對端可拒絕)", phy);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ble_phy, ble_phy, Set BLE PHY 1|2|3 and show current);

/* ble_stat [clr] — 串流掉幀儀表(破音 = tx_drop / producer_drop 增加) */
static int cmd_ble_stat(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "clr") == 0) {
        s_st.enc_frames = s_st.tx_ok = s_st.tx_retry_ok = 0;
        s_st.tx_drop = s_st.producer_drop = 0;
        s_st.bulk_pkts = s_st.bulk_retry = 0;
        g_src_crc_fail = g_src_crc_healed = g_src_crc_1st_pkt = 0;
        ipro_ble_ctlr_lld_rx_nonempty_desc_count = 0;
        ipro_ble_ctlr_lld_rx_nonempty_crcok_count = 0;
        ipro_ble_ctlr_lld_rx_nonempty_badstat_count = 0;
        ipro_ble_ctlr_lld_rx_clean_pdu_count = 0;
        ipro_ble_ctlr_lld_rx_control_pdu_count = 0;
        ipro_ble_ctlr_lld_rx_acl_pdu_count = 0;
        ipro_ble_ctlr_lld_rx_rfu_pdu_count = 0;
        ipro_ble_ctlr_lld_rx_ring_wr = 0;
        ipro_ble_ctlr_lld_rx_last_evt = 0;
        ipro_ble_ctlr_lld_rx_last_rxstat = 0;
        ipro_ble_ctlr_lld_rx_last_rxphce = 0;
        ipro_ble_ctlr_lld_rx_last_dataptr = 0;
        ipro_ble_ctlr_lld_rx_last_link = 0;
        ipro_ble_ctlr_lld_rx_last_ch = 0;
        ipro_ble_ctlr_lld_rx_last_llid = 0;
        ipro_ble_ctlr_lld_rx_last_len = 0;
        ipro_ble_ctlr_lld_rx_last_opcode = 0xff;
        IPRO_LOGI(TAG, "ble_stat: counters cleared");
        return 0;
    }

    uint32_t enc = s_st.enc_frames;
    uint32_t drop = s_st.tx_drop;
    IPRO_LOGI(TAG, "--- BLE audio stream stats ---");
    IPRO_LOGI(TAG, "PHY        : TX=%uM RX=%uM (0=未回報)", s_st.tx_phy, s_st.rx_phy);
    {
        extern volatile uint32_t ipro_ble_ctlr_hcitl_h2c_deferred;
        /* Non-zero and climbing during a bulk transfer means the host is
         * waiting on the H2C ring, not on the radio. */
        IPRO_LOGI(TAG, "H2C stalls : %lu (host write refused, ring full)",
                  (unsigned long)ipro_ble_ctlr_hcitl_h2c_deferred);
    }
    IPRO_LOGI(TAG, "DLE        : tx %u rx %u bytes (0=未回報, 27=沒協商)",
              s_link.tx_max_len, s_link.rx_max_len);
    IPRO_LOGI(TAG, "interval   : %u.%02u ms, latency %u, timeout %u ms (0=沒更新過)",
              (s_link.interval_1p25ms * 125U) / 100U, (s_link.interval_1p25ms * 125U) % 100U,
              s_link.latency, s_link.timeout_10ms * 10U);
    IPRO_LOGI(TAG, "RSSI       : N/A (not exposed by ipro_bt_host public API)");
    IPRO_LOGI(TAG, "enc frames : %lu (10ms/frame)", (unsigned long)enc);
    IPRO_LOGI(TAG, "tx ok      : %lu", (unsigned long)s_st.tx_ok);
    IPRO_LOGI(TAG, "tx retry ok: %lu  <- 壓力前兆", (unsigned long)s_st.tx_retry_ok);
    IPRO_LOGI(TAG, "tx DROP    : %lu  <- 每次都是一聲破音", (unsigned long)drop);
    IPRO_LOGI(TAG, "isr DROP   : %lu  <- queue 滿(消化不及)",
           (unsigned long)s_st.producer_drop);
    IPRO_LOGI(TAG, "bulk pkts  : %lu (get_audio 已送,裝置端無損)",
           (unsigned long)s_st.bulk_pkts);
    IPRO_LOGI(TAG, "bulk retry : %lu (退避重送,非遺失)",
           (unsigned long)s_st.bulk_retry);
    IPRO_LOGI(TAG, "bulk seq   : %s | pace: %u bytes / %u ms",
           s_bulk_seq_en ? "ON(APP 解析會失敗,僅驗證用)" : "off",
           s_pace_bytes, s_pace_ms);
    IPRO_LOGI(TAG, "ctlr evt   : evt=%lu rxirq=%lu skip=%lu txirq=%lu txdesc=%lu",
           (unsigned long)ipro_ble_ctlr_con_last_evt_cnt,
           (unsigned long)ipro_ble_ctlr_con_evt_rx_irq_cnt,
           (unsigned long)ipro_ble_ctlr_con_evt_skip_cnt,
           (unsigned long)ipro_ble_ctlr_con_tx_irq_cnt,
           (unsigned long)ipro_ble_ctlr_con_tx_desc_cnt);
    IPRO_LOGI(TAG, "ctlr rx    : desc=%lu ok=%lu crc=%lu sync=%lu gate=%lu fail_ch=%lu fail_st=0x%lx",
           (unsigned long)ipro_ble_ctlr_con_rx_desc_cnt,
           (unsigned long)ipro_ble_ctlr_con_rx_crc_ok_cnt,
           (unsigned long)ipro_ble_ctlr_con_rx_crc_err_cnt,
           (unsigned long)ipro_ble_ctlr_con_rx_sync_err_cnt,
           (unsigned long)ipro_ble_ctlr_con_rx_crc_gate_cnt,
           (unsigned long)ipro_ble_ctlr_con_last_fail_channel,
           (unsigned long)ipro_ble_ctlr_con_last_fail_status);
    IPRO_LOGI(TAG, "ctlr timing: start=%lu target=%lu interval_hs=%lu rxwin=%lu add=%lu good_evt=%lu good_off=%ld min/max=%ld/%ld good_ch=%lu hw_ch=%lu",
           (unsigned long)ipro_ble_ctlr_con_last_start_clock,
           (unsigned long)ipro_ble_ctlr_con_last_target_clock,
           (unsigned long)ipro_ble_ctlr_con_last_interval_hs,
           (unsigned long)ipro_ble_ctlr_con_last_rx_win_hus,
           (unsigned long)ipro_ble_ctlr_con_last_rx_win_add_hus,
           (unsigned long)ipro_ble_ctlr_con_last_good_evt_cnt,
           (long)ipro_ble_ctlr_con_last_good_offset_hus,
           (long)ipro_ble_ctlr_con_good_offset_min_hus,
           (long)ipro_ble_ctlr_con_good_offset_max_hus,
           (unsigned long)ipro_ble_ctlr_con_last_good_channel,
           (unsigned long)ipro_ble_ctlr_con_last_hw_channel);
    IPRO_LOGI(TAG, "ctlr instant: pending=%lu/%lu applied=%lu/%lu chmap=%08lx:%02lx hop1=%lu guard=%lu active=%lu recovery=%lu",
           (unsigned long)ipro_ble_ctlr_con_pending_instant_type,
           (unsigned long)ipro_ble_ctlr_con_pending_instant_evt,
           (unsigned long)ipro_ble_ctlr_con_applied_instant_type,
           (unsigned long)ipro_ble_ctlr_con_applied_instant_evt,
           (unsigned long)ipro_ble_ctlr_con_chmap_lo,
           (unsigned long)ipro_ble_ctlr_con_chmap_hi,
           (unsigned long)ipro_ble_ctlr_con_hop_sel_1,
           (unsigned long)ipro_ble_ctlr_con_deep_sleep_guard,
           (unsigned long)ipro_ble_ctlr_con_instant_active,
           (unsigned long)ipro_ble_ctlr_con_deep_sleep_recovery);
    for (unsigned int link = 0; link < 4; link++) {
        if (ipro_ble_ctlr_llcp_tx_try_count[link] ||
            ipro_ble_ctlr_llcp_tx_ack_count[link] ||
            ipro_ble_ctlr_llcp_rx_count[link]) {
            IPRO_LOGI(TAG,
                   "ctlr llcp[%u]: tx=%lu op=0x%02x len=%u st=%u ack=%lu op=0x%02x len=%u rx=%lu op=0x%02x len=%u",
                   link,
                   (unsigned long)ipro_ble_ctlr_llcp_tx_try_count[link],
                   ipro_ble_ctlr_llcp_last_tx_opcode[link],
                   ipro_ble_ctlr_llcp_last_tx_len[link],
                   ipro_ble_ctlr_llcp_last_tx_status[link],
                   (unsigned long)ipro_ble_ctlr_llcp_tx_ack_count[link],
                   ipro_ble_ctlr_llcp_last_ack_opcode[link],
                   ipro_ble_ctlr_llcp_last_ack_len[link],
                   (unsigned long)ipro_ble_ctlr_llcp_rx_count[link],
                   ipro_ble_ctlr_llcp_last_rx_opcode[link],
                   ipro_ble_ctlr_llcp_last_rx_len[link]);
        }
    }
    IPRO_LOGI(TAG,
           "lld rx pdu : nonempty=%lu crcok=%lu badst=%lu clean=%lu ctrl=%lu acl=%lu rfu=%lu last link=%u evt=%lu ch=%u llid=%u len=%u st=0x%04x ph=0x%04x op=0x%02x ptr=0x%04x",
           (unsigned long)ipro_ble_ctlr_lld_rx_nonempty_desc_count,
           (unsigned long)ipro_ble_ctlr_lld_rx_nonempty_crcok_count,
           (unsigned long)ipro_ble_ctlr_lld_rx_nonempty_badstat_count,
           (unsigned long)ipro_ble_ctlr_lld_rx_clean_pdu_count,
           (unsigned long)ipro_ble_ctlr_lld_rx_control_pdu_count,
           (unsigned long)ipro_ble_ctlr_lld_rx_acl_pdu_count,
           (unsigned long)ipro_ble_ctlr_lld_rx_rfu_pdu_count,
           ipro_ble_ctlr_lld_rx_last_link,
           (unsigned long)ipro_ble_ctlr_lld_rx_last_evt,
           ipro_ble_ctlr_lld_rx_last_ch,
           ipro_ble_ctlr_lld_rx_last_llid,
           ipro_ble_ctlr_lld_rx_last_len,
           ipro_ble_ctlr_lld_rx_last_rxstat,
           ipro_ble_ctlr_lld_rx_last_rxphce,
           ipro_ble_ctlr_lld_rx_last_opcode,
           ipro_ble_ctlr_lld_rx_last_dataptr);
    {
        uint32_t wr = ipro_ble_ctlr_lld_rx_ring_wr;
        uint32_t dump = (wr < 16U) ? wr : 16U;
        uint32_t start = wr - dump;
        for (uint32_t i = 0; i < dump; i++) {
            uint32_t slot = (start + i) & 31U;
            IPRO_LOGI(TAG,
                   "lld rx[%02lu]: link=%u evt=%u ch=%u llid=%u len=%u st=0x%04x ph=0x%04x op=0x%02x ptr=0x%04x data=%04x/%04x",
                   (unsigned long)(start + i),
                   ipro_ble_ctlr_lld_rx_ring_link[slot],
                   ipro_ble_ctlr_lld_rx_ring_evt[slot],
                   ipro_ble_ctlr_lld_rx_ring_ch[slot],
                   ipro_ble_ctlr_lld_rx_ring_llid[slot],
                   ipro_ble_ctlr_lld_rx_ring_len[slot],
                   ipro_ble_ctlr_lld_rx_ring_rxstat[slot],
                   ipro_ble_ctlr_lld_rx_ring_rxphce[slot],
                   ipro_ble_ctlr_lld_rx_ring_opcode[slot],
                   ipro_ble_ctlr_lld_rx_ring_dataptr[slot],
                   ipro_ble_ctlr_lld_rx_ring_data0[slot],
                   ipro_ble_ctlr_lld_rx_ring_data1[slot]);
        }
    }
    IPRO_LOGI(TAG, "src CRC    : fail %lu / healed %lu%s",
           (unsigned long)g_src_crc_fail, (unsigned long)g_src_crc_healed,
           g_src_crc_fail ? "  <<< 來源讀取層毛病!" : " (組包讀取全數與錄音真值一致)");
    if (g_src_crc_fail)
        IPRO_LOGI(TAG, "first fail @ 封包 #%lu", (unsigned long)g_src_crc_1st_pkt);
    if (enc) {
        IPRO_LOGI(TAG, "drop rate  : %lu.%02lu %%",
               (unsigned long)(drop * 100 / enc),
               (unsigned long)(drop * 10000 / enc % 100));
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ble_stat, ble_stat, Show/clear BLE audio drop stats);

static int cmd_ble_pds(int argc, char **argv)
{
    if (argc > 1) {
        bool on = (strcmp(argv[1], "on") == 0);

        if (ipro_ble_ctlr_pds_enable(on) != 0) {
            IPRO_LOGW(TAG, "ble_pds: enable(%s) rejected", on ? "on" : "off");
            return -1;
        }
        if (ipro_ble_ctlr_pds_auto_config(on, 0U) != 0) {
            IPRO_LOGW(TAG, "ble_pds: auto(%s) rejected", on ? "on" : "off");
            return -1;
        }
        s_ble.pds_requested = on;
        ipro_ble_ctlr_pds_auto_connected((s_ble.conn != NULL) && on);
        ipro_ble_ctlr_tickless_connection_active_set((s_ble.conn != NULL) && on);
        if (on) {
            ipro_ble_ctlr_tickless_stats_reset();
        }
    }

    IPRO_LOGI(TAG, "ble_pds: app=%u auto=%u enter/wake/restore/fail=%lu/%lu/%lu/%lu req=%lu elapsed=%lu state=%lu",
           ipro_ble_ctlr_pds_snapshot_enabled() ? 1U : 0U,
           ipro_ble_ctlr_pds_auto_enabled() ? 1U : 0U,
           (unsigned long)ipro_ble_ctlr_pds_diag.enter_count,
           (unsigned long)ipro_ble_ctlr_pds_diag.wake_count,
           (unsigned long)ipro_ble_ctlr_pds_diag.restore_count,
           (unsigned long)ipro_ble_ctlr_pds_diag.failure_count,
           (unsigned long)ipro_ble_ctlr_pds_diag.requested_us,
           (unsigned long)ipro_ble_ctlr_pds_diag.elapsed_us,
           (unsigned long)ipro_ble_ctlr_pds_diag.state);
    IPRO_LOGI(TAG, "ble_pds: tickless calls=%lu selected=%lu grant/reject=%lu/%lu sleep_us=%lu win grant=%lu reject nb/exp/rep/tim/pol=%lu/%lu/%lu/%lu/%lu full=%lu last_hus=%lu auto attempt/enter=%lu/%lu",
           (unsigned long)ipro_ble_ctlr_tickless_calls,
           (unsigned long)ipro_ble_ctlr_tickless_selected_ticks,
           (unsigned long)ipro_ble_ctlr_tickless_pds_grant,
           (unsigned long)ipro_ble_ctlr_tickless_pds_grant_reject,
           (unsigned long)ipro_ble_ctlr_tickless_pds_grant_sleep_us,
           (unsigned long)ipro_ble_ctlr_tickless_window_grant,
           (unsigned long)ipro_ble_ctlr_tickless_window_reject_no_budget,
           (unsigned long)ipro_ble_ctlr_tickless_window_reject_expired,
           (unsigned long)ipro_ble_ctlr_tickless_window_reject_repeat,
           (unsigned long)ipro_ble_ctlr_tickless_window_reject_timing,
           (unsigned long)ipro_ble_ctlr_tickless_window_reject_policy,
           (unsigned long)ipro_ble_ctlr_full_window_count,
           (unsigned long)ipro_ble_ctlr_full_window_last_hus,
           (unsigned long)ipro_ble_ctlr_pds_auto_attempt_count,
           (unsigned long)ipro_ble_ctlr_pds_auto_enter_count);
    IPRO_LOGI(TAG, "ble_pds: gate pending/ready/run/consume=%lu/%lu/%lu/%lu mask=0x%02lx reject hci/guard/recovery/native/rw=%lu/%lu/%lu/%lu/%lu deadline=%lu last_slack=%lu deferred grant/rejpol/rejtim=%lu/%lu/%lu",
           (unsigned long)ipro_ble_ctlr_pds_pending_seen_cnt,
           (unsigned long)ipro_ble_ctlr_pds_ready_seen_cnt,
           (unsigned long)ipro_ble_ctlr_pds_probe_run_calls,
           (unsigned long)ipro_ble_ctlr_pds_probe_consume_count,
           (unsigned long)ipro_ble_ctlr_pds_gate_mask,
           (unsigned long)ipro_ble_ctlr_pds_gate_reject_hci_cnt,
           (unsigned long)ipro_ble_ctlr_pds_gate_reject_guard_cnt,
           (unsigned long)ipro_ble_ctlr_pds_gate_reject_recovery_cnt,
           (unsigned long)ipro_ble_ctlr_pds_gate_reject_native_cnt,
           (unsigned long)ipro_ble_ctlr_pds_gate_reject_rw_cnt,
           (unsigned long)ipro_ble_ctlr_pds_deadline_reject_cnt,
           (unsigned long)ipro_ble_ctlr_pds_last_slack_hus,
           (unsigned long)ipro_ble_ctlr_tickless_deferred_grant,
           (unsigned long)ipro_ble_ctlr_tickless_deferred_reject_policy,
           (unsigned long)ipro_ble_ctlr_tickless_deferred_reject_timing);
    IPRO_LOGI(TAG, "ble_pds: hcitl ready=%u now/awake=%lu/%lu gate reject nrdy/guard=%lu/%lu bytes h2c/c2h=%lu/%lu deferred h2c/c2h=%lu/%lu",
           ipro_ble_ctlr_hcitl_host_is_ready ? 1U : 0U,
           (unsigned long)ipro_ble_ctlr_hcitl_gate_now_tick,
           (unsigned long)ipro_ble_ctlr_hcitl_gate_awake_until_tick,
           (unsigned long)ipro_ble_ctlr_hcitl_gate_reject_not_ready,
           (unsigned long)ipro_ble_ctlr_hcitl_gate_reject_guard,
           (unsigned long)ipro_ble_ctlr_hcitl_h2c_bytes,
           (unsigned long)ipro_ble_ctlr_hcitl_c2h_bytes,
           (unsigned long)ipro_ble_ctlr_hcitl_h2c_deferred,
           (unsigned long)ipro_ble_ctlr_hcitl_c2h_deferred);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ble_pds, ble_pds, Show/control BLE app tickless PDS on|off);

static int cmd_ble_param_auto(int argc, char **argv)
{
    if (argc > 1) {
        if (strcmp(argv[1], "on") == 0) {
            s_auto_conn_param_update = true;
        } else if (strcmp(argv[1], "off") == 0) {
            s_auto_conn_param_update = false;
        } else {
            IPRO_LOGI(TAG, "ble_param_auto: usage: ble_param_auto [on|off]");
            return -1;
        }
    }

    IPRO_LOGI(TAG, "ble_param_auto: %s (default off; central owns CI test updates)",
           s_auto_conn_param_update ? "on" : "off");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ble_param_auto, ble_param_auto, Show/control peripheral auto CI update);

static const char *trace_result_name(uint8_t result)
{
    switch (result) {
    case 1: return "SYNC";
    case 2: return "CRC";
    case 3: return "OK";
    default: return "-";
    }
}

static int cmd_ble_trace(int argc, char **argv)
{
    uint32_t count;
    uint32_t n = 64U;

    if (argc > 1) {
        if (strcmp(argv[1], "on") == 0) {
            ipro_ble_ctlr_con_trace_freeze = 0U;
            ipro_ble_ctlr_con_trace_count = 0U;
            ipro_ble_ctlr_con_trace_reset = 1U;
            ipro_ble_ctlr_con_trace_force = 1U;
            IPRO_LOGI(TAG, "ble_trace: on");
            return 0;
        }
        if (strcmp(argv[1], "off") == 0) {
            ipro_ble_ctlr_con_trace_force = 0U;
            IPRO_LOGI(TAG, "ble_trace: off count=%lu",
                   (unsigned long)ipro_ble_ctlr_con_trace_count);
            return 0;
        }
        if (strcmp(argv[1], "freeze") == 0) {
            ipro_ble_ctlr_con_trace_freeze = 1U;
            IPRO_LOGI(TAG, "ble_trace: freeze count=%lu",
                   (unsigned long)ipro_ble_ctlr_con_trace_count);
            return 0;
        }
        if (strcmp(argv[1], "dump") != 0) {
            IPRO_LOGI(TAG, "ble_trace: usage: ble_trace [on|off|freeze|dump [n]]");
            return -1;
        }
        if (argc > 2) {
            n = (uint32_t)strtoul(argv[2], NULL, 0);
            if (n == 0U) {
                n = 64U;
            }
            if (n > 256U) {
                n = 256U;
            }
        }
    }

    count = ipro_ble_ctlr_con_trace_count;
    IPRO_LOGI(TAG, "ble_trace: force=%lu freeze=%lu count=%lu dump=%lu",
           (unsigned long)ipro_ble_ctlr_con_trace_force,
           (unsigned long)ipro_ble_ctlr_con_trace_freeze,
           (unsigned long)count,
           (unsigned long)n);
    if (count == 0U) {
        return 0;
    }
    if (n > count) {
        n = count;
    }

    for (uint32_t seq = count - n; seq < count; ++seq) {
        uint32_t slot = seq & 255U;
        IPRO_LOGI(TAG, "ble_trace[%03lu]: evt=%u role=%u res=%s ch=%u exp=%u hop=%u rxst=0x%04x rxph=0x%04x rxcha=0x%04x clkn=%04x%04x fcnt=0x%04x target=%lu start=%lu txrx=0x%04x rxd=%u",
               (unsigned long)seq,
               (unsigned int)ipro_ble_ctlr_con_trace_evt[slot],
               (unsigned int)ipro_ble_ctlr_con_trace_role[slot],
               trace_result_name(ipro_ble_ctlr_con_trace_result[slot]),
               (unsigned int)ipro_ble_ctlr_con_trace_ch[slot],
               (unsigned int)ipro_ble_ctlr_con_trace_expected_ch[slot],
               (unsigned int)ipro_ble_ctlr_con_trace_hop_ch[slot],
               (unsigned int)ipro_ble_ctlr_con_trace_rxstat[slot],
               (unsigned int)ipro_ble_ctlr_con_trace_rxphce[slot],
               (unsigned int)ipro_ble_ctlr_con_trace_rxchass[slot],
               (unsigned int)ipro_ble_ctlr_con_trace_rxclkn1[slot],
               (unsigned int)ipro_ble_ctlr_con_trace_rxclkn0[slot],
               (unsigned int)ipro_ble_ctlr_con_trace_rxfcnt[slot],
               (unsigned long)ipro_ble_ctlr_con_trace_target[slot],
               (unsigned long)ipro_ble_ctlr_con_trace_start[slot],
               (unsigned int)ipro_ble_ctlr_con_trace_txrx[slot],
               (unsigned int)ipro_ble_ctlr_con_trace_rxdesc_idx[slot]);
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ble_trace, ble_trace, Control/dump controller connection trace);

static int cmd_ble_disc(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (s_ble.conn) {
        int err = bt_conn_disconnect(s_ble.conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        IPRO_LOGI(TAG, "ble_disc: disconnect rc=%d", err);
        return err;
    }

    (void)bt_le_adv_stop();
    vTaskDelay(pdMS_TO_TICKS(50));
    start_advertising();
    IPRO_LOGI(TAG, "ble_disc: no active conn; advertising restart requested");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ble_disc, ble_disc, Disconnect current BLE link or restart advertising);

/* pdm_seq on|off — get_audio 封包插入遞增序號(接收端抓缺號=丟包鐵證)。
 * ⚠ 開啟後既有 APP 會解析失敗,僅供 nRF Connect 之類的封包驗證。 */
static int cmd_pdm_seq(int argc, char **argv)
{
    if (argc > 1) {
        s_bulk_seq_en = (strcmp(argv[1], "on") == 0);
    }
    IPRO_LOGI(TAG, "pdm_seq: %s%s", s_bulk_seq_en ? "ON" : "off",
              s_bulk_seq_en ? " (格式=[0x?][seq][data],APP 會解析失敗)" : "");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_seq, pdm_seq, get_audio seq-byte mode on|off);

/* ble_pace [bytes ms] — get_audio 節流:每送 bytes 讓出 ms(0 0=全速) */
static int cmd_ble_pace(int argc, char **argv)
{
    if (argc >= 3) {
        s_pace_bytes = (uint16_t)atoi(argv[1]);
        s_pace_ms    = (uint16_t)atoi(argv[2]);
    }
    IPRO_LOGI(TAG, "ble_pace: %u bytes / %u ms%s", s_pace_bytes, s_pace_ms,
              s_pace_bytes ? "" : " (全速)");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ble_pace, ble_pace, get_audio throttle: ble_pace [bytes ms]);

/* ble_probe [clr] — controller EM ACL TX buffer 生命週期探針讀值。
 * 需 CONFIG_BLE_FORCE_SOURCE_BUILD=y;prebuilt 時 weak 定義全零。
 * clobber>0 = 封包在 EM 佇列期間被覆寫(在途踩踏實錘)。 */
__attribute__((weak)) ble_emprobe_t g_emprobe;
__attribute__((weak)) void ble_emprobe_expect(const uint8_t *base, uint32_t len,
                                              uint16_t chunk, uint8_t marker)
{
    (void)base; (void)len; (void)chunk; (void)marker;
}

static int cmd_ble_probe(int argc, char **argv)
{
    ble_emprobe_t *p = &g_emprobe;

    if (argc > 1 && strcmp(argv[1], "clr") == 0) {
        p->alloc_cnt = p->free_cnt = p->alloc_fail = 0;
        p->double_alloc = p->double_free = p->clobber = 0;
        p->exp_pkts = p->exp_mismatch = 0;
        p->max_inflight = p->cur_inflight;
        IPRO_LOGI(TAG, "ble_probe: counters cleared");
        return 0;
    }

    IPRO_LOGI(TAG, "EM ACL TX probe:");
    IPRO_LOGI(TAG, "  alloc/free   : %lu / %lu (fail %lu)",
              (unsigned long)p->alloc_cnt, (unsigned long)p->free_cnt,
              (unsigned long)p->alloc_fail);
    IPRO_LOGI(TAG, "  inflight     : cur %u, max %u", p->cur_inflight, p->max_inflight);
    IPRO_LOGI(TAG, "  double alloc : %lu   double free: %lu",
              (unsigned long)p->double_alloc, (unsigned long)p->double_free);
    IPRO_LOGI(TAG, "  CLOBBER      : %lu%s", (unsigned long)p->clobber,
              p->clobber ? "  <<< EM 在途被覆寫!" : "");
    if (p->clobber) {
        char line[3 * BLE_EMPROBE_SNAP_LEN + 1];
        int off = 0;

        IPRO_LOGI(TAG, "  first hit    : buf#%u len %u crc %04x->%04x",
                  p->clob_idx, p->clob_len, p->clob_crc_want, p->clob_crc_got);
        for (int i = 0; i < BLE_EMPROBE_SNAP_LEN; i++) {
            off += snprintf(&line[off], sizeof(line) - (size_t)off,
                            "%02x ", p->clob_data[i]);
        }
        IPRO_LOGI(TAG, "  data now     : %s", line);
    }
    IPRO_LOGI(TAG, "HCI 邊界內容比對 (v2, 需 pdm_seq on):");
    IPRO_LOGI(TAG, "  checked/mismatch : %lu / %lu%s",
              (unsigned long)p->exp_pkts, (unsigned long)p->exp_mismatch,
              p->exp_mismatch ? "  <<< 進 EM 前已是垃圾 = host 段汙染!" : "");
    if (p->exp_mismatch) {
        char got[3 * BLE_EMPROBE_SNAP_LEN + 1];
        char want[3 * BLE_EMPROBE_SNAP_LEN + 1];
        int goff = 0;
        int woff = 0;

        IPRO_LOGI(TAG, "  first fail   : seq %u, src off %lu",
                  p->exp_seq_at_fail, (unsigned long)p->exp_off_at_fail);
        for (int i = 0; i < BLE_EMPROBE_SNAP_LEN; i++) {
            goff += snprintf(&got[goff], sizeof(got) - (size_t)goff,
                             "%02x ", p->exp_got[i]);
            woff += snprintf(&want[woff], sizeof(want) - (size_t)woff,
                             "%02x ", p->exp_want[i]);
        }
        IPRO_LOGI(TAG, "  got  : %s", got);
        IPRO_LOGI(TAG, "  want : %s", want);
    }
    if (p->alloc_cnt == 0) {
        IPRO_LOGI(TAG, "  (全零?未開 CONFIG_BLE_FORCE_SOURCE_BUILD 或尚無 ACL TX)");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ble_probe, ble_probe, EM ACL TX lifetime probe [clr]);

/* Required by BLE controller */
void app_init(void)
{
    IPRO_LOGI(TAG, "BLE stack callback: app_init");
}

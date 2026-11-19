/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 *
 * IPRO6S Classic Bluetooth example: A2DP sink (Bluetooth speaker) with
 * AVRCP remote control.
 *
 * A phone or computer pairs with the board (Secure Simple Pairing, no PIN)
 * and plays audio to it. The board receives the SBC stream over A2DP,
 * decodes it and plays the PCM on I2S for an external DAC or amplifier.
 * Over AVRCP the board reads the track title and sends play / pause /
 * next / previous to the phone from the console.
 *
 * Console:
 *   a2dp                     stream state, packet and decoder counters
 *   avrcp play|pause|next|prev
 *   bt_unpair                forget paired phones (bt_classic_common.c)
 */
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
#if defined(CONFIG_APP_I2S_OUTPUT)
#include "a2dp_i2s_out.h"
#endif

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/classic/a2dp.h>
#include <zephyr/bluetooth/classic/a2dp_codec_sbc.h>
#include <zephyr/bluetooth/classic/avrcp.h>
#include <zephyr/bluetooth/classic/sdp.h>
#include <zephyr/bluetooth/sbc.h>
#include <zephyr/kernel.h>
#include <zephyr/net_buf.h>

#define TAG      "A2DP"
#define TAG_RC   "AVRCP"

/* ------------------------------------------------------------------ */
/* A2DP sink                                                          */
/* ------------------------------------------------------------------ */

BT_A2DP_SBC_SINK_EP_DEFAULT(s_sink_ep);
static struct bt_a2dp_stream s_stream;

/* SDP record: Audio Sink over AVDTP, A2DP 1.4. */
static struct bt_sdp_attribute s_a2dp_sink_attrs[] = {
    BT_SDP_NEW_SERVICE,
    BT_SDP_LIST(
        BT_SDP_ATTR_SVCLASS_ID_LIST,
        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 3),
        BT_SDP_DATA_ELEM_LIST(
            {
                BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                BT_SDP_ARRAY_16(BT_SDP_AUDIO_SINK_SVCLASS)
            },
        )
    ),
    BT_SDP_LIST(
        BT_SDP_ATTR_PROTO_DESC_LIST,
        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 16),
        BT_SDP_DATA_ELEM_LIST(
            {
                BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
                BT_SDP_DATA_ELEM_LIST(
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                        BT_SDP_ARRAY_16(BT_SDP_PROTO_L2CAP)
                    },
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UINT16),
                        BT_SDP_ARRAY_16(BT_UUID_AVDTP_VAL)
                    },
                )
            },
            {
                BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
                BT_SDP_DATA_ELEM_LIST(
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                        BT_SDP_ARRAY_16(BT_UUID_AVDTP_VAL)
                    },
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UINT16),
                        BT_SDP_ARRAY_16(0x0103) /* AVDTP 1.3 */
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
                        BT_SDP_ARRAY_16(BT_SDP_ADVANCED_AUDIO_SVCLASS)
                    },
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UINT16),
                        BT_SDP_ARRAY_16(0x0104) /* A2DP 1.4 */
                    },
                )
            },
        )
    ),
    BT_SDP_SERVICE_NAME("IPRO6S A2DP Sink"),
    BT_SDP_SUPPORTED_FEATURES(0x0001), /* headphone */
};

static struct bt_sdp_record s_a2dp_sink_record = BT_SDP_RECORD(s_a2dp_sink_attrs);

static struct sbc_decoder s_sbc;
static uint8_t s_pcm[512] __attribute__((aligned(4)));
static uint32_t s_sample_rate;
static uint8_t s_channels;
static volatile bool s_streaming;
static bool s_decoder_ready;
static uint32_t s_packets;
static uint32_t s_bytes;
static uint32_t s_frames;
static uint32_t s_decode_errors;
static uint32_t s_seq_gaps;
static uint16_t s_last_seq;
static bool s_seq_valid;

static void a2dp_stream_configured(struct bt_a2dp_stream *stream)
{
    struct bt_a2dp_codec_sbc_params *sbc;

    s_decoder_ready = false;
    if (stream == NULL || stream->codec_config.len != BT_A2DP_SBC_IE_LENGTH) {
        IPRO_LOGW(TAG, "unexpected codec configuration");
        return;
    }
    sbc = (struct bt_a2dp_codec_sbc_params *)stream->codec_config.codec_ie;
    s_sample_rate = bt_a2dp_sbc_get_sampling_frequency(sbc);
    s_channels = bt_a2dp_sbc_get_channel_num(sbc);
    s_decoder_ready = sbc_setup_decoder(&s_sbc) == 0;
#if defined(CONFIG_APP_I2S_OUTPUT)
    if (a2dp_i2s_configure(s_sample_rate, s_channels) != 0) {
        IPRO_LOGW(TAG, "I2S cannot play %lu Hz", (unsigned long)s_sample_rate);
    }
#endif
    IPRO_LOGI(TAG, "configured: SBC %lu Hz, %u channel(s)",
              (unsigned long)s_sample_rate, s_channels);
}

static void a2dp_stream_established(struct bt_a2dp_stream *stream)
{
    (void)stream;
    IPRO_LOGI(TAG, "stream open");
}

static void a2dp_stream_started(struct bt_a2dp_stream *stream)
{
    (void)stream;
    s_seq_valid = false;
    s_decoder_ready = sbc_setup_decoder(&s_sbc) == 0;
    s_streaming = true;
#if defined(CONFIG_APP_I2S_OUTPUT)
    a2dp_i2s_set_playing(true);
#endif
    IPRO_LOGI(TAG, "playing");
}

static void a2dp_stream_suspended(struct bt_a2dp_stream *stream)
{
    (void)stream;
    s_streaming = false;
#if defined(CONFIG_APP_I2S_OUTPUT)
    a2dp_i2s_set_playing(false);
#endif
    IPRO_LOGI(TAG, "paused (%lu packets, %lu SBC frames so far)",
              (unsigned long)s_packets, (unsigned long)s_frames);
}

static void a2dp_stream_released(struct bt_a2dp_stream *stream)
{
    (void)stream;
    s_streaming = false;
#if defined(CONFIG_APP_I2S_OUTPUT)
    a2dp_i2s_set_playing(false);
#endif
    IPRO_LOGI(TAG, "stream closed");
}

static void a2dp_decode(struct net_buf *buf)
{
    uint8_t header;
    uint8_t frame_count;
    const void *input;
    uint32_t input_size;

    if (!s_decoder_ready || buf->len < 2u) {
        return;
    }
    /* A2DP SBC payload: one media header byte, then frame_count frames. */
    header = buf->data[0];
    frame_count = BT_A2DP_SBC_MEDIA_HDR_NUM_FRAMES_GET(header);
    if (BT_A2DP_SBC_MEDIA_HDR_F_GET(header) != 0u || frame_count == 0u) {
        s_decode_errors++;  /* fragmented frames are not used by SBC sources */
        return;
    }
    input = &buf->data[1];
    input_size = buf->len - 1u;
    for (uint8_t i = 0; i < frame_count; i++) {
        uint32_t output_size = sizeof(s_pcm);

        if (sbc_decode(&s_sbc, &input, &input_size, s_pcm, &output_size) != 0 ||
            output_size == 0u) {
            s_decode_errors++;
            return;
        }
        s_frames++;
#if defined(CONFIG_APP_I2S_OUTPUT)
        (void)a2dp_i2s_write(s_pcm, output_size);
#endif
    }
}

static void a2dp_stream_recv(struct bt_a2dp_stream *stream, struct net_buf *buf,
                             uint16_t seq_num, uint32_t timestamp)
{
    (void)stream;
    (void)timestamp;
    if (s_seq_valid && seq_num != (uint16_t)(s_last_seq + 1u)) {
        s_seq_gaps++;
    }
    s_last_seq = seq_num;
    s_seq_valid = true;
    s_packets++;
    s_bytes += buf->len;
    a2dp_decode(buf);
}

static struct bt_a2dp_stream_ops s_stream_ops = {
    .configured = a2dp_stream_configured,
    .established = a2dp_stream_established,
    .released = a2dp_stream_released,
    .started = a2dp_stream_started,
    .suspended = a2dp_stream_suspended,
    .recv = a2dp_stream_recv,
};

static void a2dp_connected(struct bt_a2dp *a2dp, int err)
{
    (void)a2dp;
    if (err == 0) {
        IPRO_LOGI(TAG, "A2DP connected");
    } else {
        IPRO_LOGW(TAG, "A2DP connect failed: %d", err);
    }
}

static void a2dp_disconnected(struct bt_a2dp *a2dp)
{
    (void)a2dp;
    s_streaming = false;
#if defined(CONFIG_APP_I2S_OUTPUT)
    a2dp_i2s_set_playing(false);
#endif
    IPRO_LOGI(TAG, "A2DP disconnected");
}

static int a2dp_config_req(struct bt_a2dp *a2dp, struct bt_a2dp_ep *ep,
                           struct bt_a2dp_codec_cfg *codec_cfg,
                           struct bt_a2dp_stream **stream, uint8_t *rsp_err_code)
{
    (void)a2dp;
    (void)ep;
    (void)codec_cfg;
    bt_a2dp_stream_cb_register(&s_stream, &s_stream_ops);
    *stream = &s_stream;
    *rsp_err_code = BT_AVDTP_SUCCESS;
    return 0;
}

static int a2dp_reconfig_req(struct bt_a2dp_stream *stream,
                             struct bt_a2dp_codec_cfg *codec_cfg, uint8_t *rsp_err_code)
{
    (void)stream;
    (void)codec_cfg;
    *rsp_err_code = BT_AVDTP_SUCCESS;
    return 0;
}

static int a2dp_accept_req(struct bt_a2dp_stream *stream, uint8_t *rsp_err_code)
{
    (void)stream;
    *rsp_err_code = BT_AVDTP_SUCCESS;
    return 0;
}

static struct bt_a2dp_cb s_a2dp_callbacks = {
    .connected = a2dp_connected,
    .disconnected = a2dp_disconnected,
    .config_req = a2dp_config_req,
    .reconfig_req = a2dp_reconfig_req,
    .establish_req = a2dp_accept_req,
    .release_req = a2dp_accept_req,
    .start_req = a2dp_accept_req,
    .suspend_req = a2dp_accept_req,
    .abort_req = a2dp_accept_req,
    .get_config_req = a2dp_accept_req,
};

/* ------------------------------------------------------------------ */
/* AVRCP: controller (board -> phone keys, track title) and a minimal */
/* target (answers the phone's unit / subunit / key queries)          */
/* ------------------------------------------------------------------ */

/* Transaction labels: 0..1 metadata requests, 2..13 keys, 14 notification. */
#define AVRCP_TID_PLAY_STATUS   0u
#define AVRCP_TID_ELEMENTS      1u
#define AVRCP_TID_KEY_FIRST     2u
#define AVRCP_TID_KEY_LAST      12u
#define AVRCP_TID_NOTIFY        14u
#define AVRCP_ATTR_TITLE        1u
#define AVRCP_ATTR_ARTIST       2u
#define AVRCP_TEXT_MAX          60u

static struct bt_avrcp_ct *s_avrcp_ct;
static uint8_t s_key_tid = AVRCP_TID_KEY_FIRST;
static uint8_t s_key_opid;
static atomic_t s_key_busy;

static uint32_t be32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
}

static void avrcp_request_track_info(struct bt_avrcp_ct *ct)
{
    /* GetElementAttributes(identifier 0 = playing track, 0 attrs = all). */
    static const uint8_t request[9] = {0};
    struct net_buf *buf = bt_avrcp_create_vendor_pdu(NULL);
    int err;

    if (buf == NULL) {
        return;
    }
    net_buf_add_mem(buf, request, sizeof(request));
    err = bt_avrcp_ct_get_element_attrs(ct, AVRCP_TID_ELEMENTS, buf);
    if (err != 0) {
        net_buf_unref(buf);
    }
}

static void avrcp_element_attrs(struct bt_avrcp_ct *ct, uint8_t tid, uint8_t status,
                                struct net_buf *buf)
{
    const uint8_t *p;
    size_t left;
    unsigned int count;

    (void)ct;
    (void)tid;
    if (status != BT_AVRCP_STATUS_SUCCESS || buf == NULL || buf->len < 1u) {
        return;
    }
    /* Response: count, then per attribute id(4) charset(2) len(2) text. */
    p = buf->data;
    left = buf->len;
    count = *p++;
    left--;
    while (count-- > 0u && left >= 8u) {
        uint32_t id = be32(p);
        size_t len = ((size_t)p[6] << 8) | p[7];

        if (len > left - 8u) {
            return;
        }
        if (id == AVRCP_ATTR_TITLE || id == AVRCP_ATTR_ARTIST) {
            IPRO_LOGI(TAG_RC, "%s: %.*s", id == AVRCP_ATTR_TITLE ? "title" : "artist",
                      (int)(len > AVRCP_TEXT_MAX ? AVRCP_TEXT_MAX : len),
                      (const char *)(p + 8));
        }
        p += 8u + len;
        left -= 8u + len;
    }
}

static void avrcp_notify_changed(struct bt_avrcp_ct *ct, uint8_t event,
                                 struct bt_avrcp_event_data *data)
{
    static const char *const states[] = {"stopped", "playing", "paused",
                                         "fwd seek", "rev seek"};

    if (event != BT_AVRCP_EVT_PLAYBACK_STATUS_CHANGED || data == NULL) {
        return;
    }
    IPRO_LOGI(TAG_RC, "phone is %s",
              data->play_status < 5u ? states[data->play_status] : "in an unknown state");
    if (data->play_status == 1u) {
        avrcp_request_track_info(ct);
    }
    /* A CHANGED response ends the registration: register again. */
    (void)bt_avrcp_ct_register_notification(ct, AVRCP_TID_NOTIFY, event, 0,
                                            avrcp_notify_changed);
}

static void avrcp_notification(struct bt_avrcp_ct *ct, uint8_t tid, uint8_t status,
                               uint8_t event, struct bt_avrcp_event_data *data)
{
    (void)ct;
    (void)tid;
    (void)event;
    (void)data;
    if (status != BT_AVRCP_STATUS_SUCCESS) {
        IPRO_LOGW(TAG_RC, "phone refused play-status notifications: %u", status);
    }
}

static void avrcp_passthrough_rsp(struct bt_avrcp_ct *ct, uint8_t tid,
                                  bt_avrcp_rsp_t result,
                                  const struct bt_avrcp_passthrough_rsp *rsp)
{
    (void)ct;
    (void)tid;
    (void)rsp;
    if (result != BT_AVRCP_RSP_ACCEPTED) {
        IPRO_LOGW(TAG_RC, "phone rejected the key: %u", (unsigned int)result);
    }
}

static void avrcp_ct_connected(struct bt_conn *conn, struct bt_avrcp_ct *ct)
{
    (void)conn;
    s_avrcp_ct = ct;
    IPRO_LOGI(TAG_RC, "remote control connected");
    (void)bt_avrcp_ct_register_notification(ct, AVRCP_TID_NOTIFY,
                                            BT_AVRCP_EVT_PLAYBACK_STATUS_CHANGED, 0,
                                            avrcp_notify_changed);
    avrcp_request_track_info(ct);
}

static void avrcp_ct_disconnected(struct bt_avrcp_ct *ct)
{
    if (s_avrcp_ct == ct) {
        s_avrcp_ct = NULL;
    }
    IPRO_LOGI(TAG_RC, "remote control disconnected");
}

static const struct bt_avrcp_ct_cb s_avrcp_ct_callbacks = {
    .connected = avrcp_ct_connected,
    .disconnected = avrcp_ct_disconnected,
    .get_element_attrs = avrcp_element_attrs,
    .passthrough_rsp = avrcp_passthrough_rsp,
    .notification = avrcp_notification,
};

static void avrcp_tg_connected(struct bt_conn *conn, struct bt_avrcp_tg *tg)
{
    (void)conn;
    (void)tg;
}

static void avrcp_tg_disconnected(struct bt_avrcp_tg *tg)
{
    (void)tg;
}

static void avrcp_unit_info_req(struct bt_avrcp_tg *tg, uint8_t tid)
{
    struct bt_avrcp_unit_info_rsp rsp = {
        .unit_type = BT_AVRCP_SUBUNIT_TYPE_PANEL,
        .company_id = BT_AVRCP_COMPANY_ID_BLUETOOTH_SIG,
    };

    (void)bt_avrcp_tg_send_unit_info_rsp(tg, tid, &rsp);
}

static void avrcp_subunit_info_req(struct bt_avrcp_tg *tg, uint8_t tid)
{
    (void)bt_avrcp_tg_send_subunit_info_rsp(tg, tid);
}

static void avrcp_passthrough_req(struct bt_avrcp_tg *tg, uint8_t tid, struct net_buf *buf)
{
    const struct bt_avrcp_passthrough_cmd *cmd;
    struct net_buf *rsp;
    size_t len;

    /* Accept the phone's keys (for example its volume buttons) unchanged. */
    if (buf == NULL || buf->len < sizeof(*cmd)) {
        return;
    }
    cmd = (const struct bt_avrcp_passthrough_cmd *)buf->data;
    len = sizeof(*cmd) + cmd->data_len;
    if (len > buf->len) {
        return;
    }
    rsp = bt_avrcp_create_pdu(NULL);
    if (rsp == NULL) {
        return;
    }
    if (net_buf_tailroom(rsp) < len) {
        net_buf_unref(rsp);
        return;
    }
    net_buf_add_mem(rsp, buf->data, len);
    if (bt_avrcp_tg_send_passthrough_rsp(tg, tid, BT_AVRCP_RSP_ACCEPTED, rsp) != 0) {
        net_buf_unref(rsp);
    }
}

static const struct bt_avrcp_tg_cb s_avrcp_tg_callbacks = {
    .connected = avrcp_tg_connected,
    .disconnected = avrcp_tg_disconnected,
    .unit_info_req = avrcp_unit_info_req,
    .subunit_info_req = avrcp_subunit_info_req,
    .passthrough_req = avrcp_passthrough_req,
};

/* Keys go out from the system work queue: AVCTP serializes its transmit
 * queue with a semaphore, so the shell task must not block on it. */
static void avrcp_key_work(struct k_work *work)
{
    struct bt_avrcp_ct *ct = s_avrcp_ct;
    uint8_t tid = s_key_tid;
    int err = -ENOTCONN;

    (void)work;
    s_key_tid = (tid >= AVRCP_TID_KEY_LAST) ? AVRCP_TID_KEY_FIRST : (uint8_t)(tid + 2u);
    if (ct != NULL) {
        err = bt_avrcp_ct_passthrough(ct, tid, s_key_opid, BT_AVRCP_BUTTON_PRESSED, NULL, 0);
        if (err == 0) {
            err = bt_avrcp_ct_passthrough(ct, (uint8_t)(tid + 1u), s_key_opid,
                                          BT_AVRCP_BUTTON_RELEASED, NULL, 0);
        }
    }
    if (err != 0) {
        IPRO_LOGW(TAG_RC, "key not sent: %d", err);
    }
    atomic_clear(&s_key_busy);
}
K_WORK_DEFINE(s_avrcp_key_work, avrcp_key_work);

/* ------------------------------------------------------------------ */
/* Start-up and console                                               */
/* ------------------------------------------------------------------ */

static void profiles_register(void)
{
    int err;

    err = bt_a2dp_register_cb(&s_a2dp_callbacks);
    if (err == 0) {
        err = bt_a2dp_register_ep(&s_sink_ep, BT_AVDTP_AUDIO, BT_AVDTP_SINK);
    }
    if (err == 0) {
        err = bt_sdp_register_service(&s_a2dp_sink_record);
    }
    if (err != 0) {
        IPRO_LOGE(TAG, "A2DP sink register failed: %d", err);
        return;
    }
    /* AVRCP registers its own SDP records with the callbacks. */
    if (bt_avrcp_ct_register_cb(&s_avrcp_ct_callbacks) != 0 ||
        bt_avrcp_tg_register_cb(&s_avrcp_tg_callbacks) != 0) {
        IPRO_LOGW(TAG_RC, "AVRCP register failed");
    }
    IPRO_LOGI(TAG, "A2DP sink (SBC) and AVRCP ready");
}

#if defined(CONFIG_SHELL)
static void cmd_a2dp(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    IPRO_LOGI(TAG, "link %s, stream %s, SBC %lu Hz x %u",
              bt_classic_connected() ? "up" : "down",
              s_streaming ? "playing" : "stopped",
              (unsigned long)s_sample_rate, s_channels);
    IPRO_LOGI(TAG, "packets %lu (%lu bytes), SBC frames %lu, decode errors %lu, "
              "sequence gaps %lu",
              (unsigned long)s_packets, (unsigned long)s_bytes, (unsigned long)s_frames,
              (unsigned long)s_decode_errors, (unsigned long)s_seq_gaps);
#if defined(CONFIG_APP_I2S_OUTPUT)
    {
        a2dp_i2s_stats_t st;

        a2dp_i2s_get_stats(&st);
        IPRO_LOGI(TAG, "I2S %s, %lu bytes out, buffered %lu, underruns %lu, drops %lu",
                  st.playing ? "on" : "off", (unsigned long)st.written_bytes,
                  (unsigned long)st.buffered_bytes, (unsigned long)st.underruns,
                  (unsigned long)st.dropped_writes);
    }
#endif
}
SHELL_CMD_EXPORT_ALIAS(cmd_a2dp, a2dp, A2DP stream and decoder status);

static void cmd_avrcp(int argc, char **argv)
{
    static const struct {
        const char *name;
        uint8_t opid;
    } keys[] = {
        {"play", BT_AVRCP_OPID_PLAY},
        {"pause", BT_AVRCP_OPID_PAUSE},
        {"next", BT_AVRCP_OPID_FORWARD},
        {"prev", BT_AVRCP_OPID_BACKWARD},
    };
    size_t i;

    if (argc != 2) {
        IPRO_LOGI(TAG_RC, "usage: avrcp play|pause|next|prev");
        return;
    }
    for (i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
        if (strcmp(argv[1], keys[i].name) == 0) {
            break;
        }
    }
    if (i == sizeof(keys) / sizeof(keys[0])) {
        IPRO_LOGI(TAG_RC, "usage: avrcp play|pause|next|prev");
        return;
    }
    if (s_avrcp_ct == NULL) {
        IPRO_LOGW(TAG_RC, "no remote control connection");
        return;
    }
    if (!atomic_cas(&s_key_busy, 0, 1)) {
        IPRO_LOGW(TAG_RC, "previous key still being sent");
        return;
    }
    s_key_opid = keys[i].opid;
    if (k_work_submit(&s_avrcp_key_work) < 0) {
        atomic_clear(&s_key_busy);
        return;
    }
    IPRO_LOGI(TAG_RC, "sent %s", keys[i].name);
}
SHELL_CMD_EXPORT_ALIAS(cmd_avrcp, avrcp, Send play / pause / next / prev to the phone);
#endif

static void app_task(void *arg)
{
    (void)arg;
#if defined(CONFIG_APP_I2S_OUTPUT)
    if (a2dp_i2s_init() != 0) {
        IPRO_LOGW(TAG, "I2S output unavailable; the stream is decoded only");
    }
#endif
    if (bt_classic_host_start(profiles_register) != 0) {
        IPRO_LOGE(TAG, "Bluetooth did not start");
    }
    vTaskDelete(NULL);
}

int main(void)
{
    ipro_platform_init();
    IPRO_LOGI(TAG, "IPRO6S Classic Bluetooth A2DP sink example");

    if (bt_classic_controller_init() != 0) {
        IPRO_LOGE(TAG, "stopped: no Bluetooth controller");
    } else if (xTaskCreate(app_task, "app", 2048, NULL, 5, NULL) != pdPASS) {
        IPRO_LOGE(TAG, "stopped: cannot create the app task");
    }

    vTaskStartScheduler();
    while (1) {
    }
}

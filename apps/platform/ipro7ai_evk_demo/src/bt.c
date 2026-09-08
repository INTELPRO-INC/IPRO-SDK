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
 * @file bt.c
 * @brief Bluetooth LE + LE Audio unicast sink. See bt.h.
 */

#include <generated/autoconf.h>

#ifdef CONFIG_BLUETOOTH_LE_ENABLE

#include "bt.h"

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <FreeRTOS.h>
#include <task.h>

/* Upstream Zephyr v4.4 layout: everything lives under <zephyr/...>. The old
 * blestack used <bluetooth/...>; do not mix the two. */
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>

#if defined(CONFIG_BT_AUDIO)
#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/audio/bap.h>
#include <zephyr/bluetooth/audio/pacs.h>
#include <zephyr/bluetooth/audio/vcp.h>
#include <zephyr/bluetooth/iso.h>
#endif

/* The new stack does not start itself: the controller and the H4 transport
 * are brought up explicitly, before bt_enable(). */
#include "hci_driver_ipro_ble_ctlr.h"
#include "ipro_ble_controller.h"
#include "ipro_ble_ctlr_hcitl.h"
#include "ipro_ble_ctlr_lowpower.h"

#if defined(CONFIG_BT_SETTINGS)
#include <zephyr/settings/settings.h>
#endif

#include "bt_audio_out.h"
#include "bt_shell.h"
#include "audio.h"

#include "reg_rf.h"   /* RF_BASE */
#include "rf_reg.h"   /* RF_CAL_STATUS_* */



#include "ipro_log.h"

#define LOG_TAG "BT"

#define BT_APP_NAME_MAX  24

static char     s_name[BT_APP_NAME_MAX] = "IPRO7AI-EVK";
static bool     s_enabled;
static bool     s_ready;
static bool     s_advertising;
static bool     s_adv_wanted;   /* what the user asked for, across links */
static struct bt_conn *s_conn;
static bt_status_t s_status;

/* ---------------------------------------------------------------------------
 * Connection callbacks
 * -------------------------------------------------------------------------*/

static void addr_to_str(const bt_addr_le_t *addr, char *out, size_t n)
{
    bt_addr_le_to_str(addr, out, n);
}

static void on_connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    if (err) {
        IPRO_LOGE(LOG_TAG, "connection failed (0x%02X)", err);
        s_advertising = false;
        return;
    }

    addr_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    s_conn = bt_conn_ref(conn);
    s_advertising = false;   /* connectable advertising stops on connect */

    strncpy(s_status.addr, addr, sizeof(s_status.addr) - 1);
    s_status.addr[sizeof(s_status.addr) - 1] = '\0';
    s_status.connected = true;

    IPRO_LOGI(LOG_TAG, "connected to %s", addr);
}

static void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
    (void)conn;
    IPRO_LOGI(LOG_TAG, "disconnected (reason 0x%02X)", reason);

    if (s_conn) {
        bt_conn_unref(s_conn);
        s_conn = NULL;
    }
    s_status.connected = false;
    s_status.addr[0] = '\0';
    s_status.conn_interval = 0;
    bt_audio_out_stop();   /* signals only - must not block this thread */

    /*
     * Advertising has to be restarted by hand. Connectable advertising stops
     * the moment a central connects, and nothing was putting it back, so the
     * device went silent after its first disconnect and the phone could never
     * find it again - which is what "No valid adv" and "No pending conn" in
     * the host log are complaining about.
     *
     * Skipped if the user turned advertising off deliberately.
     */
    if (s_adv_wanted) {
        int err = bt_app_adv_start();

        if (err != BT_APP_OK) {
            IPRO_LOGE(LOG_TAG, "could not restart advertising (%d)", err);
        }
    }
}

static void on_param_updated(struct bt_conn *conn, uint16_t interval,
                             uint16_t latency, uint16_t timeout)
{
    (void)conn;
    s_status.conn_interval = interval;
    s_status.conn_latency  = latency;
    s_status.conn_timeout  = timeout;
    IPRO_LOGI(LOG_TAG, "conn params: interval %u (%u.%02u ms), latency %u, "
                       "timeout %u ms",
              interval, (unsigned)(interval * 125u / 100u),
              (unsigned)((interval * 125u) % 100u), latency, timeout * 10u);
}

static struct bt_conn_cb conn_callbacks = {
    .connected        = on_connected,
    .disconnected     = on_disconnected,
    .le_param_updated = on_param_updated,
};

/* ---------------------------------------------------------------------------
 * LE Audio - unicast sink
 * -------------------------------------------------------------------------*/

#if defined(CONFIG_BT_AUDIO) && defined(CONFIG_BT_BAP_UNICAST_SERVER)

/*
 * Published LC3 sink capability.
 *
 * 24 kHz is added to what was here before, because the DAC drains near
 * 32136 Hz and a source that picks a rate at or below that gets the clean
 * path: liblc3 upsamples to the DAC rate itself, properly, and the rate
 * matcher afterwards runs at ~1:1 where linear interpolation is fine.
 *
 * 48 kHz stays advertised, but only because bt_audio_out.c now puts a real
 * anti-alias filter in front of the decimation. Without it the phone picks
 * 48 kHz, everything between 16 and 24 kHz folds back into the audible band,
 * and it sounds rough - which is exactly what happened.
 */
static struct bt_audio_codec_cap lc3_sink_codec_cap =
    BT_AUDIO_CODEC_CAP_LC3(
        BT_AUDIO_CODEC_CAP_FREQ_16KHZ | BT_AUDIO_CODEC_CAP_FREQ_24KHZ |
            BT_AUDIO_CODEC_CAP_FREQ_32KHZ | BT_AUDIO_CODEC_CAP_FREQ_48KHZ,
        BT_AUDIO_CODEC_CAP_DURATION_7_5 | BT_AUDIO_CODEC_CAP_DURATION_10 |
            BT_AUDIO_CODEC_CAP_DURATION_PREFER_10,
        BT_AUDIO_CODEC_CAP_CHAN_COUNT_SUPPORT(1),
        26, 155, 1,
        (BT_AUDIO_CONTEXT_TYPE_MEDIA | BT_AUDIO_CONTEXT_TYPE_CONVERSATIONAL));

/*
 * Runtime override of the published rates. PACS reads the capability out of
 * this struct when the client asks for it, so patching the LTV in place takes
 * effect for the next discovery - reconnect after changing it.
 */
int bt_app_set_sink_rate(unsigned khz)
{
    enum bt_audio_codec_cap_freq f;

    switch (khz) {
    case 0:  f = BT_AUDIO_CODEC_CAP_FREQ_16KHZ | BT_AUDIO_CODEC_CAP_FREQ_24KHZ |
                 BT_AUDIO_CODEC_CAP_FREQ_32KHZ | BT_AUDIO_CODEC_CAP_FREQ_48KHZ; break;
    case 16: f = BT_AUDIO_CODEC_CAP_FREQ_16KHZ; break;
    case 24: f = BT_AUDIO_CODEC_CAP_FREQ_24KHZ; break;
    case 32: f = BT_AUDIO_CODEC_CAP_FREQ_32KHZ; break;
    case 48: f = BT_AUDIO_CODEC_CAP_FREQ_48KHZ; break;
    default: return -EINVAL;
    }
    /*
     * Returns the new data_len on success, not 0 - a positive result is a pass.
     */
    {
        int rc = bt_audio_codec_cap_set_freq(&lc3_sink_codec_cap, f);

        return (rc < 0) ? rc : 0;
    }
}

static struct bt_pacs_cap lc3_sink_cap = {
    .codec_cap = &lc3_sink_codec_cap,
};

/* ---------------------------------------------------------------------------
 * Volume Control (VCP renderer)
 *
 * CONFIG_BT_VCP_VOL_REND was already on, but nothing ever called
 * bt_vcp_vol_rend_register(), so the service was compiled in and never
 * published - the phone's volume slider had nothing to talk to. Registering it
 * maps that slider onto the same make-up gain `spk gain` drives.
 *
 * VCP volume is 0-255. LE Audio arrives near full scale, so the useful range
 * is downwards from unity: 255 maps to 0 dB and 0 to the gain floor. That is
 * only 20 dB of travel, which is coarse for a volume control but is what the
 * shared gain knob offers; widening it would change the microphone paths too.
 * -------------------------------------------------------------------------*/
/*
 * 60 dB of travel. The first attempt used 20, which was the old gain floor,
 * and a phone at its lowest setting (it sends volume 10/255, not 0) still
 * played loudly. Linear in dB across the control range is the usual mapping
 * for a volume slider, and volume 0 or an explicit mute is real silence
 * rather than just the bottom of the range.
 */
#define VCP_GAIN_MIN_DB   (-60)

static void vcp_state_cb(struct bt_conn *conn, int err, uint8_t volume, uint8_t mute)
{
    int db;

    (void)conn;

    if (err != 0) {
        IPRO_LOGW(LOG_TAG, "VCP state error %d", err);
        return;
    }

    if (mute || volume == 0u) {
        audio_set_mute(1);
        IPRO_LOGI(LOG_TAG, "volume %u/255 -> muted", (unsigned)volume);
        return;
    }

    audio_set_mute(0);

    /*
     * The attenuation is for the speaker only.
     *
     * On the UAC sink the host owns loudness - it has its own input and output
     * volume, and it is mixing at 32-bit float.  Attenuating here instead just
     * throws bits away before the samples ever leave the board: the slider's
     * midpoint is -29 dB, which is ~5 bits of a 16-bit sample, and turning the
     * host's volume back up amplifies the quantisation noise along with the
     * signal.  A USB capture device is expected to hand over its natural level
     * and let the host scale it, so that is what this does.  Mute is still
     * honoured above - a mute from the phone should be real silence either way.
     */
    if (bt_audio_out_get_sink() == BT_AO_SINK_UAC) {
        audio_set_gain(0);
        IPRO_LOGI(LOG_TAG, "volume %u/255 -> 0 dB (UAC sink: the host scales, "
                           "not us - use its own volume control)",
                  (unsigned)volume);
        return;
    }

    db = VCP_GAIN_MIN_DB + ((int)volume * -VCP_GAIN_MIN_DB) / 255;

    audio_set_gain(db);
    IPRO_LOGI(LOG_TAG, "volume %u/255 -> %+d dB", (unsigned)volume, db);
}

void bt_app_set_volume(uint8_t volume)
{
    vcp_state_cb(NULL, 0, volume, 0);
}

static struct bt_vcp_vol_rend_cb vcp_cb = {
    .state = vcp_state_cb,
};

static int vcp_register(void)
{
    struct bt_vcp_vol_rend_register_param param = {
        .step   = 16,
        .mute   = 0,
        .volume = 255,     /* unity - the source is already near full scale */
        .cb     = &vcp_cb,
    };

    return bt_vcp_vol_rend_register(&param);
}

static struct bt_bap_stream  s_sink_stream;
static struct bt_bap_ep     *s_sink_ep;

static void set_stream_state(bt_stream_state_t st)
{
    s_status.stream = st;
    IPRO_LOGI(LOG_TAG, "LE Audio stream -> %s", bt_app_stream_state_name(st));
}

static void stream_configured(struct bt_bap_stream *stream,
                              const struct bt_bap_qos_cfg_pref *pref)
{
    (void)stream; (void)pref;
    set_stream_state(BT_STREAM_CONFIGURED);
}

static void stream_qos_set(struct bt_bap_stream *stream)
{
    (void)stream;
    set_stream_state(BT_STREAM_QOS_SET);
}

static void stream_enabled(struct bt_bap_stream *stream)
{
    int err;

    set_stream_state(BT_STREAM_ENABLED);

    /*
     * A sink ASE does not reach Streaming on its own. The receiving side has
     * to declare itself ready: this sets receiver_ready, and the ASE moves
     * Enabling -> Streaming when the CIS connects. Without it the ASE sits in
     * Enabling, the phone times it out and releases, and the whole
     * configure/qos/enable dance repeats until the phone gives up and
     * disconnects.
     *
     * bt_bap_stream_start() is the public entry point; it dispatches to
     * bt_bap_unicast_server_start() internally (bap_stream.c). The reference
     * headset app calls the internal function directly - that works, but it
     * lives in subsys/.../bap_unicast_server.h, which is not a public header.
     */
    err = bt_bap_stream_start(stream);
    if (err != 0) {
        IPRO_LOGE(LOG_TAG, "receiver-start-ready failed (%d) - the stream will "
                           "fall back to idle", err);
    }
}

static void stream_started(struct bt_bap_stream *stream)
{
    s_status.rx_frames = 0;
    s_status.rx_bytes  = 0;
    s_status.rx_empty  = 0;
    s_status.rx_flagged = 0;
    set_stream_state(BT_STREAM_STREAMING);

    /*
     * What the central actually granted, as opposed to what we asked for in
     * the QoS preference - those are only hints and it is free to ignore them.
     * rtn is the one that decides whether lost packets get another chance: at
     * rtn 0 a single missed subevent is simply gone, and the concealment count
     * stops being a statement about RF and becomes a statement about QoS.
     */
    if (stream->iso != NULL) {
        struct bt_iso_info info;

        if (bt_iso_chan_get_info(stream->iso, &info) == 0) {
            const struct bt_iso_unicast_tx_info *c = &info.unicast.central;

            IPRO_LOGI(LOG_TAG, "CIS granted: phy %uM, bn %u, ft %u, "
                               "max_pdu %u, max_sdu %u",
                      (unsigned)c->phy, (unsigned)c->bn,
                      (unsigned)c->flush_timeout, (unsigned)c->max_pdu,
                      (unsigned)c->max_sdu);
            IPRO_LOGI(LOG_TAG, "CIS timing: transport latency %u us, SDU "
                               "interval %u us, CIG/CIS sync delay %u/%u us",
                      (unsigned)c->latency, (unsigned)c->sdu_interval,
                      (unsigned)info.unicast.cig_sync_delay,
                      (unsigned)info.unicast.cis_sync_delay);

            /*
             * RTN itself is not exposed, but transport latency is: a CIS with
             * retransmission opportunities has to buy them with latency, so a
             * transport latency no larger than one SDU interval means there is
             * no room for a second attempt and every missed subevent is simply
             * a lost frame. That reframes the concealment count entirely - it
             * stops being a statement about RF and becomes one about QoS.
             */
            if (c->sdu_interval != 0u && c->latency <= c->sdu_interval) {
                IPRO_LOGW(LOG_TAG, "transport latency %u us <= SDU interval "
                                   "%u us - no room for retransmissions",
                          (unsigned)c->latency, (unsigned)c->sdu_interval);
            }
        }
    }

#if defined(CONFIG_BT_AUDIO_RX)
    /*
     * The source picked the codec parameters during Config; read them back
     * rather than assuming a preset, because the phone is free to choose any
     * of the rates advertised in lc3_sink_codec_cap.
     */
    if (stream->codec_cfg != NULL) {
        int freq   = bt_audio_codec_cfg_get_freq(stream->codec_cfg);
        int dur    = bt_audio_codec_cfg_get_frame_dur(stream->codec_cfg);
        int octets = bt_audio_codec_cfg_get_octets_per_frame(stream->codec_cfg);
        int sr_hz  = (freq >= 0) ? bt_audio_codec_cfg_freq_to_freq_hz(freq) : -1;
        int dur_us = (dur  >= 0) ? bt_audio_codec_cfg_frame_dur_to_frame_dur_us(dur) : -1;

        if (sr_hz > 0 && dur_us > 0 && octets > 0) {
            (void)bt_audio_out_start((uint32_t)sr_hz, (uint32_t)dur_us,
                                     (uint32_t)octets);
        } else {
            IPRO_LOGE(LOG_TAG, "codec cfg incomplete (freq %d, dur %d, octets %d)"
                               " - frames will be counted, not played",
                      sr_hz, dur_us, octets);
        }
    }
#endif
}

static void stream_stopped(struct bt_bap_stream *stream, uint8_t reason)
{
    (void)stream;
    IPRO_LOGI(LOG_TAG, "stream stopped (reason 0x%02X)", reason);
    {
        /*
         * The two things that vary between boots on this board, printed with
         * the result so a single stop line is enough to correlate them.
         *
         * roscal is the receive DC-offset calibration - 3 means it converged.
         * The 32K drift bound follows the RC32K calibration residual, and the
         * residual is a fresh draw every boot because there is no 32.768 kHz
         * crystal fitted (its pads are the PDM microphone pins).
         */
        uint32_t st  = *(volatile uint32_t *)(RF_BASE + RF_CAL_STATUS_OFFSET);
        uint32_t ros = (st >> RF_ROSCAL_STATUS_POS) & 0x3u;
        uint32_t pct = s_status.rx_frames + s_status.rx_empty;

        pct = pct ? (s_status.rx_empty * 1000u) / pct : 0u;

        IPRO_LOGI(LOG_TAG, "rx: %u frames, %u concealed (%u.%u%%), all %u "
                           "flagged lost | roscal %u, 32K drift %u ppm",
                  (unsigned)s_status.rx_frames, (unsigned)s_status.rx_empty,
                  (unsigned)(pct / 10u), (unsigned)(pct % 10u),
                  (unsigned)s_status.rx_flagged, (unsigned)ros,
                  (unsigned)ipro_ble_ctlr_lowpower_sleep_drift_ppm_get());
    }
    bt_audio_out_stop();
    set_stream_state(BT_STREAM_STOPPED);
}

static void stream_released(struct bt_bap_stream *stream)
{
    (void)stream;
    bt_audio_out_stop();
    set_stream_state(BT_STREAM_IDLE);
}

#if defined(CONFIG_BT_AUDIO_RX)
/*
 * One LC3 frame from the source, on the BT RX thread. That thread must not
 * block, so this only copies the payload into the decoder's ring and returns;
 * bt_audio_out.c does the decode and the DAC write on its own task.
 *
 * A frame is concealed only when it is genuinely unusable: no buffer, zero
 * length, or the controller explicitly flagged it LOST or ERROR. It is
 * deliberately NOT gated on BT_ISO_FLAGS_VALID being set - requiring that
 * throws away every good frame on any path that does not populate the flag,
 * and the reference headset does not test it either. Concealing a frame that
 * actually arrived is worse than the packet loss it is meant to hide.
 */
static void stream_recv(struct bt_bap_stream *stream,
                        const struct bt_iso_recv_info *info,
                        struct net_buf *buf)
{
    bool flagged_bad = (info != NULL) &&
        ((info->flags & (BT_ISO_FLAGS_LOST | BT_ISO_FLAGS_ERROR)) != 0);

    (void)stream;

    if (buf == NULL || buf->len == 0 || flagged_bad) {
        s_status.rx_empty++;
        if (flagged_bad) {
            s_status.rx_flagged++;
        }
        bt_audio_out_push(NULL, 0);
        return;
    }
    s_status.rx_frames++;
    s_status.rx_bytes += buf->len;
    bt_audio_out_push(buf->data, buf->len);
}
#endif /* CONFIG_BT_AUDIO_RX */

static struct bt_bap_stream_ops stream_ops = {
    .configured = stream_configured,
    .qos_set    = stream_qos_set,
    .enabled    = stream_enabled,
    .started    = stream_started,
    .stopped    = stream_stopped,
    .released   = stream_released,
#if defined(CONFIG_BT_AUDIO_RX)
    .recv       = stream_recv,
#endif
};

static int unicast_config(struct bt_conn *conn, const struct bt_bap_ep *ep,
                          enum bt_audio_dir dir,
                          const struct bt_audio_codec_cfg *codec_cfg,
                          struct bt_bap_stream **stream,
                          struct bt_bap_qos_cfg_pref *const pref,
                          struct bt_bap_ascs_rsp *rsp)
{
    (void)conn; (void)codec_cfg;

    /*
     * rsp is an OUTPUT parameter: the ASCS layer sends whatever is in it back
     * to the client. Leaving it untouched means an uninitialised response code
     * goes on the wire, and the client treats it as a failure and releases the
     * ASE - which looks exactly like the stream falling back to idle straight
     * after enable. Every callback here has to fill it.
     */
    if (dir != BT_AUDIO_DIR_SINK) {
        *rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_CONF_UNSUPPORTED,
                               BT_BAP_ASCS_REASON_NONE);
        return -ENOTSUP;
    }
    if (s_sink_ep != NULL) {
        *rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_NO_MEM,
                               BT_BAP_ASCS_REASON_NONE);
        return -ENOMEM;      /* single ASE in this demo */
    }

    s_sink_ep = (struct bt_bap_ep *)ep;
    *stream = &s_sink_stream;
    bt_bap_stream_cb_register(*stream, &stream_ops);

    /*
     * QoS preference handed back to the Unicast Client. These are the values
     * from the working ipro_le_audio_headset reference; two of them are not
     * cosmetic:
     *
     *   unframed_supported = true - phones drive LE Audio with unframed
     *     ISOAL. Declaring false makes the CIS unestablishable, and the
     *     symptom is subtle: the ASE walks configured -> qos-set -> enabled
     *     and then falls straight back to idle, over and over, until the
     *     phone gives up and disconnects.
     *
     *   phy = BT_BAP_QOS_CFG_2M - the BAP QoS PHY enum, NOT BT_GAP_LE_PHY_2M.
     *     They are different enumerations; using the GAP one here is what
     *     produced "PHY 0x00 not supported".
     */
    pref->unframed_supported = true;
    pref->phy         = BT_BAP_QOS_CFG_2M;
    pref->rtn         = 2;
    pref->latency     = 10;      /* ms */
    pref->pd_min      = 10000;   /* us */
    pref->pd_max      = 40000;   /* us */
    pref->pref_pd_min = 10000;   /* us */
    pref->pref_pd_max = 20000;   /* us */

    *rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
    return 0;
}

static int unicast_reconfig(struct bt_bap_stream *stream, enum bt_audio_dir dir,
                            const struct bt_audio_codec_cfg *codec_cfg,
                            struct bt_bap_qos_cfg_pref *const pref,
                            struct bt_bap_ascs_rsp *rsp)
{
    (void)stream; (void)dir; (void)codec_cfg; (void)pref;
    *rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
    return 0;
}

static int unicast_qos(struct bt_bap_stream *stream,
                       const struct bt_bap_qos_cfg *qos,
                       struct bt_bap_ascs_rsp *rsp)
{
    (void)stream; (void)qos;
    *rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
    return 0;
}

static int unicast_enable(struct bt_bap_stream *stream,
                          const uint8_t meta[], size_t meta_len,
                          struct bt_bap_ascs_rsp *rsp)
{
    (void)stream; (void)meta; (void)meta_len;
    *rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
    return 0;
}

static int unicast_start(struct bt_bap_stream *stream,
                         struct bt_bap_ascs_rsp *rsp)
{
    (void)stream;
    *rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
    return 0;
}

static int unicast_metadata(struct bt_bap_stream *stream,
                            const uint8_t meta[], size_t meta_len,
                            struct bt_bap_ascs_rsp *rsp)
{
    (void)stream; (void)meta; (void)meta_len;
    *rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
    return 0;
}

static int unicast_disable(struct bt_bap_stream *stream,
                           struct bt_bap_ascs_rsp *rsp)
{
    (void)stream;
    *rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
    return 0;
}

static int unicast_stop(struct bt_bap_stream *stream,
                        struct bt_bap_ascs_rsp *rsp)
{
    (void)stream;
    *rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
    return 0;
}

static int unicast_release(struct bt_bap_stream *stream,
                           struct bt_bap_ascs_rsp *rsp)
{
    (void)stream;
    s_sink_ep = NULL;
    *rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
    return 0;
}

static const struct bt_bap_unicast_server_cb unicast_server_cb = {
    .config   = unicast_config,
    .reconfig = unicast_reconfig,
    .qos      = unicast_qos,
    .enable   = unicast_enable,
    .start    = unicast_start,
    .metadata = unicast_metadata,
    .disable  = unicast_disable,
    .stop     = unicast_stop,
    .release  = unicast_release,
};

static int le_audio_init(void)
{
    struct bt_pacs_register_param pacs_param = {
#if defined(CONFIG_BT_PAC_SNK)
        .snk_pac = true,
#endif
#if defined(CONFIG_BT_PAC_SNK_LOC)
        .snk_loc = true,
#endif
    };
    struct bt_bap_unicast_server_register_param server_param = {
        .snk_cnt = CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT,
        .src_cnt = CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT,
    };
    int err;

    /* PACS has to exist before any capability can be published into it. */
    err = bt_pacs_register(&pacs_param);
    if (err != 0 && err != -EALREADY) {
        IPRO_LOGE(LOG_TAG, "bt_pacs_register failed (%d)", err);
        return err;
    }

    err = bt_pacs_cap_register(BT_AUDIO_DIR_SINK, &lc3_sink_cap);
    if (err != 0) {
        IPRO_LOGE(LOG_TAG, "sink capability register failed (%d)", err);
        return err;
    }

    err = bt_pacs_set_location(BT_AUDIO_DIR_SINK,
                               BT_AUDIO_LOCATION_FRONT_LEFT);
    if (err != 0) {
        IPRO_LOGE(LOG_TAG, "sink location failed (%d)", err);
        return err;
    }

    err = bt_pacs_set_supported_contexts(
              BT_AUDIO_DIR_SINK,
              BT_AUDIO_CONTEXT_TYPE_MEDIA |
                  BT_AUDIO_CONTEXT_TYPE_CONVERSATIONAL);
    if (err != 0) {
        IPRO_LOGE(LOG_TAG, "supported contexts failed (%d)", err);
        return err;
    }

    err = bt_pacs_set_available_contexts(
              BT_AUDIO_DIR_SINK,
              BT_AUDIO_CONTEXT_TYPE_MEDIA |
                  BT_AUDIO_CONTEXT_TYPE_CONVERSATIONAL);
    if (err != 0) {
        IPRO_LOGE(LOG_TAG, "available contexts failed (%d)", err);
        return err;
    }

    err = bt_bap_unicast_server_register(&server_param);
    if (err != 0 && err != -EALREADY) {
        IPRO_LOGE(LOG_TAG, "unicast server register failed (%d)", err);
        return err;
    }

    err = bt_bap_unicast_server_register_cb(&unicast_server_cb);
    if (err != 0 && err != -EALREADY) {
        IPRO_LOGE(LOG_TAG, "unicast server callbacks failed (%d)", err);
        return err;
    }

    err = vcp_register();
    if (err != 0 && err != -EALREADY) {
        /* Not fatal - audio still plays, the phone just cannot set the level. */
        IPRO_LOGW(LOG_TAG, "VCP renderer registration failed (%d) - the phone's "
                           "volume control will not reach us", err);
    } else {
        IPRO_LOGI(LOG_TAG, "VCP renderer registered (volume 0-255 -> %+d..0 dB)",
                  VCP_GAIN_MIN_DB);
    }

    /*
     * Read the published rates back out of the capability rather than printing
     * a hardcoded list - `bt rate` patches this struct at runtime, and a fixed
     * string would keep claiming 16/32/48 whatever it was actually set to.
     */
    {
        int f = bt_audio_codec_cap_get_freq(&lc3_sink_codec_cap);
        char rates[32] = "";

        if (f > 0) {
            if (f & BT_AUDIO_CODEC_CAP_FREQ_16KHZ) strcat(rates, "16 ");
            if (f & BT_AUDIO_CODEC_CAP_FREQ_24KHZ) strcat(rates, "24 ");
            if (f & BT_AUDIO_CODEC_CAP_FREQ_32KHZ) strcat(rates, "32 ");
            if (f & BT_AUDIO_CODEC_CAP_FREQ_48KHZ) strcat(rates, "48 ");
        }
        IPRO_LOGI(LOG_TAG, "LE Audio sink ready: LC3 %skHz, %d sink ASE",
                  rates[0] ? rates : "(none) ",
                  CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT);
    }
    return 0;
}
#else
static int le_audio_init(void) { return 0; }
#endif /* CONFIG_BT_AUDIO && CONFIG_BT_BAP_UNICAST_SERVER */

/* ---------------------------------------------------------------------------
 * Advertising
 * -------------------------------------------------------------------------*/

/* ASCS is what an LE Audio source scans for; GAP appearance marks us as a
 * generic audio sink so a phone shows the right icon. */
#if defined(CONFIG_BT_AUDIO)
static const uint8_t ad_uuid16[] = {
    BT_UUID_16_ENCODE(BT_UUID_ASCS_VAL),
    BT_UUID_16_ENCODE(BT_UUID_PACS_VAL),
};
#endif

int bt_app_adv_start(void)
{
    struct bt_data ad[3];
    size_t n = 0;
    int err;

    if (!s_ready) {
        return BT_APP_ERR_STATE;
    }
    if (s_conn != NULL) {
        IPRO_LOGW(LOG_TAG, "already connected; not advertising");
        return BT_APP_ERR_STATE;
    }

    ad[n++] = (struct bt_data)BT_DATA_BYTES(BT_DATA_FLAGS,
                  (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR));
    ad[n++] = (struct bt_data)BT_DATA(BT_DATA_NAME_COMPLETE,
                                      s_name, (uint8_t)strlen(s_name));
#if defined(CONFIG_BT_AUDIO)
    ad[n++] = (struct bt_data)BT_DATA(BT_DATA_UUID16_ALL,
                                      ad_uuid16, sizeof(ad_uuid16));
#endif

    err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ad, n, NULL, 0);
    if (err) {
        IPRO_LOGE(LOG_TAG, "bt_le_adv_start failed (%d)", err);
        return err;
    }
    s_advertising = true;
    s_adv_wanted  = true;
    {
        bt_addr_le_t addrs[1];
        size_t count = 1;
        char buf[BT_ADDR_LE_STR_LEN] = "unknown";

        bt_id_get(addrs, &count);
        if (count > 0) {
            bt_addr_le_to_str(&addrs[0], buf, sizeof(buf));
        }
        IPRO_LOGI(LOG_TAG, "advertising as \"%s\" from %s - connectable, "
                           "%u AD records", s_name, buf, (unsigned)n);
    }
    return BT_APP_OK;
}

int bt_app_adv_stop(void)
{
    int err;

    s_adv_wanted = false;   /* an explicit stop survives the next disconnect */

    if (!s_advertising) {
        return BT_APP_OK;
    }
    err = bt_le_adv_stop();
    if (err) {
        IPRO_LOGE(LOG_TAG, "bt_le_adv_stop failed (%d)", err);
        return err;
    }
    s_advertising = false;
    return BT_APP_OK;
}

int bt_app_disconnect(void)
{
    if (s_conn == NULL) {
        return BT_APP_ERR_STATE;
    }
    return bt_conn_disconnect(s_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
}

/* ---------------------------------------------------------------------------
 * Init
 * -------------------------------------------------------------------------*/

static void bt_ready(int err)
{
    if (err) {
        IPRO_LOGE(LOG_TAG, "bt_enable callback reported %d", err);
        return;
    }
    s_ready = true;
    s_status.enabled = true;

    if (le_audio_init() != 0) {
        IPRO_LOGE(LOG_TAG, "LE Audio init failed; plain BLE still usable");
    }
    if (bt_shell_init() != 0) {
        IPRO_LOGW(LOG_TAG, "shell over BLE unavailable - peripherals can still "
                           "be driven from the UART console");
    }

    IPRO_LOGI(LOG_TAG, "host ready");
}

/*
 * Bring the radio up at boot, on its own task.
 *
 * Every remote client - the Android app, tools/evk.py, the web panel - can only
 * reach the board once it is advertising, and requiring `bt init` over the
 * serial console first defeats the point of controlling it wirelessly.
 *
 * It runs detached because bringing the controller up is slow: RF calibration
 * and the 32 kHz qualification together take well over ten seconds, and none of
 * that should stand between power-on and a working panel or shell.
 */
static void bt_autostart_task(void *arg)
{
    int rc;

    (void)arg;

    rc = bt_app_init();
    if (rc == BT_APP_OK) {
        IPRO_LOGI(LOG_TAG, "advertising as \"%s\" - the Android app, "
                           "tools/evk.py and the web panel can connect now",
                  bt_app_get_name());
    } else {
        IPRO_LOGE(LOG_TAG, "auto-start failed (%d) - `bt init` retries it", rc);
    }
    vTaskDelete(NULL);
}

void bt_app_autostart(void)
{
    /* Same stack the shell gives a command, since bt_app_init() is what the
     * `bt init` command runs. */
    if (xTaskCreate(bt_autostart_task, "bt_boot", 1536, NULL,
                    configMAX_PRIORITIES - 6, NULL) != pdPASS) {
        IPRO_LOGE(LOG_TAG, "could not start the radio at boot");
    }
}

int bt_app_init(void)
{
    int err;

    if (s_enabled) {
        return BT_APP_OK;
    }

    /*
     * Bring-up order for the new stack. Calling bt_enable() on its own is not
     * enough and fails in a way that looks like dead hardware: HCI_Reset goes
     * out on the wire, nothing answers, and bt_enable() blocks forever.
     *
     *   1. ipro_ble_ctlr_controller_init() starts the RivieraWaves controller.
     *   2. hci_driver_ipro_ble_ctlr_init() binds the H4 byte-stream transport
     *      between host and controller.
     *   3. Only then bt_enable().
     *
     * The delays between the steps are from the SDK's own reference
     * (apps/bluetooth/ble/ipro_ble_controller_central_test).
     */
    IPRO_LOGI(LOG_TAG, "starting controller...");
    err = ipro_ble_ctlr_controller_init(configMAX_PRIORITIES - 1);
    if (err != 0) {
        IPRO_LOGE(LOG_TAG, "controller init failed (%d)", err);
        return BT_APP_ERR_HW;
    }
    vTaskDelay(pdMS_TO_TICKS(50));

    IPRO_LOGI(LOG_TAG, "starting HCI transport...");

    /*
     * Read the calibration verdict AFTER the controller starts - RF calibration
     * runs inside controller init, and reading before it just returns an empty
     * register. It fails on a good fraction of boots and a failed one costs
     * receive sensitivity, which shows up as ISO packet loss rather than as any
     * kind of error, so every log wants this line. `bt rf` has the detail.
     */
    {
        uint32_t st  = *(volatile uint32_t *)(RF_BASE + RF_CAL_STATUS_OFFSET);
        uint32_t ros = (st >> RF_ROSCAL_STATUS_POS) & 0x3u;

        if (ros == 3u) {
            IPRO_LOGI(LOG_TAG, "RF roscal passed (RF_CAL_STATUS 0x%08lX)",
                      (unsigned long)st);
        } else {
            IPRO_LOGW(LOG_TAG, "RF roscal did not converge (status %lu, "
                               "RF_CAL_STATUS 0x%08lX) - receiver is "
                               "uncalibrated, expect ISO packet loss. "
                               "`bt rf` shows how bad.",
                      (unsigned long)ros, (unsigned long)st);
        }
    }

    err = hci_driver_ipro_ble_ctlr_init();
    if (err != 0) {
        IPRO_LOGE(LOG_TAG, "HCI driver init failed (%d)", err);
        return BT_APP_ERR_HW;
    }
    vTaskDelay(pdMS_TO_TICKS(50));

    bt_conn_cb_register(&conn_callbacks);

    /* NULL callback: this host runs bt_enable() synchronously, which is how
     * the reference app uses it - no polling for readiness afterwards. */
    err = bt_enable(NULL);
    if (err) {
        IPRO_LOGE(LOG_TAG, "bt_enable failed (%d)", err);
        return BT_APP_ERR_HW;
    }
    s_enabled = true;

    /*
     * Two steps the reference app does between bt_enable() and registering
     * anything, both of which matter:
     *
     *   - hcitl_host_ready() is the IPRO handshake telling the controller the
     *     host has finished coming up.
     *   - settings_load_subtree("bt") is what gives the device its identity
     *     address. Without it the host logs "No ID address. App must call
     *     settings_load()" and reports 00:00:00:00:00:00, and GATT service
     *     registration (bt_pacs_register) fails with -ENOEXEC.
     */
    ipro_ble_ctlr_hcitl_host_ready();

#if defined(CONFIG_BT_SETTINGS)
    err = settings_load_subtree("bt");
    if (err != 0) {
        IPRO_LOGE(LOG_TAG, "settings_load_subtree(\"bt\") failed (%d)", err);
        return BT_APP_ERR_HW;
    }
#endif

    bt_ready(0);

    /* Start advertising straight away. `bt init` on its own leaving the device
     * invisible is a two-step that is easy to miss and gives no clue from the
     * output - it just does not appear in a scan. `bt adv off` stops it. */
    if (bt_app_adv_start() != BT_APP_OK) {
        IPRO_LOGE(LOG_TAG, "host is up but advertising did not start");
    }
    return BT_APP_OK;
}

int bt_app_set_name(const char *name)
{
    if (name == NULL || name[0] == '\0') {
        return BT_APP_ERR_STATE;
    }
    strncpy(s_name, name, sizeof(s_name) - 1);
    s_name[sizeof(s_name) - 1] = '\0';
    return BT_APP_OK;
}

const char *bt_app_get_name(void) { return s_name; }

const char *bt_app_stream_state_name(bt_stream_state_t s)
{
    switch (s) {
        case BT_STREAM_CONFIGURED: return "configured";
        case BT_STREAM_QOS_SET:    return "qos-set";
        case BT_STREAM_ENABLED:    return "enabled";
        case BT_STREAM_STREAMING:  return "STREAMING";
        case BT_STREAM_STOPPED:    return "stopped";
        default:                   return "idle";
    }
}

void bt_app_get_status(bt_status_t *out)
{
    if (out == NULL) {
        return;
    }
    s_status.enabled     = s_ready;
    s_status.advertising = s_advertising;

    if (s_ready) {
        bt_addr_le_t addrs[1];
        size_t count = 1;
        bt_id_get(addrs, &count);
        if (count > 0) {
            memcpy(s_status.own_addr, addrs[0].a.val, 6);
        }
    }
    *out = s_status;
}

#endif /* CONFIG_BLUETOOTH_LE_ENABLE */

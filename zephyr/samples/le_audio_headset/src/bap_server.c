/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * BAP unicast server (sink): PACS capability, ASCS callbacks, stream ops.
 * Codec capability as the FreeRTOS headset: LC3 16/32/48 kHz, 7.5/10 ms,
 * 1 channel, 26..155 octets (48_6 at 124 kbps is the top mono set); a phone
 * usually picks 48_2 or 48_4 for media, 32_2/16_2 for calls.
 */
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/audio/bap.h>
#include <zephyr/bluetooth/audio/pacs.h>
#include <zephyr/bluetooth/audio/lc3.h>
#include <zephyr/logging/log.h>
#include "audio.h"

LOG_MODULE_REGISTER(bap_server, CONFIG_LOG_DEFAULT_LEVEL);

static const struct bt_audio_codec_cap lc3_sink_codec_cap = BT_AUDIO_CODEC_CAP_LC3(
	BT_AUDIO_CODEC_CAP_FREQ_16KHZ | BT_AUDIO_CODEC_CAP_FREQ_32KHZ |
		BT_AUDIO_CODEC_CAP_FREQ_48KHZ,
	BT_AUDIO_CODEC_CAP_DURATION_7_5 | BT_AUDIO_CODEC_CAP_DURATION_10 |
		BT_AUDIO_CODEC_CAP_DURATION_PREFER_10,
	BT_AUDIO_CODEC_CAP_CHAN_COUNT_SUPPORT(1), 26, 155, 1,
	BT_AUDIO_CONTEXT_TYPE_MEDIA | BT_AUDIO_CONTEXT_TYPE_CONVERSATIONAL);

static struct bt_pacs_cap lc3_sink_cap = {.codec_cap = &lc3_sink_codec_cap};

static struct bt_bap_stream streams[CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT];
static bool stream_used[CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT];
static struct bt_bap_stream_ops stream_ops;
static uint32_t rx_sdus, rx_lost;

static void fill_qos_pref(struct bt_bap_qos_cfg_pref *pref)
{
	pref->unframed_supported = true;
	pref->phy = BT_BAP_QOS_CFG_2M;
	pref->rtn = 2;
	pref->latency = 10;
	pref->pd_min = 10000;
	pref->pd_max = 40000;
	pref->pref_pd_min = 10000;
	pref->pref_pd_max = 20000;
}

static struct bt_bap_stream *stream_alloc(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(streams); i++) {
		if (!stream_used[i]) {
			stream_used[i] = true;
			return &streams[i];
		}
	}
	return NULL;
}

static void stream_free(struct bt_bap_stream *stream)
{
	for (size_t i = 0; i < ARRAY_SIZE(streams); i++) {
		if (&streams[i] == stream) {
			stream_used[i] = false;
		}
	}
}

static int apply_codec_cfg(const struct bt_audio_codec_cfg *codec_cfg)
{
	int freq = bt_audio_codec_cfg_get_freq(codec_cfg);
	int dur = bt_audio_codec_cfg_get_frame_dur(codec_cfg);
	int octets = bt_audio_codec_cfg_get_octets_per_frame(codec_cfg);

	if (freq < 0 || dur < 0 || octets <= 0) {
		LOG_WRN("codec config incomplete (freq %d dur %d octets %d)", freq, dur, octets);
		return -EINVAL;
	}
	return audio_configure(bt_audio_codec_cfg_freq_to_freq_hz(freq),
			       bt_audio_codec_cfg_frame_dur_to_frame_dur_us(dur), (uint16_t)octets);
}

static int cb_config(struct bt_conn *conn, const struct bt_bap_ep *ep, enum bt_audio_dir dir,
		     const struct bt_audio_codec_cfg *codec_cfg, struct bt_bap_stream **stream,
		     struct bt_bap_qos_cfg_pref *const pref, struct bt_bap_ascs_rsp *rsp)
{
	ARG_UNUSED(conn); ARG_UNUSED(ep);
	if (dir != BT_AUDIO_DIR_SINK) {
		*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_CONF_UNSUPPORTED,
				       BT_BAP_ASCS_REASON_NONE);
		return -ENOTSUP;
	}
	*stream = stream_alloc();
	if (*stream == NULL) {
		*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_NO_MEM, BT_BAP_ASCS_REASON_NONE);
		return -ENOMEM;
	}
	bt_bap_stream_cb_register(*stream, &stream_ops);
	LOG_INF("ASE config: stream %p", (void *)*stream);
	if (codec_cfg != NULL && apply_codec_cfg(codec_cfg) != 0) {
		stream_free(*stream);
		*stream = NULL;
		*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_CONF_UNSUPPORTED,
				       BT_BAP_ASCS_REASON_CODEC_DATA);
		return -ENOTSUP;
	}
	fill_qos_pref(pref);
	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

static int cb_reconfig(struct bt_bap_stream *stream, enum bt_audio_dir dir,
		       const struct bt_audio_codec_cfg *codec_cfg,
		       struct bt_bap_qos_cfg_pref *const pref, struct bt_bap_ascs_rsp *rsp)
{
	ARG_UNUSED(dir);
	LOG_INF("ASE reconfig: stream %p", (void *)stream);
	if (codec_cfg != NULL) {
		(void)apply_codec_cfg(codec_cfg);
	}
	fill_qos_pref(pref);
	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

static int cb_qos(struct bt_bap_stream *stream, const struct bt_bap_qos_cfg *qos,
		  struct bt_bap_ascs_rsp *rsp)
{
	LOG_INF("ASE QoS: stream %p interval %u framing %u phy %u sdu %u rtn %u latency %u pd %u",
		(void *)stream, qos->interval, qos->framing, qos->phy, qos->sdu, qos->rtn,
		qos->latency, qos->pd);
	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

static int cb_enable(struct bt_bap_stream *stream, const uint8_t meta[], size_t meta_len,
		     struct bt_bap_ascs_rsp *rsp)
{
	ARG_UNUSED(meta); ARG_UNUSED(meta_len);
	LOG_INF("ASE enable: stream %p", (void *)stream);
	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

static int cb_start(struct bt_bap_stream *stream, struct bt_bap_ascs_rsp *rsp)
{
	LOG_INF("ASE start: stream %p", (void *)stream);
	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

static int cb_metadata(struct bt_bap_stream *stream, const uint8_t meta[], size_t meta_len,
		       struct bt_bap_ascs_rsp *rsp)
{
	ARG_UNUSED(meta);
	LOG_INF("ASE metadata: stream %p (%zu B)", (void *)stream, meta_len);
	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

static int cb_disable(struct bt_bap_stream *stream, struct bt_bap_ascs_rsp *rsp)
{
	LOG_INF("ASE disable: stream %p", (void *)stream);
	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

static int cb_stop(struct bt_bap_stream *stream, struct bt_bap_ascs_rsp *rsp)
{
	LOG_INF("ASE stop: stream %p", (void *)stream);
	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

static int cb_release(struct bt_bap_stream *stream, struct bt_bap_ascs_rsp *rsp)
{
	LOG_INF("ASE release: stream %p", (void *)stream);
	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

static const struct bt_bap_unicast_server_cb unicast_server_cb = {
	.config = cb_config, .reconfig = cb_reconfig, .qos = cb_qos, .enable = cb_enable,
	.start = cb_start, .metadata = cb_metadata, .disable = cb_disable, .stop = cb_stop,
	.release = cb_release,
};

/* ---- stream ops ------------------------------------------------------- */

static void stream_enabled(struct bt_bap_stream *stream)
{
	/* Sink ASE: tell the client the receiver is ready so the ASE moves to
	 * Streaming once the CIS is up. */
	int err = bt_bap_stream_start(stream);

	LOG_INF("stream %p enabled, receiver start: %d", (void *)stream, err);
}

static void stream_started(struct bt_bap_stream *stream)
{
	LOG_INF("stream %p started", (void *)stream);
	rx_sdus = 0; rx_lost = 0;
	(void)audio_start();
}

static void stream_stopped(struct bt_bap_stream *stream, uint8_t reason)
{
	LOG_INF("stream %p stopped (0x%02x), %u SDUs, %u lost", (void *)stream, reason, rx_sdus,
		rx_lost);
	audio_stop();
}

static void stream_released(struct bt_bap_stream *stream)
{
	LOG_INF("stream %p released", (void *)stream);
	audio_stop();
	stream_free(stream);
}

static void stream_recv(struct bt_bap_stream *stream, const struct bt_iso_recv_info *info,
			struct net_buf *buf)
{
	ARG_UNUSED(stream);
	rx_sdus++;
	if ((info->flags & BT_ISO_FLAGS_VALID) == 0) {
		rx_lost++;
	}
	audio_push(info, buf);
}

static struct bt_bap_stream_ops stream_ops = {
	.enabled = stream_enabled,
	.started = stream_started,
	.stopped = stream_stopped,
	.released = stream_released,
	.recv = stream_recv,
};

int bap_server_init(void)
{
	struct bt_pacs_register_param pacs_param = {.snk_pac = true, .snk_loc = true};
	struct bt_bap_unicast_server_register_param server_param = {
		.snk_cnt = CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT,
		.src_cnt = CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT,
	};
	int err;

	err = bt_pacs_register(&pacs_param);
	if (err != 0 && err != -EALREADY) {
		LOG_ERR("PACS register: %d", err);
		return err;
	}
	err = bt_pacs_cap_register(BT_AUDIO_DIR_SINK, &lc3_sink_cap);
	if (err != 0) {
		LOG_ERR("PACS sink cap: %d", err);
		return err;
	}
	err = bt_pacs_set_location(BT_AUDIO_DIR_SINK, CONFIG_HEADSET_SINK_LOCATION);
	if (err != 0) {
		LOG_ERR("PACS sink location: %d", err);
		return err;
	}
	(void)bt_pacs_set_supported_contexts(BT_AUDIO_DIR_SINK,
					     BT_AUDIO_CONTEXT_TYPE_MEDIA |
						     BT_AUDIO_CONTEXT_TYPE_CONVERSATIONAL |
						     BT_AUDIO_CONTEXT_TYPE_UNSPECIFIED);
	(void)bt_pacs_set_available_contexts(BT_AUDIO_DIR_SINK,
					     BT_AUDIO_CONTEXT_TYPE_MEDIA |
						     BT_AUDIO_CONTEXT_TYPE_CONVERSATIONAL |
						     BT_AUDIO_CONTEXT_TYPE_UNSPECIFIED);
	err = bt_bap_unicast_server_register(&server_param);
	if (err != 0 && err != -EALREADY) {
		LOG_ERR("ASCS register: %d", err);
		return err;
	}
	err = bt_bap_unicast_server_register_cb(&unicast_server_cb);
	if (err != 0) {
		LOG_ERR("BAP callbacks: %d", err);
		return err;
	}
	LOG_INF("BAP unicast server ready (%u sink ASE)", CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT);
	return 0;
}

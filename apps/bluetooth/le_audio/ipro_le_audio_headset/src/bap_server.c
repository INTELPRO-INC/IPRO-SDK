/**
 * @file bap_server.c
 * @brief BAP Unicast Server Implementation (Phase 2)
 *
 * Implements Bluetooth Audio Profile (BAP) Unicast Server callbacks
 * for Audio Stream Endpoint (ASE) management and audio streaming.
 *
 * Copyright (C) IPRO SDK 2026
 */

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/audio/bap.h>
#include <zephyr/bluetooth/audio/pacs.h>
#include <zephyr/bluetooth/audio/lc3.h>
#include "bap_unicast_server.h"

#include "ipro_log.h"
#include "utils_crc.h"
#include "ble_init.h"
#include "app_audio.h"
#include "iso_content_validator.h"
#include <errno.h>
#include <FreeRTOS.h>
#include <task.h>

#define LOG_TAG "BAP_SERVER"

#if defined(CONFIG_BSP_USING_DAC) || defined(CONFIG_AUDIO_OUTPUT_UAC)
#define IPRO_LE_AUDIO_OUTPUT_ENABLED 1
#else
#define IPRO_LE_AUDIO_OUTPUT_ENABLED 0
#endif

/*
 * LC3 Sink Codec Capability:
 *   - Supported frequencies: 16kHz, 32kHz, 48kHz
 *   - Supported frame durations: 7.5ms, 10ms (prefer 10ms)
 *   - Supported channel count: 1
 *   - Supported octets per codec frame: 26-155
 *
 * 155 is LC3 48_6 at 10 ms (124 kbps), the highest mono configuration LE Audio
 * defines.  The cap used to be 120, which pinned every negotiation to 48_4 at
 * 96 kbps regardless of what the source could do.  Nothing downstream needed
 * widening: BT_ISO_RX_MTU is 251 and the app's frame ring already carries 160
 * bytes per entry.  A source that cannot do 48_5/48_6 simply picks 48_4 as
 * before.
 *   - Max frames per SDU: 1
 */
static const struct bt_audio_codec_cap lc3_sink_codec_cap =
	BT_AUDIO_CODEC_CAP_LC3(BT_AUDIO_CODEC_CAP_FREQ_16KHZ |
				       BT_AUDIO_CODEC_CAP_FREQ_32KHZ |
				       BT_AUDIO_CODEC_CAP_FREQ_48KHZ,
			       BT_AUDIO_CODEC_CAP_DURATION_7_5 |
				       BT_AUDIO_CODEC_CAP_DURATION_10 |
				       BT_AUDIO_CODEC_CAP_DURATION_PREFER_10,
			       BT_AUDIO_CODEC_CAP_CHAN_COUNT_SUPPORT(1),
			       26, 155, 1,
			       BT_AUDIO_CONTEXT_TYPE_MEDIA |
				       BT_AUDIO_CONTEXT_TYPE_CONVERSATIONAL);

static struct bt_pacs_cap lc3_sink_cap = {
	.codec_cap = &lc3_sink_codec_cap,
};

/* Maximum number of streams supported */
#define MAX_STREAMS 2

/* Keep LE Audio RX hot path low-overhead.  Full controller CIS trace dumps from
 * the BAP callback perturb 10 ms ISO traffic at 115200 baud and can create or
 * amplify the very loss pattern being measured.  Counters are still maintained;
 * enable this only for short targeted captures.
 */
#ifndef IPRO_LE_AUDIO_RX_HOTPATH_DEBUG
#define IPRO_LE_AUDIO_RX_HOTPATH_DEBUG 0
#endif

/* Audio stream pool */
static struct bt_bap_stream streams[MAX_STREAMS];
static bool stream_allocated[MAX_STREAMS];

/* Forward declarations */
static struct bt_bap_stream_ops stream_ops;
void bap_server_reset_rx_counters(void);

/* Negotiated transport contract and long-run payload integrity counters.
 * The controller/host only marks an SDU VALID after the on-air CRC succeeds;
 * these counters additionally prove that the bytes delivered to the APP have
 * the negotiated length and are not empty/all-zero data.
 */
static uint16_t s_codec_octets;
static uint16_t s_qos_sdu;
static uint32_t s_rx_cnt;
static uint16_t s_last_seq_num;
static uint32_t s_total_dropped;
static uint32_t s_loss_event_cnt;
static uint32_t s_valid_payload_cnt;
static uint32_t s_startup_no_data_cnt;
static uint32_t s_midstream_no_data_cnt;
static uint32_t s_trailing_no_data_cnt;
static uint32_t s_pending_no_data_cnt;
static uint32_t s_startup_valid_streak;
static uint32_t s_bad_len_cnt;
static uint32_t s_invalid_status_cnt;
static uint32_t s_empty_payload_cnt;
static uint32_t s_all_zero_payload_cnt;
static uint64_t s_nonzero_byte_cnt;
static uint32_t s_payload_stream_crc;
static bool s_seen_payload;
static bool s_pattern_locked;
static uint8_t s_pattern_probe_streak;
static uint32_t s_pattern_checked_cnt;
static uint32_t s_pattern_mismatch_cnt;

#define ISO_PAYLOAD_STABLE_STREAK 10U

__attribute__((weak)) void ipro_ble_ctlr_cis_debug_dump(void)
{
}

__attribute__((weak)) void ipro_ble_ctlr_iso_hop_debug_dump(void)
{
}

static void fill_qos_pref(struct bt_bap_qos_cfg_pref *pref)
{
	pref->unframed_supported = true;
	pref->phy = BT_BAP_QOS_CFG_2M;
	/* Keep the acceptor preference on the Android native baseline that passed
	 * r99/r100.  The sink CAP still allows 120-octet frames; forced 48 kHz /
	 * 20 ms / 120 B stress is driven by the dual-board central shell QoS
	 * command instead of making phones prefer the raised diagnostic condition.
	 */
	pref->rtn = 2;
	pref->latency = 10; /* ms */
	pref->pd_min = 10000; /* us */
	pref->pd_max = 40000; /* us */
	pref->pref_pd_min = 10000; /* us */
	pref->pref_pd_max = 20000; /* us */
}

/**
 * @brief Find an available stream from the pool
 */
static struct bt_bap_stream *stream_alloc(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(streams); i++) {
		if (!stream_allocated[i]) {
			stream_allocated[i] = true;
			return &streams[i];
		}
	}

	IPRO_LOGE(LOG_TAG, "No free streams available");
	return NULL;
}

/**
 * @brief Release a stream back to the pool
 */
static void stream_free(struct bt_bap_stream *stream)
{
	for (size_t i = 0; i < ARRAY_SIZE(streams); i++) {
		if (&streams[i] == stream) {
			stream_allocated[i] = false;
			IPRO_LOGI(LOG_TAG, "Stream %zu freed", i);
			return;
		}
	}
}

/**
 * @brief Config callback - called when client configures an ASE
 */
static int bap_server_config(struct bt_conn *conn, const struct bt_bap_ep *ep,
			     enum bt_audio_dir dir,
			     const struct bt_audio_codec_cfg *codec_cfg,
			     struct bt_bap_stream **stream,
			     struct bt_bap_qos_cfg_pref *const pref,
			     struct bt_bap_ascs_rsp *rsp)
{
	IPRO_LOGI(LOG_TAG, "ASE Codec Config: conn=%p ep=%p dir=%d", conn, ep, dir);

	*stream = stream_alloc();
	if (*stream == NULL) {
		IPRO_LOGE(LOG_TAG, "No free streams");
		*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_NO_MEM, BT_BAP_ASCS_REASON_NONE);
		return -ENOMEM;
	}

	IPRO_LOGI(LOG_TAG, "ASE Codec Config: stream=%p allocated", *stream);

	/* Register stream callbacks */
	bt_bap_stream_cb_register(*stream, &stream_ops);

	/* Extract codec parameters and reconfigure audio decoder */
	if (codec_cfg) {
		IPRO_LOGI(LOG_TAG, "Codec: id=0x%02x data_len=%zu meta_len=%zu path_id=%u",
			  codec_cfg->id, codec_cfg->data_len, codec_cfg->meta_len,
			  codec_cfg->path_id);
		for (size_t i = 0; i < codec_cfg->data_len; i++) {
			IPRO_LOGI(LOG_TAG, "  data[%zu]=0x%02x", i, codec_cfg->data[i]);
		}

		int freq_enum = bt_audio_codec_cfg_get_freq(codec_cfg);
		int frame_dur_enum = bt_audio_codec_cfg_get_frame_dur(codec_cfg);
		int octets = bt_audio_codec_cfg_get_octets_per_frame(codec_cfg);
		int freq = freq_enum >= 0 ?
			bt_audio_codec_cfg_freq_to_freq_hz(freq_enum) : freq_enum;
		int frame_dur = frame_dur_enum >= 0 ?
			bt_audio_codec_cfg_frame_dur_to_frame_dur_us(frame_dur_enum) :
			frame_dur_enum;

		IPRO_LOGI(LOG_TAG, "Codec parsed: freq=%d Hz, dur=%d us, octets=%d",
			  freq, frame_dur, octets);
		if (octets > 0 && octets <= UINT16_MAX) {
			s_codec_octets = (uint16_t)octets;
		}

		if (freq > 0 && frame_dur > 0 && octets > 0) {
			int content_ret = iso_content_validator_configure(
				(uint32_t)freq, (uint32_t)frame_dur, (uint16_t)octets);
			IPRO_LOGI(LOG_TAG, "LC3 content validator configure: ret=%d",
				  content_ret);
#if IPRO_LE_AUDIO_OUTPUT_ENABLED
			app_audio_config_t audio_cfg = {
				.sample_rate = (uint32_t)freq,
				.frame_duration_us = (uint32_t)frame_dur,
				.octets_per_frame = (uint16_t)octets,
				.num_channels = 1,
			};
			int ret = app_audio_reconfigure(&audio_cfg);
			IPRO_LOGI(LOG_TAG, "Audio reconfigure: ret=%d (rate=%u oct=%u)",
				  ret, audio_cfg.sample_rate, audio_cfg.octets_per_frame);
#else
			IPRO_LOGI(LOG_TAG, "Audio output disabled: parsed config kept for profile-only gate");
#endif
		} else {
			IPRO_LOGW(LOG_TAG, "Could not parse codec config, using defaults");
		}
	} else {
		IPRO_LOGW(LOG_TAG, "Codec config is NULL!");
	}

	/* Set QoS preferences for sink (audio input from client) */
	fill_qos_pref(pref);

	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

/**
 * @brief Reconfig callback - called when client reconfigures codec
 */
static int bap_server_reconfig(struct bt_bap_stream *stream, enum bt_audio_dir dir,
			       const struct bt_audio_codec_cfg *codec_cfg,
			       struct bt_bap_qos_cfg_pref *const pref,
			       struct bt_bap_ascs_rsp *rsp)
{
	IPRO_LOGI(LOG_TAG, "ASE Reconfig: stream=%p dir=%d", stream, dir);
	(void)codec_cfg;

	/* Accept reconfiguration */
	fill_qos_pref(pref);
	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

/**
 * @brief QoS callback - called when client configures QoS parameters
 */
static int bap_server_qos(struct bt_bap_stream *stream, const struct bt_bap_qos_cfg *qos,
			  struct bt_bap_ascs_rsp *rsp)
{
	IPRO_LOGI(LOG_TAG, "ASE QoS: stream=%p interval=%u framing=%u phy=%u sdu=%u rtn=%u latency=%u pd=%u",
		  stream, qos->interval, qos->framing, qos->phy, qos->sdu,
		  qos->rtn, qos->latency, qos->pd);
	s_qos_sdu = qos->sdu;

	/* Accept QoS configuration */
	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

/**
 * @brief Enable callback - called when client enables streaming
 */
static int bap_server_enable(struct bt_bap_stream *stream,
			     const uint8_t meta[],
			     size_t meta_len,
			     struct bt_bap_ascs_rsp *rsp)
{
	(void)meta;

	IPRO_LOGI(LOG_TAG, "ASE Enable: stream=%p meta_len=%zu", stream, meta_len);

	/* Accept enable request */
	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

/**
 * @brief Start callback - called when client starts streaming
 */
static int bap_server_start(struct bt_bap_stream *stream, struct bt_bap_ascs_rsp *rsp)
{
	IPRO_LOGI(LOG_TAG, "ASE Start: stream=%p", stream);

	/* TODO Phase 3: Initialize audio decoder and connect to DAC */
	
	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

/**
 * @brief Metadata callback - called when client updates metadata
 */
static int bap_server_metadata(struct bt_bap_stream *stream,
			       const uint8_t meta[],
			       size_t meta_len,
			       struct bt_bap_ascs_rsp *rsp)
{
	(void)meta;

	IPRO_LOGI(LOG_TAG, "ASE Metadata: stream=%p meta_len=%zu", stream, meta_len);

	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

/**
 * @brief Disable callback - called when client disables streaming
 */
static int bap_server_disable(struct bt_bap_stream *stream, struct bt_bap_ascs_rsp *rsp)
{
	IPRO_LOGI(LOG_TAG, "ASE Disable: stream=%p", stream);

	/* TODO Phase 3: Stop audio decoder */

	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

/**
 * @brief Stop callback - called when client stops streaming
 */
static int bap_server_stop(struct bt_bap_stream *stream, struct bt_bap_ascs_rsp *rsp)
{
	IPRO_LOGI(LOG_TAG, "ASE Stop: stream=%p", stream);

	/* TODO Phase 3: Flush audio buffers */

	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

/**
 * @brief Release callback - called when client releases ASE
 */
static int bap_server_release(struct bt_bap_stream *stream, struct bt_bap_ascs_rsp *rsp)
{
	IPRO_LOGI(LOG_TAG, "ASE Release: stream=%p", stream);

	/* Stream will be freed in stream_released callback */

	*rsp = BT_BAP_ASCS_RSP(BT_BAP_ASCS_RSP_CODE_SUCCESS, BT_BAP_ASCS_REASON_NONE);
	return 0;
}

/*
 * BAP Stream callbacks
 */
static void stream_configured(struct bt_bap_stream *stream,
			      const struct bt_bap_qos_cfg_pref *pref)
{
	(void)pref;

	IPRO_LOGI(LOG_TAG, "Stream %p configured", stream);
}

static void stream_qos_set(struct bt_bap_stream *stream)
{
	IPRO_LOGI(LOG_TAG, "Stream %p QoS set", stream);
}

static void stream_enabled(struct bt_bap_stream *stream)
{
	int err;

	IPRO_LOGI(LOG_TAG, "Stream %p enabled, starting receiver...", stream);

	/* For Sink ASEs, indicate receiver readiness.
	 * This sets receiver_ready = true so that when CIS connects,
	 * the ASE transitions from Enabling to Streaming.
	 */
	err = bt_bap_unicast_server_start(stream);
	if (err != 0) {
		IPRO_LOGE(LOG_TAG, "Failed to start receiver: %d", err);
	}
}

static void stream_started(struct bt_bap_stream *stream)
{
	IPRO_LOGI(LOG_TAG, "Stream %p started (Streaming state)", stream);

	/* Reset RX counters so packet loss tracking starts fresh each connection */
	bap_server_reset_rx_counters();
	iso_content_validator_reset();
}

static void commit_pending_no_data(void)
{
	if (s_pending_no_data_cnt == 0U) {
		return;
	}

	if (s_bad_len_cnt < 4U) {
		IPRO_LOGE(LOG_TAG, "ISO MIDSTREAM NO DATA: run=%lu",
			  (unsigned long)s_pending_no_data_cnt);
	}
	s_midstream_no_data_cnt += s_pending_no_data_cnt;
	s_bad_len_cnt += s_pending_no_data_cnt;
	s_invalid_status_cnt += s_pending_no_data_cnt;
	s_empty_payload_cnt += s_pending_no_data_cnt;
	s_pending_no_data_cnt = 0U;
}

static void stream_stopped(struct bt_bap_stream *stream, uint8_t reason)
{
	struct iso_content_stats content;

	/* Android terminates a normally released CIS with reason 0x13.  Its source
	 * stops producing payload slightly before the ASCS Release reaches us, so
	 * the controller correctly reports RXCISNPI for the final intervals.  Keep
	 * that protocol teardown separate from a no-data run which later recovers,
	 * or from any abnormal stream stop. */
	if (s_pending_no_data_cnt != 0U) {
		if (reason == 0x13U) {
			s_trailing_no_data_cnt += s_pending_no_data_cnt;
			s_pending_no_data_cnt = 0U;
		} else {
			commit_pending_no_data();
		}
	}

	IPRO_LOGI(LOG_TAG, "Stream %p stopped (reason 0x%02x)", stream, reason);
	IPRO_LOGI(LOG_TAG,
		  "ISO RX SUMMARY count=%lu valid=%lu startup_no_data=%lu "
		  "midstream_no_data=%lu trailing_no_data=%lu "
		  "expected_len=%u codec_octets=%u bad_len=%lu "
		  "invalid=%lu empty=%lu all_zero=%lu dropped=%lu loss_events=%lu "
		  "nonzero_bytes=%llu stream_crc=0x%08lx pattern=%u "
		  "pattern_checked=%lu pattern_mismatch=%lu",
		  (unsigned long)s_rx_cnt,
		  (unsigned long)s_valid_payload_cnt,
		  (unsigned long)s_startup_no_data_cnt,
		  (unsigned long)s_midstream_no_data_cnt,
		  (unsigned long)s_trailing_no_data_cnt,
		  s_qos_sdu, s_codec_octets,
		  (unsigned long)s_bad_len_cnt,
		  (unsigned long)s_invalid_status_cnt,
		  (unsigned long)s_empty_payload_cnt,
		  (unsigned long)s_all_zero_payload_cnt,
		  (unsigned long)s_total_dropped,
		  (unsigned long)s_loss_event_cnt,
		  (unsigned long long)s_nonzero_byte_cnt,
		  (unsigned long)s_payload_stream_crc,
		  s_pattern_locked ? 1U : 0U,
		  (unsigned long)s_pattern_checked_cnt,
		  (unsigned long)s_pattern_mismatch_cnt);
	iso_content_validator_wait_idle(1000U);
	iso_content_validator_get_stats(&content);
	IPRO_LOGI(LOG_TAG,
		  "ISO CONTENT SUMMARY configured=%lu submitted=%lu checked=%lu decode_ok=%lu "
		  "decode_error=%lu queue_drop=%lu pcm_samples=%lu pcm_nonzero=%lu pcm_crc=0x%08lx "
		  "pcm_min=%d pcm_max=%d positive_crossings=%lu mean_abs=%lu tone_hz_milli=%lu "
		  "active_frames=%lu silent_frames=%lu active_samples=%lu "
		  "active_crossings=%lu active_mean_abs=%lu active_tone_hz_milli=%lu",
		  (unsigned long)content.configured,
		  (unsigned long)content.submitted,
		  (unsigned long)content.checked,
		  (unsigned long)content.decode_ok,
		  (unsigned long)content.decode_error,
		  (unsigned long)content.queue_drop,
		  (unsigned long)content.pcm_samples,
		  (unsigned long)content.pcm_nonzero,
		  (unsigned long)content.pcm_crc,
		  content.pcm_min, content.pcm_max,
		  (unsigned long)content.pcm_positive_crossings,
		  (unsigned long)content.pcm_mean_abs,
		  (unsigned long)content.tone_hz_milli,
		  (unsigned long)content.pcm_active_frames,
		  (unsigned long)content.pcm_silent_frames,
		  (unsigned long)content.pcm_active_samples,
		  (unsigned long)content.pcm_active_positive_crossings,
		  (unsigned long)content.pcm_active_mean_abs,
		  (unsigned long)content.active_tone_hz_milli);
	/* Keep the complete controller trace in PSRAM for post-run CCI readback.
	 * Dumping thousands of entries over the application UART delays ASCS/CIS
	 * teardown and makes a stress run depend on console throughput. */

#if IPRO_LE_AUDIO_OUTPUT_ENABLED
	app_audio_stop();
#endif
}

static void stream_released(struct bt_bap_stream *stream)
{
	IPRO_LOGI(LOG_TAG, "Stream %p released", stream);

#if IPRO_LE_AUDIO_OUTPUT_ENABLED
	app_audio_stop();
#endif
	stream_free(stream);
}

void bap_server_reset_rx_counters(void)
{
	s_rx_cnt = 0;
	s_last_seq_num = 0;
	s_total_dropped = 0;
	s_loss_event_cnt = 0;
	s_valid_payload_cnt = 0;
	s_startup_no_data_cnt = 0;
	s_midstream_no_data_cnt = 0;
	s_trailing_no_data_cnt = 0;
	s_pending_no_data_cnt = 0;
	s_startup_valid_streak = 0;
	s_bad_len_cnt = 0;
	s_invalid_status_cnt = 0;
	s_empty_payload_cnt = 0;
	s_all_zero_payload_cnt = 0;
	s_nonzero_byte_cnt = 0;
	s_payload_stream_crc = 0;
	s_seen_payload = false;
	s_pattern_locked = false;
	s_pattern_probe_streak = 0;
	s_pattern_checked_cnt = 0;
	s_pattern_mismatch_cnt = 0;
}

static void stream_recv(struct bt_bap_stream *stream,
			const struct bt_iso_recv_info *info,
			struct net_buf *buf)
{
	uint32_t packet_crc = 0;
	uint32_t nonzero = 0;
	bool status_valid;
	bool expected_length;
	bool startup_no_data;
	bool pattern_match = true;

	s_rx_cnt++;
	status_valid = (info != NULL) &&
		((info->flags & BT_ISO_FLAGS_VALID) != 0U) &&
		((info->flags & (BT_ISO_FLAGS_ERROR | BT_ISO_FLAGS_LOST)) == 0U);
	expected_length = (s_qos_sdu == 0U) || (buf->len == s_qos_sdu);
	startup_no_data = !s_seen_payload && !status_valid && (buf->len == 0U);

	/* A Sink may enter Streaming a few ISO intervals before Android starts its
	 * media producer.  Those leading SDU_NO_DATA indications are protocol
	 * startup state, not corrupted payload.  A later no-data run is held until
	 * the next payload: recovery makes it a real midstream failure, while a
	 * normal remote release makes it expected trailing RXCISNPI teardown. */
	if (startup_no_data) {
		s_startup_no_data_cnt++;
	} else if (s_seen_payload && !status_valid && buf->len == 0U) {
		s_pending_no_data_cnt++;
	} else {
		commit_pending_no_data();
		if (buf->len == 0U) {
			s_empty_payload_cnt++;
		}
		if (!expected_length) {
			s_bad_len_cnt++;
			if (s_bad_len_cnt <= 4U) {
				IPRO_LOGE(LOG_TAG, "ISO LENGTH ERROR: got=%u expected=%u seq=%u",
					  buf->len, s_qos_sdu, info ? info->seq_num : 0U);
			}
		}
		if (!status_valid) {
			s_invalid_status_cnt++;
		}
	}

	for (uint16_t i = 0; i < buf->len; i++) {
		if (buf->data[i] != 0U) {
			nonzero++;
		}
		if ((info == NULL) ||
		    (buf->data[i] != (uint8_t)(info->seq_num + i))) {
			pattern_match = false;
		}
	}
	if (status_valid && buf->len != 0U && nonzero == 0U) {
		s_all_zero_payload_cnt++;
	}
	if (status_valid && expected_length && buf->len != 0U) {
		if (!s_seen_payload) {
			s_startup_valid_streak++;
			if (s_startup_valid_streak >= ISO_PAYLOAD_STABLE_STREAK) {
				s_seen_payload = true;
			}
		}
		s_valid_payload_cnt++;
		s_nonzero_byte_cnt += nonzero;
		packet_crc = utils_crc32(buf->data, buf->len);
		s_payload_stream_crc = utils_crc32_accumulate(
			s_payload_stream_crc, buf->data, buf->len);
		(void)iso_content_validator_check(buf->data, buf->len);

		/* The dual-board central transmits byte[i] = seq_num + i.  Lock the
		 * checker only after three complete matching SDUs, so arbitrary LC3
		 * media from Android cannot accidentally enable pattern validation. */
		if (s_pattern_locked) {
			s_pattern_checked_cnt++;
			if (!pattern_match) {
				s_pattern_mismatch_cnt++;
			}
		} else if (pattern_match) {
			s_pattern_probe_streak++;
			if (s_pattern_probe_streak == 3U) {
				s_pattern_locked = true;
				s_pattern_checked_cnt = 3U;
			}
		} else {
			s_pattern_probe_streak = 0U;
		}
	} else if (!s_seen_payload) {
		s_startup_valid_streak = 0U;
	}

	/* CRITICAL: Detect packet loss by tracking sequence numbers */
	if (info && s_rx_cnt > 1) {
		uint16_t expected_seq = (s_last_seq_num + 1) & 0xFFFF;
		if (info->seq_num != expected_seq) {
			uint32_t dropped;
			if (info->seq_num > s_last_seq_num) {
				dropped = info->seq_num - s_last_seq_num - 1;
			} else {
				/* Wrap-around case */
				dropped = (0xFFFF - s_last_seq_num) + info->seq_num;
			}
			s_total_dropped += dropped;
			s_loss_event_cnt++;
			if (s_loss_event_cnt <= 4 || (s_loss_event_cnt % 64) == 0) {
				IPRO_LOGE(LOG_TAG,
					  "PACKET LOSS! seq %u->%u (dropped %lu, total %lu, events %lu)",
					  s_last_seq_num, info->seq_num,
					  (unsigned long)dropped,
					  (unsigned long)s_total_dropped,
					  (unsigned long)s_loss_event_cnt);
			}
		}
		s_last_seq_num = info->seq_num;
	} else if (info) {
		s_last_seq_num = info->seq_num;
	}

	/* Log first 5 packets, then every 1000th to avoid slowing down processing */
	if (s_rx_cnt <= 5 || (s_rx_cnt % 1000) == 0) {
		IPRO_LOGI(LOG_TAG,
			  "ISO RX #%u: len=%u seq=%u flags=0x%02x dropped=%lu "
			  "crc=0x%08lx data=%02x%02x%02x%02x%02x%02x%02x%02x",
			  s_rx_cnt, buf->len,
			  info ? info->seq_num : 0,
			  info ? info->flags : 0,
			  (unsigned long)s_total_dropped,
			  (unsigned long)packet_crc,
			  buf->len > 0U ? buf->data[0] : 0U,
			  buf->len > 1U ? buf->data[1] : 0U,
			  buf->len > 2U ? buf->data[2] : 0U,
			  buf->len > 3U ? buf->data[3] : 0U,
			  buf->len > 4U ? buf->data[4] : 0U,
			  buf->len > 5U ? buf->data[5] : 0U,
			  buf->len > 6U ? buf->data[6] : 0U,
			  buf->len > 7U ? buf->data[7] : 0U);
#if IPRO_LE_AUDIO_RX_HOTPATH_DEBUG
		ipro_ble_ctlr_cis_debug_dump();
#endif
	}

	/* Forward LC3 encoded audio data to the audio pipeline for decoding & output */
#if IPRO_LE_AUDIO_OUTPUT_ENABLED
	if (buf->len > 0) {
		/* app_audio_process_frame() wants 0 = good frame, non-zero = run PLC.
		 * info->flags is a BT_ISO_FLAGS_* bitfield where a *healthy* packet
		 * already has BT_ISO_FLAGS_VALID (and BT_ISO_FLAGS_TS) set, so it
		 * must be translated - passing it through verbatim sent every frame
		 * down the PLC path and silenced the whole stream. */
		uint8_t rx_status = 0;

		if (info != NULL &&
		    (!(info->flags & BT_ISO_FLAGS_VALID) ||
		     (info->flags & (BT_ISO_FLAGS_ERROR | BT_ISO_FLAGS_LOST)))) {
			rx_status = 1;
		}

		app_audio_process_frame(buf->data, buf->len,
					info ? info->ts : 0,
					rx_status, 0);
	}
#else
	(void)stream;
	(void)buf;
#endif
}

static struct bt_bap_stream_ops stream_ops = {
	.configured = stream_configured,
	.qos_set = stream_qos_set,
	.enabled = stream_enabled,
	.started = stream_started,
	.stopped = stream_stopped,
	.released = stream_released,
#if defined(CONFIG_BT_AUDIO_RX)
	.recv = stream_recv,
#endif /* CONFIG_BT_AUDIO_RX */
};

/* BAP Unicast Server callbacks */
static const struct bt_bap_unicast_server_cb unicast_server_cb = {
	.config = bap_server_config,
	.reconfig = bap_server_reconfig,
	.qos = bap_server_qos,
	.enable = bap_server_enable,
	.start = bap_server_start,
	.metadata = bap_server_metadata,
	.disable = bap_server_disable,
	.stop = bap_server_stop,
	.release = bap_server_release,
};

/**
 * @brief Initialize BAP Unicast Server
 */
int bap_server_init(void)
{
	int err;
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

	IPRO_LOGI(LOG_TAG, "Initializing BAP Unicast Server");

	/* 1. Register PACS service (must be done before publishing caps) */
	err = bt_pacs_register(&pacs_param);
	if (err != 0 && err != -EALREADY) {
		IPRO_LOGE(LOG_TAG, "Failed to register PACS (err %d)", err);
		return err;
	}
	IPRO_LOGI(LOG_TAG, "PACS service registered");

	/* 2. Register LC3 sink codec capability with PACS */
	err = bt_pacs_cap_register(BT_AUDIO_DIR_SINK, &lc3_sink_cap);
	if (err != 0) {
		IPRO_LOGE(LOG_TAG, "Failed to register sink cap (err %d)", err);
		return err;
	}
	IPRO_LOGI(LOG_TAG, "LC3 sink capability registered");

	/* 3. Set audio location (mono front left for single headset) */
	err = bt_pacs_set_location(BT_AUDIO_DIR_SINK, BT_AUDIO_LOCATION_FRONT_LEFT);
	if (err != 0) {
		IPRO_LOGE(LOG_TAG, "Failed to set sink location (err %d)", err);
		return err;
	}

	/* 4. Set supported and available audio contexts */
	bt_pacs_set_supported_contexts(BT_AUDIO_DIR_SINK,
				       BT_AUDIO_CONTEXT_TYPE_MEDIA |
				       BT_AUDIO_CONTEXT_TYPE_CONVERSATIONAL |
				       BT_AUDIO_CONTEXT_TYPE_UNSPECIFIED);

	bt_pacs_set_available_contexts(BT_AUDIO_DIR_SINK,
				       BT_AUDIO_CONTEXT_TYPE_MEDIA |
				       BT_AUDIO_CONTEXT_TYPE_CONVERSATIONAL |
				       BT_AUDIO_CONTEXT_TYPE_UNSPECIFIED);

	/* 5. Register ASCS ASE instances, then BAP callbacks. */
	err = bt_bap_unicast_server_register(&server_param);
	if (err != 0 && err != -EALREADY) {
		IPRO_LOGE(LOG_TAG, "Failed to register ASCS (err %d)", err);
		return err;
	}

	err = bt_bap_unicast_server_register_cb(&unicast_server_cb);
	if (err != 0) {
		IPRO_LOGE(LOG_TAG, "Failed to register BAP callbacks (err %d)", err);
		return err;
	}

	IPRO_LOGI(LOG_TAG, "BAP Unicast Server initialized successfully");
	return 0;
}

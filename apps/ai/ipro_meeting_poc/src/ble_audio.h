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

#ifndef BLE_AUDIO_H
#define BLE_AUDIO_H

#include <stdint.h>
#include <stdbool.h>

/* --- Command IDs (phone → device) --- */
#define BLE_CMD_START_REC   0x01
#define BLE_CMD_STOP_REC    0x02
#define BLE_CMD_GET_AUDIO   0x03
#define BLE_CMD_GET_STATUS  0x04
#define BLE_CMD_SET_GAIN    0x05
#define BLE_CMD_START_STREAM 0x06
#define BLE_CMD_STOP_STREAM  0x07
#define BLE_CMD_START_VOICE_ID 0x08  /* Enable speaker recognition */
#define BLE_CMD_STOP_VOICE_ID  0x09  /* Disable speaker recognition */
#define BLE_CMD_REG_SPEAKER    0x0A  /* Register speaker, payload: name (ASCII) */
#define BLE_CMD_LIST_SPEAKERS  0x0B  /* Query registered speakers */
#define BLE_CMD_CLEAR_SPEAKERS 0x0C  /* Clear all registered speakers */
#define BLE_CMD_START_MEETING  0x0D  /* Start AUADC capture + voice ID (no LC3 stream) */
#define BLE_CMD_STOP_MEETING   0x0E  /* Stop capture + voice ID */
#define BLE_CMD_REG_PCM_START  0x0F  /* Register from phone PCM: [nlen:8][name][bytes:32LE] */
#define BLE_CMD_REG_PCM_DATA   0x10  /* PCM chunk for phone registration */
#define BLE_CMD_DELETE_SPEAKER 0x11  /* [slot:8] -> updated SPEAKER_LIST */
#define BLE_CMD_RENAME_SPEAKER 0x12  /* [slot:8][name ASCII, clamp 31] -> updated SPEAKER_LIST */
#define BLE_CMD_GET_CONFIG     0x13  /* -> BLE_RSP_CONFIG */
#define BLE_CMD_SET_CONFIG     0x14  /* [mask:8 b0=gain b1=vad b2=thr][gain:i8][vad:16LE][thr_x100:8] -> BLE_RSP_CONFIG */
#define BLE_CMD_GET_HEALTH     0x15  /* -> BLE_RSP_HEALTH */

/* --- Response IDs (device → phone) --- */
#define BLE_RSP_STATUS      0x81
#define BLE_RSP_AUDIO_HDR   0x82
#define BLE_RSP_AUDIO_DATA  0x83
#define BLE_RSP_AUDIO_DONE  0x84
#define BLE_RSP_ERROR       0x85
#define BLE_RSP_AUDIO_LC3   0x86
#define BLE_RSP_SPEAKER_ID  0x87  /* [speaker_id:8][similarity_x100:16LE][name_len:8][name...] */
#define BLE_RSP_SPEAKER_LIST 0x88 /* [count:8]{[id:8][name_len:8][name...]}... */
#define BLE_RSP_CONFIG      0x89  /* [ver:8=1][gain_db:i8][vad_rms:16LE][threshold_x100:8] */
#define BLE_RSP_HEALTH      0x8A  /* [rssi:i8,0x7F=unknown][temp_c_x10:i16LE][vbat_mv:16LE]
                                   * [model_ready:8][speaker_count:8] */

/* --- Recording status byte values --- */
#define BLE_STATUS_IDLE      0x00
#define BLE_STATUS_RECORDING 0x01
#define BLE_STATUS_SENDING   0x02
#define BLE_STATUS_STREAMING 0x03

/* Callbacks for PDM recorder integration */
typedef void (*ble_audio_start_rec_fn)(int seconds);
typedef void (*ble_audio_stop_rec_fn)(void);
typedef const int16_t *(*ble_audio_get_buf_fn)(void);
typedef uint32_t (*ble_audio_get_bytes_fn)(void);
typedef bool (*ble_audio_is_recording_fn)(void);
typedef void (*ble_audio_set_gain_fn)(int gain_db);
typedef void (*ble_audio_start_stream_fn)(void);
typedef void (*ble_audio_stop_stream_fn)(void);
typedef bool (*ble_audio_is_streaming_fn)(void);

/* Callbacks for Voice ID integration */
typedef void (*ble_audio_vi_start_fn)(void);
typedef void (*ble_audio_vi_stop_fn)(void);
typedef void (*ble_audio_vi_reg_fn)(const char *name);
typedef void (*ble_audio_vi_list_fn)(void);
typedef void (*ble_audio_vi_clear_fn)(void);
/* Start a meeting. with_stream=false keeps the original behaviour (capture +
 * voice ID only); true additionally pushes the audio over BLE so the phone can
 * transcribe it. Voice ID and streaming are independent in the capture ISR, so
 * both can run at once — the stream just costs LC3 encode + BLE bandwidth. */
typedef void (*ble_audio_meeting_start_fn)(bool with_stream);
typedef void (*ble_audio_meeting_stop_fn)(void);
typedef void (*ble_audio_reg_pcm_start_fn)(const char *name, uint32_t total_bytes);
typedef void (*ble_audio_reg_pcm_data_fn)(const uint8_t *data, uint16_t len);

/* Callbacks for speaker management / runtime config / device health */
typedef int  (*ble_audio_vi_delete_fn)(uint8_t slot);            /* 0 ok, <0 bad slot */
typedef int  (*ble_audio_vi_rename_fn)(uint8_t slot, const char *new_name);
typedef void (*ble_audio_cfg_get_fn)(int8_t *gain_db, uint16_t *vad_rms,
                                     uint8_t *threshold_x100);
typedef void (*ble_audio_cfg_set_fn)(uint8_t mask, int8_t gain_db,
                                     uint16_t vad_rms, uint8_t threshold_x100);
typedef void (*ble_audio_health_get_fn)(int16_t *temp_c_x10, uint16_t *vbat_mv,
                                        uint8_t *model_ready, uint8_t *speaker_count);

typedef struct {
    ble_audio_start_rec_fn    start_rec;
    ble_audio_stop_rec_fn     stop_rec;
    ble_audio_get_buf_fn      get_buf;
    ble_audio_get_bytes_fn    get_bytes;
    ble_audio_is_recording_fn is_recording;
    ble_audio_set_gain_fn     set_gain;
    ble_audio_start_stream_fn start_stream;
    ble_audio_stop_stream_fn  stop_stream;
    ble_audio_is_streaming_fn is_streaming;
    void *stream_queue;  /* QueueHandle_t — ISR sends int16_t* frame pointers */

    /* Voice ID callbacks (optional, NULL if not supported) */
    ble_audio_vi_start_fn     vi_start;
    ble_audio_vi_stop_fn      vi_stop;
    ble_audio_vi_reg_fn       vi_reg;
    ble_audio_vi_list_fn      vi_list;
    ble_audio_vi_clear_fn     vi_clear;
    ble_audio_meeting_start_fn meeting_start;
    ble_audio_meeting_stop_fn  meeting_stop;
    ble_audio_reg_pcm_start_fn reg_pcm_start;
    ble_audio_reg_pcm_data_fn  reg_pcm_data;

    /* Speaker management / config / health (optional, NULL if unsupported) */
    ble_audio_vi_delete_fn    vi_delete;
    ble_audio_vi_rename_fn    vi_rename;
    ble_audio_cfg_get_fn      cfg_get;
    ble_audio_cfg_set_fn      cfg_set;
    ble_audio_health_get_fn   health_get;
} ble_audio_recorder_t;

/**
 * Initialize BLE stack + NUS service + audio command protocol.
 * Must be called from a FreeRTOS task (not main before scheduler).
 */
int ble_audio_init(const ble_audio_recorder_t *recorder);

/** Notify BLE module that recording completed (sends status to phone) */
void ble_audio_notify_recording_done(void);

/** Push the current 0x81 status to the phone. Used as the "recording
 *  started — speak now" cue during device-side speaker registration. */
void ble_audio_notify_status(void);

/** Toggle LC3 encoding for streaming (true=LC3, false=raw PCM) */
void ble_audio_set_lc3(bool enable);
bool ble_audio_get_lc3(void);

/** Send speaker recognition result to phone via BLE notification */
void ble_audio_notify_speaker_id(int speaker_id, float similarity, const char *name);

/** Send speaker list to phone via BLE notification */
void ble_audio_notify_speaker_list(int count, const char **names);

#endif /* BLE_AUDIO_H */

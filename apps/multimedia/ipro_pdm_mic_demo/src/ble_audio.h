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

/* --- Response IDs (device → phone) --- */
#define BLE_RSP_STATUS      0x81
#define BLE_RSP_AUDIO_HDR   0x82
#define BLE_RSP_AUDIO_DATA  0x83
#define BLE_RSP_AUDIO_DONE  0x84
#define BLE_RSP_ERROR       0x85
#define BLE_RSP_AUDIO_LC3   0x86

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
} ble_audio_recorder_t;

/**
 * Initialize BLE stack + NUS service + audio command protocol.
 * Must be called from a FreeRTOS task (not main before scheduler).
 */
int ble_audio_init(const ble_audio_recorder_t *recorder);

/** Notify BLE module that recording completed (sends status to phone) */
void ble_audio_notify_recording_done(void);

/** Toggle LC3 encoding for streaming (true=LC3, false=raw PCM) */
void ble_audio_set_lc3(bool enable);
bool ble_audio_get_lc3(void);

/* 2M 破音驗證儀表：DMIC ISR 塞 stream_queue 失敗時呼叫(ISR-safe 純計數) */
void ble_audio_stats_producer_drop(void);

/** Did ble_audio_init() get as far as enabling the stack? */
bool ble_audio_is_started(void);

/**
 * Take the BLE link down cleanly before the radio loses power.
 *
 * Disconnects any peer with REMOTE_POWER_OFF and stops advertising, so the
 * peer learns immediately instead of waiting out a supervision timeout. Bonds
 * are untouched: the point is for the same peer to come straight back.
 *
 * Safe to call when BLE was never started, or when nothing is connected.
 *
 * @return 0
 */
int ble_audio_suspend_for_sleep(void);

#endif /* BLE_AUDIO_H */

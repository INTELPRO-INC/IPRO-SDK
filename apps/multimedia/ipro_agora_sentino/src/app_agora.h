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

#ifndef APP_AGORA_H
#define APP_AGORA_H

#include <stdint.h>
#include <stddef.h>

/* RX callback: called when decoded PCM is ready from remote */
typedef void (*agora_rx_cb_t)(const int16_t *pcm, uint32_t num_samples);

/* Statistics */
typedef struct {
    uint32_t rx_frames;
    uint32_t rx_drops;
    uint32_t tx_frames;
    uint32_t tx_errors;
} agora_stats_t;

/**
 * @brief Initialize Agora RTC SDK and join channel
 */
int app_agora_init(const char *app_id, const char *token,
                   const char *channel, uint32_t uid);

/**
 * @brief Send encoded audio data to Agora channel (queued, non-blocking)
 * @param data Encoded audio buffer (e.g. G.722)
 * @param data_len Length in bytes
 * @return 0 on success, negative on failure
 */
int app_agora_send_audio(const void *data, size_t data_len);

/**
 * @brief Check if Agora is connected to channel
 */
int app_agora_is_connected(void);

/**
 * @brief Set RX callback for decoded PCM output
 */
void app_agora_set_rx_callback(agora_rx_cb_t cb);

/**
 * @brief Check if AI agent is currently speaking
 * @return 1 if speaking, 0 if listening/idle
 */
int app_agora_is_ai_speaking(void);

/**
 * @brief Get number of remote users currently in channel
 */
uint32_t app_agora_get_remote_user_count(void);

/**
 * @brief Get RX/TX statistics
 */
void app_agora_get_stats(agora_stats_t *stats);

/**
 * @brief Leave current channel and cleanup connection
 * @return 0 on success
 */
int app_agora_leave_channel(void);

/**
 * @brief Leave current channel and join a new one
 * @return 0 on success
 */
int app_agora_rejoin(const char *app_id, const char *token,
                     const char *channel, uint32_t uid);

/**
 * @brief Create a data stream for sending messages
 * @return 0 on success, negative on failure
 */
int app_agora_create_data_stream(void);

/**
 * @brief Send data stream message
 * @param data Message payload
 * @param len Length in bytes
 * @return 0 on success, negative on failure
 */
int app_agora_send_data_stream(const char *data, size_t len);

/**
 * @brief Event notification callback type
 * @param event Event type (0=joined, 1=left, 2=user_joined, 3=user_left,
 *              4=reconnecting, 5=reconnected, 6=error)
 * @param uid Remote user ID (0 for local events)
 */
typedef void (*agora_event_cb_t)(int event, uint32_t uid);

/**
 * @brief Register event notification callback
 */
void app_agora_set_event_callback(agora_event_cb_t cb);

/**
 * @brief AI state notification callback type
 * @param state 0=idle, 1=listening, 2=thinking, 3=speaking
 */
typedef void (*agora_ai_state_cb_t)(int state);

/**
 * @brief Register AI state change callback
 */
void app_agora_set_ai_state_callback(agora_ai_state_cb_t cb);

#endif /* APP_AGORA_H */

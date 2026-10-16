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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "agora_rtc_api.h"
#include <opus.h>

/* AOSL log API (from aosl/include/api/aosl_log.h) */
extern void aosl_set_log_level(int level);
#define AOSL_LOG_DEBUG 7

#include "app_agora.h"
#include "app_json.h"
#include "app_lcd.h"
#include "utils_base64.h"

#include "app_log.h"

#define LOG_TAG "AI VOICE"

/* Audio RX queue and task */
#include "queue.h"

#define RX_QUEUE_LEN 8
#define RX_FRAME_SIZE                                                          \
    640 /* Max encoded packet size (Opus typically <200 bytes)                 \
         */

typedef struct {
    uint8_t data[RX_FRAME_SIZE];
    uint16_t len;
    uint8_t type;
} rx_audio_frame_t;

static QueueHandle_t g_rx_queue = NULL;
static TaskHandle_t g_rx_task_handle = NULL;

/* Opus encoder/decoder — kept for fallback if AUDIO_CODEC_DISABLED is needed.
 * With AUDIO_CODEC_TYPE_G722, Agora handles encode/decode internally. */
static OpusEncoder *g_opus_enc __attribute__((unused)) = NULL;
static OpusDecoder *g_opus_dec = NULL;

#define OPUS_ENC_FRAME_SAMPLES 320 /* 20ms @ 16kHz (TX) */
#define OPUS_DEC_FRAME_SAMPLES 960 /* 20ms @ 48kHz (RX) */
#define OPUS_MAX_PACKET 512

static uint8_t g_opus_enc_buf[OPUS_MAX_PACKET] __attribute__((unused));
static int16_t g_opus_dec_buf[OPUS_DEC_FRAME_SAMPLES];

/* Stream message queue and task */
#define MSG_QUEUE_LEN 8
#define MSG_MAX_SIZE 1024
#define MSG_REASSEMBLY_MAX 8192

typedef struct {
    uint32_t uid;
    int stream_id;
    uint16_t len;
    char data[MSG_MAX_SIZE];
} stream_msg_t;

static QueueHandle_t g_msg_queue = NULL;
static TaskHandle_t g_msg_task_handle = NULL;

/* RX callback */
static agora_rx_cb_t g_rx_cb = NULL;

/* Event and AI state notification callbacks */
static agora_event_cb_t g_event_notify_cb = NULL;
static agora_ai_state_cb_t g_ai_state_notify_cb = NULL;

static connection_id_t g_conn_id = CONNECTION_ID_INVALID;
static const char *g_channel = ""; /* set by app_agora_init */
static volatile int g_connected = 0;
static volatile int g_ai_speaking = 0; /* 1 when AI is speaking or thinking */
static volatile TickType_t g_ai_mute_until =
    0; /* holdoff: keep TX muted until this tick */
#define AI_TX_HOLDOFF_MS 500
static volatile uint32_t g_remote_user_count = 0;
static volatile uint32_t g_send_count = 0;
static volatile uint32_t g_send_errors = 0;

/* TX queue for audio data (to be sent from agora task context) */
#define TX_QUEUE_LEN 8
#define TX_FRAME_SIZE 640 /* 320 samples * 2 bytes */

typedef struct {
    uint8_t data[TX_FRAME_SIZE];
    uint16_t len;
} tx_audio_frame_t;

static QueueHandle_t g_tx_queue = NULL;
static TaskHandle_t g_tx_task_handle = NULL;

/* Agora RTC event callbacks */

static void on_join_channel_success(connection_id_t conn_id, uint32_t uid,
                                    int elapsed_ms) {
    __atomic_store_n(&g_connected, 1, __ATOMIC_RELEASE);
    IPRO_LOGI(LOG_TAG, "Joined channel '%s' (uid=%lu, elapsed=%d ms)",
              g_channel, (unsigned long)uid, elapsed_ms);
    if (g_event_notify_cb)
        g_event_notify_cb(0 /* JOINED */, uid);

    connection_info_t conn_info = {0};
    int ret = agora_rtc_get_connection_info(conn_id, &conn_info);
    if (ret == 0) {
        IPRO_LOGI(LOG_TAG, "Connection info: conn_id=%lu uid=%lu ch='%s'",
                  (unsigned long)conn_info.conn_id,
                  (unsigned long)conn_info.uid, conn_info.channel_name);
    } else {
        IPRO_LOGE(LOG_TAG, "get_connection_info failed: %d", ret);
    }

    /* Note: do NOT call agora_rtc_mute_local_audio() from callback context —
     * it can deadlock the SDK internal thread with AUDIO_CODEC_DISABLED mode */
}

static void on_rejoin_channel_success(connection_id_t conn_id, uint32_t uid,
                                      int elapsed_ms) {
    __atomic_store_n(&g_connected, 1, __ATOMIC_RELEASE);
    IPRO_LOGI(LOG_TAG, "Rejoined channel (uid=%lu, elapsed=%d ms)",
              (unsigned long)uid, elapsed_ms);
}

static void on_connection_lost(connection_id_t conn_id) {
    __atomic_store_n(&g_connected, 0, __ATOMIC_RELEASE);
    IPRO_LOGW(LOG_TAG, "Connection lost!");
    if (g_event_notify_cb)
        g_event_notify_cb(6 /* ERROR */, 0);
}

static void on_reconnecting(connection_id_t conn_id) {
    IPRO_LOGW(LOG_TAG, "Reconnecting...");
    if (g_event_notify_cb)
        g_event_notify_cb(4 /* RECONNECTING */, 0);
}

static void on_error(connection_id_t conn_id, int code, const char *msg) {
    IPRO_LOGE(LOG_TAG, "Error %d: %s", code, msg ? msg : "unknown");
}

static void on_user_joined(connection_id_t conn_id, uint32_t uid,
                           int elapsed_ms) {
    __atomic_add_fetch(&g_remote_user_count, 1, __ATOMIC_RELEASE);
    IPRO_LOGI(LOG_TAG, "Remote user %lu joined", (unsigned long)uid);
    if (g_event_notify_cb)
        g_event_notify_cb(2 /* USER_JOINED */, uid);

    /* Unmute remote audio to receive it */
    int ret = agora_rtc_mute_remote_audio(conn_id, uid, false);
    if (ret == 0) {
        IPRO_LOGI(LOG_TAG, "Subscribed to audio from user %lu",
                  (unsigned long)uid);
    } else {
        IPRO_LOGE(LOG_TAG, "Failed to subscribe audio from user %lu: %d",
                  (unsigned long)uid, ret);
    }
}

static void on_user_offline(connection_id_t conn_id, uint32_t uid, int reason) {
    (void)conn_id;
    uint32_t prev = __atomic_load_n(&g_remote_user_count, __ATOMIC_ACQUIRE);
    if (prev > 0)
        __atomic_sub_fetch(&g_remote_user_count, 1, __ATOMIC_RELEASE);
    IPRO_LOGI(LOG_TAG, "Remote user %lu offline (reason=%d)",
              (unsigned long)uid, reason);
    if (g_event_notify_cb)
        g_event_notify_cb(3 /* USER_LEFT */, uid);
}

static uint32_t g_rx_frame_count = 0;
static uint32_t g_rx_drop_count = 0;

static void on_audio_data(connection_id_t conn_id, uint32_t uid,
                          uint16_t sent_ts, const void *data_ptr,
                          size_t data_len, const audio_frame_info_t *info_ptr) {
    g_rx_frame_count++;

    /* Log first 10 frames, then periodically */
    if (g_rx_frame_count <= 10 || (g_rx_frame_count % 5000) == 0) {
        const uint8_t *p = (const uint8_t *)data_ptr;
        IPRO_LOGI(
            LOG_TAG,
            "RX audio #%lu: len=%zu type=%d uid=%lu "
            "hex=[%02x %02x %02x %02x %02x %02x %02x %02x]",
            (unsigned long)g_rx_frame_count, data_len,
            info_ptr ? info_ptr->data_type : -1, (unsigned long)uid,
            (data_len > 0 && p) ? p[0] : 0, (data_len > 1 && p) ? p[1] : 0,
            (data_len > 2 && p) ? p[2] : 0, (data_len > 3 && p) ? p[3] : 0,
            (data_len > 4 && p) ? p[4] : 0, (data_len > 5 && p) ? p[5] : 0,
            (data_len > 6 && p) ? p[6] : 0, (data_len > 7 && p) ? p[7] : 0);
    }

    if (data_len == 0 || data_ptr == NULL || g_rx_queue == NULL) {
        return;
    }

    /* Copy frame to queue */
    rx_audio_frame_t frame;
    frame.len = (data_len > RX_FRAME_SIZE) ? RX_FRAME_SIZE : data_len;
    frame.type = info_ptr ? info_ptr->data_type : 0;
    memcpy(frame.data, data_ptr, frame.len);

    /* Non-blocking queue send from ISR/callback context */
    if (xQueueSend(g_rx_queue, &frame, 0) != pdTRUE) {
        g_rx_drop_count++;
    }
}

/* Audio RX processing task */
static void audio_rx_task(void *arg) {
    (void)arg;
    static rx_audio_frame_t frame;
    static uint32_t rx_frame_count = 0;

    IPRO_LOGI(LOG_TAG, "Audio RX task started");

    while (1) {
        if (xQueueReceive(g_rx_queue, &frame, pdMS_TO_TICKS(1000)) == pdTRUE) {
            rx_frame_count++;

            /* Log first 20 frames with hex dump, then periodically */
            // if (rx_frame_count <= 20 || (rx_frame_count % 5000) == 0) {
            //     const uint8_t *p = frame.data;
            //     IPRO_LOGI(
            //         LOG_TAG,
            //         "RX task #%lu: type=%d len=%u "
            //         "hex=[%02x %02x %02x %02x %02x %02x %02x %02x]",
            //         (unsigned long)rx_frame_count, frame.type, frame.len,
            //         (frame.len > 0) ? p[0] : 0, (frame.len > 1) ? p[1] : 0,
            //         (frame.len > 2) ? p[2] : 0, (frame.len > 3) ? p[3] : 0,
            //         (frame.len > 4) ? p[4] : 0, (frame.len > 5) ? p[5] : 0,
            //         (frame.len > 6) ? p[6] : 0, (frame.len > 7) ? p[7] : 0);
            // }

            /* Decode and feed DAC */
            if ((frame.type == AUDIO_DATA_TYPE_OPUS ||
                 frame.type == AUDIO_DATA_TYPE_OPUSFB) &&
                g_opus_dec) {
                int dec_samples =
                    opus_decode(g_opus_dec, frame.data, frame.len,
                                g_opus_dec_buf, OPUS_DEC_FRAME_SAMPLES, 0);
                if (dec_samples > 0 && g_rx_cb) {
                    g_rx_cb(g_opus_dec_buf, (uint32_t)dec_samples);
                    {
                        static uint32_t feed_count = 0;
                        if (++feed_count <= 10 || (feed_count % 5000) == 0) {
                            IPRO_LOGI(LOG_TAG, "DAC feed #%lu: %d samples",
                                      (unsigned long)feed_count, dec_samples);
                        }
                    }
                } else if (dec_samples < 0) {
                    if (rx_frame_count <= 5 || (rx_frame_count % 1000) == 0) {
                        IPRO_LOGE(LOG_TAG, "Opus decode err: %d", dec_samples);
                    }
                }
            } else if (frame.type == AUDIO_DATA_TYPE_PCM) {
                /* PCM from Agora internal decoder (G.722 mode) */
                uint32_t samples = frame.len / 2;
                if (samples > 0 && g_rx_cb) {
                    g_rx_cb((const int16_t *)frame.data, samples);
                    {
                        static uint32_t pcm_feed_count = 0;
                        if (++pcm_feed_count <= 10 ||
                            (pcm_feed_count % 5000) == 0) {
                            IPRO_LOGI(LOG_TAG, "PCM feed #%lu: %lu samples",
                                      (unsigned long)pcm_feed_count,
                                      (unsigned long)samples);
                        }
                    }
                }
            } else {
                /* Unknown type — log for debug */
                static uint32_t unk_count = 0;
                if (++unk_count <= 5) {
                    IPRO_LOGW(LOG_TAG, "RX unknown type=%d len=%u", frame.type,
                              frame.len);
                }
            }
        } else {
            if (!g_connected) {
                IPRO_LOGW(LOG_TAG, "RX timeout, not connected");
            }
        }
    }
}

/* Start audio RX task */
static int app_agora_start_rx_task(void) {
    if (g_rx_queue != NULL) {
        return 0; /* Already started */
    }

    g_rx_queue = xQueueCreate(RX_QUEUE_LEN, sizeof(rx_audio_frame_t));
    if (g_rx_queue == NULL) {
        IPRO_LOGE(LOG_TAG, "Failed to create RX queue");
        return -1;
    }

    BaseType_t ret = xTaskCreate(audio_rx_task, "AudioRX", 8192, NULL,
                                 configMAX_PRIORITIES - 3, &g_rx_task_handle);
    if (ret != pdPASS) {
        IPRO_LOGE(LOG_TAG, "Failed to create RX task");
        vQueueDelete(g_rx_queue);
        g_rx_queue = NULL;
        return -1;
    }

    return 0;
}

static void stream_msg_task(void *arg) {
    (void)arg;
    stream_msg_t msg;

    /* Reassembly state */
    static char *reassembly_buf = NULL;
    static uint32_t reassembly_pos = 0;
    static char last_msg_id[32] = {0};

    /* Delta tracking for incremental STT output */
    static uint32_t user_prev_len = 0; /* length of last user transcription */
    static uint32_t ai_prev_len = 0;   /* length of last AI transcription */
    static uint8_t expected_index = 0;
    static uint8_t total_parts = 0;

    IPRO_LOGI(LOG_TAG, "Stream message task started");

    while (1) {
        if (xQueueReceive(g_msg_queue, &msg, portMAX_DELAY) != pdTRUE)
            continue;

        /* Null-terminate */
        msg.data[msg.len < MSG_MAX_SIZE ? msg.len : MSG_MAX_SIZE - 1] = '\0';

        /* Parse format: msg_id|cur_index|total_num|base64_payload */
        char *saveptr = NULL;
        char *msg_id = strtok_r(msg.data, "|", &saveptr);
        char *cur_idx_str = strtok_r(NULL, "|", &saveptr);
        char *total_str = strtok_r(NULL, "|", &saveptr);
        char *payload = strtok_r(NULL, "|", &saveptr);

        if (!msg_id || !cur_idx_str || !total_str || !payload) {
            /* Not in expected format, print raw */
            IPRO_LOGI(LOG_TAG, "MSG raw: %.100s%s", msg.data,
                      msg.len > 100 ? "..." : "");
            continue;
        }

        uint8_t cur_index = (uint8_t)atoi(cur_idx_str);
        uint8_t total_num = (uint8_t)atoi(total_str);

        /* New message ID → reset reassembly */
        if (strcmp(msg_id, last_msg_id) != 0) {
            if (reassembly_buf) {
                vPortFree(reassembly_buf);
                reassembly_buf = NULL;
            }
            reassembly_pos = 0;
            expected_index = 1;
            total_parts = total_num;
            strncpy(last_msg_id, msg_id, sizeof(last_msg_id) - 1);
            last_msg_id[sizeof(last_msg_id) - 1] = '\0';

            reassembly_buf = pvPortMalloc(MSG_REASSEMBLY_MAX);
            if (!reassembly_buf) {
                IPRO_LOGE(LOG_TAG, "Failed to alloc reassembly buf");
                last_msg_id[0] = '\0';
                continue;
            }
        }

        /* Sequence check */
        if (cur_index != expected_index || total_num != total_parts) {
            IPRO_LOGW(LOG_TAG, "Seq mismatch: got %u/%u, expected %u/%u, reset",
                      cur_index, total_num, expected_index, total_parts);
            if (reassembly_buf) {
                vPortFree(reassembly_buf);
                reassembly_buf = NULL;
            }
            reassembly_pos = 0;
            last_msg_id[0] = '\0';
            continue;
        }

        /* Append payload to reassembly buffer */
        size_t payload_len = strlen(payload);
        if (reassembly_pos + payload_len >= MSG_REASSEMBLY_MAX) {
            IPRO_LOGE(LOG_TAG, "Reassembly overflow (%lu + %zu > %d)",
                      (unsigned long)reassembly_pos, payload_len,
                      MSG_REASSEMBLY_MAX);
            vPortFree(reassembly_buf);
            reassembly_buf = NULL;
            reassembly_pos = 0;
            last_msg_id[0] = '\0';
            continue;
        }
        memcpy(reassembly_buf + reassembly_pos, payload, payload_len);
        reassembly_pos += payload_len;
        expected_index++;

        /* Log reassembly progress for multi-segment messages only */
        if (total_parts > 1 && (cur_index == 1 || cur_index == total_num)) {
            IPRO_LOGI(LOG_TAG, "Stream seg %u/%u (id=%s)", cur_index,
                      total_parts, msg_id);
        }

        /* Not yet complete */
        if (cur_index < total_num)
            continue;

        /* === Reassembly complete === */
        reassembly_buf[reassembly_pos] = '\0';

        /* Decode base64 — use separate output area (second half of buffer) */
        uint32_t decoded_len = 0;
        char *decode_dst = reassembly_buf + (MSG_REASSEMBLY_MAX / 2);
        iotx_err_t b64_ret = utils_base64decode(
            (const uint8_t *)reassembly_buf, reassembly_pos,
            MSG_REASSEMBLY_MAX / 2 - 1, (uint8_t *)decode_dst, &decoded_len);
        if (b64_ret != SUCCESS_RETURN || decoded_len == 0) {
            IPRO_LOGW(LOG_TAG, "Base64 decode failed");
            vPortFree(reassembly_buf);
            reassembly_buf = NULL;
            reassembly_pos = 0;
            last_msg_id[0] = '\0';
            continue;
        }
        decode_dst[decoded_len] = '\0';

        /* Parse JSON — use strstr to detect type without modifying buffer,
         * then extract only the value we need (json_extract_string is
         * destructive: it null-terminates the value in-place) */
        if (strstr(decode_dst, "\"user.transcription\"")) {
            char *text = json_extract_string(decode_dst, "text");
            if (text && text[0]) {
                bool is_final = (strstr(decode_dst, "\"final\": true") ||
                                 strstr(decode_dst, "\"final\":true"));
                uint32_t text_len = strlen(text);

                if (is_final) {
                    IPRO_LOGI(LOG_TAG, "[USER] %s", text);
                    user_prev_len = 0;
                } else {
                    if (text_len > user_prev_len) {
                        const char *delta = text + user_prev_len;
                        IPRO_LOGD(LOG_TAG, "[USER...] +%s", delta);
                    }
                    user_prev_len = text_len;
                }
            }
        } else if (strstr(decode_dst, "\"assistant.transcription\"")) {
            char *text = json_extract_string(decode_dst, "text");
            if (text && text[0]) {
                uint32_t text_len = strlen(text);

                if (text_len > ai_prev_len) {
                    const char *delta = text + ai_prev_len;
                    IPRO_LOGD(LOG_TAG, "[AI...] +%s", delta);
                }
                ai_prev_len = text_len;

                /* Update LCD TTS subtitle with AI response text */
                app_lcd_set_tts_text(text);
            }
        } else if (strstr(decode_dst, "\"message.state\"")) {
            char *state = json_extract_string(decode_dst, "state");
            if (state) {
                IPRO_LOGI(LOG_TAG, "AI state: %s", state);
                /* Half-duplex TX muting to prevent DAC→AMIC echo.
                 * Only mute during "speaking" (DAC active, echo risk).
                 * NOT during "thinking" — user may still be talking.
                 * Holdoff after speaking lets DAC audio drain before unmuting.
                 */
                if (strcmp(state, "speaking") == 0) {
                    __atomic_store_n(&g_ai_speaking, 1, __ATOMIC_RELEASE);
                    ai_prev_len = 0;
                    if (g_ai_state_notify_cb)
                        g_ai_state_notify_cb(3); /* SPEAKING */
                } else {
                    if (__atomic_load_n(&g_ai_speaking, __ATOMIC_ACQUIRE)) {
                        g_ai_mute_until = xTaskGetTickCount() +
                                          pdMS_TO_TICKS(AI_TX_HOLDOFF_MS);
                        ai_prev_len = 0;
                    }
                    __atomic_store_n(&g_ai_speaking, 0, __ATOMIC_RELEASE);

                    if (strcmp(state, "thinking") == 0) {
                        if (g_ai_state_notify_cb)
                            g_ai_state_notify_cb(2); /* THINKING */
                    } else if (strcmp(state, "listening") == 0) {
                        user_prev_len = 0;
                        if (g_ai_state_notify_cb)
                            g_ai_state_notify_cb(1); /* LISTENING */
                    } else {
                        if (g_ai_state_notify_cb)
                            g_ai_state_notify_cb(0); /* IDLE */
                    }
                }
            }
        }

        /* Cleanup */
        vPortFree(reassembly_buf);
        reassembly_buf = NULL;
        reassembly_pos = 0;
        last_msg_id[0] = '\0';
    }
}

/* Start stream message task */
static int app_agora_start_msg_task(void) {
    if (g_msg_queue != NULL) {
        return 0; /* Already started */
    }

    g_msg_queue = xQueueCreate(MSG_QUEUE_LEN, sizeof(stream_msg_t));
    if (g_msg_queue == NULL) {
        IPRO_LOGE(LOG_TAG, "Failed to create MSG queue");
        return -1;
    }

    BaseType_t ret = xTaskCreate(stream_msg_task, "StreamMSG", 3072, NULL,
                                 configMAX_PRIORITIES - 4, &g_msg_task_handle);
    if (ret != pdPASS) {
        IPRO_LOGE(LOG_TAG, "Failed to create MSG task");
        vQueueDelete(g_msg_queue);
        g_msg_queue = NULL;
        return -1;
    }

    return 0;
}

/* Audio TX processing task - runs in agora context */
static void audio_agora_tx_task(void *arg) {
    (void)arg;
    static tx_audio_frame_t frame; /* static to save ~642 bytes stack */

    IPRO_LOGI(LOG_TAG, "Agora TX task started");

    while (1) {
        if (xQueueReceive(g_tx_queue, &frame, pdMS_TO_TICKS(1000)) == pdTRUE) {
            if (!__atomic_load_n(&g_connected, __ATOMIC_ACQUIRE) ||
                g_conn_id == CONNECTION_ID_INVALID) {
                continue;
            }

            /* Send PCM directly — Agora SDK encodes to G.722 internally */
            audio_frame_info_t info = {0};
            info.data_type = AUDIO_DATA_TYPE_PCM;

            int ret = agora_rtc_send_audio_data(g_conn_id, frame.data,
                                                frame.len, &info);
            if (ret < 0) {
                g_send_errors++;
                if ((g_send_errors % 100) == 1) {
                    IPRO_LOGE(LOG_TAG, "TX error %d: %s", ret,
                              agora_rtc_err_2_str(ret));
                }
            } else {
                g_send_count++;
                if ((g_send_count % 5000) == 0) {
                    IPRO_LOGI(LOG_TAG, "Audio TX: %lu frames, %lu errors",
                              (unsigned long)g_send_count,
                              (unsigned long)g_send_errors);
                }
            }
        }
    }
}

/* Start audio TX task */
static int app_agora_start_tx_task(void) {
    if (g_tx_queue != NULL) {
        return 0; /* Already started */
    }

    g_tx_queue = xQueueCreate(TX_QUEUE_LEN, sizeof(tx_audio_frame_t));
    if (g_tx_queue == NULL) {
        IPRO_LOGE(LOG_TAG, "Failed to create TX queue");
        return -1;
    }

    /* Run TX task at same priority as main agora task.
     * Stack 3072: opus_encode uses ~1KB stack internally */
    BaseType_t ret = xTaskCreate(audio_agora_tx_task, "AgoraTX", 8192, NULL,
                                 tskIDLE_PRIORITY + 5, &g_tx_task_handle);
    if (ret != pdPASS) {
        IPRO_LOGE(LOG_TAG, "Failed to create TX task");
        vQueueDelete(g_tx_queue);
        g_tx_queue = NULL;
        return -1;
    }

    return 0;
}

static void on_token_privilege_will_expire(connection_id_t conn_id,
                                           const char *token) {
    IPRO_LOGW(LOG_TAG, "Token will expire soon");
}

static void on_license_validation_failure(connection_id_t conn_id, int error) {
    IPRO_LOGE(LOG_TAG, "License validation failed: %d", error);
}

static void on_user_joined_with_user_account(connection_id_t conn_id,
                                             const user_info_t *user,
                                             int elapsed_ms) {
    IPRO_LOGI(LOG_TAG, "User joined: account='%s' uid=%lu", user->user_account,
              (unsigned long)user->uid);
}

static void on_user_offline_with_user_account(connection_id_t conn_id,
                                              const user_info_t *user,
                                              int reason) {
    IPRO_LOGI(LOG_TAG, "User offline: account='%s' uid=%lu reason=%d",
              user->user_account, (unsigned long)user->uid, reason);
}

static void on_user_info_updated(connection_id_t conn_id,
                                 const user_info_t *user) {
    IPRO_LOGI(LOG_TAG, "User info updated: account='%s' uid=%lu",
              user->user_account, (unsigned long)user->uid);
}

static void on_user_mute_audio(connection_id_t conn_id, uint32_t uid,
                               bool muted) {
    IPRO_LOGI(LOG_TAG, "User %lu %s audio", (unsigned long)uid,
              muted ? "muted" : "unmuted");
    if (muted) {
        int ret = agora_rtc_mute_remote_audio(conn_id, uid, false);
        IPRO_LOGI(LOG_TAG, "Force unmute remote %lu: %d", (unsigned long)uid,
                  ret);
    }
}

static void on_user_mute_video(connection_id_t conn_id, uint32_t uid,
                               bool muted) {
    IPRO_LOGI(LOG_TAG, "User %lu %s video", (unsigned long)uid,
              muted ? "muted" : "unmuted");
}

static uint32_t g_mixed_rx_frame_count = 0;

static void on_mixed_audio_data(connection_id_t conn_id, const void *data_ptr,
                                size_t data_len,
                                const audio_frame_info_t *info_ptr) {
    g_mixed_rx_frame_count++;

    if (g_mixed_rx_frame_count <= 10 || (g_mixed_rx_frame_count % 5000) == 0) {
        const uint8_t *p = (const uint8_t *)data_ptr;
        IPRO_LOGI(
            LOG_TAG,
            "MIXED RX #%lu: len=%zu type=%d "
            "hex=[%02x %02x %02x %02x %02x %02x %02x %02x]",
            (unsigned long)g_mixed_rx_frame_count, data_len,
            info_ptr ? info_ptr->data_type : -1, (data_len > 0 && p) ? p[0] : 0,
            (data_len > 1 && p) ? p[1] : 0, (data_len > 2 && p) ? p[2] : 0,
            (data_len > 3 && p) ? p[3] : 0, (data_len > 4 && p) ? p[4] : 0,
            (data_len > 5 && p) ? p[5] : 0, (data_len > 6 && p) ? p[6] : 0,
            (data_len > 7 && p) ? p[7] : 0);
    }

    /* Note: enable_audio_mixer=false, so this callback should not fire.
     * Do NOT feed to RX queue here — on_audio_data handles RX to avoid
     * double-feeding which causes audio overlap/echo. */
}

static void on_video_data(connection_id_t conn_id, uint32_t uid,
                          uint16_t sent_ts, const void *data_ptr,
                          size_t data_len, const video_frame_info_t *info_ptr) {
    /* Video data received - not used in audio-only app */
}

static void on_target_bitrate_changed(connection_id_t conn_id,
                                      uint32_t target_bps) {
    IPRO_LOGI(LOG_TAG, "Target bitrate changed to %lu bps",
              (unsigned long)target_bps);
}

static void on_key_frame_gen_req(connection_id_t conn_id, uint32_t uid,
                                 video_stream_type_e stream_type) {
    /* Keyframe request - not used in audio-only app */
}

static void on_rtc_stats(connection_id_t conn_id, rtc_stats_t stats) {
    static uint32_t stats_count = 0;
    stats_count++;
    if (stats_count == 1 || (stats_count % 30) == 0) { /* ~every 60s */
        IPRO_LOGI(LOG_TAG, "RTC: TX=%u kbps, RX=%u kbps, users=%u",
                  stats.tx_audio_kbitrate, stats.rx_audio_kbitrate,
                  stats.user_count);
    }
}

static void on_media_ctrl_msg(connection_id_t conn_id, uint32_t uid,
                              const void *payload, size_t length) {
    IPRO_LOGI(LOG_TAG, "Media ctrl msg from uid=%lu, len=%zu",
              (unsigned long)uid, length);
}

static void on_stream_message(connection_id_t conn_id, uint32_t uid,
                              int stream_id, const char *data, size_t length,
                              uint64_t sent_ts) {
    /* Queue message for processing in task context */
    if (g_msg_queue != NULL && data && length > 0) {
        stream_msg_t msg;
        msg.uid = uid;
        msg.stream_id = stream_id;
        msg.len = (length < MSG_MAX_SIZE) ? length : MSG_MAX_SIZE;
        memcpy(msg.data, data, msg.len);

        if (xQueueSend(g_msg_queue, &msg, 0) != pdTRUE) {
            IPRO_LOGW(LOG_TAG, "MSG queue full, dropped message");
        }
    }
}

static int on_packet_input_hook(connection_id_t conn_id, rtc_packet_type_e type,
                                const void *src, uint16_t src_size,
                                void *dst_buf, uint16_t dst_buf_size) {
    /* Packet input hook - not used */
    return 0;
}

static int on_packet_output_hook(connection_id_t conn_id,
                                 rtc_packet_type_e type, const void *src,
                                 uint16_t src_size, void *dst_buf,
                                 uint16_t dst_buf_size) {
    /* Packet output hook - not used */
    return 0;
}

static agora_rtc_event_handler_t g_event_handler = {
    .on_error = on_error,
    .on_join_channel_success = on_join_channel_success,
    .on_reconnecting = on_reconnecting,
    .on_connection_lost = on_connection_lost,
    .on_rejoin_channel_success = on_rejoin_channel_success,
    .on_license_validation_failure = on_license_validation_failure,
    .on_token_privilege_will_expire = on_token_privilege_will_expire,
    .on_user_joined = on_user_joined,
    .on_user_offline = on_user_offline,
    .on_user_joined_with_user_account = on_user_joined_with_user_account,
    .on_user_offline_with_user_account = on_user_offline_with_user_account,
    .on_user_info_updated = on_user_info_updated,
    .on_user_mute_audio = on_user_mute_audio,
    .on_user_mute_video = on_user_mute_video,
    .on_audio_data = on_audio_data,
    .on_mixed_audio_data = on_mixed_audio_data,
    .on_video_data = on_video_data,
    .on_target_bitrate_changed = on_target_bitrate_changed,
    .on_key_frame_gen_req = on_key_frame_gen_req,
    .on_rtc_stats = on_rtc_stats,
    .on_media_ctrl_msg = on_media_ctrl_msg,
    .on_stream_message = on_stream_message,
    .on_packet_input_hook = on_packet_input_hook,
    .on_packet_output_hook = on_packet_output_hook,
};
/* Data stream TX */
static int g_data_stream_id = -1;

int app_agora_create_data_stream(void) {
    if (g_conn_id == CONNECTION_ID_INVALID)
        return -1;
    return agora_rtc_create_data_stream(g_conn_id, &g_data_stream_id, false,
                                        true);
}

int app_agora_send_data_stream(const char *data, size_t len) {
    if (g_data_stream_id < 0)
        return -1;
    return agora_rtc_send_stream_message(g_conn_id, g_data_stream_id, data,
                                         len);
}

static rtc_service_option_t service_opt = {0};

#define AOSL_PRINTF_MAX_LEN 2048

/**
 * Safe printf for AOSL SDK log output, with buffer overflow protection.
 */
int aosl_printf_safe(const char *fmt, ...) {
    va_list args;
    char buffer[AOSL_PRINTF_MAX_LEN];
    int ret;

    if (fmt == NULL) {
        return -1;
    }

    va_start(args, fmt);
    ret = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    if (ret < 0) {
        return -2;
    }

    if ((size_t)ret >= sizeof(buffer)) {
        buffer[sizeof(buffer) - 1] = '\0';
    }

    return printf("%s", buffer);
}

/**
 * @brief Fill standard channel options for G.722 AI voice.
 *
 * Centralizes the rtc_channel_options_t setup used by both
 * app_agora_init() and app_agora_rejoin().
 */
static void fill_channel_options(rtc_channel_options_t *opts) {
    memset(opts, 0, sizeof(*opts));
    opts->auto_subscribe_audio = true;
    opts->auto_subscribe_video = false;
    opts->enable_audio_jitter_buffer = true;
    opts->enable_audio_mixer = false;
    opts->enable_audio_decode = true; /* SDK decodes G.722 -> PCM */
    opts->enable_audio_ai_qos = false;
    opts->enable_audio_downlink_aec =
        false; /* SDK AEC handles echo cancellation */
    opts->enable_audio_downlink_aec_pcm_dump = false;
    opts->audio_codec_opt.audio_codec_type = AUDIO_CODEC_TYPE_G722;
    opts->audio_codec_opt.pcm_sample_rate = 16000;
    opts->audio_codec_opt.pcm_channel_num = 1;
    opts->audio_codec_opt.pcm_duration = 20;
}

int app_agora_init(const char *app_id, const char *token, const char *channel,
                   uint32_t uid) {
    int ret;
    g_channel = channel;

    IPRO_LOGI(LOG_TAG, "SDK version: %s", agora_rtc_get_version());

    /* Create Opus decoder for RX (16kHz, upsample to 32kHz DAC in feed) */
    int opus_err;
    g_opus_dec = opus_decoder_create(16000, 1, &opus_err);
    if (opus_err != OPUS_OK || !g_opus_dec) {
        IPRO_LOGE(LOG_TAG, "Opus decoder create failed: %d", opus_err);
        return -1;
    }
    IPRO_LOGI(LOG_TAG, "Opus decoder created (16kHz mono)");

    /* Create Opus encoder for TX */
    g_opus_enc =
        opus_encoder_create(16000, 1, OPUS_APPLICATION_VOIP, &opus_err);
    if (opus_err != OPUS_OK || !g_opus_enc) {
        IPRO_LOGE(LOG_TAG, "Opus encoder create failed: %d", opus_err);
        return -1;
    }
    opus_encoder_ctl(g_opus_enc, OPUS_SET_BITRATE(32000));
    opus_encoder_ctl(g_opus_enc, OPUS_SET_COMPLEXITY(3));
    opus_encoder_ctl(g_opus_enc, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
    IPRO_LOGI(LOG_TAG, "Opus encoder created (16kHz mono, 32kbps)");

    /* Start RX audio task */
    ret = app_agora_start_rx_task();
    if (ret < 0) {
        IPRO_LOGE(LOG_TAG, "Failed to start RX task");
        return ret;
    }

    /* Start TX audio task */
    ret = app_agora_start_tx_task();
    if (ret < 0) {
        IPRO_LOGE(LOG_TAG, "Failed to start TX task");
        return ret;
    }

    /* Start stream message task */
    ret = app_agora_start_msg_task();
    if (ret < 0) {
        IPRO_LOGE(LOG_TAG, "Failed to start MSG task");
        return ret;
    }

    service_opt.area_code = AREA_CODE_GLOB;
    service_opt.log_cfg.log_disable = false;
    service_opt.log_cfg.log_level = RTC_LOG_WARNING; /* 7 = max AOSL accepts */
    // service_opt.log_cfg.log_printf = printf;
    service_opt.log_cfg.log_path = NULL;
    service_opt.use_string_uid = false;

    /* Set AOSL log level BEFORE SDK init so service thread sees it immediately
     */
    // aosl_set_log_level(AOSL_LOG_DEBUG);

    /* Initialize Agora RTC SDK */
    IPRO_LOGI(LOG_TAG, "agora_rtc_init: appId='%.20s...' (len=%u)", app_id,
              (unsigned)strlen(app_id));
    IPRO_LOGI(LOG_TAG, "  token len=%u channel='%s' uid=%lu",
              (unsigned)strlen(token), channel, (unsigned long)uid);
    IPRO_LOGI(LOG_TAG, "  heap free=%lu min_ever=%lu",
              (unsigned long)xPortGetFreeHeapSize(),
              (unsigned long)xPortGetMinimumEverFreeHeapSize());
    ret = agora_rtc_init(app_id, &g_event_handler, &service_opt);
    IPRO_LOGI(LOG_TAG, "agora_rtc_init returned: %d", ret);
    if (ret < 0) {
        IPRO_LOGE(LOG_TAG, "Init failed: %d (%s)", ret,
                  agora_rtc_err_2_str(ret));
        return ret;
    }

    IPRO_LOGI(LOG_TAG, "SDK initialized");

    /* Wait for SDK internal initialization (DNS resolution, etc.) */
    vTaskDelay(pdMS_TO_TICKS(3000));

    /* Create connection */
    ret = agora_rtc_create_connection(&g_conn_id);
    if (ret < 0) {
        IPRO_LOGE(LOG_TAG, "Create connection failed: %d", ret);
        agora_rtc_fini();
        return ret;
    }
    IPRO_LOGI(LOG_TAG, "Connection created (id=%lu)", (unsigned long)g_conn_id);

    /* Configure channel options and join */
    rtc_channel_options_t channel_options;
    fill_channel_options(&channel_options);

    /* Join channel */
    ret = agora_rtc_join_channel(g_conn_id, channel, uid, token,
                                 &channel_options);
    if (ret < 0) {
        IPRO_LOGE(LOG_TAG, "Join channel failed: %d (%s)", ret,
                  agora_rtc_err_2_str(ret));
        agora_rtc_destroy_connection(g_conn_id);
        agora_rtc_fini();
        g_conn_id = CONNECTION_ID_INVALID;
        return ret;
    }
    IPRO_LOGI(LOG_TAG, "Joining channel '%s'...", channel);

    return 0;
}

int app_agora_send_audio(const void *pcm_data, size_t data_len) {

    if (!__atomic_load_n(&g_connected, __ATOMIC_ACQUIRE) ||
        g_conn_id == CONNECTION_ID_INVALID) {
        return -1;
    }

    if (g_tx_queue == NULL) {
        return -1;
    }

    if (data_len > TX_FRAME_SIZE) {
        return -1;
    }

    /* Queue audio data to be sent by agora TX task */
    static tx_audio_frame_t
        frame; /* static to avoid 642 bytes on caller's stack */
    memcpy(frame.data, pcm_data, data_len);
    frame.len = data_len;

    /* Non-blocking send - drop frame if queue full */
    if (xQueueSend(g_tx_queue, &frame, 0) != pdTRUE) {
        return -2; /* Queue full */
    }

    return 0;
}

int app_agora_is_connected(void) {
    return __atomic_load_n(&g_connected, __ATOMIC_ACQUIRE);
}

void app_agora_set_rx_callback(agora_rx_cb_t cb) { g_rx_cb = cb; }

void app_agora_set_event_callback(agora_event_cb_t cb) {
    g_event_notify_cb = cb;
}
void app_agora_set_ai_state_callback(agora_ai_state_cb_t cb) {
    g_ai_state_notify_cb = cb;
}

int app_agora_is_ai_speaking(void) {
    if (__atomic_load_n(&g_ai_speaking, __ATOMIC_ACQUIRE))
        return 1;
    /* Holdoff: keep TX muted briefly after AI stops to let DAC audio drain */
    TickType_t until = g_ai_mute_until;
    if (until != 0 && xTaskGetTickCount() < until)
        return 1;
    return 0;
}

uint32_t app_agora_get_remote_user_count(void) {
    return __atomic_load_n(&g_remote_user_count, __ATOMIC_ACQUIRE);
}

void app_agora_get_stats(agora_stats_t *stats) {
    if (!stats)
        return;
    stats->rx_frames = g_rx_frame_count;
    stats->rx_drops = g_rx_drop_count;
    stats->tx_frames = g_send_count;
    stats->tx_errors = g_send_errors;
}

int app_agora_leave_channel(void) {
    if (g_conn_id == CONNECTION_ID_INVALID)
        return -1;

    IPRO_LOGI(LOG_TAG, "Leaving channel...");
    __atomic_store_n(&g_connected, 0, __ATOMIC_RELEASE);

    int ret = agora_rtc_leave_channel(g_conn_id);
    IPRO_LOGI(LOG_TAG, "agora_rtc_leave_channel: %d", ret);

    agora_rtc_destroy_connection(g_conn_id);
    g_conn_id = CONNECTION_ID_INVALID;

    return 0;
}

int app_agora_rejoin(const char *app_id, const char *token, const char *channel,
                     uint32_t uid) {
    IPRO_LOGI(LOG_TAG, "Rejoin: ch='%s' uid=%lu", channel, (unsigned long)uid);

    /* Leave current channel if connected */
    if (g_conn_id != CONNECTION_ID_INVALID) {
        app_agora_leave_channel();
    }

    /* Create new connection + join */
    int rc = agora_rtc_create_connection(&g_conn_id);
    if (rc < 0 || g_conn_id == CONNECTION_ID_INVALID) {
        IPRO_LOGE(LOG_TAG, "Failed to create connection for rejoin: %d", rc);
        return -1;
    }

    rtc_channel_options_t channel_options;
    fill_channel_options(&channel_options);

    int ret = agora_rtc_join_channel(g_conn_id, channel, uid, token,
                                     &channel_options);
    if (ret < 0) {
        IPRO_LOGE(LOG_TAG, "Rejoin failed: %d (%s)", ret,
                  agora_rtc_err_2_str(ret));
        agora_rtc_destroy_connection(g_conn_id);
        g_conn_id = CONNECTION_ID_INVALID;
        return ret;
    }

    g_channel = channel;
    IPRO_LOGI(LOG_TAG, "Rejoin initiated, waiting for callback...");
    return 0;
}

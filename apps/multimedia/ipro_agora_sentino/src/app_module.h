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
 * @file app_module.h
 * @brief Module interface definitions for Agora Sentino AI Gateway
 *
 * Defines the ops structs (vtable pattern), error codes, and callback
 * types used by all pluggable modules (network, audio, RTC, cloud).
 * Concrete implementations register an ops instance at init time;
 * the gateway orchestrator calls through these function pointers.
 */

#ifndef APP_MODULE_H
#define APP_MODULE_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * Error codes
 * ======================================================================== */

typedef enum {
    APP_OK           =  0,
    APP_ERR_INIT     = -1,   /* Initialization failure */
    APP_ERR_TIMEOUT  = -2,   /* Operation timed out */
    APP_ERR_NETWORK  = -3,   /* Network-level error */
    APP_ERR_AGORA    = -4,   /* Agora SDK error */
    APP_ERR_MQTT     = -5,   /* MQTT / cloud broker error */
    APP_ERR_NOMEM    = -6,   /* Out of memory */
    APP_ERR_INVALID  = -7,   /* Invalid parameter */
} app_err_t;

/* ========================================================================
 * Network module ops
 * ======================================================================== */

typedef struct {
    const char *name;                           /* e.g. "lte", "emac" */
    app_err_t (*init)(void);                    /* init stack (pre-scheduler) */
    app_err_t (*connect)(void);                 /* start connection (post-scheduler) */
    app_err_t (*disconnect)(void);              /* tear down link */
    int       (*is_connected)(void);            /* 1=connected, 0=not */
    app_err_t (*wait_ready)(int timeout_s);     /* block until IP acquired */
    app_err_t (*reconnect)(void);               /* disconnect + re-connect */
} network_ops_t;

/* ========================================================================
 * Audio input (capture / microphone) module ops
 * ======================================================================== */

/**
 * @brief Callback invoked when a captured PCM frame is ready.
 * @param pcm         Pointer to int16_t PCM samples
 * @param num_samples Number of samples in the frame
 */
typedef void (*audio_frame_cb_t)(const int16_t *pcm, uint32_t num_samples);

typedef struct {
    const char *name;                           /* e.g. "pdm", "i2s_in" */
    app_err_t (*init)(void);                    /* configure hardware */
    app_err_t (*start)(void);                   /* begin capture */
    app_err_t (*stop)(void);                    /* stop capture */
    void      (*set_callback)(audio_frame_cb_t cb); /* register TX frame cb */
    app_err_t (*set_gain)(int gain);            /* input gain (dB or hw units) */
} audio_input_ops_t;

/* ========================================================================
 * Audio output (playback / DAC / speaker) module ops
 * ======================================================================== */

typedef struct {
    const char *name;                           /* e.g. "dac", "i2s_out" */
    app_err_t (*init)(void);                    /* configure hardware */
    app_err_t (*start)(void);                   /* begin playback */
    app_err_t (*stop)(void);                    /* stop playback */
    app_err_t (*feed)(const int16_t *pcm,
                      uint32_t num_samples);    /* push PCM to output */
    void      (*set_volume)(int vol);           /* 0..100 */
    int       (*get_volume)(void);              /* current volume 0..100 */
} audio_output_ops_t;

/* ========================================================================
 * RTC (Real-Time Communication) module ops
 * ======================================================================== */

/** AI agent state reported by the RTC channel */
typedef enum {
    RTC_AI_STATE_IDLE      = 0,
    RTC_AI_STATE_LISTENING = 1,
    RTC_AI_STATE_THINKING  = 2,
    RTC_AI_STATE_SPEAKING  = 3,
} rtc_ai_state_t;

/** RTC channel-level events */
typedef enum {
    RTC_EVENT_JOINED       = 0,  /* local user joined channel */
    RTC_EVENT_LEFT         = 1,  /* local user left channel */
    RTC_EVENT_USER_JOINED  = 2,  /* remote user joined */
    RTC_EVENT_USER_LEFT    = 3,  /* remote user left */
    RTC_EVENT_RECONNECTING = 4,  /* connection lost, reconnecting */
    RTC_EVENT_RECONNECTED  = 5,  /* reconnected after loss */
    RTC_EVENT_ERROR        = 6,  /* unrecoverable error */
} rtc_event_t;

/**
 * @brief Callback for decoded audio received from remote peer.
 * @param pcm         PCM samples (int16_t)
 * @param num_samples Number of samples
 */
typedef void (*rtc_rx_cb_t)(const int16_t *pcm, uint32_t num_samples);

/**
 * @brief Callback for AI agent state transitions.
 * @param state New AI state
 */
typedef void (*rtc_ai_state_cb_t)(rtc_ai_state_t state);

/**
 * @brief Callback for channel-level events.
 * @param event Event type
 * @param uid   Remote user ID (0 for local events)
 */
typedef void (*rtc_event_cb_t)(rtc_event_t event, uint32_t uid);

/** RTC channel join parameters */
typedef struct {
    const char *app_id;
    const char *token;
    const char *channel;
    uint32_t    uid;
} rtc_join_params_t;

typedef struct {
    const char *name;                             /* e.g. "agora" */
    app_err_t (*init)(void);                      /* SDK-level init */
    app_err_t (*deinit)(void);                    /* SDK cleanup */
    app_err_t (*join)(const rtc_join_params_t *p);/* join channel */
    app_err_t (*leave)(void);                     /* leave channel */
    app_err_t (*rejoin)(const rtc_join_params_t *p); /* leave + join */
    int       (*is_connected)(void);              /* 1=in channel */
    app_err_t (*send_audio)(const void *data,
                            size_t len);          /* send encoded/raw audio */
    void      (*set_rx_callback)(rtc_rx_cb_t cb);
    void      (*set_ai_state_callback)(rtc_ai_state_cb_t cb);
    void      (*set_event_callback)(rtc_event_cb_t cb);
    int       (*is_ai_speaking)(void);            /* 1=speaking, 0=idle */
    uint32_t  (*get_remote_user_count)(void);
} rtc_ops_t;

/* ========================================================================
 * Cloud (MQTT / platform) module ops
 * ======================================================================== */

/**
 * @brief Callback for incoming cloud issue/command messages.
 * @param code    Command code string (e.g. "property_set", "agora_join")
 * @param payload Full JSON payload (mutable — parser may modify in place)
 */
typedef void (*cloud_msg_handler_t)(const char *code, char *payload);

/** RTC access credentials returned by cloud */
typedef struct {
    char     app_id[64];
    char     token[256];
    char     channel[64];
    uint32_t uid;
} cloud_rtc_access_t;

typedef struct {
    const char *name;                              /* e.g. "sentino" */
    app_err_t (*init)(void);                       /* connect to broker */
    app_err_t (*deinit)(void);                     /* disconnect + cleanup */
    int       (*is_connected)(void);               /* 1=connected */
    app_err_t (*wait_connected)(int timeout_s);    /* block until ready */
    app_err_t (*publish)(const char *code,
                         const char *data_json,
                         int ack);                 /* publish report/event */
    void      (*set_issue_handler)(cloud_msg_handler_t h);
    app_err_t (*issue_response)(const char *payload); /* ack an issue */
    app_err_t (*request_rtc_access)(cloud_rtc_access_t *out);
    void      (*report_info)(void);                   /* device info report */
    void      (*time_sync)(void);                     /* request time sync */
    void      (*get_device_info)(const char **pid,
                                 const char **uuid);  /* product/device IDs */
} cloud_ops_t;

#ifdef __cplusplus
}
#endif

#endif /* APP_MODULE_H */

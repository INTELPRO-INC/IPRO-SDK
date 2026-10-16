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
 * @file rtc_agora.c
 * @brief Agora RTC module — rtc_ops_t adapter
 *
 * Wraps the existing app_agora.c API into the rtc_ops_t interface.
 * The actual Agora SDK implementation remains in app_agora.c; this
 * file is a thin adapter that maps ops callbacks to app_agora_* calls.
 */

#include "rtc_agora.h"
#include "app_agora.h"

#include "app_log.h"

#define LOG_TAG "RTC_AGORA"

/* Stored callbacks for AI state and events */
static rtc_ai_state_cb_t s_ai_state_cb;
static rtc_event_cb_t    s_event_cb;

/* Stored join params for rejoin */
static rtc_join_params_t s_last_join;

/* ------------------------------------------------------------------ */
/* rtc_ops_t implementation                                           */
/* ------------------------------------------------------------------ */

static app_err_t agora_init(void)
{
    /* SDK-level init is deferred to join (app_agora_init does both) */
    return APP_OK;
}

static app_err_t agora_deinit(void)
{
    app_agora_leave_channel();
    return APP_OK;
}

static app_err_t agora_join(const rtc_join_params_t *p)
{
    if (!p || !p->app_id || !p->token || !p->channel)
        return APP_ERR_INVALID;

    /* Store for rejoin */
    s_last_join = *p;

    int ret = app_agora_init(p->app_id, p->token, p->channel, p->uid);
    return (ret == 0) ? APP_OK : APP_ERR_AGORA;
}

static app_err_t agora_leave(void)
{
    int ret = app_agora_leave_channel();
    return (ret == 0) ? APP_OK : APP_ERR_AGORA;
}

static app_err_t agora_rejoin(const rtc_join_params_t *p)
{
    const rtc_join_params_t *params = p ? p : &s_last_join;
    if (!params->channel || !params->token)
        return APP_ERR_INVALID;

    int ret = app_agora_rejoin(params->app_id, params->token,
                                params->channel, params->uid);
    if (ret == 0 && p)
        s_last_join = *p;
    return (ret == 0) ? APP_OK : APP_ERR_AGORA;
}

static int agora_is_connected(void)
{
    return app_agora_is_connected();
}

static app_err_t agora_send_audio(const void *data, size_t len)
{
    int ret = app_agora_send_audio(data, len);
    return (ret == 0) ? APP_OK : APP_ERR_AGORA;
}

static void agora_set_rx_callback(rtc_rx_cb_t cb)
{
    /* agora_rx_cb_t and rtc_rx_cb_t have the same signature */
    app_agora_set_rx_callback(cb);
}

/* Internal callback adapters — translate int-based callbacks to typed enums */
static void on_agora_event(int event, uint32_t uid)
{
    if (s_event_cb && event >= 0 && event <= 6)
        s_event_cb((rtc_event_t)event, uid);
}

static void on_agora_ai_state(int state)
{
    if (s_ai_state_cb && state >= 0 && state <= 3)
        s_ai_state_cb((rtc_ai_state_t)state);
}

static void agora_set_ai_state_callback(rtc_ai_state_cb_t cb)
{
    s_ai_state_cb = cb;
    app_agora_set_ai_state_callback(on_agora_ai_state);
}

static void agora_set_event_callback(rtc_event_cb_t cb)
{
    s_event_cb = cb;
    app_agora_set_event_callback(on_agora_event);
}

static int agora_is_ai_speaking(void)
{
    return app_agora_is_ai_speaking();
}

static uint32_t agora_get_remote_user_count(void)
{
    return app_agora_get_remote_user_count();
}

const rtc_ops_t rtc_agora_ops = {
    .name                  = "agora",
    .init                  = agora_init,
    .deinit                = agora_deinit,
    .join                  = agora_join,
    .leave                 = agora_leave,
    .rejoin                = agora_rejoin,
    .is_connected          = agora_is_connected,
    .send_audio            = agora_send_audio,
    .set_rx_callback       = agora_set_rx_callback,
    .set_ai_state_callback = agora_set_ai_state_callback,
    .set_event_callback    = agora_set_event_callback,
    .is_ai_speaking        = agora_is_ai_speaking,
    .get_remote_user_count = agora_get_remote_user_count,
};

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

#include <rnm_server.h>

#include <assert.h>
#include <string.h>
#ifndef CFG_IPRO_WIFI
#include <bl_wifi.h>
#include <aos/yloop.h>
#else
/* On IPRO chips, bridge the legacy aos_register_event_filter API to ipro_event. */
#include "ipro_event.h"
IPRO_EVENT_DECLARE_BASE(WIFI_EVENT);
typedef struct { int32_t code; int32_t value; } input_event_t;
#endif
#include <wifi_mgmr_ext.h>

static inline void inc_stat_scratch_ctr(rnms_t *rnm, int id)
{
    ++rnm->statistic.scratch_counter[id];
}

static void rnms_mgmr_task(void *pvParameters)
{
    rnms_t *env = (rnms_t *)pvParameters;
    rnm_msg_t msg;

    while (1) {
        if (pdPASS == xQueueReceive(env->cmd_msg_queue, &msg, portMAX_DELAY)) {
            rnms_msg_process(env, &msg);
        } else {
            rnm_err("read msg queue failed\r\n");
        }
    }
}

int rnm_cmd_input(rnms_t *rnm, const rnm_msg_t *msg, int timeout_ms)
{
    if (!(rnm && msg)) {
        return -1;
    }

    TickType_t timeout;
    if (timeout_ms == -1) {
        timeout = portMAX_DELAY;
    } else {
        timeout = pdMS_TO_TICKS(timeout_ms);
    }
    if (pdPASS == xQueueSendToBack(rnm->cmd_msg_queue, msg, timeout)) {
        return 0;
    } else {
        return -1;
    }
}

int rnms_cmd_input_raw(rnms_t *rnm, const void *data, uint16_t data_len)
{
    rnm_msg_t msg;
    void *payload;

    assert(rnm && data && data_len);
    assert(data_len >= sizeof(rnm_base_msg_t));

    memset(&msg, 0, sizeof(msg));
    memcpy(&msg.cmd, data, sizeof(rnm_base_msg_t));
    if (data_len > sizeof(rnm_base_msg_t)) {
        payload = rnm_malloc(data_len);
        if (!payload) {
            inc_stat_scratch_ctr(rnm, RNMS_STAT_ERR_INPUT_NOMEM);
            return -1;
        }
        memcpy(payload, data, data_len);
        msg.free_fn = rnm_free;
        msg.data = payload;
        msg.data_len = data_len;
    }
    return rnm_cmd_input(rnm, &msg, -1);
}

#ifdef CFG_IPRO_WIFI
static void wifi_event_cb(input_event_t *event, void *private_data);
static void rnms_ipro_wifi_event_trampoline(const ipro_event_t *event, void *user_data)
{
    if (event == NULL || event->event_base != WIFI_EVENT) {
        return;
    }
    input_event_t ev = { .code = event->event_id, .value = 0 };
    wifi_event_cb(&ev, user_data);
}
#endif

static void wifi_event_cb(input_event_t *event, void *private_data)
{
    rnms_t *rnm = (rnms_t *)private_data;

    if (event->code == CODE_WIFI_ON_CONNECTED) {
        rnms_send_simple_async_msg(rnm, IPRO_CMD_STA_CONNECTED_IND);
    } else if (event->code == CODE_WIFI_ON_DISCONNECT) {
        rnm->have_valid_sta_ip = false;
        rnms_send_simple_async_msg(rnm, IPRO_CMD_STA_DISCONNECTED_IND);
    } else if (event->code == CODE_WIFI_ON_AP_STARTED) {
        rnms_send_simple_async_msg(rnm, IPRO_CMD_AP_STARTED_IND);
    } else if (event->code == CODE_WIFI_ON_AP_STOPPED) {
        rnms_send_simple_async_msg(rnm, IPRO_CMD_AP_STOPPED_IND);
    } else if (event->code == CODE_WIFI_ON_GOT_IP) {
        rnm->have_valid_sta_ip = true;
        rnms_notify_sta_ip_addr(rnm);
    }
}

rnms_t *rnm_server_init(void *output_fn_env, rnm_output_t output_fn, void *ready_cb_arg, rnm_ready_cb_t ready_cb)
{
    rnms_t *env;

    // hack to check limit at compile time
    if (!(RNMS_STAT_MAX <= RNM_STATISTIC_SCRATCH_COUNTER_NUM)) {
        void RNMS_No_enough_scratch_counter_for_stat_Consider_increase_counters(void);
        RNMS_No_enough_scratch_counter_for_stat_Consider_increase_counters();
    }

    if ((env = rnm_malloc(sizeof(rnms_t))) == NULL) {
        return NULL;
    }

    memset(env, 0, sizeof(rnms_t));

    env->output_fn_env = output_fn_env;
    env->output_fn = output_fn;

    env->ready_cb_arg = ready_cb_arg;
    env->ready_cb = ready_cb;

    env->cmd_msg_queue = xQueueCreateStatic(RNMS_CMD_MSG_QUEUE_CAP, RNMS_CMD_MSG_SIZE, env->cmd_msg_queue_storage_buffer, &env->cmd_msg_queue_buffer);
    xTaskCreateStatic(rnms_mgmr_task, "rnms", RNMS_TASK_STACK_DEPTH, env, RNMS_TASK_PRIORITY, env->cmd_task_stack_buffer, &env->cmd_task_buffer);

#ifdef CFG_IPRO_WIFI
    ipro_event_handler_register(WIFI_EVENT, IPRO_EVENT_ANY_ID, rnms_ipro_wifi_event_trampoline, env);
#else
    aos_register_event_filter(EV_WIFI, wifi_event_cb, env);
#endif

    return env;
}

int rnms_heartbeat_register_failure_cb(rnms_t *rnm, rnm_heartbeat_failure_cb_t cb, void *cb_arg)
{
    if (!rnm) {
        return -1;
    }

    rnm->heartbeat_failure_cb = cb;
    rnm->heartbeat_failure_cb_arg = cb_arg;

    return 0;
}

static void heartbeat_timer_cb(TimerHandle_t timer)
{
    rnms_t *rnm = pvTimerGetTimerID(timer);
    rnms_send_simple_async_msg_timeout(rnm, IPRO_CMD_PING, 0);
}

int rnms_heartbeat_start(rnms_t *rnm, unsigned interval_ms, uint8_t threshold)
{
    rnms_heartbeat_stop(rnm);

    rnm->heartbeat_threshold = threshold;
    rnm->heartbeat_failures = 0;
    rnm->heartbeat_timer = xTimerCreateStatic("rnm_hb", pdMS_TO_TICKS(interval_ms),
            pdTRUE, rnm, heartbeat_timer_cb, &rnm->heartbeat_timer_buffer);
    xTimerStart(rnm->heartbeat_timer, portMAX_DELAY);
    return 0;
}

int rnms_heartbeat_stop(rnms_t *rnm)
{
    if (rnm->heartbeat_timer) {
        xTimerDelete(rnm->heartbeat_timer, portMAX_DELAY);
        rnm->heartbeat_timer = NULL;
        return 0;
    }
    return -1;
}

int rnms_send_transparent_msg(rnms_t *rnm, rnm_msg_t *msg, int timeout_ms)
{
    if (!(rnm && msg)) {
        return -1;
    }
    msg->cmd.flags |= RNM_MSG_FLAG_TRANSPARENT;
    return rnm_cmd_input(rnm, msg, timeout_ms);
}

int rnms_send_simple_async_msg_timeout(rnms_t *rnm, ipro_cmd_t cmd, int timeout_ms)
{
    rnm_msg_t msg;

    memset(&msg, 0, sizeof(msg));
    msg.cmd.cmd = cmd;
    msg.cmd.flags |= RNM_MSG_FLAG_ASYNC;
    return rnms_send_transparent_msg(rnm, &msg, timeout_ms);
}

int rnms_send_simple_async_msg(rnms_t *rnm, ipro_cmd_t cmd)
{
    return rnms_send_simple_async_msg_timeout(rnm, cmd, -1);
}

int rnms_msg_output(rnms_t *rnm, const void *data, uint16_t data_length)
{
    rnm_base_msg_t *msg = (rnm_base_msg_t *)data;

    if (!(msg->cmd < IPRO_CMD_MAX)) {
        rnm_err("Unknown cmd %u!\r\n", msg->cmd);
        return -1;
    }
    rnm->statistic.cmds[msg->cmd].tx_cnt++;
    return rnm->output_fn(rnm->output_fn_env, RNM_OUT_COMMAND, data, data_length);
}

bool rnms_msg_is_get_mac(const void *msg)
{
    const rnm_base_msg_t *m = msg;
    if (m->cmd == IPRO_CMD_GET_MAC_ADDR) {
        return true;
    } else {
        return false;
    }
}

void rnms_msg_fill_common(rnms_t *rnm, void *resp, const rnm_msg_t *msg)
{
    rnm_base_msg_t *hdr = resp;
    const rnm_base_msg_t *m = &msg->cmd;

    hdr->cmd = m->cmd;
    hdr->flags = RNM_MSG_FLAG_ACK;
    hdr->status = STATUS_OK;
    hdr->session_id = m->session_id;
    hdr->msg_id = ++rnm->last_msg_id;
    hdr->msg_id_replying = m->msg_id;
}

#define MD(id, desc) RNM_MAKE_STAT_SCR_CTR_DESC(id, desc)

static rnm_stat_scr_ctr_desc_t rnms_stat_scr_ctr_desc[RNMS_STAT_MAX] = {
    MD(RNMS_STAT_ERR_INVALID_CMD, "invalid CMD"),
    MD(RNMS_STAT_ERR_INPUT_NOMEM, "malloc fail when RX msg"),
    MD(RNMS_STAT_ERR_OUTPUT_NOMEM, "malloc fail when TX msg"),
};

int rnms_statistic_dump(rnms_t *rnm)
{
    return rnm_statistic_dump(&rnm->statistic, rnms_stat_scr_ctr_desc, RNMS_STAT_MAX);
}

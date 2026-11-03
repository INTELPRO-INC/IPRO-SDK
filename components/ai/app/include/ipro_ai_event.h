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
 * @file ipro_ai_event.h
 * @brief Inference results as events: publish from the inference side,
 *        subscribe from anywhere (a GPIO, a message, the USB link).
 *
 * Small fixed-size events in a bounded queue, delivered in a dispatcher task
 * so a subscriber never runs on the inference task. Results only: raw
 * camera, audio or sensor streams never go through here.
 *
 * Overflow: when the queue is full the oldest event is dropped and counted;
 * the next delivery is preceded by an IPRO_AI_EVT_OVERFLOW event whose
 * `index` is how many were lost. Subscribers should be short (they share
 * the dispatcher); one that needs to block hands the event to its own task.
 */
#ifndef IPRO_AI_EVENT_H
#define IPRO_AI_EVENT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IPRO_AI_EVENT_NAME_MAX   15u
#define IPRO_AI_EVENT_SUBS_MAX   8u
#define IPRO_AI_EVENT_DEPTH_MAX  64u

typedef enum {
    IPRO_AI_EVT_CLASS    = 1,   /* a classification: index, label, score = probability x 1000 */
    IPRO_AI_EVT_UNKNOWN  = 2,   /* the best class scored under its floor */
    IPRO_AI_EVT_ANOMALY  = 3,   /* an anomaly alarm started (index 1) or ended (index 0) */
    IPRO_AI_EVT_SPEAKER  = 4,   /* a voice was named: label = name ("" for a stranger), score = cosine x 1000 */
    IPRO_AI_EVT_ERROR    = 5,   /* index = IPRO_AI_E_* or an app's code */
    IPRO_AI_EVT_OVERFLOW = 6,   /* index = events lost to a full queue before this one */
    IPRO_AI_EVT_CHANGED  = 7,   /* the reported class changed: index, label */
} ipro_ai_evt_type_t;

#define IPRO_AI_EVT_BIT(t) (1u << (t))
#define IPRO_AI_EVT_ALL    0xFFFFFFFFu

typedef struct {
    uint8_t  type;                          /* ipro_ai_evt_type_t */
    uint8_t  model_id;                      /* the publisher's model number */
    uint16_t reserved;
    uint32_t seq;                           /* publish order, from 1 */
    uint64_t t_us;                          /* when the input was captured (source time) */
    int32_t  index;                         /* class, alarm state, error code, lost count */
    int32_t  score;                         /* x1000 */
    char     model[IPRO_AI_EVENT_NAME_MAX + 1];
    char     label[IPRO_AI_EVENT_NAME_MAX + 1];
} ipro_ai_event_t;

typedef void (*ipro_ai_event_cb)(const ipro_ai_event_t *e, void *ctx);

typedef struct {
    uint32_t published, delivered, dropped, depth, high_water;
} ipro_ai_event_stats_t;

/**
 * @brief Set up the queue (depth 1..IPRO_AI_EVENT_DEPTH_MAX) and, with
 *        own_task, the dispatcher task. Idempotent: a second call keeps the
 *        first configuration.
 */
int  ipro_ai_event_init(uint32_t depth, bool own_task);
/** @return a handle >= 0, or IPRO_AI_E_NOMEM when every slot is taken. */
int  ipro_ai_event_subscribe(uint32_t type_mask, ipro_ai_event_cb cb, void *ctx);
/** A delivery already under way may still call cb once after this returns:
 *  keep ctx valid until the next event has been dispatched. */
int  ipro_ai_event_unsubscribe(int handle);
/** Never blocks; safe from any task (not from an ISR). Fills seq. */
int  ipro_ai_event_publish(const ipro_ai_event_t *e);
/** Deliver what is queued (the dispatcher task's loop body; for tests and
 *  for apps without the task). Waits up to timeout_ms for the first one.
 *  @return events delivered */
uint32_t ipro_ai_event_dispatch(uint32_t timeout_ms);
int  ipro_ai_event_get_stats(ipro_ai_event_stats_t *st);
const char *ipro_ai_event_type_name(uint8_t type);

/**
 * @brief Format an event as the device protocol's EVT line (06 §5):
 *        "EVT t=<ms> type=<name> model=<m> label=<l> score=<s> index=<i>".
 * @return characters written (without the NUL), as snprintf.
 */
int ipro_ai_event_format(const ipro_ai_event_t *e, char *buf, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_AI_EVENT_H */

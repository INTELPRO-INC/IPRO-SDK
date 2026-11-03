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
 * @file ipro_ai_app_port.h
 * @brief What the app framework and the event queue need from the platform.
 *        IPRO7: ipro_ai_app_port_ipro7.c; host tests: test_host/fake_port.c.
 */
#ifndef IPRO_AI_APP_PORT_H
#define IPRO_AI_APP_PORT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

uint64_t ai_app_port_now_us(void);
void    *ai_app_port_malloc(size_t bytes);
void     ai_app_port_free(void *p);

void    *ai_app_port_mutex_new(void);
void     ai_app_port_mutex_lock(void *m);
void     ai_app_port_mutex_unlock(void *m);

void    *ai_app_port_sem_new(void);           /* binary, starts empty */
void     ai_app_port_sem_give(void *s);
bool     ai_app_port_sem_take(void *s, uint32_t timeout_ms);

/** Start fn(arg) in a task; 0 or an IPRO_AI_E_* code. */
int      ai_app_port_task_start(void **task, const char *name, void (*fn)(void *arg), void *arg,
                                uint32_t stack_words, uint32_t priority);
/** Called by the task's function as its last act (deletes the task). */
void     ai_app_port_task_exit(void);
void     ai_app_port_sleep_ms(uint32_t ms);
/** Heap bytes free now and the lowest it has been. */
void     ai_app_port_heap(uint32_t *free_bytes, uint32_t *min_bytes);

#endif /* IPRO_AI_APP_PORT_H */

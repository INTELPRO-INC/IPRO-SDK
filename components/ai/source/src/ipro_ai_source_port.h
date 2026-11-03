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
 * @file ipro_ai_source_port.h
 * @brief What the source framework needs from the platform. The IPRO7 port
 *        is ipro_ai_source_port_ipro7.c; the host tests bring a fake clock.
 */
#ifndef IPRO_AI_SOURCE_PORT_H
#define IPRO_AI_SOURCE_PORT_H

#include <stdint.h>

uint64_t ai_src_port_now_us(void);
/** Sleep at least ms (0: yield). */
void ai_src_port_sleep_ms(uint32_t ms);
/** Order the producer's buffer writes before its index write (and the reverse
 *  on the consumer side). */
void ai_src_port_barrier(void);

/**
 * Run tick(arg) `due` times per period so that, on average, it is called
 * rate times a second, until ai_src_port_periodic_stop. *task receives a
 * handle. IPRO_AI_E_NOMEM when the task cannot be created.
 */
int  ai_src_port_periodic_start(void **task, void (*tick)(void *arg), void *arg, uint32_t rate);
void ai_src_port_periodic_stop(void **task);

#endif /* IPRO_AI_SOURCE_PORT_H */

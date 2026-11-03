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
 * @file ipro_ai_port.h
 * @brief What the runtime (ipro_ai_runtime.c) needs from the platform.
 *
 * ipro_ai_port_ipro7.c implements it with FreeRTOS, components/ai and the NPU
 * HAL; test_host/fake_port.c implements it on the host. Everything that
 * touches IPROAI_Model_t or an NPU register lives behind this interface.
 */
#ifndef IPRO_AI_PORT_H
#define IPRO_AI_PORT_H

#include <stddef.h>
#include <stdint.h>

#include "ipro_ai.h"   /* ipro_ai_npu_health_t */

#ifdef __cplusplus
extern "C" {
#endif

/** What the runtime reads from a loaded model. */
typedef struct {
    uint16_t version;
    uint16_t layers;
    uint16_t npu_layers;
    uint8_t  ext_input;
    uint8_t  unsign_input;
    uint8_t  outputs;
    uint16_t in_w, in_h, in_c;
    uint16_t out_w, out_h, out_c;
    uint8_t  out_on_npu;      /* the last layer runs on the NPU: its channels are padded to 4 */
    float    out_scale;
    int32_t  out_zero_point;
    uint8_t *work_input;      /* input slot in the work buffer (models without external input) */
} ai_port_net_t;

int  ai_port_init(void);
void ai_port_deinit(void);

/** Blocking NPU lock; ai_port_trylock() returns 1 when it got it. */
void ai_port_lock(void);
int  ai_port_trylock(void);
void ai_port_unlock(void);

uint64_t ai_port_now_us(void);
void    *ai_port_alloc(size_t bytes);
void     ai_port_free(void *p);

/** Load under the lock; ext_input is where the NPU will read the input (NULL when the model has none). */
int  ai_port_load(const void *inm, uint8_t *ext_input, void **hdl, ai_port_net_t *net);
void ai_port_unload(void *hdl);

/** Run under the lock: restore this model's NPU state, point the NPU at input when external
 *  is set, clean the data cache over clean_bytes of input (0: the caller made it visible,
 *  e.g. a DMA'd frame), run with timeout_ms per NPU segment (0: the default), invalidate
 *  out_bytes of the output. Returns 0, IPRO_AI_E_RUN, IPRO_AI_E_TIMEOUT (the NPU was reset
 *  before returning) or IPRO_AI_E_FAULT (refused, or the reset did not help). */
int ai_port_run(void *hdl, const uint8_t *input, int external, uint32_t clean_bytes,
                const uint8_t **out, uint32_t out_bytes, uint32_t timeout_ms);

/** The NPU completion path's counters; state is left for the runtime to fill. */
void ai_port_npu_health(ipro_ai_npu_health_t *h);
/** Reset the NPU block and accept runs again (under the lock). */
void ai_port_npu_clear_fault(void);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_AI_PORT_H */

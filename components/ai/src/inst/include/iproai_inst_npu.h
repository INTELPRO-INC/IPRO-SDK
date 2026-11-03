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

#ifndef __IPROAI_INST_NPU_H__
#define __IPROAI_INST_NPU_H__

//#include "iproai_inst_npu_encode.h"


IPROAI_Status_e iproai_npu_inst_decode_layer(iproai_model_hdl_t hdl);
IPROAI_ProcStatus_e iproai_npu_inference();

/* Bounded NPU completion (CONFIG_AI_NPU_TIMEOUT_MS): health of the one completion path
 * every NPU client shares. */
typedef struct {
    uint32_t completed;            /* segments that signalled done */
    uint32_t timeouts;
    uint32_t recoveries;           /* block resets */
    uint32_t refused;              /* runs refused while faulted */
    uint32_t timeout_ms;           /* the configured default */
    uint16_t consecutive_timeouts;
    uint8_t  faulted;
} iproai_npu_health_t;

/* 0 restores CONFIG_AI_NPU_TIMEOUT_MS. Set it under the NPU lock, for the run that follows. */
void iproai_npu_set_timeout_ms(uint32_t ms);
uint32_t iproai_npu_get_timeout_ms(void);
void iproai_npu_get_health(iproai_npu_health_t *h);
/* Reset the block again and accept runs. The supported recovery from a fault is a restart. */
void iproai_npu_clear_fault(void);
/* Why the last run failed in the NPU (TIMEOUT or FAULTED), and forget it; NO_ERROR if not. */
IPROAI_Status_e iproai_npu_take_error(void);
IPROAI_Status_e iproai_npu_stop(void);
IPROAI_Status_e iproai_npu_initCfg(iproai_model_hdl_t hdl);
IPROAI_Status_e iproai_npu_release(iproai_model_hdl_t hdl);
IPROAI_Status_e iproai_npu_layer_config(uint8_t *inst_buf, uint8_t *weights_buf,
    uint8_t *bias_buf, uint8_t *mult_shift_buf, uint8_t *input_buf, int patch_size, uint8_t b_isFirstLayers);
bool forward_NPU(struct iproai_net_info_t* net, fixed_point_t* DATA_buf, int l_current);
void iproai_npu_exec_lock_required(void);
void iproai_npu_exec_lock_release(bool in_isr);
void iproai_npu_init(void);
void iproai_npu_destroy(void);

#endif /* __IPROAI_INST_NPU_H__ */


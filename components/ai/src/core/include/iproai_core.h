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

#ifndef __IPROAI_CORE_H__
#define __IPROAI_CORE_H__

#include <stdbool.h>
#include "iproai_def.h"
#include "iproai_inst_cpu.h"
#include "iproai_inst_npu.h"
#include "iproai_inst_process.h"
#include "iproai_inst_util.h"

iproai_model_hdl_t iproai_create(void);
struct iproai_net_info_t *iproai_getNetInfo(iproai_model_hdl_t hdl);
IPROAI_Status_e iproai_load_model_from_file(iproai_model_hdl_t hdl, const char *name);
IPROAI_Status_e iproai_load_model_from_buffer(iproai_model_hdl_t hdl, const uint8_t *buffer);
uint8_t* iproai_getInputBuffer(iproai_model_hdl_t hdl);
uint8_t* iproai_getOutputBuffer(iproai_model_hdl_t hdl, uint32_t *size);
IPROAI_Status_e iproai_free(iproai_model_hdl_t hdl);
IPROAI_Status_e iproai_setResultCB(iproai_model_hdl_t hdl, iproai_inference_cb cb);
IPROAI_Status_e iproai_setCustom_Postproc_CB(iproai_model_hdl_t hdl, iproai_custom_postproc_cb cb);
IPROAI_Status_e iproai_startCompute(iproai_model_hdl_t hdl);
IPROAI_Status_e iproai_getInputResolution(iproai_model_hdl_t hdl, uint32_t* width, uint32_t* height);
IPROAI_Status_e iproai_setSourceResolution(iproai_model_hdl_t hdl, uint32_t width, uint32_t height);
bool check_IPROAI_NPU_RUN(struct iproai_net_info_t* net, struct cpu_inst_layer_t *l);

#endif /* __IPROAI_CORE_H__ */


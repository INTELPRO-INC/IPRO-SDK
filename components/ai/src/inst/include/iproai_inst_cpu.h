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

#ifndef __IPROAI_INST_CPU_H__
#define __IPROAI_INST_CPU_H__

#include "iproai_inst_cpu_ops.h"

IPROAI_ProcStatus_e iproai_cpu_inference(iproai_model_hdl_t hdl);
IPROAI_Status_e iproai_cpu_inst_decode_layer(iproai_model_hdl_t hdl);
IPROAI_Status_e iproai_cpu_release(iproai_model_hdl_t hdl);
fixed_point_t* iproai_get_output_buffer(iproai_model_hdl_t hdl);
#endif /* __IPROAI_INST_CPU_H__ */


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
 * @file npu_share.c
 * @brief NPU arbitration between the gesture and person models. See npu_share.h.
 */

#include "npu_share.h"

#include <iproai_core.h>
#include <iproai_def.h>      /* IPROAI_Model_t: the loaded sections */
#include "hal_npu.h"
#include "ipro_ai.h"         /* ipro_ai_npu_lock: the one NPU lock in the image */

int npu_share_init(void)
{
    return ipro_ai_init() == IPRO_AI_OK ? 0 : -1;
}

void npu_share_begin(iproai_model_hdl_t hdl, uint32_t ext_addr)
{
    ipro_ai_npu_lock();
    if (hdl) {
        /* what iproai_npu_initCfg() wrote for this model at load time, as the
         * runtime's port does before every run (ipro_ai_port_ipro7.c) */
        const IPROAI_Model_t *md = (const IPROAI_Model_t *)hdl;
        hal_npu_layer_setup(md->inst.npu.inst_buf, md->inst.npu.weights_buf, md->inst.npu.bias_buf,
                            md->inst.npu.mult_shift_buf);
        hal_npu_set_input_buffer(md->buffer, md->net->patch_size);
        hal_npu_set_net_param((uint8_t)md->net->unsign_input, (uint8_t)md->net->relu_n,
                              (uint8_t)md->net->use_tflite, (uint32_t)md->net->mode);
    }
    if (ext_addr) {
        NPU_Img_Ext_Addr_Cfg(ext_addr);
    }
}

void npu_share_end(void)
{
    ipro_ai_npu_unlock();
}

void npu_share_load_begin(void)
{
    ipro_ai_npu_lock();
}

void npu_share_load_end(void)
{
    ipro_ai_npu_unlock();
}

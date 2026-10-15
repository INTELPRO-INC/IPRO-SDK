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

#include <FreeRTOS.h>
#include <semphr.h>

#include <iproai_core.h>
#include <iproai_def.h>
#include "hal_npu.h"

static SemaphoreHandle_t s_lock;

int npu_share_init(void)
{
    if (!s_lock) {
        s_lock = xSemaphoreCreateMutex();
    }
    return s_lock ? 0 : -1;
}

void npu_share_begin(const struct iproai_net_info_t *net, uint32_t ext_addr)
{
    if (s_lock) {
        xSemaphoreTake(s_lock, portMAX_DELAY);
    }
    if (net) {
        /* What iproai_npu_initCfg() wrote for this model at load time; the
         * other model's load overwrote it. */
        hal_npu_set_net_param((uint8_t)net->unsign_input, (uint8_t)net->relu_n,
                              (uint8_t)net->use_tflite, (uint32_t)net->mode);
    }
    if (ext_addr) {
        NPU_Img_Ext_Addr_Cfg(ext_addr);
    }
}

void npu_share_end(void)
{
    if (s_lock) {
        xSemaphoreGive(s_lock);
    }
}

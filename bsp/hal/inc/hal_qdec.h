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

#ifndef __HAL_QDEC__H__
#define __HAL_QDEC__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "board_config.h"

enum {
    QDEC_CTRL_SET_INT,
    QDEC_CTRL_CLR_INT,
    QDEC_CTRL_RESUME,
    QDEC_CTRL_SUSPEND,
    QDEC_CTRL_GET_SAMPLE_VAL,
    QDEC_CTRL_GET_SAMPLE_DIR,
    QDEC_CTRL_GET_ERROR_CNT,
    QDEC_CTRL_SET_CALLBACK,
};

enum qdec_index_type {
    QDEC0_INDEX,
    QDEC1_INDEX,
    QDEC2_INDEX,
    QDEC_MAX_INDEX,
};

enum qdec_event_type {
    QDEC_REPORT_EVENT = 1 << 0,   /*!< report interrupt */
    QDEC_SAMPLE_EVENT = 1 << 1,   /*!< sample interrupt */
    QDEC_ERROR_EVENT = 1 << 2,    /*!< error interrupt */
    QDEC_OVERFLOW_EVENT = 1 << 3, /*!< ACC1 and ACC2 overflow interrupt */
    QDEC_ALL_EVENT = 1 << 4,      /*!< interrupt max num */
};

typedef struct qdec_device {
    uint8_t id;
    uint16_t caps;
    void (*callback)(void *dev, void *args, uint32_t size, uint32_t event);
    uint8_t acc_mode;
    uint8_t sample_mode;
    uint8_t sample_period;
    uint8_t report_mode;
    uint32_t report_period;
    uint8_t led_en;
    uint8_t led_swap;
    uint16_t led_period;
    uint8_t deglitch_en;
    uint8_t deglitch_strength;
} qdec_device_t;

#ifdef __cplusplus
}
#endif
#endif // __HAL_QDEC_H__

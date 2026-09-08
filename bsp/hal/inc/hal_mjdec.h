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

#ifndef __HAL_MJDEC_H__
#define __HAL_MJDEC_H__

#include "hal_common.h"
#include "board_config.h"

//#define COMMON_AlignCeil(x, N)          ( ((x) + ((N) - 1)) / (N) * (N) ) /*abner test*/

enum {
    MJDEC_HAL_INT_NORMAL = 0,
    MJDEC_HAL_INT_BACK_IDLE,
#if defined(CONFIG_IPRO6)
    MJDEC_HAL_INT_KICK,
#endif
    MJDEC_HAL_INT_ALL,
};

enum {
    MJDEC_HAL_FORMAT_YUV420_SP = 0,
    MJDEC_HAL_FORMAT_YUV400    = 1,
    MJDEC_HAL_FORMAT_YUV422_SP = 2,
};


typedef struct
{
    uint16_t width;
    uint16_t height;
    //--N/A for new HW.only //uint8_t quality;
    uint8_t *input_buf_addr;
    uint8_t *output_buf_addr_yy;
    uint8_t *output_buf_addr_uv;
    uint8_t yuv_mode;
    uint8_t is_skip_header;
    uint8_t out_u_order_even;
#if defined(CONFIG_IPRO6)
    uint8_t out_kick_mode_en;
    uint8_t out_kick_mode_sel;  /*0: single block mode. 1: block line mode*/
#endif
} mjdec_config_t;

typedef struct {
    uint8_t     *frame_start_addr_yy;
    uint8_t     *frame_start_addr_uv;
    uint32_t    frame_size_yy;
    uint32_t    frame_size_uv;
    uint8_t     ucIntType;
} mjdec_frame_desc_t;


typedef int (*mjdec_frame_cb)(void *usrdata, mjdec_frame_desc_t *frame_desc);

int mjdec_init(mjdec_config_t *config);
int mjdec_deinit(void);
void mjdec_disable(void);
int mjdec_start_decoder(uint8_t *jpeg_buf_addr);
int mjdec_set_callback(mjdec_frame_cb frame_cb, void *usrdata);

void mjdec_irq_handler(void);

#if defined(CONFIG_IPRO6)
void mjdec_kick(void);
void mjdec_set_out_addr(uint32_t out_yy_addr, uint32_t out_uv_addr);
#endif

#endif /* __HAL_MJDEC_H__ */


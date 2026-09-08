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

#ifndef __HAL_MJPEG__H__
#define __HAL_MJPEG__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "board_config.h"
#include "drv_mjpeg.h"


#if defined(CONFIG_IPRO7)
#define _MJPEG_SW_OUT_PING_PONG_MODE_  /*workaround for IPRO7 input ring mode stuck at 255 frame bug*/
#endif

#define YUV_MODE_400                   1
#define YUV_MODE_420                   2
#define YUV_MODE_422                   3

#define MJPEG_DEFAULT_QUALITY          50

enum {
    MJPEG_HAL_FORMAT_YUV422_YUYV = 0,
    MJPEG_HAL_FORMAT_YUV422_YVYU = 1,
    MJPEG_HAL_FORMAT_YUV422_UYVY = 2,
    MJPEG_HAL_FORMAT_YUV422_VYUY = 3,
    MJPEG_HAL_FORMAT_YUV422SP_NV16 = 4,
    MJPEG_HAL_FORMAT_YUV422SP_NV61 = 5,
    MJPEG_HAL_FORMAT_YUV420SP_NV12 = 6,
    MJPEG_HAL_FORMAT_YUV420SP_NV21 = 7,
    MJPEG_HAL_FORMAT_GRAY = 8,
};

enum {
    MJENC_HAL_INT_NORMAL = 0,
    MJENC_HAL_INT_IDLE,
    MJENC_HAL_INT_SWAP,
    MJENC_HAL_INT_ERR_CAM_OVERWRITE,    /*input frame overwrite*/
    MJENC_HAL_INT_ERR_MEM_OVERWRITE,    /*Output buffer full*/
    MJENC_HAL_INT_ERR_FRAME_OVERWRITE,  /*FIFO lost new frame done info due to no more room to store new fd info.*/
};

enum {
    MJPEG_HAL_SET_ADDR_Y = 0,
    MJPEG_HAL_SET_ADDR_UV,
};

struct mjpeg_hal_cfg_t {
    uint8_t format;
    uint8_t quality;
    uint16_t rows;
    uint16_t resolution_x;
    uint16_t resolution_y;
    uint32_t input_bufaddr0;
    uint32_t input_bufaddr1;
    uint32_t output_bufaddr;
    uint32_t output_bufsize;
    uint16_t *input_yy_table;
    uint16_t *input_uv_table;
    uint8_t output_swap_mode_en;    /*enable encode output buffer swap mode, 0 frame mode, 1 swap mode.*//*when swap enable, out bst buffer will be evenly divied into block0 and block1 as pingpong buffer.*/
    //removed //uint8_t hw_kick_mode_en;        /*for HW auto mode, enable kick mode*/
    uint8_t hw_mode_swen_en;        /*for HW auto mode, enable skip waiting first frame done*/
#if defined(CONFIG_IPRO7)
    uint8_t hw_mode_sshot_en;       /*for HW auto mode, only single shot HW and not auto restart HW after frame done.*/
#endif
    //removed //uint8_t yy_dvp2axi_sel;         /*for HW auto mode, dvp2axi_sel for yy input*/
    //removed //uint8_t uv_dvp2axi_sel;         /*for HW auto mode, dvp2axi_sel for uv input*/
    void (*callback)(uint32_t event);
};

#define mjpeg_swap_sts_info drv_mjpeg_swap_sts_info;

int  mjpeg_init(struct mjpeg_hal_cfg_t *mjpeg_hal_cfg);
int  mjpeg_deinit(void);
void mjpeg_start(int is_sw_mode);
void mjpeg_stop(void);
#if defined(CONFIG_IPRO6)
uint8_t mjpeg_get_one_frame(uint32_t *addr, uint32_t *len, uint32_t *sec_half_addr, uint32_t *sec_half_len);
#endif
#if defined(CONFIG_IPRO7)
uint8_t mjpeg_get_one_frame(uint32_t *addr, uint32_t *len);
#endif

void mjpeg_drop_one_frame(void);
void mjpeg_fill_header_tail(uint8_t *header, uint32_t header_len);
uint32_t mjpeg_headerCreate(uint8_t type, uint8_t q, int width, int height, uint8_t *out);
void mjpeg_set_mode(int is_sw_mode, int is_frame_mode,int frm_cnt);
void mjpeg_sw_run_next(void);
void mjpeg_sw_run_abort(void);
void mjpeg_sw_kick_block(uint16_t kick_count);
void mjpeg_sw_kick_stop(void);
void mjpeg_sw_kick(void);
void mjpeg_set_input_addr(int type, uint32_t addr); 
void mjpeg_get_swap_status(drv_mjpeg_swap_sts_info *swap_sts);
uint32_t mjpeg_get_swap_occur_cnt(void);
void mjpeg_free_swap_memory(void);

#ifdef __cplusplus
}
#endif
#endif

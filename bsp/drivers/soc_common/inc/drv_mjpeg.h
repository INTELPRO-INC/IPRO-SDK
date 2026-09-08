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

#ifndef __DRV_MJPEG_H__
#define __DRV_MJPEG_H__

#include "mjcomb_csr_reg.h"
#include "drv_common.h"

enum {
    MJPEG_FORMAT_YUV422_YUYV = 0,
    MJPEG_FORMAT_YUV422_YVYU = 1,
    MJPEG_FORMAT_YUV422_UYVY = 2,
    MJPEG_FORMAT_YUV422_VYUY = 3,
    MJPEG_FORMAT_YUV422SP_NV16 = 4,
    MJPEG_FORMAT_YUV422SP_NV61 = 5,
    MJPEG_FORMAT_YUV420SP_NV12 = 6,
    MJPEG_FORMAT_YUV420SP_NV21 = 7,
    MJPEG_FORMAT_GRAY = 8,
    /*MJPEG_FORMAT_YUV420P_YUV   = 9, *//*block mode not used in IPRO7*/
    /*MJPEG_FORMAT_YUV420P_YVU   = 10,*//*block mode not used in IPRO7*/
};

enum {
    MJPEG_SET_ADDR_Y = 0,
    MJPEG_SET_ADDR_UV,
};


#define MJENC_INT_STAT_NORMAL (1 << 8)
#define MJENC_INT_STAT_CAM_OV (1 << 9)
#define MJENC_INT_STAT_MEM_OV (1 << 10)
#define MJENC_INT_STAT_FRM_OV (1 << 11)
#define MJENC_INT_STAT_IDLE   (1 << 12)
#define MJENC_INT_STAT_SWAP   (1 << 13)


#define MJPEG_INTSTS_ONE_FRAME     (1 << 4)
#define MJPEG_INTCLR_ONE_FRAME     (1 << 8)
#define MJPEG_MAX_FRAME_COUNT      4

struct drv_mjpeg_config_t {
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

    //removed //uint8_t yy_dvp2axi_sel;         /*for HW auto mode, dvp2axi_sel for yy input*/
    //removed //uint8_t uv_dvp2axi_sel;         /*for HW auto mode, dvp2axi_sel for uv input*/
};

typedef enum {
    MJPEG_SW_FRAME_MODE, /*!< SW frame mode */
    MJPEG_SW_KICK_MODE,  /*!< SW kick mode */
} drv_mjpeg_sw_mode_e;

typedef struct drv_mjpeg_swap_sts_t {
    /* 0x9C : jenc_csr_7 */
    union {
        struct {
            uint32_t reserved_0_7                   :  8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t sts_enc_swap0_full             :  1; /* [    8],          r,        0x0 */
            uint32_t sts_enc_swap1_full             :  1; /* [    9],          r,        0x0 */
            uint32_t sts_enc_read_swap_idx          :  1; /* [   10],          r,        0x0 */
            uint32_t sts_enc_swap_fstart            :  1; /* [   11],          r,        0x0 */
            uint32_t sts_enc_swap_fend              :  1; /* [   12],          r,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_7;

    /* 0xA0 : jenc_csr_8 */
    union {
        struct {
            uint32_t sts_enc_frame_swap_end_bit_cnt : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_8;

} drv_mjpeg_swap_sts_info;

int  drv_mjpeg_init(const struct drv_mjpeg_config_t *config);
int  drv_mjpeg_deinit(void);
void drv_mjpeg_start(void);
void drv_mjpeg_stop(void);
void drv_mjpeg_sw_run(void);
void drv_mjpeg_sw_run_abort(void);
void drv_mjpeg_sw_enable(uint8_t frame_count, drv_mjpeg_sw_mode_e mode);
//void drv_mjpeg_sw_kick_run(uint16_t kick_count);
void drv_mjpeg_sw_kick_block(uint16_t kick_count);
void drv_mjpeg_sw_kick_stop(void);
void drv_mjpeg_sw_kick(void);
//removed //void drv_mjpeg_hw_kick(void);
void drv_mjpeg_tcint_mask(bool mask);
void drv_mjpeg_swapint_mask(bool mask);
void drv_mjpeg_idleint_mask(bool mask);
void drv_mjpeg_errint_mask(bool mask);
uint32_t drv_mjpeg_get_intstatus(void);
void drv_mjpeg_int_clear(uint32_t int_clear);
uint8_t drv_mjpeg_get_frame_count(void);
void drv_mjpeg_pop_one_frame(void);
uint32_t drv_mjpeg_get_frame_info(uint32_t *addr);
void drv_mjpeg_calculate_quantize_table(uint8_t quality, uint16_t *input_table, uint16_t *output_table);
void drv_mjpeg_fill_quantize_table(uint16_t *input_yy, uint16_t *input_uv);
void drv_mjpeg_fill_jpeg_header_tail(uint8_t *header, uint32_t header_len);
//removed //void drv_mjpeg_set_yuv420sp_cam_input(uint8_t yy, uint8_t uv);
int  drv_mjpeg_set_input_addr(int type, uint32_t addr);
int  drv_mjpeg_set_out_size_addr(uint32_t size, uint32_t addr);
void drv_mjpeg_get_out_size_addr_info(uint32_t *size, uint32_t *addr);
void  drv_mjpeg_set_hw_mode_swen(uint8_t value);
uint8_t drv_mjpeg_get_hw_mode_swen(void);
void drv_mjpeg_update_input_output_buff(void *input_buf0, void *input_buf1, void *output_buff, size_t output_buff_size);
void drv_mjpeg_get_swap_status(drv_mjpeg_swap_sts_info *swap_sts);
void drv_mjpeg_free_swap_memory(void);
#endif /* __DRV_MJPEG_H__ */

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

#ifndef __DRV_OSD_BLEND_H__
#define __DRV_OSD_BLEND_H__

#include "osd_blend_reg.h"
#include "drv_common.h"
#include <stdint.h>

/** @addtogroup  Peripheral_Driver
 *  @{
 */

/** @addtogroup  OSD_BLEND
 *  @{
 */

/** @defgroup  OSD_BLEND_Public_Macros
 *  @{
 */

/** @defgroup  OSD_BLEND layer definitions
 *  @{
 */

#define OSD_A_BLEND_LAYER0 ((osd_blend_reg_t *)(OSD_BLEND_BASE))

/*@} end of group OSD_BLEND_Public_Macros */

/** @defgroup  OSD_BLEND_Public_Constants
 *  @{
 */

/*@} end of group OSD_BLEND_Public_Constants */

/** @defgroup  OSD_BLEND_Public_Types
 *  @{
 */

/**
 *  @brief OSD_BLEND state control definition
 */
typedef enum {
    OSD_BLEND_ST_DISABLE = 0,
    OSD_BLEND_ST_ENABLE,
    OSD_BLEND_ST_MAX = 0x7FFFFFFF
} OSD_BLEND_ST_e;

/**
 *  @brief OSD_BLEND interrupt edge definition
 */
typedef enum {
    OSD_BLEND_INT_TYPE_SEOF = 0,
    OSD_BLEND_INT_TYPE_DRAIN,
    OSD_BLEND_INT_TYPE_MAX = 0x7F
} OSD_BLEND_INT_TYPE_e;

/**
 *  @brief OSD_BLEND interrupt edge definition
 */
typedef enum {
    OSD_BLEND_INT_EDGE_FALLING = 0,
    OSD_BLEND_INT_EDGE_RISING,
    OSD_BLEND_INT_EDGE_MAX = 0x7F
} OSD_BLEND_INT_EDGE_e;

/* order --- from LSB to MSB.
 * eg1: order_a = 3, order_rv=2, order_gy=1, order_bu=0
 * Byte Address: 0x7   0x6   0x5   0x4   0x3   0x2   0x1   0x0
 *     ARGB8888:  A1    R1    G1    B1    A0    R0    G0    B0
 *
 * eg2: order_a = 3, order_rv=0, order_gy=2, order_bu=1
 * Byte Address: 0x7   0x6   0x5   0x4   0x3   0x2   0x1   0x0
 *     AYUV8888:  A1    Y1    U1    V1    A0    Y0    U0    V0
 */

typedef enum {
    OSD_BLEND_COLOR_ORDER0 = 0,
    OSD_BLEND_COLOR_ORDER1,
    OSD_BLEND_COLOR_ORDER2,
    OSD_BLEND_COLOR_ORDER3,
    OSD_BLEND_COLOR_ORDER_MAX = 0x7F
} OSD_BLEND_COLOR_ORDER_e;

typedef enum {
    OSD_BLEND_COLOR_FMT_ARGB8888 = 0,
    OSD_BLEND_COLOR_FMT_AYUV8888,
    OSD_BLEND_COLOR_FMT_ARGB4444,
    OSD_BLEND_COLOR_FMT_AYUV4444,
    OSD_BLEND_COLOR_FMT_ARGB1555,
    OSD_BLEND_COLOR_FMT_AYUV1555,
    OSD_BLEND_COLOR_FMT_RGB565,
    OSD_BLEND_COLOR_FMT_YUV655,
    OSD_BLEND_COLOR_FMT_A8RGB,
    OSD_BLEND_COLOR_FMT_A8YUV,
    OSD_BLEND_COLOR_FMT_BPP8ARGB,
    OSD_BLEND_COLOR_FMT_BPP8AYUV,
    OSD_BLEND_COLOR_FMT_BPP4ARGB,
    OSD_BLEND_COLOR_FMT_BPP4AYUV,
    OSD_BLEND_COLOR_FMT_BPP2ARGB,
    OSD_BLEND_COLOR_FMT_BPP2AYUV,
    OSD_BLEND_COLOR_FMT_BPP1ARGB,
    OSD_BLEND_COLOR_FMT_BPP1AYUV,
    OSD_BLEND_COLOR_FMT_CNT,
    OSD_BLEND_COLOR_FMT_MAX = 0x7F
} OSD_BLEND_COLOR_FMT_e;

/* OSD blending keying mode: nomarl or invert */
typedef enum {
    OSB_BLEND_KEYING_NORMAL = 0,
    OSB_BLEND_KEYING_INVERT,
    OSB_BLEND_KEYING_MAX = 0x7F
} OSB_BLEND_KEYING_MODE_e;

typedef struct _osd_blend_image_set_ {
    /* obnd_mem_config1 */
    uintptr_t *data;
    /* obnd_mem_config2 */
    uint32_t stride_byte_size;
    uint32_t frame_width_byte;
    /* obnd_mem_config3 */
    uint8_t line_head_fix_bit;
    uint8_t line_tail_fix_bit;
    uint32_t frame_height;
} osd_blend_image_set_t;

typedef struct _osd_blend_color_keying_set_ {
    /* obnd_layer_config3 */
    uint8_t min_key_alpha;
    uint8_t max_key_alpha;
    uint8_t min_key_rv;
    uint8_t max_key_rv;
    /* obnd_layer_config4 */
    uint8_t min_key_gy;
    uint8_t max_key_gy;
    uint8_t min_key_bu;
    uint8_t max_key_bu;
    /* obnd_layer_config5 */
    uint8_t replace_alpha;
    uint8_t replace_rv;
    uint8_t replace_gy;
    uint8_t replace_bu;
    /* obnd_layer_config6 */
    OSB_BLEND_KEYING_MODE_e mode_alpha;
    OSB_BLEND_KEYING_MODE_e mode_rv;
    OSB_BLEND_KEYING_MODE_e mode_gy;
    OSB_BLEND_KEYING_MODE_e mode_bu;
} osd_blend_color_keying_set_t;

typedef struct _osd_blend_interrupt_status_ {
    uint8_t seof_int;
    uint8_t drain_err;
} osd_blend_interrupt_status_t;

/*@} end of group OSD_BLEND_Public_Types */

/** @defgroup  OSD_BLEND_Public_Functions
 *  @{
 */

uint32_t osd_blend_config_layer(osd_blend_reg_t *pblend,
                                   uint32_t xstart, uint32_t ystart,
                                   uint32_t xend, uint32_t yend,
                                   osd_blend_image_set_t *pset);

uint32_t osd_blend_mem_access_line(osd_blend_reg_t *pblend, uint32_t req_hlen);
uint32_t osd_blend_layer_early_commit(osd_blend_reg_t *pblend);
uint32_t osd_blend_ctrl_layer(osd_blend_reg_t *pblend, OSD_BLEND_ST_e enable);

uint32_t osd_blend_set_color_format(osd_blend_reg_t *pblend, OSD_BLEND_COLOR_FMT_e fmt,
                                       OSD_BLEND_COLOR_ORDER_e order_alpha,
                                       OSD_BLEND_COLOR_ORDER_e order_r_v,
                                       OSD_BLEND_COLOR_ORDER_e order_g_y,
                                       OSD_BLEND_COLOR_ORDER_e order_b_u);

uint32_t osd_blend_set_global_alpha(osd_blend_reg_t *pblend, uint32_t enable,
                                       uint8_t alpha);
uint32_t osd_blend_set_global_color(osd_blend_reg_t *pblend, uint32_t enable,
                                       uint8_t r_v, uint8_t g_y, uint8_t b_u);

uint32_t osd_blend_set_alpha_argb1555(osd_blend_reg_t *pblend, uint8_t alpha_0,
                                       uint8_t alpha_1);

uint32_t osd_blend_update_palette(osd_blend_reg_t *pblend, uint32_t color, uint8_t index);

uint32_t osd_blend_palette_keying_ctrl(osd_blend_reg_t *pblend, OSD_BLEND_ST_e enable);

uint32_t osd_blend_palette_keying(osd_blend_reg_t *pblend, OSD_BLEND_ST_e enable,
                                     OSB_BLEND_KEYING_MODE_e mode,
                                     uint8_t replace_index,
                                     uint8_t min_key_index, uint8_t max_key_index);

uint32_t osd_blend_palette_keying_update_range(osd_blend_reg_t *pblend,
                                                  uint8_t min_key_index, uint8_t max_key_index);

uint32_t osd_blend_color_keying(osd_blend_reg_t *pblend, OSD_BLEND_ST_e enable,
                                   osd_blend_color_keying_set_t *pset);

uint32_t osd_blend_enable_interrupt(osd_blend_reg_t *pblend, OSD_BLEND_ST_e enable, OSD_BLEND_INT_TYPE_e type);

uint32_t osd_blend_clear_interrupt(osd_blend_reg_t *pblend, OSD_BLEND_INT_TYPE_e type);

uint32_t osd_blend_set_interrupt_edge(osd_blend_reg_t *pblend, OSD_BLEND_INT_EDGE_e edge);

uint32_t osd_blend_get_interrupt(osd_blend_reg_t *pblend, osd_blend_interrupt_status_t *status);

/*@} end of group OSD_BLEND_Public_Functions */

/*@} end of group OSD_BLEND */

/*@} end of group Peripheral_Driver */

#endif /* __DRV_OSD_BLEND_H__ */

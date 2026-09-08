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

#include "drv_osd_blend.h"

/** @addtogroup  Peripheral_Driver
 *  @{
 */

/** @addtogroup  OSD_BLEND
 *  @{
 */

/** @defgroup  OSD_BLEND_Private_Macros
 *  @{
 */

/*@} end of group OSD_BLEND_Private_Macros */

/** @defgroup  OSD_BLEND_Private_Types
 *  @{
 */

/*@} end of group OSD_BLEND_Private_Types */

/** @defgroup  OSD_BLEND_Private_Variables
 *  @{
 */

/*@} end of group OSD_BLEND_Private_Variables */

/** @defgroup  OSD_BLEND_Global_Variables
 *  @{
 */

/*@} end of group OSD_BLEND_Global_Variables */

/** @defgroup  OSD_BLEND_Private_Fun_Declaration
 *  @{
 */

/*@} end of group OSD_BLEND_Private_Fun_Declaration */

/** @defgroup  OSD_BLEND_Private_Functions
 *  @{
 */

/*@} end of group OSD_BLEND_Private_Functions */

/** @defgroup  OSD_BLEND_Public_Functions
 *  @{
 */

/******************************************************************************
 * @brief  OSD blend layer configuration
 *
 * @param  pblend: pointer to a specific OSD blend layer
 * @param  xstart: OSD layer start point: x-axis, better to be even.
 * @param  ystart: OSD layer start point: y-axis, better to be even.
 * @param  xend: OSD layer end point: x-axis, better to be odd.
 * @param  yend: OSD layer end point: y-axis, better to be odd.
 * @param  data: OSD layer data buffer pointer.
 * @param  dat_len: data length in dword size(8bytes).
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_blend_config_layer(osd_blend_reg_t *pblend,
                                   uint32_t xstart, uint32_t ystart,
                                   uint32_t xend, uint32_t yend,
                                   osd_blend_image_set_t *pset)
{
    uint32_t val = 0;

    pblend->obnd_layer_xconfig.WORD = (xstart & OSD_BLEND_OBND_X_MIN_MSK) |
                                      ((xend << OSD_BLEND_OBND_X_MAX_POS) & OSD_BLEND_OBND_X_MAX_MSK);
    pblend->obnd_layer_yconfig.WORD = (ystart & OSD_BLEND_OBND_Y_MIN_MSK) |
                                      ((yend << OSD_BLEND_OBND_Y_MAX_POS) & OSD_BLEND_OBND_Y_MAX_MSK);

    pblend->obnd_mem_config1.WORD = (uint32_t)(uintptr_t)pset->data;


    val = ((pset->frame_width_byte >> 3) & OSD_BLEND_REG_FRAME_WIDTH_BYTE_X8_MSK);
    val |= (((pset->stride_byte_size >> 3) << OSD_BLEND_REG_STRIDE_BYTE_X8_POS) & OSD_BLEND_REG_STRIDE_BYTE_X8_MSK);

    pblend->obnd_mem_config2.WORD = val;

    val = (pset->frame_height & OSD_BLEND_REG_FRAME_HEIGHT_MSK);
    val |= ((pset->line_head_fix_bit << OSD_BLEND_REG_LINE_HEAD_FIX_BIT_POS ) & OSD_BLEND_REG_LINE_HEAD_FIX_BIT_MSK);
    val |= ((pset->line_tail_fix_bit << OSD_BLEND_REG_LINE_TAIL_FIX_BIT_POS ) & OSD_BLEND_REG_LINE_TAIL_FIX_BIT_MSK);

    pblend->obnd_mem_config3.WORD = val;

    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD blend layer mem access timing configuration
 *
 * @param  pblend: pointer to a specific OSD blend layer
 * @param  req_hlen: memory access timing setting in H-SYNC cycles after V-SYNC.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_blend_mem_access_line(osd_blend_reg_t *pblend, uint32_t req_hlen)
{
    pblend->obnd_sh.BF.obnd_layer_mem_req_cnt = req_hlen;

    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD blend layer force shadow control:
 *         call this API before SPP streaming on to ensure the OSD layer
 *         is activated even in frame 0.
 *         After calling this API, this OSD blending layer has to be enabled.
 *
 * @param  pblend: pointer to a specific OSD blend layer
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_blend_layer_early_commit(osd_blend_reg_t *pblend)
{
    uint32_t val = 0;

    val = pblend->obnd_mem_config0.WORD;
    /* set OSD blend force shadow bit, so that the settings will be loaded
     * immediately and mem access will be started after layer is enabled.
     */
    val |= OSD_BLEND_OBND_FORCE_SH_MSK;
    pblend->obnd_mem_config0.WORD = val;

    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD blend layer enable/disable control
 *
 * @param  pblend: pointer to a specific OSD blend layer
 * @param  enable: requested state of this blend layer.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_blend_ctrl_layer(osd_blend_reg_t *pblend, OSD_BLEND_ST_e enable)
{
    uint32_t val = 0;

    val = pblend->osd_misc.WORD;
    val &= OSD_BLEND_OBND_LAYER_EN_UMSK;
    val |= (OSD_BLEND_ST_ENABLE == enable) ? OSD_BLEND_OBND_LAYER_EN_MSK : 0;
    pblend->osd_misc.WORD = val;

    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD blend layer color format configuration
 *
 * @param  pblend: pointer to a specific OSD blend layer
 * @param  fmt: color format.
 * @param  order_alpha: order of the alpha.
 * @param  order_r_v: order of the red(RGB) or v(YUV).
 * @param  order_g_y: order of the green(RGB) or y(YUV).
 * @param  order_b_u: order of the blue(RGB) or u(YUV).
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_blend_set_color_format(osd_blend_reg_t *pblend, OSD_BLEND_COLOR_FMT_e fmt,
                                       OSD_BLEND_COLOR_ORDER_e order_alpha,
                                       OSD_BLEND_COLOR_ORDER_e order_r_v,
                                       OSD_BLEND_COLOR_ORDER_e order_g_y,
                                       OSD_BLEND_COLOR_ORDER_e order_b_u)
{
    uint32_t val = 0;

    val = pblend->obnd_layer_config0.WORD;
    val &= ~(OSD_BLEND_OBND_COLOR_FORMAT_MSK |
             OSD_BLEND_OBND_ORDER_A_MSK |
             OSD_BLEND_OBND_ORDER_RV_MSK |
             OSD_BLEND_OBND_ORDER_GY_MSK |
             OSD_BLEND_OBND_ORDER_BU_MSK);

    val |= fmt << OSD_BLEND_OBND_COLOR_FORMAT_POS;
    val |= order_alpha << OSD_BLEND_OBND_ORDER_A_POS;
    val |= order_r_v << OSD_BLEND_OBND_ORDER_RV_POS;
    val |= order_g_y << OSD_BLEND_OBND_ORDER_GY_POS;
    val |= order_b_u << OSD_BLEND_OBND_ORDER_BU_POS;

    pblend->obnd_layer_config0.WORD = val;

    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD blend layer global alpha configuration
 *
 * @param  pblend: pointer to a specific OSD blend layer
 * @param  enable: enable or disable the global alpha for this layer.
 * @param  alpha: value of the alpha.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_blend_set_global_alpha(osd_blend_reg_t *pblend, uint32_t enable,
                                       uint8_t alpha)
{
    uint32_t val = 0;

    val = pblend->obnd_layer_config0.WORD;
    val &= OSD_BLEND_OBND_GLOBAL_A_UMSK;
    val &= OSD_BLEND_OBND_GLOBAL_A_EN_UMSK;
    val |= (alpha << OSD_BLEND_OBND_GLOBAL_A_POS) & OSD_BLEND_OBND_GLOBAL_A_MSK;
    val |= (OSD_BLEND_ST_ENABLE == enable) ? OSD_BLEND_OBND_GLOBAL_A_EN_MSK : 0;
    pblend->obnd_layer_config0.WORD = val;
    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD blend layer global color configuration
 *
 * @param  pblend: pointer to a specific OSD blend layer
 * @param  enable: enable or disable the global color for this layer.
 * @param  r_v: value of the red(RGB) or v(YUV).
 * @param  g_y: value of the green(RGB) or y(YUV).
 * @param  b_u: value of the blue(RGB) or u(YUV).
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_blend_set_global_color(osd_blend_reg_t *pblend, uint32_t enable,
                                       uint8_t r_v, uint8_t g_y, uint8_t b_u)
{
    uint32_t val = 0;

    val = (OSD_BLEND_ST_ENABLE == enable) ? OSD_BLEND_OBND_GLOBAL_COLOR_EN_MSK : 0;
    val |= (r_v << OSD_BLEND_OBND_GLOBAL_RV_POS) & OSD_BLEND_OBND_GLOBAL_RV_MSK;
    val |= (g_y << OSD_BLEND_OBND_GLOBAL_GY_POS) & OSD_BLEND_OBND_GLOBAL_GY_MSK;
    val |= (b_u << OSD_BLEND_OBND_GLOBAL_BU_POS) & OSD_BLEND_OBND_GLOBAL_BU_MSK;
    pblend->obnd_layer_config1.WORD = val;

    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD blend layer alpha for ARGB1555 configuration
 *
 * @param  pblend: pointer to a specific OSD blend layer
 * @param  alpha_0: value of the alpha for A = 0.
 * @param  alpha_1: value of the alpha for A = 1.
 *
 * @return Result of the operation.
 *
*******************************************************************************/
uint32_t osd_blend_set_alpha_argb1555(osd_blend_reg_t *pblend, uint8_t alpha_0,
                                       uint8_t alpha_1)
{
    uint32_t val = 0;

    val = pblend->obnd_layer_config8.WORD;
    val &= OSD_BLEND_OBND_ALPHA_0_UMSK;
    val &= OSD_BLEND_OBND_ALPHA_1_UMSK;
    val |= (alpha_0 << OSD_BLEND_OBND_ALPHA_0_POS) & OSD_BLEND_OBND_ALPHA_0_MSK;
    val |= (alpha_1 << OSD_BLEND_OBND_ALPHA_1_POS) & OSD_BLEND_OBND_ALPHA_1_MSK;
    pblend->obnd_layer_config8.WORD = val;
    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD blend layer update palette value according to the index.
 *
 * @param  pblend: pointer to a specific OSD blend layer
 * @param  color: palette color, can be format ARGB8888 or AYUV8888.
 * @param  index: the palette index for updating.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_blend_update_palette(osd_blend_reg_t *pblend, uint32_t color, uint8_t index)
{
    /* palette should be updated when blend is disabled. */
    uint32_t *reg_addr = (uint32_t*)((uintptr_t)(pblend) + OSD_BLEND_PALE_SRAM_00_OFFSET + (index<<2));
    *reg_addr = color;

    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD blend layer palette keying control.
 *
 * @param  pblend: pointer to a specific OSD blend layer
 * @param  enable: enable or disable the palette keying for this layer.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_blend_palette_keying_ctrl(osd_blend_reg_t *pblend, OSD_BLEND_ST_e enable)
{
    uint32_t val = 0;

    val = pblend->obnd_layer_config2.WORD;
    val &= OSD_BLEND_OBND_KEY_PALETTE_EN_UMSK;

    val |= (OSD_BLEND_ST_ENABLE == enable) ? OSD_BLEND_OBND_KEY_PALETTE_EN_MSK : 0;

    pblend->obnd_layer_config2.WORD = val;
    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD blend layer palette keying configuration.
 *
 * @param  pblend: pointer to a specific OSD blend layer
 * @param  enable: enable or disable the palette keying for this layer.
 * @param  mode: palette keying mode: within or out of the min/max range.
 * @param  replace_index: the palette index that will be used as the replace color.
 * @param  min_key_index: min index for the palette keying.
 * @param  max_key_index: max index for the palette keying.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_blend_palette_keying(osd_blend_reg_t *pblend, OSD_BLEND_ST_e enable,
                                     OSB_BLEND_KEYING_MODE_e mode,
                                     uint8_t replace_index,
                                     uint8_t min_key_index, uint8_t max_key_index)
{
    uint32_t val = 0;

    /* disable palette keying */
    val = pblend->obnd_layer_config2.WORD;
    val &= OSD_BLEND_OBND_KEY_PALETTE_EN_UMSK;
    pblend->obnd_layer_config2.WORD = val;

    /* it is done if palette keying mode is disabled */
    if (OSD_BLEND_ST_ENABLE != enable) {
        return SUCCESS;
    }

    val |= (OSD_BLEND_ST_ENABLE == enable) ? OSD_BLEND_OBND_KEY_PALETTE_EN_MSK : 0;
    val |= (OSB_BLEND_KEYING_INVERT == mode) ? OSD_BLEND_OBND_KEY_INDEX_INV_MSK : 0;
    val |= (min_key_index << OSD_BLEND_OBND_KEY_INDEX_MIN_POS) & OSD_BLEND_OBND_KEY_INDEX_MIN_MSK;
    val |= (max_key_index << OSD_BLEND_OBND_KEY_INDEX_MAX_POS) & OSD_BLEND_OBND_KEY_INDEX_MAX_MSK;
    val |= (replace_index << OSD_BLEND_OBND_KEY_REPLACE_INDEX_POS) &
           OSD_BLEND_OBND_KEY_REPLACE_INDEX_MSK;

    pblend->obnd_layer_config2.WORD = val;

    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD blend layer palette keying min/max updating control.
 *
 * @param  pblend: pointer to a specific OSD blend layer
 * @param  min_key_index: min index for the palette keying.
 * @param  max_key_index: max index for the palette keying.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_blend_palette_keying_update_range(osd_blend_reg_t *pblend,
                                                  uint8_t min_key_index, uint8_t max_key_index)
{
    uint32_t val = 0;

    val = pblend->obnd_layer_config2.WORD;

    val &= OSD_BLEND_OBND_KEY_INDEX_MIN_UMSK;
    val &= OSD_BLEND_OBND_KEY_INDEX_MAX_UMSK;
    val |= (min_key_index << OSD_BLEND_OBND_KEY_INDEX_MIN_POS) & OSD_BLEND_OBND_KEY_INDEX_MIN_MSK;
    val |= (max_key_index << OSD_BLEND_OBND_KEY_INDEX_MAX_POS) & OSD_BLEND_OBND_KEY_INDEX_MAX_MSK;

    pblend->obnd_layer_config2.WORD = val;

    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD blend layer color keying configuration.
 *
 * @param  pblend: pointer to a specific OSD blend layer
 * @param  enable: enable or disable the color keying for this layer.
 * @param  pset: point to the color keying setting.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_blend_color_keying(osd_blend_reg_t *pblend, OSD_BLEND_ST_e enable,
                                   osd_blend_color_keying_set_t *pset)
{
    uint32_t val = 0;

    CHECK_PARAM(NULL != pset);

    /* disable color keying */
    val = pblend->obnd_layer_config6.WORD;
    val &= OSD_BLEND_OBND_KEY_COLOR_EN_UMSK;
    pblend->obnd_layer_config6.WORD = val;

    /* it is done if color keying mode is disabled */
    if (OSD_BLEND_ST_ENABLE != enable) {
        return SUCCESS;
    }

    /* layer_config3: min & max for alpha and rv */
    val = pset->min_key_alpha << OSD_BLEND_OBND_KEY_A_MIN_POS;
    val |= pset->max_key_alpha << OSD_BLEND_OBND_KEY_A_MAX_POS;
    val |= pset->min_key_rv << OSD_BLEND_OBND_KEY_RV_MIN_POS;
    val |= pset->max_key_rv << OSD_BLEND_OBND_KEY_RV_MAX_POS;
    pblend->obnd_layer_config3.WORD = val;

    /* layer_config4: min & max for gy and bu */
    val = pset->min_key_gy << OSD_BLEND_OBND_KEY_GY_MIN_POS;
    val |= pset->max_key_gy << OSD_BLEND_OBND_KEY_GY_MAX_POS;
    val |= pset->min_key_bu << OSD_BLEND_OBND_KEY_BU_MIN_POS;
    val |= pset->max_key_bu << OSD_BLEND_OBND_KEY_BU_MAX_POS;
    pblend->obnd_layer_config4.WORD = val;

    /* layer_config5: replace color */
    val = pset->replace_alpha << OSD_BLEND_OBND_KEY_REPLACE_A_POS;
    val |= pset->replace_rv << OSD_BLEND_OBND_KEY_REPLACE_RV_POS;
    val |= pset->replace_gy << OSD_BLEND_OBND_KEY_REPLACE_GY_POS;
    val |= pset->replace_bu << OSD_BLEND_OBND_KEY_REPLACE_BU_POS;
    pblend->obnd_layer_config5.WORD = val;

    /* color range inverted? */
    val = pblend->obnd_layer_config6.WORD;
    val &= ~(OSD_BLEND_OBND_KEY_A_INV_MSK |
             OSD_BLEND_OBND_KEY_RV_INV_MSK |
             OSD_BLEND_OBND_KEY_GY_INV_MSK |
             OSD_BLEND_OBND_KEY_BU_INV_MSK);
    val |= (OSB_BLEND_KEYING_INVERT == pset->mode_alpha) ?
               OSD_BLEND_OBND_KEY_A_INV_MSK :
               0;
    val |= (OSB_BLEND_KEYING_INVERT == pset->mode_rv) ?
               OSD_BLEND_OBND_KEY_RV_INV_MSK :
               0;
    val |= (OSB_BLEND_KEYING_INVERT == pset->mode_gy) ?
               OSD_BLEND_OBND_KEY_GY_INV_MSK :
               0;
    val |= (OSB_BLEND_KEYING_INVERT == pset->mode_bu) ?
               OSD_BLEND_OBND_KEY_BU_INV_MSK :
               0;

    /* enable color keying */
    val |= OSD_BLEND_OBND_KEY_COLOR_EN_MSK;
    pblend->obnd_layer_config6.WORD = val;

    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD blend interrupt enable
 *
 * @param  pblend: pointer to a specific OSD blend layer
 * @param  enable: enable or disable osd blend interrupt.
 * @param  type: osd blend interrupt type
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_blend_enable_interrupt(osd_blend_reg_t *pblend, OSD_BLEND_ST_e enable, OSD_BLEND_INT_TYPE_e type)
{
    uint32_t val = 0;

    if (OSD_BLEND_INT_TYPE_SEOF == type)
    {
        val = pblend->osd_misc.WORD;
        val &= OSD_BLEND_REG_OSD_SEOF_INT_MASK_W_UMSK;
        val |= (OSD_BLEND_ST_ENABLE == enable) ? 0 : OSD_BLEND_REG_OSD_SEOF_INT_MASK_W_MSK;
        pblend->osd_misc.WORD = val;
    }
    else
    {
        val = pblend->obnd_error.WORD;
        val &= OSD_BLEND_REG_RFIFO_DRAIN_MASK_W_UMSK;
        val |= (OSD_BLEND_ST_ENABLE == enable) ? 0 : OSD_BLEND_REG_RFIFO_DRAIN_MASK_W_MSK;
        pblend->obnd_error.WORD = val;
    }

    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD blend clear interrupt status
 *
 * @param  pblend: pointer to a specific OSD blend layer
 * @param  type: osd blend interrupt type
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_blend_clear_interrupt(osd_blend_reg_t *pblend, OSD_BLEND_INT_TYPE_e type)
{
    uint32_t val = 0;

    if (OSD_BLEND_INT_TYPE_SEOF == type)
    {
        val = pblend->obnd_mem_config0.WORD;
        val |= OSD_BLEND_REG_OSD_SEOF_INT_CLR_W_MSK;
        pblend->obnd_mem_config0.WORD = val;
    }
    else
    {
        val = pblend->obnd_error.WORD;
        val |= OSD_BLEND_REG_RFIFO_DRAIN_CLR_W_MSK;
        pblend->obnd_error.WORD = val;
    }

    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD blend interrupt edge configuration
 *
 * @param  pblend: pointer to a specific OSD blend layer
 * @param  edge: rising or falling edge to trigger interrupt
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_blend_set_interrupt_edge(osd_blend_reg_t *pblend, OSD_BLEND_INT_EDGE_e edge)
{
    uint32_t val = 0;

    val = pblend->osd_misc.WORD;
    val &= OSD_BLEND_REG_OSD_SEOF_INT_EDGE_W_UMSK;
    val |= (OSD_BLEND_INT_EDGE_FALLING == edge) ? 0 : OSD_BLEND_REG_OSD_SEOF_INT_EDGE_W_MSK;
    pblend->osd_misc.WORD = val;

    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD blend get interrupt status
 *
 * @param  status: pointer to a OSD blend status
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_blend_get_interrupt(osd_blend_reg_t *pblend, osd_blend_interrupt_status_t *status)
{
    uint32_t val = 0;

    val = pblend->obnd_mem_config0.WORD;
    status->seof_int = (val & OSD_BLEND_STS_OSD_SEOF_INT_R_MSK) >> OSD_BLEND_STS_OSD_SEOF_INT_R_POS;

    val = pblend->obnd_error.WORD;
    status->drain_err = (val & OSD_BLEND_STS_RFIFO_DRAIN_R_MSK) >> OSD_BLEND_STS_RFIFO_DRAIN_R_POS;

    return SUCCESS;
}

/*@} end of group OSD_BLEND_Public_Functions */

/*@} end of group OSD_BLEND */

/*@} end of group Peripheral_Driver */

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

#include "drv_osd_draw.h"

/** @addtogroup  Peripheral_Driver
 *  @{
 */

/** @addtogroup  OSD_DRAW
 *  @{
 */

/** @defgroup  OSD_DRAW_Private_Macros
 *  @{
 */

/*@} end of group OSD_DRAW_Private_Macros */

/** @defgroup  OSD_DRAW_Private_Types
 *  @{
 */

/*@} end of group OSD_DRAW_Private_Types */

/** @defgroup  OSD_DRAW_Private_Variables
 *  @{
 */

/*@} end of group OSD_DRAW_Private_Variables */

/** @defgroup  OSD_DRAW_Global_Variables
 *  @{
 */

/*@} end of group OSD_DRAW_Global_Variables */

/** @defgroup  OSD_DRAW_Private_Fun_Declaration
 *  @{
 */

/*@} end of group OSD_DRAW_Private_Fun_Declaration */

/** @defgroup  OSD_DRAW_Private_Functions
 *  @{
 */

/*@} end of group OSD_DRAW_Private_Functions */

/** @defgroup  OSD_DRAW_Public_Functions
 *  @{
 */

/******************************************************************************
 * @brief  OSD draw layer state control
 *
 * @param  odraw: pointer to a specific OSD draw HW IP.
 * @param  layer: draw layer.
 * @param  state: enable or disable the layer in this draw HW IP.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_draw_set_layer_state(osd_draw_reg_t *odraw, uint32_t layer, OSD_RECT_STATE_Type state)
{
    uint32_t tmpval = 0;
    uint32_t layer_mask = 1 << OSD_VALID_LAYER(layer);

    tmpval = odraw->sd_ctrl_000.WORD;

    if (state == OSD_RECT_STATE_ENABLE) {
        tmpval |= layer_mask;
    } else {
        tmpval &= ~layer_mask;
    }

    odraw->sd_ctrl_000.WORD = tmpval;
    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD draw layer state get
 *
 * @param  odraw: pointer to a specific OSD draw HW IP.
 * @param  layer: draw layer.
 * @param  state: pointer to return the state value.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_draw_get_layer_state(osd_draw_reg_t *odraw, uint32_t layer, OSD_RECT_STATE_Type *state)
{
    uint32_t tmpval = 0;
    uint32_t layer_mask = 1 << OSD_VALID_LAYER(layer);

    CHECK_PARAM(NULL != odraw);
    CHECK_PARAM(NULL != state);

    tmpval = odraw->sd_ctrl_000.WORD;
    tmpval &= layer_mask;
    *state = (!!tmpval) ? OSD_RECT_STATE_ENABLE : OSD_RECT_STATE_DISABLE;

    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD draw layer style set
 *
 * @param  odraw: pointer to a specific OSD draw HW IP.
 * @param  layer: draw layer.
 * @param  style: solid or hollow style.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_draw_set_layer_style(osd_draw_reg_t *odraw, uint32_t layer, OSD_RECT_STYLE_Type style)
{
    uint32_t tmpval = 0;
    uint32_t layer_mask = 1 << OSD_VALID_LAYER(layer);

    if (style == OSD_RECT_STYLE_SOLID) {
        tmpval |= layer_mask << OSD_DRAW_REG_DRAW_TYPE_L_POS;
    } else {
        tmpval &= ~(layer_mask << OSD_DRAW_REG_DRAW_TYPE_L_POS);
    }

    odraw->sd_ctrl_000.WORD = tmpval;
    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD draw layer style get
 *
 * @param  odraw: pointer to a specific OSD draw HW IP.
 * @param  layer: draw layer.
 * @param  style: pointer to return the solid or hollow style.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_draw_get_layer_style(osd_draw_reg_t *odraw, uint32_t layer, OSD_RECT_STYLE_Type *style)
{
    uint32_t tmpval = 0;
    uint32_t layer_mask = 1 << OSD_VALID_LAYER(layer);

    CHECK_PARAM(NULL != odraw);
    CHECK_PARAM(NULL != style);

    tmpval = odraw->sd_ctrl_000.WORD;
    tmpval &= layer_mask << OSD_DRAW_REG_DRAW_TYPE_L_POS;
    *style = (!!tmpval) ? OSD_RECT_STYLE_SOLID : OSD_RECT_STYLE_HOLLOW;

    return SUCCESS;
}

/******************************************************************************
 * @brief  OSD draw layer control
 *
 * @param  odraw: pointer to a specific OSD draw HW IP.
 * @param  layer: draw layer.
 * @param  state: enable or disable this draw layer.
 * @param  style: solid or hollow style for this layer.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_draw_config_rect_layer(osd_draw_reg_t *odraw, uint32_t layer, OSD_RECT_STATE_Type state, OSD_RECT_STYLE_Type style)
{
    uint32_t tmpval = 0;
    uint32_t layer_mask = 1 << OSD_VALID_LAYER(layer);

    tmpval = odraw->sd_ctrl_000.WORD;

    if (state == OSD_RECT_STATE_ENABLE) {
        tmpval |= layer_mask;
    } else {
        tmpval &= ~layer_mask;
    }

    if (style == OSD_RECT_STYLE_SOLID) {
        tmpval |= layer_mask << OSD_DRAW_REG_DRAW_TYPE_L_POS;
    } else {
        tmpval &= ~(layer_mask << OSD_DRAW_REG_DRAW_TYPE_L_POS);
    }

    odraw->sd_ctrl_000.WORD = tmpval;
    return SUCCESS;
}

/******************************************************************************
 * @brief  Draw a specific rectangle with OSD draw HW IP.
 *
 * @param  odraw: pointer to a specific OSD draw HW IP.
 * @param  rect: pointer to a rectangle setting structure.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_draw_rectangle(osd_draw_reg_t *odraw, osd_rectangle_t *rect)
{
    osd_rectangle_desc_t *desc = OSD_RECT_DESC((odraw), OSD_VALID_LAYER(rect->layer));

    CHECK_PARAM(NULL != odraw);
    CHECK_PARAM(NULL != rect);

    osd_draw_set_layer_state(odraw, rect->layer, OSD_RECT_STATE_DISABLE);
    desc->attr = rect->desc.attr;

    /* EVEN and ODD */
    rect->desc.pos.x.start &= -2;
    rect->desc.pos.x.end |= 1;
    rect->desc.pos.y.start &= -2;
    rect->desc.pos.y.end |= 1;
    rect->desc.attr.yuvt.thinkness += (rect->desc.attr.yuvt.thinkness & 1) ? 1 : 0;

    desc->pos.x = rect->desc.pos.x;
    desc->pos.y = rect->desc.pos.y;
    osd_draw_config_rect_layer(odraw, rect->layer, OSD_RECT_STATE_ENABLE, rect->style);

    return SUCCESS;
}

/******************************************************************************
 * @brief  Draw a specific rectangle with OSD draw HW IP on the top if possible.
 *
 * @param  odraw: pointer to a specific OSD draw HW IP.
 * @param  rect: pointer to a rectangle setting structure.
 * @param  layer: pointer to return the current drawing layer.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_draw_rectangle_onTop(osd_draw_reg_t *odraw, osd_rectangle_t *rect, uint32_t *layer)
{
    uint32_t tmpval = 0;
    uint32_t draw_layer = 0;

    CHECK_PARAM(NULL != layer);
    CHECK_PARAM(NULL != rect);

    tmpval = odraw->sd_ctrl_000.WORD;
    tmpval &= 0x0000FFFF;

// #ifdef ARCH_RISCV
    tmpval = __builtin_clz(tmpval);
// #else
//     tmpval = __CLZ(tmpval);
// #endif
    if (OSD_MAX_LAYER == tmpval) {
        /* no available TOP layer since all layers used */
        *layer = (uint32_t)-1;
        return NORESC;
    }

    draw_layer = sizeof(uintptr_t) * 8 - tmpval;

    rect->layer = draw_layer;
    *layer = draw_layer;
    osd_draw_rectangle(odraw, rect);

    return SUCCESS;
}

/******************************************************************************
 * @brief  Init the OSD draw HW IP.
 *
 * @param  odraw: pointer to a specific OSD draw HW IP.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_draw_init(osd_draw_reg_t *odraw)
{
    odraw->sd_ctrl_000.WORD = 0;
    return SUCCESS;
}

/******************************************************************************
 * @brief  DeInit the OSD draw HW IP.
 *
 * @param  odraw: pointer to a specific OSD draw HW IP.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

uint32_t osd_draw_deinit(osd_draw_reg_t *odraw)
{
    odraw->sd_ctrl_000.WORD = 0;
    return SUCCESS;
}

/*@} end of group OSD_DRAW_Public_Functions */

/*@} end of group OSD_DRAW */

/*@} end of group Peripheral_Driver */

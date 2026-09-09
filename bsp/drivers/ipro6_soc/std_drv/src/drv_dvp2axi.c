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

#include <drv_dvp2axi.h>
#include <dvp2axi_reg.h>

#define IS_MULTIPULE_OF_8(x)    (((x) & 0x7) == 0)

enum {
    DVP2AXI_BURST_TYPE_SINGLE,              /*!< AXI burst type:single */
    DVP2AXI_BURST_TYPE_INCR4,               /*!< AXI burst type:incrementing 4 */
    DVP2AXI_BURST_TYPE_INCR8,               /*!< AXI burst type:incrementing 8 */
    DVP2AXI_BURST_TYPE_INCR16,              /*!< AXI burst type:incrementing 16 */
    DVP2AXI_BURST_TYPE_INCR32 = 5,          /*!< AXI burst type:incrementing 32 */
    DVP2AXI_BURST_TYPE_INCR64,              /*!< AXI burst type:incrementing 64 */
};

static const uint32_t DVP2AXI_BASE_ADDR_SET[] =
    {DVP2AXI0_BASE, DVP2AXI1_BASE};

void DVP2AXI_Init(DVP2AXI_PORT_E port, DVP2AXI_INIT_CFG_S* init)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;
    uint32_t bytesPerLine;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    tmpVal = readl(DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_FRAM_VLD_POL, init->framePol);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_LINE_VLD_POL, init->linePol);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_XLEN, DVP2AXI_BURST_TYPE_INCR64);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_DVP_MODE, init->sensorMode);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_DVP_DATA_MODE, init->bitWidth);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_V_SUBSAMPLE_EN, init->subSampleEn);
    tmpVal = reg_clr_bit(tmpVal, DVP2AXI_REG_V_SUBSAMPLE_POL); // use default now, it may be parameters from argument
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_WRAP_MODE, init->wrapEn);
    // if (init->wrapEn)  // ring mode, and keep write address
    //     tmpVal = reg_clr_bit(tmpVal, DVP2AXI_REG_WRAP_ADDR_START);
    // else // frame mode, and always start from base address
        tmpVal = reg_set_bit(tmpVal, DVP2AXI_REG_WRAP_ADDR_START);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_DVP_DATA_BSEL, init->byte_sel);
    writel(tmpVal, DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);

    tmpVal = readl(DVP2AXIx + DVP2AXI_STATUS_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_FRAME_CNT_TRGR_INT, init->swIntCnt);    /* generate interrupt every frame */
    writel(tmpVal, DVP2AXIx + DVP2AXI_STATUS_OFFSET);

    writel(init->memStart, DVP2AXIx + DVP2AXI_ADDR_START_OFFSET);

    tmpVal = readl(DVP2AXIx + DVP2AXI_WRAP_MEM_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_WRAP_LCNT, init->wrapLine);
    writel(tmpVal, DVP2AXIx + DVP2AXI_WRAP_MEM_OFFSET);

    switch(init->bitWidth)
    {
        case DVP2AXI_DATA_8_BIT:
            bytesPerLine = init->imgWidth;
        break;
        case DVP2AXI_DATA_12_BIT:
            bytesPerLine = ((init->imgWidth * 3) >> 1);
        break;
        case DVP2AXI_DATA_16_BIT:
        case DVP2AXI_DATA_24_COMP_16BIT:
            bytesPerLine = (init->imgWidth << 1);
        break;
        case DVP2AXI_DATA_24_BIT:
            bytesPerLine = init->imgWidth * 3;
        break;
        case DVP2AXI_DATA_24_EXP_32BIT:
            bytesPerLine = (init->imgWidth << 2);
        break;
        default:
            bytesPerLine = (init->imgWidth << 1);
        break;
    }

    tmpVal = readl(DVP2AXIx + DVP2AXI_FRAME_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_FRAME_WIDTH, bytesPerLine);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_FRAME_HEIGHT, init->imgHeight);
    writel(tmpVal, DVP2AXIx + DVP2AXI_FRAME_OFFSET);

    if (IS_MULTIPULE_OF_8(bytesPerLine))
    {
        //do nothing
    }
    else
    {
        // set stride to meet 8 alignment
        DVP2AXI_SetStrideSize(port, (bytesPerLine % 8));
    }

    // set warning and urgent
    tmpVal = readl(DVP2AXIx + DVP2AXI_MISC_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_FIFO_THRE_URGENT, 1); // 1/2 FIFO
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_FIFO_THRE_WARNING, 0); // 1/4 FIFO
    writel(tmpVal, DVP2AXIx + DVP2AXI_MISC_OFFSET);
}

void DVP2AXI_Config(DVP2AXI_PORT_E port, DVP2AXI_CFG_S *cfg)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    tmpVal = readl(DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
    if (DVP2AXI_DATA_8_BIT == cfg->bit_width) {
        if (DVP2AXI_SEL_HIGH_BYTE == cfg->byte_sel) {
            tmpVal = reg_set_bit(tmpVal, DVP2AXI_REG_DVP_DATA_BSEL);
        } else {
            tmpVal = reg_clr_bit(tmpVal, DVP2AXI_REG_DVP_DATA_BSEL);
        }
    }

    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_DVP_DATA_MODE, cfg->bit_width);

    if (ENABLE == cfg->v_subsampl_enable) {
        tmpVal = reg_set_bit(tmpVal, DVP2AXI_REG_V_SUBSAMPLE_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, DVP2AXI_REG_V_SUBSAMPLE_EN);
    }

    writel(tmpVal, DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
}

void DVP2AXI_SetFrameValid(DVP2AXI_PORT_E port, DVP2AXI_DATA_POLARITY_E frm_vld)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    tmpVal = readl(DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_FRAM_VLD_POL, frm_vld);
    writel(tmpVal, DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
}

void DVP2AXI_SetFrameSize(DVP2AXI_PORT_E port, uint32_t bytesPerLine, uint32_t height)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    tmpVal = readl(DVP2AXIx + DVP2AXI_FRAME_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_FRAME_WIDTH, bytesPerLine);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_FRAME_HEIGHT, height);
    writel(tmpVal, DVP2AXIx + DVP2AXI_FRAME_OFFSET);

    if (IS_MULTIPULE_OF_8(bytesPerLine))
    {
        //do nothing
    }
    else
    {
        // set stride to meet 8 alignment
        DVP2AXI_SetStrideSize(port, (bytesPerLine % 8));
    }
}

void DVP2AXI_MemConfig(DVP2AXI_PORT_E port, uint32_t bufAddr, uint32_t linesForRing)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    if (0 != bufAddr)
        writel(bufAddr, DVP2AXIx + DVP2AXI_ADDR_START_OFFSET);

    tmpVal = readl(DVP2AXIx + DVP2AXI_WRAP_MEM_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_WRAP_LCNT, linesForRing);
    writel(tmpVal, DVP2AXIx + DVP2AXI_WRAP_MEM_OFFSET);
}

void DVP2AXI_Enable(DVP2AXI_PORT_E port)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    tmpVal = readl(DVP2AXIx + DVP2AXI_DVP_W1P_OFFSET);
    tmpVal = reg_set_bit(tmpVal, DVP2AXI_CR_SW_SHD);
    writel(tmpVal, DVP2AXIx + DVP2AXI_DVP_W1P_OFFSET);

    tmpVal = readl(DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, DVP2AXI_REG_DVP_ENABLE);
    writel(tmpVal, DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
}

void DVP2AXI_Disable(DVP2AXI_PORT_E port)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
	uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    tmpVal = readl(DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, DVP2AXI_REG_DVP_ENABLE);
    writel(tmpVal, DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
}

uint32_t DVP2AXI_GetBusyStatus(DVP2AXI_PORT_E port)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    tmpVal = readl(DVP2AXIx + DVP2AXI_STATUS_OFFSET);

    return (reg_is_bit_set(tmpVal, DVP2AXI_ST_IDLE)) ? RESET : SET;
}

void DVP2AXI_SetFrameActive(DVP2AXI_PORT_E port, DVP2AXI_DATA_POLARITY_E frame_active)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    tmpVal = readl(DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
    if (DVP2AXI_DATA_ACTIVE_HIGH == frame_active) {
        tmpVal = reg_set_bit(tmpVal, DVP2AXI_REG_FRAM_VLD_POL);
    } else if (DVP2AXI_DATA_ACTIVE_LOW == frame_active) {
        tmpVal = reg_clr_bit(tmpVal, DVP2AXI_REG_FRAM_VLD_POL);
    }
    writel(tmpVal, DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
}

void DVP2AXI_SetLineActive(DVP2AXI_PORT_E port, DVP2AXI_DATA_POLARITY_E line_active)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    tmpVal = readl(DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
    if (DVP2AXI_DATA_ACTIVE_HIGH == line_active) {
        tmpVal = reg_set_bit(tmpVal, DVP2AXI_REG_LINE_VLD_POL);
    } else if (DVP2AXI_DATA_ACTIVE_LOW == line_active) {
        tmpVal = reg_clr_bit(tmpVal, DVP2AXI_REG_LINE_VLD_POL);
    }
    writel(tmpVal, DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
}

void DVP2AXI_SetBitWidth(DVP2AXI_PORT_E port, DVP2AXI_DATA_MODE_E bitwidth)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    tmpVal = readl(DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_DVP_DATA_MODE, bitwidth);
    writel(tmpVal, DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
}

void DVP2AXI_GetFrameBuffer(DVP2AXI_PORT_E port, uint32_t *frameAddr, uint32_t *byteCnt)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t bytesCntLine, lineCnt, strideCntLine;
    uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    *frameAddr = readl(DVP2AXIx + DVP2AXI_ADDR_START_OFFSET);

    tmpVal = readl(DVP2AXIx + DVP2AXI_WRAP_MEM_OFFSET);
    strideCntLine = reg_get_bits(tmpVal, DVP2AXI_REG_STRIDE_WIDTH);

    tmpVal = readl(DVP2AXIx + DVP2AXI_FRAME_OFFSET);
    bytesCntLine = reg_get_bits(tmpVal, DVP2AXI_REG_FRAME_WIDTH);
    lineCnt = reg_get_bits(tmpVal, DVP2AXI_REG_FRAME_HEIGHT);
    *byteCnt = (bytesCntLine + strideCntLine) * lineCnt;
}

void DVP2AXI_SetAddress(DVP2AXI_PORT_E port, uint32_t bufAddr)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    if (0 != bufAddr)
        writel(bufAddr, DVP2AXIx + DVP2AXI_ADDR_START_OFFSET);
}

void DVP2AXI_Set_Frame_Skip(DVP2AXI_PORT_E port, uint32_t bitwiseFrmVld, uint32_t frmPeriodCnt)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];

    writel(bitwiseFrmVld, DVP2AXIx + DVP2AXI_FRAME_VLD_OFFSET);
    writel(frmPeriodCnt & 0x1F, DVP2AXIx + DVP2AXI_FRAME_PERIOD_OFFSET);
}

void DVP2AXI_Crop(DVP2AXI_PORT_E port, uint32_t h_start, int32_t h_end, uint32_t v_start, int32_t v_end)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    tmpVal = readl(DVP2AXIx + DVP2AXI_HSYNC_CROP_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_HSYNC_ACT_START, h_start);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_HSYNC_ACT_END, h_end);
    writel(tmpVal, DVP2AXIx + DVP2AXI_HSYNC_CROP_OFFSET);

    tmpVal = readl(DVP2AXIx + DVP2AXI_VSYNC_CROP_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_VSYNC_ACT_START, v_start);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_VSYNC_ACT_END, v_end);
    writel(tmpVal, DVP2AXIx + DVP2AXI_VSYNC_CROP_OFFSET);
}

void DVP2AXI_Valid_Cnt_Exm(DVP2AXI_PORT_E port, uint32_t hcnt, uint32_t vcnt)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    tmpVal = readl(DVP2AXIx + DVP2AXI_FRAM_EXM_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_TOTAL_HCNT, hcnt);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_TOTAL_VCNT, vcnt);
    writel(tmpVal, DVP2AXIx + DVP2AXI_FRAM_EXM_OFFSET);
}

void DVP2AXI_WrapModeEnable(DVP2AXI_PORT_E port)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    tmpVal = readl(DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, DVP2AXI_REG_WRAP_MODE);
    writel(tmpVal, DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
}

void DVP2AXI_WrapModeDisable(DVP2AXI_PORT_E port)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    tmpVal = readl(DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, DVP2AXI_REG_WRAP_MODE);
    writel(tmpVal, DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
}

void DVP2AXI_SetStrideSize(DVP2AXI_PORT_E port, uint32_t bytesPerLine)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    tmpVal = readl(DVP2AXIx + DVP2AXI_WRAP_MEM_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_STRIDE_WIDTH, bytesPerLine);
    writel(tmpVal, DVP2AXIx + DVP2AXI_WRAP_MEM_OFFSET);
}

void DVP2AXI_FlipModeEnable(DVP2AXI_PORT_E port)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    tmpVal = readl(DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, DVP2AXI_REG_FLIP_MODE);
    writel(tmpVal, DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
}

void DVP2AXI_FlipModeDisable(DVP2AXI_PORT_E port)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));

    tmpVal = readl(DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, DVP2AXI_REG_FLIP_MODE);
    writel(tmpVal, DVP2AXIx + DVP2AXI_CONFIGUE_OFFSET);
}

void DVP2AXI_16_Bit_RGB_order(DVP2AXI_PORT_E port, DVP2AXI_16_BIT_RGB_E rgbType)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_DVP2AXI_ID_TYPE(port));
    CHECK_PARAM(IS_DVP2AXI_16_BIT_RGB_TYPE(rgbType));

    tmpVal = readl(DVP2AXIx + DVP2AXI_MISC_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP2AXI_REG_FORMAT_565, rgbType);
    writel(tmpVal, DVP2AXIx + DVP2AXI_MISC_OFFSET);
}

void DVP2AXI_IntMask(DVP2AXI_PORT_E port, DVP2AXI_INT_Type intType, int intMask)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    tmpVal = readl(DVP2AXIx + DVP2AXI_STATUS_OFFSET);

    switch (intType) {
        case DVP2AXI_INT_NORMAL:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = reg_set_bit(tmpVal, DVP2AXI_REG_INT_NORMAL_EN);
            } else {
                /* Disable this interrupt */
                tmpVal = reg_clr_bit(tmpVal, DVP2AXI_REG_INT_NORMAL_EN);
            }
            break;

        case DVP2AXI_INT_FIFO_OVERWRITE:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = reg_set_bit(tmpVal, DVP2AXI_REG_INT_FIFO_EN);
            } else {
                /* Disable this interrupt */
                tmpVal = reg_clr_bit(tmpVal, DVP2AXI_REG_INT_FIFO_EN);
            }
            break;

        case DVP2AXI_INT_VSYNC_CNT_ERROR:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = reg_set_bit(tmpVal, DVP2AXI_REG_INT_VCNT_EN);
            } else {
                /* Disable this interrupt */
                tmpVal = reg_clr_bit(tmpVal, DVP2AXI_REG_INT_VCNT_EN);
            }
            break;

        case DVP2AXI_INT_HSYNC_CNT_ERROR:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = reg_set_bit(tmpVal, DVP2AXI_REG_INT_HCNT_EN);
            } else {
                /* Disable this interrupt */
                tmpVal = reg_clr_bit(tmpVal, DVP2AXI_REG_INT_HCNT_EN);
            }
            break;

        case DVP2AXI_INT_ALL:
            if (intMask == UNMASK) {
                /* Enable all interrupt */
                tmpVal |= 0x9C0;
            } else {
                /* Disable all interrupt */
                tmpVal &= 0xFFFFF63F;
            }
            break;

        default:
            break;
    }

    writel(tmpVal, DVP2AXIx + DVP2AXI_STATUS_OFFSET);
}

void DVP2AXI_IntClr(DVP2AXI_PORT_E port, DVP2AXI_INT_Type intType)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal;

    tmpVal = readl(DVP2AXIx + DVP2AXI_DVP_W1P_OFFSET);

    switch (intType) {
        case DVP2AXI_INT_NORMAL:
            tmpVal = reg_set_bit(tmpVal, DVP2AXI_REG_INT_NORMAL_CLR);
            break;

        case DVP2AXI_INT_FIFO_OVERWRITE:
            tmpVal = reg_set_bit(tmpVal, DVP2AXI_REG_INT_FIFO_CLR);
            break;

        case DVP2AXI_INT_VSYNC_CNT_ERROR:
            tmpVal = reg_set_bit(tmpVal, DVP2AXI_REG_INT_VCNT_CLR);
            break;

        case DVP2AXI_INT_HSYNC_CNT_ERROR:
            tmpVal = reg_set_bit(tmpVal, DVP2AXI_REG_INT_HCNT_CLR);
            break;

        case DVP2AXI_INT_ALL:
            tmpVal = 0xFFFF0;

        default:
            break;
    }

    writel(tmpVal, DVP2AXIx + DVP2AXI_DVP_W1P_OFFSET);
}

uint32_t DVP2AXI_IntStatus(DVP2AXI_PORT_E port, DVP2AXI_INT_Type intType)
{
    uint32_t DVP2AXIx = DVP2AXI_BASE_ADDR_SET[port];
    uint32_t tmpVal = 0;

    tmpVal = readl(DVP2AXIx + DVP2AXI_STATUS_OFFSET);

    switch (intType) {
        case DVP2AXI_INT_NORMAL:
            tmpVal = reg_get_bits(tmpVal, DVP2AXI_STS_NORMAL_INT);
            break;

        case DVP2AXI_INT_FIFO_OVERWRITE:
            tmpVal = reg_get_bits(tmpVal, DVP2AXI_STS_FIFO_INT);
            break;

        case DVP2AXI_INT_VSYNC_CNT_ERROR:
            tmpVal = reg_get_bits(tmpVal, DVP2AXI_STS_VCNT_INT);
            break;

        case DVP2AXI_INT_HSYNC_CNT_ERROR:
            tmpVal = reg_get_bits(tmpVal, DVP2AXI_STS_HCNT_INT);
            break;

        default:
            break;
    }
    return tmpVal;
}

uint16_t DVP2AXI_GetLineOffset(uint32_t bytesPerLine)
{
    if (IS_MULTIPULE_OF_8(bytesPerLine))
        return bytesPerLine;
    else
        return (bytesPerLine + (bytesPerLine %8));
}

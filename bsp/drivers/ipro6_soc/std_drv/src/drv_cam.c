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



#include "ipro6.h"
#include "drv_cam.h"
#include "drv_glb.h"

static const uint32_t camAddr[CAM_ID_MAX] = { DVP2AXI0_BASE, DVP2AXI1_BASE };
#ifndef IPRO_USE_HAL_DRIVER
static intCallback_Type *camIntCbfArra[CAM_ID_MAX][CAM_INT_ALL] = { { NULL } };
#endif

#ifndef IPRO_USE_HAL_DRIVER
static void CAM_IntHandler(CAM_ID_Type camId);
#endif

#ifndef IPRO_USE_HAL_DRIVER
static void CAM_IntHandler(CAM_ID_Type camId)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    tmpVal = readl(CAMx + CAM_DVP_STATUS_AND_ERROR_OFFSET);

    if (reg_is_bit_set(tmpVal, CAM_STS_NORMAL_INT)) {
        CAM_IntClr(camId, CAM_INT_NORMAL);

        if (camIntCbfArra[camId][CAM_INT_NORMAL] != NULL) {
            /* call the callback function */
            camIntCbfArra[camId][CAM_INT_NORMAL]();
        }
    }

    if (reg_is_bit_set(tmpVal, CAM_STS_MEM_INT)) {
        CAM_IntClr(camId, CAM_INT_MEMORY_OVERWRITE);

        if (camIntCbfArra[camId][CAM_INT_MEMORY_OVERWRITE] != NULL) {
            /* call the callback function */
            camIntCbfArra[camId][CAM_INT_MEMORY_OVERWRITE]();
        }
    }

    if (reg_is_bit_set(tmpVal, CAM_STS_FRAME_INT)) {
        CAM_IntClr(camId, CAM_INT_FRAME_OVERWRITE);

        if (camIntCbfArra[camId][CAM_INT_FRAME_OVERWRITE] != NULL) {
            /* call the callback function */
            camIntCbfArra[camId][CAM_INT_FRAME_OVERWRITE]();
        }
    }

    if (reg_is_bit_set(tmpVal, CAM_STS_FIFO_INT)) {
        CAM_IntClr(camId, CAM_INT_FIFO_OVERWRITE);

        if (camIntCbfArra[camId][CAM_INT_FIFO_OVERWRITE] != NULL) {
            /* call the callback function */
            camIntCbfArra[camId][CAM_INT_FIFO_OVERWRITE]();
        }
    }

    if (reg_is_bit_set(tmpVal, CAM_STS_HCNT_INT)) {
        CAM_IntClr(camId, CAM_INT_HSYNC_CNT_ERROR);

        if (camIntCbfArra[camId][CAM_INT_HSYNC_CNT_ERROR] != NULL) {
            /* call the callback function */
            camIntCbfArra[camId][CAM_INT_HSYNC_CNT_ERROR]();
        }
    }

    if (reg_is_bit_set(tmpVal, CAM_STS_VCNT_INT)) {
        CAM_IntClr(camId, CAM_INT_VSYNC_CNT_ERROR);

        if (camIntCbfArra[camId][CAM_INT_VSYNC_CNT_ERROR] != NULL) {
            /* call the callback function */
            camIntCbfArra[camId][CAM_INT_VSYNC_CNT_ERROR]();
        }
    }
}
#endif

void CAM_DFE_Init(CAM_DFE_Cfg_Type* dfeCfg)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(MM_MISC_BASE + DVP_MISC_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP_MISC_RG_DVPAS_HS_INV, dfeCfg->hSyncLevel);
    tmpVal = reg_set_bits(tmpVal, DVP_MISC_RG_DVPAS_VS_INV, dfeCfg->vSyncLevel);
    tmpVal = reg_set_bits(tmpVal, DVP_MISC_RG_DVPAS_DA_ORDER, dfeCfg->dataOrder);
    tmpVal = reg_set_bits(tmpVal, DVP_MISC_RG_DVPAS_FIFO_TH, dfeCfg->fifoThreshold);
    writel(tmpVal, MM_MISC_BASE + DVP_MISC_CONFIG_OFFSET);
}

void CAM_DFE_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + DVP_MISC_CONFIG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, DVP_MISC_RG_DVPAS_ENABLE);
    writel(tmpVal, MM_MISC_BASE + DVP_MISC_CONFIG_OFFSET);
}

void CAM_DFE_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + DVP_MISC_CONFIG_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, DVP_MISC_RG_DVPAS_ENABLE);
    writel(tmpVal, MM_MISC_BASE + DVP_MISC_CONFIG_OFFSET);
}

void CAM_DFE_Set_Power_Down(CAM_DFE_PWDN_Reset_Level_Type level)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + DVP_MISC_SNSR_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP_MISC_RG_SNSR_PWDN, level);
    writel(tmpVal, MM_MISC_BASE + DVP_MISC_SNSR_CTRL_OFFSET);
}

void CAM_DFE_Set_Reset(CAM_DFE_PWDN_Reset_Level_Type level)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + DVP_MISC_SNSR_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP_MISC_RG_SNSR_RST, level);
    writel(tmpVal, MM_MISC_BASE + DVP_MISC_SNSR_CTRL_OFFSET);
}

void CAM_DFE_Function_Select(CAM_DFE_Function_Type type)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + DVP_MISC_DVP2BUS_SRC_SEL_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP_MISC_RG_D2X_DVP_SEL, type);
    writel(tmpVal, MM_MISC_BASE + DVP_MISC_DVP2BUS_SRC_SEL_1_OFFSET);
}

void CAM_DFE_Frame_ID_Init(CAM_DFE_Frame_ID_Edge_Type edgeType, uint16_t count)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(MM_MISC_BASE + DVP_MISC_ISP_ID_YUV_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP_MISC_REG_YUV_IDGEN_EDGE, edgeType);
    tmpVal = reg_set_bits(tmpVal, DVP_MISC_REG_YUV_IDGEN_CNT_INCR, count);
    writel(tmpVal, MM_MISC_BASE + DVP_MISC_ISP_ID_YUV_OFFSET);
}

void CAM_DFE_Frame_ID_Reset(void)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + DVP_MISC_ISP_ID_YUV_OFFSET);
    writel(reg_set_bit(tmpVal, DVP_MISC_REG_YUV_IDGEN_RST), MM_MISC_BASE + DVP_MISC_ISP_ID_YUV_OFFSET);
}

void CAM_Init(CAM_ID_Type camId, CAM_CFG_Type *cfg)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    /* Disable clock gate */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_CAM);

    /* Set camera configuration */
    tmpVal = readl(CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, CAM_REG_DVP_ENABLE);
    writel(tmpVal, CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);

    tmpVal = reg_set_bits(tmpVal, CAM_REG_SW_MODE, cfg->swMode);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_DVP_DATA_MODE, cfg->pixWidth);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_FRAM_VLD_POL, cfg->framePol);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_LINE_VLD_POL, cfg->linePol);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_XLEN, cfg->burstType);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_DVP_MODE, cfg->camSensorMode);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_DVP_WAIT_CYCLE, cfg->waitCount);

    switch (cfg->dropMode) {
        case CAM_DROP_NONE:
            tmpVal = reg_clr_bit(tmpVal, CAM_REG_DROP_EN);
            tmpVal = reg_clr_bit(tmpVal, CAM_REG_V_SUBSAMPLE_EN);
            break;

        case CAM_DROP_ODD_ROW_ODD_PIXEL:
            tmpVal = reg_clr_bit(tmpVal, CAM_REG_DROP_EN);
            tmpVal = reg_set_bit(tmpVal, CAM_REG_V_SUBSAMPLE_EN);
            tmpVal = reg_clr_bit(tmpVal, CAM_REG_V_SUBSAMPLE_POL);
            break;

        case CAM_DROP_ODD_ROW_EVEN_PIXEL:
            tmpVal = reg_clr_bit(tmpVal, CAM_REG_DROP_EN);
            tmpVal = reg_set_bit(tmpVal, CAM_REG_V_SUBSAMPLE_EN);
            tmpVal = reg_set_bit(tmpVal, CAM_REG_V_SUBSAMPLE_POL);
            break;

        case CAM_DROP_ALL_ODD_PIXEL:
            tmpVal = reg_set_bit(tmpVal, CAM_REG_DROP_EN);
            tmpVal = reg_clr_bit(tmpVal, CAM_REG_DROP_EVEN);
            tmpVal = reg_clr_bit(tmpVal, CAM_REG_V_SUBSAMPLE_EN);
            break;

        case CAM_DROP_ALL_EVEN_PIXEL:
            tmpVal = reg_set_bit(tmpVal, CAM_REG_DROP_EN);
            tmpVal = reg_set_bit(tmpVal, CAM_REG_DROP_EVEN);
            tmpVal = reg_clr_bit(tmpVal, CAM_REG_V_SUBSAMPLE_EN);
            break;

        default:
            break;
    }

    writel(tmpVal, CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);

    /* Set frame count to issue interrupt at sw mode */
    tmpVal = readl(CAMx + CAM_DVP_STATUS_AND_ERROR_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_FRAME_CNT_TRGR_INT, cfg->swIntCnt);
    writel(tmpVal, CAMx + CAM_DVP_STATUS_AND_ERROR_OFFSET);

    /* Set camera memory start address, memory brust size and frame byte size */
    writel(cfg->memStart & 0xFFFFFFF0, CAMx + CAM_DVP2AXI_ADDR_START_OFFSET);

    switch (cfg->burstType) {
        case CAM_BURST_TYPE_SINGLE:
            tmpVal = cfg->memSize / 8;
            break;

        case CAM_BURST_TYPE_INCR4:
            tmpVal = cfg->memSize / 32;
            break;

        case CAM_BURST_TYPE_INCR8:
            tmpVal = cfg->memSize / 64;
            break;

        case CAM_BURST_TYPE_INCR16:
            tmpVal = cfg->memSize / 128;
            break;

        case CAM_BURST_TYPE_INCR32:
            tmpVal = cfg->memSize / 256;
            break;

        case CAM_BURST_TYPE_INCR64:
            tmpVal = cfg->memSize / 512;
            break;

        default:
            tmpVal = 0;
            break;
    }

    writel(tmpVal, CAMx + CAM_DVP2AXI_MEM_BCNT_OFFSET);

    writel(cfg->frameSize, CAMx + CAM_DVP2AXI_FRAME_BCNT_OFFSET);

    /* Clear interrupt */
    writel(0xFFFF0, CAMx + CAM_DVP_FRAME_FIFO_POP_OFFSET);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(DVP2BUS_INT0_IRQn, CAM0_IRQHandler);
    Interrupt_Handler_Register(DVP2BUS_INT1_IRQn, CAM1_IRQHandler);
#endif
}

void CAM_Deinit(CAM_ID_Type camId)
{
    switch(camId){
        case CAM0_ID:
            // GLB_AHB_DSP_Software_Reset(GLB_AHB_DSP_SW_SWRST_DVP2BUSA);   TODO
            break;
        case CAM1_ID:
            // GLB_AHB_DSP_Software_Reset(GLB_AHB_DSP_SW_SWRST_DVP2BUSB);   TODO
            break;
        default:
            break;
    }
}

void CAM_Enable(CAM_ID_Type camId)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    /* Enable camera module */
    tmpVal = readl(CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, CAM_REG_DVP_ENABLE);
    writel(tmpVal, CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);
}

void CAM_Disable(CAM_ID_Type camId)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    /* Disable camera module */
    tmpVal = readl(CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, CAM_REG_DVP_ENABLE);
    writel(tmpVal, CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);
}

void CAM_Clock_Gate(CAM_ID_Type camId, int enable)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    tmpVal = readl(CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_DVP_PIX_CLK_CG, enable);
    writel(tmpVal, CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);
}

void CAM_Hsync_Crop(CAM_ID_Type camId, uint16_t start, uint16_t end)
{
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    writel((start << 16) + end, CAMx + CAM_DVP2AXI_HSYNC_CROP_OFFSET);
}

void CAM_Vsync_Crop(CAM_ID_Type camId, uint16_t start, uint16_t end)
{
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    writel((start << 16) + end, CAMx + CAM_DVP2AXI_VSYNC_CROP_OFFSET);
}

void CAM_Set_Hsync_Total_Count(CAM_ID_Type camId, uint16_t count)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    tmpVal = readl(CAMx + CAM_DVP2AXI_FRAM_EXM_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_TOTAL_HCNT, count);
    writel(tmpVal, CAMx + CAM_DVP2AXI_FRAM_EXM_OFFSET);
}

void CAM_Set_Vsync_Total_Count(CAM_ID_Type camId, uint16_t count)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    tmpVal = readl(CAMx + CAM_DVP2AXI_FRAM_EXM_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_TOTAL_VCNT, count);
    writel(tmpVal, CAMx + CAM_DVP2AXI_FRAM_EXM_OFFSET);
}

void CAM_Get_Frame_Info(CAM_ID_Type camId, CAM_Frame_Info *info)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    tmpVal = readl(CAMx + CAM_DVP_STATUS_AND_ERROR_OFFSET);

    info->validFrames = reg_get_bits(tmpVal, CAM_FRAME_VALID_CNT);
    info->curFrameId = reg_get_bits(readl(CAMx + CAM_FRAME_ID_STS01_OFFSET), CAM_FRAME_ID_0);
    info->curFrameAddr = readl(CAMx + CAM_FRAME_START_ADDR0_OFFSET);
    info->curFrameBytes = readl(CAMx + CAM_DVP2AXI_FRAME_BCNT_OFFSET);
    info->status = tmpVal;
}

uint8_t CAM_Get_Frame_Count(CAM_ID_Type camId)
{
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    return reg_get_bits(readl(CAMx + CAM_DVP_STATUS_AND_ERROR_OFFSET), CAM_FRAME_VALID_CNT);
}

void CAM_Pop_Frame(CAM_ID_Type camId)
{
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    /* Pop one frame */
    writel(1, CAMx + CAM_DVP_FRAME_FIFO_POP_OFFSET);
}

void CAM_IntMask(CAM_ID_Type camId, CAM_INT_Type intType, int intMask)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    tmpVal = readl(CAMx + CAM_DVP_STATUS_AND_ERROR_OFFSET);

    switch (intType) {
        case CAM_INT_NORMAL:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = reg_set_bit(tmpVal, CAM_REG_INT_NORMAL_EN);
            } else {
                /* Disable this interrupt */
                tmpVal = reg_clr_bit(tmpVal, CAM_REG_INT_NORMAL_EN);
            }

            break;

        case CAM_INT_MEMORY_OVERWRITE:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = reg_set_bit(tmpVal, CAM_REG_INT_MEM_EN);
            } else {
                /* Disable this interrupt */
                tmpVal = reg_clr_bit(tmpVal, CAM_REG_INT_MEM_EN);
            }

            break;

        case CAM_INT_FRAME_OVERWRITE:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = reg_set_bit(tmpVal, CAM_REG_INT_FRAME_EN);
            } else {
                /* Disable this interrupt */
                tmpVal = reg_clr_bit(tmpVal, CAM_REG_INT_FRAME_EN);
            }

            break;

        case CAM_INT_FIFO_OVERWRITE:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = reg_set_bit(tmpVal, CAM_REG_INT_FIFO_EN);
            } else {
                /* Disable this interrupt */
                tmpVal = reg_clr_bit(tmpVal, CAM_REG_INT_FIFO_EN);
            }

            break;

        case CAM_INT_VSYNC_CNT_ERROR:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = reg_set_bit(tmpVal, CAM_REG_INT_VCNT_EN);
            } else {
                /* Disable this interrupt */
                tmpVal = reg_clr_bit(tmpVal, CAM_REG_INT_VCNT_EN);
            }

            break;

        case CAM_INT_HSYNC_CNT_ERROR:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = reg_set_bit(tmpVal, CAM_REG_INT_HCNT_EN);
            } else {
                /* Disable this interrupt */
                tmpVal = reg_clr_bit(tmpVal, CAM_REG_INT_HCNT_EN);
            }

            break;

        case CAM_INT_ALL:
            if (intMask == UNMASK) {
                /* Enable all interrupt */
                tmpVal |= 0xFC0;
            } else {
                /* Disable all interrupt */
                tmpVal &= 0xFFFFF03F;
            }

            break;

        default:
            break;
    }

    writel(tmpVal, CAMx + CAM_DVP_STATUS_AND_ERROR_OFFSET);
}

void CAM_IntClr(CAM_ID_Type camId, CAM_INT_Type intType)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    tmpVal = readl(CAMx + CAM_DVP_FRAME_FIFO_POP_OFFSET);

    switch (intType) {
        case CAM_INT_NORMAL:
            tmpVal = reg_set_bit(tmpVal, CAM_REG_INT_NORMAL_CLR);
            break;

        case CAM_INT_MEMORY_OVERWRITE:
            tmpVal = reg_set_bit(tmpVal, CAM_REG_INT_MEM_CLR);
            break;

        case CAM_INT_FRAME_OVERWRITE:
            tmpVal = reg_set_bit(tmpVal, CAM_REG_INT_FRAME_CLR);
            break;

        case CAM_INT_FIFO_OVERWRITE:
            tmpVal = reg_set_bit(tmpVal, CAM_REG_INT_FIFO_CLR);
            break;

        case CAM_INT_VSYNC_CNT_ERROR:
            tmpVal = reg_set_bit(tmpVal, CAM_REG_INT_VCNT_CLR);
            break;

        case CAM_INT_HSYNC_CNT_ERROR:
            tmpVal = reg_set_bit(tmpVal, CAM_REG_INT_HCNT_CLR);
            break;

        case CAM_INT_ALL:
            tmpVal = 0xFFFF0;

        default:
            break;
    }

    writel(tmpVal, CAMx + CAM_DVP_FRAME_FIFO_POP_OFFSET);
}

#ifndef IPRO_USE_HAL_DRIVER
void CAM_Int_Callback_Install(CAM_ID_Type camId, CAM_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */

    camIntCbfArra[camId][intType] = cbFun;
}
#endif

void CAM_8_Bit_Byte_Select(CAM_ID_Type camId, CAM_8_Bit_Byte_Type byteType)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    tmpVal = readl(CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_DVP_DATA_BSEL, byteType);
    writel(tmpVal, CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);
}

void CAM_16_Bit_RGB_order(CAM_ID_Type camId, CAM_16_Bit_RGB_Type rgbType)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    tmpVal = readl(CAMx + CAM_DVP2AXI_MISC_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_FORMAT_565, rgbType);
    writel(tmpVal, CAMx + CAM_DVP2AXI_MISC_OFFSET);
}

void CAM_32_Bit_Alpha_Set(CAM_ID_Type camId, uint8_t alpha)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    tmpVal = readl(CAMx + CAM_DVP2AXI_MISC_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_ALPHA, alpha);
    writel(tmpVal, CAMx + CAM_DVP2AXI_MISC_OFFSET);
}

void CAM_Set_Frame_Valid_Period(CAM_ID_Type camId, uint8_t period, uint32_t validBit)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    tmpVal = readl(CAMx + CAM_DVP2AXI_FRAME_PERIOD_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_FRAME_PERIOD, period);
    writel(tmpVal, CAMx + CAM_DVP2AXI_FRAME_PERIOD_OFFSET);

    writel(validBit, CAMx + CAM_DVP2AXI_FRAME_VLD_OFFSET);
}

void CAM_HW_Cover_First_Frame_Everytime(CAM_ID_Type camId, int enable)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    /* Check the parameters */

    tmpVal = readl(CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_HW_MODE_FWRAP, enable);
    writel(tmpVal, CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);
}

#ifndef IPRO_USE_HAL_DRIVER
void CAM0_IRQHandler(void)
{
    CAM_IntHandler(CAM0_ID);
}
#endif

#ifndef IPRO_USE_HAL_DRIVER
void CAM1_IRQHandler(void)
{
    CAM_IntHandler(CAM1_ID);
}
#endif


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

#include "ipro7.h"
#include "drv_dvp2axi.h"
#include "drv_glb.h"

/** @defgroup  CAM_Private_Variables
 *  @{
 */
static const uint32_t camAddr[CAM_ID_MAX] = { D2XA_BASE, D2XB_BASE };

/****************************************************************************//**
 * @brief  CAM data front end function init
 *
 * @param  dfeCfg: CAM data front end configuration structure pointer
 *
 * @return None
 *
*******************************************************************************/
void CAM_DFE_Init(CAM_DFE_Cfg_Type* dfeCfg)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_DVP_AS_REG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_RG_DVPAS_HS_INV, dfeCfg->hSyncLevel);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_RG_DVPAS_VS_INV, dfeCfg->vSyncLevel);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_RG_DVPAS_DA_ORDER, dfeCfg->dataOrder);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_RG_DVPAS_FIFO_TH, dfeCfg->fifoThreshold);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_DVP_AS_REG_OFFSET);
}

/****************************************************************************//**
 * @brief  CAM data front end function enable
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void CAM_DFE_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_DVP_AS_REG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, MM_MISC_RG_DVPAS_ENABLE);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_DVP_AS_REG_OFFSET);
}

/****************************************************************************//**
 * @brief  CAM data front end function disable
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void CAM_DFE_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_DVP_AS_REG_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, MM_MISC_RG_DVPAS_ENABLE);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_DVP_AS_REG_OFFSET);
}

/* TODO
   d2x_mux, yuv422_mux, yuv444_mux
    DVP Mux selection
    [0] dvp_as
    [1] scaler_down (low cost)
    [2] osd draw
    [3] yuv444to422
    [4] blai
    [5] yuv422to444
    [6] yuv2rgb
    [7] yuv422to420
    [8] isp_out
    [9] awb3_ink
    [10] scaler_up_down
*/


void CAM_Init(CAM_ID_Type camId, CAM_CFG_Type *cfg)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    /* Disable clock gate */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_CAM);
    //GLB_AHB_Slave1_Clock_Gate(DISABLE, AHB_SLAVE1_CAM); // TODO

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
    writel(tmpVal, D2XA_BASE + CAM_DVP_STATUS_AND_ERROR_OFFSET);

    /* Set camera memory start address, memory size and frame size in burst */
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

    /* Enable camera module */
    tmpVal = readl(CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, CAM_REG_DVP_ENABLE);
    writel(tmpVal, CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);
}

void CAM_Disable(CAM_ID_Type camId)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    /* Disable camera module */
    tmpVal = readl(CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, CAM_REG_DVP_ENABLE);
    writel(tmpVal, CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);
}

void CAM_Clock_Gate(CAM_ID_Type camId, int enable)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    tmpVal = readl(CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_DVP_PIX_CLK_CG, enable);
    writel(tmpVal, CAMx + CAM_DVP2AXI_CONFIGUE_OFFSET);
}

void CAM_Hsync_Crop(CAM_ID_Type camId, uint16_t start, uint16_t end)
{
    uint32_t CAMx = camAddr[camId];
    writel((start << 16) + end, CAMx + CAM_DVP2AXI_HSYNC_CROP_OFFSET);
}

void CAM_Vsync_Crop(CAM_ID_Type camId, uint16_t start, uint16_t end)
{
    uint32_t CAMx = camAddr[camId];
    writel((start << 16) + end, CAMx + CAM_DVP2AXI_VSYNC_CROP_OFFSET);
}

void CAM_Set_Hsync_Total_Count(CAM_ID_Type camId, uint16_t count)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

    tmpVal = readl(CAMx + CAM_DVP2AXI_FRAM_EXM_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_TOTAL_HCNT, count);
    writel(tmpVal, CAMx + CAM_DVP2AXI_FRAM_EXM_OFFSET);
}

void CAM_Set_Vsync_Total_Count(CAM_ID_Type camId, uint16_t count)
{
    uint32_t tmpVal;

    tmpVal = readl(D2XA_BASE + CAM_DVP2AXI_FRAM_EXM_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CAM_REG_TOTAL_VCNT, count);
    writel(tmpVal, D2XA_BASE + CAM_DVP2AXI_FRAM_EXM_OFFSET);
}

void CAM_Get_Frame_Info(CAM_ID_Type camId, CAM_Frame_Info *info)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

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

    return reg_get_bits(readl(CAMx + CAM_DVP_STATUS_AND_ERROR_OFFSET), CAM_FRAME_VALID_CNT);
}

void CAM_Pop_Frame(CAM_ID_Type camId)
{
    uint32_t CAMx = camAddr[camId];

    /* Pop one frame */
    writel(1, CAMx + CAM_DVP_FRAME_FIFO_POP_OFFSET);
}

void CAM_IntMask(CAM_ID_Type camId, CAM_INT_Type intType, int intMask)
{
    uint32_t tmpVal;
    uint32_t CAMx = camAddr[camId];

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

uint32_t CAM_IntStatus(CAM_ID_Type camId, CAM_INT_Type intType)
{
    uint32_t tmpVal = 0;
    uint32_t CAMx = camAddr[camId];

    tmpVal = readl(CAMx + CAM_DVP_STATUS_AND_ERROR_OFFSET);

    switch (intType) {
        case CAM_INT_NORMAL:
            tmpVal = reg_get_bits(tmpVal, CAM_STS_NORMAL_INT);
            break;

        case CAM_INT_MEMORY_OVERWRITE:
            tmpVal = reg_get_bits(tmpVal, CAM_STS_MEM_INT);
            break;

        case CAM_INT_FRAME_OVERWRITE:
            tmpVal = reg_get_bits(tmpVal, CAM_STS_FRAME_INT);
            break;

        case CAM_INT_FIFO_OVERWRITE:
            tmpVal = reg_get_bits(tmpVal, CAM_STS_FIFO_INT);
            break;

        case CAM_INT_VSYNC_CNT_ERROR:
            tmpVal = reg_get_bits(tmpVal, CAM_STS_VCNT_INT);
            break;

        case CAM_INT_HSYNC_CNT_ERROR:
            tmpVal = reg_get_bits(tmpVal, CAM_STS_HCNT_INT);
            break;

        default:
            break;
    }
    return tmpVal;
}


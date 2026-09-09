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
#include "drv_dma.h"
#include "drv_glb.h"

#define DMA_CHANNEL_OFFSET           0x100
#define DMA_Get_Channel(id_base, ch) ((id_base) + DMA_CHANNEL_OFFSET + (ch)*0x100)

static const uint32_t dmaAddr[DRV_DMA_ID_MAX] = { DMA_BASE };
#ifndef IPRO_USE_HAL_DRIVER
static intCallback_Type *dmaIntCbfArra[DRV_DMA_ID_MAX][DMA_CH_MAX][DMA_INT_ALL] = {
    { { NULL } }
};
#endif
static DMA_LLI_Ctrl_Type PingPongListArra[DRV_DMA_ID_MAX][DMA_CH_MAX][2];

#ifndef IPRO_USE_HAL_DRIVER
static void DMA_IntHandler(DMA_ID_Type dmaId);
#endif
static void CPU_Interrupt_Enable_DMA(DMA_ID_Type dmaId);
static void CPU_Interrupt_Disable_DMA(DMA_ID_Type dmaId);

#ifndef IPRO_USE_HAL_DRIVER
static void DMA_IntHandler(DMA_ID_Type dmaId)
{
    uint32_t tmpVal;
    uint32_t intClr;
    uint8_t ch;
    /* Get DMA register */
    uint32_t DMAChs = dmaAddr[dmaId];

    for (ch = 0; ch < DMA_CH_MAX; ch++) {
        tmpVal = readl(DMAChs + DMA_INTTCSTATUS_OFFSET);

        if ((reg_get_bits(tmpVal, DMA_INTTCSTATUS) & (1 << ch)) != 0) {
            /* Clear interrupt */
            tmpVal = readl(DMAChs + DMA_INTTCCLEAR_OFFSET);
            intClr = reg_get_bits(tmpVal, DMA_INTTCCLEAR);
            intClr |= (1 << ch);
            tmpVal = reg_set_bits(tmpVal, DMA_INTTCCLEAR, intClr);
            writel(tmpVal, DMAChs + DMA_INTTCCLEAR_OFFSET);

            if (dmaIntCbfArra[dmaId][ch][DMA_INT_TCOMPLETED] != NULL) {
                /* Call the callback function */
                dmaIntCbfArra[dmaId][ch][DMA_INT_TCOMPLETED]();
            }
        }
    }

    for (ch = 0; ch < DMA_CH_MAX; ch++) {
        tmpVal = readl(DMAChs + DMA_INTERRORSTATUS_OFFSET);

        if ((reg_get_bits(tmpVal, DMA_INTERRORSTATUS) & (1 << ch)) != 0) {
            /*Clear interrupt */
            tmpVal = readl(DMAChs + DMA_INTERRCLR_OFFSET);
            intClr = reg_get_bits(tmpVal, DMA_INTERRCLR);
            intClr |= (1 << ch);
            tmpVal = reg_set_bits(tmpVal, DMA_INTERRCLR, intClr);
            writel(tmpVal, DMAChs + DMA_INTERRCLR_OFFSET);

            if (dmaIntCbfArra[dmaId][ch][DMA_INT_ERR] != NULL) {
                /* Call the callback function */
                dmaIntCbfArra[dmaId][ch][DMA_INT_ERR]();
            }
        }
    }
}
#endif

static void CPU_Interrupt_Enable_DMA(DMA_ID_Type dmaId)
{
    /* Check the parameters */

    if (DRV_DMA0_ID == dmaId) {
        CPU_Interrupt_Enable(DMA0_ALL_IRQn);
    }
}

static void CPU_Interrupt_Disable_DMA(DMA_ID_Type dmaId)
{
    /* Check the parameters */

    if (DRV_DMA0_ID == dmaId) {
        CPU_Interrupt_Disable(DMA0_ALL_IRQn);
    }
}

void DMA_Enable(DMA_ID_Type dmaId)
{
    uint32_t tmpVal;
    /* Get DMA register */
    uint32_t DMAChs = dmaAddr[dmaId];

    /* Check the parameters */

    tmpVal = readl(DMAChs + DMA_TOP_CONFIG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, DMA_E);
    writel(tmpVal, DMAChs + DMA_TOP_CONFIG_OFFSET);
#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(DMA0_ALL_IRQn, DMA0_ALL_IRQHandler);
#endif
}

void DMA_Disable(DMA_ID_Type dmaId)
{
    uint32_t tmpVal;
    /* Get DMA register */
    uint32_t DMAChs = dmaAddr[dmaId];

    /* Check the parameters */

    tmpVal = readl(DMAChs + DMA_TOP_CONFIG_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, DMA_E);
    writel(tmpVal, DMAChs + DMA_TOP_CONFIG_OFFSET);
}

void DMA_Channel_Init(DMA_ID_Type dmaId, DMA_Channel_Cfg_Type *chCfg)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(dmaAddr[dmaId], chCfg->ch);

    /* Check the parameters */

    /* Disable clock gate when use DMA0 */
    if (DRV_DMA0_ID == dmaId) {
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DMA_0);
    }

    /* Config channel config */
    writel(chCfg->srcDmaAddr, DMAChs + DMA_SRCADDR_OFFSET);
    writel(chCfg->destDmaAddr, DMAChs + DMA_DSTADDR_OFFSET);

    tmpVal = readl(DMAChs + DMA_CONTROL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DMA_TRANSFERSIZE, chCfg->transfLength);
    tmpVal = reg_set_bits(tmpVal, DMA_SWIDTH, chCfg->srcTransfWidth);
    tmpVal = reg_set_bits(tmpVal, DMA_DWIDTH, chCfg->dstTransfWidth);
    tmpVal = reg_set_bits(tmpVal, DMA_SBSIZE, chCfg->srcBurstSize);
    tmpVal = reg_set_bits(tmpVal, DMA_DBSIZE, chCfg->dstBurstSize);

    tmpVal = reg_set_bits(tmpVal, DMA_DST_ADD_MODE, chCfg->dstAddMode);
    tmpVal = reg_set_bits(tmpVal, DMA_DST_MIN_MODE, chCfg->dstMinMode);
    tmpVal = reg_set_bits(tmpVal, DMA_FIX_CNT, chCfg->fixCnt);

    tmpVal = reg_set_bits(tmpVal, DMA_SI, chCfg->srcAddrInc);
    tmpVal = reg_set_bits(tmpVal, DMA_DI, chCfg->destAddrInc);
    writel(tmpVal, DMAChs + DMA_CONTROL_OFFSET);

    tmpVal = readl(DMAChs + DMA_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DMA_FLOWCNTRL, chCfg->dir);
    tmpVal = reg_set_bits(tmpVal, DMA_DSTPERIPHERAL, chCfg->dstPeriph);
    tmpVal = reg_set_bits(tmpVal, DMA_SRCPERIPHERAL, chCfg->srcPeriph);
    writel(tmpVal, DMAChs + DMA_CONFIG_OFFSET);
}

void DMA_DeInit(DMA_ID_Type dmaId)
{
    switch (dmaId) {
        case DRV_DMA0_ID:
            GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_DMA);
            break;

        default:
            break;
    }
}

void DMA_Channel_Update_SrcMemcfg(DMA_ID_Type dmaId, uint8_t ch, uint32_t memAddr, uint32_t len)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(dmaAddr[dmaId], ch);

    /* Check the parameters */

    /* config channel config*/
    writel(memAddr, DMAChs + DMA_SRCADDR_OFFSET);
    tmpVal = readl(DMAChs + DMA_CONTROL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DMA_TRANSFERSIZE, len);
    writel(tmpVal, DMAChs + DMA_CONTROL_OFFSET);
}

void DMA_Channel_Update_DstMemcfg(DMA_ID_Type dmaId, uint8_t ch, uint32_t memAddr, uint32_t len)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(dmaAddr[dmaId], ch);

    /* Check the parameters */

    /* config channel config*/
    writel(memAddr, DMAChs + DMA_DSTADDR_OFFSET);
    tmpVal = readl(DMAChs + DMA_CONTROL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DMA_TRANSFERSIZE, len);
    writel(tmpVal, DMAChs + DMA_CONTROL_OFFSET);
}

uint32_t DMA_Channel_TranferSize(DMA_ID_Type dmaId, uint8_t ch)
{
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(dmaAddr[dmaId], ch);

    /* Check the parameters */

    return reg_get_bits(readl(DMAChs + DMA_CONTROL_OFFSET), DMA_TRANSFERSIZE);
}

int DMA_Channel_Is_Busy(DMA_ID_Type dmaId, uint8_t ch)
{
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(dmaAddr[dmaId], ch);

    /* Check the parameters */

    return reg_is_bit_set(readl(DMAChs + DMA_CONFIG_OFFSET), DMA_E) == 1 ? SET : RESET;
}

void DMA_Channel_Enable(DMA_ID_Type dmaId, uint8_t ch)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(dmaAddr[dmaId], ch);

    /* Check the parameters */

    tmpVal = readl(DMAChs + DMA_CONFIG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, DMA_E);
    writel(tmpVal, DMAChs + DMA_CONFIG_OFFSET);
}

void DMA_Channel_Disable(DMA_ID_Type dmaId, uint8_t ch)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(dmaAddr[dmaId], ch);

    /* Check the parameters */

    tmpVal = readl(DMAChs + DMA_CONFIG_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, DMA_E);
    writel(tmpVal, DMAChs + DMA_CONFIG_OFFSET);
}

void DMA_LLI_Init(DMA_ID_Type dmaId, uint8_t ch, DMA_LLI_Cfg_Type *lliCfg)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(dmaAddr[dmaId], ch);

    /* Check the parameters */

    /* Disable clock gate when use DMA0 */
    if (DRV_DMA0_ID == dmaId) {
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DMA_0);
    }

    tmpVal = readl(DMAChs + DMA_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DMA_FLOWCNTRL, lliCfg->dir);
    tmpVal = reg_set_bits(tmpVal, DMA_DSTPERIPHERAL, lliCfg->dstPeriph);
    tmpVal = reg_set_bits(tmpVal, DMA_SRCPERIPHERAL, lliCfg->srcPeriph);
    writel(tmpVal, DMAChs + DMA_CONFIG_OFFSET);
}

void DMA_LLI_Update(DMA_ID_Type dmaId, uint8_t ch, uint32_t LLI)
{
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(dmaAddr[dmaId], ch);

    /* Check the parameters */

    /* Config channel config */
    //writel(LLI, DMAChs + DMA_LLI_OFFSET);
    ARCH_MemCpy4((uint32_t *)(uintptr_t)DMAChs, (uint32_t *)(uintptr_t)LLI, 4);
}

uint32_t DMA_LLI_Get_Counter(DMA_ID_Type dmaId, uint8_t ch)
{
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(dmaAddr[dmaId], ch);

    /* Check the parameters */

    return reg_get_bits(readl(DMAChs + DMA_CONFIG_OFFSET), DMA_LLICOUNTER);
}

uint32_t DMA_LLI_Get_Dstaddr(DMA_ID_Type dmaId, uint8_t ch)
{
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(dmaAddr[dmaId], ch);

    /* Check the parameters */

    return readl(DMAChs + DMA_DSTADDR_OFFSET);
}

int DMA_LLI_PpStruct_Set_Transfer_Len(DMA_LLI_PP_Struct *dmaPpStruct, uint16_t Ping_Transfer_len, uint16_t Pong_Transfer_len)
{
    struct DMA_Control_Reg dmaCtrlRegVal_temp;

    if (Ping_Transfer_len > 4096 || Pong_Transfer_len > 4096) {
        return ERROR;
    }

    dmaCtrlRegVal_temp = PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PING_INDEX].dmaCtrl;
    dmaCtrlRegVal_temp.TransferSize = Ping_Transfer_len;
    PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PING_INDEX].dmaCtrl = dmaCtrlRegVal_temp;

    dmaCtrlRegVal_temp = PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PONG_INDEX].dmaCtrl;
    dmaCtrlRegVal_temp.TransferSize = Pong_Transfer_len;
    PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PONG_INDEX].dmaCtrl = dmaCtrlRegVal_temp;

    DMA_LLI_Init(dmaPpStruct->dmaId, dmaPpStruct->dmaChan, dmaPpStruct->lliCfg);
    DMA_LLI_Update(dmaPpStruct->dmaId, dmaPpStruct->dmaChan, (uint32_t)(uintptr_t)&PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PING_INDEX]);

    return SUCCESS;
}

void DMA_LLI_PpBuf_Start_New_Transmit(DMA_LLI_PP_Buf *dmaPpBuf)
{
    CPU_Interrupt_Disable_DMA(dmaPpBuf->dmaId);

    if (dmaPpBuf->lliListHeader[dmaPpBuf->idleIndex] != NULL) {
        DMA_LLI_Update(dmaPpBuf->dmaId, dmaPpBuf->dmaChan, (uint32_t)(uintptr_t)dmaPpBuf->lliListHeader[dmaPpBuf->idleIndex]);
        DMA_Channel_Enable(dmaPpBuf->dmaId, dmaPpBuf->dmaChan);
        dmaPpBuf->idleIndex = (dmaPpBuf->idleIndex == 0) ? 1 : 0;
    }

    CPU_Interrupt_Enable_DMA(dmaPpBuf->dmaId);
}

DMA_LLI_Ctrl_Type *DMA_LLI_PpBuf_Remove_Completed_List(DMA_LLI_PP_Buf *dmaPpBuf)
{
    CPU_Interrupt_Disable_DMA(dmaPpBuf->dmaId);

    dmaPpBuf->lliListHeader[!dmaPpBuf->idleIndex] = NULL;
    CPU_Interrupt_Enable_DMA(dmaPpBuf->dmaId);
    return dmaPpBuf->lliListHeader[!dmaPpBuf->idleIndex];
}

void DMA_LLI_PpBuf_Append(DMA_LLI_PP_Buf *dmaPpBuf, DMA_LLI_Ctrl_Type *dmaLliList)
{
    DMA_LLI_Ctrl_Type *pLliList = NULL;
    CPU_Interrupt_Disable_DMA(dmaPpBuf->dmaId);

    pLliList = dmaPpBuf->lliListHeader[dmaPpBuf->idleIndex];

    if (pLliList == NULL) {
        dmaLliList->nextLLI = 0;
        dmaLliList->dmaCtrl.I = 1;
        dmaPpBuf->lliListHeader[dmaPpBuf->idleIndex] = dmaLliList;
    } else {
        /*Append to last */
        while (pLliList->nextLLI != 0) {
            pLliList = (DMA_LLI_Ctrl_Type *)(uintptr_t)pLliList->nextLLI;
        }

        pLliList->nextLLI = (uint32_t)(uintptr_t)dmaLliList;
        pLliList->dmaCtrl.I = 0;
        dmaLliList->nextLLI = 0;
        dmaLliList->dmaCtrl.I = 1;
    }

    if (DMA_Channel_Is_Busy(dmaPpBuf->dmaId, dmaPpBuf->dmaChan) == RESET) {
        /* DMA stopped: maybe stop just a few minutes ago(not enter INT due to CPU_Interrupt_Disable)
		   or has already stopped before this function is called */
        if (dmaPpBuf->lliListHeader[!dmaPpBuf->idleIndex] == NULL) {
            /* DMA has already stopped before this function is called */
            DMA_LLI_PpBuf_Start_New_Transmit(dmaPpBuf);
        }
    }

    CPU_Interrupt_Enable_DMA(dmaPpBuf->dmaId);
}

void DMA_LLI_PpBuf_Destroy(DMA_LLI_PP_Buf *dmaPpBuf)
{
    /* DMA LLI Disable */
    DMA_Channel_Disable(dmaPpBuf->dmaId, dmaPpBuf->dmaChan);

    if (dmaPpBuf->lliListHeader[0] != NULL && dmaPpBuf->onTransCompleted != NULL) {
        dmaPpBuf->onTransCompleted(dmaPpBuf->lliListHeader[0]);
    }

    dmaPpBuf->lliListHeader[0] = NULL;

    if (dmaPpBuf->lliListHeader[1] != NULL && dmaPpBuf->onTransCompleted != NULL) {
        dmaPpBuf->onTransCompleted(dmaPpBuf->lliListHeader[1]);
    }

    dmaPpBuf->lliListHeader[1] = NULL;
    dmaPpBuf->idleIndex = 0;
}

void DMA_IntMask(DMA_ID_Type dmaId, uint8_t ch, DMA_INT_Type intType, int intMask)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(dmaAddr[dmaId], ch);

    /* Check the parameters */

    switch (intType) {
        case DMA_INT_TCOMPLETED:
            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                tmpVal = reg_clr_bit(readl(DMAChs + DMA_CONFIG_OFFSET), DMA_ITC);
                writel(tmpVal, DMAChs + DMA_CONFIG_OFFSET);
                tmpVal = reg_set_bit(readl(DMAChs + DMA_CONTROL_OFFSET), DMA_I);
                writel(tmpVal, DMAChs + DMA_CONTROL_OFFSET);
            } else {
                /* MASK(Disable) this interrupt */
                tmpVal = reg_set_bit(readl(DMAChs + DMA_CONFIG_OFFSET), DMA_ITC);
                writel(tmpVal, DMAChs + DMA_CONFIG_OFFSET);
                tmpVal = reg_clr_bit(readl(DMAChs + DMA_CONTROL_OFFSET), DMA_I);
                writel(tmpVal, DMAChs + DMA_CONTROL_OFFSET);
            }

            break;

        case DMA_INT_ERR:
            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                tmpVal = reg_clr_bit(readl(DMAChs + DMA_CONFIG_OFFSET), DMA_IE);
                writel(tmpVal, DMAChs + DMA_CONFIG_OFFSET);
            } else {
                /* MASK(Disable) this interrupt */
                tmpVal = reg_set_bit(readl(DMAChs + DMA_CONFIG_OFFSET), DMA_IE);
                writel(tmpVal, DMAChs + DMA_CONFIG_OFFSET);
            }

            break;

        case DMA_INT_ALL:
            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                tmpVal = readl(DMAChs + DMA_CONFIG_OFFSET);
                tmpVal = reg_clr_bit(tmpVal, DMA_ITC);
                tmpVal = reg_clr_bit(tmpVal, DMA_IE);
                writel(tmpVal, DMAChs + DMA_CONFIG_OFFSET);
                tmpVal = readl(DMAChs + DMA_CONTROL_OFFSET);
                tmpVal = reg_set_bit(tmpVal, DMA_I);
                writel(tmpVal, DMAChs + DMA_CONTROL_OFFSET);
            } else {
                /* MASK(Disable) this interrupt */
                tmpVal = readl(DMAChs + DMA_CONFIG_OFFSET);
                tmpVal = reg_set_bit(tmpVal, DMA_ITC);
                tmpVal = reg_set_bit(tmpVal, DMA_IE);
                writel(tmpVal, DMAChs + DMA_CONFIG_OFFSET);
                tmpVal = readl(DMAChs + DMA_CONTROL_OFFSET);
                tmpVal = reg_clr_bit(tmpVal, DMA_I);
                writel(tmpVal, DMAChs + DMA_CONTROL_OFFSET);
            }

            break;

        default:
            break;
    }
}

#ifndef IPRO_USE_HAL_DRIVER
void DMA_Int_Callback_Install(DMA_ID_Type dmaId, DMA_Chan_Type dmaChan, DMA_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */

    dmaIntCbfArra[dmaId][dmaChan][intType] = cbFun;
}
#endif

#ifndef IPRO_USE_HAL_DRIVER
void DMA0_ALL_IRQHandler(void)
{
    DMA_IntHandler(DRV_DMA0_ID);
}
#endif

uint32_t DMA_GetIntStatus(DMA_ID_Type dmaId)
{
	return reg_get_bits(readl(dmaAddr[dmaId] + DMA_INTTCSTATUS_OFFSET), DMA_INTTCSTATUS);
}

void DMA_ClrIntStatus(DMA_ID_Type dmaId, uint8_t ch)
{
	uint32_t tmpVal = readl(dmaAddr[dmaId] + DMA_INTTCCLEAR_OFFSET);
	uint32_t intClr = reg_get_bits(tmpVal, DMA_INTTCCLEAR);
	intClr |= (1 << ch);
	tmpVal = reg_set_bits(tmpVal, DMA_INTTCCLEAR, intClr);
	writel(tmpVal, dmaAddr[dmaId] + DMA_INTTCCLEAR_OFFSET);
}

void DMA_ClrErrStatus(DMA_ID_Type dmaId, uint8_t ch)
{
	uint32_t tmpVal = readl(dmaAddr[dmaId] + DMA_INTERRCLR_OFFSET);
	uint32_t intClr = reg_get_bits(tmpVal, DMA_INTERRCLR);
	intClr |= (1 << ch);
	tmpVal = reg_set_bits(tmpVal, DMA_INTERRCLR, intClr);
	writel(tmpVal, dmaAddr[dmaId] + DMA_INTERRCLR_OFFSET);
}

uint32_t DMA_GetErrStatus(DMA_ID_Type dmaId)
{
	return reg_get_bits(readl(dmaAddr[dmaId] + DMA_INTERRORSTATUS_OFFSET), DMA_INTERRORSTATUS);
}

int DMA_LLI_PpStruct_Init(DMA_LLI_PP_Struct *dmaPpStruct)
{
    //setup lliList
    dmaPpStruct->dmaCtrlRegVal.I = 1;
    dmaPpStruct->pingpongIndex = 0;

    if (dmaPpStruct->lliCfg->dir == DMA_TRNS_M2P) {
        PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PING_INDEX].srcDmaAddr = dmaPpStruct->pingpongBufAddr[0];
        PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PING_INDEX].destDmaAddr = dmaPpStruct->operatePeriphAddr;

        PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PONG_INDEX].srcDmaAddr = dmaPpStruct->pingpongBufAddr[1];
        PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PONG_INDEX].destDmaAddr = dmaPpStruct->operatePeriphAddr;
    } else if (dmaPpStruct->lliCfg->dir == DMA_TRNS_P2M) {
        PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PING_INDEX].srcDmaAddr = dmaPpStruct->operatePeriphAddr;
        PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PING_INDEX].destDmaAddr = dmaPpStruct->pingpongBufAddr[0];

        PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PONG_INDEX].srcDmaAddr = dmaPpStruct->operatePeriphAddr;
        PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PONG_INDEX].destDmaAddr = dmaPpStruct->pingpongBufAddr[1];
    } else {
        return ERROR;
        /*V1.0 version DMA LLI Ping-Pong structure not support P2P & M2M MODE*/
    }

    PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PING_INDEX].nextLLI = (uint32_t)(uintptr_t)&PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PONG_INDEX];
    PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PING_INDEX].dmaCtrl = dmaPpStruct->dmaCtrlRegVal;

    PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PONG_INDEX].nextLLI = (uint32_t)(uintptr_t)&PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PING_INDEX];
    PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PONG_INDEX].dmaCtrl = dmaPpStruct->dmaCtrlRegVal;

    DMA_LLI_Init(dmaPpStruct->dmaId, dmaPpStruct->dmaChan, dmaPpStruct->lliCfg);

    DMA_LLI_Update(dmaPpStruct->dmaId, dmaPpStruct->dmaChan, (uint32_t)(uintptr_t)&PingPongListArra[dmaPpStruct->dmaId][dmaPpStruct->dmaChan][PING_INDEX]);

    return SUCCESS;
}

void DMA_LLI_PpStruct_Start(DMA_LLI_PP_Struct *dmaPpStruct)
{
    DMA_Channel_Enable(dmaPpStruct->dmaId, dmaPpStruct->dmaChan);
}

void DMA_LLI_PpStruct_Stop(DMA_LLI_PP_Struct *dmaPpStruct)
{
    DMA_Channel_Disable(dmaPpStruct->dmaId, dmaPpStruct->dmaChan);
}


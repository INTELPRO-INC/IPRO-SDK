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
#include "drv_emac.h"
#include "drv_glb.h"

#define PHY_MAX_RETRY (0x3F)

static const uint32_t emacAddr[EMAC_ID_MAX] = { EMAC_BASE };

static void EMAC_SetMACAddress(EMAC_ID_Type emacId, uint8_t macAddr[6])
{
    uint32_t EMACx = emacAddr[emacId];
    writel((macAddr[0] << 8) | macAddr[1], EMACx + EMAC_MAC_ADDR1_OFFSET);
    writel((macAddr[2] << 24) | (macAddr[3] << 16) | (macAddr[4] << 8) | (macAddr[5] << 0), EMACx + EMAC_MAC_ADDR0_OFFSET);
}

void EMAC_Phy_SetAddress(EMAC_ID_Type emacId, uint16_t phyAddress)
{
    uint32_t tmpVal;
	uint32_t EMACx = emacAddr[emacId];

    /* Set Phy Address */
    tmpVal = readl(EMACx + EMAC_MIIADDRESS_OFFSET);
    tmpVal = reg_set_bits(tmpVal, EMAC_FIAD, phyAddress);
    writel(tmpVal, EMACx + EMAC_MIIADDRESS_OFFSET);
}

void EMAC_Phy_Set_Full_Duplex(EMAC_ID_Type emacId, uint8_t fullDuplex)
{
    uint32_t tmpVal;
	uint32_t EMACx = emacAddr[emacId];

    /* Set MAC duplex config */
    tmpVal = readl(EMACx + EMAC_MODE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, EMAC_FULLD, fullDuplex);
    writel(tmpVal, EMACx + EMAC_MODE_OFFSET);
}

int EMAC_Phy_Read(EMAC_ID_Type emacId, uint16_t phyReg, uint16_t *regValue)
{
    uint32_t tmpVal;
    uint32_t EMACx = emacAddr[emacId];

    /* Set Register Address */
    tmpVal = readl(EMACx + EMAC_MIIADDRESS_OFFSET);
    tmpVal = reg_set_bits(tmpVal, EMAC_RGAD, phyReg);
    writel(tmpVal, EMACx + EMAC_MIIADDRESS_OFFSET);

    /* Trigger read */
    tmpVal = readl(EMACx + EMAC_MIICOMMAND_OFFSET);
    tmpVal = reg_set_bit(tmpVal, EMAC_RSTAT);
    writel(tmpVal, EMACx + EMAC_MIICOMMAND_OFFSET);

    DRV_DUMMY_ACT;

    do {
        tmpVal = readl(EMACx + EMAC_MIISTATUS_OFFSET);
        arch_delay_us(16);
    } while (reg_is_bit_set(tmpVal, EMAC_MIIM_BUSY));

    *regValue = readl(EMACx + EMAC_MIIRX_DATA_OFFSET);

    return SUCCESS;
}

int EMAC_Phy_Write(EMAC_ID_Type emacId, uint16_t phyReg, uint16_t regValue)
{
    uint32_t tmpVal;
    uint32_t EMACx = emacAddr[emacId];

    /* Set Register Address */
    tmpVal = readl(EMACx + EMAC_MIIADDRESS_OFFSET);
    tmpVal = reg_set_bits(tmpVal, EMAC_RGAD, phyReg);
    writel(tmpVal, EMACx + EMAC_MIIADDRESS_OFFSET);

    /* Set Write data */
    writel(regValue, EMACx + EMAC_MIITX_DATA_OFFSET);

    /* Trigger write */
    tmpVal = readl(EMACx + EMAC_MIICOMMAND_OFFSET);
    tmpVal = reg_set_bit(tmpVal, EMAC_WCTRLDATA);
    writel(tmpVal, EMACx + EMAC_MIICOMMAND_OFFSET);

    DRV_DUMMY_ACT;

    do {
        tmpVal = readl(EMACx + EMAC_MIISTATUS_OFFSET);
    } while (reg_is_bit_set(tmpVal, EMAC_MIIM_BUSY));

    return SUCCESS;
}

int EMAC_Init(EMAC_ID_Type emacId, EMAC_CFG_Type *cfg)
{
    uint32_t tmpVal;
    uint32_t EMACx = emacAddr[emacId];

    /* Set MAC config */
    tmpVal = readl(EMACx + EMAC_MODE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, EMAC_RMII_EN, cfg->speed_mode);
    tmpVal = reg_set_bits(tmpVal, EMAC_RECSMALL, cfg->recvSmallFrame);
    tmpVal = reg_set_bits(tmpVal, EMAC_PAD, cfg->padEnable);
    tmpVal = reg_set_bits(tmpVal, EMAC_HUGEN, cfg->recvHugeFrame);
    tmpVal = reg_set_bits(tmpVal, EMAC_CRCEN, cfg->crcEnable);
    tmpVal = reg_set_bits(tmpVal, EMAC_NOPRE, cfg->noPreamble);
    tmpVal = reg_set_bits(tmpVal, EMAC_BRO, cfg->recvBroadCast);
    tmpVal = reg_set_bits(tmpVal, EMAC_PRO, ENABLE);
    //tmpVal |= (1 << 7); /* local loopback in emac */
    tmpVal = reg_set_bits(tmpVal, EMAC_IFG, cfg->interFrameGapCheck);
    writel(tmpVal, EMACx + EMAC_MODE_OFFSET);

    /* Set inter frame gap value */
    writel(cfg->interFrameGapValue, EMACx + EMAC_IPGT_OFFSET);

    /* Set MII interface */
    tmpVal = readl(EMACx + EMAC_MIIMODE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, EMAC_MIINOPRE, cfg->miiNoPreamble);
    tmpVal = reg_set_bits(tmpVal, EMAC_CLKDIV, cfg->miiClkDiv);
    writel(tmpVal, EMACx + EMAC_MIIMODE_OFFSET);

    /* Set collision */
    tmpVal = readl(EMACx + EMAC_COLLCONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, EMAC_MAXRET, cfg->maxTxRetry);
    tmpVal = reg_set_bits(tmpVal, EMAC_COLLVALID, cfg->collisionValid);
    writel(tmpVal, EMACx + EMAC_COLLCONFIG_OFFSET);

    /* Set frame length */
    tmpVal = readl(EMACx + EMAC_PACKETLEN_OFFSET);
    tmpVal = reg_set_bits(tmpVal, EMAC_MINFL, cfg->minFrameLen);
    tmpVal = reg_set_bits(tmpVal, EMAC_MAXFL, cfg->maxFrameLen);
    writel(tmpVal, EMACx + EMAC_PACKETLEN_OFFSET);

    EMAC_SetMACAddress(emacId, cfg->macAddr);

    return SUCCESS;
}

int EMAC_DeInit(EMAC_ID_Type emacId)
{
    EMAC_Disable(emacId);

    return SUCCESS;
}

int EMAC_DMADescListInit(EMAC_ID_Type emacId, EMAC_Handle_Type *handle, uint8_t *txBuff, uint32_t txBuffCount, uint8_t *rxBuff, uint32_t rxBuffCount)
{
    uint32_t i = 0;
    uint32_t EMACx = emacAddr[emacId];

    /* Set the Ethernet handler env */
    handle->bd = (EMAC_BD_Desc_Type *)(uintptr_t)(EMACx + EMAC_DMA_DESC_OFFSET);
    handle->txIndexEMAC = 0;
    handle->txIndexCPU = 0;
    handle->txBuffLimit = txBuffCount - 1;
    /* The receive descriptors' address starts right after the last transmit BD. */
    handle->rxIndexEMAC = txBuffCount;
    handle->rxIndexCPU = txBuffCount;
    handle->rxBuffLimit = txBuffCount + rxBuffCount - 1;

    /* Fill each DMARxDesc descriptor with the right values */
    for (i = 0; i < txBuffCount; i++) {
        /* Get the pointer on the ith member of the Tx Desc list */
        handle->bd[i].Buffer = (NULL == txBuff) ? 0 : (uint32_t)(uintptr_t)(txBuff + (ETH_MAX_PACKET_SIZE * i));
        handle->bd[i].C_S_L = 0;
    }

    /* For the last TX DMA Descriptor, it should be wrap back */
    handle->bd[handle->txBuffLimit].C_S_L |= EMAC_BD_FIELD_MSK(TX_WR);

    for (i = txBuffCount; i < (txBuffCount + rxBuffCount); i++) {
        /* Get the pointer on the ith member of the Rx Desc list */
        handle->bd[i].Buffer = (NULL == rxBuff) ? 0 : (uint32_t)(uintptr_t)(rxBuff + (ETH_MAX_PACKET_SIZE * (i - txBuffCount)));
        handle->bd[i].C_S_L = (ETH_MAX_PACKET_SIZE << 16) |
                              EMAC_BD_FIELD_MSK(RX_IRQ) |
                              EMAC_BD_FIELD_MSK(RX_E);
    }

    /* For the last RX DMA Descriptor, it should be wrap back */
    handle->bd[handle->rxBuffLimit].C_S_L |= EMAC_BD_FIELD_MSK(RX_WR);

    /* For the TX DMA Descriptor, it will wrap to 0 according to EMAC_TX_BD_NUM*/
    writel(txBuffCount, EMACx + EMAC_TX_BD_NUM_OFFSET);

    return SUCCESS;
}

int EMAC_DMATxDescGet(EMAC_Handle_Type *handle, EMAC_BD_Desc_Type **txDMADesc)
{
    return SUCCESS;
}

int EMAC_StartTx(EMAC_Handle_Type *handle, EMAC_BD_Desc_Type *txDMADesc, uint32_t len)
{
    return SUCCESS;
}

int EMAC_Enable(EMAC_ID_Type emacId)
{
    uint32_t tmpVal;
    uint32_t EMACx = emacAddr[emacId];

    /* Enable EMAC */
    tmpVal = readl(EMACx + EMAC_MODE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, EMAC_TXEN);
    tmpVal = reg_set_bit(tmpVal, EMAC_RXEN);
    writel(tmpVal, EMACx + EMAC_MODE_OFFSET);

    return SUCCESS;
}

int EMAC_Enable_TX(EMAC_ID_Type emacId)
{
    uint32_t tmpVal;
    uint32_t EMACx = emacAddr[emacId];

    /* Enable EMAC */
    tmpVal = readl(EMACx + EMAC_MODE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, EMAC_TXEN);
    writel(tmpVal, EMACx + EMAC_MODE_OFFSET);

    return SUCCESS;
}

int EMAC_Disable_TX(EMAC_ID_Type emacId)
{
    uint32_t tmpVal;
    uint32_t EMACx = emacAddr[emacId];

    /* Enable EMAC */
    tmpVal = readl(EMACx + EMAC_MODE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, EMAC_TXEN);
    writel(tmpVal, EMACx + EMAC_MODE_OFFSET);

    return SUCCESS;
}

int EMAC_Enable_RX(EMAC_ID_Type emacId)
{
    uint32_t tmpval;
    uint32_t EMACx = emacAddr[emacId];

    /* Enable EMAC TX*/
    tmpval = readl(EMACx + EMAC_MODE_OFFSET);
    tmpval = reg_set_bit(tmpval, EMAC_RXEN);
    writel(tmpval, EMACx + EMAC_MODE_OFFSET);

    return SUCCESS;
}

int EMAC_Disable_RX(EMAC_ID_Type emacId)
{
    uint32_t tmpval;
    uint32_t EMACx = emacAddr[emacId];

    /* Disable EMAC RX*/
    tmpval = readl(EMACx + EMAC_MODE_OFFSET);
    tmpval = reg_clr_bit(tmpval, EMAC_RXEN);
    writel(tmpval, EMACx + EMAC_MODE_OFFSET);

    return SUCCESS;
}

int EMAC_Disable(EMAC_ID_Type emacId)
{
    uint32_t tmpVal;
    uint32_t EMACx = emacAddr[emacId];

    /* Enable EMAC */
    tmpVal = readl(EMACx + EMAC_MODE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, EMAC_TXEN);
    tmpVal = reg_clr_bit(tmpVal, EMAC_RXEN);
    writel(tmpVal, EMACx + EMAC_MODE_OFFSET);

    return SUCCESS;
}

int EMAC_IntMask(EMAC_ID_Type emacId, EMAC_INT_Type intType, int intMask)
{
    uint32_t tmpVal;
    uint32_t EMACx = emacAddr[emacId];

    /* Check the parameters */

    tmpVal = readl(EMACx + EMAC_INT_MASK_OFFSET);

    /* Mask or unmask certain or all interrupt */
    if (MASK == intMask) {
        tmpVal |= intType;
    } else {
        tmpVal &= (~intType);
    }

    /* Write back */
    writel(tmpVal, EMACx + EMAC_INT_MASK_OFFSET);

    return SUCCESS;
}

int EMAC_GetIntStatus(EMAC_ID_Type emacId, EMAC_INT_Type intType)
{
    uint32_t tmpVal;
    uint32_t EMACx = emacAddr[emacId];

    /* Check the parameters */

    tmpVal = readl(EMACx + EMAC_INT_SOURCE_OFFSET);

    return (tmpVal & intType) ? SET : RESET;
}

int EMAC_ClrIntStatus(EMAC_ID_Type emacId, EMAC_INT_Type intType)
{
    uint32_t tmpVal;
    uint32_t EMACx = emacAddr[emacId];

    /* Check the parameters */

    tmpVal = readl(EMACx + EMAC_INT_SOURCE_OFFSET);

    writel(tmpVal | intType, EMACx + EMAC_INT_SOURCE_OFFSET);

    return SUCCESS;
}

int EMAC_TxPauseReq(EMAC_ID_Type emacId, uint16_t timeCount)
{
    uint32_t EMACx = emacAddr[emacId];

    writel((1 << 16) | timeCount, EMACx + EMAC_TXCTRL_OFFSET);

    return SUCCESS;
}

int EMAC_SetHash(EMAC_ID_Type emacId, uint32_t hash0, uint32_t hash1)
{
    uint32_t EMACx = emacAddr[emacId];

    writel(hash0, EMACx + EMAC_HASH0_ADDR_OFFSET);

    writel(hash1, EMACx + EMAC_HASH1_ADDR_OFFSET);

    return SUCCESS;
}

int EMAC_GetFramLen(EMAC_ID_Type emacId, uint16_t * max, uint16_t *min)
{
    uint32_t EMACx = emacAddr[emacId];
    uint32_t tmpval;

    tmpval = readl(EMACx + EMAC_PACKETLEN_OFFSET);
    *max = reg_get_bits(tmpval, EMAC_MAXFL);
    *min = reg_get_bits(tmpval, EMAC_MINFL);

    return SUCCESS;
}

int EMAC_GetBD(EMAC_ID_Type emacId, uint32_t *bd)
{
    uint32_t EMACx = emacAddr[emacId];

    *bd = readl(EMACx + EMAC_TX_BD_NUM_OFFSET);

    return SUCCESS;
}


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



#include "drv_i2c.h"
#include "drv_glb.h"

#define I2C_FIFO_STATUS_TIMEOUT (160 * 1000 * 2)
#define PUT_UINT32_LE(n, b, i)               \
    {                                        \
        (b)[(i)] = (uint8_t)((n));           \
        (b)[(i) + 1] = (uint8_t)((n) >> 8);  \
        (b)[(i) + 2] = (uint8_t)((n) >> 16); \
        (b)[(i) + 3] = (uint8_t)((n) >> 24); \
    }

static const uint32_t i2cAddr[I2C_ID_MAX] = { I2C0_BASE, I2C1_BASE };
#ifndef IPRO_USE_HAL_DRIVER
intCallback_Type *i2cIntCbfArra[I2C_ID_MAX][I2C_INT_ALL] = { { NULL } };
#endif

#ifndef IPRO_USE_HAL_DRIVER
static void I2C_IntHandler(I2C_ID_Type i2cNo)
{
    uint32_t tmpVal;
    uint32_t I2Cx = i2cAddr[i2cNo];

    /* Check the parameters */

    tmpVal = readl(I2Cx + I2C_INT_STS_OFFSET);

    if (reg_is_bit_set(tmpVal, I2C_END_INT)) {
        if (i2cIntCbfArra[i2cNo][I2C_TRANS_END_INT] != NULL) {
            /* Call the callback function */
            i2cIntCbfArra[i2cNo][I2C_TRANS_END_INT]();
        }
    }

    if (reg_is_bit_set(tmpVal, I2C_TXF_INT)) {
        if (i2cIntCbfArra[i2cNo][I2C_TX_FIFO_READY_INT] != NULL) {
            /* Call the callback function */
            i2cIntCbfArra[i2cNo][I2C_TX_FIFO_READY_INT]();
        }
    }

    if (reg_is_bit_set(tmpVal, I2C_RXF_INT)) {
        if (i2cIntCbfArra[i2cNo][I2C_RX_FIFO_READY_INT] != NULL) {
            /* Call the callback function */
            i2cIntCbfArra[i2cNo][I2C_RX_FIFO_READY_INT]();
        }
    }

    if (reg_is_bit_set(tmpVal, I2C_NAK_INT)) {
        if (i2cIntCbfArra[i2cNo][I2C_NACK_RECV_INT] != NULL) {
            /* Call the callback function */
            i2cIntCbfArra[i2cNo][I2C_NACK_RECV_INT]();
        }
    }

    if (reg_is_bit_set(tmpVal, I2C_ARB_INT)) {
        if (i2cIntCbfArra[i2cNo][I2C_ARB_LOST_INT] != NULL) {
            /* Call the callback function */
            i2cIntCbfArra[i2cNo][I2C_ARB_LOST_INT]();
        }
    }

    if (reg_is_bit_set(tmpVal, I2C_FER_INT)) {
        if (i2cIntCbfArra[i2cNo][I2C_FIFO_ERR_INT] != NULL) {
            /* Call the callback function */
            i2cIntCbfArra[i2cNo][I2C_FIFO_ERR_INT]();
        }
    }
}
#endif

int I2C_SendWord(I2C_ID_Type i2cNo, uint32_t data)
{
    uint32_t I2Cx = i2cAddr[i2cNo];
    uint32_t timeOut = 0;

    /* Check the parameters */

    timeOut = I2C_FIFO_STATUS_TIMEOUT;

    while (reg_get_bits(readl(I2Cx + I2C_FIFO_CONFIG_1_OFFSET), I2C_TX_FIFO_CNT) == 0) {
        timeOut--;
        if (timeOut == 0) {
            return TIMEOUT;
        }
    }

    writel(data, I2Cx + I2C_FIFO_WDATA_OFFSET);

    return SUCCESS;
}

int I2C_RecieveWord(I2C_ID_Type i2cNo, uint32_t *recvBuff)
{
    uint32_t I2Cx = i2cAddr[i2cNo];
    uint32_t timeOut = 0;

    /* Check the parameters */

    timeOut = I2C_FIFO_STATUS_TIMEOUT;

    while (reg_get_bits(readl(I2Cx + I2C_FIFO_CONFIG_1_OFFSET), I2C_RX_FIFO_CNT) == 0) {
        timeOut--;

        if (timeOut == 0) {
            return TIMEOUT;
        }
    }

    *recvBuff = readl(I2Cx + I2C_FIFO_RDATA_OFFSET);

    return SUCCESS;
}

void I2C_Enable(I2C_ID_Type i2cNo)
{
    uint32_t tmpVal;
    uint32_t I2Cx = i2cAddr[i2cNo];

    /* Check the parameters */

    tmpVal = readl(I2Cx + I2C_CONFIG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_M_EN);
    writel(tmpVal, I2Cx + I2C_CONFIG_OFFSET);
}

void I2C_Disable(I2C_ID_Type i2cNo)
{
    uint32_t tmpVal;
    uint32_t I2Cx = i2cAddr[i2cNo];

    /* Check the parameters */

    tmpVal = readl(I2Cx + I2C_CONFIG_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_M_EN);
    writel(tmpVal, I2Cx + I2C_CONFIG_OFFSET);

    /* Clear I2C fifo */
    tmpVal = readl(I2Cx + I2C_FIFO_CONFIG_0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, I2C_TX_FIFO_CLR);
    tmpVal = reg_set_bit(tmpVal, I2C_RX_FIFO_CLR);
    writel(tmpVal, I2Cx + I2C_FIFO_CONFIG_0_OFFSET);

    /* Clear I2C interrupt status */
    tmpVal = readl(I2Cx + I2C_INT_STS_OFFSET);
    tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_END_CLR);
    tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_NAK_CLR);
    tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_ARB_CLR);
    writel(tmpVal, I2Cx + I2C_INT_STS_OFFSET);
}

int I2C_DeInit(I2C_ID_Type i2cNo)
{
    /* Check the parameters */

    if (i2cNo == I2C0_ID) {
        GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_I2C0);
    } else if (i2cNo == I2C1_ID) {
        GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_I2C1);
    }

    return SUCCESS;
}

void I2C_Init(I2C_ID_Type i2cNo, I2C_Direction_Type direct, I2C_Transfer_Cfg *cfg)
{
    uint32_t tmpVal;
    uint32_t I2Cx = i2cAddr[i2cNo];

    /* Check the parameters */

    // I2C_ClockSet(i2cNo, cfg->clk);

    /* Disable clock gate when use I2C0 */
    if (I2C0_ID == i2cNo) {
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_I2C);
    } else if (I2C1_ID == i2cNo) {
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_I2C1);
    }

    if (cfg->dataSize > 256) {
        cfg->dataSize = 256;
    }

    /* I2C write config */
    tmpVal = readl(I2Cx + I2C_CONFIG_OFFSET);

    if (direct == I2C_WRITE) {
        tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_PKT_DIR);
    } else {
        tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_PKT_DIR);
    }

    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_SLV_ADDR, cfg->slaveAddr);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_10B_ADDR_EN, cfg->slaveAddr10Bit);

    if (cfg->subAddrSize > 0) {
        tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_SUB_ADDR_EN);
        tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_SUB_ADDR_BC, cfg->subAddrSize - 1);
    } else {
        tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_SUB_ADDR_EN);
    }

    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PKT_LEN, cfg->dataSize - 1);
    writel(tmpVal, I2Cx + I2C_CONFIG_OFFSET);

    /* Set sub address */
    writel(cfg->subAddr, I2Cx + I2C_SUB_ADDR_OFFSET);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(I2C0_IRQn, I2C0_IRQHandler);
    Interrupt_Handler_Register(I2C1_IRQn, I2C1_IRQHandler);
#endif
}

int I2C_SetDeglitchCount(I2C_ID_Type i2cNo, uint8_t cnt)
{
    uint32_t tmpVal;
    uint32_t I2Cx = i2cAddr[i2cNo];

    /* Check the parameters */

    tmpVal = readl(I2Cx + I2C_CONFIG_OFFSET);

    if (cnt > 0) {
        /* enable de-glitch function */
        tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_DEG_EN);
        /* Set count value */
        tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_DEG_CNT, cnt - 1);
    } else {
        /* disable de-glitch function */
        tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_DEG_EN);
        /* Set count value as 0 */
        tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_DEG_CNT, 0);
    }

    writel(tmpVal, I2Cx + I2C_CONFIG_OFFSET);

    return SUCCESS;
}

void I2C_SetPrd(I2C_ID_Type i2cNo, uint8_t phase)
{
    uint32_t tmpVal;
    uint32_t I2Cx = i2cAddr[i2cNo];

    /* Check the parameters */

    //phase_cycles = (32000000 / phase / 4) - 1;
    tmpVal = readl(I2Cx + I2C_PRD_START_OFFSET);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_S_PH_0, phase);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_S_PH_1, phase);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_S_PH_2, phase);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_S_PH_3, phase);
    writel(tmpVal, I2Cx + I2C_PRD_START_OFFSET);
    tmpVal = readl(I2Cx + I2C_PRD_STOP_OFFSET);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_P_PH_0, phase);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_P_PH_1, phase);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_P_PH_2, phase);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_P_PH_3, phase);
    writel(tmpVal, I2Cx + I2C_PRD_STOP_OFFSET);
    tmpVal = readl(I2Cx + I2C_PRD_DATA_OFFSET);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_D_PH_0, phase);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_D_PH_1, phase);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_D_PH_2, phase);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_D_PH_3, phase);
    writel(tmpVal, I2Cx + I2C_PRD_DATA_OFFSET);
}

void I2C_SetTimingPhase(I2C_ID_Type i2cNo, I2C_Timing_Phase_Type *timing)
{
    uint32_t tmpVal;
    uint32_t I2Cx = i2cAddr[i2cNo];

    /* Check the parameters */

    tmpVal = readl(I2Cx + I2C_PRD_START_OFFSET);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_S_PH_0, timing->start_phase0);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_S_PH_1, timing->start_phase1);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_S_PH_2, timing->start_phase2);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_S_PH_3, timing->start_phase3);
    writel(tmpVal, I2Cx + I2C_PRD_START_OFFSET);
    tmpVal = readl(I2Cx + I2C_PRD_STOP_OFFSET);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_P_PH_0, timing->stop_phase0);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_P_PH_1, timing->stop_phase1);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_P_PH_2, timing->stop_phase2);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_P_PH_3, timing->stop_phase3);
    writel(tmpVal, I2Cx + I2C_PRD_STOP_OFFSET);
    tmpVal = readl(I2Cx + I2C_PRD_DATA_OFFSET);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_D_PH_0, timing->data_phase0);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_D_PH_1, timing->data_phase1);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_D_PH_2, timing->data_phase2);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_D_PH_3, timing->data_phase3);
    writel(tmpVal, I2Cx + I2C_PRD_DATA_OFFSET);
}

void I2C_GetTimingPhase(I2C_ID_Type i2cNo, I2C_Timing_Phase_Type *timing)
{
    uint32_t tmpVal;
    uint32_t I2Cx = i2cAddr[i2cNo];

    /* Check the parameters */

    tmpVal = readl(I2Cx + I2C_PRD_START_OFFSET);
    timing->start_phase0 = reg_get_bits(tmpVal, I2C_CR_I2C_PRD_S_PH_0);
    timing->start_phase1 = reg_get_bits(tmpVal, I2C_CR_I2C_PRD_S_PH_1);
    timing->start_phase2 = reg_get_bits(tmpVal, I2C_CR_I2C_PRD_S_PH_2);
    timing->start_phase3 = reg_get_bits(tmpVal, I2C_CR_I2C_PRD_S_PH_3);
    tmpVal = readl(I2Cx + I2C_PRD_STOP_OFFSET);
    timing->stop_phase0 = reg_get_bits(tmpVal, I2C_CR_I2C_PRD_P_PH_0);
    timing->stop_phase1 = reg_get_bits(tmpVal, I2C_CR_I2C_PRD_P_PH_1);
    timing->stop_phase2 = reg_get_bits(tmpVal, I2C_CR_I2C_PRD_P_PH_2);
    timing->stop_phase3 = reg_get_bits(tmpVal, I2C_CR_I2C_PRD_P_PH_3);
    tmpVal = readl(I2Cx + I2C_PRD_DATA_OFFSET);
    timing->data_phase0 = reg_get_bits(tmpVal, I2C_CR_I2C_PRD_D_PH_0);
    timing->data_phase1 = reg_get_bits(tmpVal, I2C_CR_I2C_PRD_D_PH_1);
    timing->data_phase2 = reg_get_bits(tmpVal, I2C_CR_I2C_PRD_D_PH_2);
    timing->data_phase3 = reg_get_bits(tmpVal, I2C_CR_I2C_PRD_D_PH_3);
}

void I2C_ClockSet(I2C_ID_Type i2cNo, uint32_t clk)
{
    uint32_t tmpVal;
    uint32_t I2Cx = i2cAddr[i2cNo];
    int32_t phase, phase0, phase1, phase2, phase3;
    int32_t bias;

    /* Check the parameters */

    if (i2cNo == I2C0_ID) {
        phase = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_I2C0);
    } else if (i2cNo == I2C1_ID) {
        phase = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_I2C1);
    } else {
        return;
    }
    phase = (phase + clk / 2) / clk;
    if (clk <= 100 * 1000) {
        /* when SCL clock <= 100KHz, duty cycle is default 50%  */
        phase0 = (phase + 2) / 4;
        phase1 = phase0;
        phase2 = phase / 2 - phase0;
        phase3 = phase - phase0 - phase1 - phase2;
    } else {
        /* when SCL clock > 100KHz, duty cycle isdefault 33% */
        phase0 = (phase + 2) / 3;
        phase1 = (phase + 3) / 6;
        phase2 = (phase + 1) / 3 - phase1;
        phase3 = phase - phase0 - phase1 - phase2;
    }

    /* calculate rectify phase when de-glitch or clock-stretching is enabled */
    tmpVal = readl(I2Cx + I2C_CONFIG_OFFSET);
    if (reg_is_bit_set(tmpVal, I2C_CR_I2C_DEG_EN) && (reg_is_bit_set(tmpVal, I2C_CR_I2C_SCL_SYNC_EN))) {
        bias = reg_get_bits(tmpVal, I2C_CR_I2C_DEG_CNT);
        bias += 1;
    } else {
        bias = 0;
    }
    if (reg_is_bit_set(tmpVal, I2C_CR_I2C_SCL_SYNC_EN)) {
        bias += 3;
    }

    /* value should decrement one before write to register */
    phase0 = (phase0 <= 0) ? 1 : phase0;
    phase1 = (phase1 <= bias) ? (bias + 1) : phase1;
    phase2 = (phase2 <= 0) ? 1 : phase2;
    phase3 = (phase3 <= 0) ? 1 : phase3;
    /* only 8bit is available for phase0~3 in register */
    phase0 = (phase0 >= 256) ? 256 : phase0;
    phase1 = (phase1 >= 256) ? 256 : phase1;
    phase2 = (phase2 >= 256) ? 256 : phase2;
    phase3 = (phase0 >= 256) ? 256 : phase3;

    /* calculate data phase */
    tmpVal = readl(I2Cx + I2C_PRD_DATA_OFFSET);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_D_PH_0, phase0 - 1);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_D_PH_1, ((phase1 - bias - 1) <= 0) ? 1 : (phase1 - bias - 1));
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_D_PH_2, phase2 - 1);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_D_PH_3, phase3 - 1);
    writel(tmpVal, I2Cx + I2C_PRD_DATA_OFFSET);
    /* calculate start phase */
    tmpVal = readl(I2Cx + I2C_PRD_START_OFFSET);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_S_PH_0, phase0 - 1);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_S_PH_1, ((phase0 + phase3 - 1) >= 256) ? 255 : (phase0 + phase3 - 1));
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_S_PH_2, ((phase1 + phase2 - 1) >= 256) ? 255 : (phase1 + phase2 - 1));
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_S_PH_3, phase3 - 1);
    writel(tmpVal, I2Cx + I2C_PRD_START_OFFSET);
    /* calculate stop phase */
    tmpVal = readl(I2Cx + I2C_PRD_STOP_OFFSET);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_P_PH_0, phase0 - 1);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_P_PH_1, ((phase1 + phase2 - 1) >= 256) ? 255 : (phase1 + phase2 - 1));
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_P_PH_2, phase0 - 1);
    tmpVal = reg_set_bits(tmpVal, I2C_CR_I2C_PRD_P_PH_3, phase3 - 1);
    writel(tmpVal, I2Cx + I2C_PRD_STOP_OFFSET);
}

void I2C_SetSclSync(I2C_ID_Type i2cNo, uint8_t enable)
{
    uint32_t tmpVal;
    uint32_t I2Cx = i2cAddr[i2cNo];

    /* Check the parameters */

    tmpVal = readl(I2Cx + I2C_CONFIG_OFFSET);

    if (enable) {
        tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_SCL_SYNC_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_SCL_SYNC_EN);
    }

    writel(tmpVal, I2Cx + I2C_CONFIG_OFFSET);
}

int I2C_IsBusy(I2C_ID_Type i2cNo)
{
    uint32_t tmpVal;
    uint32_t I2Cx = i2cAddr[i2cNo];

    /* Check the parameters */

    tmpVal = readl(I2Cx + I2C_BUS_BUSY_OFFSET);
    return ((reg_is_bit_set(tmpVal, I2C_STS_I2C_BUS_BUSY)) ? SET : RESET);
}

int I2C_TransferEndStatus(I2C_ID_Type i2cNo)
{
    uint32_t tmpVal;
    uint32_t I2Cx = i2cAddr[i2cNo];

    /* Check the parameters */

    tmpVal = readl(I2Cx + I2C_INT_STS_OFFSET);
    return ((reg_is_bit_set(tmpVal, I2C_END_INT)) ? SET : RESET);
}

int I2C_TransferNackStatus(I2C_ID_Type i2cNo)
{
    uint32_t tmpVal;
    uint32_t I2Cx = i2cAddr[i2cNo];

    /* Check the parameters */

    tmpVal = readl(I2Cx + I2C_INT_STS_OFFSET);
    return ((reg_is_bit_set(tmpVal, I2C_NAK_INT)) ? SET : RESET);
}

int I2C_MasterSendBlocking(I2C_ID_Type i2cNo, I2C_Transfer_Cfg *cfg)
{
    uint16_t i;
    uint32_t timeOut = 0;
    uint32_t temp = 0;
    uint32_t I2Cx = i2cAddr[i2cNo];

    /* Check the parameters */

    I2C_Disable(i2cNo);
    I2C_Init(i2cNo, I2C_WRITE, cfg);

    /* Set I2C write data */
    for (i = 0; i < cfg->dataSize; i++) {
        temp += (cfg->data[i] << ((i % 4) * 8));

        if ((i + 1) % 4 == 0) {
            timeOut = I2C_FIFO_STATUS_TIMEOUT;

            while (reg_get_bits(readl(I2Cx + I2C_FIFO_CONFIG_1_OFFSET), I2C_TX_FIFO_CNT) == 0) {
                timeOut--;

                if (timeOut == 0) {
                    I2C_Disable(i2cNo);
                    return TIMEOUT;
                }
            }

            writel(temp, I2Cx + I2C_FIFO_WDATA_OFFSET);
            if (reg_get_bits(readl(I2Cx + I2C_CONFIG_OFFSET), I2C_CR_I2C_M_EN) == 0) {
                I2C_Enable(i2cNo);
            }
            temp = 0;
        }
    }

    if ((cfg->dataSize % 4) != 0) {
        timeOut = I2C_FIFO_STATUS_TIMEOUT;

        while (reg_get_bits(readl(I2Cx + I2C_FIFO_CONFIG_1_OFFSET), I2C_TX_FIFO_CNT) == 0) {
            timeOut--;

            if (timeOut == 0) {
                I2C_Disable(i2cNo);
                return TIMEOUT;
            }
        }

        writel(temp, I2Cx + I2C_FIFO_WDATA_OFFSET);
        if (reg_get_bits(readl(I2Cx + I2C_CONFIG_OFFSET), I2C_CR_I2C_M_EN) == 0) {
            I2C_Enable(i2cNo);
        }
    }

    timeOut = I2C_FIFO_STATUS_TIMEOUT;

    while (I2C_IsBusy(i2cNo) || !I2C_TransferEndStatus(i2cNo) || I2C_TransferNackStatus(i2cNo)) {
        timeOut--;

        if (timeOut == 0) {
            I2C_Disable(i2cNo);
            return TIMEOUT;
        }
    }

    I2C_Disable(i2cNo);
    return SUCCESS;
}

int I2C_MasterReceiveBlocking(I2C_ID_Type i2cNo, I2C_Transfer_Cfg *cfg)
{
    uint16_t i = 0;
    uint32_t timeOut = 0;
    uint32_t temp = 0;
    uint32_t I2Cx = i2cAddr[i2cNo];

    /* Check the parameters */

    I2C_Disable(i2cNo);
    I2C_Init(i2cNo, I2C_READ, cfg);
    I2C_Enable(i2cNo);

    /* Read I2C data */
    while (cfg->dataSize - i >= 4) {
        timeOut = I2C_FIFO_STATUS_TIMEOUT;

        while (reg_get_bits(readl(I2Cx + I2C_FIFO_CONFIG_1_OFFSET), I2C_RX_FIFO_CNT) == 0) {
            timeOut--;

            if (timeOut == 0) {
                I2C_Disable(i2cNo);
                return TIMEOUT;
            }
        }

        temp = readl(I2Cx + I2C_FIFO_RDATA_OFFSET);
        PUT_UINT32_LE(temp, cfg->data, i);
        i += 4;
    }

    if (i < cfg->dataSize) {
        timeOut = I2C_FIFO_STATUS_TIMEOUT;

        while (reg_get_bits(readl(I2Cx + I2C_FIFO_CONFIG_1_OFFSET), I2C_RX_FIFO_CNT) == 0) {
            timeOut--;

            if (timeOut == 0) {
                I2C_Disable(i2cNo);
                return TIMEOUT;
            }
        }

        temp = readl(I2Cx + I2C_FIFO_RDATA_OFFSET);

        while (i < cfg->dataSize) {
            cfg->data[i] = (temp & 0xff);
            temp = (temp >> 8);
            i++;
        }
    }

    timeOut = I2C_FIFO_STATUS_TIMEOUT;

    while (I2C_IsBusy(i2cNo) || !I2C_TransferEndStatus(i2cNo)) {
        timeOut--;

        if (timeOut == 0) {
            I2C_Disable(i2cNo);
            return TIMEOUT;
        }
    }

    I2C_Disable(i2cNo);
    return SUCCESS;
}

void I2C_IntMask(I2C_ID_Type i2cNo, I2C_INT_Type intType, int intMask)
{
    uint32_t tmpVal;
    uint32_t I2Cx = i2cAddr[i2cNo];

    /* Check the parameters */

    tmpVal = readl(I2Cx + I2C_INT_STS_OFFSET);

    switch (intType) {
        case I2C_TRANS_END_INT:
            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_END_EN);
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_END_MASK);
            } else {
                /* MASK(Disable) this interrupt */
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_END_EN);
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_END_MASK);
            }

            break;

        case I2C_TX_FIFO_READY_INT:
            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_TXF_EN);
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_TXF_MASK);
            } else {
                /* MASK(Disable) this interrupt */
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_TXF_EN);
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_TXF_MASK);
            }

            break;

        case I2C_RX_FIFO_READY_INT:
            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_RXF_EN);
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_RXF_MASK);
            } else {
                /* MASK(Disable) this interrupt */
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_RXF_EN);
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_RXF_MASK);
            }

            break;

        case I2C_NACK_RECV_INT:
            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_NAK_EN);
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_NAK_MASK);
            } else {
                /* MASK(Disable) this interrupt */
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_NAK_EN);
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_NAK_MASK);
            }

            break;

        case I2C_ARB_LOST_INT:
            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_ARB_EN);
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_ARB_MASK);
            } else {
                /* MASK(Disable) this interrupt */
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_ARB_EN);
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_ARB_MASK);
            }

            break;

        case I2C_FIFO_ERR_INT:
            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_FER_EN);
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_FER_MASK);
            } else {
                /* MASK(Disable) this interrupt */
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_FER_EN);
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_FER_MASK);
            }

            break;

        case I2C_INT_ALL:
            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_END_EN);
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_TXF_EN);
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_RXF_EN);
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_NAK_EN);
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_ARB_EN);
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_FER_EN);
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_END_MASK);
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_TXF_MASK);
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_RXF_MASK);
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_NAK_MASK);
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_ARB_MASK);
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_FER_MASK);
            } else {
                /* MASK(Disable) this interrupt */
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_END_EN);
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_TXF_EN);
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_RXF_EN);
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_NAK_EN);
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_ARB_EN);
                tmpVal = reg_clr_bit(tmpVal, I2C_CR_I2C_FER_EN);
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_END_MASK);
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_TXF_MASK);
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_RXF_MASK);
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_NAK_MASK);
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_ARB_MASK);
                tmpVal = reg_set_bit(tmpVal, I2C_CR_I2C_FER_MASK);
            }

            break;

        default:
            break;
    }

    writel(tmpVal, I2Cx + I2C_INT_STS_OFFSET);
}

#ifndef IPRO_USE_HAL_DRIVER
void I2C_Int_Callback_Install(I2C_ID_Type i2cNo, I2C_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */

    i2cIntCbfArra[i2cNo][intType] = cbFun;
}
#endif

#ifndef IPRO_USE_HAL_DRIVER
void I2C0_IRQHandler(void)
{
    I2C_IntHandler(I2C0_ID);
}
#endif

#ifndef IPRO_USE_HAL_DRIVER
void I2C1_IRQHandler(void)
{
    I2C_IntHandler(I2C1_ID);
}
#endif


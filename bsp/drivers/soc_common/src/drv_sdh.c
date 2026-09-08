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

#include <stdio.h>
#include <string.h>
#include <drv_sdh.h>
#include "drv_common.h"   /* arch_delay_ms + per-SoC core timer (SysTimer/MTimer) */
#include "drv_clock.h"    /* CPU_Get_MTimer_Counter (IPRO6LE) */
#include "system_soc.h"   /* L1C_DCACHE_CLEAN_RANGE */

/* Driver-level millisecond tick for bounded SDH DMA polling. Built directly on
 * the per-SoC driver timer (the same primitives hal_mtimer composes) so this
 * driver does NOT depend on the hal layer — drv_* sits below hal_*, not above.
 * us>>10 ~= ms (2.4% fast, fine for a multi-second poll cap). */
static inline uint32_t sdh_now_ms(void)
{
#if defined(CONFIG_IPRO6LE)
    return (uint32_t)(CPU_Get_MTimer_Counter() >> 10);
#elif defined(CONFIG_IPRO7)
    return (uint32_t)((SysTimer_GetLoadValue() >> 2) >> 10);
#elif defined(CONFIG_IPRO6)
    return (uint32_t)(SysTimer_GetLoadValue() >> 10);
#else
    #error "drv_sdh: unknown platform for the DMA poll timer"
#endif
}

/* Bound polling of the SDH DMA data phase so a non-responding card cannot
 * wedge the calling task forever. Matches the 5s budget already used by the
 * non-DMA PIO paths in hal_sdh.c (SDIO_DATATIMEOUT_MS). */
#define SDH_DATA_POLL_TIMEOUT_MS  5000U

/* Runtime override of the data-completion poll cap + a quiet flag. Diagnostic
 * paths (e.g. LA capture of the CMD53 write CRC-status window) lower the
 * timeout so each wedging write returns fast and can be fired in a tight loop,
 * and silence the per-timeout register dump so the loop doesn't flood UART.
 * Defaults preserve the original 5s verbose behaviour. */
uint32_t g_sdh_data_poll_timeout_ms = SDH_DATA_POLL_TIMEOUT_MS;
volatile int g_sdh_data_quiet_timeout = 0;

/* Optional hook for the DMA data-phase wait. A HAL that knows the calling
 * task overrides it to run that wait at a lower priority (the SDH interrupt
 * does not work on IPRO7, so it cannot be an ISR wait); the default says
 * "not available" and the poll below runs unchanged. 0 = a data-phase event
 * was seen (re-read the status), -1 = not available or timed out. */
__attribute__((weak)) int SDH_WaitDataHook(SD_ID_Type sdId, uint32_t timeout_ms)
{
    (void)sdId; (void)timeout_ms;
    return -1;
}
/* When set, clear BLK_CNT_EN for single (blockCount<=1) transfers (SD-host spec-correct,
 * A/B for the 4-bit single-block read false-CRC). Default 0 = legacy behaviour. */
int g_sdh_single_no_blkcnt = 0;
void sdh_set_single_no_blkcnt(int en) { g_sdh_single_no_blkcnt = en ? 1 : 0; }

static uint32_t bootDummy = 0;
static const uint32_t sdAddr[SD_ID_MAX] = {SDH_BASE};

void SDH_EnableDMA(SDH_DMA_Mode_Type dmaMode, SD_ID_Type sdId)
{
    uint16_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];


    /* Select DMA type */
    tmpVal = readw(SdBaseAddr + SDH_SD_HOST_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SDH_DMA_SEL, dmaMode);
    writew(tmpVal, SdBaseAddr + SDH_SD_HOST_CTRL_OFFSET);

    tmpVal = readw(SdBaseAddr + SDH_SD_TRANSFER_MODE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SDH_DMA_EN);
    writew(tmpVal, SdBaseAddr + SDH_SD_TRANSFER_MODE_OFFSET);
}

void SDH_DisableDMA(SD_ID_Type sdId)
{
    uint16_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];

    tmpVal = readw(SdBaseAddr + SDH_SD_TRANSFER_MODE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SDH_DMA_EN);
    writew(tmpVal, SdBaseAddr + SDH_SD_TRANSFER_MODE_OFFSET);
}

void SDH_EnableBootMode(SD_ID_Type sdId)
{
    uint16_t tmpVal;
    uint32_t SdBaseAddr = sdAddr[sdId];

    tmpVal = readw(SdBaseAddr + SDH_SD_TRANSFER_MODE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SDH_BOOT_OPREATION);
    writew(tmpVal, SdBaseAddr + SDH_SD_TRANSFER_MODE_OFFSET);
    printf("tmpVal = 0x%x\r\n", tmpVal);
}

void SDH_DisableBootMode(SD_ID_Type sdId)
{
    uint16_t tmpVal;
    uint32_t SdBaseAddr = sdAddr[sdId];

    tmpVal = readw(SdBaseAddr + SDH_SD_TRANSFER_MODE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SDH_BOOT_OPREATION);
    writew(tmpVal, SdBaseAddr + SDH_SD_TRANSFER_MODE_OFFSET);
}

void SDH_ITConfig(uint32_t SDH_IT_FLAG, int NewState)
{
    if (NewState != DISABLE) {
        /* Enable the SDH interrupts */
        SDH_EnableIntSource(SDH_IT_FLAG);
    } else {
        /* Disable the SDH interrupts */
        SDH_DisableIntSource(SDH_IT_FLAG);
    }
}

void SDH_EnableIntStatus(uint32_t mask, SD_ID_Type sdId)
{
	uint32_t SdBaseAddr = sdAddr[sdId];

    writel(readl(SdBaseAddr + SDH_SD_NORMAL_INT_STATUS_EN_OFFSET) | mask, SdBaseAddr + SDH_SD_NORMAL_INT_STATUS_EN_OFFSET);
}
void SDH_DisableIntStatus(uint32_t mask, SD_ID_Type sdId)
{
	uint32_t SdBaseAddr = sdAddr[sdId];

    writel(readl(SdBaseAddr + SDH_SD_NORMAL_INT_STATUS_EN_OFFSET) & (~(mask)), SdBaseAddr + SDH_SD_NORMAL_INT_STATUS_EN_OFFSET);
}

void SDH_EnableStatus(SD_ID_Type sdId)
{
    uint32_t tmpVal;

    /* Disable all interrupts */
    SDH_DisableIntStatus(SDH_INT_ALL, sdId);

    tmpVal = (SDH_INT_CMD_TIMEOUT | SDH_INT_CMD_CRC_ERROR | SDH_INT_CMD_ENDBIT_ERROR |
              SDH_INT_CMD_INDEX_ERROR | SDH_INT_CMD_COMPLETED |
              SDH_INT_CARD_INSERT | SDH_INT_CARD_REMOVE |
              SDH_INT_DATA_COMPLETED | SDH_INT_DMA_COMPLETED |
              SDH_INT_BOOT_ACK | SDH_INT_BOOT_DONE |
              SDH_INT_BUFFER_WRITE_READY | SDH_INT_BUFFER_READ_READY |
              SDH_INT_DATA_TIMEOUT | SDH_INT_DATA_CRC_ERROR | SDH_INT_DATA_ENDBIT_ERROR |
              SDH_INT_AUTO_CMD12_ERROR | SDH_INT_DMA_ERROR | SDH_INT_BLK_GAP_EVENT |
              SDH_INT_RETUNE_EVENT | SDH_INT_TUNE_ERROR);

    SDH_EnableIntStatus(tmpVal, sdId);
}

uint32_t SDH_GetIntEnableStatus(SD_ID_Type sdId)
{
	uint32_t SdBaseAddr = sdAddr[sdId];
    return readl(SdBaseAddr + SDH_SD_NORMAL_INT_STATUS_INT_EN_OFFSET);
}

uint32_t SDH_GetIntStatus(SD_ID_Type sdId)
{
	uint32_t SdBaseAddr = sdAddr[sdId];
    return readl(SdBaseAddr + SDH_SD_NORMAL_INT_STATUS_OFFSET);
}

void SDH_ClearIntStatus(uint32_t mask, SD_ID_Type sdId)
{
	uint32_t SdBaseAddr = sdAddr[sdId];
    writel(mask, SdBaseAddr + SDH_SD_NORMAL_INT_STATUS_OFFSET);
}

SDH_Stat_Type SDH_WaitCmdIdle(SD_ID_Type sdId)
{
    uint16_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];
	
    int timeout = 1000;
    while (timeout-- > 0) {
        tmpVal = readw(SdBaseAddr + SDH_SD_PRESENT_STATE_1_OFFSET);
        if (!reg_is_bit_set(tmpVal, SDH_CMD_INHIBIT_CMD))
            return SDH_STAT_SUCCESS;
        arch_delay_ms(1);
    }
    return SDH_STAT_BUSY;
}

SDH_Stat_Type SDH_WaitDatIdle(SD_ID_Type sdId)
{
    uint16_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];
	
    int timeout = 1000;
    while (timeout-- > 0) {
        tmpVal = readw(SdBaseAddr + SDH_SD_PRESENT_STATE_1_OFFSET);
        if (!reg_is_bit_set(tmpVal, SDH_CMD_INHIBIT_DAT))
            return SDH_STAT_SUCCESS;
        arch_delay_ms(1);
    }
    return SDH_STAT_BUSY;
}
/****************************************************************************/ /**
 * @brief  SDH enable interrupt
 *
 * @param  dataCfg: Transfer data config pointer
 *
 * @return SDH status
 *
*******************************************************************************/
SDH_Stat_Type SDH_ConfigDataTranfer(SDH_Data_Cfg_Type *dataCfg, SD_ID_Type sdId)
{
    uint16_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];

    if (dataCfg != NULL) {
        /* For transfer boot continous, only need to set the CREQ bit */
        if (dataCfg->dataType == SDH_TRANS_DATA_BOOT_CONT) {
            tmpVal = readw(SdBaseAddr + SDH_SD_BLOCK_GAP_CTRL_OFFSET);
            /* Clear stop at block gap request */
            tmpVal = reg_clr_bit(tmpVal, SDH_STOP_AT_BLOCK_GAP_REQ);
            /* Continous transfer data */
            tmpVal = reg_set_bit(tmpVal, SDH_CONT_REQ);
            writew(tmpVal, SdBaseAddr + SDH_SD_BLOCK_GAP_CTRL_OFFSET);

            return SDH_STAT_SUCCESS;
        }

        SDH_WaitDatIdle(sdId);

        tmpVal = readw(SdBaseAddr + SDH_SD_TRANSFER_MODE_OFFSET);

        /* Check transfer block count */
        if ((dataCfg->blockCount > SDH_MAX_BLOCK_COUNT) || ((dataCfg->txData == NULL) && (dataCfg->rxData == NULL))) {
            return SDH_STAT_INVLAID_PARA;
        }

        if (dataCfg->rxData) {
            tmpVal = reg_set_bit(tmpVal, SDH_TO_HOST_DIR);
        } else {
            tmpVal = reg_clr_bit(tmpVal, SDH_TO_HOST_DIR);
        }

        if (dataCfg->cmd23_emmc_mode)
            tmpVal = reg_set_bit(tmpVal, SDH_CMD23_EMMC_MODE);
        else
            tmpVal = reg_clr_bit(tmpVal, SDH_CMD23_EMMC_MODE);

        tmpVal = reg_set_bits(tmpVal, SDH_AUTO_CMD_EN, 0x00);

        /* auto command 23, auto send set block count cmd before multiple read/write */
        if (dataCfg->enableAutoCommand23) {
            tmpVal = reg_set_bits(tmpVal, SDH_AUTO_CMD_EN, 0x02);
            if (dataCfg->enableRelibaleWR) {
                writel(dataCfg->blockCount | (0x1 << 31), SdBaseAddr + SDH_SD_SYS_ADDR_LOW_OFFSET);
            } else {
                writel(dataCfg->blockCount, SdBaseAddr + SDH_SD_SYS_ADDR_LOW_OFFSET);
            }
        } else if (dataCfg->enableAutoCommand12) {
            /* auto command 12 */
            tmpVal = reg_set_bits(tmpVal, SDH_AUTO_CMD_EN, 0x01);
        }

        if (dataCfg->blockCount > 1U) {
            tmpVal = reg_set_bit(tmpVal, SDH_MULTI_BLK_SEL);
        } else {
            tmpVal = reg_clr_bit(tmpVal, SDH_MULTI_BLK_SEL);
        }

        /* Block count enable. Per SD-host spec a single (non-multi) transfer should run
         * with BLK_CNT_EN=0; forcing it on frames a counted block, which can mis-place the
         * CRC window on a 4-bit single-block READ. g_sdh_single_no_blkcnt gates this so we
         * can A/B it without changing behaviour for other in-tree SDIO/SD paths. */
        if (g_sdh_single_no_blkcnt && dataCfg->blockCount <= 1U) {
            tmpVal = reg_clr_bit(tmpVal, SDH_BLK_CNT_EN);
        } else {
            tmpVal = reg_set_bit(tmpVal, SDH_BLK_CNT_EN);
        }

        if (dataCfg->dataType == SDH_TRANS_DATA_BOOT)
            tmpVal = reg_set_bit(tmpVal, SDH_BOOT_OPREATION);

        writew(tmpVal, SdBaseAddr + SDH_SD_TRANSFER_MODE_OFFSET);

//        if (dataCfg->dataType != SDH_TRANS_DATA_BOOT) {
            /* config data block size/block count */
            writew(dataCfg->blockSize, SdBaseAddr + SDH_SD_BLOCK_SIZE_OFFSET);
            writew(dataCfg->blockCount, SdBaseAddr + SDH_SD_BLOCK_COUNT_OFFSET);
//        }
    } else {
        /* clear data flags */
        tmpVal = readw(SdBaseAddr + SDH_SD_TRANSFER_MODE_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SDH_AUTO_CMD_EN, 0x00);
        writew(tmpVal, SdBaseAddr + SDH_SD_TRANSFER_MODE_OFFSET);

        tmpVal = readw(SdBaseAddr + SDH_SD_PRESENT_STATE_1_OFFSET);

        /* Check data inhibit flag */
        if (reg_is_bit_set(tmpVal, SDH_CMD_INHIBIT_CMD)) {
            return SDH_STAT_BUSY;
        }
    }

    return SDH_STAT_SUCCESS;
}

SDH_Stat_Type SDH_GetCmdResp(SDH_CMD_Cfg_Type *cmd, SD_ID_Type sdId)
{
	uint32_t SdBaseAddr = sdAddr[sdId];

    if (cmd->respType != SDH_RESP_NONE) {
        cmd->response[0U] = readl(SdBaseAddr + SDH_SD_RESP_0_OFFSET);
        cmd->response[1U] = readl(SdBaseAddr + SDH_SD_RESP_2_OFFSET);
        cmd->response[2U] = readl(SdBaseAddr + SDH_SD_RESP_4_OFFSET);
        cmd->response[3U] = readl(SdBaseAddr + SDH_SD_RESP_6_OFFSET);
    }

    return SDH_STAT_SUCCESS;
}

uint32_t SDH_ReadDataPort(SDH_Data_Cfg_Type *dataCfg, SD_ID_Type sdId)
{
    uint32_t tmpVal;
    uint32_t totalLen, recvLen = 0;
	uint32_t SdBaseAddr = sdAddr[sdId];

    totalLen = (dataCfg->blockCount * dataCfg->blockSize);
    tmpVal = readl(SdBaseAddr + SDH_SD_TRANSFER_MODE_OFFSET);

    if (!reg_is_bit_set(tmpVal, SDH_DMA_EN)) {
        do {
            dataCfg->rxData[dataCfg->rxDataLen++] = readl(SdBaseAddr + SDH_SD_BUFFER_DATA_PORT_0_OFFSET);
            recvLen += 4;
        } while (recvLen < totalLen);
    }

    return recvLen;
}

SDH_Stat_Type SDH_ReadDataPortBlock(SDH_Data_Cfg_Type *dataCfg, SD_ID_Type sdId)
{
    uint32_t intStatus = 0U;
    SDH_Stat_Type error = SDH_STAT_SUCCESS;

    /* Make sure the buffer size is 4 byte aligned */
    if (dataCfg->blockSize % sizeof(uint32_t) != 0U) {
        dataCfg->blockSize += sizeof(uint32_t) - (dataCfg->blockSize % sizeof(uint32_t));
    }

    while (!(intStatus & (SDH_INT_BUFFER_READ_READY | SDH_INT_DATA_ERRORS | SDH_INT_TUNE_ERROR))) {
        intStatus = SDH_GetIntStatus(sdId);
    }

    /* during std tuning process, software do not need to read data, but wait BRR is enough */
    if ((dataCfg->dataType == SDH_TRANS_DATA_TUNING) && (intStatus & SDH_INT_BUFFER_READ_READY)) {
        SDH_ClearIntStatus(SDH_INT_BUFFER_READ_READY, sdId);
        return SDH_STAT_SUCCESS;
    } else if ((intStatus & SDH_INT_TUNE_ERROR) != 0U) {
        SDH_ClearIntStatus(SDH_INT_TUNE_ERROR, sdId);
        /* if tuning error occur ,return directly */
        error = SDH_STAT_TUNE_ERROR;
    } else if ((intStatus & SDH_INT_DATA_ERRORS) != 0U) {
        /* enableIgnoreError tolerates a (false) DATA_CRC, but a genuine DATA_TIMEOUT
         * (card NAK / not driving) must still fail — else we return stale buffer data. */
        if (!(dataCfg->enableIgnoreError) || (intStatus & SDH_INT_DATA_TIMEOUT)) {
            error = SDH_STAT_RECV_DATA_FAIL;
        }

        /* clear data error flag */
        SDH_ClearIntStatus(SDH_INT_DATA_ERRORS, sdId);
    }

    if (error == SDH_STAT_SUCCESS) {
        SDH_ReadDataPort(dataCfg, sdId);
        /* clear buffer read ready */
        SDH_ClearIntStatus(SDH_INT_BUFFER_READ_READY, sdId);
        intStatus = 0U;
    }

    /* Clear data complete flag after the last read operation. */
    SDH_ClearIntStatus(SDH_INT_DATA_COMPLETED, sdId);

    return error;
}

uint32_t SDH_WriteDataPort(SDH_Data_Cfg_Type *dataCfg, SD_ID_Type sdId)
{
    uint32_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];

    tmpVal = readl(SdBaseAddr + SDH_SD_TRANSFER_MODE_OFFSET);

    if (!reg_is_bit_set(tmpVal, SDH_DMA_EN)) {
        writel(dataCfg->txData[dataCfg->txDataLen++], SdBaseAddr + SDH_SD_BUFFER_DATA_PORT_0_OFFSET);
    }

    return 4;
}

SDH_Stat_Type SDH_WriteDataPortBlock(SDH_Data_Cfg_Type *dataCfg, SD_ID_Type sdId)
{
    uint32_t totalLen;
    uint32_t txLen = 0U, intStatus = 0U;
    SDH_Stat_Type error = SDH_STAT_SUCCESS;

    /* Make sure the buffer size is 4 byte aligned */
    if (dataCfg->blockSize % sizeof(uint32_t) != 0U) {
        dataCfg->blockSize += sizeof(uint32_t) - (dataCfg->blockSize % sizeof(uint32_t));
    }

    totalLen = (dataCfg->blockCount * dataCfg->blockSize);

    while ((error == SDH_STAT_SUCCESS) && (txLen < totalLen)) {
        while (!(intStatus & (SDH_INT_BUFFER_WRITE_READY | SDH_INT_DATA_ERRORS | SDH_INT_TUNE_ERROR))) {
            intStatus = SDH_GetIntStatus(sdId);
        }

        if ((intStatus & SDH_INT_TUNE_ERROR) != 0U) {
            SDH_ClearIntStatus(SDH_INT_TUNE_ERROR, sdId);
            /* if tuning error occur ,return directly */
            error = SDH_STAT_TUNE_ERROR;
        } else if ((intStatus & SDH_INT_DATA_ERRORS) != 0U) {
            if (!(dataCfg->enableIgnoreError)) {
                error = SDH_STAT_RECV_DATA_FAIL;
            }

            /* clear data error flag */
            SDH_ClearIntStatus(SDH_INT_DATA_ERRORS, sdId);
        }

        if (error == SDH_STAT_SUCCESS) {
            /* clear buffer read ready */
            SDH_ClearIntStatus(SDH_INT_BUFFER_WRITE_READY, sdId);
//            while(txLen < totalLen)
            txLen += SDH_WriteDataPort(dataCfg, sdId);
            intStatus = 0U;
//            break;
        }
    }

    while (!(intStatus & (SDH_INT_DATA_COMPLETED | SDH_INT_DATA_ERRORS))) {
        intStatus = SDH_GetIntStatus(sdId);
    }

    if ((intStatus & SDH_INT_DATA_ERRORS) != 0U) {
        if (!(dataCfg->enableIgnoreError)) {
            error = SDH_STAT_RECV_DATA_FAIL;
        }

        /* clear data error flag */
        SDH_ClearIntStatus(SDH_INT_DATA_ERRORS, sdId);
    }

    SDH_ClearIntStatus(SDH_INT_DATA_COMPLETED, sdId);

    return error;
}

void SDH_SendCommand(SDH_CMD_Cfg_Type *cmd, SD_ID_Type sdId)
{
    uint32_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];
    uint32_t flag = cmd->flag;
    //uint32_t flag = 0;

    tmpVal = readl(SdBaseAddr + SDH_SD_PRESENT_STATE_1_OFFSET);

    if (cmd->type != SDH_CMD_EMPTY) {
        /* Define the flag corresponding to each response type. */
        switch (cmd->respType) {
            case SDH_RESP_NONE:
                break;

            case SDH_RESP_R1: /* Response 1 */
            case SDH_RESP_R5: /* Response 5 */
            case SDH_RESP_R6: /* Response 6 */
            case SDH_RESP_R7: /* Response 7 */
                flag |= (SDH_TRANS_FLAG_RESP_48BITS | SDH_TRANS_FLAG_EN_CRC_CHECK | SDH_TRANS_FLAG_EN_INDEX_CHECK);
                break;

            case SDH_RESP_R1B: /* Response 1 with busy */
            case SDH_RESP_R5B: /* Response 5 with busy */
                flag |= (SDH_TRANS_FLAG_RESP_48BITS_WITH_BUSY | SDH_TRANS_FLAG_EN_CRC_CHECK | SDH_TRANS_FLAG_EN_INDEX_CHECK);
                break;

            case SDH_RESP_R2: /* Response 2 */
                flag |= (SDH_TRANS_FLAG_RESP_136BITS | SDH_TRANS_FLAG_EN_CRC_CHECK);
                break;

            case SDH_RESP_R3: /* Response 3 */
            case SDH_RESP_R4: /* Response 4 */
                flag |= (SDH_TRANS_FLAG_RESP_48BITS);
                break;

            default:
                break;
        }

        if (SDH_WaitCmdIdle(sdId) != SDH_STAT_SUCCESS)
            return;

        if (SDH_WaitDatIdle(sdId) != SDH_STAT_SUCCESS)
            return;

        /* Config command index */
        tmpVal = flag >> 16;
        tmpVal = reg_set_bits(tmpVal, SDH_CMD_TYPE, cmd->type);
        tmpVal = reg_set_bits(tmpVal, SDH_CMD_INDEX, cmd->index);

        /* Config command argument */
        writel(cmd->argument, SdBaseAddr + SDH_SD_ARG_LOW_OFFSET);

        /* Config command index */
        writew(tmpVal, SdBaseAddr + SDH_SD_CMD_OFFSET);
    } else {  /* FIXME:how to enable again */
        SDH_DisableIntSource(SDH_INT_CMD_COMPLETED);
    }
}

SDH_Stat_Type SDH_WaitCommandDone(SDH_CMD_Cfg_Type *cmd, SD_ID_Type sdId)
{
    uint32_t intStatus = 0U;
    SDH_Stat_Type stat = SDH_STAT_SUCCESS;
    uint32_t t0 = sdh_now_ms();

    /* Wait command complete or encounters error. */
    while (!(intStatus & (SDH_INT_CMD_COMPLETED | SDH_INT_CMD_ERRORS | SDH_INT_TUNE_ERROR))) {
        intStatus = SDH_GetIntStatus(sdId);
        /* A command phase is ~48 SD clocks (sub-ms even at the 400kHz ID clock) —
         * if it never completes the bus is wedged, and without this bound the
         * caller spins here forever (observed: task hard-hang mid-CMD53 on a
         * marginal 4-bit bus). Reset the CMD+DAT engines and report failure so
         * callers' retry/error paths run instead. */
        if (sdh_now_ms() - t0 > 1000U) {
            uint32_t base = sdAddr[sdId];
            uint16_t tc = readw(base + 0x2E);
            uint32_t spin = 100000U;
            writew(tc | (1U << 9) | (1U << 10), base + 0x2E);   /* SW_RST_CMD | SW_RST_DAT */
            while ((readw(base + 0x2E) & ((1U << 9) | (1U << 10))) && --spin) {}
            if (!spin) printf("  SW_RST_CMD/DAT did not self-clear!\r\n");
            SDH_ClearIntStatus(SDH_GetIntStatus(sdId), sdId);
            printf("SDH cmd-phase timeout: CMD%d never completed, bus reset\r\n",
                   (int)cmd->index);
            return SDH_STAT_SEND_CMD_FAIL;
        }
    }

    if ((intStatus & SDH_INT_TUNE_ERROR) != 0U) {
        stat = SDH_STAT_TUNE_ERROR;
    } else if ((intStatus & SDH_INT_CMD_ERRORS) != 0U) {
        stat = SDH_STAT_SEND_CMD_FAIL;
    }

    /* Receive response when command completes successfully. */
    if (stat == SDH_STAT_SUCCESS) {
        stat = SDH_GetCmdResp(cmd, sdId);
    }

    /* Clear ONLY the command-phase events. On a short, fast data transfer (e.g. a
     * 4-byte CMD53 at 4-bit ~= 8 SD-clocks ~= 0.2us @24MHz) the data engine can
     * finish and latch DATA_COMPLETED / DMA_COMPLETED / BUFFER_READ_READY into the
     * same W1C NORMAL_INT_STATUS word *before* software returns here. Clearing the
     * whole word (intStatus) would swallow those bits, so the subsequent
     * SDH_TransferDataBlocking()/SDH_ReadDataPortBlock() poll waits forever
     * (intStatus=0, engine idle -> 5s DataTimeout). 1-bit / large transfers take
     * long enough that the data bit has not yet latched here, so they were unaffected.
     * Preserve the data-phase bits for the data-wait stage. */
    SDH_ClearIntStatus(intStatus & (SDH_INT_CMD_COMPLETED | SDH_INT_CMD_ERRORS |
                                    SDH_INT_TUNE_ERROR), sdId);

    return stat;
}

/* SDHCI error recovery: SW-reset the DAT line so CMD_INHIBIT_DAT drops and the
 * controller can accept the next data command. Without this a single data-phase
 * error leaves TX/RX_ACTIVE + CMD_INHIBIT_DAT stuck, the next CMD53 is silently
 * dropped in SDH_WaitDatIdle and SDH_WaitCommandDone (no timeout) spins forever —
 * so caller-side retries can never recover. */
static void SDH_ResetDatLine(SD_ID_Type sdId)
{
    uint32_t base = sdAddr[sdId];
    uint16_t tc = readw(base + 0x2E);
    uint32_t spin = 100000U;

    writew(tc | (1U << 10), base + 0x2E);   /* SW_RST_DAT, self-clearing */
    while ((readw(base + 0x2E) & (1U << 10)) && --spin) {}
    if (!spin) printf("  SW_RST_DAT did not self-clear!\r\n");
}

SDH_Stat_Type SDH_TransferDataBlocking(SDH_Data_Cfg_Type *dataCfg, uint8_t enDMA, SD_ID_Type sdId)
{
    SDH_Stat_Type stat = SDH_STAT_SUCCESS;
    uint32_t intStatus = 0U;

    if (enDMA) {
        uint32_t t0 = sdh_now_ms();
        /* The command is already on the bus; let the HAL wait for the data
         * phase at a priority of its choosing. On timeout, or with no hook,
         * the poll below runs exactly as before, diagnostics included. */
        if (SDH_WaitDataHook(sdId, g_sdh_data_poll_timeout_ms) == 0) {
            intStatus = SDH_GetIntStatus(sdId);
        }
        /* Wait dataCfg complete or encounters error. */
        while (!(intStatus & (SDH_INT_DATA_COMPLETED | SDH_INT_DATA_ERRORS |
                              SDH_INT_DMA_ERROR | SDH_INT_TUNE_ERROR))) {
            intStatus = SDH_GetIntStatus(sdId);
            if (sdh_now_ms() - t0 > g_sdh_data_poll_timeout_ms) {
                uint32_t base = sdAddr[sdId];
                uint32_t adma = readl(base + 0x58);
                if (!g_sdh_data_quiet_timeout) {
                printf("SDH_TransferDataBlocking timeout: %lu ms, intStatus=0x%lx\r\n",
                       sdh_now_ms() - t0, intStatus);
                printf("  err_int_status   = 0x%04x\r\n", readw(base + 0x32));
                printf("  present_state_1  = 0x%08lx\r\n", readl(base + 0x24));
                printf("  blk_size/cnt     = 0x%04x / 0x%04x\r\n",
                       readw(base + 0x04), readw(base + 0x06));
                printf("  transfer_mode    = 0x%04x\r\n", readw(base + 0x0C));
                printf("  host_ctrl        = 0x%04x\r\n", readw(base + 0x28));
                printf("  sys_addr_low     = 0x%08lx\r\n", readl(base + 0x00));
                printf("  adma_sys_addr    = 0x%08lx\r\n", adma);
                printf("  norm_int_st_en   = 0x%08lx\r\n", readl(base + 0x34));
                printf("  clk_ctrl(0x2C)   = 0x%04x   tmo_ctrl(0x2E)=0x%04x\r\n",
                       readw(base + 0x2C), readw(base + 0x2E));
                printf("  present_state_2  = 0x%04x   host_ctrl2(0x3E)=0x%04x\r\n",
                       readw(base + 0x26), readw(base + 0x3E));
                printf("  adma_err(0x54)   = 0x%04x\r\n", readw(base + 0x54));
                printf("  shared_bus(0xE0) = 0x%08lx\r\n", readl(base + 0xE0));
                }
                SDH_ResetDatLine(sdId);
                /* Decode descriptor[0] which the SDH controller already
                 * consumed (adma advanced by 8 bytes). Re-read from base. */
                if (!g_sdh_data_quiet_timeout && adma >= 8 && adma < 0x12000000) {
                    uint32_t *d = (uint32_t *)(adma - 8);
                    printf("  adma_desc[0]     = attr=0x%08lx addr=0x%08lx\r\n",
                           d[0], d[1]);
                    uint32_t buf_addr = d[1];
                    if (buf_addr >= 0x11000000 && buf_addr < 0x12000000) {
                        uint32_t *b = (uint32_t *)buf_addr;
                        printf("  tx_buf[0..15]    = %08lx %08lx %08lx %08lx\r\n",
                               b[0], b[1], b[2], b[3]);
                    }
                }
                SDH_ClearIntStatus(intStatus, sdId);
                return SDH_STAT_SEND_DATA_FAIL;
            }
        }

        if ((intStatus & SDH_INT_TUNE_ERROR) != 0U) {
            stat = SDH_STAT_TUNE_ERROR;
        } else if ((intStatus & (SDH_INT_DATA_ERRORS | SDH_INT_DMA_ERROR)) != 0U) {
            if ((!(dataCfg->enableIgnoreError)) || (intStatus & SDH_INT_DATA_TIMEOUT)) {
                stat = SDH_STAT_SEND_DATA_FAIL;
            }
            /* Diagnostic: classify the fast data-phase error (vs the 5s timeout above).
             * err_int @0x32: bit0=CMD_TMO bit1=CMD_CRC ... bit4=DAT_TMO bit5=DAT_CRC
             * bit6=DAT_END_BIT bit9=ADMA. intStatus shows if DATA_COMPLETED(0x2) co-set. */
            if (!g_sdh_data_quiet_timeout) {
                printf("SDH data-ERR: intStatus=0x%08lx err_int=0x%04x present=0x%08lx\r\n",
                       (unsigned long)intStatus, readw(sdAddr[sdId] + 0x32),
                       (unsigned long)readl(sdAddr[sdId] + 0x24));
            }
        }

        /* Any data-phase/tuning error can leave the DAT engine claiming the bus
         * (same wedge the 5s-timeout branch above recovers from) — reset it so
         * the caller's retry actually re-issues the CMD53 instead of wedging. */
        if (stat != SDH_STAT_SUCCESS) {
            SDH_ResetDatLine(sdId);
        }

        /* load dummy data */
        if ((dataCfg->dataType == SDH_TRANS_DATA_BOOT_CONT) && (stat == SDH_STAT_SUCCESS)) {
            *(dataCfg->rxData) = bootDummy;
        }

        SDH_ClearIntStatus(intStatus, sdId);
        //SDH_DisableDMA();

    } else {
        if (dataCfg->rxData) {
            stat = SDH_ReadDataPortBlock(dataCfg, sdId);
        } else {
            stat = SDH_WriteDataPortBlock(dataCfg, sdId);
        }
    }
//    printf("intStatus = 0x%lx, dmaerr = 0x%x\r\n", intStatus, SDH_GetDMAErrStatus());

    return stat;
}

void SDH_SetSdClock(uint32_t srcClock, uint32_t busClock, SD_ID_Type sdId)
{
    uint16_t tmpVal;
    uint16_t div = srcClock / busClock / 2;
	uint32_t SdBaseAddr = sdAddr[sdId];

    tmpVal = readw(SdBaseAddr + SDH_SD_CLOCK_CTRL_OFFSET);

    /* Disable internal clock*/
    tmpVal = reg_clr_bit(tmpVal, SDH_INT_CLK_EN);
    tmpVal = reg_clr_bit(tmpVal, SDH_SD_CLK_EN);

    /* Set divdier */
    tmpVal = reg_set_bits(tmpVal, SDH_SD_FREQ_SEL_LO, div & 0xff);
    tmpVal = reg_set_bits(tmpVal, SDH_SD_FREQ_SEL_HI, (div >> 8) & 0x3);
    /* 0x1 = Programmable Clock Mode,0x0 = Divided Clock mode */
    tmpVal = reg_set_bits(tmpVal, SDH_CLK_GEN_SEL, 0);

    /* Enable internal clock*/
    tmpVal = reg_set_bit(tmpVal, SDH_INT_CLK_EN);
    tmpVal = reg_set_bit(tmpVal, SDH_SD_CLK_EN);

    writew(tmpVal, SdBaseAddr + SDH_SD_CLOCK_CTRL_OFFSET);

    /* Wait for clock stable */
    do {
        tmpVal = readw(SdBaseAddr + SDH_SD_CLOCK_CTRL_OFFSET);
    } while (!reg_is_bit_set(tmpVal, SDH_INT_CLK_STABLE));
}

void SDH_DisableSdClock(SD_ID_Type sdId)
{
    uint32_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];

    tmpVal = readl(SdBaseAddr + SDH_SD_CLOCK_CTRL_OFFSET);

    /* Disable internal clock*/
    tmpVal = reg_clr_bit(tmpVal, SDH_INT_CLK_EN);
    tmpVal = reg_clr_bit(tmpVal, SDH_SD_CLK_EN);

    writel(tmpVal, SdBaseAddr + SDH_SD_CLOCK_CTRL_OFFSET);
}

void SDH_Powon(SD_ID_Type sdId)
{
	
    uint32_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];
    tmpVal = readw(SdBaseAddr + SDH_SD_HOST_CTRL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SDH_SD_BUS_POWER);
    writew(tmpVal, SdBaseAddr + SDH_SD_HOST_CTRL_OFFSET);
}

void SDH_Ctrl_Init(const SDH_Cfg_Type *cfg, SD_ID_Type sdId)
{

    uint32_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];

    SDH_SetSdClock(cfg->srcClock, cfg->busClock, sdId);
    SDH_DisableDMA(sdId);

    /* Set SDH config */
    tmpVal = readw(SdBaseAddr + SDH_SD_HOST_CTRL_OFFSET);

    if (cfg->dataWidth == SDH_DATA_BUS_WIDTH_8BITS) {
        tmpVal = reg_set_bit(tmpVal, SDH_EX_DATA_WIDTH);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SDH_EX_DATA_WIDTH);
        tmpVal = reg_set_bits(tmpVal, SDH_DATA_WIDTH, cfg->dataWidth);
    }

    tmpVal = reg_set_bits(tmpVal, SDH_HI_SPEED_EN, cfg->highSpeed);
    tmpVal = reg_set_bits(tmpVal, SDH_SD_BUS_VLT, cfg->volt);

    writew(tmpVal, SdBaseAddr + SDH_SD_HOST_CTRL_OFFSET);

    if (cfg->vlot18Enable) {
        tmpVal = readw(SdBaseAddr + SDH_HOST_CTRL_2_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SDH_UHS_MODE_SEL, cfg->speed);
        tmpVal = reg_set_bits(tmpVal, SDH_SDH_V18_EN, cfg->vlot18Enable);
        writew(tmpVal, SdBaseAddr + SDH_HOST_CTRL_2_OFFSET);
    }

    SDH_EnableStatus(sdId);
}

void SDH_Deinit(SD_ID_Type sdId)
{
    SDH_DisableDMA(sdId);
}

void SDH_Reset(SD_ID_Type sdId)
{
    uint16_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];

    tmpVal = readw(SdBaseAddr + SDH_SD_TIMEOUT_CTRL_SW_RESET_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SDH_SW_RST_ALL);
    writew(tmpVal, SdBaseAddr + SDH_SD_TIMEOUT_CTRL_SW_RESET_OFFSET);

    /* Wait for reset done */
    do {
        tmpVal = readw(SdBaseAddr + SDH_SD_TIMEOUT_CTRL_SW_RESET_OFFSET);
    } while (reg_is_bit_set(tmpVal, SDH_SW_RST_ALL));
}

void SDH_Set_Timeout(uint8_t tmo, SD_ID_Type sdId)
{
    uint16_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];

    tmpVal = readw(SdBaseAddr + SDH_SD_TIMEOUT_CTRL_SW_RESET_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SDH_TIMEOUT_VALUE, tmo);
    writew(tmpVal, SdBaseAddr + SDH_SD_TIMEOUT_CTRL_SW_RESET_OFFSET);
}

void SDH_GetCapability(SDH_Cap_Cfg_Type *capability)
{
    /* Get the capability  */
    capability->maxBlockLength = 512;
    capability->maxBlockCount = 65536;
    capability->flags = (SDH_CAP_SUPPORT_8BITS |
                         SDH_CAP_SUPPORT_ADMA2 |
                         SDH_CAP_SUPPORT_ADMA1 |
                         SDH_CAP_SUPPORT_HIGHSPEED |
                         SDH_CAP_SUPPORT_SDMA |
                         SDH_CAP_SUPPORT_SUS_RES |
                         SDH_CAP_SUPPORT_VLT33 |
                         SDH_CAP_SUPPORT_VLT18 |
                         SDH_CAP_SUPPORT_SBUS_64BITS |
                         SDH_CAP_SUPPORT_ASYNC_INT |
                         SDH_CAP_SUPPORT_DDR50 |
                         SDH_CAP_SUPPORT_SDR104 |
                         SDH_CAP_SUPPORT_SDR50 |
                         SDH_CAP_SUPPORT_DRV_TYPE_A |
                         SDH_CAP_SUPPORT_DRV_TYPE_C |
                         SDH_CAP_SUPPORT_DRV_TYPE_D |
                         SDH_CAP_SUPPORT_SDR50_TUNE);
}

SDH_Stat_Type SDH_CreateADMA1Descriptor(uint32_t *adma1Entries, uint32_t maxEntries,
                                        const uint32_t *data, uint32_t dataLen, SDH_ADMA_Flag_Type flag)
{
    uint32_t miniEntries, startEntries = 0U;
    uint32_t i, dmaBufferLen = 0U;

    if (((uintptr_t)data % SDH_ADMA1_ADDRESS_ALIGN) != 0U) {
        return SDH_STAT_DMA_ADDR_NOT_ALIGN;
    }

    if (flag == SDH_ADMA_FLAG_MULTI_DESC) {
        return SDH_STAT_NOT_SUPPORT;
    }

    /* TODO:align */
    /* Make sure the buffer size is 4 byte aligned */
    if (dataLen % sizeof(uint32_t) != 0U) {
        /* make the data length as word-aligned */
        dataLen += sizeof(uint32_t) - (dataLen % sizeof(uint32_t));
    }

    /* Check if ADMA descriptor's number is enough. */
    if ((dataLen % SDH_ADMA1_DESCRIPTOR_MAX_LENGTH_PER_ENTRY) == 0U) {
        miniEntries = dataLen / SDH_ADMA1_DESCRIPTOR_MAX_LENGTH_PER_ENTRY;
    } else {
        miniEntries = ((dataLen / SDH_ADMA1_DESCRIPTOR_MAX_LENGTH_PER_ENTRY) + 1U);
    }

    /* ADMA1 needs two descriptors to finish a transfer */
    miniEntries <<= 1U;

    if (miniEntries + startEntries > maxEntries) {
        return SDH_STAT_OUTOF_RANGE;
    }

    for (i = startEntries; i < (miniEntries + startEntries); i += 2U) {
        if (dataLen > SDH_ADMA1_DESCRIPTOR_MAX_LENGTH_PER_ENTRY) {
            dmaBufferLen = SDH_ADMA1_DESCRIPTOR_MAX_LENGTH_PER_ENTRY;
        } else {
            dmaBufferLen = dataLen;
        }

        adma1Entries[i] = (dmaBufferLen << SDH_ADMA1_DESCRIPTOR_LENGTH_POS);
        adma1Entries[i] |= SDH_ADMA1_DESC_FLAG_SETLEN;
        adma1Entries[i + 1U] = (uintptr_t)(data);
        adma1Entries[i + 1U] |= SDH_ADMA1_DESC_FLAG_TRANSFER;
        data += (dmaBufferLen / sizeof(uint32_t));
        dataLen -= dmaBufferLen;
    }

    /* Set end of the descriptor */
    adma1Entries[i - 1U] |= SDH_ADMA1_DESC_FLAG_END;

    return SDH_STAT_SUCCESS;
}

SDH_Stat_Type SDH_CreateADMA2Descriptor(SDH_ADMA2_Desc_Type *adma2Entries, uint32_t maxEntries,
                                        const uint32_t *data, uint32_t dataLen, SDH_ADMA_Flag_Type flag)
{
    uint32_t miniEntries, startEntries = 0U;
    uint32_t i, dmaBufferLen = 0U;

    if (((uintptr_t)data % SDH_ADMA2_ADDRESS_ALIGN) != 0U) {
        return SDH_STAT_DMA_ADDR_NOT_ALIGN;
    }

    /* TODO:align */
    /* Make sure the buffer size is 4 byte aligned */
    if (dataLen % sizeof(uint32_t) != 0U) {
        /* make the data length as word-aligned */
        dataLen += sizeof(uint32_t) - (dataLen % sizeof(uint32_t));
    }

    /* Check if ADMA descriptor's number is enough. */
    if ((dataLen % SDH_ADMA2_DESCRIPTOR_MAX_LENGTH_PER_ENTRY) == 0U) {
        miniEntries = dataLen / SDH_ADMA2_DESCRIPTOR_MAX_LENGTH_PER_ENTRY;
    } else {
        miniEntries = ((dataLen / SDH_ADMA2_DESCRIPTOR_MAX_LENGTH_PER_ENTRY) + 1U);
    }

    /* Calculate the start entry for multiple descriptor mode, ADMA engine is not stop, so update the descriptor
	data address and data size is enough */
    if (flag == SDH_ADMA_FLAG_MULTI_DESC) {
        for (i = 0U; i < maxEntries; i++) {
            if ((adma2Entries[i].attribute & SDH_ADMA2_DESC_FLAG_VALID) == 0U) {
                break;
            }
        }

        startEntries = i;
        /* Add one entry for dummy entry */
        miniEntries += 1U;
    }

    if (miniEntries + startEntries > maxEntries) {
        return SDH_STAT_OUTOF_RANGE;
    }

    for (i = startEntries; i < (miniEntries + startEntries); i++) {
        if (dataLen > SDH_ADMA2_DESCRIPTOR_MAX_LENGTH_PER_ENTRY) {
            dmaBufferLen = SDH_ADMA2_DESCRIPTOR_MAX_LENGTH_PER_ENTRY;
        } else {
            /* TODO:align */
            /* Adma don't support 0 data length transfer descriptor */
            dmaBufferLen = (dataLen == 0U ? sizeof(uint32_t) : dataLen);
        }

        /* Each descriptor for ADMA2 is 64-bit in length */
        adma2Entries[i].address = (dataLen == 0U) ? (uint32_t)(uintptr_t)&bootDummy : (uint32_t)(uintptr_t)data;
        adma2Entries[i].attribute = (dmaBufferLen << SDH_ADMA2_DESCRIPTOR_LENGTH_POS);
        adma2Entries[i].attribute |= (dataLen == 0U) ? 0U : (SDH_ADMA2_DESC_FLAG_TRANSFER);
        // (dataLen == 0U) ? 0U : (SDH_ADMA2_DESC_FLAG_TRANSFER | SDH_ADMA2_DESC_FLAG_INT);
        /* TODO:align */
        data += (dmaBufferLen / sizeof(uint32_t));

        if (dataLen != 0U) {
            dataLen -= dmaBufferLen;
        }
    }

    /* add a dummy valid ADMA descriptor for multiple descriptor mode, this is useful when transfer boot data, the ADMA
	engine  will not stop at block gap */
    if (flag == SDH_ADMA_FLAG_MULTI_DESC) {
        //adma2Entries[startEntries + 1U].attribute |= SDH_ADMA2_DESC_FLAG_TRANSFER;
        adma2Entries[i - 1U].attribute |= SDH_ADMA2_DESC_FLAG_TRANSFER;
    } else {
        /* Set the end bit */
        adma2Entries[i - 1U].attribute |= SDH_ADMA2_DESC_FLAG_END;
    }

    return SDH_STAT_SUCCESS;
}

SDH_Stat_Type SDH_SetInternalDmaConfig(SDH_DMA_Cfg_Type *dmaCfg, const uint32_t *data, uint8_t enAutoCmd23, SD_ID_Type sdId)
{
	uint32_t SdBaseAddr = sdAddr[sdId];

    if (dmaCfg->dmaMode == SDH_DMA_MODE_SIMPLE) {
        /* Check DMA data buffer address align or not */
        if (((uintptr_t)data % SDH_SDMA_LENGTH_ALIGN) != 0U) {
            return SDH_STAT_DMA_ADDR_NOT_ALIGN;
        }

        /* In simple DMA mode if use auto CMD23, address should load to ADMA addr,
		     and block count should load to DS_ADDR*/
        if (enAutoCmd23) {
            writel((uintptr_t)data, SdBaseAddr + SDH_SD_ADMA_SYS_ADDR_1_OFFSET);
        } else {
            writel((uintptr_t)data, SdBaseAddr + SDH_SD_SYS_ADDR_LOW_OFFSET);
        }
    } else {
        /* When use ADMA, disable simple DMA */
        writel(0, SdBaseAddr + SDH_SD_SYS_ADDR_LOW_OFFSET);
        writel((uintptr_t)(dmaCfg->admaEntries), SdBaseAddr + SDH_SD_ADMA_SYS_ADDR_1_OFFSET);
        /* The descriptor table was just written by the CPU; the ADMA master
         * fetches it from physical RAM. If the table sits in cached memory
         * (e.g. a caller whose descriptor pool is in cached OCRAM/.bss instead
         * of non-cached TCM) the fetch reads stale data and the write data
         * engine parks at ST_FDS (CMD53 write clocks no data). Flush it.
         * No-op-safe when the table is already in non-cached TCM. */
        L1C_DCACHE_CLEAN_RANGE((uintptr_t)dmaCfg->admaEntries,
                               dmaCfg->maxEntries * sizeof(SDH_ADMA2_Desc_Type));
    }

    /* Select DMA mode and config the burst length */
    SDH_EnableDMA(dmaCfg->dmaMode, sdId);

    return SDH_STAT_SUCCESS;
}

SDH_Stat_Type SDH_CreateAdmaEntryConfig(SDH_DMA_Cfg_Type *dmaCfg, SDH_Data_Cfg_Type *dataCfg, SDH_ADMA_Flag_Type flag, SD_ID_Type sdId)
{
    SDH_Stat_Type stat = SDH_STAT_SUCCESS;

    /* TODO:align */
    uint32_t bootDummyOffset = (dataCfg->dataType == SDH_TRANS_DATA_BOOT_CONT ? sizeof(uint32_t) : 0);
    const uint32_t *data = (const uint32_t *)((dataCfg->rxData == NULL) ? dataCfg->txData : dataCfg->rxData);
    uint32_t dataLen = dataCfg->blockSize * dataCfg->blockCount;

    data += bootDummyOffset;
    dataLen -= bootDummyOffset;

    switch (dmaCfg->dmaMode) {
        case SDH_DMA_MODE_SIMPLE:
            stat = SDH_STAT_SUCCESS;
            break;

        case SDH_DMA_MODE_ADMA1:
            stat = SDH_CreateADMA1Descriptor(dmaCfg->admaEntries, dmaCfg->maxEntries, data, dataLen, flag);
            break;

        case SDH_DMA_MODE_ADMA2:
            stat = SDH_CreateADMA2Descriptor((SDH_ADMA2_Desc_Type *)dmaCfg->admaEntries, dmaCfg->maxEntries, data, dataLen, flag);
            break;

        default:
            return SDH_STAT_SEND_DATA_FAIL;
    }

    /* For internal dma, internal DMA configurations should not update the configurations when continous transfer the
     * boot data, only the DMA descriptor need update */
    if ((stat == SDH_STAT_SUCCESS) && (dataCfg->dataType != SDH_TRANS_DATA_BOOT_CONT)) {
        stat = SDH_SetInternalDmaConfig(dmaCfg, data, dataCfg->enableAutoCommand23, sdId);
    }

    return stat;
}

SDH_Stat_Type SDH_TransferBlocking(SDH_DMA_Cfg_Type *dmaCfg, SDH_Trans_Cfg_Type *transfer, SD_ID_Type sdId)
{
    SDH_Stat_Type stat = SDH_STAT_SUCCESS;
    SDH_CMD_Cfg_Type *cmdCfg = transfer->cmdCfg;
    SDH_Data_Cfg_Type *dataCfg = transfer->dataCfg;
    uint32_t intStatus;
    uint8_t enDMA = 1;
    uint8_t executeTuning = ((dataCfg == NULL) ? 0 : dataCfg->dataType == SDH_TRANS_DATA_TUNING);
    //volatile uint32_t tmp;
    /* Check re-tuning request */
    intStatus = SDH_GetIntStatus(sdId);
    if ((intStatus & SDH_INT_RETUNE_EVENT) != 0U) {
        SDH_ClearIntStatus(SDH_INT_RETUNE_EVENT, sdId);
        return SDH_STAT_RETUNE_REQ;
    }

    /* Update ADMA descriptor table according to different DMA mode(simple DMA, ADMA1, ADMA2).*/
    if ((dataCfg != NULL) && (dmaCfg != NULL) && (!executeTuning)) {
        stat = SDH_CreateAdmaEntryConfig(dmaCfg, dataCfg, (dataCfg->dataType & SDH_TRANS_DATA_BOOT) ? SDH_ADMA_FLAG_MULTI_DESC : SDH_ADMA_FLAG_SINGLE_DESC, sdId);
    }

    /* If the DMA desciptor configure fail or not needed , disable it */
    if ((stat != SDH_STAT_SUCCESS) || (dmaCfg == NULL)) {
        enDMA = 0;
        /* Disable DMA, using polling mode in this situation */
        SDH_DisableDMA(sdId);
    }

    /* Config the data transfer parameter */
    stat = SDH_ConfigDataTranfer(dataCfg, sdId);

    if (SDH_STAT_SUCCESS != stat) {
        return stat;
    }

    /* A data transfer must start with a clean data-phase status. Since
     * SDH_WaitCommandDone preserves data bits across its W1C (so a short
     * transfer's completion latched during command processing survives), any
     * DATA/DMA/BUFFER bit still set HERE is stale residue from an earlier
     * transfer. Left set, the data-wait consumes it and restarts the ADMA
     * engine mid-stream — observed as the first 32-byte burst of the first
     * CMD53 read after a write vanishing (SMID GET_MAC reply desync). */
    if (dataCfg != NULL) {
        SDH_ClearIntStatus(SDH_INT_DATA_COMPLETED | SDH_INT_DMA_COMPLETED |
                           SDH_INT_BUFFER_WRITE_READY | SDH_INT_BUFFER_READ_READY |
                           SDH_INT_DATA_ERRORS | SDH_INT_DMA_ERROR, sdId);
    }

    /* Send command first */
    if (dataCfg != NULL) {
        cmdCfg->flag = SDH_TRANS_FLAG_DATA_PRESENT;
    } else {
        cmdCfg->flag = 0;
    }

    SDH_SendCommand(cmdCfg, sdId);

    /* Wait command done */
    if ((dataCfg == NULL) || (dataCfg->dataType == SDH_TRANS_DATA_NORMAL)) {
        stat = SDH_WaitCommandDone(cmdCfg, sdId);
    }

    /* Wait for transferring data finish */
    if ((dataCfg != NULL) && (stat == SDH_STAT_SUCCESS)) {
        stat = SDH_TransferDataBlocking(dataCfg, enDMA, sdId);
    }

    return stat;
}

SDH_Stat_Type SDH_TransferNonBlocking(SDH_DMA_Cfg_Type *dmaCfg, SDH_Trans_Cfg_Type *transfer, SD_ID_Type sdId)
{
    SDH_Stat_Type stat = SDH_STAT_SUCCESS;
    SDH_CMD_Cfg_Type *cmdCfg = transfer->cmdCfg;
    SDH_Data_Cfg_Type *dataCfg = transfer->dataCfg;
    uint32_t intStatus;
    uint8_t executeTuning = ((dataCfg == NULL) ? 0 : dataCfg->dataType == SDH_TRANS_DATA_TUNING);

    /* Check re-tuning request */
    intStatus = SDH_GetIntStatus(sdId);

    if ((intStatus & SDH_INT_RETUNE_EVENT) != 0U) {
        SDH_ClearIntStatus(SDH_INT_RETUNE_EVENT, sdId);
        return SDH_STAT_RETUNE_REQ;
    }

    /* Update ADMA descriptor table according to different DMA mode(simple DMA, ADMA1, ADMA2).*/
    if ((dataCfg != NULL) && (dmaCfg != NULL) && (!executeTuning)) {
        stat = SDH_CreateAdmaEntryConfig(dmaCfg, dataCfg, (dataCfg->dataType & SDH_TRANS_DATA_BOOT) ? SDH_ADMA_FLAG_MULTI_DESC : SDH_ADMA_FLAG_SINGLE_DESC, sdId);
    }

    /* If the DMA desciptor configure fail or not needed , disable it */
    if (stat != SDH_STAT_SUCCESS) {
        /* Disable DMA, using polling mode in this situation */
        //SDH_DisableDMA();
        return stat;
    }

    /* Config the data transfer parameter */
    stat = SDH_ConfigDataTranfer(dataCfg, sdId);

    if (SDH_STAT_SUCCESS != stat) {
        return stat;
    }

    /* Send command first */
    if (dataCfg != NULL) {
        cmdCfg->flag |= SDH_TRANS_FLAG_DATA_PRESENT;
    }

#if 0
	SDH_SendCommand(cmdCfg);


	/* Wait command done */
	if((dataCfg == NULL) || (dataCfg->dataType == SDH_TRANS_DATA_NORMAL))
	{
		stat = SDH_WaitCommandDone(cmdCfg);
	}

#endif

    return SDH_STAT_SUCCESS;
}

uint32_t EMMC_CheckHighSpeedSup(SD_ID_Type sdId)
{
    uint16_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];
	tmpVal = readw(SdBaseAddr + SDH_SD_CAPABILITIES_2_OFFSET);
	tmpVal = reg_get_bits(tmpVal, SDH_HI_SPEED_SUPPORT);
	return tmpVal;

}

void EMMC_EnableHighSpeed(SD_ID_Type sdId)
{
    uint16_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];
	
	tmpVal = readw(SdBaseAddr + SDH_HOST_CTRL_2_OFFSET);
	tmpVal = reg_set_bit(tmpVal, SDH_HI_SPEED_EN);
	tmpVal = reg_set_bit(tmpVal, SDH_DATA_WIDTH);
	writel(tmpVal, SdBaseAddr + SDH_SD_HOST_CTRL_OFFSET);

	printf("enable high speed: 0x%x\r\n", tmpVal);
}

uint32_t EMMC_CheckPresentValEnable(SD_ID_Type sdId)
{
    uint16_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];
	tmpVal = readw(SdBaseAddr + SDH_SD_CAPABILITIES_2_OFFSET);
	tmpVal = reg_get_bits(tmpVal, SDH_PRE_VAL_EN);
	return tmpVal;

}


void EMMC_ExeTuning(SD_ID_Type sdId)
{
    uint16_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];

    /* execute tuning */
    tmpVal = readw(SdBaseAddr + SDH_HOST_CTRL_2_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SDH_EXE_TUNING);
    writew(tmpVal, SdBaseAddr + SDH_HOST_CTRL_2_OFFSET);

	printf("execute tuning\r\n");
}

/*uint32_t EMMC_CheckDat0Busy(void)
{
	uint16_t tmpVal;
	SDH_PRESENT_FLAG_DATA0_LINE_LEVEL

}*/

uint32_t EMMC_CheckExeTuning(SD_ID_Type sdId)
{
    uint16_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];

    /* execute tuning */
    tmpVal = readw(SdBaseAddr + SDH_HOST_CTRL_2_OFFSET);
    tmpVal = reg_get_bits(tmpVal, SDH_EXE_TUNING);
	printf("[EMMC_CheckExeTuning] tmpVal:%d\r\n", tmpVal);
    return tmpVal;
}

uint32_t EMMC_CheckSamplingClock(SD_ID_Type sdId)
{
    uint16_t tmpVal;
	uint32_t SdBaseAddr = sdAddr[sdId];

    /* execute tuning */
    tmpVal = readw(SdBaseAddr + SDH_HOST_CTRL_2_OFFSET);
    tmpVal = reg_get_bits(tmpVal, SDH_SAMPLING_CLK_SEL);
	printf("[EMMC_CheckSamplingClock] tmpVal:%d\r\n", tmpVal);
    return tmpVal;
}

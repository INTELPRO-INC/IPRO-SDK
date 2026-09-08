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

#include "drv_glb.h"
#include "hal_sdh.h"

#if SDIO_SDCARD_INT_MODE
#define SDH_SDCARD_IRQn                 SMIH0_IRQn
#endif

/* Private variables ---------------------------------------------------------*/
static uint32_t sdhClockInit[SD_ID_MAX] = {400000ul};
#ifdef FPGA
static uint32_t sdhClockSrc[SD_ID_MAX] = {50000000ul/2};
static uint32_t sdhClockTransfer[SD_ID_MAX] = {50000000ul/2};
#else
static uint32_t sdhClockSrc[SD_ID_MAX] = {96000000ul};
static uint32_t sdhClockTransfer[SD_ID_MAX] = {48000000ul};
#endif

#if SDH_DEBUG
//const char *mmcSpeedStr[] = {"Default", "High Speed", "HS200", "HS400"};
//const char *mmcBusWidthStr[] = {"1-bit", "4-bit", "8-bit", "RSV", "RSV", "DDR 4-bit", "DDR 8-bit"};
#endif


#if SDIO_SDCARD_INT_MODE
static QueueHandle_t sdhIntSemphr;
#endif
static sd_card_t *pSDCardInfo[SD_ID_MAX];
static SDH_Cfg_Type SDH_Cfg_Type_Instance[SD_ID_MAX];

#if SDIO_SDCARD_INT_MODE
static volatile SD_Error SDH_DataWaitStatus = SD_WAITING;
static volatile SD_Error SDH_CMDWaitStatus = SD_WAITING;

static SDH_Trans_Callback_Cfg_Type SDH_Trans_Callback_Cfg_TypeInstance;
static SDH_Handle_Cfg_Type SDH_Handle_Cfg_TypeInstance;
#endif

static SDH_DMA_Cfg_Type SDH_DMA_Cfg_TypeInstance[SD_ID_MAX];
/*causion: ADMA related variables must on OCRAM or shared ram*/
static ATTR_DTCM_SECTION __EALIGN(64) SDH_ADMA2_Desc_Type adma2Entries[SD_ID_MAX][32];

/* Private function prototypes -----------------------------------------------*/
#if SDIO_SDCARD_INT_MODE
static void SDH_INT_Init(void);
#endif

static void SD_DecodeCid(sd_card_t *card, uint32_t *rawCid)
{
    sd_cid_t *cid;

    cid = &(card->cid);
    cid->manufacturerID = (uint8_t)((rawCid[3U] & 0xFF0000U) >> 16U);
    cid->applicationID = (uint16_t)((rawCid[3U] & 0xFFFFU) >> 0U);

    cid->productName[0U] = (uint8_t)((rawCid[1U] & 0xFF000000U) >> 24);
    cid->productName[1U] = (uint8_t)((rawCid[2U] & 0xFF) >> 0U);
    cid->productName[2U] = (uint8_t)((rawCid[2U] & 0xFF00U) >> 8U);
    cid->productName[3U] = (uint8_t)((rawCid[2U] & 0xFF0000U) >> 16U);
    cid->productName[4U] = (uint8_t)((rawCid[2U] & 0xFF000000U) >> 24U);

    cid->productVersion = (uint8_t)((rawCid[1U] & 0xFF0000U) >> 16U);

    cid->productSerialNumber = (uint32_t)((rawCid[1U] & 0xFFFFU) << 16U);
    cid->productSerialNumber |= (uint32_t)((rawCid[0U] & 0xFFFF0000U) >> 16U);

    cid->manufacturerData = (uint16_t)((rawCid[0U] & 0xFFFU) >> 0U);
}
static void SD_DecodeCsd(sd_card_t *card, uint32_t *rawCsd)
{
    sd_csd_t *csd;

    csd = &(card->csd);
    csd->csdStructure = (uint8_t)((rawCsd[3U] & 0xC00000U) >> 22U);
    csd->dataReadAccessTime1 = (uint8_t)((rawCsd[3U] & 0xFF00U) >> 8U);
    csd->dataReadAccessTime2 = (uint8_t)((rawCsd[3U] & 0xFFU) >> 0U);

    csd->transferSpeed = (uint8_t)((rawCsd[2U] & 0xFF000000U) >> 24);
    csd->cardCommandClass = (uint16_t)((rawCsd[2U] & 0xFFF000U) >> 12U);
    csd->readBlockLength = (uint8_t)((rawCsd[2U] & 0xF00U) >> 8U);

    switch (csd->csdStructure) {
        /*csd version 1.1*/
        case 0:
            csd->deviceSize = (uint32_t)((rawCsd[2U] & 0x3U) << 10U);
            csd->deviceSize |= (uint32_t)((rawCsd[1U] & 0xFFC00000U) >> 22U);

            csd->deviceSizeMultiplier = (uint8_t)((rawCsd[1U] & 0x380U) >> 7U);

            /* Get card total block count and block size. */
            card->blockCount = ((csd->deviceSize + 1U) << (csd->deviceSizeMultiplier + 2U));
            card->blockSize = (1U << (csd->readBlockLength));

            if (card->blockSize != SDH_DEFAULT_BLOCK_SIZE) {
                card->blockCount = (card->blockCount * card->blockSize);
                card->blockSize = SDH_DEFAULT_BLOCK_SIZE;
                card->blockCount = (card->blockCount / card->blockSize);
            }

            break;

        /*csd version 2.0*/
        case 1:
            card->blockSize = SDH_DEFAULT_BLOCK_SIZE;
            csd->deviceSize = (uint32_t)((rawCsd[1U] & 0x3FFFFF00U) >> 8U);

            if (csd->deviceSize >= 0xFFFFU) {
                card->flags |= SD_SupportSdxcFlag;
            }

            card->blockCount = ((csd->deviceSize + 1U) * 1024U);
            break;
        /*csd for emmc*/
        case 3:
            csd->deviceSize = (uint32_t)((rawCsd[2U] & 0x3U) << 10U);
            csd->deviceSize |= (uint32_t)((rawCsd[1U] & 0xFFC00000U) >> 22U);
            csd->deviceSizeMultiplier = (uint8_t)((rawCsd[1U] & 0x380U) >> 7U);

            /* Get card total block count and block size. */
            card->blockCount = ((csd->deviceSize + 1U) << (csd->deviceSizeMultiplier + 2U));
            card->blockSize = (1U << (csd->readBlockLength));

            if (card->blockSize != SDH_DEFAULT_BLOCK_SIZE) {
                card->blockCount = (card->blockCount * card->blockSize);
                card->blockSize = SDH_DEFAULT_BLOCK_SIZE;
                card->blockCount = (card->blockCount / card->blockSize);
            }
            break;

        default:
            break;
    }
}
static void SD_DecodeScr(sd_card_t *card, uint32_t *rawScr)
{
    sd_scr_t *scr;

    scr = &(card->scr);
    scr->scrStructure = (uint8_t)((rawScr[1U] & 0xF0000000U) >> 28U);
    scr->sdSpecification = (uint8_t)((rawScr[1U] & 0xF000000U) >> 24U);

    if ((uint8_t)((rawScr[1U] & 0x800000U) >> 23U)) {
        scr->flags |= SD_ScrDataStatusAfterErase;
    }

    scr->sdSecurity = (uint8_t)((rawScr[1U] & 0x700000U) >> 20U);
    scr->sdBusWidths = (uint8_t)((rawScr[1U] & 0xF0000U) >> 16U);

    if ((uint8_t)((rawScr[0U] & 0x8000U) >> 15U)) {
        scr->flags |= SD_ScrSdSpecification3;
    }

    scr->extendedSecurity = (uint8_t)((rawScr[1U] & 0x7800U) >> 10U);
    scr->commandSupport = (uint8_t)(rawScr[1U] & 0x3U);
    scr->reservedForManufacturer = rawScr[0U];

    /* Get specification version. */
    switch (scr->sdSpecification) {
        case 0U:
            card->version = SD_SpecificationVersion1_0;
            break;

        case 1U:
            card->version = SD_SpecificationVersion1_1;
            break;

        case 2U:
            card->version = SD_SpecificationVersion2_0;

            if (card->scr.flags & SD_ScrSdSpecification3) {
                card->version = SD_SpecificationVersion3_0;
            }

            break;

        default:
            break;
    }

    if (card->scr.sdBusWidths & 0x4U) {
        card->flags |= SD_Support4BitWidthFlag;
    }

    /* speed class control cmd */
    if (card->scr.commandSupport & 0x01U) {
        card->flags |= SD_SupportSpeedClassControlCmd;
    }

    /* set block count cmd */
    if (card->scr.commandSupport & 0x02U) {
        card->flags |= SD_SupportSetBlockCountCmd;
    }
}

#if SDIO_SDCARD_INT_MODE
/*!< SDH transfer complete callback */
void SDH_DataTransferFinished_CallBack(SDH_Handle_Cfg_Type *handle, SDH_Stat_Type status, void *userData)
{
    if (status != SDH_STAT_SUCCESS) {
        SDH_DataWaitStatus = SD_DataCfg_ERROR;
    } else {
        SDH_DataWaitStatus = SD_OK;
    }
}
/*!< SDH transfer complete callback */
void SDH_CMDTransferFinished_CallBack(SDH_Handle_Cfg_Type *handle, SDH_Stat_Type status, void *userData)
{
    if (status != SDH_STAT_SUCCESS) {
        SDH_CMDWaitStatus = SD_CMD_ERROR;
    } else {
        SDH_CMDWaitStatus = SD_OK;
    }
}

void SDH_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t intFlag, intMask;

    intMask = SDH_GetIntEnableStatus(0);
    intFlag = SDH_GetIntStatus(0);
    //printf("intFlag = 0x%lx, 0x%lx, 0x%lx\r\n", intFlag, intMask, intFlag & intMask);
    intFlag &= intMask;

    /* Card insert interrupt */
    if (intFlag & SDH_INT_CARD_INSERT) {
    }

    /* Card remove interrupt */
    if (intFlag & SDH_INT_CARD_REMOVE) {
    }

    /* Command error interrupt */
    if (intFlag & (SDH_INT_CMD_ERRORS)) {
        SDH_CMDWaitStatus = SD_CMD_ERROR;
    }

    /* Command success interrupt */
    if (intFlag & SDH_INT_CMD_COMPLETED) {
        SDH_CMDWaitStatus = SD_OK;
    }

    /* Data error interrupt */
    if (intFlag & (SDH_INT_DATA_ERRORS | SDH_INT_DMA_ERROR)) {
        SDH_DataWaitStatus = SD_DataCfg_ERROR;
    }

    /* Data success interrupt*/
    if (intFlag & SDH_INT_DATA_COMPLETED) {
        SDH_DataWaitStatus = SD_OK;
    }

    /* Card interrupt */
    if (intFlag & SDH_INT_CARD) {
    }

    /* Buffer read ready interrupt*/
    if (intFlag & SDH_INT_BUFFER_READ_READY) {
        SDH_DataWaitStatus = SD_OK;
    }

    /* Buffer write ready interrupt */
    if (intFlag & SDH_INT_BUFFER_WRITE_READY) {
        SDH_DataWaitStatus = SD_OK;
    }

    /* DMA success interrupt*/
    if (intFlag & SDH_INT_DMA_COMPLETED) {
    }

    /* Block gap event interrupt */
    if (intFlag & SDH_INT_BLK_GAP_EVENT) {
    }

    /* Re-tuning event interrupt */
    if (intFlag & SDH_INT_RETUNE_EVENT) {
    }
    SDH_ClearIntStatus(intFlag, 0);
    //printf("Before give %u\r\n", uxSemaphoreGetCountFromISR(sdhIntSemphr));
    xSemaphoreGiveFromISR(sdhIntSemphr, &xHigherPriorityTaskWoken);
    //printf("After give %u\r\n", uxSemaphoreGetCountFromISR(sdhIntSemphr));
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

}

static int SDH_GetDataTransfer_Timeout(uint32_t transfer_size_kb, SD_ID_Type sdId)
{
    int transfer_bw = 0;
    if (SDH_Cfg_Type_Instance[sdId].dataWidth == SDH_DATA_BUS_WIDTH_1BIT) {
        transfer_bw = 2*1000;
    } else if (SDH_Cfg_Type_Instance[sdId].dataWidth == SDH_DATA_BUS_WIDTH_4BITS) {
        transfer_bw = 8*1000;
    } else if (SDH_Cfg_Type_Instance[sdId].dataWidth == SDH_DATA_BUS_WIDTH_8BITS) {
        transfer_bw = 16*1000;
    }
    return (transfer_size_kb / transfer_bw + 1)*1000*2;
}
/****************************************************************************/ /**
 * @brief  SDH INT init
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
static void SDH_INT_Init(void)
{
    sdhIntSemphr = xSemaphoreCreateCounting(1, 0);

    CPU_Interrupt_Enable(SDH_SDCARD_IRQn);
    Interrupt_Handler_Register(SDH_SDCARD_IRQn, SDH_IRQHandler);

    SDH_EnableIntStatus(SDH_INT_ALL);
    SDH_DisableIntSource(SDH_INT_ALL);
}
#endif

static status_t SDH_PIOTransfer(SDH_Data_Cfg_Type *SDH_Data_Cfg_TypeInstance, SD_ID_Type sdId)
{
    uint32_t time_node = (uint32_t)SDH_GET_TIME();
    SD_Error sd_status = SD_WAITING;

#if SDIO_SDCARD_INT_MODE

    SDH_DataWaitStatus = SD_WAITING;
    SDH_EnableIntSource(SDH_INT_BUFFER_READ_READY | SDH_INT_DATA_ERRORS | SDH_INT_DMA_ERROR | SDH_INT_AUTO_CMD12_ERROR);

    if (xSemaphoreTake(sdhIntSemphr, SDIO_CMDTIMEOUT_MS) == pdFAIL) {
        SDH_ERR_MSG("SDH_PIOTransfer wait buffer read ready timeout: %ld ms\r\n", (uint32_t)SDH_GET_TIME() - time_node);
        SDH_DisableIntSource(SDH_INT_BUFFER_READ_READY | SDH_INT_DATA_ERRORS | SDH_INT_DMA_ERROR | SDH_INT_AUTO_CMD12_ERROR);
        return Status_Timeout;
    }
    SDH_DisableIntSource(SDH_INT_BUFFER_READ_READY | SDH_INT_DATA_ERRORS | SDH_INT_DMA_ERROR | SDH_INT_AUTO_CMD12_ERROR);
    sd_status = SDH_DataWaitStatus;

    if (sd_status == SD_OK) {
        SDH_ReadDataPort(SDH_Data_Cfg_TypeInstance, sdId);
        SDH_DataWaitStatus = SD_WAITING;
        SDH_EnableIntSource(SDH_INT_DATA_COMPLETED | SDH_INT_DATA_ERRORS | SDH_INT_DMA_ERROR | SDH_INT_AUTO_CMD12_ERROR);

        if (xSemaphoreTake(sdhIntSemphr, SDIO_CMDTIMEOUT_MS) == pdFAIL) {
            SDH_ERR_MSG("SDH_PIOTransfer wait transfer done timeout: %ld ms\r\n", (uint32_t)SDH_GET_TIME() - time_node);
            SDH_DisableIntSource(SDH_INT_DATA_COMPLETED | SDH_INT_DATA_ERRORS | SDH_INT_DMA_ERROR | SDH_INT_AUTO_CMD12_ERROR);
            return Status_Timeout;
        }
        SDH_DisableIntSource(SDH_INT_DATA_COMPLETED | SDH_INT_DATA_ERRORS | SDH_INT_DMA_ERROR | SDH_INT_AUTO_CMD12_ERROR);
        sd_status = SDH_DataWaitStatus;
    }

#else
    uint32_t intFlag = 0;
    while (1) {
        intFlag = SDH_GetIntStatus(sdId);
        if (intFlag & SDH_INT_DATA_ERRORS || intFlag & SDH_INT_DMA_ERROR) {
            sd_status = SD_DataCfg_ERROR;
            break;

        } else if (intFlag & SDH_INT_BUFFER_READ_READY) {
            sd_status = SD_OK;
            break;

        } else if ((uint32_t)SDH_GET_TIME() - time_node > SDIO_DATATIMEOUT_MS) {
            SDH_ERR_MSG("SDH_PIOTransfer timeout: %ld ms\r\n", (uint32_t)SDH_GET_TIME() - time_node);
            return Status_Timeout;
        }
    }

    if (sd_status == SD_OK) {
        SDH_ReadDataPort(SDH_Data_Cfg_TypeInstance, sdId);
        while (1) {
            intFlag = SDH_GetIntStatus(sdId);
            if (intFlag & SDH_INT_DATA_COMPLETED) {
                sd_status = SD_OK;
                break;
            } else if (intFlag & SDH_INT_DATA_ERRORS || intFlag & SDH_INT_DMA_ERROR) {
                sd_status = SD_DataCfg_ERROR;
                break;
            } else if ((uint32_t)SDH_GET_TIME() - time_node > SDIO_DATATIMEOUT_MS) {
                SDH_ERR_MSG("SDH_PIOTransfer timeout: %ld ms\r\n", (uint32_t)SDH_GET_TIME() - time_node);
                return Status_Timeout;
            }
        }
    }
    SDH_ClearIntStatus(SDH_INT_DATA_ERRORS | SDH_INT_DMA_ERROR | SDH_INT_DATA_COMPLETED | SDH_INT_BUFFER_READ_READY, sdId);

#endif
    return sd_status;
}

static status_t SDH_SendCardCommand(SDH_CMD_Cfg_Type *cmd, int retries, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SD_Error sd_status;
    uint32_t time_node;
    uint32_t intFlag = 0;

    if (retries == 0)
        retries = 1;

    while (retries-- > 0) {
//        SDH_MSG("Send CMD %lu retry: %d\r\n", cmd->index, retries);
        SDH_ClearIntStatus(SDH_INT_CMD_COMPLETED | SDH_INT_CMD_ERRORS, sdId);
        SDH_SendCommand(cmd, sdId);
        time_node = (uint32_t)SDH_GET_TIME();

#if SDIO_SDCARD_INT_MODE

        SDH_CMDWaitStatus = SD_WAITING;
        SDH_EnableIntSource(SDH_INT_CMD_COMPLETED | SDH_INT_CMD_ERRORS);

        if (xSemaphoreTake(sdhIntSemphr, SDIO_CMDTIMEOUT_MS) == pdFAIL) {
            SDH_ERR_MSG("SDH send CMD%ld timeout: %ld ms\r\n", cmd->index, (uint32_t)SDH_GET_TIME() - time_node);
            SDH_DisableIntSource(SDH_INT_CMD_COMPLETED | SDH_INT_CMD_ERRORS);
            return Status_Timeout;
        }

        sd_status = SDH_CMDWaitStatus;
        SDH_DisableIntSource(SDH_INT_CMD_COMPLETED | SDH_INT_CMD_ERRORS);

#else

        while (1) {
            intFlag = SDH_GetIntStatus(sdId);
            if (intFlag & SDH_INT_CMD_ERRORS) {
                sd_status = SD_CMD_ERROR;
                break;

            } else if (intFlag & SDH_INT_CMD_COMPLETED) {
                sd_status = SD_OK;
                break;

            } else if ((uint32_t)SDH_GET_TIME() - time_node > SDIO_CMDTIMEOUT_MS) {
                SDH_MSG("SDH send CMD%ld timeout: %ld ms\r\n", cmd->index, (uint32_t)SDH_GET_TIME() - time_node);
                sd_status = SD_CMD_ERROR;
                break;
            }
        }
        SDH_ClearIntStatus(intFlag, sdId);

#endif

        // SDH_MSG("SDH send CMD%ld used time : %ld\r\n", cmd->index, (uint32_t)SDH_GET_TIME() - time_node);

        if (sd_status != SD_OK) {
            SDH_MSG("SDH send CMD%ld error, intFlag = 0x%lx\r\n", cmd->index, intFlag);
            errorstatus = Status_SDH_CmdResponseError;
        } else {
            SDH_GetCmdResp(cmd, sdId);
            errorstatus = Status_Success;
            //SDH_MSG("SDH send CMD%ld success\r\n", cmd->index);
            break;
        }
    }

    return errorstatus;
}

static status_t SDH_CardTransferBlocking(SDH_DMA_Cfg_Type *dmaCfg, SDH_Trans_Cfg_Type *transfer, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_Stat_Type stat = SDH_STAT_SUCCESS;
    stat = SDH_TransferBlocking(dmaCfg, transfer, sdId);
    if (stat != SDH_STAT_SUCCESS) {
        SDH_ERR_MSG("Transfer Error: %d\r\n", stat);
        return Status_SDH_TransferFailed;
    }
    return errorstatus;
}

static void SDH_HostInit(SD_ID_Type sdId, uint8_t mmcBootMode)
{

    /* initialise SDH controller*/
    SDH_Cfg_Type_Instance[sdId].vlot18Enable = DISABLE;
    SDH_Cfg_Type_Instance[sdId].highSpeed = DISABLE;
    SDH_Cfg_Type_Instance[sdId].dataWidth = SDH_DATA_BUS_WIDTH_1BIT;
    SDH_Cfg_Type_Instance[sdId].volt = SDH_VOLTAGE_3P3V;
    SDH_Cfg_Type_Instance[sdId].srcClock = sdhClockSrc[sdId];
    if (mmcBootMode)
        SDH_Cfg_Type_Instance[sdId].busClock = sdhClockTransfer[sdId];
    else
        SDH_Cfg_Type_Instance[sdId].busClock = sdhClockInit[sdId];
    SDH_Ctrl_Init(&SDH_Cfg_Type_Instance[sdId], sdId);

    /*setup timeout counter*/
    SDH_Set_Timeout(0x0e, sdId);

    /*power on host controller*/
    SDH_Powon(sdId);
}

/*
* GO_IDLE_STATE, send card to reset state
*/
static status_t SDH_GoIdle(SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;

    /*CMD0: GO_IDLE_STATE, send card to reset state*/
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_GO_IDLE_STATE;
    SDH_CMD_Cfg_TypeInstance.argument = 0;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_NONE;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 0, sdId);
    return errorstatus;
}

static status_t SDH_GoPreIdle(SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;

    /*CMD0: GO_IDLE_STATE, send card to reset state*/
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_GO_IDLE_STATE;
    SDH_CMD_Cfg_TypeInstance.argument = 0xF0F0F0F0;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_NONE;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 0, sdId);
    return errorstatus;
}

static status_t SD_SendApplicationCmd(uint32_t relativeAddress, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;

    /* send CMD55 */
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_APP_CMD;
    SDH_CMD_Cfg_TypeInstance.argument = relativeAddress;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R1;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 0, sdId);

    if (errorstatus != Status_Success) {
        return Status_SDH_CmdResponseError;
    } else if (SDH_CMD_Cfg_TypeInstance.response[0] & SD_CSR_ERRORBITS) {
        return Status_SDH_CmdResponseError;
    }

    return errorstatus;
}

static status_t SD_SendInterfaceCondition(SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;

    /* CMD8: SEND_IF_COND */
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_HS_SEND_EXT_CSD;
    SDH_CMD_Cfg_TypeInstance.argument = SD_CHECK_PATTERN;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R7;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 3, sdId);

    if (errorstatus != Status_Success) {
        return Status_SDH_CmdResponseError;
    }

    SDH_MSG("Response to CMD8 is: 0x%02lX.\r\n", SDH_CMD_Cfg_TypeInstance.response[0]);

    if ((SDH_CMD_Cfg_TypeInstance.response[0U] & 0xFFU) != (SD_CHECK_PATTERN & 0xff)) {
        return Status_SDH_CardNotSupport;
    }

    return errorstatus;
}

static status_t SD_ApplicationSendOperationCondition(sd_card_t *card, uint32_t argument, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    uint32_t response = 0, count = 0, validvoltage = 0;
    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;

    do {
        if (Status_Success != (errorstatus = SD_SendApplicationCmd(0, sdId))) {
            return errorstatus;
        }

        /*ACMD41*/
        SDH_CMD_Cfg_TypeInstance.index = SD_CMD_SD_APP_OP_COND;
        SDH_CMD_Cfg_TypeInstance.argument = argument;
        SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
        SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R3;
        SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;

        errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 0, sdId);

        if (errorstatus != Status_Success) {
            return Status_SDH_CmdResponseError;
        }

        response = SDH_CMD_Cfg_TypeInstance.response[0];
        validvoltage = (((response >> 31) == 1) ? 1 : 0);
        count++;
    } while ((!validvoltage) && (count < SD_MAX_VOLT_TRIAL));

    if (count == SD_MAX_VOLT_TRIAL) {
        return Status_Timeout;
    } else {
        card->ocr = response;

        if (response &= SD_OcrHostCapacitySupportFlag) {
            /* change from sdsc to sdhc */
            card->flags |= SD_SupportHighCapacityFlag;
            card->access_mode = 1;
        }
    }

    return errorstatus;
}
static status_t SD_AllSendCid(sd_card_t *card, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;

    /* CMD2: SD_CMD_ALL_SEND_CID */
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_ALL_SEND_CID;
    SDH_CMD_Cfg_TypeInstance.argument = 0;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R2;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 3, sdId);

    if (errorstatus != Status_Success) {
        return Status_SDH_CmdResponseError;
    }

    card->rawCid[0] = SDH_CMD_Cfg_TypeInstance.response[0];
    card->rawCid[1] = SDH_CMD_Cfg_TypeInstance.response[1];
    card->rawCid[2] = SDH_CMD_Cfg_TypeInstance.response[2];
    card->rawCid[3] = SDH_CMD_Cfg_TypeInstance.response[3];

    SD_DecodeCid(card, card->rawCid);

    return errorstatus;
}

static status_t SD_SendRca(sd_card_t *card, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;

    /*CMD3: send relative card address*/
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_SET_REL_ADDR;
    SDH_CMD_Cfg_TypeInstance.argument = 0;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R6;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 3, sdId);

    if (errorstatus != Status_Success) {
        return Status_SDH_CmdResponseError;
    }

    card->relativeAddress = SDH_CMD_Cfg_TypeInstance.response[0] >> 16;

    return errorstatus;
}

static status_t SD_SendCsd(sd_card_t *card, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;

    /*CMD9: send card-specific data(CSD)*/
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_SEND_CSD;
    SDH_CMD_Cfg_TypeInstance.argument = (uint32_t)((card->relativeAddress) << 16);
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R2;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 3, sdId);

    if (errorstatus != Status_Success) {
        return Status_SDH_CmdResponseError;
    }

    card->rawCsd[0] = SDH_CMD_Cfg_TypeInstance.response[0];
    card->rawCsd[1] = SDH_CMD_Cfg_TypeInstance.response[1];
    card->rawCsd[2] = SDH_CMD_Cfg_TypeInstance.response[2];
    card->rawCsd[3] = SDH_CMD_Cfg_TypeInstance.response[3];

    SD_DecodeCsd(card, card->rawCsd);

    return errorstatus;
}

static status_t SD_SelectCard(sd_card_t *card, int NewState, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;

    /* CMD7: select/deselect specified card */
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_SEL_DESEL_CARD;

    if (NewState == ENABLE) {
        SDH_CMD_Cfg_TypeInstance.argument = (uint32_t)((card->relativeAddress) << 16);
        SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R1B;
    } else {
        SDH_CMD_Cfg_TypeInstance.argument = 0;
        SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_NONE;
    }

    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 0, sdId);

    if (errorstatus != Status_Success) {
        return Status_SDH_TransferFailed;
    } else if ((NewState == ENABLE) && (SDH_CMD_Cfg_TypeInstance.response[0] & SD_CSR_ERRORBITS)) {
        return Status_SDH_CmdResponseError;
    }

    return errorstatus;
}

/* get CSR */
static status_t SD_SendScr(sd_card_t *card, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_Stat_Type stat = SDH_STAT_SUCCESS;
    SD_Error sd_status;

    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;
    SDH_Data_Cfg_Type SDH_Data_Cfg_TypeInstance;
    uint32_t tempscr[2] = { 0, 0 };
    uint32_t intFlag = 0;

    /* send CMD55 */
    errorstatus = SD_SendApplicationCmd((uint32_t)((card->relativeAddress) << 16), sdId);

    if (errorstatus != Status_Success) {
        goto out;
    }

    /*!< Set Block Size To 8 Bytes */
    SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = DISABLE;
    SDH_Data_Cfg_TypeInstance.enableAutoCommand23 = DISABLE;
    SDH_Data_Cfg_TypeInstance.enableIgnoreError = DISABLE;
    SDH_Data_Cfg_TypeInstance.dataType = SDH_TRANS_DATA_NORMAL;
    SDH_Data_Cfg_TypeInstance.blockSize = 8;
    SDH_Data_Cfg_TypeInstance.blockCount = 1;
    SDH_Data_Cfg_TypeInstance.rxDataLen = 0;
    SDH_Data_Cfg_TypeInstance.rxData = tempscr;
    SDH_Data_Cfg_TypeInstance.txDataLen = 0;
    SDH_Data_Cfg_TypeInstance.txData = NULL;
    /* Config the data transfer parameter */
    stat = SDH_ConfigDataTranfer(&SDH_Data_Cfg_TypeInstance, sdId);

    if (SDH_STAT_SUCCESS != stat) {
        return Status_SDH_TransferFailed;
    }

    /*!< Send ACMD51 SD_APP_SEND_SCR with argument as 0 */
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_SD_APP_SEND_SCR;
    SDH_CMD_Cfg_TypeInstance.argument = 0;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R1;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_DATA_PRESENT;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 3, sdId);

    if (errorstatus != Status_Success) {
        goto out;
    } else if (SDH_CMD_Cfg_TypeInstance.response[0] & SD_CSR_ERRORBITS) {
        errorstatus = Status_SDH_CmdResponseError;
        goto out;
    }

    sd_status = SDH_PIOTransfer(&SDH_Data_Cfg_TypeInstance, sdId);

    if (sd_status == SD_OK) {
        card->rawScr[1] = ((tempscr[0] & SD_0TO7BITS) << 24) | ((tempscr[0] & SD_8TO15BITS) << 8) | ((tempscr[0] & SD_16TO23BITS) >> 8) | ((tempscr[0] & SD_24TO31BITS) >> 24);
        card->rawScr[0] = ((tempscr[1] & SD_0TO7BITS) << 24) | ((tempscr[1] & SD_8TO15BITS) << 8) | ((tempscr[1] & SD_16TO23BITS) >> 8) | ((tempscr[1] & SD_24TO31BITS) >> 24);
        SD_DecodeScr(card, card->rawScr);
        SDH_MSG("SDH get csr success\r\n");
    } else {
        errorstatus = Status_SDH_TransferFailed;
        SDH_ERR_MSG("SDH get csr failed, intFlag = 0x%lx\r\n", intFlag);
        goto out;
    }

out:
    return errorstatus;
}

/* get SSR */
static status_t SD_SendSsr(sd_card_t *card, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_Stat_Type stat = SDH_STAT_SUCCESS;
    SD_Error sd_status;
    uint32_t intFlag = 0;

    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;
    SDH_Data_Cfg_Type SDH_Data_Cfg_TypeInstance;

    errorstatus = SD_SendApplicationCmd((uint32_t)((card->relativeAddress) << 16), sdId);

    if (errorstatus != Status_Success) {
        goto out;
    }

    /*!< Set Block Size To 512 Bytes */
    SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = DISABLE;
    SDH_Data_Cfg_TypeInstance.enableAutoCommand23 = DISABLE;
    SDH_Data_Cfg_TypeInstance.enableIgnoreError = DISABLE;
    SDH_Data_Cfg_TypeInstance.dataType = SDH_TRANS_DATA_NORMAL;
    SDH_Data_Cfg_TypeInstance.blockSize = 64;
    SDH_Data_Cfg_TypeInstance.blockCount = 1;
    SDH_Data_Cfg_TypeInstance.rxDataLen = 0;
    SDH_Data_Cfg_TypeInstance.rxData = card->rawSsr;
    SDH_Data_Cfg_TypeInstance.txDataLen = 0;
    SDH_Data_Cfg_TypeInstance.txData = NULL;
    /* Config the data transfer parameter */
    stat = SDH_ConfigDataTranfer(&SDH_Data_Cfg_TypeInstance, sdId);

    if (SDH_STAT_SUCCESS != stat) {
        return Status_SDH_TransferFailed;
    }

    /*!< Send ACMD13 SD_APP_SEND_SCR with argument as 0 */
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_SD_APP_STAUS;
    SDH_CMD_Cfg_TypeInstance.argument = 0;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R1;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_DATA_PRESENT;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 3, sdId);

    if (errorstatus != Status_Success) {
        goto out;
    } else if (SDH_CMD_Cfg_TypeInstance.response[0] & SD_CSR_ERRORBITS) {
        errorstatus = Status_SDH_CmdResponseError;
        goto out;
    }

    sd_status = SDH_PIOTransfer(&SDH_Data_Cfg_TypeInstance, sdId);

    if (sd_status == SD_OK) {
        SDH_MSG("SDH get ssr success, intFlag = 0x%lx\r\n", intFlag);
    } else {
        errorstatus = Status_SDH_TransferFailed;
        SDH_ERR_MSG("SDH get ssr failed, intFlag = 0x%lx\r\n", intFlag);
        goto out;
    }

out:
    return errorstatus;
}

/* Set Data Bus Width */
static status_t SD_SetDataBusWidth(sd_card_t *card, SDH_Data_Bus_Width_Type width, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;

    /* reinitialise SDH controller*/
    SDH_Cfg_Type_Instance[sdId].vlot18Enable = DISABLE;
    SDH_Cfg_Type_Instance[sdId].highSpeed = DISABLE;
    SDH_Cfg_Type_Instance[sdId].dataWidth = width;
    SDH_Cfg_Type_Instance[sdId].volt = SDH_VOLTAGE_3P3V;
    SDH_Cfg_Type_Instance[sdId].srcClock = sdhClockSrc[sdId];
    SDH_Cfg_Type_Instance[sdId].busClock = sdhClockTransfer[sdId];
    SDH_Ctrl_Init(&SDH_Cfg_Type_Instance[sdId], sdId);

    if (width == SDH_DATA_BUS_WIDTH_1BIT) {
        SDH_CMD_Cfg_TypeInstance.argument = 0;
        SDH_Cfg_Type_Instance[sdId].highSpeed = DISABLE;
    } else if (width == SDH_DATA_BUS_WIDTH_4BITS) {
        SDH_CMD_Cfg_TypeInstance.argument = 2;
        SDH_Cfg_Type_Instance[sdId].highSpeed = ENABLE;
    } else {
        return Status_InvalidArgument;
    }

    errorstatus = SD_SendApplicationCmd((uint32_t)((card->relativeAddress) << 16), sdId);

    if (errorstatus != Status_Success) {
        goto out;
    }

    /*!< Send ACMD6 APP_CMD with argument as 2 for wide bus mode */
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_APP_SD_SET_BUSWIDTH;

    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R1;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 0, sdId);

    if (errorstatus != Status_Success) {
        goto out;
    } else if (SDH_CMD_Cfg_TypeInstance.response[0] & SD_CSR_ERRORBITS) {
        errorstatus = Status_SDH_CmdResponseError;
        goto out;
    }

out:
    return errorstatus;
}

/* switch function
   mode: 0 check function, 1 set function
   group: group number,1~6
   number:
 */
static status_t SD_SwitchFunction(uint32_t mode, uint32_t group, uint32_t number, uint32_t status[16], SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_Stat_Type stat = SDH_STAT_SUCCESS;
    SD_Error sd_status;

    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;
    SDH_Data_Cfg_Type SDH_Data_Cfg_TypeInstance;

    /*!< Set Block Size To 64 Bytes */
    SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = DISABLE;
    SDH_Data_Cfg_TypeInstance.enableAutoCommand23 = DISABLE;
    SDH_Data_Cfg_TypeInstance.enableIgnoreError = DISABLE;
    SDH_Data_Cfg_TypeInstance.dataType = SDH_TRANS_DATA_NORMAL;
    SDH_Data_Cfg_TypeInstance.blockSize = 64;
    SDH_Data_Cfg_TypeInstance.blockCount = 1;
    SDH_Data_Cfg_TypeInstance.rxDataLen = 0;
    SDH_Data_Cfg_TypeInstance.rxData = status;
    SDH_Data_Cfg_TypeInstance.txDataLen = 0;
    SDH_Data_Cfg_TypeInstance.txData = NULL;
    /* Config the data transfer parameter */
    stat = SDH_ConfigDataTranfer(&SDH_Data_Cfg_TypeInstance, sdId);

    if (SDH_STAT_SUCCESS != stat) {
        return Status_SDH_TransferFailed;
    }

    /*!< Send CMD6 SD_CMD_HS_SWITCH with argument as 0 */
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_HS_SWITCH;
    SDH_CMD_Cfg_TypeInstance.argument = (mode << 31U | 0x00FFFFFFU);
    SDH_CMD_Cfg_TypeInstance.argument &= ~((uint32_t)(0xFU) << (group * 4U));
    SDH_CMD_Cfg_TypeInstance.argument |= (number << (group * 4U));
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R1;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_DATA_PRESENT;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 3, sdId);

    if (errorstatus != Status_Success) {
        goto out;
    } else if (SDH_CMD_Cfg_TypeInstance.response[0] & SD_CSR_ERRORBITS) {
        errorstatus = Status_SDH_CmdResponseError;
        goto out;
    }

    sd_status = SDH_PIOTransfer(&SDH_Data_Cfg_TypeInstance, sdId);

    if (sd_status == SD_OK) {
        SDH_MSG("SDH get CMD6 status data success\r\n");
    } else {
        errorstatus = Status_SDH_TransferFailed;
        SDH_ERR_MSG("SDH get CMD6 status data failed\r\n");
        goto out;
    }

out:
    return errorstatus;
}

/*  */
static ATTR_USED status_t SD_SelectFunction(uint32_t group, uint32_t function, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    uint32_t cmd6Status[16] = { 0 };
    uint16_t functionGroupInfo[6U] = { 0 };
    uint32_t currentFunctionStatus = 0U;

    uint32_t i;

    /* Check if card support high speed mode. */
    if (Status_Success != SD_SwitchFunction(SDH_SwitchCheck, group, function, cmd6Status, sdId)) {
        return Status_SDH_SDIO_SwitchHighSpeedFail;
    }

    for (i = 0; i < 16; i++) {
        SDH_MSG("cmd6Status[%ld]=0x%lX.\r\n", i, cmd6Status[i]);
    }

    /* In little endian mode, SD bus byte transferred first is the byte stored in lowest byte position in
    a word which will cause 4 byte's sequence in a word is not consistent with their original sequence from
    card. So the sequence of 4 bytes received in a word should be converted. */
    cmd6Status[0U] = SWAP_WORD_BYTE_SEQUENCE(cmd6Status[0U]);
    cmd6Status[1U] = SWAP_WORD_BYTE_SEQUENCE(cmd6Status[1U]);
    cmd6Status[2U] = SWAP_WORD_BYTE_SEQUENCE(cmd6Status[2U]);
    cmd6Status[3U] = SWAP_WORD_BYTE_SEQUENCE(cmd6Status[3U]);
    cmd6Status[4U] = SWAP_WORD_BYTE_SEQUENCE(cmd6Status[4U]);

    functionGroupInfo[5U] = (uint16_t)cmd6Status[0U];
    functionGroupInfo[4U] = (uint16_t)(cmd6Status[1U] >> 16U);
    functionGroupInfo[3U] = (uint16_t)(cmd6Status[1U]);
    functionGroupInfo[2U] = (uint16_t)(cmd6Status[2U] >> 16U);
    functionGroupInfo[1U] = (uint16_t)(cmd6Status[2U]);
    functionGroupInfo[0U] = (uint16_t)(cmd6Status[3U] >> 16U);
    currentFunctionStatus = ((cmd6Status[3U] & 0xFFFFU) << 8U) | (cmd6Status[4U] >> 24U);

    for (i = 0; i < 6; i++) {
        SDH_MSG("functionGroupInfo[%ld]=0x%X.\r\n", i, functionGroupInfo[i]);
    }

    SDH_MSG("currentFunctionStatus = 0x%lX.\r\n", currentFunctionStatus);

    /* check if function is support */
    if (((functionGroupInfo[group] & (1 << function)) == 0U) ||
        ((currentFunctionStatus >> (group * 4U)) & 0xFU) != function) {
        return Status_SDH_SDIO_SwitchHighSpeedFail;
    }

    /* Check if card support high speed mode. */
    if (Status_Success != SD_SwitchFunction(SDH_SwitchSet, group, function, cmd6Status, sdId)) {
        return Status_SDH_SDIO_SwitchHighSpeedFail;
    }

    /* In little endian mode is little endian, SD bus byte transferred first is the byte stored in lowest byte
    position in a word which will cause 4 byte's sequence in a word is not consistent with their original
    sequence from card. So the sequence of 4 bytes received in a word should be converted. */
    cmd6Status[3U] = SWAP_WORD_BYTE_SEQUENCE(cmd6Status[3U]);
    cmd6Status[4U] = SWAP_WORD_BYTE_SEQUENCE(cmd6Status[4U]);

    /* According to the "switch function status[bits 511~0]" return by switch command in mode "set function":
         -check if group 1 is successfully changed to function 1 by checking if bits 379~376 equal value 1;
     */
    currentFunctionStatus = ((cmd6Status[3U] & 0xFFFFU) << 8U) | (cmd6Status[4U] >> 24U);
    SDH_MSG("currentFunctionStatus = 0x%lX.\r\n", currentFunctionStatus);

    if (((currentFunctionStatus >> (group * 4U)) & 0xFU) != function) {
        return Status_SDH_SDIO_SwitchHighSpeedFail;
    }

    return errorstatus;
}

static status_t SD_SetBlockSize(uint32_t blockSize, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;

    /*!< Set Block Size for SDSC Card,cmd16,no impact on SDHC card */
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_SET_BLOCKLEN;
    SDH_CMD_Cfg_TypeInstance.argument = (uint32_t)blockSize;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R1;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 5, sdId);

    if (errorstatus != Status_Success) {
        goto out;
    } else if (SDH_CMD_Cfg_TypeInstance.response[0] & SD_CSR_ERRORBITS) {
        errorstatus = Status_SDH_CmdResponseError;
        goto out;
    }

out:
    return errorstatus;
}

/*check sd card state*/
static status_t IsCardProgramming(uint8_t *pstatus, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;

    /*cmd13 addressed card send its status*/
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_SEND_STATUS;
    SDH_CMD_Cfg_TypeInstance.argument = (uint32_t)(pSDCardInfo[sdId]->relativeAddress) << 16;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R1;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 0, sdId);

    if (errorstatus != Status_Success) {
        goto out;
    } else if (SDH_CMD_Cfg_TypeInstance.response[0] & SD_CSR_ERRORBITS) {
        errorstatus = Status_SDH_CmdResponseError;
        goto out;
    }

    /*!< Find out card status */
    *pstatus = (uint8_t)((SDH_CMD_Cfg_TypeInstance.response[0] >> 9) & 0x0000000F); //status[12:9] :cardstate

out:
    return (errorstatus);
}

static status_t WaitInProgramming(SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
#if 0
    //uint32_t maxdelay = 0;
    //maxdelay = 120000/(sdhClockSrc/sdhClockTransfer);

    //while(maxdelay--){}
    /*!< Wait till the card is in programming state */
    errorstatus = IsCardProgramming(&cardstate, sdId);

    while ((errorstatus == Status_Success) && ((SD_CARD_PROGRAMMING == cardstate) || (SD_CARD_RECEIVING == cardstate))) {
        errorstatus = IsCardProgramming(&cardstate, sdId);
    }
#else
    while (SDH_WaitDatIdle(sdId) == SDH_STAT_BUSY);
#endif
    return errorstatus;
}
#if 0
static void dump_extcsd_raw(uint8_t *extcsd_buf)
{
#if 1
    SDH_MSG("000: ");
    for (int i = 0; i < 512; i++) {
        if (i && i % 16 == 0)
            SDH_MSG("\r\n%03d: ", i);
        SDH_MSG("%02x ", extcsd_buf[i]);
    }
    SDH_MSG("\r\n");
#else
    for (int i = 0; i < 512; i++) {
        printf("%03d %02x\r\n", i, extcsd_buf[i]);
    }

#endif
}
#endif
void EMMC_SetRelWR(int enable, SD_ID_Type sdId)
{
    pSDCardInfo[sdId]->rel_wr = enable;
}

static void EMMC_ParseExtCSD(sd_card_t *card)
{
    SDH_MSG("EXT_CSD Information:\r\n");
    SDH_MSG(" CSD Structure Version: 0x%02x\r\n", card->ext_csd.csdStructure);
    SDH_MSG(" Extended CSD Revision: 0x%02x\r\n", card->ext_csd.csdRev);
    SDH_MSG(" Part Config: 0x%02x, Boot Size: %u KB, Boot Bus Config: 0x%02x\r\n", card->ext_csd.partConfig, card->ext_csd.bootMultSize*128, card->ext_csd.bootbusContions);
    SDH_MSG(" Card Type: 0x%02x\r\n", card->ext_csd.cardType);
    if (card->ext_csd.cardType & EXT_CSD_CARD_TYPE_HS_26)
        SDH_MSG("   Card can run at 26MHz\r\n");
    if (card->ext_csd.cardType & EXT_CSD_CARD_TYPE_DDR_1_8V)
        SDH_MSG("   Card can run at 52MHz,  DDR mode @1.8V or 3V I/O\r\n");
    else if (card->ext_csd.cardType & EXT_CSD_CARD_TYPE_HS_52)
        SDH_MSG("   Card can run at 52MHz\r\n");
    if (card->ext_csd.cardType & EXT_CSD_CARD_TYPE_DDR_1_2V)
        SDH_MSG("   Card can run at 52MHz,  DDR mode @1.2V I/O\r\n");
    if (card->ext_csd.cardType & EXT_CSD_CARD_TYPE_HS200_1_8V)
        SDH_MSG("   Card can run at 200MHz, SDR mode @1.8V I/O\r\n");
    if (card->ext_csd.cardType & EXT_CSD_CARD_TYPE_HS200_1_2V)
        SDH_MSG("   Card can run at 200MHz, SDR mode @1.2V I/O\r\n");
    if (card->ext_csd.cardType & EXT_CSD_CARD_TYPE_HS400_1_8V)
        SDH_MSG("   Card can run at 200MHz, DDR mode @1.8V I/O\r\n");
    if (card->ext_csd.cardType & EXT_CSD_CARD_TYPE_HS400_1_2V)
        SDH_MSG("   Card can run at 200MHz, DDR mode @1.2V I/O\r\n");
    if (card->ext_csd.cardType & EXT_CSD_CARD_TYPE_HS400ES)
        SDH_MSG("   Card can run at HS400ES\r\n");
    SDH_MSG("Write Reliability:\r\n");
    SDH_MSG("  REL_WR_SEC_C: %d\r\n", card->ext_csd.rel_wr_sec_count);
    SDH_MSG("  WR_REL_SET: 0x%x\r\n", card->ext_csd.rel_set);
    SDH_MSG("  WR_REL_PARAM: 0x%x\r\n", card->ext_csd.rel_param);
//    printf(" High Speed Interface Timing: %s\r\n", mmcSpeedStr[card->ext_csd.hsTiming]);
//    printf(" Bus Width: %s\r\n", mmcBusWidthStr[card->ext_csd.busWidth]);
    SDH_MSG(" Power Class: 0x%02x\r\n", card->ext_csd.powerClass);
    SDH_MSG(" Power Class for:\r\n");
    SDH_MSG("  26MHz @ 3.6V : 0x%02x\r\n", card->ext_csd.powerClass_26_360);
    SDH_MSG("  52MHz @ 3.6V : 0x%02x\r\n", card->ext_csd.powerClass_52_360);
    SDH_MSG("  26MHz @ 1.95V: 0x%02x\r\n", card->ext_csd.powerClass_26_195);
    SDH_MSG("  52MHz @ 1.95V: 0x%02x\r\n", card->ext_csd.powerClass_52_195);

//    dump_extcsd_raw((uint8_t *)&card->ext_csd);
}

static status_t EMMC_SendExtCsd(sd_card_t *card, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    static SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;
    static SDH_Data_Cfg_Type SDH_Data_Cfg_TypeInstance;
    static SDH_Trans_Cfg_Type SDH_Trans_Cfg_TypeInstance = { &SDH_Data_Cfg_TypeInstance, &SDH_CMD_Cfg_TypeInstance };
    uint32_t time_node __attribute__((unused)) ;

    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_HS_SEND_EXT_CSD;
    SDH_CMD_Cfg_TypeInstance.argument = 0;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R1;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_DATA_PRESENT;
    SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = DISABLE;
    SDH_Data_Cfg_TypeInstance.enableIgnoreError = DISABLE;
    SDH_Data_Cfg_TypeInstance.dataType = SDH_TRANS_DATA_NORMAL;
    SDH_Data_Cfg_TypeInstance.blockSize = 512;
    SDH_Data_Cfg_TypeInstance.blockCount = 1;
    SDH_Data_Cfg_TypeInstance.rxDataLen = 0;
    SDH_Data_Cfg_TypeInstance.rxData = (uint32_t *)&card->ext_csd;
    SDH_Data_Cfg_TypeInstance.txDataLen = 0;
    SDH_Data_Cfg_TypeInstance.txData = NULL;
    memset(adma2Entries[sdId], 0, sizeof(adma2Entries[sdId]));
    SDH_DMA_Cfg_TypeInstance[sdId].dmaMode = SDH_DMA_MODE_ADMA2;
    SDH_DMA_Cfg_TypeInstance[sdId].admaEntries = (uint32_t *)adma2Entries;
    SDH_DMA_Cfg_TypeInstance[sdId].maxEntries = sizeof(adma2Entries) / sizeof(adma2Entries[0]);
    L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)&card->ext_csd, sizeof(card->ext_csd));
    time_node = (uint32_t)SDH_GET_TIME();
    errorstatus = SDH_CardTransferBlocking(&SDH_DMA_Cfg_TypeInstance[sdId], &SDH_Trans_Cfg_TypeInstance, sdId);
    if (errorstatus != Status_Success) {
        SDH_ERR_MSG("SDH Transfer err:%ld\r\n", errorstatus);
    }

    EMMC_ParseExtCSD(card);
    return errorstatus;

}

static status_t EMMC_SwitchFunction(mmc_switch_mode_t mode, uint8_t index, uint8_t val, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;

    /* CMD6 */
    SDH_CMD_Cfg_TypeInstance.index = MMC_SWITCH;
    SDH_CMD_Cfg_TypeInstance.argument = (mode << 24) | (index << 16) | (val << 8) | EXT_CSD_CMD_SET_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R1B;

    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;
    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 3, sdId);

    if (errorstatus != Status_Success) {
        return Status_SDH_TransferFailed;
    } else if (SDH_CMD_Cfg_TypeInstance.response[0] & SD_CSR_ERRORBITS) {
        errorstatus = Status_SDH_CmdResponseError;
        return errorstatus;
    }
    return errorstatus;
}

/* for emmc */
static status_t EMMC_SendOPCondition(sd_card_t *card, uint32_t argument, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    uint32_t count=0;
    int busy = 1;
    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;
    do {
        SDH_MSG("Send CMD1 with argument 0x%lx\r\n", argument);

        /* CMD1: SEND_OP_COND */
        SDH_CMD_Cfg_TypeInstance.index = SD_CMD_SEND_OP_COND;
        SDH_CMD_Cfg_TypeInstance.argument = argument; //0x40300000;//0x80FF8080;//0xC0FF8000;
        SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
        SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R3;
        SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;

        errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 0, sdId);

        if (errorstatus != Status_Success) {
            return Status_SDH_CmdResponseError;
        }

        SDH_MSG("Response to CMD1 is: 0x%02lX.\r\n", SDH_CMD_Cfg_TypeInstance.response[0]);
        count++;
        if (SDH_CMD_Cfg_TypeInstance.response[0] & EMMC_OCR_BUSY) {
            busy = 0;
            break;
        }
        arch_delay_ms(4);
    } while (count < 1000/4); // TIMEOUT: 1s, send ocr period: 4ms

    if (busy == 1) {
        SDH_MSG("EMMC Busy for 1 second\r\n");
        return Status_SDH_CmdResponseError;
    }

    if ((SDH_CMD_Cfg_TypeInstance.response[0U] & 0xFFU) != (0x40FF8080 & 0xff)) {
        return Status_SDH_CardNotSupport;
    }else{
        card->ocr = SDH_CMD_Cfg_TypeInstance.response[0];
        if (SDH_CMD_Cfg_TypeInstance.response[0] &= EMMC_SECTOR_MODE) {
            card->access_mode = 1;
            SDH_MSG("emmc access mode is sector mode. \r\n");
        }
    }

    return errorstatus;
}
static status_t EMMC_SetRca(sd_card_t *card, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;

    /*CMD3: send relative card address*/
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_SET_REL_ADDR;
    SDH_CMD_Cfg_TypeInstance.argument = (uint32_t)((card->relativeAddress) << 16);
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R6;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 0, sdId);

    if (errorstatus != Status_Success) {
        return Status_SDH_CmdResponseError;
    }

    return errorstatus;
}

static status_t SDH_SDCardInit(uint32_t bus_wide, sd_card_t *card, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    uint32_t applicationCommand41Argument = SD_OcrVdd33_34Flag | SD_OcrVdd32_33Flag;

    /* reset variables */
    card->flags = 0U;

    SDH_MSG("SD CARD GO IDEL...\r\n");
    errorstatus = SDH_GoIdle(sdId);

    if (errorstatus != SD_OK) {
        return Status_SDH_GoIdleFailed;
    }

    SDH_MSG("SD CARD GO IDEL END\r\n");

    for (uint16_t i = 0; i < 4; i++) {
        /* send CMD8 */
        errorstatus = SD_SendInterfaceCondition(sdId);
        /* check response */
        if (errorstatus == Status_Success) {
            /* SDHC or SDXC card */
            applicationCommand41Argument |= SD_OcrHostCapacitySupportFlag;
            card->flags |= SD_SupportSdhcFlag;
            card->type = SD_TYPE_SDC;
            break;
        } else {
            /* Try sending CMD8 again */
            SDH_MSG("Try sending CMD8 again:%d\r\n", i + 1);
            errorstatus = SDH_GoIdle(sdId);
            if (errorstatus != Status_Success) {
                return Status_SDH_GoIdleFailed;
            }
            //go emmc flow
        }
    }

    if(card->type == SD_TYPE_SDC) {
        /* Set card interface condition according to SDHC capability and card's supported interface condition. */
        errorstatus = SD_ApplicationSendOperationCondition(card, applicationCommand41Argument, sdId);

        if (errorstatus != Status_Success) {
            return Status_SDH_SendApplicationCommandFailed;
        }

        SDH_MSG("\r\nOCR is: 0x%02lX.\r\n", card->ocr);
        SDH_MSG("\t SDHC supported[%s].\r\n\r\n", ((card->flags & SD_SupportHighCapacityFlag) ? "YES" : "NO"));

        errorstatus = SD_AllSendCid(card, sdId);

        if (errorstatus != Status_Success) {
            return Status_SDH_AllSendCidFailed;
        }

        SDH_MSG("\r\nCID is: 0x%02lX-0x%02lX-0x%02lX-0x%02lX.\r\n",
                card->rawCid[0], card->rawCid[1], card->rawCid[2], card->rawCid[3]);
        SDH_MSG("\t manufacturerID is: 0x%02X.\r\n", card->cid.manufacturerID);
        SDH_MSG("\t applicationID is: %c%c.\r\n", (card->cid.applicationID) >> 8, card->cid.applicationID);
        SDH_MSG("\t productName is: %c%c%c%c%c.\r\n",
                card->cid.productName[0], card->cid.productName[1], card->cid.productName[2], card->cid.productName[3], card->cid.productName[4]);
        SDH_MSG("\t manufacturerData is: 0x%02X.\r\n\r\n", card->cid.manufacturerData);

        errorstatus = SD_SendRca(card, sdId);

        if (errorstatus != Status_Success) {
            return Status_SDH_SendRelativeAddressFailed;
        }

        SDH_MSG("\r\nRCA is: 0x%02lX.\r\n\r\n", card->relativeAddress);

        errorstatus = SD_SendCsd(card, sdId);

        if (errorstatus != Status_Success) {
            return Status_SDH_SendCsdFailed;
        }

        SDH_MSG("\r\nCSD is: 0x%02lX-0x%02lX-0x%02lX-0x%02lX.\r\n",
                card->rawCsd[0], card->rawCsd[1], card->rawCsd[2], card->rawCsd[3]);
        SDH_MSG("\t CSD Version is: %s .\r\n", card->csd.csdStructure ? "csd version 2.0" : "csd version 1.0");
        SDH_MSG("\t blockLen=%ld, blockCounter=%ld, CardSize is %ld[MBytes].\r\n\r\n", card->blockSize, card->blockCount, (card->blockCount) >> 11);

        errorstatus = SD_SelectCard(card, ENABLE, sdId);

        if (errorstatus != Status_Success) {
            return Status_SDH_SelectCardFailed;
        }

        errorstatus = SD_SendScr(card, sdId);

        if (errorstatus != Status_Success) {
            return Status_SDH_SendScrFailed;
        }

        SDH_MSG("\r\nSCR is: 0x%lX-0x%lX.\r\n", card->rawScr[0], card->rawScr[1]);
        SDH_MSG("\t SD Spec Version is: [0x%02lX]%s.\r\n", card->version,
                (card->version & SD_SpecificationVersion3_0) ? "V3.0" : ((card->version & SD_SpecificationVersion2_0) ? "V2.0" : ((card->version & SD_SpecificationVersion1_1) ? "V1.1" : "V1.0")));
        SDH_MSG("\t Erased bit is %d.\r\n", (card->scr.flags & SD_ScrDataStatusAfterErase));
        SDH_MSG("\t 4-line supported[%s].\r\n", ((card->flags & SD_Support4BitWidthFlag) ? "YES" : "NO"));
        SDH_MSG("\t SetBlockCountCmd supported[%s].\r\n", ((card->flags & SD_SupportSetBlockCountCmd) ? "YES" : "NO"));
        SDH_MSG("\t SDXC supported[%s].\r\n\r\n", ((card->flags & SD_SupportSdxcFlag) ? "YES" : "NO"));

        SD_SelectFunction(SDH_GroupTimingMode, SDH_TimingSDR25HighSpeedMode, sdId);

        errorstatus = SD_SetBlockSize(SDH_DEFAULT_BLOCK_SIZE, sdId);

        if (errorstatus != Status_Success) {
            return Status_SDH_SetCardBlockSizeFailed;
        }

        if (card->flags & SD_Support4BitWidthFlag) {
            errorstatus = SD_SetDataBusWidth(card, (SDH_Data_Bus_Width_Type)bus_wide, sdId);
        } else {
            errorstatus = SD_SetDataBusWidth(card, SDH_DATA_BUS_WIDTH_1BIT, sdId);
        }

        if (errorstatus != Status_Success) {
            return Status_SDH_SetDataBusWidthFailed;
        }

        errorstatus = SD_SendSsr(card, sdId);

        if (errorstatus != Status_Success) {
            return Status_SDH_SendSsrFailed;
        }

        SDH_MSG("\r\nSSR[0] is: 0x%lX.\r\n", card->rawSsr[0]);
        SDH_MSG("\t Current is %d-line mode.\r\n\r\n", (card->rawSsr[0] & 0x80) ? 4 : 1);


        //if (bus_wide != SDH_DATA_BUS_WIDTH_1BIT) {

        //}
        return errorstatus;
        }
    else {
        SDH_MSG("SDCard not found, check emmc\r\n");
        applicationCommand41Argument |= SD_OcrHostCapacitySupportFlag;
        for (uint16_t i = 0; i < 4; i++) {
            /* send CMD1 */
            errorstatus = EMMC_SendOPCondition(card, applicationCommand41Argument, sdId);
            /* check response */
            if (errorstatus == Status_Success) {
                /* EMMC card */
                applicationCommand41Argument |= SD_OcrHostCapacitySupportFlag;
                card->flags |= SD_SupportVoltage180v;
                card->type = SD_TYPE_EMMC;
                SDH_MSG("\r\nOCR is: 0x%02lX.\r\n", card->ocr);
                break;
            } else {
                /* Try sending CMD1 again */
                SDH_MSG("Try sending CMD1 again:%d\r\n", i + 1);
                errorstatus = SDH_GoIdle(sdId);
                if (errorstatus != Status_Success) {
                    return Status_SDH_GoIdleFailed;
                }
            }
        }
        if (card->type != SD_TYPE_EMMC) {
            SDH_ERR_MSG("[%s]Both EMMC and SD check fail, invalid card\r\n", sdId == SDH_ID ? "SDH" : "SDC");
            return Status_SDH_SDIO_InvalidCard;
        }

        errorstatus = SD_AllSendCid(card, sdId);

        if (errorstatus != Status_Success) {
            return Status_SDH_AllSendCidFailed;
        }

        SDH_MSG("\r\nCID is: 0x%02lX-0x%02lX-0x%02lX-0x%02lX.\r\n",
                card->rawCid[0], card->rawCid[1], card->rawCid[2], card->rawCid[3]);
        SDH_MSG("\t manufacturerID is: 0x%02X.\r\n", card->cid.manufacturerID);
        SDH_MSG("\t applicationID is: %c%c.\r\n", (card->cid.applicationID) >> 8, card->cid.applicationID);
        SDH_MSG("\t productName is: %c%c%c%c%c.\r\n",
                card->cid.productName[0], card->cid.productName[1], card->cid.productName[2], card->cid.productName[3], card->cid.productName[4]);
        SDH_MSG("\t manufacturerData is: 0x%02X.\r\n\r\n", card->cid.manufacturerData);


        card->relativeAddress = 1;

        errorstatus = EMMC_SetRca(card, sdId);

        if (errorstatus != Status_Success) {
            return Status_SDH_SendRelativeAddressFailed;
        }

        SDH_MSG("\r\nRCA is: 0x%02lX.\r\n\r\n", card->relativeAddress);

        errorstatus = SD_SendCsd(card, sdId);

        if (errorstatus != Status_Success) {
            return Status_SDH_SendCsdFailed;
        }

        SDH_MSG("\r\nCSD is: 0x%02lX-0x%02lX-0x%02lX-0x%02lX.\r\n",
                card->rawCsd[0], card->rawCsd[1], card->rawCsd[2], card->rawCsd[3]);
        SDH_MSG("\t CSD Version is: %s .\r\n", card->csd.csdStructure ? "csd version 2.0" : "csd version 1.0");
        SDH_MSG("\t blockLen=%ld, blockCounter=%ld, CardSize is %ld[MBytes].\r\n\r\n", card->blockSize, card->blockCount, (card->blockCount) >> 11);

        errorstatus = SD_SelectCard(card, ENABLE, sdId);

        if (errorstatus != Status_Success) {
            return Status_SDH_SelectCardFailed;
        }

        card->flags |= SD_SupportSetBlockCountCmd;
        card->flags |= SD_Support4BitWidthFlag;
        card->flags |= SD_SupportSpeedClassControlCmd;

        // CMD8: EXT_CSD
        errorstatus = EMMC_SendExtCsd(card, sdId);
        if (errorstatus != Status_Success) {
            return Status_SDH_SendExtendedCsdFailed;
        }

        // High speed mode:
#if EMMC_HIGHSPEED_MODE
        uint8_t mmc_bus_width = SDH_DATA_BUS_WIDTH_1BIT;
        uint8_t ext_csd_bus_width = EXT_CSD_BUS_WIDTH_1;
        if (card->ext_csd.cardType & EXT_CSD_CARD_TYPE_HS) {
            errorstatus = EMMC_SwitchFunction(MMC_SWITCH_MODE_WRITE_BYTE, EXT_CSD_HS_TIMING, EXT_CSD_TIMING_HS, sdId);

            if (errorstatus != Status_Success) {
                SDH_ERR_MSG("MMC switch HS fail, status 0x%lx\r\n", (uint32_t)errorstatus);
                return Status_SDH_SDIO_SwitchHighSpeedFail;
            }

            while (SDH_WaitDatIdle(sdId) != SDH_STAT_SUCCESS);

			if (bus_wide == SDH_DATA_BUS_WIDTH_4BITS) {
			    mmc_bus_width = SDH_DATA_BUS_WIDTH_4BITS;
				ext_csd_bus_width = EXT_CSD_BUS_WIDTH_4;
			}
			else if (bus_wide == SDH_DATA_BUS_WIDTH_8BITS) {
				mmc_bus_width = SDH_DATA_BUS_WIDTH_8BITS;
				ext_csd_bus_width = EXT_CSD_BUS_WIDTH_8;
			}
            errorstatus = EMMC_SwitchFunction(MMC_SWITCH_MODE_WRITE_BYTE, EXT_CSD_BUS_WIDTH, ext_csd_bus_width, sdId);
            if (errorstatus != Status_Success) {
                SDH_ERR_MSG("MMC switch bus width fail, status 0x%lx\r\n", (uint32_t)errorstatus);
                return Status_SDH_SetDataBusWidthFailed;
            }

        }

        /* initialise SDH controller*/
        SDH_Cfg_Type_Instance[sdId].vlot18Enable = DISABLE;
        SDH_Cfg_Type_Instance[sdId].highSpeed = ENABLE;
        SDH_Cfg_Type_Instance[sdId].dataWidth = mmc_bus_width;
        SDH_Cfg_Type_Instance[sdId].volt = SDH_VOLTAGE_3P3V;
        SDH_Cfg_Type_Instance[sdId].srcClock = sdhClockSrc[sdId];
        SDH_Cfg_Type_Instance[sdId].busClock = sdhClockTransfer[sdId];
        SDH_Ctrl_Init(&SDH_Cfg_Type_Instance[sdId], sdId);

        SDH_MSG("EMMC with %d-bit and High Speed\r\n", mmc_bus_width == SDH_DATA_BUS_WIDTH_1BIT ? 1 : mmc_bus_width == SDH_DATA_BUS_WIDTH_4BITS ? 4 : 8);

#endif

        return errorstatus;
    }
}

/**
  * @brief  Initializes SD Card clock.
  * @retval SD status
  */
status_t SDH_ClockSet(uint32_t clockInit, uint32_t clockSrc, uint32_t clockTransfer, SD_ID_Type sdId)
{
    sdhClockInit[sdId] = clockInit;
    sdhClockSrc[sdId] = clockSrc;
    sdhClockTransfer[sdId] = clockTransfer;

    return Status_Success;
}

static status_t MMC_GetBootModeData(void *data, uint32_t data_len, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    static SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;
    static SDH_Data_Cfg_Type SDH_Data_Cfg_TypeInstance;
    SDH_Stat_Type stat = SDH_STAT_SUCCESS;
    uint32_t intFlag;

    SDH_GoPreIdle(sdId);

    SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = DISABLE;
    SDH_Data_Cfg_TypeInstance.enableIgnoreError = DISABLE;
    SDH_Data_Cfg_TypeInstance.dataType = SDH_TRANS_DATA_BOOT;
    SDH_Data_Cfg_TypeInstance.blockSize = 512;
    SDH_Data_Cfg_TypeInstance.blockCount = (data_len+511)/512;
    SDH_Data_Cfg_TypeInstance.rxDataLen = 0;
    SDH_Data_Cfg_TypeInstance.rxData = (uint32_t *)data;
    SDH_Data_Cfg_TypeInstance.txDataLen = 0;
    SDH_Data_Cfg_TypeInstance.txData = NULL;
    memset(adma2Entries[sdId], 0, sizeof(adma2Entries[sdId]));
    SDH_DMA_Cfg_TypeInstance[sdId].dmaMode = SDH_DMA_MODE_ADMA2;
    SDH_DMA_Cfg_TypeInstance[sdId].admaEntries = (uint32_t *)adma2Entries[sdId];
    SDH_DMA_Cfg_TypeInstance[sdId].maxEntries = sizeof(adma2Entries[sdId]) / sizeof(adma2Entries[sdId][0]);

    /* Update ADMA descriptor table according to different DMA mode(simple DMA, ADMA1, ADMA2).*/
    stat = SDH_CreateAdmaEntryConfig(&SDH_DMA_Cfg_TypeInstance[sdId], &SDH_Data_Cfg_TypeInstance, SDH_ADMA_FLAG_SINGLE_DESC, sdId);
    if (stat != SDH_STAT_SUCCESS) {
        return stat;
    }

    /* Config the data transfer parameter */
    stat = SDH_ConfigDataTranfer(&SDH_Data_Cfg_TypeInstance, sdId);
    if (stat != SDH_STAT_SUCCESS) {
        return stat;
    }

    SDH_CMD_Cfg_TypeInstance.argument = 0;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_NONE;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_DATA_PRESENT;

    SDH_SendCommand(&SDH_CMD_Cfg_TypeInstance, sdId);
    while (1) {
        intFlag = SDH_GetIntStatus(sdId);
//        printf("intFlag = 0x%lx\r\n", intFlag);
        if (intFlag & SDH_INT_CMD_ERRORS) {
            errorstatus = Status_Fail;
            break;

        } else if (intFlag & SDH_INT_BOOT_DONE) {
            errorstatus = Status_Success;
            break;

        }
    }
    SDH_ClearIntStatus(intFlag, sdId);
    SDH_DisableBootMode(sdId);

    return errorstatus;

}

status_t MMC_BootMode_Init(void *data, uint32_t data_len, SD_ID_Type sdId)
{
    /*
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SMIH1);
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SMIH1);
    */

    /* config sdh clock */

    /* reset SDH controller*/
    SDH_Reset(sdId);

    SDH_HostInit(sdId, 1);

    return MMC_GetBootModeData(data, data_len, sdId);
}

status_t SDH_Init(uint32_t bus_wide, sd_card_t *pOutCardInfo, SD_ID_Type sdId)
{
    pSDCardInfo[sdId] = pOutCardInfo;

    /* reset controller and ungate */

    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SDH);
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SDH);

#if SDIO_SDCARD_INT_MODE
    SDH_INT_Init();
#endif

    /* reset SDH controller*/
    SDH_Reset(sdId);

    SDH_HostInit(sdId, 0);

    if (pOutCardInfo == NULL) {
        return Status_InvalidArgument;
    } else {
        return SDH_SDCardInit(bus_wide, pSDCardInfo[sdId], sdId);
    }
}

/**
  * @brief  Allows to erase memory area specified for the given card.
  * @param  startaddr: the start address.
  * @param  endaddr: the end address.
  * @retval SD_Error: SD Card Error code.
  */
status_t SD_Erase(uint32_t startaddr, uint32_t endaddr, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    uint8_t cardstate = 0;

    SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;

    /* SDSC card uses byte unit address*/
    if (!(pSDCardInfo[sdId]->access_mode)) {
        startaddr *= 512;
        endaddr *= 512;
    }

    /*!< Send CMD32 SD_ERASE_GRP_START with argument as addr  */
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_SD_ERASE_GRP_START;
    SDH_CMD_Cfg_TypeInstance.argument = startaddr;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R1;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 0, sdId);

    if (errorstatus != Status_Success) {
        goto out;
    } else if (SDH_CMD_Cfg_TypeInstance.response[0] & SD_CSR_ERRORBITS) {
        errorstatus = Status_SDH_CmdResponseError;
        goto out;
    }

    /*!< Send CMD33 SD_ERASE_GRP_END with argument as addr  */
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_SD_ERASE_GRP_END;
    SDH_CMD_Cfg_TypeInstance.argument = endaddr;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R1;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 0, sdId);

    if (errorstatus != Status_Success) {
        goto out;
    } else if (SDH_CMD_Cfg_TypeInstance.response[0] & SD_CSR_ERRORBITS) {
        errorstatus = Status_SDH_CmdResponseError;
        goto out;
    }

    /*!< Send CMD38 ERASE */
    SDH_CMD_Cfg_TypeInstance.index = SD_CMD_ERASE;
    SDH_CMD_Cfg_TypeInstance.argument = 0;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R1B;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_NONE;

    errorstatus = SDH_SendCardCommand(&SDH_CMD_Cfg_TypeInstance, 0, sdId);

    if (errorstatus != Status_Success) {
        goto out;
    } else if (SDH_CMD_Cfg_TypeInstance.response[0] & SD_CSR_ERRORBITS) {
        errorstatus = Status_SDH_CmdResponseError;
        goto out;
    }

    /*!< Wait till the card is in programming state */
    errorstatus = IsCardProgramming(&cardstate, sdId);

    while ((errorstatus == SD_OK) && ((SD_CARD_PROGRAMMING == cardstate) || (SD_CARD_RECEIVING == cardstate))) {
        errorstatus = IsCardProgramming(&cardstate, sdId);
    }

out:
    return errorstatus;
}

void MMC_SwitchPartition(int part, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;

    errorstatus = EMMC_SwitchFunction(MMC_SWITCH_MODE_WRITE_BYTE, EXT_CSD_PART_CONFIG, part, sdId);

    if (errorstatus != Status_Success) {
        SDH_ERR_MSG("MMC_SwitchPartition fail, status 0x%lx\r\n", (uint32_t)errorstatus);
        return;
    }

    EMMC_SendExtCsd(pSDCardInfo[sdId], sdId);

}

/* Transmit data in non-blocking mode, Only the sending status of commands is checked */
static status_t SDH_CardTransferNonBlocking(SDH_DMA_Cfg_Type *dmaCfg, SDH_Trans_Cfg_Type *transfer, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SDH_Stat_Type stat = SDH_STAT_SUCCESS;

    stat = SDH_TransferNonBlocking(dmaCfg, transfer, sdId);

    if (stat != SDH_STAT_SUCCESS) {
        return Status_SDH_TransferFailed;
    }

    /* Flush ADMA2-descriptor-table to RAM, Otherwise ADMA2 will fail */
    L1C_DCACHE_CLEAN_RANGE((uintptr_t)(dmaCfg->admaEntries), dmaCfg->maxEntries * sizeof(SDH_ADMA2_Desc_Type));

    errorstatus = SDH_SendCardCommand(transfer->cmdCfg, 0, sdId);

    if (errorstatus != Status_Success) {
        return errorstatus;
    } else if (transfer->cmdCfg->response[0] & SD_CSR_ERRORBITS) {
        return Status_SDH_CmdResponseError;
    }

    return errorstatus;
}

status_t SDH_ReadMultiBlocks(uint8_t *readbuff, uint32_t ReadAddr, uint16_t BlockSize, uint32_t NumberOfBlocks, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SD_Error sd_status;
    uint32_t time_node;

    static SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;
    static SDH_Data_Cfg_Type SDH_Data_Cfg_TypeInstance;
    static SDH_Trans_Cfg_Type SDH_Trans_Cfg_TypeInstance = { &SDH_Data_Cfg_TypeInstance, &SDH_CMD_Cfg_TypeInstance };

    /* IPRO7 supports only 8-byte aligned addresses */
    if ((uintptr_t)readbuff % 8 != 0) {
        SDH_ERR_MSG("readbuff not 8-byte aligned! addr: %p\r\n", readbuff);
        return Status_InvalidArgument;
    }

    /* SDSC card uses byte unit address*/
    if (!(pSDCardInfo[sdId]->access_mode)) {
        BlockSize = 512;
        ReadAddr *= 512;
    }

    SDH_MSG_TRANS("\r\nRead-->IN, block num: %ld, block addr: %ld, read buffer addr: %p.\r\n", NumberOfBlocks, ReadAddr, readbuff);

    /*set cmd parameter for READ_MULTIPLE_BLOCK*/
    if (NumberOfBlocks <= 1) {
        SDH_CMD_Cfg_TypeInstance.index = SD_CMD_READ_SINGLE_BLOCK;
    } else {
        SDH_CMD_Cfg_TypeInstance.index = SD_CMD_READ_MULT_BLOCK;
    }

    SDH_CMD_Cfg_TypeInstance.argument = (uint32_t)ReadAddr;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R1;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_DATA_PRESENT;

    /*set data parameter for READ_MULTIPLE_BLOCK*/
    if (NumberOfBlocks <= 1) {
        SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = DISABLE;
    } else {
        if (pSDCardInfo[sdId]->flags & SD_SupportSetBlockCountCmd) {
            SDH_Data_Cfg_TypeInstance.enableAutoCommand23 = ENABLE;
            SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = DISABLE;
        } else {
            SDH_Data_Cfg_TypeInstance.enableAutoCommand23 = DISABLE;
            SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = ENABLE;
        }
    }

    SDH_Data_Cfg_TypeInstance.enableIgnoreError = DISABLE;
    SDH_Data_Cfg_TypeInstance.dataType = SDH_TRANS_DATA_NORMAL;
    SDH_Data_Cfg_TypeInstance.blockSize = BlockSize;
    SDH_Data_Cfg_TypeInstance.blockCount = NumberOfBlocks;
    SDH_Data_Cfg_TypeInstance.rxDataLen = 0;
    SDH_Data_Cfg_TypeInstance.rxData = (uint32_t *)readbuff;
    SDH_Data_Cfg_TypeInstance.txDataLen = 0;
    SDH_Data_Cfg_TypeInstance.txData = NULL;
    SDH_Data_Cfg_TypeInstance.txDataLen = 0;
    SDH_Data_Cfg_TypeInstance.txData = NULL;

    /*set parameters for SDH_DMA_Cfg_TypeInstance*/
    SDH_DMA_Cfg_TypeInstance[sdId].dmaMode = SDH_DMA_MODE_ADMA2;
//    SDH_DMA_Cfg_TypeInstance.burstSize = SDH_BURST_SIZE_64_BYTES;
//    SDH_DMA_Cfg_TypeInstance.fifoThreshold = SDH_FIFO_THRESHOLD_256_BYTES;
    SDH_DMA_Cfg_TypeInstance[sdId].admaEntries = (uint32_t *)adma2Entries[sdId];
    SDH_DMA_Cfg_TypeInstance[sdId].maxEntries = sizeof(adma2Entries[sdId]) / sizeof(adma2Entries[sdId][0]);

    L1C_DCACHE_CLEAN_RANGE((uintptr_t)(readbuff), 0);
    L1C_DCACHE_CLEAN_RANGE((uintptr_t)(readbuff) + BlockSize * NumberOfBlocks, 0);
    L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)(readbuff), BlockSize * NumberOfBlocks);

    errorstatus = SDH_CardTransferNonBlocking(&SDH_DMA_Cfg_TypeInstance[sdId], &SDH_Trans_Cfg_TypeInstance, sdId);

    if (errorstatus != Status_Success) {
        SDH_ERR_MSG("SDH Transfer err:%ld\r\n", errorstatus);
        goto out;
    }

    time_node = (uint32_t)SDH_GET_TIME();

#if SDIO_SDCARD_INT_MODE

    SDH_DataWaitStatus = SD_WAITING;
    SDH_EnableIntSource(SDH_INT_DATA_COMPLETED | SDH_INT_DATA_ERRORS | SDH_INT_DMA_ERROR | SDH_INT_AUTO_CMD12_ERROR);

    uint32_t timeout = SDH_GetDataTransfer_Timeout(BlockSize * NumberOfBlocks << 10, sdId);
    if (xSemaphoreTake(sdhIntSemphr, timeout) == pdFAIL) {
        SDH_ERR_MSG("SDH read data timeout: %ld", timeout);
        SDH_DisableIntSource(SDH_INT_DATA_COMPLETED | SDH_INT_DATA_ERRORS | SDH_INT_DMA_ERROR | SDH_INT_AUTO_CMD12_ERROR);
        return Status_Timeout;
    }
    sd_status = SDH_DataWaitStatus;
    SDH_DisableIntSource(SDH_INT_DATA_COMPLETED | SDH_INT_DATA_ERRORS | SDH_INT_DMA_ERROR | SDH_INT_AUTO_CMD12_ERROR);

#else

    uint32_t intFlag;
    while (1) {
        intFlag = SDH_GetIntStatus(sdId);
        if (intFlag & SDH_INT_DATA_ERRORS || intFlag & SDH_INT_DMA_ERROR || intFlag & SDH_INT_AUTO_CMD12_ERROR) {
            sd_status = SD_CMD_ERROR;
            break;

        } else if (intFlag & SDH_INT_DATA_COMPLETED) {
            sd_status = SD_OK;
            break;

        } else if ((uint32_t)SDH_GET_TIME() - time_node > SDIO_DATATIMEOUT_MS) {
            SDH_ERR_MSG("SDH read data timeout: %ld ms\r\n", (uint32_t)SDH_GET_TIME() - time_node);
            return Status_Timeout;
        }
    }
    SDH_ClearIntStatus(intFlag, sdId);

#endif

    if (sd_status != SD_OK) {
        errorstatus = Status_SDH_TransferFailed;
        goto out;
    }

    SDH_MSG_TRANS("Read data used time: %ld ms\r\n", (uint32_t)SDH_GET_TIME() - time_node);
    SDH_MSG_TRANS("Read-->OUT, block num: %ld, block addr: %ld, read buffer addr: %p.\r\n", NumberOfBlocks, ReadAddr, readbuff);

out:
    return (errorstatus);
}

status_t SDH_WriteMultiBlocks(const uint8_t *writebuff, uint32_t WriteAddr, uint16_t BlockSize, uint32_t NumberOfBlocks, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    SD_Error sd_status;
    uint32_t time_node;
    uint32_t intFlag = 0;

    static SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;
    static SDH_Data_Cfg_Type SDH_Data_Cfg_TypeInstance;
    static SDH_Trans_Cfg_Type SDH_Trans_Cfg_TypeInstance = { &SDH_Data_Cfg_TypeInstance, &SDH_CMD_Cfg_TypeInstance };

    if ((uintptr_t)writebuff % 8 != 0) {
        SDH_ERR_MSG("readbuff not 8-byte aligned! addr: %p\r\n", writebuff);
        return Status_InvalidArgument;
    }

    if (!(pSDCardInfo[sdId]->access_mode)) {
        /* It's SDCS card,SDSC card uses byte unit address*/
        BlockSize = 512;
        WriteAddr *= 512;
    }

    SDH_MSG_TRANS("\r\nWrite-->IN, block num: %ld, block addr: %ld, read buffer addr: %p.\r\n", NumberOfBlocks, WriteAddr, writebuff);

    /*set cmd parameter for SD_CMD_WRITE_MULT_BLOCK*/
    if (NumberOfBlocks <= 1) {
        SDH_CMD_Cfg_TypeInstance.index = SD_CMD_WRITE_SINGLE_BLOCK;
    } else {
        SDH_CMD_Cfg_TypeInstance.index = SD_CMD_WRITE_MULT_BLOCK;
    }

    SDH_CMD_Cfg_TypeInstance.argument = (uint32_t)WriteAddr;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R1;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_DATA_PRESENT;

    /*set data parameter for WRITE_MULTIPLE_BLOCK*/
    if (pSDCardInfo[sdId]->type == SD_TYPE_EMMC)
        SDH_Data_Cfg_TypeInstance.cmd23_emmc_mode = ENABLE;
    else
        SDH_Data_Cfg_TypeInstance.cmd23_emmc_mode = DISABLE;

    if (pSDCardInfo[sdId]->type == SD_TYPE_EMMC && pSDCardInfo[sdId]->rel_wr &&
        (pSDCardInfo[sdId]->ext_csd.rel_param & EXT_CSD_WR_REL_PARAM_EN /*|| pSDCardInfo[sdId]->ext_csd.rel_wr_sec_count > 0*/ )) {
        // EMMC with REL_WR supported, not support for legacy mode (rel_param_en=0, wr_sec_count=1, which needs to change transfer block size if needed)
        SDH_Data_Cfg_TypeInstance.enableAutoCommand23 = ENABLE;
        SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = DISABLE;
        SDH_Data_Cfg_TypeInstance.enableRelibaleWR = ENABLE;
    } else {
        SDH_Data_Cfg_TypeInstance.enableRelibaleWR = DISABLE;
        if (NumberOfBlocks <= 1) {
            SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = DISABLE;
        } else {
            if (pSDCardInfo[sdId]->flags & SD_SupportSetBlockCountCmd) {
                SDH_Data_Cfg_TypeInstance.enableAutoCommand23 = ENABLE;
                SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = DISABLE;
            } else {
                SDH_Data_Cfg_TypeInstance.enableAutoCommand23 = DISABLE;
                SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = ENABLE;
            }
        }
    }

    SDH_Data_Cfg_TypeInstance.enableIgnoreError = DISABLE;
    SDH_Data_Cfg_TypeInstance.dataType = SDH_TRANS_DATA_NORMAL;
    SDH_Data_Cfg_TypeInstance.blockSize = BlockSize;
    SDH_Data_Cfg_TypeInstance.blockCount = NumberOfBlocks;
    SDH_Data_Cfg_TypeInstance.rxDataLen = 0;
    SDH_Data_Cfg_TypeInstance.rxData = NULL;
    SDH_Data_Cfg_TypeInstance.txDataLen = 0;
    SDH_Data_Cfg_TypeInstance.txData = (uint32_t *)writebuff;
    /*set parameters for SDH_DMA_Cfg_TypeInstance*/
    SDH_DMA_Cfg_TypeInstance[sdId].dmaMode = SDH_DMA_MODE_ADMA2;
//    SDH_DMA_Cfg_TypeInstance.burstSize = SDH_BURST_SIZE_64_BYTES;
//    SDH_DMA_Cfg_TypeInstance.fifoThreshold = SDH_FIFO_THRESHOLD_256_BYTES;
    SDH_DMA_Cfg_TypeInstance[sdId].admaEntries = (uint32_t *)adma2Entries[sdId];
    SDH_DMA_Cfg_TypeInstance[sdId].maxEntries = sizeof(adma2Entries[sdId]) / sizeof(adma2Entries[sdId][0]);

    L1C_DCACHE_CLEAN_RANGE((uintptr_t)(writebuff), BlockSize * NumberOfBlocks);

    errorstatus = SDH_CardTransferNonBlocking(&SDH_DMA_Cfg_TypeInstance[sdId], &SDH_Trans_Cfg_TypeInstance, sdId);

    if (errorstatus != Status_Success) {
        SDH_ERR_MSG("SDH Transfer err:%ld\r\n", errorstatus);
        return errorstatus;
    }

    time_node = (uint32_t)SDH_GET_TIME();

#if SDIO_SDCARD_INT_MODE

    SDH_DataWaitStatus = SD_WAITING;
    SDH_EnableIntSource(SDH_INT_DATA_COMPLETED | SDH_INT_DATA_ERRORS | SDH_INT_DMA_ERROR | SDH_INT_AUTO_CMD12_ERROR);

    uint32_t timeout = SDH_GetDataTransfer_Timeout(BlockSize * NumberOfBlocks << 10, sdId);
    if (xSemaphoreTake(sdhIntSemphr, timeout) == pdFAIL) {
        SDH_ERR_MSG("SDH write data timeout: %ld ms\r\n", timeout);
        SDH_DisableIntSource(SDH_INT_DATA_COMPLETED | SDH_INT_DATA_ERRORS | SDH_INT_DMA_ERROR | SDH_INT_AUTO_CMD12_ERROR);
        errorstatus = Status_Timeout;
        return errorstatus;
    }

    SDH_DisableIntSource(SDH_INT_DATA_COMPLETED | SDH_INT_DATA_ERRORS | SDH_INT_DMA_ERROR | SDH_INT_AUTO_CMD12_ERROR);
    sd_status = SDH_DataWaitStatus;

#else

    while (1) {
        intFlag = SDH_GetIntStatus(sdId);
        if (intFlag & SDH_INT_DATA_ERRORS || intFlag & SDH_INT_DMA_ERROR || intFlag & SDH_INT_AUTO_CMD12_ERROR) {
            sd_status = SD_DataCfg_ERROR;
            break;

        } else if (intFlag & SDH_INT_DATA_COMPLETED) {
            sd_status = SD_OK;
            break;

        } else if ((uint32_t)SDH_GET_TIME() - time_node > SDIO_DATATIMEOUT_MS) {
            SDH_ERR_MSG("SDH write data timeout: %ld ms\r\n", (uint32_t)SDH_GET_TIME() - time_node);
            return Status_Timeout;
        }
    }
    SDH_ClearIntStatus(intFlag, sdId);

#endif

    if (sd_status != SD_OK) {
        errorstatus = Status_SDH_TransferFailed;
        SDH_ERR_MSG("intFlag = 0x%lx\r\n", intFlag);
        goto out;
    } else {
        errorstatus = WaitInProgramming(sdId);
    }

    SDH_MSG_TRANS("Write data used time: %ld ms\r\n", (uint32_t)SDH_GET_TIME() - time_node);
    SDH_MSG_TRANS("Write-->OUT, block num: %ld, block addr: %ld, read buffer addr: %p.\r\n", NumberOfBlocks, WriteAddr, writebuff);

out:
    return (errorstatus);
}

status_t SDH_ReadMultiBlocksBlocking(uint8_t *readbuff, uint32_t ReadAddr, uint16_t BlockSize, uint32_t NumberOfBlocks, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    static SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;
    static SDH_Data_Cfg_Type SDH_Data_Cfg_TypeInstance;
    static SDH_Trans_Cfg_Type SDH_Trans_Cfg_TypeInstance = { &SDH_Data_Cfg_TypeInstance, &SDH_CMD_Cfg_TypeInstance };
    uint32_t time_node __attribute__((unused)) ;

    if ((uintptr_t)readbuff % 8 != 0) {
        SDH_MSG("readbuff not 8-byte aligned! addr: %p\r\n", readbuff);
        return Status_InvalidArgument;
    }

    if (!(pSDCardInfo[sdId]->access_mode)) {
        BlockSize = 512;
        ReadAddr *= 512;
    }
    //SDH_MSG_TRANS("\r\nRead-->IN, block num: %ld, block addr: %ld, read buffer addr: %p.\r\n", NumberOfBlocks, ReadAddr, readbuff);
    if (NumberOfBlocks <= 1) {
        SDH_CMD_Cfg_TypeInstance.index = SD_CMD_READ_SINGLE_BLOCK;
    } else {
        SDH_CMD_Cfg_TypeInstance.index = SD_CMD_READ_MULT_BLOCK;
    }
    SDH_CMD_Cfg_TypeInstance.argument = (uint32_t)ReadAddr;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R1;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_DATA_PRESENT;
    if (NumberOfBlocks <= 1) {
        SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = DISABLE;
    } else {
        if (pSDCardInfo[sdId]->flags & SD_SupportSetBlockCountCmd) {
            SDH_Data_Cfg_TypeInstance.enableAutoCommand23 = ENABLE;
            SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = DISABLE;
        } else {
            SDH_Data_Cfg_TypeInstance.enableAutoCommand23 = DISABLE;
            SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = ENABLE;
        }
    }
    SDH_Data_Cfg_TypeInstance.enableIgnoreError = DISABLE;
    SDH_Data_Cfg_TypeInstance.dataType = SDH_TRANS_DATA_NORMAL;
    SDH_Data_Cfg_TypeInstance.blockSize = BlockSize;
    SDH_Data_Cfg_TypeInstance.blockCount = NumberOfBlocks;
    SDH_Data_Cfg_TypeInstance.rxDataLen = 0;
    SDH_Data_Cfg_TypeInstance.rxData = (uint32_t *)readbuff;
    SDH_Data_Cfg_TypeInstance.txDataLen = 0;
    SDH_Data_Cfg_TypeInstance.txData = NULL;
    memset(adma2Entries[sdId], 0, sizeof(adma2Entries[sdId]));
    SDH_DMA_Cfg_TypeInstance[sdId].dmaMode = SDH_DMA_MODE_ADMA2;
//    SDH_DMA_Cfg_TypeInstance.burstSize = SDH_BURST_SIZE_64_BYTES;
//    SDH_DMA_Cfg_TypeInstance.fifoThreshold = SDH_FIFO_THRESHOLD_256_BYTES;
    SDH_DMA_Cfg_TypeInstance[sdId].admaEntries = (uint32_t *)adma2Entries[sdId];
    SDH_DMA_Cfg_TypeInstance[sdId].maxEntries = sizeof(adma2Entries[sdId]) / sizeof(adma2Entries[sdId][0]);

//    L1C_DCACHE_CLEAN_RANGE((uintptr_t)(readbuff), 0);
//    L1C_DCACHE_CLEAN_RANGE((uintptr_t)(readbuff) + BlockSize * NumberOfBlocks, 0);
    L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)(readbuff), BlockSize * NumberOfBlocks);
    time_node = (uint32_t)SDH_GET_TIME();
    errorstatus = SDH_CardTransferBlocking(&SDH_DMA_Cfg_TypeInstance[sdId], &SDH_Trans_Cfg_TypeInstance, sdId);
    if (errorstatus != Status_Success) {
        SDH_MSG("SDH Transfer err:%ld\r\n", errorstatus);
    }
    SDH_MSG_TRANS("Read data used time: %ld ms\r\n", (uint32_t)SDH_GET_TIME() - time_node);
    SDH_MSG_TRANS("Read-->OUT, block num: %ld, block addr: %ld, read buffer addr: %p.\r\n", NumberOfBlocks, ReadAddr, readbuff);

    return (errorstatus);
}
status_t SDH_WriteMultiBlocksBlocking(uint8_t *writebuff, uint32_t WriteAddr, uint16_t BlockSize, uint32_t NumberOfBlocks, SD_ID_Type sdId)
{
    status_t errorstatus = Status_Success;
    static SDH_CMD_Cfg_Type SDH_CMD_Cfg_TypeInstance;
    static SDH_Data_Cfg_Type SDH_Data_Cfg_TypeInstance;
    static SDH_Trans_Cfg_Type SDH_Trans_Cfg_TypeInstance = { &SDH_Data_Cfg_TypeInstance, &SDH_CMD_Cfg_TypeInstance };
    uint32_t time_node __attribute__((unused)) ;

    if ((uintptr_t)writebuff % 8 != 0) {
        SDH_MSG("writebuff not 8-byte aligned! addr: %p\r\n", writebuff);
        return Status_InvalidArgument;
    }

    if (!(pSDCardInfo[sdId]->access_mode)) {
        BlockSize = 512;
        WriteAddr *= 512;
    }

    SDH_MSG_TRANS("\r\nWrite-->IN, block num: %ld, block addr: %ld, write buffer addr: %p.\r\n", NumberOfBlocks, WriteAddr, writebuff);
    if (NumberOfBlocks <= 1) {
        SDH_CMD_Cfg_TypeInstance.index = SD_CMD_WRITE_SINGLE_BLOCK;
    } else {
        SDH_CMD_Cfg_TypeInstance.index = SD_CMD_WRITE_MULT_BLOCK;
    }
    SDH_CMD_Cfg_TypeInstance.argument = (uint32_t)WriteAddr;
    SDH_CMD_Cfg_TypeInstance.type = SDH_CMD_NORMAL;
    SDH_CMD_Cfg_TypeInstance.respType = SDH_RESP_R1;
    SDH_CMD_Cfg_TypeInstance.flag = SDH_TRANS_FLAG_DATA_PRESENT;
    if (NumberOfBlocks <= 1) {
        SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = DISABLE;
    } else {
        if (pSDCardInfo[sdId]->flags & SD_SupportSetBlockCountCmd) {
            SDH_Data_Cfg_TypeInstance.enableAutoCommand23 = ENABLE;
            SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = DISABLE;
        } else {
            SDH_Data_Cfg_TypeInstance.enableAutoCommand23 = DISABLE;
            SDH_Data_Cfg_TypeInstance.enableAutoCommand12 = ENABLE;
        }
    }
    SDH_Data_Cfg_TypeInstance.enableIgnoreError = DISABLE;
    SDH_Data_Cfg_TypeInstance.dataType = SDH_TRANS_DATA_NORMAL;
    SDH_Data_Cfg_TypeInstance.blockSize = BlockSize;
    SDH_Data_Cfg_TypeInstance.blockCount = NumberOfBlocks;
    SDH_Data_Cfg_TypeInstance.rxDataLen = 0;
    SDH_Data_Cfg_TypeInstance.rxData = NULL;
    SDH_Data_Cfg_TypeInstance.txDataLen = 0;
    SDH_Data_Cfg_TypeInstance.txData = (uint32_t *)writebuff;
    memset(adma2Entries[sdId], 0, sizeof(adma2Entries[sdId]));
    SDH_DMA_Cfg_TypeInstance[sdId].dmaMode = SDH_DMA_MODE_ADMA2;
//    SDH_DMA_Cfg_TypeInstance.burstSize = SDH_BURST_SIZE_64_BYTES;
//    SDH_DMA_Cfg_TypeInstance.fifoThreshold = SDH_FIFO_THRESHOLD_256_BYTES;
    SDH_DMA_Cfg_TypeInstance[sdId].admaEntries = (uint32_t *)adma2Entries[sdId];
    SDH_DMA_Cfg_TypeInstance[sdId].maxEntries = sizeof(adma2Entries[sdId]) / sizeof(adma2Entries[sdId][0]);

//    L1C_DCACHE_CLEAN_RANGE((uintptr_t)(writebuff), BlockSize * NumberOfBlocks);
    time_node = (uint32_t)SDH_GET_TIME();
    errorstatus = SDH_CardTransferBlocking(&SDH_DMA_Cfg_TypeInstance[sdId], &SDH_Trans_Cfg_TypeInstance, sdId);
    if (errorstatus != Status_Success) {
        SDH_MSG("SDH Transfer err:%ld\r\n", errorstatus);
        return errorstatus;
    }
    SDH_MSG_TRANS("Write data used time: %ld ms\r\n", (uint32_t)SDH_GET_TIME() - time_node);
    SDH_MSG_TRANS("Write-->OUT, block num: %ld, block addr: %ld, read buffer addr: %p.\r\n", NumberOfBlocks, WriteAddr, writebuff);
    return (errorstatus);
}


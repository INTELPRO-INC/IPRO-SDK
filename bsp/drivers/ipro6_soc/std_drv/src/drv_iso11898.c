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
#include "drv_iso11898.h"

#ifndef IPRO_USE_HAL_DRIVER
static intCallback_Type * iso11898IntCbfArra[ISO11898_INT_ALL] = { NULL };
static void ISO11898_IntHandler(void);
static void ISO11898_IRQHandler(void);
#endif

void ISO11898_Init(void)
{
    uint32_t tmpVal;

    /* Ungate ISO11898 clock */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_ISO11898);

    /* Enable UART clock*/
    tmpVal=readl(GLB_BASE + GLB_UART_CFG0_OFFSET);
    tmpVal=reg_set_bit(tmpVal, GLB_UART_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_UART_CFG0_OFFSET);

    /* Need to enter reset mode before operating */
    ISO11898_Mode_Set(ISO11898_MODE_TYPE_RESET, ISO11898_MODE_VALUE_RESET);

    /* Force using PeliISO11898 not BasicISO11898 */
    tmpVal = readl(ISO11898_BASE + ISO11898_CLOCK_DIVIDER_OFFSET);
    tmpVal = reg_set_bit(tmpVal, ISO11898_MODE);
    writel(tmpVal, ISO11898_BASE + ISO11898_CLOCK_DIVIDER_OFFSET);

    /* Disable all interrupts */
    writel(0, ISO11898_BASE + ISO11898_INTERRUPT_ENABLE_OFFSET);

    /* Set clock and sample point  */
    ISO11898_Clock_Div_Set(0);
    ISO11898_Synchronization_Jump_Width_Set(3);
    ISO11898_Sample_Parameter_Set(1, 8, 5);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(ISO11898_IRQn, ISO11898_IRQHandler);
#endif
}

void ISO11898_Mode_Set(ISO11898_MODE_Type type, ISO11898_MODE_VALUE_Type value)
{
    uint32_t tmpVal;

    /* Check the parameters */

    /* Set mode value */
    tmpVal = readl(ISO11898_BASE + ISO11898_MODE_OFFSET);
    tmpVal &= ~(1 << type);
    tmpVal |= (value << type);    

    /* Write back */
    writel(tmpVal, ISO11898_BASE + ISO11898_MODE_OFFSET);
}

ISO11898_MODE_VALUE_Type ISO11898_Mode_Get(ISO11898_MODE_Type type)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(ISO11898_BASE + ISO11898_MODE_OFFSET);
    tmpVal &= (1 << type);
    tmpVal >>= type;
    return (ISO11898_MODE_VALUE_Type)tmpVal;
}

void ISO11898_Int_Enable(ISO11898_INT_Type intType, int enable)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(ISO11898_BASE + ISO11898_INTERRUPT_ENABLE_OFFSET);
    /* Enable or Disable certain or all interrupt */
    if(ISO11898_INT_ALL == intType){
        if(ENABLE == enable){
            tmpVal |= 0xFF;
        }else{
            tmpVal &= 0;
        }
    }else{
        if(ENABLE == enable){
            tmpVal |= 1<<intType;
        }else{
            tmpVal &= ~(1<<intType);
        }
    }

    /* Write back */
    writel(tmpVal, ISO11898_BASE + ISO11898_INTERRUPT_ENABLE_OFFSET);
}

void ISO11898_Cmd_Request_Transmit(void)
{
    uint32_t tmpVal;

    tmpVal = readl(ISO11898_BASE + ISO11898_COMMAND_OFFSET);
    tmpVal = reg_set_bit(tmpVal, ISO11898_TRANSMIT_REQUEST);
    writel(tmpVal, ISO11898_BASE + ISO11898_COMMAND_OFFSET);
}
void ISO11898_Cmd_Abort_Transmit(void)
{
    uint32_t tmpVal;

    tmpVal = readl(ISO11898_BASE + ISO11898_COMMAND_OFFSET);
    tmpVal = reg_set_bit(tmpVal, ISO11898_ABORT_TRANSMISSION);
    writel(tmpVal, ISO11898_BASE + ISO11898_COMMAND_OFFSET);
}
void ISO11898_Cmd_Release_Recv_Buff(void)
{
    uint32_t tmpVal;

    tmpVal = readl(ISO11898_BASE + ISO11898_COMMAND_OFFSET);
    tmpVal = reg_set_bit(tmpVal, ISO11898_RELEASE_RECEIVE_BUFFER);
    writel(tmpVal, ISO11898_BASE + ISO11898_COMMAND_OFFSET);
}
void ISO11898_Cmd_Clear_Data_Overrun(void)
{
    uint32_t tmpVal;

    tmpVal = readl(ISO11898_BASE + ISO11898_COMMAND_OFFSET);
    tmpVal = reg_set_bit(tmpVal, ISO11898_CLEAR_DATA_OVERRUN);
    writel(tmpVal, ISO11898_BASE + ISO11898_COMMAND_OFFSET);
}
void ISO11898_Cmd_Request_Self_Reception(void)
{
    uint32_t tmpVal;

    tmpVal = readl(ISO11898_BASE + ISO11898_COMMAND_OFFSET);
    tmpVal = reg_set_bit(tmpVal, ISO11898_SELF_REQUEST);
    writel(tmpVal, ISO11898_BASE + ISO11898_COMMAND_OFFSET);
}

ISO11898_STATUS_VALUE_Type ISO11898_Status_Get(ISO11898_STATUS_Type type)
{
    volatile uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(ISO11898_BASE + ISO11898_STATUS_OFFSET);
    tmpVal &= (1 << type);
    tmpVal >>= type;
    return (ISO11898_STATUS_VALUE_Type)tmpVal;
}

uint8_t ISO11898_Arbitration_Lost_Position_Get(void)
{
    uint8_t tmpVal;

    tmpVal = readl(ISO11898_BASE + ISO11898_ARB_LOST_CAPTURE_OFFSET);
    tmpVal = reg_get_bits(tmpVal, ISO11898_ALC4_0);
    return tmpVal;
}

void ISO11898_Error_Get(ISO11898_ERROR_Type *const type)
{
    uint8_t tmpVal;

    tmpVal = readl(ISO11898_BASE + ISO11898_ERROR_CODE_CAPTURE_OFFSET);
    type->code = reg_get_bits(tmpVal, ISO11898_ERR_CODE);
    type->dir = reg_get_bits(tmpVal, ISO11898_DIR);
    type->location = reg_get_bits(tmpVal, ISO11898_SEGMENT4_0);
}

int ISO11898_Error_Warning_Limit_Set(uint8_t limit)
{
    if(ISO11898_MODE_VALUE_NORMAL == ISO11898_Mode_Get(ISO11898_MODE_TYPE_RESET))
    {
        return ERROR;
    }
    writel(limit, ISO11898_BASE + ISO11898_ERROR_WARNING_LIMIT_OFFSET);
    return SUCCESS;
}

uint8_t ISO11898_Error_Warning_Limit_Get(void)
{
    return readl(ISO11898_BASE + ISO11898_ERROR_WARNING_LIMIT_OFFSET);
}

uint8_t ISO11898_Error_Rx_Counter_Get(void)
{
    return readl(ISO11898_BASE + ISO11898_RX_ERR_COUNT_OFFSET);
}

uint8_t ISO11898_Error_Tx_Counter_Get(void)
{
    return readl(ISO11898_BASE + ISO11898_TX_ERR_COUNT_OFFSET);
}

int ISO11898_Transmit(const ISO11898_CFG_Type *cfg)
{
    uint32_t timeOut;
    uint32_t tmpVal;
    uint32_t i;
    volatile uint32_t *pData;

    /* Check the parameters */

    timeOut = cfg->timeout;

    /* Chcek Timeout type */
    while(ISO11898_STATUS_TRANSMIT_BUFFER_LOCKED == ISO11898_Status_Get(ISO11898_STATUS_TRANSMIT_BUFFER))
    {
        /* Wait no time */
        if (cfg->timeout == 0)
        {
            return TIMEOUT;
        }
        /* Wait until transmit buffer accessible */
        if (cfg->timeout >= ISO11898_TIMEOUT_MAX)
        {

        }
        /* Wait time = cfg->timeOut */
        else
        {
            if(timeOut == 0)
            {
                return TIMEOUT;
            }
            timeOut--;
        }
    }

    /* Config information register */
    tmpVal = cfg->dataLen;
    tmpVal = reg_set_bits(tmpVal, ISO11898_INF_FF, cfg->frameFormat);
    tmpVal = reg_set_bits(tmpVal, ISO11898_INF_RTR, cfg->frameType);
    writel(tmpVal, ISO11898_BASE + ISO11898_INFORMATION_OFFSET);

    tmpVal = cfg->id;
    /* Transmit format with 11 bits ID, SFF */
    if(ISO11898_FRAME_FORMAT_STANDARD == cfg->frameFormat)
    {
        /* Config identifier registers */
        writel((tmpVal >> 3) & 0xFF, ISO11898_BASE + ISO11898_IDENTIFIER_1_OFFSET);
        tmpVal <<= 5;
        tmpVal |= (cfg->frameType << 4);
        writel(tmpVal & 0xFF, ISO11898_BASE + ISO11898_IDENTIFIER_2_OFFSET);
        /* Config SFF data registers start address */
        pData = (uint32_t *)(ISO11898_BASE + ISO11898_SFF_DATA_1_OFFSET);
    }
    /* Transmit format with 29 bits ID, EFF */
    else
    {
        /* Config identifier registers */
        writel((tmpVal >> 21) & 0xFF, ISO11898_BASE + ISO11898_IDENTIFIER_1_OFFSET);
        writel((tmpVal >> 13) & 0xFF, ISO11898_BASE + ISO11898_IDENTIFIER_2_OFFSET);
        writel((tmpVal >> 5) & 0xFF, ISO11898_BASE + ISO11898_IDENTIFIER_3_OFFSET);
        tmpVal <<= 3;
        tmpVal |= (cfg->frameType << 2);
        writel(tmpVal & 0xFF, ISO11898_BASE + ISO11898_IDENTIFIER_4_OFFSET);
        /* Config EFF data registers start address */
        pData = (uint32_t *)(ISO11898_BASE + ISO11898_EFF_DATA_1_OFFSET);
    }
    /* Move data from user buff to registers */
    for (i = 0; i < cfg->dataLen; i++)
    {
        *(pData + i) = (uint32_t)(*(cfg->dataAddr + i));
    }

    /* Trig a request for transmit */
    if(ISO11898_MODE_VALUE_RESET == ISO11898_Mode_Get(ISO11898_MODE_TYPE_RESET))
    {
        return ERROR;
    }
    else if(ISO11898_MODE_VALUE_LISTEN_ONLY == ISO11898_Mode_Get(ISO11898_MODE_TYPE_LISTEN_ONLY))
    {
        return ERROR;
    }
    else if(ISO11898_MODE_VALUE_SELF_TSET == ISO11898_Mode_Get(ISO11898_MODE_TYPE_SELF_TSET))
    {
        ISO11898_Cmd_Request_Self_Reception();
    }
    else
    {
        ISO11898_Cmd_Request_Transmit();
    }

    return SUCCESS;
}

int ISO11898_Receive(ISO11898_CFG_Type *cfg)
{
    uint32_t timeOut;
    uint32_t tmpVal;
    uint32_t i;
    uint32_t id;
    volatile uint32_t *pData;

    /* Check the parameters */

    timeOut = cfg->timeout;

    /* Chcek Timeout type */
    while(ISO11898_STATUS_RECEIVE_BUFFER_EMPTY == ISO11898_Status_Get(ISO11898_STATUS_RECEIVE_BUFFER))
    {
        /* Wait no time */
        if (cfg->timeout == 0)
        {
            return TIMEOUT;
        }
        /* Wait until transmit buffer accessible */
        if (cfg->timeout >= ISO11898_TIMEOUT_MAX)
        {

        }
        /* Wait time = cfg->timeOut */
        else
        {
            if(timeOut == 0)
            {
                return TIMEOUT;
            }
            timeOut--;
        }
    }

    /* Read information register */
    tmpVal = readl(ISO11898_BASE + ISO11898_INFORMATION_OFFSET);
    cfg->frameFormat = reg_get_bits(tmpVal, ISO11898_INF_FF);
    cfg->frameType = reg_get_bits(tmpVal, ISO11898_INF_RTR);
    cfg->dataLen = reg_get_bits(tmpVal, ISO11898_INF_DLC);

    /* Receive with 11 bits ID */
    if(ISO11898_FRAME_FORMAT_STANDARD == cfg->frameFormat)
    {
        /* Read identifier registers */
        tmpVal = readl(ISO11898_BASE + ISO11898_IDENTIFIER_1_OFFSET);
        id = ((tmpVal & 0xFF) << 3);
        tmpVal = readl(ISO11898_BASE + ISO11898_IDENTIFIER_2_OFFSET);
        id |= (tmpVal & 0xFF) >> 5;
        /* Config SFF data registers start address */
        pData = (uint32_t *)(ISO11898_BASE + ISO11898_SFF_DATA_1_OFFSET);
    }
    /* Receive with 29 bits ID */
    else
    {
        /* Read identifier registers */
        tmpVal = readl(ISO11898_BASE + ISO11898_IDENTIFIER_1_OFFSET);
        id = ((tmpVal & 0xFF) << 21);
        tmpVal = readl(ISO11898_BASE + ISO11898_IDENTIFIER_2_OFFSET);
        id |= (tmpVal & 0xFF) << 13;
        tmpVal = readl(ISO11898_BASE + ISO11898_IDENTIFIER_3_OFFSET);
        id |= (tmpVal & 0xFF) << 5;
        tmpVal = readl(ISO11898_BASE + ISO11898_IDENTIFIER_4_OFFSET);
        id |= (tmpVal & 0xFF) >> 3;
        /* Config EFF data registers start address */
        pData = (uint32_t *)(ISO11898_BASE + ISO11898_EFF_DATA_1_OFFSET);
    }
    cfg->id = id;

    /* Move data from registers to user buff */
    for (i = 0; i < cfg->dataLen; i++)
    {
        *(cfg->dataAddr + i) = (uint8_t)(*(pData + i) & 0xFF);
    }

    ISO11898_Cmd_Release_Recv_Buff();

    return SUCCESS;
}

uint8_t ISO11898_Receive_Buff_Frame_Count_Get(void)
{
    volatile uint32_t tmpVal;

    tmpVal = readl(ISO11898_BASE + ISO11898_RX_MESSAGE_COUNT_OFFSET);
    return (uint8_t)(tmpVal & 0xFF);
}

int ISO11898_Clock_Div_Set(uint8_t div)
{
    uint32_t tmpVal;

    /* Must be operated in reset mode */
    if (ISO11898_MODE_VALUE_NORMAL == ISO11898_Mode_Get(ISO11898_MODE_TYPE_RESET))
    {
        return ERROR;
    }

    tmpVal = readl(ISO11898_BASE + ISO11898_BUS_TIMING_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISO11898_BAUD_RATE_PRESCALER, div);
    writel(tmpVal, ISO11898_BASE + ISO11898_BUS_TIMING_0_OFFSET);

    return SUCCESS;
}

uint8_t ISO11898_Clock_Div_Get(void)
{
    uint32_t tmpVal;

    tmpVal = readl(ISO11898_BASE + ISO11898_BUS_TIMING_0_OFFSET);
    return (uint8_t)reg_get_bits(tmpVal, ISO11898_BAUD_RATE_PRESCALER);
}

int ISO11898_Synchronization_Jump_Width_Set(uint8_t width)
{
    uint32_t tmpVal;

    /* Must be operated in reset mode */
    if (ISO11898_MODE_VALUE_NORMAL == ISO11898_Mode_Get(ISO11898_MODE_TYPE_RESET))
    {
        return ERROR;
    }

    tmpVal = readl(ISO11898_BASE + ISO11898_BUS_TIMING_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISO11898_SYNC_JUMP_WIDTH, width);
    writel(tmpVal, ISO11898_BASE + ISO11898_BUS_TIMING_0_OFFSET);

    return SUCCESS;
}

uint8_t ISO11898_Synchronization_Jump_Width_Get(void)
{
    uint32_t tmpVal;

    tmpVal = readl(ISO11898_BASE + ISO11898_BUS_TIMING_0_OFFSET);
    return (uint8_t)reg_get_bits(tmpVal, ISO11898_SYNC_JUMP_WIDTH);
}

int ISO11898_Sample_Parameter_Set(uint8_t num, uint8_t head, uint8_t tail)
{
    uint32_t tmpVal = 0;

    /* Must be operated in reset mode */
    if (ISO11898_MODE_VALUE_NORMAL == ISO11898_Mode_Get(ISO11898_MODE_TYPE_RESET))
    {
        return ERROR;
    }

    tmpVal = reg_set_bits(tmpVal, ISO11898_TIME_SEGMENT_1, head);
    tmpVal = reg_set_bits(tmpVal, ISO11898_TIME_SEGMENT_2, tail);
    tmpVal = reg_set_bits(tmpVal, ISO11898_SAMPLING, num);
    writel(tmpVal & 0xFF, ISO11898_BASE + ISO11898_BUS_TIMING_1_OFFSET);

    return SUCCESS;
}

void ISO11898_Sample_Parameter_Get(uint8_t *num, uint8_t *head, uint8_t *tail)
{
    uint32_t tmpVal;

    tmpVal = readl(ISO11898_BASE + ISO11898_BUS_TIMING_1_OFFSET);
    *head = (uint8_t)reg_get_bits(tmpVal, ISO11898_TIME_SEGMENT_1);
    *tail = (uint8_t)reg_get_bits(tmpVal, ISO11898_TIME_SEGMENT_2);
    *num = (uint8_t)reg_get_bits(tmpVal, ISO11898_SAMPLING);
}

void ISO11898_Filter_Set(const ISO11898_FILTER_CFG_Type *cfg)
{
    uint8_t tmpVal[8] = {0}; //tmpVal[0~3] for match, tmpVal[4~7] for mask

    /* Config filter must be in reset mode */
    ISO11898_MODE_VALUE_Type modeReset = ISO11898_Mode_Get(ISO11898_MODE_TYPE_RESET);
    ISO11898_Mode_Set(ISO11898_MODE_TYPE_RESET, ISO11898_MODE_VALUE_RESET);

    /* Single filter mode */
    if (ISO11898_MODE_VALUE_ACCEPTANCE_FILTER_SINGLE == cfg->filterMode)
    {
        ISO11898_Mode_Set(ISO11898_MODE_TYPE_ACCEPTANCE_FILTER, ISO11898_MODE_VALUE_ACCEPTANCE_FILTER_SINGLE);
        /* Filter: ID10~0 + RTR + Data1 + Data2 */
        if (ISO11898_FRAME_FORMAT_STANDARD == cfg->formatType)
        {
            tmpVal[0] = (uint8_t)(cfg->matchId1 >> 3);
            tmpVal[1] = (uint8_t)(cfg->matchId1 << 5);
            tmpVal[1] |= (uint8_t)(cfg->matchRtr1 << 4);
            tmpVal[2] = cfg->matchData1;
            tmpVal[3] = cfg->matchData2;
            tmpVal[4] = (uint8_t)(cfg->maskId1 >> 3);
            tmpVal[5] = (uint8_t)(cfg->maskId1 << 5);
            tmpVal[5] |= (uint8_t)(cfg->maskRtr1 << 4);
            tmpVal[5] |= (uint8_t)0x0F;//0b1111 for unused bit 3:0
            tmpVal[6] = cfg->maskData1;
            tmpVal[7] = cfg->maskData2;
        }
        /* Filter: ID28~0 + RTR */
        else if(ISO11898_FRAME_FORMAT_EXTENDED == cfg->formatType)
        {
            tmpVal[0] = (uint8_t)(cfg->matchId1 >> 21);
            tmpVal[1] = (uint8_t)(cfg->matchId1 >> 13);
            tmpVal[2] = (uint8_t)(cfg->matchId1 >> 5);
            tmpVal[3] = (uint8_t)(cfg->matchId1 << 3);
            tmpVal[3] |= (uint8_t)(cfg->matchRtr1 << 2);
            tmpVal[4] = (uint8_t)(cfg->maskId1 >> 21);
            tmpVal[5] = (uint8_t)(cfg->maskId1 >> 13);
            tmpVal[6] = (uint8_t)(cfg->maskId1 >> 5);
            tmpVal[7] = (uint8_t)(cfg->maskId1 << 3);
            tmpVal[7] |= (uint8_t)(cfg->maskRtr1 << 2);
            tmpVal[7] |= (uint8_t)0x03;//0b11 for unused bit 1:0
        }
    }
    /* Dual filter mode */
    else if (ISO11898_MODE_VALUE_ACCEPTANCE_FILTER_DUAL == cfg->filterMode)
    {
        ISO11898_Mode_Set(ISO11898_MODE_TYPE_ACCEPTANCE_FILTER, ISO11898_MODE_VALUE_ACCEPTANCE_FILTER_DUAL);
        /* Filter1: ID10~0 + RTR + Data1 */
        /* Filter2: ID10~0 + RTR */
        if (ISO11898_FRAME_FORMAT_STANDARD == cfg->formatType)
        {
            tmpVal[0] = (uint8_t)(cfg->matchId1 >> 3);
            tmpVal[1] = (uint8_t)(cfg->matchId1 << 5);
            tmpVal[1] |= (uint8_t)(cfg->matchRtr1 << 4);
            tmpVal[1] |= (cfg->matchData1 >> 4);
            tmpVal[2] = (uint8_t)(cfg->matchId2 >> 3);
            tmpVal[3] = (uint8_t)(cfg->matchId2 << 5);
            tmpVal[3] |= (uint8_t)(cfg->matchRtr2 << 4);
            tmpVal[3] |= (cfg->matchData1 & 0xF);
            tmpVal[4] = (uint8_t)(cfg->maskId1 >> 3);
            tmpVal[5] = (uint8_t)(cfg->maskId1 << 5);
            tmpVal[5] |= (uint8_t)(cfg->maskRtr1 << 4);
            tmpVal[5] |= (cfg->maskData1 >> 4);
            tmpVal[6] = (uint8_t)(cfg->maskId2 >> 3);
            tmpVal[7] = (uint8_t)(cfg->maskId2 << 5);
            tmpVal[7] |= (uint8_t)(cfg->maskRtr2 << 4);
            tmpVal[7] |= (cfg->maskData1 & 0xF);
        }
        /* Filter1: ID28~13 */
        /* Filter2: ID28~13 */
        else if (ISO11898_FRAME_FORMAT_EXTENDED == cfg->formatType)
        {
            tmpVal[0] = (uint8_t)(cfg->matchId1 >> 21);
            tmpVal[1] = (uint8_t)(cfg->matchId1 >> 13);
            tmpVal[2] = (uint8_t)(cfg->matchId2 >> 21);
            tmpVal[3] = (uint8_t)(cfg->matchId2 >> 13);
            tmpVal[4] = (uint8_t)(cfg->maskId1 >> 21);
            tmpVal[5] = (uint8_t)(cfg->maskId1 >> 13);
            tmpVal[6] = (uint8_t)(cfg->maskId2 >> 21);
            tmpVal[7] = (uint8_t)(cfg->maskId2 >> 13);
        }
    }
    /* Write configuration to Registers*/
    writel((uint32_t)tmpVal[0], ISO11898_BASE + ISO11898_FILTER_ACCEPTANCE_0_OFFSET);
    writel((uint32_t)tmpVal[1], ISO11898_BASE + ISO11898_FILTER_ACCEPTANCE_1_OFFSET);
    writel((uint32_t)tmpVal[2], ISO11898_BASE + ISO11898_FILTER_ACCEPTANCE_2_OFFSET);
    writel((uint32_t)tmpVal[3], ISO11898_BASE + ISO11898_FILTER_ACCEPTANCE_3_OFFSET);
    writel((uint32_t)tmpVal[4], ISO11898_BASE + ISO11898_FILTER_Mask_0_OFFSET);
    writel((uint32_t)tmpVal[5], ISO11898_BASE + ISO11898_FILTER_Mask_1_OFFSET);
    writel((uint32_t)tmpVal[6], ISO11898_BASE + ISO11898_FILTER_Mask_2_OFFSET);
    writel((uint32_t)tmpVal[7], ISO11898_BASE + ISO11898_FILTER_Mask_3_OFFSET);

    /* Recovery reset mode value */
    ISO11898_Mode_Set(ISO11898_MODE_TYPE_RESET, modeReset);
}

void ISO11898_Filter_Register_Set(ISO11898_FILTER_REGISTER_Type reg, uint8_t value)
{
    /* Config filter must be in reset mode */
    ISO11898_MODE_VALUE_Type modeReset = ISO11898_Mode_Get(ISO11898_MODE_TYPE_RESET);
    ISO11898_Mode_Set(ISO11898_MODE_TYPE_RESET, ISO11898_MODE_VALUE_RESET);

    /* Write configuration to Register*/
    writel(value, ISO11898_BASE + ISO11898_FILTER_ACCEPTANCE_0_OFFSET + 4 * reg);

    /* Recovery reset mode value */
    ISO11898_Mode_Set(ISO11898_MODE_TYPE_RESET, modeReset);
}

void ISO11898_Filter_Registers_Set(uint8_t *valueArr)
{
    /* Config filter must be in reset mode */
    ISO11898_MODE_VALUE_Type modeReset = ISO11898_Mode_Get(ISO11898_MODE_TYPE_RESET);
    ISO11898_Mode_Set(ISO11898_MODE_TYPE_RESET, ISO11898_MODE_VALUE_RESET);

    /* Write configuration to Registers*/
    writel((uint32_t)valueArr[0], ISO11898_BASE + ISO11898_FILTER_ACCEPTANCE_0_OFFSET);
    writel((uint32_t)valueArr[1], ISO11898_BASE + ISO11898_FILTER_ACCEPTANCE_1_OFFSET);
    writel((uint32_t)valueArr[2], ISO11898_BASE + ISO11898_FILTER_ACCEPTANCE_2_OFFSET);
    writel((uint32_t)valueArr[3], ISO11898_BASE + ISO11898_FILTER_ACCEPTANCE_3_OFFSET);
    writel((uint32_t)valueArr[4], ISO11898_BASE + ISO11898_FILTER_Mask_0_OFFSET);
    writel((uint32_t)valueArr[5], ISO11898_BASE + ISO11898_FILTER_Mask_1_OFFSET);
    writel((uint32_t)valueArr[6], ISO11898_BASE + ISO11898_FILTER_Mask_2_OFFSET);
    writel((uint32_t)valueArr[7], ISO11898_BASE + ISO11898_FILTER_Mask_3_OFFSET);

    /* Recovery reset mode value */
    ISO11898_Mode_Set(ISO11898_MODE_TYPE_RESET, modeReset);
}

#ifndef IPRO_USE_HAL_DRIVER
int ISO11898_Int_Callback_Install(ISO11898_INT_Type intType,intCallback_Type* cbFun)
{
    /* Check the parameters */

    iso11898IntCbfArra[intType] = cbFun;

    return SUCCESS;
}

static void ISO11898_IntHandler(void)
{
    uint32_t tmpVal = 0;
    uint32_t i = 0;

    tmpVal = readl(ISO11898_BASE + ISO11898_INTERRUPT_OFFSET);

    for(i=0; i<ISO11898_INT_ALL; i++)
    {
        if(tmpVal & (1 << i))
        {
            if (iso11898IntCbfArra[i])
            {
                iso11898IntCbfArra[i]();
            }
        }
    }
}

void ISO11898_IRQHandler(void)
{
    ISO11898_IntHandler();
}

#endif

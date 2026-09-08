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

#ifndef __DRV_DMA_H__
#define __DRV_DMA_H__

#include "dma_reg.h"
#include "drv_common.h"

typedef enum {
    DRV_DMA0_ID,    /*!< DMA0 port define,WLSYS,8 channels */
    DRV_DMA_ID_MAX, /*!< DMA MAX ID define */
} DMA_ID_Type;

typedef enum {
    DMA_LITTLE_ENDIAN = 0, /*!< DMA use little endian */
    DMA_BIG_ENDIAN,        /*!< DMA use big endian */
} DMA_Endian_Type;

typedef enum {
    DMA_SYNC_LOGIC_ENABLE = 0, /*!< DMA synchronization logic enable */
    DMA_SYNC_LOGIC_DISABLE,    /*!< DMA synchronization logic disable */
} DMA_Sync_Logic_Type;

typedef enum {
    DMA_TRNS_WIDTH_8BITS = 0, /*!< DMA transfer width:8 bits */
    DMA_TRNS_WIDTH_16BITS,    /*!< DMA transfer width:16 bits */
    DMA_TRNS_WIDTH_32BITS,    /*!< DMA transfer width:32 bits */
} DMA_Trans_Width_Type;

typedef enum {
    DMA_TRNS_M2M = 0, /*!< DMA transfer tyep:memory to memory */
    DMA_TRNS_M2P,     /*!< DMA transfer tyep:memory to peripheral */
    DMA_TRNS_P2M,     /*!< DMA transfer tyep:peripheral to memory */
    DMA_TRNS_P2P,     /*!< DMA transfer tyep:peripheral to peripheral */
} DMA_Trans_Dir_Type;

typedef enum {
    DMA_BURST_SIZE_1 = 0, /*!< DMA transfer width:8 bits */
    DMA_BURST_SIZE_4,     /*!< DMA transfer width:16 bits */
    DMA_BURST_SIZE_8,     /*!< DMA transfer width:32 bits */
    DMA_BURST_SIZE_16,    /*!< DMA transfer width:64 bits */
} DMA_Burst_Size_Type;

typedef enum {
    DMA_REQ_UART0_RX = 0, /*!< DMA request peripheral:UART0 RX */
    DMA_REQ_UART0_TX,     /*!< DMA request peripheral:UART0 TX */
    DMA_REQ_UART1_RX,     /*!< DMA request peripheral:UART1 RX */
    DMA_REQ_UART1_TX,     /*!< DMA request peripheral:UART1 TX */
    DMA_REQ_I2C_RX = 6,   /*!< DMA request peripheral:I2C RX */
    DMA_REQ_I2C_TX,       /*!< DMA request peripheral:I2C TX */
    DMA_REQ_SPI_RX = 10,  /*!< DMA request peripheral:SPI RX */
    DMA_REQ_SPI_TX,       /*!< DMA request peripheral:SPI TX */
    DMA_REQ_I2S_RX = 20,  /*!< DMA request peripheral:SPI RX */
    DMA_REQ_I2S_TX,       /*!< DMA request peripheral:SPI TX */
    DMA_REQ_GPADC0 = 22,  /*!< DMA request peripheral:GPADC0 */
    DMA_REQ_GPADC1,       /*!< DMA request peripheral:GPADC1 */
    DMA_REQ_NONE = 0,     /*!< DMA request peripheral:None */
} DMA_Periph_Req_Type;

typedef enum {
    DMA_CH0 = 0, /*!< DMA channel 0 */
    DMA_CH1,     /*!< DMA channel 1 */
    DMA_CH2,     /*!< DMA channel 2 */
    DMA_CH3,     /*!< DMA channel 3 */
    DMA_CH4,     /*!< DMA channel 4 */
    DMA_CH5,     /*!< DMA channel 5 */
    DMA_CH6,     /*!< DMA channel 6 */
    DMA_CH7,     /*!< DMA channel 7 */
    DMA_CH_MAX,  /*!<  */
} DMA_Chan_Type;

typedef enum {
    PING_INDEX = 0, /*!< PING INDEX */
    PONG_INDEX,     /*!< PONG INDEX */
} DMA_LLI_PP_Index_Type;

typedef enum {
    DMA_INT_TCOMPLETED = 0, /*!< DMA completed interrupt */
    DMA_INT_ERR,            /*!< DMA error interrupt */
    DMA_INT_ALL,            /*!< All the interrupt */
} DMA_INT_Type;

typedef struct
{
    DMA_Endian_Type endian;        /*!< DMA endian type */
    DMA_Sync_Logic_Type syncLogic; /*!< DMA synchronization logic */
} DMA_Cfg_Type;

typedef struct
{
    uint32_t srcDmaAddr;                 /*!< Source address of DMA transfer */
    uint32_t destDmaAddr;                /*!< Destination address of DMA transfer */
    uint32_t transfLength;               /*!< Transfer length, 0~4095, this is burst count */
    DMA_Trans_Dir_Type dir;              /*!< Transfer dir control. 0: Memory to Memory, 1: Memory to peripheral, 2: Peripheral to memory */
    DMA_Chan_Type ch;                    /*!< Channel select 0-7 */
    DMA_Trans_Width_Type srcTransfWidth; /*!< Transfer width. 0: 8  bits, 1: 16  bits, 2: 32  bits */
    DMA_Trans_Width_Type dstTransfWidth; /*!< Transfer width. 0: 8  bits, 1: 16  bits, 2: 32  bits */
    DMA_Burst_Size_Type srcBurstSize;    /*!< Number of data items for burst transaction length. Each item width is as same as tansfer width.
                                                 0: 1 item, 1: 4 items, 2: 8 items, 3: 16 items */
    DMA_Burst_Size_Type dstBurstSize;    /*!< Number of data items for burst transaction length. Each item width is as same as tansfer width.
                                                 0: 1 item, 1: 4 items, 2: 8 items, 3: 16 items */
    int dstAddMode;              /*!<  */
    int dstMinMode;              /*!<  */
    uint8_t fixCnt;                      /*!<  */
    uint8_t srcAddrInc;                  /*!< Source address increment. 0: No change, 1: Increment */
    uint8_t destAddrInc;                 /*!< Destination address increment. 0: No change, 1: Increment */
    DMA_Periph_Req_Type srcPeriph;       /*!< Source peripheral select */
    DMA_Periph_Req_Type dstPeriph;       /*!< Destination peripheral select */
} DMA_Channel_Cfg_Type;

typedef struct
{
    uint32_t srcDmaAddr;            /*!< Source address of DMA transfer */
    uint32_t destDmaAddr;           /*!< Destination address of DMA transfer */
    uint32_t nextLLI;               /*!< Next LLI address */
    struct DMA_Control_Reg dmaCtrl; /*!< DMA transaction control */
} DMA_LLI_Ctrl_Type;

typedef struct
{
    DMA_Trans_Dir_Type dir;        /*!< Transfer dir control. 0: Memory to Memory, 1: Memory to peripheral, 2: Peripheral to memory */
    DMA_Periph_Req_Type srcPeriph; /*!< Source peripheral select */
    DMA_Periph_Req_Type dstPeriph; /*!< Destination peripheral select */
} DMA_LLI_Cfg_Type;

typedef struct
{
    uint8_t idleIndex;                             /*!< Index Idle lliListHeader */
    uint8_t dmaId;                                 /*!< DMA ID used */
    uint8_t dmaChan;                               /*!< DMA LLI Channel used */
    DMA_LLI_Ctrl_Type *lliListHeader[2];           /*!< Ping-Pong BUf List Header */
    void (*onTransCompleted)(DMA_LLI_Ctrl_Type *); /*!< Completed Transmit One List Callback Function */
} DMA_LLI_PP_Buf;

typedef struct
{
    uint8_t trans_index;                  /*!< Ping or Pong Trigger TC */
    uint8_t dmaChan;                      /*!< DMA LLI Channel used */
    struct DMA_Control_Reg dmaCtrlRegVal; /*!< DMA Basic Pararmeter */
    DMA_LLI_Cfg_Type *DMA_LLI_Cfg;        /*!< LLI Config parameter */
    uint32_t operatePeriphAddr;           /*!< Operate Peripheral register address */
    uint32_t chache_buf_addr[2];          /*!< Ping-Pong structure chache */
    int is_single_mode;           /*!< is Ping-pong running forever or single mode ,if is single mode ping-pong will run only once
                                                 after one start */
} DMA_LLI_PP_Struct;

#define DMA_PINC_ENABLE  1
#define DMA_PINC_DISABLE 0
#define DMA_MINC_ENABLE  1
#define DMA_MINC_DISABLE 0

void DMA_Enable(DMA_ID_Type dmaId);
void DMA_Disable(DMA_ID_Type dmaId);
void DMA_Channel_Init(DMA_ID_Type dmaId, DMA_Channel_Cfg_Type *chCfg);
void DMA_Channel_Update_SrcMemcfg(DMA_ID_Type dmaId, uint8_t ch, uint32_t memAddr, uint32_t len);
void DMA_Channel_Update_DstMemcfg(DMA_ID_Type dmaId, uint8_t ch, uint32_t memAddr, uint32_t len);
uint32_t DMA_Channel_TranferSize(DMA_ID_Type dmaId, uint8_t ch);
int DMA_Channel_Is_Busy(DMA_ID_Type dmaId, uint8_t ch);
void DMA_Channel_Enable(DMA_ID_Type dmaId, uint8_t ch);
void DMA_Channel_Disable(DMA_ID_Type dmaId, uint8_t ch);
void DMA_LLI_Init(DMA_ID_Type dmaId, uint8_t ch, DMA_LLI_Cfg_Type *lliCfg);
void DMA_LLI_Update(DMA_ID_Type dmaId, uint8_t ch, uint32_t LLI);
void DMA_IntMask(DMA_ID_Type dmaId, uint8_t ch, DMA_INT_Type intType, int intMask);
uint32_t DMA_GetIntStatus(DMA_ID_Type dmaId);
uint32_t DMA_GetErrStatus(DMA_ID_Type dmaId);
void DMA_ClrIntStatus(DMA_ID_Type dmaId, uint8_t ch);
void DMA_ClrErrStatus(DMA_ID_Type dmaId, uint8_t ch);
void DMA_LLI_PpBuf_Start_New_Transmit(DMA_ID_Type dmaId, DMA_LLI_PP_Buf *dmaPpBuf);
DMA_LLI_Ctrl_Type *DMA_LLI_PpBuf_Remove_Completed_List(DMA_LLI_PP_Buf *dmaPpBuf);
void DMA_LLI_PpBuf_Append(DMA_ID_Type dmaId, DMA_LLI_PP_Buf *dmaPpBuf, DMA_LLI_Ctrl_Type *dmaLliList);
void DMA_LLI_PpBuf_Destroy(DMA_ID_Type dmaId, DMA_LLI_PP_Buf *dmaPpBuf);
void DMA_LLI_PpStruct_Start(DMA_ID_Type dmaId, DMA_LLI_PP_Struct *dmaPpStruct);
void DMA_LLI_PpStruct_Stop(DMA_ID_Type dmaId, DMA_LLI_PP_Struct *dmaPpStruct);
int DMA_LLI_PpStruct_Init(DMA_ID_Type dmaId, DMA_LLI_PP_Struct *dmaPpStruct);
int DMA_LLI_PpStruct_Set_Transfer_Len(DMA_ID_Type dmaId, DMA_LLI_PP_Struct *dmaPpStruct,
                                              uint16_t Ping_Transfer_len, uint16_t Pong_Transfer_len);
#endif /* __DRV_DMA_H__ */

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

#include "dma_ctrl_reg.h"
#include "drv_common.h"

/* Compatibility aliases for register offset macros */
#define DMA_INTTCSTATUS_OFFSET      DMA_CTRL_DMA_INTTCSTATUS_OFFSET
#define DMA_INTTCCLEAR_OFFSET       DMA_CTRL_DMA_INTTCCLEAR_OFFSET
#define DMA_INTERRORSTATUS_OFFSET   DMA_CTRL_DMA_INTERRORSTATUS_OFFSET
#define DMA_INTERRCLR_OFFSET        DMA_CTRL_DMA_INTERRCLR_OFFSET
#define DMA_TOP_CONFIG_OFFSET       DMA_CTRL_DMA_TOP_CONFIG_OFFSET

/* Channel register offsets (relative to channel base) */
#define DMA_SRCADDR_OFFSET          (0x00)
#define DMA_DSTADDR_OFFSET          (0x04)
#define DMA_LLI_OFFSET              (0x08)
#define DMA_CONTROL_OFFSET          (0x0C)
#define DMA_CONFIG_OFFSET           (0x10)

/* Compatibility aliases for field macros */
#define DMA_INTTCSTATUS             DMA_CTRL_INTTCSTATUS
#define DMA_INTTCSTATUS_POS         DMA_CTRL_INTTCSTATUS_POS
#define DMA_INTTCSTATUS_MSK         DMA_CTRL_INTTCSTATUS_MSK
#define DMA_INTTCSTATUS_UMSK        DMA_CTRL_INTTCSTATUS_UMSK

#define DMA_INTTCCLEAR              DMA_CTRL_INTTCCLEAR
#define DMA_INTTCCLEAR_POS          DMA_CTRL_INTTCCLEAR_POS
#define DMA_INTTCCLEAR_MSK          DMA_CTRL_INTTCCLEAR_MSK
#define DMA_INTTCCLEAR_UMSK         DMA_CTRL_INTTCCLEAR_UMSK

#define DMA_INTERRORSTATUS          DMA_CTRL_INTERRORSTATUS
#define DMA_INTERRORSTATUS_POS      DMA_CTRL_INTERRORSTATUS_POS
#define DMA_INTERRORSTATUS_MSK      DMA_CTRL_INTERRORSTATUS_MSK
#define DMA_INTERRORSTATUS_UMSK     DMA_CTRL_INTERRORSTATUS_UMSK

#define DMA_INTERRCLR               DMA_CTRL_INTERRCLR
#define DMA_INTERRCLR_POS           DMA_CTRL_INTERRCLR_POS
#define DMA_INTERRCLR_MSK           DMA_CTRL_INTERRCLR_MSK
#define DMA_INTERRCLR_UMSK          DMA_CTRL_INTERRCLR_UMSK

#define DMA_E                       DMA_CTRL_E
#define DMA_E_POS                   DMA_CTRL_E_POS
#define DMA_E_MSK                   DMA_CTRL_E_MSK
#define DMA_E_UMSK                  DMA_CTRL_E_UMSK

#define DMA_TRANSFERSIZE            DMA_CTRL_TRANSFERSIZE
#define DMA_TRANSFERSIZE_POS        DMA_CTRL_TRANSFERSIZE_POS
#define DMA_TRANSFERSIZE_MSK        DMA_CTRL_TRANSFERSIZE_MSK
#define DMA_TRANSFERSIZE_UMSK       DMA_CTRL_TRANSFERSIZE_UMSK

#define DMA_SWIDTH                  DMA_CTRL_SWIDTH
#define DMA_SWIDTH_POS              DMA_CTRL_SWIDTH_POS
#define DMA_SWIDTH_MSK              DMA_CTRL_SWIDTH_MSK
#define DMA_SWIDTH_UMSK             DMA_CTRL_SWIDTH_UMSK

#define DMA_DWIDTH                  DMA_CTRL_DWIDTH
#define DMA_DWIDTH_POS              DMA_CTRL_DWIDTH_POS
#define DMA_DWIDTH_MSK              DMA_CTRL_DWIDTH_MSK
#define DMA_DWIDTH_UMSK             DMA_CTRL_DWIDTH_UMSK

#define DMA_SBSIZE                  DMA_CTRL_SBSIZE
#define DMA_SBSIZE_POS              DMA_CTRL_SBSIZE_POS
#define DMA_SBSIZE_MSK              DMA_CTRL_SBSIZE_MSK
#define DMA_SBSIZE_UMSK             DMA_CTRL_SBSIZE_UMSK

#define DMA_DBSIZE                  DMA_CTRL_DBSIZE
#define DMA_DBSIZE_POS              DMA_CTRL_DBSIZE_POS
#define DMA_DBSIZE_MSK              DMA_CTRL_DBSIZE_MSK
#define DMA_DBSIZE_UMSK             DMA_CTRL_DBSIZE_UMSK

#define DMA_SI                      DMA_CTRL_SI
#define DMA_SI_POS                  DMA_CTRL_SI_POS
#define DMA_SI_MSK                  DMA_CTRL_SI_MSK
#define DMA_SI_UMSK                 DMA_CTRL_SI_UMSK

#define DMA_DI                      DMA_CTRL_DI
#define DMA_DI_POS                  DMA_CTRL_DI_POS
#define DMA_DI_MSK                  DMA_CTRL_DI_MSK
#define DMA_DI_UMSK                 DMA_CTRL_DI_UMSK

#define DMA_I                       DMA_CTRL_I
#define DMA_I_POS                   DMA_CTRL_I_POS
#define DMA_I_MSK                   DMA_CTRL_I_MSK
#define DMA_I_UMSK                  DMA_CTRL_I_UMSK

#define DMA_ITC                     DMA_CTRL_ITC
#define DMA_ITC_POS                 DMA_CTRL_ITC_POS
#define DMA_ITC_MSK                 DMA_CTRL_ITC_MSK
#define DMA_ITC_UMSK                DMA_CTRL_ITC_UMSK

#define DMA_IE                      DMA_CTRL_IE
#define DMA_IE_POS                  DMA_CTRL_IE_POS
#define DMA_IE_MSK                  DMA_CTRL_IE_MSK
#define DMA_IE_UMSK                 DMA_CTRL_IE_UMSK

#define DMA_FLOWCNTRL               DMA_CTRL_FLOWCNTRL
#define DMA_FLOWCNTRL_POS           DMA_CTRL_FLOWCNTRL_POS
#define DMA_FLOWCNTRL_MSK           DMA_CTRL_FLOWCNTRL_MSK
#define DMA_FLOWCNTRL_UMSK          DMA_CTRL_FLOWCNTRL_UMSK

#define DMA_SRCPERIPHERAL           DMA_CTRL_SRCPERIPHERAL
#define DMA_SRCPERIPHERAL_POS       DMA_CTRL_SRCPERIPHERAL_POS
#define DMA_SRCPERIPHERAL_MSK       DMA_CTRL_SRCPERIPHERAL_MSK
#define DMA_SRCPERIPHERAL_UMSK      DMA_CTRL_SRCPERIPHERAL_UMSK

#define DMA_DSTPERIPHERAL           DMA_CTRL_DSTPERIPHERAL
#define DMA_DSTPERIPHERAL_POS       DMA_CTRL_DSTPERIPHERAL_POS
#define DMA_DSTPERIPHERAL_MSK       DMA_CTRL_DSTPERIPHERAL_MSK
#define DMA_DSTPERIPHERAL_UMSK      DMA_CTRL_DSTPERIPHERAL_UMSK

#define DMA_LLICOUNTER              DMA_CTRL_LLICOUNTER
#define DMA_LLICOUNTER_POS          DMA_CTRL_LLICOUNTER_POS
#define DMA_LLICOUNTER_MSK          DMA_CTRL_LLICOUNTER_MSK
#define DMA_LLICOUNTER_UMSK         DMA_CTRL_LLICOUNTER_UMSK

#define DMA_DST_ADD_MODE            DMA_CTRL_DST_ADD_MODE
#define DMA_DST_ADD_MODE_POS        DMA_CTRL_DST_ADD_MODE_POS
#define DMA_DST_ADD_MODE_MSK        DMA_CTRL_DST_ADD_MODE_MSK
#define DMA_DST_ADD_MODE_UMSK       DMA_CTRL_DST_ADD_MODE_UMSK

#define DMA_DST_MIN_MODE            DMA_CTRL_DST_MIN_MODE
#define DMA_DST_MIN_MODE_POS        DMA_CTRL_DST_MIN_MODE_POS
#define DMA_DST_MIN_MODE_MSK        DMA_CTRL_DST_MIN_MODE_MSK
#define DMA_DST_MIN_MODE_UMSK       DMA_CTRL_DST_MIN_MODE_UMSK

#define DMA_FIX_CNT                 DMA_CTRL_FIX_CNT
#define DMA_FIX_CNT_POS             DMA_CTRL_FIX_CNT_POS
#define DMA_FIX_CNT_MSK             DMA_CTRL_FIX_CNT_MSK
#define DMA_FIX_CNT_UMSK            DMA_CTRL_FIX_CNT_UMSK

/* DMA Control Register bit field structure */
struct DMA_Control_Reg {
    uint32_t TransferSize   : 12; /* [11: 0], r/w, 0x0 */
    uint32_t SBSize         :  2; /* [13:12], r/w, 0x1 */
    uint32_t dst_min_mode   :  1; /* [   14], r/w, 0x0 */
    uint32_t DBSize         :  2; /* [16:15], r/w, 0x1 */
    uint32_t dst_add_mode   :  1; /* [   17], r/w, 0x0 */
    uint32_t SWidth         :  2; /* [19:18], r/w, 0x2 */
    uint32_t reserved_20    :  1; /* [   20], rsvd, 0x0 */
    uint32_t DWidth         :  2; /* [22:21], r/w, 0x2 */
    uint32_t fix_cnt        :  3; /* [25:23], r/w, 0x0 */
    uint32_t SI             :  1; /* [   26], r/w, 0x1 */
    uint32_t DI             :  1; /* [   27], r/w, 0x1 */
    uint32_t Prot           :  3; /* [30:28], r/w, 0x0 */
    uint32_t I              :  1; /* [   31], r/w, 0x0 */
};

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
    DMA_TRNS_WIDTH_64BITS,    /*!< DMA transfer width:64 bits, only for DMA2 channel 0 and channel 1, others should not use this */
} DMA_Trans_Width_Type;

typedef enum {
    DMA_TRNS_M2M = 0, /*!< DMA transfer tyep:memory to memory */
    DMA_TRNS_M2P,     /*!< DMA transfer tyep:memory to peripheral */
    DMA_TRNS_P2M,     /*!< DMA transfer tyep:peripheral to memory */
    DMA_TRNS_P2P,     /*!< DMA transfer tyep:peripheral to peripheral */
} DMA_Trans_Dir_Type;

typedef enum {
    DMA_BURST_SIZE_1 = 0, /*!< DMA burst size:1 * transfer width */
    DMA_BURST_SIZE_4,     /*!< DMA burst size:4 * transfer width */
    DMA_BURST_SIZE_8,     /*!< DMA burst size:8 * transfer width */
    DMA_BURST_SIZE_16,    /*!< DMA burst size:16 * transfer width */
} DMA_Burst_Size_Type;

typedef enum {
    DMA_REQ_UART0_RX = 0,  /*!< DMA request peripheral:UART0 RX */
    DMA_REQ_UART0_TX = 1,  /*!< DMA request peripheral:UART0 TX */
    DMA_REQ_UART1_RX = 2,  /*!< DMA request peripheral:UART1 RX */
    DMA_REQ_UART1_TX = 3,  /*!< DMA request peripheral:UART1 TX */
    DMA_REQ_I2C0_RX = 6,   /*!< DMA request peripheral:I2C0 RX */
    DMA_REQ_I2C0_TX = 7,   /*!< DMA request peripheral:I2C0 TX */
    DMA_REQ_GPIO_TX = 9,   /*!< DMA request peripheral:GPIO TX */
    DMA_REQ_SPI0_RX = 10,  /*!< DMA request peripheral:SPI0 RX */
    DMA_REQ_SPI0_TX = 11,  /*!< DMA request peripheral:SPI0 TX */
    DMA_REQ_AUDIO_TX = 13, /*!< DMA request peripheral:AUDIO TX */
    DMA_REQ_I2C1_RX = 14,  /*!< DMA request peripheral:I2C1 RX */
    DMA_REQ_I2C1_TX = 15,  /*!< DMA request peripheral:I2C1 TX */
    DMA_REQ_I2S_RX = 16,   /*!< DMA request peripheral:I2S RX */
    DMA_REQ_I2S_TX = 17,   /*!< DMA request peripheral:I2S TX */
    DMA_REQ_DBI_TX = 20,   /*!< DMA request peripheral:DBI TX */
    DMA_REQ_SOLO_RX = 21,  /*!< DMA request peripheral:SOLO RX */
    DMA_REQ_GPADC_RX = 22, /*!< DMA request peripheral:GPADC RX */
    DMA_REQ_GPDAC_TX = 23, /*!< DMA request peripheral:GPDAC TX */
    DMA_REQ_PEC0_RX = 24,  /*!< DMA request peripheral:PEC0 RX */
    DMA_REQ_PEC1_RX = 25,  /*!< DMA request peripheral:PEC1 RX */
    DMA_REQ_PEC2_RX = 26,  /*!< DMA request peripheral:PEC2 RX */
    DMA_REQ_PEC3_RX = 27,  /*!< DMA request peripheral:PEC3 RX */
    DMA_REQ_PEC0_TX = 28,  /*!< DMA request peripheral:PEC0 TX */
    DMA_REQ_PEC1_TX = 29,  /*!< DMA request peripheral:PEC1 TX */
    DMA_REQ_PEC2_TX = 30,  /*!< DMA request peripheral:PEC2 TX */
    DMA_REQ_PEC3_TX = 31,  /*!< DMA request peripheral:PEC3 TX */
    DMA_REQ_NONE = 0,      /*!< DMA request peripheral:None */
} DMA_Periph_Req_Type;

typedef enum {
    DMA_CH0 = 0, /*!< DMA channel 0 */
    DMA_CH1,     /*!< DMA channel 1 */
    DMA_CH2,     /*!< DMA channel 2 */
    DMA_CH3,     /*!< DMA channel 3 */
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
    DMA_Trans_Width_Type srcTransfWidth; /*!< Transfer width. 0: 8  bits, 1: 16  bits, 2: 32  bits, 3: 64  bits(only for DMA2 channel 0 and
                                                 channel 1) */
    DMA_Trans_Width_Type dstTransfWidth; /*!< Transfer width. 0: 8  bits, 1: 16  bits, 2: 32  bits, 3: 64  bits(only for DMA2 channel 0 and
                                                 channel 1) */
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
    uint8_t pingpongIndex;                /*!< Ping or Pong Trigger TC */
    uint8_t dmaId;                        /*!< DMA ID used */
    uint8_t dmaChan;                      /*!< DMA LLI Channel used */
    struct DMA_Control_Reg dmaCtrlRegVal; /*!< DMA Basic Pararmeter */
    DMA_LLI_Cfg_Type *lliCfg;             /*!< LLI Config parameter */
    uint32_t operatePeriphAddr;           /*!< Operate Peripheral register address */
    uint32_t pingpongBufAddr[2];          /*!< Ping-Pong addr */
} DMA_LLI_PP_Struct;

#define IS_DMA_ID_TYPE(type) (((type) == DMA0_ID) || \
                              ((type) == DMA_ID_MAX))

#define IS_DMA_ENDIAN_TYPE(type) (((type) == DMA_LITTLE_ENDIAN) || \
                                  ((type) == DMA_BIG_ENDIAN))

#define IS_DMA_SYNC_LOGIC_TYPE(type) (((type) == DMA_SYNC_LOGIC_ENABLE) || \
                                      ((type) == DMA_SYNC_LOGIC_DISABLE))

#define IS_DMA_TRANS_WIDTH_TYPE(type) (((type) == DMA_TRNS_WIDTH_8BITS) ||  \
                                       ((type) == DMA_TRNS_WIDTH_16BITS) || \
                                       ((type) == DMA_TRNS_WIDTH_32BITS) || \
                                       ((type) == DMA_TRNS_WIDTH_64BITS))

#define IS_DMA_TRANS_DIR_TYPE(type) (((type) == DMA_TRNS_M2M) || \
                                     ((type) == DMA_TRNS_M2P) || \
                                     ((type) == DMA_TRNS_P2M) || \
                                     ((type) == DMA_TRNS_P2P))

#define IS_DMA_BURST_SIZE_TYPE(type) (((type) == DMA_BURST_SIZE_1) || \
                                      ((type) == DMA_BURST_SIZE_4) || \
                                      ((type) == DMA_BURST_SIZE_8) || \
                                      ((type) == DMA_BURST_SIZE_16))

#define IS_DMA_PERIPH_REQ_TYPE(type) (((type) == DMA_REQ_UART0_RX) || \
                                      ((type) == DMA_REQ_UART0_TX) || \
                                      ((type) == DMA_REQ_UART1_RX) || \
                                      ((type) == DMA_REQ_UART1_TX) || \
                                      ((type) == DMA_REQ_UART2_RX) || \
                                      ((type) == DMA_REQ_UART2_TX) || \
                                      ((type) == DMA_REQ_I2C0_RX) ||  \
                                      ((type) == DMA_REQ_I2C0_TX) ||  \
                                      ((type) == DMA_REQ_IR_TX) ||    \
                                      ((type) == DMA_REQ_GPIO_TX) ||  \
                                      ((type) == DMA_REQ_SPI0_RX) ||  \
                                      ((type) == DMA_REQ_SPI0_TX) ||  \
                                      ((type) == DMA_REQ_AUDIO_RX) || \
                                      ((type) == DMA_REQ_AUDIO_TX) || \
                                      ((type) == DMA_REQ_I2C1_RX) ||  \
                                      ((type) == DMA_REQ_I2C1_TX) ||  \
                                      ((type) == DMA_REQ_I2S_RX) ||   \
                                      ((type) == DMA_REQ_I2S_TX) ||   \
                                      ((type) == DMA_REQ_PDM_RX) ||   \
                                      ((type) == DMA_REQ_GPADC_RX) || \
                                      ((type) == DMA_REQ_GPADC_TX) || \
                                      ((type) == DMA_REQ_UART3_RX) || \
                                      ((type) == DMA_REQ_UART3_TX) || \
                                      ((type) == DMA_REQ_SPI1_RX) ||  \
                                      ((type) == DMA_REQ_SPI1_TX) ||  \
                                      ((type) == DMA_REQ_UART4_RX) || \
                                      ((type) == DMA_REQ_UART4_TX) || \
                                      ((type) == DMA_REQ_I2C2_RX) ||  \
                                      ((type) == DMA_REQ_I2C2_TX) ||  \
                                      ((type) == DMA_REQ_I2C3_RX) ||  \
                                      ((type) == DMA_REQ_I2C3_TX) ||  \
                                      ((type) == DMA_REQ_DSI_RX) ||   \
                                      ((type) == DMA_REQ_DBI_TX) ||   \
                                      ((type) == DMA_REQ_DBI_TX) ||   \
                                      ((type) == DMA_REQ_NONE))

#define IS_DMA_CHAN_TYPE(type) (((type) == DMA_CH0) || \
                                ((type) == DMA_CH1) || \
                                ((type) == DMA_CH2) || \
                                ((type) == DMA_CH3) || \
                                ((type) == DMA_CH4) || \
                                ((type) == DMA_CH5) || \
                                ((type) == DMA_CH6) || \
                                ((type) == DMA_CH7) || \
                                ((type) == DMA_CH_MAX))

#define IS_DMA_LLI_PP_INDEX_TYPE(type) (((type) == PING_INDEX) || \
                                        ((type) == PONG_INDEX))

#define IS_DMA_INT_TYPE(type) (((type) == DMA_INT_TCOMPLETED) || \
                               ((type) == DMA_INT_ERR) ||        \
                               ((type) == DMA_INT_ALL))

#define DMA_PINC_ENABLE  1
#define DMA_PINC_DISABLE 0
#define DMA_MINC_ENABLE  1
#define DMA_MINC_DISABLE 0

#ifndef IPRO_USE_HAL_DRIVER
void DMA0_ALL_IRQHandler(void);
void DMA1_ALL_IRQHandler(void);
void DMA2_INT0_IRQHandler(void);
void DMA2_INT1_IRQHandler(void);
void DMA2_INT2_IRQHandler(void);
void DMA2_INT3_IRQHandler(void);
void DMA2_INT4_IRQHandler(void);
void DMA2_INT5_IRQHandler(void);
void DMA2_INT6_IRQHandler(void);
void DMA2_INT7_IRQHandler(void);
#endif
void DMA_Enable(DMA_ID_Type dmaId);
void DMA_Disable(DMA_ID_Type dmaId);
void DMA_Channel_Init(DMA_ID_Type dmaId, DMA_Channel_Cfg_Type *chCfg);
void DMA_DeInit(DMA_ID_Type dmaId);
void DMA_Channel_Update_SrcMemcfg(DMA_ID_Type dmaId, uint8_t ch, uint32_t memAddr, uint32_t len);
void DMA_Channel_Update_DstMemcfg(DMA_ID_Type dmaId, uint8_t ch, uint32_t memAddr, uint32_t len);
uint32_t DMA_Channel_TranferSize(DMA_ID_Type dmaId, uint8_t ch);
int DMA_Channel_Is_Busy(DMA_ID_Type dmaId, uint8_t ch);
void DMA_Channel_Enable(DMA_ID_Type dmaId, uint8_t ch);
void DMA_Channel_Disable(DMA_ID_Type dmaId, uint8_t ch);
void DMA_LLI_Init(DMA_ID_Type dmaId, uint8_t ch, DMA_LLI_Cfg_Type *lliCfg);
void DMA_LLI_Update(DMA_ID_Type dmaId, uint8_t ch, uint32_t LLI);
uint32_t DMA_LLI_Get_Counter(DMA_ID_Type dmaId, uint8_t ch);
uint32_t DMA_LLI_Get_Dstaddr(DMA_ID_Type dmaId, uint8_t ch);
void DMA_IntMask(DMA_ID_Type dmaId, uint8_t ch, DMA_INT_Type intType, int intMask);
uint32_t DMA_GetIntStatus(DMA_ID_Type dmaId);
void DMA_ClrIntStatus(DMA_ID_Type dmaId, uint8_t ch);
void DMA_ClrErrStatus(DMA_ID_Type dmaId, uint8_t ch);
uint32_t DMA_GetErrStatus(DMA_ID_Type dmaId);
void DMA_LLI_PpBuf_Start_New_Transmit(DMA_LLI_PP_Buf *dmaPpBuf);
DMA_LLI_Ctrl_Type *DMA_LLI_PpBuf_Remove_Completed_List(DMA_LLI_PP_Buf *dmaPpBuf);
void DMA_LLI_PpBuf_Append(DMA_LLI_PP_Buf *dmaPpBuf, DMA_LLI_Ctrl_Type *dmaLliList);
void DMA_LLI_PpBuf_Destroy(DMA_LLI_PP_Buf *dmaPpBuf);
void DMA_Int_Callback_Install(DMA_ID_Type dmaId, DMA_Chan_Type dmaChan, DMA_INT_Type intType, intCallback_Type *cbFun);
void DMA_LLI_PpStruct_Start(DMA_LLI_PP_Struct *dmaPpStruct);
void DMA_LLI_PpStruct_Stop(DMA_LLI_PP_Struct *dmaPpStruct);
int DMA_LLI_PpStruct_Init(DMA_LLI_PP_Struct *dmaPpStruct);
int DMA_LLI_PpStruct_Set_Transfer_Len(DMA_LLI_PP_Struct *dmaPpStruct,
                                              uint16_t Ping_Transfer_len, uint16_t Pong_Transfer_len);

#endif /* __DRV_DMA_H__ */

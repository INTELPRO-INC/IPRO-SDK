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


#ifndef __DRV_EMAC_H__
#define __DRV_EMAC_H__

#include "emac_reg.h"
#include "drv_common.h"

typedef enum {
    EMAC0_ID,    /*!< EMAC0 port define */
    EMAC_ID_MAX, /*!< I2S MAX ID define */
} EMAC_ID_Type;

typedef enum {
    EMAC_MODE_HALFDUPLEX, /*!< ETH half-duplex */
    EMAC_MODE_FULLDUPLEX, /*!< ETH full-duplex */
} EMAC_Duplex_Type;

typedef enum {
    EMAC_SPEED_10M,  /*!< ETH speed 10M */
    EMAC_SPEED_100M, /*!< ETH speed 100M */
} EMAC_Speed_Type;

typedef enum {
    EMAC_INT_TX_DONE = 0x01,  /*!< Transmit one frame done */
    EMAC_INT_TX_ERROR = 0x02, /*!< Transmit error occur */
    EMAC_INT_RX_DONE = 0x04,  /*!< Receive one frame done */
    EMAC_INT_RX_ERROR = 0x08, /*!< Receive error occur */
    EMAC_INT_RX_BUSY = 0x10,  /*!< Receive busy due to no free RX buffer Descripter */
    EMAC_INT_TX_CTRL = 0x20,  /*!< Transmit control frame */
    EMAC_INT_RX_CTRL = 0x40,  /*!< Receive control frame */
    EMAC_INT_ALL = 0x7F,      /*!<  */
} EMAC_INT_Type;

typedef enum {
    EMAC_INT_TX_DONE_IDX = 0x0,  /*!< Transmit one frame done */
    EMAC_INT_TX_ERROR_IDX = 0x1, /*!< Transmit error occur */
    EMAC_INT_RX_DONE_IDX = 0x2,  /*!< Receive one frame done */
    EMAC_INT_RX_ERROR_IDX = 0x3, /*!< Receive error occur */
    EMAC_INT_RX_BUSY_IDX = 0x4,  /*!< Receive busy due to no free RX buffer Descripter */
    EMAC_INT_TX_CTRL_IDX = 0x5,  /*!< Transmit control frame */
    EMAC_INT_RX_CTRL_IDX = 0x6,  /*!< Receive control frame */
    EMAC_INT_CNT,                /*!< EMAC INT source count */
} EMAC_INT_Index;

typedef struct
{
    int recvSmallFrame;     /*!< Receive small frmae or not */
    int recvHugeFrame;      /*!< Receive huge frmae(>64K bytes) or not */
    int padEnable;          /*!< Enable padding for frame which is less than MINFL or not */
    int crcEnable;          /*!< Enable hardware CRC or not */
    int noPreamble;         /*!< Enable preamble or not */
    int recvBroadCast;      /*!< Receive broadcast frame or not */
    int interFrameGapCheck; /*!< Check inter frame gap or not */
    int miiNoPreamble;      /*!< Enable MII interface preamble or not */
    int speed_mode;          /*!< Speed mode */
    uint8_t miiClkDiv;              /*!< MII interface clock divider from bus clock */
    uint8_t maxTxRetry;             /*!< Maximum tx retry count */
    uint16_t interFrameGapValue;    /*!< Inter frame gap vaule in clock cycles(default 24),which equals 9.6 us for 10 Mbps and 0.96 us
                                                 for 100 Mbps mode */
    uint16_t minFrameLen;           /*!< Minimum frame length */
    uint16_t maxFrameLen;           /*!< Maximum frame length */
    uint16_t collisionValid;        /*!< Collision valid value */
    uint8_t macAddr[6];             /*!< MAC Address */
} EMAC_CFG_Type;

typedef struct
{
    uint32_t CS    : 1;  /*!< Carrier Sense Lost */
    uint32_t DF    : 1;  /*!< Defer Indication */
    uint32_t LC    : 1;  /*!< Late Collision */
    uint32_t RL    : 1;  /*!< Retransmission Limit */
    uint32_t RC    : 4;  /*!< Retry Count */
    uint32_t UR    : 1;  /*!< Underrun */
    uint32_t RSVD  : 1;  /*!< Reserved */
    uint32_t EoF   : 1;  /*!< EOF or not */
    uint32_t CRC   : 1;  /*!< CRC Enable */
    uint32_t PAD   : 1;  /*!< PAD enable */
    uint32_t WR    : 1;  /*!< Wrap */
    uint32_t IRQ   : 1;  /*!< Interrupt Request Enable */
    uint32_t Busy  : 1;  /*!< The data buffer is ready for transmission or is currently being transmitted. You are not
                                                 allowed to change it */
    uint32_t TxLen : 16; /*!< TX Data buffer length */
    uint32_t TXBuffer;   /*!< TX buffer address */
} EMAC_Tx_DMA_Desc_Type;

typedef struct
{
    uint32_t LC    : 1;  /*!< Late Collision */
    uint32_t CRC   : 1;  /*!< RX CRC Error */
    uint32_t SF    : 1;  /*!< Short Frame */
    uint32_t TL    : 1;  /*!< Too Long */
    uint32_t DN    : 4;  /*!< Dribble Nibble */
    uint32_t RE    : 1;  /*!< Receive Error */
    uint32_t OR    : 1;  /*!< Overrun */
    uint32_t M     : 1;  /*!< Miss */
    uint32_t CF    : 1;  /*!< Control Frame */
    uint32_t RSVD  : 4;  /*!< PAD enable */
    uint32_t WR    : 1;  /*!< Wrap */
    uint32_t IRQ   : 1;  /*!< Interrupt Request Enable */
    uint32_t Empty : 1;  /*!< The data buffer is empty (and ready for receiving data) or currently receiving data */
    uint32_t RxLen : 16; /*!< RX Data buffer length */
    uint32_t RXBuffer;   /*!< RX buffer address */
} EMAC_Rx_DMA_Desc_Type;

typedef struct
{
    uint32_t C_S_L;  /*!< Buffer Descriptors(BD) control,status,length */
    uint32_t Buffer; /*!< BD buffer address */
} EMAC_BD_Desc_Type;

typedef struct
{
    EMAC_BD_Desc_Type *bd; /*!< Tx descriptor header pointer */
    uint8_t txIndexEMAC;   /*!< TX index: EMAC */
    uint8_t txIndexCPU;    /*!< TX index: CPU/SW */
    uint8_t txBuffLimit;   /*!< TX index max */
    uint8_t rsv0;          /*!< RSV0 */
    uint8_t rxIndexEMAC;   /*!< RX index: EMAC */
    uint8_t rxIndexCPU;    /*!< RX index: CPU/SW */
    uint8_t rxBuffLimit;   /*!< RX index max */
    uint8_t rsv1;          /*!< rsv1 */
} EMAC_Handle_Type;

#define IS_EMAC_ID_TYPE(type) (((type) == EMAC0_ID) || \
                               ((type) == EMAC_ID_MAX))

#define IS_EMAC_DUPLEX_TYPE(type) (((type) == EMAC_MODE_HALFDUPLEX) || \
                                   ((type) == EMAC_MODE_FULLDUPLEX))

#define IS_EMAC_SPEED_TYPE(type) (((type) == EMAC_SPEED_10M) || \
                                  ((type) == EMAC_SPEED_100M))

#define IS_EMAC_INT_TYPE(type) (((type) == EMAC_INT_TX_DONE) ||  \
                                ((type) == EMAC_INT_TX_ERROR) || \
                                ((type) == EMAC_INT_RX_DONE) ||  \
                                ((type) == EMAC_INT_RX_ERROR) || \
                                ((type) == EMAC_INT_RX_BUSY) ||  \
                                ((type) == EMAC_INT_TX_CTRL) ||  \
                                ((type) == EMAC_INT_RX_CTRL) ||  \
                                ((type) == EMAC_INT_ALL))

#define IS_EMAC_INT_INDEX(type) (((type) == EMAC_INT_TX_DONE_IDX) ||  \
                                 ((type) == EMAC_INT_TX_ERROR_IDX) || \
                                 ((type) == EMAC_INT_RX_DONE_IDX) ||  \
                                 ((type) == EMAC_INT_RX_ERROR_IDX) || \
                                 ((type) == EMAC_INT_RX_BUSY_IDX) ||  \
                                 ((type) == EMAC_INT_TX_CTRL_IDX) ||  \
                                 ((type) == EMAC_INT_RX_CTRL_IDX) ||  \
                                 ((type) == EMAC_INT_CNT))

#define BD_TX_CS_POS              (0) /*!< Carrier Sense Lost */
#define BD_TX_CS_LEN              (1)
#define BD_TX_DF_POS              (1) /*!< Defer Indication */
#define BD_TX_DF_LEN              (1)
#define BD_TX_LC_POS              (2) /*!< Late Collision */
#define BD_TX_LC_LEN              (1)
#define BD_TX_RL_POS              (3) /*!< Retransmission Limit */
#define BD_TX_RL_LEN              (1)
#define BD_TX_RTRY_POS            (4) /*!< Retry Count */
#define BD_TX_RTRY_LEN            (4)
#define BD_TX_UR_POS              (8) /*!< Underrun */
#define BD_TX_UR_LEN              (1)
#define BD_TX_EOF_POS             (10) /*!< EOF */
#define BD_TX_EOF_LEN             (1)
#define BD_TX_CRC_POS             (11) /*!< CRC Enable */
#define BD_TX_CRC_LEN             (1)
#define BD_TX_PAD_POS             (12) /*!< PAD enable */
#define BD_TX_PAD_LEN             (1)
#define BD_TX_WR_POS              (13) /*!< Wrap */
#define BD_TX_WR_LEN              (1)
#define BD_TX_IRQ_POS             (14) /*!< Interrupt Request Enable */
#define BD_TX_IRQ_LEN             (1)
#define BD_TX_RD_POS              (15) /*!< The data buffer is ready for transmission or is currently being transmitted. You are not allowed to change it */
#define BD_TX_RD_LEN              (1)
#define BD_TX_LEN_POS             (16) /*!< TX Data buffer length */
#define BD_TX_LEN_LEN             (16)
#define BD_RX_LC_POS              (0) /*!< Late Collision */
#define BD_RX_LC_LEN              (1)
#define BD_RX_CRC_POS             (1) /*!< RX CRC Error */
#define BD_RX_CRC_LEN             (1)
#define BD_RX_SF_POS              (2) /*!< Short Frame */
#define BD_RX_SF_LEN              (1)
#define BD_RX_TL_POS              (3) /*!< Too Long */
#define BD_RX_TL_LEN              (1)
#define BD_RX_DN_POS              (4) /*!< Dribble Nibble */
#define BD_RX_DN_LEN              (1)
#define BD_RX_RE_POS              (5) /*!< Receive Error */
#define BD_RX_RE_LEN              (1)
#define BD_RX_OR_POS              (6) /*!< Overrun */
#define BD_RX_OR_LEN              (1)
#define BD_RX_M_POS               (7) /*!< Miss */
#define BD_RX_M_LEN               (1)
#define BD_RX_CF_POS              (8) /*!< Control Frame Received */
#define BD_RX_CF_LEN              (1)
#define BD_RX_WR_POS              (13) /*!< Wrap */
#define BD_RX_WR_LEN              (1)
#define BD_RX_IRQ_POS             (14) /*!< Interrupt Request Enable */
#define BD_RX_IRQ_LEN             (1)
#define BD_RX_E_POS               (15) /*!< The data buffer is empty (and ready for receiving data) or currently receiving data */
#define BD_RX_E_LEN               (1)
#define BD_RX_LEN_POS             (16) /*!< RX Data buffer length */
#define BD_RX_LEN_LEN             (16)
#define EMAC_BD_FIELD_MSK(field)  (((1U << BD_##field##_LEN) - 1) << BD_##field##_POS)
#define EMAC_BD_FIELD_UMSK(field) (~(((1U << BD_##field##_LEN) - 1) << BD_##field##_POS))
/* DMA Descriptor offset */
#define EMAC_DMA_DESC_OFFSET 0x400

/* ETH packet size */
// ETH     | Header | Extra | VLAN tag | Payload   | CRC |
// Size    | 14     | 2     | 4        | 46 ~ 1500 | 4   |
#define ETH_MAX_PACKET_SIZE          ((uint32_t)1524U) /*!< ETH_HEADER + ETH_EXTRA + ETH_VLAN_TAG + ETH_MAX_ETH_PAYLOAD + ETH_CRC */
#define ETH_HEADER_SZIE              ((uint32_t)14U) /*!< 6 byte Dest addr, 6 byte Src addr, 2 byte length/type */
#define ETH_CRC_SIZE                 ((uint32_t)4U) /*!< Ethernet CRC */
#define ETH_EXTRA_SIZE               ((uint32_t)2U) /*!< Extra bytes in some cases */
#define ETH_VLAN_TAG_SIZE            ((uint32_t)4U) /*!< optional 802.1q VLAN Tag */
#define ETH_MIN_ETH_PAYLOAD_SIZE     ((uint32_t)46U) /*!< Minimum Ethernet payload size */
#define ETH_MAX_ETH_PAYLOAD_SIZE     ((uint32_t)1500U) /*!< Maximum Ethernet payload size */
#define ETH_JUMBO_FRAME_PAYLOAD_SIZE ((uint32_t)9000U) /*!< Jumbo frame payload size */

int EMAC_Init(EMAC_ID_Type emacId, EMAC_CFG_Type *cfg);
int EMAC_DeInit(EMAC_ID_Type emacId);
int EMAC_DMADescListInit(EMAC_ID_Type emacId, EMAC_Handle_Type *handle, uint8_t *txBuff, uint32_t txBuffCount,
                                 uint8_t *rxBuff, uint32_t rxBuffCount);
int EMAC_DMATxDescGet(EMAC_Handle_Type *handle, EMAC_BD_Desc_Type **txDMADesc);
int EMAC_StartTx(EMAC_Handle_Type *handle, EMAC_BD_Desc_Type *txDMADesc, uint32_t len);
int EMAC_Enable(EMAC_ID_Type emacId);
int EMAC_Disable(EMAC_ID_Type emacId);
int EMAC_IntMask(EMAC_ID_Type emacId, EMAC_INT_Type intType, int intMask);
int EMAC_GetIntStatus(EMAC_ID_Type emacId, EMAC_INT_Type intType);
int EMAC_ClrIntStatus(EMAC_ID_Type emacId, EMAC_INT_Type intType);
int EMAC_Int_Callback_Install(EMAC_ID_Type emacId, EMAC_INT_Index intIdx, intCallback_Type *cbFun);
int EMAC_TxPauseReq(EMAC_ID_Type emacId, uint16_t timeCount);
int EMAC_SetHash(EMAC_ID_Type emacId, uint32_t hash0, uint32_t hash1);
int EMAC_Phy_Read(EMAC_ID_Type emacId, uint16_t phyReg, uint16_t *regValue);
int EMAC_Phy_Write(EMAC_ID_Type emacId, uint16_t phyReg, uint16_t regValue);
int EMAC_Enable_TX(EMAC_ID_Type emacId);
int EMAC_Disable_TX(EMAC_ID_Type emacId);
int EMAC_Enable_RX(EMAC_ID_Type emacId);
int EMAC_Disable_RX(EMAC_ID_Type emacId);
int EMAC_GetFramLen(EMAC_ID_Type emacId, uint16_t * max, uint16_t *min);
int EMAC_GetBD(EMAC_ID_Type emacId, uint32_t *bd);
void EMAC_Phy_SetAddress(EMAC_ID_Type emacId,uint16_t phyAddress);
void EMAC_Phy_Set_Full_Duplex(EMAC_ID_Type emacId,uint8_t fullDuplex);

#endif /* __DRV_EMAC_H__ */

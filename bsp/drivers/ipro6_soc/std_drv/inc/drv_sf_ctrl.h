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


#ifndef __DRV_SF_CTRL_H__
#define __DRV_SF_CTRL_H__

#include "sf_ctrl_reg.h"
#include "drv_common.h"

/*============================================================================
 * Compatibility aliases for SF_CTRL register naming changes
 * (Old driver uses short names, new register file uses SF_CTRL_SF_* prefix)
 *============================================================================*/

/* Interface SAHB base offsets */
#define SF_CTRL_IF1_SAHB_OFFSET             SF_CTRL_SF_IF_SAHB_0_OFFSET   /* IF1 base: 0x08 */
#define SF_CTRL_IF2_SAHB_OFFSET             SF_CTRL_SF_IF2_SAHB_0_OFFSET  /* IF2 base: 0xC8 */

/* Interface SAHB relative offsets */
#define SF_CTRL_IF_SAHB_0_OFFSET            (0x00)
#define SF_CTRL_IF_SAHB_1_OFFSET            (0x04)
#define SF_CTRL_IF_SAHB_2_OFFSET            (0x08)

/* Interface busy status */
#define SF_CTRL_IF_BUSY                     SF_CTRL_SF_IF_BUSY
#define SF_CTRL_IF_BUSY_POS                 SF_CTRL_SF_IF_BUSY_POS
#define SF_CTRL_IF_BUSY_LEN                 SF_CTRL_SF_IF_BUSY_LEN
#define SF_CTRL_IF_BUSY_MSK                 SF_CTRL_SF_IF_BUSY_MSK
#define SF_CTRL_IF_BUSY_UMSK                SF_CTRL_SF_IF_BUSY_UMSK

/* AES region base offset (region 0 starts at 0x200, each region is 0x80 bytes) */
#define SF_CTRL_AES_REGION_OFFSET           (0x200)

/* AES IV offsets within a region (relative to region base) */
#define SF_CTRL_SF_AES_IV_W0_OFFSET         (0x20)  /* IV word 0 */
#define SF_CTRL_SF_AES_IV_W1_OFFSET         (0x24)  /* IV word 1 */
#define SF_CTRL_SF_AES_IV_W2_OFFSET         (0x28)  /* IV word 2 */
#define SF_CTRL_SF_AES_IV_W3_OFFSET         (0x2C)  /* IV word 3 */

/* AES KEY offsets within a region (relative to region base) */
#define SF_CTRL_SF_AES_KEY_0_OFFSET         (0x00)
#define SF_CTRL_SF_AES_KEY_1_OFFSET         (0x04)
#define SF_CTRL_SF_AES_KEY_2_OFFSET         (0x08)
#define SF_CTRL_SF_AES_KEY_3_OFFSET         (0x0C)
#define SF_CTRL_SF_AES_KEY_4_OFFSET         (0x10)
#define SF_CTRL_SF_AES_KEY_5_OFFSET         (0x14)
#define SF_CTRL_SF_AES_KEY_6_OFFSET         (0x18)
#define SF_CTRL_SF_AES_KEY_7_OFFSET         (0x1C)

/* AES region field aliases (all regions have same bit positions) */
#define SF_CTRL_SF_AES_REGION_EN            SF_CTRL_SF_AES_R0_EN
#define SF_CTRL_SF_AES_REGION_EN_POS        SF_CTRL_SF_AES_R0_EN_POS
#define SF_CTRL_SF_AES_REGION_EN_MSK        SF_CTRL_SF_AES_R0_EN_MSK
#define SF_CTRL_SF_AES_REGION_EN_UMSK       SF_CTRL_SF_AES_R0_EN_UMSK

#define SF_CTRL_SF_AES_REGION_LOCK          SF_CTRL_SF_AES_R0_LOCK
#define SF_CTRL_SF_AES_REGION_LOCK_POS      SF_CTRL_SF_AES_R0_LOCK_POS
#define SF_CTRL_SF_AES_REGION_LOCK_MSK      SF_CTRL_SF_AES_R0_LOCK_MSK
#define SF_CTRL_SF_AES_REGION_LOCK_UMSK     SF_CTRL_SF_AES_R0_LOCK_UMSK

#define SF_CTRL_SF_AES_REGION_START         SF_CTRL_SF_AES_R0_START
#define SF_CTRL_SF_AES_REGION_START_POS     SF_CTRL_SF_AES_R0_START_POS
#define SF_CTRL_SF_AES_REGION_START_MSK     SF_CTRL_SF_AES_R0_START_MSK
#define SF_CTRL_SF_AES_REGION_START_UMSK    SF_CTRL_SF_AES_R0_START_UMSK

#define SF_CTRL_SF_AES_REGION_END           SF_CTRL_SF_AES_R0_END
#define SF_CTRL_SF_AES_REGION_END_POS       SF_CTRL_SF_AES_R0_END_POS
#define SF_CTRL_SF_AES_REGION_END_MSK       SF_CTRL_SF_AES_R0_END_MSK
#define SF_CTRL_SF_AES_REGION_END_UMSK      SF_CTRL_SF_AES_R0_END_UMSK

#define SF_CTRL_SF_AES_REGION_HW_KEY_EN         SF_CTRL_SF_AES_R0_HW_KEY_EN
#define SF_CTRL_SF_AES_REGION_HW_KEY_EN_POS     SF_CTRL_SF_AES_R0_HW_KEY_EN_POS
#define SF_CTRL_SF_AES_REGION_HW_KEY_EN_MSK     SF_CTRL_SF_AES_R0_HW_KEY_EN_MSK
#define SF_CTRL_SF_AES_REGION_HW_KEY_EN_UMSK    SF_CTRL_SF_AES_R0_HW_KEY_EN_UMSK

/* AES START/END offsets within a region (relative to region base) */
#define SF_CTRL_SF_AES_START_OFFSET         (0x30)  /* Start address register */
#define SF_CTRL_SF_AES_END_OFFSET           (0x34)  /* End address register */

/* IO delay select aliases (IO 0-3) */
#define SF_CTRL_IO_0_DO_DLY_SEL             SF_CTRL_SF_IO_0_DO_DLY_SEL
#define SF_CTRL_IO_0_DO_DLY_SEL_POS         SF_CTRL_SF_IO_0_DO_DLY_SEL_POS
#define SF_CTRL_IO_0_DO_DLY_SEL_MSK         SF_CTRL_SF_IO_0_DO_DLY_SEL_MSK
#define SF_CTRL_IO_0_DO_DLY_SEL_UMSK        SF_CTRL_SF_IO_0_DO_DLY_SEL_UMSK
#define SF_CTRL_IO_0_DI_DLY_SEL             SF_CTRL_SF_IO_0_DI_DLY_SEL
#define SF_CTRL_IO_0_DI_DLY_SEL_POS         SF_CTRL_SF_IO_0_DI_DLY_SEL_POS
#define SF_CTRL_IO_0_DI_DLY_SEL_MSK         SF_CTRL_SF_IO_0_DI_DLY_SEL_MSK
#define SF_CTRL_IO_0_DI_DLY_SEL_UMSK        SF_CTRL_SF_IO_0_DI_DLY_SEL_UMSK
#define SF_CTRL_IO_0_OE_DLY_SEL             SF_CTRL_SF_IO_0_OE_DLY_SEL
#define SF_CTRL_IO_0_OE_DLY_SEL_POS         SF_CTRL_SF_IO_0_OE_DLY_SEL_POS
#define SF_CTRL_IO_0_OE_DLY_SEL_MSK         SF_CTRL_SF_IO_0_OE_DLY_SEL_MSK
#define SF_CTRL_IO_0_OE_DLY_SEL_UMSK        SF_CTRL_SF_IO_0_OE_DLY_SEL_UMSK

#define SF_CTRL_IO_1_DO_DLY_SEL             SF_CTRL_SF_IO_1_DO_DLY_SEL
#define SF_CTRL_IO_1_DO_DLY_SEL_POS         SF_CTRL_SF_IO_1_DO_DLY_SEL_POS
#define SF_CTRL_IO_1_DO_DLY_SEL_MSK         SF_CTRL_SF_IO_1_DO_DLY_SEL_MSK
#define SF_CTRL_IO_1_DO_DLY_SEL_UMSK        SF_CTRL_SF_IO_1_DO_DLY_SEL_UMSK
#define SF_CTRL_IO_1_DI_DLY_SEL             SF_CTRL_SF_IO_1_DI_DLY_SEL
#define SF_CTRL_IO_1_DI_DLY_SEL_POS         SF_CTRL_SF_IO_1_DI_DLY_SEL_POS
#define SF_CTRL_IO_1_DI_DLY_SEL_MSK         SF_CTRL_SF_IO_1_DI_DLY_SEL_MSK
#define SF_CTRL_IO_1_DI_DLY_SEL_UMSK        SF_CTRL_SF_IO_1_DI_DLY_SEL_UMSK
#define SF_CTRL_IO_1_OE_DLY_SEL             SF_CTRL_SF_IO_1_OE_DLY_SEL
#define SF_CTRL_IO_1_OE_DLY_SEL_POS         SF_CTRL_SF_IO_1_OE_DLY_SEL_POS
#define SF_CTRL_IO_1_OE_DLY_SEL_MSK         SF_CTRL_SF_IO_1_OE_DLY_SEL_MSK
#define SF_CTRL_IO_1_OE_DLY_SEL_UMSK        SF_CTRL_SF_IO_1_OE_DLY_SEL_UMSK

#define SF_CTRL_IO_2_DO_DLY_SEL             SF_CTRL_SF_IO_2_DO_DLY_SEL
#define SF_CTRL_IO_2_DO_DLY_SEL_POS         SF_CTRL_SF_IO_2_DO_DLY_SEL_POS
#define SF_CTRL_IO_2_DO_DLY_SEL_MSK         SF_CTRL_SF_IO_2_DO_DLY_SEL_MSK
#define SF_CTRL_IO_2_DO_DLY_SEL_UMSK        SF_CTRL_SF_IO_2_DO_DLY_SEL_UMSK
#define SF_CTRL_IO_2_DI_DLY_SEL             SF_CTRL_SF_IO_2_DI_DLY_SEL
#define SF_CTRL_IO_2_DI_DLY_SEL_POS         SF_CTRL_SF_IO_2_DI_DLY_SEL_POS
#define SF_CTRL_IO_2_DI_DLY_SEL_MSK         SF_CTRL_SF_IO_2_DI_DLY_SEL_MSK
#define SF_CTRL_IO_2_DI_DLY_SEL_UMSK        SF_CTRL_SF_IO_2_DI_DLY_SEL_UMSK
#define SF_CTRL_IO_2_OE_DLY_SEL             SF_CTRL_SF_IO_2_OE_DLY_SEL
#define SF_CTRL_IO_2_OE_DLY_SEL_POS         SF_CTRL_SF_IO_2_OE_DLY_SEL_POS
#define SF_CTRL_IO_2_OE_DLY_SEL_MSK         SF_CTRL_SF_IO_2_OE_DLY_SEL_MSK
#define SF_CTRL_IO_2_OE_DLY_SEL_UMSK        SF_CTRL_SF_IO_2_OE_DLY_SEL_UMSK

#define SF_CTRL_IO_3_DO_DLY_SEL             SF_CTRL_SF_IO_3_DO_DLY_SEL
#define SF_CTRL_IO_3_DO_DLY_SEL_POS         SF_CTRL_SF_IO_3_DO_DLY_SEL_POS
#define SF_CTRL_IO_3_DO_DLY_SEL_MSK         SF_CTRL_SF_IO_3_DO_DLY_SEL_MSK
#define SF_CTRL_IO_3_DO_DLY_SEL_UMSK        SF_CTRL_SF_IO_3_DO_DLY_SEL_UMSK
#define SF_CTRL_IO_3_DI_DLY_SEL             SF_CTRL_SF_IO_3_DI_DLY_SEL
#define SF_CTRL_IO_3_DI_DLY_SEL_POS         SF_CTRL_SF_IO_3_DI_DLY_SEL_POS
#define SF_CTRL_IO_3_DI_DLY_SEL_MSK         SF_CTRL_SF_IO_3_DI_DLY_SEL_MSK
#define SF_CTRL_IO_3_DI_DLY_SEL_UMSK        SF_CTRL_SF_IO_3_DI_DLY_SEL_UMSK
#define SF_CTRL_IO_3_OE_DLY_SEL             SF_CTRL_SF_IO_3_OE_DLY_SEL
#define SF_CTRL_IO_3_OE_DLY_SEL_POS         SF_CTRL_SF_IO_3_OE_DLY_SEL_POS
#define SF_CTRL_IO_3_OE_DLY_SEL_MSK         SF_CTRL_SF_IO_3_OE_DLY_SEL_MSK
#define SF_CTRL_IO_3_OE_DLY_SEL_UMSK        SF_CTRL_SF_IO_3_OE_DLY_SEL_UMSK

/* IO delay register offsets - interface base addresses */
#define SF_CTRL_IF_IO_DLY_1_OFFSET          SF_CTRL_SF_IF_IO_DLY_0_OFFSET   /* SF1 base: 0x30 */
#define SF_CTRL_IF_IO_DLY_2_OFFSET          SF_CTRL_SF2_IF_IO_DLY_0_OFFSET  /* SF2 base: 0x48 */
#define SF_CTRL_IF_IO_DLY_3_OFFSET          SF_CTRL_SF3_IF_IO_DLY_0_OFFSET  /* SF3 base: 0x5C */

/* IO delay register offsets - relative to interface base */
#define SF_CTRL_IO_DLY_1_OFFSET             (0x00)  /* IO 0 delay */
#define SF_CTRL_IO_DLY_2_OFFSET             (0x04)  /* IO 1 delay */
#define SF_CTRL_IO_DLY_3_OFFSET             (0x08)  /* IO 2 delay */
#define SF_CTRL_IO_DLY_4_OFFSET             (0x0C)  /* IO 3 delay */

/* Interface 0 command/address/data field aliases */
#define SF_CTRL_IF_0_ADR_BYTE               SF_CTRL_SF_IF_0_ADR_BYTE
#define SF_CTRL_IF_0_ADR_BYTE_POS           SF_CTRL_SF_IF_0_ADR_BYTE_POS
#define SF_CTRL_IF_0_ADR_BYTE_MSK           SF_CTRL_SF_IF_0_ADR_BYTE_MSK
#define SF_CTRL_IF_0_ADR_BYTE_UMSK          SF_CTRL_SF_IF_0_ADR_BYTE_UMSK

#define SF_CTRL_IF_0_ADR_EN                 SF_CTRL_SF_IF_0_ADR_EN
#define SF_CTRL_IF_0_ADR_EN_POS             SF_CTRL_SF_IF_0_ADR_EN_POS
#define SF_CTRL_IF_0_ADR_EN_MSK             SF_CTRL_SF_IF_0_ADR_EN_MSK
#define SF_CTRL_IF_0_ADR_EN_UMSK            SF_CTRL_SF_IF_0_ADR_EN_UMSK

#define SF_CTRL_IF_0_CMD_BYTE               SF_CTRL_SF_IF_0_CMD_BYTE
#define SF_CTRL_IF_0_CMD_BYTE_POS           SF_CTRL_SF_IF_0_CMD_BYTE_POS
#define SF_CTRL_IF_0_CMD_BYTE_MSK           SF_CTRL_SF_IF_0_CMD_BYTE_MSK
#define SF_CTRL_IF_0_CMD_BYTE_UMSK          SF_CTRL_SF_IF_0_CMD_BYTE_UMSK

#define SF_CTRL_IF_0_CMD_EN                 SF_CTRL_SF_IF_0_CMD_EN
#define SF_CTRL_IF_0_CMD_EN_POS             SF_CTRL_SF_IF_0_CMD_EN_POS
#define SF_CTRL_IF_0_CMD_EN_MSK             SF_CTRL_SF_IF_0_CMD_EN_MSK
#define SF_CTRL_IF_0_CMD_EN_UMSK            SF_CTRL_SF_IF_0_CMD_EN_UMSK

#define SF_CTRL_IF_0_DAT_BYTE               SF_CTRL_SF_IF_0_DAT_BYTE
#define SF_CTRL_IF_0_DAT_BYTE_POS           SF_CTRL_SF_IF_0_DAT_BYTE_POS
#define SF_CTRL_IF_0_DAT_BYTE_MSK           SF_CTRL_SF_IF_0_DAT_BYTE_MSK
#define SF_CTRL_IF_0_DAT_BYTE_UMSK          SF_CTRL_SF_IF_0_DAT_BYTE_UMSK

#define SF_CTRL_IF_0_DAT_EN                 SF_CTRL_SF_IF_0_DAT_EN
#define SF_CTRL_IF_0_DAT_EN_POS             SF_CTRL_SF_IF_0_DAT_EN_POS
#define SF_CTRL_IF_0_DAT_EN_MSK             SF_CTRL_SF_IF_0_DAT_EN_MSK
#define SF_CTRL_IF_0_DAT_EN_UMSK            SF_CTRL_SF_IF_0_DAT_EN_UMSK

#define SF_CTRL_IF_0_DAT_RW                 SF_CTRL_SF_IF_0_DAT_RW
#define SF_CTRL_IF_0_DAT_RW_POS             SF_CTRL_SF_IF_0_DAT_RW_POS
#define SF_CTRL_IF_0_DAT_RW_MSK             SF_CTRL_SF_IF_0_DAT_RW_MSK
#define SF_CTRL_IF_0_DAT_RW_UMSK            SF_CTRL_SF_IF_0_DAT_RW_UMSK

#define SF_CTRL_IF_0_DMY_BYTE               SF_CTRL_SF_IF_0_DMY_BYTE
#define SF_CTRL_IF_0_DMY_BYTE_POS           SF_CTRL_SF_IF_0_DMY_BYTE_POS
#define SF_CTRL_IF_0_DMY_BYTE_MSK           SF_CTRL_SF_IF_0_DMY_BYTE_MSK
#define SF_CTRL_IF_0_DMY_BYTE_UMSK          SF_CTRL_SF_IF_0_DMY_BYTE_UMSK

#define SF_CTRL_IF_0_DMY_EN                 SF_CTRL_SF_IF_0_DMY_EN
#define SF_CTRL_IF_0_DMY_EN_POS             SF_CTRL_SF_IF_0_DMY_EN_POS
#define SF_CTRL_IF_0_DMY_EN_MSK             SF_CTRL_SF_IF_0_DMY_EN_MSK
#define SF_CTRL_IF_0_DMY_EN_UMSK            SF_CTRL_SF_IF_0_DMY_EN_UMSK

#define SF_CTRL_IF_0_QPI_MODE_EN            SF_CTRL_SF_IF_0_QPI_MODE_EN
#define SF_CTRL_IF_0_QPI_MODE_EN_POS        SF_CTRL_SF_IF_0_QPI_MODE_EN_POS
#define SF_CTRL_IF_0_QPI_MODE_EN_MSK        SF_CTRL_SF_IF_0_QPI_MODE_EN_MSK
#define SF_CTRL_IF_0_QPI_MODE_EN_UMSK       SF_CTRL_SF_IF_0_QPI_MODE_EN_UMSK

#define SF_CTRL_IF_0_SPI_MODE               SF_CTRL_SF_IF_0_SPI_MODE
#define SF_CTRL_IF_0_SPI_MODE_POS           SF_CTRL_SF_IF_0_SPI_MODE_POS
#define SF_CTRL_IF_0_SPI_MODE_MSK           SF_CTRL_SF_IF_0_SPI_MODE_MSK
#define SF_CTRL_IF_0_SPI_MODE_UMSK          SF_CTRL_SF_IF_0_SPI_MODE_UMSK

#define SF_CTRL_IF_0_TRIG                   SF_CTRL_SF_IF_0_TRIG
#define SF_CTRL_IF_0_TRIG_POS               SF_CTRL_SF_IF_0_TRIG_POS
#define SF_CTRL_IF_0_TRIG_MSK               SF_CTRL_SF_IF_0_TRIG_MSK
#define SF_CTRL_IF_0_TRIG_UMSK              SF_CTRL_SF_IF_0_TRIG_UMSK

#define SF_CTRL_PAD1    (0)
#define SF_CTRL_PAD2    (1)
#define SF_CTRL_PAD3    (2)

#define SF_IO_EMB_SWAP_IO3IO0                                   (0x0)
#define SF_IO_EMB_SWAP_IO3IO0_IO2CS                             (0x1)
#define SF_IO_EMB_SWAP_NONE                                     (0x2)
#define SF_IO_EMB_SWAP_IO2CS                                    (0x3)
#define SF_IO_EXT_SF2_SWAP_IO3IO0                               (0x4)
#define SF_IO_EXT_SF3                                           (0x8)
#define SF_IO_EMB_SWAP_IO3IO0_AND_SF2_SWAP_IO3IO0               (0x14)
#define SF_IO_EMB_SWAP_IO3IO0_IO2CS_AND_SF2_SWAP_IO3IO0         (0x15)
#define SF_IO_EMB_SWAP_NONE_AND_SF2_SWAP_IO3IO0                 (0x16)
#define SF_IO_EMB_SWAP_IO2CS_AND_SF2_SWAP_IO3IO0                (0x17)
#define SF_IO_EXT_SF2                                           (0x24)
#define SF_IO_EMB_SWAP_IO3IO0_AND_SF2                           (0x34)
#define SF_IO_EMB_SWAP_IO3IO0_IO2CS_AND_SF2                     (0x35)
#define SF_IO_EMB_SWAP_NONE_AND_SF2                             (0x36)
#define SF_IO_EMB_SWAP_IO2CS_AND_SF2                            (0x37)

#define SF_CTRL_FLASH_BANK0     (0)
#define SF_CTRL_FLASH_BANK1     (1)

#define SF_CTRL_WRAP_MODE_0     (0)
#define SF_CTRL_WRAP_MODE_1     (1)
#define SF_CTRL_WRAP_MODE_2     (2)
#define SF_CTRL_WRAP_MODE_3     (3)

#define SF_CTRL_WRAP_LEN_8      (0)
#define SF_CTRL_WRAP_LEN_16     (1)
#define SF_CTRL_WRAP_LEN_32     (2)
#define SF_CTRL_WRAP_LEN_64     (3)
#define SF_CTRL_WRAP_LEN_128    (4)
#define SF_CTRL_WRAP_LEN_256    (5)
#define SF_CTRL_WRAP_LEN_512    (6)
#define SF_CTRL_WRAP_LEN_1024   (7)
#define SF_CTRL_WRAP_LEN_2048   (8)
#define SF_CTRL_WRAP_LEN_4096   (9)

#define SF_CTRL_ORIGINAL_MEMORY_MAP     (0)
#define SF_CTRL_REMAP_16MB              (1)
#define SF_CTRL_REMAP_8MB               (2)
#define SF_CTRL_REMAP_4MB               (3)

#define SF_CTRL_OWNER_SAHB      (0)
#define SF_CTRL_OWNER_IAHB      (1)

#define SF_CTRL_READ    (0)
#define SF_CTRL_WRITE   (1)

#define SF_CTRL_NIO_MODE    (0)
#define SF_CTRL_DO_MODE     (1)
#define SF_CTRL_QO_MODE     (2)
#define SF_CTRL_DIO_MODE    (3)
#define SF_CTRL_QIO_MODE    (4)

#define SF_CTRL_SPI_MODE    (0)
#define SF_CTRL_QPI_MODE    (1)

#define SF_CTRL_CMD_1_LINE      (0)
#define SF_CTRL_CMD_4_LINES     (1)

#define SF_CTRL_ADDR_1_LINE     (0)
#define SF_CTRL_ADDR_2_LINES    (1)
#define SF_CTRL_ADDR_4_LINES    (2)

#define SF_CTRL_DUMMY_1_LINE    (0)
#define SF_CTRL_DUMMY_2_LINES   (1)
#define SF_CTRL_DUMMY_4_LINES   (2)

#define SF_CTRL_DATA_1_LINE     (0)
#define SF_CTRL_DATA_2_LINES    (1)
#define SF_CTRL_DATA_4_LINES    (2)

#define SF_CTRL_AES_CTR_MODE    (0)
#define SF_CTRL_AES_XTS_MODE    (1)

#define SF_CTRL_AES_128BITS                 (0)
#define SF_CTRL_AES_256BITS                 (1)
#define SF_CTRL_AES_192BITS                 (2)
#define SF_CTRL_AES_128BITS_DOUBLE_KEY      (3)

typedef struct
{
    uint8_t owner; /*!< Sflash interface bus owner */
    uint8_t en32bAddr;    /*!< Sflash enable 32-bits address */
    uint8_t clkDelay;         /*!< Clock count for read due to pad delay */
    uint8_t clkInvert;        /*!< Clock invert */
    uint8_t rxClkInvert;      /*!< RX clock invert */
    uint8_t doDelay;          /*!< Data out delay */
    uint8_t diDelay;          /*!< Data in delay */
    uint8_t oeDelay;          /*!< Output enable delay */
} SF_Ctrl_Cfg_Type;

typedef struct
{
    uint8_t sbus2Select;         /*!< Select sbus2 as 2nd flash controller */
    uint8_t bank2RxClkInvertSrc; /*!< Select bank2 rx clock invert source */
    uint8_t bank2RxClkInvertSel; /*!< Select inveted bank2 rx clock */
    uint8_t bank2DelaySrc;       /*!< Select bank2 read delay source */
    uint8_t bank2ClkDelay;           /*!< Bank2 read delay cycle = n + 1 */
    uint8_t doDelay;                 /*!< Data out delay */
    uint8_t diDelay;                 /*!< Data in delay */
    uint8_t oeDelay;                 /*!< Output enable delay */
    uint8_t remap;        /*!< Select dual flash memory remap set */
    uint8_t remapLock;               /*!< Select memory remap lock */
} SF_Ctrl_Bank2_Cfg;

typedef struct
{
    uint8_t ackLatency;                /*!< SF Ctrl ack latency cycles */
    uint8_t cmdsCoreEn;            /*!< SF Ctrl cmds core enable */
    uint8_t cmdsEn;                /*!< SF Ctrl cmds enable */
    uint8_t cmdsWrapMode;    /*!< SF Ctrl cmds wrap mode */
    uint8_t cmdsWrapLen; /*!< SF Ctrl cmds wrap length */
} SF_Ctrl_Cmds_Cfg;

typedef struct
{
    uint8_t rwFlag;                  /*!< Read write flag */
    uint8_t cmdMode;   /*!< Command mode */
    uint8_t addrMode; /*!< Address mode */
    uint8_t addrSize;                /*!< Address size */
    uint8_t dummyClks;               /*!< Dummy clocks */
    uint8_t dummyMode; /*!< Dummy mode */
    uint8_t dataMode; /*!< Data mode */
    uint8_t rsv[1];                  /*!<  */
    uint32_t nbData;                 /*!< Transfer number of bytes */
    uint32_t cmdBuf[2];              /*!< Command buffer */
} SF_Ctrl_Cmd_Cfg_Type;

typedef struct
{
    uint8_t mode; /*!< Serail flash AES CTR/XTS mode */
    uint8_t type;  /*!< Serail flash AES key bit length */
    uint8_t aes_region;         /*!< Serail flash AES region 0/1/2 */
    uint32_t addr;              /*!< Serail flash AES decrypt start address */
    uint32_t len;               /*!< Serail flash AES decrypt length */
    uint8_t *iv;                /*!< Serail flash AES iv */
} SF_Ctrl_Decrypt_Type;

#define IS_SF_CTRL_PAD_TYPE(type) (((type) == SF_CTRL_PAD1) || \
                                   ((type) == SF_CTRL_PAD2) || \
                                   ((type) == SF_CTRL_PAD3))

#define IS_SF_CTRL_PIN_SELECT(type) (((type) == SF_IO_EMB_SWAP_IO3IO0) ||                          \
                                    ((type) == SF_IO_EMB_SWAP_IO3IO0_IO2CS) ||                     \
                                    ((type) == SF_IO_EMB_SWAP_NONE) ||                             \
                                    ((type) == SF_IO_EMB_SWAP_IO2CS) ||                            \
                                    ((type) == SF_IO_EXT_SF2_SWAP_IO3IO0) ||                       \
                                    ((type) == SF_IO_EXT_SF3) ||                                   \
                                    ((type) == SF_IO_EMB_SWAP_IO3IO0_AND_SF2_SWAP_IO3IO0) ||       \
                                    ((type) == SF_IO_EMB_SWAP_IO3IO0_IO2CS_AND_SF2_SWAP_IO3IO0) || \
                                    ((type) == SF_IO_EMB_SWAP_NONE_AND_SF2_SWAP_IO3IO0) ||         \
                                    ((type) == SF_IO_EMB_SWAP_IO2CS_AND_SF2_SWAP_IO3IO0) ||        \
                                    ((type) == SF_IO_EXT_SF2) ||                                   \
                                    ((type) == SF_IO_EMB_SWAP_IO3IO0_AND_SF2) ||                   \
                                    ((type) == SF_IO_EMB_SWAP_IO3IO0_IO2CS_AND_SF2) ||             \
                                    ((type) == SF_IO_EMB_SWAP_NONE_AND_SF2) ||                     \
                                    ((type) == SF_IO_EMB_SWAP_IO2CS_AND_SF2))

#define IS_SF_CTRL_BANK_SELECT(type) (((type) == SF_CTRL_FLASH_BANK0) || \
                                      ((type) == SF_CTRL_FLASH_BANK1))

#define IS_SF_CTRL_WRAP_MODE(type) (((type) == SF_CTRL_WRAP_MODE_0) || \
                                    ((type) == SF_CTRL_WRAP_MODE_1) || \
                                    ((type) == SF_CTRL_WRAP_MODE_2) || \
                                    ((type) == SF_CTRL_WRAP_MODE_3))

#define IS_SF_CTRL_WRAP_LEN_TYPE(type) (((type) == SF_CTRL_WRAP_LEN_8) ||    \
                                        ((type) == SF_CTRL_WRAP_LEN_16) ||   \
                                        ((type) == SF_CTRL_WRAP_LEN_32) ||   \
                                        ((type) == SF_CTRL_WRAP_LEN_64) ||   \
                                        ((type) == SF_CTRL_WRAP_LEN_128) ||  \
                                        ((type) == SF_CTRL_WRAP_LEN_256) ||  \
                                        ((type) == SF_CTRL_WRAP_LEN_512) ||  \
                                        ((type) == SF_CTRL_WRAP_LEN_1024) || \
                                        ((type) == SF_CTRL_WRAP_LEN_2048) || \
                                        ((type) == SF_CTRL_WRAP_LEN_4096))

#define IS_SF_CTRL_REMAP_TYPE(type) (((type) == SF_CTRL_ORIGINAL_MEMORY_MAP) || \
                                     ((type) == SF_CTRL_REMAP_16MB) ||          \
                                     ((type) == SF_CTRL_REMAP_8MB) ||           \
                                     ((type) == SF_CTRL_REMAP_4MB))

#define IS_SF_CTRL_OWNER_TYPE(type) (((type) == SF_CTRL_OWNER_SAHB) || \
                                     ((type) == SF_CTRL_OWNER_IAHB))

#define IS_SF_CTRL_RW_TYPE(type) (((type) == SF_CTRL_READ) || \
                                  ((type) == SF_CTRL_WRITE))

#define IS_SF_CTRL_IO_TYPE(type) (((type) == SF_CTRL_NIO_MODE) || \
                                  ((type) == SF_CTRL_DO_MODE) ||  \
                                  ((type) == SF_CTRL_QO_MODE) ||  \
                                  ((type) == SF_CTRL_DIO_MODE) || \
                                  ((type) == SF_CTRL_QIO_MODE))

#define IS_SF_CTRL_MODE_TYPE(type) (((type) == SF_CTRL_SPI_MODE) || \
                                    ((type) == SF_CTRL_QPI_MODE))

#define IS_SF_CTRL_CMD_MODE_TYPE(type) (((type) == SF_CTRL_CMD_1_LINE) || \
                                        ((type) == SF_CTRL_CMD_4_LINES))

#define IS_SF_CTRL_ADDR_MODE_TYPE(type) (((type) == SF_CTRL_ADDR_1_LINE) ||  \
                                         ((type) == SF_CTRL_ADDR_2_LINES) || \
                                         ((type) == SF_CTRL_ADDR_4_LINES))

#define IS_SF_CTRL_DMY_MODE_TYPE(type) (((type) == SF_CTRL_DUMMY_1_LINE) ||  \
                                        ((type) == SF_CTRL_DUMMY_2_LINES) || \
                                        ((type) == SF_CTRL_DUMMY_4_LINES))

#define IS_SF_CTRL_DATA_MODE_TYPE(type) (((type) == SF_CTRL_DATA_1_LINE) ||  \
                                         ((type) == SF_CTRL_DATA_2_LINES) || \
                                         ((type) == SF_CTRL_DATA_4_LINES))

#define IS_SF_CTRL_AES_MODE_TYPE(type) (((type) == SF_CTRL_AES_CTR_MODE) || \
                                        ((type) == SF_CTRL_AES_XTS_MODE))

#define IS_SF_CTRL_AES_KEY_TYPE(type) (((type) == SF_CTRL_AES_128BITS) || \
                                       ((type) == SF_CTRL_AES_256BITS) || \
                                       ((type) == SF_CTRL_AES_192BITS) || \
                                       ((type) == SF_CTRL_AES_128BITS_DOUBLE_KEY))

#define SF_CTRL_NO_ADDRESS       0xFFFFFFFF
#define NOR_FLASH_CTRL_BUF_SIZE  256
#define NAND_FLASH_CTRL_BUF_SIZE 512

#ifndef IPRO_USE_HAL_DRIVER
void SF_Ctrl_IRQHandler(void);
#endif
void SF_Ctrl_Enable(const SF_Ctrl_Cfg_Type *cfg);
void SF_Ctrl_Bank2_Enable(const SF_Ctrl_Bank2_Cfg *bank2Cfg);
void SF_Ctrl_Set_IO_Delay(uint8_t pad, uint8_t doDelay, uint8_t diDelay, uint8_t oeDelay);
void SF_Ctrl_Sbus2_Hold_Sram(void);
void SF_Ctrl_Sbus2_Release_Sram(void);
int SF_Ctrl_Is_Sbus2_Enable(void);
void SF_Ctrl_Sbus2_Replace(uint8_t pad);
void SF_Ctrl_Sbus2_Revoke_replace(void);
void SF_Ctrl_Sbus2_Set_Delay(uint8_t clkDelay, uint8_t rxClkInvert);
void SF_Ctrl_32bits_Addr_En(int en32BitsAddr);
uint8_t SF_Ctrl_Get_Clock_Delay(void);
void SF_Ctrl_Set_Clock_Delay(uint8_t delay);
uint8_t SF_Ctrl_Get_Wrap_Queue_Value(void);
void SF_Ctrl_Cmds_Set(SF_Ctrl_Cmds_Cfg *cmdsCfg, uint8_t bank);
void SF_Ctrl_Remap_Set(uint8_t remap, uint8_t lock);
void SF_Ctrl_Select_Pad(uint8_t sel);
void SF_Ctrl_Sbus_Select_Bank(uint8_t bank);
void SF_Ctrl_Set_Owner(uint8_t owner);
void SF_Ctrl_Disable(void);
void SF_Ctrl_AES_Enable_BE(void);
void SF_Ctrl_AES_Enable_LE(void);
void SF_Ctrl_AES_Set_Region(uint8_t region, uint8_t enable, uint8_t hwKey, uint32_t startAddr, uint32_t endAddr,
                            uint8_t locked);
void SF_Ctrl_AES_Set_Key(uint8_t region, uint8_t *key, uint8_t keyType);
void SF_Ctrl_AES_Set_Key_BE(uint8_t region, uint8_t *key, uint8_t keyType);
void SF_Ctrl_AES_XTS_Set_Key(uint8_t region, uint8_t *key, uint8_t keyType);
void SF_Ctrl_AES_XTS_Set_Key_BE(uint8_t region, uint8_t *key, uint8_t keyType);
void SF_Ctrl_AES_Set_IV(uint8_t region, uint8_t *iv, uint32_t addrOffset);
void SF_Ctrl_AES_XTS_Set_IV(uint8_t region, uint8_t *iv, uint32_t addrOffset);
void SF_Ctrl_AES_Set_IV_BE(uint8_t region, uint8_t *iv, uint32_t addrOffset);
void SF_Ctrl_AES_XTS_Set_IV_BE(uint8_t region, uint8_t *iv, uint32_t addrOffset);
void SF_Ctrl_AES_Set_Mode(uint8_t mode);
void SF_Ctrl_AES_Enable(void);
void SF_Ctrl_AES_Disable(void);
int SF_Ctrl_Is_AES_Enable(void);
void SF_Ctrl_Set_Flash_Image_Offset(uint32_t addrOffset, uint8_t group, uint8_t bank);
uint32_t SF_Ctrl_Get_Flash_Image_Offset(uint8_t group, uint8_t bank);
void SF_Ctrl_Lock_Flash_Image_Offset(uint8_t lock);
void SF_Ctrl_SendCmd(SF_Ctrl_Cmd_Cfg_Type *cfg);
void SF_Ctrl_Disable_Wrap_Access(uint8_t disable);
void SF_Ctrl_Flash_Read_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid);
void SF_Ctrl_Flash2_Read_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid);
int SF_Ctrl_GetBusyState(void);
void SF_Ctrl_AES_Get_IV_BE(uint8_t region, uint8_t *iv);
void SF_Ctrl_AES_Get_IV_LE(uint8_t region, uint8_t *iv);
int32_t SF_Ctrl_AES_Set_Decrypt_Region_BE(SF_Ctrl_Decrypt_Type *parm);
int32_t SF_Ctrl_AES_Set_Decrypt_Region_LE(SF_Ctrl_Decrypt_Type *parm);
void SF_Ctrl_SetDelay(uint8_t clkDelay);

#endif /* __DRV_SF_CTRL_H__ */

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

#ifndef __IPRO6_SDIO3_REG_H__
#define __IPRO6_SDIO3_REG_H__

/*
 * SMID (SDIO 3.0 / SD3.0 / eMMC device controller) register map for IPRO6.
 *
 * IP block : SMID, per the SMID user guide rev 3.58.
 * Base : SDU_BASE / SMID_BASE = 0x30084000 (offsets below are byte offsets).
 *
 * Cross-checked against:
 *   - the SMID datasheet Section 9.1 (CSR address map) + Section 9.2 (register
 *     field tables) + Section 7.18 (ADMA2 descriptor format).
 *   - the SMID device driver source (getreg32/putreg32/mask/shift usage).
 *
 * Only register-map symbols (offsets + hardware bitfields) live here.
 * Software-only tokens (commands, IRQ events, transfer states, capability
 * flags, custom-RAM-window layout, queue sizing, debug) are defined in the
 * driver headers, NOT here.
 */

/* ------------------------------------------------------------------------- */
/* Register offsets (Section 9.1, Table 45 "Configuration Register Types")   */
/* ------------------------------------------------------------------------- */

/* 0x00 Control Register: card/program state, output edge, lock-unlock cfg */
#define IPRO6_SDIO3_CONTROL_REGISTER_OFFSET                 (0x00)
/* 0x04 Command Register (ROC): last-cmd index, block size, bus width */
#define IPRO6_SDIO3_COMMAND_REGISTER_OFFSET                 (0x04)
/* 0x08 Argument Register (ROC): 32-bit arg of last SD/MMC/SDIO command */
#define IPRO6_SDIO3_ARGUMENT_REGISTER_OFFSET                (0x08)
/* 0x0C Block Count Register (ROC): blocks of current transfer */
#define IPRO6_SDIO3_BLOCK_COUNT_REGISTER_OFFSET             (0x0C)
/* 0x10 ADMA / DMA1 Address Register: ADMA desc table addr (ADMA mode) */
#define IPRO6_SDIO3_DMA1_ADDRESS_REGISTER_OFFSET            (0x10)
/* 0x14 ADMA / DMA1 Control Register: address-valid + DMA1 page size */
#define IPRO6_SDIO3_DMA1_CONTROL_REGISTER_OFFSET            (0x14)
/* 0x3C Interrupt Status Register (RW1C): transfer/reset/cmd11/crc/abort */
#define IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET        (0x3C)
/* 0x40 Interrupt Status Enable Register: per-bit status enable (mask) */
#define IPRO6_SDIO3_INTERRUPT_STATUS_ENABLE_REGISTER_OFFSET (0x40)
/* 0x44 Interrupt Signal Enable Register: per-bit IRQ-line enable */
#define IPRO6_SDIO3_INTERRUPT_SIGNAL_ENABLE_REGISTER_OFFSET (0x44)
/* 0x48 Card Address Register: indirect-access address pointer */
#define IPRO6_SDIO3_CUSTOM_ADDRESS_REGISTER_OFFSET          (0x48)
/* 0x4C Card Data Register: indirect-access data port */
#define IPRO6_SDIO3_CUSTOM_DATA_REGISTER_OFFSET             (0x4C)
/* 0x50 IOREADY Register: per-function ready bits (reflected in CMD5 R4) */
#define IPRO6_SDIO3_IOREADY_REGISTER_OFFSET                 (0x50)
/* 0x54 Function1 Control Register: Function1 device-initiated read count */
#define IPRO6_SDIO3_FUNCTION1_CONTROL_REGISTER_OFFSET       (0x54)
/* 0x58 Function2 Control Register: Function2 device-initiated read count */
#define IPRO6_SDIO3_FUNCTION2_CONTROL_REGISTER_OFFSET       (0x58)
/* 0x5C SDIO CCCR Control Register: CCCR capability/feature support bits */
#define IPRO6_SDIO3_SDIO_CCCR_CONTROL_REGISTER_OFFSET       (0x5C)
/* 0x60-0x7C SDIO FBRx Control Registers: per-function FBR settings.
 * Bits [3:0] = SDIO Standard Function Interface Code (host reads it as the
 * function class; 7h = WLAN). One register per function, 4 bytes apart. */
#define IPRO6_SDIO3_FBR1_CONTROL_REGISTER_OFFSET            (0x60)
#define IPRO6_SDIO3_FBR2_CONTROL_REGISTER_OFFSET            (0x64)
#define IPRO6_SDIO3_FBR_STD_IF_CODE_MASK                    (0x0Fu)
/* 0x80 Card Size Register: capacity in 512B units (out-of-range check) */
#define IPRO6_SDIO3_CARD_SIZE_REGISTER_OFFSET               (0x80)
/* 0x84 Card OCR Register: OCR voltage window + 1.8V switch + I/O func num */
#define IPRO6_SDIO3_CARD_OCR_REGISTER_OFFSET                (0x84)
/* 0x88 Control2 Register: CMD60 R1b mode + ADMA enable */
#define IPRO6_SDIO3_CONTROL2_REGISTER_OFFSET                (0x88)
/* 0x9C Interrupt Status2 Register (RW1C): func3-5 reset, packed, ADMA err */
#define IPRO6_SDIO3_INTERRUPT_STATUS2_REGISTER_OFFSET       (0x9C)
/* 0xA0 Interrupt Status2 Enable Register: per-bit status2 enable (mask) */
#define IPRO6_SDIO3_INTERRUPT_STATUS2_ENABLE_REGISTER_OFFSET (0xA0)
/* 0xA4 Interrupt Signal2 Enable Register: per-bit status2 IRQ-line enable */
#define IPRO6_SDIO3_INTERRUPT_SIGNAL_ENABLE2_REGISTER_OFFSET (0xA4)
/* 0xB8 ADMA Error Status Register: ADMA error state + length-mismatch */
#define IPRO6_SDIO3_ADMA_ERROR_STATUS_REGISTER_OFFSET       (0xB8)
/* 0xBC RCA Register (RO): 16-bit relative card address */
#define IPRO6_SDIO3_RCA_REGISTER_OFFSET                     (0xBC)
/* 0xDC AHB Master Burst Size Register: per-burst-type enable bitmap */
#define IPRO6_SDIO3_AHB_MASTER_BURSTSIZE_REGISTER_OFFSET    (0xDC)
/* 0xE0 Argument2 Register (ROC): 32-bit arg (CMD23 / auto-CMD23 path) */
#define IPRO6_SDIO3_ARGUMENT2_REGISTER_OFFSET               (0xE0)

/* ------------------------------------------------------------------------- */
/* Control Register (0x00) bitfields  (Section 9.2.1, Table 45)              */
/* ------------------------------------------------------------------------- */
#define IPRO6_SDIO3_PROGRAM_DONE                (1u << 0)   /* bit0  set on prog/erase done (release DAT0) */
#define IPRO6_SDIO3_CARD_INIT_DONE              (1u << 2)   /* bit2  1=card ready to operate */
#define IPRO6_SDIO3_CMD_DATA_OUTPUT_EDGE        (1u << 11)  /* bit11 0=falling edge drive, 1=rising edge */

/* ------------------------------------------------------------------------- */
/* Command Register (0x04) bitfields  (Section 9.2.2, Table 46, ROC)         */
/* ------------------------------------------------------------------------- */
#define IPRO6_SDIO3_BLOCK_SIZE_SHIFT            (1)         /* field [12:1] data-block size in bytes */
#define IPRO6_SDIO3_BLOCK_SIZE_MASK             (0xFFFu << IPRO6_SDIO3_BLOCK_SIZE_SHIFT)
#define IPRO6_SDIO3_CURRENT_BUS_WIDTH_SHIFT     (19)        /* field [20:19] 0=1bit,1=4bit,2=8bit */
#define IPRO6_SDIO3_CURRENT_BUS_WIDTH_MASK      (0x3u << IPRO6_SDIO3_CURRENT_BUS_WIDTH_SHIFT)

/* ------------------------------------------------------------------------- */
/* ADMA/DMA1 Control Register (0x14) bitfields  (Section 9.2.6, Table 10)    */
/* ------------------------------------------------------------------------- */
#define IPRO6_SDIO3_DMA1_ADDRESS_VALID          (1u << 0)   /* bit0  1=addr reg + buffer size valid (start) */
#define IPRO6_SDIO3_DMA1_BUFFER_SIZE_MASK       (0x7u << 1) /* field [3:1] DMA1 page/buffer size (000=4K..111=512K) */

/* ------------------------------------------------------------------------- */
/* Interrupt Status Register (0x3C) flags  (Section 9.2.16, Table 20, RW1C)  */
/* (Status Enable @0x40 and Signal Enable @0x44 share this same bit layout)  */
/* ------------------------------------------------------------------------- */
#define IPRO6_SDIO3_TRANSFER_COMPLETE_INTERRUPT       (1u << 0)   /* bit0  transfer complete */
#define IPRO6_SDIO3_DMA1_INTERRUPT                    (1u << 1)   /* bit1  DMA1 page-boundary */
/* bit2  SLEEP/AWAKE (MMC) - not used by driver */
#define IPRO6_SDIO3_WRITE_START_INTERRUPT             (1u << 3)   /* bit3  new write (download) */
#define IPRO6_SDIO3_READ_START_INTERRUPT              (1u << 4)   /* bit4  new read (upload) */
#define IPRO6_SDIO3_CMD11_INTERRUPT                   (1u << 11)  /* bit11 CMD11 voltage-switch */
#define IPRO6_SDIO3_CMD0_CMD52_SOFT_RESET             (1u << 12)  /* bit12 CMD0/CMD52 soft reset */
#define IPRO6_SDIO3_FUNCTION1_RESET                   (1u << 21)  /* bit21 Function1 reset */
#define IPRO6_SDIO3_FUNCTION2_RESET                   (1u << 22)  /* bit22 Function2 reset */
#define IPRO6_SDIO3_CMD11_CLK_STOP                    (1u << 23)  /* bit23 CMD11 clock stop */
#define IPRO6_SDIO3_CMD11_CLK_START                   (1u << 24)  /* bit24 CMD11 clock start */
#define IPRO6_SDIO3_PROGRAM_START                     (1u << 25)  /* bit25 program start (card recognized) */
#define IPRO6_SDIO3_FUNCTIONX_CRC_END_ERROR_INTERRUPT (1u << 28)  /* bit28 FunctionX CRC/end-bit error */
#define IPRO6_SDIO3_FUNCTIONX_ABORT_INTERRUPT         (1u << 29)  /* bit29 FunctionX write/read abort */

/* ------------------------------------------------------------------------- */
/* Interrupt Status2 Register (0x9C) flags  (Section 9.2.19, Table 23, RW1C) */
/* (Status2 Enable @0xA0 and Signal2 Enable @0xA4 share this bit layout)     */
/* ------------------------------------------------------------------------- */
#define IPRO6_SDIO3_ADMA_ERROR_INTERRUPT        (1u << 6)   /* bit6  ADMA error during transfer */

/* ------------------------------------------------------------------------- */
/* SDIO CCCR Control Register (0x5C) flags  (Section 9.2.43, Table 47)       */
/* ------------------------------------------------------------------------- */
#define IPRO6_SDIO3_S8B                         (1u << 12)  /* bit12 8-bit bus support */
#define IPRO6_SDIO3_SDC                         (1u << 14)  /* bit14 direct command (CMD52) support */
#define IPRO6_SDIO3_SMB                         (1u << 15)  /* bit15 multiple-block support */
#define IPRO6_SDIO3_SRW                         (1u << 16)  /* bit16 read-wait support */
#define IPRO6_SDIO3_SBS                         (1u << 17)  /* bit17 bus suspend/resume support */
#define IPRO6_SDIO3_S4MI                        (1u << 18)  /* bit18 block-gap (4MI) interrupt support */
#define IPRO6_SDIO3_SMPC                        (1u << 21)  /* bit21 master power control support */
#define IPRO6_SDIO3_SHS                         (1u << 22)  /* bit22 high-speed support */
#define IPRO6_SDIO3_SDR50                       (1u << 23)  /* bit23 SDR50 support */
#define IPRO6_SDIO3_SDR104                      (1u << 24)  /* bit24 SDR104 support */
#define IPRO6_SDIO3_DDR50                       (1u << 25)  /* bit25 DDR50 support */
#define IPRO6_SDIO3_SAI                         (1u << 29)  /* bit29 asynchronous interrupt support */

/* ------------------------------------------------------------------------- */
/* Card OCR Register (0x84) bitfields  (Section 9.2.46, Table 50)            */
/* ------------------------------------------------------------------------- */
#define IPRO6_SDIO3_CARD_OCR_SHIFT              (0)         /* field [23:0] OCR voltage window */
#define IPRO6_SDIO3_CARD_OCR_MASK               (0xFFFFFFu << IPRO6_SDIO3_CARD_OCR_SHIFT)
#define IPRO6_SDIO3_SWITCHIN_TO_1P8V            (1u << 24)  /* bit24 1=switching to 1.8V accepted */

/* ------------------------------------------------------------------------- */
/* Control2 Register (0x88) bitfields  (Section 9.2.47)                      */
/* ------------------------------------------------------------------------- */
#define IPRO6_SDIO3_ADMA_ENABLE                 (1u << 2)   /* bit2  1=ADMA mode, 0=DMA1 mode */

/* ------------------------------------------------------------------------- */
/* AHB Master Burst Size Register (0xDC) bitfields  (Section 9.2.41, Tbl 45) */
/* ------------------------------------------------------------------------- */
#define IPRO6_SDIO3_AHBMAS_BURST_SIZE_ENABLE_SHIFT  (0)     /* field [6:0] per-burst-type enable */
#define IPRO6_SDIO3_AHBMAS_BURST_SIZE_ENABLE_MASK   (0x7Fu << IPRO6_SDIO3_AHBMAS_BURST_SIZE_ENABLE_SHIFT)
/* bit0 INCR4, bit1 INCR8, bit2 INCR16, bit3 INCR, bit4 WRAP4, bit5 WRAP8, bit6 WRAP16. Reset 7'h07. */

/* ------------------------------------------------------------------------- */
/* ADMA2 descriptor attribute bits  (Section 7.18, Figure 5)                 */
/* Standard SD ADMA2: Attribute[5:0] = {Act2, Act1, 0, Int, End, Valid};      */
/* Act = bits[5:4]; Tran (transfer data) = Act={10}b -> bit5.                 */
/* NOTE: first read as Act=[4:3] (Tran=bit4); on HW that descriptor was       */
/* treated as Reserved/Nop -> the ADMA did NOT write the received data (the    */
/* RX buffer stayed uninitialized garbage). bit5 is the SD-spec ADMA2 Tran.   */
/* ------------------------------------------------------------------------- */
#define IPRO6_SDIO3_ADMA_ATTR_VALID             (1u << 0)   /* bit0  descriptor line is effective */
#define IPRO6_SDIO3_ADMA_ATTR_END               (1u << 1)   /* bit1  end of descriptor */
#define IPRO6_SDIO3_ADMA_ATTR_INT               (1u << 2)   /* bit2  generate DMA interrupt on completion */
#define IPRO6_SDIO3_ADMA_ATTR_ACT_TRAN          (1u << 5)   /* [5:4] Act={10}b -> Tran (transfer one line) */

#endif /* __IPRO6_SDIO3_REG_H__ */

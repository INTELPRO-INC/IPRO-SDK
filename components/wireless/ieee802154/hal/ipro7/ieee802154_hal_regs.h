/**
 * @file ieee802154_hal_regs.h
 * @brief IEEE 802.15.4 HAL Register Mapping
 * 
 * This file provides convenient register aliases mapping to the auto-generated
 * register headers from IPRO7AI_REG_all_20251223.xlsx
 * 
 * @copyright Copyright (c) 2025 INTELPRO Inc.
 */

#ifndef __IEEE802154_HAL_REGS_H__
#define __IEEE802154_HAL_REGS_H__

/* Auto-generated register headers */
#include "m154_reg.h"
#include "m154_mem_reg.h"
#include "m154_aes_reg.h"

/* BZ PHY registers from ipro7_rf/reg/ (added via CMake include) */
#include "bz_phy_reg.h"

/* BZ PHY Base Address (IPRO7: 0x30002000, matches ipro7.h) */
#ifndef BZ_PHY_BASE
#define BZ_PHY_BASE                     (0x30002000)
#endif

/* BZ PHY Register access macros */
#define BZ_PHY_REG(offset)              (BZ_PHY_BASE + (offset))
#define BZ_PHY_REG_READ(offset)         (*((volatile uint32_t *)(BZ_PHY_BASE + (offset))))
#define BZ_PHY_REG_WRITE(offset, val)   (*((volatile uint32_t *)(BZ_PHY_BASE + (offset))) = (val))

/*===========================================================================
 * Register Access Macros
 *===========================================================================*/

#define HAL_REG_READ(addr)          (*((volatile uint32_t *)(addr)))
#define HAL_REG_WRITE(addr, val)    (*((volatile uint32_t *)(addr)) = (val))

/*===========================================================================
 * M154 MAC Register Aliases (from m154_reg.h)
 * 
 * Maps to actual registers defined in m154_reg.h (base 0x3C000000)
 *===========================================================================*/

/* M154 Base Address */
#define HAL_M154_BASE               M154_BASE

/* Global Configuration */
#define HAL_M154_GLB_CFG            M154_M154_GLB_CFG           /* 0x000 */
#define HAL_M154_DATA_CFG           M154_M154_DATA_CFG          /* 0x004 */
#define HAL_M154_SWRST              M154_M154_SWRST             /* 0x008 */

/* SWRST Bit Masks (M154_SWRST register bits) */
#define HAL_M154_SOFT_RESET_TX_CMD      (1 << 3)  /* Soft reset TX engine */
#define HAL_M154_SOFT_RESET_CSMA_CA_CMD (1 << 4)  /* Soft reset CSMA-CA engine */
#define HAL_M154_SOFT_RESET_RX_CMD      (1 << 2)  /* Soft reset RX engine */

/* Debug */
#define HAL_M154_DBG_SEL            M154_M154_DBG_SEL           /* 0x00C */
#define HAL_M154_DBG_INT_EN         M154_M154_DBG_INT_EN        /* 0x010 */

/* Interrupts */
#define HAL_M154_INT_EN             M154_M154_INT_EN            /* 0x020 */
#define HAL_M154_INT_CLR            M154_M154_INT_CLR           /* 0x024 */
#define HAL_M154_INT_MASK           M154_M154_INT_MASK          /* 0x028 */
#define HAL_M154_INT_RAW            M154_M154_INT_RAW           /* 0x02C */
#define HAL_M154_INT                M154_M154_INT               /* 0x030 */

/* GCI Control */
#define HAL_M154_GCI_CTRL0          M154_M154_GCI_CTRL0         /* 0x034 */
#define HAL_M154_GCI_CTRL1          M154_M154_GCI_CTRL1         /* 0x038 */

/* Symbol Counter & Timestamps */
#define HAL_M154_SYM_CNT            M154_M154_SYM_CNT           /* 0x040 */
#define HAL_M154_SYM_CNT_FRC        M154_M154_SYM_CNT_FRC       /* 0x044 */
#define HAL_M154_TS_RX_BEACON_P1    M154_M154_TS_RX_BEACON_P1   /* 0x048 */
#define HAL_M154_TS_RX_BEACON_P2    M154_M154_TS_RX_BEACON_P2   /* 0x04C */
#define HAL_M154_TS_RX_START        M154_M154_TS_RX_START       /* 0x050 */
#define HAL_M154_TS_RX_END          M154_M154_TS_RX_END         /* 0x054 */
#define HAL_M154_TS_CSMA_CA_START   M154_M154_TS_CSMA_CA_START  /* 0x058 */
#define HAL_M154_TS_TX_BEACON       M154_M154_TS_TX_BEACON      /* 0x05C */
#define HAL_M154_TS_TX_END          M154_M154_TS_TX_END         /* 0x060 */

/* Alarm Targets */
#define HAL_M154_ALARM_00_TARGET    M154_M154_ALARM_00_TARGET   /* 0x064 */
#define HAL_M154_ALARM_01_TARGET    M154_M154_ALARM_01_TARGET   /* 0x068 */
#define HAL_M154_ALARM_02_TARGET    M154_M154_ALARM_02_TARGET   /* 0x06C */
#define HAL_M154_ALARM_03_TARGET    M154_M154_ALARM_03_TARGET   /* 0x070 */
#define HAL_M154_ALARM_04_TARGET    M154_M154_ALARM_04_TARGET   /* 0x074 */
#define HAL_M154_ALARM_05_TARGET    M154_M154_ALARM_05_TARGET   /* 0x078 */
#define HAL_M154_ALARM_06_TARGET    M154_M154_ALARM_06_TARGET   /* 0x07C */
#define HAL_M154_ALARM_07_TARGET    M154_M154_ALARM_07_TARGET   /* 0x080 */
#define HAL_M154_ALARM_08_TARGET    M154_M154_ALARM_08_TARGET   /* 0x084 */
#define HAL_M154_ALARM_09_TARGET    M154_M154_ALARM_09_TARGET   /* 0x088 */

/* CSMA-CA Configuration */
#define HAL_M154_CSMA_0             M154_M154_CSMA_0            /* 0x090 */
#define HAL_M154_CSMA_1             M154_M154_CSMA_1            /* 0x094 */
#define HAL_M154_CSMA_2             M154_M154_CSMA_2            /* 0x098 */
#define HAL_M154_CSMA_3             M154_M154_CSMA_3            /* 0x09C */
#define HAL_M154_CSMA_4             M154_M154_CSMA_4            /* 0x0A0 */

/* TX Configuration */
#define HAL_M154_TX_0               M154_M154_TX_0              /* 0x0B0 */
#define HAL_M154_TX_AUTO_ACK_DOING  (1 << 11)  /* Auto ACK in progress flag (TX_0 bit 11) */
#define HAL_M154_TX_1               M154_M154_TX_1              /* 0x0B4 */
#define HAL_M154_TX_2               M154_M154_TX_2              /* 0x0B8 */
#define HAL_M154_TX_3               M154_M154_TX_3              /* 0x0BC */
#define HAL_M154_TX_FSM_CURR_MASK   0x7FF      /* TX FSM state mask (TX_3 bits 0-10) */
#define HAL_M154_TX_FSM_IDLE        0x001      /* TX FSM IDLE state value (== 1) */

/* RX Configuration (address filtering, etc.) */
#define HAL_M154_RX_00              M154_M154_RX_00             /* 0x0C0 */
#define HAL_M154_RX_01              M154_M154_RX_01             /* 0x0C4 */
#define HAL_M154_RX_02              M154_M154_RX_02             /* 0x0C8 */
#define HAL_M154_RX_03              M154_M154_RX_03             /* 0x0CC */
#define HAL_M154_RX_04              M154_M154_RX_04             /* 0x0D0 */
#define HAL_M154_RX_05              M154_M154_RX_05             /* 0x0D4 */
#define HAL_M154_RX_06              M154_M154_RX_06             /* 0x0D8 */
#define HAL_M154_RX_07              M154_M154_RX_07             /* 0x0DC */
#define HAL_M154_RX_08              M154_M154_RX_08             /* 0x0E0 */
#define HAL_M154_RX_09              M154_M154_RX_09             /* 0x0E4 */
#define HAL_M154_RX_OFF_TRIG_BY_END_EN  (1 << 5)   /* Auto RX restart control */
#define HAL_M154_RX_OFF_CMD         (1 << 8)   /* RX OFF command */
#define HAL_M154_RX_ON_CMD          (1 << 12)  /* RX ON command */
#define HAL_M154_RX_SFD_FOUND       (1 << 18)  /* SFD found flag - CORRECTED from bit 24 to 18 */
#define HAL_M154_RX_10              M154_M154_RX_10             /* 0x0E8 */
#define HAL_M154_RX_11              M154_M154_RX_11             /* 0x0EC */
#define HAL_M154_RX_12              M154_M154_RX_12             /* 0x0F0 */
#define HAL_M154_RX_13              M154_M154_RX_13             /* 0x0F4 */
#define HAL_M154_RX_14              M154_M154_RX_14             /* 0x0F8 */
#define HAL_M154_RX_15              M154_M154_RX_15             /* 0x0FC */
#define HAL_M154_RX_16              M154_M154_RX_16             /* 0x100 */
#define HAL_M154_RX_17              M154_M154_RX_17             /* 0x104 */

/* Miscellaneous */
#define HAL_M154_CAPT_FRM_00        M154_M154_CAPT_FRM_00       /* 0x10C */
#define HAL_M154_SW_PROFILE         M154_M154_SW_PROFILE        /* 0x110 */

/* Coexistence */
#define HAL_M154_COEX_PRIORITY      M154_M154_COEX_PRIORITY     /* 0x180 */

/* TX/RX PPDU Control */
#define HAL_M154_TX_PPDU_PHR        M154_M154_TX_PPDU_PHR       /* 0x280 */
#define HAL_M154_TX_PPDU_CTRL       M154_M154_TX_PPDU_CTRL      /* 0x300 */
#define HAL_M154_RX_PPDU_STATUS     M154_M154_RX_PPDU_STATUS    /* 0x340 */

/* Neighbor Control */
#define HAL_M154_NBR_CTRL_0         M154_M154_NBR_CTRL_0        /* 0x380 */

/*===========================================================================
 * M154 Memory Register Aliases (from m154_mem_reg.h)
 * TX/RX MPDU buffers at offset 0x400+ and 0x500+
 *===========================================================================*/

/* TX MPDU buffer base - 256 bytes (64 x 32-bit words) at 0x400 */
#define HAL_TX_MPDU_BASE            (M154_MEM_BASE + M154_MEM_M154_TX_MPDU_00_OFFSET)
#define HAL_TX_MPDU(idx)            (HAL_TX_MPDU_BASE + ((idx) * 4))

/* RX MPDU buffer base - 256 bytes (64 x 32-bit words) at 0x500 */
#define HAL_RX_MPDU_BASE            (M154_MEM_BASE + M154_MEM_M154_RX_MPDU_00_OFFSET)
#define HAL_RX_MPDU(idx)            (HAL_RX_MPDU_BASE + ((idx) * 4))

/*===========================================================================
 * M154 AES Register Aliases (from m154_aes_reg.h)
 * AES registers at offset 0x800+
 *===========================================================================*/

/* AES Control */
#define HAL_AES_CTRL                M154_AES_M154_AES_ACR       /* 0x800 - AES Control Register */
#define HAL_AES_MODE                M154_AES_M154_AES_AMR       /* 0x804 - AES Mode Register */
#define HAL_AES_LEN                 M154_AES_M154_AES_ALR       /* 0x808 - AES Length Register */
#define HAL_AES_RXALEN              M154_AES_M154_AES_RXALR     /* 0x80C - RX A-Length Register */

/* AES Address Pointers */
#define HAL_AES_AASAR               M154_AES_M154_AES_AASAR     /* 0x810 - AAD Start Address */
#define HAL_AES_AMSAR               M154_AES_M154_AES_AMSAR     /* 0x814 - Message Start Address */
#define HAL_AES_AMDAR               M154_AES_M154_AES_AMDAR     /* 0x818 - Message Dest Address */
#define HAL_AES_RXAADAR             M154_AES_M154_AES_RXAADAR   /* 0x81C - RX AAD Address */
#define HAL_AES_RXAMDAR             M154_AES_M154_AES_RXAMDAR   /* 0x820 - RX Message Dest */

/* AES Nonce (128-bit, 4x32-bit) at 0x830 */
#define HAL_AES_NONCE_BASE          (M154_AES_BASE + M154_AES_M154_AES_ANR0_OFFSET)
#define HAL_AES_NONCE(idx)          (HAL_AES_NONCE_BASE + ((idx) * 4))
#define HAL_AES_NONCE0              M154_AES_M154_AES_ANR0      /* 0x830 */
#define HAL_AES_NONCE1              M154_AES_M154_AES_ANR1      /* 0x834 */
#define HAL_AES_NONCE2              M154_AES_M154_AES_ANR2      /* 0x838 */
#define HAL_AES_NONCE3              M154_AES_M154_AES_ANR3      /* 0x83C */

/* AES Key (256-bit, 8x32-bit) at 0x840 */
#define HAL_AES_KEY_BASE            (M154_AES_BASE + M154_AES_M154_AES_AKR0_OFFSET)
#define HAL_AES_KEY(idx)            (HAL_AES_KEY_BASE + ((idx) * 4))
#define HAL_AES_KEY0                M154_AES_M154_AES_AKR0      /* 0x840 */
#define HAL_AES_KEY1                M154_AES_M154_AES_AKR1      /* 0x844 */
#define HAL_AES_KEY2                M154_AES_M154_AES_AKR2      /* 0x848 */
#define HAL_AES_KEY3                M154_AES_M154_AES_AKR3      /* 0x84C */
#define HAL_AES_KEY4                M154_AES_M154_AES_AKR4      /* 0x850 */
#define HAL_AES_KEY5                M154_AES_M154_AES_AKR5      /* 0x854 */
#define HAL_AES_KEY6                M154_AES_M154_AES_AKR6      /* 0x858 */
#define HAL_AES_KEY7                M154_AES_M154_AES_AKR7      /* 0x85C */

/* AES MIC Result (128-bit, 4x32-bit) at 0x860 */
#define HAL_AES_MIC_BASE            (M154_AES_BASE + M154_AES_M154_AES_AMICR0_OFFSET)
#define HAL_AES_MIC(idx)            (HAL_AES_MIC_BASE + ((idx) * 4))
#define HAL_AES_MIC0                M154_AES_M154_AES_AMICR0    /* 0x860 */
#define HAL_AES_MIC1                M154_AES_M154_AES_AMICR1    /* 0x864 */
#define HAL_AES_MIC2                M154_AES_M154_AES_AMICR2    /* 0x868 */
#define HAL_AES_MIC3                M154_AES_M154_AES_AMICR3    /* 0x86C */

/* AES Status */
#define HAL_AES_STATUS1             M154_AES_M154_AES_STATUS1   /* 0x870 */
#define HAL_AES_STATUS2             M154_AES_M154_AES_STATUS2   /* 0x874 */
#define HAL_AES_STATUS3             M154_AES_M154_AES_STATUS3   /* 0x878 */

/* AES Interrupts */
#define HAL_AES_IRQ_EN              M154_AES_M154_AES_IER       /* 0x880 - Interrupt Enable */
#define HAL_AES_IRQ_CLR             M154_AES_M154_AES_ICR       /* 0x884 - Interrupt Clear */
#define HAL_AES_IRQ_MASK            M154_AES_M154_AES_IMR       /* 0x888 - Interrupt Mask */
#define HAL_AES_IRQ_STATUS          M154_AES_M154_AES_ISR       /* 0x88C - Interrupt Status */
#define HAL_AES_IRQ_RAW             M154_AES_M154_AES_IR        /* 0x890 - Interrupt Raw */

/*===========================================================================
 * BZ PHY Register Aliases (from bz_phy_reg.h)
 * PHY registers at base 0x30002000, actual registers at offset 0x800+
 * Use offset-based macros for register access
 *===========================================================================*/

/* PHY Top Control */
#define HAL_PHY_TOP_0               BZ_PHY_REG(BZ_PHY_PHY_TOP_0_OFFSET)            /* 0x800 */

/* PHY TX DFE */
#define HAL_PHY_TX_DFE_0            BZ_PHY_REG(BZ_PHY_PHY_TX_DFE_0_OFFSET)         /* 0x804 */
#define HAL_PHY_TX_DFE_1            BZ_PHY_REG(BZ_PHY_PHY_TX_DFE_1_OFFSET)         /* 0x808 */
#define HAL_PHY_TX_DFE_2            BZ_PHY_REG(BZ_PHY_PHY_TX_DFE_2_OFFSET)         /* 0x80C */

/* PHY RX DFE */
#define HAL_PHY_RX_DFE_0            BZ_PHY_REG(BZ_PHY_PHY_RX_DFE_0_OFFSET)         /* 0x810 */
#define HAL_PHY_RX_DFE_1            BZ_PHY_REG(BZ_PHY_PHY_RX_DFE_1_OFFSET)         /* 0x814 */
#define HAL_PHY_RX_DFE_2            BZ_PHY_REG(BZ_PHY_PHY_RX_DFE_2_OFFSET)         /* 0x818 */
#define HAL_PHY_RX_DFE_3            BZ_PHY_REG(BZ_PHY_PHY_RX_DFE_3_OFFSET)         /* 0x81C */

/* PHY ZB Receiver (Zigbee/802.15.4 specific) */
#define HAL_PHY_RX_ZB_RECEIVER_0    BZ_PHY_REG(BZ_PHY_PHY_RX_ZB_RECEIVER_0_OFFSET) /* 0x820 */
#define HAL_PHY_RX_ZB_RECEIVER_1    BZ_PHY_REG(BZ_PHY_PHY_RX_ZB_RECEIVER_1_OFFSET) /* 0x824 */
#define HAL_PHY_RX_ZB_RECEIVER_2    BZ_PHY_REG(BZ_PHY_PHY_RX_ZB_RECEIVER_2_OFFSET) /* 0x828 */
#define HAL_PHY_RX_ZB_RECEIVER_3    BZ_PHY_REG(BZ_PHY_PHY_RX_ZB_RECEIVER_3_OFFSET) /* 0x82C */
#define HAL_PHY_RX_ZB_RECEIVER_4    BZ_PHY_REG(BZ_PHY_PHY_RX_ZB_RECEIVER_4_OFFSET) /* 0x830 */

/* PHY CCA */
#define HAL_PHY_CCA_0               BZ_PHY_REG(BZ_PHY_PHY_CCA_0_OFFSET)            /* 0x834 */
#define HAL_PHY_CCA_1               BZ_PHY_REG(BZ_PHY_PHY_CCA_1_OFFSET)            /* 0x92C */

/* PHY Status */
#define HAL_PHY_STATUS_0            BZ_PHY_REG(BZ_PHY_PHY_STATUS_0_OFFSET)         /* 0x860 */
#define HAL_PHY_STATUS_1            BZ_PHY_REG(BZ_PHY_PHY_STATUS_1_OFFSET)         /* 0x864 */
#define HAL_PHY_STATUS_2            BZ_PHY_REG(BZ_PHY_PHY_STATUS_2_OFFSET)         /* 0x868 */
#define HAL_PHY_STATUS_3            BZ_PHY_REG(BZ_PHY_PHY_STATUS_3_OFFSET)         /* 0x86C */
#define HAL_PHY_STATUS_4            BZ_PHY_REG(BZ_PHY_PHY_STATUS_4_OFFSET)         /* 0x870 */
#define HAL_PHY_STATUS_5            BZ_PHY_REG(BZ_PHY_PHY_STATUS_5_OFFSET)         /* 0x874 */
#define HAL_PHY_STATUS_6            BZ_PHY_REG(BZ_PHY_PHY_STATUS_6_OFFSET)         /* 0x878 */
#define HAL_PHY_STATUS_7            BZ_PHY_REG(BZ_PHY_PHY_STATUS_7_OFFSET)         /* 0x87C */
#define HAL_PHY_STATUS_8            BZ_PHY_REG(BZ_PHY_PHY_STATUS_8_OFFSET)         /* 0x880 */
#define HAL_PHY_STATUS_9            BZ_PHY_REG(BZ_PHY_PHY_STATUS_9_OFFSET)         /* 0x884 */
#define HAL_PHY_STATUS_10           BZ_PHY_REG(BZ_PHY_PHY_STATUS_10_OFFSET)        /* 0x888 */

/* PHY Low Power */
#define HAL_PHY_LOW_PWR_0           BZ_PHY_REG(BZ_PHY_PHY_LOW_PWR_0_OFFSET)        /* 0x8A0 */

/* PHY RX DDC IF Control */
#define HAL_PHY_RX_DDC_IF_CTRL_0    BZ_PHY_REG(BZ_PHY_RX_DDC_IF_CTRL_0_OFFSET)     /* 0x950 */
#define HAL_PHY_RX_DDC_IF_CTRL_1    BZ_PHY_REG(BZ_PHY_RX_DDC_IF_CTRL_1_OFFSET)     /* 0x954 */
#define HAL_PHY_RX_DDC_IF_CTRL_2    BZ_PHY_REG(BZ_PHY_RX_DDC_IF_CTRL_2_OFFSET)     /* 0x958 */
#define HAL_PHY_RX_DDC_IF_CTRL_3    BZ_PHY_REG(BZ_PHY_RX_DDC_IF_CTRL_3_OFFSET)     /* 0x95C */

/* PHY PA Reference DAC */
#define HAL_PHY_PA_REF_DAC0         BZ_PHY_REG(BZ_PHY_PA_REF_DAC0_OFFSET)          /* 0x930 */
#define HAL_PHY_PA_REF_DAC1         BZ_PHY_REG(BZ_PHY_PA_REF_DAC1_OFFSET)          /* 0x934 */
#define HAL_PHY_PA_REF_DAC2         BZ_PHY_REG(BZ_PHY_PA_REF_DAC2_OFFSET)          /* 0x938 */
#define HAL_PHY_PA_REF_DAC3         BZ_PHY_REG(BZ_PHY_PA_REF_DAC3_OFFSET)          /* 0x93C */
#define HAL_PHY_PA_REF_DAC4         BZ_PHY_REG(BZ_PHY_PA_REF_DAC4_OFFSET)          /* 0x940 */
#define HAL_PHY_PA_REF_DAC5         BZ_PHY_REG(BZ_PHY_PA_REF_DAC5_OFFSET)          /* 0x944 */
#define HAL_PHY_PA_REF_DAC6         BZ_PHY_REG(BZ_PHY_PA_REF_DAC6_OFFSET)          /* 0x948 */

/*===========================================================================
 * BZ PHY AGC Register Aliases
 * AGC registers use reg_bz_phy_agc.h inline accessors - define address macros
 * AGC base: 0x30003000, registers at offset 0xC00+
 *===========================================================================*/

#ifndef BZ_PHY_AGC_BASE
#define BZ_PHY_AGC_BASE             (0x30003000)
#endif

#define BZ_PHY_AGC_REG(offset)      (BZ_PHY_AGC_BASE + (offset))

/* AGC Control */
#define HAL_AGC_CTRL_0              BZ_PHY_AGC_REG(0xC00)       /* 0xC00 */
#define HAL_AGC_CTRL_1              BZ_PHY_AGC_REG(0xC04)       /* 0xC04 */
#define HAL_AGC_CTRL_2              BZ_PHY_AGC_REG(0xC08)       /* 0xC08 */

/* AGC Peak Detect */
#define HAL_AGC_PKDET_0             BZ_PHY_AGC_REG(0xC0C)       /* 0xC0C */

/* AGC ADC SAT (Zigbee specific) */
#define HAL_AGC_ADC_SAT_0           BZ_PHY_AGC_REG(0xC10)       /* 0xC10 */
#define HAL_AGC_ADC_SAT_1_ZB        BZ_PHY_AGC_REG(0xC14)       /* 0xC14 */

/* AGC FE SAT (Zigbee specific) */
#define HAL_AGC_FE_SAT_0            BZ_PHY_AGC_REG(0xC24)       /* 0xC24 */
#define HAL_AGC_FE_SAT_1_ZB         BZ_PHY_AGC_REG(0xC28)       /* 0xC28 */

/* AGC Gain (Zigbee specific) */
#define HAL_AGC_GAIN_UNLOCK_0_ZB    BZ_PHY_AGC_REG(0xC40)       /* 0xC40 */
#define HAL_AGC_GAIN_LOCK_0_ZB      BZ_PHY_AGC_REG(0xC44)       /* 0xC44 */

/* AGC Status */
#define HAL_AGC_STATUS_0            BZ_PHY_AGC_REG(0xCC0)       /* 0xCC0 */
#define HAL_AGC_STATUS_1            BZ_PHY_AGC_REG(0xCC4)       /* 0xCC4 */
#define HAL_AGC_STATUS_2            BZ_PHY_AGC_REG(0xCC8)       /* 0xCC8 */
#define HAL_AGC_STATUS_3            BZ_PHY_AGC_REG(0xCCC)       /* 0xCCC */
#define HAL_AGC_STATUS_4            BZ_PHY_AGC_REG(0xCD4)       /* 0xCD4 */
#define HAL_AGC_STATUS_5            BZ_PHY_AGC_REG(0xCD8)       /* 0xCD8 */
#define HAL_AGC_STATUS_6            BZ_PHY_AGC_REG(0xCDC)       /* 0xCDC */
#define HAL_AGC_STATUS_7            BZ_PHY_AGC_REG(0xCEC)       /* 0xCEC */
#define HAL_AGC_STATUS_8            BZ_PHY_AGC_REG(0xCF0)       /* 0xCF0 */
#define HAL_AGC_STATUS_9            BZ_PHY_AGC_REG(0xCF4)       /* 0xCF4 */

/* AGC LNA Gain Table */
#define HAL_AGC_LNA_GAIN_TABLE_0    BZ_PHY_AGC_REG(0xCE0)       /* 0xCE0 */
#define HAL_AGC_LNA_GAIN_TABLE_1    BZ_PHY_AGC_REG(0xCE4)       /* 0xCE4 */
#define HAL_AGC_LNA_GAIN_TABLE_2    BZ_PHY_AGC_REG(0xCE8)       /* 0xCE8 */

/*===========================================================================
 * Register Bit Field Definitions
 * 
 * IMPORTANT: These bit definitions are based on M154 MAC hardware.
 * Features marked [SW] require software implementation as no dedicated
 * hardware registers exist for them.
 * 
 * TODO: Verify exact bit positions from hardware specification document.
 *===========================================================================*/

/*
 * M154 GLB_CFG Register Bits (offset 0x000) - from m154_reg.h
 * WARNING: This register does NOT have RX_EN, TX_START, AUTO_ACK,
 *          FRAME_FILTER, PROMISCUOUS, or CSMA_EN bits!
 * Those are controlled by RX_09 (RX), TX_1 (TX/ACK), and RX_02 (promiscuous).
 */
#define HAL_M154_GLB_CFG_CLK_EN         (1 << 0)    /* M154_CLK_EN: Module clock enable */
#define HAL_M154_GLB_CFG_BEACON_MODE    (1 << 1)    /* M154_BEACON_MODE_EN */
#define HAL_M154_GLB_CFG_ZBEN_PROTECT   (1 << 2)    /* M154_ZBEN_PROTECT */
/* bits [9:4]: M154_CLK_XTAL_FREQ (6-bit crystal frequency) */
/* bits [13:12]: M154_STD_VERSION */
/* bits [27:16]: M154_RF_CENTER_FREQ_REG */

/* Backward compat alias */
#define HAL_M154_GLB_CFG_ENABLE         HAL_M154_GLB_CFG_CLK_EN

/*
 * M154 RX Control: RX_09 Register (offset 0x0E4)
 * RX ON/OFF commands and status bits
 */
/* HAL_M154_RX_OFF_TRIG_BY_END_EN already defined: (1 << 5) */
/* HAL_M154_RX_OFF_CMD already defined: (1 << 8) */
/* HAL_M154_RX_ON_CMD already defined: (1 << 12) */

/*
 * M154 TX Control: TX_1 Register (offset 0x0B4)
 * TX trigger, auto-ACK enable, TX start offset
 */
#define HAL_M154_TX_TRIG_CMD            (1 << 0)    /* M154_TX_TRIG_CMD: Trigger TX */
#define HAL_M154_TX_MPDU_SRC_SEL        (1 << 1)    /* M154_TX_MPDU_SRC_SEL */
#define HAL_M154_TX_TRIG_TIME_SEL       (1 << 2)    /* M154_TX_TRIG_TIME_SEL */
#define HAL_M154_TX_AUTO_ACK_EN         (1 << 4)    /* M154_TX_AUTO_ACK_EN: HW auto-ACK */
/* bits [15:8]: M154_TX_START_OFFSET (8-bit) */

/*
 * M154 RX_02 Register (offset 0x0C8) - Address filter & promiscuous
 */
#define HAL_M154_RX_PAN_COORD_1ST       (1 << 0)    /* M154_RX_PAN_COORDINATOR_1ST */
#define HAL_M154_RX_PAN_COORD_2ND       (1 << 4)    /* M154_RX_PAN_COORDINATOR_2ND */
#define HAL_M154_RX_STACK_1ST_EN        (1 << 8)    /* M154_RX_STACK_1ST_EN */
#define HAL_M154_RX_STACK_2ND_EN        (1 << 12)   /* M154_RX_STACK_2ND_EN */
#define HAL_M154_RX_PROMISCUOUS_MODE    (1 << 16)   /* M154_RX_PROMISCUOUS_MODE */

/*
 * M154 SWRST Register (offset 0x008) - Soft reset commands
 */
#define HAL_M154_SWRST_REGS             (1 << 0)    /* M154_SOFT_RESET_REGS_CMD */
#define HAL_M154_SWRST_TCON             (1 << 1)    /* M154_SOFT_RESET_TCON_CMD */
#define HAL_M154_SWRST_RX               (1 << 2)    /* M154_SOFT_RESET_RX_CMD */
#define HAL_M154_SWRST_TX               (1 << 3)    /* M154_SOFT_RESET_TX_CMD */
#define HAL_M154_SWRST_CSMA_CA          (1 << 4)    /* M154_SOFT_RESET_CSMA_CA_CMD */

/* 
 * NOTE: The following IEEE 802.15.4-2015/2020 features do NOT have dedicated 
 * hardware registers and MUST be implemented in software:
 *   - CSL (Coordinated Sampled Listening) - Thread/Zigbee low power
 *   - Enhanced ACK with IE (Information Elements) 
 *   - Frame Pending management for indirect transmission
 *   - Link Metrics reporting
 * 
 * Software implementation flags (stored in HAL state, not actual registers):
 */
#define HAL_SW_FEATURE_CSL_EN           (1 << 0)    /* [SW] CSL Enable */
#define HAL_SW_FEATURE_ENH_ACK          (1 << 1)    /* [SW] Enhanced ACK */
#define HAL_SW_FEATURE_FRAME_PENDING    (1 << 2)    /* [SW] Frame Pending */
#define HAL_SW_FEATURE_LINK_METRICS     (1 << 3)    /* [SW] Link Metrics */

/* 
 * M154 Interrupt Bits (M154_INT_EN / M154_INT / M154_INT_CLR registers)
 * Based on m154_reg.h actual hardware bit positions
 * 
 * Bits 0-9: Alarm interrupts (10 alarm channels)
 * Bits 10-19: Various RX events (FRAK, ENH_ACK, EXT, MPP, NBR, CS, RX_START, etc.)
 * Bits 20-24: RX frame type interrupts
 * Bits 25-27: CSMA-CA interrupts  
 * Bits 28-29: TX interrupts
 * Bits 30-31: Abort interrupts
 */

/* Alarm interrupts (bits 0-9) */
#define HAL_M154_INT_ALARM_00           (1 << 0)    /* Alarm 0 */
#define HAL_M154_INT_ALARM_01           (1 << 1)    /* Alarm 1 */
#define HAL_M154_INT_ALARM_02           (1 << 2)    /* Alarm 2 */
#define HAL_M154_INT_ALARM_03           (1 << 3)    /* Alarm 3 */
#define HAL_M154_INT_ALARM_04           (1 << 4)    /* Alarm 4 */
#define HAL_M154_INT_ALARM_05           (1 << 5)    /* Alarm 5 */
#define HAL_M154_INT_ALARM_06           (1 << 6)    /* Alarm 6 */
#define HAL_M154_INT_ALARM_07           (1 << 7)    /* Alarm 7 */
#define HAL_M154_INT_ALARM_08           (1 << 8)    /* Alarm 8 */
#define HAL_M154_INT_ALARM_09           (1 << 9)    /* Alarm 9 */

/* RX event interrupts (bits 10-19) */
#define HAL_M154_INT_RX_END_FRAK        (1 << 10)   /* RX End FRAK */
#define HAL_M154_INT_RX_END_REQ_ENH_ACK (1 << 11)   /* RX End Request Enhanced ACK */
#define HAL_M154_INT_RX_END_EXT         (1 << 12)   /* RX End Extended */
#define HAL_M154_INT_RX_END_MPP         (1 << 13)   /* RX End MPP */
#define HAL_M154_INT_RX_NBR_SEARCH_FAIL (1 << 14)   /* RX Neighbor Search Fail */
#define HAL_M154_INT_CS_ALERT           (1 << 15)   /* Carrier Sense Alert */
#define HAL_M154_INT_RX_START           (1 << 16)   /* RX Start */
#define HAL_M154_INT_RX_MHR             (1 << 17)   /* RX MHR (MAC Header) */
#define HAL_M154_INT_RX_MHR_SECURITY    (1 << 18)   /* RX MHR Security */
#define HAL_M154_INT_RX_FILTER_FAIL     (1 << 19)   /* RX Filter Fail */

/* RX frame type interrupts (bits 20-24) */
#define HAL_M154_INT_RX_END_BEACON      (1 << 20)   /* RX End Beacon */
#define HAL_M154_INT_RX_END_DATA        (1 << 21)   /* RX End Data */
#define HAL_M154_INT_RX_END_ACK         (1 << 22)   /* RX End ACK */
#define HAL_M154_INT_RX_END_CMD         (1 << 23)   /* RX End Command */
#define HAL_M154_INT_RX_END_ACK_OTHERS  (1 << 24)   /* RX End ACK Others (promiscuous) */

/* CSMA-CA interrupts (bits 25-27) */
#define HAL_M154_INT_CSMA_CA_SUCCESS    (1 << 25)   /* CSMA-CA Success */
#define HAL_M154_INT_CSMA_CA_FAILURE    (1 << 26)   /* CSMA-CA Failure */
#define HAL_M154_INT_CSMA_CA_TIMEOUT    (1 << 27)   /* CSMA-CA Timeout */

/* TX interrupts (bits 28-29) */
#define HAL_M154_INT_TX_DONE            (1 << 28)   /* TX Complete */
#define HAL_M154_INT_TX_AUTO_ACK_DONE   (1 << 29)   /* TX Auto-ACK Done */

/* Abort interrupts (bits 30-31) */
#define HAL_M154_INT_RX_ABORT           (1 << 30)   /* RX Abort */
#define HAL_M154_INT_TX_ABORT           (1 << 31)   /* TX Abort */

/* Convenience masks for common interrupt groups */
#define HAL_M154_INT_RX_ALL             (0x00F00000)  /* All RX frame type interrupts (bits 20-23) */
#define HAL_M154_INT_CSMA_ALL           (0x0E000000)  /* All CSMA interrupts (bits 25-27) */
#define HAL_M154_INT_TX_ALL             (0x30000000)  /* All TX interrupts (bits 28-29) */
#define HAL_M154_INT_ALARM_ALL          (0x000003FF)  /* All Alarm interrupts (bits 0-9) */

/* BZ PHY Control Bits (PHY_TOP_0) */
#define HAL_PHY_CTRL_SLEEP              (1 << 0)    /* Sleep Mode */
#define HAL_PHY_CTRL_RX_EN              (1 << 1)    /* RX Enable */
#define HAL_PHY_CTRL_TX_EN              (1 << 2)    /* TX Enable */

/* CCA Status Bits (from CSMA_4 register) */
#define HAL_CCA_DONE                    (1 << 0)    /* CCA Complete */
#define HAL_CCA_CLEAR                   (1 << 1)    /* Channel Clear */

/* AES Control Bits (AES_ACR register) */
#define HAL_AES_CTRL_START              (1 << 0)    /* Start AES Operation */
#define HAL_AES_CTRL_ENCRYPT            (1 << 1)    /* Encrypt Mode */
#define HAL_AES_CTRL_DECRYPT            (0 << 1)    /* Decrypt Mode */

/* AES Status Bits (AES_STATUS1 register) */
#define HAL_AES_STATUS_DONE             (1 << 0)    /* AES Complete */
#define HAL_AES_STATUS_MIC_VALID        (1 << 1)    /* MIC Valid */

/*===========================================================================
 * Address Configuration Mapping
 * 
 * Based on lmac154 register layout (verified from lmac154.c):
 * - RX_03: PAN ID [31:16] + Short Address [15:0] (packed in one register)
 * - RX_04: Extended Address Low (bytes 0-3)
 * - RX_05: Extended Address High (bytes 4-7)
 * - RX_06: 2nd stack PAN ID [31:16] + Short Address [15:0]
 * - RX_07/RX_08: 2nd stack Extended Address
 *
 * WARNING: RX_00/RX_01/RX_02 are RX config registers, NOT address registers!
 *   RX_00: RX config (filter fail flags, etc.)
 *   RX_01: RX status (CRC_FAIL, FRAME_TYPE_FAIL, DST/SRC fail bits, etc.)
 *   RX_02: RX_PAN_COORD(bit0), RX_STACK_1ST_EN(bit8), RX_PROMISCUOUS(bit16)
 *===========================================================================*/

#define HAL_M154_PAN_SHORT_ADDR     HAL_M154_RX_03      /* PAN[31:16] + SHORT[15:0] */
#define HAL_M154_EXT_ADDR_L         HAL_M154_RX_04      /* Extended addr bytes 0-3 */
#define HAL_M154_EXT_ADDR_H         HAL_M154_RX_05      /* Extended addr bytes 4-7 */

/* Legacy aliases - DO NOT USE RX_00/01/02 for addresses! */
/* #define HAL_M154_PAN_ID          HAL_M154_RX_00 */  /* WRONG - was clobbering RX config! */
/* #define HAL_M154_SHORT_ADDR      HAL_M154_RX_01 */  /* WRONG */

/*===========================================================================
 * Helper Macros
 *===========================================================================*/

/* Calculate channel frequency (2.4 GHz band) */
#define HAL_CHANNEL_TO_FREQ_MHZ(ch)     (2405 + 5 * ((ch) - 11))

/* Frame buffer size */
#define HAL_MAX_FRAME_SIZE              127
#define HAL_TX_BUFFER_WORDS             32      /* 128 bytes / 4 */
#define HAL_RX_BUFFER_WORDS             32      /* 128 bytes / 4 */

#endif /* __IEEE802154_HAL_REGS_H__ */

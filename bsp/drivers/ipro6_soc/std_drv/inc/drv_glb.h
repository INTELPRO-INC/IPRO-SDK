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


#ifndef __DRV_GLB_H__
#define __DRV_GLB_H__

#include "glb_reg.h"
#include "cci_reg.h"
#include "pds_reg.h"
#include "drv_gpio.h"
#include "drv_hbn.h"
#include "drv_sf_ctrl.h"
#include "drv_sf_cfg.h"
#include "drv_aon.h"
#include "drv_ef_ctrl.h"
#include "drv_pds.h"
#include "drv_common.h"

/* IPRO6 compatibility stubs for unsupported peripherals */
#ifdef IPRO6
#include "ipro6_compat.h"
#endif

#define GLB_CORE_ID_M0          (0)
#define GLB_CORE_ID_MAX         (1)
#define GLB_CORE_ID_INVALID     (2)

typedef enum {
    CCI0_ID,    /*!< CCI0 port define */
    CCI_ID_MAX, /*!< CCI MAX ID define */
} CCI_ID_Type;

typedef enum {
    GLB_MCU_ALL_INT_BMX_ERR_INT = 0,                       /*!< bmx_err_intmcu_dec_err_int */
    GLB_MCU_ALL_INT_BMX_TIMEOUT_INT_MCU_TIMEOUT_INT = 1,   /*!< bmx_timeout_intmcu_timeout_int */
    GLB_MCU_ALL_INT_IPC0_NP2AP_IRQ = 2,                    /*!< ipc0_np2ap_irq */
    GLB_MCU_ALL_INT_IPC0_AP2NP_IRQ = 3,                    /*!< ipc0_ap2np_irq */
    GLB_MCU_ALL_INT_AUDIO_INT = 4,                         /*!< audio_int */
    GLB_MCU_ALL_INT_RF_TOP_INT_0 = 5,                      /*!< rf_top_int[0] */
    GLB_MCU_ALL_INT_RF_TOP_INT_1 = 6,                      /*!< rf_top_int[1] */
    GLB_MCU_ALL_INT_LZ4_INT = 7,                           /*!< lz4_int */
    GLB_MCU_ALL_INT_RSV8 = 8,                              /*!< rsv8 */
    GLB_MCU_ALL_INT_SEC_ENG_ID0_GMAC_INT = 9,              /*!< sec_eng_id0_gmac_int */
    GLB_MCU_ALL_INT_SEC_ENG_ID0_CDET_INT = 10,             /*!< sec_eng_id0_cdet_int */
    GLB_MCU_ALL_INT_SEC_ENG_ID0_PKA_INT = 11,              /*!< sec_eng_id0_pka_int */
    GLB_MCU_ALL_INT_SEC_ENG_ID0_TRNG_INT = 12,             /*!< sec_eng_id0_trng_int */
    GLB_MCU_ALL_INT_SEC_ENG_ID0_AES_INT = 13,              /*!< sec_eng_id0_aes_int */
    GLB_MCU_ALL_INT_SEC_ENG_ID0_SHA_INT = 14,              /*!< sec_eng_id0_sha_int */
    GLB_MCU_ALL_INT_DMA_INTR_ALL = 15,                     /*!< DMA_INTR_ALL */
    GLB_MCU_ALL_INT_DMA2_INTR_ALL = 16,                    /*!< DMA2_INTR_ALL */
    GLB_MCU_ALL_INT_SDH_MMC1_INT_SDH2PMU_WAKEUP_INT1 = 17, /*!< sdh_mmc1_intsdh2pmu_wakeup_int1 */
    GLB_MCU_ALL_INT_MM_IRQ_ALL = 18,                       /*!< mm_irq_all */
    GLB_MCU_ALL_INT_IRTX_INT = 19,                         /*!< irtx_int */
    GLB_MCU_ALL_INT_IRRX_INT = 20,                         /*!< irrx_int */
    GLB_MCU_ALL_INT_USB_INT = 21,                          /*!< usb_int */
    GLB_MCU_ALL_INT_AUPDM_INT = 22,                        /*!< aupdm_int */
    GLB_MCU_ALL_INT_SF_CTRL_ID0_INT = 23,                  /*!< sf_ctrl_id0_int */
    GLB_MCU_ALL_INT_EMAC_INT = 24,                         /*!< emac_int */
    GLB_MCU_ALL_INT_GPADC_DMA_INT = 25,                    /*!< gpadc_dma_int */
    GLB_MCU_ALL_INT_EFUSE_INT = 26,                        /*!< efuse_int */
    GLB_MCU_ALL_INT_SPI_0_INT = 27,                        /*!< spi_0_int */
    GLB_MCU_ALL_INT_UART_IRQ = 28,                         /*!< uart_irq */
    GLB_MCU_ALL_INT_UART1_IRQ = 29,                        /*!< uart1_irq */
    GLB_MCU_ALL_INT_UART2_IRQ = 30,                        /*!< uart2_irq */
    GLB_MCU_ALL_INT_GPIO_DMA_INT = 31,                     /*!< gpio_dma_int */
    GLB_MCU_ALL_INT_I2C_0_INT = 32,                        /*!< i2c_0_int */
    GLB_MCU_ALL_INT_PWM_INT = 33,                          /*!< pwm_int */
    GLB_MCU_ALL_INT_IPC1_NP2AP_IRQ = 34,                   /*!< ipc1_np2ap_irq */
    GLB_MCU_ALL_INT_IPC1_AP2NP_IRQ = 35,                   /*!< ipc1_ap2np_irq */
    GLB_MCU_ALL_INT_TIMER0_2_IRQ = 36,                     /*!< timer0_2_irq */
    GLB_MCU_ALL_INT_TIMER0_3_IRQ = 37,                     /*!< timer0_3_irq */
    GLB_MCU_ALL_INT_WDT0_IRQ = 38,                         /*!< wdt0_irq */
    GLB_MCU_ALL_INT_I2C_1_INT = 39,                        /*!< i2c_1_int */
    GLB_MCU_ALL_INT_I2S0_INT = 40,                         /*!< i2s0_int */
    GLB_MCU_ALL_INT_RSV41 = 41,                            /*!< rsv41 */
    GLB_MCU_ALL_INT_RSV42 = 42,                            /*!< rsv42 */
    GLB_MCU_ALL_INT_ANA_OCP_OUT_TO_CPU_IRQ = 43,           /*!< ana_ocp_out_to_cpu_irq[2:0] */
    GLB_MCU_ALL_INT_GPIO_IRQ = 44,                         /*!< gpio_irq */
    GLB_MCU_ALL_INT_DM_IRQ = 45,                           /*!< dm_irq */
    GLB_MCU_ALL_INT_BT_IRQ = 46,                           /*!< bt_irq */
    GLB_MCU_ALL_INT_M154_REQ_ENH_ACK_INT = 47,             /*!< m154_req_enh_ack_int */
    GLB_MCU_ALL_INT_M154_INT = 48,                         /*!< m154_int */
    GLB_MCU_ALL_INT_M154_AES_INT = 49,                     /*!< m154_aes_int */
    GLB_MCU_ALL_INT_PDS_INT = 50,                          /*!< pds_int */
    GLB_MCU_ALL_INT_HBN_IRQ_OUT_0 = 51,                    /*!< hbn_irq_out[0] */
    GLB_MCU_ALL_INT_HBN_IRQ_OUT_1 = 52,                    /*!< hbn_irq_out[1] */
    GLB_MCU_ALL_INT_BOR_OUT = 53,                          /*!< bor_out */
    GLB_MCU_ALL_INT_WIFI_TO_CPU_IRQ_N = 54,                /*!< wifi_to_cpu_irq_n */
    GLB_MCU_ALL_INT_BZ_PHY_INT = 55,                       /*!< bz_phy_int */
    GLB_MCU_ALL_INT_BLE_INT = 56,                          /*!< ble_int */
    GLB_MCU_ALL_INT_MAC_INT_TX_RX_TIMER = 57,              /*!< mac_int_tx_rx_timer */
    GLB_MCU_ALL_INT_MAC_INT_TX_RX_MISC = 58,               /*!< mac_int_tx_rx_misc */
    GLB_MCU_ALL_INT_MAC_INT_RX_TRIGGER = 59,               /*!< mac_int_rx_trigger */
    GLB_MCU_ALL_INT_MAC_INT_TX_TRIGGER = 60,               /*!< mac_int_tx_trigger */
    GLB_MCU_ALL_INT_MAC_INT_GEN = 61,                      /*!< mac_int_gen */
    GLB_MCU_ALL_INT_MAC_INT_PROT_TRIGGER = 62,             /*!< mac_int_prot_trigger */
    GLB_MCU_ALL_INT_WIFI_IPC = 63,                         /*!< wifi_ipc */
} GLB_MCU_ALL_INT_Type;

#define GLB_MCU_MUXPLL_SEL_WIFIPLL_160M     (0)
#define GLB_MCU_MUXPLL_SEL_AUPLL_DIV3       (1)
#define GLB_MCU_MUXPLL_SEL_TOP_AUPLL_DIV2   (2)
#define GLB_MCU_MUXPLL_SEL_AUPLL_DIV2P5     (3)

#define GLB_MCU_MUXPLL_SEL_WIFIPLL_80M      (0)
#define GLB_MCU_MUXPLL_SEL_TOP_AUPLL_DIV5   (1)
#define GLB_MCU_MUXPLL_SEL_TOP_AUPLL_DIV6   (2)

#define GLB_ISP_MUXPLL_SEL_WIFIPLL_80M      (0)
#define GLB_ISP_MUXPLL_SEL_AUPLL_DIV5       (1)
#define GLB_ISP_MUXPLL_SEL_AUPLL_DIV6       (2)

#define GLB_PLL_CGEN_ISP_WIFIPLL_80M        (2)
#define GLB_PLL_CGEN_ISP_AUPLL_DIV5         (3)
#define GLB_PLL_CGEN_ISP_AUPLL_DIV6         (4)
#define GLB_PLL_CGEN_TOP_AUPLL_DIV5         (5)
#define GLB_PLL_CGEN_TOP_AUPLL_DIV6         (6)
#define GLB_PLL_CGEN_PSRAMB_WIFIPLL_320M    (7)
#define GLB_PLL_CGEN_PSRAMB_AUPLL_DIV1      (8)
#define GLB_PLL_CGEN_TOP_WIFIPLL_240M       (13)
#define GLB_PLL_CGEN_TOP_WIFIPLL_320M       (14)
#define GLB_PLL_CGEN_TOP_AUPLL_DIV2         (15)
#define GLB_PLL_CGEN_TOP_AUPLL_DIV1         (16)

#define GLB_MCU_SYS_CLK_RC32M               (0)
#define GLB_MCU_SYS_CLK_XTAL                (1)
#define GLB_MCU_SYS_CLK_TOP_AUPLL_DIV2      (2)
#define GLB_MCU_SYS_CLK_TOP_AUPLL_DIV1      (3)
#define GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M    (4)
#define GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M    (5)

#define GLB_PLL_REFCLK_XTAL     (0)
#define GLB_PLL_REFCLK_RC32M    (3)

typedef struct {
    uint8_t clkpllRefdivRatio;  /*!< xxx pll_refdiv_ratio */
    uint8_t clkpllIntFracSw;    /*!< xxx pll_int_frac_sw */
    uint8_t clkpllIcp1u;        /*!< xxx pll_icp_1u */
    uint8_t clkpllIcp5u;        /*!< xxx pll_icp_5u */
    uint8_t clkpllRz;           /*!< xxx pll_rz */
    uint8_t clkpllCz;           /*!< xxx pll_cz */
    uint8_t clkpllC3;           /*!< xxx pll_c3 */
    uint8_t clkpllR4Short;      /*!< xxx pll_r4_short */
    uint8_t clkpllC4En;         /*!< xxx pll_r4_en */
    uint8_t clkpllSelSampleClk; /*!< xxx pll_sel_sample_clk */
    uint8_t clkpllVcoSpeed;     /*!< xxx pll_vco_speed */
    uint8_t clkpllSdmCtrlHw;    /*!< xxx pll_sdm_ctrl_hw */
    uint8_t clkpllSdmBypass;    /*!< xxx pll_sdm_bypass */
} GLB_WA_PLL_CFG_BASIC_Type;

typedef struct {
    const GLB_WA_PLL_CFG_BASIC_Type *const basicCfg;  /*!< pll basic configuration */
    uint32_t clkpllSdmin;                             /*!< pll sdmin value */
    uint8_t clkpllPostDiv;                            /*!< aupll aupll_postdiv value */
} GLB_WA_PLL_Cfg_Type;

typedef enum {
    GLB_CAM_CLK_XCLK,           /*!< Select XCLK as CAM clock */
    GLB_CAM_CLK_WIFIPLL_96M,    /*!< Select WIFIPLL_96M as CAM clock */
    GLB_CAM_CLK_TOP_AUPLL_DIV5, /*!< Select TOP_AUPLL_DIV5 as CAM clock */
} GLB_CAM_CLK_Type;


#define GLB_UART_SIG_SWAP_GRP_GPIO0_GPIO11      (0)
#define GLB_UART_SIG_SWAP_GRP_GPIO12_GPIO23     (1)
#define GLB_UART_SIG_SWAP_GRP_GPIO24_GPIO35     (2)
#define GLB_UART_SIG_SWAP_GRP_GPIO36_GPIO45     (3)

typedef enum {
    GLB_I2S_OUT_REF_CLK_NONE, /*!< no output reference clock on I2S_0 ref_clock port */
    GLB_I2S_OUT_REF_CLK_SRC,  /*!< output reference clock on I2S_0 ref_clock port */
} GLB_I2S_OUT_REF_CLK_Type;

#define GLB_DMA0_CLK_CH0    (0)
#define GLB_DMA0_CLK_CH1    (1)
#define GLB_DMA0_CLK_CH2    (2)
#define GLB_DMA0_CLK_CH3    (3)

#define GLB_PERI_DMA_UART0_RX   (0)
#define GLB_PERI_DMA_UART0_TX   (1)
#define GLB_PERI_DMA_UART1_RX   (2)
#define GLB_PERI_DMA_UART1_TX   (3)
#define GLB_PERI_DMA_UART2_RX   (4)
#define GLB_PERI_DMA_UART2_TX   (5)
#define GLB_PERI_DMA_I2C_0_RX   (6)
#define GLB_PERI_DMA_I2C_0_TX   (7)
#define GLB_PERI_DMA_IRTX_TX    (8)
#define GLB_PERI_DMA_GPIO_TX    (9)
#define GLB_PERI_DMA_SPI_RX     (10)
#define GLB_PERI_DMA_SPI_TX     (11)
#define GLB_PERI_DMA_AUDIO_RX   (12)
#define GLB_PERI_DMA_AUDIO_TX   (13)
#define GLB_PERI_DMA_I2C_1_RX   (14)
#define GLB_PERI_DMA_I2C_1_TX   (15)
#define GLB_PERI_DMA_I2S_0_RX   (16)
#define GLB_PERI_DMA_I2S_0_TX   (17)
#define GLB_PERI_DMA_PDM_RX     (18)
#define GLB_PERI_DMA_PADC       (19)
#define GLB_PERI_DMA_GAUGE      (20)
#define GLB_PERI_DMA_GPADC      (22)
#define GLB_PERI_DMA_GPDAC_TX   (23)
#define GLB_PERI_DMA_PEC_RX0    (24)
#define GLB_PERI_DMA_PEC_RX1    (25)
#define GLB_PERI_DMA_PEC_RX2    (26)
#define GLB_PERI_DMA_PEC_RX3    (27)
#define GLB_PERI_DMA_PEC_TX0    (28)
#define GLB_PERI_DMA_PEC_TX1    (29)
#define GLB_PERI_DMA_PEC_TX2    (30)
#define GLB_PERI_DMA_PEC_TX3    (31)

#define GLB_PERI_DMA_CN_SEL_DMA0     (0)

#define GLB_SFLASH_CLK_WIFIPLL_120M     (0)
#define GLB_SFLASH_CLK_XTAL             (1)
#define GLB_SFLASH_CLK_TOP_AUPLL_DIV5   (2)
#define GLB_SFLASH_CLK_MUXPLL_80M       (3)
#define GLB_SFLASH_CLK_BCLK             (4)
#define GLB_SFLASH_CLK_WIFIPLL_96M      (5)

typedef enum {
    GLB_I2C_CLK_BCLK,       /*!< Select bus clk as I2C clock */
    GLB_I2C_CLK_XCLK,       /*!< Select xclk as I2C clock */
} GLB_I2C_CLK_Type;

typedef enum {
    GLB_SPI_CLK_MCU_MUXPLL_160M, /*!< Select MCU MUXPLL 160M as SPI clock */
    GLB_SPI_CLK_XCLK,            /*!< Select xclk as SPI clock */
} GLB_SPI_CLK_Type;


#define GLB_PWM1_IO_SINGLE_END      (0)
#define GLB_PWM1_IO_DIFF_END        (1)

#define GLB_PDM_IO_SEL_AUDIO_TOP    (0)
#define GLB_PDM_IO_SEL_AUPDM_TOP    (1)

typedef enum {
    GLB_DBI_CLK_MCU_MUXPLL_160M, /*!< Select MCU MUXPLL 160M as DBI clock */
    GLB_DBI_CLK_XCLK,            /*!< Select xclk as DBI clock */
} GLB_DBI_CLK_Type;

typedef enum {
    GLB_AUDIO_AUTO_AUPLL,   /*!< Select aupll as audio auto clock */
} GLB_AUDIO_AUTO_CLK_Type;

typedef enum {
    GLB_AUDIO_ADC_AUPLL,    /*!< Select aupll as audio adc clock */
} GLB_AUDIO_ADC_CLK_Type;

typedef enum {
    GLB_AUDIO_SOLO_AUPLL,   /*!< Select aupll as audio solo clock */
} GLB_AUDIO_SOLO_CLK_Type;

#define GLB_SPI_PAD_ACT_AS_SLAVE    (0)
#define GLB_SPI_PAD_ACT_AS_MASTER   (1)

typedef struct {
    uint8_t reset_recorder_ana_powb         : 1; /* reset record bit [0] */
    uint8_t reset_recorder_ext_rst_n        : 1; /* reset record bit [1] */
    uint8_t reset_recorder_pds_reset        : 1; /* reset record bit [2] */
    uint8_t reset_recorder_wdt_rst_n        : 1; /* reset record bit [3] */
    uint8_t reset_recorder_cpu_porst_n      : 1; /* reset record bit [4] */
    uint8_t reset_recorder_sys_reset_n      : 1; /* reset record bit [5] */
    uint8_t reset_recorder_cpu_sys_rstreq_n : 1; /* reset record bit [6] */
    uint8_t reset_recorder_rsvd             : 1; /* reset record bit [7] */
} GLB_RESET_RECORD_Type ;

/* swrst_cfg0 (addr=0x540) bit[0:31] */
#define GLB_AHB_MCU_SW_GLB          (0)
#define GLB_AHB_MCU_SW_RF           (1)
#define GLB_AHB_MCU_SW_GPIP         (2)
#define GLB_AHB_MCU_SW_SEC_ENG      (3)     /* ipro7 compat: SEC_ENG */
#define GLB_AHB_MCU_SW_TZ2          (4)
#define GLB_AHB_MCU_SW_CCI          (5)
#define GLB_AHB_MCU_SW_MCU_MISC     (6)
#define GLB_AHB_MCU_SW_SF           (7)
#define GLB_AHB_MCU_SW_DMA          (8)
#define GLB_AHB_MCU_SW_SDU          (9)
#define GLB_AHB_MCU_SW_UART0        (10)
#define GLB_AHB_MCU_SW_UART1        (11)
#define GLB_AHB_MCU_SW_SPI          (12)
#define GLB_AHB_MCU_SW_I2C0         (13)
#define GLB_AHB_MCU_SW_I2C1         (14)
#define GLB_AHB_MCU_SW_PWM          (15)
#define GLB_AHB_MCU_SW_TIMER        (16)    /* ipro7 compat: TIMER */
#define GLB_AHB_MCU_SW_CHECKSUM     (17)    /* ipro7 compat: CHECKSUM */
#define GLB_AHB_MCU_SW_I2S          (18)    /* ipro7 compat: I2S */
#define GLB_AHB_MCU_SW_USB          (19)
#define GLB_AHB_MCU_SW_USB_PHY      (20)
#define GLB_AHB_MCU_SW_EMAC         (21)
#define GLB_AHB_MCU_SW_PDS          (22)
#define GLB_AHB_MCU_SW_RF2          (23)
#define GLB_AHB_MCU_SW_WIFI         (24)
#define GLB_AHB_MCU_SW_BLE          (25)
#define GLB_AHB_MCU_SW_M154         (26)
#define GLB_AHB_MCU_SW_BLE2         (27)
#define GLB_AHB_MCU_SW_M1542        (28)
#define GLB_AHB_MCU_SW_MM           (29)
#define GLB_AHB_MCU_SW_DBI          (30)
#define GLB_AHB_MCU_SW_QDID         (31)
#define GLB_AHB_MCU_SW_SPI2         (32)
#define GLB_AHB_MCU_SW_PWRON_RST    (64)
#define GLB_AHB_MCU_SW_CPU_RESET    (65)
#define GLB_AHB_MCU_SW_SYS_RESET    (66)
#define GLB_AHB_MCU_SW_CHIP_RESET   (69)

typedef enum {
    GLB_DISRST_GPIP = 2,
    GLB_DISRST_SEC_ENG = 4,
    GLB_DISRST_CCI = 8,
    GLB_DISRST_SF = 11,
    GLB_DISRST_UART0 = 16,
    GLB_DISRST_UART1 = 17,
    GLB_DISRST_SPI = 18,
    GLB_DISRST_I2C0 = 19,
    GLB_DISRST_PWM = 20,
    GLB_DISRST_TIMER = 21,
    GLB_DISRST_IR_REMOTE = 22,
    GLB_DISRST_CHECKSUM = 23,
    GLB_DISRST_DBI = 24,
    GLB_DISRST_I2C1 = 25,
    GLB_DISRST_ISO11898 = 26,
} GLB_DISRST_Type;

#define GLB_PKA_CLK_MCU_BCLK            (0)
#define GLB_PKA_CLK_MCU_MUXPLL_160M     (1)

#define GLB_MCU_SW_SYSTEM_CTRL_MCU  (1)

typedef enum {
    BMX_ARB_ROUND_ROBIN = 0, /*!< 0->round-robin */
    BMX_ARB_FIX = 1,         /*!< 1->fix */
} BMX_ARB_Type;

typedef enum {
    BMX_LATCH_FIRST_ERROR = 0, /*!< 0->Latch first error */
    BMX_LATCH_LAST_ERROR = 1,  /*!< 1->Latch last error */
} BMX_LATCH_Type;

typedef struct {
    BMX_ARB_Type arbMod; /*!< 0->fix, 2->round-robin, 3->random */
    uint8_t timeoutEn;   /*!< Bus timeout enable: detect slave no reaponse in 1024 cycles */
} BMX_TO_Cfg_Type;

typedef enum {
    BMX_BUS_ERR_TRUSTZONE_DECODE, /*!< Bus trustzone decode error */
    BMX_BUS_ERR_ADDR_DECODE,      /*!< Bus addr decode error */
} BMX_BUS_ERR_Type;

typedef enum {
    BMX_ERR_INT_ERR, /*!< BMX bus err interrupt */
    BMX_ERR_INT_ALL, /*!< BMX bus err interrupt max num */
} BMX_ERR_INT_Type;

typedef enum {
    BMX_TO_INT_TIMEOUT, /*!< BMX timeout interrupt */
    BMX_TO_INT_ALL,     /*!< BMX timeout interrupt max num */
} BMX_TO_INT_Type;

#define GLB_ETH_REF_CLK_OUT_OUTSIDE_50M     (0)
#define GLB_ETH_REF_CLK_OUT_INSIDE_50M      (1)

#define GLB_WRAM160KB_EM0KB     (0)
#define GLB_WRAM128KB_EM32KB    (1)
#define GLB_WRAM96KB_EM64KB     (2)

#define GLB_PACKAGE_TYPE_QFN40      (0)
#define GLB_PACKAGE_TYPE_QFN40M     (1)
#define GLB_PACKAGE_TYPE_QFN56      (2)

typedef enum {
    GLB_ADC_CLK_AUPLL, /*!< use AUPLL as ADC clock */
    GLB_ADC_CLK_XCLK,  /*!< use XCLK as ADC clock */
} GLB_ADC_CLK_Type;

typedef enum {
    GLB_DAC_CLK_32M,  /*!< use 32M as DAC clock */
    GLB_DAC_CLK_XCLK, /*!< use XCLK as DAC clock */
} GLB_DAC_CLK_Type;

typedef enum {
    GLB_CHIP_CLK_OUT_0_CAM_REF_CLK = 0,   /*!< cam_ref_clk */
    GLB_CHIP_CLK_OUT_0_I2S_REF_CLK = 1,   /*!< i2s_ref_clk out */
    GLB_CHIP_CLK_OUT_0_NONE = 2,          /*!< no clock out */
    GLB_CHIP_CLK_OUT_0_SOLO_IN_128FS = 3, /*!< clk_solo_in_128fs */
} GLB_CHIP_CLK_OUT_0_Type;

typedef enum {
    GLB_CHIP_CLK_OUT_1_CAM_REF_CLK = 0,   /*!< no chip clock out */
    GLB_CHIP_CLK_OUT_1_I2S_REF_CLK = 1,   /*!< i2s_ref_clk out */
    GLB_CHIP_CLK_OUT_1_NONE = 2,          /*!< no clock out */
    GLB_CHIP_CLK_OUT_1_SOLO_IN_128FS = 3, /*!< clk_solo_in_128fs */
} GLB_CHIP_CLK_OUT_1_Type;

typedef enum {
    GLB_CHIP_CLK_OUT_2_CAM_REF_CLK = 0,  /*!< cam_ref_clk */
    GLB_CHIP_CLK_OUT_2_I2S_REF_CLK = 1,  /*!< i2s_ref_clk */
    GLB_CHIP_CLK_OUT_2_ANA_XTAL_CLK = 2, /*!< ana_xtal_clk */
    GLB_CHIP_CLK_OUT_2_WIFIPLL_32M = 3,  /*!< wifipll_32m_clk */
} GLB_CHIP_CLK_OUT_2_Type;

typedef enum {
    GLB_CHIP_CLK_OUT_3_CAM_REF_CLK = 0, /*!< no chip clock out */
    GLB_CHIP_CLK_OUT_3_I2S_REF_CLK = 1, /*!< i2s_ref_clk out */
    GLB_CHIP_CLK_OUT_3_NONE = 2,        /*!< no clock out */
    GLB_CHIP_CLK_OUT_3_WIFIPLL_48M = 3, /*!< wifipll_48m_clk */
} GLB_CHIP_CLK_OUT_3_Type;

#define GLB_DIG_CLK_WIFIPLL_32M     (0)
#define GLB_DIG_CLK_XCLK            (1)
#define GLB_DIG_CLK_AUPLL           (2)

typedef enum {
    GLB_512K_CLK_OUT_512K, /*!< select 512K clock out */
    GLB_512K_CLK_OUT_256K, /*!< select 256K clock out */
    GLB_512K_CLK_OUT_128K, /*!< select 128K clock out */
} GLB_512K_CLK_OUT_Type;

typedef enum {
    GLB_BT_BANDWIDTH_1M, /*!< BT bandwidth 1MHz */
    GLB_BT_BANDWIDTH_2M, /*!< BT bandwidth 2MHz */
} GLB_BT_BANDWIDTH_Type;

#define GLB_UART_SIG_0      (0)
#define GLB_UART_SIG_1      (1)
#define GLB_UART_SIG_2      (2)
#define GLB_UART_SIG_3      (3)
#define GLB_UART_SIG_4      (4)
#define GLB_UART_SIG_5      (5)
#define GLB_UART_SIG_6      (6)
#define GLB_UART_SIG_7      (7)
#define GLB_UART_SIG_8      (8)
#define GLB_UART_SIG_9      (9)
#define GLB_UART_SIG_10     (10)
#define GLB_UART_SIG_11     (11)

#define GLB_UART_SIG_FUN_UART0_RTS          (0)
#define GLB_UART_SIG_FUN_UART0_CTS          (1)
#define GLB_UART_SIG_FUN_UART0_TXD          (2)
#define GLB_UART_SIG_FUN_UART0_RXD          (3)
#define GLB_UART_SIG_FUN_UART1_RTS          (4)
#define GLB_UART_SIG_FUN_UART1_CTS          (5)
#define GLB_UART_SIG_FUN_UART1_TXD          (6)
#define GLB_UART_SIG_FUN_UART1_RXD          (7)
#define GLB_UART_SIG_FUN_N_ISO11898_TXD     (8)
#define GLB_UART_SIG_FUN_RESERVED           (9)
#define GLB_UART_SIG_FUN_ISO11898_TXD       (10)
#define GLB_UART_SIG_FUN_ISO11898_RXD       (11)

#define GLB_XTAL_NONE   (0)
#define GLB_XTAL_24M    (1)
#define GLB_XTAL_32M    (2)
#define GLB_XTAL_38P4M  (3)
#define GLB_XTAL_40M    (4)
#define GLB_XTAL_26M    (5)
#define GLB_XTAL_RC32M  (6)
#define GLB_XTAL_MAX    (7)

#define GLB_PLL_NONE        (0)
#define GLB_PLL_WIFIPLL     (1)
#define GLB_PLL_AUPLL       (2)

typedef enum {
    GLB_WA_PLL_WIFIPLL, /*!< wifi pll */
    GLB_WA_PLL_AUPLL,   /*!< audio pll */
} GLB_WA_PLL_Type;

typedef enum {
    GLB_PSRAMB_EMI_WIFIPLL_320M, /*!< emi_wifipll_320m_clk */
    GLB_PSRAMB_EMI_AUPLL_DIV1,   /*!< emi_aupll_div1_clk */
} GLB_PSRAMB_PLL_Type;

typedef enum {
    GLB_UART_CLK_BCLK,     /*!< Select bclk clock as UART clock */
    GLB_UART_CLK_PLL_160M, /*!< Select PLL 160M as UART clock */
} GLB_UART_CLK_Type;

#define GLB_I2S_DI_SEL_I2S_DI_INPUT     (0)
#define GLB_I2S_DI_SEL_I2S_REF_OUTPUT   (1)

#define GLB_SPI_SIG_SWAP_GRP_GPIO0_GPIO11   (0)
#define GLB_SPI_SIG_SWAP_GRP_GPIO12_GPIO23  (1)
#define GLB_SPI_SIG_SWAP_GRP_GPIO24_GPIO35  (2)
#define GLB_SPI_SIG_SWAP_GRP_GPIO36_GPIO45  (3)

#define GLB_I2S_DO_SEL_I2S_DO_OUTPT     (0)
#define GLB_I2S_DO_SEL_I2S_REF_OUTPUT   (1)

typedef enum {
    GLB_AHB_CLOCK_IP_CPU,
    GLB_AHB_CLOCK_IP_RSVD,
    GLB_AHB_CLOCK_IP_SEC,
    GLB_AHB_CLOCK_IP_DMA_0,
    GLB_AHB_CLOCK_IP_DMA_1,
    GLB_AHB_CLOCK_IP_DMA_2,
    GLB_AHB_CLOCK_IP_CCI,
    GLB_AHB_CLOCK_IP_RF_TOP,
    GLB_AHB_CLOCK_IP_GPIP,
    GLB_AHB_CLOCK_IP_TZC,
    GLB_AHB_CLOCK_IP_EF_CTRL,
    GLB_AHB_CLOCK_IP_SF_CTRL,
    GLB_AHB_CLOCK_IP_EMAC,
    GLB_AHB_CLOCK_IP_UART0,
    GLB_AHB_CLOCK_IP_UART1,
    GLB_AHB_CLOCK_IP_UART2,
    GLB_AHB_CLOCK_IP_UART3,
    GLB_AHB_CLOCK_IP_UART4,
    GLB_AHB_CLOCK_IP_SPI,
    GLB_AHB_CLOCK_SPI0=GLB_AHB_CLOCK_IP_SPI,
    GLB_AHB_CLOCK_IP_I2C,
    GLB_AHB_CLOCK_IP_PWM,
    GLB_AHB_CLOCK_IP_TIMER,
    GLB_AHB_CLOCK_IP_IR,
    GLB_AHB_CLOCK_IP_CHECKSUM,
    GLB_AHB_CLOCK_IP_QDEC,
    GLB_AHB_CLOCK_IP_KYS,
    GLB_AHB_CLOCK_IP_I2S,
    GLB_AHB_CLOCK_IP_USB11,
    GLB_AHB_CLOCK_IP_CAM,
    GLB_AHB_CLOCK_IP_MJPEG,
    GLB_AHB_CLOCK_IP_BT_BLE_NORMAL,
    GLB_AHB_CLOCK_IP_BT_BLE_LP,
    GLB_AHB_CLOCK_IP_ZB_NORMAL,
    GLB_AHB_CLOCK_IP_ZB_LP,
    GLB_AHB_CLOCK_IP_WIFI_NORMAL,
    GLB_AHB_CLOCK_IP_WIFI_LP,
    GLB_AHB_CLOCK_IP_BT_BLE_2_NORMAL,
    GLB_AHB_CLOCK_IP_BT_BLE_2_LP,
    GLB_AHB_CLOCK_IP_EMI_MISC,
    GLB_AHB_CLOCK_IP_PSRAM0_CTRL,
    GLB_AHB_CLOCK_IP_PSRAM1_CTRL,
    GLB_AHB_CLOCK_IP_USB20_SDU,
    GLB_AHB_CLOCK_IP_MIX2,
    GLB_AHB_CLOCK_IP_AUDIO,
    GLB_AHB_CLOCK_IP_SDH,
    GLB_AHB_CLOCK_IP_ZB2_NORMAL,
    GLB_AHB_CLOCK_IP_ZB2_LP,
    GLB_AHB_CLOCK_IP_I2C1,
    GLB_AHB_CLOCK_IP_WIFI_PHY,
    GLB_AHB_CLOCK_IP_WIFI_MAC_PHY,
    GLB_AHB_CLOCK_IP_WIFI_PLATFORM,
    GLB_AHB_CLOCK_IP_LZ4,
    GLB_AHB_CLOCK_IP_AUPDM,
    GLB_AHB_CLOCK_IP_GAUGE,
    GLB_AHB_CLOCK_IP_DBI,
    GLB_AHB_CLOCK_IP_PEC,
    GLB_AHB_CLOCK_IP_ISO11898,
    GLB_AHB_CLOCK_IP_AUSOLO_TOP,
    GLB_AHB_CLOCK_IP_DMA_GPIO,
    GLB_AHB_CLOCK_IP_MM_MISC,
} GLB_AHB_CLOCK_IP_Type;

typedef enum {
    GLB_SLAVE_GRP_0_ADC,
    GLB_SLAVE_GRP_0_I2C,
    GLB_SLAVE_GRP_0_SPI,
    GLB_SLAVE_GRP_0_DBI,
    GLB_SLAVE_GRP_0_AUDIO_AUTO,
    GLB_SLAVE_GRP_0_AUDIO_ADC,
    GLB_SLAVE_GRP_0_AUDIO_SOLO,
    GLB_SLAVE_GRP_0_CAM,
    GLB_SLAVE_GRP_0_PSRAMB,
    GLB_SLAVE_GRP_0_MAX,
} GLB_SLAVE_GRP_0_Type;

typedef struct {
    uint32_t clkOffSetAddr;
    uint8_t clkEnPos;
    uint8_t clkSelPos;
    uint8_t clkDivPos;
    uint8_t clkEnLen;
    uint8_t clkSelLen;
    uint8_t clkDivLen;
} GLB_SLAVE_GRP_0_TBL_Type;

typedef union {
    GLB_ADC_CLK_Type        adcClkSel;
    GLB_I2C_CLK_Type        i2cClkSel;
    GLB_SPI_CLK_Type        spiClkSel;
    GLB_DBI_CLK_Type        dbiClkSel;
    GLB_AUDIO_AUTO_CLK_Type audioAutoClkSel;
    GLB_AUDIO_ADC_CLK_Type  audioAdcClkSel;
    GLB_AUDIO_SOLO_CLK_Type audioSoloClkSel;
    GLB_CAM_CLK_Type        camClkSel;
    GLB_PSRAMB_PLL_Type     psrambClkSel;
    uint32_t                clkSel;
} GLB_SLAVE_GRP_0_CLK_SEL_Type;

typedef enum {
   GLB_EM_0KB =  0x0,   /*!< 0x0  --> 0KB */
   GLB_EM_16KB = 0x8,   /*!< 0x8 --> 16KB */
   GLB_EM_32KB = 0xC,   /*!< 0xC --> 32KB */
   GLB_EM_48KB = 0xE,   /*!< 0xE --> 48KB */
   GLB_EM_64KB = 0xF,   /*!< 0xF --> 64KB */
} GLB_EM_Type;

/* Unused, only used to sync api with other platform */
typedef enum {
   GLB_EMAC_CAM_PIN_EMAC, /*!< select pin as emac */
   GLB_EMAC_CAM_PIN_CAM,  /*!< select pin as cam */
} GLB_EMAC_CAM_PIN_Type;



#define IS_GLB_CORE_ID_TYPE(type) (((type) == GLB_CORE_ID_M0) ||  \
                                   ((type) == GLB_CORE_ID_MAX) || \
                                   ((type) == GLB_CORE_ID_INVALID))

#define IS_CCI_ID_TYPE(type) (((type) == CCI0_ID) || \
                              ((type) == CCI_ID_MAX))

#define IS_GLB_MCU_ALL_INT_TYPE(type) (((type) == GLB_MCU_ALL_INT_BMX_ERR_INT) ||                      \
                                       ((type) == GLB_MCU_ALL_INT_BMX_TIMEOUT_INT_MCU_TIMEOUT_INT) ||  \
                                       ((type) == GLB_MCU_ALL_INT_IPC0_NP2AP_IRQ) ||                   \
                                       ((type) == GLB_MCU_ALL_INT_IPC0_AP2NP_IRQ) ||                   \
                                       ((type) == GLB_MCU_ALL_INT_AUDIO_INT) ||                        \
                                       ((type) == GLB_MCU_ALL_INT_RF_TOP_INT_0) ||                     \
                                       ((type) == GLB_MCU_ALL_INT_RF_TOP_INT_1) ||                     \
                                       ((type) == GLB_MCU_ALL_INT_LZ4_INT) ||                          \
                                       ((type) == GLB_MCU_ALL_INT_RSV8) ||                             \
                                       ((type) == GLB_MCU_ALL_INT_SEC_ENG_ID0_GMAC_INT) ||             \
                                       ((type) == GLB_MCU_ALL_INT_SEC_ENG_ID0_CDET_INT) ||             \
                                       ((type) == GLB_MCU_ALL_INT_SEC_ENG_ID0_PKA_INT) ||              \
                                       ((type) == GLB_MCU_ALL_INT_SEC_ENG_ID0_TRNG_INT) ||             \
                                       ((type) == GLB_MCU_ALL_INT_SEC_ENG_ID0_AES_INT) ||              \
                                       ((type) == GLB_MCU_ALL_INT_SEC_ENG_ID0_SHA_INT) ||              \
                                       ((type) == GLB_MCU_ALL_INT_DMA_INTR_ALL) ||                     \
                                       ((type) == GLB_MCU_ALL_INT_DMA2_INTR_ALL) ||                    \
                                       ((type) == GLB_MCU_ALL_INT_SDH_MMC1_INT_SDH2PMU_WAKEUP_INT1) || \
                                       ((type) == GLB_MCU_ALL_INT_MM_IRQ_ALL) ||                       \
                                       ((type) == GLB_MCU_ALL_INT_IRTX_INT) ||                         \
                                       ((type) == GLB_MCU_ALL_INT_IRRX_INT) ||                         \
                                       ((type) == GLB_MCU_ALL_INT_USB_INT) ||                          \
                                       ((type) == GLB_MCU_ALL_INT_AUPDM_INT) ||                        \
                                       ((type) == GLB_MCU_ALL_INT_SF_CTRL_ID0_INT) ||                  \
                                       ((type) == GLB_MCU_ALL_INT_EMAC_INT) ||                         \
                                       ((type) == GLB_MCU_ALL_INT_GPADC_DMA_INT) ||                    \
                                       ((type) == GLB_MCU_ALL_INT_EFUSE_INT) ||                        \
                                       ((type) == GLB_MCU_ALL_INT_SPI_0_INT) ||                        \
                                       ((type) == GLB_MCU_ALL_INT_UART_IRQ) ||                         \
                                       ((type) == GLB_MCU_ALL_INT_UART1_IRQ) ||                        \
                                       ((type) == GLB_MCU_ALL_INT_UART2_IRQ) ||                        \
                                       ((type) == GLB_MCU_ALL_INT_GPIO_DMA_INT) ||                     \
                                       ((type) == GLB_MCU_ALL_INT_I2C_0_INT) ||                        \
                                       ((type) == GLB_MCU_ALL_INT_PWM_INT) ||                          \
                                       ((type) == GLB_MCU_ALL_INT_IPC1_NP2AP_IRQ) ||                   \
                                       ((type) == GLB_MCU_ALL_INT_IPC1_AP2NP_IRQ) ||                   \
                                       ((type) == GLB_MCU_ALL_INT_TIMER0_2_IRQ) ||                     \
                                       ((type) == GLB_MCU_ALL_INT_TIMER0_3_IRQ) ||                     \
                                       ((type) == GLB_MCU_ALL_INT_WDT0_IRQ) ||                         \
                                       ((type) == GLB_MCU_ALL_INT_I2C_1_INT) ||                        \
                                       ((type) == GLB_MCU_ALL_INT_I2S0_INT) ||                         \
                                       ((type) == GLB_MCU_ALL_INT_RSV41) ||                            \
                                       ((type) == GLB_MCU_ALL_INT_RSV42) ||                            \
                                       ((type) == GLB_MCU_ALL_INT_ANA_OCP_OUT_TO_CPU_IRQ) ||           \
                                       ((type) == GLB_MCU_ALL_INT_GPIO_IRQ) ||                         \
                                       ((type) == GLB_MCU_ALL_INT_DM_IRQ) ||                           \
                                       ((type) == GLB_MCU_ALL_INT_BT_IRQ) ||                           \
                                       ((type) == GLB_MCU_ALL_INT_M154_REQ_ENH_ACK_INT) ||             \
                                       ((type) == GLB_MCU_ALL_INT_M154_INT) ||                         \
                                       ((type) == GLB_MCU_ALL_INT_M154_AES_INT) ||                     \
                                       ((type) == GLB_MCU_ALL_INT_PDS_INT) ||                          \
                                       ((type) == GLB_MCU_ALL_INT_HBN_IRQ_OUT_0) ||                    \
                                       ((type) == GLB_MCU_ALL_INT_HBN_IRQ_OUT_1) ||                    \
                                       ((type) == GLB_MCU_ALL_INT_BOR_OUT) ||                          \
                                       ((type) == GLB_MCU_ALL_INT_WIFI_TO_CPU_IRQ_N) ||                \
                                       ((type) == GLB_MCU_ALL_INT_BZ_PHY_INT) ||                       \
                                       ((type) == GLB_MCU_ALL_INT_BLE_INT) ||                          \
                                       ((type) == GLB_MCU_ALL_INT_MAC_INT_TX_RX_TIMER) ||              \
                                       ((type) == GLB_MCU_ALL_INT_MAC_INT_TX_RX_MISC) ||               \
                                       ((type) == GLB_MCU_ALL_INT_MAC_INT_RX_TRIGGER) ||               \
                                       ((type) == GLB_MCU_ALL_INT_MAC_INT_TX_TRIGGER) ||               \
                                       ((type) == GLB_MCU_ALL_INT_MAC_INT_GEN) ||                      \
                                       ((type) == GLB_MCU_ALL_INT_MAC_INT_PROT_TRIGGER) ||             \
                                       ((type) == GLB_MCU_ALL_INT_WIFI_IPC))

#define IS_GLB_MCU_MUXPLL_160M_CLK_SEL_TYPE(type) (((type) == GLB_MCU_MUXPLL_SEL_WIFIPLL_160M) ||    \
                                                   ((type) == GLB_MCU_MUXPLL_SEL_AUPLL_DIV3) ||      \
                                                   ((type) == GLB_MCU_MUXPLL_SEL_TOP_AUPLL_DIV2) ||  \
                                                   ((type) == GLB_MCU_MUXPLL_SEL_AUPLL_DIV2P5))

#define IS_GLB_MCU_MUXPLL_80M_CLK_SEL_TYPE(type) (((type) == GLB_MCU_MUXPLL_SEL_WIFIPLL_80M) ||    \
                                                  ((type) == GLB_MCU_MUXPLL_SEL_TOP_AUPLL_DIV5) || \
                                                  ((type) == GLB_MCU_MUXPLL_SEL_TOP_AUPLL_DIV6))

#define IS_GLB_ISP_MUXPLL_80M_CLK_SEL_TYPE(type) (((type) == GLB_ISP_MUXPLL_SEL_WIFIPLL_80M) ||    \
                                                  ((type) == GLB_ISP_MUXPLL_SEL_AUPLL_DIV5) ||     \
                                                  ((type) == GLB_ISP_MUXPLL_SEL_AUPLL_DIV6))

#define IS_GLB_PLL_CGEN_TYPE(type) (((type) == GLB_PLL_CGEN_ISP_WIFIPLL_80M) ||     \
                                    ((type) == GLB_PLL_CGEN_ISP_AUPLL_DIV5) ||      \
                                    ((type) == GLB_PLL_CGEN_ISP_AUPLL_DIV6) ||      \
                                    ((type) == GLB_PLL_CGEN_TOP_AUPLL_DIV5) ||      \
                                    ((type) == GLB_PLL_CGEN_TOP_AUPLL_DIV6) ||      \
                                    ((type) == GLB_PLL_CGEN_PSRAMB_WIFIPLL_320M) || \
                                    ((type) == GLB_PLL_CGEN_PSRAMB_AUPLL_DIV1) ||   \
                                    ((type) == GLB_PLL_CGEN_TOP_WIFIPLL_240M) ||    \
                                    ((type) == GLB_PLL_CGEN_TOP_WIFIPLL_320M) ||    \
                                    ((type) == GLB_PLL_CGEN_TOP_AUPLL_DIV2) ||      \
                                    ((type) == GLB_PLL_CGEN_TOP_AUPLL_DIV1))

#define IS_GLB_MCU_SYS_CLK_TYPE(type) (((type) == GLB_MCU_SYS_CLK_RC32M) ||            \
                                       ((type) == GLB_MCU_SYS_CLK_XTAL) ||             \
                                       ((type) == GLB_MCU_SYS_CLK_TOP_AUPLL_DIV2) ||   \
                                       ((type) == GLB_MCU_SYS_CLK_TOP_AUPLL_DIV1) ||   \
                                       ((type) == GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M) || \
                                       ((type) == GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M))

#define IS_GLB_PLL_REF_CLK_TYPE(type) (((type) == GLB_PLL_REFCLK_XTAL) || \
                                       ((type) == GLB_PLL_REFCLK_RC32M))

#define IS_GLB_WIFIPLL_REF_CLK_TYPE(type) (((type) == GLB_WIFIPLL_REF_XTAL_SOC_CLK) || \
                                           ((type) == GLB_WIFIPLL_REF_XTAL_CLK) ||     \
                                           ((type) == GLB_WIFIPLL_REF_XTAL_LO_CLK) ||  \
                                           ((type) == GLB_WIFIPLL_REF_RC32M_CLK))

#define IS_GLB_AUPLL_REF_CLK_TYPE(type) (((type) == GLB_AUPLL_REF_XTAL_SOC_CLK) || \
                                         ((type) == GLB_AUPLL_REF_RC32M_CLK))

#define IS_GLB_CPUPLL_REF_CLK_TYPE(type) (((type) == GLB_CPUPLL_REF_XTAL_SOC_CLK) || \
                                          ((type) == GLB_CPUPLL_REF_RC32M_CLK))

#define IS_GLB_UHSPLL_REF_CLK_TYPE(type) (((type) == GLB_UHSPLL_REF_XTAL_SOC_CLK) || \
                                          ((type) == GLB_UHSPLL_REF_RC32M_CLK))

#define IS_GLB_CAM_CLK_TYPE(type) (((type) == GLB_CAM_CLK_XCLK) ||         \
                                   ((type) == GLB_CAM_CLK_WIFIPLL_96M) ||  \
                                   ((type) == GLB_CAM_CLK_TOP_AUPLL_DIV5))


#define IS_GLB_UART_SIG_SWAP_GRP_TYPE(type) (((type) == GLB_UART_SIG_SWAP_GRP_GPIO0_GPIO11) ||  \
                                             ((type) == GLB_UART_SIG_SWAP_GRP_GPIO12_GPIO23) || \
                                             ((type) == GLB_UART_SIG_SWAP_GRP_GPIO24_GPIO35) || \
                                             ((type) == GLB_UART_SIG_SWAP_GRP_GPIO36_GPIO45))

#define IS_GLB_I2S_OUT_REF_CLK_TYPE(type) (((type) == GLB_I2S_OUT_REF_CLK_NONE) || \
                                           ((type) == GLB_I2S_OUT_REF_CLK_SRC))

#define IS_GLB_DMA_CLK_ID_TYPE(type) (((type) == GLB_DMA0_CLK_CH0) || \
                                      ((type) == GLB_DMA0_CLK_CH1) || \
                                      ((type) == GLB_DMA0_CLK_CH2) || \
                                      ((type) == GLB_DMA0_CLK_CH3))

#define IS_GLB_PERI_DMA_TYPE(type) (((type) == GLB_PERI_DMA_UART0_RX) || \
                                    ((type) == GLB_PERI_DMA_UART0_TX) ||  \
                                    ((type) == GLB_PERI_DMA_UART1_RX) ||  \
                                    ((type) == GLB_PERI_DMA_UART1_TX) ||  \
                                    ((type) == GLB_PERI_DMA_UART2_RX) ||  \
                                    ((type) == GLB_PERI_DMA_UART2_TX) ||  \
                                    ((type) == GLB_PERI_DMA_I2C_0_RX) ||  \
                                    ((type) == GLB_PERI_DMA_I2C_0_TX) ||  \
                                    ((type) == GLB_PERI_DMA_IRTX_TX) ||   \
                                    ((type) == GLB_PERI_DMA_GPIO_TX) ||   \
                                    ((type) == GLB_PERI_DMA_SPI_RX) ||    \
                                    ((type) == GLB_PERI_DMA_SPI_TX) ||    \
                                    ((type) == GLB_PERI_DMA_AUDIO_RX) ||  \
                                    ((type) == GLB_PERI_DMA_AUDIO_TX) ||  \
                                    ((type) == GLB_PERI_DMA_I2C_1_RX) ||  \
                                    ((type) == GLB_PERI_DMA_I2C_1_TX) ||  \
                                    ((type) == GLB_PERI_DMA_I2S_0_RX) ||  \
                                    ((type) == GLB_PERI_DMA_I2S_0_TX) ||  \
                                    ((type) == GLB_PERI_DMA_PDM_RX) ||    \
                                    ((type) == GLB_PERI_DMA_PADC) ||      \
                                    ((type) == GLB_PERI_DMA_GAUGE) ||     \
                                    ((type) == GLB_PERI_DMA_GPADC) ||     \
                                    ((type) == GLB_PERI_DMA_GPDAC_TX) ||  \
                                    ((type) == GLB_PERI_DMA_PEC_RX0) ||   \
                                    ((type) == GLB_PERI_DMA_PEC_RX1) ||   \
                                    ((type) == GLB_PERI_DMA_PEC_RX2) ||   \
                                    ((type) == GLB_PERI_DMA_PEC_RX3) ||   \
                                    ((type) == GLB_PERI_DMA_PEC_TX0) ||   \
                                    ((type) == GLB_PERI_DMA_PEC_TX1) ||   \
                                    ((type) == GLB_PERI_DMA_PEC_TX2) ||   \
                                    ((type) == GLB_PERI_DMA_PEC_TX3))

#define IS_GLB_PERI_DMA_CN_SEL_TYPE(type) (((type) == GLB_PERI_DMA_CN_SEL_DMA0))

#define IS_GLB_SFLASH_CLK_TYPE(type) (((type) == GLB_SFLASH_CLK_WIFIPLL_120M) ||   \
                                      ((type) == GLB_SFLASH_CLK_XTAL) ||           \
                                      ((type) == GLB_SFLASH_CLK_TOP_AUPLL_DIV5) || \
                                      ((type) == GLB_SFLASH_CLK_MUXPLL_80M) ||     \
                                      ((type) == GLB_SFLASH_CLK_BCLK) ||           \
                                      ((type) == GLB_SFLASH_CLK_WIFIPLL_96M))

#define IS_GLB_I2C_CLK_TYPE(type) (((type) == GLB_I2C_CLK_BCLK) || \
                                   ((type) == GLB_I2C_CLK_XCLK))

#define IS_GLB_SPI_CLK_TYPE(type) (((type) == GLB_SPI_CLK_PLL160M) || \
                                   ((type) == GLB_SPI_CLK_XCLK))


#define IS_GLB_PWM1_IO_SEL_TYPE(type) (((type) == GLB_PWM1_IO_SINGLE_END) || \
                                       ((type) == GLB_PWM1_IO_DIFF_END))

#define IS_GLB_PWM2_IO_SEL_TYPE(type) (((type) == GLB_PWM2_IO_SINGLE_END) || \
                                       ((type) == GLB_PWM2_IO_SINGLE_END_BLDC))

#define IS_GLB_PDM_IO_SEL_TYPE(type) (((type) == GLB_PDM_IO_SEL_AUDIO_TOP) || \
                                      ((type) == GLB_PDM_IO_SEL_AUPDM_TOP))

#define IS_GLB_DBI_CLK_TYPE(type) (((type) == GLB_DBI_CLK_PLL160M) || \
                                   ((type) == GLB_DBI_CLK_XCLK))

#define IS_GLB_AUDIO_AUTO_CLK_TYPE(type) (((type) == GLB_AUDIO_AUTO_AUPLL))

#define IS_GLB_AUDIO_ADC_CLK_TYPE(type) (((type) == GLB_AUDIO_ADC_AUPLL))

#define IS_GLB_AUDIO_SOLO_CLK_TYPE(type) (((type) == GLB_AUDIO_SOLO_AUPLL))

#define IS_GLB_SPI_PAD_ACT_AS_TYPE(type) (((type) == GLB_SPI_PAD_ACT_AS_SLAVE) || \
                                          ((type) == GLB_SPI_PAD_ACT_AS_MASTER))

#define IS_GLB_AHB_MCU_SW_TYPE(type) (((type) == GLB_AHB_MCU_SW_RSV0) ||        \
                                      ((type) == GLB_AHB_MCU_SW_RSV1) ||        \
                                      ((type) == GLB_AHB_MCU_SW_WIFI) ||        \
                                      ((type) == GLB_AHB_MCU_SW_BTDM) ||        \
                                      ((type) == GLB_AHB_MCU_SW_ZIGBEE) ||      \
                                      ((type) == GLB_AHB_MCU_SW_BLE2) ||        \
                                      ((type) == GLB_AHB_MCU_SW_ZIGBEE2) ||     \
                                      ((type) == GLB_AHB_MCU_SW_EMI_MISC) ||    \
                                      ((type) == GLB_AHB_MCU_SW_MM_MISC) ||     \
                                      ((type) == GLB_AHB_MCU_SW_PSRAM1_CTRL) || \
                                      ((type) == GLB_AHB_MCU_SW_EXT_USB) ||     \
                                      ((type) == GLB_AHB_MCU_SW_EXT_AUPWM) ||   \
                                      ((type) == GLB_AHB_MCU_SW_EXT_SDH) ||     \
                                      ((type) == GLB_AHB_MCU_SW_EXT_EMAC) ||    \
                                      ((type) == GLB_AHB_MCU_SW_D2XA) ||        \
                                      ((type) == GLB_AHB_MCU_SW_D2XB) ||        \
                                      ((type) == GLB_AHB_MCU_SW_JENC) ||        \
                                      ((type) == GLB_AHB_MCU_SW_EXT_PEC) ||     \
                                      ((type) == GLB_AHB_MCU_SW_GLB) ||         \
                                      ((type) == GLB_AHB_MCU_SW_MIX) ||         \
                                      ((type) == GLB_AHB_MCU_SW_GPIP) ||        \
                                      ((type) == GLB_AHB_MCU_SW_SEC_DBG) ||     \
                                      ((type) == GLB_AHB_MCU_SW_SEC_ENG) ||     \
                                      ((type) == GLB_AHB_MCU_SW_TZ1) ||         \
                                      ((type) == GLB_AHB_MCU_SW_RSV38) ||       \
                                      ((type) == GLB_AHB_MCU_SW_EFUSE) ||       \
                                      ((type) == GLB_AHB_MCU_SW_RSV40) ||       \
                                      ((type) == GLB_AHB_MCU_SW_PERI) ||        \
                                      ((type) == GLB_AHB_MCU_SW_RSV42) ||       \
                                      ((type) == GLB_AHB_MCU_SW_SF) ||          \
                                      ((type) == GLB_AHB_MCU_SW_DMA) ||         \
                                      ((type) == GLB_AHB_MCU_SW_SDU) ||         \
                                      ((type) == GLB_AHB_MCU_SW_PDS) ||         \
                                      ((type) == GLB_AHB_MCU_SW_RSV47) ||       \
                                      ((type) == GLB_AHB_MCU_SW_UART0) ||       \
                                      ((type) == GLB_AHB_MCU_SW_UART1) ||       \
                                      ((type) == GLB_AHB_MCU_SW_SPI) ||         \
                                      ((type) == GLB_AHB_MCU_SW_I2C0) ||        \
                                      ((type) == GLB_AHB_MCU_SW_PWM) ||         \
                                      ((type) == GLB_AHB_MCU_SW_TIMER) ||       \
                                      ((type) == GLB_AHB_MCU_SW_IR_REMOTE) ||   \
                                      ((type) == GLB_AHB_MCU_SW_CHECKSUM) ||    \
                                      ((type) == GLB_AHB_MCU_SW_DBI) ||         \
                                      ((type) == GLB_AHB_MCU_SW_I2C1) ||        \
                                      ((type) == GLB_AHB_MCU_SW_ISO11898) ||    \
                                      ((type) == GLB_AHB_MCU_SW_I2S) ||         \
                                      ((type) == GLB_AHB_MCU_SW_AUSOLO) ||      \
                                      ((type) == GLB_AHB_MCU_SW_RSV61) ||       \
                                      ((type) == GLB_AHB_MCU_SW_RSV62) ||       \
                                      ((type) == GLB_AHB_MCU_SW_RSV63) ||       \
                                      ((type) == GLB_AHB_MCU_SW_PWRON_RST) ||   \
                                      ((type) == GLB_AHB_MCU_SW_CPU_RESET) ||   \
                                      ((type) == GLB_AHB_MCU_SW_SYS_RESET) ||   \
                                      ((type) == GLB_AHB_MCU_SW_CHIP_RESET))

#define IS_GLB_DISRST_TYPE(type) (((type) == GLB_DISRST_GPIP) ||      \
                                  ((type) == GLB_DISRST_SEC_ENG) ||   \
                                  ((type) == GLB_DISRST_CCI) ||       \
                                  ((type) == GLB_DISRST_SF) ||        \
                                  ((type) == GLB_DISRST_UART0) ||     \
                                  ((type) == GLB_DISRST_UART1) ||     \
                                  ((type) == GLB_DISRST_SPI) ||       \
                                  ((type) == GLB_DISRST_I2C0) ||      \
                                  ((type) == GLB_DISRST_PWM) ||       \
                                  ((type) == GLB_DISRST_TIMER) ||     \
                                  ((type) == GLB_DISRST_IR_REMOTE) || \
                                  ((type) == GLB_DISRST_CHECKSUM) ||  \
                                  ((type) == GLB_DISRST_DBI) ||       \
                                  ((type) == GLB_DISRST_I2C1) ||      \
                                  ((type) == GLB_DISRST_ISO11898))

#define IS_GLB_PKA_CLK_TYPE(type) (((type) == GLB_PKA_CLK_MCU_BCLK) || \
                                   ((type) == GLB_PKA_CLK_MCU_MUXPLL_160M))

#define IS_GLB_MCU_SW_SYSTEM_TYPE(type) (((type) == GLB_MCU_SW_SYSTEM_CTRL_MCU))

#define IS_BMX_ARB_TYPE(type) (((type) == BMX_ARB_ROUND_ROBIN) || \
                               ((type) == BMX_ARB_FIX))

#define IS_BMX_LATCH_TYPE(type) (((type) == BMX_LATCH_FIRST_ERROR) || \
                                 ((type) == BMX_LATCH_LAST_ERROR))

#define IS_BMX_BUS_ERR_TYPE(type) (((type) == BMX_BUS_ERR_TRUSTZONE_DECODE) || \
                                   ((type) == BMX_BUS_ERR_ADDR_DECODE))

#define IS_BMX_ERR_INT_TYPE(type) (((type) == BMX_ERR_INT_ERR) || \
                                   ((type) == BMX_ERR_INT_ALL))

#define IS_BMX_TO_INT_TYPE(type) (((type) == BMX_TO_INT_TIMEOUT) || \
                                  ((type) == BMX_TO_INT_ALL))

#define IS_GLB_ETH_REF_CLK_OUT_TYPE(type) (((type) == GLB_ETH_REF_CLK_OUT_OUTSIDE_50M) || \
                                           ((type) == GLB_ETH_REF_CLK_OUT_INSIDE_50M))

#define IS_GLB_EM_TYPE(type) (((type) == GLB_WRAM160KB_EM0KB) ||  \
                              ((type) == GLB_WRAM128KB_EM32KB) || \
                              ((type) == GLB_WRAM96KB_EM64KB))

#define IS_GLB_ADC_CLK_TYPE(type) (((type) == GLB_ADC_CLK_AUPLL) || \
                                   ((type) == GLB_ADC_CLK_XCLK))

#define IS_GLB_DAC_CLK_TYPE(type) (((type) == GLB_DAC_CLK_32M) || \
                                   ((type) == GLB_DAC_CLK_XCLK))

#define IS_GLB_CHIP_CLK_INOUT_TYPE(type) (((type) == GLB_CHIP_CLK_INOUT_INPUT) || \
                                          ((type) == GLB_CHIP_CLK_INOUT_OUTPUT))

#define IS_GLB_CHIP_CLK_OUT_0_TYPE(type) (((type) == GLB_CHIP_CLK_OUT_0_CAM_REF_CLK) || \
                                          ((type) == GLB_CHIP_CLK_OUT_0_I2S_REF_CLK) || \
                                          ((type) == GLB_CHIP_CLK_OUT_0_NONE) ||        \
                                          ((type) == GLB_CHIP_CLK_OUT_0_SOLO_IN_128FS))

#define IS_GLB_CHIP_CLK_OUT_1_TYPE(type) (((type) == GLB_CHIP_CLK_OUT_1_CAM_REF_CLK) || \
                                          ((type) == GLB_CHIP_CLK_OUT_1_I2S_REF_CLK) || \
                                          ((type) == GLB_CHIP_CLK_OUT_1_NONE) ||        \
                                          ((type) == GLB_CHIP_CLK_OUT_1_SOLO_IN_128FS))

#define IS_GLB_CHIP_CLK_OUT_2_TYPE(type) (((type) == GLB_CHIP_CLK_OUT_2_CAM_REF_CLK) ||  \
                                          ((type) == GLB_CHIP_CLK_OUT_2_I2S_REF_CLK) ||  \
                                          ((type) == GLB_CHIP_CLK_OUT_2_ANA_XTAL_CLK) || \
                                          ((type) == GLB_CHIP_CLK_OUT_2_WIFIPLL_32M))

#define IS_GLB_CHIP_CLK_OUT_3_TYPE(type) (((type) == GLB_CHIP_CLK_OUT_3_CAM_REF_CLK) || \
                                          ((type) == GLB_CHIP_CLK_OUT_3_I2S_REF_CLK) || \
                                          ((type) == GLB_CHIP_CLK_OUT_3_NONE) ||        \
                                          ((type) == GLB_CHIP_CLK_OUT_3_WIFIPLL_48M))

#define IS_GLB_DIG_CLK_TYPE(type) (((type) == GLB_DIG_CLK_WIFIPLL_32M) || \
                                   ((type) == GLB_DIG_CLK_XCLK) ||        \
                                   ((type) == GLB_DIG_CLK_AUPLL))

#define IS_GLB_512K_CLK_OUT_TYPE(type) (((type) == GLB_512K_CLK_OUT_512K) || \
                                        ((type) == GLB_512K_CLK_OUT_256K) || \
                                        ((type) == GLB_512K_CLK_OUT_128K))

#define IS_GLB_BT_BANDWIDTH_TYPE(type) (((type) == GLB_BT_BANDWIDTH_1M) || \
                                        ((type) == GLB_BT_BANDWIDTH_2M))

#define IS_GLB_UART_SIG_TYPE(type) (((type) == GLB_UART_SIG_0) ||  \
                                    ((type) == GLB_UART_SIG_1) ||  \
                                    ((type) == GLB_UART_SIG_2) ||  \
                                    ((type) == GLB_UART_SIG_3) ||  \
                                    ((type) == GLB_UART_SIG_4) ||  \
                                    ((type) == GLB_UART_SIG_5) ||  \
                                    ((type) == GLB_UART_SIG_6) ||  \
                                    ((type) == GLB_UART_SIG_7) ||  \
                                    ((type) == GLB_UART_SIG_8) ||  \
                                    ((type) == GLB_UART_SIG_9) ||  \
                                    ((type) == GLB_UART_SIG_10) || \
                                    ((type) == GLB_UART_SIG_11))

#define IS_GLB_UART_SIG_FUN_TYPE(type) (((type) == GLB_UART_SIG_FUN_UART0_RTS) || \
                                        ((type) == GLB_UART_SIG_FUN_UART0_CTS) || \
                                        ((type) == GLB_UART_SIG_FUN_UART0_TXD) || \
                                        ((type) == GLB_UART_SIG_FUN_UART0_RXD) || \
                                        ((type) == GLB_UART_SIG_FUN_UART1_RTS) || \
                                        ((type) == GLB_UART_SIG_FUN_UART1_CTS) || \
                                        ((type) == GLB_UART_SIG_FUN_UART1_TXD) || \
                                        ((type) == GLB_UART_SIG_FUN_UART1_RXD) || \
                                        ((type) == GLB_UART_SIG_FUN_N_ISO11898_TXD) || \
                                        ((type) == GLB_UART_SIG_FUN_RESERVED)  || \
                                        ((type) == GLB_UART_SIG_FUN_ISO11898_TXD)   || \
                                        ((type) == GLB_UART_SIG_FUN_ISO11898_RXD))

#define IS_GLB_XTAL_TYPE(type) (((type) == GLB_XTAL_NONE) ||  \
                                ((type) == GLB_XTAL_24M) ||   \
                                ((type) == GLB_XTAL_32M) ||   \
                                ((type) == GLB_XTAL_38P4M) || \
                                ((type) == GLB_XTAL_40M) ||   \
                                ((type) == GLB_XTAL_26M) ||   \
                                ((type) == GLB_XTAL_RC32M) || \
                                ((type) == GLB_XTAL_MAX))

#define IS_GLB_PLL_TYPE(type) (((type) == GLB_PLL_NONE) ||    \
                               ((type) == GLB_PLL_WIFIPLL) || \
                               ((type) == GLB_PLL_AUPLL))

#define IS_GLB_WA_PLL_TYPE(type) (((type) == GLB_WA_PLL_WIFIPLL) || \
                                   ((type) == GLB_WA_PLL_AUPLL))

#define IS_GLB_PSRAMB_PLL_TYPE(type) (((type) == GLB_PSRAMB_EMI_WIFIPLL_320M) ||  \
                                      ((type) == GLB_PSRAMB_EMI_AUPLL_DIV1))

#define IS_GLB_UART_CLK_TYPE(type) (((type) == GLB_UART_CLK_BCLK) || \
                                    ((type) == GLB_UART_CLK_PLL_160M))

#define IS_GLB_I2S_DI_REF_CLK_TYPE(type) (((type) == GLB_I2S_DI_SEL_I2S_DI_INPUT) || \
                                          ((type) == GLB_I2S_DI_SEL_I2S_REF_OUTPUT))

#define IS_GLB_SPI_SIG_SWAP_GRP_TYPE(type) (((type) == GLB_SPI_SIG_SWAP_GRP_GPIO0_GPIO11) ||  \
                                            ((type) == GLB_SPI_SIG_SWAP_GRP_GPIO12_GPIO23) || \
                                            ((type) == GLB_SPI_SIG_SWAP_GRP_GPIO24_GPIO35) || \
                                            ((type) == GLB_SPI_SIG_SWAP_GRP_GPIO36_GPIO45))

#define IS_GLB_I2S_DO_REF_CLK_TYPE(type) (((type) == GLB_I2S_DO_SEL_I2S_DO_OUTPT) || \
                                          ((type) == GLB_I2S_DO_SEL_I2S_REF_OUTPUT))

#define IS_GLB_SLAVE_GRP_0_TYPE(type) (((type) == GLB_SLAVE_GRP_0_ADC) || \
                                       ((type) == GLB_SLAVE_GRP_0_I2C) || \
                                       ((type) == GLB_SLAVE_GRP_0_SPI) || \
                                       ((type) == GLB_SLAVE_GRP_0_PEC) || \
                                       ((type) == GLB_SLAVE_GRP_0_DBI) || \
                                       ((type) == GLB_SLAVE_GRP_0_AUDIO_AUTO) || \
                                       ((type) == GLB_SLAVE_GRP_0_AUDIO_ADC) || \
                                       ((type) == GLB_SLAVE_GRP_0_AUDIO_SOLO) || \
                                       ((type) == GLB_SLAVE_GRP_0_CAM) || \
                                       ((type) == GLB_SLAVE_GRP_0_PSRAMB) || \
                                       ((type) == GLB_SLAVE_GRP_0_MAX))

#define UART_SIG_SWAP_NONE          (0x00) /* uart_sig[0:11] -> uart_sig[0:11] */
#define UART_SIG_SWAP_GPIO0_GPIO11  (0x01) /* GPIO0-11  uart_sig[ 0:11] -> uart_sig[ 6:11], uart_sig[ 0: 5] */
#define UART_SIG_SWAP_GPIO12_GPIO23 (0x02) /* GPIO12-23 uart_sig[12:23] -> uart_sig[18:23], uart_sig[12:17] */
#define UART_SIG_SWAP_GPIO24_GPIO35 (0x04) /* GPIO24-35 uart_sig[24:35] -> uart_sig[30:35], uart_sig[24:29] */
#define UART_SIG_SWAP_GPIO36_GPIO47 (0x08) /* GPIO36-47 uart_sig[36:47] -> uart_sig[42:47], uart_sig[36:41] */

#define JTAG_SIG_SWAP_NONE 0x00 /* GPIO0-22  E21_TMS/E21_TDI/E21_TCK/E21_TDO <- E21_TCK/E21_TDO/E21_TMS/E21_TDI */

#define GLB_AHB_CLOCK_CPU               (1ULL<<GLB_AHB_CLOCK_IP_CPU            )
#define GLB_AHB_CLOCK_RSVD              (1ULL<<GLB_AHB_CLOCK_IP_RSVD           )
#define GLB_AHB_CLOCK_SEC               (1ULL<<GLB_AHB_CLOCK_IP_SEC            )
#define GLB_AHB_CLOCK_DMA_0             (1ULL<<GLB_AHB_CLOCK_IP_DMA_0          )
#define GLB_AHB_CLOCK_DMA_1             (1ULL<<GLB_AHB_CLOCK_IP_DMA_1          )
#define GLB_AHB_CLOCK_DMA_2             (1ULL<<GLB_AHB_CLOCK_IP_DMA_2          )
#define GLB_AHB_CLOCK_CCI               (1ULL<<GLB_AHB_CLOCK_IP_CCI            )
#define GLB_AHB_CLOCK_RF_TOP            (1ULL<<GLB_AHB_CLOCK_IP_RF_TOP         )
#define GLB_AHB_CLOCK_GPIP              (1ULL<<GLB_AHB_CLOCK_IP_GPIP           )
#define GLB_AHB_CLOCK_TZC               (1ULL<<GLB_AHB_CLOCK_IP_TZC            )
#define GLB_AHB_CLOCK_EF_CTRL           (1ULL<<GLB_AHB_CLOCK_IP_EF_CTRL        )
#define GLB_AHB_CLOCK_SF_CTRL           (1ULL<<GLB_AHB_CLOCK_IP_SF_CTRL        )
#define GLB_AHB_CLOCK_EMAC              (1ULL<<GLB_AHB_CLOCK_IP_EMAC           )
#define GLB_AHB_CLOCK_UART0             (1ULL<<GLB_AHB_CLOCK_IP_UART0          )
#define GLB_AHB_CLOCK_UART1             (1ULL<<GLB_AHB_CLOCK_IP_UART1          )
#define GLB_AHB_CLOCK_UART2             (1ULL<<GLB_AHB_CLOCK_IP_UART2          )
#define GLB_AHB_CLOCK_UART3             (1ULL<<GLB_AHB_CLOCK_IP_UART3          )
#define GLB_AHB_CLOCK_UART4             (1ULL<<GLB_AHB_CLOCK_IP_UART4          )
#define GLB_AHB_CLOCK_SPI               (1ULL<<GLB_AHB_CLOCK_IP_SPI            )
#define GLB_AHB_CLOCK_I2C               (1ULL<<GLB_AHB_CLOCK_IP_I2C            )
#define GLB_AHB_CLOCK_PWM               (1ULL<<GLB_AHB_CLOCK_IP_PWM            )
#define GLB_AHB_CLOCK_TIMER             (1ULL<<GLB_AHB_CLOCK_IP_TIMER          )
#define GLB_AHB_CLOCK_IR                (1ULL<<GLB_AHB_CLOCK_IP_IR             )
#define GLB_AHB_CLOCK_CHECKSUM          (1ULL<<GLB_AHB_CLOCK_IP_CHECKSUM       )
#define GLB_AHB_CLOCK_QDEC              (1ULL<<GLB_AHB_CLOCK_IP_QDEC           )
#define GLB_AHB_CLOCK_KYS               (1ULL<<GLB_AHB_CLOCK_IP_KYS            )
#define GLB_AHB_CLOCK_I2S               (1ULL<<GLB_AHB_CLOCK_IP_I2S            )
#define GLB_AHB_CLOCK_USB11             (1ULL<<GLB_AHB_CLOCK_IP_USB11          )
#define GLB_AHB_CLOCK_CAM               (1ULL<<GLB_AHB_CLOCK_IP_CAM            )
#define GLB_AHB_CLOCK_MJPEG             (1ULL<<GLB_AHB_CLOCK_IP_MJPEG          )
#define GLB_AHB_CLOCK_BT_BLE_NORMAL     (1ULL<<GLB_AHB_CLOCK_IP_BT_BLE_NORMAL  )
#define GLB_AHB_CLOCK_BT_BLE_LP         (1ULL<<GLB_AHB_CLOCK_IP_BT_BLE_LP      )
#define GLB_AHB_CLOCK_ZB_NORMAL         (1ULL<<GLB_AHB_CLOCK_IP_ZB_NORMAL      )
#define GLB_AHB_CLOCK_ZB_LP             (1ULL<<GLB_AHB_CLOCK_IP_ZB_LP          )
#define GLB_AHB_CLOCK_WIFI_NORMAL       (1ULL<<GLB_AHB_CLOCK_IP_WIFI_NORMAL    )
#define GLB_AHB_CLOCK_WIFI_LP           (1ULL<<GLB_AHB_CLOCK_IP_WIFI_LP        )
#define GLB_AHB_CLOCK_BT_BLE_2_NORMAL   (1ULL<<GLB_AHB_CLOCK_IP_BT_BLE_2_NORMAL)
#define GLB_AHB_CLOCK_BT_BLE_2_LP       (1ULL<<GLB_AHB_CLOCK_IP_BT_BLE_2_LP    )
#define GLB_AHB_CLOCK_EMI_MISC          (1ULL<<GLB_AHB_CLOCK_IP_EMI_MISC       )
#define GLB_AHB_CLOCK_PSRAM0_CTRL       (1ULL<<GLB_AHB_CLOCK_IP_PSRAM0_CTRL    )
#define GLB_AHB_CLOCK_PSRAM1_CTRL       (1ULL<<GLB_AHB_CLOCK_IP_PSRAM1_CTRL    )
#define GLB_AHB_CLOCK_USB20_SDU         (1ULL<<GLB_AHB_CLOCK_IP_USB20_SDU      )
#define GLB_AHB_CLOCK_MIX2              (1ULL<<GLB_AHB_CLOCK_IP_MIX2           )
#define GLB_AHB_CLOCK_AUDIO             (1ULL<<GLB_AHB_CLOCK_IP_AUDIO          )
#define GLB_AHB_CLOCK_SDH               (1ULL<<GLB_AHB_CLOCK_IP_SDH            )
#define GLB_AHB_CLOCK_ZB_2_NORMAL       (1ULL<<GLB_AHB_CLOCK_IP_ZB2_NORMAL     )
#define GLB_AHB_CLOCK_ZB_2_LP           (1ULL<<GLB_AHB_CLOCK_IP_ZB2_LP         )
#define GLB_AHB_CLOCK_I2C1              (1ULL<<GLB_AHB_CLOCK_IP_I2C1           )
#define GLB_AHB_CLOCK_WIFI_PHY          (1ULL<<GLB_AHB_CLOCK_IP_WIFI_PHY       )
#define GLB_AHB_CLOCK_WIFI_MAC_PHY      (1ULL<<GLB_AHB_CLOCK_IP_WIFI_MAC_PHY   )
#define GLB_AHB_CLOCK_WIFI_PLATFORM     (1ULL<<GLB_AHB_CLOCK_IP_WIFI_PLATFORM  )
#define GLB_AHB_CLOCK_LZ4               (1ULL<<GLB_AHB_CLOCK_IP_LZ4            )
#define GLB_AHB_CLOCK_AUPDM             (1ULL<<GLB_AHB_CLOCK_IP_AUPDM          )
#define GLB_AHB_CLOCK_GAUGE             (1ULL<<GLB_AHB_CLOCK_IP_GAUGE          )
#define GLB_AHB_CLOCK_DBI               (1ULL<<GLB_AHB_CLOCK_IP_DBI            )
#define GLB_AHB_CLOCK_PEC               (1ULL<<GLB_AHB_CLOCK_IP_PEC            )
#define GLB_AHB_CLOCK_ISO11898          (1ULL<<GLB_AHB_CLOCK_IP_ISO11898       )
#define GLB_AHB_CLOCK_AUSOLO_TOP        (1ULL<<GLB_AHB_CLOCK_IP_AUSOLO_TOP     )
#define GLB_AHB_CLOCK_DMA_GPIO          (1ULL<<GLB_AHB_CLOCK_IP_DMA_GPIO       )
#define GLB_AHB_CLOCK_MM_MISC           (1ULL<<GLB_AHB_CLOCK_IP_MM_MISC        )

#define GLB_AUTO_CALC_XTAL_FLAG_MASK  0x0000ff00
#define GLB_AUTO_CALC_XTAL_FLAG_VALUE 0x5800

extern const GLB_WA_PLL_Cfg_Type wifiPllCfg_960M[GLB_XTAL_MAX];
extern const GLB_SLAVE_GRP_0_TBL_Type glb_slave_grp_0_table[GLB_SLAVE_GRP_0_MAX];

/*----------*/
#ifndef IPRO_USE_HAL_DRIVER
void BMX_ERR_IRQHandler(void);
void BMX_TO_IRQHandler(void);
#endif
/*----------*/
int GLB_Set_MCU_Muxpll_160M_Sel(uint8_t clkSel);
uint8_t GLB_Get_MCU_Muxpll_160M_Sel(void);
int GLB_Set_MCU_Muxpll_80M_Sel(uint8_t clkSel);
uint8_t GLB_Get_MCU_Muxpll_80M_Sel(void);
int GLB_Set_ISP_Muxpll_80M_Sel(uint8_t clkSel);
uint8_t GLB_Get_ISP_Muxpll_80M_Sel(void);
/*----------*/
int GLB_Power_On_XTAL_And_PLL_CLK(uint8_t xtalType, uint8_t pllPuType);
int GLB_Power_Off_WIFIPLL(void);
int GLB_WIFIPLL_Ref_Clk_Sel(uint8_t refClk);
int GLB_Power_On_WIFIPLL(const GLB_WA_PLL_Cfg_Type *const cfg, uint8_t waitStable);
int GLB_Set_USB_CLK_From_WIFIPLL(uint8_t enable);
int GLB_Set_SSC_CLK_From_WIFIPLL(uint8_t enable);
/*----------*/
int GLB_Set_MCU_System_CLK_Div(uint8_t mcuClkDiv, uint8_t mcuPBclkDiv);
int GLB_Get_MCU_System_CLK_Div(uint8_t *mcuClkDiv, uint8_t *mcuPBclkDiv);
int GLB_Set_MCU_System_CLK(uint8_t clkFreq);
/*----------*/
uint8_t GLB_Get_Core_Type(void);
int GLB_Set_CPU_Reset_Address(uint8_t coreID, uint32_t addr);
/*----------*/
int GLB_Set_MCU_MTimer_CLK(uint8_t enable, uint16_t div, uint8_t rst);
int GLB_Set_ADC_CLK(uint8_t enable, GLB_ADC_CLK_Type clkSel, uint8_t div);
int GLB_Set_DMA_CLK(uint8_t enable, uint8_t clk);
int GLB_Set_Peripheral_DMA_CN(uint8_t peri, uint8_t cn);
int GLB_IR_RX_GPIO_Sel(uint8_t gpio);
int GLB_Set_UART_CLK(uint8_t enable, uint8_t clkSel, uint8_t div);
int GLB_UART_Fun_Sel(uint8_t sig, uint8_t fun);
uint8_t GLB_UART_Get_Fun(uint8_t sig);
uint8_t GLB_UART_Find_Sig_By_Fun(uint8_t fun);
int GLB_UART_Fun_Sel_Swap(uint8_t sig, uint8_t fun);
int GLB_Set_SF_CLK(uint8_t enable, uint8_t clkSel, uint8_t div);
int GLB_Set_I2C_CLK(uint8_t enable, GLB_I2C_CLK_Type clkSel, uint8_t div);
int GLB_Set_I2S_CLK(uint8_t refClkEn, uint8_t refClkDiv, uint8_t inRef, uint8_t outRef);
int GLB_Set_SPI_CLK(uint8_t enable, GLB_SPI_CLK_Type clkSel, uint8_t div);
int GLB_SPI_Sig_Swap_Set(uint8_t group, uint8_t swap);
int GLB_Set_PWM1_IO_Sel(uint8_t ioSel);
int GLB_Set_PDM_IO_Sel(uint8_t ioSel);
int GLB_Set_DBI_CLK(uint8_t enable, GLB_DBI_CLK_Type clkSel, uint8_t div);
/*----------*/
int GLB_Set_DIG_CLK_Sel(uint8_t clkSel);
int GLB_Set_DIG_512K_CLK(uint8_t enable, uint8_t compensationEn, uint8_t div);
int GLB_Set_DIG_32K_CLK(uint8_t enable, uint8_t compensationEn, uint16_t div);
int GLB_Platform_Wakeup_PDS_Enable(uint8_t enable);
/*----------*/
int GLB_Sel_MCU_TMR_GPIO_Clock(uint8_t gpioPin);
int GLB_Set_Chip_Clock_Out0_Sel(GLB_CHIP_CLK_OUT_0_Type clkOutType);
int GLB_Set_Chip_Clock_Out1_Sel(GLB_CHIP_CLK_OUT_1_Type clkOutType);
int GLB_Set_Chip_Clock_Out2_Sel(GLB_CHIP_CLK_OUT_2_Type clkOutType);
int GLB_Set_Chip_Clock_Out3_Sel(GLB_CHIP_CLK_OUT_3_Type clkOutType);

/*----------*/
int GLB_BMX_TO_Init(BMX_TO_Cfg_Type *BmxCfg);
uint8_t GLB_Get_BMX_TO_Status(void);
int GLB_Clr_BMX_TO_Status(void);
/*----------*/
int GLB_Set_Audio_AUTO_CLK(uint8_t divEn);
int GLB_Set_Audio_ADC_CLK(uint8_t enable, uint8_t div);
int GLB_Set_Audio_SOLO_CLK(uint8_t enable, uint8_t div);
/*----------*/
int GLB_Invert_ETH_RX_CLK(uint8_t enable);
int GLB_Invert_ETH_TX_CLK(uint8_t enable);
int GLB_Invert_ETH_REF_O_CLK(uint8_t enable);
int GLB_Set_ETH_REF_O_CLK_Sel(uint8_t clkSel);
/*----------*/
int GLB_Set_CAM_CLK(uint8_t enable, GLB_CAM_CLK_Type clkSel, uint8_t div);
/*----------*/
int GLB_Config_SDIO_Host_Reset_System(uint8_t enable);
int GLB_Config_SDIO_Host_Interrupt_CPU(uint8_t enable);
/*----------*/
int GLB_UART_Sig_Swap_Set(uint8_t group, uint8_t swap);
int GLB_Swap_SPI_0_MOSI_With_MISO(int newState);
int GLB_Set_SPI_0_ACT_MOD_Sel(uint8_t mod);
int GLB_Set_SFlash_IO_PARM(uint8_t selEmbedded, uint8_t swapIo3Io0, uint8_t swapIo2Cs);
int GLB_Set_SFlash2_IO_PARM(uint8_t swapIo3Io0);
int GLB_Set_SPI2DVP_State(uint8_t enable);
/*----------*/
int GLB_Clr_Reset_Reason(void);
int GLB_Get_Reset_Reason(GLB_RESET_RECORD_Type *reason);
/*----------*/
int GLB_AHB_MCU_Software_Reset(uint8_t swrst);
int GLB_PER_Clock_Gate(uint64_t ips);
int GLB_PER_Clock_UnGate(uint64_t ips);
int GLB_PLL_CGEN_Clock_Gate(uint8_t clk);
int GLB_PLL_CGEN_Clock_UnGate(uint8_t clk);
int GLB_Set_PKA_CLK_Sel(uint8_t clkSel);
int GLB_MCU_SW_System_Reset(uint8_t sysPart);
int GLB_SW_System_Reset(void);
int GLB_SW_CPU_Reset(void);
int GLB_SW_POR_Reset(void);
int GLB_Disrst_Set(uint8_t enable, GLB_DISRST_Type disrst);
/*----------*/
int GLB_Set_Auto_Calc_Xtal_Type(uint8_t calcXtalType);
int GLB_Get_Auto_Calc_Xtal_Type(uint8_t *calcXtalType);
int GLB_Set_Flash_Id_Value(uint32_t idValue);
uint32_t GLB_Get_Flash_Id_Value(void);
/*----------*/
int GLB_Trim_Ldo18ioVoutSel(void);
int GLB_Trim_Ldo18ioBypass(void);
int GLB_Trim_Ldo18ioVoutTrim(void);
void GLB_Power_Down_Ldo18ioVout(void);
/*----------*/
int GLB_Set_SRAM_RET(uint32_t value);
uint32_t GLB_Get_SRAM_RET(void);
int GLB_Set_SRAM_SLP(uint32_t value);
uint32_t GLB_Get_SRAM_SLP(void);
int GLB_Set_SRAM_PARM(uint32_t value);
uint32_t GLB_Get_SRAM_PARM(void);
int GLB_Set_EM_Sel(uint8_t emType);
int GLB_Set_PSRAMB_CLK_Sel(uint8_t enable, GLB_PSRAMB_PLL_Type clkSel, uint8_t div);
/*----------*/
int GLB_Set_Slave_Grp_0_CLK(GLB_SLAVE_GRP_0_Type slave, uint8_t enable, GLB_SLAVE_GRP_0_CLK_SEL_Type clkSel, uint32_t div);
/*----------*/
int GLB_Config_WIFI_PLL(uint8_t xtalType, const GLB_WA_PLL_Cfg_Type * pllCfgList);
/*----------*/
int GLB_Simple_Set_MCU_System_CLK(uint8_t clkFreq, uint8_t mcuClkDiv, uint8_t mcuPBclkDiv);
/*----------*/
void glb_40M_delay_us(uint32_t us);
int GLB_Fast_Power_On_WIFIPLL(const GLB_WA_PLL_Cfg_Type *const cfg, uint8_t waitStable);
int GLB_Fast_Power_On_XTAL_40M_And_WIFIPLL(void);
int GLB_Fast_Set_MCU_System_CLK_Div(uint8_t mcuClkDiv, uint8_t mcuPBclkDiv);
int GLB_Fast_Set_MCU_System_CLK(uint8_t clkFreq);
/*----------*/
uint8_t GLB_Get_Package_Type(void);
int GLB_Get_PAD_Bonging_to_GND_Sts(void);
/*----------*/
int GLB_SWAP_EMAC_CAM_Pin(GLB_EMAC_CAM_PIN_Type pinType); /* Unused, only used to sync api with other platform */

/*=============================================================================
 * IPRO7 API Compatibility Aliases
 * Map IPRO7 naming conventions to IPRO6LE equivalents for driver compatibility
 *===========================================================================*/

/* Clock type aliases */
#ifndef GLB_AHB_CLOCK_SEC_ENG
#define GLB_AHB_CLOCK_SEC_ENG       GLB_AHB_CLOCK_SEC       /* SEC_ENG → SEC */
#endif
#ifndef GLB_AHB_CLOCK_USB
#define GLB_AHB_CLOCK_USB           GLB_AHB_CLOCK_USB11     /* USB → USB11 */
#endif
#ifndef GLB_AHB_CLOCK_PKA
#define GLB_AHB_CLOCK_PKA           0ULL                    /* PKA not present in IPRO6LE */
#endif

/* Reset type aliases */
#ifndef GLB_AHB_MCU_SW_USB
#define GLB_AHB_MCU_SW_USB          GLB_AHB_MCU_SW_EXT_USB  /* USB → EXT_USB */
#endif
#ifndef GLB_AHB_MCU_SW_EMAC
#define GLB_AHB_MCU_SW_EMAC         GLB_AHB_MCU_SW_EXT_EMAC /* EMAC → EXT_EMAC */
#endif

#endif /* __DRV_GLB_H__ */

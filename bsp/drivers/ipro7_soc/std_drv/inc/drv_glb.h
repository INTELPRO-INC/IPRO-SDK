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

#include "cci_reg.h"
#include "glb_reg.h"
#include "pds_reg.h"
#include "mcu_misc_reg.h"
#include "drv_gpio.h"
#include "drv_l1c.h"
#include "drv_hbn.h"
#include "drv_sf_ctrl.h"
#include "drv_sf_cfg.h"
#include "drv_aon.h"
#include "drv_ef_ctrl.h"
#include "drv_pds.h"
#include "drv_common.h"

typedef enum {
    GLB_ROOT_CLK_RC32M, /*!< root clock select RC32M */
    GLB_ROOT_CLK_XTAL,  /*!< root clock select XTAL */
    GLB_ROOT_CLK_DLL,   /*!< root clock select PLL others */
} GLB_ROOT_CLK_Type;

typedef enum {
    GLB_SYS_CLK_RC32M,    /*!< use RC32M as system clock frequency */
    GLB_SYS_CLK_XTAL,     /*!< use XTAL as system clock */
    GLB_SYS_CLK_DLL25P6M, /*!< use DLL output 25.6M as system clock */
    GLB_SYS_CLK_PLL160M,  /*!< use PLL output 160M as system clock */
    GLB_SYS_CLK_PLL192M,  /*!< use PLL output 192M as system clock */
    GLB_SYS_CLK_DLL128M,  /*!< use DLL output 128M as system clock */
} GLB_SYS_CLK_Type;

typedef enum {
    GLB_CAM_CLK_XCLK,   /*!< Select XCLK as CAM clock */
    GLB_CAM_CLK_DLL42P66M, /*!< Select DLL_DIV3 as CAM clock */
    GLB_CAM_CLK_PLL48M, /*!< Select PLL_DIV10 as CAM clock */
    GLB_CAM_CLK_CLKPLL_POSTDIV_OUTCLK, /*!< Select CLKPLL_POSTDIV_OUTCLK as CAM clock */
} GLB_CAM_CLK_Type;

typedef enum {
    GLB_I2S_OUT_REF_CLK_NONE, /*!< no output reference clock on I2S_0 ref_clock port */
    GLB_I2S_OUT_REF_CLK_SRC,  /*!< output reference clock on I2S_0 ref_clock port */
} GLB_I2S_OUT_REF_CLK_Type;

typedef enum {
    GLB_I2S_REF_CLK_SRC_2P032M,             /*!< 2.032MHz (128/63 from DLL) */
    GLB_I2S_REF_CLK_SRC_6P095M,             /*!< 6.095MHz (128/21 from DLL) */
    GLB_I2S_REF_CLK_SRC_48M,                /*!< pll_48m_clk */
    GLB_I2S_REF_CLK_SRC_CLKPLL_POSTDIV,     /*!< clkpll_postdiv_outclk */
} GLB_I2S_REF_CLK_SRC_Type;

typedef enum {
    GLB_QDEC_CLK_XCLK, /*!< Select XCLK as QDEC clock */
    GLB_QDEC_CLK_F32K, /*!< Select f32k as QDEC clock (PDS mode) */
} GLB_QDEC_CLK_Type;

typedef enum {
    GLB_SPI_CLK_BCLK, /*!< Select BCLK as SPI clock */
    GLB_SPI_CLK_XCLK, /*!< Select XCLK as SPI clock */
} GLB_SPI_CLK_Type;

typedef enum {
    GLB_DMA_CLK_DMA0_CH0, /*!< DMA clock ID:channel 0 */
    GLB_DMA_CLK_DMA0_CH1, /*!< DMA clock ID:channel 1 */
    GLB_DMA_CLK_DMA0_CH2, /*!< DMA clock ID:channel 2 */
    GLB_DMA_CLK_DMA0_CH3, /*!< DMA clock ID:channel 3 */
    GLB_DMA_CLK_DMA0_CH4, /*!< DMA clock ID:channel 4 */
    GLB_DMA_CLK_DMA0_CH5, /*!< DMA clock ID:channel 5 */
    GLB_DMA_CLK_DMA0_CH6, /*!< DMA clock ID:channel 6 */
    GLB_DMA_CLK_DMA0_CH7, /*!< DMA clock ID:channel 7 */
} GLB_DMA_CLK_ID_Type;

typedef enum {
    GLB_IR_CLK_SRC_XCLK, /*!< IR clock source select XCLK */
} GLB_IR_CLK_SRC_Type;

typedef enum {
    GLB_AUDIO_CLK_SRC_2P032M, /*!< Select 2.032MHz as AUDIO clock source */
    GLB_AUDIO_CLK_SRC_6P095M, /*!< Select 6.095MHz as AUDIO clock source */
    GLB_AUDIO_CLK_SRC_48M, /*!< Select 6.095MHz as AUDIO clock source */
    GLB_AUDIO_CLK_SRC_CLKPLL_POSTDIV_OUTCLK,    /*!< Select CLKPLL_POSTDIV_OUTCLK as AUDIO clock source */

} GLB_AUDIO_CLK_SRC_Type;

typedef enum {
    GLB_SFLASH_CLK_XCLK,  /*!< Select XCLK as flash clock */
    GLB_SFLASH_CLK_64M, /*!< Select 64M as flash clock */
    GLB_SFLASH_CLK_BCLK,  /*!< Select BCLK as flash clock */
    GLB_SFLASH_CLK_80M,   /*!< Select 80M as flash clock */
} GLB_SFLASH_CLK_Type;

typedef enum {
    GLB_CHIP_CLK_OUT_NONE,          /*!< no chip clock out */
    GLB_CHIP_CLK_OUT_I2S_REF_CLK,   /*!< i2s_ref_clk out */
    GLB_CHIP_CLK_OUT_PLL_48M_CLK, /*!< pll_48m_clk out */
    GLB_CHIP_CLK_OUT_CLKPLL_POSTDIV_OUTCLK,  /*!<  clkpll_postdiv_outclk */
} GLB_CHIP_CLK_OUT_Type;

typedef enum {
    GLB_ETH_REF_CLK_OUT_OUTSIDE_50M, /*!< select outside 50MHz RMII ref clock */
    GLB_ETH_REF_CLK_OUT_INSIDE_50M,  /*!< select inside 50MHz RMII ref clock */
} GLB_ETH_REF_CLK_OUT_Type;

typedef enum {
    GLB_SPI_PAD_ACT_AS_SLAVE,  /*!< SPI pad act as slave */
    GLB_SPI_PAD_ACT_AS_MASTER, /*!< SPI pad act as master */
} GLB_SPI_PAD_ACT_AS_Type;

typedef enum {
    GLB_PKA_CLK_PLL96M,   /*!< Select PLL 96M as PKA clock */
    GLB_PKA_CLK_DLL128M,  /*!< Select DLL 128M as PKA clock */
    GLB_PKA_CLK_DLL64M,   /*!< Select DLL 64M as PKA clock */
    GLB_PKA_CLK_DLL42P6M, /*!< Select DLL 42.6M as PKA clock */
} GLB_PKA_CLK_Type;

typedef enum {
    BMX_ARB_FIX,         /*!< 0->fix */
    BMX_ARB_ROUND_ROBIN, /*!< 2->round-robin */
    BMX_ARB_RANDOM,      /*!< 3->random */
} BMX_ARB_Type;

typedef struct {
    uint8_t timeoutEn;   /*!< Bus timeout enable: detect slave no reaponse in 1024 cycles */
    int errEn;   /*!< Bus error response enable */
    BMX_ARB_Type arbMod; /*!< 0->fix, 2->round-robin, 3->random */
} BMX_Cfg_Type;

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

typedef enum {
    GLB_EM_0KB =  0x0,   /*!< 0x0  --> 0KB */
    GLB_EM_16KB = 0x8,   /*!< 0x8 --> 16KB */
    GLB_EM_32KB = 0xC,   /*!< 0xC --> 32KB */
    GLB_EM_48KB = 0xE,   /*!< 0xE --> 48KB */
    GLB_EM_64KB = 0xF,   /*!< 0xF --> 64KB */
} GLB_EM_Type;

typedef enum {
    GLB_EMAC_CAM_PIN_EMAC, /*!< select pin as emac */
    GLB_EMAC_CAM_PIN_CAM,  /*!< select pin as cam */
} GLB_EMAC_CAM_PIN_Type;

typedef enum {
    GLB_MTIMER_CLK_XCLK, /*!< Xtal CLK */
    GLB_MTIMER_CLK_32K,  /*!< 32KHz */
} GLB_MTIMER_CLK_Type;

typedef enum {
    GLB_ADC_CLK_F32K,      /*!< use F32K as ADC clock */
    GLB_ADC_CLK_XCLK,      /*!< use XCLK as ADC clock */
} GLB_ADC_CLK_Type;

typedef enum {
    GLB_DAC_CLK_XCLK_32M,   /*!< select XCLK as DAC clock source */
    GLB_DAC_CLK_RC32M,      /*!< select PLL 32M as DAC clock source */
} GLB_DAC_CLK_Type;

typedef enum {
    GLB_DIG_CLK_PLL_32M,   /*!< select PLL 32M as DIG clock source */
    GLB_DIG_CLK_XCLK,      /*!< select XCLK as DIG clock source */
    GLB_DIG_CLK_AUDIO_PLL, /*!< select Audio PLL as DIG clock source */
} GLB_DIG_CLK_Type;

typedef enum {
    GLB_PSRAMB_BCLK = 0,
    GLB_PSRAMB_PLL_160M,
    GLB_PSRAMB_PLL_480M,
    GLB_PSRAMB_PLL_192M,
} GLB_PSRAMB_PLL_Type;

typedef enum {
    GLB_BT_BANDWIDTH_1M, /*!< BT bandwidth 1MHz */
    GLB_BT_BANDWIDTH_2M, /*!< BT bandwidth 2MHz */
} GLB_BT_BANDWIDTH_Type;

typedef enum {
    GLB_UART_SIG_0,  /*!< UART signal 0 */
    GLB_UART_SIG_1,  /*!< UART signal 1 */
    GLB_UART_SIG_2,  /*!< UART signal 2 */
    GLB_UART_SIG_3,  /*!< UART signal 3 */
    GLB_UART_SIG_4,  /*!< UART signal 4 */
    GLB_UART_SIG_5,  /*!< UART signal 5 */
    GLB_UART_SIG_6,  /*!< UART signal 6 */
    GLB_UART_SIG_7,  /*!< UART signal 7 */
    GLB_UART_SIG_8,  /*!< UART signal 8 */
    GLB_UART_SIG_9,  /*!< UART signal 9 */
    GLB_UART_SIG_10, /*!< UART signal 10 */
    GLB_UART_SIG_11, /*!< UART signal 11 */

} GLB_UART_SIG_Type;

typedef enum {
    GLB_UART_SIG_FUN_UART0_RTS, /*!< UART funtion: UART 0 RTS */
    GLB_UART_SIG_FUN_UART0_CTS, /*!< UART funtion: UART 0 CTS */
    GLB_UART_SIG_FUN_UART0_TXD, /*!< UART funtion: UART 0 TXD */
    GLB_UART_SIG_FUN_UART0_RXD, /*!< UART funtion: UART 0 RXD */
    GLB_UART_SIG_FUN_UART1_RTS, /*!< UART funtion: UART 1 RTS */
    GLB_UART_SIG_FUN_UART1_CTS, /*!< UART funtion: UART 1 CTS */
    GLB_UART_SIG_FUN_UART1_TXD, /*!< UART funtion: UART 1 TXD */
    GLB_UART_SIG_FUN_UART1_RXD, /*!< UART funtion: UART 1 RXD */
} GLB_UART_SIG_FUN_Type;

typedef enum {
    GLB_DLL_CLK_RF,     /*!< DLL output div1 RF clock */
    GLB_DLL_CLK_2P032M, /*!< DLL output 2.032MHz clock */
    GLB_DLL_CLK_6P095M, /*!< DLL output 6.095MHz clock */
    GLB_DLL_CLK_25P6M,  /*!< DLL output 25.6MHz clock */
    GLB_DLL_CLK_42P67M, /*!< DLL output 42.67MHz clock */
    GLB_DLL_CLK_64M,    /*!< DLL output 64MHz clock */
    GLB_DLL_CLK_128M,   /*!< DLL output 128MHz clock */
} GLB_DLL_CLK_Type;

typedef enum {
    GLB_GPIO_INT_TRIG_NEG_PULSE, /*!< GPIO negedge pulse trigger interrupt */
    GLB_GPIO_INT_TRIG_POS_PULSE, /*!< GPIO posedge pulse trigger interrupt */
    GLB_GPIO_INT_TRIG_NEG_LEVEL, /*!< GPIO negedge level trigger interrupt (32k 3T) */
    GLB_GPIO_INT_TRIG_POS_LEVEL, /*!< GPIO posedge level trigger interrupt (32k 3T) */
} GLB_GPIO_INT_TRIG_Type;

typedef enum {
    GLB_GPIO_INT_CONTROL_SYNC,  /*!< GPIO interrupt sync mode */
    GLB_GPIO_INT_CONTROL_ASYNC, /*!< GPIO interrupt async mode */
} GLB_GPIO_INT_CONTROL_Type;

typedef enum {
    GLB_DLL_XTAL_NONE,  /*!< XTAL is none */
    GLB_DLL_XTAL_32M,   /*!< XTAL is 32M */
    GLB_DLL_XTAL_RC32M, /*!< XTAL is RC32M */
} GLB_DLL_XTAL_Type;

typedef enum {
    GLB_SDH_CLK_DLL_64M,
    GLB_SDH_CLK_PLL_96M,    /*!< Select WIFIPLL_96M as SDH clock */
} GLB_SDH_CLK_Type;

typedef enum {
    GLB_LOCK_REQ_SF = 0,
    GLB_LOCK_REQ_OPI
} GLB_LOCK_REQ_Type;

typedef enum {
    GLB_PLL_XTAL_NONE,  /*!< XTAL is none */
    GLB_PLL_XTAL_32M,   /*!< XTAL is 32M */
    GLB_PLL_XTAL_RC32M, /*!< XTAL is RC32M */
} GLB_PLL_XTAL_Type;

typedef enum {
    GLB_PLL_FREQ_480M, /*!< PLL VCO frequency 480MHz, SDMIN=0x780000 */
    GLB_PLL_FREQ_400M, /*!< PLL VCO frequency 400MHz, SDMIN=0x640000 */
} GLB_PLL_FREQ_Type;

typedef enum {
    GLB_PLL_CLK_480M, /*!< PLL output clock:480M */
    GLB_PLL_CLK_240M, /*!< PLL output clock:240M */
    GLB_PLL_CLK_192M, /*!< PLL output clock:192M */
    GLB_PLL_CLK_160M, /*!< PLL output clock:160M */
    GLB_PLL_CLK_120M, /*!< PLL output clock:120M */
    GLB_PLL_CLK_96M,  /*!< PLL output clock:96M */
    GLB_PLL_CLK_80M,  /*!< PLL output clock:80M */
    GLB_PLL_CLK_48M,  /*!< PLL output clock:48M */
    GLB_PLL_CLK_32M,  /*!< PLL output clock:32M */
} GLB_PLL_CLK_Type;

typedef enum {
    AUDIO_PLL_12288000_HZ, /*!< PDS AUDIO PLL SET AS 12.288MHZ */
    AUDIO_PLL_11289600_HZ, /*!< PDS AUDIO PLL SET AS 11.2896HZ */
    AUDIO_PLL_5644800_HZ,  /*!< PDS AUDIO PLL SET AS 2.822400HZ */
    AUDIO_PLL_24576000_HZ, /*!< PDS AUDIO PLL SET AS 24.576000MHZ */
    AUDIO_PLL_24000000_HZ, /*!< PDS AUDIO PLL SET AS 24.000000MHZ */
    AUDIO_PLL_50000000_HZ, /*!< PDS AUDIO PLL SET AS 50.000000MHZ */
} GLB_AUDIO_PLL_Type;

typedef enum {
    GLB_I2C_CLK_SRC_BCLK,
    GLB_I2C_CLK_SRC_XCLK,
} GLB_I2C_CLK_SRC_Type;


#define UART_SIG_SWAP_GPIO0_GPIO7   0x01 /* GPIO0-7   uart_sig[0:7] -> uart_sig[4:7], uart_sig[0:3] */
#define UART_SIG_SWAP_GPIO8_GPIO15  0x02 /* GPIO8-15  uart_sig[0:7] -> uart_sig[4:7], uart_sig[0:3] */
#define UART_SIG_SWAP_GPIO16_GPIO22 0x04 /* GPIO16-22 uart_sig[0:7] -> uart_sig[4:7], uart_sig[0:3] */
#define UART_SIG_SWAP_NONE          0x00 /* GPIO0-22  uart_sig[0:7] <- uart_sig[4:7], uart_sig[0:3] */
#define JTAG_SIG_SWAP_GPIO0_GPIO3   0x01 /* GPIO0-3   E21_TMS/E21_TDI/E21_TCK/E21_TDO -> E21_TCK/E21_TDO/E21_TMS/E21_TDI */
#define JTAG_SIG_SWAP_GPIO4_GPIO7   0x02 /* GPIO4-7   E21_TMS/E21_TDI/E21_TCK/E21_TDO -> E21_TCK/E21_TDO/E21_TMS/E21_TDI */
#define JTAG_SIG_SWAP_GPIO8_GPIO11  0x04 /* GPIO8-11  E21_TMS/E21_TDI/E21_TCK/E21_TDO -> E21_TCK/E21_TDO/E21_TMS/E21_TDI */
#define JTAG_SIG_SWAP_GPIO12_GPIO15 0x08 /* GPIO12-15 E21_TMS/E21_TDI/E21_TCK/E21_TDO -> E21_TCK/E21_TDO/E21_TMS/E21_TDI */
#define JTAG_SIG_SWAP_GPIO16_GPIO19 0x10 /* GPIO16-19 E21_TMS/E21_TDI/E21_TCK/E21_TDO -> E21_TCK/E21_TDO/E21_TMS/E21_TDI */
#define JTAG_SIG_SWAP_GPIO20_GPIO22 0x20 /* GPIO20-22 E21_TMS/E21_TDI/E21_TCK/E21_TDO -> E21_TCK/E21_TDO/E21_TMS/E21_TDI */
#define JTAG_SIG_SWAP_NONE          0x00 /* GPIO0-22  E21_TMS/E21_TDI/E21_TCK/E21_TDO <- E21_TCK/E21_TDO/E21_TMS/E21_TDI */

GLB_ROOT_CLK_Type GLB_Get_Root_CLK_Sel(void);
int GLB_Set_System_CLK_Div(uint8_t hclkDiv, uint8_t bclkDiv);
uint8_t GLB_Get_BCLK_Div(void);
uint8_t GLB_Get_HCLK_Div(void);
int Update_SystemCoreClockWith_XTAL(GLB_DLL_XTAL_Type xtalType);
int GLB_Set_System_CLK(GLB_DLL_XTAL_Type xtalType, GLB_SYS_CLK_Type clkFreq);
int GLB_Set_System_CLK_Ex(GLB_DLL_XTAL_Type xtalType, GLB_SYS_CLK_Type clkFreq,
                          uint8_t pllOn, uint8_t dllOn);
int System_Core_Clock_Update_From_RC32M(void);

int GLB_Set_CAM_CLK(uint8_t enable, GLB_CAM_CLK_Type clkSel, uint8_t div);
int GLB_Set_SDH_CLK(uint8_t enable, GLB_SDH_CLK_Type clkSel, uint8_t div);
int GLB_Set_MAC154_ZIGBEE_CLK(uint8_t enable);
int GLB_Set_BLE_CLK(uint8_t enable);
int GLB_Set_I2S_CLK(uint8_t enable, GLB_I2S_OUT_REF_CLK_Type outRef);
int GLB_Set_I2S_CLK_Sel(GLB_I2S_REF_CLK_SRC_Type clkSel, uint8_t div);
int GLB_Set_USB_CLK(uint8_t enable);
int GLB_Set_QDEC_CLK(GLB_QDEC_CLK_Type clkSel, uint8_t div);

int GLB_Set_DMA_CLK(uint8_t enable, GLB_DMA_CLK_ID_Type clk);
int GLB_Set_IR_CLK(uint8_t enable, GLB_IR_CLK_SRC_Type clkSel, uint8_t div);
int GLB_Set_SF_CLK(uint8_t enable, GLB_SFLASH_CLK_Type clkSel, uint8_t div);
int GLB_Set_UART_CLK(uint8_t enable, HBN_UART_CLK_Type clkSel, uint8_t div);

int GLB_Set_Chip_Out_0_CLK_Sel(GLB_CHIP_CLK_OUT_Type clkSel);
int GLB_Set_Chip_Out_1_CLK_Sel(GLB_CHIP_CLK_OUT_Type clkSel);
int GLB_Set_Chip_Out_2_CLK_Sel(GLB_CHIP_CLK_OUT_Type clkSel);
int GLB_Set_I2C_CLK(uint8_t enable, GLB_I2C_CLK_SRC_Type clkSel, uint8_t div);
int GLB_Invert_ETH_RX_CLK(uint8_t enable);
int GLB_Invert_RF_TEST_O_CLK(uint8_t enable);
int GLB_Set_SPI_CLK(uint8_t enable, GLB_SPI_CLK_Type sel, uint8_t div);
int GLB_Set_SPI_2_CLK(uint8_t enable, GLB_SPI_CLK_Type sel, uint8_t div);
int GLB_Invert_ETH_TX_CLK(uint8_t enable);
int GLB_Invert_ETH_REF_O_CLK(uint8_t enable);
int GLB_Set_ETH_REF_O_CLK_Sel(GLB_ETH_REF_CLK_OUT_Type clkSel);

int GLB_Set_PKA_CLK_Sel(GLB_PKA_CLK_Type clkSel);
int GLB_SW_System_Reset(void);
int GLB_SW_CPU_Reset(void);
int GLB_SW_POR_Reset(void);
int GLB_AHB_MCU_Software_Reset(DRV_AHB_MCU_SW_Type swrst);
uint64_t GLB_PER_Clock_Gate_Status_Get(void);
int GLB_PER_Clock_Gate(GLB_AHB_CLOCK_Type module);
int GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_Type module);

int GLB_BMX_Init(BMX_Cfg_Type *BmxCfg);
int GLB_BMX_Addr_Monitor_Enable(void);
int GLB_BMX_Addr_Monitor_Disable(void);
int GLB_BMX_BusErrResponse_Enable(void);
int GLB_BMX_BusErrResponse_Disable(void);
int GLB_BMX_Get_Status(BMX_BUS_ERR_Type errType);
uint32_t GLB_BMX_Get_Err_Addr(void);
int BMX_ERR_INT_Callback_Install(BMX_ERR_INT_Type intType, intCallback_Type *cbFun);
int BMX_TIMEOUT_INT_Callback_Install(BMX_TO_INT_Type intType, intCallback_Type *cbFun);

int GLB_Set_SRAM_RET(uint32_t value);
uint32_t GLB_Get_SRAM_RET(void);
int GLB_Set_SRAM_SLP(uint32_t value);
uint32_t GLB_Get_SRAM_SLP(void);
int GLB_Set_SRAM_PARM(uint32_t value);
uint32_t GLB_Get_SRAM_PARM(void);

int GLB_Set_EM_Sel(GLB_EM_Type emType);

int GLB_SWAP_EMAC_CAM_Pin(GLB_EMAC_CAM_PIN_Type pinType);
int GLB_Set_Ext_Rst_Smt(uint8_t enable);
int GLB_Set_Kys_Drv_Col(uint8_t enable);
int GLB_UART_Sig_Swap_Set(uint8_t swapSel);
int GLB_JTAG_Sig_Swap_Set(uint8_t swapSel);
int GLB_CCI_Use_IO_0_1_2_7(uint8_t enable);
int GLB_CCI_Use_Jtag_Pin(uint8_t enable);
int GLB_Swap_SPI_0_MOSI_With_MISO(int newState);
int GLB_Set_SPI_0_ACT_MOD_Sel(GLB_SPI_PAD_ACT_AS_Type mod);
int GLB_Select_Internal_Flash(void);
int GLB_Select_External_Flash(void);
int GLB_Deswap_Flash_Pin(void);
int GLB_Swap_Flash_CS_IO2_Pin();
int GLB_Swap_Flash_IO0_IO3_Pin();
int GLB_Swap_Flash_Pin(void);
int GLB_Select_Internal_PSram(void);
uint8_t GLB_Get_UsePsramIO(void);
void GLB_Set_UsePsramIO(uint8_t val);
int GLB_Set_Embedded_FLash_IO_PARM(uint8_t reverse, uint8_t swapIo3Io0, uint8_t swapIo2Cs);
int GLB_Set_PSRAMB_CLK_Sel(uint8_t enable, GLB_PSRAMB_PLL_Type clkSel, uint8_t div);

int GLB_Set_PDM_CLK(uint8_t enable, uint8_t div);

int GLB_Set_MTimer_CLK(uint8_t enable, GLB_MTIMER_CLK_Type clkSel, uint32_t div);

int GLB_Set_ADC_CLK(uint8_t enable, GLB_ADC_CLK_Type clkSel, uint8_t div);
int GLB_Set_DAC_CLK(uint8_t enable, GLB_DAC_CLK_Type clkSel, uint8_t div);
int GLB_Set_GPDAC_CLK_Sel(GLB_DAC_CLK_Type clkSel, int div, int en);
int GLB_Set_AUDIO_CLK(uint8_t clkDivEn, uint8_t autoDivEn, GLB_AUDIO_CLK_SRC_Type clkSel, uint8_t div);

int GLB_Set_DIG_CLK_Sel(GLB_DIG_CLK_Type clkSel);
int GLB_Set_DIG_512K_CLK(uint8_t enable, uint8_t compensation, uint8_t div);
int GLB_Set_DIG_32K_CLK(uint8_t enable, uint8_t compensation, uint16_t div);

int GLB_Set_BT_Coex_Signal(uint8_t enable, GLB_BT_BANDWIDTH_Type bandWidth,
                                   uint8_t pti, uint8_t channel);

int GLB_UART_Fun_Sel(GLB_UART_SIG_Type sig, GLB_UART_SIG_FUN_Type fun);

int GLB_Power_Off_DLL(void);
int GLB_Power_On_DLL(GLB_DLL_XTAL_Type xtalType);
int GLB_Enable_DLL_All_Clks(void);
int GLB_Enable_DLL_Clk(GLB_DLL_CLK_Type dllClk);
int GLB_Disable_DLL_All_Clks(void);
int GLB_Disable_DLL_Clk(GLB_DLL_CLK_Type dllClk);

int GLB_IR_RX_GPIO_Sel(int gpio);
int GLB_Set_Flash_Id_Value(uint32_t idValue);
uint32_t GLB_Get_Flash_Id_Value(void);
int GLB_IR_LED_Driver_Enable(void);
int GLB_IR_LED_Driver_Disable(void);
int GLB_IR_LED_Driver_Output_Enable(int gpio);
int GLB_IR_LED_Driver_Output_Disable(int gpio);
int GLB_IR_LED_Driver_Ibias(uint8_t ibias);

int GLB_GPIO_Init(GLB_GPIO_Cfg_Type *cfg);
int GLB_GPIO_Func_Init(GLB_GPIO_FUNC_Type gpioFun, uint8_t *pinList, uint8_t cnt);
int GLB_GPIO_INPUT_Enable(int gpioPin);
int GLB_GPIO_INPUT_Disable(int gpioPin);
int GLB_GPIO_OUTPUT_Enable(int gpioPin);
int GLB_GPIO_OUTPUT_Disable(int gpioPin);
int GLB_GPIO_Set_HZ(int gpioPin);
int GLB_Set_Flash_Pad_HZ(void);
int GLB_Set_Psram_Pad_HZ(void);
uint8_t GLB_GPIO_Get_Fun(int gpioPin);
int GLB_GPIO_Write(int gpioPin, uint32_t val);
int GLB_GPIO_Toggle(int gpioPin);
uint32_t GLB_GPIO_Read(int gpioPin);
int GLB_GPIO_IntMask(int gpioPin, int intMask);
int GLB_GPIO_IntClear(int gpioPin, int intClear);
int GLB_Get_GPIO_IntStatus(int gpioPin);
int GLB_Set_GPIO_IntMod(int gpioPin, GLB_GPIO_INT_CONTROL_Type intCtlMod,
                                GLB_GPIO_INT_TRIG_Type intTrgMod);
GLB_GPIO_INT_CONTROL_Type GLB_Get_GPIO_IntCtlMod(int gpioPin);
int GLB_GPIO_Int2Mask(int gpioPin, int intMask);
int GLB_GPIO_Int2Clear(int gpioPin, int intClear);
int GLB_Get_GPIO_Int2Status(int gpioPin);
int GLB_Set_GPIO_Int2Mod(int gpioPin, GLB_GPIO_INT_CONTROL_Type intCtlMod,
                                 GLB_GPIO_INT_TRIG_Type intTrgMod);
GLB_GPIO_INT_CONTROL_Type GLB_Get_GPIO_Int2CtlMod(int gpioPin);
int GLB_GPIO_OUTPUT_Mode_Set(int gpioPin, GLB_GPIO_OUTPUT_MODE_Type mode);

int GLB_Switch_System_CLK(GLB_DLL_XTAL_Type xtalType,GLB_SYS_CLK_Type clkFreq, uint8_t hclkDiv,uint8_t bclkDiv);
int GLB_SetUnLockReq(GLB_LOCK_REQ_Type target);

int GLB_Power_Off_PLL(void);
int GLB_Set_Audio_PLL_Freq(GLB_AUDIO_PLL_Type audioPLLFreq);
int GLB_Trim_RC32M(void);
int GLB_Select_RC32M_As_PLL_Ref(void);
int GLB_RC32K_Cal(void);
int GLB_RC32M_Cal(void);
int GLB_Set_IPROAISram(int en);
int GLB_Select_XTAL_As_PLL_Ref(void);
int GLB_Power_On_PLL(GLB_PLL_XTAL_Type xtalType);
int GLB_Power_On_PLL_With_Freq(GLB_PLL_XTAL_Type xtalType, GLB_PLL_FREQ_Type freq);
int GLB_Enable_PLL_All_Clks(void);
int GLB_Enable_PLL_Clk(GLB_PLL_CLK_Type pllClk);
int GLB_Disable_PLL_All_Clks(void);
int GLB_Disable_PLL_Clk(GLB_PLL_CLK_Type pllClk);

#endif /* __DRV_GLB_H__ */

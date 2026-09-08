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

#ifndef __DRV_ROMDRIVER_H__
#define __DRV_ROMDRIVER_H__

#include "drv_aon.h"
#include "drv_clock.h"
#include "drv_common.h"
#include "drv_ef_cfg.h"
#include "drv_ef_ctrl.h"
#include "drv_glb.h"
#include "drv_gpio.h"
#include "drv_hbn.h"
#include "drv_pds.h"
#include "drv_sf_cfg.h"
#include "drv_sf_ctrl.h"
#include "drv_sflash.h"
#include "drv_uart.h"
#include "drv_xip_sflash.h"

#include "drv_ir.h"
#include "drv_kys.h"
#include "drv_psram.h"
#include "drv_timer.h"

#include "misc.h"
#include "utils_crc.h"

#define ROMAPI_INDEX_SECT_SIZE     (0x800)
#define ROMAPI_INDEX_MAX           (ROMAPI_INDEX_SECT_SIZE / 4 - 1)

typedef enum {
    ROM_API_INDEX_VERSION = 0,
    ROM_API_INDEX_RSVD_0,
    ROM_API_INDEX_RSVD_1,
    ROM_API_INDEX_RSVD_LAST,


    ROM_API_INDEX_AON_Power_On_MBG,
    ROM_API_INDEX_AON_Power_Off_MBG,
    ROM_API_INDEX_AON_Power_On_XTAL,
    ROM_API_INDEX_AON_Set_Xtal_CapCode,
    ROM_API_INDEX_AON_Get_Xtal_CapCode,
    ROM_API_INDEX_AON_Set_Xtal_CapCode_Extra,
    ROM_API_INDEX_AON_Power_Off_XTAL,
    ROM_API_INDEX_AON_Power_On_BG,
    ROM_API_INDEX_AON_Power_Off_BG,
    ROM_API_INDEX_AON_Power_On_LDO11_SOC,
    ROM_API_INDEX_AON_Power_Off_LDO11_SOC,
    ROM_API_INDEX_AON_Power_On_SFReg,
    ROM_API_INDEX_AON_Power_Off_SFReg,
    ROM_API_INDEX_AON_Set_LDO11_SOC_Sstart_Delay,
    ROM_API_INDEX_AON_Set_DCDC14_Top_0,
    ROM_API_INDEX_AON_Trim_Ldo11socVoutTrim,
    ROM_API_INDEX_AON_Trim_Ldo14VoutTrim,
    ROM_API_INDEX_AON_Trim_Dcdc14VoutTrim,

    ROM_API_INDEX_Clock_System_Clock_Get,

    ROM_API_INDEX_SystemCoreClockGet,

    ROM_API_INDEX_Clock_Peripheral_Clock_Get_Flash,
    ROM_API_INDEX_Clock_Peripheral_Clock_Get,

    ROM_API_INDEX_arch_delay_us,
    ROM_API_INDEX_arch_delay_ms,

    ROM_API_INDEX_EF_Ctrl_Get_Trim_Parity,
    ROM_API_INDEX_EF_Ctrl_Read_Common_Trim,
    ROM_API_INDEX_EF_Ctrl_Write_Common_Trim,
    ROM_API_INDEX_EF_Ctrl_Is_MAC_Address_Slot_Empty,
    ROM_API_INDEX_EF_Ctrl_Write_MAC_Address_Opt,
    ROM_API_INDEX_EF_Ctrl_Read_MAC_Address_Opt,
    ROM_API_INDEX_EF_Ctrl_Sw_AHB_Clk_0,
    ROM_API_INDEX_EF_Ctrl_Program_Efuse_0,
    ROM_API_INDEX_EF_Ctrl_Load_Efuse_R0,
    ROM_API_INDEX_EF_Ctrl_Busy,
    ROM_API_INDEX_EF_Ctrl_AutoLoad_Done,
    ROM_API_INDEX_EF_Ctrl_Write_Dbg_Pwd,
    ROM_API_INDEX_EF_Ctrl_Read_Dbg_Pwd,
    ROM_API_INDEX_EF_Ctrl_Readlock_Dbg_Pwd,
    ROM_API_INDEX_EF_Ctrl_Writelock_Dbg_Pwd,
    ROM_API_INDEX_EF_Ctrl_Write_Secure_Cfg,
    ROM_API_INDEX_EF_Ctrl_Read_Secure_Cfg,
    ROM_API_INDEX_EF_Ctrl_Write_Secure_Boot,
    ROM_API_INDEX_EF_Ctrl_Read_Secure_Boot,
    ROM_API_INDEX_EF_Ctrl_Write_Sw_Usage,
    ROM_API_INDEX_EF_Ctrl_Read_Sw_Usage,
    ROM_API_INDEX_EF_Ctrl_Writelock_Sw_Usage,
    ROM_API_INDEX_EF_Ctrl_Write_MAC_Address,
    ROM_API_INDEX_EF_Ctrl_Get_Byte_Zero_Cnt,
    ROM_API_INDEX_EF_Ctrl_Is_All_Bits_Zero,
    ROM_API_INDEX_EF_Ctrl_Read_MAC_Address,
    ROM_API_INDEX_EF_Ctrl_Writelock_MAC_Address,
    ROM_API_INDEX_EF_Ctrl_Read_Chip_ID,
    ROM_API_INDEX_EF_Ctrl_Read_Device_Info,
    ROM_API_INDEX_EF_Ctrl_Is_CapCode_Empty,
    ROM_API_INDEX_EF_Ctrl_Write_CapCode_Opt,
    ROM_API_INDEX_EF_Ctrl_Read_CapCode_Opt,
    ROM_API_INDEX_EF_Ctrl_Is_PowerOffset_Slot_Empty,
    ROM_API_INDEX_EF_Ctrl_Write_PowerOffset_Opt,
    ROM_API_INDEX_EF_Ctrl_Read_PowerOffset_Opt,
    ROM_API_INDEX_EF_Ctrl_Write_AES_Key,
    ROM_API_INDEX_EF_Ctrl_Read_AES_Key,
    ROM_API_INDEX_EF_Ctrl_Writelock_AES_Key,
    ROM_API_INDEX_EF_Ctrl_Readlock_AES_Key,
    ROM_API_INDEX_EF_Ctrl_Program_Direct_R0,
    ROM_API_INDEX_EF_Ctrl_Read_Direct_R0,
    ROM_API_INDEX_EF_Ctrl_Clear,
    ROM_API_INDEX_EF_Ctrl_Crc_Enable,
    ROM_API_INDEX_EF_Ctrl_Crc_Is_Busy,
    ROM_API_INDEX_EF_Ctrl_Crc_Set_Golden,
    ROM_API_INDEX_EF_Ctrl_Crc_Result,

    ROM_API_INDEX_GLB_Get_Root_CLK_Sel,
    ROM_API_INDEX_GLB_Set_System_CLK_Div,
    ROM_API_INDEX_GLB_Get_BCLK_Div,
    ROM_API_INDEX_GLB_Get_HCLK_Div,
    ROM_API_INDEX_GLB_Switch_System_CLK,
    ROM_API_INDEX_GLB_Set_System_CLK,

    ROM_API_INDEX_System_Core_Clock_Update_From_RC32M,

    ROM_API_INDEX_GLB_Enable_DLL_All_Clks,
    ROM_API_INDEX_GLB_Enable_DLL_Clk,
    ROM_API_INDEX_GLB_Disable_DLL_All_Clks,
    ROM_API_INDEX_GLB_Disable_DLL_Clk,
    ROM_API_INDEX_GLB_Set_CAM_CLK,
    ROM_API_INDEX_GLB_Set_MAC154_ZIGBEE_CLK,
    ROM_API_INDEX_GLB_Set_BLE_CLK,
    ROM_API_INDEX_GLB_Set_I2S_CLK,
    ROM_API_INDEX_GLB_Set_USB_CLK,
    ROM_API_INDEX_GLB_Set_QDEC_CLK,
    ROM_API_INDEX_GLB_Set_DMA_CLK,
    ROM_API_INDEX_GLB_Set_IR_CLK,
    ROM_API_INDEX_GLB_Set_SF_CLK,
    ROM_API_INDEX_GLB_Set_UART_CLK,
    ROM_API_INDEX_GLB_Set_Chip_Out_0_CLK_Sel,
    ROM_API_INDEX_GLB_Set_Chip_Out_1_CLK_Sel,
    ROM_API_INDEX_GLB_Set_Chip_Out_2_CLK_Sel,
    ROM_API_INDEX_GLB_Set_I2C_CLK,
    ROM_API_INDEX_GLB_Invert_ETH_RX_CLK,
    ROM_API_INDEX_GLB_Invert_RF_TEST_O_CLK,
    ROM_API_INDEX_GLB_Set_SPI_CLK,
    ROM_API_INDEX_GLB_Invert_ETH_TX_CLK,
    ROM_API_INDEX_GLB_Invert_ETH_REF_O_CLK,
    ROM_API_INDEX_GLB_Set_ETH_REF_O_CLK_Sel,
    ROM_API_INDEX_GLB_Set_PKA_CLK_Sel,
    ROM_API_INDEX_GLB_SW_System_Reset,
    ROM_API_INDEX_GLB_SW_CPU_Reset,
    ROM_API_INDEX_GLB_SW_POR_Reset,
    ROM_API_INDEX_GLB_AHB_MCU_Software_Reset,
    ROM_API_INDEX_GLB_PER_Clock_Gate,
    ROM_API_INDEX_GLB_PER_Clock_UnGate,
    ROM_API_INDEX_GLB_Set_EM_Sel,
    ROM_API_INDEX_GLB_Set_IPROAISram,
    ROM_API_INDEX_GLB_Set_Kys_Drv_Col,
    ROM_API_INDEX_GLB_JTAG_Sig_Swap_Set,
    ROM_API_INDEX_GLB_CCI_Use_IO_0_1_2_7,
    ROM_API_INDEX_GLB_CCI_Use_Jtag_Pin,
    ROM_API_INDEX_GLB_Swap_SPI_0_MOSI_With_MISO,
    ROM_API_INDEX_GLB_Set_SPI_0_ACT_MOD_Sel,
    ROM_API_INDEX_GLB_Set_Embedded_FLash_IO_PARM,
    ROM_API_INDEX_GLB_Set_MTimer_CLK,
    ROM_API_INDEX_GLB_Set_ADC_CLK,
    ROM_API_INDEX_GLB_Set_DIG_CLK_Sel,
    ROM_API_INDEX_GLB_Set_DIG_32K_CLK,
    ROM_API_INDEX_GLB_UART_Fun_Sel,
    ROM_API_INDEX_GLB_Power_Off_DLL,
    ROM_API_INDEX_GLB_Power_On_DLL,
    ROM_API_INDEX_GLB_Select_RC32M_As_PLL_Ref,
    ROM_API_INDEX_GLB_Select_XTAL_As_PLL_Ref,
    ROM_API_INDEX_GLB_Power_On_PLL,
    ROM_API_INDEX_GLB_Enable_PLL_All_Clks,
    ROM_API_INDEX_GLB_Disable_PLL_All_Clks,
    ROM_API_INDEX_GLB_Enable_PLL_Clk,
    ROM_API_INDEX_GLB_Disable_PLL_Clk,
    ROM_API_INDEX_GLB_Power_Off_PLL,
    ROM_API_INDEX_GLB_Set_Audio_PLL_Freq,
    ROM_API_INDEX_GLB_Set_Flash_Id_Value,
    ROM_API_INDEX_GLB_Get_Flash_Id_Value,
    ROM_API_INDEX_GLB_SetUnLockReq,
    ROM_API_INDEX_GLB_Trim_RC32M,
    ROM_API_INDEX_GLB_GPIO_Init,
    ROM_API_INDEX_GLB_GPIO_Func_Init,
    ROM_API_INDEX_GLB_GPIO_OUTPUT_Mode_Set,
    ROM_API_INDEX_GLB_GPIO_INPUT_Enable,
    ROM_API_INDEX_GLB_GPIO_INPUT_Disable,
    ROM_API_INDEX_GLB_GPIO_OUTPUT_Enable,
    ROM_API_INDEX_GLB_GPIO_OUTPUT_Disable,
    ROM_API_INDEX_GLB_GPIO_Set_HZ,
    ROM_API_INDEX_GLB_Set_Flash_Pad_HZ,
    ROM_API_INDEX_GLB_Set_Psram_Pad_HZ,
    ROM_API_INDEX_GLB_GPIO_Get_Fun,
    ROM_API_INDEX_GLB_GPIO_Write,
    ROM_API_INDEX_GLB_GPIO_Toggle,
    ROM_API_INDEX_GLB_GPIO_Read,
    ROM_API_INDEX_GLB_GPIO_IntMask,
    ROM_API_INDEX_GLB_GPIO_IntClear,
    ROM_API_INDEX_GLB_Get_GPIO_IntStatus,
    ROM_API_INDEX_GLB_Set_GPIO_IntMod,
    ROM_API_INDEX_GLB_Get_GPIO_IntCtlMod,
    ROM_API_INDEX_GLB_GPIO_Int2Mask,
    ROM_API_INDEX_GLB_GPIO_Int2Clear,
    ROM_API_INDEX_GLB_Get_GPIO_Int2Status,
    ROM_API_INDEX_GLB_Set_GPIO_Int2Mod,
    ROM_API_INDEX_GLB_Get_GPIO_Int2CtlMod,

    ROM_API_INDEX_HBN_Mode_Enter,
    ROM_API_INDEX_HBN_GPIO_Wakeup_Set,
    ROM_API_INDEX_HBN_Power_Down_Flash,
    ROM_API_INDEX_HBN_Enable,
    ROM_API_INDEX_HBN_Reset,
    ROM_API_INDEX_HBN_App_Reset,
    ROM_API_INDEX_HBN_Disable,
    ROM_API_INDEX_HBN_Get_BOR_OUT_State,
    ROM_API_INDEX_HBN_Set_BOR_Config,
    ROM_API_INDEX_HBN_Set_Ldo11_Aon_Vout,
    ROM_API_INDEX_HBN_Set_Ldo11_Soc_Vout,
    ROM_API_INDEX_HBN_Set_Ldo11_All_Vout,
    ROM_API_INDEX_HBN_32K_Sel,
    ROM_API_INDEX_HBN_Set_UART_CLK_Sel,
    ROM_API_INDEX_HBN_Set_XCLK_CLK_Sel,
    ROM_API_INDEX_HBN_Set_ROOT_CLK,
    ROM_API_INDEX_HBN_Get_ROOT_CLK,
    ROM_API_INDEX_HBN_Set_HRAM_slp,
    ROM_API_INDEX_HBN_Set_HRAM_Ret,
    ROM_API_INDEX_HBN_Power_On_Xtal_32K,
    ROM_API_INDEX_HBN_Power_Off_Xtal_32K,
    ROM_API_INDEX_HBN_Power_On_RC32K,
    ROM_API_INDEX_HBN_Power_Off_RC32K,
    ROM_API_INDEX_HBN_Trim_RC32K,
    ROM_API_INDEX_HBN_Get_Status_Flag,
    ROM_API_INDEX_HBN_Set_Status_Flag,
    ROM_API_INDEX_HBN_Get_Wakeup_Addr,
    ROM_API_INDEX_HBN_Set_Wakeup_Addr,
    ROM_API_INDEX_HBN_Get_User_Boot_Config,
    ROM_API_INDEX_HBN_Set_User_Boot_Config,
    ROM_API_INDEX_HBN_Clear_RTC_Counter,
    ROM_API_INDEX_HBN_Enable_RTC_Counter,
    ROM_API_INDEX_HBN_Set_RTC_Timer,
    ROM_API_INDEX_HBN_Get_RTC_Timer_Val,
    ROM_API_INDEX_HBN_Clear_RTC_INT,
    ROM_API_INDEX_HBN_GPIO_INT_Enable,
    ROM_API_INDEX_HBN_GPIO_INT_Disable,
    ROM_API_INDEX_HBN_Get_INT_State,
    ROM_API_INDEX_HBN_Get_Pin_Wakeup_Mode,
    ROM_API_INDEX_HBN_Clear_IRQ,
    ROM_API_INDEX_HBN_Hw_Pu_Pd_Cfg,
    ROM_API_INDEX_HBN_Comm_Pad_Wakeup_En_Cfg,
    ROM_API_INDEX_HBN_Aon_Pad_IeSmt_Cfg,
    ROM_API_INDEX_HBN_Pin_WakeUp_Mask,
    ROM_API_INDEX_HBN_Enable_AComp0_IRQ,
    ROM_API_INDEX_HBN_Disable_AComp0_IRQ,
    ROM_API_INDEX_HBN_Enable_AComp1_IRQ,
    ROM_API_INDEX_HBN_Disable_AComp1_IRQ,
    ROM_API_INDEX_HBN_Enable_BOR_IRQ,
    ROM_API_INDEX_HBN_Disable_BOR_IRQ,
    ROM_API_INDEX_HBN_Get_Reset_Event,
    ROM_API_INDEX_HBN_Clear_Reset_Event,
    ROM_API_INDEX_HBN_GPIO_Dbg_Pull_Cfg,
    ROM_API_INDEX_HBN_Set_BOR_Cfg,

    ROM_API_INDEX_PDS_Set_GPIO_Pad_Cfg,
    ROM_API_INDEX_PDS_GPIO_Write,
    ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMask,
    ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMode,
    ROM_API_INDEX_PDS_Set_GPIO_Pad_IntClr,
    ROM_API_INDEX_PDS_Set_All_GPIO_IntClear,
    ROM_API_INDEX_PDS_Get_GPIO_Pad_IntStatus,
    ROM_API_INDEX_PDS_Disable_GPIO_Keep,
    ROM_API_INDEX_PDS_Enable,
    ROM_API_INDEX_PDS_Force_Config,
    ROM_API_INDEX_PDS_RAM_Config,
    ROM_API_INDEX_PDS_Default_Level_Config,
    ROM_API_INDEX_PDS_Wakeup_Src_En,
    ROM_API_INDEX_PDS_Get_Wakeup_Src,
    ROM_API_INDEX_PDS_IntMask,
    ROM_API_INDEX_PDS_Get_IntStatus,
    ROM_API_INDEX_PDS_IntClear,
    ROM_API_INDEX_PDS_Get_PdsRfStstus,
    ROM_API_INDEX_PDS_Get_PdsStstus,
    ROM_API_INDEX_PDS_Clear_Reset_Event,
    ROM_API_INDEX_PDS_Get_Reset_Event,
    ROM_API_INDEX_PDS_Auto_Enable,
    ROM_API_INDEX_PDS_Manual_Force_Turn_Off,
    ROM_API_INDEX_PDS_Manual_Force_Turn_On,
    ROM_API_INDEX_PDS_Set_KYD_Matrix_Size,
    ROM_API_INDEX_PDS_Set_KYD_Col_Value,
    ROM_API_INDEX_PDS_Set_KYD_Row_Pull,
    ROM_API_INDEX_PDS_Set_KYD_Wakeup_En,

    ROM_API_INDEX_SF_Cfg_Init_Ext_Flash_Gpio,
    ROM_API_INDEX_SF_Cfg_Deinit_Ext_Flash_Gpio,
    ROM_API_INDEX_SF_Cfg_Get_Flash_Cfg_Need_Lock,
    ROM_API_INDEX_SF_Cfg_Init_Flash_Gpio,
    ROM_API_INDEX_SF_Cfg_Flash_Identify,
    ROM_API_INDEX_SF_Ctrl_SetDelay,
    ROM_API_INDEX_SF_Ctrl_Enable,
    ROM_API_INDEX_SF_Ctrl_Psram_Init,
    ROM_API_INDEX_SF_Ctrl_Get_Clock_Delay,
    ROM_API_INDEX_SF_Ctrl_Set_Clock_Delay,
    ROM_API_INDEX_SF_Ctrl_Cmds_Set,
    ROM_API_INDEX_SF_Ctrl_Burst_Toggle_Set,
    ROM_API_INDEX_SF_Ctrl_Select_Pad,
    ROM_API_INDEX_SF_Ctrl_Select_Bank,
    ROM_API_INDEX_SF_Ctrl_Set_Owner,
    ROM_API_INDEX_SF_Ctrl_Disable,
    ROM_API_INDEX_SF_Ctrl_AES_Enable_BE,
    ROM_API_INDEX_SF_Ctrl_AES_Enable_LE,
    ROM_API_INDEX_SF_Ctrl_AES_Set_Region,
    ROM_API_INDEX_SF_Ctrl_AES_Set_Key,
    ROM_API_INDEX_SF_Ctrl_AES_Set_Key_BE,
    ROM_API_INDEX_SF_Ctrl_AES_Set_IV,
    ROM_API_INDEX_SF_Ctrl_AES_Set_IV_BE,
    ROM_API_INDEX_SF_Ctrl_AES_Enable,
    ROM_API_INDEX_SF_Ctrl_AES_Disable,
    ROM_API_INDEX_SF_Ctrl_Is_AES_Enable,
    ROM_API_INDEX_SF_Ctrl_Set_Flash_Image_Offset,
    ROM_API_INDEX_SF_Ctrl_Get_Flash_Image_Offset,
    ROM_API_INDEX_SF_Ctrl_SendCmd,
    ROM_API_INDEX_SF_Ctrl_Flash_Read_Icache_Set,
    ROM_API_INDEX_SF_Ctrl_Psram_Write_Icache_Set,
    ROM_API_INDEX_SF_Ctrl_Psram_Read_Icache_Set,
    ROM_API_INDEX_SF_Ctrl_GetBusyState,

    ROM_API_INDEX_SFlash_Init,
    ROM_API_INDEX_SFlash_SetSPIMode,
    ROM_API_INDEX_SFlash_Read_Reg,
    ROM_API_INDEX_SFlash_Write_Reg,
    ROM_API_INDEX_SFlash_Read_Reg_With_Cmd,
    ROM_API_INDEX_SFlash_Write_Reg_With_Cmd,
    ROM_API_INDEX_SFlash_Busy,
    ROM_API_INDEX_SFlash_Write_Enable,
    ROM_API_INDEX_SFlash_Qspi_Enable,
    ROM_API_INDEX_SFlash_Volatile_Reg_Write_Enable,
    ROM_API_INDEX_SFlash_Chip_Erase,
    ROM_API_INDEX_SFlash_Sector_Erase,
    ROM_API_INDEX_SFlash_Blk32_Erase,
    ROM_API_INDEX_SFlash_Blk64_Erase,
    ROM_API_INDEX_SFlash_Erase,
    ROM_API_INDEX_SFlash_Program,
    ROM_API_INDEX_SFlash_GetUniqueId,
    ROM_API_INDEX_SFlash_GetJedecId,
    ROM_API_INDEX_SFlash_GetDeviceId,
    ROM_API_INDEX_SFlash_Powerdown,
    ROM_API_INDEX_SFlash_Releae_Powerdown,
    ROM_API_INDEX_SFlash_Restore_From_Powerdown,
    ROM_API_INDEX_SFlash_SetBurstWrap,
    ROM_API_INDEX_SFlash_DisableBurstWrap,
    ROM_API_INDEX_SFlash_Software_Reset,
    ROM_API_INDEX_SFlash_Reset_Continue_Read,
    ROM_API_INDEX_SFlash_Set_IDbus_Cfg,
    ROM_API_INDEX_SFlash_IDbus_Read_Enable,
    ROM_API_INDEX_SFlash_Cache_Read_Enable,
    ROM_API_INDEX_SFlash_Read,

    ROM_API_INDEX_UART_SetBaudRate,
    ROM_API_INDEX_UART_Init,
    ROM_API_INDEX_UART_DeInit,
    ROM_API_INDEX_UART_FifoConfig,
    ROM_API_INDEX_UART_Enable,
    ROM_API_INDEX_UART_Disable,
    ROM_API_INDEX_UART_DmaEnable,
    ROM_API_INDEX_UART_DmaDisable,
    ROM_API_INDEX_UART_SetTxDataLength,
    ROM_API_INDEX_UART_SetRxDataLength,
    ROM_API_INDEX_UART_SetRxTimeoutValue,
    ROM_API_INDEX_UART_SetDeglitchCount,
    ROM_API_INDEX_UART_ApplyAbrResult,
    ROM_API_INDEX_UART_SetRtsValue,
    ROM_API_INDEX_UART_ClrRtsValue,
    ROM_API_INDEX_UART_SetTxValue,
    ROM_API_INDEX_UART_ClrTxValue,
    ROM_API_INDEX_UART_TxFreeRun,
    ROM_API_INDEX_UART_AutoBaudDetection,
    ROM_API_INDEX_UART_SetAllowableError0X55,
    ROM_API_INDEX_UART_GetBitWidth0X55,
    ROM_API_INDEX_UART_SetRS485,
    ROM_API_INDEX_UART_TxFifoClear,
    ROM_API_INDEX_UART_RxFifoClear,
    ROM_API_INDEX_UART_IntMask,
    ROM_API_INDEX_UART_IntClear,
    ROM_API_INDEX_UART_SendData,
    ROM_API_INDEX_UART_SendDataBlock,
    ROM_API_INDEX_UART_ReceiveData,
    ROM_API_INDEX_UART_GetAutoBaudCount,
    ROM_API_INDEX_UART_GetRxByteCount,
    ROM_API_INDEX_UART_GetTxFifoCount,
    ROM_API_INDEX_UART_GetRxFifoCount,
    ROM_API_INDEX_UART_GetIntStatus,
    ROM_API_INDEX_UART_GetTxBusBusyStatus,
    ROM_API_INDEX_UART_GetRxBusBusyStatus,
    ROM_API_INDEX_UART_GetOverflowStatus,

    ROM_API_INDEX_XIP_SFlash_Opt_Enter,
    ROM_API_INDEX_XIP_SFlash_Opt_Exit,
    ROM_API_INDEX_XIP_SFlash_State_Save,
    ROM_API_INDEX_XIP_SFlash_State_Restore,
    ROM_API_INDEX_XIP_SFlash_Erase_Need_Lock,
    ROM_API_INDEX_XIP_SFlash_Write_Need_Lock,
    ROM_API_INDEX_XIP_SFlash_Read_Need_Lock,
    ROM_API_INDEX_XIP_SFlash_GetJedecId_Need_Lock,
    ROM_API_INDEX_XIP_SFlash_GetDeviceId_Need_Lock,
    ROM_API_INDEX_XIP_SFlash_GetUniqueId_Need_Lock,
    ROM_API_INDEX_XIP_SFlash_Read_Via_Cache_Need_Lock,

    ROM_API_INDEX_IR_TxInit,
    ROM_API_INDEX_IR_TxPulseWidthConfig,
    ROM_API_INDEX_IR_TxSWMPulseWidthConfig,
    ROM_API_INDEX_IR_RxInit,
    ROM_API_INDEX_IR_DeInit,
    ROM_API_INDEX_IR_Enable,
    ROM_API_INDEX_IR_Disable,
    ROM_API_INDEX_IR_TxSWM,
    ROM_API_INDEX_IR_RxFIFOClear,
    ROM_API_INDEX_IR_SendData,
    ROM_API_INDEX_IR_SWMSendData,
    ROM_API_INDEX_IR_SendCommand,
    ROM_API_INDEX_IR_SWMSendCommand,
    ROM_API_INDEX_IR_SendNEC,
    ROM_API_INDEX_IR_IntMask,
    ROM_API_INDEX_IR_ClrIntStatus,
    ROM_API_INDEX_IR_GetIntStatus,
    ROM_API_INDEX_IR_GetRxFIFOStatus,
    ROM_API_INDEX_IR_ReceiveData,
    ROM_API_INDEX_IR_SWMReceiveData,
    ROM_API_INDEX_IR_ReceiveNEC,
    ROM_API_INDEX_IR_GetRxDataBitCount,
    ROM_API_INDEX_IR_GetRxFIFOCount,
    ROM_API_INDEX_IR_LearnToInit,
    ROM_API_INDEX_IR_LearnToReceive,
    ROM_API_INDEX_IR_LearnToSend,
    ROM_API_INDEX_IR_LEDInit,
    ROM_API_INDEX_IR_LEDSend,

    ROM_API_INDEX_KYS_Init,
    ROM_API_INDEX_KYS_Enable,
    ROM_API_INDEX_KYS_Disable,
    ROM_API_INDEX_KYS_IntMask,
    ROM_API_INDEX_KYS_IntClear,
    ROM_API_INDEX_KYS_GetIntStatus,
    ROM_API_INDEX_KYS_GetKeycode,

    ROM_API_INDEX_Psram_Init,
    ROM_API_INDEX_Psram_ReadReg,
    ROM_API_INDEX_Psram_WriteReg,
    ROM_API_INDEX_Psram_SetDriveStrength,
    ROM_API_INDEX_Psram_SetBurstWrap,
    ROM_API_INDEX_Psram_ReadId,
    ROM_API_INDEX_Psram_EnterQuadMode,
    ROM_API_INDEX_Psram_ExitQuadMode,
    ROM_API_INDEX_Psram_ToggleBurstLength,
    ROM_API_INDEX_Psram_SoftwareReset,
    ROM_API_INDEX_Psram_Set_IDbus_Cfg,
    ROM_API_INDEX_Psram_Cache_Write_Set,
    ROM_API_INDEX_Psram_Write,
    ROM_API_INDEX_Psram_Read,

    ROM_API_INDEX_TIMER_GetCompValue,
    ROM_API_INDEX_TIMER_SetCompValue,
    ROM_API_INDEX_TIMER_GetCounterValue,
    ROM_API_INDEX_TIMER_GetMatchStatus,
    ROM_API_INDEX_TIMER_GetPreloadValue,
    ROM_API_INDEX_TIMER_SetPreloadValue,
    ROM_API_INDEX_TIMER_SetPreloadSrc,
    ROM_API_INDEX_TIMER_SetCountMode,
    ROM_API_INDEX_TIMER_ClearIntStatus,
    ROM_API_INDEX_TIMER_SetDiv,
    ROM_API_INDEX_TIMER_Init,
    ROM_API_INDEX_TIMER_Enable,
    ROM_API_INDEX_TIMER_Disable,
    ROM_API_INDEX_TIMER_IntMask,

    ROM_API_INDEX_WDT_Set_Clock,
    ROM_API_INDEX_WDT_GetMatchValue,
    ROM_API_INDEX_WDT_SetCompValue,
    ROM_API_INDEX_WDT_GetCounterValue,
    ROM_API_INDEX_WDT_ResetCounterValue,
    ROM_API_INDEX_WDT_GetResetStatus,
    ROM_API_INDEX_WDT_ClearResetStatus,
    ROM_API_INDEX_WDT_Enable,
    ROM_API_INDEX_WDT_Disable,
    ROM_API_INDEX_WDT_ClearInt,
    ROM_API_INDEX_WDT_IntMask,

    ROM_API_INDEX_arch_memcpy,
    ROM_API_INDEX_arch_memcpy4,
    ROM_API_INDEX_arch_memcpy_fast,
    ROM_API_INDEX_arch_memset4,
    ROM_API_INDEX_arch_memcmp,

    ROM_API_INDEX_memcopy_to_fifo,

    ROM_API_INDEX_fifocopy_to_mem,

    ROM_API_INDEX_utils_crc16,
    ROM_API_INDEX_utils_crc32_accumulate,
    ROM_API_INDEX_utils_crc32,
    ROM_API_INDEX_utils_crc32_stream_init,
    ROM_API_INDEX_utils_crc32_stream_feed,
    ROM_API_INDEX_utils_crc32_stream_feed_block,
    ROM_API_INDEX_utils_crc32_stream_results,
    ROM_API_INDEX_utils_crc16_ccitt,

    ROM_API_INDEX_FUNC_EMPTY_START,

    ROM_API_INDEX_FUNC_EMPTY_END = ROMAPI_INDEX_MAX

} ROM_API_INDEX_e;


#define ROM_APITABLE            ((uint32_t *)0x10FF3000)


#define RomDriver_AON_Power_On_MBG                        \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_On_MBG])

#define RomDriver_AON_Power_Off_MBG                       \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_Off_MBG])

#define RomDriver_AON_Power_On_XTAL                       \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_On_XTAL])

#define RomDriver_AON_Set_Xtal_CapCode                    \
    ((int (*) (uint8_t capIn, uint8_t capOut))ROM_APITABLE[ROM_API_INDEX_AON_Set_Xtal_CapCode])

#define RomDriver_AON_Get_Xtal_CapCode                    \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Get_Xtal_CapCode])

#define RomDriver_AON_Set_Xtal_CapCode_Extra              \
    ((int (*) (uint8_t extra))ROM_APITABLE[ROM_API_INDEX_AON_Set_Xtal_CapCode_Extra])

#define RomDriver_AON_Power_Off_XTAL                      \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_Off_XTAL])

#define RomDriver_AON_Power_On_BG                         \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_On_BG])

#define RomDriver_AON_Power_Off_BG                        \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_Off_BG])

#define RomDriver_AON_Power_On_LDO11_SOC                  \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_On_LDO11_SOC])

#define RomDriver_AON_Power_Off_LDO11_SOC                 \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_Off_LDO11_SOC])

#define RomDriver_AON_Power_On_SFReg                      \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_On_SFReg])

#define RomDriver_AON_Power_Off_SFReg                     \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_Off_SFReg])

#define RomDriver_AON_Set_LDO11_SOC_Sstart_Delay          \
    ((int (*) (uint8_t delay))ROM_APITABLE[ROM_API_INDEX_AON_Set_LDO11_SOC_Sstart_Delay])

#define RomDriver_AON_Set_DCDC14_Top_0                    \
    ((int (*) (uint8_t voutSel, uint8_t vpfm))ROM_APITABLE[ROM_API_INDEX_AON_Set_DCDC14_Top_0])

#define RomDriver_AON_Trim_Ldo11socVoutTrim               \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Trim_Ldo11socVoutTrim])

#define RomDriver_AON_Trim_Ldo14VoutTrim                  \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Trim_Ldo14VoutTrim])

#define RomDriver_AON_Trim_Dcdc14VoutTrim                 \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Trim_Dcdc14VoutTrim])

#define RomDriver_Clock_System_Clock_Get                  \
    ((uint32_t (*) (Drv_System_Clock_Type type))ROM_APITABLE[ROM_API_INDEX_Clock_System_Clock_Get])

#define RomDriver_SystemCoreClockGet                      \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_SystemCoreClockGet])

#define RomDriver_Clock_Peripheral_Clock_Get_Flash        \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_Clock_Peripheral_Clock_Get_Flash])

#define RomDriver_Clock_Peripheral_Clock_Get              \
    ((uint32_t (*) (Drv_Peripheral_Type_Type type))ROM_APITABLE[ROM_API_INDEX_Clock_Peripheral_Clock_Get])

#define RomDriver_arch_delay_us                           \
    ((void (*) (uint32_t tick_us))ROM_APITABLE[ROM_API_INDEX_arch_delay_us])

#define RomDriver_arch_delay_ms                           \
    ((void (*) (uint32_t cnt))ROM_APITABLE[ROM_API_INDEX_arch_delay_ms])

#define RomDriver_EF_Ctrl_Get_Trim_Parity                 \
    ((uint8_t (*) (uint32_t val, uint8_t len))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Get_Trim_Parity])

#define RomDriver_EF_Ctrl_Read_Common_Trim                \
    ((void (*) (char *name, Efuse_Common_Trim_Type *trim))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_Common_Trim])

#define RomDriver_EF_Ctrl_Write_Common_Trim               \
    ((void (*) (char *name, uint8_t trim_en, uint32_t trim_value))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_Common_Trim])

#define RomDriver_EF_Ctrl_Is_MAC_Address_Slot_Empty       \
    ((uint8_t (*) (uint8_t slot, uint8_t reload))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Is_MAC_Address_Slot_Empty])

#define RomDriver_EF_Ctrl_Write_MAC_Address_Opt           \
    ((int (*) (uint8_t slot, uint8_t mac[8], uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_MAC_Address_Opt])

#define RomDriver_EF_Ctrl_Read_MAC_Address_Opt            \
    ((int (*) (uint8_t slot, uint8_t mac[8], uint8_t reload))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_MAC_Address_Opt])

#define RomDriver_EF_Ctrl_Sw_AHB_Clk_0                    \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Sw_AHB_Clk_0])

#define RomDriver_EF_Ctrl_Program_Efuse_0                 \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Program_Efuse_0])

#define RomDriver_EF_Ctrl_Load_Efuse_R0                   \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Load_Efuse_R0])

#define RomDriver_EF_Ctrl_Busy                            \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Busy])

#define RomDriver_EF_Ctrl_AutoLoad_Done                   \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_AutoLoad_Done])

#define RomDriver_EF_Ctrl_Write_Dbg_Pwd                   \
    ((void (*) (uint32_t passWdLow, uint32_t passWdHigh, uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_Dbg_Pwd])

#define RomDriver_EF_Ctrl_Read_Dbg_Pwd                    \
    ((void (*) (uint32_t *passWdLow, uint32_t *passWdHigh))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_Dbg_Pwd])

#define RomDriver_EF_Ctrl_Readlock_Dbg_Pwd                \
    ((void (*) (uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Readlock_Dbg_Pwd])

#define RomDriver_EF_Ctrl_Writelock_Dbg_Pwd               \
    ((void (*) (uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Writelock_Dbg_Pwd])

#define RomDriver_EF_Ctrl_Write_Secure_Cfg                \
    ((void (*) (EF_Ctrl_Sec_Param_Type *cfg, uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_Secure_Cfg])

#define RomDriver_EF_Ctrl_Read_Secure_Cfg                 \
    ((void (*) (EF_Ctrl_Sec_Param_Type *cfg))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_Secure_Cfg])

#define RomDriver_EF_Ctrl_Write_Secure_Boot               \
    ((void (*) (uint8_t sign[1], uint8_t aes[1], uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_Secure_Boot])

#define RomDriver_EF_Ctrl_Read_Secure_Boot                \
    ((void (*) (uint8_t sign[1], uint8_t aes[1]))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_Secure_Boot])

#define RomDriver_EF_Ctrl_Write_Sw_Usage                  \
    ((void (*) (uint32_t index, uint32_t usage, uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_Sw_Usage])

#define RomDriver_EF_Ctrl_Read_Sw_Usage                   \
    ((void (*) (uint32_t index, uint32_t *usage))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_Sw_Usage])

#define RomDriver_EF_Ctrl_Writelock_Sw_Usage              \
    ((void (*) (uint32_t index, uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Writelock_Sw_Usage])

#define RomDriver_EF_Ctrl_Write_MAC_Address               \
    ((void (*) (uint8_t mac[8], uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_MAC_Address])

#define RomDriver_EF_Ctrl_Get_Byte_Zero_Cnt               \
    ((uint32_t (*) (uint8_t val))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Get_Byte_Zero_Cnt])

#define RomDriver_EF_Ctrl_Is_All_Bits_Zero                \
    ((uint8_t (*) (uint32_t val, uint8_t start, uint8_t len))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Is_All_Bits_Zero])

#define RomDriver_EF_Ctrl_Read_MAC_Address                \
    ((int (*) (uint8_t mac[8]))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_MAC_Address])

#define RomDriver_EF_Ctrl_Writelock_MAC_Address           \
    ((void (*) (uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Writelock_MAC_Address])

#define RomDriver_EF_Ctrl_Read_Chip_ID                    \
    ((int (*) (uint8_t chipID[8]))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_Chip_ID])

#define RomDriver_EF_Ctrl_Read_Device_Info                \
    ((void (*) (Efuse_Device_Info_Type *deviceInfo))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_Device_Info])

#define RomDriver_EF_Ctrl_Is_CapCode_Empty                \
    ((uint8_t (*) (uint8_t slot, uint8_t reload))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Is_CapCode_Empty])

#define RomDriver_EF_Ctrl_Write_CapCode_Opt               \
    ((int (*) (uint8_t slot, uint8_t code, uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_CapCode_Opt])

#define RomDriver_EF_Ctrl_Read_CapCode_Opt                \
    ((int (*) (uint8_t slot, uint8_t *code, uint8_t reload))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_CapCode_Opt])

#define RomDriver_EF_Ctrl_Is_PowerOffset_Slot_Empty       \
    ((uint8_t (*) (uint8_t slot, uint8_t reload))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Is_PowerOffset_Slot_Empty])

#define RomDriver_EF_Ctrl_Write_PowerOffset_Opt           \
    ((int (*) (uint8_t slot, int8_t pwrOffset[4], uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_PowerOffset_Opt])

#define RomDriver_EF_Ctrl_Read_PowerOffset_Opt            \
    ((int (*) (uint8_t slot, int8_t pwrOffset[4], uint8_t reload))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_PowerOffset_Opt])

#define RomDriver_EF_Ctrl_Write_AES_Key                   \
    ((void (*) (uint8_t index, uint32_t *keyData, uint32_t len, uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_AES_Key])

#define RomDriver_EF_Ctrl_Read_AES_Key                    \
    ((void (*) (uint8_t index, uint32_t *keyData, uint32_t len))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_AES_Key])

#define RomDriver_EF_Ctrl_Writelock_AES_Key               \
    ((void (*) (uint8_t index, uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Writelock_AES_Key])

#define RomDriver_EF_Ctrl_Readlock_AES_Key                \
    ((void (*) (uint8_t index, uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Readlock_AES_Key])

#define RomDriver_EF_Ctrl_Program_Direct_R0               \
    ((void (*) (uint32_t index, uint32_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Program_Direct_R0])

#define RomDriver_EF_Ctrl_Read_Direct_R0                  \
    ((void (*) (uint32_t index, uint32_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_Direct_R0])

#define RomDriver_EF_Ctrl_Clear                           \
    ((void (*) (uint32_t index, uint32_t len))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Clear])

#define RomDriver_EF_Ctrl_Crc_Enable                      \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Crc_Enable])

#define RomDriver_EF_Ctrl_Crc_Is_Busy                     \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Crc_Is_Busy])

#define RomDriver_EF_Ctrl_Crc_Set_Golden                  \
    ((void (*) (uint32_t goldenValue))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Crc_Set_Golden])

#define RomDriver_EF_Ctrl_Crc_Result                      \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Crc_Result])

#define RomDriver_GLB_Get_Root_CLK_Sel                    \
    ((GLB_ROOT_CLK_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Get_Root_CLK_Sel])

#define RomDriver_GLB_Set_System_CLK_Div                  \
    ((int (*) (uint8_t hclkDiv, uint8_t bclkDiv))ROM_APITABLE[ROM_API_INDEX_GLB_Set_System_CLK_Div])

#define RomDriver_GLB_Get_BCLK_Div                        \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Get_BCLK_Div])

#define RomDriver_GLB_Get_HCLK_Div                        \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Get_HCLK_Div])

#define RomDriver_GLB_Switch_System_CLK                   \
    ((int (*) (GLB_DLL_XTAL_Type xtalType,GLB_SYS_CLK_Type clkFreq, uint8_t hclkDiv,uint8_t bclkDiv))ROM_APITABLE[ROM_API_INDEX_GLB_Switch_System_CLK])

#define RomDriver_GLB_Set_System_CLK                      \
    ((int (*) (GLB_DLL_XTAL_Type xtalType, GLB_SYS_CLK_Type clkFreq))ROM_APITABLE[ROM_API_INDEX_GLB_Set_System_CLK])

#define RomDriver_System_Core_Clock_Update_From_RC32M     \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_System_Core_Clock_Update_From_RC32M])

#define RomDriver_GLB_Enable_DLL_All_Clks                 \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Enable_DLL_All_Clks])

#define RomDriver_GLB_Enable_DLL_Clk                      \
    ((int (*) (GLB_DLL_CLK_Type dllClk))ROM_APITABLE[ROM_API_INDEX_GLB_Enable_DLL_Clk])

#define RomDriver_GLB_Disable_DLL_All_Clks                \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Disable_DLL_All_Clks])

#define RomDriver_GLB_Disable_DLL_Clk                     \
    ((int (*) (GLB_DLL_CLK_Type dllClk))ROM_APITABLE[ROM_API_INDEX_GLB_Disable_DLL_Clk])

#define RomDriver_GLB_Set_CAM_CLK                         \
    ((int (*) (uint8_t enable, GLB_CAM_CLK_Type clkSel, uint8_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_CAM_CLK])

#define RomDriver_GLB_Set_MAC154_ZIGBEE_CLK               \
    ((int (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_Set_MAC154_ZIGBEE_CLK])

#define RomDriver_GLB_Set_BLE_CLK                         \
    ((int (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_Set_BLE_CLK])

#define RomDriver_GLB_Set_I2S_CLK                         \
    ((int (*) (uint8_t enable, GLB_I2S_OUT_REF_CLK_Type outRef))ROM_APITABLE[ROM_API_INDEX_GLB_Set_I2S_CLK])

#define RomDriver_GLB_Set_USB_CLK                         \
    ((int (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_Set_USB_CLK])

#define RomDriver_GLB_Set_QDEC_CLK                        \
    ((int (*) (GLB_QDEC_CLK_Type clkSel, uint8_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_QDEC_CLK])

#define RomDriver_GLB_Set_DMA_CLK                         \
    ((int (*) (uint8_t enable, GLB_DMA_CLK_ID_Type clk))ROM_APITABLE[ROM_API_INDEX_GLB_Set_DMA_CLK])

#define RomDriver_GLB_Set_IR_CLK                          \
    ((int (*) (uint8_t enable, GLB_IR_CLK_SRC_Type clkSel, uint8_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_IR_CLK])

#define RomDriver_GLB_Set_SF_CLK                          \
    ((int (*) (uint8_t enable, GLB_SFLASH_CLK_Type clkSel, uint8_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_SF_CLK])

#define RomDriver_GLB_Set_UART_CLK                        \
    ((int (*) (uint8_t enable, HBN_UART_CLK_Type clkSel, uint8_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_UART_CLK])

#define RomDriver_GLB_Set_Chip_Out_0_CLK_Sel              \
    ((int (*) (GLB_CHIP_CLK_OUT_Type clkSel))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Chip_Out_0_CLK_Sel])

#define RomDriver_GLB_Set_Chip_Out_1_CLK_Sel              \
    ((int (*) (GLB_CHIP_CLK_OUT_Type clkSel))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Chip_Out_1_CLK_Sel])

#define RomDriver_GLB_Set_Chip_Out_2_CLK_Sel              \
    ((int (*) (GLB_CHIP_CLK_OUT_Type clkSel))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Chip_Out_2_CLK_Sel])

#define RomDriver_GLB_Set_I2C_CLK                         \
    ((int (*) (uint8_t enable, uint8_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_I2C_CLK])

#define RomDriver_GLB_Invert_ETH_RX_CLK                   \
    ((int (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_Invert_ETH_RX_CLK])

#define RomDriver_GLB_Invert_RF_TEST_O_CLK                \
    ((int (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_Invert_RF_TEST_O_CLK])

#define RomDriver_GLB_Set_SPI_CLK                         \
    ((int (*) (uint8_t enable, GLB_SPI_CLK_Type sel, uint8_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_SPI_CLK])

#define RomDriver_GLB_Invert_ETH_TX_CLK                   \
    ((int (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_Invert_ETH_TX_CLK])

#define RomDriver_GLB_Invert_ETH_REF_O_CLK                \
    ((int (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_Invert_ETH_REF_O_CLK])

#define RomDriver_GLB_Set_ETH_REF_O_CLK_Sel               \
    ((int (*) (GLB_ETH_REF_CLK_OUT_Type clkSel))ROM_APITABLE[ROM_API_INDEX_GLB_Set_ETH_REF_O_CLK_Sel])

#define RomDriver_GLB_Set_PKA_CLK_Sel                     \
    ((int (*) (GLB_PKA_CLK_Type clkSel))ROM_APITABLE[ROM_API_INDEX_GLB_Set_PKA_CLK_Sel])

#define RomDriver_GLB_SW_System_Reset                     \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_SW_System_Reset])

#define RomDriver_GLB_SW_CPU_Reset                        \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_SW_CPU_Reset])

#define RomDriver_GLB_SW_POR_Reset                        \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_SW_POR_Reset])

#define RomDriver_GLB_AHB_MCU_Software_Reset              \
    ((int (*) (DRV_AHB_MCU_SW_Type swrst))ROM_APITABLE[ROM_API_INDEX_GLB_AHB_MCU_Software_Reset])

#define RomDriver_GLB_PER_Clock_Gate                      \
    ((int (*) (GLB_AHB_CLOCK_Type module))ROM_APITABLE[ROM_API_INDEX_GLB_PER_Clock_Gate])

#define RomDriver_GLB_PER_Clock_UnGate                    \
    ((int (*) (GLB_AHB_CLOCK_Type module))ROM_APITABLE[ROM_API_INDEX_GLB_PER_Clock_UnGate])

#define RomDriver_GLB_Set_EM_Sel                          \
    ((int (*) (GLB_EM_Type emType))ROM_APITABLE[ROM_API_INDEX_GLB_Set_EM_Sel])

#define RomDriver_GLB_Set_IPROAISram                        \
    ((int (*) (int en))ROM_APITABLE[ROM_API_INDEX_GLB_Set_IPROAISram])

#define RomDriver_GLB_Set_Kys_Drv_Col                     \
    ((int (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Kys_Drv_Col])

#define RomDriver_GLB_JTAG_Sig_Swap_Set                   \
    ((int (*) (uint8_t swapSel))ROM_APITABLE[ROM_API_INDEX_GLB_JTAG_Sig_Swap_Set])

#define RomDriver_GLB_CCI_Use_IO_0_1_2_7                  \
    ((int (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_CCI_Use_IO_0_1_2_7])

#define RomDriver_GLB_CCI_Use_Jtag_Pin                    \
    ((int (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_CCI_Use_Jtag_Pin])

#define RomDriver_GLB_Swap_SPI_0_MOSI_With_MISO           \
    ((int (*) (int newState))ROM_APITABLE[ROM_API_INDEX_GLB_Swap_SPI_0_MOSI_With_MISO])

#define RomDriver_GLB_Set_SPI_0_ACT_MOD_Sel               \
    ((int (*) (GLB_SPI_PAD_ACT_AS_Type mod))ROM_APITABLE[ROM_API_INDEX_GLB_Set_SPI_0_ACT_MOD_Sel])

#define RomDriver_GLB_Set_Embedded_FLash_IO_PARM          \
    ((int (*) (uint8_t reverse, uint8_t swapIo3Io0, uint8_t swapIo2Cs))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Embedded_FLash_IO_PARM])

#define RomDriver_GLB_Set_MTimer_CLK                      \
    ((int (*) (uint8_t enable, GLB_MTIMER_CLK_Type clkSel, uint32_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_MTimer_CLK])

#define RomDriver_GLB_Set_ADC_CLK                         \
    ((int (*) (uint8_t enable, GLB_ADC_CLK_Type clkSel, uint8_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_ADC_CLK])

#define RomDriver_GLB_Set_DIG_CLK_Sel                     \
    ((int (*) (GLB_DIG_CLK_Type clkSel))ROM_APITABLE[ROM_API_INDEX_GLB_Set_DIG_CLK_Sel])

#define RomDriver_GLB_Set_DIG_32K_CLK                     \
    ((int (*) (uint8_t enable, uint8_t compensation, uint16_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_DIG_32K_CLK])

#define RomDriver_GLB_UART_Fun_Sel                        \
    ((int (*) (GLB_UART_SIG_Type sig, GLB_UART_SIG_FUN_Type fun))ROM_APITABLE[ROM_API_INDEX_GLB_UART_Fun_Sel])

#define RomDriver_GLB_Power_Off_DLL                       \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Power_Off_DLL])

#define RomDriver_GLB_Power_On_DLL                        \
    ((int (*) (GLB_DLL_XTAL_Type xtalType))ROM_APITABLE[ROM_API_INDEX_GLB_Power_On_DLL])

#define RomDriver_GLB_Select_RC32M_As_PLL_Ref             \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Select_RC32M_As_PLL_Ref])

#define RomDriver_GLB_Select_XTAL_As_PLL_Ref              \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Select_XTAL_As_PLL_Ref])

#define RomDriver_GLB_Power_On_PLL                        \
    ((int (*) (GLB_PLL_XTAL_Type xtalType))ROM_APITABLE[ROM_API_INDEX_GLB_Power_On_PLL])

#define RomDriver_GLB_Enable_PLL_All_Clks                 \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Enable_PLL_All_Clks])

#define RomDriver_GLB_Disable_PLL_All_Clks                \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Disable_PLL_All_Clks])

#define RomDriver_GLB_Enable_PLL_Clk                      \
    ((int (*) (GLB_PLL_CLK_Type pllClk))ROM_APITABLE[ROM_API_INDEX_GLB_Enable_PLL_Clk])

#define RomDriver_GLB_Disable_PLL_Clk                     \
    ((int (*) (GLB_PLL_CLK_Type pllClk))ROM_APITABLE[ROM_API_INDEX_GLB_Disable_PLL_Clk])

#define RomDriver_GLB_Power_Off_PLL                       \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Power_Off_PLL])

#define RomDriver_GLB_Set_Audio_PLL_Freq                  \
    ((int (*) (GLB_AUDIO_PLL_Type audioPLLFreq))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Audio_PLL_Freq])

#define RomDriver_GLB_Set_Flash_Id_Value                  \
    ((int (*) (uint32_t idValue))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Flash_Id_Value])

#define RomDriver_GLB_Get_Flash_Id_Value                  \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Get_Flash_Id_Value])

#define RomDriver_GLB_SetUnLockReq                        \
    ((int (*) (GLB_LOCK_REQ_Type target))ROM_APITABLE[ROM_API_INDEX_GLB_SetUnLockReq])

#define RomDriver_GLB_Trim_RC32M                          \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Trim_RC32M])

#define RomDriver_GLB_GPIO_Init                           \
    ((int (*) (GLB_GPIO_Cfg_Type *cfg))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Init])

#define RomDriver_GLB_GPIO_Func_Init                      \
    ((int (*) (GLB_GPIO_FUNC_Type gpioFun, int *pinList, uint8_t cnt))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Func_Init])

#define RomDriver_GLB_GPIO_OUTPUT_Mode_Set                \
    ((int (*) (int gpioPin, GLB_GPIO_OUTPUT_MODE_Type mode))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_OUTPUT_Mode_Set])

#define RomDriver_GLB_GPIO_INPUT_Enable                   \
    ((int (*) (int gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_INPUT_Enable])

#define RomDriver_GLB_GPIO_INPUT_Disable                  \
    ((int (*) (int gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_INPUT_Disable])

#define RomDriver_GLB_GPIO_OUTPUT_Enable                  \
    ((int (*) (int gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_OUTPUT_Enable])

#define RomDriver_GLB_GPIO_OUTPUT_Disable                 \
    ((int (*) (int gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_OUTPUT_Disable])

#define RomDriver_GLB_GPIO_Set_HZ                         \
    ((int (*) (int gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Set_HZ])

#define RomDriver_GLB_Set_Flash_Pad_HZ                    \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Flash_Pad_HZ])

#define RomDriver_GLB_Set_Psram_Pad_HZ                    \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Psram_Pad_HZ])

#define RomDriver_GLB_GPIO_Get_Fun                        \
    ((uint8_t (*) (int gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Get_Fun])

#define RomDriver_GLB_GPIO_Write                          \
    ((int (*) (int gpioPin, uint32_t val))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Write])

#define RomDriver_GLB_GPIO_Toggle                         \
    ((int (*) (int gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Toggle])

#define RomDriver_GLB_GPIO_Read                           \
    ((uint32_t (*) (int gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Read])

#define RomDriver_GLB_GPIO_IntMask                        \
    ((int (*) (int gpioPin, int intMask))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_IntMask])

#define RomDriver_GLB_GPIO_IntClear                       \
    ((int (*) (int gpioPin, int intClear))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_IntClear])

#define RomDriver_GLB_Get_GPIO_IntStatus                  \
    ((int (*) (int gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_Get_GPIO_IntStatus])

#define RomDriver_GLB_Set_GPIO_IntMod                     \
    ((int (*) (int gpioPin, GLB_GPIO_INT_CONTROL_Type intCtlMod, GLB_GPIO_INT_TRIG_Type intTrgMod))ROM_APITABLE[ROM_API_INDEX_GLB_Set_GPIO_IntMod])

#define RomDriver_GLB_Get_GPIO_IntCtlMod                  \
    ((GLB_GPIO_INT_CONTROL_Type (*) (int gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_Get_GPIO_IntCtlMod])

#define RomDriver_GLB_GPIO_Int2Mask                       \
    ((int (*) (int gpioPin, int intMask))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Int2Mask])

#define RomDriver_GLB_GPIO_Int2Clear                      \
    ((int (*) (int gpioPin, int intClear))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Int2Clear])

#define RomDriver_GLB_Get_GPIO_Int2Status                 \
    ((int (*) (int gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_Get_GPIO_Int2Status])

#define RomDriver_GLB_Set_GPIO_Int2Mod                    \
    ((int (*) (int gpioPin, GLB_GPIO_INT_CONTROL_Type intCtlMod, GLB_GPIO_INT_TRIG_Type intTrgMod))ROM_APITABLE[ROM_API_INDEX_GLB_Set_GPIO_Int2Mod])

#define RomDriver_GLB_Get_GPIO_Int2CtlMod                 \
    ((GLB_GPIO_INT_CONTROL_Type (*) (int gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_Get_GPIO_Int2CtlMod])

#define RomDriver_HBN_Mode_Enter                          \
    ((void (*) (HBN_APP_CFG_Type *cfg))ROM_APITABLE[ROM_API_INDEX_HBN_Mode_Enter])

#define RomDriver_HBN_GPIO_Wakeup_Set                     \
    ((void (*) (uint16_t gpio_wakeup_src, HBN_GPIO_INT_Trigger_Type gpio_trig_type))ROM_APITABLE[ROM_API_INDEX_HBN_GPIO_Wakeup_Set])

#define RomDriver_HBN_Power_Down_Flash                    \
    ((void (*) (SPI_Flash_Cfg_Type *flashCfg))ROM_APITABLE[ROM_API_INDEX_HBN_Power_Down_Flash])

#define RomDriver_HBN_Enable                              \
    ((void (*) (HBN_LDO_LEVEL_Type ldoLevel, HBN_LEVEL_Type hbnLevel))ROM_APITABLE[ROM_API_INDEX_HBN_Enable])

#define RomDriver_HBN_Reset                               \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Reset])

#define RomDriver_HBN_App_Reset                           \
    ((int (*) (uint8_t npXtalType, uint8_t bclkDiv, uint8_t apXtalType, uint8_t fclkDiv))ROM_APITABLE[ROM_API_INDEX_HBN_App_Reset])

#define RomDriver_HBN_Disable                             \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Disable])

#define RomDriver_HBN_Get_BOR_OUT_State                   \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Get_BOR_OUT_State])

#define RomDriver_HBN_Set_BOR_Config                      \
    ((int (*) (uint8_t enable, HBN_BOR_THRES_Type threshold, HBN_BOR_MODE_Type mode))ROM_APITABLE[ROM_API_INDEX_HBN_Set_BOR_Config])

#define RomDriver_HBN_Set_Ldo11_Aon_Vout                  \
    ((int (*) (HBN_LDO_LEVEL_Type ldoLevel))ROM_APITABLE[ROM_API_INDEX_HBN_Set_Ldo11_Aon_Vout])

#define RomDriver_HBN_Set_Ldo11_Soc_Vout                  \
    ((int (*) (HBN_LDO_LEVEL_Type ldoLevel))ROM_APITABLE[ROM_API_INDEX_HBN_Set_Ldo11_Soc_Vout])

#define RomDriver_HBN_Set_Ldo11_All_Vout                  \
    ((int (*) (HBN_LDO_LEVEL_Type ldoLevel))ROM_APITABLE[ROM_API_INDEX_HBN_Set_Ldo11_All_Vout])

#define RomDriver_HBN_32K_Sel                             \
    ((int (*) (HBN_32K_CLK_Type clkType))ROM_APITABLE[ROM_API_INDEX_HBN_32K_Sel])

#define RomDriver_HBN_Set_UART_CLK_Sel                    \
    ((int (*) (HBN_UART_CLK_Type clkSel))ROM_APITABLE[ROM_API_INDEX_HBN_Set_UART_CLK_Sel])

#define RomDriver_HBN_Set_XCLK_CLK_Sel                    \
    ((int (*) (HBN_XCLK_CLK_Type xClk))ROM_APITABLE[ROM_API_INDEX_HBN_Set_XCLK_CLK_Sel])

#define RomDriver_HBN_Set_ROOT_CLK                        \
    ((int (*) (HBN_ROOT_CLK_Type rootClk))ROM_APITABLE[ROM_API_INDEX_HBN_Set_ROOT_CLK])

#define RomDriver_HBN_Get_ROOT_CLK                        \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Get_ROOT_CLK])

#define RomDriver_HBN_Set_HRAM_slp                        \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Set_HRAM_slp])

#define RomDriver_HBN_Set_HRAM_Ret                        \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Set_HRAM_Ret])

#define RomDriver_HBN_Power_On_Xtal_32K                   \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Power_On_Xtal_32K])

#define RomDriver_HBN_Power_Off_Xtal_32K                  \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Power_Off_Xtal_32K])

#define RomDriver_HBN_Power_On_RC32K                      \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Power_On_RC32K])

#define RomDriver_HBN_Power_Off_RC32K                     \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Power_Off_RC32K])

#define RomDriver_HBN_Trim_RC32K                          \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Trim_RC32K])

#define RomDriver_HBN_Get_Status_Flag                     \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Get_Status_Flag])

#define RomDriver_HBN_Set_Status_Flag                     \
    ((int (*) (uint32_t flag))ROM_APITABLE[ROM_API_INDEX_HBN_Set_Status_Flag])

#define RomDriver_HBN_Get_Wakeup_Addr                     \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Get_Wakeup_Addr])

#define RomDriver_HBN_Set_Wakeup_Addr                     \
    ((int (*) (uint32_t addr))ROM_APITABLE[ROM_API_INDEX_HBN_Set_Wakeup_Addr])

#define RomDriver_HBN_Get_User_Boot_Config                \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Get_User_Boot_Config])

#define RomDriver_HBN_Set_User_Boot_Config                \
    ((int (*) (uint8_t ubCfg))ROM_APITABLE[ROM_API_INDEX_HBN_Set_User_Boot_Config])

#define RomDriver_HBN_Clear_RTC_Counter                   \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Clear_RTC_Counter])

#define RomDriver_HBN_Enable_RTC_Counter                  \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Enable_RTC_Counter])

#define RomDriver_HBN_Set_RTC_Timer                       \
    ((int (*) (HBN_RTC_INT_Delay_Type delay, uint32_t compValLow, uint32_t compValHigh, uint8_t compMode))ROM_APITABLE[ROM_API_INDEX_HBN_Set_RTC_Timer])

#define RomDriver_HBN_Get_RTC_Timer_Val                   \
    ((int (*) (uint32_t *valLow, uint32_t *valHigh))ROM_APITABLE[ROM_API_INDEX_HBN_Get_RTC_Timer_Val])

#define RomDriver_HBN_Clear_RTC_INT                       \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Clear_RTC_INT])

#define RomDriver_HBN_GPIO_INT_Enable                     \
    ((int (*) (HBN_GPIO_INT_Trigger_Type gpioIntTrigType))ROM_APITABLE[ROM_API_INDEX_HBN_GPIO_INT_Enable])

#define RomDriver_HBN_GPIO_INT_Disable                    \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_GPIO_INT_Disable])

#define RomDriver_HBN_Get_INT_State                       \
    ((int (*) (HBN_INT_Type irqType))ROM_APITABLE[ROM_API_INDEX_HBN_Get_INT_State])

#define RomDriver_HBN_Get_Pin_Wakeup_Mode                 \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Get_Pin_Wakeup_Mode])

#define RomDriver_HBN_Clear_IRQ                           \
    ((int (*) (HBN_INT_Type irqType))ROM_APITABLE[ROM_API_INDEX_HBN_Clear_IRQ])

#define RomDriver_HBN_Hw_Pu_Pd_Cfg                        \
    ((int (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_HBN_Hw_Pu_Pd_Cfg])

#define RomDriver_HBN_Comm_Pad_Wakeup_En_Cfg              \
    ((int (*) (uint8_t padCfg))ROM_APITABLE[ROM_API_INDEX_HBN_Comm_Pad_Wakeup_En_Cfg])

#define RomDriver_HBN_Aon_Pad_IeSmt_Cfg                   \
    ((int (*) (uint8_t padCfg))ROM_APITABLE[ROM_API_INDEX_HBN_Aon_Pad_IeSmt_Cfg])

#define RomDriver_HBN_Pin_WakeUp_Mask                     \
    ((int (*) (uint16_t maskVal))ROM_APITABLE[ROM_API_INDEX_HBN_Pin_WakeUp_Mask])

#define RomDriver_HBN_Enable_AComp0_IRQ                   \
    ((int (*) (HBN_ACOMP_INT_EDGE_Type edge))ROM_APITABLE[ROM_API_INDEX_HBN_Enable_AComp0_IRQ])

#define RomDriver_HBN_Disable_AComp0_IRQ                  \
    ((int (*) (HBN_ACOMP_INT_EDGE_Type edge))ROM_APITABLE[ROM_API_INDEX_HBN_Disable_AComp0_IRQ])

#define RomDriver_HBN_Enable_AComp1_IRQ                   \
    ((int (*) (HBN_ACOMP_INT_EDGE_Type edge))ROM_APITABLE[ROM_API_INDEX_HBN_Enable_AComp1_IRQ])

#define RomDriver_HBN_Disable_AComp1_IRQ                  \
    ((int (*) (HBN_ACOMP_INT_EDGE_Type edge))ROM_APITABLE[ROM_API_INDEX_HBN_Disable_AComp1_IRQ])

#define RomDriver_HBN_Enable_BOR_IRQ                      \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Enable_BOR_IRQ])

#define RomDriver_HBN_Disable_BOR_IRQ                     \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Disable_BOR_IRQ])

#define RomDriver_HBN_Get_Reset_Event                     \
    ((int (*) (HBN_RST_EVENT_Type event))ROM_APITABLE[ROM_API_INDEX_HBN_Get_Reset_Event])

#define RomDriver_HBN_Clear_Reset_Event                   \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Clear_Reset_Event])

#define RomDriver_HBN_GPIO_Dbg_Pull_Cfg                   \
    ((int (*) (int pupdEn, int dlyEn, uint8_t dlySec, HBN_INT_Type gpioIrq, int gpioMask))ROM_APITABLE[ROM_API_INDEX_HBN_GPIO_Dbg_Pull_Cfg])

#define RomDriver_HBN_Set_BOR_Cfg                         \
    ((int (*) (HBN_BOR_CFG_Type *cfg))ROM_APITABLE[ROM_API_INDEX_HBN_Set_BOR_Cfg])

#define RomDriver_PDS_Set_GPIO_Pad_Cfg                    \
    ((int (*) (PDS_GPIO_Type pin, PDS_GPIO_Cfg_Type *cfg))ROM_APITABLE[ROM_API_INDEX_PDS_Set_GPIO_Pad_Cfg])

#define RomDriver_PDS_GPIO_Write                          \
    ((int (*) (PDS_GPIO_GROUP_SET_Type grp, uint32_t val))ROM_APITABLE[ROM_API_INDEX_PDS_GPIO_Write])

#define RomDriver_PDS_Set_GPIO_Pad_IntMask                \
    ((int (*) (int pin, int intMask))ROM_APITABLE[ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMask])

#define RomDriver_PDS_Set_GPIO_Pad_IntMode                \
    ((int (*) (PDS_GPIO_GROUP_SET_Type grp, PDS_GPIO_INT_TRIG_Type trig))ROM_APITABLE[ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMode])

#define RomDriver_PDS_Set_GPIO_Pad_IntClr                 \
    ((int (*) (PDS_GPIO_GROUP_SET_Type grp))ROM_APITABLE[ROM_API_INDEX_PDS_Set_GPIO_Pad_IntClr])

#define RomDriver_PDS_Set_All_GPIO_IntClear               \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_PDS_Set_All_GPIO_IntClear])

#define RomDriver_PDS_Get_GPIO_Pad_IntStatus              \
    ((int (*) (PDS_GPIO_Type pin))ROM_APITABLE[ROM_API_INDEX_PDS_Get_GPIO_Pad_IntStatus])

#define RomDriver_PDS_Disable_GPIO_Keep                   \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_PDS_Disable_GPIO_Keep])

#define RomDriver_PDS_Enable                              \
    ((int (*) (PDS_CTL_Type *cfg, PDS_CTL4_Type *cfg4, uint32_t pdsSleepCnt))ROM_APITABLE[ROM_API_INDEX_PDS_Enable])

#define RomDriver_PDS_Force_Config                        \
    ((int (*) (PDS_CTL2_Type *cfg2, PDS_CTL3_Type *cfg3))ROM_APITABLE[ROM_API_INDEX_PDS_Force_Config])

#define RomDriver_PDS_RAM_Config                          \
    ((int (*) (PDS_RAM_CFG_Type *ramCfg))ROM_APITABLE[ROM_API_INDEX_PDS_RAM_Config])

#define RomDriver_PDS_Default_Level_Config                \
    ((int (*) (PDS_DEFAULT_LV_CFG_Type *defaultLvCfg, uint32_t pdsSleepCnt))ROM_APITABLE[ROM_API_INDEX_PDS_Default_Level_Config])

#define RomDriver_PDS_Wakeup_Src_En                       \
    ((int (*) (PDS_WAKEUP_SRC_Type intType, int enable))ROM_APITABLE[ROM_API_INDEX_PDS_Wakeup_Src_En])

#define RomDriver_PDS_Get_Wakeup_Src                      \
    ((int (*) (PDS_WAKEUP_SRC_Type intType))ROM_APITABLE[ROM_API_INDEX_PDS_Get_Wakeup_Src])

#define RomDriver_PDS_IntMask                             \
    ((int (*) (PDS_INT_Type intType, int intMask))ROM_APITABLE[ROM_API_INDEX_PDS_IntMask])

#define RomDriver_PDS_Get_IntStatus                       \
    ((int (*) (PDS_INT_Type intType))ROM_APITABLE[ROM_API_INDEX_PDS_Get_IntStatus])

#define RomDriver_PDS_IntClear                            \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_PDS_IntClear])

#define RomDriver_PDS_Get_PdsRfStstus                     \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_PDS_Get_PdsRfStstus])

#define RomDriver_PDS_Get_PdsStstus                       \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_PDS_Get_PdsStstus])

#define RomDriver_PDS_Clear_Reset_Event                   \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_PDS_Clear_Reset_Event])

#define RomDriver_PDS_Get_Reset_Event                     \
    ((int (*) (PDS_RST_EVENT_Type event))ROM_APITABLE[ROM_API_INDEX_PDS_Get_Reset_Event])

#define RomDriver_PDS_Auto_Enable                         \
    ((void (*) (PDS_AUTO_POWER_DOWN_CFG_Type *powerCfg, PDS_AUTO_NORMAL_CFG_Type *normalCfg, int enable))ROM_APITABLE[ROM_API_INDEX_PDS_Auto_Enable])

#define RomDriver_PDS_Manual_Force_Turn_Off               \
    ((void (*) (PDS_FORCE_Type domain))ROM_APITABLE[ROM_API_INDEX_PDS_Manual_Force_Turn_Off])

#define RomDriver_PDS_Manual_Force_Turn_On                \
    ((void (*) (PDS_FORCE_Type domain))ROM_APITABLE[ROM_API_INDEX_PDS_Manual_Force_Turn_On])

#define RomDriver_PDS_Set_KYD_Matrix_Size                 \
    ((int (*) (uint8_t col_size, uint8_t row_size))ROM_APITABLE[ROM_API_INDEX_PDS_Set_KYD_Matrix_Size])

#define RomDriver_PDS_Set_KYD_Col_Value                   \
    ((int (*) (uint8_t val))ROM_APITABLE[ROM_API_INDEX_PDS_Set_KYD_Col_Value])

#define RomDriver_PDS_Set_KYD_Row_Pull                    \
    ((int (*) (uint8_t en))ROM_APITABLE[ROM_API_INDEX_PDS_Set_KYD_Row_Pull])

#define RomDriver_PDS_Set_KYD_Wakeup_En                   \
    ((int (*) (uint8_t en))ROM_APITABLE[ROM_API_INDEX_PDS_Set_KYD_Wakeup_En])

#define RomDriver_SF_Cfg_Init_Ext_Flash_Gpio              \
    ((void (*) (uint8_t extFlashPin))ROM_APITABLE[ROM_API_INDEX_SF_Cfg_Init_Ext_Flash_Gpio])

#define RomDriver_SF_Cfg_Deinit_Ext_Flash_Gpio            \
    ((void (*) (uint8_t extFlashPin))ROM_APITABLE[ROM_API_INDEX_SF_Cfg_Deinit_Ext_Flash_Gpio])

#define RomDriver_SF_Cfg_Get_Flash_Cfg_Need_Lock          \
    ((int (*) (uint32_t flashID, SPI_Flash_Cfg_Type *pFlashCfg))ROM_APITABLE[ROM_API_INDEX_SF_Cfg_Get_Flash_Cfg_Need_Lock])

#define RomDriver_SF_Cfg_Init_Flash_Gpio                  \
    ((void (*) (uint8_t flashPinCfg, uint8_t restoreDefault))ROM_APITABLE[ROM_API_INDEX_SF_Cfg_Init_Flash_Gpio])

#define RomDriver_SF_Cfg_Flash_Identify                   \
    ((uint32_t (*) (uint8_t callFromFlash, uint32_t autoScan, uint32_t flashPinCfg, uint8_t restoreDefault, SPI_Flash_Cfg_Type *pFlashCfg))ROM_APITABLE[ROM_API_INDEX_SF_Cfg_Flash_Identify])

#define RomDriver_SF_Ctrl_SetDelay                        \
    ((void (*) (uint8_t clkDelay))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_SetDelay])

#define RomDriver_SF_Ctrl_Enable                          \
    ((void (*) (const SF_Ctrl_Cfg_Type *cfg))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Enable])

#define RomDriver_SF_Ctrl_Psram_Init                      \
    ((void (*) (SF_Ctrl_Psram_Cfg *sfCtrlPsramCfg))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Psram_Init])

#define RomDriver_SF_Ctrl_Get_Clock_Delay                 \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Get_Clock_Delay])

#define RomDriver_SF_Ctrl_Set_Clock_Delay                 \
    ((void (*) (uint8_t delay))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Set_Clock_Delay])

#define RomDriver_SF_Ctrl_Cmds_Set                        \
    ((void (*) (SF_Ctrl_Cmds_Cfg *cmdsCfg, SF_Ctrl_Select sel))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Cmds_Set])

#define RomDriver_SF_Ctrl_Burst_Toggle_Set                \
    ((void (*) (uint8_t burstToggleEn, uint8_t mode))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Burst_Toggle_Set])

#define RomDriver_SF_Ctrl_Select_Pad                      \
    ((void (*) (SF_Ctrl_Pad_Select sel))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Select_Pad])

#define RomDriver_SF_Ctrl_Select_Bank                     \
    ((void (*) (SF_Ctrl_Select sel))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Select_Bank])

#define RomDriver_SF_Ctrl_Set_Owner                       \
    ((void (*) (SF_Ctrl_Owner_Type owner))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Set_Owner])

#define RomDriver_SF_Ctrl_Disable                         \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Disable])

#define RomDriver_SF_Ctrl_AES_Enable_BE                   \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Enable_BE])

#define RomDriver_SF_Ctrl_AES_Enable_LE                   \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Enable_LE])

#define RomDriver_SF_Ctrl_AES_Set_Region                  \
    ((void (*) (uint8_t region, uint8_t enable, uint8_t hwKey, uint32_t startAddr, uint32_t endAddr, uint8_t locked))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Set_Region])

#define RomDriver_SF_Ctrl_AES_Set_Key                     \
    ((void (*) (uint8_t region, uint8_t *key, SF_Ctrl_AES_Key_Type keyType))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Set_Key])

#define RomDriver_SF_Ctrl_AES_Set_Key_BE                  \
    ((void (*) (uint8_t region, uint8_t *key, SF_Ctrl_AES_Key_Type keyType))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Set_Key_BE])

#define RomDriver_SF_Ctrl_AES_Set_IV                      \
    ((void (*) (uint8_t region, uint8_t *iv, uint32_t addrOffset))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Set_IV])

#define RomDriver_SF_Ctrl_AES_Set_IV_BE                   \
    ((void (*) (uint8_t region, uint8_t *iv, uint32_t addrOffset))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Set_IV_BE])

#define RomDriver_SF_Ctrl_AES_Enable                      \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Enable])

#define RomDriver_SF_Ctrl_AES_Disable                     \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Disable])

#define RomDriver_SF_Ctrl_Is_AES_Enable                   \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Is_AES_Enable])

#define RomDriver_SF_Ctrl_Set_Flash_Image_Offset          \
    ((void (*) (uint32_t addrOffset))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Set_Flash_Image_Offset])

#define RomDriver_SF_Ctrl_Get_Flash_Image_Offset          \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Get_Flash_Image_Offset])

#define RomDriver_SF_Ctrl_SendCmd                         \
    ((void (*) (SF_Ctrl_Cmd_Cfg_Type *cfg))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_SendCmd])

#define RomDriver_SF_Ctrl_Flash_Read_Icache_Set           \
    ((void (*) (SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Flash_Read_Icache_Set])

#define RomDriver_SF_Ctrl_Psram_Write_Icache_Set          \
    ((void (*) (SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Psram_Write_Icache_Set])

#define RomDriver_SF_Ctrl_Psram_Read_Icache_Set           \
    ((void (*) (SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Psram_Read_Icache_Set])

#define RomDriver_SF_Ctrl_GetBusyState                    \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_GetBusyState])

#define RomDriver_SFlash_Init                             \
    ((void (*) (const SF_Ctrl_Cfg_Type *pSfCtrlCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Init])

#define RomDriver_SFlash_SetSPIMode                       \
    ((int (*) (SF_Ctrl_Mode_Type mode))ROM_APITABLE[ROM_API_INDEX_SFlash_SetSPIMode])

#define RomDriver_SFlash_Read_Reg                         \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg, uint8_t regIndex, uint8_t *regValue, uint8_t regLen))ROM_APITABLE[ROM_API_INDEX_SFlash_Read_Reg])

#define RomDriver_SFlash_Write_Reg                        \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg, uint8_t regIndex, uint8_t *regValue, uint8_t regLen))ROM_APITABLE[ROM_API_INDEX_SFlash_Write_Reg])

#define RomDriver_SFlash_Read_Reg_With_Cmd                \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg, uint8_t readRegCmd, uint8_t *regValue, uint8_t regLen))ROM_APITABLE[ROM_API_INDEX_SFlash_Read_Reg_With_Cmd])

#define RomDriver_SFlash_Write_Reg_With_Cmd               \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg, uint8_t writeRegCmd, uint8_t *regValue, uint8_t regLen))ROM_APITABLE[ROM_API_INDEX_SFlash_Write_Reg_With_Cmd])

#define RomDriver_SFlash_Busy                             \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Busy])

#define RomDriver_SFlash_Write_Enable                     \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Write_Enable])

#define RomDriver_SFlash_Qspi_Enable                      \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Qspi_Enable])

#define RomDriver_SFlash_Volatile_Reg_Write_Enable        \
    ((void (*) (SPI_Flash_Cfg_Type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Volatile_Reg_Write_Enable])

#define RomDriver_SFlash_Chip_Erase                       \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Chip_Erase])

#define RomDriver_SFlash_Sector_Erase                     \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg, uint32_t secNum))ROM_APITABLE[ROM_API_INDEX_SFlash_Sector_Erase])

#define RomDriver_SFlash_Blk32_Erase                      \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg, uint32_t blkNum))ROM_APITABLE[ROM_API_INDEX_SFlash_Blk32_Erase])

#define RomDriver_SFlash_Blk64_Erase                      \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg, uint32_t blkNum))ROM_APITABLE[ROM_API_INDEX_SFlash_Blk64_Erase])

#define RomDriver_SFlash_Erase                            \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg, uint32_t startaddr, uint32_t endaddr))ROM_APITABLE[ROM_API_INDEX_SFlash_Erase])

#define RomDriver_SFlash_Program                          \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_SFlash_Program])

#define RomDriver_SFlash_GetUniqueId                      \
    ((void (*) (uint8_t *data, uint8_t idLen))ROM_APITABLE[ROM_API_INDEX_SFlash_GetUniqueId])

#define RomDriver_SFlash_GetJedecId                       \
    ((void (*) (SPI_Flash_Cfg_Type *flashCfg, uint8_t *data))ROM_APITABLE[ROM_API_INDEX_SFlash_GetJedecId])

#define RomDriver_SFlash_GetDeviceId                      \
    ((void (*) (uint8_t *data))ROM_APITABLE[ROM_API_INDEX_SFlash_GetDeviceId])

#define RomDriver_SFlash_Powerdown                        \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_SFlash_Powerdown])

#define RomDriver_SFlash_Releae_Powerdown                 \
    ((void (*) (SPI_Flash_Cfg_Type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Releae_Powerdown])

#define RomDriver_SFlash_Restore_From_Powerdown           \
    ((int (*) (SPI_Flash_Cfg_Type *pFlashCfg, uint8_t flashContRead))ROM_APITABLE[ROM_API_INDEX_SFlash_Restore_From_Powerdown])

#define RomDriver_SFlash_SetBurstWrap                     \
    ((void (*) (SPI_Flash_Cfg_Type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_SetBurstWrap])

#define RomDriver_SFlash_DisableBurstWrap                 \
    ((void (*) (SPI_Flash_Cfg_Type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_DisableBurstWrap])

#define RomDriver_SFlash_Software_Reset                   \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Software_Reset])

#define RomDriver_SFlash_Reset_Continue_Read              \
    ((void (*) (SPI_Flash_Cfg_Type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Reset_Continue_Read])

#define RomDriver_SFlash_Set_IDbus_Cfg                    \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg, SF_Ctrl_IO_Type ioMode, uint8_t contRead, uint32_t addr, uint32_t len))ROM_APITABLE[ROM_API_INDEX_SFlash_Set_IDbus_Cfg])

#define RomDriver_SFlash_IDbus_Read_Enable                \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg, SF_Ctrl_IO_Type ioMode, uint8_t contRead))ROM_APITABLE[ROM_API_INDEX_SFlash_IDbus_Read_Enable])

#define RomDriver_SFlash_Cache_Read_Enable                \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg, SF_Ctrl_IO_Type ioMode, uint8_t contRead, uint8_t wayDisable))ROM_APITABLE[ROM_API_INDEX_SFlash_Cache_Read_Enable])

#define RomDriver_SFlash_Read                             \
    ((int (*) (SPI_Flash_Cfg_Type *flashCfg, SF_Ctrl_IO_Type ioMode, uint8_t contRead, uint32_t addr, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_SFlash_Read])

#define RomDriver_UART_SetBaudRate                        \
    ((int (*) (UART_ID_Type uartId, uint32_t baudRate))ROM_APITABLE[ROM_API_INDEX_UART_SetBaudRate])

#define RomDriver_UART_Init                               \
    ((int (*) (UART_ID_Type uartId, UART_CFG_Type *uartCfg))ROM_APITABLE[ROM_API_INDEX_UART_Init])

#define RomDriver_UART_DeInit                             \
    ((int (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_DeInit])

#define RomDriver_UART_FifoConfig                         \
    ((int (*) (UART_ID_Type uartId, UART_FifoCfg_Type *fifoCfg))ROM_APITABLE[ROM_API_INDEX_UART_FifoConfig])

#define RomDriver_UART_Enable                             \
    ((int (*) (UART_ID_Type uartId, UART_Direction_Type direct))ROM_APITABLE[ROM_API_INDEX_UART_Enable])

#define RomDriver_UART_Disable                            \
    ((int (*) (UART_ID_Type uartId, UART_Direction_Type direct))ROM_APITABLE[ROM_API_INDEX_UART_Disable])

#define RomDriver_UART_DmaEnable                          \
    ((int (*) (UART_ID_Type uartId, UART_Direction_Type direct))ROM_APITABLE[ROM_API_INDEX_UART_DmaEnable])

#define RomDriver_UART_DmaDisable                         \
    ((int (*) (UART_ID_Type uartId, UART_Direction_Type direct))ROM_APITABLE[ROM_API_INDEX_UART_DmaDisable])

#define RomDriver_UART_SetTxDataLength                    \
    ((int (*) (UART_ID_Type uartId, uint16_t length))ROM_APITABLE[ROM_API_INDEX_UART_SetTxDataLength])

#define RomDriver_UART_SetRxDataLength                    \
    ((int (*) (UART_ID_Type uartId, uint16_t length))ROM_APITABLE[ROM_API_INDEX_UART_SetRxDataLength])

#define RomDriver_UART_SetRxTimeoutValue                  \
    ((int (*) (UART_ID_Type uartId, uint8_t time))ROM_APITABLE[ROM_API_INDEX_UART_SetRxTimeoutValue])

#define RomDriver_UART_SetDeglitchCount                   \
    ((int (*) (UART_ID_Type uartId, uint8_t deglitchCnt))ROM_APITABLE[ROM_API_INDEX_UART_SetDeglitchCount])

#define RomDriver_UART_ApplyAbrResult                     \
    ((int (*) (UART_ID_Type uartId, UART_AutoBaudDetection_Type autoBaudDet))ROM_APITABLE[ROM_API_INDEX_UART_ApplyAbrResult])

#define RomDriver_UART_SetRtsValue                        \
    ((int (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_SetRtsValue])

#define RomDriver_UART_ClrRtsValue                        \
    ((int (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_ClrRtsValue])

#define RomDriver_UART_SetTxValue                         \
    ((int (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_SetTxValue])

#define RomDriver_UART_ClrTxValue                         \
    ((int (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_ClrTxValue])

#define RomDriver_UART_TxFreeRun                          \
    ((int (*) (UART_ID_Type uartId, int txFreeRun))ROM_APITABLE[ROM_API_INDEX_UART_TxFreeRun])

#define RomDriver_UART_AutoBaudDetection                  \
    ((int (*) (UART_ID_Type uartId, int autoBaud))ROM_APITABLE[ROM_API_INDEX_UART_AutoBaudDetection])

#define RomDriver_UART_SetAllowableError0X55              \
    ((int (*) (UART_ID_Type uartId, uint8_t allowableError))ROM_APITABLE[ROM_API_INDEX_UART_SetAllowableError0X55])

#define RomDriver_UART_GetBitWidth0X55                    \
    ((int (*) (UART_ID_Type uartId, uint16_t *width))ROM_APITABLE[ROM_API_INDEX_UART_GetBitWidth0X55])

#define RomDriver_UART_SetRS485                           \
    ((int (*) (UART_ID_Type uartId, int enable, UART_RS485Polarity_Type polarity))ROM_APITABLE[ROM_API_INDEX_UART_SetRS485])

#define RomDriver_UART_TxFifoClear                        \
    ((int (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_TxFifoClear])

#define RomDriver_UART_RxFifoClear                        \
    ((int (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_RxFifoClear])

#define RomDriver_UART_IntMask                            \
    ((int (*) (UART_ID_Type uartId, UART_INT_Type intType, int intMask))ROM_APITABLE[ROM_API_INDEX_UART_IntMask])

#define RomDriver_UART_IntClear                           \
    ((int (*) (UART_ID_Type uartId, UART_INT_Type intType))ROM_APITABLE[ROM_API_INDEX_UART_IntClear])

#define RomDriver_UART_SendData                           \
    ((int (*) (UART_ID_Type uartId, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_UART_SendData])

#define RomDriver_UART_SendDataBlock                      \
    ((int (*) (UART_ID_Type uartId, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_UART_SendDataBlock])

#define RomDriver_UART_ReceiveData                        \
    ((uint32_t (*) (UART_ID_Type uartId, uint8_t *data, uint32_t maxLen))ROM_APITABLE[ROM_API_INDEX_UART_ReceiveData])

#define RomDriver_UART_GetAutoBaudCount                   \
    ((uint16_t (*) (UART_ID_Type uartId, UART_AutoBaudDetection_Type autoBaudDet))ROM_APITABLE[ROM_API_INDEX_UART_GetAutoBaudCount])

#define RomDriver_UART_GetRxByteCount                     \
    ((uint16_t (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_GetRxByteCount])

#define RomDriver_UART_GetTxFifoCount                     \
    ((uint8_t (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_GetTxFifoCount])

#define RomDriver_UART_GetRxFifoCount                     \
    ((uint8_t (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_GetRxFifoCount])

#define RomDriver_UART_GetIntStatus                       \
    ((int (*) (UART_ID_Type uartId, UART_INT_Type intType))ROM_APITABLE[ROM_API_INDEX_UART_GetIntStatus])

#define RomDriver_UART_GetTxBusBusyStatus                 \
    ((int (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_GetTxBusBusyStatus])

#define RomDriver_UART_GetRxBusBusyStatus                 \
    ((int (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_GetRxBusBusyStatus])

#define RomDriver_UART_GetOverflowStatus                  \
    ((int (*) (UART_ID_Type uartId, UART_Overflow_Type overflow))ROM_APITABLE[ROM_API_INDEX_UART_GetOverflowStatus])

#define RomDriver_XIP_SFlash_Opt_Enter                    \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_Opt_Enter])

#define RomDriver_XIP_SFlash_Opt_Exit                     \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_Opt_Exit])

#define RomDriver_XIP_SFlash_State_Save                   \
    ((int (*) (SPI_Flash_Cfg_Type *pFlashCfg, uint32_t *offset))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_State_Save])

#define RomDriver_XIP_SFlash_State_Restore                \
    ((int (*) (SPI_Flash_Cfg_Type *pFlashCfg, uint32_t offset))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_State_Restore])

#define RomDriver_XIP_SFlash_Erase_Need_Lock              \
    ((int (*) (SPI_Flash_Cfg_Type *pFlashCfg, uint32_t startaddr, uint32_t endaddr))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_Erase_Need_Lock])

#define RomDriver_XIP_SFlash_Write_Need_Lock              \
    ((int (*) (SPI_Flash_Cfg_Type *pFlashCfg, uint32_t addr, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_Write_Need_Lock])

#define RomDriver_XIP_SFlash_Read_Need_Lock               \
    ((int (*) (SPI_Flash_Cfg_Type *pFlashCfg, uint32_t addr, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_Read_Need_Lock])

#define RomDriver_XIP_SFlash_GetJedecId_Need_Lock         \
    ((int (*) (SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_GetJedecId_Need_Lock])

#define RomDriver_XIP_SFlash_GetDeviceId_Need_Lock        \
    ((int (*) (SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_GetDeviceId_Need_Lock])

#define RomDriver_XIP_SFlash_GetUniqueId_Need_Lock        \
    ((int (*) (SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data, uint8_t idLen))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_GetUniqueId_Need_Lock])

#define RomDriver_XIP_SFlash_Read_Via_Cache_Need_Lock     \
    ((int (*) (uint32_t addr, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_Read_Via_Cache_Need_Lock])

#define RomDriver_IR_TxInit                               \
    ((int (*) (IR_TxCfg_Type *irTxCfg))ROM_APITABLE[ROM_API_INDEX_IR_TxInit])

#define RomDriver_IR_TxPulseWidthConfig                   \
    ((int (*) (IR_TxPulseWidthCfg_Type *irTxPulseWidthCfg))ROM_APITABLE[ROM_API_INDEX_IR_TxPulseWidthConfig])

#define RomDriver_IR_TxSWMPulseWidthConfig                \
    ((int (*) (IR_TxSWMPulseWidthCfg_Type *irTxSWMPulseWidthCfg))ROM_APITABLE[ROM_API_INDEX_IR_TxSWMPulseWidthConfig])

#define RomDriver_IR_RxInit                               \
    ((int (*) (IR_RxCfg_Type *irRxCfg))ROM_APITABLE[ROM_API_INDEX_IR_RxInit])

#define RomDriver_IR_DeInit                               \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_IR_DeInit])

#define RomDriver_IR_Enable                               \
    ((int (*) (IR_Direction_Type direct))ROM_APITABLE[ROM_API_INDEX_IR_Enable])

#define RomDriver_IR_Disable                              \
    ((int (*) (IR_Direction_Type direct))ROM_APITABLE[ROM_API_INDEX_IR_Disable])

#define RomDriver_IR_TxSWM                                \
    ((int (*) (int txSWM))ROM_APITABLE[ROM_API_INDEX_IR_TxSWM])

#define RomDriver_IR_RxFIFOClear                          \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_IR_RxFIFOClear])

#define RomDriver_IR_SendData                             \
    ((int (*) (IR_Word_Type irWord, uint32_t data))ROM_APITABLE[ROM_API_INDEX_IR_SendData])

#define RomDriver_IR_SWMSendData                          \
    ((int (*) (uint16_t *data, uint8_t length))ROM_APITABLE[ROM_API_INDEX_IR_SWMSendData])

#define RomDriver_IR_SendCommand                          \
    ((int (*) (uint32_t word1, uint32_t word0))ROM_APITABLE[ROM_API_INDEX_IR_SendCommand])

#define RomDriver_IR_SWMSendCommand                       \
    ((int (*) (uint16_t *data, uint8_t length))ROM_APITABLE[ROM_API_INDEX_IR_SWMSendCommand])

#define RomDriver_IR_SendNEC                              \
    ((int (*) (uint8_t address, uint8_t command))ROM_APITABLE[ROM_API_INDEX_IR_SendNEC])

#define RomDriver_IR_IntMask                              \
    ((int (*) (IR_INT_Type intType, int intMask))ROM_APITABLE[ROM_API_INDEX_IR_IntMask])

#define RomDriver_IR_ClrIntStatus                         \
    ((int (*) (IR_INT_Type intType))ROM_APITABLE[ROM_API_INDEX_IR_ClrIntStatus])

#define RomDriver_IR_GetIntStatus                         \
    ((int (*) (IR_INT_Type intType))ROM_APITABLE[ROM_API_INDEX_IR_GetIntStatus])

#define RomDriver_IR_GetRxFIFOStatus                      \
    ((int (*) (IR_FifoStatus_Type fifoSts))ROM_APITABLE[ROM_API_INDEX_IR_GetRxFIFOStatus])

#define RomDriver_IR_ReceiveData                          \
    ((uint32_t (*) (IR_Word_Type irWord))ROM_APITABLE[ROM_API_INDEX_IR_ReceiveData])

#define RomDriver_IR_SWMReceiveData                       \
    ((uint8_t (*) (uint16_t *data, uint8_t length))ROM_APITABLE[ROM_API_INDEX_IR_SWMReceiveData])

#define RomDriver_IR_ReceiveNEC                           \
    ((int (*) (uint8_t *address, uint8_t *command))ROM_APITABLE[ROM_API_INDEX_IR_ReceiveNEC])

#define RomDriver_IR_GetRxDataBitCount                    \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_IR_GetRxDataBitCount])

#define RomDriver_IR_GetRxFIFOCount                       \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_IR_GetRxFIFOCount])

#define RomDriver_IR_LearnToInit                          \
    ((IR_RxMode_Type (*) (uint32_t *data, uint8_t *length))ROM_APITABLE[ROM_API_INDEX_IR_LearnToInit])

#define RomDriver_IR_LearnToReceive                       \
    ((uint8_t (*) (IR_RxMode_Type mode, uint32_t *data))ROM_APITABLE[ROM_API_INDEX_IR_LearnToReceive])

#define RomDriver_IR_LearnToSend                          \
    ((int (*) (IR_RxMode_Type mode, uint32_t *data, uint8_t length))ROM_APITABLE[ROM_API_INDEX_IR_LearnToSend])

#define RomDriver_IR_LEDInit                              \
    ((int (*) (HBN_XCLK_CLK_Type clk, uint8_t div, uint8_t unit, uint8_t code0H, uint8_t code0L, uint8_t code1H, uint8_t code1L))ROM_APITABLE[ROM_API_INDEX_IR_LEDInit])

#define RomDriver_IR_LEDSend                              \
    ((int (*) (uint32_t data))ROM_APITABLE[ROM_API_INDEX_IR_LEDSend])

#define RomDriver_KYS_Init                                \
    ((int (*) (KYS_CFG_Type *kysCfg))ROM_APITABLE[ROM_API_INDEX_KYS_Init])

#define RomDriver_KYS_Enable                              \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_KYS_Enable])

#define RomDriver_KYS_Disable                             \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_KYS_Disable])

#define RomDriver_KYS_IntMask                             \
    ((int (*) (int intMask))ROM_APITABLE[ROM_API_INDEX_KYS_IntMask])

#define RomDriver_KYS_IntClear                            \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_KYS_IntClear])

#define RomDriver_KYS_GetIntStatus                        \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_KYS_GetIntStatus])

#define RomDriver_KYS_GetKeycode                          \
    ((uint8_t (*) (KYS_Keycode_Type keycode, uint8_t *col, uint8_t *row))ROM_APITABLE[ROM_API_INDEX_KYS_GetKeycode])

#define RomDriver_Psram_Init                              \
    ((void (*) (SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_Cmds_Cfg *cmdsCfg, SF_Ctrl_Psram_Cfg *sfCtrlPsramCfg))ROM_APITABLE[ROM_API_INDEX_Psram_Init])

#define RomDriver_Psram_ReadReg                           \
    ((void (*) (SPI_Psram_Cfg_Type *psramCfg, uint8_t *regValue))ROM_APITABLE[ROM_API_INDEX_Psram_ReadReg])

#define RomDriver_Psram_WriteReg                          \
    ((void (*) (SPI_Psram_Cfg_Type *psramCfg, uint8_t *regValue))ROM_APITABLE[ROM_API_INDEX_Psram_WriteReg])

#define RomDriver_Psram_SetDriveStrength                  \
    ((int (*) (SPI_Psram_Cfg_Type *psramCfg))ROM_APITABLE[ROM_API_INDEX_Psram_SetDriveStrength])

#define RomDriver_Psram_SetBurstWrap                      \
    ((int (*) (SPI_Psram_Cfg_Type *psramCfg))ROM_APITABLE[ROM_API_INDEX_Psram_SetBurstWrap])

#define RomDriver_Psram_ReadId                            \
    ((void (*) (SPI_Psram_Cfg_Type *psramCfg, uint8_t *data))ROM_APITABLE[ROM_API_INDEX_Psram_ReadId])

#define RomDriver_Psram_EnterQuadMode                     \
    ((int (*) (SPI_Psram_Cfg_Type *psramCfg))ROM_APITABLE[ROM_API_INDEX_Psram_EnterQuadMode])

#define RomDriver_Psram_ExitQuadMode                      \
    ((int (*) (SPI_Psram_Cfg_Type *psramCfg))ROM_APITABLE[ROM_API_INDEX_Psram_ExitQuadMode])

#define RomDriver_Psram_ToggleBurstLength                 \
    ((int (*) (SPI_Psram_Cfg_Type *psramCfg, PSRAM_Ctrl_Mode ctrlMode))ROM_APITABLE[ROM_API_INDEX_Psram_ToggleBurstLength])

#define RomDriver_Psram_SoftwareReset                     \
    ((int (*) (SPI_Psram_Cfg_Type *psramCfg, PSRAM_Ctrl_Mode ctrlMode))ROM_APITABLE[ROM_API_INDEX_Psram_SoftwareReset])

#define RomDriver_Psram_Set_IDbus_Cfg                     \
    ((int (*) (SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr, uint32_t len))ROM_APITABLE[ROM_API_INDEX_Psram_Set_IDbus_Cfg])

#define RomDriver_Psram_Cache_Write_Set                   \
    ((int (*) (SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_IO_Type ioMode, int wtEn, int wbEn, int waEn))ROM_APITABLE[ROM_API_INDEX_Psram_Cache_Write_Set])

#define RomDriver_Psram_Write                             \
    ((int (*) (SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_Psram_Write])

#define RomDriver_Psram_Read                              \
    ((int (*) (SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_Psram_Read])

#define RomDriver_TIMER_GetCompValue                      \
    ((uint32_t (*) (TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo))ROM_APITABLE[ROM_API_INDEX_TIMER_GetCompValue])

#define RomDriver_TIMER_SetCompValue                      \
    ((void (*) (TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo, uint32_t val))ROM_APITABLE[ROM_API_INDEX_TIMER_SetCompValue])

#define RomDriver_TIMER_GetCounterValue                   \
    ((uint32_t (*) (TIMER_Chan_Type timerCh))ROM_APITABLE[ROM_API_INDEX_TIMER_GetCounterValue])

#define RomDriver_TIMER_GetMatchStatus                    \
    ((int (*) (TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo))ROM_APITABLE[ROM_API_INDEX_TIMER_GetMatchStatus])

#define RomDriver_TIMER_GetPreloadValue                   \
    ((uint32_t (*) (TIMER_Chan_Type timerCh))ROM_APITABLE[ROM_API_INDEX_TIMER_GetPreloadValue])

#define RomDriver_TIMER_SetPreloadValue                   \
    ((void (*) (TIMER_Chan_Type timerCh, uint32_t val))ROM_APITABLE[ROM_API_INDEX_TIMER_SetPreloadValue])

#define RomDriver_TIMER_SetPreloadSrc                     \
    ((void (*) (TIMER_Chan_Type timerCh, TIMER_PreLoad_Trig_Type plSrc))ROM_APITABLE[ROM_API_INDEX_TIMER_SetPreloadSrc])

#define RomDriver_TIMER_SetCountMode                      \
    ((void (*) (TIMER_Chan_Type timerCh, TIMER_CountMode_Type countMode))ROM_APITABLE[ROM_API_INDEX_TIMER_SetCountMode])

#define RomDriver_TIMER_ClearIntStatus                    \
    ((void (*) (TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo))ROM_APITABLE[ROM_API_INDEX_TIMER_ClearIntStatus])

#define RomDriver_TIMER_SetDiv                            \
    ((void (*) (int timer, uint8_t div))ROM_APITABLE[ROM_API_INDEX_TIMER_SetDiv])

#define RomDriver_TIMER_Init                              \
    ((int (*) (TIMER_CFG_Type *timerCfg))ROM_APITABLE[ROM_API_INDEX_TIMER_Init])

#define RomDriver_TIMER_Enable                            \
    ((void (*) (TIMER_Chan_Type timerCh))ROM_APITABLE[ROM_API_INDEX_TIMER_Enable])

#define RomDriver_TIMER_Disable                           \
    ((void (*) (TIMER_Chan_Type timerCh))ROM_APITABLE[ROM_API_INDEX_TIMER_Disable])

#define RomDriver_TIMER_IntMask                           \
    ((void (*) (TIMER_Chan_Type timerCh, TIMER_INT_Type intType, int intMask))ROM_APITABLE[ROM_API_INDEX_TIMER_IntMask])

#define RomDriver_WDT_Set_Clock                           \
    ((void (*) (TIMER_ClkSrc_Type clkSrc, uint8_t div))ROM_APITABLE[ROM_API_INDEX_WDT_Set_Clock])

#define RomDriver_WDT_GetMatchValue                       \
    ((uint16_t (*) (void))ROM_APITABLE[ROM_API_INDEX_WDT_GetMatchValue])

#define RomDriver_WDT_SetCompValue                        \
    ((void (*) (uint16_t val))ROM_APITABLE[ROM_API_INDEX_WDT_SetCompValue])

#define RomDriver_WDT_GetCounterValue                     \
    ((uint16_t (*) (void))ROM_APITABLE[ROM_API_INDEX_WDT_GetCounterValue])

#define RomDriver_WDT_ResetCounterValue                   \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_WDT_ResetCounterValue])

#define RomDriver_WDT_GetResetStatus                      \
    ((int (*) (void))ROM_APITABLE[ROM_API_INDEX_WDT_GetResetStatus])

#define RomDriver_WDT_ClearResetStatus                    \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_WDT_ClearResetStatus])

#define RomDriver_WDT_Enable                              \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_WDT_Enable])

#define RomDriver_WDT_Disable                             \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_WDT_Disable])

#define RomDriver_WDT_ClearInt                            \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_WDT_ClearInt])

#define RomDriver_WDT_IntMask                             \
    ((void (*) (WDT_INT_Type intType, int intMask))ROM_APITABLE[ROM_API_INDEX_WDT_IntMask])

#define RomDriver_arch_memcpy                             \
    ((void * (*) (void *dst, const void *src, uint32_t n))ROM_APITABLE[ROM_API_INDEX_arch_memcpy])

#define RomDriver_arch_memcpy4                            \
    ((uint32_t * (*) (uint32_t *dst, const uint32_t *src, uint32_t n))ROM_APITABLE[ROM_API_INDEX_arch_memcpy4])

#define RomDriver_arch_memcpy_fast                        \
    ((void * (*) (void *pdst, const void *psrc, uint32_t n))ROM_APITABLE[ROM_API_INDEX_arch_memcpy_fast])

#define RomDriver_arch_memset4                            \
    ((uint32_t * (*) (uint32_t *dst, const uint32_t val, uint32_t n))ROM_APITABLE[ROM_API_INDEX_arch_memset4])

#define RomDriver_arch_memcmp                             \
    ((int (*) (const void *s1, const void *s2, uint32_t n))ROM_APITABLE[ROM_API_INDEX_arch_memcmp])

#define RomDriver_memcopy_to_fifo                         \
    ((void (*) (void *fifo_addr, uint8_t *data, uint32_t length))ROM_APITABLE[ROM_API_INDEX_memcopy_to_fifo])

#define RomDriver_fifocopy_to_mem                         \
    ((void (*) (void *fifo_addr, uint8_t *data, uint32_t length))ROM_APITABLE[ROM_API_INDEX_fifocopy_to_mem])

#define RomDriver_utils_crc16                             \
    ((uint16_t (*) (const void * dataIn, uint32_t len))ROM_APITABLE[ROM_API_INDEX_utils_crc16])

#define RomDriver_utils_crc32_accumulate                  \
    ((uint32_t (*) (uint32_t crc, const void *dataIn, uint32_t len))ROM_APITABLE[ROM_API_INDEX_utils_crc32_accumulate])

#define RomDriver_utils_crc32                             \
    ((uint32_t (*) (const void *dataIn, uint32_t len))ROM_APITABLE[ROM_API_INDEX_utils_crc32])

#define RomDriver_utils_crc32_stream_init                 \
    ((void (*) (struct crc32_stream_ctx *ctx))ROM_APITABLE[ROM_API_INDEX_utils_crc32_stream_init])

#define RomDriver_utils_crc32_stream_feed                 \
    ((void (*) (struct crc32_stream_ctx *ctx, uint8_t data))ROM_APITABLE[ROM_API_INDEX_utils_crc32_stream_feed])

#define RomDriver_utils_crc32_stream_feed_block           \
    ((void (*) (struct crc32_stream_ctx *ctx, const uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_utils_crc32_stream_feed_block])

#define RomDriver_utils_crc32_stream_results              \
    ((uint32_t (*) (struct crc32_stream_ctx *ctx))ROM_APITABLE[ROM_API_INDEX_utils_crc32_stream_results])

#define RomDriver_utils_crc16_ccitt                       \
    ((uint16_t (*) (const void *dataIn, uint32_t len))ROM_APITABLE[ROM_API_INDEX_utils_crc16_ccitt])

#endif /* __DRV_ROMDRIVER_H__ */

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

#include "drv_romdriver.h"


__attribute__((weak)) ATTR_TCM_SECTION
int AON_Output_Float_DCDC18(void){
    return RomDriver_AON_Output_Float_DCDC18();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Output_Float_LDO15_RF(void){
    return RomDriver_AON_Output_Float_LDO15_RF();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Output_LDO15_RF_Ctrl_by_Pd(void){
    return RomDriver_AON_Output_LDO15_RF_Ctrl_by_Pd();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Output_Pulldown_DCDC18(void){
    return RomDriver_AON_Output_Pulldown_DCDC18();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Output_Pulldown_LDO15_RF(void){
    return RomDriver_AON_Output_Pulldown_LDO15_RF();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Power_Off_BG(void){
    return RomDriver_AON_Power_Off_BG();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Power_Off_LDO15_RF(void){
    return RomDriver_AON_Power_Off_LDO15_RF();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Power_Off_MBG(void){
    return RomDriver_AON_Power_Off_MBG();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Power_Off_MicBias(void){
    return RomDriver_AON_Power_Off_MicBias();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Power_Off_SFReg(void){
    return RomDriver_AON_Power_Off_SFReg();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Power_Off_XTAL(void){
    return RomDriver_AON_Power_Off_XTAL();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Power_On_BG(void){
    return RomDriver_AON_Power_On_BG();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Power_On_LDO15_RF(void){
    return RomDriver_AON_Power_On_LDO15_RF();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Power_On_MBG(void){
    return RomDriver_AON_Power_On_MBG();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Power_On_MicBias(void){
    return RomDriver_AON_Power_On_MicBias();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Power_On_SFReg(void){
    return RomDriver_AON_Power_On_SFReg();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Power_On_XTAL(void){
    return RomDriver_AON_Power_On_XTAL();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Set_Xtal_CapCode(uint8_t capIn, uint8_t capOut){
    return RomDriver_AON_Set_Xtal_CapCode(capIn,capOut);
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Trim_DcdcDis(void){
    return RomDriver_AON_Trim_DcdcDis();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Trim_DcdcVoutSel(void){
    return RomDriver_AON_Trim_DcdcVoutSel();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Trim_DcdcVoutTrim(void){
    return RomDriver_AON_Trim_DcdcVoutTrim();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Trim_Ldo11socVoutTrim(void){
    return RomDriver_AON_Trim_Ldo11socVoutTrim();
}

__attribute__((weak)) ATTR_TCM_SECTION
int AON_Trim_Usb20RcalCode(void){
    return RomDriver_AON_Trim_Usb20RcalCode();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t AON_Get_Xtal_CapCode(void){
    return RomDriver_AON_Get_Xtal_CapCode();
}

__attribute__((weak)) ATTR_TCM_SECTION
int CPU_MTimer_Delay_MS(uint32_t cnt){
    return RomDriver_CPU_MTimer_Delay_MS(cnt);
}

__attribute__((weak)) ATTR_TCM_SECTION
int CPU_MTimer_Delay_US(uint32_t cnt){
    return RomDriver_CPU_MTimer_Delay_US(cnt);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint16_t CPU_Get_MTimer_Toggle_Cnt(void){
    return RomDriver_CPU_Get_MTimer_Toggle_Cnt();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t CPU_Get_MTimer_Clock(void){
    return RomDriver_CPU_Get_MTimer_Clock();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t Clock_Audio_ADC_Clock_Get(void){
    return RomDriver_Clock_Audio_ADC_Clock_Get();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t Clock_Get_ISP_Clk(void){
    return RomDriver_Clock_Get_ISP_Clk();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t Clock_Get_PSRAMB_Clk(void){
    return RomDriver_Clock_Get_PSRAMB_Clk();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_Type type){
    return RomDriver_Clock_Peripheral_Clock_Get(type);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t Clock_Peripheral_Clock_Get_Flash(void){
    return RomDriver_Clock_Peripheral_Clock_Get_Flash();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t Clock_Peripheral_Clock_Get_Uart(void){
    return RomDriver_Clock_Peripheral_Clock_Get_Uart();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_Type type){
    return RomDriver_Clock_System_Clock_Get(type);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t SystemCoreClockGet(void){
    return RomDriver_SystemCoreClockGet();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint64_t CPU_Get_CPU_Cycle(void){
    return RomDriver_CPU_Get_CPU_Cycle();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint64_t CPU_Get_MTimer_Counter(void){
    return RomDriver_CPU_Get_MTimer_Counter();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint64_t CPU_Get_MTimer_MS(void){
    return RomDriver_CPU_Get_MTimer_MS();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint64_t CPU_Get_MTimer_US(void){
    return RomDriver_CPU_Get_MTimer_US();
}

__attribute__((weak)) ATTR_TCM_SECTION
void CPU_Set_MTimer_Toggle_Cnt(uint16_t cnt){
    return RomDriver_CPU_Set_MTimer_Toggle_Cnt(cnt);
}

__attribute__((weak)) ATTR_TCM_SECTION
int EF_Ctrl_AutoLoad_Done(void){
    return RomDriver_EF_Ctrl_AutoLoad_Done();
}

__attribute__((weak)) ATTR_TCM_SECTION
int EF_Ctrl_Busy(void){
    return RomDriver_EF_Ctrl_Busy();
}

__attribute__((weak)) ATTR_TCM_SECTION
int EF_Ctrl_Crc_Is_Busy(void){
    return RomDriver_EF_Ctrl_Crc_Is_Busy();
}

__attribute__((weak)) ATTR_TCM_SECTION
int EF_Ctrl_Crc_Result(void){
    return RomDriver_EF_Ctrl_Crc_Result();
}

__attribute__((weak)) ATTR_TCM_SECTION
int EF_Ctrl_Get_Customer_PIDVID(uint16_t pid[1], uint16_t vid[1]){
    return RomDriver_EF_Ctrl_Get_Customer_PIDVID(pid,vid);
}

__attribute__((weak)) ATTR_TCM_SECTION
int EF_Ctrl_Read_Bus_Remap(void){
    return RomDriver_EF_Ctrl_Read_Bus_Remap();
}

__attribute__((weak)) ATTR_TCM_SECTION
int EF_Ctrl_Read_Chip_ID(uint8_t chipID[8]){
    return RomDriver_EF_Ctrl_Read_Chip_ID(chipID);
}

__attribute__((weak)) ATTR_TCM_SECTION
int EF_Ctrl_Read_MAC_Address(uint8_t mac[6]){
    return RomDriver_EF_Ctrl_Read_MAC_Address(mac);
}

__attribute__((weak)) ATTR_TCM_SECTION
int EF_Ctrl_Read_MAC_Address_Raw(uint8_t mac[7]){
    return RomDriver_EF_Ctrl_Read_MAC_Address_Raw(mac);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t EF_Ctrl_Get_Trim_Parity(uint32_t val, uint8_t len){
    return RomDriver_EF_Ctrl_Get_Trim_Parity(val,len);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t EF_Ctrl_Is_All_Bits_Zero(uint32_t val, uint8_t start, uint8_t len){
    return RomDriver_EF_Ctrl_Is_All_Bits_Zero(val,start,len);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t EF_Ctrl_Read_FlashDlyCoe(void){
    return RomDriver_EF_Ctrl_Read_FlashDlyCoe();
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Clear(uint32_t index, uint32_t len){
    return RomDriver_EF_Ctrl_Clear(index,len);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Crc_Enable(void){
    return RomDriver_EF_Ctrl_Crc_Enable();
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Crc_Set_Golden(uint32_t goldenValue){
    return RomDriver_EF_Ctrl_Crc_Set_Golden(goldenValue);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Load_Efuse_R0(void){
    return RomDriver_EF_Ctrl_Load_Efuse_R0();
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Program_Direct(uint32_t offset, uint32_t *pword, uint32_t count){
    return RomDriver_EF_Ctrl_Program_Direct(offset,pword,count);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Program_Direct_R0(uint32_t index, uint32_t *data, uint32_t len){
    return RomDriver_EF_Ctrl_Program_Direct_R0(index,data,len);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Program_Efuse_0(void){
    return RomDriver_EF_Ctrl_Program_Efuse_0();
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_ADC_Gain_Trim(Efuse_ADC_Gain_Coeff_Type *trim){
    return RomDriver_EF_Ctrl_Read_ADC_Gain_Trim(trim);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len){
    return RomDriver_EF_Ctrl_Read_AES_Key(index,keyData,len);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_Dbg_Pwd(uint8_t slot, uint32_t *passWdLow, uint32_t *passWdHigh){
    return RomDriver_EF_Ctrl_Read_Dbg_Pwd(slot,passWdLow,passWdHigh);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_DcdcDis_Trim(Efuse_Ana_DcdcDis_Type *trim){
    return RomDriver_EF_Ctrl_Read_DcdcDis_Trim(trim);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_DcdcVoutSel_Trim(Efuse_Ana_DcdcVoutSel_Type *trim){
    return RomDriver_EF_Ctrl_Read_DcdcVoutSel_Trim(trim);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_DcdcVoutTrim_Trim(Efuse_Ana_DcdcVoutTrim_Type *trim){
    return RomDriver_EF_Ctrl_Read_DcdcVoutTrim_Trim(trim);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_Device_Info(Efuse_Device_Info_Type *deviceInfo){
    return RomDriver_EF_Ctrl_Read_Device_Info(deviceInfo);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_Direct(uint32_t offset, uint32_t *pword, uint32_t count){
    return RomDriver_EF_Ctrl_Read_Direct(offset,pword,count);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_Direct_R0(uint32_t index, uint32_t *data, uint32_t len){
    return RomDriver_EF_Ctrl_Read_Direct_R0(index,data,len);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_Ldo11socVoutTrim_Trim(Efuse_Ana_Ldo11socVoutTrim_Type *trim){
    return RomDriver_EF_Ctrl_Read_Ldo11socVoutTrim_Trim(trim);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_Ldo18ioBypass_Trim(Efuse_Ana_Ldo18ioBypass_Type *trim){
    return RomDriver_EF_Ctrl_Read_Ldo18ioBypass_Trim(trim);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_Ldo18ioVoutSel_Trim(Efuse_Ana_Ldo18ioVoutSel_Type *trim){
    return RomDriver_EF_Ctrl_Read_Ldo18ioVoutSel_Trim(trim);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_Ldo18ioVoutTrim_Trim(Efuse_Ana_Ldo18ioVoutTrim_Type *trim){
    return RomDriver_EF_Ctrl_Read_Ldo18ioVoutTrim_Trim(trim);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_Ldo33VoutTrim_Trim(Efuse_Ana_Ldo33VoutTrim_Type *trim){
    return RomDriver_EF_Ctrl_Read_Ldo33VoutTrim_Trim(trim);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_RC32K_Trim(Efuse_Ana_RC32K_Trim_Type *trim){
    return RomDriver_EF_Ctrl_Read_RC32K_Trim(trim);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_RC32M_Trim(Efuse_Ana_RC32M_Trim_Type *trim){
    return RomDriver_EF_Ctrl_Read_RC32M_Trim(trim);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_Secure_Boot(uint8_t aes[2]){
    return RomDriver_EF_Ctrl_Read_Secure_Boot(aes);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_Secure_Cfg(EF_Ctrl_Sec_Param_Type *cfg){
    return RomDriver_EF_Ctrl_Read_Secure_Cfg(cfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_Sw_Usage(uint32_t index, uint32_t *usage){
    return RomDriver_EF_Ctrl_Read_Sw_Usage(index,usage);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_TSEN_Trim(Efuse_TSEN_Refcode_Corner_Type *trim){
    return RomDriver_EF_Ctrl_Read_TSEN_Trim(trim);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Read_Usb20RcalCode_Trim(Efuse_Ana_Usb20RcalCode_Type *trim){
    return RomDriver_EF_Ctrl_Read_Usb20RcalCode_Trim(trim);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Readlock_AES_Key(uint8_t index, uint8_t program){
    return RomDriver_EF_Ctrl_Readlock_AES_Key(index,program);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Readlock_Dbg_Pwd(uint8_t program){
    return RomDriver_EF_Ctrl_Readlock_Dbg_Pwd(program);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Set_sf_key_re_sel(uint8_t ef_sf_key_re_sel){
    return RomDriver_EF_Ctrl_Set_sf_key_re_sel(ef_sf_key_re_sel);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Sw_AHB_Clk_0(void){
    return RomDriver_EF_Ctrl_Sw_AHB_Clk_0();
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Write_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len, uint8_t program){
    return RomDriver_EF_Ctrl_Write_AES_Key(index,keyData,len,program);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Write_Dbg_Pwd(uint8_t slot, uint32_t passWdLow, uint32_t passWdHigh, uint8_t program){
    return RomDriver_EF_Ctrl_Write_Dbg_Pwd(slot,passWdLow,passWdHigh,program);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Write_MAC_Address(uint8_t mac[6], uint8_t program){
    return RomDriver_EF_Ctrl_Write_MAC_Address(mac,program);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Write_Secure_Boot(uint8_t sign[1], uint8_t aes[1], uint8_t program){
    return RomDriver_EF_Ctrl_Write_Secure_Boot(sign,aes,program);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Write_Secure_Cfg(EF_Ctrl_Sec_Param_Type *cfg, uint8_t program){
    return RomDriver_EF_Ctrl_Write_Secure_Cfg(cfg,program);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Write_Sw_Usage(uint32_t index, uint32_t usage, uint8_t program){
    return RomDriver_EF_Ctrl_Write_Sw_Usage(index,usage,program);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Writelock_AES_Key(uint8_t index, uint8_t program){
    return RomDriver_EF_Ctrl_Writelock_AES_Key(index,program);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Writelock_Dbg_Pwd(uint8_t program){
    return RomDriver_EF_Ctrl_Writelock_Dbg_Pwd(program);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Writelock_MAC_Address(uint8_t program){
    return RomDriver_EF_Ctrl_Writelock_MAC_Address(program);
}

__attribute__((weak)) ATTR_TCM_SECTION
void EF_Ctrl_Writelock_Sw_Usage(uint32_t index, uint8_t program){
    return RomDriver_EF_Ctrl_Writelock_Sw_Usage(index,program);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_AHB_MCU_Software_Reset(uint8_t swrst){
    return RomDriver_GLB_AHB_MCU_Software_Reset(swrst);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_BMX_TO_Init(BMX_TO_Cfg_Type *BmxCfg){
    return RomDriver_GLB_BMX_TO_Init(BmxCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Clr_BMX_TO_Status(void){
    return RomDriver_GLB_Clr_BMX_TO_Status();
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Clr_Reset_Reason(void){
    return RomDriver_GLB_Clr_Reset_Reason();
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Config_SDIO_Host_Interrupt_CPU(uint8_t enable){
    return RomDriver_GLB_Config_SDIO_Host_Interrupt_CPU(enable);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Config_SDIO_Host_Reset_System(uint8_t enable){
    return RomDriver_GLB_Config_SDIO_Host_Reset_System(enable);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Config_WIFI_PLL(uint8_t xtalType, const GLB_WA_PLL_Cfg_Type * pllCfgList){
    return RomDriver_GLB_Config_WIFI_PLL(xtalType,pllCfgList);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Disrst_Set(uint8_t enable, GLB_DISRST_Type disrst){
    return RomDriver_GLB_Disrst_Set(enable,disrst);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Fast_Power_On_WIFIPLL(const GLB_WA_PLL_Cfg_Type *const cfg, uint8_t waitStable){
    return RomDriver_GLB_Fast_Power_On_WIFIPLL(cfg,waitStable);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Fast_Power_On_XTAL_40M_And_WIFIPLL(void){
    return RomDriver_GLB_Fast_Power_On_XTAL_40M_And_WIFIPLL();
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Fast_Set_MCU_System_CLK(uint8_t clkFreq){
    return RomDriver_GLB_Fast_Set_MCU_System_CLK(clkFreq);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Fast_Set_MCU_System_CLK_Div(uint8_t mcuClkDiv, uint8_t mcuPBclkDiv){
    return RomDriver_GLB_Fast_Set_MCU_System_CLK_Div(mcuClkDiv,mcuPBclkDiv);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Get_Auto_Calc_Xtal_Type(uint8_t *calcXtalType){
    return RomDriver_GLB_Get_Auto_Calc_Xtal_Type(calcXtalType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Get_MCU_System_CLK_Div(uint8_t *mcuClkDiv, uint8_t *mcuPBclkDiv){
    return RomDriver_GLB_Get_MCU_System_CLK_Div(mcuClkDiv,mcuPBclkDiv);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Get_PAD_Bonging_to_GND_Sts(void){
    return RomDriver_GLB_Get_PAD_Bonging_to_GND_Sts();
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Get_Reset_Reason(GLB_RESET_RECORD_Type *reason){
    return RomDriver_GLB_Get_Reset_Reason(reason);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_IR_RX_GPIO_Sel(uint8_t gpio){
    return RomDriver_GLB_IR_RX_GPIO_Sel(gpio);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Invert_ETH_REF_O_CLK(uint8_t enable){
    return RomDriver_GLB_Invert_ETH_REF_O_CLK(enable);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Invert_ETH_RX_CLK(uint8_t enable){
    return RomDriver_GLB_Invert_ETH_RX_CLK(enable);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Invert_ETH_TX_CLK(uint8_t enable){
    return RomDriver_GLB_Invert_ETH_TX_CLK(enable);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_MCU_SW_System_Reset(uint8_t sysPart){
    return RomDriver_GLB_MCU_SW_System_Reset(sysPart);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_PER_Clock_Gate(uint64_t ips){
    return RomDriver_GLB_PER_Clock_Gate(ips);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_PER_Clock_UnGate(uint64_t ips){
    return RomDriver_GLB_PER_Clock_UnGate(ips);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_PLL_CGEN_Clock_Gate(uint8_t clk){
    return RomDriver_GLB_PLL_CGEN_Clock_Gate(clk);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_PLL_CGEN_Clock_UnGate(uint8_t clk){
    return RomDriver_GLB_PLL_CGEN_Clock_UnGate(clk);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Platform_Wakeup_PDS_Enable(uint8_t enable){
    return RomDriver_GLB_Platform_Wakeup_PDS_Enable(enable);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Power_Off_WIFIPLL(void){
    return RomDriver_GLB_Power_Off_WIFIPLL();
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Power_On_WIFIPLL(const GLB_WA_PLL_Cfg_Type *const cfg, uint8_t waitStable){
    return RomDriver_GLB_Power_On_WIFIPLL(cfg,waitStable);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Power_On_XTAL_And_PLL_CLK(uint8_t xtalType, uint8_t pllType){
    return RomDriver_GLB_Power_On_XTAL_And_PLL_CLK(xtalType,pllType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_SPI_Sig_Swap_Set(uint8_t group, uint8_t swap){
    return RomDriver_GLB_SPI_Sig_Swap_Set(group,swap);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_SW_CPU_Reset(void){
    return RomDriver_GLB_SW_CPU_Reset();
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_SW_POR_Reset(void){
    return RomDriver_GLB_SW_POR_Reset();
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_SW_System_Reset(void){
    return RomDriver_GLB_SW_System_Reset();
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Sel_MCU_TMR_GPIO_Clock(uint8_t gpioPin){
    return RomDriver_GLB_Sel_MCU_TMR_GPIO_Clock(gpioPin);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_ADC_CLK(uint8_t enable, GLB_ADC_CLK_Type clkSel, uint8_t div){
    return RomDriver_GLB_Set_ADC_CLK(enable,clkSel,div);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_Audio_ADC_CLK(uint8_t enable, uint8_t div){
    return RomDriver_GLB_Set_Audio_ADC_CLK(enable,div);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_Audio_AUTO_CLK(uint8_t divEn){
    return RomDriver_GLB_Set_Audio_AUTO_CLK(divEn);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_Audio_SOLO_CLK(uint8_t enable, uint8_t div){
    return RomDriver_GLB_Set_Audio_SOLO_CLK(enable,div);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_Auto_Calc_Xtal_Type(uint8_t calcXtalType){
    return RomDriver_GLB_Set_Auto_Calc_Xtal_Type(calcXtalType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_CAM_CLK(uint8_t enable, GLB_CAM_CLK_Type clkSel, uint8_t div){
    return RomDriver_GLB_Set_CAM_CLK(enable,clkSel,div);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_CPU_Reset_Address(uint8_t coreID, uint32_t addr){
    return RomDriver_GLB_Set_CPU_Reset_Address(coreID,addr);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_Chip_Clock_Out0_Sel(GLB_CHIP_CLK_OUT_0_Type clkOutType){
    return RomDriver_GLB_Set_Chip_Clock_Out0_Sel(clkOutType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_Chip_Clock_Out1_Sel(GLB_CHIP_CLK_OUT_1_Type clkOutType){
    return RomDriver_GLB_Set_Chip_Clock_Out1_Sel(clkOutType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_Chip_Clock_Out2_Sel(GLB_CHIP_CLK_OUT_2_Type clkOutType){
    return RomDriver_GLB_Set_Chip_Clock_Out2_Sel(clkOutType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_Chip_Clock_Out3_Sel(GLB_CHIP_CLK_OUT_3_Type clkOutType){
    return RomDriver_GLB_Set_Chip_Clock_Out3_Sel(clkOutType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_DBI_CLK(uint8_t enable, GLB_DBI_CLK_Type clkSel, uint8_t div){
    return RomDriver_GLB_Set_DBI_CLK(enable,clkSel,div);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_DIG_32K_CLK(uint8_t enable, uint8_t compensationEn, uint16_t div){
    return RomDriver_GLB_Set_DIG_32K_CLK(enable,compensationEn,div);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_DIG_512K_CLK(uint8_t enable, uint8_t compensationEn, uint8_t div){
    return RomDriver_GLB_Set_DIG_512K_CLK(enable,compensationEn,div);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_DIG_CLK_Sel(uint8_t clkSel){
    return RomDriver_GLB_Set_DIG_CLK_Sel(clkSel);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_DMA_CLK(uint8_t enable, uint8_t clk){
    return RomDriver_GLB_Set_DMA_CLK(enable,clk);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_EM_Sel(uint8_t emType){
    return RomDriver_GLB_Set_EM_Sel(emType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_ETH_REF_O_CLK_Sel(uint8_t clkSel){
    return RomDriver_GLB_Set_ETH_REF_O_CLK_Sel(clkSel);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_Flash_Id_Value(uint32_t idValue){
    return RomDriver_GLB_Set_Flash_Id_Value(idValue);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_I2C_CLK(uint8_t enable, GLB_I2C_CLK_Type clkSel, uint8_t div){
    return RomDriver_GLB_Set_I2C_CLK(enable,clkSel,div);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_I2S_CLK(uint8_t refClkEn, uint8_t refClkDiv, uint8_t inRef, uint8_t outRef){
    return RomDriver_GLB_Set_I2S_CLK(refClkEn,refClkDiv,inRef,outRef);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_ISP_Muxpll_80M_Sel(uint8_t clkSel){
    return RomDriver_GLB_Set_ISP_Muxpll_80M_Sel(clkSel);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_MCU_Muxpll_160M_Sel(uint8_t clkSel){
    return RomDriver_GLB_Set_MCU_Muxpll_160M_Sel(clkSel);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_MCU_Muxpll_80M_Sel(uint8_t clkSel){
    return RomDriver_GLB_Set_MCU_Muxpll_80M_Sel(clkSel);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_MCU_System_CLK(uint8_t clkFreq){
    return RomDriver_GLB_Set_MCU_System_CLK(clkFreq);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_MCU_System_CLK_Div(uint8_t mcuClkDiv, uint8_t mcuPBclkDiv){
    return RomDriver_GLB_Set_MCU_System_CLK_Div(mcuClkDiv,mcuPBclkDiv);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_PDM_IO_Sel(uint8_t ioSel){
    return RomDriver_GLB_Set_PDM_IO_Sel(ioSel);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_PKA_CLK_Sel(uint8_t clkSel){
    return RomDriver_GLB_Set_PKA_CLK_Sel(clkSel);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_PSRAMB_CLK_Sel(uint8_t enable, GLB_PSRAMB_PLL_Type clkSel, uint8_t div){
    return RomDriver_GLB_Set_PSRAMB_CLK_Sel(enable,clkSel,div);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_PWM1_IO_Sel(uint8_t ioSel){
    return RomDriver_GLB_Set_PWM1_IO_Sel(ioSel);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_Peripheral_DMA_CN(uint8_t peri, uint8_t cn){
    return RomDriver_GLB_Set_Peripheral_DMA_CN(peri,cn);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_SF_CLK(uint8_t enable, uint8_t clkSel, uint8_t div){
    return RomDriver_GLB_Set_SF_CLK(enable,clkSel,div);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_SFlash2_IO_PARM(uint8_t swapIo3Io0){
    return RomDriver_GLB_Set_SFlash2_IO_PARM(swapIo3Io0);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_SFlash_IO_PARM(uint8_t selEmbedded, uint8_t swapIo3Io0, uint8_t swapIo2Cs){
    return RomDriver_GLB_Set_SFlash_IO_PARM(selEmbedded,swapIo3Io0,swapIo2Cs);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_SPI_0_ACT_MOD_Sel(uint8_t mod){
    return RomDriver_GLB_Set_SPI_0_ACT_MOD_Sel(mod);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_SPI_CLK(uint8_t enable, GLB_SPI_CLK_Type clkSel, uint8_t div){
    return RomDriver_GLB_Set_SPI_CLK(enable,clkSel,div);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_SRAM_PARM(uint32_t value){
    return RomDriver_GLB_Set_SRAM_PARM(value);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_SRAM_RET(uint32_t value){
    return RomDriver_GLB_Set_SRAM_RET(value);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_SRAM_SLP(uint32_t value){
    return RomDriver_GLB_Set_SRAM_SLP(value);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_SSC_CLK_From_WIFIPLL(uint8_t enable){
    return RomDriver_GLB_Set_SSC_CLK_From_WIFIPLL(enable);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_Slave_Grp_0_CLK(GLB_SLAVE_GRP_0_Type slave, uint8_t enable, GLB_SLAVE_GRP_0_CLK_SEL_Type clkSel, uint32_t div){
    return RomDriver_GLB_Set_Slave_Grp_0_CLK(slave,enable,clkSel,div);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_UART_CLK(uint8_t enable, uint8_t clkSel, uint8_t div){
    return RomDriver_GLB_Set_UART_CLK(enable,clkSel,div);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_USB_CLK_From_WIFIPLL(uint8_t enable){
    return RomDriver_GLB_Set_USB_CLK_From_WIFIPLL(enable);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Simple_Set_MCU_System_CLK(uint8_t clkFreq, uint8_t mcuClkDiv, uint8_t mcuPBclkDiv){
    return RomDriver_GLB_Simple_Set_MCU_System_CLK(clkFreq,mcuClkDiv,mcuPBclkDiv);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Swap_SPI_0_MOSI_With_MISO(int newState){
    return RomDriver_GLB_Swap_SPI_0_MOSI_With_MISO(newState);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Trim_Ldo18ioBypass(void){
    return RomDriver_GLB_Trim_Ldo18ioBypass();
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Trim_Ldo18ioVoutSel(void){
    return RomDriver_GLB_Trim_Ldo18ioVoutSel();
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Trim_Ldo18ioVoutTrim(void){
    return RomDriver_GLB_Trim_Ldo18ioVoutTrim();
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_UART_Fun_Sel(uint8_t sig, uint8_t fun){
    return RomDriver_GLB_UART_Fun_Sel(sig,fun);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_UART_Fun_Sel_Swap(uint8_t sig, uint8_t fun){
    return RomDriver_GLB_UART_Fun_Sel_Swap(sig,fun);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_UART_Sig_Swap_Set(uint8_t group, uint8_t swap){
    return RomDriver_GLB_UART_Sig_Swap_Set(group,swap);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_WIFIPLL_Ref_Clk_Sel(uint8_t refClk){
    return RomDriver_GLB_WIFIPLL_Ref_Clk_Sel(refClk);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t GLB_Get_Flash_Id_Value(void){
    return RomDriver_GLB_Get_Flash_Id_Value();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t GLB_Get_SRAM_PARM(void){
    return RomDriver_GLB_Get_SRAM_PARM();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t GLB_Get_SRAM_RET(void){
    return RomDriver_GLB_Get_SRAM_RET();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t GLB_Get_SRAM_SLP(void){
    return RomDriver_GLB_Get_SRAM_SLP();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t GLB_Get_BMX_TO_Status(void){
    return RomDriver_GLB_Get_BMX_TO_Status();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t GLB_Get_Core_Type(void){
    return RomDriver_GLB_Get_Core_Type();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t GLB_Get_ISP_Muxpll_80M_Sel(void){
    return RomDriver_GLB_Get_ISP_Muxpll_80M_Sel();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t GLB_Get_MCU_Muxpll_160M_Sel(void){
    return RomDriver_GLB_Get_MCU_Muxpll_160M_Sel();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t GLB_Get_MCU_Muxpll_80M_Sel(void){
    return RomDriver_GLB_Get_MCU_Muxpll_80M_Sel();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t GLB_Get_Package_Type(void){
    return RomDriver_GLB_Get_Package_Type();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t GLB_UART_Find_Sig_By_Fun(uint8_t fun){
    return RomDriver_GLB_UART_Find_Sig_By_Fun(fun);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t GLB_UART_Get_Fun(uint8_t sig){
    return RomDriver_GLB_UART_Get_Fun(sig);
}

__attribute__((weak)) ATTR_TCM_SECTION
void GLB_Power_Down_Ldo18ioVout(void){
    return RomDriver_GLB_Power_Down_Ldo18ioVout();
}

__attribute__((weak)) ATTR_TCM_SECTION
void glb_40M_delay_us(uint32_t us){
    return RomDriver_glb_40M_delay_us(us);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Clr_GPIO_IntStatus(uint8_t gpioPin){
    return RomDriver_GLB_Clr_GPIO_IntStatus(gpioPin);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Embedded_Flash_Pad_Enable(uint8_t swapIo2Cs){
    return RomDriver_GLB_Embedded_Flash_Pad_Enable(swapIo2Cs);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Clr(uint8_t gpioPin){
    return RomDriver_GLB_GPIO_Clr(gpioPin);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Fifo_Clear(void){
    return RomDriver_GLB_GPIO_Fifo_Clear();
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Fifo_Disable(void){
    return RomDriver_GLB_GPIO_Fifo_Disable();
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Fifo_Enable(void){
    return RomDriver_GLB_GPIO_Fifo_Enable();
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Fifo_GetIntStatus(uint8_t intType){
    return RomDriver_GLB_GPIO_Fifo_GetIntStatus(intType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Fifo_Init(GLB_GPIO_FIFO_CFG_Type *cfg){
    return RomDriver_GLB_GPIO_Fifo_Init(cfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Fifo_IntClear(uint8_t intType){
    return RomDriver_GLB_GPIO_Fifo_IntClear(intType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Fifo_IntMask(uint8_t intType, int intMask){
    return RomDriver_GLB_GPIO_Fifo_IntMask(intType,intMask);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Fifo_Push(uint16_t *data, uint16_t len){
    return RomDriver_GLB_GPIO_Fifo_Push(data,len);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Func_Init(uint8_t gpioFun, uint8_t *pinList, uint8_t cnt){
    return RomDriver_GLB_GPIO_Func_Init(gpioFun,pinList,cnt);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Init(GLB_GPIO_Cfg_Type *cfg){
    return RomDriver_GLB_GPIO_Init(cfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Input_Disable(uint8_t gpioPin){
    return RomDriver_GLB_GPIO_Input_Disable(gpioPin);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Input_Enable(uint8_t gpioPin){
    return RomDriver_GLB_GPIO_Input_Enable(gpioPin);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_IntClear(int gpioPin, int intClear){
    return RomDriver_GLB_GPIO_IntClear(gpioPin,intClear);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_IntMask(uint8_t gpioPin, int intMask){
    return RomDriver_GLB_GPIO_IntMask(gpioPin,intMask);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Int_Init(GLB_GPIO_INT_Cfg_Type *intCfg){
    return RomDriver_GLB_GPIO_Int_Init(intCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Output_Disable(uint8_t gpioPin){
    return RomDriver_GLB_GPIO_Output_Disable(gpioPin);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Output_Enable(uint8_t gpioPin){
    return RomDriver_GLB_GPIO_Output_Enable(gpioPin);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Pad_LeadOut_Sts(uint8_t gpioPin){
    return RomDriver_GLB_GPIO_Pad_LeadOut_Sts(gpioPin);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Set(uint8_t gpioPin){
    return RomDriver_GLB_GPIO_Set(gpioPin);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Set_HZ(uint8_t gpioPin){
    return RomDriver_GLB_GPIO_Set_HZ(gpioPin);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Toggle(int gpioPin){
    return RomDriver_GLB_GPIO_Toggle(gpioPin);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_GPIO_Write(uint8_t gpioPin, uint32_t val){
    return RomDriver_GLB_GPIO_Write(gpioPin,val);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Get_GPIO_IntStatus(uint8_t gpioPin){
    return RomDriver_GLB_Get_GPIO_IntStatus(gpioPin);
}

__attribute__((weak)) ATTR_TCM_SECTION
int GLB_Set_GPIO_IntMod(int gpioPin, GLB_GPIO_INT_CONTROL_Type intCtlMod, GLB_GPIO_INT_TRIG_Type intTrgMod){
    return RomDriver_GLB_Set_GPIO_IntMod(gpioPin,intCtlMod,intTrgMod);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t GLB_GPIO_Fifo_GetCount(void){
    return RomDriver_GLB_GPIO_Fifo_GetCount();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t GLB_GPIO_Read(uint8_t gpioPin){
    return RomDriver_GLB_GPIO_Read(gpioPin);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t GLB_GPIO_Get_Fun(uint8_t gpioPin){
    return RomDriver_GLB_GPIO_Get_Fun(gpioPin);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_32K_Sel(uint8_t clkType){
    return RomDriver_HBN_32K_Sel(clkType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Aon_Pad_Cfg(uint8_t aonPadHwCtrlEn, uint8_t aonGpio, HBN_AON_PAD_CFG_Type *aonPadCfg){
    return RomDriver_HBN_Aon_Pad_Cfg(aonPadHwCtrlEn,aonGpio,aonPadCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Aon_Pad_Cfg_Set(uint8_t aonPadHwCtrlEn, uint8_t aonGpio){
    return RomDriver_HBN_Aon_Pad_Cfg_Set(aonPadHwCtrlEn,aonGpio);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Aon_Pad_Ctrl(uint32_t aonPadCtl1, uint32_t aonPadCtl2){
    return RomDriver_HBN_Aon_Pad_Ctrl(aonPadCtl1,aonPadCtl2);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Aon_Pad_WakeUpCfg(int puPdEn, uint8_t trigMode, uint32_t maskVal, int dlyEn, uint8_t dlySec){
    return RomDriver_HBN_Aon_Pad_WakeUpCfg(puPdEn,trigMode,maskVal,dlyEn,dlySec);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Clear_Gpio_Keep(uint8_t gpioKeep){
    return RomDriver_HBN_Clear_Gpio_Keep(gpioKeep);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Clear_IRQ(uint8_t irqType){
    return RomDriver_HBN_Clear_IRQ(irqType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Clear_PDS_Gpio_Keep(uint8_t gpioKeep){
    return RomDriver_HBN_Clear_PDS_Gpio_Keep(gpioKeep);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Clear_RTC_Counter(void){
    return RomDriver_HBN_Clear_RTC_Counter();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Clear_RTC_INT(void){
    return RomDriver_HBN_Clear_RTC_INT();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Disable_AComp0_IRQ(void){
    return RomDriver_HBN_Disable_AComp0_IRQ();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Disable_AComp1_IRQ(void){
    return RomDriver_HBN_Disable_AComp1_IRQ();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Disable_AComp_IRQ(uint8_t acompId, HBN_ACOMP_INT_EDGE_Type edge){
    return RomDriver_HBN_Disable_AComp_IRQ(acompId,edge);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Disable_BOD_IRQ(void){
    return RomDriver_HBN_Disable_BOD_IRQ();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Enable_AComp0_IRQ(void){
    return RomDriver_HBN_Enable_AComp0_IRQ();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Enable_AComp1_IRQ(void){
    return RomDriver_HBN_Enable_AComp1_IRQ();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Enable_AComp_IRQ(uint8_t acompId, HBN_ACOMP_INT_EDGE_Type edge){
    return RomDriver_HBN_Enable_AComp_IRQ(acompId,edge);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Enable_BOD_IRQ(void){
    return RomDriver_HBN_Enable_BOD_IRQ();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Enable_RTC_Counter(void){
    return RomDriver_HBN_Enable_RTC_Counter();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_GPIO_INT_Disable(void){
    return RomDriver_HBN_GPIO_INT_Disable();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_GPIO_INT_Enable(uint8_t gpioIntTrigType){
    return RomDriver_HBN_GPIO_INT_Enable(gpioIntTrigType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Get_BOD_OUT_State(void){
    return RomDriver_HBN_Get_BOD_OUT_State();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Get_Flash_Power_Delay(uint8_t *flashPwrDly){
    return RomDriver_HBN_Get_Flash_Power_Delay(flashPwrDly);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Get_Gpio_Keep(uint8_t *gpioKeep){
    return RomDriver_HBN_Get_Gpio_Keep(gpioKeep);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Get_INT_State(uint8_t irqType){
    return RomDriver_HBN_Get_INT_State(irqType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Get_Ldo18io_Power_Delay_Config(uint16_t *pwrOffDly, uint16_t *pwrOnDly){
    return RomDriver_HBN_Get_Ldo18io_Power_Delay_Config(pwrOffDly,pwrOnDly);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Get_PDS_Gpio_Keep(uint8_t *gpioKeep){
    return RomDriver_HBN_Get_PDS_Gpio_Keep(gpioKeep);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Get_RTC_Timer_Val(uint32_t *valLow, uint32_t *valHigh){
    return RomDriver_HBN_Get_RTC_Timer_Val(valLow,valHigh);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Get_Reset_Reason(uint16_t *rstReason){
    return RomDriver_HBN_Get_Reset_Reason(rstReason);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Get_Xtal_Type(uint8_t *xtalType){
    return RomDriver_HBN_Get_Xtal_Type(xtalType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Get_Xtal_Value(uint32_t *xtalVal){
    return RomDriver_HBN_Get_Xtal_Value(xtalVal);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Hw_Pu_Pd_Cfg(uint8_t enable){
    return RomDriver_HBN_Hw_Pu_Pd_Cfg(enable);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Keep_On_RC32K(void){
    return RomDriver_HBN_Keep_On_RC32K();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_PIR_Disable(void){
    return RomDriver_HBN_PIR_Disable();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_PIR_Enable(void){
    return RomDriver_HBN_PIR_Enable();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_PIR_HPF_Sel(HBN_PIR_HPF_Type hpf){
    return RomDriver_HBN_PIR_HPF_Sel(hpf);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_PIR_INT_Config(HBN_PIR_INT_CFG_Type *pirIntCfg){
    return RomDriver_HBN_PIR_INT_Config(pirIntCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_PIR_LPF_Sel(HBN_PIR_LPF_Type lpf){
    return RomDriver_HBN_PIR_LPF_Sel(lpf);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Pin_WakeUp_Mask(uint8_t maskVal){
    return RomDriver_HBN_Pin_WakeUp_Mask(maskVal);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Power_Off_RC32K(void){
    return RomDriver_HBN_Power_Off_RC32K();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Power_Off_Xtal_32K(void){
    return RomDriver_HBN_Power_Off_Xtal_32K();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Power_On_Xtal_32K(void){
    return RomDriver_HBN_Power_On_Xtal_32K();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Recal_RC32K(int32_t expected_counter, int32_t actual_counter){
    return RomDriver_HBN_Recal_RC32K(expected_counter,actual_counter);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Reset(void){
    return RomDriver_HBN_Reset();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_BOD_Cfg(HBN_BOD_CFG_Type *cfg){
    return RomDriver_HBN_Set_BOD_Cfg(cfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_BOD_Config(uint8_t enable, HBN_BOD_THRES_Type threshold, HBN_BOD_MODE_Type mode){
    return RomDriver_HBN_Set_BOD_Config(enable,threshold,mode);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_Core_Reboot_Config(uint8_t core, uint8_t hcfg){
    return RomDriver_HBN_Set_Core_Reboot_Config(core,hcfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_GPADC_CLK_Sel(uint8_t clkSel){
    return RomDriver_HBN_Set_GPADC_CLK_Sel(clkSel);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_Gpio_Keep(uint8_t gpioKeep){
    return RomDriver_HBN_Set_Gpio_Keep(gpioKeep);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_HRAM_Ret(void){
    return RomDriver_HBN_Set_HRAM_Ret();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_HRAM_slp(void){
    return RomDriver_HBN_Set_HRAM_slp();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_Ldo11_All_Vout(uint8_t ldoLevel){
    return RomDriver_HBN_Set_Ldo11_All_Vout(ldoLevel);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_Ldo11_Aon_Vout(uint8_t ldoLevel){
    return RomDriver_HBN_Set_Ldo11_Aon_Vout(ldoLevel);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_Ldo11_Rt_Vout(uint8_t ldoLevel){
    return RomDriver_HBN_Set_Ldo11_Rt_Vout(ldoLevel);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_Ldo11_Soc_Vout(uint8_t ldoLevel){
    return RomDriver_HBN_Set_Ldo11_Soc_Vout(ldoLevel);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_Ldo18io_Power_Delay_Config(uint16_t pwrOffDly, uint16_t pwrOnDly){
    return RomDriver_HBN_Set_Ldo18io_Power_Delay_Config(pwrOffDly,pwrOnDly);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_MCU_Root_CLK_Sel(uint8_t rootClk){
    return RomDriver_HBN_Set_MCU_Root_CLK_Sel(rootClk);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_MCU_XCLK_Sel(uint8_t xclk){
    return RomDriver_HBN_Set_MCU_XCLK_Sel(xclk);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_PDS_Gpio_Keep(uint8_t gpioKeep){
    return RomDriver_HBN_Set_PDS_Gpio_Keep(gpioKeep);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_PIR_Interval(uint16_t interval){
    return RomDriver_HBN_Set_PIR_Interval(interval);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_PIR_Threshold(uint16_t threshold){
    return RomDriver_HBN_Set_PIR_Threshold(threshold);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_RTC_Timer(uint8_t delay, uint32_t compValLow, uint32_t compValHigh, uint8_t compMode){
    return RomDriver_HBN_Set_RTC_Timer(delay,compValLow,compValHigh,compMode);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_Reset_Reason(uint16_t rstReason){
    return RomDriver_HBN_Set_Reset_Reason(rstReason);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_Status_Flag(uint32_t flag){
    return RomDriver_HBN_Set_Status_Flag(flag);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_UART_CLK_Sel(uint8_t clkSel){
    return RomDriver_HBN_Set_UART_CLK_Sel(clkSel);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_User_Boot_Config(uint8_t ubCfg){
    return RomDriver_HBN_Set_User_Boot_Config(ubCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_Wakeup_Addr(uint32_t addr){
    return RomDriver_HBN_Set_Wakeup_Addr(addr);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Set_Xtal_Type(uint8_t xtalType){
    return RomDriver_HBN_Set_Xtal_Type(xtalType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Trim_Ldo33VoutTrim(void){
    return RomDriver_HBN_Trim_Ldo33VoutTrim();
}

__attribute__((weak)) ATTR_TCM_SECTION
int HBN_Trim_RC32K(void){
    return RomDriver_HBN_Trim_RC32K();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint16_t HBN_Get_PIR_Interval(void){
    return RomDriver_HBN_Get_PIR_Interval();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint16_t HBN_Get_PIR_Threshold(void){
    return RomDriver_HBN_Get_PIR_Threshold();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t HBN_Get_Status_Flag(void){
    return RomDriver_HBN_Get_Status_Flag();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t HBN_Get_Wakeup_Addr(void){
    return RomDriver_HBN_Get_Wakeup_Addr();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t HBN_Get_Core_Unhalt_Config(void){
    return RomDriver_HBN_Get_Core_Unhalt_Config();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t HBN_Get_MCU_Root_CLK_Sel(void){
    return RomDriver_HBN_Get_MCU_Root_CLK_Sel();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t HBN_Get_MCU_XCLK_Sel(void){
    return RomDriver_HBN_Get_MCU_XCLK_Sel();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t HBN_Get_Pin_Wakeup_Mode(void){
    return RomDriver_HBN_Get_Pin_Wakeup_Mode();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t HBN_Get_User_Boot_Config(void){
    return RomDriver_HBN_Get_User_Boot_Config();
}

__attribute__((weak)) ATTR_TCM_SECTION
void HBN_Clr_Reset_Event(void){
    return RomDriver_HBN_Clr_Reset_Event();
}

__attribute__((weak)) ATTR_TCM_SECTION
void HBN_Enable(uint32_t aGPIOIeCfg, uint8_t ldoLevel, HBN_LEVEL_Type hbnLevel, uint8_t dcdcPuSeq){
    return RomDriver_HBN_Enable(aGPIOIeCfg,ldoLevel,hbnLevel,dcdcPuSeq);
}

__attribute__((weak)) ATTR_TCM_SECTION
void HBN_Get_Reset_Event(uint8_t *event){
    return RomDriver_HBN_Get_Reset_Event(event);
}

__attribute__((weak)) ATTR_TCM_SECTION
void HBN_Mode_Enter(HBN_APP_CFG_Type *cfg){
    return RomDriver_HBN_Mode_Enter(cfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
void HBN_Power_Down_Flash(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_HBN_Power_Down_Flash(flashCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Default_Level_Config(PDS_DEFAULT_LV_CFG_Type *defaultLvCfg, uint32_t pdsSleepCnt){
    return RomDriver_PDS_Default_Level_Config(defaultLvCfg,pdsSleepCnt);
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Disable_GPIO_Keep(void){
    return RomDriver_PDS_Disable_GPIO_Keep();
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Enable(PDS_CTL_Type *cfg, PDS_CTL4_Type *cfg4, uint32_t pdsSleepCnt){
    return RomDriver_PDS_Enable(cfg,cfg4,pdsSleepCnt);
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Force_Config(PDS_CTL2_Type *cfg2, PDS_CTL3_Type *cfg3, PDS_CTL5_Type *cfg5){
    return RomDriver_PDS_Force_Config(cfg2,cfg3,cfg5);
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Get_GPIO_Pad_IntStatus(uint8_t pad){
    return RomDriver_PDS_Get_GPIO_Pad_IntStatus(pad);
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Get_IntStatus(uint8_t intType){
    return RomDriver_PDS_Get_IntStatus(intType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_IntClear(void){
    return RomDriver_PDS_IntClear();
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_IntMask(uint8_t intType, int intMask){
    return RomDriver_PDS_IntMask(intType,intMask);
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Power_Off_WB(void){
    return RomDriver_PDS_Power_Off_WB();
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Power_On_WB(void){
    return RomDriver_PDS_Power_On_WB();
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Pu_PLL_Disable(void){
    return RomDriver_PDS_Pu_PLL_Disable();
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Pu_PLL_Enable(void){
    return RomDriver_PDS_Pu_PLL_Enable();
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_RAM_Config(PDS_CTRL_RAM1_Type *ram1Cfg, PDS_CTRL_RAM2_Type *ram2Cfg, PDS_CTRL_RAM3_Type *ram3Cfg, PDS_CTRL_RAM4_Type *ram4Cfg){
    return RomDriver_PDS_RAM_Config(ram1Cfg,ram2Cfg,ram3Cfg,ram4Cfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Reset_Low_Power_USB_Ctrl(void){
    return RomDriver_PDS_Reset_Low_Power_USB_Ctrl();
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Set_Flash_Pad_Pull_None(uint8_t pinCfg){
    return RomDriver_PDS_Set_Flash_Pad_Pull_None(pinCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Set_Flash_Pad_Pull_None_Fast(uint8_t pinCfg){
    return RomDriver_PDS_Set_Flash_Pad_Pull_None_Fast(pinCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Set_GPIO_Pad_IntClr(uint8_t set){
    return RomDriver_PDS_Set_GPIO_Pad_IntClr(set);
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Set_GPIO_Pad_IntMask(uint8_t pad, int intMask){
    return RomDriver_PDS_Set_GPIO_Pad_IntMask(pad,intMask);
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Set_GPIO_Pad_IntMode(uint8_t set, uint8_t trig){
    return RomDriver_PDS_Set_GPIO_Pad_IntMode(set,trig);
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(uint8_t grp, uint8_t pu, uint8_t pd, uint8_t ie){
    return RomDriver_PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(grp,pu,pd,ie);
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Set_MCU0_Clock_Disable(void){
    return RomDriver_PDS_Set_MCU0_Clock_Disable();
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Set_MCU0_Clock_Enable(void){
    return RomDriver_PDS_Set_MCU0_Clock_Enable();
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Set_MCU0_Reset_Address(uint32_t addr){
    return RomDriver_PDS_Set_MCU0_Reset_Address(addr);
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Set_USB_Resume(void){
    return RomDriver_PDS_Set_USB_Resume();
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Set_USB_Suspend(void){
    return RomDriver_PDS_Set_USB_Suspend();
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Trim_RC32M(void){
    return RomDriver_PDS_Trim_RC32M();
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Turn_Off_USB(void){
    return RomDriver_PDS_Turn_Off_USB();
}

__attribute__((weak)) ATTR_TCM_SECTION
int PDS_Turn_On_USB(uint8_t waitReady){
    return RomDriver_PDS_Turn_On_USB(waitReady);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SF_Cfg_Deinit_Ext_Flash_Gpio(uint8_t extFlashPin){
    return RomDriver_SF_Cfg_Deinit_Ext_Flash_Gpio(extFlashPin);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SF_Cfg_Flash_Init(uint8_t sel, const SF_Ctrl_Cfg_Type *pSfCtrlCfg, const SF_Ctrl_Bank2_Cfg *pBank2Cfg){
    return RomDriver_SF_Cfg_Flash_Init(sel,pSfCtrlCfg,pBank2Cfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SF_Cfg_Get_Flash_Cfg_Need_Lock(uint32_t flashID, SPI_Flash_Cfg_Type *pFlashCfg){
    return RomDriver_SF_Cfg_Get_Flash_Cfg_Need_Lock(flashID,pFlashCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(uint32_t flashID, SPI_Flash_Cfg_Type *pFlashCfg){
    return RomDriver_SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(flashID,pFlashCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SF_Cfg_Init_Ext_Flash_Gpio(uint8_t extFlashPin){
    return RomDriver_SF_Cfg_Init_Ext_Flash_Gpio(extFlashPin);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SF_Cfg_Init_Flash2_Gpio(uint8_t swap){
    return RomDriver_SF_Cfg_Init_Flash2_Gpio(swap);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SF_Cfg_Init_Flash_Gpio(uint8_t flashPinCfg, uint8_t restoreDefault){
    return RomDriver_SF_Cfg_Init_Flash_Gpio(flashPinCfg,restoreDefault);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SF_Cfg_Sbus2_Flash_Init(uint8_t sel, const SF_Ctrl_Bank2_Cfg *pBank2Cfg){
    return RomDriver_SF_Cfg_Sbus2_Flash_Init(sel,pBank2Cfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t SF_Cfg_Flash_Identify(uint8_t callFromFlash, uint8_t flashPinCfg, uint8_t restoreDefault, SPI_Flash_Cfg_Type *pFlashCfg, uint8_t group, uint8_t bank){
    return RomDriver_SF_Cfg_Flash_Identify(callFromFlash,flashPinCfg,restoreDefault,pFlashCfg,group,bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t SF_Cfg_Flash_Identify_Ext(uint8_t callFromFlash, uint8_t flashPinCfg, uint8_t restoreDefault, SPI_Flash_Cfg_Type *pFlashCfg, uint8_t group, uint8_t bank){
    return RomDriver_SF_Cfg_Flash_Identify_Ext(callFromFlash,flashPinCfg,restoreDefault,pFlashCfg,group,bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SF_Ctrl_GetBusyState(void){
    return RomDriver_SF_Ctrl_GetBusyState();
}

__attribute__((weak)) ATTR_TCM_SECTION
int SF_Ctrl_Is_AES_Enable(void){
    return RomDriver_SF_Ctrl_Is_AES_Enable();
}

__attribute__((weak)) ATTR_TCM_SECTION
int SF_Ctrl_Is_Sbus2_Enable(void){
    return RomDriver_SF_Ctrl_Is_Sbus2_Enable();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t SF_Ctrl_Get_Flash_Image_Offset(uint8_t group, uint8_t bank){
    return RomDriver_SF_Ctrl_Get_Flash_Image_Offset(group,bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t SF_Ctrl_Get_Clock_Delay(void){
    return RomDriver_SF_Ctrl_Get_Clock_Delay();
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t SF_Ctrl_Get_Wrap_Queue_Value(void){
    return RomDriver_SF_Ctrl_Get_Wrap_Queue_Value();
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_32bits_Addr_En(int en32BitsAddr){
    return RomDriver_SF_Ctrl_32bits_Addr_En(en32BitsAddr);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_AES_Disable(void){
    return RomDriver_SF_Ctrl_AES_Disable();
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_AES_Enable(void){
    return RomDriver_SF_Ctrl_AES_Enable();
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_AES_Enable_BE(void){
    return RomDriver_SF_Ctrl_AES_Enable_BE();
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_AES_Enable_LE(void){
    return RomDriver_SF_Ctrl_AES_Enable_LE();
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_AES_Get_IV_BE(uint8_t region, uint8_t *iv){
    return RomDriver_SF_Ctrl_AES_Get_IV_BE(region,iv);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_AES_Get_IV_LE(uint8_t region, uint8_t *iv){
    return RomDriver_SF_Ctrl_AES_Get_IV_LE(region,iv);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_AES_Set_IV(uint8_t region, uint8_t *iv, uint32_t addrOffset){
    return RomDriver_SF_Ctrl_AES_Set_IV(region,iv,addrOffset);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_AES_Set_IV_BE(uint8_t region, uint8_t *iv, uint32_t addrOffset){
    return RomDriver_SF_Ctrl_AES_Set_IV_BE(region,iv,addrOffset);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_AES_Set_Key(uint8_t region, uint8_t *key, uint8_t keyType){
    return RomDriver_SF_Ctrl_AES_Set_Key(region,key,keyType);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_AES_Set_Key_BE(uint8_t region, uint8_t *key, uint8_t keyType){
    return RomDriver_SF_Ctrl_AES_Set_Key_BE(region,key,keyType);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_AES_Set_Mode(uint8_t mode){
    return RomDriver_SF_Ctrl_AES_Set_Mode(mode);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_AES_Set_Region(uint8_t region, uint8_t enable, uint8_t hwKey, uint32_t startAddr, uint32_t endAddr, uint8_t locked){
    return RomDriver_SF_Ctrl_AES_Set_Region(region,enable,hwKey,startAddr,endAddr,locked);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_AES_XTS_Set_IV(uint8_t region, uint8_t *iv, uint32_t addrOffset){
    return RomDriver_SF_Ctrl_AES_XTS_Set_IV(region,iv,addrOffset);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_AES_XTS_Set_IV_BE(uint8_t region, uint8_t *iv, uint32_t addrOffset){
    return RomDriver_SF_Ctrl_AES_XTS_Set_IV_BE(region,iv,addrOffset);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_AES_XTS_Set_Key(uint8_t region, uint8_t *key, uint8_t keyType){
    return RomDriver_SF_Ctrl_AES_XTS_Set_Key(region,key,keyType);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_AES_XTS_Set_Key_BE(uint8_t region, uint8_t *key, uint8_t keyType){
    return RomDriver_SF_Ctrl_AES_XTS_Set_Key_BE(region,key,keyType);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Bank2_Enable(const SF_Ctrl_Bank2_Cfg *bank2Cfg){
    return RomDriver_SF_Ctrl_Bank2_Enable(bank2Cfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Cmds_Set(SF_Ctrl_Cmds_Cfg *cmdsCfg, uint8_t bank){
    return RomDriver_SF_Ctrl_Cmds_Set(cmdsCfg,bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Disable(void){
    return RomDriver_SF_Ctrl_Disable();
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Disable_Wrap_Access(uint8_t disable){
    return RomDriver_SF_Ctrl_Disable_Wrap_Access(disable);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Enable(const SF_Ctrl_Cfg_Type *cfg){
    return RomDriver_SF_Ctrl_Enable(cfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Flash2_Read_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid){
    return RomDriver_SF_Ctrl_Flash2_Read_Icache_Set(cfg,cmdValid);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Flash_Read_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid){
    return RomDriver_SF_Ctrl_Flash_Read_Icache_Set(cfg,cmdValid);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Lock_Flash_Image_Offset(uint8_t lock){
    return RomDriver_SF_Ctrl_Lock_Flash_Image_Offset(lock);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Remap_Set(uint8_t remap, uint8_t lock){
    return RomDriver_SF_Ctrl_Remap_Set(remap,lock);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Sbus2_Hold_Sram(void){
    return RomDriver_SF_Ctrl_Sbus2_Hold_Sram();
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Sbus2_Release_Sram(void){
    return RomDriver_SF_Ctrl_Sbus2_Release_Sram();
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Sbus2_Replace(uint8_t pad){
    return RomDriver_SF_Ctrl_Sbus2_Replace(pad);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Sbus2_Revoke_replace(void){
    return RomDriver_SF_Ctrl_Sbus2_Revoke_replace();
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Sbus2_Set_Delay(uint8_t clkDelay, uint8_t rxClkInvert){
    return RomDriver_SF_Ctrl_Sbus2_Set_Delay(clkDelay,rxClkInvert);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Sbus_Select_Bank(uint8_t bank){
    return RomDriver_SF_Ctrl_Sbus_Select_Bank(bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Select_Pad(uint8_t sel){
    return RomDriver_SF_Ctrl_Select_Pad(sel);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_SendCmd(SF_Ctrl_Cmd_Cfg_Type *cfg){
    return RomDriver_SF_Ctrl_SendCmd(cfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_SetDelay(uint8_t clkDelay){
    return RomDriver_SF_Ctrl_SetDelay(clkDelay);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Set_Clock_Delay(uint8_t delay){
    return RomDriver_SF_Ctrl_Set_Clock_Delay(delay);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Set_Flash_Image_Offset(uint32_t addrOffset, uint8_t group, uint8_t bank){
    return RomDriver_SF_Ctrl_Set_Flash_Image_Offset(addrOffset,group,bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Set_IO_Delay(uint8_t pad, uint8_t doDelay, uint8_t diDelay, uint8_t oeDelay){
    return RomDriver_SF_Ctrl_Set_IO_Delay(pad,doDelay,diDelay,oeDelay);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SF_Ctrl_Set_Owner(uint8_t owner){
    return RomDriver_SF_Ctrl_Set_Owner(owner);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Blk32_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t blkNum){
    return RomDriver_SFlash_Blk32_Erase(flashCfg,blkNum);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Blk64_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t blkNum){
    return RomDriver_SFlash_Blk64_Erase(flashCfg,blkNum);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Busy(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Busy(flashCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Chip_Erase(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Chip_Erase(flashCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Clear_Status_Register(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Clear_Status_Register(flashCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t startaddr, uint32_t endaddr){
    return RomDriver_SFlash_Erase(flashCfg,startaddr,endaddr);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Erase_Security_Register(SPI_Flash_Cfg_Type *pFlashCfg, SFlash_Sec_Reg_Cfg *pSecRegCfg){
    return RomDriver_SFlash_Erase_Security_Register(pFlashCfg,pSecRegCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_IDbus_Read_Enable(SPI_Flash_Cfg_Type *flashCfg, uint8_t ioMode, uint8_t contRead, uint8_t bank){
    return RomDriver_SFlash_IDbus_Read_Enable(flashCfg,ioMode,contRead,bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Program(SPI_Flash_Cfg_Type *flashCfg, uint8_t ioMode, uint32_t addr, uint8_t *data, uint32_t len){
    return RomDriver_SFlash_Program(flashCfg,ioMode,addr,data,len);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Program_Security_Register(SPI_Flash_Cfg_Type *pFlashCfg, SFlash_Sec_Reg_Cfg *pSecRegCfg){
    return RomDriver_SFlash_Program_Security_Register(pFlashCfg,pSecRegCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Qspi_Disable(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Qspi_Disable(flashCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Qspi_Enable(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Qspi_Enable(flashCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_RCV_Enable(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t rCmd, uint8_t wCmd, uint8_t bitPos){
    return RomDriver_SFlash_RCV_Enable(pFlashCfg,rCmd,wCmd,bitPos);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Read(SPI_Flash_Cfg_Type *flashCfg, uint8_t ioMode, uint8_t contRead, uint32_t addr, uint8_t *data, uint32_t len){
    return RomDriver_SFlash_Read(flashCfg,ioMode,contRead,addr,data,len);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Read_Reg(SPI_Flash_Cfg_Type *flashCfg, uint8_t regIndex, uint8_t *regValue, uint8_t regLen){
    return RomDriver_SFlash_Read_Reg(flashCfg,regIndex,regValue,regLen);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Read_Reg_With_Cmd(SPI_Flash_Cfg_Type *flashCfg, uint8_t readRegCmd, uint8_t *regValue, uint8_t regLen){
    return RomDriver_SFlash_Read_Reg_With_Cmd(flashCfg,readRegCmd,regValue,regLen);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Read_Security_Register(SFlash_Sec_Reg_Cfg *pSecRegCfg){
    return RomDriver_SFlash_Read_Security_Register(pSecRegCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Restore_From_Powerdown(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t flashContRead, uint8_t bank){
    return RomDriver_SFlash_Restore_From_Powerdown(pFlashCfg,flashContRead,bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Sector_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t secNum){
    return RomDriver_SFlash_Sector_Erase(flashCfg,secNum);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Set32BitsAddrMode(SPI_Flash_Cfg_Type *flashCfg, int en32BitsAddr){
    return RomDriver_SFlash_Set32BitsAddrMode(flashCfg,en32BitsAddr);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_SetSPIMode(uint8_t mode){
    return RomDriver_SFlash_SetSPIMode(mode);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Set_IDbus_Cfg(SPI_Flash_Cfg_Type *flashCfg, uint8_t ioMode, uint8_t contRead, uint32_t addr, uint32_t len, uint8_t bank){
    return RomDriver_SFlash_Set_IDbus_Cfg(flashCfg,ioMode,contRead,addr,len,bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Software_Reset(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Software_Reset(flashCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Write_Enable(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Write_Enable(flashCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Write_Reg(SPI_Flash_Cfg_Type *flashCfg, uint8_t regIndex, uint8_t *regValue, uint8_t regLen){
    return RomDriver_SFlash_Write_Reg(flashCfg,regIndex,regValue,regLen);
}

__attribute__((weak)) ATTR_TCM_SECTION
int SFlash_Write_Reg_With_Cmd(SPI_Flash_Cfg_Type *flashCfg, uint8_t writeRegCmd, uint8_t *regValue, uint8_t regLen){
    return RomDriver_SFlash_Write_Reg_With_Cmd(flashCfg,writeRegCmd,regValue,regLen);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SFlash_DisableBurstWrap(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_DisableBurstWrap(flashCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SFlash_GetDeviceId(uint8_t *data, int is32BitsAddr){
    return RomDriver_SFlash_GetDeviceId(data,is32BitsAddr);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SFlash_GetJedecId(SPI_Flash_Cfg_Type *flashCfg, uint8_t *data){
    return RomDriver_SFlash_GetJedecId(flashCfg,data);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SFlash_GetUniqueId(uint8_t *data, uint8_t idLen){
    return RomDriver_SFlash_GetUniqueId(data,idLen);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SFlash_IDbus_Read_Disable(void){
    return RomDriver_SFlash_IDbus_Read_Disable();
}

__attribute__((weak)) ATTR_TCM_SECTION
void SFlash_Init(const SF_Ctrl_Cfg_Type *pSfCtrlCfg, const SF_Ctrl_Bank2_Cfg *pBank2Cfg){
    return RomDriver_SFlash_Init(pSfCtrlCfg,pBank2Cfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SFlash_Powerdown(void){
    return RomDriver_SFlash_Powerdown();
}

__attribute__((weak)) ATTR_TCM_SECTION
void SFlash_Release_Powerdown(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Release_Powerdown(flashCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SFlash_Reset_Continue_Read(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Reset_Continue_Read(flashCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SFlash_SetBurstWrap(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_SetBurstWrap(flashCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
void SFlash_Volatile_Reg_Write_Enable(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Volatile_Reg_Write_Enable(flashCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_ApplyAbrResult(uint8_t uartId, uint8_t autoBaudDet){
    return RomDriver_UART_ApplyAbrResult(uartId,autoBaudDet);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_AutoBaudDetection(uint8_t uartId, int autoBaud){
    return RomDriver_UART_AutoBaudDetection(uartId,autoBaud);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_ClrRtsValue(uint8_t uartId){
    return RomDriver_UART_ClrRtsValue(uartId);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_ClrTxValue(uint8_t uartId){
    return RomDriver_UART_ClrTxValue(uartId);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_DeInit(uint8_t uartId){
    return RomDriver_UART_DeInit(uartId);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_Disable(uint8_t uartId, uint8_t direct){
    return RomDriver_UART_Disable(uartId,direct);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_DmaDisable(UART_ID_Type uartId, UART_Direction_Type direct){
    return RomDriver_UART_DmaDisable(uartId,direct);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_DmaEnable(UART_ID_Type uartId, UART_Direction_Type direct){
    return RomDriver_UART_DmaEnable(uartId,direct);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_Enable(uint8_t uartId, uint8_t direct){
    return RomDriver_UART_Enable(uartId,direct);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_FifoConfig(uint8_t uartId, UART_FifoCfg_Type *fifoCfg){
    return RomDriver_UART_FifoConfig(uartId,fifoCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_GetBitWidth0X55(uint8_t uartId, uint16_t *width){
    return RomDriver_UART_GetBitWidth0X55(uartId,width);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_GetIntStatus(uint8_t uartId, uint8_t intType){
    return RomDriver_UART_GetIntStatus(uartId,intType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_GetOverflowStatus(uint8_t uartId, uint8_t overflow){
    return RomDriver_UART_GetOverflowStatus(uartId,overflow);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_GetRxBusBusyStatus(uint8_t uartId){
    return RomDriver_UART_GetRxBusBusyStatus(uartId);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_GetTxBusBusyStatus(uint8_t uartId){
    return RomDriver_UART_GetTxBusBusyStatus(uartId);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_Init(uint8_t uartId, UART_CFG_Type *uartCfg){
    return RomDriver_UART_Init(uartId,uartCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_IntClear(uint8_t uartId, uint8_t intType){
    return RomDriver_UART_IntClear(uartId,intType);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_IntMask(uint8_t uartId, uint8_t intType, int intMask){
    return RomDriver_UART_IntMask(uartId,intType,intMask);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_IrConfig(uint8_t uartId, UART_IrCfg_Type *irCfg){
    return RomDriver_UART_IrConfig(uartId,irCfg);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_RxFifoClear(uint8_t uartId){
    return RomDriver_UART_RxFifoClear(uartId);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_SendData(uint8_t uartId, uint8_t *data, uint32_t len){
    return RomDriver_UART_SendData(uartId,data,len);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_SendDataBlock(uint8_t uartId, uint8_t *data, uint32_t len){
    return RomDriver_UART_SendDataBlock(uartId,data,len);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_SetAllowableError0X55(uint8_t uartId, uint8_t allowableError){
    return RomDriver_UART_SetAllowableError0X55(uartId,allowableError);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_SetBaudRate(uint8_t uartId, uint32_t baudRate){
    return RomDriver_UART_SetBaudRate(uartId,baudRate);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_SetDeglitchCount(uint8_t uartId, uint8_t deglitchCnt){
    return RomDriver_UART_SetDeglitchCount(uartId,deglitchCnt);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_SetRS485(uint8_t uartId, int enable, uint8_t polarity){
    return RomDriver_UART_SetRS485(uartId,enable,polarity);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_SetRtsValue(uint8_t uartId){
    return RomDriver_UART_SetRtsValue(uartId);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_SetRxByteCount(uint8_t uartId, uint16_t count){
    return RomDriver_UART_SetRxByteCount(uartId,count);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_SetRxDataLength(uint8_t uartId, uint16_t length){
    return RomDriver_UART_SetRxDataLength(uartId,length);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_SetRxTimeoutValue(uint8_t uartId, uint8_t time){
    return RomDriver_UART_SetRxTimeoutValue(uartId,time);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_SetTxDataLength(uint8_t uartId, uint16_t length){
    return RomDriver_UART_SetTxDataLength(uartId,length);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_SetTxValue(uint8_t uartId){
    return RomDriver_UART_SetTxValue(uartId);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_TxFifoClear(uint8_t uartId){
    return RomDriver_UART_TxFifoClear(uartId);
}

__attribute__((weak)) ATTR_TCM_SECTION
int UART_TxFreeRun(uint8_t uartId, int txFreeRun){
    return RomDriver_UART_TxFreeRun(uartId,txFreeRun);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint16_t UART_GetAutoBaudCount(uint8_t uartId, uint8_t autoBaudDet){
    return RomDriver_UART_GetAutoBaudCount(uartId,autoBaudDet);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint16_t UART_GetRxByteCount(uint8_t uartId){
    return RomDriver_UART_GetRxByteCount(uartId);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint32_t UART_ReceiveData(uint8_t uartId, uint8_t *data, uint32_t maxLen){
    return RomDriver_UART_ReceiveData(uartId,data,maxLen);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t UART_GetRxFifoCount(uint8_t uartId){
    return RomDriver_UART_GetRxFifoCount(uartId);
}

__attribute__((weak)) ATTR_TCM_SECTION
uint8_t UART_GetTxFifoCount(uint8_t uartId){
    return RomDriver_UART_GetTxFifoCount(uartId);
}

__attribute__((weak)) ATTR_TCM_SECTION
int XIP_SFlash_Clear_Status_Register_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t group, uint8_t bank){
    return RomDriver_XIP_SFlash_Clear_Status_Register_Need_Lock(pFlashCfg,group,bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
int XIP_SFlash_Erase_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t startaddr, int len, uint8_t group, uint8_t bank){
    return RomDriver_XIP_SFlash_Erase_Need_Lock(pFlashCfg,startaddr,len,group,bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
int XIP_SFlash_GetDeviceId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, int is32BitsAddr, uint8_t *data, uint8_t group, uint8_t bank){
    return RomDriver_XIP_SFlash_GetDeviceId_Need_Lock(pFlashCfg,is32BitsAddr,data,group,bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
int XIP_SFlash_GetJedecId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data, uint8_t group, uint8_t bank){
    return RomDriver_XIP_SFlash_GetJedecId_Need_Lock(pFlashCfg,data,group,bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
int XIP_SFlash_GetUniqueId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data, uint8_t idLen, uint8_t group, uint8_t bank){
    return RomDriver_XIP_SFlash_GetUniqueId_Need_Lock(pFlashCfg,data,idLen,group,bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
int XIP_SFlash_Read_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t addr, uint8_t *data, uint32_t len, uint8_t group, uint8_t bank){
    return RomDriver_XIP_SFlash_Read_Need_Lock(pFlashCfg,addr,data,len,group,bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
int XIP_SFlash_Read_Via_Cache_Need_Lock(uint32_t addr, uint8_t *data, uint32_t len){
    return RomDriver_XIP_SFlash_Read_Via_Cache_Need_Lock(addr,data,len);
}

__attribute__((weak)) ATTR_TCM_SECTION
int XIP_SFlash_State_Restore(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t offset, uint8_t group, uint8_t bank){
    return RomDriver_XIP_SFlash_State_Restore(pFlashCfg,offset,group,bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
int XIP_SFlash_State_Save(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t *offset, uint8_t group, uint8_t bank){
    return RomDriver_XIP_SFlash_State_Save(pFlashCfg,offset,group,bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
int XIP_SFlash_Write_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t addr, uint8_t *data, uint32_t len, uint8_t group, uint8_t bank){
    return RomDriver_XIP_SFlash_Write_Need_Lock(pFlashCfg,addr,data,len,group,bank);
}

__attribute__((weak)) ATTR_TCM_SECTION
void XIP_SFlash_Opt_Enter(uint8_t *aesEnable){
    return RomDriver_XIP_SFlash_Opt_Enter(aesEnable);
}

__attribute__((weak)) ATTR_TCM_SECTION
void XIP_SFlash_Opt_Exit(uint8_t aesEnable){
    return RomDriver_XIP_SFlash_Opt_Exit(aesEnable);
}
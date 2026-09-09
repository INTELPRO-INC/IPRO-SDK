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


#ifndef __DRV_TZC_SEC_H__
#define __DRV_TZC_SEC_H__

#include "tzc_sec_reg.h"
#include "drv_common.h"

typedef enum {
    TZC_SEC_MASTER_RSVD,  /*!< TZC Master:Reserved */
    TZC_SEC_MASTER_RSVD1, /*!< TZC Master:Reserved */
    TZC_SEC_MASTER_USB,   /*!< TZC Master:USB */
    TZC_SEC_MASTER_WIFI,  /*!< TZC Master:WIFI */
    TZC_SEC_MASTER_CCI,   /*!< TZC Master:CCI */
    TZC_SEC_MASTER_SDH,   /*!< TZC Master:SDH */
    TZC_SEC_MASTER_EMAC,  /*!< TZC Master:EMAC */
    TZC_SEC_MASTER_M0,    /*!< TZC Master:M0 CPU */
    TZC_SEC_MASTER_DMA0,  /*!< TZC Master:DMA0 */
    TZC_SEC_MASTER_RSVD2, /*!< TZC Master:Reserved */
    TZC_SEC_MASTER_RSVD3, /*!< TZC Master:Reserved */
    TZC_SEC_MASTER_SDU,   /*!< TZC Master:SDU */
    TZC_SEC_MASTER_MAX,   /*!< TZC Master max */
} TZC_SEC_Master_Type;

typedef enum {
    TZC_SEC_SLAVE_S0_RSVD,          /*!< TZC Slave:Reserved */
    TZC_SEC_SLAVE_S0_DMA,           /*!< TZC Slave:DMA */
    TZC_SEC_SLAVE_S0_RSVD1,         /*!< TZC Slave:Reserved */
    TZC_SEC_SLAVE_S0_PWR,           /*!< TZC Slave:PWR */
    TZC_SEC_SLAVE_S0_SDH,           /*!< TZC Slave:SDH */
    TZC_SEC_SLAVE_S0_EMAC,          /*!< TZC Slave:EMAC */
    TZC_SEC_SLAVE_S0_SDU,           /*!< TZC Slave:SDU */
    TZC_SEC_SLAVE_S1_GLB,           /*!< TZC Slave:GLB */
    TZC_SEC_SLAVE_S1_MIX,           /*!< TZC Slave:MIX */
    TZC_SEC_SLAVE_S1_GPIP,          /*!< TZC Slave:GPIP */
    TZC_SEC_SLAVE_S1_DBG,           /*!< TZC Slave:DBG */
    TZC_SEC_SLAVE_S1_RSVD2,         /*!< TZC Slave:Reserved */
    TZC_SEC_SLAVE_S1_TZC1,          /*!< TZC Slave:TZC1 */
    TZC_SEC_SLAVE_S1_TZC2,          /*!< TZC Slave:TZC2 */
    TZC_SEC_SLAVE_S1_RSVD3,         /*!< TZC Slave:Reserved */
    TZC_SEC_SLAVE_S1_CCI,           /*!< TZC Slave:CCI */
    TZC_SEC_SLAVE_S1_MCU_MISC,      /*!< TZC Slave:MCU_MISC */
    TZC_SEC_SLAVE_S2_EMI_MISC = 23, /*!< TZC Slave:EMI MISC */
    TZC_SEC_SLAVE_S2_RSVD4,         /*!< TZC Slave:Reserved */
    TZC_SEC_SLAVE_S2_PSRAM0,        /*!< TZC Slave:PSRAM0 */
    TZC_SEC_SLAVE_S2_USB_HS,        /*!< TZC Slave:USB HS */
    TZC_SEC_SLAVE_S2_RSVD5,         /*!< TZC Slave:Reserved */
    TZC_SEC_SLAVE_S2_AUDIO,         /*!< TZC Slave:AUDIO */
    TZC_SEC_SLAVE_S2_EF_CTRL,       /*!< TZC Slave:efuse control*/
    TZC_SEC_SLAVE_S2_D2XA,          /*!< TZC Slave:D2XA */
    TZC_SEC_SLAVE_S2_D2XB,          /*!< TZC Slave:D2XB */
    TZC_SEC_SLAVE_S2_JENC,          /*!< TZC Slave:JENC */
    TZC_SEC_SLAVE_S2_PEC,           /*!< TZC Slave:PEC */
    TZC_SEC_SLAVE_S1A_UART0 = 39,   /*!< TZC Slave:UART0 */
    TZC_SEC_SLAVE_S1A_UART1,        /*!< TZC Slave:UART1 */
    TZC_SEC_SLAVE_S1A_SPI,          /*!< TZC Slave:SPI */
    TZC_SEC_SLAVE_S1A_I2C0,         /*!< TZC Slave:I2C0 */
    TZC_SEC_SLAVE_S1A_PWM,          /*!< TZC Slave:PWM */
    TZC_SEC_SLAVE_S1A_TMR,          /*!< TZC Slave:TMR */
    TZC_SEC_SLAVE_S1A_IRR,          /*!< TZC Slave:IRR */
    TZC_SEC_SLAVE_S1A_CKS,          /*!< TZC Slave:CKS */
    TZC_SEC_SLAVE_S1A_DBI,          /*!< TZC Slave:DBI */
    TZC_SEC_SLAVE_S1A_I2C1,         /*!< TZC Slave:I2C1 */
    TZC_SEC_SLAVE_S1A_CAN,          /*!< TZC Slave:CAN */
    TZC_SEC_SLAVE_S1A_I2S,          /*!< TZC Slave:I2S */
    TZC_SEC_SLAVE_S1A_PDM,          /*!< TZC Slave:PDM */
    TZC_SEC_SLAVE_S1A_LZ4,          /*!< TZC Slave:LZ4 */
    TZC_SEC_SLAVE_MAX,              /*!< TZC slave max*/
} TZC_SEC_Slave_Type;

typedef enum {
    TZC_SEC_GLB_CTRL_POR_RESET,  /*!< TZC GLB Ctrl: Power on reset */
    TZC_SEC_GLB_CTRL_CPU_RESET,  /*!< TZC GLB Ctrl: CPU reset */
    TZC_SEC_GLB_CTRL_SYS_RESET,  /*!< TZC GLB Ctrl: System reset */
    TZC_SEC_GLB_CTRL_CCPU_RESET, /*!< TZC GLB Ctrl: CCPU */
    TZC_SEC_GLB_CTRL_MISC,       /*!< TZC GLB Ctrl: MISC */
    TZC_SEC_GLB_CTRL_SRAM,       /*!< TZC GLB Ctrl: SRAM */
    TZC_SEC_GLB_CTRL_SWRESET,    /*!< TZC GLB Ctrl: Software reset */
    TZC_SEC_GLB_CTRL_BMX,        /*!< TZC GLB Ctrl: BMX */
    TZC_SEC_GLB_CTRL_DBG,        /*!< TZC GLB Ctrl: DBG */
    TZC_SEC_GLB_CTRL_MBIST,      /*!< TZC GLB Ctrl: MBIST */
    TZC_SEC_GLB_CTRL_CLK,        /*!< TZC GLB Ctrl: CLK */
    TZC_SEC_GLB_CTRL_INT,        /*!< TZC GLB Ctrl: Interrupt */
    TZC_SEC_GLB_CTRL_PWR,        /*!< TZC GLB Ctrl: Power */
    TZC_SEC_GLB_CTRL_MAX,
} TZC_SEC_GLB_Ctrl_Type;

typedef enum {
    TZC_SEC_SE_CTRL_SHA,  /*!< TZC SE Ctrl: SHA */
    TZC_SEC_SE_CTRL_AES,  /*!< TZC SE Ctrl: AES */
    TZC_SEC_SE_CTRL_TRNG, /*!< TZC SE Ctrl: TRNG */
    TZC_SEC_SE_CTRL_PKA,  /*!< TZC SE Ctrl: PKA */
    TZC_SEC_SE_CTRL_CDET, /*!< TZC SE Ctrl: CEDT */
    TZC_SEC_SE_CTRL_GMAC, /*!< TZC SE Ctrl: GMAC */
    TZC_SEC_SE_CTRL_MAX,
} TZC_SEC_SE_Ctrl_Type;

typedef enum {
    TZC_SEC_SF_CTRL_CR,  /*!< TZC SF Ctrl: control register */
    TZC_SEC_SF_CTRL_SEC, /*!< TZC SF Ctrl: security register */
    TZC_SEC_SF_CTRL_MAX,
} TZC_SEC_SF_Ctrl_Type;

typedef enum {
    TZC_SEC_SE_MODE_ARB, /*!< TZC SE Ctrl mode: cpus arbitrate */
    TZC_SEC_SE_MODE_TZC, /*!< TZC SE Ctrl: TZC control as group */
} TZC_SEC_SE_Ctrl_Mode;

typedef enum {
    TZC_SEC_SF_MODE_ARB, /*!< TZC SF Ctrl mode: cpus arbitrate */
    TZC_SEC_SF_MODE_TZC, /*!< TZC SF Ctrl: TZC control as group */
} TZC_SEC_SF_Ctrl_Mode;

typedef enum {
    TZC_SEC_AUTH_GRP_0, /*!< TZC auth group 0 */
    TZC_SEC_AUTH_GRP_1, /*!< TZC auth group 1 */
} TZC_SEC_Auth_Group;

typedef enum {
    TZC_SEC_ADV_AUTH_GRP_0_IBUS = 0x01, /*!< TZC advance auth group 0 IBUS */
    TZC_SEC_ADV_AUTH_GRP_0_DBUS = 0x02, /*!< TZC advance auth group 0 DBUS */
    TZC_SEC_ADV_AUTH_GRP_1_IBUS = 0x04, /*!< TZC advance auth group 1 IBUS */
    TZC_SEC_ADV_AUTH_GRP_1_DBUS = 0x08, /*!< TZC advance auth group 1 DBUS */
} TZC_SEC_Advance_Auth_Group;

#define TZC_SEC_MAX_AUTH_GRP 3

void Tzc_Sec_Set_Sboot_Done(void);
void Tzc_Sec_Set_Bus_Remap(uint8_t busRmpEn);
void Tzc_Sec_Set_Master_Group(TZC_SEC_Master_Type masterType, uint8_t group);
void Tzc_Sec_Set_Codec_Group(uint8_t group);
void Tzc_Sec_Set_CPU_Group(uint8_t cpu, uint8_t group);
void Tzc_Sec_Set_Slave_Group(TZC_SEC_Slave_Type slaveType, uint8_t group);
void Tzc_Sec_Set_Glb_Ctrl_Group(TZC_SEC_GLB_Ctrl_Type slaveType, uint8_t group);
void Tzc_Sec_ROM_Access_Set(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_ROM_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_OCRAM_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_OCRAM_Access_Set_Regionx(uint8_t group);
void Tzc_Sec_WRAM_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_WRAM_Access_Set_Regionx(uint8_t group);
void Tzc_Sec_Flash_Access_Set(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_Flash_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_Flash_Access_Set_Regionx(uint8_t group);
void Tzc_Sec_PSRAMB_Access_Set(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_PSRAMB_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_PSRAMB_Access_Release(void);
void Tzc_Sec_Set_Se_Ctrl_Mode(TZC_SEC_SE_Ctrl_Mode mode);
void Tzc_Sec_Set_Sf_Ctrl_Mode(TZC_SEC_SF_Ctrl_Mode mode);
void Tzc_Sec_Set_Se_Group(TZC_SEC_SE_Ctrl_Type slaveType, uint8_t group);
void Tzc_Sec_Set_Sf_Group(TZC_SEC_SF_Ctrl_Type slaveType, uint8_t group);
void Tzc_Sec_Set_WTD_Rst_Delay(uint16_t delayValue);

#endif /* __DRV_TZC_SEC_H__ */

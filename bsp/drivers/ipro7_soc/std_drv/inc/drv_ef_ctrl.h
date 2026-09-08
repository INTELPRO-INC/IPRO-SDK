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

#ifndef __DRV_EF_CTRL_H__
#define __DRV_EF_CTRL_H__

#include "ef_ctrl_reg.h"
#include "drv_common.h"

typedef enum {
    EF_CTRL_KEY_SLOT0, /*!< key slot 0 */
    EF_CTRL_KEY_SLOT1, /*!< key slot 1 */
    EF_CTRL_KEY_SLOT2, /*!< key slot 2 */
    EF_CTRL_KEY_SLOT3, /*!< key slot 3 */
    EF_CTRL_KEY_SLOT4, /*!< key slot 4 */
    EF_CTRL_KEY_SLOT5, /*!< key slot 5 */
    EF_CTRL_KEY_MAX,   /*!<  */
} EF_Ctrl_Key_Type;

typedef enum {
    EF_CTRL_SIGN_NONE, /*!< no sign */
    EF_CTRL_SIGN_RSA,  /*!< use RSA to sign */
    EF_CTRL_SIGN_ECC,  /*!< use ECC to sign */
} EF_Ctrl_Sign_Type;

typedef enum {
    EF_CTRL_SF_AES_NONE, /*!< No AES */
    EF_CTRL_SF_AES_128,  /*!< AES 128 */
    EF_CTRL_SF_AES_192,  /*!< AES 192 */
    EF_CTRL_SF_AES_256,  /*!< AES 256 */
} EF_Ctrl_SF_AES_Type;

typedef enum {
    EF_CTRL_DBG_OPEN = 0,  /*!< Open debug */
    EF_CTRL_DBG_PASSWORD,  /*!< Open debug with password */
    EF_CTRL_DBG_CLOSE = 4, /*!< Close debug */
} EF_Ctrl_Dbg_Mode_Type;

typedef enum {
    EF_CTRL_EF_CLK,   /*!< Select efuse clock */
    EF_CTRL_SAHB_CLK, /*!< Select SAHB clock */
} EF_Ctrl_CLK_Type;

typedef enum {
    EF_CTRL_PARA_DFT,    /*!< Select default cyc parameter */
    EF_CTRL_PARA_MANUAL, /*!< Select manual cyc parameter */
} EF_Ctrl_CYC_PARA_Type;

typedef enum {
    EF_CTRL_OP_MODE_AUTO,   /*!< Select efuse program auto mode */
    EF_CTRL_OP_MODE_MANUAL, /*!< Select efuse program manual mode */
} EF_Ctrl_OP_MODE_Type;

typedef struct
{
    EF_Ctrl_Dbg_Mode_Type ef_dbg_mode; /*!< Efuse debug mode */
    uint8_t ef_dbg_jtag_0_dis;         /*!< Jtag debug disable config value */
    uint8_t ef_sboot_en;               /*!< Secure boot enable config value */
} EF_Ctrl_Sec_Param_Type;

typedef struct
{
    uint32_t rsvd_21_0   : 22; /*!< Reserved */
    uint32_t sf_swap_cfg : 2;  /*!< 0:swap none, 1:swap SF2_CS & SF2_IO2, 2:swap SF2_IO0 & SF2_IO3, 3:swap both */
    uint32_t psram_cfg   : 2;  /*!< 0:no psram, 1:2MB psram, 2:external psram, 3:reserved */
    uint32_t flash_cfg   : 3;  /*!< 0:embedded 1MB flash SF1, 1:external flash SF2, 2:external flash SF3 */
    uint32_t sf_reverse  : 1;  /*!< 0:no reverse, 1:reverse */
    uint32_t pkg_info    : 2;  /*!< 0:QFN32, 1:QFN40, 2:QFN48, 3:reserved */
} Efuse_Device_Info_Type;

typedef struct
{
    uint32_t capCode : 6;  /*!< Cap code value */
    uint32_t parity  : 1;  /*!< Parity of capcode */
    uint32_t en      : 1;  /*!< Enable status */
    uint32_t rsvd    : 24; /*!< Reserved */
} Efuse_Capcode_Info_Type;

typedef struct
{
    char *name;           /*!< trim anme */
    uint16_t en_addr;     /*!< enable address */
    uint16_t parity_addr; /*!< parity address */
    uint16_t value_addr;  /*!< value address */
    uint16_t value_len;   /*!< value length */
} Efuse_Common_Trim_Cfg;

typedef struct
{
    uint8_t en;     /*!< Enable status */
    uint8_t parity; /*!< Trim parity */
    uint8_t empty;  /*!< Trim empty */
    uint8_t len;    /*!< Trim value len in bit */
    uint32_t value; /*!< Trim value */
} Efuse_Common_Trim_Type;


#define EF_CTRL_EFUSE_R0_SIZE 128

void EF_Ctrl_Sw_AHB_Clk_0(void);
void EF_Ctrl_Load_Efuse_R0(void);
void EF_Ctrl_Program_Efuse_0(void);
int EF_Ctrl_Busy(void);
int EF_Ctrl_AutoLoad_Done(void);
void EF_Ctrl_Write_Dbg_Pwd(uint32_t passWdLow, uint32_t passWdHigh, uint8_t program);
void EF_Ctrl_Read_Dbg_Pwd(uint32_t *passWdLow, uint32_t *passWdHigh);
void EF_Ctrl_Readlock_Dbg_Pwd(uint8_t program);
void EF_Ctrl_Writelock_Dbg_Pwd(uint8_t program);
void EF_Ctrl_Write_Secure_Cfg(EF_Ctrl_Sec_Param_Type *cfg, uint8_t program);
void EF_Ctrl_Read_Secure_Cfg(EF_Ctrl_Sec_Param_Type *cfg);
void EF_Ctrl_Write_Secure_Boot(uint8_t sign[1], uint8_t aes[1], uint8_t program);
void EF_Ctrl_Read_Secure_Boot(uint8_t sign[1], uint8_t aes[1]);
void EF_Ctrl_Write_Sw_Usage(uint32_t index, uint32_t usage, uint8_t program);
void EF_Ctrl_Read_Sw_Usage(uint32_t index, uint32_t *usage);
void EF_Ctrl_Writelock_Sw_Usage(uint32_t index, uint8_t program);
uint32_t EF_Ctrl_Get_Byte_Zero_Cnt(uint8_t val);
uint8_t EF_Ctrl_Is_All_Bits_Zero(uint32_t val, uint8_t start, uint8_t len);
void EF_Ctrl_Write_MAC_Address(uint8_t mac[8], uint8_t program);
int EF_Ctrl_Read_MAC_Address(uint8_t mac[8]);
void EF_Ctrl_Writelock_MAC_Address(uint8_t program);
int EF_Ctrl_Read_Chip_ID(uint8_t id[8]);
/*----------*/
void EF_Ctrl_Read_Device_Info(Efuse_Device_Info_Type *deviceInfo);
uint8_t EF_Ctrl_Is_CapCode_Empty(uint8_t slot, uint8_t reload);
int EF_Ctrl_Write_CapCode_Opt(uint8_t slot, uint8_t code, uint8_t program);
int EF_Ctrl_Read_CapCode_Opt(uint8_t slot, uint8_t *code, uint8_t reload);
uint8_t EF_Ctrl_Is_PowerOffset_Slot_Empty(uint8_t slot, uint8_t reload);
int EF_Ctrl_Write_PowerOffset_Opt(uint8_t slot, int8_t pwrOffset[4], uint8_t program);
int EF_Ctrl_Read_PowerOffset_Opt(uint8_t slot, int8_t pwrOffset[4], uint8_t reload);
void EF_Ctrl_Write_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len, uint8_t program);
void EF_Ctrl_Read_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len);
void EF_Ctrl_Writelock_AES_Key(uint8_t index, uint8_t program);
void EF_Ctrl_Readlock_AES_Key(uint8_t index, uint8_t program);
void EF_Ctrl_Program_Direct_R0(uint32_t index, uint32_t *data, uint32_t len);
void EF_Ctrl_Read_Direct_R0(uint32_t index, uint32_t *data, uint32_t len);
void EF_Ctrl_Clear(uint32_t index, uint32_t len);
void EF_Ctrl_Crc_Enable(void);
int EF_Ctrl_Crc_Is_Busy(void);
void EF_Ctrl_Crc_Set_Golden(uint32_t goldenValue);
int EF_Ctrl_Crc_Result(void);
#endif /* __DRV_EF_CTRL_H__ */

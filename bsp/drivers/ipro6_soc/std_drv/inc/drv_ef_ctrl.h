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
#include "ef_data_reg.h"
#include "drv_common.h"

/* Backwards-compatible JTAG aliases (old name without DBG prefix) */
#define EF_DATA_EF_JTAG_0_DIS_POS           EF_DATA_EF_DBG_JTAG_0_DIS_POS
#define EF_DATA_EF_JTAG_0_DIS_LEN           EF_DATA_EF_DBG_JTAG_0_DIS_LEN
#define EF_DATA_EF_JTAG_0_DIS_MSK           EF_DATA_EF_DBG_JTAG_0_DIS_MSK
#define EF_DATA_EF_JTAG_0_DIS_UMSK          EF_DATA_EF_DBG_JTAG_0_DIS_UMSK

#define EF_DATA_EF_JTAG_1_DIS_POS           EF_DATA_EF_DBG_JTAG_1_DIS_POS
#define EF_DATA_EF_JTAG_1_DIS_LEN           EF_DATA_EF_DBG_JTAG_1_DIS_LEN
#define EF_DATA_EF_JTAG_1_DIS_MSK           EF_DATA_EF_DBG_JTAG_1_DIS_MSK
#define EF_DATA_EF_JTAG_1_DIS_UMSK          EF_DATA_EF_DBG_JTAG_1_DIS_UMSK

typedef enum {
    EF_CTRL_KEY_SLOT0, /*!< key slot 0 */
    EF_CTRL_KEY_SLOT1, /*!< key slot 1 */
    EF_CTRL_KEY_SLOT2, /*!< key slot 2 */
    EF_CTRL_KEY_SLOT3, /*!< key slot 3 */
    EF_CTRL_KEY_SLOT4, /*!< key slot 4 */
    EF_CTRL_KEY_SLOT5, /*!< key slot 5 */
    EF_CTRL_KEY_MAX,   /*!<  */
} EF_Ctrl_Key_Type;

#define EF_CTRL_SIGN_NONE   (0)
#define EF_CTRL_SIGN_RSA    (1)
#define EF_CTRL_SIGN_ECC    (2)

#define EF_CTRL_SF_AES_NONE (0)
#define EF_CTRL_SF_AES_128  (1)
#define EF_CTRL_SF_AES_192  (2)
#define EF_CTRL_SF_AES_256  (3)

#define EF_CTRL_DBG_OPEN        (0)
#define EF_CTRL_DBG_PASSWORD	(1)
#define EF_CTRL_DBG_CLOSE       (4)

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
    uint8_t ef_dbg_mode; /*!< Efuse debug mode */
    uint8_t ef_dbg_jtag_0_dis;         /*!< Jtag debug disable config value */
    uint8_t ef_dbg_jtag_1_dis;         /*!< Jtag debug disable config value */
    uint8_t ef_sboot_en;               /*!< Secure boot enable config value */
} EF_Ctrl_Sec_Param_Type;

typedef struct
{
    uint32_t dcdcDisAonEn     : 1;  /*!< Efuse analog dcdc_dis_aon_en */
    uint32_t dcdcDisAonParity : 1;  /*!< Efuse analog dcdc_dis_aon_parity */
    uint32_t dcdcDisAon       : 1;  /*!< Efuse analog dcdc_dis_aon */
    uint32_t reserved         : 29; /*!< Efuse analog reserved */
} Efuse_Ana_DcdcDis_Type;

typedef struct
{
    uint32_t dcdcVoutSelAonEn     : 1;  /*!< Efuse analog dcdc_vout_sel_aon_en */
    uint32_t dcdcVoutSelAonParity : 1;  /*!< Efuse analog dcdc_vout_sel_aon_parity */
    uint32_t dcdcVoutSelAon       : 5;  /*!< Efuse analog dcdc_vout_sel_aon[4:0] */
    uint32_t reserved             : 25; /*!< Efuse analog reserved */
} Efuse_Ana_DcdcVoutSel_Type;

typedef struct
{
    uint32_t ldo18ioVoutSelEn     : 1;  /*!< Efuse analog ldo18io_vout_sel_en */
    uint32_t ldo18ioVoutSelParity : 1;  /*!< Efuse analog ldo18io_vout_sel_parity */
    uint32_t ldo18ioVoutSelAon    : 4;  /*!< Efuse analog ldo18io_vout_sel_aon[3:0] */
    uint32_t reserved             : 26; /*!< Efuse analog reserved */
} Efuse_Ana_Ldo18ioVoutSel_Type;

typedef struct
{
    uint32_t ldo18ioBypassEn     : 1;  /*!< Efuse analog ldo18io_bypass_en */
    uint32_t ldo18ioBypassParity : 1;  /*!< Efuse analog ldo18io_bypass_parity */
    uint32_t ldo18ioBypassAon    : 1;  /*!< Efuse analog ldo18io_bypass_aon */
    uint32_t reserved            : 29; /*!< Efuse analog reserved */
} Efuse_Ana_Ldo18ioBypass_Type;

typedef struct
{
    uint32_t ldo33VoutTrimAonEn     : 1;  /*!< Efuse analog ldo33_vout_trim_aon_en */
    uint32_t ldo33VoutTrimAonParity : 1;  /*!< Efuse analog ldo33_vout_trim_aon_parity */
    uint32_t ldo33VoutTrimAon       : 4;  /*!< Efuse analog ldo33_vout_trim_aon[3:0] */
    uint32_t reserved               : 26; /*!< Efuse analog reserved */
} Efuse_Ana_Ldo33VoutTrim_Type;

typedef struct
{
    uint32_t ldo18ioVoutTrimEn     : 1;  /*!< Efuse analog ldo18io_vout_trim_en */
    uint32_t ldo18ioVoutTrimParity : 1;  /*!< Efuse analog ldo18io_vout_trim_parity */
    uint32_t ldo18ioVoutTrimAon    : 4;  /*!< Efuse analog ldo18io_vout_trim_aon[3:0] */
    uint32_t reserved              : 26; /*!< Efuse analog reserved */
} Efuse_Ana_Ldo18ioVoutTrim_Type;

typedef struct
{
    uint32_t dcdcVoutTrimAonEn     : 1;  /*!< Efuse analog dcdc_vout_trim_aon_en */
    uint32_t dcdcVoutTrimAonParity : 1;  /*!< Efuse analog dcdc_vout_trim_aon_parity */
    uint32_t dcdcVoutTrimAon       : 4;  /*!< Efuse analog dcdc_vout_trim_aon[3:0] */
    uint32_t reserved              : 26; /*!< Efuse analog reserved */
} Efuse_Ana_DcdcVoutTrim_Type;

typedef struct
{
    uint32_t ldo11socVoutTrimAonEn   : 1;  /*!< Efuse analog ldo11soc_vout_trim_aon_en */
    uint32_t ldo11socVoutTrimAonTrim : 1;  /*!< Efuse analog ldo11soc_vout_trim_aon_trim */
    uint32_t ldo11socVoutTrimAon     : 4;  /*!< Efuse analog ldo11soc_vout_trim_aon[3:0] */
    uint32_t reserved                : 26; /*!< Efuse analog reserved */
} Efuse_Ana_Ldo11socVoutTrim_Type;

typedef struct
{
    uint32_t rc32mCodeFrExt2En     : 1;  /*!< Efuse analog rc32m_code_fr_ext2_en */
    uint32_t rc32mCodeFrExt2Parity : 1;  /*!< Efuse analog rc32m_code_fr_ext2_parity */
    uint32_t rc32mCodeFrExt2       : 8;  /*!< Efuse analog rc32m_code_fr_ext2[7:0] */
    uint32_t reserved              : 22; /*!< Efuse analog reserved */
} Efuse_Ana_RC32M_Trim_Type;

typedef struct
{
    uint32_t rc32kCodeFrExtEn     : 1;  /*!< Efuse analog rc32k_code_fr_ext_en */
    uint32_t rc32kCodeFrExtParity : 1;  /*!< Efuse analog rc32k_code_fr_ext_parity */
    uint32_t rc32kCodeFrExt       : 10; /*!< Efuse analog rc32k_code_fr_ext[9:0] */
    uint32_t reserved             : 20; /*!< Efuse analog reserved */
} Efuse_Ana_RC32K_Trim_Type;

typedef struct
{
    uint32_t usb20RcalCodeAonEn     : 1;  /*!< Efuse analog usb20_rcal_code_aon_en */
    uint32_t usb20RcalCodeAonParity : 1;  /*!< Efuse analog usb20_rcal_code_aon_parity */
    uint32_t usb20RcalCodeAon       : 6;  /*!< Efuse analog usb20_rcal_code_aon[5:0] */
    uint32_t reserved               : 24; /*!< Efuse analog reserved */
} Efuse_Ana_Usb20RcalCode_Type;

typedef struct
{
    uint32_t tsenRefcodeCorner        : 12; /*!< TSEN refcode */
    uint32_t tsenRefcodeCornerParity  : 1;  /*!< TSEN refcode parity */
    uint32_t tsenRefcodeCornerEn      : 1;  /*!< TSEN refcode enable */
    uint32_t tsenRefcodeCornerVersion : 1;  /*!< TSEN refcode version */
    uint32_t reserved                 : 17; /*!< TSEN analog trim:reserved */
} Efuse_TSEN_Refcode_Corner_Type;

typedef struct
{
    uint32_t adcGainCoeff       : 12; /*!< ADC gain coeff */
    uint32_t adcGainCoeffParity : 1;  /*!< ADC gain coeff parity */
    uint32_t adcGainCoeffEn     : 1;  /*!< ADC gain coeff enable */
    uint32_t reserved           : 18; /*!< ADC gain coeff:reserved */
} Efuse_ADC_Gain_Coeff_Type;

typedef struct
{
    uint32_t rsvd       : 22; /*!< Reserved */
    uint32_t deviceInfo : 2;  /*!< Efuse device information */
    uint32_t psramInfo  : 2;  /*!< Efuse psram info 0:no psram, 1:WB 4MB*/
    uint32_t memoryInfo : 3;  /*!< Efuse memory info 0:no memory, 8:1MB flash */
    uint32_t chipInfo   : 3;  /*!< Efuse chip revision */
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

#define IS_EF_CTRL_KEY_TYPE(type) (((type) == EF_CTRL_KEY_SLOT0) || \
                                   ((type) == EF_CTRL_KEY_SLOT1) || \
                                   ((type) == EF_CTRL_KEY_SLOT2) || \
                                   ((type) == EF_CTRL_KEY_SLOT3) || \
                                   ((type) == EF_CTRL_KEY_SLOT4) || \
                                   ((type) == EF_CTRL_KEY_SLOT5) || \
                                   ((type) == EF_CTRL_KEY_MAX))

#define IS_EF_CTRL_SIGN_TYPE(type) (((type) == EF_CTRL_SIGN_NONE) || \
                                    ((type) == EF_CTRL_SIGN_RSA) ||  \
                                    ((type) == EF_CTRL_SIGN_ECC))

#define IS_EF_CTRL_SF_AES_TYPE(type) (((type) == EF_CTRL_SF_AES_NONE) || \
                                      ((type) == EF_CTRL_SF_AES_128) ||  \
                                      ((type) == EF_CTRL_SF_AES_192) ||  \
                                      ((type) == EF_CTRL_SF_AES_256))

#define IS_EF_CTRL_DBG_MODE_TYPE(type) (((type) == EF_CTRL_DBG_OPEN) ||     \
                                        ((type) == EF_CTRL_DBG_PASSWORD) || \
                                        ((type) == EF_CTRL_DBG_CLOSE))

#define IS_EF_CTRL_CLK_TYPE(type) (((type) == EF_CTRL_EF_CLK) || \
                                   ((type) == EF_CTRL_SAHB_CLK))

#define IS_EF_CTRL_CYC_PARA_TYPE(type) (((type) == EF_CTRL_PARA_DFT) || \
                                        ((type) == EF_CTRL_PARA_MANUAL))

#define IS_EF_CTRL_OP_MODE_TYPE(type) (((type) == EF_CTRL_OP_MODE_AUTO) || \
                                       ((type) == EF_CTRL_OP_MODE_MANUAL))

#define EF_CTRL_EFUSE_R0_SIZE (128 * 4)

/*----------*/
int EF_Ctrl_Get_Customer_PIDVID(uint16_t pid[1], uint16_t vid[1]);
/*----------*/
void EF_Ctrl_Load_Efuse_R0(void);
void EF_Ctrl_Program_Efuse_0(void);
int EF_Ctrl_Busy(void);
int EF_Ctrl_AutoLoad_Done(void);
void EF_Ctrl_Write_Dbg_Pwd(uint8_t slot, uint32_t passWdLow, uint32_t passWdHigh, uint8_t program);
void EF_Ctrl_Read_Dbg_Pwd(uint8_t slot, uint32_t *passWdLow, uint32_t *passWdHigh);
void EF_Ctrl_Readlock_Dbg_Pwd(uint8_t program);
void EF_Ctrl_Writelock_Dbg_Pwd(uint8_t program);
void EF_Ctrl_Write_Secure_Cfg(EF_Ctrl_Sec_Param_Type *cfg, uint8_t program);
void EF_Ctrl_Read_Secure_Cfg(EF_Ctrl_Sec_Param_Type *cfg);
void EF_Ctrl_Write_Secure_Boot(uint8_t sign[1], uint8_t aes[1], uint8_t program);
void EF_Ctrl_Read_Secure_Boot(uint8_t aes[2]);
int EF_Ctrl_Read_Bus_Remap(void);
void EF_Ctrl_Set_sf_key_re_sel(uint8_t ef_sf_key_re_sel);
/*----------*/
uint8_t EF_Ctrl_Get_Trim_Parity(uint32_t val, uint8_t len);
void EF_Ctrl_Read_DcdcDis_Trim(Efuse_Ana_DcdcDis_Type *trim);
void EF_Ctrl_Read_DcdcVoutSel_Trim(Efuse_Ana_DcdcVoutSel_Type *trim);
void EF_Ctrl_Read_Ldo18ioVoutSel_Trim(Efuse_Ana_Ldo18ioVoutSel_Type *trim);
void EF_Ctrl_Read_Ldo18ioBypass_Trim(Efuse_Ana_Ldo18ioBypass_Type *trim);
void EF_Ctrl_Read_Ldo33VoutTrim_Trim(Efuse_Ana_Ldo33VoutTrim_Type *trim);
void EF_Ctrl_Read_Ldo18ioVoutTrim_Trim(Efuse_Ana_Ldo18ioVoutTrim_Type *trim);
void EF_Ctrl_Read_DcdcVoutTrim_Trim(Efuse_Ana_DcdcVoutTrim_Type *trim);
void EF_Ctrl_Read_Ldo11socVoutTrim_Trim(Efuse_Ana_Ldo11socVoutTrim_Type *trim);
void EF_Ctrl_Read_RC32M_Trim(Efuse_Ana_RC32M_Trim_Type *trim);
void EF_Ctrl_Read_RC32K_Trim(Efuse_Ana_RC32K_Trim_Type *trim);
void EF_Ctrl_Read_Usb20RcalCode_Trim(Efuse_Ana_Usb20RcalCode_Type *trim);
/*----------*/
void EF_Ctrl_Read_TSEN_Trim(Efuse_TSEN_Refcode_Corner_Type *trim);
void EF_Ctrl_Read_ADC_Gain_Trim(Efuse_ADC_Gain_Coeff_Type *trim);
void EF_Ctrl_Write_Sw_Usage(uint32_t index, uint32_t usage, uint8_t program);
void EF_Ctrl_Read_Sw_Usage(uint32_t index, uint32_t *usage);
void EF_Ctrl_Writelock_Sw_Usage(uint32_t index, uint8_t program);
void EF_Ctrl_Write_MAC_Address(uint8_t mac[6], uint8_t program);
int EF_Ctrl_Read_MAC_Address(uint8_t mac[6]);
int EF_Ctrl_Read_MAC_Address_Raw(uint8_t mac[7]);
int EF_Ctrl_Read_ZiggBee_MAC_Address(uint8_t mac[8]);
void EF_Ctrl_Writelock_MAC_Address(uint8_t program);
int EF_Ctrl_Read_Chip_ID(uint8_t id[8]);
void EF_Ctrl_Write_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len, uint8_t program);
void EF_Ctrl_Read_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len);
void EF_Ctrl_Writelock_AES_Key(uint8_t index, uint8_t program);
void EF_Ctrl_Readlock_AES_Key(uint8_t index, uint8_t program);
void EF_Ctrl_Program_Direct(uint32_t offset, uint32_t *data, uint32_t len);
void EF_Ctrl_Read_Direct(uint32_t offset, uint32_t *data, uint32_t len);
void EF_Ctrl_Clear(uint32_t index, uint32_t len);
void EF_Ctrl_Crc_Enable(void);
int EF_Ctrl_Crc_Is_Busy(void);
void EF_Ctrl_Crc_Set_Golden(uint32_t goldenValue);
int EF_Ctrl_Crc_Result(void);
void EF_Ctrl_Sw_AHB_Clk_0(void);
uint8_t EF_Ctrl_Is_All_Bits_Zero(uint32_t val, uint8_t start, uint8_t len);
uint8_t EF_Ctrl_Read_FlashDlyCoe(void);

uint32_t EF_Ctrl_Get_Common_Trim_List(Efuse_Common_Trim_Cfg **trim_list);
void EF_Ctrl_Read_Common_Trim(char *name, Efuse_Common_Trim_Type *trim, uint8_t reload);
void EF_Ctrl_Write_Common_Trim(char *name, uint32_t value, uint8_t program);
void EF_Ctrl_Program_Direct_R0(uint32_t index, uint32_t *data, uint32_t len);
void EF_Ctrl_Read_Direct_R0(uint32_t index, uint32_t *data, uint32_t len); 
void EF_Ctrl_Read_Device_Info(Efuse_Device_Info_Type *deviceInfo);

#endif /* __DRV_EF_CTRL_H__ */

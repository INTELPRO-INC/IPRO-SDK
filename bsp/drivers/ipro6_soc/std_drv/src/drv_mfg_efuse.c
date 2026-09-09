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

#include "drv_glb.h"
#include "drv_mfg_efuse.h"
#include "drv_ef_ctrl.h"
#include "drv_ef_cfg.h"

static uint8_t rf_cal_slots = 3;
extern void main(void);
#define RF_CAL_SLOT_CFG_OFFSET (4 * 13)
#ifdef IPRO_MCU_SDK
#include "ipro_platform.h"
#define mfg_print MSG
#else
#define mfg_print printf
#endif

uint8_t mfg_efuse_get_rf_cal_slots(void)
{
    return rf_cal_slots;
}

void mfg_efuse_set_rf_cal_slots(uint8_t slots)
{
    rf_cal_slots = slots;
}

static int mfg_efuse_get_empty_slot(char *name, int *slot, int32_t *last_val, uint8_t reload)
{
    Efuse_Common_Trim_Type trim;
    char buf[16] = { 0 };
    int len = strlen(name);

    *slot = -1;
    *last_val = -1;
    memcpy(buf, name, len);
    buf[len + 1] = '\0';

    if (rf_cal_slots >= 1) {
        buf[len] = '0';
        EF_Ctrl_Read_Common_Trim(buf, &trim, reload);
        if (trim.empty) {
            mfg_print("Empty slot:%d\r\n", 0);
            *slot = 0;
            return 0;
        } else {
            if (trim.en == 1 && trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value, trim.len)) {
                *last_val = (int32_t)trim.value;
            }
        }
    }
    if (rf_cal_slots >= 2) {
        buf[len] = '1';
        EF_Ctrl_Read_Common_Trim(buf, &trim, reload);
        if (trim.empty) {
            mfg_print("Empty slot:%d\r\n", 1);
            *slot = 1;
            return 0;
        } else {
            if (trim.en == 1 && trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value, trim.len)) {
                *last_val = (int32_t)trim.value;
            }
        }
    }
    if (rf_cal_slots >= 3) {
        buf[len] = '2';
        EF_Ctrl_Read_Common_Trim(buf, &trim, reload);
        if (trim.empty) {
            mfg_print("Empty slot:%d\r\n", 2);
            *slot = 2;
            return 0;
        } else {
            if (trim.en == 1 && trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value, trim.len)) {
                *last_val = (int32_t)trim.value;
            }
        }
    }

    return -1;
}

uint8_t mfg_efuse_is_xtal_capcode_slot_empty(uint8_t reload)
{
    int slot;
    int32_t last_val;

    mfg_efuse_get_empty_slot("xtal", &slot, &last_val, reload);

    if (slot == -1) {
        return 0;
    } else {
        return 1;
    }
}

int mfg_efuse_write_xtal_capcode_pre(uint8_t capcode, uint8_t program)
{
    int slot;
    int32_t last_val;
    char buf[16];

    mfg_efuse_get_empty_slot("xtal", &slot, &last_val, 1);

    if (slot == -1) {
        return -1;
    } else {
        sprintf(buf, "xtal%d", slot);
        EF_Ctrl_Write_Common_Trim(buf, capcode, program);
        mfg_print("Write slot:%d\r\n", slot);
        return 0;
    }
}

void mfg_efuse_write_xtal_capcode(void)
{
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_Ctrl_Program_Efuse_0();
}

int mfg_efuse_read_xtal_capcode(uint8_t *capcode, uint8_t reload)
{
    int slot;
    int32_t last_val;

    mfg_efuse_get_empty_slot("xtal", &slot, &last_val, reload);

    if (last_val == -1) {
        mfg_print("No written slot found\r\n");
        return -1;
    } else {
        *capcode = (uint8_t)last_val;
        return 0;
    }
}

static void mfg_efuse_poweroffset_linear(int8_t pwr_offset[14], int8_t pwr_offset_tmp[3])
{
    int32_t step = 0;

    memset(pwr_offset, 0, 14);

    pwr_offset[0] = pwr_offset_tmp[0];

    step = (pwr_offset_tmp[1] - pwr_offset_tmp[0]) * 100 / 6;
    pwr_offset[1] = (step + 50) / 100 + pwr_offset_tmp[0];
    pwr_offset[2] = (step * 2 + 50) / 100 + pwr_offset_tmp[0];
    pwr_offset[3] = (step * 3 + 50) / 100 + pwr_offset_tmp[0];
    pwr_offset[4] = (step * 4 + 50) / 100 + pwr_offset_tmp[0];
    pwr_offset[5] = (step * 5 + 50) / 100 + pwr_offset_tmp[0];

    pwr_offset[6] = pwr_offset_tmp[1];

    step = (pwr_offset_tmp[2] - pwr_offset_tmp[1]) * 100 / 6;
    pwr_offset[7] = (step + 50) / 100 + pwr_offset_tmp[1];
    pwr_offset[8] = (step * 2 + 50) / 100 + pwr_offset_tmp[1];
    pwr_offset[9] = (step * 3 + 50) / 100 + pwr_offset_tmp[1];
    pwr_offset[10] = (step * 4 + 50) / 100 + pwr_offset_tmp[1];
    pwr_offset[11] = (step * 5 + 50) / 100 + pwr_offset_tmp[1];

    pwr_offset[12] = pwr_offset_tmp[2];

    pwr_offset[13] = (step * 7 + 50) / 100 + pwr_offset_tmp[1];
}

uint8_t mfg_efuse_is_hp_poweroffset_slot_empty(uint8_t reload)
{
    int slot;
    int32_t last_val;

    mfg_efuse_get_empty_slot("hp_poffset", &slot, &last_val, reload);

    if (slot == -1) {
        return 0;
    } else {
        return 1;
    }
}

int mfg_efuse_write_hp_poweroffset_pre(int8_t pwr_offset[14], uint8_t program)
{
    int slot;
    int32_t last_val;
    char buf[32];
    uint32_t cur_val = 0;

    mfg_efuse_get_empty_slot("hp_poffset", &slot, &last_val, 1);

    if (slot == -1) {
        return -1;
    } else {
        sprintf(buf, "hp_poffset%d", slot);
        cur_val = (pwr_offset[0] & 0x1f) | ((pwr_offset[6] & 0x1f) << 5) | ((pwr_offset[12] & 0x1f) << 10);
        EF_Ctrl_Write_Common_Trim(buf, cur_val, program);
        mfg_print("Write slot:%d\r\n", slot);
        return 0;
    }
}

void mfg_efuse_write_hp_poweroffset(void)
{
    EF_Ctrl_Program_Direct(0, NULL, 0);
    while (SET == EF_Ctrl_Busy())
        ;
}

int mfg_efuse_read_hp_poweroffset(int8_t pwr_offset[14], uint8_t reload)
{
    int slot;
    int32_t last_val;
    int8_t pwr_offset_tmp[3];
    uint32_t tmp = 0, k;

    mfg_efuse_get_empty_slot("hp_poffset", &slot, &last_val, reload);

    if (last_val == -1) {
        mfg_print("No written slot found\r\n");
        return -1;
    } else {
        for (k = 0; k < 3; k++) {
            tmp = (last_val >> (k * 5)) & 0x1f;

            if (tmp >= 16) {
                pwr_offset_tmp[k] = tmp - 32;
            } else {
                pwr_offset_tmp[k] = tmp;
            }
        }
        mfg_efuse_poweroffset_linear(pwr_offset, pwr_offset_tmp);
        return 0;
    }
}

uint8_t mfg_efuse_is_lp_poweroffset_slot_empty(uint8_t reload)
{
    int slot;
    int32_t last_val;

    mfg_efuse_get_empty_slot("lp_poffset", &slot, &last_val, reload);

    if (slot == -1) {
        return 0;
    } else {
        return 1;
    }
}

int mfg_efuse_write_lp_poweroffset_pre(int8_t pwr_offset[14], uint8_t program)
{
    int slot;
    int32_t last_val;
    char buf[32];
    uint32_t cur_val = 0;

    mfg_efuse_get_empty_slot("lp_poffset", &slot, &last_val, 1);

    if (slot == -1) {
        return -1;
    } else {
        sprintf(buf, "lp_poffset%d", slot);
        cur_val = (pwr_offset[0] & 0x1f) | ((pwr_offset[6] & 0x1f) << 5) | ((pwr_offset[12] & 0x1f) << 10);
        EF_Ctrl_Write_Common_Trim(buf, cur_val, program);
        mfg_print("Write slot:%d\r\n", slot);
        return 0;
    }
}

void mfg_efuse_write_lp_poweroffset(void)
{
    EF_Ctrl_Program_Direct(0, NULL, 0);
    while (SET == EF_Ctrl_Busy())
        ;
}

int mfg_efuse_read_lp_poweroffset(int8_t pwr_offset[14], uint8_t reload)
{
    int slot;
    int32_t last_val;
    int8_t pwr_offset_tmp[3];
    uint32_t tmp = 0, k;

    mfg_efuse_get_empty_slot("lp_poffset", &slot, &last_val, reload);

    if (last_val == -1) {
        mfg_print("No written slot found\r\n");
        return -1;
    } else {
        for (k = 0; k < 3; k++) {
            tmp = (last_val >> (k * 5)) & 0x1f;

            if (tmp >= 16) {
                pwr_offset_tmp[k] = tmp - 32;
            } else {
                pwr_offset_tmp[k] = tmp;
            }
        }
        mfg_efuse_poweroffset_linear(pwr_offset, pwr_offset_tmp);
        return 0;
    }
}

uint8_t mfg_efuse_is_macaddr_slot_empty(uint8_t reload)
{
    uint8_t empty = 0;

    if (rf_cal_slots >= 1 && EF_Ctrl_Is_MAC_Address_Slot_Empty(0, reload)) {
        empty = 1;
    } else if (rf_cal_slots >= 2 && EF_Ctrl_Is_MAC_Address_Slot_Empty(1, reload)) {
        empty = 1;
    } else if (rf_cal_slots >= 3 && EF_Ctrl_Is_MAC_Address_Slot_Empty(2, reload)) {
        empty = 1;
    } else {
    }

    return empty;
}

int8_t mfg_efuse_write_macaddr_pre(uint8_t mac[6], uint8_t program)
{
    int ret = SUCCESS;
    uint8_t slot = 0xff;

    if (rf_cal_slots >= 1 && EF_Ctrl_Is_MAC_Address_Slot_Empty(0, 1)) {
        slot = 0;
    } else if (rf_cal_slots >= 2 && EF_Ctrl_Is_MAC_Address_Slot_Empty(1, 1)) {
        slot = 1;
    } else if (rf_cal_slots >= 3 && EF_Ctrl_Is_MAC_Address_Slot_Empty(2, 1)) {
        slot = 2;
    } else {
        mfg_print("No empty slot found\r\n");
    }

    if (slot != 0xff) {
        ret = EF_Ctrl_Write_MAC_Address_Opt(slot, mac, program);
        mfg_print("Write slot:%d\r\n", slot);
    }

    if (ret == SUCCESS) {
        return 0;
    } else {
        return -1;
    }
}

void mfg_efuse_write_macaddr(void)
{
    EF_Ctrl_Program_Direct(0, NULL, 0);
    while (SET == EF_Ctrl_Busy())
        ;
}

int8_t mfg_efuse_read_macaddr(uint8_t mac[6], uint8_t reload)
{
    uint8_t slot = 0xff;
    int ret = ERROR;

    if (rf_cal_slots >= 3 && (!EF_Ctrl_Is_MAC_Address_Slot_Empty(2, reload))) {
        slot = 2;
    } else if (rf_cal_slots >= 2 && (!EF_Ctrl_Is_MAC_Address_Slot_Empty(1, reload))) {
        slot = 1;
    } else if (rf_cal_slots >= 1 && (!EF_Ctrl_Is_MAC_Address_Slot_Empty(0, reload))) {
        slot = 0;
    }

    if (slot != 0xff) {
        mfg_print("Read slot:%d\r\n", slot);
        ret = EF_Ctrl_Read_MAC_Address_Opt(slot, mac, reload);
    } else {
        mfg_print("No written slot found\r\n");
    }

    if (ret == SUCCESS) {
        return 0;
    } else {
        return -1;
    }
}

uint8_t mfg_efuse_is_bz_poweroffset_slot_empty(uint8_t reload)
{
    int slot;
    int32_t last_val;

    mfg_efuse_get_empty_slot("bz_poffset", &slot, &last_val, reload);

    if (slot == -1) {
        return 0;
    } else {
        return 1;
    }
}

int mfg_efuse_write_bz_poweroffset_pre(int8_t pwr_offset[5], uint8_t program)
{
    int slot;
    int32_t last_val;
    char buf[32];
    uint32_t cur_val = 0;

    mfg_efuse_get_empty_slot("bz_poffset", &slot, &last_val, 1);

    if (slot == -1) {
        return -1;
    } else {
        sprintf(buf, "bz_poffset%d", slot);
        cur_val = (pwr_offset[0] & 0x1f) | ((pwr_offset[1] & 0x1f) << 5) | ((pwr_offset[2] & 0x1f) << 10) | ((pwr_offset[3] & 0x1f) << 15) | ((pwr_offset[4] & 0x1f) << 20);
        EF_Ctrl_Write_Common_Trim(buf, cur_val, program);
        mfg_print("Write slot:%d\r\n", slot);
        return 0;
    }
}

void mfg_efuse_write_bz_poweroffset(void)
{
    EF_Ctrl_Program_Direct(0, NULL, 0);
    while (SET == EF_Ctrl_Busy())
        ;
}

int mfg_efuse_read_bz_poweroffset(int8_t pwr_offset[5], uint8_t reload)
{
    int slot;
    int32_t last_val;
    uint32_t tmp = 0, k;

    mfg_efuse_get_empty_slot("bz_poffset", &slot, &last_val, reload);

    if (last_val == -1) {
        mfg_print("No written slot found\r\n");
        return -1;
    } else {
        for (k = 0; k < 5; k++) {
            tmp = (last_val >> (k * 5)) & 0x1f;

            if (tmp >= 16) {
                pwr_offset[k] = tmp - 32;
            } else {
                pwr_offset[k] = tmp;
            }
        }
        return 0;
    }
}

void mfg_efuse_program(uint32_t addr,uint32_t *pword,uint32_t countInword,uint32_t program)
{
     uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + addr);

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* Add delay for CLK to be stable */
    arch_delay_us(4);

    if (pword != NULL) {
        ARCH_MemCpy4(pEfuseStart0, pword, countInword);
    }

    if(program){
        EF_Ctrl_Program_Efuse_0();
        while (SET == EF_Ctrl_Busy())
        ;
    }
}

void mfg_efuse_read(uint32_t addr,uint32_t *pword,uint32_t countInword,uint8_t reload)
{
    uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + addr);
    if(reload){
        EF_Ctrl_Load_Efuse_R0();
    }

    ARCH_MemCpy4(pword, pEfuseStart0, countInword);
}

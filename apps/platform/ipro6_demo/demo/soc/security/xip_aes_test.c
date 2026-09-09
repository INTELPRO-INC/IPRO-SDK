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

#include "shell.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <hal_sec_aes.h>
#include <hal_flash.h>

#define USER_DATA_START_ADDR 0xC8000
#define USER_DATA_LEN        0x10000
static uint8_t xip_aes_iv[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x00, 0x00, 0x00, 0x00 };
static uint8_t user_data_buf[4096] ATTR_NOCACHE_RAM_SECTION;

static void _hex_dump(const uint8_t *data, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        if (i % 16 == 0) {
            printf("\r\n%08lx: ", i);
        }
        printf("%02x ", data[i]);
    }
    printf("\r\n");
}

static void change_user_data_and_encrypt_back(uint32_t user_data_offset)
{
    uint8_t *pdata = (uint8_t *)(FLASH_XIP_BASE + USER_DATA_START_ADDR - SF_Ctrl_Get_Flash_Image_Offset(0, 0));
    uint8_t keySel = 1; /* use key slot 4 */
    uint32_t xip_aes_iv_new = user_data_offset / 16;
    sec_aes_handle_t handle;

    /* change iv according to offset */
    xip_aes_iv[15] = xip_aes_iv_new & 0xff;
    xip_aes_iv[14] = (xip_aes_iv_new >> 8) & 0xff;
    xip_aes_iv[13] = (xip_aes_iv_new >> 16) & 0xff;
    xip_aes_iv[12] = (xip_aes_iv_new >> 24) & 0xff;

    memcpy(user_data_buf, pdata + user_data_offset, sizeof(user_data_buf));
    /* change data in temp buffer */
    user_data_buf[0] = 0x12;
    user_data_buf[1] = 0x34;
    user_data_buf[2] = 0x56;
    user_data_buf[3] = 0x78;

    /* use sec eng encrypt the data buf */
    sec_aes_init(&handle, SEC_AES_CTR, SEC_AES_KEY_128);
    sec_aes_setkey(&handle, &keySel, 0, xip_aes_iv, SEC_AES_DIR_ENCRYPT);
    sec_aes_encrypt(&handle, user_data_buf, sizeof(user_data_buf), 0, user_data_buf);
    sec_aes_deinit(&handle);

    /* erase flash */
    flash_erase(USER_DATA_START_ADDR + user_data_offset, sizeof(user_data_buf));
    /* write flash data */
    flash_write(USER_DATA_START_ADDR + user_data_offset, user_data_buf, sizeof(user_data_buf));
}

static int cmd_xip_decrypt_test(int argc, char **argv)
{
    uint8_t *pdata = (uint8_t *)(FLASH_XIP_BASE + USER_DATA_START_ADDR - SF_Ctrl_Get_Flash_Image_Offset(0, 0));
    uint8_t *pdata_another = pdata + 4096;


    printf("XIP decrypt user data case\r\n");

    printf("\r\nBefore enable XIP decrypt,0x%08x=", (unsigned int)pdata);
    _hex_dump(pdata, 256);
    printf("\r\nBefore enable XIP decrypt,0x%08x=", (unsigned int)pdata_another);
    _hex_dump(pdata_another, 256);

    /* enable xip decrypt */
    SF_Ctrl_AES_Set_Mode(SF_CTRL_AES_CTR_MODE);
    SF_Ctrl_AES_Set_Key_BE(1 /* region 1*/, NULL, SF_CTRL_AES_128BITS);
    SF_Ctrl_AES_Set_IV_BE(1 /* region 1*/, xip_aes_iv, USER_DATA_START_ADDR);

    SF_Ctrl_AES_Set_Region(1 /* region 1*/, 1 /*enable this region*/, 1 /*hardware key*/,
                           USER_DATA_START_ADDR,
                           USER_DATA_START_ADDR + USER_DATA_LEN - 1,
                           0 /*not lock*/);
    printf("\r\nAfter enable XIP decrypt,0x%08x=", (unsigned int)pdata);
    /* Invalid those data */
    L1C_DCACHE_INVALID_RANGE(pdata, 256);
    _hex_dump(pdata, 256);
    printf("\r\nAfter enable XIP decrypt,0x%08x=", (unsigned int)pdata_another);
    /* Invalid those data */
    L1C_DCACHE_INVALID_RANGE(pdata_another, 256);
    _hex_dump(pdata_another, 256);

    printf("\r\nUse sec eng change user data\r\n");
    change_user_data_and_encrypt_back(0);
    change_user_data_and_encrypt_back(4096);

    SF_Ctrl_AES_Set_Region(1 /* region 1*/, 0 /*disable this region*/, 1 /*hardware key*/,
                           USER_DATA_START_ADDR,
                           USER_DATA_START_ADDR + USER_DATA_LEN - 1,
                           0 /*not lock*/);
    printf("\r\nBefore enable XIP decrypt,0x%08x=", (unsigned int)pdata);
    /* Invalid those data */
    L1C_DCACHE_INVALID_RANGE(pdata, 256);
    _hex_dump(pdata, 256);
    printf("\r\nBefore enable XIP decrypt,0x%08x=", (unsigned int)pdata_another);
    /* Invalid those data */
    L1C_DCACHE_INVALID_RANGE(pdata_another, 256);
    _hex_dump(pdata_another, 256);

    SF_Ctrl_AES_Set_Region(1 /* region 1*/, 1 /*enable this region*/, 1 /*hardware key*/,
                           USER_DATA_START_ADDR,
                           USER_DATA_START_ADDR + USER_DATA_LEN - 1,
                           0 /*not lock*/);
    printf("\r\nAfter enable XIP decrypt,0x%08x=", (unsigned int)pdata);
    /* Invalid those data */
    L1C_DCACHE_INVALID_RANGE(pdata, 256);
    _hex_dump(pdata, 256);
    printf("\r\nAfter enable XIP decrypt,0x%08x=", (unsigned int)pdata_another);
    /* Invalid those data */
    L1C_DCACHE_INVALID_RANGE(pdata_another, 256);
    _hex_dump(pdata_another, 256);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_xip_decrypt_test, xip_decrypt, XIP decrypt demo);

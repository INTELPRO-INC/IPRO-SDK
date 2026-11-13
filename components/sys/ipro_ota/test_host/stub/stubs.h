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

/* What the tests see of the fake board. */
#ifndef IPRO_OTA_TEST_STUBS_H
#define IPRO_OTA_TEST_STUBS_H
#include <stdint.h>
#include "hal_ptmgr.h"

#define FAKE_FLASH_BASE  0x000E0000u
#define FAKE_FLASH_SIZE  0x00040000u

extern uint8_t  fake_flash[FAKE_FLASH_SIZE];
extern unsigned fake_erases;            /* flash_erase calls */
extern uint32_t fake_erased_bytes;
extern int      fake_flash_fail;        /* nonzero: every flash call fails */
extern PtTable_Entry_Config fake_fw;    /* what hal_boot2 hands out */
extern int      fake_pt_updates;        /* hal_boot2_update_ptable calls */

void fake_reset(uint8_t fill);
void fake_sha256(const uint8_t *data, uint32_t len, uint8_t out[32]);
#endif

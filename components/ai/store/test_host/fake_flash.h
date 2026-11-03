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

#ifndef FAKE_FLASH_H
#define FAKE_FLASH_H

#include <stdint.h>

#define FAKE_PART_ADDR 0x00100000u
#define FAKE_MEM_BYTES (256u * 1024u)

typedef struct {
    uint8_t  mem[FAKE_MEM_BYTES];
    uint32_t part_size;
    long     budget;     /* operations before power fails; -1 = never */
    int      dead;       /* power is off: every operation fails */
    long     ops, erases, writes;
} fake_flash_t;

extern fake_flash_t g_flash;

void flash_reset(uint32_t part_size);
/** A reboot after a cut: the flash keeps its contents, operations work again. */
void flash_power_on(void);

#endif

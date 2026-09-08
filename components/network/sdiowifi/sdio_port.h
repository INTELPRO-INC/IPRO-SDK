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


#ifndef __SDIO_PORT_H__
#define __SDIO_PORT_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <hostrouter_log.h>

#ifdef CFG_IPRO_WIFI
#define SDIO_MAWOC(addr) ((((uintptr_t)(addr) & ~0xF0000000) | 0x20000000))
#else
#define SDIO_MAWOC(addr) (addr)
#endif

void sdio_glb_reset(void);
void sdio_gpio_init(void);

uint8_t sdu_read_s_reg(uint32_t offset);
void sdu_write_s_reg(uint32_t offset, uint8_t value);

uint32_t sdio_get_rxbuf(uint8_t port_num);
void sdio_attach_rxbuf(uint8_t fn_num, uint8_t port_num);

void sdio_host_check_init(void);

void gen_card2host_int(void);

/* NOTE: the IPRO6 SMID transport exposes its device handle via the dedicated
 * sdio_port_ipro6.h (NOT here): pulling ipro6_sdio3.h -> drv_common.h ->
 * platform_common.h into this widely-included header collides with the wifi
 * stack's platform_al.h declaration of platform_get_random. */

#endif

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

#pragma once
#include "psa_ipro/asm.h"
#define PSA_XIP_BASE          PSA_U(0x13000000)
#define PSA_SPE_FLASH_BASE    PSA_U(0x13000000)
#define PSA_SPE_FLASH_SIZE    PSA_U(0x00040000)
#define PSA_NSPE_FLASH_BASE   PSA_U(0x13040000)
#define PSA_NSPE_FLASH_SIZE   PSA_U(0x00040000)
#define PSA_BOOT2_PARAM_BASE  PSA_U(0x11010000)
#define PSA_BOOT2_PARAM_SIZE  PSA_U(0x00000400)
#define PSA_SPE_RAM_BASE      PSA_U(0x11010400)
#define PSA_SPE_RAM_END       PSA_U(0x11020000)
#define PSA_SPE_STACK_SIZE    PSA_U(0x00002000)
#define PSA_NSPE_OCRAM_BASE   PSA_U(0x11020000)
#define PSA_NSPE_OCRAM_SIZE   PSA_U(0x00020000)
#define PSA_NSPE_OCRAM2_BASE  PSA_U(0x11040000)
#define PSA_NSPE_OCRAM2_SIZE  PSA_U(0x00010000)
#define PSA_NSPE_PSRAM_BASE   PSA_U(0x1e000000)
#define PSA_NSPE_PSRAM_SIZE   PSA_U(0x00800000)
/* UART0 (console) without RFDTBT2 */
#define PSA_NSPE_UART_BASE    PSA_U(0x3000B000)
#define PSA_NSPE_UART_SIZE    PSA_U(0x00001000)
/* DMA0 and USB register blocks (CONFIG_PSA_NSPE_DMA_USB) */
#define PSA_NSPE_DMA_BASE     PSA_U(0x30082000)
#define PSA_NSPE_DMA_SIZE     PSA_U(0x00001000)
#define PSA_NSPE_USB_BASE     PSA_U(0x30084000)
#define PSA_NSPE_USB_SIZE     PSA_U(0x00001000)
#define PSA_ITS_PARTITION     "PSM"

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

#ifndef __HAL_COMMON__H__
#define __HAL_COMMON__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_common.h"

#define COMMON_CAPS_INT_TX   (1 << 0)
#define COMMON_CAPS_INT_RX   (1 << 1)
#define COMMON_CAPS_DMA_TX   (1 << 2)
#define COMMON_CAPS_DMA_RX   (1 << 3)

#define COMMON_AlignCeil(x, N)          ( ((x) + ((N) - 1)) / (N) * (N) ) /*abner test*/


typedef enum {
    HAL_REBOOT_AS_BOOTPIN,           /*!< reboot as bootpin level */
    HAL_REBOOT_FROM_INTERFACE,       /*!< reboot from interface, download mode */
    HAL_REBOOT_FROM_MEDIA,           /*!< reboot from media, running mode */
    HAL_REBOOT_MAX                   /*!< reboot max value */
} hal_reboot_cfg_t;

void cpu_global_irq_enable(void);
void cpu_global_irq_disable(void);
void hal_por_reset(void);
void hal_system_reset(void);
void hal_cpu_reset(void);
void hal_reboot_config(hal_reboot_cfg_t rbot);
void hal_get_chip_id(uint8_t chip_id[8]);
void hal_enter_usb_iap(void);
void hal_jump2app(uint32_t flash_offset);
int hal_get_trng_seed(void);
void hal_system_init(void);
int hal_is_cacheable_addr(const void *addr);
void* hal_get_noncacheable_addr(const void *addr);


#ifdef __cplusplus
}
#endif

#endif

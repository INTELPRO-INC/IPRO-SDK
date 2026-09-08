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

/*
 * PSRAM Early Initialization Header
 */

#ifndef __PSRAM_INIT_H__
#define __PSRAM_INIT_H__

#include <stdint.h>

#if defined(CONFIG_LINK_FLASH) && defined(CONFIG_USE_PSRAM)

/**
 * @brief Initialize PSRAM and load PSRAM sections
 * 
 * Called from _premain_init() after SysTimer initialization.
 */
void psram_init_and_load(void);

/* Global flag to indicate PSRAM availability */
extern uint32_t __psram_available;

#endif /* CONFIG_LINK_FLASH && CONFIG_USE_PSRAM */

#endif /* __PSRAM_INIT_H__ */

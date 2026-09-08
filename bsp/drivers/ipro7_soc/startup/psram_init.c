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
 * PSRAM availability flag
 *
 * PSRAM HW init + section loading is done in startup.S via
 * psram_early_hw_init() (or boot2 via GLB_RSV3 magic check).
 * DQS config and this flag are set in _premain_init().
 */

#include <stdint.h>

#if defined(CONFIG_LINK_FLASH) && defined(CONFIG_USE_PSRAM)

/* Global flag to indicate PSRAM availability */
uint32_t __psram_available = 0;

#endif /* CONFIG_LINK_FLASH && CONFIG_USE_PSRAM */

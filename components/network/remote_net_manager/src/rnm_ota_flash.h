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

#include "rnm_ota_core.h"

/* FW entry slot 1 over the ROM flash API. */
const rnm_ota_store_t *rnm_ota_flash_store(void);
/* Active FW entry activeIndex (0 or 1), or -1 on error. */
int rnm_ota_flash_pending(void);
/* boot2's reason for not installing the last package, 0 if none. */
uint8_t rnm_ota_boot2_result(void);

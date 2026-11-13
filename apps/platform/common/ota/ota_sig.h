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
 * Transport-agnostic OTA staging core (see ota_common.h).
 */

/* Moved to components/sys/ipro_ota. This file keeps the old path working for
 * applications that add it to their sources or include it; new code enables
 * CONFIG_IPRO_OTA and includes ipro_ota.h / ipro_ota_sig.h instead. */
#ifndef OTA_SIG_H
#define OTA_SIG_H
#include "../../../../components/sys/ipro_ota/include/ipro_ota_sig.h"
#endif /* OTA_SIG_H */

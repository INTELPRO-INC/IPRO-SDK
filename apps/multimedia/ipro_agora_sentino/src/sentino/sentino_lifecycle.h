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

#ifndef SENTINO_LIFECYCLE_H
#define SENTINO_LIFECYCLE_H

/**
 * @brief Report device bind status
 * @return 0 on success, negative on failure
 */
int sentino_lifecycle_bind(void);

/**
 * @brief Report device info (model, firmware version, etc.)
 * @return 0 on success, negative on failure
 */
int sentino_lifecycle_info_report(void);

/**
 * @brief Request server time synchronization
 * @return 0 on success, negative on failure
 */
int sentino_lifecycle_time_sync(void);

/**
 * @brief Handle factory reset command from server
 */
void sentino_lifecycle_reset(void);

#endif /* SENTINO_LIFECYCLE_H */

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

/**
 * @file app_keys.h
 * @brief Board keys -> VCP volume renderer (IPRO7AI Reef)
 */

#ifndef APP_KEYS_H
#define APP_KEYS_H

/**
 * @brief Start the key poll task.
 *
 * Call after the VCP volume renderer is registered; a key pressed before
 * that is ignored by the stack.
 *
 * @return 0 on success, -1 if the task could not be created
 */
int app_keys_init(void);

#endif /* APP_KEYS_H */

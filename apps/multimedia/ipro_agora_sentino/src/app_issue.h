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
 * @file app_issue.h
 * @brief Issue dispatcher — handles cloud commands (volume, agora_join, reset, etc.)
 */

#ifndef APP_ISSUE_H
#define APP_ISSUE_H

#include "app_module.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Configuration for the issue dispatcher */
typedef struct {
    const audio_output_ops_t *audio_out; /* for volume control */
    const rtc_ops_t          *rtc;       /* for channel rejoin */
    const cloud_ops_t        *cloud;     /* for issue response + report */
} app_issue_cfg_t;

/**
 * @brief Initialize the issue dispatcher with module ops.
 *
 * Returns a cloud_msg_handler_t that can be registered with the cloud
 * module's set_issue_handler().
 */
app_err_t app_issue_init(const app_issue_cfg_t *cfg);

/**
 * @brief Get the issue handler callback (to register with cloud module)
 */
cloud_msg_handler_t app_issue_get_handler(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_ISSUE_H */

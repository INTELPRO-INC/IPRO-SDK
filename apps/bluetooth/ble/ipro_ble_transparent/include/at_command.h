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

#ifndef __AT_COMMAND_H__
#define __AT_COMMAND_H__

#include <stdint.h>
#include <stddef.h>

typedef int (*at_resp_fn)(const char *resp);

int at_command_init(at_resp_fn resp_fn);
int at_command_process(const char *line);
int at_command_load_config(void);
int at_command_save_config(void);

#endif /* __AT_COMMAND_H__ */

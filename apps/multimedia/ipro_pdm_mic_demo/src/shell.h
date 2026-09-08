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

#include <generated/autoconf.h>

#if defined(CONFIG_SHELL)
#include_next "shell.h"
#else
/*
 * Product PDM build can disable CONFIG_SHELL while keeping source files that
 * still contain legacy shell command handlers.  Do not register commands when
 * the shell is off; taking the function address keeps -Wunused-function from
 * becoming a build error under the app's -Werror policy.
 */
#define SHELL_CMD_EXPORT_ALIAS(fn, name, desc) \
    static int (* const __shell_keep_##fn)(int, char **) __attribute__((unused)) = (fn)
#endif

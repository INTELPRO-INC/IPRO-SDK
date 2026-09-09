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

#ifndef VERSION_H
#define VERSION_H

#ifndef VERSION_STR_POSTFIX
#define VERSION_STR_POSTFIX ""
#endif /* VERSION_STR_POSTFIX */

#ifndef GIT_VERSION_STR_POSTFIX
#define GIT_VERSION_STR_POSTFIX ""
#endif /* GIT_VERSION_STR_POSTFIX */

#define VERSION_STR "2.10-devel" VERSION_STR_POSTFIX GIT_VERSION_STR_POSTFIX

#endif /* VERSION_H */

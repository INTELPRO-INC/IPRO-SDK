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

#ifndef __ELOOP_RTOS_H
#define __ELOOP_RTOS_H

#ifdef __cplusplus
extern "C" {
#endif

#define ELOOP_MSG_INBUF_LEN     600
#define ELOOP_MSG_OUTBUF_LEN    128

enum {
    ELOOP_EVT_WPA_IFACE_CTRL = 1,
    ELOOP_EVT_WPA_DRV_CTRL   = 2,
    ELOOP_EVT_WPA_L2_DATA    = 3,
};

#ifdef __cplusplus
}
#endif

/* __ELOOP_RTOS_H */
#endif

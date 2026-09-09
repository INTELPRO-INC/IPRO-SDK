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

#ifndef __IPRO_COEX_IMPL_H__
#define __IPRO_COEX_IMPL_H__

#include <ipro_coex.h>

/* Per-chip backend. One ipro_coex_impl_<chip>.c implements all of these. */

int ipro_coex_dump_all_impl(void);
int ipro_coex_init_default_impl(void);
int ipro_coex_force_wlan_impl(void);
int ipro_coex_force_bt_impl(void);
int ipro_coex_force_mode_impl(enum IPRO_COEX_FORCE_MODE mode);

int ipro_coex_event_handler_impl(enum IPRO_COEX_EVENT event, void *event_arg);

#endif /* __IPRO_COEX_IMPL_H__ */

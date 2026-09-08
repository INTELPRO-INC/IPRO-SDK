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


#ifndef __NOTIFIER_H__
#define __NOTIFIER_H__

#include <utils_list.h>

typedef struct utils_list ntf_list_t;
typedef int (*utils_notifier_fn_t)(void *cb_arg, void *env);

typedef struct utils_notifier {
    struct utils_list_hdr node;
    utils_notifier_fn_t cb;
    void *cb_arg;
    int priority;
} utils_notifier_t;

int utils_notifier_chain_init(ntf_list_t *nl);
int utils_notifier_chain_register(ntf_list_t *nl, utils_notifier_t *node);
int utils_notifier_chain_call(ntf_list_t *nl, void *env);

#endif


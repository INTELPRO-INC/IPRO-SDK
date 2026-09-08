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


#include "utils_notifier.h"

int utils_notifier_chain_init(ntf_list_t *nl)
{
    utils_list_init((struct utils_list *)nl);

    return 0;
}

int utils_notifier_chain_register(ntf_list_t *nl, utils_notifier_t *node)
{
    if (!nl || !node) {
        return -1;
    }
    utils_list_push_back((struct utils_list *)nl, (struct utils_list_hdr *)node);

    return 0;
}

int utils_notifier_chain_call(ntf_list_t *nl, void *env)
{
    utils_notifier_t *raw;

    if (!nl) {
        return 0;
    }

    raw = (utils_notifier_t *)nl->first;

    while (raw) {
        if (raw->cb) {
            raw->cb(raw->cb_arg, env);
        }
        raw = (utils_notifier_t *)raw->node.next;
    }

    return 0;
}


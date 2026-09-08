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

#ifndef __UTILS_MEMP_H__
#define __UTILS_MEMP_H__


struct utils_memp_node {
    struct utils_memp_node *next;
};

typedef struct pool {
    uint32_t node_size;
    uint32_t pool_cap;
    uint32_t pool_size;
    uint8_t align_req;
    uint32_t padded_node_size;
    void *first_node;
    void *last_node;
    struct utils_memp_node *mem;
}utils_memp_pool_t;

int utils_memp_init(utils_memp_pool_t **pool, uint16_t node_size, uint16_t pool_cap, uint8_t align_req);
int utils_memp_init_alloced(utils_memp_pool_t *pool, uint16_t node_size, uint16_t pool_cap, uint8_t align_req);
int utils_memp_deinit(utils_memp_pool_t *pool);
void *utils_memp_malloc(utils_memp_pool_t *pool);
int utils_memp_free(utils_memp_pool_t *pool, void *node);

#endif

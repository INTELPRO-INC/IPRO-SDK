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

#ifndef _LFS_PORT_H
#define _LFS_PORT_H

#include <generated/autoconf.h>
#include "lfs.h"

#ifdef CONFIG_FREERTOS
#include <FreeRTOS.h>
#include <semphr.h>
#endif

struct lfs_context {
    lfs_t lfs;
    uint32_t flash_addr;
#ifdef CONFIG_FREERTOS
    SemaphoreHandle_t fs_giant_lock;
#endif
#ifdef CONFIG_LITTLEFS_USE_PARTITION_TABLE
    char *partition_name;
#endif
};

lfs_t *lfs_xip_init(struct lfs_context *lfs_xip_ctx, struct lfs_config *cfg);

int lfs_xip_flash_read(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, void *buffer, lfs_size_t size);
int lfs_xip_flash_prog(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, const void *buffer, lfs_size_t size);
int lfs_xip_flash_erase(const struct lfs_config *c, lfs_block_t block);
int lfs_xip_flash_sync(const struct lfs_config *c);

#if defined(LFS_THREADSAFE) && defined(CONFIG_FREERTOS)
int lfs_xip_flash_lock(const struct lfs_config *c);
int lfs_xip_flash_unlock(const struct lfs_config *c);
#endif

#endif

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

/*
 * LittleFS Flash Port for IPRO SDK
 * Adapted for IPRO platform
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <generated/autoconf.h>

#include "lfs.h"
#include "hal_flash.h"
#include "hal_boot2.h"

#ifdef CONFIG_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif
#include "lfs_port.h"
#include "drv_sf_ctrl.h"

#if defined(LFS_THREADSAFE) && defined(CONFIG_FREERTOS)
int lfs_xip_flash_lock(const struct lfs_config *c);
int lfs_xip_flash_unlock(const struct lfs_config *c);
#endif

lfs_t *lfs_xip_init(struct lfs_context *lfs_xip_ctx, struct lfs_config *cfg)
{
    int ret;
    lfs_t *lfs = &lfs_xip_ctx->lfs;

    if (lfs->cfg == cfg) {
        return lfs;
    } else {
        cfg->context = lfs_xip_ctx;
        cfg->read = lfs_xip_flash_read;
        cfg->prog = lfs_xip_flash_prog;
        cfg->erase = lfs_xip_flash_erase;
        cfg->sync = lfs_xip_flash_sync;
#if defined(LFS_THREADSAFE) && defined(CONFIG_FREERTOS)
        cfg->lock = lfs_xip_flash_lock;
        cfg->unlock = lfs_xip_flash_unlock;
#endif
    }

#ifdef CONFIG_LITTLEFS_USE_PARTITION_TABLE
    uint32_t addr, size;

    ret = hal_boot2_partition_addr_active(lfs_xip_ctx->partition_name, &addr, &size);
    if (ret != 0) {
        printf("[LFS] no valid %s partition found\r\n", lfs_xip_ctx->partition_name);
        errno = LFS_ERR_IO;
        return NULL;
    }

    printf("[LFS] Found valid %s partition, flash addr 0x%08lx, size %lu\r\n",
           lfs_xip_ctx->partition_name,
           addr,
           size);
    lfs_xip_ctx->flash_addr = addr;
    cfg->block_count = size / cfg->block_size;
#else
    /* Use fixed flash address from Kconfig */
    lfs_xip_ctx->flash_addr = CONFIG_LITTLEFS_FLASH_ADDRESS;
    cfg->block_count = CONFIG_LITTLEFS_FLASH_SIZE / cfg->block_size;
#endif

#ifdef CONFIG_FREERTOS
#if configUSE_RECURSIVE_MUTEXES
    lfs_xip_ctx->fs_giant_lock = xSemaphoreCreateRecursiveMutex();
#else
    lfs_xip_ctx->fs_giant_lock = xSemaphoreCreateMutex();
#endif
#endif

    /* mount the filesystem */
    ret = lfs_mount(lfs, cfg);

    /* reformat if we can't mount the filesystem */
    /* this should only happen on the first boot */
    if (ret == LFS_ERR_CORRUPT) {
        printf("[LFS] try to reformat\r\n");
        ret = lfs_format(lfs, cfg);
        if (ret) {
            printf("[LFS] reformat fail\r\n");
            errno = LFS_ERR_CORRUPT;
            return NULL;
        }

        printf("[LFS] reformat success\r\n");
        ret = lfs_mount(lfs, cfg);
        if (ret) {
            errno = ret;
            return NULL;
        }
    } else if (ret != LFS_ERR_OK) {
        errno = ret;
        return NULL;
    }

    printf("[LFS] mount success\r\n");

    return lfs;
}

#if defined(LFS_THREADSAFE) && defined(CONFIG_FREERTOS)
int lfs_xip_flash_lock(const struct lfs_config *c)
{
    struct lfs_context *ctx = c->context;
    if (ctx->fs_giant_lock == NULL) {
        return 0;  /* No lock created yet (e.g., during format before mount) */
    }
#if configUSE_RECURSIVE_MUTEXES
    xSemaphoreTakeRecursive(ctx->fs_giant_lock, portMAX_DELAY);
#else
    xSemaphoreTake(ctx->fs_giant_lock, portMAX_DELAY);
#endif
    return 0;
}

int lfs_xip_flash_unlock(const struct lfs_config *c)
{
    struct lfs_context *ctx = c->context;
    if (ctx->fs_giant_lock == NULL) {
        return 0;  /* No lock created yet (e.g., during format before mount) */
    }
#if configUSE_RECURSIVE_MUTEXES
    xSemaphoreGiveRecursive(ctx->fs_giant_lock);
#else
    xSemaphoreGive(ctx->fs_giant_lock);
#endif
    return 0;
}
#endif

/*****************************************************************************
* @brief        Read a region in a block. Negative error codes are propagated
*               to the user.
* @param[in]    c
* @param[in]    block
* @param[in]    off
* @param[out]   buffer
* @param[in]    size
*
* @retval int
*****************************************************************************/
int lfs_xip_flash_read(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, void *buffer, lfs_size_t size)
{
    struct lfs_context *ctx = c->context;
    uint32_t xip_addr = FLASH_XIP_BASE + ctx->flash_addr +
                         block * c->block_size + off -
                         SF_Ctrl_Get_Flash_Image_Offset(0, 0);
    MInvalDCacheRange(xip_addr, size);
    memcpy(buffer, (const void *)xip_addr, size);
    return 0;
}

/*****************************************************************************
* @brief        Program a region in a block. The block must have previously
*               been erased. Negative error codes are propagated to the user.
*               May return LFS_ERR_CORRUPT if the block should be considered bad.
* @param[in]    c
* @param[in]    block
* @param[in]    off
* @param[in]    buffer
* @param[in]    size
*
* @retval int
*****************************************************************************/
int lfs_xip_flash_prog(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, const void *buffer, lfs_size_t size)
{
    struct lfs_context *ctx = c->context;
    uint32_t flash_off = ctx->flash_addr + block * c->block_size + off;
    int ret = flash_write(flash_off, (uint8_t *)buffer, size);
    uint32_t xip_addr = FLASH_XIP_BASE + flash_off -
                         SF_Ctrl_Get_Flash_Image_Offset(0, 0);
    MInvalDCacheRange(xip_addr, size);
    return ret;
}

/*****************************************************************************
* @brief        Erase a block. A block must be erased before being programmed.
*               The state of an erased block is undefined. Negative error codes
*               are propagated to the user.
*               May return LFS_ERR_CORRUPT if the block should be considered bad.
* @param[in]    c
* @param[in]    block
*
* @retval int
*****************************************************************************/
int lfs_xip_flash_erase(const struct lfs_config *c, lfs_block_t block)
{
    struct lfs_context *ctx = c->context;
    uint32_t flash_off = ctx->flash_addr + block * c->block_size;
    int ret = flash_erase(flash_off, c->block_size);
    uint32_t xip_addr = FLASH_XIP_BASE + flash_off -
                         SF_Ctrl_Get_Flash_Image_Offset(0, 0);
    MInvalDCacheRange(xip_addr, c->block_size);
    return ret;
}

/*****************************************************************************
* @brief        Sync the state of the underlying block device. Negative error
*               codes are propagated to the user.
* @param[in]    c
*
* @retval int
*****************************************************************************/
int lfs_xip_flash_sync(const struct lfs_config *c)
{
    /* if use xip, may need to clean cache */
    (void)c;
    return 0;
}

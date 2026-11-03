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
 * Host lfs_xip_init: the mount, reformat and error rules of
 * components/fs/littlefs/port/lfs_xip_flash.c on a RAM NOR flash
 * (erase to 0xff, program ANDs).
 */
#define _DEFAULT_SOURCE

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include "lfs_port.h"
#include "lfs_ram.h"

static uint8_t *g_flash;
static size_t g_blocks;
static size_t g_first;
static size_t g_count;

int lfs_ram_init(size_t n)
{
    g_flash = mmap(NULL, n * LFS_RAM_BLOCK, PROT_READ | PROT_WRITE,
                   MAP_SHARED | MAP_ANON, -1, 0);
    if (g_flash == MAP_FAILED) {
        g_flash = NULL;
        return -1;
    }
    g_blocks = n;
    memset(g_flash, 0xff, n * LFS_RAM_BLOCK);
    return 0;
}

uint8_t *lfs_ram_block(size_t b)
{
    return &g_flash[b * LFS_RAM_BLOCK];
}

void lfs_ram_window(size_t first, size_t count)
{
    g_first = first;
    g_count = count;
}

void lfs_ram_fill(size_t first, size_t count, uint8_t v)
{
    memset(lfs_ram_block(first), v, count * LFS_RAM_BLOCK);
}

static uint8_t *ram_at(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off)
{
    const struct lfs_context *ctx = c->context;

    return &g_flash[ctx->flash_addr + block * c->block_size + off];
}

int lfs_xip_flash_read(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, void *buffer, lfs_size_t size)
{
    memcpy(buffer, ram_at(c, block, off), size);
    return 0;
}

int lfs_xip_flash_prog(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, const void *buffer, lfs_size_t size)
{
    uint8_t *p = ram_at(c, block, off);
    const uint8_t *b = buffer;

    for (lfs_size_t i = 0; i < size; i++) {
        p[i] &= b[i];
    }
    return 0;
}

int lfs_xip_flash_erase(const struct lfs_config *c, lfs_block_t block)
{
    memset(ram_at(c, block, 0), 0xff, c->block_size);
    return 0;
}

int lfs_xip_flash_sync(const struct lfs_config *c)
{
    (void)c;
    return 0;
}

lfs_t *lfs_xip_init(struct lfs_context *ctx, struct lfs_config *cfg)
{
    lfs_t *lfs = &ctx->lfs;
    int ret;

    if (lfs->cfg == cfg) {
        return lfs;
    }
    cfg->context = ctx;
    cfg->read = lfs_xip_flash_read;
    cfg->prog = lfs_xip_flash_prog;
    cfg->erase = lfs_xip_flash_erase;
    cfg->sync = lfs_xip_flash_sync;
    if (g_flash == NULL || g_count == 0 || g_first + g_count > g_blocks) {
        printf("[LFS] no valid %s partition found\r\n", ctx->partition_name);
        errno = LFS_ERR_IO;
        return NULL;
    }
    ctx->flash_addr = (uint32_t)(g_first * LFS_RAM_BLOCK);
    cfg->block_count = (lfs_size_t)(g_count * LFS_RAM_BLOCK /
                                    cfg->block_size);
    ret = lfs_mount(lfs, cfg);
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

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
 * LittleFS CLI Commands for IPRO SDK
 * Uses the new shell (RT-Thread FinSH based)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "lfs.h"
#include "lfs_port.h"
#include "hal_boot2.h"

#ifdef CONFIG_SHELL
#include "shell.h"
#endif

/* EVFS integration */
#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_USE_LITTLEFS)
#include "evfs.h"
#include "evfs/littlefs_fs.h"
#include "evfs_posix.h"
#define LFS_USE_EVFS 1
#define LFS_MOUNT_POINT "/lfs"
#endif

/* LFS context and config - must be initialized by application before using CLI */
static struct lfs_context lfs_ctx;
static struct lfs_config lfs_cfg = {
    .read_size = 16,
    .prog_size = 16,
    .block_size = 4096,
    .cache_size = 256,
    .lookahead_size = 16,
    .block_cycles = 500,
};

static lfs_t *lfs = NULL;
static int lfs_mounted = 0;

/* Get the mounted LittleFS instance (for external use) */
lfs_t *lfs_get_instance(void)
{
    return lfs_mounted ? lfs : NULL;
}

/* Mount LittleFS on the given partition (callable from code or shell) */
int lfs_mount_partition(const char *partition_name)
{
    if (lfs_mounted) {
        printf("LFS already mounted\r\n");
        return 0;
    }

    lfs_ctx.partition_name = (char *)partition_name;

    lfs = lfs_xip_init(&lfs_ctx, &lfs_cfg);
    if (lfs == NULL) {
        printf("LFS mount failed\r\n");
        return -1;
    }

    lfs_mounted = 1;

#ifdef LFS_USE_EVFS
    /* Register with EVFS (evfs_init() should be called in main.c) */
    int ret = evfs_register_littlefs("lfs", lfs, false);
    if (ret != EVFS_OK) {
        printf("EVFS registration failed: %d\r\n", ret);
    } else {
        /* Mount at /lfs mount point */
        ret = evfs_posix_mount(LFS_MOUNT_POINT, "lfs");
        if (ret != 0) {
            printf("EVFS mount failed: %d\r\n", ret);
        } else {
            printf("LFS mounted at %s\r\n", LFS_MOUNT_POINT);
        }
    }
#endif

    printf("LFS mounted successfully\r\n");
    return 0;
}

static int lfs_mount_cmd(int argc, char **argv)
{
    const char *name = (argc >= 2) ? argv[1] : "PSM";
    return lfs_mount_partition(name);
}
SHELL_CMD_EXPORT_ALIAS(lfs_mount_cmd, lfs_mount, mount littlefs [partition]);

static int lfs_unmount_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (!lfs_mounted) {
        printf("LFS not mounted\r\n");
        return -1;
    }

#ifdef LFS_USE_EVFS
    /* Unmount and unregister from EVFS */
    evfs_posix_umount(LFS_MOUNT_POINT);
    Evfs *vfs = evfs_find_vfs("lfs");
    if (vfs) {
        evfs_unregister(vfs);
    }
#endif

    lfs_unmount(lfs);
    lfs_mounted = 0;
    printf("LFS unmounted\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(lfs_unmount_cmd, lfs_unmount, unmount littlefs);

static int lfs_ls_cmd(int argc, char **argv)
{
    if (!lfs_mounted) {
        printf("LFS not mounted\r\n");
        return -1;
    }

    const char *path = (argc >= 2) ? argv[1] : "/";

    lfs_dir_t dir;
    int err = lfs_dir_open(lfs, &dir, path);
    if (err) {
        printf("Failed to open directory: %d\r\n", err);
        return err;
    }

    struct lfs_info info;
    while (lfs_dir_read(lfs, &dir, &info) > 0) {
        if (info.type == LFS_TYPE_DIR) {
            printf("  [DIR] %s\r\n", info.name);
        } else {
            printf("  [FILE] %s (%lu bytes)\r\n", info.name, (unsigned long)info.size);
        }
    }

    lfs_dir_close(lfs, &dir);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(lfs_ls_cmd, lfs_ls, list files [path]);

static int lfs_cat_cmd(int argc, char **argv)
{
    if (!lfs_mounted) {
        printf("LFS not mounted\r\n");
        return -1;
    }

    if (argc < 2) {
        printf("usage: lfs_cat <filename>\r\n");
        return -1;
    }

    lfs_file_t file;
    int err = lfs_file_open(lfs, &file, argv[1], LFS_O_RDONLY);
    if (err) {
        printf("Failed to open file: %d\r\n", err);
        return err;
    }

    char read_buf[64];
    lfs_ssize_t size;
    while ((size = lfs_file_read(lfs, &file, read_buf, sizeof(read_buf) - 1)) > 0) {
        read_buf[size] = '\0';
        printf("%s", read_buf);
    }
    printf("\r\n");

    lfs_file_close(lfs, &file);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(lfs_cat_cmd, lfs_cat, cat file content);

static int lfs_write_cmd(int argc, char **argv)
{
    if (!lfs_mounted) {
        printf("LFS not mounted\r\n");
        return -1;
    }

    if (argc < 3) {
        printf("usage: lfs_write <filename> <content>\r\n");
        return -1;
    }

    lfs_file_t file;
    int err = lfs_file_open(lfs, &file, argv[1], LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC);
    if (err) {
        printf("Failed to open file: %d\r\n", err);
        return err;
    }

    lfs_ssize_t size = lfs_file_write(lfs, &file, argv[2], strlen(argv[2]));
    if (size < 0) {
        printf("Write failed: %d\r\n", (int)size);
        lfs_file_close(lfs, &file);
        return (int)size;
    }

    printf("Wrote %d bytes\r\n", (int)size);
    lfs_file_close(lfs, &file);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(lfs_write_cmd, lfs_write, write file content);

static int lfs_rm_cmd(int argc, char **argv)
{
    if (!lfs_mounted) {
        printf("LFS not mounted\r\n");
        return -1;
    }

    if (argc < 2) {
        printf("usage: lfs_rm <filename>\r\n");
        return -1;
    }

    int err = lfs_remove(lfs, argv[1]);
    if (err) {
        printf("Remove failed: %d\r\n", err);
        return err;
    }

    printf("Removed: %s\r\n", argv[1]);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(lfs_rm_cmd, lfs_rm, remove file);

static int lfs_mkdir_cmd(int argc, char **argv)
{
    if (!lfs_mounted) {
        printf("LFS not mounted\r\n");
        return -1;
    }

    if (argc < 2) {
        printf("usage: lfs_mkdir <dirname>\r\n");
        return -1;
    }

    int err = lfs_mkdir(lfs, argv[1]);
    if (err) {
        printf("Mkdir failed: %d\r\n", err);
        return err;
    }

    printf("Created directory: %s\r\n", argv[1]);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(lfs_mkdir_cmd, lfs_mkdir, make directory);

static int lfs_format_cmd(int argc, char **argv)
{
    if (lfs_mounted) {
        printf("Unmount LFS first\r\n");
        return -1;
    }

    lfs_ctx.partition_name = (argc >= 2) ? argv[1] : "PSM";

    /* Initialize context for formatting */
    lfs_cfg.context = &lfs_ctx;
    lfs_cfg.read = lfs_xip_flash_read;
    lfs_cfg.prog = lfs_xip_flash_prog;
    lfs_cfg.erase = lfs_xip_flash_erase;
    lfs_cfg.sync = lfs_xip_flash_sync;
#if defined(LFS_THREADSAFE) && defined(CONFIG_FREERTOS)
    lfs_cfg.lock = lfs_xip_flash_lock;
    lfs_cfg.unlock = lfs_xip_flash_unlock;
#endif

#ifdef CONFIG_LITTLEFS_USE_PARTITION_TABLE
    uint32_t addr, size;
    int ret = hal_boot2_partition_addr_active(lfs_ctx.partition_name, &addr, &size);
    if (ret != 0) {
        printf("[LFS] no valid %s partition found\r\n", lfs_ctx.partition_name);
        return -1;
    }
    printf("[LFS] Format partition %s, flash addr 0x%08" PRIx32 ", size %" PRIu32 "\r\n",
           lfs_ctx.partition_name, addr, size);
    lfs_ctx.flash_addr = addr;
    lfs_cfg.block_count = size / lfs_cfg.block_size;
#else
    lfs_ctx.flash_addr = CONFIG_LITTLEFS_FLASH_ADDRESS;
    lfs_cfg.block_count = CONFIG_LITTLEFS_FLASH_SIZE / lfs_cfg.block_size;
#endif

    int err = lfs_format(&lfs_ctx.lfs, &lfs_cfg);
    if (err) {
        printf("Format failed: %d\r\n", err);
        return err;
    }

    printf("LFS formatted successfully\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(lfs_format_cmd, lfs_format, format littlefs);

static int lfs_info_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (!lfs_mounted) {
        printf("LFS not mounted\r\n");
        return -1;
    }

    lfs_ssize_t used = lfs_fs_size(lfs);
    if (used < 0) {
        printf("Failed to get FS size: %d\r\n", (int)used);
        return (int)used;
    }

    printf("LFS Info:\r\n");
    printf("  Block size: %lu bytes\r\n", (unsigned long)lfs_cfg.block_size);
    printf("  Block count: %lu\r\n", (unsigned long)lfs_cfg.block_count);
    printf("  Total size: %lu bytes\r\n", (unsigned long)(lfs_cfg.block_size * lfs_cfg.block_count));
    printf("  Used blocks: %lu\r\n", (unsigned long)used);
    printf("  Used size: %lu bytes\r\n", (unsigned long)(used * lfs_cfg.block_size));
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(lfs_info_cmd, lfs_info, show littlefs info);

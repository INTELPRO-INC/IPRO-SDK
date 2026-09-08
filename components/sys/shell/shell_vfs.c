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
 * Shell VFS Commands
 * Provides file system browsing commands for shell using EVFS
 */

#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "ipro_osal.h"

#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_POSIX)

#include "evfs.h"
#include "evfs_posix.h"

/* List directory contents */
static int shell_ls(int argc, char **argv)
{
    char path_name[384];
    evfs_DIR *dp;
    struct evfs_dirent *entry;
    struct stat st;

    memset(path_name, 0, sizeof(path_name));

    const char *path = (argc >= 2) ? argv[1] : "/";

    /* Open directory */
    dp = evfs_posix_opendir(path);
    if (dp == NULL) {
        printf("Cannot open directory: %s\r\n", path);
        return -1;
    }

    printf("      Size                           Name              Type\r\n");
    printf("----------------------------------------------------------------------\r\n");

    while ((entry = evfs_posix_readdir(dp)) != NULL) {
        /* Build full path for stat */
        if (strcmp(path, "/") == 0) {
            snprintf(path_name, sizeof(path_name) - 1, "/%s", entry->d_name);
        } else {
            snprintf(path_name, sizeof(path_name) - 1, "%s/%s", path, entry->d_name);
        }

        if (evfs_posix_stat(path_name, &st) == 0) {
            const char *type_str = S_ISDIR(st.st_mode) ? "Directory" : "File";
            printf("%10ld  %30s  %s\r\n", (long)st.st_size, entry->d_name, type_str);
        } else {
            printf("%10s  %30s  %s\r\n", "?", entry->d_name, "Unknown");
        }
    }

    evfs_posix_closedir(dp);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_ls, ls, List directory contents);

/* Display file contents */
static int shell_cat(int argc, char **argv)
{
    int fd;
    char buf[256];
    ssize_t n;

    if (argc < 2) {
        printf("Usage: cat <filename>\r\n");
        return -1;
    }

    fd = evfs_posix_open(argv[1], O_RDONLY);
    if (fd < 0) {
        printf("Cannot open file: %s\r\n", argv[1]);
        return -1;
    }

    while ((n = evfs_posix_read(fd, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("%s", buf);
    }
    printf("\r\n");

    evfs_posix_close(fd);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_cat, cat, Display file contents);

/* Hexdump file contents */
static int shell_hexdump(int argc, char **argv)
{
    int fd;
    unsigned char buf[16];
    ssize_t n;
    int offset = 0;
    int i;

    if (argc < 2) {
        printf("Usage: hexdump <filename>\r\n");
        return -1;
    }

    fd = evfs_posix_open(argv[1], O_RDONLY);
    if (fd < 0) {
        printf("Cannot open file: %s\r\n", argv[1]);
        return -1;
    }

    while ((n = evfs_posix_read(fd, buf, sizeof(buf))) > 0) {
        /* Print offset */
        printf("%08x  ", offset);

        /* Print hex values */
        for (i = 0; i < 16; i++) {
            if (i < n) {
                printf("%02x ", buf[i]);
            } else {
                printf("   ");
            }
            if (i == 7) {
                printf(" ");
            }
        }

        /* Print ASCII */
        printf(" |");
        for (i = 0; i < n; i++) {
            if (buf[i] >= 0x20 && buf[i] < 0x7f) {
                printf("%c", buf[i]);
            } else {
                printf(".");
            }
        }
        printf("|\r\n");

        offset += n;
    }

    printf("%08x\r\n", offset);
    evfs_posix_close(fd);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_hexdump, hexdump, Hexdump file contents);

/* Print working directory */
static int shell_pwd(int argc, char **argv)
{
    char cwd[128];
    (void)argc;
    (void)argv;

    if (evfs_posix_getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\r\n", cwd);
    } else {
        printf("/\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_pwd, pwd, Print working directory);

/* Change directory */
static int shell_cd(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: cd <path>\r\n");
        return -1;
    }

    if (evfs_posix_chdir(argv[1]) != 0) {
        printf("Cannot change to directory: %s\r\n", argv[1]);
        return -1;
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_cd, cd, Change directory);

/* Create directory */
static int shell_mkdir(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: mkdir <path>\r\n");
        return -1;
    }

    if (evfs_posix_mkdir(argv[1], 0755) != 0) {
        printf("Cannot create directory: %s\r\n", argv[1]);
        return -1;
    }
    printf("Directory created: %s\r\n", argv[1]);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_mkdir, mkdir, Create directory);

/* Remove file or directory */
static int shell_rm(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: rm <path>\r\n");
        return -1;
    }

    if (evfs_posix_remove(argv[1]) != 0) {
        printf("Cannot remove: %s\r\n", argv[1]);
        return -1;
    }
    printf("Removed: %s\r\n", argv[1]);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_rm, rm, Remove file or directory);

/* Copy file between filesystems */
static int shell_cp(int argc, char **argv)
{
    int src_fd = -1;
    int dst_fd = -1;
    int ret = -1;
    ssize_t bytes_read, bytes_written;
    size_t total_bytes = 0;
    struct stat st;

    /* Large buffer from PSRAM for better throughput */
    const size_t buf_size = 256 * 1024;
    char *buf = NULL;

    if (argc < 3) {
        printf("Usage: cp <source> <destination>\r\n");
        printf("  Example: cp /lfs/data.bin /sdcard/data.bin\r\n");
        return -1;
    }

    const char *src_path = argv[1];
    const char *dst_path = argv[2];

    /* Check source file exists and get size */
    if (evfs_posix_stat(src_path, &st) != 0) {
        printf("Source file not found: %s\r\n", src_path);
        return -1;
    }

    if (S_ISDIR(st.st_mode)) {
        printf("Cannot copy directory (use recursive copy for directories)\r\n");
        return -1;
    }

    /* Allocate buffer from PSRAM via OSAL */
    buf = ipro_osal_malloc(buf_size);
    if (buf == NULL) {
        printf("Failed to allocate buffer\r\n");
        return -1;
    }

    /* Open source file */
    src_fd = evfs_posix_open(src_path, O_RDONLY);
    if (src_fd < 0) {
        printf("Cannot open source: %s\r\n", src_path);
        goto cleanup;
    }

    /* Open/create destination file */
    dst_fd = evfs_posix_open(dst_path, O_WRONLY | O_CREAT | O_TRUNC);
    if (dst_fd < 0) {
        printf("Cannot create destination: %s\r\n", dst_path);
        goto cleanup;
    }

    /* Copy data */
    printf("Copying %s -> %s (%ld bytes)\r\n", src_path, dst_path, (long)st.st_size);

    while ((bytes_read = evfs_posix_read(src_fd, buf, buf_size)) > 0) {
        bytes_written = evfs_posix_write(dst_fd, buf, bytes_read);
        if (bytes_written != bytes_read) {
            printf("Write error at offset %zu\r\n", total_bytes);
            goto cleanup;
        }
        total_bytes += bytes_written;

        /* Progress indicator for large files */
        if (st.st_size > 0 && total_bytes % (64 * 1024) == 0) {
            printf("  %zu / %ld bytes (%ld%%)\r\n",
                   total_bytes, (long)st.st_size,
                   (long)(total_bytes * 100 / st.st_size));
        }
    }

    if (bytes_read < 0) {
        printf("Read error at offset %zu\r\n", total_bytes);
        goto cleanup;
    }

    /* Sync destination file */
    evfs_posix_fsync(dst_fd);

    printf("Copied %zu bytes successfully\r\n", total_bytes);
    ret = 0;

cleanup:
    if (src_fd >= 0) {
        evfs_posix_close(src_fd);
    }
    if (dst_fd >= 0) {
        evfs_posix_close(dst_fd);
    }
    if (buf != NULL) {
        ipro_osal_free(buf);
    }
    return ret;
}
SHELL_CMD_EXPORT_ALIAS(shell_cp, cp, Copy file between filesystems);

#endif /* CONFIG_EVFS && CONFIG_EVFS_POSIX */

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
 * Filesystem Test for LittleFS and FatFS
 * Tests read/write/verify operations using EVFS POSIX API
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "shell.h"

#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_POSIX)
#include "evfs.h"
#include "evfs_posix.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define TEST_FILE_SIZE      1024
#define TEST_PATTERN_SIZE   256

/* Test result tracking */
static int tests_passed = 0;
static int tests_failed = 0;

static void test_result(const char *name, int passed)
{
    if (passed) {
        printf("  [PASS] %s\r\n", name);
        tests_passed++;
    } else {
        printf("  [FAIL] %s\r\n", name);
        tests_failed++;
    }
}

/* Generate test pattern */
static void generate_pattern(uint8_t *buf, size_t size, uint8_t seed)
{
    for (size_t i = 0; i < size; i++) {
        buf[i] = (uint8_t)((seed + i) & 0xFF);
    }
}

/* Verify test pattern */
static int verify_pattern(const uint8_t *buf, size_t size, uint8_t seed)
{
    for (size_t i = 0; i < size; i++) {
        if (buf[i] != (uint8_t)((seed + i) & 0xFF)) {
            printf("    Mismatch at offset %zu: expected 0x%02X, got 0x%02X\r\n",
                   i, (seed + i) & 0xFF, buf[i]);
            return 0;
        }
    }
    return 1;
}

/* Test basic write operation */
static int test_write(const char *path, size_t size, uint8_t seed)
{
    uint8_t *buf = malloc(size);
    if (!buf) {
        printf("    Failed to allocate buffer\r\n");
        return 0;
    }

    generate_pattern(buf, size, seed);

    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        printf("    Failed to open file for writing: %s\r\n", path);
        free(buf);
        return 0;
    }

    ssize_t written = write(fd, buf, size);
    close(fd);
    free(buf);

    if (written != (ssize_t)size) {
        printf("    Write failed: expected %zu, wrote %zd\r\n", size, written);
        return 0;
    }

    return 1;
}

/* Test basic read operation */
static int test_read(const char *path, size_t size, uint8_t seed)
{
    uint8_t *buf = malloc(size);
    if (!buf) {
        printf("    Failed to allocate buffer\r\n");
        return 0;
    }

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("    Failed to open file for reading: %s\r\n", path);
        free(buf);
        return 0;
    }

    ssize_t bytes_read = read(fd, buf, size);
    close(fd);

    if (bytes_read != (ssize_t)size) {
        printf("    Read failed: expected %zu, read %zd\r\n", size, bytes_read);
        free(buf);
        return 0;
    }

    int result = verify_pattern(buf, size, seed);
    free(buf);
    return result;
}

/* Test file stat */
static int test_stat(const char *path, size_t expected_size)
{
    struct stat st;
    if (stat(path, &st) != 0) {
        printf("    stat() failed for: %s\r\n", path);
        return 0;
    }

    if ((size_t)st.st_size != expected_size) {
        printf("    Size mismatch: expected %zu, got %ld\r\n",
               expected_size, (long)st.st_size);
        return 0;
    }

    return 1;
}

/* Test file delete */
static int test_delete(const char *path)
{
    if (unlink(path) != 0) {
        printf("    unlink() failed for: %s\r\n", path);
        return 0;
    }

    /* Verify file is gone */
    struct stat st;
    if (stat(path, &st) == 0) {
        printf("    File still exists after delete: %s\r\n", path);
        return 0;
    }

    return 1;
}

/* Test append operation */
static int test_append(const char *path, size_t append_size, uint8_t seed)
{
    uint8_t *buf = malloc(append_size);
    if (!buf) {
        printf("    Failed to allocate buffer\r\n");
        return 0;
    }

    generate_pattern(buf, append_size, seed);

    int fd = open(path, O_WRONLY | O_APPEND);
    if (fd < 0) {
        printf("    Failed to open file for append: %s\r\n", path);
        free(buf);
        return 0;
    }

    ssize_t written = write(fd, buf, append_size);
    close(fd);
    free(buf);

    if (written != (ssize_t)append_size) {
        printf("    Append failed: expected %zu, wrote %zd\r\n", append_size, written);
        return 0;
    }

    return 1;
}

/* Test seek and read */
static int test_seek_read(const char *path, off_t offset, size_t size, uint8_t seed)
{
    uint8_t *buf = malloc(size);
    if (!buf) {
        printf("    Failed to allocate buffer\r\n");
        return 0;
    }

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("    Failed to open file: %s\r\n", path);
        free(buf);
        return 0;
    }

    if (lseek(fd, offset, SEEK_SET) != offset) {
        printf("    Seek failed\r\n");
        close(fd);
        free(buf);
        return 0;
    }

    ssize_t bytes_read = read(fd, buf, size);
    close(fd);

    if (bytes_read != (ssize_t)size) {
        printf("    Read after seek failed\r\n");
        free(buf);
        return 0;
    }

    /* Verify pattern at offset */
    int result = verify_pattern(buf, size, (uint8_t)(seed + offset));
    free(buf);
    return result;
}

/* Run filesystem tests on a mount point */
static void run_fs_tests(const char *mount_point, const char *fs_name)
{
    char test_file[64];
    snprintf(test_file, sizeof(test_file), "%s/test.bin", mount_point);

    printf("\r\n=== %s Tests (%s) ===\r\n", fs_name, mount_point);

    /* Reset counters */
    tests_passed = 0;
    tests_failed = 0;

    /* Test 1: Basic write */
    test_result("Write 1KB file", test_write(test_file, TEST_FILE_SIZE, 0x5A));

    /* Test 2: Stat file */
    test_result("Stat file size", test_stat(test_file, TEST_FILE_SIZE));

    /* Test 3: Read and verify */
    test_result("Read and verify", test_read(test_file, TEST_FILE_SIZE, 0x5A));

    /* Test 4: Seek and read */
    test_result("Seek and read", test_seek_read(test_file, 256, 128, 0x5A));

    /* Test 5: Append */
    test_result("Append data", test_append(test_file, 256, 0xA5));
    test_result("Verify new size", test_stat(test_file, TEST_FILE_SIZE + 256));

    /* Test 6: Delete */
    test_result("Delete file", test_delete(test_file));

    /* Test 7: Large write/read */
    size_t large_size = 4096;
    snprintf(test_file, sizeof(test_file), "%s/large.bin", mount_point);
    test_result("Write 4KB file", test_write(test_file, large_size, 0x33));
    test_result("Read 4KB file", test_read(test_file, large_size, 0x33));
    test_result("Delete large file", test_delete(test_file));

    printf("\r\n%s Summary: %d passed, %d failed\r\n",
           fs_name, tests_passed, tests_failed);
}

/* LittleFS test command */
static int cmd_lfs_test(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    const char *mount_point = "/lfs";

    /* Check if mounted */
    struct stat st;
    if (stat(mount_point, &st) != 0) {
        printf("LittleFS not mounted at %s\r\n", mount_point);
        printf("Run 'lfs_mount' first\r\n");
        return -1;
    }

    run_fs_tests(mount_point, "LittleFS");
    return (tests_failed == 0) ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_lfs_test, lfs_test, LittleFS read/write test);

/* FatFS test command */
static int cmd_fatfs_test(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    const char *mount_point = "/sdcard";

    /* Check if mounted */
    struct stat st;
    if (stat(mount_point, &st) != 0) {
        printf("FatFS not mounted at %s\r\n", mount_point);
        printf("Run 'fatfs_mount' first\r\n");
        return -1;
    }

    run_fs_tests(mount_point, "FatFS");
    return (tests_failed == 0) ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_fatfs_test, fatfs_test, FatFS read/write test);

/* Combined filesystem test */
static int cmd_fs_test(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int lfs_available = 0;
    int fatfs_available = 0;
    struct stat st;

    printf("\r\n========================================\r\n");
    printf("  Filesystem Read/Write Test Suite\r\n");
    printf("========================================\r\n");

    /* Check available filesystems */
    if (stat("/lfs", &st) == 0) {
        lfs_available = 1;
        printf("LittleFS: mounted at /lfs\r\n");
    } else {
        printf("LittleFS: not mounted\r\n");
    }

    if (stat("/sdcard", &st) == 0) {
        fatfs_available = 1;
        printf("FatFS: mounted at /sdcard\r\n");
    } else {
        printf("FatFS: not mounted\r\n");
    }

    if (!lfs_available && !fatfs_available) {
        printf("\r\nNo filesystem mounted!\r\n");
        printf("Use 'lfs_mount' or SD card init to mount filesystems\r\n");
        return -1;
    }

    int total_passed = 0;
    int total_failed = 0;

    if (lfs_available) {
        run_fs_tests("/lfs", "LittleFS");
        total_passed += tests_passed;
        total_failed += tests_failed;
    }

    if (fatfs_available) {
        run_fs_tests("/sdcard", "FatFS");
        total_passed += tests_passed;
        total_failed += tests_failed;
    }

    printf("\r\n========================================\r\n");
    printf("  Total: %d passed, %d failed\r\n", total_passed, total_failed);
    printf("========================================\r\n");

    return (total_failed == 0) ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_fs_test, fs_test, Run all filesystem tests);

#else /* !CONFIG_EVFS || !CONFIG_EVFS_POSIX */

static int cmd_fs_test(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("EVFS POSIX not enabled. Enable CONFIG_EVFS and CONFIG_EVFS_POSIX\r\n");
    return -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_fs_test, fs_test, Run filesystem tests);

#endif /* CONFIG_EVFS && CONFIG_EVFS_POSIX */

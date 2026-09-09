/**
 * @file hostfs_demo.c
 * @brief Host Filesystem Demo via POSIX API
 *
 * Tests semihosting through EVFS POSIX layer (/host/ mount point):
 *   - host_read  <file>       : Read file from host
 *   - host_write <file> <msg> : Write message to host file
 *   - host_perf [size] [blk]  : Performance test
 *
 * This validates the full stack: POSIX -> EVFS -> semihost_fs -> semihosting
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 IPRO Technology Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <shell.h>
#include "hal_mtimer.h"
#include "evfs_posix.h"

#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_USE_SEMIHOST)

#define READ_BUF_SIZE   256
#define MAX_FILE_SIZE   4096

/* Performance test configuration */
#define PERF_DEFAULT_SIZE       (16 * 1024)
#define PERF_DEFAULT_BLOCK_SIZE 256
#define PERF_MAX_BLOCK_SIZE     1024
#define PERF_TEST_FILE          "/host/hostfs_perf.tmp"

/**
 * @brief Read file from host via POSIX API
 *
 * Usage: host_read <filepath>
 * Example: host_read /host/test.txt
 */
static int cmd_host_read(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: host_read <filepath>\n");
        printf("Example: host_read /host/test.txt\n");
        return -1;
    }

    const char *filepath = argv[1];
    printf("Reading file via POSIX: %s\n", filepath);

    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        printf("Error: Failed to open file\n");
        return -1;
    }

    char buf[READ_BUF_SIZE + 1];
    int total_read = 0;
    ssize_t bytes_read;

    printf("--- Content Start ---\n");
    while (total_read < MAX_FILE_SIZE) {
        bytes_read = read(fd, buf, READ_BUF_SIZE);
        if (bytes_read <= 0)
            break;

        buf[bytes_read] = '\0';
        printf("%s", buf);
        total_read += bytes_read;
    }
    printf("\n--- Content End ---\n");
    printf("Total bytes read: %d\n", total_read);

    close(fd);
    return 0;
}

/**
 * @brief Write message to file on host via POSIX API
 *
 * Usage: host_write <filepath> <message>
 * Example: host_write /host/output.txt "Hello from POSIX"
 */
static int cmd_host_write(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: host_write <filepath> <message>\n");
        printf("Example: host_write /host/output.txt \"Hello\"\n");
        return -1;
    }

    const char *filepath = argv[1];
    const char *message = argv[2];
    int msg_len = strlen(message);

    printf("Writing via POSIX: %s\n", filepath);
    printf("Message (%d bytes): %s\n", msg_len, message);

    int fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd < 0) {
        printf("Error: Failed to open file for writing\n");
        return -1;
    }

    ssize_t written = write(fd, message, msg_len);
    if (written < 0) {
        printf("Error: Write failed\n");
        close(fd);
        return -1;
    }

    write(fd, "\n", 1);
    printf("Successfully wrote %zd bytes\n", written);

    close(fd);
    return 0;
}

/**
 * @brief Test host filesystem connectivity via POSIX
 */
static int cmd_host_test(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    const char *tmpfile = "/host/hostfs_test.tmp";

    printf("=== Host FS (POSIX) Test ===\n");

    /* Test 1: Write */
    printf("Test 1: Write file... ");
    int fd = open(tmpfile, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd >= 0) {
        ssize_t written = write(fd, "Hello POSIX!", 12);
        close(fd);
        printf("%s (wrote %zd bytes)\n", written == 12 ? "OK" : "FAIL", written);
    } else {
        printf("FAIL (open)\n");
        return -1;
    }

    /* Test 2: Read */
    printf("Test 2: Read file... ");
    fd = open(tmpfile, O_RDONLY);
    if (fd >= 0) {
        char buf[64] = {0};
        ssize_t bytes_read = read(fd, buf, sizeof(buf) - 1);
        close(fd);
        if (bytes_read > 0) {
            printf("OK (read %zd bytes: \"%s\")\n", bytes_read, buf);
        } else {
            printf("FAIL\n");
        }
    } else {
        printf("FAIL (open)\n");
    }

    printf("=== Test Complete ===\n");
    return 0;
}

/**
 * @brief Performance test via POSIX API
 *
 * Usage: host_perf [total_size] [block_size]
 */
static int cmd_host_perf(int argc, char **argv)
{
    int total_size = PERF_DEFAULT_SIZE;
    int block_size = PERF_DEFAULT_BLOCK_SIZE;

    if (argc >= 2)
        total_size = atoi(argv[1]);
    if (argc >= 3)
        block_size = atoi(argv[2]);

    if (block_size > PERF_MAX_BLOCK_SIZE)
        block_size = PERF_MAX_BLOCK_SIZE;
    if (block_size <= 0)
        block_size = PERF_DEFAULT_BLOCK_SIZE;
    if (total_size <= 0)
        total_size = PERF_DEFAULT_SIZE;

    printf("=== Host FS (POSIX) Performance Test ===\n");
    printf("Total: %d bytes, Block: %d bytes\n\n", total_size, block_size);

    /* Show mount points for debug */
    evfs_posix_list_mounts();
    printf("\n");

    char buf[PERF_MAX_BLOCK_SIZE];
    for (int i = 0; i < block_size; i++)
        buf[i] = (char)('A' + (i % 26));

    /* === Write Test === */
    printf("[Write Test]\n");
    printf("Opening: %s\n", PERF_TEST_FILE);
    int fd = open(PERF_TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd < 0) {
        printf("Error: Failed to open file for write (errno=%d)\n", errno);
        return -1;
    }

    uint64_t write_start = mtimer_get_time_us();
    int written_total = 0;

    while (written_total < total_size) {
        int to_write = (total_size - written_total) < block_size ?
                       (total_size - written_total) : block_size;
        ssize_t written = write(fd, buf, to_write);
        if (written < 0) {
            printf("Error: Write failed\n");
            close(fd);
            return -1;
        }
        written_total += written;
    }

    uint64_t write_end = mtimer_get_time_us();
    close(fd);

    uint64_t write_us = write_end - write_start;
    uint32_t write_throughput = 0;
    if (write_us > 0)
        write_throughput = (uint32_t)((uint64_t)written_total * 1000000 / write_us);

    printf("  Written: %d bytes\n", written_total);
    printf("  Time: %llu us, Throughput: %.2f KB/s\n\n", write_us, write_throughput / 1024.0f);

    /* === Read Test === */
    printf("[Read Test]\n");
    fd = open(PERF_TEST_FILE, O_RDONLY);
    if (fd < 0) {
        printf("Error: Failed to open file for read\n");
        return -1;
    }

    uint64_t read_start = mtimer_get_time_us();
    int read_total = 0;

    while (1) {
        ssize_t bytes_read = read(fd, buf, block_size);
        if (bytes_read <= 0)
            break;
        read_total += bytes_read;
    }

    uint64_t read_end = mtimer_get_time_us();
    close(fd);

    uint64_t read_us = read_end - read_start;
    uint32_t read_throughput = 0;
    if (read_us > 0)
        read_throughput = (uint32_t)((uint64_t)read_total * 1000000 / read_us);

    printf("  Read: %d bytes\n", read_total);
    printf("  Time: %llu us, Throughput: %.2f KB/s\n\n", read_us, read_throughput / 1024.0f);

    /* === Summary === */
    printf("[Summary]\n");
    printf("  Block size: %d bytes\n", block_size);
    printf("  Write: %.2f KB/s\n", write_throughput / 1024.0f);
    printf("  Read:  %.2f KB/s\n", read_throughput / 1024.0f);
    printf("=== Test Complete ===\n");
    return 0;
}

/* Register shell commands */
SHELL_CMD_EXPORT_ALIAS(cmd_host_read, host_read, Read file from /host/ via POSIX);
SHELL_CMD_EXPORT_ALIAS(cmd_host_write, host_write, Write message to /host/ via POSIX);
SHELL_CMD_EXPORT_ALIAS(cmd_host_test, host_test, Test /host/ filesystem via POSIX);
SHELL_CMD_EXPORT_ALIAS(cmd_host_perf, host_perf, Performance test via POSIX);

#endif /* CONFIG_EVFS && CONFIG_EVFS_USE_SEMIHOST */

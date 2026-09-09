/**
 * @file semihost_demo.c
 * @brief Semihosting File I/O Demo
 *
 * Demonstrates semihosting file operations with host PC:
 *   - sh_read  <file>       : Read file from host and display
 *   - sh_write <file> <msg> : Write message to file on host
 *
 * Usage:
 *   1. Start OpenOCD with semihosting enabled
 *   2. Connect GDB and run target
 *   3. Use shell commands to read/write host files
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 IPRO Technology Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <shell.h>

#include "semihost_fileio.h"
#include "hal_mtimer.h"

#define READ_BUF_SIZE   256
#define MAX_FILE_SIZE   4096

/* Performance test configuration */
#define PERF_DEFAULT_SIZE       (16 * 1024)  /* 16KB default */
#define PERF_DEFAULT_BLOCK_SIZE 256          /* 256 bytes per call */
#define PERF_MAX_BLOCK_SIZE     1024         /* Max block size */
#define PERF_TEST_FILE          "semihost_perf.tmp"

/**
 * @brief Read file from host and display content
 *
 * Usage: sh_read <filepath>
 * Example: sh_read /tmp/test.txt
 */
static int cmd_sh_read(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: sh_read <filepath>\n");
        printf("Example: sh_read /tmp/test.txt\n");
        return -1;
    }

    const char *filepath = argv[1];
    printf("Reading file from host: %s\n", filepath);

    /* Open file for reading (binary mode) */
    semihost_fd_t fd = semihost_open(filepath, SEMIHOST_MODE_RB);
    if (fd < 0) {
        printf("Error: Failed to open file (err=%d)\n", fd);
        printf("Make sure OpenOCD semihosting is enabled and file exists.\n");
        return -1;
    }

    /* Read and display content (read until EOF) */
    char buf[READ_BUF_SIZE + 1];
    int total_read = 0;
    int bytes_read;

    printf("--- Content Start ---\n");
    while (total_read < MAX_FILE_SIZE) {
        bytes_read = semihost_read(fd, buf, READ_BUF_SIZE);
        if (bytes_read <= 0) {
            break;  /* EOF or error */
        }

        buf[bytes_read] = '\0';
        printf("%s", buf);
        total_read += bytes_read;
    }
    printf("\n--- Content End ---\n");

    printf("Total bytes read: %d\n", total_read);

    semihost_close(fd);
    return 0;
}

/**
 * @brief Write message to file on host
 *
 * Usage: sh_write <filepath> <message>
 * Example: sh_write /tmp/output.txt "Hello from FPGA"
 */
static int cmd_sh_write(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: sh_write <filepath> <message>\n");
        printf("Example: sh_write /tmp/output.txt \"Hello from FPGA\"\n");
        return -1;
    }

    const char *filepath = argv[1];
    const char *message = argv[2];
    int msg_len = strlen(message);

    printf("Writing to host file: %s\n", filepath);
    printf("Message (%d bytes): %s\n", msg_len, message);

    /* Open file for writing (create/truncate, binary mode) */
    semihost_fd_t fd = semihost_open(filepath, SEMIHOST_MODE_WB);
    if (fd < 0) {
        printf("Error: Failed to open file for writing (err=%d)\n", fd);
        printf("Make sure OpenOCD semihosting is enabled.\n");
        return -1;
    }

    /* Write message */
    int written = semihost_write(fd, message, msg_len);
    if (written < 0) {
        printf("Error: Write failed (err=%d)\n", written);
        semihost_close(fd);
        return -1;
    }

    /* Write newline */
    semihost_write(fd, "\n", 1);

    printf("Successfully wrote %d bytes\n", written);

    semihost_close(fd);
    return 0;
}

/**
 * @brief Append message to file on host
 *
 * Usage: sh_append <filepath> <message>
 */
static int cmd_sh_append(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: sh_append <filepath> <message>\n");
        return -1;
    }

    const char *filepath = argv[1];
    const char *message = argv[2];
    int msg_len = strlen(message);

    printf("Appending to host file: %s\n", filepath);

    /* Open file for appending (binary mode) */
    semihost_fd_t fd = semihost_open(filepath, SEMIHOST_MODE_AB);
    if (fd < 0) {
        printf("Error: Failed to open file for appending (err=%d)\n", fd);
        return -1;
    }

    /* Write message with newline */
    int written = semihost_write(fd, message, msg_len);
    semihost_write(fd, "\n", 1);

    if (written < 0) {
        printf("Error: Write failed\n");
        semihost_close(fd);
        return -1;
    }

    printf("Appended %d bytes\n", written);

    semihost_close(fd);
    return 0;
}

/* Note: SYS_GETCWD (0x17) is not supported by OpenOCD semihosting */

/**
 * @brief Test semihosting connectivity
 */
static int cmd_sh_test(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("=== Semihosting File I/O Test ===\n");

    /* Test 1: Write file */
    printf("Test 1: Write file... ");
    const char *tmpfile = "semihost_test.tmp";
    semihost_fd_t fd = semihost_open(tmpfile, SEMIHOST_MODE_WB);
    if (fd >= 0) {
        int written = semihost_write(fd, "Hello Semihosting!", 18);
        semihost_close(fd);
        if (written == 18) {
            printf("OK (wrote %d bytes)\n", written);
        } else {
            printf("FAIL (wrote %d bytes)\n", written);
        }
    } else {
        printf("FAIL (open err=%d)\n", fd);
        printf("=== Test Aborted ===\n");
        return -1;
    }

    /* Test 2: Read file back */
    printf("Test 2: Read file... ");
    fd = semihost_open(tmpfile, SEMIHOST_MODE_RB);
    if (fd >= 0) {
        char buf[64] = {0};
        int read_bytes = semihost_read(fd, buf, sizeof(buf) - 1);
        semihost_close(fd);
        if (read_bytes > 0) {
            printf("OK (read %d bytes: \"%s\")\n", read_bytes, buf);
        } else {
            printf("FAIL (read err=%d)\n", read_bytes);
        }
    } else {
        printf("FAIL (open err=%d)\n", fd);
    }

    /* Test 3: Append to file */
    printf("Test 3: Append file... ");
    fd = semihost_open(tmpfile, SEMIHOST_MODE_AB);
    if (fd >= 0) {
        int written = semihost_write(fd, " Appended!", 10);
        semihost_close(fd);
        printf("%s\n", written > 0 ? "OK" : "FAIL");
    } else {
        printf("FAIL (open err=%d)\n", fd);
    }

    /* Test 4: Read again to verify append */
    printf("Test 4: Verify append... ");
    fd = semihost_open(tmpfile, SEMIHOST_MODE_RB);
    if (fd >= 0) {
        char buf[64] = {0};
        int read_bytes = semihost_read(fd, buf, sizeof(buf) - 1);
        semihost_close(fd);
        if (read_bytes > 0) {
            printf("OK (\"%s\")\n", buf);
        } else {
            printf("FAIL\n");
        }
    } else {
        printf("FAIL\n");
    }

    printf("=== Test Complete ===\n");
    printf("Note: Check '%s' in semihosting_basedir on host\n", tmpfile);
    return 0;
}

/**
 * @brief Performance test for semihosting write
 *
 * Usage: sh_perf_write [total_size] [block_size]
 * Example: sh_perf_write 16384 256
 */
static int cmd_sh_perf_write(int argc, char **argv)
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

    printf("=== Semihosting Write Performance Test ===\n");
    printf("Total size: %d bytes, Block size: %d bytes\n", total_size, block_size);

    /* Prepare test data */
    char buf[PERF_MAX_BLOCK_SIZE];
    for (int i = 0; i < block_size; i++)
        buf[i] = (char)('A' + (i % 26));

    /* Open file */
    semihost_fd_t fd = semihost_open(PERF_TEST_FILE, SEMIHOST_MODE_WB);
    if (fd < 0) {
        printf("Error: Failed to open file (err=%d)\n", fd);
        return -1;
    }

    /* Write with timing */
    uint64_t start_us = mtimer_get_time_us();
    int written_total = 0;
    int iterations = 0;

    while (written_total < total_size) {
        int to_write = (total_size - written_total) < block_size ?
                       (total_size - written_total) : block_size;
        int written = semihost_write(fd, buf, to_write);
        if (written < 0) {
            printf("Error: Write failed at %d bytes (err=%d)\n", written_total, written);
            semihost_close(fd);
            return -1;
        }
        written_total += written;
        iterations++;
    }

    uint64_t end_us = mtimer_get_time_us();
    semihost_close(fd);

    /* Calculate throughput */
    uint64_t elapsed_us = end_us - start_us;
    uint32_t throughput = 0;
    if (elapsed_us > 0)
        throughput = (uint32_t)((uint64_t)written_total * 1000000 / elapsed_us);

    printf("Result:\n");
    printf("  Written: %d bytes in %d calls\n", written_total, iterations);
    printf("  Time: %llu us (%.3f s)\n", elapsed_us, elapsed_us / 1000000.0f);
    printf("  Throughput: %lu bytes/s (%.2f KB/s)\n", throughput, throughput / 1024.0f);
    printf("=== Test Complete ===\n");
    return 0;
}

/**
 * @brief Performance test for semihosting read
 *
 * Usage: sh_perf_read [block_size]
 * Note: Uses file created by sh_perf_write
 */
static int cmd_sh_perf_read(int argc, char **argv)
{
    int block_size = PERF_DEFAULT_BLOCK_SIZE;

    if (argc >= 2)
        block_size = atoi(argv[1]);

    if (block_size > PERF_MAX_BLOCK_SIZE)
        block_size = PERF_MAX_BLOCK_SIZE;
    if (block_size <= 0)
        block_size = PERF_DEFAULT_BLOCK_SIZE;

    printf("=== Semihosting Read Performance Test ===\n");
    printf("Block size: %d bytes\n", block_size);
    printf("Note: Run sh_perf_write first to create test file\n");

    /* Open file */
    semihost_fd_t fd = semihost_open(PERF_TEST_FILE, SEMIHOST_MODE_RB);
    if (fd < 0) {
        printf("Error: Failed to open file (err=%d)\n", fd);
        printf("Run 'sh_perf_write' first to create the test file.\n");
        return -1;
    }

    /* Read with timing */
    char buf[PERF_MAX_BLOCK_SIZE];
    uint64_t start_us = mtimer_get_time_us();
    int read_total = 0;
    int iterations = 0;

    while (1) {
        int bytes_read = semihost_read(fd, buf, block_size);
        if (bytes_read <= 0)
            break;  /* EOF or error */
        read_total += bytes_read;
        iterations++;
    }

    uint64_t end_us = mtimer_get_time_us();
    semihost_close(fd);

    /* Calculate throughput */
    uint64_t elapsed_us = end_us - start_us;
    uint32_t throughput = 0;
    if (elapsed_us > 0)
        throughput = (uint32_t)((uint64_t)read_total * 1000000 / elapsed_us);

    printf("Result:\n");
    printf("  Read: %d bytes in %d calls\n", read_total, iterations);
    printf("  Time: %llu us (%.3f s)\n", elapsed_us, elapsed_us / 1000000.0f);
    printf("  Throughput: %lu bytes/s (%.2f KB/s)\n", throughput, throughput / 1024.0f);
    printf("=== Test Complete ===\n");
    return 0;
}

/**
 * @brief Combined performance test (write + read)
 *
 * Usage: sh_perf [total_size] [block_size]
 */
static int cmd_sh_perf(int argc, char **argv)
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

    printf("=== Semihosting Performance Test ===\n");
    printf("Total size: %d bytes, Block size: %d bytes\n\n", total_size, block_size);

    /* Prepare test data */
    char buf[PERF_MAX_BLOCK_SIZE];
    for (int i = 0; i < block_size; i++)
        buf[i] = (char)('A' + (i % 26));

    /* === Write Test === */
    printf("[Write Test]\n");
    semihost_fd_t fd = semihost_open(PERF_TEST_FILE, SEMIHOST_MODE_WB);
    if (fd < 0) {
        printf("Error: Failed to open file for write (err=%d)\n", fd);
        return -1;
    }

    uint64_t write_start = mtimer_get_time_us();
    int written_total = 0;

    while (written_total < total_size) {
        int to_write = (total_size - written_total) < block_size ?
                       (total_size - written_total) : block_size;
        int written = semihost_write(fd, buf, to_write);
        if (written < 0) {
            printf("Error: Write failed (err=%d)\n", written);
            semihost_close(fd);
            return -1;
        }
        written_total += written;
    }

    uint64_t write_end = mtimer_get_time_us();
    semihost_close(fd);

    uint64_t write_us = write_end - write_start;
    uint32_t write_throughput = 0;
    if (write_us > 0)
        write_throughput = (uint32_t)((uint64_t)written_total * 1000000 / write_us);

    printf("  Written: %d bytes\n", written_total);
    printf("  Time: %llu us, Throughput: %.2f KB/s\n\n", write_us, write_throughput / 1024.0f);

    /* === Read Test === */
    printf("[Read Test]\n");
    fd = semihost_open(PERF_TEST_FILE, SEMIHOST_MODE_RB);
    if (fd < 0) {
        printf("Error: Failed to open file for read (err=%d)\n", fd);
        return -1;
    }

    uint64_t read_start = mtimer_get_time_us();
    int read_total = 0;

    while (1) {
        int bytes_read = semihost_read(fd, buf, block_size);
        if (bytes_read <= 0)
            break;
        read_total += bytes_read;
    }

    uint64_t read_end = mtimer_get_time_us();
    semihost_close(fd);

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
SHELL_CMD_EXPORT_ALIAS(cmd_sh_read, sh_read, Read file from host via semihosting);
SHELL_CMD_EXPORT_ALIAS(cmd_sh_write, sh_write, Write message to host file);
SHELL_CMD_EXPORT_ALIAS(cmd_sh_append, sh_append, Append message to host file);
SHELL_CMD_EXPORT_ALIAS(cmd_sh_test, sh_test, Test semihosting file I/O);
SHELL_CMD_EXPORT_ALIAS(cmd_sh_perf_write, sh_perf_write, Performance test: write);
SHELL_CMD_EXPORT_ALIAS(cmd_sh_perf_read, sh_perf_read, Performance test: read);
SHELL_CMD_EXPORT_ALIAS(cmd_sh_perf, sh_perf, Performance test: write + read);

/*
 * Copyright (c) 2026 IPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Semihosting common definitions and low-level call interface.
 * Shared by printf and file I/O semihosting modules.
 */

#ifndef SEMIHOST_COMMON_H
#define SEMIHOST_COMMON_H

#include <stdint.h>

/*
 * ARM Semihosting syscall numbers (RISC-V uses the same definitions)
 * Reference: ARM DUI 0471C - ARM Compiler toolchain Developer Guide
 */
#define SYS_OPEN      0x01
#define SYS_CLOSE     0x02
#define SYS_WRITEC    0x03
#define SYS_WRITE0    0x04
#define SYS_WRITE     0x05
#define SYS_READ      0x06
#define SYS_READC     0x07
#define SYS_ISERROR   0x08
#define SYS_ISATTY    0x09
#define SYS_SEEK      0x0A
#define SYS_FLEN      0x0C
#define SYS_TMPNAM    0x0D
#define SYS_REMOVE    0x0E
#define SYS_RENAME    0x0F
#define SYS_CLOCK     0x10
#define SYS_TIME      0x11
#define SYS_SYSTEM    0x12
#define SYS_ERRNO     0x13
#define SYS_GET_CMDLINE 0x15
#define SYS_HEAPINFO  0x16
#define SYS_GETCWD    0x17

/*
 * RISC-V Semihosting call
 *
 * The semihosting call sequence must be exactly:
 *   01f01013  slli zero,zero,0x1f   # Entry NOP
 *   00100073  ebreak                # Break to debugger
 *   40705013  srai zero,zero,0x7    # Exit NOP
 *
 * IMPORTANT: Must use .option norvc to prevent compressed instructions!
 * OpenOCD checks for exact 32-bit instruction encodings.
 *
 * Registers:
 *   a0 = operation number (syscall)
 *   a1 = parameter block pointer
 *
 * Returns:
 *   a0 = result from host
 */
static inline long semihost_call(long op, long arg)
{
    register long a0 asm("a0") = op;
    register long a1 asm("a1") = arg;

    asm volatile (
        ".option push\n"
        ".option norvc\n"
        ".balign 4\n"
        "slli zero, zero, 0x1f\n"
        "ebreak\n"
        "srai zero, zero, 0x7\n"
        ".option pop\n"
        : "+r"(a0)
        : "r"(a1)
        : "memory"
    );

    return a0;
}

#endif /* SEMIHOST_COMMON_H */

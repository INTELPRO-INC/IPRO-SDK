/*
 * Copyright (c) 2026 IPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Semihosting printf support - redirect console output to debugger.
 */

#ifndef SEMIHOST_PRINTF_H
#define SEMIHOST_PRINTF_H

/*
 * Write null-terminated string via semihosting.
 * Fast path: entire string in one semihosting call.
 */
void semihost_puts(const char *s);

/*
 * Write single character via semihosting.
 */
void semihost_putc(char c);

#endif /* SEMIHOST_PRINTF_H */

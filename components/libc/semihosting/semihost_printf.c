/*
 * Copyright (c) 2026 IPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Semihosting printf support implementation.
 */

#include "semihost_common.h"
#include "semihost_printf.h"

void semihost_puts(const char *s)
{
    semihost_call(SYS_WRITE0, (long)s);
}

void semihost_putc(char c)
{
    char buf[2] = {c, '\0'};
    semihost_call(SYS_WRITE0, (long)buf);
}

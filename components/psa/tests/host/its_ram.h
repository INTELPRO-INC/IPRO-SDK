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
 * RAM its_backend of the ITS host tests: a static table of named
 * files with hooks to inspect, tamper with and fail them.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "its_store.h"

#define ITS_RAM_FILES 16u

extern const struct its_backend its_ram;

/* Drops every file and pending failure */
void its_ram_reset(void);

size_t its_ram_count(void);

/* Content of name and its length; NULL when absent */
uint8_t *its_ram_data(const char *name, size_t *len);

/* Sets the stored length of name (truncation); -1 when absent */
int its_ram_set_len(const char *name, size_t len);

/* Renames from to to, replacing to; -1 when from is absent */
int its_ram_rename(const char *from, const char *to);

/* The next write returns rc and leaves the files unchanged */
void its_ram_fail_write(int rc);

/* The next read returns rc */
void its_ram_fail_read(int rc);

/* The next format returns rc and leaves the files unchanged */
void its_ram_fail_format(int rc);

/* The next count returns rc */
void its_ram_fail_count(int rc);

/* Writes and formats after its_ram_reset */
int its_ram_writes(void);
int its_ram_formats(void);

/* why of the last format; "" when none since its_ram_reset */
const char *its_ram_format_why(void);

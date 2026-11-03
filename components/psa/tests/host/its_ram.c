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
 * RAM its_backend of the ITS host tests.
 */
#include <string.h>

#include "its_ram.h"

struct ram_file {
    char name[ITS_NAME_LEN];
    size_t len;
    int used;
    uint8_t data[ITS_BLOB_MAX];
};

static struct ram_file g_files[ITS_RAM_FILES];
static int g_fail_write;
static int g_fail_read;
static int g_fail_format;
static int g_fail_count;
static int g_writes;
static int g_formats;
static const char *g_format_why;

static struct ram_file *ram_find(const char *name)
{
    for (size_t i = 0; i < ITS_RAM_FILES; i++) {
        if (g_files[i].used && strcmp(g_files[i].name, name) == 0) {
            return &g_files[i];
        }
    }
    return NULL;
}

static struct ram_file *ram_free_slot(void)
{
    for (size_t i = 0; i < ITS_RAM_FILES; i++) {
        if (!g_files[i].used) {
            return &g_files[i];
        }
    }
    return NULL;
}

static int ram_read(const char *name, void *buf, size_t *len)
{
    const struct ram_file *f = ram_find(name);
    int rc = g_fail_read;

    g_fail_read = 0;
    if (rc != 0) {
        return rc;
    }
    if (f == NULL) {
        return ITS_BE_ENOENT;
    }
    if (f->len > *len) {
        return ITS_BE_EIO;
    }
    memcpy(buf, f->data, f->len);
    *len = f->len;
    return ITS_BE_OK;
}

static int ram_write(const char *name, const void *buf, size_t len)
{
    struct ram_file *f = ram_find(name);
    int rc = g_fail_write;

    g_fail_write = 0;
    g_writes++;
    if (rc != 0) {
        return rc;
    }
    if (strlen(name) >= ITS_NAME_LEN || len > ITS_BLOB_MAX) {
        return ITS_BE_ENOSPC;
    }
    if (f == NULL) {
        f = ram_free_slot();
    }
    if (f == NULL) {
        return ITS_BE_ENOSPC;
    }
    strcpy(f->name, name);
    memcpy(f->data, buf, len);
    f->len = len;
    f->used = 1;
    return ITS_BE_OK;
}

static int ram_remove(const char *name)
{
    struct ram_file *f = ram_find(name);

    if (f == NULL) {
        return ITS_BE_ENOENT;
    }
    memset(f, 0, sizeof(*f));
    return ITS_BE_OK;
}

static int ram_format(const char *why)
{
    int rc = g_fail_format;

    g_fail_format = 0;
    g_formats++;
    g_format_why = why;
    if (rc == 0) {
        memset(g_files, 0, sizeof(g_files));
    }
    return rc;
}

static int ram_count(size_t *n)
{
    int rc = g_fail_count;

    g_fail_count = 0;
    *n = 0;
    if (rc != 0) {
        return rc;
    }
    for (size_t i = 0; i < ITS_RAM_FILES; i++) {
        if (g_files[i].used && strncmp(g_files[i].name, "/its/", 5) == 0 &&
            strcmp(g_files[i].name, ITS_META_NAME) != 0) {
            (*n)++;
        }
    }
    return ITS_BE_OK;
}

const struct its_backend its_ram = {
    ram_read, ram_write, ram_remove, ram_format, ram_count,
};

void its_ram_reset(void)
{
    memset(g_files, 0, sizeof(g_files));
    g_fail_write = 0;
    g_fail_read = 0;
    g_fail_format = 0;
    g_fail_count = 0;
    g_writes = 0;
    g_formats = 0;
    g_format_why = NULL;
}

size_t its_ram_count(void)
{
    size_t n = 0;

    for (size_t i = 0; i < ITS_RAM_FILES; i++) {
        n += g_files[i].used ? 1u : 0u;
    }
    return n;
}

uint8_t *its_ram_data(const char *name, size_t *len)
{
    struct ram_file *f = ram_find(name);

    if (f == NULL) {
        return NULL;
    }
    *len = f->len;
    return f->data;
}

int its_ram_set_len(const char *name, size_t len)
{
    struct ram_file *f = ram_find(name);

    if (f == NULL || len > ITS_BLOB_MAX) {
        return -1;
    }
    f->len = len;
    return 0;
}

int its_ram_rename(const char *from, const char *to)
{
    struct ram_file *f = ram_find(from);
    struct ram_file *t = ram_find(to);

    if (f == NULL) {
        return -1;
    }
    if (t != NULL && t != f) {
        memset(t, 0, sizeof(*t));
    }
    strcpy(f->name, to);
    return 0;
}

void its_ram_fail_write(int rc)
{
    g_fail_write = rc;
}

void its_ram_fail_read(int rc)
{
    g_fail_read = rc;
}

void its_ram_fail_format(int rc)
{
    g_fail_format = rc;
}

void its_ram_fail_count(int rc)
{
    g_fail_count = rc;
}

int its_ram_writes(void)
{
    return g_writes;
}

int its_ram_formats(void)
{
    return g_formats;
}

const char *its_ram_format_why(void)
{
    return g_format_why != NULL ? g_format_why : "";
}

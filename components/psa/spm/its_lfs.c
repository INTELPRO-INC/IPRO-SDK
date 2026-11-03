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
 * ITS backend: littlefs on the PSA_ITS_PARTITION flash partition with
 * static buffers and state, filled in at mount.
 */
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "its_lfs.h"
#include "lfs.h"
#include "lfs_port.h"
#include "psa_ipro/memmap.h"
#include "spm_harden.h"
#include "spm_log.h"

#define ITS_LFS_IO        256u
#define ITS_LFS_LOOKAHEAD 32u
/*
 * Blocks an object write must leave free: the meta rewrite at every
 * boot and a directory split or compaction
 */
#define ITS_LFS_RESERVE   4u
/* ITS_META_NAME as a name in the /its directory */
#define ITS_LFS_META      (&ITS_META_NAME[sizeof("/its/") - 1])

static uint8_t its_lfs_read_buf[ITS_LFS_IO] __attribute__((aligned(4)));
static uint8_t its_lfs_prog_buf[ITS_LFS_IO] __attribute__((aligned(4)));
static uint8_t its_lfs_file_buf[ITS_LFS_IO] __attribute__((aligned(4)));
static uint32_t its_lfs_lookahead[ITS_LFS_LOOKAHEAD / 4u];
static struct lfs_context its_lfs_ctx;
static struct lfs_config its_lfs_cfg;
static const struct lfs_file_config its_lfs_fcfg = {
    .buffer = its_lfs_file_buf,
};
static lfs_t *its_lfs;

/* littlefs errors with their its_backend code; others are EIO */
static const struct {
    int lfs;
    int be;
} its_lfs_errs[] = {
    { LFS_ERR_NOENT, ITS_BE_ENOENT },
    { LFS_ERR_NOSPC, ITS_BE_ENOSPC },
    { LFS_ERR_CORRUPT, ITS_BE_CORRUPT },
    { LFS_ERR_NOTDIR, ITS_BE_CORRUPT },
    { LFS_ERR_ISDIR, ITS_BE_CORRUPT },
    { LFS_ERR_FBIG, ITS_BE_CORRUPT },
};

static int its_lfs_rc(int rc)
{
    if (rc >= 0) {
        return ITS_BE_OK;
    }
    for (size_t i = 0; i < sizeof(its_lfs_errs) / sizeof(its_lfs_errs[0]);
         i++) {
        if (its_lfs_errs[i].lfs == rc) {
            return its_lfs_errs[i].be;
        }
    }
    return ITS_BE_EIO;
}

static int its_lfs_read(const char *name, void *buf, size_t *len)
{
    lfs_file_t f;
    lfs_soff_t size;
    int rc = lfs_file_opencfg(its_lfs, &f, name, LFS_O_RDONLY,
                              &its_lfs_fcfg);

    if (rc < 0) {
        return its_lfs_rc(rc);
    }
    size = lfs_file_size(its_lfs, &f);
    if (size < 0) {
        rc = (int)size;
    } else if ((size_t)size > *len) {
        rc = LFS_ERR_FBIG;
    } else if (lfs_file_read(its_lfs, &f, buf, (lfs_size_t)size) != size) {
        rc = LFS_ERR_IO;
    }
    if (lfs_file_close(its_lfs, &f) < 0) {
        rc = LFS_ERR_IO;
    }
    if (rc >= 0) {
        *len = (size_t)size;
    }
    return its_lfs_rc(rc);
}

/*
 * ENOSPC when an object write of len bytes would leave fewer than
 * ITS_LFS_RESERVE blocks free; the old copy counts as used. Up to
 * ITS_LFS_IO bytes (block_size / 8 or more) a file is inline.
 */
static int its_lfs_room(const char *name, size_t len)
{
    lfs_size_t bs = its_lfs->cfg->block_size;
    lfs_ssize_t used;

    if (strcmp(name, ITS_META_NAME) == 0 || len <= ITS_LFS_IO) {
        return LFS_ERR_OK;
    }
    used = lfs_fs_size(its_lfs);
    if (used < 0) {
        return (int)used;
    }
    return (lfs_size_t)used + (len + bs - 1u) / bs + ITS_LFS_RESERVE >
                   its_lfs->cfg->block_count
               ? LFS_ERR_NOSPC
               : LFS_ERR_OK;
}

/* littlefs commits at close; a failed write leaves the old content */
static int its_lfs_write(const char *name, const void *buf, size_t len)
{
    lfs_file_t f;
    lfs_ssize_t w;
    int rc = its_lfs_room(name, len);

    if (rc == LFS_ERR_OK) {
        rc = lfs_file_opencfg(its_lfs, &f, name,
                              LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC,
                              &its_lfs_fcfg);
    }
    if (rc < 0) {
        return its_lfs_rc(rc);
    }
    w = lfs_file_write(its_lfs, &f, buf, (lfs_size_t)len);
    rc = lfs_file_close(its_lfs, &f);
    if (w < 0) {
        rc = (int)w;
    } else if ((size_t)w != len && rc >= 0) {
        rc = LFS_ERR_IO;
    }
    return its_lfs_rc(rc);
}

static int its_lfs_remove(const char *name)
{
    return its_lfs_rc(lfs_remove(its_lfs, name));
}

/* Regular files in /its other than the meta file */
static int its_lfs_count(size_t *n)
{
    struct lfs_info info;
    lfs_dir_t dir;
    int rc = lfs_dir_open(its_lfs, &dir, "/its");

    *n = 0;
    if (rc < 0) {
        return its_lfs_rc(rc);
    }
    while ((rc = lfs_dir_read(its_lfs, &dir, &info)) > 0) {
        if (info.type == LFS_TYPE_REG &&
            strcmp(info.name, ITS_LFS_META) != 0) {
            (*n)++;
        }
    }
    if (lfs_dir_close(its_lfs, &dir) < 0) {
        rc = LFS_ERR_IO;
    }
    return its_lfs_rc(rc);
}

/* Formats and mounts lfs, creates /its; lfs is unmounted on entry */
static int its_lfs_remake(lfs_t *lfs)
{
    int rc = lfs_format(lfs, &its_lfs_cfg);

    if (rc == 0) {
        rc = lfs_mount(lfs, &its_lfs_cfg);
    }
    if (rc == 0) {
        rc = lfs_mkdir(lfs, "/its");
    }
    return rc;
}

/* Logs "[SPE] ITS <why>, reformatting", then formats */
static int its_lfs_format(const char *why)
{
    int rc;

    SPM_LOG("ITS %s, reformatting", why);
    rc = lfs_unmount(its_lfs);
    if (rc == 0) {
        rc = its_lfs_remake(its_lfs);
    }
    return its_lfs_rc(rc);
}

/* its_lfs_format with why "store unreadable (lfs <rc>)" */
static int its_lfs_format_rc(int rc)
{
    char why[40];

    snprintf(why, sizeof(why), "store unreadable (lfs %d)", rc);
    return its_lfs_format(why);
}

/* block_count comes from the partition size (lfs_xip_init) */
static void its_lfs_setup(void)
{
    its_lfs_ctx.partition_name = PSA_ITS_PARTITION;
    its_lfs_cfg.read_size = ITS_LFS_IO;
    its_lfs_cfg.prog_size = ITS_LFS_IO;
    its_lfs_cfg.block_size = 4096;
    its_lfs_cfg.block_cycles = 500;
    its_lfs_cfg.cache_size = ITS_LFS_IO;
    its_lfs_cfg.lookahead_size = ITS_LFS_LOOKAHEAD;
    its_lfs_cfg.read_buffer = its_lfs_read_buf;
    its_lfs_cfg.prog_buffer = its_lfs_prog_buf;
    its_lfs_cfg.lookahead_buffer = its_lfs_lookahead;
}

const struct its_backend *its_lfs_mount(void)
{
    static const struct its_backend be = {
        its_lfs_read, its_lfs_write, its_lfs_remove, its_lfs_format,
        its_lfs_count,
    };
    int rc;

    if (its_lfs == NULL) {
        if (spm_psm_layout_ok() != 0) {
            return NULL;
        }
        its_lfs_setup();
        its_lfs = lfs_xip_init(&its_lfs_ctx, &its_lfs_cfg);
        rc = its_lfs == NULL ? errno : 0;
        /* INVAL: a littlefs of other geometry or version */
        if (rc == LFS_ERR_INVAL) {
            SPM_LOG("ITS store unreadable (lfs %d), reformatting", rc);
            rc = its_lfs_remake(&its_lfs_ctx.lfs);
            its_lfs = rc == 0 ? &its_lfs_ctx.lfs : NULL;
        }
    }
    if (its_lfs == NULL) {
        return NULL;
    }
    rc = lfs_mkdir(its_lfs, "/its");
    if (its_lfs_rc(rc) == ITS_BE_CORRUPT) {
        rc = its_lfs_format_rc(rc) == ITS_BE_OK ? 0 : rc;
    }
    return rc == 0 || rc == LFS_ERR_EXIST ? &be : NULL;
}

/* SPDX-License-Identifier: MIT
 * Copyright 2026 IPRO Technology Co., Ltd.
 *
 * EVFS Semihosting filesystem backend.
 * Wraps semihosting file I/O to provide EVFS-compatible interface.
 */

#include <string.h>
#include "evfs.h"
#include "evfs_internal.h"
#include "evfs/semihost_fs.h"
#include "semihost_fileio.h"

/* File object for semihosting */
typedef struct SemihostFile_s {
    EvfsFile base;
    semihost_fd_t fd;
} SemihostFile;

/* ==================== File access methods ==================== */

static int semihost__file_ctrl(EvfsFile *fh, int cmd, void *arg)
{
    (void)fh;
    (void)cmd;
    (void)arg;
    return EVFS_OK;
}

static int semihost__file_close(EvfsFile *fh)
{
    SemihostFile *fil = (SemihostFile *)fh;

    if (fil->fd < 0) {
        return EVFS_OK;
    }

    int ret = semihost_close(fil->fd);
    fil->fd = SEMIHOST_INVALID_FD;

    return (ret == 0) ? EVFS_OK : EVFS_ERR_IO;
}

static ptrdiff_t semihost__file_read(EvfsFile *fh, void *buf, size_t size)
{
    SemihostFile *fil = (SemihostFile *)fh;

    int ret = semihost_read(fil->fd, buf, (int)size);
    if (ret < 0) {
        return EVFS_ERR_IO;
    }

    return (ptrdiff_t)ret;
}

static ptrdiff_t semihost__file_write(EvfsFile *fh, const void *buf, size_t size)
{
    SemihostFile *fil = (SemihostFile *)fh;

    int ret = semihost_write(fil->fd, buf, (int)size);
    if (ret < 0) {
        return EVFS_ERR_IO;
    }

    return (ptrdiff_t)ret;
}

static int semihost__file_truncate(EvfsFile *fh, evfs_off_t size)
{
    (void)fh;
    (void)size;
    /* Semihosting does not support truncate */
    return EVFS_ERR_NO_SUPPORT;
}

static int semihost__file_sync(EvfsFile *fh)
{
    (void)fh;
    /* Semihosting has no explicit sync - writes go directly to host */
    return EVFS_OK;
}

static evfs_off_t semihost__file_size(EvfsFile *fh)
{
    SemihostFile *fil = (SemihostFile *)fh;

    int len = semihost_flen(fil->fd);
    if (len < 0) {
        return 0;
    }

    return (evfs_off_t)len;
}

static int semihost__file_seek(EvfsFile *fh, evfs_off_t offset, EvfsSeekDir origin)
{
    SemihostFile *fil = (SemihostFile *)fh;
    int abs_pos;

    switch (origin) {
    case EVFS_SEEK_TO:
        abs_pos = (int)offset;
        break;

    case EVFS_SEEK_REL:
        /* Semihosting only supports absolute seek, need to calculate */
        /* This is a limitation - we don't track current position */
        return EVFS_ERR_NO_SUPPORT;

    case EVFS_SEEK_REV:
        {
            int flen = semihost_flen(fil->fd);
            if (flen < 0) {
                return EVFS_ERR_IO;
            }
            abs_pos = flen - (int)offset;
            if (abs_pos < 0) {
                abs_pos = 0;
            }
        }
        break;

    default:
        return EVFS_ERR_BAD_ARG;
    }

    int ret = semihost_seek(fil->fd, abs_pos);
    return (ret == 0) ? EVFS_OK : EVFS_ERR_IO;
}

static evfs_off_t semihost__file_tell(EvfsFile *fh)
{
    (void)fh;
    /* Semihosting does not have a tell operation */
    /* Return 0 as we cannot track position */
    return 0;
}

static bool semihost__file_eof(EvfsFile *fh)
{
    (void)fh;
    /* Cannot determine EOF without tell support */
    return false;
}

static EvfsFileMethods s_semihost_methods = {
    .m_ctrl     = semihost__file_ctrl,
    .m_close    = semihost__file_close,
    .m_read     = semihost__file_read,
    .m_write    = semihost__file_write,
    .m_truncate = semihost__file_truncate,
    .m_sync     = semihost__file_sync,
    .m_size     = semihost__file_size,
    .m_seek     = semihost__file_seek,
    .m_tell     = semihost__file_tell,
    .m_eof      = semihost__file_eof
};

/* ==================== FS access methods ==================== */

/*
 * Convert EVFS flags to semihosting mode.
 *
 * EVFS flags mapping:
 *   EVFS_READ                 -> "rb"  (mode 1)
 *   EVFS_WRITE                -> "wb"  (mode 5)
 *   EVFS_READ | EVFS_WRITE    -> "r+b" (mode 3)
 *   EVFS_OVERWRITE            -> "wb"  (mode 5)
 *   EVFS_APPEND               -> "ab"  (mode 9)
 *   EVFS_APPEND | EVFS_READ   -> "a+b" (mode 11)
 */
static int evfs_to_semihost_mode(int flags)
{
    if (flags & EVFS_APPEND) {
        if (flags & EVFS_READ) {
            return SEMIHOST_MODE_APLUSB;  /* "a+b" */
        }
        return SEMIHOST_MODE_AB;  /* "ab" */
    }

    if (flags & EVFS_OVERWRITE) {
        if (flags & EVFS_READ) {
            return SEMIHOST_MODE_WPLUSB;  /* "w+b" */
        }
        return SEMIHOST_MODE_WB;  /* "wb" */
    }

    if ((flags & EVFS_WRITE) && !(flags & EVFS_READ)) {
        return SEMIHOST_MODE_WB;  /* "wb" */
    }

    if ((flags & EVFS_WRITE) && (flags & EVFS_READ)) {
        return SEMIHOST_MODE_RPLUSB;  /* "r+b" */
    }

    /* Default: read only */
    return SEMIHOST_MODE_RB;  /* "rb" */
}

static int semihost__open(Evfs *vfs, const char *path, EvfsFile *fh, int flags)
{
    SemihostFile *fil = (SemihostFile *)fh;
    (void)vfs;

    memset(fil, 0, sizeof(*fil));
    fh->methods = &s_semihost_methods;
    fil->fd = SEMIHOST_INVALID_FD;

    /* Skip leading slash to make path relative to debugger cwd */
    while (path[0] == '/')
        path++;

    int mode = evfs_to_semihost_mode(flags);
    semihost_fd_t fd = semihost_open(path, mode);

    if (fd < 0) {
        /* Try create if OPEN_OR_NEW and file doesn't exist */
        if (flags & EVFS_OPEN_OR_NEW) {
            fd = semihost_open(path, SEMIHOST_MODE_WPLUSB);
        }
        if (fd < 0) {
            return EVFS_ERR_NO_FILE;
        }
    }

    fil->fd = fd;
    return EVFS_OK;
}

static int semihost__stat(Evfs *vfs, const char *path, EvfsInfo *info)
{
    (void)vfs;

    memset(info, 0, sizeof(*info));

    /* Skip leading slash */
    while (path[0] == '/')
        path++;

    /* Open file temporarily to get size */
    semihost_fd_t fd = semihost_open(path, SEMIHOST_MODE_RB);
    if (fd < 0) {
        return EVFS_ERR_NO_FILE;
    }

    int len = semihost_flen(fd);
    semihost_close(fd);

    if (len >= 0) {
        info->size = (evfs_off_t)len;
    }

    /* Semihosting doesn't provide mtime or type info */
    info->mtime = 0;
    info->type = 0;  /* Regular file (not directory) */

    return EVFS_OK;
}

static int semihost__delete(Evfs *vfs, const char *path)
{
    (void)vfs;

    /* Skip leading slash */
    while (path[0] == '/')
        path++;

    int ret = semihost_remove(path);
    return (ret == 0) ? EVFS_OK : EVFS_ERR_IO;
}

static int semihost__rename(Evfs *vfs, const char *old_path, const char *new_path)
{
    (void)vfs;

    /* Skip leading slash */
    while (old_path[0] == '/')
        old_path++;
    while (new_path[0] == '/')
        new_path++;

    int ret = semihost_rename(old_path, new_path);
    return (ret == 0) ? EVFS_OK : EVFS_ERR_IO;
}

static int semihost__vfs_ctrl(Evfs *vfs, int cmd, void *arg)
{
    (void)vfs;

    switch (cmd) {
    case EVFS_CMD_UNREGISTER:
        return EVFS_OK;

    case EVFS_CMD_GET_STAT_FIELDS:
        {
            unsigned *v = (unsigned *)arg;
            *v = EVFS_INFO_SIZE;  /* Only size is available */
        }
        return EVFS_OK;

    case EVFS_CMD_GET_DIR_FIELDS:
        {
            unsigned *v = (unsigned *)arg;
            *v = 0;  /* Directory operations not supported */
        }
        return EVFS_OK;

    default:
        return EVFS_ERR_NO_SUPPORT;
    }
}

/* Static VFS instance */
static Evfs s_semihost_vfs = {
    .vfs_name = "semihost",
    .vfs_file_size = sizeof(SemihostFile),
    .vfs_dir_size = sizeof(EvfsDir),  /* Directory not supported */

    .fs_data = NULL,

    /* Required methods */
    .m_open = semihost__open,
    .m_stat = semihost__stat,

    /* Optional methods */
    .m_delete = semihost__delete,
    .m_rename = semihost__rename,
    .m_make_dir = NULL,     /* Not supported */
    .m_open_dir = NULL,     /* Not supported */
    .m_get_cur_dir = NULL,  /* Not supported */
    .m_set_cur_dir = NULL,  /* Not supported */
    .m_vfs_ctrl = semihost__vfs_ctrl,
};

/*
 * Register semihosting filesystem.
 *
 * This VFS is always named "semihost". There should only be one instance
 * per application.
 *
 * Args:
 *   default_vfs: Make this the default VFS when true
 *
 * Returns:
 *   EVFS_OK on success
 */
int evfs_register_semihost(bool default_vfs)
{
    return evfs_register(&s_semihost_vfs, default_vfs);
}

/* SPDX-License-Identifier: MIT
 * Copyright 2025 IPRO Technology Co., Ltd.
 *
 * POSIX dirent.h compatibility layer for EVFS
 * Provides standard POSIX directory functions using EVFS backend.
 */

#ifndef _EVFS_DIRENT_H
#define _EVFS_DIRENT_H

#include "evfs_posix.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Map standard POSIX types to EVFS types */
typedef evfs_DIR DIR;

struct dirent {
    int      d_ino;          /* inode number (always 0 for EVFS) */
    uint8_t  d_type;         /* file type */
    char     d_name[256];    /* file name */
};

/* File type constants (POSIX compatible) */
#define DT_UNKNOWN  EVFS_DT_UNKNOWN
#define DT_REG      EVFS_DT_REG
#define DT_DIR      EVFS_DT_DIR
#define DT_LNK      EVFS_DT_LNK

/* Map POSIX functions to EVFS POSIX layer */
static inline DIR *opendir(const char *name)
{
    return evfs_posix_opendir(name);
}

static inline struct dirent *readdir(DIR *dirp)
{
    /* Note: struct evfs_dirent and struct dirent are binary compatible */
    return (struct dirent *)evfs_posix_readdir(dirp);
}

static inline int closedir(DIR *dirp)
{
    return evfs_posix_closedir(dirp);
}

static inline void rewinddir(DIR *dirp)
{
    evfs_posix_rewinddir(dirp);
}

static inline long telldir(DIR *dirp)
{
    return evfs_posix_telldir(dirp);
}

static inline void seekdir(DIR *dirp, long loc)
{
    evfs_posix_seekdir(dirp, loc);
}

#ifdef __cplusplus
}
#endif

#endif /* _EVFS_DIRENT_H */

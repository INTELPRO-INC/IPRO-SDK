/* SPDX-License-Identifier: MIT
 * Copyright 2025 IPRO Technology Co., Ltd.
 *
 * EVFS POSIX Compatibility Layer
 * Provides POSIX-like file I/O API using EVFS backend.
 */

#ifndef EVFS_POSIX_H
#define EVFS_POSIX_H

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum mount points */
#ifndef EVFS_POSIX_MAX_MOUNTS
#define EVFS_POSIX_MAX_MOUNTS   4
#endif

/* Maximum open files */
#ifndef EVFS_POSIX_MAX_FDS
#define EVFS_POSIX_MAX_FDS      16
#endif

/* Maximum open directories */
#ifndef EVFS_POSIX_MAX_DIRS
#define EVFS_POSIX_MAX_DIRS     8
#endif

/* File descriptor offset (reserve 0-2 for stdin/stdout/stderr) */
#define EVFS_FD_OFFSET          3

/* POSIX dirent structure */
struct evfs_dirent {
    int      d_ino;     /* inode number (always 0 for EVFS) */
    uint8_t  d_type;    /* file type */
    char     d_name[256]; /* file name */
};

/* POSIX DIR structure */
typedef struct evfs_dir_s evfs_DIR;

/* File type constants */
#define EVFS_DT_UNKNOWN  0
#define EVFS_DT_REG      8   /* regular file */
#define EVFS_DT_DIR      4   /* directory */
#define EVFS_DT_LNK      10  /* symbolic link */

/* Initialize EVFS POSIX layer */
int evfs_posix_init(void);

/* File operations */
int evfs_posix_open(const char *path, int flags, ...);
int evfs_posix_close(int fd);
ssize_t evfs_posix_read(int fd, void *buf, size_t count);
ssize_t evfs_posix_write(int fd, const void *buf, size_t count);
off_t evfs_posix_lseek(int fd, off_t offset, int whence);
int evfs_posix_fstat(int fd, struct stat *buf);
int evfs_posix_fsync(int fd);
int evfs_posix_ftruncate(int fd, off_t length);

/* Path operations */
int evfs_posix_stat(const char *path, struct stat *buf);
int evfs_posix_unlink(const char *path);
int evfs_posix_rename(const char *oldpath, const char *newpath);
int evfs_posix_mkdir(const char *path, mode_t mode);
int evfs_posix_rmdir(const char *path);
int evfs_posix_access(const char *path, int mode);

/* Directory operations */
evfs_DIR *evfs_posix_opendir(const char *path);
struct evfs_dirent *evfs_posix_readdir(evfs_DIR *dirp);
int evfs_posix_closedir(evfs_DIR *dirp);
void evfs_posix_rewinddir(evfs_DIR *dirp);
long evfs_posix_telldir(evfs_DIR *dirp);
void evfs_posix_seekdir(evfs_DIR *dirp, long loc);

/* High-level functions */
int evfs_posix_remove(const char *path);
char *evfs_posix_getcwd(char *buf, size_t size);
int evfs_posix_chdir(const char *path);

/* Mount point management */
int evfs_posix_mount(const char *mount_path, const char *vfs_name);
int evfs_posix_umount(const char *mount_path);
void evfs_posix_list_mounts(void);

#ifdef __cplusplus
}
#endif

#endif /* EVFS_POSIX_H */

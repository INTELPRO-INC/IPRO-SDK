/* SPDX-License-Identifier: MIT
 * Copyright 2025 IPRO Technology Co., Ltd.
 *
 * EVFS POSIX Compatibility Layer Implementation
 * Supports multiple mount points for different filesystems.
 */

#include "evfs_posix.h"
#include "evfs.h"
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include "compiler/common.h"

#if defined(CONFIG_EVFS_USE_SEMIHOST)
#include "evfs/semihost_fs.h"
#endif

/* Mount point entry */
typedef struct {
    char mount_path[32];    /* Mount point path (e.g., "/lfs") */
    char vfs_name[16];      /* EVFS VFS name */
    bool in_use;            /* Entry is in use */
} mount_entry_t;

/* File descriptor table entry */
typedef struct {
    EvfsFile *fh;           /* EVFS file handle */
    char vfs_name[16];      /* VFS name for this fd */
    int flags;              /* open flags */
    bool in_use;            /* entry is in use */
} fd_entry_t;

/* Directory handle table entry */
typedef struct evfs_dir_s {
    EvfsDir *dh;                /* EVFS directory handle */
    char vfs_name[16];          /* VFS name for this dir */
    struct evfs_dirent entry;   /* current dirent entry */
    long position;              /* directory position */
    bool in_use;                /* entry is in use */
} evfs_dir_entry_t;

/* Mount point table */
#ifndef EVFS_POSIX_MAX_MOUNTS
#define EVFS_POSIX_MAX_MOUNTS   4
#endif

static ATTR_PSRAM_SECTION mount_entry_t s_mount_table[EVFS_POSIX_MAX_MOUNTS];

/* File descriptor table */
static ATTR_PSRAM_SECTION fd_entry_t s_fd_table[EVFS_POSIX_MAX_FDS];

/* Directory handle table */
static ATTR_PSRAM_SECTION evfs_dir_entry_t s_dir_table[EVFS_POSIX_MAX_DIRS];

/* Current working directory */
static ATTR_PSRAM_SECTION char s_cwd[256] = "/";

/* Initialization flag */
static bool s_initialized = false;

/* Convert EVFS error to errno */
static int evfs_to_errno(int evfs_err)
{
    switch (evfs_err) {
        case EVFS_OK:            return 0;
        case EVFS_ERR:           return EIO;
        case EVFS_ERR_NO_PATH:   return ENOENT;
        case EVFS_ERR_NO_FILE:   return ENOENT;
        case EVFS_ERR_EXISTS:    return EEXIST;
        case EVFS_ERR_IS_DIR:    return EISDIR;
        case EVFS_ERR_FS_FULL:   return ENOSPC;
        case EVFS_ERR_IO:        return EIO;
        case EVFS_ERR_BAD_ARG:   return EINVAL;
        case EVFS_ERR_NOT_EMPTY: return ENOTEMPTY;
        default:                 return EIO;
    }
}

/*
 * Parse path to extract VFS name and relative path.
 * Path format: /mount_point/relative_path
 * Example: /lfs/dir/file.txt -> vfs_name="lfs", rel_path="/dir/file.txt"
 *
 * Returns: true if mount point found, false otherwise
 */
static bool parse_mount_path(const char *path, const char **vfs_name, const char **rel_path)
{
    if (!path || path[0] != '/') {
        return false;
    }

    /* Check each mount point */
    for (int i = 0; i < EVFS_POSIX_MAX_MOUNTS; i++) {
        if (!s_mount_table[i].in_use) {
            continue;
        }

        size_t mount_len = strlen(s_mount_table[i].mount_path);

        /* Check if path starts with this mount point */
        if (strncmp(path, s_mount_table[i].mount_path, mount_len) == 0) {
            /* Must be exact match or followed by '/' */
            if (path[mount_len] == '\0' || path[mount_len] == '/') {
                *vfs_name = s_mount_table[i].vfs_name;
                /* Relative path starts after mount point */
                if (path[mount_len] == '\0') {
                    *rel_path = "/";
                } else {
                    *rel_path = &path[mount_len];
                }
                return true;
            }
        }
    }

    return false;
}

/* Allocate a file descriptor */
static int alloc_fd(void)
{
    for (int i = 0; i < EVFS_POSIX_MAX_FDS; i++) {
        if (!s_fd_table[i].in_use) {
            s_fd_table[i].in_use = true;
            return i + EVFS_FD_OFFSET;
        }
    }
    return -1;
}

/* Free a file descriptor */
static void free_fd(int fd)
{
    int idx = fd - EVFS_FD_OFFSET;
    if (idx >= 0 && idx < EVFS_POSIX_MAX_FDS) {
        s_fd_table[idx].fh = NULL;
        s_fd_table[idx].flags = 0;
        s_fd_table[idx].vfs_name[0] = '\0';
        s_fd_table[idx].in_use = false;
    }
}

/* Get fd entry */
static fd_entry_t *get_fd_entry(int fd)
{
    int idx = fd - EVFS_FD_OFFSET;
    if (idx >= 0 && idx < EVFS_POSIX_MAX_FDS && s_fd_table[idx].in_use) {
        return &s_fd_table[idx];
    }
    return NULL;
}

/* Allocate a directory handle */
static evfs_DIR *alloc_dir(void)
{
    for (int i = 0; i < EVFS_POSIX_MAX_DIRS; i++) {
        if (!s_dir_table[i].in_use) {
            s_dir_table[i].in_use = true;
            s_dir_table[i].position = 0;
            return &s_dir_table[i];
        }
    }
    return NULL;
}

/* Free a directory handle */
static void free_dir(evfs_DIR *dirp)
{
    if (dirp) {
        dirp->dh = NULL;
        dirp->position = 0;
        dirp->vfs_name[0] = '\0';
        dirp->in_use = false;
    }
}

/* Convert POSIX flags to EVFS flags */
static int posix_to_evfs_flags(int flags)
{
    int evfs_flags = 0;

    if ((flags & O_ACCMODE) == O_RDONLY) {
        evfs_flags = EVFS_READ;
    } else if ((flags & O_ACCMODE) == O_WRONLY) {
        evfs_flags = EVFS_WRITE;
    } else if ((flags & O_ACCMODE) == O_RDWR) {
        evfs_flags = EVFS_READ | EVFS_WRITE;
    }

    if (flags & O_CREAT) {
        evfs_flags |= EVFS_OPEN_OR_NEW;
    }
    if (flags & O_TRUNC) {
        evfs_flags |= EVFS_OVERWRITE;
    }
    if (flags & O_APPEND) {
        evfs_flags |= EVFS_APPEND;
    }

    return evfs_flags;
}

/* Initialize EVFS POSIX layer */
int evfs_posix_init(void)
{
    if (s_initialized) {
        return 0;
    }

    memset(s_mount_table, 0, sizeof(s_mount_table));
    memset(s_fd_table, 0, sizeof(s_fd_table));
    memset(s_dir_table, 0, sizeof(s_dir_table));
    strcpy(s_cwd, "/");

    evfs_init();
    s_initialized = true;

#if defined(CONFIG_EVFS_USE_SEMIHOST)
    /* Auto-register and mount semihosting filesystem at /host */
    if (evfs_register_semihost(false) == EVFS_OK) {
        evfs_posix_mount("/host", "semihost");
    }
#endif

    return 0;
}

/* Mount a filesystem at a given path */
int evfs_posix_mount(const char *mount_path, const char *vfs_name)
{
    if (!s_initialized) {
        evfs_posix_init();
    }

    if (!mount_path || !vfs_name || mount_path[0] != '/') {
        return -EINVAL;
    }

    /* Check if VFS exists */
    if (!evfs_find_vfs(vfs_name)) {
        return -ENODEV;
    }

    /* Find free slot or existing mount */
    for (int i = 0; i < EVFS_POSIX_MAX_MOUNTS; i++) {
        if (s_mount_table[i].in_use &&
            strcmp(s_mount_table[i].mount_path, mount_path) == 0) {
            /* Already mounted at this path */
            return -EBUSY;
        }
    }

    for (int i = 0; i < EVFS_POSIX_MAX_MOUNTS; i++) {
        if (!s_mount_table[i].in_use) {
            strncpy(s_mount_table[i].mount_path, mount_path,
                    sizeof(s_mount_table[i].mount_path) - 1);
            strncpy(s_mount_table[i].vfs_name, vfs_name,
                    sizeof(s_mount_table[i].vfs_name) - 1);
            s_mount_table[i].in_use = true;
            return 0;
        }
    }

    return -ENOMEM; /* No free mount slots */
}

/* Unmount a filesystem */
int evfs_posix_umount(const char *mount_path)
{
    if (!mount_path) {
        return -EINVAL;
    }

    for (int i = 0; i < EVFS_POSIX_MAX_MOUNTS; i++) {
        if (s_mount_table[i].in_use &&
            strcmp(s_mount_table[i].mount_path, mount_path) == 0) {
            s_mount_table[i].in_use = false;
            s_mount_table[i].mount_path[0] = '\0';
            s_mount_table[i].vfs_name[0] = '\0';
            return 0;
        }
    }

    return -EINVAL; /* Not mounted */
}

/* Open a file */
int evfs_posix_open(const char *path, int flags, ...)
{
    if (!s_initialized) {
        evfs_posix_init();
    }

    if (!path) {
        errno = EINVAL;
        return -1;
    }

    const char *vfs_name = NULL;
    const char *rel_path = NULL;

    if (!parse_mount_path(path, &vfs_name, &rel_path)) {
        errno = ENOENT;
        return -1;
    }

    int fd = alloc_fd();
    if (fd < 0) {
        errno = EMFILE;
        return -1;
    }

    fd_entry_t *entry = get_fd_entry(fd);
    int evfs_flags = posix_to_evfs_flags(flags);

    int ret = evfs_open_ex(rel_path, &entry->fh, evfs_flags, vfs_name);
    if (ret != EVFS_OK) {
        /* If O_CREAT and file doesn't exist, try with overwrite */
        if ((flags & O_CREAT) && ret == EVFS_ERR_NO_FILE) {
            evfs_flags |= EVFS_OVERWRITE;
            ret = evfs_open_ex(rel_path, &entry->fh, evfs_flags, vfs_name);
        }
        if (ret != EVFS_OK) {
            free_fd(fd);
            errno = evfs_to_errno(ret);
            return -1;
        }
    }

    entry->flags = flags;
    strncpy(entry->vfs_name, vfs_name, sizeof(entry->vfs_name) - 1);
    return fd;
}

/* Close a file */
int evfs_posix_close(int fd)
{
    fd_entry_t *entry = get_fd_entry(fd);
    if (!entry) {
        errno = EBADF;
        return -1;
    }

    int ret = evfs_file_close(entry->fh);
    free_fd(fd);

    if (ret != EVFS_OK) {
        errno = evfs_to_errno(ret);
        return -1;
    }
    return 0;
}

/* Read from a file */
ssize_t evfs_posix_read(int fd, void *buf, size_t count)
{
    fd_entry_t *entry = get_fd_entry(fd);
    if (!entry) {
        errno = EBADF;
        return -1;
    }

    if (!buf) {
        errno = EINVAL;
        return -1;
    }

    ptrdiff_t ret = evfs_file_read(entry->fh, buf, count);
    if (ret < 0) {
        errno = EIO;
        return -1;
    }
    return (ssize_t)ret;
}

/* Write to a file */
ssize_t evfs_posix_write(int fd, const void *buf, size_t count)
{
    fd_entry_t *entry = get_fd_entry(fd);
    if (!entry) {
        errno = EBADF;
        return -1;
    }

    if (!buf) {
        errno = EINVAL;
        return -1;
    }

    ptrdiff_t ret = evfs_file_write(entry->fh, buf, count);
    if (ret < 0) {
        errno = EIO;
        return -1;
    }
    return (ssize_t)ret;
}

/* Seek in a file */
off_t evfs_posix_lseek(int fd, off_t offset, int whence)
{
    fd_entry_t *entry = get_fd_entry(fd);
    if (!entry) {
        errno = EBADF;
        return -1;
    }

    EvfsSeekDir origin;
    switch (whence) {
        case SEEK_SET: origin = EVFS_SEEK_TO; break;
        case SEEK_CUR: origin = EVFS_SEEK_REL; break;
        case SEEK_END: origin = EVFS_SEEK_REV; break;
        default:
            errno = EINVAL;
            return -1;
    }

    int ret = evfs_file_seek(entry->fh, offset, origin);
    if (ret != EVFS_OK) {
        errno = EINVAL;
        return -1;
    }

    return (off_t)evfs_file_tell(entry->fh);
}

/* Get file status by fd */
int evfs_posix_fstat(int fd, struct stat *buf)
{
    fd_entry_t *entry = get_fd_entry(fd);
    if (!entry) {
        errno = EBADF;
        return -1;
    }

    if (!buf) {
        errno = EINVAL;
        return -1;
    }

    memset(buf, 0, sizeof(struct stat));
    buf->st_size = evfs_file_size(entry->fh);
    buf->st_mode = S_IFREG | 0644;

    return 0;
}

/* Sync file to disk */
int evfs_posix_fsync(int fd)
{
    fd_entry_t *entry = get_fd_entry(fd);
    if (!entry) {
        errno = EBADF;
        return -1;
    }

    int ret = evfs_file_sync(entry->fh);
    if (ret != EVFS_OK) {
        errno = EIO;
        return -1;
    }
    return 0;
}

/* Truncate file */
int evfs_posix_ftruncate(int fd, off_t length)
{
    fd_entry_t *entry = get_fd_entry(fd);
    if (!entry) {
        errno = EBADF;
        return -1;
    }

    int ret = evfs_file_truncate(entry->fh, length);
    if (ret != EVFS_OK) {
        errno = EIO;
        return -1;
    }
    return 0;
}

/* Get file status by path */
int evfs_posix_stat(const char *path, struct stat *buf)
{
    if (!s_initialized) {
        evfs_posix_init();
    }

    if (!path || !buf) {
        errno = EINVAL;
        return -1;
    }

    const char *vfs_name = NULL;
    const char *rel_path = NULL;

    if (!parse_mount_path(path, &vfs_name, &rel_path)) {
        errno = ENOENT;
        return -1;
    }

    EvfsInfo info;
    int ret = evfs_stat_ex(rel_path, &info, vfs_name);
    if (ret != EVFS_OK) {
        errno = evfs_to_errno(ret);
        return -1;
    }

    memset(buf, 0, sizeof(struct stat));
    buf->st_size = info.size;
    buf->st_mtime = info.mtime;

    if (info.type & EVFS_FILE_DIR) {
        buf->st_mode = S_IFDIR | 0755;
    } else {
        buf->st_mode = S_IFREG | 0644;
    }

    return 0;
}

/* Remove a file */
int evfs_posix_unlink(const char *path)
{
    if (!s_initialized) {
        evfs_posix_init();
    }

    if (!path) {
        errno = EINVAL;
        return -1;
    }

    const char *vfs_name = NULL;
    const char *rel_path = NULL;

    if (!parse_mount_path(path, &vfs_name, &rel_path)) {
        errno = ENOENT;
        return -1;
    }

    int ret = evfs_delete_ex(rel_path, vfs_name);
    if (ret != EVFS_OK) {
        errno = evfs_to_errno(ret);
        return -1;
    }
    return 0;
}

/* Rename a file */
int evfs_posix_rename(const char *oldpath, const char *newpath)
{
    if (!s_initialized) {
        evfs_posix_init();
    }

    if (!oldpath || !newpath) {
        errno = EINVAL;
        return -1;
    }

    const char *old_vfs = NULL;
    const char *old_rel = NULL;
    const char *new_vfs = NULL;
    const char *new_rel = NULL;

    if (!parse_mount_path(oldpath, &old_vfs, &old_rel) ||
        !parse_mount_path(newpath, &new_vfs, &new_rel)) {
        errno = ENOENT;
        return -1;
    }

    /* Can only rename within the same VFS */
    if (strcmp(old_vfs, new_vfs) != 0) {
        errno = EXDEV;
        return -1;
    }

    int ret = evfs_rename_ex(old_rel, new_rel, old_vfs);
    if (ret != EVFS_OK) {
        errno = evfs_to_errno(ret);
        return -1;
    }
    return 0;
}

/* Create a directory */
int evfs_posix_mkdir(const char *path, mode_t mode)
{
    (void)mode; /* EVFS doesn't support mode */

    if (!s_initialized) {
        evfs_posix_init();
    }

    if (!path) {
        errno = EINVAL;
        return -1;
    }

    const char *vfs_name = NULL;
    const char *rel_path = NULL;

    if (!parse_mount_path(path, &vfs_name, &rel_path)) {
        errno = ENOENT;
        return -1;
    }

    int ret = evfs_make_dir_ex(rel_path, vfs_name);
    if (ret != EVFS_OK) {
        errno = evfs_to_errno(ret);
        return -1;
    }
    return 0;
}

/* Remove a directory */
int evfs_posix_rmdir(const char *path)
{
    if (!s_initialized) {
        evfs_posix_init();
    }

    if (!path) {
        errno = EINVAL;
        return -1;
    }

    const char *vfs_name = NULL;
    const char *rel_path = NULL;

    if (!parse_mount_path(path, &vfs_name, &rel_path)) {
        errno = ENOENT;
        return -1;
    }

    int ret = evfs_delete_ex(rel_path, vfs_name);
    if (ret != EVFS_OK) {
        errno = evfs_to_errno(ret);
        return -1;
    }
    return 0;
}

/* Check file access */
int evfs_posix_access(const char *path, int mode)
{
    if (!s_initialized) {
        evfs_posix_init();
    }

    if (!path) {
        errno = EINVAL;
        return -1;
    }

    const char *vfs_name = NULL;
    const char *rel_path = NULL;

    if (!parse_mount_path(path, &vfs_name, &rel_path)) {
        errno = ENOENT;
        return -1;
    }

    /* Just check if file exists for now */
    if (evfs_existing_file_ex(rel_path, vfs_name) ||
        evfs_existing_dir_ex(rel_path, vfs_name)) {
        return 0;
    }

    errno = ENOENT;
    return -1;
}

/* Open a directory */
evfs_DIR *evfs_posix_opendir(const char *path)
{
    if (!s_initialized) {
        evfs_posix_init();
    }

    if (!path) {
        errno = EINVAL;
        return NULL;
    }

    /* Handle root directory - list mount points */
    if (strcmp(path, "/") == 0) {
        evfs_DIR *dirp = alloc_dir();
        if (!dirp) {
            errno = EMFILE;
            return NULL;
        }
        dirp->dh = NULL; /* Special marker for root */
        strcpy(dirp->vfs_name, "");
        return dirp;
    }

    const char *vfs_name = NULL;
    const char *rel_path = NULL;

    if (!parse_mount_path(path, &vfs_name, &rel_path)) {
        errno = ENOENT;
        return NULL;
    }

    evfs_DIR *dirp = alloc_dir();
    if (!dirp) {
        errno = EMFILE;
        return NULL;
    }

    int ret = evfs_open_dir_ex(rel_path, &dirp->dh, vfs_name);
    if (ret != EVFS_OK) {
        free_dir(dirp);
        errno = evfs_to_errno(ret);
        return NULL;
    }

    strncpy(dirp->vfs_name, vfs_name, sizeof(dirp->vfs_name) - 1);
    return dirp;
}

/* Read directory entry */
struct evfs_dirent *evfs_posix_readdir(evfs_DIR *dirp)
{
    if (!dirp || !dirp->in_use) {
        errno = EBADF;
        return NULL;
    }

    /* Handle root directory - enumerate mount points */
    if (dirp->dh == NULL && dirp->vfs_name[0] == '\0') {
        /* Find next active mount point */
        while (dirp->position < EVFS_POSIX_MAX_MOUNTS) {
            int idx = dirp->position++;
            if (s_mount_table[idx].in_use) {
                dirp->entry.d_ino = 0;
                dirp->entry.d_type = EVFS_DT_DIR;
                /* Copy mount point name without leading '/' */
                const char *name = s_mount_table[idx].mount_path + 1;
                strncpy(dirp->entry.d_name, name, sizeof(dirp->entry.d_name) - 1);
                dirp->entry.d_name[sizeof(dirp->entry.d_name) - 1] = '\0';
                return &dirp->entry;
            }
        }
        return NULL; /* No more mount points */
    }

    EvfsInfo info;
    int ret = evfs_dir_read(dirp->dh, &info);
    if (ret != EVFS_OK) {
        if (ret == EVFS_DONE) {
            return NULL; /* End of directory */
        }
        errno = EIO;
        return NULL;
    }

    /* Fill in dirent structure */
    dirp->entry.d_ino = 0;
    dirp->entry.d_type = (info.type & EVFS_FILE_DIR) ? EVFS_DT_DIR : EVFS_DT_REG;
    strncpy(dirp->entry.d_name, info.name, sizeof(dirp->entry.d_name) - 1);
    dirp->entry.d_name[sizeof(dirp->entry.d_name) - 1] = '\0';
    dirp->position++;

    return &dirp->entry;
}

/* Close a directory */
int evfs_posix_closedir(evfs_DIR *dirp)
{
    if (!dirp || !dirp->in_use) {
        errno = EBADF;
        return -1;
    }

    int ret = EVFS_OK;
    if (dirp->dh != NULL) {
        ret = evfs_dir_close(dirp->dh);
    }
    free_dir(dirp);

    if (ret != EVFS_OK) {
        errno = EIO;
        return -1;
    }
    return 0;
}

/* Rewind directory */
void evfs_posix_rewinddir(evfs_DIR *dirp)
{
    if (dirp && dirp->in_use) {
        if (dirp->dh != NULL) {
            evfs_dir_rewind(dirp->dh);
        }
        dirp->position = 0;
    }
}

/* Get directory position */
long evfs_posix_telldir(evfs_DIR *dirp)
{
    if (!dirp || !dirp->in_use) {
        return -1;
    }
    return dirp->position;
}

/* Set directory position */
void evfs_posix_seekdir(evfs_DIR *dirp, long loc)
{
    if (!dirp || !dirp->in_use) {
        return;
    }

    if (dirp->dh != NULL) {
        /* Rewind and read forward to position */
        evfs_dir_rewind(dirp->dh);
        dirp->position = 0;

        EvfsInfo info;
        while (dirp->position < loc) {
            if (evfs_dir_read(dirp->dh, &info) != EVFS_OK) {
                break;
            }
            dirp->position++;
        }
    } else {
        /* Root directory - just set position */
        dirp->position = loc;
    }
}

/* Remove file or directory */
int evfs_posix_remove(const char *path)
{
    return evfs_posix_unlink(path);
}

/* Get current working directory */
char *evfs_posix_getcwd(char *buf, size_t size)
{
    if (!buf || size == 0) {
        errno = EINVAL;
        return NULL;
    }

    size_t len = strlen(s_cwd);
    if (len >= size) {
        errno = ERANGE;
        return NULL;
    }

    strcpy(buf, s_cwd);
    return buf;
}

/* Change current working directory */
int evfs_posix_chdir(const char *path)
{
    if (!s_initialized) {
        evfs_posix_init();
    }

    if (!path) {
        errno = EINVAL;
        return -1;
    }

    /* Handle root directory */
    if (strcmp(path, "/") == 0) {
        strcpy(s_cwd, "/");
        return 0;
    }

    const char *vfs_name = NULL;
    const char *rel_path = NULL;

    if (!parse_mount_path(path, &vfs_name, &rel_path)) {
        errno = ENOENT;
        return -1;
    }

    if (!evfs_existing_dir_ex(rel_path, vfs_name)) {
        errno = ENOENT;
        return -1;
    }

    strncpy(s_cwd, path, sizeof(s_cwd) - 1);
    s_cwd[sizeof(s_cwd) - 1] = '\0';

    return 0;
}

/* List all mount points (for debugging) */
void evfs_posix_list_mounts(void)
{
    printf("Mount points:\r\n");
    for (int i = 0; i < EVFS_POSIX_MAX_MOUNTS; i++) {
        if (s_mount_table[i].in_use) {
            printf("  %s -> %s\r\n",
                   s_mount_table[i].mount_path,
                   s_mount_table[i].vfs_name);
        }
    }
}

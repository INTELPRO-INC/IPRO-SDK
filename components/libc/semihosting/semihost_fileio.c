/*
 * Copyright (c) 2026 IPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Semihosting file I/O implementation.
 */

#include <string.h>
#include "semihost_common.h"
#include "semihost_fileio.h"

semihost_fd_t semihost_open(const char *path, int mode)
{
    long args[3];

    if (!path) {
        return SEMIHOST_ERR_GENERIC;
    }

    args[0] = (long)path;
    args[1] = (long)mode;
    args[2] = (long)strlen(path);

    long ret = semihost_call(SYS_OPEN, (long)args);

    /* SYS_OPEN returns -1 on failure */
    if (ret == -1) {
        return SEMIHOST_ERR_NOTFOUND;
    }

    return (semihost_fd_t)ret;
}

int semihost_close(semihost_fd_t fd)
{
    long args[1];

    args[0] = (long)fd;

    long ret = semihost_call(SYS_CLOSE, (long)args);

    return (ret == 0) ? 0 : SEMIHOST_ERR_GENERIC;
}

int semihost_read(semihost_fd_t fd, void *buf, int len)
{
    long args[3];

    if (!buf || len <= 0) {
        return SEMIHOST_ERR_GENERIC;
    }

    args[0] = (long)fd;
    args[1] = (long)buf;
    args[2] = (long)len;

    /*
     * SYS_READ returns:
     *   0 if successful and all bytes read
     *   number of bytes NOT read if partial read or EOF
     *   same as len if error or EOF at start
     */
    long not_read = semihost_call(SYS_READ, (long)args);

    if (not_read < 0) {
        return SEMIHOST_ERR_GENERIC;
    }

    int bytes_read = len - (int)not_read;

    return bytes_read;
}

int semihost_write(semihost_fd_t fd, const void *buf, int len)
{
    long args[3];

    if (!buf || len <= 0) {
        return SEMIHOST_ERR_GENERIC;
    }

    args[0] = (long)fd;
    args[1] = (long)buf;
    args[2] = (long)len;

    /*
     * SYS_WRITE returns:
     *   0 if successful and all bytes written
     *   number of bytes NOT written on error
     */
    long not_written = semihost_call(SYS_WRITE, (long)args);

    if (not_written < 0) {
        return SEMIHOST_ERR_GENERIC;
    }

    int bytes_written = len - (int)not_written;

    /* If nothing written, treat as error */
    if (bytes_written == 0 && len > 0) {
        return SEMIHOST_ERR_GENERIC;
    }

    return bytes_written;
}

int semihost_seek(semihost_fd_t fd, int offset)
{
    long args[2];

    if (offset < 0) {
        return SEMIHOST_ERR_GENERIC;
    }

    args[0] = (long)fd;
    args[1] = (long)offset;

    long ret = semihost_call(SYS_SEEK, (long)args);

    /* SYS_SEEK returns 0 on success, negative on failure */
    return (ret == 0) ? 0 : SEMIHOST_ERR_GENERIC;
}

int semihost_flen(semihost_fd_t fd)
{
    long args[1];

    args[0] = (long)fd;

    long ret = semihost_call(SYS_FLEN, (long)args);

    /* SYS_FLEN returns file length or -1 on error */
    if (ret < 0) {
        return SEMIHOST_ERR_GENERIC;
    }

    return (int)ret;
}

int semihost_remove(const char *path)
{
    long args[2];

    if (!path) {
        return SEMIHOST_ERR_GENERIC;
    }

    args[0] = (long)path;
    args[1] = (long)strlen(path);

    long ret = semihost_call(SYS_REMOVE, (long)args);

    /* SYS_REMOVE returns 0 on success */
    return (ret == 0) ? 0 : SEMIHOST_ERR_GENERIC;
}

int semihost_rename(const char *old_path, const char *new_path)
{
    long args[4];

    if (!old_path || !new_path) {
        return SEMIHOST_ERR_GENERIC;
    }

    args[0] = (long)old_path;
    args[1] = (long)strlen(old_path);
    args[2] = (long)new_path;
    args[3] = (long)strlen(new_path);

    long ret = semihost_call(SYS_RENAME, (long)args);

    /* SYS_RENAME returns 0 on success */
    return (ret == 0) ? 0 : SEMIHOST_ERR_GENERIC;
}

int semihost_isatty(semihost_fd_t fd)
{
    long args[1];

    args[0] = (long)fd;

    long ret = semihost_call(SYS_ISATTY, (long)args);

    /*
     * SYS_ISATTY returns:
     *   1 if interactive device (TTY)
     *   0 if not
     */
    return (int)ret;
}

int semihost_getcwd(char *buf, int len)
{
    long args[2];

    if (!buf || len <= 0) {
        return SEMIHOST_ERR_GENERIC;
    }

    args[0] = (long)buf;
    args[1] = (long)len;

    long ret = semihost_call(SYS_GETCWD, (long)args);

    /* SYS_GETCWD returns 0 on success, non-zero on failure */
    return (ret == 0) ? 0 : SEMIHOST_ERR_GENERIC;
}

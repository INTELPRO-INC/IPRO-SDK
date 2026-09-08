/*
 * Copyright (c) 2026 IPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Semihosting file I/O API.
 * Enables FPGA to read/write files on host PC through debugger.
 */

#ifndef SEMIHOST_FILEIO_H
#define SEMIHOST_FILEIO_H

#include <stddef.h>

/* File handle type */
typedef int semihost_fd_t;

/* Special handles (always available) */
#define SEMIHOST_STDIN   0
#define SEMIHOST_STDOUT  1
#define SEMIHOST_STDERR  2

/* Invalid file descriptor */
#define SEMIHOST_INVALID_FD  (-1)

/*
 * Open modes (matches ARM semihosting specification)
 *
 * Mode values map to fopen modes:
 *   0 = "r"   (read only)
 *   1 = "rb"  (read only, binary)
 *   2 = "r+"  (read/write)
 *   3 = "r+b" (read/write, binary)
 *   4 = "w"   (write only, truncate)
 *   5 = "wb"  (write only, binary, truncate)
 *   6 = "w+"  (read/write, truncate)
 *   7 = "w+b" (read/write, binary, truncate)
 *   8 = "a"   (append)
 *   9 = "ab"  (append, binary)
 *  10 = "a+"  (read/append)
 *  11 = "a+b" (read/append, binary)
 */
#define SEMIHOST_MODE_R      0   /* "r"   */
#define SEMIHOST_MODE_RB     1   /* "rb"  */
#define SEMIHOST_MODE_RPLUS  2   /* "r+"  */
#define SEMIHOST_MODE_RPLUSB 3   /* "r+b" */
#define SEMIHOST_MODE_W      4   /* "w"   */
#define SEMIHOST_MODE_WB     5   /* "wb"  */
#define SEMIHOST_MODE_WPLUS  6   /* "w+"  */
#define SEMIHOST_MODE_WPLUSB 7   /* "w+b" */
#define SEMIHOST_MODE_A      8   /* "a"   */
#define SEMIHOST_MODE_AB     9   /* "ab"  */
#define SEMIHOST_MODE_APLUS  10  /* "a+"  */
#define SEMIHOST_MODE_APLUSB 11  /* "a+b" */

/* Error codes (negative values) */
#define SEMIHOST_ERR_GENERIC  (-1)
#define SEMIHOST_ERR_NOTFOUND (-2)
#define SEMIHOST_ERR_PERM     (-3)
#define SEMIHOST_ERR_NOMEM    (-4)

/*
 * Open a file on the host.
 *
 * @param path  Path to file on host (absolute or relative to GDB cwd)
 * @param mode  Open mode (SEMIHOST_MODE_*)
 * @return      File descriptor (>= 0) on success, negative error code on failure
 */
semihost_fd_t semihost_open(const char *path, int mode);

/*
 * Close an open file.
 *
 * @param fd    File descriptor from semihost_open()
 * @return      0 on success, negative error code on failure
 */
int semihost_close(semihost_fd_t fd);

/*
 * Read data from file.
 *
 * @param fd    File descriptor
 * @param buf   Buffer to read into
 * @param len   Maximum bytes to read
 * @return      Bytes actually read (>= 0), or negative error code
 *              Returns 0 at end of file
 */
int semihost_read(semihost_fd_t fd, void *buf, int len);

/*
 * Write data to file.
 *
 * @param fd    File descriptor
 * @param buf   Data to write
 * @param len   Bytes to write
 * @return      Bytes actually written (>= 0), or negative error code
 */
int semihost_write(semihost_fd_t fd, const void *buf, int len);

/*
 * Seek to position in file.
 *
 * @param fd      File descriptor
 * @param offset  Absolute position from start of file
 * @return        0 on success, negative error code on failure
 */
int semihost_seek(semihost_fd_t fd, int offset);

/*
 * Get file length.
 *
 * @param fd    File descriptor
 * @return      File length in bytes (>= 0), or negative error code
 */
int semihost_flen(semihost_fd_t fd);

/*
 * Remove (delete) a file on the host.
 *
 * @param path  Path to file
 * @return      0 on success, negative error code on failure
 */
int semihost_remove(const char *path);

/*
 * Rename a file on the host.
 *
 * @param old_path  Current file path
 * @param new_path  New file path
 * @return          0 on success, negative error code on failure
 */
int semihost_rename(const char *old_path, const char *new_path);

/*
 * Check if file descriptor is a terminal (TTY).
 *
 * @param fd    File descriptor
 * @return      1 if TTY, 0 if not, negative error code on failure
 */
int semihost_isatty(semihost_fd_t fd);

/*
 * Get current working directory of the host debugger.
 *
 * @param buf   Buffer to store path
 * @param len   Buffer size
 * @return      0 on success, negative error code on failure
 */
int semihost_getcwd(char *buf, int len);

#endif /* SEMIHOST_FILEIO_H */

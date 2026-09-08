/* SPDX-License-Identifier: MIT
 * Copyright 2026 IPRO Technology Co., Ltd.
 *
 * EVFS Semihosting filesystem backend.
 * Provides access to host filesystem through debugger semihosting interface.
 */

#ifndef EVFS_SEMIHOST_FS_H
#define EVFS_SEMIHOST_FS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Register semihosting filesystem with EVFS.
 *
 * This VFS is always named "semihost". There should only be one instance
 * per application. Files are accessed relative to the debugger's working
 * directory.
 *
 * @param default_vfs   Make this the default VFS when true
 * @return              EVFS_OK on success, error code on failure
 */
int evfs_register_semihost(bool default_vfs);

#ifdef __cplusplus
}
#endif

#endif /* EVFS_SEMIHOST_FS_H */

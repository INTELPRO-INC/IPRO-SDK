# SPDX-FileCopyrightText: 2020 Kevin Thibedeau
# SPDX-FileCopyrightText: 2025 IPRO Technology Co., Ltd.
# SPDX-License-Identifier: MIT
#
# EVFS - Embedded Virtual Filesystem
# Ported to IPRO SDK with FreeRTOS support

__generate_library()

__component_add_include_dirs(include)
__component_add_include_dirs(include/evfs)

# Core EVFS sources
set(EVFS_CORE_SOURCES
    evfs.c
    evfs_path.c
    evfs_freertos.c
)

# Utility sources
set(EVFS_UTIL_SOURCES
    util/glob.c
    util/range_strings.c
)

# Filesystem drivers - conditionally included
set(EVFS_FS_SOURCES "")

# FatFs support
if(CONFIG_EVFS_USE_FATFS)
    list(APPEND EVFS_FS_SOURCES fatfs_fs.c)
    if(CONFIG_EVFS_USE_FATFS_IMAGE)
        list(APPEND EVFS_FS_SOURCES fatfs_image.c)
    endif()
endif()

# LittleFS support
if(CONFIG_EVFS_USE_LITTLEFS)
    list(APPEND EVFS_FS_SOURCES littlefs_fs.c)
    if(CONFIG_EVFS_USE_LITTLEFS_IMAGE)
        list(APPEND EVFS_FS_SOURCES littlefs_image.c)
    endif()
endif()

# Romfs support
if(CONFIG_EVFS_USE_ROMFS)
    list(APPEND EVFS_FS_SOURCES romfs_common.c romfs_fs.c)
endif()

# Tar support
if(CONFIG_EVFS_USE_TAR)
    list(APPEND EVFS_FS_SOURCES tar_fs.c tar_iter.c)
    if(CONFIG_EVFS_USE_TAR_RSRC)
        list(APPEND EVFS_FS_SOURCES tar_rsrc_fs.c tar_iter_rsrc.c)
    endif()
endif()

# Semihosting support
if(CONFIG_EVFS_USE_SEMIHOST)
    list(APPEND EVFS_FS_SOURCES semihost_fs.c)
endif()

# Shim layers
set(EVFS_SHIM_SOURCES "")
if(CONFIG_EVFS_USE_SHIM_TRACE)
    list(APPEND EVFS_SHIM_SOURCES shim_trace.c)
endif()
if(CONFIG_EVFS_USE_SHIM_JAIL)
    list(APPEND EVFS_SHIM_SOURCES shim_jail.c)
endif()
if(CONFIG_EVFS_USE_SHIM_ROTATE)
    list(APPEND EVFS_SHIM_SOURCES shim_rotate.c)
endif()

# POSIX compatibility layer
set(EVFS_POSIX_SOURCES "")
if(CONFIG_EVFS_POSIX)
    __component_add_include_dirs(posix/include)
    list(APPEND EVFS_POSIX_SOURCES posix/evfs_posix.c)
endif()

__component_add_sources(
    ${EVFS_CORE_SOURCES}
    ${EVFS_UTIL_SOURCES}
    ${EVFS_FS_SOURCES}
    ${EVFS_SHIM_SOURCES}
    ${EVFS_POSIX_SOURCES}
)

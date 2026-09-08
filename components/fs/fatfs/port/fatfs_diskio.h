/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef FS_FAT_DISKIO_H
#define FS_FAT_DISKIO_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "diskio.h"

#define SD_MOUNTPOINT      "/sdcard"
#define SD_PARTITION_ID    "sd:"

#define MMC_MOUNTPOINT      "/mmc"
#define MMC_PARTITION_ID    "mmc:"

#define USB_MOUNTPOINT      "/usb"
#define USB_PARTITION_ID    "usb:"

#define RAM_MOUNTPOINT      "/ramdisk"
#define RAM_PARTITION_ID    "ram:"


#ifdef __cplusplus
 }
#endif


#endif

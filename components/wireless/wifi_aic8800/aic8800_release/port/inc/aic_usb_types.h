/*
 * Copyright (C) 2018-2025 AICSemi Ltd.
 *
 * All Rights Reserved
 *
 * Host-owned USB type definitions.
 * Provides USB constants without platform dependencies.
 *
 * 注意: USB endpoint 号和 vendor/product ID 不在此定义。
 *   - endpoint 号由平台 USB 子系统在运行时动态发现 (参考 Linux
 *     aicwf_usb.c::aicwf_parse_usb 遍历 endpoint descriptor),
 *     不应硬编码为常量。
 *   - vendor/product ID 由平台 USB 驱动注册时使用 (usb_device_id table),
 *     属于平台层职责, 不放在 host 通用头文件中。
 *
 * USB_RX_BUF_COUNT/SIZE 定义在 usb_co.h (与 SDIO 的 sdio_co.h 对应),
 * 不在此重复定义。
 */

#ifndef _AIC_USB_TYPES_H_
#define _AIC_USB_TYPES_H_

#include <stdint.h>

/* USB TX aggregation limits (mirror SDIO defines) */
#define USB_TX_SLIST_MAX        136

/* USB scatter-gather list (parallel to sdio_sg_list) */
typedef struct {
    void     *addr;
    uint32_t  len;
} usb_sg_list;

#endif /* _AIC_USB_TYPES_H_ */

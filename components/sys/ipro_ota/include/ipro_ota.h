/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

/*
 * ipro_ota: the OTA core every transport shares.
 *
 * A transport (BLE, UART zmodem, USB CDC, HTTP) receives a boot2 OTA package
 * and hands it to this component, which stages it in the FW partition's second
 * slot, checks it, and switches the partition table so boot2 installs it on the
 * next boot. Nothing here knows how the bytes arrived.
 *
 *   - ota_pkg_parse():  what a 512-byte package header says (pure, no HAL).
 *   - ota_sink*():      a sequential stream into the slot, with SHA-256.
 *   - ota_validate():   the stream just received against its header.
 *   - ota_validate_flash(): a package already in flash (a transport that
 *                       writes the slot itself, out of order) against its header.
 *   - ota_get_fw_b() / ota_mark_fwb_active(): the slot, and the switch.
 *
 * Staging buffers are malloc'd in begin() and freed in end(), so nothing here
 * occupies RAM at idle. Signed packages: ipro_ota_sig.h.
 */
#ifndef IPRO_OTA_H
#define IPRO_OTA_H

#include <stdint.h>
#include <stdbool.h>

/* OTA package layout - must match tools/create_ota.py and boot2's boot2_process_ota. */
#define OTA_HEADER_SIZE     512
#define OTA_FLASH_BUF       4096

/* ---- package header ---- */

enum {
    OTA_PKG_OK         = 0,
    OTA_PKG_SHORT      = -1,    /* fewer than 512 bytes */
    OTA_PKG_BAD_MAGIC  = -2,    /* not "IPRO_OTA_Ver1.0": a raw .bin, for one */
    OTA_PKG_BAD_TYPE   = -3,    /* neither "XZ  " nor "RAW " (ota_validate*: not XZ) */
    OTA_PKG_BAD_LENGTH = -4,    /* payload length 0, or not what was received */
    OTA_PKG_BAD_SHA    = -5,    /* the payload is not the one the header names */
    OTA_PKG_IO         = -6,    /* flash or memory failed while checking */
};

typedef enum {
    OTA_PKG_TYPE_XZ  = 0,
    OTA_PKG_TYPE_RAW = 1,
} ota_pkg_type_t;

typedef struct {
    ota_pkg_type_t type;
    uint32_t       payload_len;         /* bytes after the header */
    char           hw_version[17];      /* NUL-terminated */
    char           fw_version[17];
    uint8_t        sha256[32];          /* of the payload */
    bool           has_signature;       /* a "GSG1" block: see ipro_ota_sig.h */
} ota_pkg_info_t;

/* Read a header. OTA_PKG_OK, or BAD_MAGIC / BAD_TYPE / BAD_LENGTH (length 0). */
int ota_pkg_parse(const uint8_t hdr[OTA_HEADER_SIZE], ota_pkg_info_t *out);

/* ---- the slot ---- */

/* Read the FW partition's FW-B (slot 1) address/length. 0 on success. */
int  ota_get_fw_b(uint32_t *addr, uint32_t *max_len);

/* Flip the FW entry's activeIndex 0 -> 1 (commit to the inactive PT). 0 on success. */
int  ota_mark_fwb_active(void);

/* ---- a sequential stream into the slot ---- */

/* The sink erases each 4 KB of the slot right before it writes it, so the
 * caller erases nothing and the flash is never busy for a whole slot at once
 * (a 4 KB erase stalls an XIP system, and a wireless link, for its duration). */
#define OTA_SINK_ERASE_AS_WRITTEN   0x1u

/* Sink lifecycle. begin() malloc's the staging buffers + inits SHA256; end()
 * frees them (safe to call on any exit path). ota_sink_begin() is
 * ota_sink_begin_ex(.., 0): call it after erasing FW-B. */
int  ota_sink_begin(uint32_t fw_b_addr, uint32_t fw_b_max);
int  ota_sink_begin_ex(uint32_t fw_b_addr, uint32_t fw_b_max, uint32_t flags);
int  ota_sink(const uint8_t *data, uint32_t n);   /* write FW-B + hash + capture hdr */
int  ota_sink_flush(void);                        /* flush the partial 4KB tail */
void ota_sink_end(void);

uint32_t ota_recv_len(void);    /* bytes received so far */
uint32_t ota_total_len(void);   /* 512 + payload_len, or 0 until the header arrives */
bool     ota_overflowed(void);  /* package exceeded the FW-B slot */

/* Finish SHA256 and validate magic + XZ + length + hash. OTA_PKG_OK, or
 * OTA_PKG_SHORT .. OTA_PKG_BAD_SHA (-1 .. -5, as before). */
int  ota_validate(void);

/* ---- a package a transport wrote to the slot itself ---- */

/* Check the package at @p addr, @p total_len bytes long as received: header,
 * XZ, the length it names, and the SHA-256 of its payload read back from
 * flash. Call it before ota_mark_fwb_active(). OTA_PKG_OK or OTA_PKG_*. */
int  ota_validate_flash(uint32_t addr, uint32_t total_len);

#endif /* IPRO_OTA_H */

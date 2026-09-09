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
 * Transport-agnostic OTA staging core, shared by ota_zmodem (UART) and
 * ota_usb (USB CDC). Receivers stream the OTA package through ota_sink();
 * it is written to FW-B, the 512B header is captured, and the payload is
 * SHA256-hashed. ota_validate() then checks it before ota_mark_fwb_active()
 * commits the apply. Staging buffers are malloc'd in begin() / freed in end(),
 * so nothing here occupies RAM at idle.
 */
#ifndef OTA_COMMON_H
#define OTA_COMMON_H

#include <stdint.h>
#include <stdbool.h>

/* OTA package layout — must match tools/create_ota.py + boot2 boot2_process_ota. */
#define OTA_HEADER_SIZE     512
#define OTA_FLASH_BUF       4096

/* Read the FW partition's FW-B (slot 1) address/length. 0 on success. */
int  ota_get_fw_b(uint32_t *addr, uint32_t *max_len);

/* Flip the FW entry's activeIndex 0 -> 1 (commit to the inactive PT). 0 on success. */
int  ota_mark_fwb_active(void);

/* Sink lifecycle. begin() malloc's the staging buffers + inits SHA256 (call
 * after erasing FW-B); end() frees them (safe to call on any exit path). */
int  ota_sink_begin(uint32_t fw_b_addr, uint32_t fw_b_max);
int  ota_sink(const uint8_t *data, uint32_t n);   /* write FW-B + hash + capture hdr */
int  ota_sink_flush(void);                        /* flush the partial 4KB tail */
void ota_sink_end(void);

uint32_t ota_recv_len(void);    /* bytes received so far */
uint32_t ota_total_len(void);   /* 512 + payload_len, or 0 until the header arrives */
bool     ota_overflowed(void);  /* package exceeded the FW-B slot */

/* Finish SHA256 and validate magic + XZ + length + hash. 0 ok; -1..-5 on
 * (short / magic / type / length / SHA) mismatch. */
int  ota_validate(void);

#endif /* OTA_COMMON_H */

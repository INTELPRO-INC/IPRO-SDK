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
 * IPRO6S FMAC download protocol, loader <-> host. The FMAC FW serves
 * INFO and REBOOT and sends READY after the jump. The host driver keeps
 * a copy of this file; change both together.
 */
#ifndef FMAC_DL_H
#define FMAC_DL_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#define FMAC_DL_REQ_MAGIC     0x4D463649u   /* "I6FM" */
#define FMAC_DL_ACK_MAGIC     0x41463649u   /* "I6FA" */
#define FMAC_DL_PROTO_VER     1u

/* SDIO block; a request is padded to whole blocks */
#define FMAC_DL_BLK           512u
#define FMAC_DL_HDR_LEN       32u
#define FMAC_DL_PAYLOAD_MAX   4096u
#define FMAC_DL_PKT_MAX       4608u

/* OCRAM download window */
#define FMAC_DL_WIN_BASE      0x11000000u
#define FMAC_DL_WIN_SIZE      0x00050000u
#define FMAC_DL_NOCACHE_OFS   0x10000000u

enum fmac_dl_type {
    FMAC_DL_INFO = 1,
    FMAC_DL_WRITE = 2,
    FMAC_DL_VERIFY = 3,
    FMAC_DL_RUN = 4,
    FMAC_DL_REBOOT = 5,
    FMAC_DL_READY = 6,
};

enum fmac_dl_status {
    FMAC_DL_OK = 0,
    FMAC_DL_E_HDR = 1,
    FMAC_DL_E_CRC = 2,
    FMAC_DL_E_RANGE = 3,
    FMAC_DL_E_ALIGN = 4,
    FMAC_DL_E_TYPE = 5,
};

enum fmac_stage {
    FMAC_STAGE_LOADER = 1,
    FMAC_STAGE_FW = 2,
};

enum fmac_bus {
    FMAC_BUS_SDIO = 1,
    FMAC_BUS_USB = 2,
};

/*
 * Request: 32-byte header, then len payload bytes (WRITE only).
 * WRITE:  addr = OCRAM address, crc32 = payload CRC
 * VERIFY: addr, len = OCRAM range
 * RUN:    addr = load address, len = image length, crc32 = image CRC,
 *         arg = entry point
 */
typedef struct {
    uint32_t magic;
    uint16_t type;
    uint16_t flags;
    uint32_t seq;
    uint32_t addr;
    uint32_t len;
    uint32_t crc32;
    uint32_t arg;
    uint32_t rsv;
} fmac_dl_req_t;

/*
 * Ack: type and seq echo the request; READY has seq 0.
 * INFO / READY: value = stage, info[0] = proto ver | loader ver << 16,
 *               info[1] = window base, info[2] = window size,
 *               info[3] = max payload (INFO) or FW version (READY)
 * WRITE / VERIFY / RUN: value = CRC32 the device computed
 */
typedef struct {
    uint32_t magic;
    uint16_t type;
    uint16_t status;
    uint32_t seq;
    uint32_t value;
    uint32_t info[4];
} fmac_dl_ack_t;

/* Image file: this header, then the bytes loaded at load_addr */
#define FMAC_IMG_MAGIC        0x57463649u   /* "I6FW" */
#define FMAC_IMG_HDR_VER      1u

typedef struct {
    uint32_t magic;
    uint16_t hdr_len;
    uint16_t hdr_ver;
    uint32_t load_addr;
    uint32_t len;
    uint32_t entry;
    uint32_t crc32;
    uint32_t fw_ver;
    uint32_t flags;
    char build[32];
} fmac_img_hdr_t;

/*
 * Loader -> FW handoff at the top 4 KB of WRAM. FW startup does not
 * clear it; the FW reads it once.
 */
#define FMAC_HANDOFF_ADDR     0x21077000u
#define FMAC_HANDOFF_MAGIC    0x46483649u   /* "I6HF" */
#define FMAC_HANDOFF_VER      1u
#define FMAC_RFTLV_MAX        2048u

typedef struct {
    uint32_t magic;
    uint16_t ver;
    uint16_t size;
    uint32_t bus;
    uint32_t loader_ver;
    uint32_t jumps;
    uint32_t sdio_blk;
    uint32_t rftlv_len;
    uint32_t rftlv_crc;
    uint32_t rsv[8];
    uint8_t rftlv[FMAC_RFTLV_MAX];
} fmac_handoff_t;

_Static_assert(sizeof(fmac_dl_req_t) == FMAC_DL_HDR_LEN, "req size");
_Static_assert(sizeof(fmac_dl_ack_t) == 32, "ack size");
_Static_assert(FMAC_DL_PKT_MAX % FMAC_DL_BLK == 0 &&
               FMAC_DL_HDR_LEN + FMAC_DL_PAYLOAD_MAX <= FMAC_DL_PKT_MAX,
               "pkt size");
_Static_assert(sizeof(fmac_img_hdr_t) == 64, "img hdr size");
_Static_assert(sizeof(fmac_handoff_t) <= 4096, "handoff size");

#ifndef __KERNEL__
/* Reflected CRC-32 (poly 0xEDB88320); fmac_crc32(0, ...) == zlib crc32 */
uint32_t fmac_crc32(uint32_t crc, const void *buf, uint32_t len);
#endif

#endif

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
 * ipronet_ota.h - IPRO6S Wi-Fi firmware update over ipronet.
 */
#ifndef IPRONET_OTA_H
#define IPRONET_OTA_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Device error codes in ipronet_ota_report_t.dev_err. */
enum {
    IPRONET_OTA_E_STATE = 8888,  /* no session */
    IPRONET_OTA_E_NO_SLOT,       /* no FW slot 1 in the PT */
    IPRONET_OTA_E_HEADER,        /* magic, type, length, chip */
    IPRONET_OTA_E_TOO_BIG,       /* larger than the slot */
    IPRONET_OTA_E_CKSUM,         /* read-back SHA-256 differs */
    IPRONET_OTA_E_GAP,           /* offset ahead of the device */
    IPRONET_OTA_E_SIZE,          /* chunk length, early commit */
    IPRONET_OTA_E_MISC,          /* flash, PT or memory */
};

/* boot2 refused the package after the reboot (dev_err with rc -13). */
enum {
    IPRONET_OTA_E_BOOT2 = 8900,
    IPRONET_OTA_E_BOOT2_MAGIC,   /* 8901 bad package magic */
    IPRONET_OTA_E_BOOT2_HEADER,  /* 8902 type or length */
    IPRONET_OTA_E_BOOT2_SHA,     /* 8903 payload SHA-256 */
    IPRONET_OTA_E_BOOT2_XZ,      /* 8904 XZ stream does not decode */
    IPRONET_OTA_E_BOOT2_TOO_BIG, /* 8905 image larger than FW-A */
    IPRONET_OTA_E_BOOT2_NOT_FW,  /* 8906 not a FW image */
    IPRONET_OTA_E_BOOT2_SECURE,  /* 8907 signature or anti-rollback */
    IPRONET_OTA_E_BOOT2_RSVD,
    IPRONET_OTA_E_BOOT2_READ,    /* 8909 flash read error, still pending */
};

/* Package source: sequential reader; size is header + payload. */
typedef struct {
    uint32_t size;
    /* Returns 1..len bytes; <= 0 is an error. */
    int (*read)(void *ctx, uint8_t *buf, uint32_t len);
    void *ctx;
} ipronet_ota_src_t;

typedef struct {
    uint32_t reboot_window_ms;   /* back on SDIO; default 60000 */
    uint32_t ip_timeout_ms;      /* after reconnect; default 30000 */
    void (*on_progress)(void *arg, uint32_t done, uint32_t total);
    void *arg;
} ipronet_ota_opts_t;

enum {
    IPRONET_OTA_T_XFER,
    IPRONET_OTA_T_COMMIT,
    IPRONET_OTA_T_REBOOT,
    IPRONET_OTA_T_BRINGUP,
    IPRONET_OTA_T_ASSOC,
    IPRONET_OTA_T_IP,
    IPRONET_OTA_T_MAX,
};

typedef struct {
    uint32_t ms[IPRONET_OTA_T_MAX];   /* time per phase */
    uint32_t bytes;
    uint32_t chunks;
    uint32_t retries;
    uint16_t dev_err;        /* IPRONET_OTA_E_* or RNM ack status */
    uint8_t  phase;          /* last IPRONET_OTA_T_* entered */
    uint8_t  reconnected;
    uint8_t  power_cycled;   /* reboot wait needed a rail cycle */
    uint8_t  installed;      /* 1 yes, 0 no, 2 unknown */
} ipronet_ota_report_t;

void ipronet_ota_opts_default(ipronet_ota_opts_t *o);
/* Blocking; needs ipronet_is_ready(); not from ipronet callbacks.
 * Returns IPRONET_OK or IPRONET_ERR_*. */
int ipronet_ota_update(const ipronet_ota_src_t *src,
                       const ipronet_ota_opts_t *opts,
                       ipronet_ota_report_t *rep);

#ifdef __cplusplus
}
#endif
#endif /* IPRONET_OTA_H */

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
 * Transfer buffers shared by the SDIO and USB transports. An image that
 * runs only one transport at a time builds with FMAC_BUS_POOL and defines
 * both pools once, in uncached RAM; the other transport's buffers
 * overlay them.
 */
#ifndef FMAC_BUS_POOL_H
#define FMAC_BUS_POOL_H

#include <stdint.h>
#include "fmac_dl.h"

typedef union {
    uint8_t smid[FMAC_SMID_NBUF][FMAC_DL_PKT_MAX];
    uint8_t usb[FMAC_USB_NBUF][FMAC_DL_PKT_MAX];
} fmac_dnld_pool_t;

typedef union {
    uint8_t smid[FMAC_SMID_UPLD_MAX];
    /* + 4: a whole-packet transfer gets 4 zero bytes, so it ends short */
    uint8_t usb[FMAC_USB_IN_MAX + 4];
} fmac_upld_pool_t;

extern fmac_dnld_pool_t fmac_dnld_pool;
extern fmac_upld_pool_t fmac_upld_pool;

#endif

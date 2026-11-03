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

#ifndef FMAC_HOST_H
#define FMAC_HOST_H

#include <stdint.h>

#define FMAC_FW_VER     3u
/* Capability bits of fmac_info_t.fw_ver (fmac_wire.h) */
#ifdef CONFIG_IPRO6_FMAC_AP
#define FMAC_FW_CAPS    FMAC_FW_CAP_AP
#else
#define FMAC_FW_CAPS    0u
#endif

/* Takes the bus over from the loader and starts the host link task */
int fmac_host_init(void);
/* Address of the RF TLV the loader handed over, or 0 */
uint32_t fmac_rftlv_addr(void);

#endif

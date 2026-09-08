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

#ifndef __SDIO_PORT_IPRO6_H__
#define __SDIO_PORT_IPRO6_H__

/*
 * IPRO6-only port declarations. Kept SEPARATE from sdio_port.h because it pulls
 * ipro6_sdio3.h -> drv_common.h -> platform_common.h, whose platform_get_random
 * prototype collides with the wifi stack's platform_al.h. sdio_port.h is included
 * by chip-agnostic files (e.g. sdiowifi_tx_buf.c) that also include platform_al.h;
 * this header is included only by the IPRO6 transport (sdio_port_ipro6.c /
 * trcver_sdio_ipro6.c), which never includes platform_al.h.
 */

#include "ipro6_sdio3.h"

/* Shared SMID device handle (CSR base + ECLIC IRQ); defined in sdio_port_ipro6.c. */
ipro6_smid_dev_t *sdio_smid_dev(void);

/* Raw device->host upload (no sdio_top_msg framing); copies `data`, frees on
 * completion. For the data-plane bring-up / loopback test only. */
int ipro_sdio_send_raw(const void *data, uint16_t len);

#endif /* __SDIO_PORT_IPRO6_H__ */

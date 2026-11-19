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

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* UART1 pads and baud rate of the RCP link the BR uses (the one the Spinel
 * UART opened with, or the stored/build-default selection before OT start).
 * The selection itself is made with the `rcp_select` shell command. */
void br_rcp_active_link(uint8_t *tx_pin, uint8_t *rx_pin, uint32_t *baud);

#ifdef __cplusplus
}
#endif

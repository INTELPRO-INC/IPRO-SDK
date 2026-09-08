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

#ifndef __HELPER_SOCKET_H__
#define __HELPER_SOCKET_H__

#include <stdint.h>

/* Transport write callback: set to CDC or UART send function */
typedef int (*iproc_write_fn_t)(const void *buf, uint32_t len);

void iproc_set_write_fn(iproc_write_fn_t fn);

/* Process one complete IPROC packet received from any transport */
void iproc_process_bulk(uint8_t *data, uint32_t len);

/* UART server task entry (kept for fallback) */
void socket_server_entry(void);

int helper_socket_cli_init(void);

#endif /* __HELPER_SOCKET_H__ */

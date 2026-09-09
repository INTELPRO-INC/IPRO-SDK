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

#ifndef __SIGMA_DUT_H__
#define __SIGMA_DUT_H__

#include <stdint.h>

#define SIGMA_DUT_DEFAULT_PORT  9000

/**
 * Start the sigma-dut CAPI agent.
 *
 * Creates a FreeRTOS task that listens on the given TCP port for
 * WFA CAPI commands from the UCC test bed.
 *
 * @param port  TCP listen port (0 = use default 9000)
 * @return 0 on success, -1 on error
 */
int sigma_dut_start(uint16_t port);

/**
 * Stop the sigma-dut CAPI agent.
 *
 * Closes the listen socket and terminates the server task.
 *
 * @return 0 on success, -1 if not running
 */
int sigma_dut_stop(void);

#endif /* __SIGMA_DUT_H__ */

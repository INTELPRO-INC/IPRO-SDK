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

#ifndef __BZ_9361_REG_ACCESS_H__
#define __BZ_9361_REG_ACCESS_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef CONFIG_FPGA
#ifdef CONFIG_RF_AD9361
// 9361 related
void spi_9361_wr_cr(uint32_t, uint32_t);
uint32_t spi_9361_rd_cr(uint32_t);
#endif
#endif

#endif

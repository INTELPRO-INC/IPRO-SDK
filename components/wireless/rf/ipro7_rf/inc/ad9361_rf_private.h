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

#include <stdbool.h>
#ifndef _AD9361_RF_PRIVATE_H_
#define _AD9361_RF_PRIVATE_H_

//  9361 related
#ifdef FPGA
#ifdef RFDTB9361
bool spi_9361_chk_done(uint32_t, uint8_t, uint32_t);
bool spi_9361_wait_cal_done(uint32_t, uint8_t, uint32_t, uint32_t, const char*);
void spi_9361_set_ch(uint32_t ch_freq);
void spi_9361_wr_le_freq_table(void);
void spi_9361_init(void);
#endif
#endif


#endif

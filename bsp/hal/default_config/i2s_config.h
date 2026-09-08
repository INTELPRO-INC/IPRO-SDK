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

#ifndef _I2S_CONFIG_H
#define _I2S_CONFIG_H

#define I2S_ADUIO_PLL_DEFAULT AUDIO_PLL_12288000_HZ
#define I2S_DATA_ENDIAN       I2S_DATA_ENDIAN_MSB
#define I2S_MONO_CHANNEL      I2S_RX_MONO_MODE_RIGHT_CHANNEL
#define I2S_LR_EXCHANGE       DISABLE /*The position of L/R channel data within each entry is exchanged if enabled*/
#define I2S_FS_INVERT         DISABLE
#define I2S_BCLK_INVERT       DISABLE

#endif
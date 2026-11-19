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

/**
 ****************************************************************************************
 *
 * @file mac_chan.h
 *
 * @brief Channel flags, channel definition and operating channel types.
 *
 ****************************************************************************************
 */

#ifndef _MAC_CHAN_H_
#define _MAC_CHAN_H_

#include <stdint.h>

/// Channel Flag
enum mac_chan_flags
{
    /// Cannot initiate radiation on this channel
    CHAN_NO_IR = 0x0001,
    /// Channel is not allowed
    CHAN_DISABLED = 0x0002,
    /// Radar detection required on this channel
    CHAN_RADAR = 0x0004,
    /// Can be primary channel of HT40- operating channel
    CHAN_HT40M = 0x0008,
    /// Can be primary channel of HT40+ operating channel
    CHAN_HT40P = 0x0010,
    /// Can be primary channel of VHT80 (1st position 10/70) operating channel
    CHAN_VHT80_10_70 = 0x0020,
    /// Can be primary channel of VHT80 (2nd position 30/50) operating channel
    CHAN_VHT80_30_50 = 0x0040,
    /// Can be primary channel of VHT80 (3rd position 50/30) operating channel
    CHAN_VHT80_50_30 = 0x0080,
    /// Can be primary channel of VHT80 (4th position 70/10) operating channel
    CHAN_VHT80_70_10 = 0x0100,
};

/// Primary Channel definition
struct mac_chan_def
{
    /// Frequency of the channel (in MHz)
    uint16_t freq;
    /// RF band (@ref mac_chan_band)
    uint8_t band;
    /// Max transmit power allowed on this channel (dBm)
    int8_t tx_power;
    /// Additional information (@ref mac_chan_flags)
    uint16_t flags;
};

/// Operating Channel
struct mac_chan_op
{
    /// Band (@ref mac_chan_band)
    uint8_t band;
    /// Channel type (@ref mac_chan_bandwidth)
    uint8_t type;
    /// Frequency for Primary 20MHz channel (in MHz)
    uint16_t prim20_freq;
    /// Frequency center of the contiguous channel or center of Primary 80+80 (in MHz)
    uint16_t center1_freq;
    /// Frequency center of the non-contiguous secondary 80+80 (in MHz)
    uint16_t center2_freq;
    /// Additional information (@ref mac_chan_flags)
    uint16_t flags;
    /// Max transmit power allowed on this channel (dBm)
    int8_t tx_power;
};

#endif // _MAC_CHAN_H_

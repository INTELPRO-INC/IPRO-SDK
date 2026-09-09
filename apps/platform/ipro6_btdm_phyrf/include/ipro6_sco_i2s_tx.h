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

#ifndef IPRO6_SCO_I2S_TX_H
#define IPRO6_SCO_I2S_TX_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t input_packets;
    uint32_t input_bytes;
    uint32_t input_drops;
    uint32_t dma_frames;
    uint32_t dma_underruns;
    uint32_t dma_errors;
    uint32_t dma_callback_ch[8];
    uint32_t dma_lli_changes;
    uint32_t dma_lli_repeats;
    uint32_t dma_last_lli;
    uint32_t tx_hash;
    uint32_t ring_bytes;
    uint32_t sample_rate_hz;
    uint16_t dma_bytes;
    uint8_t channels;
    uint8_t a2dp_active;
    uint8_t sco_active;
    uint8_t tone_active;
    uint8_t primed;
    uint8_t dma_channel;
} ipro6_sco_i2s_stats_t;

int ipro6_sco_i2s_init(void);
int ipro6_sco_i2s_configure(uint32_t sample_rate_hz, uint8_t channels);
void ipro6_sco_i2s_set_a2dp_active(bool active);
void ipro6_sco_i2s_set_sco_active(bool active);
bool ipro6_sco_i2s_write(const uint8_t *pcm, size_t len);
void ipro6_sco_i2s_set_tone(bool enable);
void ipro6_sco_i2s_get_stats(ipro6_sco_i2s_stats_t *stats);

#endif

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

#ifndef IPRO_ISO_CONTENT_VALIDATOR_H
#define IPRO_ISO_CONTENT_VALIDATOR_H

#include <stdbool.h>
#include <stdint.h>

struct iso_content_stats {
	uint32_t configured;
	uint32_t submitted;
	uint32_t checked;
	uint32_t decode_ok;
	uint32_t decode_error;
	uint32_t queue_drop;
	uint32_t pcm_samples;
	uint32_t pcm_nonzero;
	uint32_t pcm_crc;
	uint32_t pcm_positive_crossings;
	uint32_t pcm_mean_abs;
	uint32_t tone_hz_milli;
	uint32_t pcm_active_frames;
	uint32_t pcm_silent_frames;
	uint32_t pcm_active_samples;
	uint32_t pcm_active_positive_crossings;
	uint32_t pcm_active_mean_abs;
	uint32_t active_tone_hz_milli;
	int16_t pcm_min;
	int16_t pcm_max;
};

int iso_content_validator_configure(uint32_t sample_rate,
				    uint32_t frame_duration_us,
				    uint16_t octets_per_frame);
void iso_content_validator_reset(void);
bool iso_content_validator_check(const uint8_t *data, uint16_t len);
void iso_content_validator_wait_idle(uint32_t timeout_ms);
void iso_content_validator_get_stats(struct iso_content_stats *stats);

#endif

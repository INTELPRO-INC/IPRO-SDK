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

#include "iso_content_validator.h"

#include <lc3.h>
#include <string.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "ipro_log.h"
#include "utils_crc.h"

#define LOG_TAG "ISO_CONTENT"
#define ISO_CONTENT_MAX_SAMPLES 480U
#define ISO_CONTENT_MAX_OCTETS 120U
#define ISO_CONTENT_QUEUE_DEPTH 16U
#define ISO_CONTENT_TASK_STACK 8192U
#define ISO_CONTENT_ACTIVE_MEAN_ABS_MIN 512U
#define ISO_CONTENT_ACTIVE_PEAK_TO_PEAK_MIN 2048

struct iso_content_frame {
	uint16_t len;
	uint8_t data[ISO_CONTENT_MAX_OCTETS];
};

static struct {
	lc3_decoder_mem_48k_t decoder_mem;
	lc3_decoder_t decoder;
	uint32_t sample_rate;
	uint32_t frame_duration_us;
	uint16_t octets_per_frame;
	int16_t pcm[ISO_CONTENT_MAX_SAMPLES];
	struct iso_content_stats stats;
	QueueHandle_t queue;
	TaskHandle_t task;
	volatile uint32_t processing;
	uint64_t pcm_abs_sum;
	uint64_t pcm_active_abs_sum;
	int16_t previous_pcm;
	bool have_previous_pcm;
	int16_t previous_active_pcm;
	bool have_previous_active_pcm;
} s_validator;

static void iso_content_decode_frame(const struct iso_content_frame *frame)
{
	int samples;
	int ret;
	uint64_t frame_abs_sum = 0U;
	int16_t frame_min = INT16_MAX;
	int16_t frame_max = INT16_MIN;
	bool active;

	s_validator.stats.checked++;
	ret = lc3_decode(s_validator.decoder, frame->data, frame->len,
			 LC3_PCM_FORMAT_S16, s_validator.pcm, 1);
	if (ret != 0) {
		s_validator.stats.decode_error++;
		if (s_validator.stats.decode_error <= 4U) {
			IPRO_LOGE(LOG_TAG, "LC3 decode rejected frame %lu: ret=%d len=%u",
				  (unsigned long)s_validator.stats.checked, ret, frame->len);
		}
		return;
	}

	s_validator.stats.decode_ok++;
	samples = lc3_frame_samples(s_validator.frame_duration_us,
				    s_validator.sample_rate);
	s_validator.stats.pcm_crc = utils_crc32_accumulate(
		s_validator.stats.pcm_crc, s_validator.pcm,
		(uint32_t)samples * sizeof(s_validator.pcm[0]));
	for (int i = 0; i < samples; i++) {
		int32_t magnitude = s_validator.pcm[i];

		if (magnitude < 0) {
			magnitude = -magnitude;
		}
		frame_abs_sum += (uint32_t)magnitude;
		if (s_validator.pcm[i] < frame_min) {
			frame_min = s_validator.pcm[i];
		}
		if (s_validator.pcm[i] > frame_max) {
			frame_max = s_validator.pcm[i];
		}
	}
	active = samples > 0 &&
		(frame_abs_sum / (uint32_t)samples) >=
			ISO_CONTENT_ACTIVE_MEAN_ABS_MIN &&
		((int32_t)frame_max - (int32_t)frame_min) >=
			ISO_CONTENT_ACTIVE_PEAK_TO_PEAK_MIN;
	if (active) {
		s_validator.stats.pcm_active_frames++;
		s_validator.stats.pcm_active_samples += (uint32_t)samples;
		s_validator.pcm_active_abs_sum += frame_abs_sum;
	} else {
		s_validator.stats.pcm_silent_frames++;
		s_validator.have_previous_active_pcm = false;
	}
	for (int i = 0; i < samples; i++) {
		int16_t sample = s_validator.pcm[i];
		int32_t magnitude = sample;

		if (magnitude < 0) {
			magnitude = -magnitude;
		}
		s_validator.pcm_abs_sum += (uint32_t)magnitude;
		if (s_validator.have_previous_pcm &&
		    s_validator.previous_pcm <= 0 && sample > 0) {
			s_validator.stats.pcm_positive_crossings++;
		}
		s_validator.previous_pcm = sample;
		s_validator.have_previous_pcm = true;
		if (active) {
			if (s_validator.have_previous_active_pcm &&
			    s_validator.previous_active_pcm <= 0 && sample > 0) {
				s_validator.stats.pcm_active_positive_crossings++;
			}
			s_validator.previous_active_pcm = sample;
			s_validator.have_previous_active_pcm = true;
		}

		if (sample != 0) {
			s_validator.stats.pcm_nonzero++;
		}
		if (sample < s_validator.stats.pcm_min) {
			s_validator.stats.pcm_min = sample;
		}
		if (sample > s_validator.stats.pcm_max) {
			s_validator.stats.pcm_max = sample;
		}
	}
	s_validator.stats.pcm_samples += (uint32_t)samples;
}

static void iso_content_task(void *arg)
{
	struct iso_content_frame frame;

	(void)arg;
	for (;;) {
		if (xQueueReceive(s_validator.queue, &frame, portMAX_DELAY) == pdTRUE) {
			s_validator.processing = 1U;
			iso_content_decode_frame(&frame);
			s_validator.processing = 0U;
		}
	}
}

int iso_content_validator_configure(uint32_t sample_rate,
				    uint32_t frame_duration_us,
				    uint16_t octets_per_frame)
{
	int samples;

	if (!LC3_CHECK_SR_HZ(sample_rate) ||
	    !LC3_CHECK_DT_US(frame_duration_us) || octets_per_frame == 0U ||
	    octets_per_frame > ISO_CONTENT_MAX_OCTETS) {
		return -1;
	}

	samples = lc3_frame_samples(frame_duration_us, sample_rate);
	if (samples <= 0 || samples > (int)ISO_CONTENT_MAX_SAMPLES) {
		return -2;
	}

	s_validator.sample_rate = sample_rate;
	s_validator.frame_duration_us = frame_duration_us;
	s_validator.octets_per_frame = octets_per_frame;
	if (s_validator.queue == NULL) {
		s_validator.queue = xQueueCreate(ISO_CONTENT_QUEUE_DEPTH,
					 sizeof(struct iso_content_frame));
		if (s_validator.queue == NULL) {
			return -3;
		}
	}
	if (s_validator.task == NULL) {
		if (xTaskCreate(iso_content_task, "iso_lc3", ISO_CONTENT_TASK_STACK,
				NULL, 10, &s_validator.task) != pdPASS) {
			return -4;
		}
	}
	s_validator.decoder = lc3_setup_decoder(frame_duration_us, sample_rate, 0,
						  &s_validator.decoder_mem);
	if (s_validator.decoder == NULL) {
		return -5;
	}

	iso_content_validator_reset();
	s_validator.stats.configured = 1U;
	IPRO_LOGI(LOG_TAG, "LC3 validator ready: %lu Hz, %lu us, %u B, %d samples",
		  (unsigned long)sample_rate, (unsigned long)frame_duration_us,
		  octets_per_frame, samples);
	return 0;
}

void iso_content_validator_reset(void)
{
	uint32_t configured = s_validator.decoder != NULL ? 1U : 0U;

	memset(&s_validator.stats, 0, sizeof(s_validator.stats));
	s_validator.stats.configured = configured;
	s_validator.stats.pcm_min = INT16_MAX;
	s_validator.stats.pcm_max = INT16_MIN;
	s_validator.pcm_abs_sum = 0U;
	s_validator.pcm_active_abs_sum = 0U;
	s_validator.previous_pcm = 0;
	s_validator.have_previous_pcm = false;
	s_validator.previous_active_pcm = 0;
	s_validator.have_previous_active_pcm = false;
	if (configured != 0U) {
		xQueueReset(s_validator.queue);
		s_validator.processing = 0U;
		s_validator.decoder = lc3_setup_decoder(s_validator.frame_duration_us,
						  s_validator.sample_rate, 0,
						  &s_validator.decoder_mem);
	}
}

bool iso_content_validator_check(const uint8_t *data, uint16_t len)
{
	struct iso_content_frame frame;

	if (s_validator.decoder == NULL || data == NULL ||
	    len != s_validator.octets_per_frame) {
		return false;
	}

	frame.len = len;
	memcpy(frame.data, data, len);
	s_validator.stats.submitted++;
	if (xQueueSend(s_validator.queue, &frame, 0U) != pdTRUE) {
		s_validator.stats.queue_drop++;
		return false;
	}
	return true;
}

void iso_content_validator_wait_idle(uint32_t timeout_ms)
{
	TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(timeout_ms);

	while ((uxQueueMessagesWaiting(s_validator.queue) != 0U ||
		s_validator.processing != 0U) &&
	       (int32_t)(deadline - xTaskGetTickCount()) > 0) {
		vTaskDelay(pdMS_TO_TICKS(1));
	}
}

void iso_content_validator_get_stats(struct iso_content_stats *stats)
{
	if (stats != NULL) {
		*stats = s_validator.stats;
		if (stats->pcm_samples != 0U) {
			stats->pcm_mean_abs = (uint32_t)(
				(s_validator.pcm_abs_sum + stats->pcm_samples / 2U) /
				stats->pcm_samples);
			stats->tone_hz_milli = (uint32_t)(
				((uint64_t)stats->pcm_positive_crossings *
				 s_validator.sample_rate * 1000U +
				 stats->pcm_samples / 2U) /
				stats->pcm_samples);
		}
		if (stats->pcm_active_samples != 0U) {
			stats->pcm_active_mean_abs = (uint32_t)(
				(s_validator.pcm_active_abs_sum +
				 stats->pcm_active_samples / 2U) /
				stats->pcm_active_samples);
			stats->active_tone_hz_milli = (uint32_t)(
				((uint64_t)stats->pcm_active_positive_crossings *
				 s_validator.sample_rate * 1000U +
				 stats->pcm_active_samples / 2U) /
				stats->pcm_active_samples);
		}
	}
}

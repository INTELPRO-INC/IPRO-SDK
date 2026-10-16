/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HEADSET_AUDIO_H
#define HEADSET_AUDIO_H

#include <stdint.h>
#include <stdbool.h>
#include <zephyr/net_buf.h>
#include <zephyr/bluetooth/iso.h>

/* Codec configuration negotiated on the sink ASE. */
int audio_configure(uint32_t sample_rate, uint32_t frame_dur_us, uint16_t octets_per_frame);
/* Streaming state: opens/closes the speaker path. */
int audio_start(void);
void audio_stop(void);
/* One SDU from the CIS (any context); decoded on the audio thread. */
void audio_push(const struct bt_iso_recv_info *info, struct net_buf *buf);
void audio_set_volume(uint8_t volume, bool mute);

#endif

/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Force-included (-include) ahead of the upstream main.c when
 * CONFIG_BAP_BROADCAST_HIGH_RELIABILITY is set: the sample only knows the
 * low-latency "_1" presets (RTN 2, 10 ms latency); this swaps the one it
 * selects for its "_2" high-reliability twin (RTN 4, 60 ms latency, same
 * codec configuration) without touching the upstream source.
 */
#include <zephyr/bluetooth/audio/bap_lc3_preset.h>

#undef BT_BAP_LC3_BROADCAST_PRESET_16_2_1
#define BT_BAP_LC3_BROADCAST_PRESET_16_2_1 BT_BAP_LC3_BROADCAST_PRESET_16_2_2
#undef BT_BAP_LC3_BROADCAST_PRESET_24_2_1
#define BT_BAP_LC3_BROADCAST_PRESET_24_2_1 BT_BAP_LC3_BROADCAST_PRESET_24_2_2
#undef BT_BAP_LC3_BROADCAST_PRESET_48_2_1
#define BT_BAP_LC3_BROADCAST_PRESET_48_2_1 BT_BAP_LC3_BROADCAST_PRESET_48_2_2

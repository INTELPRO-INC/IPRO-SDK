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
 * @file sensor_src.h
 * @brief Where samples come from: int16, channels interleaved, one rate.
 *
 *  sim_vib .. sim_sound  simulated sensors (sensor_sim.h): a vibrating
 *                        machine, a worn IMU, room climate, a motor's phase
 *                        currents, sounds - each with states, chosen with SIM
 *                        or key SW4, and parameters set with SIMCFG.
 *  mic                   the Reef's PDM microphone, 16 kHz, 100 Hz high-pass, +38 dB.
 *
 * Each is an entry of the platform's source registry (ipro_ai_source.h). A
 * real sensor joins by adding an entry to the table in sensor_src.c (an
 * I2C/SPI part through ipro_ai_source_sampled_register, a stream through
 * ipro_ai_source_ring_register). Only the sensor task calls these.
 */
#ifndef SENSOR_SRC_H
#define SENSOR_SRC_H

#include <stdbool.h>
#include <stdint.h>

#include "ipro_ai_source.h"
#include "sensor_sim.h"

/* the first SIM_TYPES ids are sensor_sim.h's types, in its order */
typedef enum {
    SRC_SIM_VIB = 0, SRC_SIM_IMU, SRC_SIM_ENV, SRC_SIM_MOTOR, SRC_SIM_SOUND,
    SRC_MIC, SRC_COUNT
} src_id_t;

typedef struct {
    const char *name;
    uint32_t    rate;             /* samples per second per channel */
    uint32_t    channels;
    float       lsb_per_unit;
    const char *unit;
} src_info_t;

void src_init(void);
const src_info_t *src_info(src_id_t id);
/** Source id for a name, or -1. */
int  src_find(const char *name);
bool src_is_sim(src_id_t id);

int      src_open(src_id_t id);
void     src_close(void);
bool     src_is_open(void);
src_id_t src_current(void);
/** Up to max_frames frames (channels interleaved); waits up to about 10 ms. */
uint32_t src_read(int16_t *dst, uint32_t max_frames);

/** The current simulated source's state by name; -1 for an unknown name or
 *  a source that is not simulated. */
int         src_sim_set(const char *state);
void        src_sim_next(void);
const char *src_sim_state(void);              /* "-" when not simulated */
const sim_params_t *src_sim_params(void);     /* NULL when not simulated */
/** New parameters for the current simulated source; *reopen says the rate
 *  or channel count changed, so the caller must reopen it. */
int src_sim_configure(const sim_params_t *p, bool *reopen);

/** The open source's counters (frames, losses, stalls); -1 when none is open. */
int src_stats(ipro_ai_source_stats_t *st);

#endif /* SENSOR_SRC_H */

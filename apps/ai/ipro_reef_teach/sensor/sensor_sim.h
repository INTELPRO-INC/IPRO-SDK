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
 * @file sensor_sim.h
 * @brief Simulated sensors, so the whole Sensor Teach flow runs before a
 *        customer's sensor is wired in. Pure C (math.h only): the host test
 *        compiles the same file to check every simulated state can be told
 *        apart (test_host/check_sim.py).
 *
 *  type       channels                         default rate  states
 *  sim_vib    x, y, z acceleration (g)          1 kHz        off normal imbalance misalign bearing looseness
 *  sim_imu    ax ay az (g), gx gy gz (500 dps)  100 Hz       still walk run stairs shake fall
 *  sim_env    temperature, pressure, humidity   10 Hz        normal heating cooling_fail leak door_open sensor_fault
 *  sim_motor  phase currents A, B, C (A)        2 kHz        off normal overload phase_loss broken_bar bearing unbalanced
 *  sim_sound  pressure (full scale)             8 kHz        quiet hum alarm knock hiss voice
 *
 * Parameters: rate and channel count (the first n channels), noise and drift
 * as multiples of the default, severity (how strong the state's signature
 * is, 0..1) and ramp (seconds over which a new state grows from nothing to
 * that severity - a fault that develops).
 */
#ifndef SENSOR_SIM_H
#define SENSOR_SIM_H

#include <stdint.h>

typedef enum { SIM_VIB = 0, SIM_IMU, SIM_ENV, SIM_MOTOR, SIM_SOUND, SIM_TYPES } sim_type_t;

#define SIM_MAX_CH     6u
#define SIM_MAX_STATES 8u

typedef struct {
    const char *name;
    uint32_t    rate, min_rate, max_rate;       /* Hz */
    uint32_t    channels;                       /* all of them */
    float       lsb_per_unit;
    const char *unit;
    uint32_t    n_states;
    const char *states[SIM_MAX_STATES];
} sim_type_info_t;

typedef struct {
    uint32_t rate;                              /* 0: the type's default */
    uint32_t channels;                          /* 0: all */
    float    noise;                             /* x default */
    float    drift;                             /* x default */
    float    severity;                          /* 0..1 */
    float    ramp_s;                            /* 0: at once */
} sim_params_t;

/** noise 1, drift 1, severity 1, no ramp, the type's rate and channels */
sim_params_t sim_default_params(void);
const sim_type_info_t *sim_type_info(sim_type_t t);
/** Type id for a name, or -1. */
int sim_find(const char *name);

/** Start a type with parameters (NULL: defaults; clamped to what it
 *  supports), in state 1 - the first one past idle. */
void sim_start(sim_type_t t, const sim_params_t *p, uint32_t seed);
const sim_params_t *sim_params(void);
sim_type_t sim_type(void);

int         sim_set_state(const char *name);   /* -1 for a name the type lacks */
void        sim_next_state(void);
const char *sim_state(void);
float       sim_severity_now(void);            /* with the ramp applied */

/** frames x channels int16, interleaved, continuing the signal. */
void sim_generate(int16_t *dst, uint32_t frames);

#endif /* SENSOR_SIM_H */

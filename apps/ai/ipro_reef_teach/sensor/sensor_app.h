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
 * @file sensor_app.h
 * @brief The sensor task: one source into a ring, streaming to Studio, and
 *        on-board inference every quarter second when a model is deployed.
 *
 * Requests are queued to the task, which owns the source and the ring; the
 * replies and stream go out through the sink (the CDC link). Lines:
 *   READY <model|none> <classes> <src> <rate> <ch> <lsb per unit> <unit> <sim state> <labels|->
 *   SRC <name> <rate> <ch> <lsb per unit> <unit>      SIM <state>
 *   SRCS <name> ..        SIMS <type> <state> <state> ..   (SIMS none off a simulated source)
 *   SIMCFG <type> rate <Hz> ch <n> noise <%> drift <%> severity <%> ramp <s>
 *   STREAM ON|OFF, then DATA <bytes> + int16 frames (channels interleaved)
 *   LIVE ON|OFF, then RES <ms> <class> <p0> .. <pn-1> (probabilities x 1000; class -1
 *   is UNKNOWN: the best class scored under the floor the model carries)
 */
#ifndef SENSOR_APP_H
#define SENSOR_APP_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*sa_line_fn)(const char *line);

#define SA_SET_RATE     1u
#define SA_SET_CH       2u
#define SA_SET_NOISE    4u
#define SA_SET_DRIFT    8u
#define SA_SET_SEVERITY 16u
#define SA_SET_RAMP     32u

typedef struct {
    uint32_t set;                 /* SA_SET_*: the fields given */
    uint32_t rate, channels, noise_pct, drift_pct, severity_pct, ramp_s;
} sa_simcfg_t;
typedef void (*sa_bin_fn)(const char *header, const void *data, uint32_t bytes);

int  sensor_app_init(void);
void sensor_app_set_sink(sa_line_fn line, sa_bin_fn bin);

int sensor_app_hello(void);
int sensor_app_src(const char *name);
int sensor_app_sim(const char *state);
int sensor_app_stream(bool on);
int sensor_app_live(bool on);
int sensor_app_srcs(void);
int sensor_app_sims(void);
int sensor_app_simcfg(const sa_simcfg_t *c);
/** Stop the source and inference before the model in flash is rewritten. */
int sensor_app_pause(void);
/** Start again after a pause that did not end in a restart (a refused DEL or ROLLBACK). */
int sensor_app_resume(void);

void sensor_app_set_print(bool on);
void sensor_app_print_status(void);

#endif /* SENSOR_APP_H */

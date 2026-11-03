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
 * @file reef_multi.c
 * @brief `multi`: two models through the IPRO Edge AI runtime at once - the
 *        measurement behind docs/edge_ai/04 §5 (multi-model scheduling).
 *
 * In voice mode the voiceprint trunk is loaded; this opens the Sensor
 * model from its slot beside it and runs both:
 *
 *   1. each alone, recording every output (the reference);
 *   2. both at once from two tasks for n runs each, every output compared
 *      byte for byte with its reference, latencies and the runtime's lock
 *      waits counted.
 *
 * The runtime serialises the NPU (one context, docs/edge_ai/02 §3), so
 * "at once" means interleaved runs: what this measures is that each model
 * keeps its own weights and state between the other's runs, and what one
 * costs the other in latency.
 */
#include <generated/autoconf.h>

#ifdef CONFIG_SHELL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "hal_mtimer.h"
#include "ipro_ai.h"
#include "ipro_ai_store.h"
#include "shell.h"

#include "reef_mode.h"
#include "sensor_front.h"
#include "sensor_model.h"
#include "spk_model.h"

#define INPUTS      4u
#define OUT_MAX     8192u
#define TASK_WORDS  1024u

typedef struct {
    const char       *name;
    int             (*run)(const uint8_t *in, uint8_t *out, uint32_t *bytes, uint32_t *us);
    uint32_t          in_bytes;
    uint8_t          *in[INPUTS];
    uint8_t          *ref[INPUTS];
    uint32_t          ref_bytes;
    uint32_t          n, mismatches, failures;
    uint64_t          us_sum;
    uint32_t          us_max, us_alone;
    SemaphoreHandle_t done;
} lane_t;

static ipro_ai_model_t *s_sensor;

static int run_voice(const uint8_t *in, uint8_t *out, uint32_t *bytes, uint32_t *us)
{
    return spk_model_run_raw(in, SPK_MODEL_FRAMES * 80u, out, OUT_MAX, bytes, us);
}

static int run_sensor(const uint8_t *in, uint8_t *out, uint32_t *bytes, uint32_t *us)
{
    ipro_ai_tensor_info_t ti;
    const uint8_t *o = NULL;
    /* a runtime-owned input: fill it, run on it (the handle is this lane's alone) */
    memcpy(ipro_ai_input_buffer(s_sensor), in, SF_IMG_BYTES);
    const uint64_t t0 = mtimer_get_time_us();
    const int rc = ipro_ai_invoke(s_sensor, NULL, &o);
    *us = (uint32_t)(mtimer_get_time_us() - t0);
    if (rc != IPRO_AI_OK) {
        return -1;
    }
    ipro_ai_tensor_get_info(s_sensor, IPRO_AI_OUTPUT, 0, &ti);
    memcpy(out, o, ti.bytes);
    *bytes = ti.bytes;
    return 0;
}

static void lane_task(void *arg)
{
    lane_t *l = arg;
    uint8_t *out = pvPortMalloc(OUT_MAX);

    if (!out) {
        l->failures = l->n;                        /* reported, not passed off as 0 differences */
    }
    for (uint32_t i = 0; out && i < l->n; i++) {
        uint32_t bytes = 0, us = 0;
        const uint32_t k = i % INPUTS;
        if (l->run(l->in[k], out, &bytes, &us) != 0) {
            l->failures++;
            continue;
        }
        l->mismatches += bytes != l->ref_bytes || memcmp(out, l->ref[k], bytes) != 0;
        l->us_sum += us;
        l->us_max = us > l->us_max ? us : l->us_max;
    }
    vPortFree(out);
    xSemaphoreGive(l->done);
    vTaskDelete(NULL);
}

/* fixed inputs: a ramp, alternating rails, a pattern per index */
static void fill(uint8_t *p, uint32_t bytes, uint32_t k)
{
    for (uint32_t i = 0; i < bytes; i++) {
        p[i] = (uint8_t)(k == 0 ? i * 7u : k == 1 ? ((i / 97u) & 1u ? 0xFFu : 0u)
                                         : (i * 2654435761u >> (8u + k)));
    }
}

static int lane_init(lane_t *l)
{
    for (uint32_t k = 0; k < INPUTS; k++) {
        l->in[k] = pvPortMalloc(l->in_bytes);
        l->ref[k] = pvPortMalloc(OUT_MAX);
        if (!l->in[k] || !l->ref[k]) {
            return -1;
        }
        fill(l->in[k], l->in_bytes, k);
        uint32_t us = 0;
        if (l->run(l->in[k], l->ref[k], &l->ref_bytes, &us) != 0) {
            return -1;
        }
        l->us_alone = us > l->us_alone ? us : l->us_alone;
    }
    l->done = xSemaphoreCreateBinary();
    return l->done ? 0 : -1;
}

static void lane_free(lane_t *l)
{
    for (uint32_t k = 0; k < INPUTS; k++) {
        vPortFree(l->in[k]);
        vPortFree(l->ref[k]);
    }
    if (l->done) {
        vSemaphoreDelete(l->done);
    }
}

static int open_sensor(void)
{
    ipro_ai_blob_t found;
    ipro_ai_inm_header_t h;

    if (ipro_ai_store_open(sensor_model_slot(), &found) != 0 || found.state == IPRO_AI_SLOT_NONE ||
        ipro_ai_inm_peek(found.data, found.bytes, &h) != IPRO_AI_OK) {
        printf("no Sensor model in its slot\r\n");
        return -1;
    }
    const ipro_ai_model_cfg_t mc = { .inm = found.data, .max_bytes = h.bytes,
                                     .input_bytes = SF_IMG_BYTES, .name = "multi-sensor" };
    const int rc = ipro_ai_model_open(&mc, &s_sensor);
    if (rc != IPRO_AI_OK) {
        printf("sensor model: %s\r\n", ipro_ai_strerror(rc));
        return -1;
    }
    return 0;
}

static int cmd_multi(int argc, char **argv)
{
    const long req = argc > 1 ? strtol(argv[1], NULL, 10) : 200;
    const uint32_t n = req > 0 && req <= 100000 ? (uint32_t)req : 0u;
    lane_t v = { .name = "voice trunk", .run = run_voice, .in_bytes = SPK_MODEL_FRAMES * 80u, .n = n };
    lane_t s = { .name = "sensor", .run = run_sensor, .in_bytes = SF_IMG_BYTES, .n = n };
    ipro_ai_runtime_info_t before, after;

    if (reef_mode_current() != REEF_MODE_VOICE || !spk_model_ready()) {
        printf("run in voice mode with a voiceprint model loaded\r\n");
        return -1;
    }
    if (n == 0 || open_sensor() != 0) {
        return -1;
    }
    const uint32_t heap = (uint32_t)xPortGetFreeHeapSize();
    int rc = lane_init(&v) | lane_init(&s);
    if (rc == 0) {
        ipro_ai_get_runtime_info(&before);
        const uint64_t t0 = mtimer_get_time_us();
        const BaseType_t cv = xTaskCreate(lane_task, "multi_v", TASK_WORDS, &v, configMAX_PRIORITIES - 6, NULL);
        const BaseType_t cs = xTaskCreate(lane_task, "multi_s", TASK_WORDS, &s, configMAX_PRIORITIES - 6, NULL);
        if (cv == pdPASS) {
            xSemaphoreTake(v.done, portMAX_DELAY);
        }
        if (cs == pdPASS) {
            xSemaphoreTake(s.done, portMAX_DELAY);
        }
        if (cv != pdPASS || cs != pdPASS) {
            printf("could not start the lanes\r\n");
            v.failures += cv != pdPASS ? v.n : 0u;
            s.failures += cs != pdPASS ? s.n : 0u;
        }
        const uint32_t wall = (uint32_t)((mtimer_get_time_us() - t0) / 1000u);
        ipro_ai_get_runtime_info(&after);
        printf("two models open, %lu B heap for the second one and the buffers\r\n",
               (unsigned long)(heap - (uint32_t)xPortGetFreeHeapSize()));
        for (lane_t *l = &v; l; l = l == &v ? &s : NULL) {
            printf("  %-11s %lu runs: %lu outputs differ from the model alone, %lu failed; "
                   "%lu us alone, %lu us avg / %lu max together\r\n", l->name, (unsigned long)l->n,
                   (unsigned long)l->mismatches, (unsigned long)l->failures, (unsigned long)l->us_alone,
                   (unsigned long)(l->us_sum / (l->n - l->failures ? l->n - l->failures : 1)),
                   (unsigned long)l->us_max);
        }
        printf("  %lu ms for both lanes; the runtime counted %lu waits for the NPU lock\r\n",
               (unsigned long)wall, (unsigned long)(after.lock_waits - before.lock_waits));
        rc = v.mismatches || s.mismatches || v.failures || s.failures ? -1 : 0;
    } else {
        printf("out of memory\r\n");
    }
    lane_free(&v);
    lane_free(&s);
    ipro_ai_model_close(s_sensor);
    s_sensor = NULL;
    return rc;
}
SHELL_CMD_EXPORT_ALIAS(cmd_multi, multi, two models at once through the runtime: multi [runs]);

#endif /* CONFIG_SHELL */

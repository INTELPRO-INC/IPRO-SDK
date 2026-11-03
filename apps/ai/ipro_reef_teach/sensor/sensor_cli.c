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
 * @file sensor_cli.c
 * @brief `sens` shell command: status, the simulated machine, results on UART0.
 */

#include <generated/autoconf.h>

#ifdef CONFIG_SHELL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "hal_mtimer.h"
#include <math.h>
#include "sensor_front.h"
#include "sensor_model.h"
#include "ipro_ai_source.h"
#include "shell.h"

#include "sensor_app.h"
#include "sensor_link.h"
#include "sensor_src.h"

static void usage(void)
{
    printf("sens status\r\n"
           "sens src <sim_vib|sim_imu|sim_env|sim_motor|sim_sound|mic>\r\n"
           "sens sim <state>           the simulated source's state (sens status lists them)\r\n"
           "sens print on|off          every result on the console\r\n"
           "sens poll <Hz> <s>         check the polled-source helper with a counter\r\n"
           "sens prof                  time the front end and the libm calls it makes\r\n");
}

/* `sens prof`: where the front end's time goes - the whole window, then the
 * libm calls it makes per window at the same count (docs/edge_ai/04 §3). */
static volatile float s_sink;

static float prof_sample(void *ctx, uint32_t ch, uint32_t i)
{
    (void)ctx;
    return (float)((int32_t)(ch * 977u + i * 131u) % 2001 - 1000);
}

static void prof(void)
{
    const sensor_cfg_t *m = sensor_model_cfg();
    int8_t *img = pvPortMalloc(SF_IMG_BYTES);
    const uint32_t n = 64u * 63u * 3u;                 /* values a 3-channel spectrogram computes */
    uint64_t t0;
    float acc = 0.0f;

    if (!img) {
        return;
    }
    if (m) {
        t0 = mtimer_get_time_us();
        sensor_front(&m->front, prof_sample, NULL, img);
        printf("front end (model's settings)  %8lu us\r\n", (unsigned long)(mtimer_get_time_us() - t0));
    }
    t0 = mtimer_get_time_us();
    for (uint32_t i = 0; i < n; i++) {
        acc += logf(1.0f + (float)i * 0.37f);
    }
    printf("logf   x%lu                %8lu us\r\n", (unsigned long)n, (unsigned long)(mtimer_get_time_us() - t0));
    t0 = mtimer_get_time_us();
    for (uint32_t i = 0; i < n; i++) {
        acc += sqrtf(1.0f + (float)i * 0.37f);
    }
    printf("sqrtf  x%lu                %8lu us\r\n", (unsigned long)n, (unsigned long)(mtimer_get_time_us() - t0));
    t0 = mtimer_get_time_us();
    for (uint32_t i = 0; i < 2u * n; i++) {
        acc += (1.0f + (float)i) / (3.0f + (float)(i & 7u));
    }
    printf("fdiv   x%lu                %8lu us\r\n", (unsigned long)(2u * n), (unsigned long)(mtimer_get_time_us() - t0));
    t0 = mtimer_get_time_us();
    for (uint32_t i = 0; i < 192u; i++) {
        acc += asinhf((float)i * 0.5f);
    }
    printf("asinhf x192                 %8lu us\r\n", (unsigned long)(mtimer_get_time_us() - t0));
    t0 = mtimer_get_time_us();
    for (uint32_t i = 0; i < 20480u; i++) {
        acc += logf(1.0f + (float)i * 0.37f);
    }
    printf("logf   x20480 (a voiceprint) %8lu us\r\n", (unsigned long)(mtimer_get_time_us() - t0));
    s_sink = acc;
    vPortFree(img);
}

/* `sens poll`: the platform's polled-source helper (what an I2C or SPI sensor
 * would use) with a counter as the "device", read like any source. Reports
 * the rate the clock saw, losses, and whether every value arrived in order. */
static int16_t s_poll_count;

static int poll_sample(void *user, int16_t *frame)
{
    (void)user;
    frame[0] = s_poll_count;
    frame[1] = (int16_t)-s_poll_count;
    s_poll_count++;
    return 0;
}

static int poll_check(uint32_t rate, uint32_t seconds)
{
    static int16_t store[1024 * 2];
    static ipro_ai_source_sampled_t poll;
    int16_t buf[64 * 2];
    ipro_ai_source_meta_t meta;

    poll = (ipro_ai_source_sampled_t){ .ring = { .buf = store, .capacity = 1024, .channels = 2,
                                                 .rate = rate, .unit = "count" },
                                       .sample = poll_sample };
    s_poll_count = 0;
    if (ipro_ai_source_sampled_register("poll_check", &poll) != 0) {
        printf("register failed\r\n");
        return -1;
    }
    ipro_ai_source_t *src = ipro_ai_source_find("poll_check");
    uint64_t frames = 0, first_t = 0, last_t = 0;
    uint32_t lost = 0, order_errors = 0;
    int16_t expect = 0;
    int rc = ipro_ai_source_open(src, NULL);
    rc = rc ? rc : ipro_ai_source_start(src);
    const uint64_t t0 = mtimer_get_time_us();
    while (rc == 0 && mtimer_get_time_us() - t0 < (uint64_t)seconds * 1000000u) {
        const int32_t n = ipro_ai_source_read(src, buf, 64, 50, &meta);
        if (n <= 0) {
            continue;
        }
        if (frames == 0) {
            first_t = meta.t_us;
        }
        for (int32_t i = 0; i < n; i++, expect++) {
            order_errors += buf[2 * i] != expect || buf[2 * i + 1] != (int16_t)-expect;
        }
        lost += meta.lost;                       /* dropped after these frames (ipro_ai_source.h) */
        expect = (int16_t)(expect + (int16_t)meta.lost);
        last_t = meta.t_us + (uint64_t)(n - 1) * 1000000u / rate;
        frames += (uint64_t)n;
    }
    ipro_ai_source_stats_t st;
    ipro_ai_source_get_stats(src, &st);
    ipro_ai_source_close(src);
    ipro_ai_source_unregister("poll_check");
    if (rc != 0) {
        printf("open/start failed: %d\r\n", rc);
        return -1;
    }
    const double span = (double)(last_t - first_t) / 1e6;
    printf("poll %lu Hz %lu s: %llu frames, %lu lost, %lu out of order, %lu stalls; "
           "timestamps span %.3f s -> %.2f Hz\r\n", (unsigned long)rate, (unsigned long)seconds,
           (unsigned long long)frames, (unsigned long)lost, (unsigned long)order_errors,
           (unsigned long)st.stalls, span, span > 0 ? (double)(frames - 1u) / span : 0.0);
    return order_errors ? -1 : 0;
}

static int cmd_sens(int argc, char **argv)
{
    int rc = 0;

    if (argc < 2 || !strcmp(argv[1], "status")) {
        sensor_app_print_status();
        sensor_link_print();
        return 0;
    }
    if (!strcmp(argv[1], "src") && argc > 2) {
        rc = sensor_app_src(argv[2]);
    } else if (!strcmp(argv[1], "sim") && argc > 2) {
        rc = sensor_app_sim(argv[2]);
    } else if (!strcmp(argv[1], "print") && argc > 2) {
        sensor_app_set_print(!strcmp(argv[2], "on"));
    } else if (!strcmp(argv[1], "prof")) {
        prof();
    } else if (!strcmp(argv[1], "poll") && argc > 3) {
        rc = poll_check((uint32_t)atoi(argv[2]), (uint32_t)atoi(argv[3]));
    } else {
        usage();
        return -1;
    }
    if (rc != 0) {
        printf("refused\r\n");
    }
    return rc;
}
SHELL_CMD_EXPORT_ALIAS(cmd_sens, sens, Sensor Teach status and simulated machine);

#endif /* CONFIG_SHELL */

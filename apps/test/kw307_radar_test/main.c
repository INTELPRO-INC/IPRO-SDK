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

/*
 * KW307 24 GHz presence radar on IPRO7AI - bring-up and on-board analysis.
 *
 * The radar runs continuously in its own task: every frame is decoded and
 * pushed into the PSRAM history ring, whether or not anyone is watching. That
 * matters because the interesting analysis needs a window, not a frame - the
 * breathing estimate alone wants 51.2 s of uninterrupted history.
 *
 * Output goes to the UART console and, when CONFIG_DEBUG_CONSOLE_USB is set,
 * is mirrored to a USB CDC-ACM port; shell input works from either.
 *
 * Shell:
 *   kw_init [tx] [rx] [gpo]   bring UART1 up on those pads (defaults below)
 *   kw_sniff [sec]            raw frame census - use this on first contact
 *   kw_mon [sec] [every]      decoded view; print 1 frame in `every` (0 = count)
 *   kw_stat                   link statistics
 *   kw_dist [sec]             average one measurement point into one line
 *   kw_scale [n]              distance divisor (measured: 2)
 *   kw_gain [0..8] | kw_range <min> <max> | kw_fov <deg> | kw_power <0|1>
 *   kw_out <mode> | kw_gpo
 *   kw_hist                   history ring status
 *   kw_act [sec]              activity classification over a window
 *   kw_breath                 breathing estimate (needs ~52 s of history)
 *   kw_map [sec]              range-time waterfall - the NPU input, drawn
 *   kw_capture <label> [sec]  same map as a CSV line, for collecting training data
 *   kw_spectro [label]        magnitude spectrogram - the DENSE 2D input; add
 *                             a label to also emit it as CSV
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "shell.h"
#include "board_config.h"
#include "freertos_app_hooks.h"

#include "kw307_dev.h"
#include "kw307_history.h"
#include "kw307_analysis.h"
#include "kw307_map.h"
#include "kw307_spectro.h"

#ifdef CONFIG_DEBUG_CONSOLE_USB
#include "usb_console.h"
#endif

/* Defaults. With the camera unused, VDDIO_1 stays at 3.3 V and GPIO0-2 are a
 * legitimate home for a 3.3 V peripheral; see check_pad_bank(). */
#ifndef KW307_DEFAULT_TX_PIN
#define KW307_DEFAULT_TX_PIN   0    /* IPRO7 TX -> module RX (P3) */
#endif
#ifndef KW307_DEFAULT_RX_PIN
#define KW307_DEFAULT_RX_PIN   1    /* IPRO7 RX <- module TX (P4) */
#endif
#ifndef KW307_DEFAULT_GPO_PIN
#define KW307_DEFAULT_GPO_PIN  2    /* module GPO (P5) presence flag */
#endif

#ifndef CONFIG_RADAR_KW307_HISTORY_SECONDS
#define CONFIG_RADAR_KW307_HISTORY_SECONDS 900
#endif

static bool s_ready;
static bool s_advanced;          /* module put into advanced tracker-state mode */

/* Per-frame printing is throttled independently of reception: printing is the
 * slowest thing in the path, so leaving it on is what turns into "missed
 * frames". 0 prints nothing. */
static uint32_t s_print_every;
static uint32_t s_seen;

/* ------------------------------------------------------------------ */
/* Pad bank warning                                                   */
/* ------------------------------------------------------------------ */

static bool in_vddio1_bank(int pin)
{
    return (pin >= 0 && pin <= 9) || (pin >= 36 && pin <= 41);
}

static void check_pad_bank(int tx, int rx, int gpo)
{
#ifdef CONFIG_IPRO7AI_EVK
    if (in_vddio1_bank(tx) || in_vddio1_bank(rx) || in_vddio1_bank(gpo)) {
        printf("\r\n*** CHECK J6 BEFORE POWERING THE MODULE ***\r\n");
        printf("  GPIO%d/%d/%d are in the VDDIO_1 bank (GPIO0-9, GPIO36-41),\r\n",
               tx, rx, gpo);
        printf("  which J6 sets to 1.8 V (PIN1/PIN3) or 3.3 V (PIN3/PIN5).\r\n");
        printf("  The KW307 is 3.3 V logic and NOT 5 V tolerant, so this bank\r\n");
        printf("  MUST be on 3.3 V - J6 shorting PIN3/PIN5. At 1.8 V the\r\n");
        printf("  module's 3.3 V TX exceeds the pad rating (a damage path) and\r\n");
        printf("  our 1.8 V TX never reaches its input threshold.\r\n");
        printf("  At 3.3 V the camera is unusable: its DOVDD is 1.8 V and its\r\n");
        printf("  DVP data lines are these same pads. This app does not use it.\r\n\r\n");
    }
#else
    (void)tx; (void)rx; (void)gpo;
#endif
}

/* ------------------------------------------------------------------ */
/* Frame path - history first, printing second                        */
/* ------------------------------------------------------------------ */

static void print_slot(const char *name, const kw307_tracker_t *t, bool moving)
{
    if (!kw307_tracker_valid(t, moving, s_advanced)) {
        printf("  %s: --", name);
        return;
    }
    const uint16_t mm = kw307_distance_mm(t);
    printf("  %s: %4u.%ucm %+5d.%udeg mag=%5u cnt=%2u",
           name, mm / 10, mm % 10,
           t->angle_tenth / 10, (unsigned)(abs(t->angle_tenth) % 10),
           t->magnitude, t->counter);
}

static void on_frame(const kw307_frame_t *f, void *arg)
{
    (void)arg;

    /* Unconditional: the ring is the input to every windowed analysis, so it
     * must not depend on whether a command happens to be watching. */
    kw307_history_push(f, s_advanced);

    s_seen++;
    if (!s_print_every || (s_seen % s_print_every)) {
        return;
    }
    printf("[%5u] %s", f->index, f->human_flag ? "PRESENT" : "  empty");
    print_slot("mov0", &f->moving[0], true);
    print_slot("mov1", &f->moving[1], true);
    print_slot("still", &f->stationary[0], false);
    printf("\r\n");
}

/* The radar owns a task so the history is continuous. 10 ms is five times the
 * 20 Hz frame period, which leaves margin for a print stall. */
static void radar_task(void *arg)
{
    (void)arg;
    for (;;) {
        if (s_ready) {
            kw307_dev_poll();
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/* ------------------------------------------------------------------ */
/* Helpers                                                            */
/* ------------------------------------------------------------------ */

static int require_ready(void)
{
    if (!s_ready) {
        printf("run kw_init first\r\n");
        return -1;
    }
    return 0;
}

static void report(const char *what, int rc)
{
    if (rc == 0) {
        printf("%s: ACK OK\r\n", what);
    } else if (rc < 0) {
        printf("%s: no response (rc=%d) - check wiring, levels and baud\r\n", what, rc);
    } else {
        printf("%s: rejected - %s (0x%02X)\r\n", what, kw307_ack_status_str((uint8_t)rc), rc);
    }
}

/* Run for `secs` and report how long that ACTUALLY took: the loop costs more
 * than its delay, so dividing by the nominal seconds overstates the rate. */
static uint32_t run_for(int secs)
{
    const TickType_t t0 = xTaskGetTickCount();
    const TickType_t want = pdMS_TO_TICKS((uint32_t)secs * 1000u);

    while ((TickType_t)(xTaskGetTickCount() - t0) < want) {
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    return (uint32_t)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS);
}

static uint32_t frame_rate_dhz(uint32_t frames, uint32_t ms)
{
    return ms ? (frames * 10000u) / ms : 0u;
}

/* ------------------------------------------------------------------ */
/* Link commands                                                      */
/* ------------------------------------------------------------------ */

static int cmd_kw_init(int argc, char **argv)
{
    int tx  = argc > 1 ? atoi(argv[1]) : KW307_DEFAULT_TX_PIN;
    int rx  = argc > 2 ? atoi(argv[2]) : KW307_DEFAULT_RX_PIN;
    int gpo = argc > 3 ? atoi(argv[3]) : KW307_DEFAULT_GPO_PIN;

    check_pad_bank(tx, rx, gpo);

    int rc = kw307_dev_init(tx, rx, gpo);
    if (rc == -2) {
        printf("GPIO%d and GPIO%d share UART signal slot %d - pick pads whose\r\n"
               "numbers differ modulo 12\r\n", tx, rx, tx % 12);
        return -1;
    }
    if (rc) {
        printf("init failed (%d)\r\n", rc);
        return -1;
    }

    kw307_dev_set_frame_cb(on_frame, NULL);
    s_print_every = 0;
    s_ready = true;

    printf("UART1 %d baud: TX=GPIO%d (-> module P3 RX), RX=GPIO%d (<- module P4 TX)",
           KW307_DEV_BAUD, tx, rx);
    if (gpo >= 0) {
        printf(", GPO=GPIO%d", gpo);
    }
    printf("\r\nHistory is now filling; kw_hist shows it.\r\n");
    return 0;
}

static int cmd_kw_sniff(int argc, char **argv)
{
    if (require_ready()) {
        return -1;
    }
    int secs = argc > 1 ? atoi(argv[1]) : 5;

    kw307_dev_stats_reset();
    printf("sniffing %d s...\r\n", secs);
    uint32_t ms = run_for(secs);

    const kw307_stats_t *s = kw307_dev_stats();
    uint32_t dhz = frame_rate_dhz(s->frames, ms);
    printf("frames=%lu in %lu ms (%lu.%lu Hz) other=%lu short=%lu\r\n",
           (unsigned long)s->frames, (unsigned long)ms,
           (unsigned long)(dhz / 10), (unsigned long)(dhz % 10),
           (unsigned long)s->other_frames, (unsigned long)s->short_frames);
    printf("crc_errors=%lu dropped_bytes=%lu rx_overruns=%lu\r\n",
           (unsigned long)s->crc_errors, (unsigned long)s->dropped_bytes,
           (unsigned long)s->rx_overruns);

    if (!s->frames && !s->dropped_bytes) {
        printf("nothing arrived at all - check GND, the module's 5V rail, and\r\n"
               "that our RX pad is on the module's TX (P4)\r\n");
        return -1;
    }
    if (!s->frames) {
        printf("bytes but no valid frames - suspect baud, level mismatch or a\r\n"
               "swapped TX/RX pair\r\n");
        return -1;
    }
    return 0;
}

static int cmd_kw_mon(int argc, char **argv)
{
    if (require_ready()) {
        return -1;
    }
    int secs = argc > 1 ? atoi(argv[1]) : 10;
    s_print_every = argc > 2 ? (uint32_t)strtoul(argv[2], NULL, 0) : 1;
    s_seen = 0;

    if (!s_print_every) {
        printf("counting only, no per-frame output (%d s)...\r\n", secs);
    }
    kw307_dev_stats_reset();
    uint32_t ms = run_for(secs);
    s_print_every = 0;

    const kw307_stats_t *s = kw307_dev_stats();
    uint32_t dhz = frame_rate_dhz(s->frames, ms);
    printf("%lu frames in %lu ms (%lu.%lu Hz), missed=%lu, crc=%lu, "
           "overruns=%lu, dropped=%lu\r\n",
           (unsigned long)s->frames, (unsigned long)ms,
           (unsigned long)(dhz / 10), (unsigned long)(dhz % 10),
           (unsigned long)s->missed, (unsigned long)s->crc_errors,
           (unsigned long)s->rx_overruns, (unsigned long)s->dropped_bytes);
    if (s->missed && !s->rx_overruns) {
        printf("missed without an overrun: the module skipped frames, not us\r\n");
    }
    return 0;
}

static int cmd_kw_stat(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (require_ready()) {
        return -1;
    }
    const kw307_stats_t *s = kw307_dev_stats();
    printf("TX=GPIO%d RX=GPIO%d\r\n", kw307_dev_tx_pin(), kw307_dev_rx_pin());
    printf("frames=%lu missed=%lu other=%lu short=%lu\r\n",
           (unsigned long)s->frames, (unsigned long)s->missed,
           (unsigned long)s->other_frames, (unsigned long)s->short_frames);
    printf("crc_errors=%lu dropped_bytes=%lu rx_overruns=%lu\r\n",
           (unsigned long)s->crc_errors, (unsigned long)s->dropped_bytes,
           (unsigned long)s->rx_overruns);
#ifdef CONFIG_DEBUG_CONSOLE_USB
    printf("usb console: %s\r\n", usb_console_is_connected() ? "connected" : "not open");
#endif
    return 0;
}

/* ------------------------------------------------------------------ */
/* Measurement point                                                  */
/* ------------------------------------------------------------------ */

/* Integer arithmetic throughout: this FreeRTOS port does not save the FPU
 * context across a task switch, so float in a preemptible task is a silent
 * corruption source. Spread (max-min) replaces a standard deviation - no sqrt,
 * and it says the same thing here. */
typedef struct {
    uint32_t n;
    uint32_t dist_sum;
    uint16_t dist_min, dist_max;
    int32_t  ang_sum;
    uint32_t mag_sum;
    uint16_t mag_min, mag_max;
} slot_acc_t;

static void acc_add(slot_acc_t *a, uint16_t mm, int16_t ang, uint16_t mag)
{
    if (!a->n) {
        a->dist_min = a->dist_max = mm;
        a->mag_min  = a->mag_max  = mag;
    }
    a->n++;
    a->dist_sum += mm;
    a->ang_sum  += ang;
    a->mag_sum  += mag;
    if (mm < a->dist_min) a->dist_min = mm;
    if (mm > a->dist_max) a->dist_max = mm;
    if (mag < a->mag_min) a->mag_min = mag;
    if (mag > a->mag_max) a->mag_max = mag;
}

static void acc_report(const char *name, const slot_acc_t *a, uint32_t frames)
{
    if (!a->n) {
        printf("  %-6s never valid\r\n", name);
        return;
    }
    const uint32_t dist = a->dist_sum / a->n;
    const int32_t  ang  = a->ang_sum / (int32_t)a->n;
    const uint32_t mag  = a->mag_sum / a->n;

    printf("  %-6s %lu%% | dist %lu.%lucm (%u.%u..%u.%u, spread %u.%ucm)"
           " | ang %+ld.%ld deg | mag %lu (%u..%u)\r\n",
           name, (unsigned long)(a->n * 100 / (frames ? frames : 1)),
           (unsigned long)(dist / 10), (unsigned long)(dist % 10),
           a->dist_min / 10, a->dist_min % 10, a->dist_max / 10, a->dist_max % 10,
           (uint16_t)((a->dist_max - a->dist_min) / 10),
           (uint16_t)((a->dist_max - a->dist_min) % 10),
           (long)(ang / 10), (long)(ang < 0 ? -ang : ang) % 10,
           (unsigned long)mag, a->mag_min, a->mag_max);
}

static int cmd_kw_dist(int argc, char **argv)
{
    if (require_ready()) {
        return -1;
    }
    int secs = argc > 1 ? atoi(argv[1]) : 5;

    printf("averaging %d s - hold still...\r\n", secs);
    const uint32_t before = kw307_history_count();
    run_for(secs);

    /* Read the window back out of the history rather than accumulating in the
     * frame callback: same numbers, and it exercises the ring the analysis uses. */
    uint32_t n = kw307_history_count() - before;
    if (n > kw307_history_count()) {
        n = kw307_history_count();
    }

    slot_acc_t acc[3];
    memset(acc, 0, sizeof(acc));
    for (uint32_t age = 0; age < n; age++) {
        kw307_sample_t s;
        if (!kw307_history_at(age, &s)) {
            break;
        }
        if (s.valid_mask & KW307_VALID_M0)    acc_add(&acc[0], s.m0_dist_mm, s.m0_ang, s.m0_mag);
        if (s.valid_mask & KW307_VALID_M1)    acc_add(&acc[1], s.m1_dist_mm, s.m1_ang, s.m1_mag);
        if (s.valid_mask & KW307_VALID_STILL) acc_add(&acc[2], s.s_dist_mm,  s.s_ang,  s.s_mag);
    }

    printf("%lu frames\r\n", (unsigned long)n);
    acc_report("mov0", &acc[0], n);
    acc_report("mov1", &acc[1], n);
    acc_report("still", &acc[2], n);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Configuration                                                      */
/* ------------------------------------------------------------------ */

static int cmd_kw_scale(int argc, char **argv)
{
    if (argc > 1) {
        int d = atoi(argv[1]);
        if (d < 1 || d > 100) {
            printf("divisor must be 1..100\r\n");
            return -1;
        }
        kw307_dist_divisor = (uint16_t)d;
    }
    printf("distance divisor = %u%s\r\n", kw307_dist_divisor,
           kw307_dist_divisor == 1 ? "  (raw wire value, uncorrected)" : "");
    printf("Measured 2026-08-31 by a two-point test at 1.0 m and 3.0 m: the\r\n");
    printf("module reports twice the true range, i.e. the round-trip path.\r\n");
    printf("This contradicts SDK User Guide 2.1 - re-check it on new firmware.\r\n");
    return 0;
}

static int cmd_kw_gain(int argc, char **argv)
{
    if (require_ready()) {
        return -1;
    }
    static const int db[] = { -3, 0, 3, 6, 9, 12, 15, 18, 21 };

    if (argc < 2) {
        uint8_t idx = 0;
        int rc = kw307_dev_get_gain(&idx);
        if (rc) {
            report("gain read", rc);
            return -1;
        }
        printf("gain index %u (%+d dB)\r\n", idx, idx < 9 ? db[idx] : 0);
        return 0;
    }
    int idx = atoi(argv[1]);
    if (idx < 0 || idx > 8) {
        printf("gain index must be 0..8 (-3 .. +21 dB)\r\n");
        return -1;
    }
    report("gain", kw307_dev_set_gain((uint8_t)idx));
    return 0;
}

static int cmd_kw_range(int argc, char **argv)
{
    if (require_ready()) return -1;
    if (argc < 3) {
        printf("Usage: kw_range <min_cm> <max_cm>   (boot default 50 1000)\r\n");
        return -1;
    }
    report("range", kw307_dev_set_detect_range((uint16_t)atoi(argv[1]),
                                               (uint16_t)atoi(argv[2])));
    return 0;
}

static int cmd_kw_fov(int argc, char **argv)
{
    if (require_ready()) return -1;
    if (argc < 2) {
        printf("Usage: kw_fov <deg>   1..180, boot default 120\r\n");
        return -1;
    }
    report("fov", kw307_dev_set_detect_fov((uint16_t)atoi(argv[1])));
    return 0;
}

static int cmd_kw_power(int argc, char **argv)
{
    if (require_ready()) return -1;
    if (argc < 2) {
        printf("Usage: kw_power <0|1>   0 = deep sleep, 1 = normal\r\n");
        return -1;
    }
    report("power mode", kw307_dev_set_power_mode(atoi(argv[1]) != 0));
    return 0;
}

static int cmd_kw_out(int argc, char **argv)
{
    if (require_ready()) return -1;
    if (argc < 2) {
        printf("Usage: kw_out <mode>   CMD 0x11 output-mode byte\r\n");
        return -1;
    }
    report("output mode", kw307_dev_set_output_mode((uint8_t)strtoul(argv[1], NULL, 0)));
    return 0;
}

static int cmd_kw_gpo(int argc, char **argv)
{
    (void)argc; (void)argv;
    int level = kw307_dev_gpo_level();
    if (level < 0) {
        printf("no GPO pad configured\r\n");
        return -1;
    }
    printf("GPO = %d (%s)\r\n", level, level ? "presence" : "empty");
    return 0;
}

/* ------------------------------------------------------------------ */
/* History and analysis                                               */
/* ------------------------------------------------------------------ */

static int cmd_kw_hist(int argc, char **argv)
{
    if (argc > 1 && !strcmp(argv[1], "clear")) {
        kw307_history_clear();
        printf("history cleared\r\n");
        return 0;
    }
    const uint32_t cap = kw307_history_capacity();
    const uint32_t n = kw307_history_count();
    if (!cap) {
        printf("history ring not allocated\r\n");
        return -1;
    }
    printf("history %lu/%lu samples (%lu.%lu s of %lu s), %lu KB, overwritten=%lu\r\n",
           (unsigned long)n, (unsigned long)cap,
           (unsigned long)(n / KW307_HISTORY_RATE_HZ),
           (unsigned long)((n * 10 / KW307_HISTORY_RATE_HZ) % 10),
           (unsigned long)(cap / KW307_HISTORY_RATE_HZ),
           (unsigned long)(kw307_history_bytes() / 1024),
           (unsigned long)kw307_history_dropped());
    return 0;
}

static int cmd_kw_act(int argc, char **argv)
{
    int secs = argc > 1 ? atoi(argv[1]) : 5;
    kw307_activity_t a;

    if (!kw307_activity((uint32_t)secs, &a)) {
        printf("not enough history yet (need a second); run kw_init first\r\n");
        return -1;
    }
    printf("%-8s over %u frames | presence %u%% moving %u%%\r\n",
           kw307_activity_str(a.cls), a.window_samples, a.presence_pct, a.moving_pct);
    if (a.dist_mm) {
        printf("  range %u.%ucm  angle %+d.%ddeg  radial %+d mm/s\r\n",
               a.dist_mm / 10, a.dist_mm % 10,
               a.angle_tenth / 10, abs(a.angle_tenth) % 10, a.radial_mm_s);
    }
    return 0;
}

static int cmd_kw_breath(int argc, char **argv)
{
    (void)argc; (void)argv;
    kw307_breath_t b;

    if (!kw307_breath(&b)) {
        printf("need %d samples (%d s) of history; have %lu\r\n",
               KW307_BREATH_FFT_N, KW307_BREATH_FFT_N / KW307_HISTORY_RATE_HZ,
               (unsigned long)kw307_history_count());
        return -1;
    }
    printf("window %u samples, stationary slot Valid in %u of them\r\n",
           b.window_samples, b.valid_samples);
    if (!b.valid) {
        printf("no usable peak (snr %u.%u) - subject must sit still, and the\r\n"
               "stationary slot must hold for most of the window\r\n",
               b.snr_x10 / 10, b.snr_x10 % 10);
        return 0;
    }
    printf("breathing %u.%u /min  (bin %u, snr %u.%u)\r\n",
           b.rate_bpm_x10 / 10, b.rate_bpm_x10 % 10,
           b.peak_bin, b.snr_x10 / 10, b.snr_x10 % 10);
    printf("NOTE: this is an amplitude proxy - the module gives magnitude, not\r\n");
    printf("phase - and the SNR floor has not been checked against a reference.\r\n");
    return 0;
}

/* ------------------------------------------------------------------ */
/* Range-time map - the NPU input                                     */
/* ------------------------------------------------------------------ */

/* KW307_MAP_CELLS is 2 KB, too much for a shell task stack, and the map is
 * built one command at a time, so a single static buffer is enough. */
static uint8_t s_map[KW307_MAP_CELLS];

static int cmd_kw_map(int argc, char **argv)
{
    const uint32_t window_s = argc > 1 ? (uint32_t)atoi(argv[1]) : 60;
    kw307_map_info_t info;

    if (!kw307_map_build(window_s, s_map, &info)) {
        printf("need %lu s of history; have %lu.%lu s\r\n",
               (unsigned long)window_s,
               (unsigned long)(kw307_history_count() / KW307_HISTORY_RATE_HZ),
               (unsigned long)((kw307_history_count() * 10 / KW307_HISTORY_RATE_HZ) % 10));
        return -1;
    }
    kw307_map_print(s_map, &info);
    return 0;
}

/* Capture one labelled example for training. The map is exactly the tensor a
 * model would be fed, so what is collected here and what runs on the NPU later
 * cannot drift apart. */
static int cmd_kw_capture(int argc, char **argv)
{
    const char *label = argc > 1 ? argv[1] : "unlabelled";
    const uint32_t window_s = argc > 2 ? (uint32_t)atoi(argv[2]) : 60;
    kw307_map_info_t info;

    if (!kw307_map_build(window_s, s_map, &info)) {
        printf("need %lu s of history\r\n", (unsigned long)window_s);
        return -1;
    }
    kw307_map_dump_csv(s_map, &info, label);
    return 0;
}

/* The spectrogram is the DENSE representation and the intended NPU input; the
 * range-time map above stays because it is the readable picture of a
 * trajectory, but on this module it runs at about 2% occupancy - the part
 * reports a tracked point, not a range profile, so one bin is lit per column. */
static uint8_t s_spec[KW307_SPECTRO_CELLS];

static int cmd_kw_spectro(int argc, char **argv)
{
    kw307_spectro_info_t info;

    if (!kw307_spectro_build(s_spec, &info)) {
        const uint32_t have = kw307_history_count();
        if (have < KW307_SPECTRO_NEED_SAMPLES) {
            printf("need %d samples (%d s) of history; have %lu (%lu s)\r\n",
                   KW307_SPECTRO_NEED_SAMPLES,
                   KW307_SPECTRO_NEED_SAMPLES / KW307_HISTORY_RATE_HZ,
                   (unsigned long)have,
                   (unsigned long)(have / KW307_HISTORY_RATE_HZ));
        } else {
            printf("the stationary slot was Valid for too little of the window -\r\n"
                   "the gaps would read as a slow oscillation, so no image\r\n");
        }
        return -1;
    }
    kw307_spectro_print(s_spec, &info);
    if (argc > 1) {
        kw307_spectro_dump_csv(s_spec, &info, argv[1]);
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_kw_init,   kw_init,   Bring UART1 up on the radar pads);
SHELL_CMD_EXPORT_ALIAS(cmd_kw_sniff,  kw_sniff,  Raw frame census);
SHELL_CMD_EXPORT_ALIAS(cmd_kw_mon,    kw_mon,    Decoded live view);
SHELL_CMD_EXPORT_ALIAS(cmd_kw_stat,   kw_stat,   Link statistics);
SHELL_CMD_EXPORT_ALIAS(cmd_kw_dist,   kw_dist,   Average one measurement point);
SHELL_CMD_EXPORT_ALIAS(cmd_kw_scale,  kw_scale,  Show or set the distance divisor);
SHELL_CMD_EXPORT_ALIAS(cmd_kw_gain,   kw_gain,   Read or set RF gain);
SHELL_CMD_EXPORT_ALIAS(cmd_kw_range,  kw_range,  Set detection range in cm);
SHELL_CMD_EXPORT_ALIAS(cmd_kw_fov,    kw_fov,    Set detection FOV in degrees);
SHELL_CMD_EXPORT_ALIAS(cmd_kw_power,  kw_power,  Power mode);
SHELL_CMD_EXPORT_ALIAS(cmd_kw_out,    kw_out,    Output mode byte);
SHELL_CMD_EXPORT_ALIAS(cmd_kw_gpo,    kw_gpo,    Read the presence pin);
SHELL_CMD_EXPORT_ALIAS(cmd_kw_hist,   kw_hist,   History ring status);
SHELL_CMD_EXPORT_ALIAS(cmd_kw_act,    kw_act,    Activity over a window);
SHELL_CMD_EXPORT_ALIAS(cmd_kw_breath, kw_breath, Breathing estimate);
SHELL_CMD_EXPORT_ALIAS(cmd_kw_map,    kw_map,    Range-time waterfall);
SHELL_CMD_EXPORT_ALIAS(cmd_kw_capture,kw_capture,Dump one labelled map as CSV);
SHELL_CMD_EXPORT_ALIAS(cmd_kw_spectro,kw_spectro,Magnitude spectrogram - the NPU input);

/* ------------------------------------------------------------------ */

/* ipro_platform_init() already starts the shell task, so this app must not
 * start a second one - doing so corrupts the shell and faults on the first
 * prompt. This task exists only for the work that must happen after the
 * scheduler is running. */
static void setup_task(void *arg)
{
    (void)arg;

#ifdef CONFIG_DEBUG_CONSOLE_USB
    int rc = usb_console_init();
    printf("usb console: %s (%d)\r\n", rc == 0 ? "up" : "failed", rc);
#endif

    if (kw307_history_init(CONFIG_RADAR_KW307_HISTORY_SECONDS) != 0) {
        printf("history ring: PSRAM allocation FAILED - windowed analysis is off\r\n");
    } else {
        printf("history ring: %lu s, %lu KB in PSRAM\r\n",
               (unsigned long)CONFIG_RADAR_KW307_HISTORY_SECONDS,
               (unsigned long)(kw307_history_bytes() / 1024));
    }

    int arc = kw307_analysis_init();
    printf("analysis: %s (%d)\r\n", arc == 0 ? "ready" : "unavailable", arc);

    int src = kw307_spectro_init();
    printf("spectrogram: %s (%d), needs %d s of history\r\n",
           src == 0 ? "ready" : "unavailable", src,
           KW307_SPECTRO_NEED_SAMPLES / KW307_HISTORY_RATE_HZ);

    xTaskCreate(radar_task, "radar", 1024, NULL, 6, NULL);
    vTaskDelete(NULL);
}

static void print_banner(void)
{
    printf("\r\n========================================\r\n");
    printf("  KW307 24GHz radar on IPRO7AI\r\n");
    printf("========================================\r\n");
    printf("  kw_init [tx] [rx] [gpo]   default %d %d %d\r\n",
           KW307_DEFAULT_TX_PIN, KW307_DEFAULT_RX_PIN, KW307_DEFAULT_GPO_PIN);
    printf("  kw_sniff [sec] | kw_mon [sec] [every] | kw_stat | kw_dist [sec]\r\n");
    printf("  kw_gain [0..8] | kw_range <min> <max> | kw_fov <deg>\r\n");
    printf("  kw_power <0|1> | kw_out <mode> | kw_gpo | kw_scale [n]\r\n");
    printf("  kw_hist [clear] | kw_act [sec] | kw_breath\r\n");
    printf("  kw_map [sec] | kw_capture <label> [sec] | kw_spectro [label]\r\n");
    printf("========================================\r\n");
}

int main(void)
{
    ipro_platform_init();
    print_banner();

    xTaskCreate(setup_task, "setup", 1024, NULL, configMAX_PRIORITIES - 1, NULL);
    vTaskStartScheduler();

    while (1) { }
    return 0;
}

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
 * @file sensor_sim.c
 * @brief Simulated sensors. See sensor_sim.h.
 *
 * Single precision only (the IPRO7 FPU has no double). Phases are integrated
 * and wrapped rather than computed from an ever-growing time, so a source can
 * run for days without losing precision.
 */
#include "sensor_sim.h"

#include <math.h>
#include <string.h>

#define TWO_PI  6.28318531f
#define REDRAW  10.0f                   /* s: a new speed, level and noise this often */

static const sim_type_info_t s_types[SIM_TYPES] = {
    [SIM_VIB]   = { "sim_vib", 1000u, 200u, 8000u, 3u, 8192.0f, "g", 6u,
                    { "off", "normal", "imbalance", "misalign", "bearing", "looseness" } },
    [SIM_IMU]   = { "sim_imu", 100u, 25u, 1000u, 6u, 8192.0f, "g,500dps", 6u,
                    { "still", "walk", "run", "stairs", "shake", "fall" } },
    [SIM_ENV]   = { "sim_env", 10u, 1u, 100u, 3u, 100.0f, "C,hPa-1000,%", 6u,
                    { "normal", "heating", "cooling_fail", "leak", "door_open", "sensor_fault" } },
    [SIM_MOTOR] = { "sim_motor", 2000u, 500u, 8000u, 3u, 1000.0f, "A", 7u,
                    { "off", "normal", "overload", "phase_loss", "broken_bar", "bearing",
                      "unbalanced" } },
    [SIM_SOUND] = { "sim_sound", 8000u, 4000u, 16000u, 1u, 32768.0f, "fs", 6u,
                    { "quiet", "hum", "alarm", "knock", "hiss", "voice" } },
};

typedef struct { float a1, a2, b, y1, y2; } reso_t;   /* struck resonance: r^n sin(w(n+1)) */
typedef struct { float b0, b2, a1, a2, x1, x2, y1, y2; } band_t;   /* RBJ band-pass, b1 = 0 */

static struct {
    sim_type_t   type;
    sim_params_t p;
    uint32_t     state, rng;
    float        rate;
    uint32_t     since_draw, n_state;  /* frames since the last draw / the state change */
    /* drawn every REDRAW s; the level glides to its new value (a load
     * changes over seconds: a step would read as modulation) */
    float level, level_to, d0, fd, noise;
    float ph[16];
    /* integrated phases, wrapped */
    float drift_ph, shaft, a, b, c, d;
    float next_event, event_left, hold[SIM_MAX_CH];
    reso_t reso[3];
    band_t band;
} S;

/* ---------------------------------------------------------------- helpers */

static float urand(void)
{
    S.rng ^= S.rng << 13;
    S.rng ^= S.rng >> 17;
    S.rng ^= S.rng << 5;
    return (float)(S.rng >> 8) * (1.0f / 16777216.0f);
}

static float uniform(float lo, float hi)
{
    return lo + (hi - lo) * urand();
}

static float gauss(void)
{
    float u = urand();
    if (u < 1e-7f) {
        u = 1e-7f;
    }
    return sqrtf(-2.0f * logf(u)) * cosf(TWO_PI * urand());
}

/* phase += 2 pi f / rate, kept in [0, wrap) */
static float advance(float *phase, float f, float wrap)
{
    *phase += TWO_PI * f / S.rate;
    if (*phase >= wrap) {
        *phase -= wrap;
    }
    return *phase;
}

static void reso_init(reso_t *r, float f, float decay)
{
    const float rr = expf(-decay / S.rate), w = TWO_PI * f / S.rate;
    r->a1 = 2.0f * rr * cosf(w);
    r->a2 = -rr * rr;
    r->b = sinf(w);
    r->y1 = r->y2 = 0.0f;
}

static float reso_step(reso_t *r, float x)
{
    const float y = r->a1 * r->y1 + r->a2 * r->y2 + r->b * x;
    r->y2 = r->y1;
    r->y1 = y;
    return y;
}

static void band_init(band_t *f, float fc, float q)
{
    const float w = TWO_PI * fc / S.rate, alpha = sinf(w) / (2.0f * q), a0 = 1.0f + alpha;
    f->b0 = alpha / a0;
    f->b2 = -alpha / a0;
    f->a1 = -2.0f * cosf(w) / a0;
    f->a2 = (1.0f - alpha) / a0;
    f->x1 = f->x2 = f->y1 = f->y2 = 0.0f;
}

static float band_step(band_t *f, float x)
{
    const float y = f->b0 * x + f->b2 * f->x2 - f->a1 * f->y1 - f->a2 * f->y2;
    f->x2 = f->x1;
    f->x1 = x;
    f->y2 = f->y1;
    f->y1 = y;
    return y;
}

static void draw(void)
{
    S.level_to = uniform(0.8f, 1.25f);
    if (S.level == 0.0f) {
        S.level = S.level_to;
    }
    S.d0 = uniform(-1.0f, 1.0f);
    S.fd = uniform(0.05f, 0.2f);
    S.noise = uniform(0.7f, 1.4f);
    for (unsigned i = 0; i < sizeof(S.ph) / sizeof(S.ph[0]); i++) {
        S.ph[i] = uniform(0.0f, TWO_PI);
    }
    S.since_draw = 0;
}

float sim_severity_now(void)
{
    float sev = S.p.severity;
    if (S.p.ramp_s > 0.0f) {
        const float grown = (float)S.n_state / (S.p.ramp_s * S.rate);
        sev *= grown < 1.0f ? grown : 1.0f;
    }
    return sev;
}

/* drift, slowly wandering: +-d percent around 1 */
static float wander(float percent)
{
    S.drift_ph += TWO_PI * S.fd / S.rate;
    if (S.drift_ph >= TWO_PI) {
        S.drift_ph -= TWO_PI;
    }
    return 1.0f + 0.01f * percent * S.p.drift * (S.d0 + 0.5f * sinf(S.drift_ph));
}

/* ---------------------------------------------------------------- sim_vib */

#define VIB_F0     25.0f                /* shaft, Hz (1500 rpm) */
#define VIB_BPFO   3.58f                /* outer-race defect / shaft */
#define VIB_RING   320.0f               /* Hz, rung by each bearing impact */

static void gen_vib(float *g, float sev)
{
    const float n = 0.02f * S.noise * S.p.noise;
    for (int a = 0; a < 3; a++) {
        g[a] = n * gauss() * (S.state == 0 ? 0.5f : 1.0f);
    }
    if (S.state == 0) {
        return;
    }
    const float f = VIB_F0 * wander(4.0f);
    const float th = advance(&S.shaft, f, 2.0f * TWO_PI);     /* 4 pi: 0.5x stays continuous */
    const float L = S.level;
#define RADIAL(order, amp, slot) do { \
        const float p_ = (order) * th + S.ph[slot]; \
        g[0] += (amp) * L * sinf(p_); \
        g[1] += (amp) * L * sinf(p_ + TWO_PI / 4.0f); \
    } while (0)
    RADIAL(1.0f, 0.05f, 0);
    RADIAL(2.0f, 0.02f, 1);
    RADIAL(7.0f, 0.03f, 2);
    g[2] += 0.02f * L * sinf(th + S.ph[3]);
    switch (S.state) {
    case 2:                                                   /* imbalance */
        RADIAL(1.0f, 0.25f * sev, 4);
        break;
    case 3:                                                   /* misalign */
        RADIAL(2.0f, 0.2f * sev, 5);
        g[2] += 0.1f * sev * L * sinf(th + S.ph[6]);
        break;
    case 4: {                                                 /* bearing */
        float x = 0.0f;
        if (--S.next_event <= 0.0f) {
            x = uniform(0.2f, 0.35f) * L * sev;
            S.next_event += S.rate / (VIB_BPFO * f) * uniform(0.98f, 1.02f);
        }
        const float y = reso_step(&S.reso[0], x);
        g[0] += y;
        g[1] += 0.6f * y;
        break;
    }
    case 5:                                                   /* looseness */
        for (int k = 1; k <= 10; k++) {
            RADIAL((float)k, 0.04f * sev / sqrtf((float)k), 2 + k);
        }
        RADIAL(0.5f, 0.05f * sev, 13);
        break;
    default:
        break;
    }
#undef RADIAL
}

/* ---------------------------------------------------------------- sim_imu */

/* ax ay az in g, gx gy gz in units of 500 dps; upright: gravity on z */
static void gen_imu(float *g, float sev)
{
    const float na = 0.01f * S.noise * S.p.noise, ng = 0.004f * S.noise * S.p.noise;
    const float A = sev * S.level;
    float ax = 0.0f, ay = 0.0f, az = 1.0f, gx = 0.0f, gy = 0.0f, gz = 0.0f;

    switch (S.state) {
    case 1:                                                   /* walk: 1.8 steps/s */
    case 3: {                                                 /* stairs: slower, lifts */
        const float cad = (S.state == 1 ? 1.8f : 1.4f) * wander(5.0f);
        const float ph = advance(&S.a, cad, 2.0f * TWO_PI);   /* one stride = two steps */
        az += 0.25f * A * sinf(2.0f * ph) + (S.state == 3 ? 0.15f * A * sinf(2.0f * ph + 0.6f) +
                                              0.08f * A * fmaxf(0.0f, sinf(2.0f * ph - 1.0f)) : 0.0f);
        ax += 0.15f * A * sinf(2.0f * ph + 1.57f);
        ay += 0.10f * A * sinf(ph);
        gx += 0.10f * A * sinf(ph);
        gy += 0.06f * A * sinf(2.0f * ph);
        gz += 0.04f * A * sinf(ph + 0.4f);
        break;
    }
    case 2: {                                                 /* run: 2.8 steps/s, heel strikes */
        const float ph = advance(&S.a, 2.8f * wander(5.0f), 2.0f * TWO_PI);
        const float s2 = fmaxf(0.0f, sinf(2.0f * ph));
        az += A * (2.5f * s2 * s2 * s2 * s2 * s2 * s2 - 0.4f);
        ax += 0.4f * A * sinf(2.0f * ph + 1.0f);
        ay += 0.2f * A * sinf(ph);
        gx += 0.3f * A * sinf(ph);
        gy += 0.2f * A * sinf(2.0f * ph + 0.5f);
        gz += 0.1f * A * sinf(ph);
        break;
    }
    case 4: {                                                 /* shake: 8 Hz */
        const float ph = advance(&S.a, 8.0f * wander(8.0f), TWO_PI);
        ax += 0.8f * A * sinf(ph);
        ay += 0.5f * A * sinf(ph + 1.0f);
        gz += 0.6f * A * sinf(ph + 0.3f);
        gx += 0.2f * A * sinf(ph + 2.0f);
        break;
    }
    case 5: {                                                 /* fall, every 8 s */
        S.a += 1.0f / S.rate;
        if (S.a >= 8.0f) {
            S.a -= 8.0f;
        }
        const float t = S.a;
        if (t < 3.0f) {                                       /* walking up to it */
            const float ph = advance(&S.b, 1.8f, 2.0f * TWO_PI);
            az += 0.25f * sinf(2.0f * ph);
            ax += 0.15f * sinf(2.0f * ph + 1.57f);
            gx += 0.1f * sinf(ph);
        } else if (t < 3.4f) {                                /* free fall */
            az = 1.0f - 0.9f * A;
            gy = 0.8f * A;
        } else if (t < 3.6f) {                                /* impact */
            const float k = expf(-(t - 3.4f) * 30.0f);
            ax = (1.0f - k) * 1.0f + 3.0f * A * k * gauss();
            ay = 2.0f * A * k * gauss();
            az = 3.5f * A * k;
            gx = 1.5f * A * k * gauss();
            gy = 1.5f * A * k * gauss();
        } else if (t < 6.5f) {                                /* lying: gravity on x */
            ax = 1.0f;
            az = 0.0f;
        } else {                                              /* getting up */
            const float u = (t - 6.5f) / 1.5f;
            ax = cosf(u * TWO_PI / 4.0f);
            az = sinf(u * TWO_PI / 4.0f);
            gy = -0.35f;
        }
        if (A < 1.0f && t >= 3.0f) {                          /* a milder fall: less of it */
            ax *= A;
            az = 1.0f + (az - 1.0f) * A;
        }
        break;
    }
    default:                                                  /* still */
        break;
    }
    const float v[6] = { ax, ay, az, gx, gy, gz };
    for (int i = 0; i < 6; i++) {
        g[i] = v[i] + (i < 3 ? na : ng) * gauss();
    }
}

/* ---------------------------------------------------------------- sim_env */

/* temperature (C), pressure (hPa - 1000), humidity (%) */
static void gen_env(float *g, float sev)
{
    const float n = S.noise * S.p.noise, dr = S.p.drift;
    const float day = advance(&S.a, 1.0f / 120.0f, TWO_PI);   /* compressed day */
    float t = 25.0f + 0.3f * dr * sinf(day);
    float p = 13.0f + 0.5f * dr * sinf(advance(&S.b, 1.0f / 300.0f, TWO_PI) + S.ph[0]);
    float h = 50.0f + 2.0f * dr * sinf(advance(&S.c, 1.0f / 180.0f, TWO_PI) + S.ph[1]);

    switch (S.state) {
    case 1: {                                                 /* heating: 90 s ramps */
        const float k = advance(&S.d, 1.0f / 90.0f, TWO_PI) / TWO_PI;
        t += 3.6f * sev * k;
        h -= 0.5f * 3.6f * sev * k;
        break;
    }
    case 2: {                                                 /* cooling fails: thermostat swings */
        const float sw = sinf(advance(&S.d, 1.0f / 40.0f, TWO_PI));
        t += 1.5f * sev * (sw > 0.0f ? 1.0f : -1.0f) * fminf(1.0f, fabsf(sw) * 3.0f);
        break;
    }
    case 3: {                                                 /* leak: pressure falls, 120 s */
        const float k = advance(&S.d, 1.0f / 120.0f, TWO_PI) / TWO_PI;
        p -= 6.0f * sev * k;
        p += 0.05f * sev * gauss();
        break;
    }
    case 4: {                                                 /* door open 10 s in every 40 s */
        const float k = advance(&S.d, 1.0f / 40.0f, TWO_PI) / TWO_PI;
        const float open = k < 0.25f ? 1.0f - expf(-k * 40.0f * 0.5f) : expf(-(k - 0.25f) * 40.0f * 0.3f);
        t -= 2.0f * sev * open;
        h += 10.0f * sev * open;
        break;
    }
    case 5:                                                   /* sensor fault: stuck readings, spikes */
        g[0] = t + 0.02f * n * gauss();
        g[1] = p + 0.03f * n * gauss();
        g[2] = h + 0.1f * n * gauss();
        if (S.event_left > 0.0f) {                            /* stuck: the same value, no noise */
            S.event_left -= 1.0f;
            g[0] = S.hold[0];
            g[2] = S.hold[2];
        } else if (urand() < 0.1f * sev / S.rate) {
            S.event_left = uniform(3.0f, 10.0f) * S.rate;
            S.hold[0] = g[0];
            S.hold[2] = g[2];
        }
        if (urand() < 0.3f * sev / S.rate) {
            g[0] += (urand() < 0.5f ? -1.0f : 1.0f) * uniform(3.0f, 8.0f) * sev;
        }
        if (urand() < 0.3f * sev / S.rate) {
            g[2] += (urand() < 0.5f ? -1.0f : 1.0f) * uniform(8.0f, 20.0f) * sev;
        }
        return;
    default:
        break;
    }
    g[0] = t + 0.02f * n * gauss();
    g[1] = p + 0.03f * n * gauss();
    g[2] = h + 0.1f * n * gauss();
}

/* ---------------------------------------------------------------- sim_motor */

#define MOTOR_F    60.0f                /* supply, Hz */
#define MOTOR_SLIP 0.03f
#define MOTOR_BRG  104.0f               /* bearing defect torque ripple, Hz */

static void gen_motor(float *g, float sev)
{
    const float n = 0.02f * S.noise * S.p.noise;

    if (S.state == 0) {
        for (int k = 0; k < 3; k++) {
            g[k] = 0.5f * n * gauss();
        }
        return;
    }
    const float fs = MOTOR_F * (1.0f + 0.003f * (wander(100.0f) - 1.0f));   /* +-0.3 Hz */
    const float th = advance(&S.a, fs, TWO_PI);
    const float lo = advance(&S.b, fs * (1.0f - 2.0f * MOTOR_SLIP), TWO_PI);
    const float hi = advance(&S.c, fs * (1.0f + 2.0f * MOTOR_SLIP), TWO_PI);
    const float bm = advance(&S.d, MOTOR_BRG, TWO_PI);
    const float load = 5.0f * S.level * (1.0f + 0.02f * S.p.drift * sinf(S.drift_ph + S.ph[2]));
    float amp[3] = { 1.0f, 1.0f, 1.0f }, shift[3] = { 0.0f, 0.0f, 0.0f }, harm = 1.0f;

    switch (S.state) {
    case 2:                                                   /* overload */
        amp[0] = amp[1] = amp[2] = 1.0f + 0.8f * sev;
        harm = 1.0f + 2.0f * sev;
        break;
    case 3:                                                   /* phase loss: C opens */
        amp[2] = 1.0f - sev;
        amp[0] = amp[1] = 1.0f + 0.7f * sev;
        shift[0] = 0.52f * sev;                               /* A and B swing towards 180 deg apart */
        shift[1] = -0.52f * sev;
        break;
    case 6:                                                   /* unbalanced supply */
        amp[1] = 1.0f - 0.15f * sev;
        amp[2] = 1.0f + 0.12f * sev;
        shift[2] = 0.09f * sev;
        break;
    default:
        break;
    }
    for (int k = 0; k < 3; k++) {
        const float pk = th - (float)k * TWO_PI / 3.0f + shift[k];
        float i = sinf(pk) + 0.03f * harm * sinf(5.0f * pk + S.ph[3]) +
                  0.02f * harm * sinf(7.0f * pk + S.ph[4]);
        if (S.state == 4) {                                   /* broken rotor bar: sidebands */
            const float off = (float)k * TWO_PI / 3.0f;
            i += 0.08f * sev * (sinf(lo - off) + sinf(hi - off));
        } else if (S.state == 5) {                            /* bearing: torque ripple */
            i *= 1.0f + 0.06f * sev * sinf(bm + S.ph[5]);
        }
        g[k] = load * amp[k] * i + n * gauss();
    }
}

/* ---------------------------------------------------------------- sim_sound */

static void gen_sound(float *g, float sev)
{
    const float nyq = 0.45f * S.rate;
    float x = 0.003f * S.noise * S.p.noise * gauss();

    switch (S.state) {
    case 1: {                                                 /* hum */
        const float ph = advance(&S.a, 120.0f * wander(1.0f), TWO_PI);
        x += 0.05f * sev * S.level * (sinf(ph) + 0.6f * sinf(2.0f * ph + S.ph[0]) +
                                       0.3f * sinf(3.0f * ph + S.ph[1]));
        x += 0.006f * sev * gauss();
        break;
    }
    case 2: {                                                 /* alarm: 0.2 s beeps */
        const float on = advance(&S.b, 2.5f, TWO_PI) < TWO_PI / 2.0f ? 1.0f : 0.0f;
        x += 0.2f * sev * on * sinf(advance(&S.a, fminf(2400.0f, nyq), TWO_PI));
        break;
    }
    case 3: {                                                 /* knock: ~3 per second */
        float imp = 0.0f;
        if (--S.next_event <= 0.0f) {
            imp = uniform(0.3f, 0.5f) * sev;
            S.next_event = S.rate * uniform(0.15f, 0.5f);
        }
        x += reso_step(&S.reso[0], imp);
        break;
    }
    case 4:                                                   /* hiss: a leak, 2-3.5 kHz */
        x += 0.25f * sev * band_step(&S.band, gauss());
        break;
    case 5: {                                                 /* voice-like: pitch through formants */
        const float pitch = 150.0f * (1.0f + 0.15f * sinf(advance(&S.c, 0.7f, TWO_PI)));
        float pulse = 0.0f;
        S.d += pitch / S.rate;
        if (S.d >= 1.0f) {
            S.d -= 1.0f;
            pulse = 1.0f;
        }
        const float syl = fmaxf(0.0f, sinf(advance(&S.b, 4.0f, TWO_PI)));
        const float v = reso_step(&S.reso[0], pulse) + 0.6f * reso_step(&S.reso[1], pulse) +
                        0.3f * reso_step(&S.reso[2], pulse);
        x += 0.15f * sev * syl * v;
        break;
    }
    default:
        break;
    }
    g[0] = x;
}

/* ---------------------------------------------------------------- control */

const sim_type_info_t *sim_type_info(sim_type_t t)
{
    return t < SIM_TYPES ? &s_types[t] : NULL;
}

int sim_find(const char *name)
{
    for (int i = 0; i < SIM_TYPES; i++) {
        if (!strcmp(name, s_types[i].name)) {
            return i;
        }
    }
    return -1;
}

const sim_params_t *sim_params(void)
{
    return &S.p;
}

sim_type_t sim_type(void)
{
    return S.type;
}

/* filters that depend on the rate and the state */
static void arm_state(void)
{
    const sim_type_info_t *ti = &s_types[S.type];
    const float nyq = 0.45f * S.rate;

    S.n_state = 0;
    S.next_event = 1.0f;
    S.event_left = 0.0f;
    if (S.type == SIM_VIB) {
        reso_init(&S.reso[0], fminf(VIB_RING, nyq), 60.0f);
    } else if (S.type == SIM_SOUND) {
        if (!strcmp(ti->states[S.state], "knock")) {
            reso_init(&S.reso[0], fminf(1500.0f, nyq), 40.0f);
        } else if (!strcmp(ti->states[S.state], "voice")) {
            reso_init(&S.reso[0], fminf(700.0f, nyq), 80.0f);
            reso_init(&S.reso[1], fminf(1200.0f, nyq), 100.0f);
            reso_init(&S.reso[2], fminf(2600.0f, nyq), 150.0f);
        }
        band_init(&S.band, fminf(2800.0f, nyq), 1.5f);
    }
}

sim_params_t sim_default_params(void)
{
    const sim_params_t p = { 0u, 0u, 1.0f, 1.0f, 1.0f, 0.0f };
    return p;
}

void sim_start(sim_type_t t, const sim_params_t *p, uint32_t seed)
{
    const sim_type_info_t *ti = &s_types[t < SIM_TYPES ? t : SIM_VIB];

    memset(&S, 0, sizeof(S));
    S.type = t < SIM_TYPES ? t : SIM_VIB;
    S.rng = seed | 1u;
    S.p = p ? *p : sim_default_params();
    if (S.p.rate == 0) {
        S.p.rate = ti->rate;
    }
    S.p.rate = S.p.rate < ti->min_rate ? ti->min_rate : S.p.rate > ti->max_rate ? ti->max_rate : S.p.rate;
    if (S.p.channels == 0 || S.p.channels > ti->channels) {
        S.p.channels = ti->channels;
    }
    S.p.noise = fminf(fmaxf(S.p.noise, 0.0f), 10.0f);
    S.p.drift = fminf(fmaxf(S.p.drift, 0.0f), 10.0f);
    S.p.severity = fminf(fmaxf(S.p.severity, 0.0f), 1.0f);
    S.p.ramp_s = fminf(fmaxf(S.p.ramp_s, 0.0f), 3600.0f);
    S.rate = (float)S.p.rate;
    S.state = ti->n_states > 1 ? 1u : 0u;
    draw();
    arm_state();
}

int sim_set_state(const char *name)
{
    const sim_type_info_t *ti = &s_types[S.type];

    for (uint32_t i = 0; i < ti->n_states; i++) {
        if (!strcmp(name, ti->states[i])) {
            S.state = i;
            draw();
            arm_state();
            return 0;
        }
    }
    return -1;
}

void sim_next_state(void)
{
    S.state = (S.state + 1u) % s_types[S.type].n_states;
    draw();
    arm_state();
}

const char *sim_state(void)
{
    return s_types[S.type].states[S.state];
}

void sim_generate(int16_t *dst, uint32_t frames)
{
    const sim_type_info_t *ti = &s_types[S.type];
    const uint32_t ch = S.p.channels;
    float g[SIM_MAX_CH];

    for (uint32_t i = 0; i < frames; i++) {
        if (++S.since_draw >= (uint32_t)(REDRAW * S.rate)) {
            draw();
        }
        S.level += (S.level_to - S.level) / (2.0f * S.rate);      /* ~2 s glide */
        const float sev = sim_severity_now();
        S.n_state++;
        switch (S.type) {
        case SIM_IMU:   gen_imu(g, sev);   break;
        case SIM_ENV:   gen_env(g, sev);   break;
        case SIM_MOTOR: gen_motor(g, sev); break;
        case SIM_SOUND: gen_sound(g, sev); break;
        default:        gen_vib(g, sev);   break;
        }
        for (uint32_t c = 0; c < ch; c++) {
            const float v = roundf(g[c] * ti->lsb_per_unit);
            dst[i * ch + c] = (int16_t)(v > 32767.0f ? 32767 : v < -32768.0f ? -32768 : v);
        }
    }
}

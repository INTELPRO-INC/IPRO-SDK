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

#ifndef __VIBRATION_ANALYSIS_H__
#define __VIBRATION_ANALYSIS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* Configuration                                                      */
/* ------------------------------------------------------------------ */
#define VIB_FFT_SIZE        2048    /* Radix-2 FFT length (zero-pad from 2000) */
#define VIB_SAMPLE_RATE     2000    /* Hz — actual rate depends on I2C speed */
#define VIB_NUM_BANDS       10      /* Configurable frequency bands */
#define VIB_NUM_AXES        3       /* X, Y, Z */

/* ------------------------------------------------------------------ */
/* Frequency band definition                                          */
/* ------------------------------------------------------------------ */
typedef struct {
    float freq_lo;      /* Lower frequency bound (Hz) */
    float freq_hi;      /* Upper frequency bound (Hz) */
} vib_band_t;

/* Order-based band indices (bands 0-2, recalculated from RPM) */
#define VIB_ORDER_BAND_FIRST  0
#define VIB_ORDER_BAND_LAST   2
#define VIB_ORDER_BAND_COUNT  3

/* Default bands matching Robust Sensor spec (page 6 of PDF) */
/* Band 01-03 (index 0-2): Order-based — computed at runtime from RPM, init to {0,0}
 * Band 04-10 (index 3-9): Fixed Hz bands */
#define VIB_DEFAULT_BANDS { \
    {   0.0f,     0.0f },  /* Band 01: 1X order (RPM-based, recalculated) */ \
    {   0.0f,     0.0f },  /* Band 02: 2X order (RPM-based, recalculated) */ \
    {   0.0f,     0.0f },  /* Band 03: 3X order (RPM-based, recalculated) */ \
    {   5.0f,   200.0f },  /* Band 04: 5-200 Hz      */ \
    { 201.0f,   500.0f },  /* Band 05: 201-500 Hz    */ \
    { 501.0f,  1000.0f },  /* Band 06: 501-1000 Hz   */ \
    {1001.0f,  2000.0f },  /* Band 07: 1001-2000 Hz  */ \
    {2001.0f,  3000.0f },  /* Band 08: 2001-3000 Hz  */ \
    {3001.0f,  4000.0f },  /* Band 09: 3001-4000 Hz  */ \
    {4001.0f,  5000.0f },  /* Band 10: 4001-5000 Hz  */ \
}

/* ------------------------------------------------------------------ */
/* Time-domain features (per axis, per 1-second window)               */
/* ------------------------------------------------------------------ */
typedef struct {
    float rms;          /* Overall Acceleration RMS (OA) — ISO-10816 style */
    float peak;         /* Maximum absolute value */
    float peak_to_peak; /* Max - Min */
    float crest_factor; /* Peak / RMS */
    float mean;         /* DC offset */
    float variance;     /* Signal variance */
} vib_time_features_t;

/* ------------------------------------------------------------------ */
/* Frequency-domain features (per axis)                               */
/* ------------------------------------------------------------------ */
typedef struct {
    float band_power[VIB_NUM_BANDS];    /* Power in each frequency band */
    float dominant_freq;                /* Frequency with max amplitude */
    float dominant_amp;                 /* Amplitude at dominant frequency */
} vib_freq_features_t;

/* ------------------------------------------------------------------ */
/* Complete feature set for one measurement window (1 second)         */
/* ------------------------------------------------------------------ */
typedef struct {
    uint32_t timestamp;                         /* Tick count at window start */
    uint16_t sample_count;                      /* Actual samples collected */

    vib_time_features_t time[VIB_NUM_AXES];     /* X=0, Y=1, Z=2 */
    vib_freq_features_t freq[VIB_NUM_AXES];     /* X=0, Y=1, Z=2 */
} vib_features_t;

/* ------------------------------------------------------------------ */
/* API                                                                */
/* ------------------------------------------------------------------ */

/**
 * @brief  Initialize vibration analysis module.
 *         Allocates FFT working buffers in PSRAM.
 * @param  bands  Array of VIB_NUM_BANDS frequency band definitions.
 *                Pass NULL to use VIB_DEFAULT_BANDS.
 * @return 0 on success, -1 on error
 */
int vib_init(const vib_band_t *bands);

/**
 * @brief  Set/update frequency band configuration.
 */
void vib_set_bands(const vib_band_t *bands);

/**
 * @brief  Set shaft RPM for order-based band calculation.
 *         Computes bands 0-2 as N*order_freq ± 0.5*order_freq where
 *         order_freq = rpm / 60.  Disables auto-RPM detection.
 * @param  rpm  Shaft speed in RPM (>0)
 */
void vib_set_rpm(float rpm);

/**
 * @brief  Get current RPM estimate (auto-detected or manually set).
 * @return RPM value, or 0.0f if not yet estimated.
 */
float vib_get_rpm(void);

/**
 * @brief  Enable or disable automatic RPM detection.
 *         When enabled (default), vib_compute() updates RPM from the
 *         Z-axis dominant frequency (5-200 Hz range) each window.
 *         When disabled (e.g. after vib_set_rpm()), RPM is fixed.
 * @param  enable  true = auto-detect, false = use manually set RPM
 */
void vib_set_auto_rpm(bool enable);

/**
 * @brief  Compute all features from a window of raw samples.
 *         Expects interleaved accel data: [ax0,ay0,az0, ax1,ay1,az1, ...]
 * @param  accel_x   Array of int16 accel X samples
 * @param  accel_y   Array of int16 accel Y samples
 * @param  accel_z   Array of int16 accel Z samples
 * @param  n_samples Number of samples (typically 2000)
 * @param  accel_scale  Conversion factor: raw → m/s² (= 9.80665 / sensitivity)
 * @param  out       Output feature structure
 * @return 0 on success, -1 on error
 */
int vib_compute(const int16_t *accel_x,
                const int16_t *accel_y,
                const int16_t *accel_z,
                uint16_t n_samples,
                float accel_scale,
                vib_features_t *out);

/**
 * @brief  Print features as CSV header (column names).
 */
void vib_print_csv_header(void);

/**
 * @brief  Print one feature set as a CSV data line.
 */
void vib_print_csv(const vib_features_t *f);

/**
 * @brief  Print human-readable feature summary.
 */
void vib_print_summary(const vib_features_t *f);

#ifdef __cplusplus
}
#endif

#endif /* __VIBRATION_ANALYSIS_H__ */

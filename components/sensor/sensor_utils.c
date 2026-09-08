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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>


#include "sensor_utils.h"

#include <stdint.h>
#include <limits.h>

#define DB_STEP_Q   1536  // 1EV = 6dB
#define ALPHA_Q         12      // Q12 fractional
#define ALPHA_ONE       (1 << ALPHA_Q)

#define ABSI(x)     ((x) < 0 ? -(x) : (x))

int _clip(int min, int max, int data)
{
    if (data < min) return min;
    if (data > max) return max;
    return data;
}

#if 0
/*typedef struct {
    int position;
    int value;
} intpl_point_t;

#define DIV_ROUND_SIGNED(divident, divider) \
    ((((divident) < 0) ^ ((divider) < 0)) ? \
    (((divident) - (divider)/2)/(divider)) : (((divident) + (divider)/2)/(divider))) \


*/

static intpl_point_t create_intpl_point(int position, int value)
{
    intpl_point_t intpl_point;
    intpl_point.position = position;
    intpl_point.value = value;

    return intpl_point;
}

static int linear_interpolate(intpl_point_t *point_L, intpl_point_t *point_H, int pos, int *value)
{
    if (point_H->position <= point_L->position) {
        printf("linear_interpolate: invalid param!\r\n");
        return -1;
    }

    if (pos <= point_L->position) {
        *value = point_L->value;
    } else if (pos >= point_H->position) {
        *value = point_H->value;
    } else {
        *value = DIV_ROUND_SIGNED(
                (point_H->position - pos) * point_L->value +
                (pos - point_L->position) * point_H->value,
                point_H->position - point_L->position);
    }

    //printf("linear_interpolate: (%d, %d) ~ (%d, %d) -> (%d, %d)\r\n", point_L->position, point_L->value,
    //    point_H->position, point_H->value, pos, *value);

    return 0;
}
#endif

static inline int sign_floor_div(int a, int b)
{
    // b > 0
    if (a >= 0) return a / b;
    // 讓結果往 -inf（例如 -1/1536 => -1）
    return -(((-a) + b - 1) / b);
}

// 線性插值：v = v0 + (v1 - v0) * alpha，alpha 是 Q12(0..4096)
// 以 int64 計算避免 overflow，含四捨五入
static inline int lerp_q(int v0, int v1, int alpha_q)
{
    int64_t dv = (int64_t)(v1 - v0);
    int64_t t  = dv * alpha_q;
    // rounding
    if (t >= 0) t += (ALPHA_ONE / 2);
    else        t -= (ALPHA_ONE / 2);
    return v0 + (int)(t >> ALPHA_Q);
}


#if 1
int ana_ae_gain(int gain_db,
                const int *expo_map,
                const int *global_map,
                const int *digital_map,
                uint16_t *expo,
                uint16_t *global_gain,
                uint8_t  *digital_gain)
{
    const int n = EXPO_GAIN_TBL_NUM;  // 21
    const int base_idx = n / 2;       // 10

    int sys_gain_val = _clip(SYS_GAIN_DB_MIN, SYS_GAIN_DB_MAX, gain_db / 256);
    if (sys_gain_val == SYS_GAIN_DB_MIN || sys_gain_val == SYS_GAIN_DB_MAX) {
        gain_db = sys_gain_val * 256;
    }

    int step = sign_floor_div(gain_db, DB_STEP_Q);
    int idx  = base_idx + step;
    idx = _clip(0, n - 2, idx);

    int gain_L = (idx - base_idx) * DB_STEP_Q;
    int delta  = gain_db - gain_L;
    if (delta < 0) delta = 0;
    if (delta > DB_STEP_Q) delta = DB_STEP_Q;

    int alpha_q = (delta * ALPHA_ONE + (DB_STEP_Q / 2)) / DB_STEP_Q;

    int expo_val        = lerp_q(expo_map[idx],   expo_map[idx + 1],   alpha_q);
    int global_gain_val = lerp_q(global_map[idx], global_map[idx + 1], alpha_q);
    int digital_gain_val= lerp_q(digital_map[idx],digital_map[idx + 1],alpha_q);

    if (expo_val < 0) expo_val = 0;
    if (global_gain_val < 0) global_gain_val = 0;
    if (digital_gain_val < 0) digital_gain_val = 0;

    *expo         = (uint16_t)expo_val;
    *global_gain  = (uint16_t)global_gain_val;
    *digital_gain = (uint8_t) digital_gain_val;

    return 0;
}


#else
int ana_ae_gain(int gain_db, int *expo_map, int *global_map, int *digital_map, uint16_t *expo, uint16_t *global_gain, uint8_t *digital_gain)
{
    int base_idx = EXPO_GAIN_TBL_NUM / 2;//10
    int sys_gain_val = _clip(SYS_GAIN_DB_MIN, SYS_GAIN_DB_MAX, gain_db / 256);
    if ((sys_gain_val == SYS_GAIN_DB_MIN) || (sys_gain_val == SYS_GAIN_DB_MAX)) gain_db = sys_gain_val * 256;

    int sys_gain_idx = sys_gain_val / 6;

    if (gain_db < 0) sys_gain_idx += (base_idx - 1);
    else sys_gain_idx += base_idx;

    int expo_val = 0;
    int global_gain_val = 0;
    int digital_gain_val = 0;
    int gain_L, gain_H;
    intpl_point_t point_L, point_H;
    //printf("gain_db %d, sys_gain_val %d, sys_gain_idx %d\r\n", gain_db, sys_gain_val, sys_gain_idx);

    gain_L = (sys_gain_idx - base_idx) * 1536;
    gain_H = gain_L + 1536;
    point_L = create_intpl_point(gain_L, expo_map[sys_gain_idx]);
    point_H = create_intpl_point(gain_H, expo_map[sys_gain_idx+1]);
    linear_interpolate(&point_L, &point_H, gain_db, &expo_val);

    point_L = create_intpl_point(gain_L, global_map[sys_gain_idx]);
    point_H = create_intpl_point(gain_H, global_map[sys_gain_idx+1]);
    linear_interpolate(&point_L, &point_H, gain_db, &global_gain_val);

    point_L = create_intpl_point(gain_L, digital_map[sys_gain_idx]);
    point_H = create_intpl_point(gain_H, digital_map[sys_gain_idx+1]);
    linear_interpolate(&point_L, &point_H, gain_db, &digital_gain_val);

    *expo = expo_val;
    *global_gain = global_gain_val;
    *digital_gain = digital_gain_val;

    return 0;
}
#endif
#if 0

/*
 * 找最接近 route index：
 * 1) 先找 |expo - expo_map[i]| 最小
 * 2) 若多個 i 同樣接近 expo（例如 expo_map 重複），再比 global_gain
 * 3) 再比 digital_gain
 */
static int find_route_idx(uint16_t expo, uint16_t global_gain, uint8_t digital_gain,
                          const int *expo_map, const int *global_map, const int *digital_map,
                          int n)
{
    int best = 0;
    int best_de = INT_MAX, best_dg = INT_MAX, best_dd = INT_MAX;

    for (int i = 0; i < n; i++) {
        int de = ABSI((int)expo - expo_map[i]);
        int dg = ABSI((int)global_gain - global_map[i]);
        int dd = ABSI((int)digital_gain - digital_map[i]);

        if (de < best_de ||
           (de == best_de && dg < best_dg) ||
           (de == best_de && dg == best_dg && dd < best_dd)) {
            best = i;
            best_de = de;
            best_dg = dg;
            best_dd = dd;

            // 若完全一致可提前結束
            if (de == 0 && dg == 0 && dd == 0)
                break;
        }
    }
    return best;
}


int ana_ae_gain_from_expo(uint16_t expo, uint16_t global_gain, uint8_t digital_gain,
                          const int *expo_map, const int *global_map, const int *digital_map,
                          int *value)
{
    const int n = EXPO_GAIN_TBL_NUM;      // 21
    const int base_idx = n / 2;           // 10

    int idx = find_route_idx(expo, global_gain, digital_gain,
                             expo_map, global_map, digital_map, n);

    int gain_db = (idx - base_idx) * DB_STEP_Q;

    int lo = -base_idx * DB_STEP_Q;
    int hi = (n - 1 - base_idx) * DB_STEP_Q;
    *value = _clip(lo, hi, gain_db);

    return 0;
}
#else

int ana_ae_gain_from_expo(uint16_t expo, uint16_t global_gain, uint8_t digital_gain,
                          const int *expo_map, const int *global_map, const int *digital_map,
                          int *value)
{
    int gain_db = 0, i = 0;
    int base_idx = EXPO_GAIN_TBL_NUM / 2;//10

    for (i = 12; i > 0; i--) {
        if (expo >= expo_map[i]) {
            gain_db = 1536 * (i - base_idx);
            break;
        }
    }

#ifdef CONFIG_SINGLE_PRECISION_FLOAT
    float z = (float)global_gain / 32.0f;
    float x = log2f(z);
#else
    double z = (double)global_gain / 32;
    double x = log(z) / log(2);  // log base 2
#endif
    //printf("x %f, gain_db %d \r\n", x, gain_db);
    gain_db += (x * 1536) - 1536;
    
    int lo = -base_idx * DB_STEP_Q;
    int hi = (EXPO_GAIN_TBL_NUM - 1 - base_idx) * DB_STEP_Q;
    *value = _clip(lo, hi, gain_db);

    return 0;
}
#endif

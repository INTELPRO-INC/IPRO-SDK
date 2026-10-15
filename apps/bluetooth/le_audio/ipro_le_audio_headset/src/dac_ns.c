/**
 * @file dac_ns.c
 * @brief 16x oversampling noise shaper for the 10-bit GPDAC (see dac_ns.h)
 *
 * Copyright (c) 2026 INTELPRO
 */

#include "dac_ns.h"

/*
 * Working unit: 1/256 DAC code. An input sample (1/16 code) times the
 * interpolation step count (16) lands there directly.
 */
#define NS_FRAC_BITS        8

#if (DAC_NS_OSR << DAC_NS_IN_SHIFT) != (1 << NS_FRAC_BITS)
#error "interpolation step must land on 1/256 code"
#endif

/* Numerical Recipes LCG: one multiply per output; only the top byte is used. */
#define NS_LCG_A            1664525u
#define NS_LCG_C            1013904223u

#if defined(__riscv)
#define DAC_NS_FAST         __attribute__((section(".tcm_code.dac_ns_run")))
#else
#define DAC_NS_FAST
#endif

void dac_ns_reset(dac_ns_t *ns, uint8_t order)
{
    ns->prev  = 0;
    ns->e1    = 0;
    ns->e2    = 0;
    ns->rng   = 0x9E3779B9u;
    ns->order = (order == 2) ? 2 : 1;
}

/*
 * q = floor((v + u) / 256) with u uniform in 0..255 is rounding with
 * rectangular +-1/2 code dither, so the error e = 256 q - v stays within one
 * code. Error feedback: order 1 subtracts e[n-1], order 2 subtracts
 * 2 e[n-1] - e[n-2], giving NTF (1 - z^-1)^order.
 */
void DAC_NS_FAST dac_ns_run(dac_ns_t *ns, const int16_t *in, uint32_t n, uint16_t *out)
{
    int32_t  prev = ns->prev;
    int32_t  e1 = ns->e1;
    int32_t  e2 = ns->e2;
    uint32_t r = ns->rng;

    if (ns->order == 2) {
        for (uint32_t i = 0; i < n; i++) {
            const int32_t x = in[i];
            const int32_t step = x - prev;
            int32_t acc = prev * DAC_NS_OSR;

            for (int j = 0; j < DAC_NS_OSR; j++) {
                acc += step;
                const int32_t v = acc - 2 * e1 + e2;
                r = r * NS_LCG_A + NS_LCG_C;
                const int32_t q = (v + (int32_t)(r >> 24)) >> NS_FRAC_BITS;
                e2 = e1;
                e1 = q * (1 << NS_FRAC_BITS) - v;
                *out++ = (uint16_t)(q + 512);
            }
            prev = x;
        }
    } else {
        for (uint32_t i = 0; i < n; i++) {
            const int32_t x = in[i];
            const int32_t step = x - prev;
            int32_t acc = prev * DAC_NS_OSR;

            for (int j = 0; j < DAC_NS_OSR; j++) {
                acc += step;
                const int32_t v = acc - e1;
                r = r * NS_LCG_A + NS_LCG_C;
                const int32_t q = (v + (int32_t)(r >> 24)) >> NS_FRAC_BITS;
                e1 = q * (1 << NS_FRAC_BITS) - v;
                *out++ = (uint16_t)(q + 512);
            }
            prev = x;
        }
    }

    ns->prev = prev;
    ns->e1   = e1;
    ns->e2   = e2;
    ns->rng  = r;
}

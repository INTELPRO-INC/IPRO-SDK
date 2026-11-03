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

#include <string.h>
#include "hal_sec_ecc_v2.h"
#include "hal_sec_trng_v2.h"
#include "hal_sec_v2_priv.h"
#include "drv_sec_eng.h"

/*
 * PKA MMUL/MADD/MSUB time depends on operand values, not on register
 * indices. Secret-dependent operands are randomized per call.
 */

#define ECC_V2_R32         SEC_ENG_PKA_REG_SIZE_32
#define ECC_V2_WORDS       8
#define ECC_V2_BITS        256
#define ECC_V2_RAND_TRIES  64
#define ECC_V2_SIGN_TRIES  16
#define ECC_V2_RETRY       1

/* 32-byte PKA registers; MMUL takes the modulus at 0, -m^-1 at 1. */
enum {
    ECC_M,
    ECC_NP,
    ECC_R2,
    ECC_ONE,
    ECC_B,
    ECC_P0,
    ECC_P1 = ECC_P0 + 3,
    ECC_T0 = ECC_P1 + 3,
    ECC_T1,
    ECC_T2,
    ECC_T3,
    ECC_T4,
    ECC_TA,
    ECC_TB,
    ECC_NREG
};

/* Program operands: a register, or a bound point coordinate. */
enum {
    ECC_X1 = 32,
    ECC_Y1,
    ECC_Z1,
    ECC_X2,
    ECC_Y2,
    ECC_Z2,
    ECC_X3,
    ECC_Y3,
    ECC_Z3
};

enum {
    ECC_V2_MUL,
    ECC_V2_ADD,
    ECC_V2_SUB
};

struct ecc_v2_op {
    uint8_t op;
    uint8_t d;
    uint8_t a;
    uint8_t b;
};

/*
 * Complete projective addition, a = -3 (Renes-Costello-Batina 2016
 * alg. 4), Montgomery form. Valid for P == Q and for infinity
 * (0 : 1 : 0); P3 may alias P1 or P2 (inputs are read by op 15).
 */
static const struct ecc_v2_op ecc_v2_add_prog[] = {
    { ECC_V2_MUL, ECC_T0, ECC_X1, ECC_X2 },
    { ECC_V2_MUL, ECC_T1, ECC_Y1, ECC_Y2 },
    { ECC_V2_MUL, ECC_T2, ECC_Z1, ECC_Z2 },
    { ECC_V2_ADD, ECC_T3, ECC_X1, ECC_Y1 },
    { ECC_V2_ADD, ECC_T4, ECC_X2, ECC_Y2 },
    { ECC_V2_MUL, ECC_T3, ECC_T3, ECC_T4 },
    { ECC_V2_ADD, ECC_T4, ECC_T0, ECC_T1 },
    { ECC_V2_SUB, ECC_T3, ECC_T3, ECC_T4 },
    { ECC_V2_ADD, ECC_T4, ECC_Y1, ECC_Z1 },
    { ECC_V2_ADD, ECC_TA, ECC_Y2, ECC_Z2 },
    { ECC_V2_MUL, ECC_T4, ECC_T4, ECC_TA },
    { ECC_V2_ADD, ECC_TA, ECC_T1, ECC_T2 },
    { ECC_V2_SUB, ECC_T4, ECC_T4, ECC_TA },
    { ECC_V2_ADD, ECC_TA, ECC_X1, ECC_Z1 },
    { ECC_V2_ADD, ECC_TB, ECC_X2, ECC_Z2 },
    { ECC_V2_MUL, ECC_X3, ECC_TA, ECC_TB },
    { ECC_V2_ADD, ECC_Y3, ECC_T0, ECC_T2 },
    { ECC_V2_SUB, ECC_Y3, ECC_X3, ECC_Y3 },
    { ECC_V2_MUL, ECC_Z3, ECC_B, ECC_T2 },
    { ECC_V2_SUB, ECC_X3, ECC_Y3, ECC_Z3 },
    { ECC_V2_ADD, ECC_Z3, ECC_X3, ECC_X3 },
    { ECC_V2_ADD, ECC_X3, ECC_X3, ECC_Z3 },
    { ECC_V2_SUB, ECC_Z3, ECC_T1, ECC_X3 },
    { ECC_V2_ADD, ECC_X3, ECC_T1, ECC_X3 },
    { ECC_V2_MUL, ECC_Y3, ECC_B, ECC_Y3 },
    { ECC_V2_ADD, ECC_T1, ECC_T2, ECC_T2 },
    { ECC_V2_ADD, ECC_T2, ECC_T1, ECC_T2 },
    { ECC_V2_SUB, ECC_Y3, ECC_Y3, ECC_T2 },
    { ECC_V2_SUB, ECC_Y3, ECC_Y3, ECC_T0 },
    { ECC_V2_ADD, ECC_T1, ECC_Y3, ECC_Y3 },
    { ECC_V2_ADD, ECC_Y3, ECC_T1, ECC_Y3 },
    { ECC_V2_ADD, ECC_T1, ECC_T0, ECC_T0 },
    { ECC_V2_ADD, ECC_T0, ECC_T1, ECC_T0 },
    { ECC_V2_SUB, ECC_T0, ECC_T0, ECC_T2 },
    { ECC_V2_MUL, ECC_T1, ECC_T4, ECC_Y3 },
    { ECC_V2_MUL, ECC_T2, ECC_T0, ECC_Y3 },
    { ECC_V2_MUL, ECC_Y3, ECC_X3, ECC_Z3 },
    { ECC_V2_ADD, ECC_Y3, ECC_Y3, ECC_T2 },
    { ECC_V2_MUL, ECC_X3, ECC_T3, ECC_X3 },
    { ECC_V2_SUB, ECC_X3, ECC_X3, ECC_T1 },
    { ECC_V2_MUL, ECC_Z3, ECC_T4, ECC_Z3 },
    { ECC_V2_MUL, ECC_T1, ECC_T3, ECC_T0 },
    { ECC_V2_ADD, ECC_Z3, ECC_Z3, ECC_T1 },
};

/* T0 = y^2 - (x^3 - 3x + b) for affine (X1, Y1), Montgomery form. */
static const struct ecc_v2_op ecc_v2_curve_prog[] = {
    { ECC_V2_MUL, ECC_T0, ECC_Y1, ECC_Y1 },
    { ECC_V2_MUL, ECC_T1, ECC_X1, ECC_X1 },
    { ECC_V2_MUL, ECC_T1, ECC_T1, ECC_X1 },
    { ECC_V2_ADD, ECC_T2, ECC_X1, ECC_X1 },
    { ECC_V2_ADD, ECC_T2, ECC_T2, ECC_X1 },
    { ECC_V2_SUB, ECC_T1, ECC_T1, ECC_T2 },
    { ECC_V2_ADD, ECC_T1, ECC_T1, ECC_B },
    { ECC_V2_SUB, ECC_T0, ECC_T0, ECC_T1 },
};

#define ECC_V2_CONST static const uint8_t __attribute__((aligned(4)))

ECC_V2_CONST ecc_v2_p[32] = {
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};
ECC_V2_CONST ecc_v2_p_np[32] = {
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
};
ECC_V2_CONST ecc_v2_p_r2[32] = {
    0x00, 0x00, 0x00, 0x04, 0xff, 0xff, 0xff, 0xfd,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
    0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
};
ECC_V2_CONST ecc_v2_p_m2[32] = {
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd,
};
ECC_V2_CONST ecc_v2_n[32] = {
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xbc, 0xe6, 0xfa, 0xad, 0xa7, 0x17, 0x9e, 0x84,
    0xf3, 0xb9, 0xca, 0xc2, 0xfc, 0x63, 0x25, 0x51,
};
ECC_V2_CONST ecc_v2_n_np[32] = {
    0x60, 0xd0, 0x66, 0x33, 0xa9, 0xd6, 0x28, 0x1c,
    0x50, 0xfe, 0x77, 0xec, 0xc5, 0x88, 0xc6, 0xf6,
    0x48, 0xc9, 0x44, 0x08, 0x7d, 0x74, 0xd2, 0xe4,
    0xcc, 0xd1, 0xc8, 0xaa, 0xee, 0x00, 0xbc, 0x4f,
};
ECC_V2_CONST ecc_v2_n_r2[32] = {
    0x66, 0xe1, 0x2d, 0x94, 0xf3, 0xd9, 0x56, 0x20,
    0x28, 0x45, 0xb2, 0x39, 0x2b, 0x6b, 0xec, 0x59,
    0x46, 0x99, 0x79, 0x9c, 0x49, 0xbd, 0x6f, 0xa6,
    0x83, 0x24, 0x4c, 0x95, 0xbe, 0x79, 0xee, 0xa2,
};
ECC_V2_CONST ecc_v2_n_m2[32] = {
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xbc, 0xe6, 0xfa, 0xad, 0xa7, 0x17, 0x9e, 0x84,
    0xf3, 0xb9, 0xca, 0xc2, 0xfc, 0x63, 0x25, 0x4f,
};
ECC_V2_CONST ecc_v2_b[32] = {
    0x5a, 0xc6, 0x35, 0xd8, 0xaa, 0x3a, 0x93, 0xe7,
    0xb3, 0xeb, 0xbd, 0x55, 0x76, 0x98, 0x86, 0xbc,
    0x65, 0x1d, 0x06, 0xb0, 0xcc, 0x53, 0xb0, 0xf6,
    0x3b, 0xce, 0x3c, 0x3e, 0x27, 0xd2, 0x60, 0x4b,
};
ECC_V2_CONST ecc_v2_g[64] = {
    0x6b, 0x17, 0xd1, 0xf2, 0xe1, 0x2c, 0x42, 0x47,
    0xf8, 0xbc, 0xe6, 0xe5, 0x63, 0xa4, 0x40, 0xf2,
    0x77, 0x03, 0x7d, 0x81, 0x2d, 0xeb, 0x33, 0xa0,
    0xf4, 0xa1, 0x39, 0x45, 0xd8, 0x98, 0xc2, 0x96,
    0x4f, 0xe3, 0x42, 0xe2, 0xfe, 0x1a, 0x7f, 0x9b,
    0x8e, 0xe7, 0xeb, 0x4a, 0x7c, 0x0f, 0x9e, 0x16,
    0x2b, 0xce, 0x33, 0x57, 0x6b, 0x31, 0x5e, 0xce,
    0xcb, 0xb6, 0x40, 0x68, 0x37, 0xbf, 0x51, 0xf5,
};
ECC_V2_CONST ecc_v2_one[32] = { [31] = 1 };
ECC_V2_CONST ecc_v2_zero[32] = { 0 };

struct ecc_v2_mod {
    const uint8_t *m;
    const uint8_t *np;
    const uint8_t *r2;
    const uint8_t *m2;
};

static const struct ecc_v2_mod ecc_v2_fp = {
    ecc_v2_p, ecc_v2_p_np, ecc_v2_p_r2, ecc_v2_p_m2
};
static const struct ecc_v2_mod ecc_v2_fn = {
    ecc_v2_n, ecc_v2_n_np, ecc_v2_n_r2, ecc_v2_n_m2
};

/* --- constant-time 256-bit helpers on big-endian bytes --- */

/* out = a - b; returns 1 when a < b. */
static uint32_t ecc_v2_sub(uint8_t out[32], const uint8_t a[32],
                           const uint8_t b[32])
{
    uint32_t borrow = 0;

    for (int i = 31; i >= 0; i--) {
        uint32_t t = (uint32_t)a[i] - b[i] - borrow;

        out[i] = (uint8_t)t;
        borrow = (t >> 8) & 1u;
    }
    return borrow;
}

static uint32_t ecc_v2_lt(const uint8_t a[32], const uint8_t b[32])
{
    uint8_t t[32];
    uint32_t lt = ecc_v2_sub(t, a, b);

    hal_sec_v2_zeroize(t, sizeof(t));
    return lt;
}

static uint32_t ecc_v2_is_zero(const uint8_t *a, size_t len)
{
    uint32_t acc = 0;

    for (size_t i = 0; i < len; i++) {
        acc |= a[i];
    }
    return ((acc - 1u) >> 8) & 1u;
}

/* out = a mod m for a < 2m. */
static void ecc_v2_reduce(uint8_t out[32], const uint8_t a[32],
                          const uint8_t m[32])
{
    uint8_t t[32];
    uint8_t keep = (uint8_t)(0u - ecc_v2_sub(t, a, m));

    for (int i = 0; i < 32; i++) {
        out[i] = (uint8_t)((a[i] & keep) | (t[i] & (uint8_t)~keep));
    }
    hal_sec_v2_zeroize(t, sizeof(t));
}

static uint32_t ecc_v2_scalar_ok(const uint8_t d[32])
{
    return (ecc_v2_is_zero(d, 32) ^ 1u) & ecc_v2_lt(d, ecc_v2_n);
}

static uint32_t ecc_v2_coords_ok(const uint8_t q[64])
{
    return ecc_v2_lt(q, ecc_v2_p) & ecc_v2_lt(q + 32, ecc_v2_p);
}

/* SEC1 4.1.3 step 5, then mod n. */
static void ecc_v2_hash_to_e(uint8_t e[32], const uint8_t *hash,
                             size_t len)
{
    uint8_t t[32] = { 0 };

    if (len >= 32) {
        memcpy(t, hash, 32);
    } else {
        memcpy(t + 32 - len, hash, len);
    }
    ecc_v2_reduce(e, t, ecc_v2_n);
    hal_sec_v2_zeroize(t, sizeof(t));
}

/* --- PKA primitives: every arithmetic op waits for completion --- */

static void ecc_v2_mmul(uint8_t d, uint8_t a, uint8_t b)
{
    Sec_Eng_PKA_MMUL(ECC_V2_R32, d, ECC_V2_R32, a, ECC_V2_R32, b,
                     ECC_V2_R32, ECC_M, 1);
}

static void ecc_v2_madd(uint8_t d, uint8_t a, uint8_t b)
{
    Sec_Eng_PKA_MADD(ECC_V2_R32, d, ECC_V2_R32, a, ECC_V2_R32, b,
                     ECC_V2_R32, ECC_M, 1);
}

static void ecc_v2_msub(uint8_t d, uint8_t a, uint8_t b)
{
    Sec_Eng_PKA_MSUB(ECC_V2_R32, d, ECC_V2_R32, a, ECC_V2_R32, b,
                     ECC_V2_R32, ECC_M, 1);
}

/* src must be 4-byte aligned. */
static void ecc_v2_load(uint8_t reg, const void *src)
{
    Sec_Eng_PKA_Write_Data(ECC_V2_R32, reg, (const uint32_t *)src,
                           ECC_V2_WORDS, 0);
}

static void ecc_v2_read(uint8_t reg, uint32_t out[8])
{
    Sec_Eng_PKA_Read_Data(ECC_V2_R32, reg, out, ECC_V2_WORDS);
}

/* reg = src * R mod m */
static void ecc_v2_load_mont(uint8_t reg, const void *src)
{
    ecc_v2_load(reg, src);
    ecc_v2_mmul(reg, reg, ECC_R2);
}

static uint8_t ecc_v2_reg(uint8_t sym, const uint8_t bind[9])
{
    return sym >= ECC_X1 ? bind[sym - ECC_X1] : sym;
}

static void ecc_v2_run(const struct ecc_v2_op *prog, size_t len,
                       const uint8_t bind[9])
{
    for (size_t i = 0; i < len; i++) {
        uint8_t d = ecc_v2_reg(prog[i].d, bind);
        uint8_t a = ecc_v2_reg(prog[i].a, bind);
        uint8_t b = ecc_v2_reg(prog[i].b, bind);

        if (prog[i].op == ECC_V2_MUL) {
            ecc_v2_mmul(d, a, b);
        } else if (prog[i].op == ECC_V2_ADD) {
            ecc_v2_madd(d, a, b);
        } else {
            ecc_v2_msub(d, a, b);
        }
    }
}

/* Point at register base r3 = (r1) + (r2); each point is X, Y, Z. */
static void ecc_v2_add(uint8_t r3, uint8_t r1, uint8_t r2)
{
    uint8_t bind[9] = {
        r1, (uint8_t)(r1 + 1), (uint8_t)(r1 + 2),
        r2, (uint8_t)(r2 + 1), (uint8_t)(r2 + 2),
        r3, (uint8_t)(r3 + 1), (uint8_t)(r3 + 2)
    };

    ecc_v2_run(ecc_v2_add_prog,
               sizeof(ecc_v2_add_prog) / sizeof(ecc_v2_add_prog[0]), bind);
    hal_sec_v2_zeroize(bind, sizeof(bind));
}

/* 1 when the Montgomery affine point (x, y) is on the curve. */
static uint32_t ecc_v2_on_curve(uint8_t x, uint8_t y)
{
    const uint8_t bind[9] = { x, y };
    uint32_t diff[8];
    uint32_t ok;

    ecc_v2_run(ecc_v2_curve_prog,
               sizeof(ecc_v2_curve_prog) / sizeof(ecc_v2_curve_prog[0]),
               bind);
    ecc_v2_read(ECC_T0, diff);
    ok = ecc_v2_is_zero((const uint8_t *)diff, sizeof(diff));
    hal_sec_v2_zeroize(diff, sizeof(diff));
    return ok;
}

/* acc = base^e in Montgomery form; e is public. acc != base. */
static void ecc_v2_pow(uint8_t acc, uint8_t base, const uint8_t e[32])
{
    ecc_v2_mmul(acc, ECC_ONE, ECC_R2);
    for (unsigned int i = 0; i < ECC_V2_BITS; i++) {
        ecc_v2_mmul(acc, acc, acc);
        if ((e[i >> 3] >> (7 - (i & 7))) & 1u) {
            ecc_v2_mmul(acc, acc, base);
        }
    }
}

static void ecc_v2_domain(const struct ecc_v2_mod *md)
{
    ecc_v2_load(ECC_M, md->m);
    ecc_v2_load(ECC_NP, md->np);
    ecc_v2_load(ECC_R2, md->r2);
    ecc_v2_load(ECC_ONE, ecc_v2_one);
    if (md == &ecc_v2_fp) {
        ecc_v2_load_mont(ECC_B, ecc_v2_b);
    }
}

/* Affine 64-byte point to Montgomery (X : Y : 1) at base. */
static void ecc_v2_load_point(uint8_t base, const uint8_t q[64])
{
    ecc_v2_load_mont(base, q);
    ecc_v2_load_mont(base + 1, q + 32);
    ecc_v2_mmul(base + 2, ECC_ONE, ECC_R2);
}

/* Infinity (0 : 1 : 0) at base. */
static void ecc_v2_set_inf(uint8_t base)
{
    ecc_v2_load(base, ecc_v2_zero);
    ecc_v2_load(base + 2, ecc_v2_zero);
    ecc_v2_mmul(base + 1, ECC_ONE, ECC_R2);
}

/* Random value in [1, m - 1] from the TRNG; ERR_HW on TRNG failure. */
static int ecc_v2_rand_below(uint8_t k[32], const uint8_t m[32])
{
    for (int i = 0; i < ECC_V2_RAND_TRIES; i++) {
        if (hal_sec_trng_v2_read(k, 32) != HAL_SEC_V2_OK) {
            break;
        }
        if ((ecc_v2_is_zero(k, 32) ^ 1u) & ecc_v2_lt(k, m)) {
            return HAL_SEC_V2_OK;
        }
    }
    hal_sec_v2_zeroize(k, 32);
    return HAL_SEC_V2_ERR_HW;
}

/* Point at base scaled to (lX : lY : lZ) for a random l != 0 mod p. */
static int ecc_v2_rand_z(uint8_t base)
{
    uint32_t l[8];
    int ret = ecc_v2_rand_below((uint8_t *)l, ecc_v2_p);

    if (ret == HAL_SEC_V2_OK) {
        ecc_v2_load(ECC_T0, l);
        for (uint8_t i = 0; i < 3; i++) {
            ecc_v2_mmul(base + i, base + i, ECC_T0);
        }
    }
    hal_sec_v2_zeroize(l, sizeof(l));
    return ret;
}

static int ecc_v2_begin(void)
{
    if (sec_v2_lock(HAL_SEC_V2_PKA) != HAL_SEC_V2_OK) {
        return HAL_SEC_V2_ERR_TIMEOUT;
    }
    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();
    ecc_v2_domain(&ecc_v2_fp);
    return HAL_SEC_V2_OK;
}

/* PKA reset keeps the register SRAM: overwrite, clear, then reset. */
static void ecc_v2_end(void)
{
    for (uint8_t r = 0; r < ECC_NREG; r++) {
        ecc_v2_load(r, ecc_v2_zero);
        Sec_Eng_PKA_CREG(ECC_V2_R32, r, ECC_V2_WORDS, 1);
    }
    Sec_Eng_PKA_Reset();
    sec_v2_unlock(HAL_SEC_V2_PKA);
}

/*
 * kl = k + n or k + 2n, whichever has bit 256 set: 257 bits for every
 * k in [1, n - 1], and kl P = k P.
 */
static void ecc_v2_ladder_scalar(uint8_t kl[33], const uint8_t k[32])
{
    uint8_t t[33];
    uint32_t c1 = 0, c2 = 0;
    uint8_t keep;

    for (int i = 31; i >= 0; i--) {
        uint32_t a = (uint32_t)k[i] + ecc_v2_n[i] + c1;
        uint32_t b;

        t[i + 1] = (uint8_t)a;
        c1 = a >> 8;
        b = (uint32_t)t[i + 1] + ecc_v2_n[i] + c2;
        kl[i + 1] = (uint8_t)b;
        c2 = b >> 8;
    }
    t[0] = (uint8_t)c1;
    kl[0] = (uint8_t)(c1 + c2);
    keep = (uint8_t)(0u - c1);
    for (int i = 0; i < 33; i++) {
        kl[i] = (uint8_t)((t[i] & keep) | (kl[i] & (uint8_t)~keep));
    }
    hal_sec_v2_zeroize(t, sizeof(t));
}

/*
 * Montgomery ladder over the 257 bits of kl: P0 = kl P for P at P1
 * and P0 = infinity. Each bit runs the same two complete additions;
 * the bit only picks register bases, arithmetically, no branch.
 */
static void ecc_v2_ladder(const uint8_t k[32])
{
    uint8_t kl[33];

    ecc_v2_ladder_scalar(kl, k);
    for (unsigned int i = 7; i < 8u * sizeof(kl); i++) {
        uint32_t bit = (kl[i >> 3] >> (7 - (i & 7))) & 1u;
        uint8_t rb = (uint8_t)(ECC_P0 + 3u * bit);
        uint8_t rn = (uint8_t)(ECC_P1 - 3u * bit);

        ecc_v2_add(rn, ECC_P0, ECC_P1);
        ecc_v2_add(rb, rb, rb);
    }
    hal_sec_v2_zeroize(kl, sizeof(kl));
}

/* P0 to affine; ERR_HW when the result is not on the curve. */
static int ecc_v2_affine(uint32_t x[8], uint32_t y[8])
{
    uint32_t ok;

    ecc_v2_pow(ECC_T3, ECC_P0 + 2, ecc_v2_p_m2);
    ecc_v2_mmul(ECC_TA, ECC_P0, ECC_T3);
    ecc_v2_mmul(ECC_TB, ECC_P0 + 1, ECC_T3);
    ok = ecc_v2_on_curve(ECC_TA, ECC_TB);
    ecc_v2_mmul(ECC_TA, ECC_TA, ECC_ONE);
    ecc_v2_mmul(ECC_TB, ECC_TB, ECC_ONE);
    ecc_v2_read(ECC_TA, x);
    if (y) {
        ecc_v2_read(ECC_TB, y);
    }
    return ok ? HAL_SEC_V2_OK : HAL_SEC_V2_ERR_HW;
}

/*
 * out = x(kP) (out_len 32) or kP (out_len 64); P = G when pt is NULL.
 * k must be in [1, n - 1] and pt coordinates < p.
 */
static int ecc_v2_mul(const uint8_t k[32], const uint8_t *pt,
                      uint8_t *out, size_t out_len)
{
    uint32_t q[16];
    int ret;

    ret = ecc_v2_begin();
    if (ret != HAL_SEC_V2_OK) {
        return ret;
    }
    memcpy(q, pt ? pt : ecc_v2_g, sizeof(q));
    ecc_v2_load_point(ECC_P1, (const uint8_t *)q);
    if (!ecc_v2_on_curve(ECC_P1, ECC_P1 + 1)) {
        ret = pt ? HAL_SEC_V2_ERR_PARAM : HAL_SEC_V2_ERR_HW;
    } else {
        ecc_v2_set_inf(ECC_P0);
        ret = ecc_v2_rand_z(ECC_P0);
        if (ret == HAL_SEC_V2_OK) {
            ret = ecc_v2_rand_z(ECC_P1);
        }
        if (ret == HAL_SEC_V2_OK) {
            ecc_v2_ladder(k);
            ret = ecc_v2_affine(q, q + 8);
        }
    }
    ecc_v2_end();
    if (ret == HAL_SEC_V2_OK) {
        memcpy(out, q, out_len);
    }
    hal_sec_v2_zeroize(q, sizeof(q));
    return ret;
}

static int ecc_v2_rand_scalar(uint8_t k[32])
{
    return ecc_v2_rand_below(k, ecc_v2_n);
}

int hal_sec_p256_v2_check_scalar(const uint8_t d[32])
{
    if (!d) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    return ecc_v2_scalar_ok(d) ? HAL_SEC_V2_OK : HAL_SEC_V2_ERR_PARAM;
}

int hal_sec_p256_v2_check_point(const uint8_t q[64])
{
    uint32_t buf[16];
    uint32_t ok;
    int ret;

    if (!q || !ecc_v2_coords_ok(q)) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    ret = ecc_v2_begin();
    if (ret != HAL_SEC_V2_OK) {
        return ret;
    }
    memcpy(buf, q, sizeof(buf));
    ecc_v2_load_point(ECC_P1, (const uint8_t *)buf);
    ok = ecc_v2_on_curve(ECC_P1, ECC_P1 + 1);
    ecc_v2_end();
    hal_sec_v2_zeroize(buf, sizeof(buf));
    return ok ? HAL_SEC_V2_OK : HAL_SEC_V2_ERR_PARAM;
}

int hal_sec_p256_v2_pubkey(const uint8_t d[32], uint8_t q[64])
{
    if (!d || !q || !ecc_v2_scalar_ok(d)) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    return ecc_v2_mul(d, NULL, q, 64);
}

int hal_sec_p256_v2_keygen(uint8_t d[32], uint8_t q[64])
{
    int ret;

    if (!d || !q) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    ret = ecc_v2_rand_scalar(d);
    if (ret == HAL_SEC_V2_OK) {
        ret = ecc_v2_mul(d, NULL, q, 64);
    }
    if (ret != HAL_SEC_V2_OK) {
        hal_sec_v2_zeroize(d, 32);
    }
    return ret;
}

int hal_sec_p256_v2_ecdh(const uint8_t d[32], const uint8_t peer[64],
                         uint8_t x[32])
{
    if (!d || !peer || !x || !ecc_v2_scalar_ok(d) ||
        !ecc_v2_coords_ok(peer)) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    return ecc_v2_mul(d, peer, x, 32);
}

/* out = (a - b) mod n for a, b < n. */
static void ecc_v2_sub_n(uint8_t out[32], const uint8_t a[32],
                         const uint8_t b[32])
{
    uint8_t mask = (uint8_t)(0u - ecc_v2_sub(out, a, b));
    uint32_t carry = 0;

    for (int i = 31; i >= 0; i--) {
        uint32_t t = (uint32_t)out[i] + (ecc_v2_n[i] & mask) + carry;

        out[i] = (uint8_t)t;
        carry = t >> 8;
    }
}

/*
 * s = (k b)^-1 (b e + b r d1 + b r d2) mod n, d = d1 + d2; b, d1 from
 * the TRNG. Domain fn loaded; T2 = s.
 */
static int ecc_v2_sign_s(const uint8_t d[32], const uint8_t e[32],
                         const uint8_t k[32], const uint8_t r[32])
{
    uint32_t b[8], d1[8], d2[8], w[8];
    int ret = ecc_v2_rand_below((uint8_t *)b, ecc_v2_n);

    if (ret == HAL_SEC_V2_OK) {
        ret = ecc_v2_rand_below((uint8_t *)d1, ecc_v2_n);
    }
    if (ret == HAL_SEC_V2_OK) {
        ecc_v2_sub_n((uint8_t *)d2, d, (const uint8_t *)d1);
        memcpy(w, k, sizeof(w));
        ecc_v2_load_mont(ECC_P0, w);
        ecc_v2_load_mont(ECC_P0 + 1, b);
        ecc_v2_mmul(ECC_P1, ECC_P0, ECC_P0 + 1);
        ecc_v2_pow(ECC_T3, ECC_P1, ecc_v2_n_m2);
        memcpy(w, e, sizeof(w));
        ecc_v2_load(ECC_P0 + 2, w);
        memcpy(w, r, sizeof(w));
        ecc_v2_load_mont(ECC_P1 + 1, w);
        ecc_v2_load(ECC_TA, d1);
        ecc_v2_load(ECC_TB, d2);
        ecc_v2_mmul(ECC_T0, ECC_P0 + 1, ECC_P0 + 2);
        ecc_v2_mmul(ECC_T1, ECC_P0 + 1, ECC_P1 + 1);
        ecc_v2_mmul(ECC_T2, ECC_T1, ECC_TA);
        ecc_v2_mmul(ECC_T4, ECC_T1, ECC_TB);
        ecc_v2_madd(ECC_T0, ECC_T0, ECC_T2);
        ecc_v2_madd(ECC_T0, ECC_T0, ECC_T4);
        ecc_v2_mmul(ECC_T2, ECC_T0, ECC_T3);
    }
    hal_sec_v2_zeroize(b, sizeof(b));
    hal_sec_v2_zeroize(d1, sizeof(d1));
    hal_sec_v2_zeroize(d2, sizeof(d2));
    hal_sec_v2_zeroize(w, sizeof(w));
    return ret;
}

/* One attempt with nonce k; ECC_V2_RETRY when r or s is 0. */
static int ecc_v2_sign_once(const uint8_t d[32], const uint8_t e[32],
                            const uint8_t k[32], uint8_t sig[64])
{
    uint32_t w[8];
    uint8_t r[32] __attribute__((aligned(4)));
    uint8_t s[32] __attribute__((aligned(4)));
    int ret;

    ret = ecc_v2_begin();
    if (ret != HAL_SEC_V2_OK) {
        return ret;
    }
    ecc_v2_load_point(ECC_P1, ecc_v2_g);
    if (!ecc_v2_on_curve(ECC_P1, ECC_P1 + 1)) {
        ret = HAL_SEC_V2_ERR_HW;
    } else {
        ecc_v2_set_inf(ECC_P0);
        ret = ecc_v2_rand_z(ECC_P0);
        if (ret == HAL_SEC_V2_OK) {
            ret = ecc_v2_rand_z(ECC_P1);
        }
        if (ret == HAL_SEC_V2_OK) {
            ecc_v2_ladder(k);
            ret = ecc_v2_affine(w, NULL);
        }
    }
    if (ret == HAL_SEC_V2_OK) {
        ecc_v2_reduce(r, (const uint8_t *)w, ecc_v2_n);
        ecc_v2_domain(&ecc_v2_fn);
        ret = ecc_v2_sign_s(d, e, k, r);
        ecc_v2_read(ECC_T2, (uint32_t *)s);
    }
    ecc_v2_end();
    hal_sec_v2_zeroize(w, sizeof(w));
    if (ret == HAL_SEC_V2_OK) {
        if (ecc_v2_is_zero(r, 32) | ecc_v2_is_zero(s, 32)) {
            ret = ECC_V2_RETRY;
        } else {
            memcpy(sig, r, 32);
            memcpy(sig + 32, s, 32);
        }
    }
    hal_sec_v2_zeroize(r, sizeof(r));
    hal_sec_v2_zeroize(s, sizeof(s));
    return ret;
}

int hal_sec_p256_v2_sign(const uint8_t d[32], const uint8_t *hash,
                         size_t hash_len, uint8_t sig[64])
{
    uint8_t e[32] __attribute__((aligned(4)));
    uint8_t k[32];
    int ret = HAL_SEC_V2_ERR_HW;

    if (!d || !hash || !hash_len || !sig || !ecc_v2_scalar_ok(d)) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    ecc_v2_hash_to_e(e, hash, hash_len);
    for (int i = 0; i < ECC_V2_SIGN_TRIES; i++) {
        ret = ecc_v2_rand_scalar(k);
        if (ret == HAL_SEC_V2_OK) {
            ret = ecc_v2_sign_once(d, e, k, sig);
        }
        if (ret != ECC_V2_RETRY) {
            break;
        }
    }
    hal_sec_v2_zeroize(k, sizeof(k));
    hal_sec_v2_zeroize(e, sizeof(e));
    return ret == ECC_V2_RETRY ? HAL_SEC_V2_ERR_HW : ret;
}

/* u1 = e / s, u2 = r / s mod n. */
static void ecc_v2_verify_u(const uint8_t e[32], const uint8_t r[32],
                            const uint8_t s[32], uint32_t u1[8],
                            uint32_t u2[8])
{
    ecc_v2_domain(&ecc_v2_fn);
    ecc_v2_load_mont(ECC_P0, s);
    ecc_v2_pow(ECC_T3, ECC_P0, ecc_v2_n_m2);
    ecc_v2_load(ECC_P0 + 1, e);
    ecc_v2_mmul(ECC_T0, ECC_P0 + 1, ECC_T3);
    ecc_v2_load(ECC_P0 + 2, r);
    ecc_v2_mmul(ECC_T1, ECC_P0 + 2, ECC_T3);
    ecc_v2_read(ECC_T0, u1);
    ecc_v2_read(ECC_T1, u2);
    ecc_v2_domain(&ecc_v2_fp);
}

static void ecc_v2_read_point(uint8_t base, uint32_t pt[24])
{
    for (int i = 0; i < 3; i++) {
        ecc_v2_read(base + i, pt + 8 * i);
    }
}

static void ecc_v2_load_raw_point(uint8_t base, const uint32_t pt[24])
{
    for (int i = 0; i < 3; i++) {
        ecc_v2_load(base + i, pt + 8 * i);
    }
}

/*
 * Shamir: P0 = u1 G + u2 Q with table {G, Q, G + Q}; x(P0) mod n
 * into v. Public data only. ERR_VERIFY for a bad Q or infinity.
 */
static int ecc_v2_verify_sum(const uint32_t q[16], const uint32_t u1[8],
                             const uint32_t u2[8], uint8_t v[32])
{
    uint32_t tbl[3][24];
    const uint8_t *a = (const uint8_t *)u1;
    const uint8_t *b = (const uint8_t *)u2;
    uint32_t x[8];

    ecc_v2_load_point(ECC_P1, (const uint8_t *)q);
    if (!ecc_v2_on_curve(ECC_P1, ECC_P1 + 1)) {
        return HAL_SEC_V2_ERR_VERIFY;
    }
    ecc_v2_load_point(ECC_P0, ecc_v2_g);
    ecc_v2_read_point(ECC_P0, tbl[0]);
    ecc_v2_read_point(ECC_P1, tbl[1]);
    ecc_v2_add(ECC_P1, ECC_P0, ECC_P1);
    ecc_v2_read_point(ECC_P1, tbl[2]);
    ecc_v2_set_inf(ECC_P0);
    for (unsigned int i = 0; i < ECC_V2_BITS; i++) {
        unsigned int sh = 7 - (i & 7);
        unsigned int idx = ((a[i >> 3] >> sh) & 1u) |
                           (((b[i >> 3] >> sh) & 1u) << 1);

        ecc_v2_add(ECC_P0, ECC_P0, ECC_P0);
        if (idx) {
            ecc_v2_load_raw_point(ECC_P1, tbl[idx - 1]);
            ecc_v2_add(ECC_P0, ECC_P0, ECC_P1);
        }
    }
    ecc_v2_read(ECC_P0 + 2, x);
    if (ecc_v2_is_zero((const uint8_t *)x, sizeof(x))) {
        return HAL_SEC_V2_ERR_VERIFY;
    }
    ecc_v2_pow(ECC_T3, ECC_P0 + 2, ecc_v2_p_m2);
    ecc_v2_mmul(ECC_TA, ECC_P0, ECC_T3);
    ecc_v2_mmul(ECC_TA, ECC_TA, ECC_ONE);
    ecc_v2_read(ECC_TA, x);
    ecc_v2_reduce(v, (const uint8_t *)x, ecc_v2_n);
    return HAL_SEC_V2_OK;
}

int hal_sec_p256_v2_verify(const uint8_t q[64], const uint8_t *hash,
                           size_t hash_len, const uint8_t sig[64])
{
    uint32_t qw[16];
    uint32_t rs[16];
    uint32_t e[8];
    uint32_t u1[8];
    uint32_t u2[8];
    uint8_t v[32];
    int ret;

    if (!q || !hash || !hash_len || !sig) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    if (!ecc_v2_scalar_ok(sig) || !ecc_v2_scalar_ok(sig + 32) ||
        !ecc_v2_coords_ok(q)) {
        return HAL_SEC_V2_ERR_VERIFY;
    }
    memcpy(qw, q, sizeof(qw));
    memcpy(rs, sig, sizeof(rs));
    ecc_v2_hash_to_e((uint8_t *)e, hash, hash_len);
    ret = ecc_v2_begin();
    if (ret != HAL_SEC_V2_OK) {
        return ret;
    }
    ecc_v2_verify_u((const uint8_t *)e, (const uint8_t *)rs,
                    (const uint8_t *)(rs + 8), u1, u2);
    ret = ecc_v2_verify_sum(qw, u1, u2, v);
    ecc_v2_end();
    if (ret == HAL_SEC_V2_OK && memcmp(v, rs, 32) != 0) {
        ret = HAL_SEC_V2_ERR_VERIFY;
    }
    return ret;
}

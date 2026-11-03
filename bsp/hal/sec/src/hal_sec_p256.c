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

#include "hal_sec_p256.h"
#include "drv_sec_eng.h"
#include <string.h>

extern const uint8_t secp256r1P[32], secp256r1PrimeN_P[32], secp256r1_1[32];

/* 32-byte registers: 0=p, 1=-p^-1, 2..6=points, 7..11=temporaries,
 * 16=ordinary integer one, 17=R^2 mod p for fixed-cost field conversion.
 * The fixed field-operation schedules below contain no scalar-bit branches.
 */
#define R32 SEC_ENG_PKA_REG_SIZE_32
/* Complete each arithmetic operation before submitting its dependents.
 * Unbounded batches fail the silicon KAT despite passing the integer model.
 * Loads must not raise an unconsumed last-op interrupt before this barrier. */
#define MUL(d, a, b) Sec_Eng_PKA_MMUL(R32, d, R32, a, R32, b, R32, 0, 1)
#define ADD(d, a, b) Sec_Eng_PKA_MADD(R32, d, R32, a, R32, b, R32, 0, 1)
#define SUB(d, a, b) Sec_Eng_PKA_MSUB(R32, d, R32, a, R32, b, R32, 0, 1)
static const uint8_t p256_r2[32] __attribute__((aligned(4))) = {
    0x00,0x00,0x00,0x04,0xff,0xff,0xff,0xfd,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,
    0xff,0xff,0xff,0xfb,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03
};

static void erase(void *data, unsigned size)
{
    volatile uint8_t *p = data;
    while (size--) *p++ = 0;
}

static int zero(const uint32_t data[8])
{
    uint32_t bits = 0;
    for (unsigned i = 0; i < 8; ++i) bits |= data[i];
    return bits == 0;
}

static int reg_zero(unsigned reg)
{
    uint32_t data[8];
    Sec_Eng_PKA_Read_Data(R32, reg, data, 8);
    int result = zero(data);
    erase(data, sizeof(data));
    return result;
}

static void load(unsigned reg, const uint32_t data[8])
{
    Sec_Eng_PKA_Write_Data(R32, reg, data, 8, 0);
    MUL(reg, reg, 17); /* x * R^2 * R^-1 = xR, without division/MREM. */
}

static void output(sec_p256_point_t *r, unsigned x, unsigned y, unsigned z)
{
    /* Multiplication by ordinary one removes the Montgomery factor R. */
    MUL(x, x, 16);
    MUL(y, y, 16);
    MUL(z, z, 16);
    Sec_Eng_PKA_Read_Data(R32, x, r->x, 8);
    Sec_Eng_PKA_Read_Data(R32, y, r->y, 8);
    Sec_Eng_PKA_Read_Data(R32, z, r->z, 8);
}

void sec_p256_field_init(void)
{
    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();
    Sec_Eng_PKA_Write_Data(R32, 0, (const uint32_t *)secp256r1P, 8, 0);
    Sec_Eng_PKA_Write_Data(R32, 1, (const uint32_t *)secp256r1PrimeN_P, 8, 0);
    Sec_Eng_PKA_Write_Data(R32, 16, (const uint32_t *)secp256r1_1, 8, 0);
    Sec_Eng_PKA_Write_Data(R32, 17, (const uint32_t *)p256_r2, 8, 0);
}

void sec_p256_field_free(void)
{
    /* Reset only resets the engine control register; explicitly erase SRAM. */
    for (unsigned reg = 0; reg <= 17; ++reg)
        Sec_Eng_PKA_CREG(R32, reg, 8, 1);
    Sec_Eng_PKA_Reset();
}

void sec_p256_double(sec_p256_point_t *r, const sec_p256_point_t *p)
{
    load(2, p->x);
    load(3, p->y);
    load(4, p->z);
    /* Jacobian doubling for a=-3: M=3(X+Z^2)(X-Z^2), S=4XY^2. */
    MUL(8, 4, 4);
    ADD(9, 2, 8);
    SUB(10, 2, 8);
    MUL(7, 9, 10);
    ADD(9, 7, 7);
    ADD(7, 9, 7);
    MUL(9, 3, 3);
    ADD(9, 9, 9);
    MUL(8, 2, 9);
    ADD(8, 8, 8);
    MUL(10, 9, 9);
    ADD(10, 10, 10);
    MUL(9, 7, 7);
    ADD(11, 8, 8);
    SUB(9, 9, 11);
    SUB(8, 8, 9);
    MUL(8, 8, 7);
    SUB(8, 8, 10);
    MUL(10, 3, 4);
    ADD(10, 10, 10);
    output(r, 9, 8, 10);
}

void sec_p256_add_mixed(sec_p256_point_t *r, const sec_p256_point_t *p,
                        const sec_p256_point_t *q)
{
    if (zero(p->z)) {
        *r = *q;
        return;
    }
    load(2, p->x);
    load(3, p->y);
    load(4, p->z);
    load(5, q->x);
    load(6, q->y);
    MUL(7, 4, 4);
    MUL(8, 7, 4);
    MUL(7, 7, 5);
    SUB(7, 7, 2);
    MUL(8, 8, 6);
    SUB(8, 8, 3);
    /* Same exceptional cases as mbedTLS's comb-compatible mixed addition.
     * These cannot occur in its secret-dependent comb loop. */
    if (reg_zero(7)) {
        if (reg_zero(8)) {
            sec_p256_double(r, p);
        } else {
            memset(r, 0, sizeof(*r));
            ((uint8_t *)r->x)[31] = 1;
            ((uint8_t *)r->y)[31] = 1;
        }
        return;
    }
    MUL(4, 4, 7);
    MUL(9, 7, 7);
    MUL(10, 9, 7);
    MUL(9, 9, 2);
    ADD(7, 9, 9);
    MUL(2, 8, 8);
    SUB(2, 2, 7);
    SUB(2, 2, 10);
    SUB(9, 9, 2);
    MUL(9, 9, 8);
    MUL(10, 10, 3);
    SUB(3, 9, 10);
    output(r, 2, 3, 4);
}

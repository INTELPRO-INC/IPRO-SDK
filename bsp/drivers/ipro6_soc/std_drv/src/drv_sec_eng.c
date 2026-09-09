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



#include "string.h"
#include "drv_sec_eng.h"
// #include "drv_tzc_sec.h"    TODO
#include "drv_l1c.h"

#define PUT_UINT32_BE(n, b, i)               \
    {                                        \
        (b)[(i)] = (uint8_t)((n) >> 24);     \
        (b)[(i) + 1] = (uint8_t)((n) >> 16); \
        (b)[(i) + 2] = (uint8_t)((n) >> 8);  \
        (b)[(i) + 3] = (uint8_t)((n));       \
    }
#define PUT_UINT32(n, b, i)                  \
    {                                        \
        (b)[(i) + 3] = (uint8_t)((n) >> 24); \
        (b)[(i) + 2] = (uint8_t)((n) >> 16); \
        (b)[(i) + 1] = (uint8_t)((n) >> 8);  \
        (b)[(i) + 0] = (uint8_t)((n));       \
    }
#define PUT_UINT64_BE(n, b, i)               \
    {                                        \
        (b)[(i)] = (uint8_t)((n) >> 56);     \
        (b)[(i) + 1] = (uint8_t)((n) >> 48); \
        (b)[(i) + 2] = (uint8_t)((n) >> 40); \
        (b)[(i) + 3] = (uint8_t)((n) >> 32); \
        (b)[(i) + 4] = (uint8_t)((n) >> 24); \
        (b)[(i) + 5] = (uint8_t)((n) >> 16); \
        (b)[(i) + 6] = (uint8_t)((n) >> 8);  \
        (b)[(i) + 7] = (uint8_t)((n));       \
    }
#define SEC_ENG_SHA_BUSY_TIMEOUT_COUNT  (100 * 160 * 1000)
#define SEC_ENG_AES_BUSY_TIMEOUT_COUNT  (100 * 160 * 1000)
#define SEC_ENG_TRNG_BUSY_TIMEOUT_COUNT (100 * 160 * 1000)
#define SEC_ENG_PKA_INT_TIMEOUT_COUNT   (100 * 160 * 1000)
#define SEC_ENG_GMAC_BUSY_TIMEOUT_COUNT (100 * 160 * 1000)

struct pka0_pld_cfg {
    union {
        struct
        {
            uint32_t size        : 12; /*[11: 0],       r/w,        0x0 */
            uint32_t d_reg_index : 8;  /*[19:12],        r/w,        0x0 */
            uint32_t d_reg_type  : 4;  /*[23:20],        r/w,        0x0 */
            uint32_t op          : 7;  /*[30:24],        r/w,        0x0 */
            uint32_t last_op     : 1;  /*[31:31],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } value;
};

struct pka0_pldi_cfg {
    union {
        struct
        {
            uint32_t rsvd        : 12; /*[11: 0],       r/w,        0x0 */
            uint32_t d_reg_index : 8;  /*[19:12],        r/w,        0x0 */
            uint32_t d_reg_type  : 4;  /*[23:20],        r/w,        0x0 */
            uint32_t op          : 7;  /*[30:24],        r/w,        0x0 */
            uint32_t last_op     : 1;  /*[31:31],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } value;
};

struct pka0_common_op_first_cfg {
    union {
        struct
        {
            uint32_t s0_reg_idx  : 8; /*[7: 0],       r/w,        0x0 */
            uint32_t s0_reg_type : 4; /*[11:8],       r/w,        0x0 */
            uint32_t d_reg_idx   : 8; /*[19:12],      r/w,        0x0 */
            uint32_t d_reg_type  : 4; /*[23:20],      r/w,        0x0 */
            uint32_t op          : 7; /*[30:24],      r/w,        0x0 */
            uint32_t last_op     : 1; /*[31:31],      r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } value;
};

struct pka0_common_op_snd_cfg_S1_only {
    union {
        struct
        {
            uint32_t reserved_0_11  : 12; /*[11: 0],       rsvd,       0x0 */
            uint32_t s1_reg_idx     : 8;  /*[19:12],       r/w,        0x0 */
            uint32_t s1_reg_type    : 4;  /*[23:20],       r/w,        0x0 */
            uint32_t reserved_24_31 : 8;  /*[31:24],       rsvd,       0x0 */
        } BF;
        uint32_t WORD;
    } value;
};

struct pka0_common_op_snd_cfg_S2_only {
    union {
        struct
        {
            uint32_t s2_reg_idx     : 8;  /*[7 : 0],       r/w,        0x0 */
            uint32_t s2_reg_type    : 4;  /*[11: 8],       r/w,        0x0 */
            uint32_t reserved_12_31 : 20; /*[31:12],       rsvd,       0x0 */
        } BF;
        uint32_t WORD;
    } value;
};

struct pka0_common_op_snd_cfg_S1_S2 {
    union {
        struct
        {
            uint32_t s2_reg_idx     : 8; /*[7 : 0],       r/w,        0x0 */
            uint32_t s2_reg_type    : 4; /*[11: 8],       r/w,        0x0 */
            uint32_t s1_reg_idx     : 8; /*[19:12],       r/w,        0x0 */
            uint32_t s1_reg_type    : 4; /*[23:20],       r/w,        0x0 */
            uint32_t reserved_24_31 : 8; /*[31:24],       rsvd,       0x0 */
        } BF;
        uint32_t WORD;
    } value;
};

struct pka0_bit_shift_op_cfg {
    union {
        struct
        {
            uint32_t bit_shift      : 15; /*[14: 0],       r/w,        0x0 */
            uint32_t reserved_24_31 : 17; /*[31:15],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } value;
};

#ifndef IPRO_USE_HAL_DRIVER
static intCallback_Type *secEngIntCbfArra[SEC_ENG_INT_ALL] = { NULL };
#endif

void Sec_Eng_SHA256_Init(SEC_Eng_SHA256_Ctx *shaCtx, SEC_ENG_SHA_ID_Type shaNo, SEC_ENG_SHA_Type shaType, uint32_t shaTmpBuf[16], uint32_t padding[16])
{
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;

    /* Check the parameters */

    /* Deal SHA control register to set SHA mode */
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

    if (shaType < 8) {
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_MODE_EXT, 0);
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_MODE, shaType);
    } else {
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_MODE_EXT, shaType - 7);
    }

    writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

    /* Clear context */
    ARCH_MemSet(shaCtx, 0, sizeof(SEC_Eng_SHA256_Ctx));

    /* Init temp buffer and padding buffer */
    shaCtx->shaBuf = shaTmpBuf;
    shaCtx->shaPadding = padding;
    ARCH_MemSet(shaCtx->shaPadding, 0, 64);
    ARCH_MemSet(shaCtx->shaPadding, 0x80, 1);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_SHA_IRQHandler);
#endif
}

void Sec_Eng_SHA512_Init(SEC_Eng_SHA512_Ctx *shaCtx, SEC_ENG_SHA_ID_Type shaNo, SEC_ENG_SHA_Type shaType, uint64_t shaTmpBuf[16], uint64_t padding[16])
{
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;

    /* Check the parameters */

    /* Deal SHA control register to set SHA mode */
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

    if (shaType < 8) {
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_MODE_EXT, 0);
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_MODE, shaType);
    } else {
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_MODE_EXT, shaType - 7);
    }

    writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

    /* Clear context */
    ARCH_MemSet(shaCtx, 0, sizeof(SEC_Eng_SHA512_Ctx));

    /* Init temp buffer and padding buffer */
    shaCtx->shaBuf = shaTmpBuf;
    shaCtx->shaPadding = padding;
    ARCH_MemSet(shaCtx->shaPadding, 0, 128);
    ARCH_MemSet(shaCtx->shaPadding, 0x80, 1);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_SHA_IRQHandler);
    Interrupt_Handler_Register(SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_SHA_IRQHandler);
#endif
}

void Sec_Eng_MD5_Init(SEC_Eng_MD5_Ctx *md5Ctx, SEC_ENG_SHA_ID_Type shaNo, SEC_ENG_SHA_Type shaType, uint32_t md5TmpBuf[16], uint32_t padding[16])
{
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;

    /* Check the parameters */

    /* Deal SHA control register to set SHA mode */
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

    if (shaType < 8) {
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_MODE_EXT, 0);
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_MODE, shaType);
    } else {
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_MODE_EXT, shaType - 7);
    }

    writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

    /* Clear context */
    ARCH_MemSet(md5Ctx, 0, sizeof(SEC_Eng_MD5_Ctx));

    /* Init temp buffer and padding buffer */
    md5Ctx->md5Buf = md5TmpBuf;
    md5Ctx->md5Padding = padding;
    ARCH_MemSet(md5Ctx->md5Padding, 0, 64);
    ARCH_MemSet(md5Ctx->md5Padding, 0x80, 1);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_SHA_IRQHandler);
    Interrupt_Handler_Register(SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_SHA_IRQHandler);
#endif
}

void Sec_Eng_SHA_Start(SEC_ENG_SHA_ID_Type shaNo)
{
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;

    /* Check the parameters */

    /* Set SHA enable */
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_EN);
    /* Hash sel 0 for new start */
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_SHA_0_HASH_SEL);

    writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
}

int Sec_Eng_SHA256_Update(SEC_Eng_SHA256_Ctx *shaCtx, SEC_ENG_SHA_ID_Type shaNo, const uint8_t *input, uint32_t len)
{
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t fill;
    uint32_t left;
    uint32_t timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    if (len == 0) {
        return SUCCESS;
    }

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    /* SHA need set se_sha_sel to 1 to keep the last SHA state */
    tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_HASH_SEL, shaCtx->shaFeed);

    left = shaCtx->total[0] & 0x3F;
    fill = 64 - left;

    shaCtx->total[0] += (uint32_t)len;
    shaCtx->total[0] &= 0xFFFFFFFF;

    if (shaCtx->total[0] < (uint32_t)len) {
        shaCtx->total[1]++;
    }

    if (left && len >= fill) {
        ARCH_MemCpy_Fast((void *)((uint8_t *)shaCtx->shaBuf + left), input, fill);
        /* Set data source address */
        writel((uintptr_t)shaCtx->shaBuf, SHAx + SEC_ENG_SE_SHA_0_MSA_OFFSET);

        /* Set data length */
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_MSG_LEN, 1);
        writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        /* Trigger */
        tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_TRIG_1T);
        writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        shaCtx->shaFeed = 1;
        input += fill;
        len -= fill;
        left = 0;
    }

    fill = len / 64;
    len = len % 64;

    if (fill > 0) {
        /* Wait finished */
        timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

        do {
            tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return TIMEOUT;
            }
        } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

        /* SHA need set se_sha_sel to 1 to keep the last sha state */
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_HASH_SEL, shaCtx->shaFeed);

        /* Fill data */
        writel((uintptr_t)input, SHAx + SEC_ENG_SE_SHA_0_MSA_OFFSET);

        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_MSG_LEN, fill);
        writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_TRIG_1T);
        writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        input += (fill * 64);
        shaCtx->shaFeed = 1;
    }

    if (len > 0) {
        /* Wait finished */
        timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

        do {
            tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return TIMEOUT;
            }
        } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

        /* Copy left data into temp buffer */
        ARCH_MemCpy_Fast((void *)((uint8_t *)shaCtx->shaBuf + left), input, len);
    }

    /* Wait finished */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    return SUCCESS;
}

int Sec_Eng_SHA512_Update(SEC_Eng_SHA512_Ctx *shaCtx, SEC_ENG_SHA_ID_Type shaNo, const uint8_t *input, uint64_t len)
{
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t fill;
    uint32_t left;
    uint32_t timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    if (len == 0) {
        return SUCCESS;
    }

    if ((((uintptr_t)input) & 0x07) != 0) {
        return ERROR;
    }

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    /* SHA need set se_sha_sel to 1 to keep the last SHA state */
    tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_HASH_SEL, shaCtx->shaFeed);

    left = shaCtx->total[0] & 0x7F;
    fill = 128 - left;

    shaCtx->total[0] += (uint64_t)len;

    if (shaCtx->total[0] < (uint64_t)len) {
        shaCtx->total[1]++;
    }

    if (left && len >= fill) {
        ARCH_MemCpy_Fast((void *)((uint8_t *)shaCtx->shaBuf + left), input, fill);
        /* Set data source address */
        writel((uintptr_t)shaCtx->shaBuf, SHAx + SEC_ENG_SE_SHA_0_MSA_OFFSET);

        /* Set data length */
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_MSG_LEN, 1);

        /* Trigger */
        tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_TRIG_1T);
        writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        shaCtx->shaFeed = 1;
        input += fill;
        len -= fill;
        left = 0;
    }

    fill = len / 128;
    len = len % 128;

    if (fill > 0) {
        /* Wait finished */
        timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

        do {
            tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return TIMEOUT;
            }
        } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

        /* SHA need set se_sha_sel to 1 to keep the last sha state */
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_HASH_SEL, shaCtx->shaFeed);

        /* Fill data */
        writel((uintptr_t)input, SHAx + SEC_ENG_SE_SHA_0_MSA_OFFSET);

        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_MSG_LEN, fill);
        tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_TRIG_1T);
        writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        input += (fill * 128);
        shaCtx->shaFeed = 1;
    }

    if (len > 0) {
        /* Wait finished */
        timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

        do {
            tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return TIMEOUT;
            }
        } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

        /* Copy left data into temp buffer */
        ARCH_MemCpy_Fast((void *)((uint8_t *)shaCtx->shaBuf + left), input, len);
    }

    /* Wait finished */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    return SUCCESS;
}

int Sec_Eng_MD5_Update(SEC_Eng_MD5_Ctx *md5Ctx, SEC_ENG_SHA_ID_Type shaNo, const uint8_t *input, uint32_t len)
{
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t fill;
    uint32_t left;
    uint32_t timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    if (len == 0) {
        return SUCCESS;
    }

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    /* SHA need set se_sha_sel to 1 to keep the last SHA state */
    tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_HASH_SEL, md5Ctx->md5Feed);

    left = md5Ctx->total[0] & 0x3F;
    fill = 64 - left;

    md5Ctx->total[0] += (uint32_t)len;
    md5Ctx->total[0] &= 0xFFFFFFFF;

    if (md5Ctx->total[0] < (uint32_t)len) {
        md5Ctx->total[1]++;
    }

    if (left && len >= fill) {
        ARCH_MemCpy_Fast((void *)((uint8_t *)md5Ctx->md5Buf + left), input, fill);

        /* Set data source address */
        writel((uintptr_t)md5Ctx->md5Buf, SHAx + SEC_ENG_SE_SHA_0_MSA_OFFSET);

        /* Set data length */
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_MSG_LEN, 1);
        writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        /* Trigger */
        tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_TRIG_1T);
        writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        md5Ctx->md5Feed = 1;
        input += fill;
        len -= fill;
        left = 0;
    }

    fill = len / 64;
    len = len % 64;

    if (fill > 0) {
        /* Wait finished */
        timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

        do {
            tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return TIMEOUT;
            }
        } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

        /* SHA need set se_sha_sel to 1 to keep the last sha state */
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_HASH_SEL, md5Ctx->md5Feed);

        /* Fill data */
        writel((uintptr_t)input, SHAx + SEC_ENG_SE_SHA_0_MSA_OFFSET);

        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_SHA_0_MSG_LEN, fill);
        writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_TRIG_1T);
        writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        input += (fill * 64);
        md5Ctx->md5Feed = 1;
    }

    if (len > 0) {
        /* Wait finished */
        timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

        do {
            tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return TIMEOUT;
            }
        } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

        /* Copy left data into temp buffer */
        ARCH_MemCpy_Fast((void *)((uint8_t *)md5Ctx->md5Buf + left), input, len);
    }

    /* Wait finished */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    return SUCCESS;
}

int Sec_Eng_SHA256_Finish(SEC_Eng_SHA256_Ctx *shaCtx, SEC_ENG_SHA_ID_Type shaNo, uint8_t *hash)
{
    uint32_t last, padn;
    uint32_t high, low;
    uint8_t shaMode;
    uint8_t msgLen[8];
    uint8_t *p = (uint8_t *)hash;
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    /* Check the parameters */

    /* Wait finished */
    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    high = (shaCtx->total[0] >> 29) | (shaCtx->total[1] << 3);
    low = (shaCtx->total[0] << 3);

    PUT_UINT32_BE(high, msgLen, 0);
    PUT_UINT32_BE(low, msgLen, 4);

    last = shaCtx->total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    Sec_Eng_SHA256_Update(shaCtx, shaNo, (uint8_t *)shaCtx->shaPadding, padn);

    /* Wait for  shaPadding idle */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    ARCH_MemCpy_Fast(shaCtx->shaPadding, msgLen, 8);
    Sec_Eng_SHA256_Update(shaCtx, shaNo, (uint8_t *)shaCtx->shaPadding, 8);

    /* Wait finished */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    shaMode = (SEC_ENG_SHA_Type)reg_get_bits(tmpVal, SEC_ENG_SE_SHA_0_MODE);
    /* Copy SHA value */
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_0_OFFSET);
    *p++ = (tmpVal & 0xff);
    *p++ = ((tmpVal >> 8) & 0xff);
    *p++ = ((tmpVal >> 16) & 0xff);
    *p++ = ((tmpVal >> 24) & 0xff);
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_1_OFFSET);
    *p++ = (tmpVal & 0xff);
    *p++ = ((tmpVal >> 8) & 0xff);
    *p++ = ((tmpVal >> 16) & 0xff);
    *p++ = ((tmpVal >> 24) & 0xff);
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_2_OFFSET);
    *p++ = (tmpVal & 0xff);
    *p++ = ((tmpVal >> 8) & 0xff);
    *p++ = ((tmpVal >> 16) & 0xff);
    *p++ = ((tmpVal >> 24) & 0xff);
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_3_OFFSET);
    *p++ = (tmpVal & 0xff);
    *p++ = ((tmpVal >> 8) & 0xff);
    *p++ = ((tmpVal >> 16) & 0xff);
    *p++ = ((tmpVal >> 24) & 0xff);
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_4_OFFSET);
    *p++ = (tmpVal & 0xff);
    *p++ = ((tmpVal >> 8) & 0xff);
    *p++ = ((tmpVal >> 16) & 0xff);
    *p++ = ((tmpVal >> 24) & 0xff);

    if (shaMode == SEC_ENG_SHA224 || shaMode == SEC_ENG_SHA256) {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_5_OFFSET);
        *p++ = (tmpVal & 0xff);
        *p++ = ((tmpVal >> 8) & 0xff);
        *p++ = ((tmpVal >> 16) & 0xff);
        *p++ = ((tmpVal >> 24) & 0xff);
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_6_OFFSET);
        *p++ = (tmpVal & 0xff);
        *p++ = ((tmpVal >> 8) & 0xff);
        *p++ = ((tmpVal >> 16) & 0xff);
        *p++ = ((tmpVal >> 24) & 0xff);

        if (shaMode == SEC_ENG_SHA256) {
            tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_7_OFFSET);
            *p++ = (tmpVal & 0xff);
            *p++ = ((tmpVal >> 8) & 0xff);
            *p++ = ((tmpVal >> 16) & 0xff);
            *p++ = ((tmpVal >> 24) & 0xff);
        }
    }

    /* Disable SHA engine*/
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_SHA_0_HASH_SEL);
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_SHA_0_EN);
    writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

    return SUCCESS;
}

int Sec_Eng_SHA512_Finish(SEC_Eng_SHA512_Ctx *shaCtx, SEC_ENG_SHA_ID_Type shaNo, uint8_t *hash)
{
    uint64_t last, padn;
    uint64_t high, low;
    uint8_t shaMode;
    uint8_t msgLen[16];
    uint8_t *p = (uint8_t *)hash;
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    /* Check the parameters */

    /* Wait finished */
    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    high = (shaCtx->total[0] >> 61) | (shaCtx->total[1] << 3);
    low = (shaCtx->total[0] << 3);

    PUT_UINT64_BE(high, msgLen, 0);
    PUT_UINT64_BE(low, msgLen, 8);

    last = shaCtx->total[0] & 0x7F;
    padn = (last < 112) ? (112 - last) : (240 - last);

    Sec_Eng_SHA512_Update(shaCtx, shaNo, (uint8_t *)shaCtx->shaPadding, padn);

    /* Wait for  shaPadding idle */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    ARCH_MemCpy_Fast(shaCtx->shaPadding, msgLen, 16);
    Sec_Eng_SHA512_Update(shaCtx, shaNo, (uint8_t *)shaCtx->shaPadding, 16);

    /* Wait finished */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    shaMode = (SEC_ENG_SHA_Type)reg_get_bits(tmpVal, SEC_ENG_SE_SHA_0_MODE);
    /* Copy SHA value */
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_H_0_OFFSET);
    *p++ = (tmpVal & 0xff);
    *p++ = ((tmpVal >> 8) & 0xff);
    *p++ = ((tmpVal >> 16) & 0xff);
    *p++ = ((tmpVal >> 24) & 0xff);
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_0_OFFSET);
    *p++ = (tmpVal & 0xff);
    *p++ = ((tmpVal >> 8) & 0xff);
    *p++ = ((tmpVal >> 16) & 0xff);
    *p++ = ((tmpVal >> 24) & 0xff);
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_H_1_OFFSET);
    *p++ = (tmpVal & 0xff);
    *p++ = ((tmpVal >> 8) & 0xff);
    *p++ = ((tmpVal >> 16) & 0xff);
    *p++ = ((tmpVal >> 24) & 0xff);
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_1_OFFSET);
    *p++ = (tmpVal & 0xff);
    *p++ = ((tmpVal >> 8) & 0xff);
    *p++ = ((tmpVal >> 16) & 0xff);
    *p++ = ((tmpVal >> 24) & 0xff);
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_H_2_OFFSET);
    *p++ = (tmpVal & 0xff);
    *p++ = ((tmpVal >> 8) & 0xff);
    *p++ = ((tmpVal >> 16) & 0xff);
    *p++ = ((tmpVal >> 24) & 0xff);
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_2_OFFSET);
    *p++ = (tmpVal & 0xff);
    *p++ = ((tmpVal >> 8) & 0xff);
    *p++ = ((tmpVal >> 16) & 0xff);
    *p++ = ((tmpVal >> 24) & 0xff);
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_H_3_OFFSET);
    *p++ = (tmpVal & 0xff);
    *p++ = ((tmpVal >> 8) & 0xff);
    *p++ = ((tmpVal >> 16) & 0xff);
    *p++ = ((tmpVal >> 24) & 0xff);

    if (shaMode == SEC_ENG_SHA512T256 || shaMode == SEC_ENG_SHA384 || shaMode == SEC_ENG_SHA512) {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_3_OFFSET);
        *p++ = (tmpVal & 0xff);
        *p++ = ((tmpVal >> 8) & 0xff);
        *p++ = ((tmpVal >> 16) & 0xff);
        *p++ = ((tmpVal >> 24) & 0xff);

        if (shaMode == SEC_ENG_SHA384 || shaMode == SEC_ENG_SHA512) {
            tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_H_4_OFFSET);
            *p++ = (tmpVal & 0xff);
            *p++ = ((tmpVal >> 8) & 0xff);
            *p++ = ((tmpVal >> 16) & 0xff);
            *p++ = ((tmpVal >> 24) & 0xff);
            tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_4_OFFSET);
            *p++ = (tmpVal & 0xff);
            *p++ = ((tmpVal >> 8) & 0xff);
            *p++ = ((tmpVal >> 16) & 0xff);
            *p++ = ((tmpVal >> 24) & 0xff);
            tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_H_5_OFFSET);
            *p++ = (tmpVal & 0xff);
            *p++ = ((tmpVal >> 8) & 0xff);
            *p++ = ((tmpVal >> 16) & 0xff);
            *p++ = ((tmpVal >> 24) & 0xff);
            tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_5_OFFSET);
            *p++ = (tmpVal & 0xff);
            *p++ = ((tmpVal >> 8) & 0xff);
            *p++ = ((tmpVal >> 16) & 0xff);
            *p++ = ((tmpVal >> 24) & 0xff);

            if (shaMode == SEC_ENG_SHA512) {
                tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_H_6_OFFSET);
                *p++ = (tmpVal & 0xff);
                *p++ = ((tmpVal >> 8) & 0xff);
                *p++ = ((tmpVal >> 16) & 0xff);
                *p++ = ((tmpVal >> 24) & 0xff);
                tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_6_OFFSET);
                *p++ = (tmpVal & 0xff);
                *p++ = ((tmpVal >> 8) & 0xff);
                *p++ = ((tmpVal >> 16) & 0xff);
                *p++ = ((tmpVal >> 24) & 0xff);
                tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_H_7_OFFSET);
                *p++ = (tmpVal & 0xff);
                *p++ = ((tmpVal >> 8) & 0xff);
                *p++ = ((tmpVal >> 16) & 0xff);
                *p++ = ((tmpVal >> 24) & 0xff);
                tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_7_OFFSET);
                *p++ = (tmpVal & 0xff);
                *p++ = ((tmpVal >> 8) & 0xff);
                *p++ = ((tmpVal >> 16) & 0xff);
                *p++ = ((tmpVal >> 24) & 0xff);
            }
        }
    }

    /* Disable SHA engine*/
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_SHA_0_HASH_SEL);
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_SHA_0_EN);
    writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

    return SUCCESS;
}

int Sec_Eng_MD5_Finish(SEC_Eng_MD5_Ctx *md5Ctx, SEC_ENG_SHA_ID_Type shaNo, uint8_t *hash)
{
    uint32_t last, padn;
    uint32_t high, low;
    uint8_t msgLen[8];
    uint8_t *p = (uint8_t *)hash;
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    /* Check the parameters */

    /* Wait finished */
    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    high = (md5Ctx->total[0] >> 29) | (md5Ctx->total[1] << 3);
    low = (md5Ctx->total[0] << 3);

    PUT_UINT32(low, msgLen, 0);
    PUT_UINT32(high, msgLen, 4);

    last = md5Ctx->total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    Sec_Eng_MD5_Update(md5Ctx, shaNo, (uint8_t *)md5Ctx->md5Padding, padn);

    /* Wait for  shaPadding idle */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    ARCH_MemCpy_Fast(md5Ctx->md5Padding, msgLen, 8);
    Sec_Eng_MD5_Update(md5Ctx, shaNo, (uint8_t *)md5Ctx->md5Padding, 8);

    /* Wait finished */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    /* Copy SHA value */
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_0_OFFSET);
    *p++ = (tmpVal & 0xff);
    *p++ = ((tmpVal >> 8) & 0xff);
    *p++ = ((tmpVal >> 16) & 0xff);
    *p++ = ((tmpVal >> 24) & 0xff);
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_1_OFFSET);
    *p++ = (tmpVal & 0xff);
    *p++ = ((tmpVal >> 8) & 0xff);
    *p++ = ((tmpVal >> 16) & 0xff);
    *p++ = ((tmpVal >> 24) & 0xff);
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_2_OFFSET);
    *p++ = (tmpVal & 0xff);
    *p++ = ((tmpVal >> 8) & 0xff);
    *p++ = ((tmpVal >> 16) & 0xff);
    *p++ = ((tmpVal >> 24) & 0xff);
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_HASH_L_3_OFFSET);
    *p++ = (tmpVal & 0xff);
    *p++ = ((tmpVal >> 8) & 0xff);
    *p++ = ((tmpVal >> 16) & 0xff);
    *p++ = ((tmpVal >> 24) & 0xff);

    /* Disable SHA engine*/
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_SHA_0_HASH_SEL);
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_SHA_0_EN);
    writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

    return SUCCESS;
}

void Sec_Eng_SHA_Enable_Link(SEC_ENG_SHA_ID_Type shaNo)
{
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;

    /* Check the parameters */

    /* Enable sha and enable link mode */
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_EN);
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_LINK_MODE);
    writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
}

void Sec_Eng_SHA_Disable_Link(SEC_ENG_SHA_ID_Type shaNo)
{
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;

    /* Check the parameters */

    /* Disable sha and disable link mode */
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_SHA_0_LINK_MODE);
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_SHA_0_EN);
    writel(tmpVal, SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
}

void Sec_Eng_SHA256_Link_Init(SEC_Eng_SHA256_Link_Ctx *shaCtx, SEC_ENG_SHA_ID_Type shaNo, uint32_t linkAddr, uint32_t shaTmpBuf[16], uint32_t padding[16])
{
    /* Check the parameters */

    /* Clear context */
    ARCH_MemSet(shaCtx, 0, sizeof(SEC_Eng_SHA256_Link_Ctx));

    /* Init temp buffer,padding buffer and link address */
    shaCtx->shaBuf = shaTmpBuf;
    shaCtx->shaPadding = padding;
    ARCH_MemSet(shaCtx->shaPadding, 0, 64);
    ARCH_MemSet(shaCtx->shaPadding, 0x80, 1);
    shaCtx->linkAddr = linkAddr;

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_SHA_IRQHandler);
    Interrupt_Handler_Register(SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_SHA_IRQHandler);
#endif
}

void Sec_Eng_SHA512_Link_Init(SEC_Eng_SHA512_Link_Ctx *shaCtx, SEC_ENG_SHA_ID_Type shaNo, uint32_t linkAddr, uint64_t shaTmpBuf[16], uint64_t padding[16])
{
    /* Check the parameters */

    /* Clear context */
    ARCH_MemSet(shaCtx, 0, sizeof(SEC_Eng_SHA512_Link_Ctx));

    /* Init temp buffer,padding buffer and link address */
    shaCtx->shaBuf = shaTmpBuf;
    shaCtx->shaPadding = padding;
    ARCH_MemSet(shaCtx->shaPadding, 0, 128);
    ARCH_MemSet(shaCtx->shaPadding, 0x80, 1);
    shaCtx->linkAddr = linkAddr;

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_SHA_IRQHandler);
    Interrupt_Handler_Register(SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_SHA_IRQHandler);
#endif
}

void Sec_Eng_MD5_Link_Init(SEC_Eng_MD5_Link_Ctx *md5Ctx, SEC_ENG_SHA_ID_Type shaNo, uint32_t linkAddr, uint32_t md5TmpBuf[16], uint32_t padding[16])
{
    /* Check the parameters */

    /* Clear context */
    ARCH_MemSet(md5Ctx, 0, sizeof(SEC_Eng_MD5_Link_Ctx));

    /* Init temp buffer,padding buffer and link address */
    md5Ctx->md5Buf = md5TmpBuf;
    md5Ctx->md5Padding = padding;
    ARCH_MemSet(md5Ctx->md5Padding, 0, 64);
    ARCH_MemSet(md5Ctx->md5Padding, 0x80, 1);
    md5Ctx->linkAddr = linkAddr;

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_SHA_IRQHandler);
    Interrupt_Handler_Register(SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_SHA_IRQHandler);
#endif
}

int Sec_Eng_SHA256_Link_Update(SEC_Eng_SHA256_Link_Ctx *shaCtx, SEC_ENG_SHA_ID_Type shaNo, const uint8_t *input, uint32_t len)
{
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t fill;
    uint32_t left;
    uint32_t timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    if (len == 0) {
        return SUCCESS;
    }

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    /* Set link address */
    writel(shaCtx->linkAddr, SHAx + SEC_ENG_SE_SHA_0_LINK_OFFSET);

    left = shaCtx->total[0] & 0x3F;
    fill = 64 - left;

    shaCtx->total[0] += (uint32_t)len;
    shaCtx->total[0] &= 0xFFFFFFFF;

    if (shaCtx->total[0] < (uint32_t)len) {
        shaCtx->total[1]++;
    }

    if (left && len >= fill) {
        ARCH_MemCpy_Fast((void *)((uint8_t *)shaCtx->shaBuf + left), input, fill);
        /* Set data source address */
        *(uint32_t *)(uintptr_t)(shaCtx->linkAddr + 4) = (uint32_t)(uintptr_t)shaCtx->shaBuf;

        /* Set data length */
        *((uint16_t *)(uintptr_t)shaCtx->linkAddr + 1) = 1;

        /* Trigger */
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        writel(reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_TRIG_1T), SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        /* Choose accumulating last hash in the next time */
        *((uint32_t *)(uintptr_t)shaCtx->linkAddr) |= 0x40;
        input += fill;
        len -= fill;
        left = 0;
    }

    fill = len / 64;
    len = len % 64;

    if (fill > 0) {
        /* Wait finished */
        timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

        do {
            tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return TIMEOUT;
            }
        } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

        /* Fill data */
        *(uint32_t *)(uintptr_t)(shaCtx->linkAddr + 4) = (uint32_t)(uintptr_t)input;
        *((uint16_t *)(uintptr_t)shaCtx->linkAddr + 1) = fill;

        /* Trigger */
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        writel(reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_TRIG_1T), SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        input += (fill * 64);
        /* Choose accumulating last hash in the next time */
        *((uint32_t *)(uintptr_t)shaCtx->linkAddr) |= 0x40;
    }

    if (len > 0) {
        /* Wait finished */
        timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

        do {
            tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return TIMEOUT;
            }
        } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

        /* Copy left data into temp buffer */
        ARCH_MemCpy_Fast((void *)((uint8_t *)shaCtx->shaBuf + left), input, len);
    }

    /* Wait finished */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    return SUCCESS;
}

int Sec_Eng_SHA512_Link_Update(SEC_Eng_SHA512_Link_Ctx *shaCtx, SEC_ENG_SHA_ID_Type shaNo, const uint8_t *input, uint64_t len)
{
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t fill;
    uint32_t left;
    uint32_t timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    if (len == 0) {
        return SUCCESS;
    }

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    /* Set link address */
    writel(shaCtx->linkAddr, SHAx + SEC_ENG_SE_SHA_0_LINK_OFFSET);

    left = shaCtx->total[0] & 0x7F;
    fill = 128 - left;

    shaCtx->total[0] += (uint64_t)len;

    if (shaCtx->total[0] < (uint64_t)len) {
        shaCtx->total[1]++;
    }

    if (left && len >= fill) {
        ARCH_MemCpy_Fast((void *)((uint8_t *)shaCtx->shaBuf + left), input, fill);
        /* Set data source address */
        *(uint32_t *)(uintptr_t)(shaCtx->linkAddr + 4) = (uint32_t)(uintptr_t)shaCtx->shaBuf;

        /* Set data length */
        *((uint16_t *)(uintptr_t)shaCtx->linkAddr + 1) = 1;

        /* Trigger */
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        writel(reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_TRIG_1T), SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        /* Choose accumulating last hash in the next time */
        *((uint32_t *)(uintptr_t)shaCtx->linkAddr) |= 0x40;
        input += fill;
        len -= fill;
        left = 0;
    }

    fill = len / 128;
    len = len % 128;

    if (fill > 0) {
        /* Wait finished */
        timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

        do {
            tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return TIMEOUT;
            }
        } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

        /* Fill data */
        *(uint32_t *)(uintptr_t)(shaCtx->linkAddr + 4) = (uint32_t)(uintptr_t)input;
        *((uint16_t *)(uintptr_t)shaCtx->linkAddr + 1) = fill;

        /* Trigger */
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        writel(reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_TRIG_1T), SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        input += (fill * 128);
        /* Choose accumulating last hash in the next time */
        *((uint32_t *)(uintptr_t)shaCtx->linkAddr) |= 0x40;
    }

    if (len > 0) {
        /* Wait finished */
        timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

        do {
            tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return TIMEOUT;
            }
        } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

        /* Copy left data into temp buffer */
        ARCH_MemCpy_Fast((void *)((uint8_t *)shaCtx->shaBuf + left), input, len);
    }

    /* Wait finished */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    return SUCCESS;
}

int Sec_Eng_MD5_Link_Update(SEC_Eng_MD5_Link_Ctx *md5Ctx, SEC_ENG_SHA_ID_Type shaNo, const uint8_t *input, uint32_t len)
{
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t fill;
    uint32_t left;
    uint32_t timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    if (len == 0) {
        return SUCCESS;
    }

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    /* Set link address */
    writel(md5Ctx->linkAddr, SHAx + SEC_ENG_SE_SHA_0_LINK_OFFSET);

    left = md5Ctx->total[0] & 0x3F;
    fill = 64 - left;

    md5Ctx->total[0] += (uint32_t)len;
    md5Ctx->total[0] &= 0xFFFFFFFF;

    if (md5Ctx->total[0] < (uint32_t)len) {
        md5Ctx->total[1]++;
    }

    if (left && len >= fill) {
        ARCH_MemCpy_Fast((void *)((uint8_t *)md5Ctx->md5Buf + left), input, fill);

        /* Set data source address */
        *(uint32_t *)(uintptr_t)(md5Ctx->linkAddr + 4) = (uint32_t)(uintptr_t)md5Ctx->md5Buf;

        /* Set data length */
        *((uint16_t *)(uintptr_t)md5Ctx->linkAddr + 1) = 1;

        /* Trigger */
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        writel(reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_TRIG_1T), SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        /* Choose accumulating last hash in the next time */
        *((uint32_t *)(uintptr_t)md5Ctx->linkAddr) |= 0x40;
        input += fill;
        len -= fill;
        left = 0;
    }

    fill = len / 64;
    len = len % 64;

    if (fill > 0) {
        /* Wait finished */
        timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

        do {
            tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return TIMEOUT;
            }
        } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

        /* Fill data */
        *(uint32_t *)(uintptr_t)(md5Ctx->linkAddr + 4) = (uint32_t)(uintptr_t)input;
        *((uint16_t *)(uintptr_t)md5Ctx->linkAddr + 1) = fill;

        /* Trigger */
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        writel(reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_TRIG_1T), SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        input += (fill * 64);
        /* Choose accumulating last hash in the next time */
        *((uint32_t *)(uintptr_t)md5Ctx->linkAddr) |= 0x40;
    }

    if (len > 0) {
        /* Wait finished */
        timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

        do {
            tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return TIMEOUT;
            }
        } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

        /* Copy left data into temp buffer */
        ARCH_MemCpy_Fast((void *)((uint8_t *)md5Ctx->md5Buf + left), input, len);
    }

    /* Wait finished */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    return SUCCESS;
}

int Sec_Eng_CRC16_Link_Work(SEC_ENG_SHA_ID_Type shaNo, uint32_t linkAddr, const uint8_t *in, uint32_t len, uint8_t *out)
{
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    /* Check the parameters */

    /* Link address should word align */
    if ((linkAddr & 0x03) != 0) {
        return ERROR;
    }

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    /* Set link address */
    writel(linkAddr, SHAx + SEC_ENG_SE_SHA_0_LINK_OFFSET);

    /* Change source buffer address and destination buffer address */
    *(uint32_t *)(uintptr_t)(linkAddr + 4) = (uint32_t)(uintptr_t)in;

    /* Set data length, 16 bits per block */
    *((uint16_t *)(uintptr_t)linkAddr + 1) = len * 8 / 16;

    /* Trigger */
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    writel(reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_TRIG_1T), SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

    /* Wait finished */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    /* CRC16 code len is 16 bits */
    ARCH_MemCpy_Fast(out, (uint8_t *)(uintptr_t)(linkAddr + 0x10), 2);

    return SUCCESS;
}

int Sec_Eng_CRC32_Link_Work(SEC_ENG_SHA_ID_Type shaNo, uint32_t linkAddr, const uint8_t *in, uint32_t len, uint8_t *out)
{
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    /* Check the parameters */

    /* Link address should word align */
    if ((linkAddr & 0x03) != 0) {
        return ERROR;
    }

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    /* Set link address */
    writel(linkAddr, SHAx + SEC_ENG_SE_SHA_0_LINK_OFFSET);

    /* Change source buffer address and destination buffer address */
    *(uint32_t *)(uintptr_t)(linkAddr + 4) = (uint32_t)(uintptr_t)in;

    /* Set data length , 32 bits per block*/
    *((uint16_t *)(uintptr_t)linkAddr + 1) = len * 8 / 32;

    /* Trigger */
    tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    writel(reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_TRIG_1T), SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

    /* Wait finished */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    /* CRC32 code len is 32 bits */
    ARCH_MemCpy_Fast(out, (uint8_t *)(uintptr_t)(linkAddr + 0x10), 4);

    return SUCCESS;
}

int Sec_Eng_SHA256_Link_Finish(SEC_Eng_SHA256_Link_Ctx *shaCtx, SEC_ENG_SHA_ID_Type shaNo, uint8_t *hash)
{
    uint32_t last, padn;
    uint32_t high, low;
    uint8_t msgLen[8];
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t shaMode = (*(uint32_t *)(uintptr_t)shaCtx->linkAddr) >> 2 & 0x7;
    uint32_t timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    /* Check the parameters */

    /* Wait finished */
    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    /* Set link address */
    writel(shaCtx->linkAddr, SHAx + SEC_ENG_SE_SHA_0_LINK_OFFSET);

    high = (shaCtx->total[0] >> 29) | (shaCtx->total[1] << 3);
    low = (shaCtx->total[0] << 3);

    PUT_UINT32_BE(high, msgLen, 0);
    PUT_UINT32_BE(low, msgLen, 4);

    last = shaCtx->total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    Sec_Eng_SHA256_Link_Update(shaCtx, shaNo, (uint8_t *)shaCtx->shaPadding, padn);

    /* Wait for shaPadding idle */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    Sec_Eng_SHA256_Link_Update(shaCtx, shaNo, msgLen, 8);

    /* Wait finished */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    /* Get result according to SHA mode,result is placed in (link address + offset:8) */
    switch (shaMode) {
        case 0:
            ARCH_MemCpy_Fast(hash, (uint8_t *)(uintptr_t)(shaCtx->linkAddr + 8), 32);
            break;

        case 1:
            ARCH_MemCpy_Fast(hash, (uint8_t *)(uintptr_t)(shaCtx->linkAddr + 8), 28);
            break;

        case 2:
            ARCH_MemCpy_Fast(hash, (uint8_t *)(uintptr_t)(shaCtx->linkAddr + 8), 20);
            break;

        case 3:
            ARCH_MemCpy_Fast(hash, (uint8_t *)(uintptr_t)(shaCtx->linkAddr + 8), 20);
            break;

        default:
            break;
    }

    /* Choose new hash in the next time */
    *((uint32_t *)(uintptr_t)shaCtx->linkAddr) &= ~0x40;

    return SUCCESS;
}

int Sec_Eng_SHA512_Link_Finish(SEC_Eng_SHA512_Link_Ctx *shaCtx, SEC_ENG_SHA_ID_Type shaNo, uint8_t *hash)
{
    uint64_t last, padn;
    uint64_t high, low;
    uint8_t msgLen[16];
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t shaMode = (*(uint32_t *)(uintptr_t)shaCtx->linkAddr) >> 2 & 0x7;
    uint32_t timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    /* Check the parameters */

    /* Wait finished */
    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    /* Set link address */
    writel(shaCtx->linkAddr, SHAx + SEC_ENG_SE_SHA_0_LINK_OFFSET);

    high = (shaCtx->total[0] >> 61) | (shaCtx->total[1] << 3);
    low = (shaCtx->total[0] << 3);

    PUT_UINT64_BE(high, msgLen, 0);
    PUT_UINT64_BE(low, msgLen, 8);

    last = shaCtx->total[0] & 0x7F;
    padn = (last < 112) ? (112 - last) : (240 - last);

    Sec_Eng_SHA512_Link_Update(shaCtx, shaNo, (uint8_t *)shaCtx->shaPadding, padn);

    /* Wait for shaPadding idle */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    Sec_Eng_SHA512_Link_Update(shaCtx, shaNo, msgLen, 16);

    /* Wait finished */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    /* Get result according to SHA mode,result is placed in (link address + offset:8) */
    switch (shaMode) {
        case 4:
            ARCH_MemCpy_Fast(hash, (uint8_t *)(uintptr_t)(shaCtx->linkAddr + 8), 64);
            break;

        case 5:
            ARCH_MemCpy_Fast(hash, (uint8_t *)(uintptr_t)(shaCtx->linkAddr + 8), 48);
            break;

        case 6:
            ARCH_MemCpy_Fast(hash, (uint8_t *)(uintptr_t)(shaCtx->linkAddr + 8), 28);
            break;

        case 7:
            ARCH_MemCpy_Fast(hash, (uint8_t *)(uintptr_t)(shaCtx->linkAddr + 8), 32);
            break;

        default:
            break;
    }

    /* Choose new hash in the next time */
    *((uint32_t *)(uintptr_t)shaCtx->linkAddr) &= ~0x40;

    return SUCCESS;
}

int Sec_Eng_MD5_Link_Finish(SEC_Eng_MD5_Link_Ctx *md5Ctx, SEC_ENG_SHA_ID_Type shaNo, uint8_t *hash)
{
    uint32_t last, padn;
    uint32_t high, low;
    uint8_t msgLen[8];
    uint32_t SHAx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    /* Check the parameters */

    /* Wait finished */
    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    /* Set link address */
    writel(md5Ctx->linkAddr, SHAx + SEC_ENG_SE_SHA_0_LINK_OFFSET);

    high = (md5Ctx->total[0] >> 29) | (md5Ctx->total[1] << 3);
    low = (md5Ctx->total[0] << 3);

    PUT_UINT32(low, msgLen, 0);
    PUT_UINT32(high, msgLen, 4);

    last = md5Ctx->total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    Sec_Eng_MD5_Link_Update(md5Ctx, shaNo, (uint8_t *)md5Ctx->md5Padding, padn);

    /* Wait for shaPadding idle */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    Sec_Eng_MD5_Link_Update(md5Ctx, shaNo, msgLen, 8);

    /* Wait finished */
    timeoutCnt = SEC_ENG_SHA_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(SHAx + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_BUSY));

    ARCH_MemCpy_Fast(hash, (uint8_t *)(uintptr_t)(md5Ctx->linkAddr + 8), 16);

    /* Choose new hash in the next time */
    *((uint32_t *)(uintptr_t)md5Ctx->linkAddr) &= ~0x40;

    return SUCCESS;
}

int Sec_Eng_Group0_Request_SHA_Access(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

    if ((tmpVal & 0x03) == 0x03) {
        /* SHA control idle, write to request*/
        writel(0x02, SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_PROT_OFFSET);

        /* Check request result */
        tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

        if ((tmpVal & 0x03) == 0x01) {
            return SUCCESS;
        }
    }

    return ERROR;
}

int Sec_Eng_Group0_Release_SHA_Access(void)
{
    writel(0x06, SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_PROT_OFFSET);

    return SUCCESS;
}

int Sec_Eng_Group1_Request_SHA_Access(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

    if ((tmpVal & 0x03) == 0x03) {
        /* SHA control idle, write to request*/
        writel(0x04, SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_PROT_OFFSET);

        /* Check request result */
        tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

        if ((tmpVal & 0x03) == 0x02) {
            return SUCCESS;
        }
    }

    return ERROR;
}

int Sec_Eng_Group1_Release_SHA_Access(void)
{
    writel(0x06, SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_PROT_OFFSET);

    return SUCCESS;
}

int Sec_Eng_AES_Init(SEC_Eng_AES_Ctx *aesCtx, SEC_ENG_AES_ID_Type aesNo, SEC_ENG_AES_Type aesType, SEC_ENG_AES_Key_Type keyType, SEC_ENG_AES_EnDec_Type enDecType)
{
    uint32_t AESx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t timeoutCnt = SEC_ENG_AES_BUSY_TIMEOUT_COUNT;

    /* Check the parameters */

    /* Wait finished */
    do {
        tmpVal = readl(AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_AES_0_BUSY));

    /* Set AES mode type*/
    tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_AES_0_BLOCK_MODE, aesType);

    /* Set AES key type */
    tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_AES_0_MODE, keyType);

    /* Set AES encryption or decryption */
    tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_AES_0_DEC_EN, enDecType);

    /* Clear dec_key_sel to select new key */
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_AES_0_DEC_KEY_SEL);

    /* Clear aes iv sel to select new iv */
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_AES_0_IV_SEL);

    /* Clear AES interrupt */
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_AES_0_INT_CLR_1T);

    /* Enable AES */
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_AES_0_EN);

    writel(tmpVal, AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);

    /* Clear AES context */
    memset(aesCtx, 0, sizeof(SEC_Eng_AES_Ctx));

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_AES_IRQHandler);
    Interrupt_Handler_Register(SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_AES_IRQHandler);
#endif

    return SUCCESS;
}

void Sec_Eng_AES_Enable_BE(SEC_ENG_AES_ID_Type aesNo)
{
    uint32_t AESx = SEC_ENG_BASE;

    /* Check the parameters */

    /* set 0x0f to 0x1f for xts mode */
    writel(0x1f, AESx + SEC_ENG_SE_AES_0_ENDIAN_OFFSET);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_AES_IRQHandler);
    Interrupt_Handler_Register(SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_AES_IRQHandler);
#endif
}

void Sec_Eng_AES_Enable_LE(SEC_ENG_AES_ID_Type aesNo)
{
    uint32_t AESx = SEC_ENG_BASE;

    /* Check the parameters */

    /* set 0x00 to 0x0f for xts mode */
    writel(0x10, AESx + SEC_ENG_SE_AES_0_ENDIAN_OFFSET);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_AES_IRQHandler);
    Interrupt_Handler_Register(SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_AES_IRQHandler);
#endif
}

void Sec_Eng_AES_Enable_Link(SEC_ENG_AES_ID_Type aesNo)
{
    uint32_t AESx = SEC_ENG_BASE;
    uint32_t tmpVal;

    /* Check the parameters */

    /* Enable aes link mode */
    tmpVal = readl(AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
    writel(reg_set_bit(tmpVal, SEC_ENG_SE_AES_0_LINK_MODE), AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
}

void Sec_Eng_AES_Disable_Link(SEC_ENG_AES_ID_Type aesNo)
{
    uint32_t AESx = SEC_ENG_BASE;
    uint32_t tmpVal;

    /* Check the parameters */

    /* Disable aes link mode */
    tmpVal = readl(AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
    writel(reg_clr_bit(tmpVal, SEC_ENG_SE_AES_0_LINK_MODE), AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
}

int Sec_Eng_AES_Link_Work(SEC_ENG_AES_ID_Type aesNo, uint32_t linkAddr, const uint8_t *in, uint32_t len, uint8_t *out)
{
    uint32_t AESx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t timeoutCnt = SEC_ENG_AES_BUSY_TIMEOUT_COUNT;

    /* Check the parameters */

    /* Link address should word align */
    if ((linkAddr & 0x03) != 0 || len % 16 != 0) {
        return ERROR;
    }

    /* Wait finished */
    do {
        tmpVal = readl(AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_AES_0_BUSY));

    /* Set link address */
    writel(linkAddr, AESx + SEC_ENG_SE_AES_0_LINK_OFFSET);

    /* Change source buffer address and destination buffer address */
    *(uint32_t *)(uintptr_t)(linkAddr + 4) = (uint32_t)(uintptr_t)in;
    *(uint32_t *)(uintptr_t)(linkAddr + 8) = (uint32_t)(uintptr_t)out;

    /* Set data length */
    *((uint16_t *)(uintptr_t)linkAddr + 1) = len / 16;

    /* Enable aes */
    tmpVal = readl(AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
    writel(reg_set_bit(tmpVal, SEC_ENG_SE_AES_0_EN), AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);

    /* Start aes engine and wait finishing */
    tmpVal = readl(AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
    writel(reg_set_bit(tmpVal, SEC_ENG_SE_AES_0_TRIG_1T), AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);

    __NOP();
    __NOP();
    timeoutCnt = SEC_ENG_AES_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_AES_0_BUSY));

    /* Disable aes */
    writel(reg_clr_bit(tmpVal, SEC_ENG_SE_AES_0_EN), AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);

    return SUCCESS;
}

void Sec_Eng_AES_Set_Hw_Key_Src(SEC_ENG_AES_ID_Type aesNo, uint8_t src)
{
    uint32_t AESx = SEC_ENG_BASE;
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(AESx + SEC_ENG_SE_AES_SBOOT_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_AES_SBOOT_KEY_SEL, src);

    writel(tmpVal, AESx + SEC_ENG_SE_AES_SBOOT_OFFSET);
}

void Sec_Eng_AES_Set_Key_IV(SEC_ENG_AES_ID_Type aesNo, SEC_ENG_AES_Key_Src_Type keySrc, const uint8_t *key, const uint8_t *iv)
{
    uint32_t AESx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t keyType;
    uint32_t aesType;

    /* Check the parameters */

    tmpVal = readl(AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
    aesType = reg_get_bits(tmpVal, SEC_ENG_SE_AES_0_BLOCK_MODE);

    /* Set IV, XTS mode and other mode are different */
    if (aesType == (uint32_t)SEC_ENG_AES_XTS) {
        writel(DRV_WORD_FROM_BYTES(iv), AESx + SEC_ENG_SE_AES_0_IV_0_OFFSET);
        iv += 4;
        writel(DRV_WORD_FROM_BYTES(iv), AESx + SEC_ENG_SE_AES_0_IV_1_OFFSET);
        iv += 4;
        writel(DRV_WORD_FROM_BYTES(iv), AESx + SEC_ENG_SE_AES_0_IV_2_OFFSET);
        iv += 4;
        writel(DRV_WORD_FROM_BYTES(iv), AESx + SEC_ENG_SE_AES_0_IV_3_OFFSET);
        iv += 4;
    } else {
        writel(__REV(DRV_WORD_FROM_BYTES(iv)), AESx + SEC_ENG_SE_AES_0_IV_3_OFFSET);
        iv += 4;
        writel(__REV(DRV_WORD_FROM_BYTES(iv)), AESx + SEC_ENG_SE_AES_0_IV_2_OFFSET);
        iv += 4;
        writel(__REV(DRV_WORD_FROM_BYTES(iv)), AESx + SEC_ENG_SE_AES_0_IV_1_OFFSET);
        iv += 4;
        writel(__REV(DRV_WORD_FROM_BYTES(iv)), AESx + SEC_ENG_SE_AES_0_IV_0_OFFSET);
        iv += 4;
    }

    /* Select hardware key */
    if (keySrc == SEC_ENG_AES_KEY_HW) {
        tmpVal = readl(AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_AES_0_HW_KEY_EN, SEC_ENG_AES_KEY_HW);
        writel(tmpVal, AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);

        tmpVal = readl(AESx + SEC_ENG_SE_AES_0_KEY_SEL_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_AES_0_KEY_SEL, *key);
        writel(tmpVal, AESx + SEC_ENG_SE_AES_0_KEY_SEL_OFFSET);

        tmpVal = readl(AESx + SEC_ENG_SE_AES_1_KEY_SEL_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_AES_1_KEY_SEL, *key);
        writel(tmpVal, AESx + SEC_ENG_SE_AES_1_KEY_SEL_OFFSET);

        return;
    }

    writel(__REV(DRV_WORD_FROM_BYTES(key)), AESx + SEC_ENG_SE_AES_0_KEY_7_OFFSET);
    key += 4;
    writel(__REV(DRV_WORD_FROM_BYTES(key)), AESx + SEC_ENG_SE_AES_0_KEY_6_OFFSET);
    key += 4;
    writel(__REV(DRV_WORD_FROM_BYTES(key)), AESx + SEC_ENG_SE_AES_0_KEY_5_OFFSET);
    key += 4;
    writel(__REV(DRV_WORD_FROM_BYTES(key)), AESx + SEC_ENG_SE_AES_0_KEY_4_OFFSET);
    key += 4;

    tmpVal = readl(AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
    keyType = reg_get_bits(tmpVal, SEC_ENG_SE_AES_0_MODE);

    if (keyType == (uint32_t)SEC_ENG_AES_KEY_192BITS) {
        writel(__REV(DRV_WORD_FROM_BYTES(key)), AESx + SEC_ENG_SE_AES_0_KEY_3_OFFSET);
        key += 4;
        writel(__REV(DRV_WORD_FROM_BYTES(key)), AESx + SEC_ENG_SE_AES_0_KEY_2_OFFSET);
        key += 4;
    } else if (keyType == (uint32_t)SEC_ENG_AES_KEY_256BITS || keyType == (uint32_t)SEC_ENG_AES_DOUBLE_KEY_128BITS || aesType == (uint32_t)SEC_ENG_AES_XTS) {
        writel(__REV(DRV_WORD_FROM_BYTES(key)), AESx + SEC_ENG_SE_AES_0_KEY_3_OFFSET);
        key += 4;
        writel(__REV(DRV_WORD_FROM_BYTES(key)), AESx + SEC_ENG_SE_AES_0_KEY_2_OFFSET);
        key += 4;
        writel(__REV(DRV_WORD_FROM_BYTES(key)), AESx + SEC_ENG_SE_AES_0_KEY_1_OFFSET);
        key += 4;
        writel(__REV(DRV_WORD_FROM_BYTES(key)), AESx + SEC_ENG_SE_AES_0_KEY_0_OFFSET);
        key += 4;
    }

    /* Select software key */
    tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_AES_0_HW_KEY_EN, SEC_ENG_AES_KEY_SW);

    writel(tmpVal, AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
}

void Sec_Eng_AES_Set_Key_IV_BE(SEC_ENG_AES_ID_Type aesNo, SEC_ENG_AES_Key_Src_Type keySrc, const uint8_t *key, const uint8_t *iv)
{
    uint32_t AESx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t keyType;
    uint32_t aesType;

    /* Check the parameters */

    tmpVal = readl(AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
    aesType = reg_get_bits(tmpVal, SEC_ENG_SE_AES_0_BLOCK_MODE);

    /* Set IV, XTS mode and other mode are different */
    if (aesType == (uint32_t)SEC_ENG_AES_XTS) {
        writel(__REV(DRV_WORD_FROM_BYTES(iv)), AESx + SEC_ENG_SE_AES_0_IV_3_OFFSET);
        iv += 4;
        writel(__REV(DRV_WORD_FROM_BYTES(iv)), AESx + SEC_ENG_SE_AES_0_IV_2_OFFSET);
        iv += 4;
        writel(__REV(DRV_WORD_FROM_BYTES(iv)), AESx + SEC_ENG_SE_AES_0_IV_1_OFFSET);
        iv += 4;
        writel(__REV(DRV_WORD_FROM_BYTES(iv)), AESx + SEC_ENG_SE_AES_0_IV_0_OFFSET);
        iv += 4;
    } else {
        writel(DRV_WORD_FROM_BYTES(iv), AESx + SEC_ENG_SE_AES_0_IV_0_OFFSET);
        iv += 4;
        writel(DRV_WORD_FROM_BYTES(iv), AESx + SEC_ENG_SE_AES_0_IV_1_OFFSET);
        iv += 4;
        writel(DRV_WORD_FROM_BYTES(iv), AESx + SEC_ENG_SE_AES_0_IV_2_OFFSET);
        iv += 4;
        writel(DRV_WORD_FROM_BYTES(iv), AESx + SEC_ENG_SE_AES_0_IV_3_OFFSET);
        iv += 4;
    }

    /* Select hardware key */
    if (keySrc == SEC_ENG_AES_KEY_HW) {
        tmpVal = readl(AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_AES_0_HW_KEY_EN, SEC_ENG_AES_KEY_HW);
        writel(tmpVal, AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);

        tmpVal = readl(AESx + SEC_ENG_SE_AES_0_KEY_SEL_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_AES_0_KEY_SEL, *key);
        writel(tmpVal, AESx + SEC_ENG_SE_AES_0_KEY_SEL_OFFSET);

        tmpVal = readl(AESx + SEC_ENG_SE_AES_1_KEY_SEL_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_AES_1_KEY_SEL, *key);
        writel(tmpVal, AESx + SEC_ENG_SE_AES_1_KEY_SEL_OFFSET);

        return;
    }

    writel(DRV_WORD_FROM_BYTES(key), AESx + SEC_ENG_SE_AES_0_KEY_0_OFFSET);
    key += 4;
    writel(DRV_WORD_FROM_BYTES(key), AESx + SEC_ENG_SE_AES_0_KEY_1_OFFSET);
    key += 4;
    writel(DRV_WORD_FROM_BYTES(key), AESx + SEC_ENG_SE_AES_0_KEY_2_OFFSET);
    key += 4;
    writel(DRV_WORD_FROM_BYTES(key), AESx + SEC_ENG_SE_AES_0_KEY_3_OFFSET);
    key += 4;

    tmpVal = readl(AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
    keyType = reg_get_bits(tmpVal, SEC_ENG_SE_AES_0_MODE);

    if (keyType == (uint32_t)SEC_ENG_AES_KEY_192BITS) {
        writel(DRV_WORD_FROM_BYTES(key), AESx + SEC_ENG_SE_AES_0_KEY_4_OFFSET);
        key += 4;
        writel(DRV_WORD_FROM_BYTES(key), AESx + SEC_ENG_SE_AES_0_KEY_5_OFFSET);
        key += 4;
    } else if (keyType == (uint32_t)SEC_ENG_AES_KEY_256BITS || keyType == (uint32_t)SEC_ENG_AES_DOUBLE_KEY_128BITS || aesType == (uint32_t)SEC_ENG_AES_XTS) {
        writel(DRV_WORD_FROM_BYTES(key), AESx + SEC_ENG_SE_AES_0_KEY_4_OFFSET);
        key += 4;
        writel(DRV_WORD_FROM_BYTES(key), AESx + SEC_ENG_SE_AES_0_KEY_5_OFFSET);
        key += 4;
        writel(DRV_WORD_FROM_BYTES(key), AESx + SEC_ENG_SE_AES_0_KEY_6_OFFSET);
        key += 4;
        writel(DRV_WORD_FROM_BYTES(key), AESx + SEC_ENG_SE_AES_0_KEY_7_OFFSET);
        key += 4;
    }

    /* Select software key */
    tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_AES_0_HW_KEY_EN, SEC_ENG_AES_KEY_SW);

    writel(tmpVal, AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
}

void Sec_Eng_AES_Set_Counter_Byte(SEC_ENG_AES_ID_Type aesNo, SEC_ENG_AES_Counter_Type counterType)
{
    uint32_t AESx = SEC_ENG_BASE;
    uint32_t tmpVal;

    /* Check the parameters */

    /* Set counter type */
    tmpVal = readl(AESx + SEC_ENG_SE_AES_0_ENDIAN_OFFSET);
    writel(reg_set_bits(tmpVal, SEC_ENG_SE_AES_0_CTR_LEN, counterType), AESx + SEC_ENG_SE_AES_0_ENDIAN_OFFSET);
}

int Sec_Eng_AES_Crypt(SEC_Eng_AES_Ctx *aesCtx, SEC_ENG_AES_ID_Type aesNo, const uint8_t *in, uint32_t len, uint8_t *out)
{
    uint32_t AESx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t timeoutCnt = SEC_ENG_AES_BUSY_TIMEOUT_COUNT;

    if (len % 16 != 0) {
        return ERROR;
    }

    /* Wait finished */
    do {
        tmpVal = readl(AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_AES_0_BUSY));

    /* Clear trigger */
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_AES_0_TRIG_1T);
    writel(tmpVal, AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);

    /* Set input and output address */
    writel((uintptr_t)in, AESx + SEC_ENG_SE_AES_0_MSA_OFFSET);
    writel((uintptr_t)out, AESx + SEC_ENG_SE_AES_0_MDA_OFFSET);

    /* Set message length */
    tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_AES_0_MSG_LEN, len / 16);

    if (aesCtx->mode == SEC_ENG_AES_CTR) {
        tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_AES_0_DEC_KEY_SEL);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_AES_0_DEC_KEY_SEL);
    }

    /* Set IV sel:0 for new, 1 for last */
    tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_AES_0_IV_SEL, aesCtx->aesFeed);
    writel(tmpVal, AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);

    /* Trigger AES Engine */
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_AES_0_TRIG_1T);
    writel(tmpVal, AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);

    /* Wait finished */
    timeoutCnt = SEC_ENG_AES_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_AES_0_BUSY));

    aesCtx->aesFeed = 1;

    return SUCCESS;
}

int Sec_Eng_AES_Finish(SEC_ENG_AES_ID_Type aesNo)
{
    uint32_t AESx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t timeoutCnt = SEC_ENG_AES_BUSY_TIMEOUT_COUNT;

    /* Wait finished */
    do {
        tmpVal = readl(AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_AES_0_BUSY));

    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_AES_0_EN);

    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_AES_0_DEC_KEY_SEL);

    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_AES_0_IV_SEL);

    writel(tmpVal, AESx + SEC_ENG_SE_AES_0_CTRL_OFFSET);

    return SUCCESS;
}

int Sec_Eng_Group0_Request_AES_Access(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

    if (((tmpVal >> 2) & 0x03) == 0x03) {
        /* SHA control idle, write to request*/
        writel(0x02, SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_PROT_OFFSET);

        /* Check request result */
        tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

        if (((tmpVal >> 2) & 0x03) == 0x01) {
            return SUCCESS;
        }
    }

    return ERROR;
}

int Sec_Eng_Group0_Release_AES_Access(void)
{
    /* SHA control idle, write to request*/
    writel(0x06, SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_PROT_OFFSET);

    return SUCCESS;
}

int Sec_Eng_Group1_Request_AES_Access(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

    if (((tmpVal >> 2) & 0x03) == 0x03) {
        /* SHA control idle, write to request*/
        writel(0x04, SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_PROT_OFFSET);

        /* Check request result */
        tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

        if (((tmpVal >> 2) & 0x03) == 0x02) {
            return SUCCESS;
        }
    }

    return ERROR;
}

int Sec_Eng_Group1_Release_AES_Access(void)
{
    writel(0x06, SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_PROT_OFFSET);

    return SUCCESS;
}

int Sec_Eng_Trng_Enable(void)
{
    uint32_t TRNGx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t timeoutCnt = SEC_ENG_TRNG_BUSY_TIMEOUT_COUNT;

    tmpVal = readl(TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    /* FIXME:default reseed number is 0x1ff, to verify, use 0xa to speed up */
    //tmpVal=reg_set_bits(tmpVal, SEC_ENG_SE_TRNG_0_RESEED_N, 0x1ff);

    /* No interrupt as default */
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_TRNG_0_EN);
    writel(tmpVal, TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_TRNG_0_INT_CLR_1T);
    writel(tmpVal, TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    /* busy will be set to 1 after trigger, the gap is 1T */
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    do {
        tmpVal = readl(TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_TRNG_0_BUSY));

    /* Clear trng interrupt */
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_TRNG_0_INT_CLR_1T);
    writel(tmpVal, TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_TRNG_IRQHandler);
    Interrupt_Handler_Register(SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_TRNG_IRQHandler);
#endif

    return SUCCESS;
}

void Sec_Eng_Trng_Int_Enable(void)
{
    uint32_t TRNGx = SEC_ENG_BASE;
    uint32_t tmpVal;

    tmpVal = readl(TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_TRNG_0_INT_MASK);

    writel(tmpVal, TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
}

void Sec_Eng_Trng_Int_Disable(void)
{
    uint32_t TRNGx = SEC_ENG_BASE;
    uint32_t tmpVal;

    tmpVal = readl(TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_TRNG_0_INT_MASK);

    writel(tmpVal, TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
}

int Sec_Eng_Trng_Read(uint8_t data[32])
{
    uint8_t *p = (uint8_t *)data;
    uint32_t TRNGx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t timeoutCnt = SEC_ENG_TRNG_BUSY_TIMEOUT_COUNT;

    tmpVal = readl(TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    /* Trigger */
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_TRNG_0_TRIG_1T);
    writel(tmpVal, TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    /* busy will be set to 1 after trigger, the gap is 1T */
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    do {
        tmpVal = readl(TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_TRNG_0_BUSY));

    /* copy trng value */
    DRV_WORD_TO_BYTES(p, readl(TRNGx + SEC_ENG_SE_TRNG_0_DOUT_0_OFFSET));
    p += 4;
    DRV_WORD_TO_BYTES(p, readl(TRNGx + SEC_ENG_SE_TRNG_0_DOUT_1_OFFSET));
    p += 4;
    DRV_WORD_TO_BYTES(p, readl(TRNGx + SEC_ENG_SE_TRNG_0_DOUT_2_OFFSET));
    p += 4;
    DRV_WORD_TO_BYTES(p, readl(TRNGx + SEC_ENG_SE_TRNG_0_DOUT_3_OFFSET));
    p += 4;
    DRV_WORD_TO_BYTES(p, readl(TRNGx + SEC_ENG_SE_TRNG_0_DOUT_4_OFFSET));
    p += 4;
    DRV_WORD_TO_BYTES(p, readl(TRNGx + SEC_ENG_SE_TRNG_0_DOUT_5_OFFSET));
    p += 4;
    DRV_WORD_TO_BYTES(p, readl(TRNGx + SEC_ENG_SE_TRNG_0_DOUT_6_OFFSET));
    p += 4;
    DRV_WORD_TO_BYTES(p, readl(TRNGx + SEC_ENG_SE_TRNG_0_DOUT_7_OFFSET));
    p += 4;

    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_TRNG_0_TRIG_1T);
    writel(tmpVal, TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    /* Clear data */
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_TRNG_0_DOUT_CLR_1T);
    writel(tmpVal, TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_TRNG_0_DOUT_CLR_1T);
    writel(tmpVal, TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    return SUCCESS;
}

int Sec_Eng_Trng_Get_Random(uint8_t *data, uint32_t len)
{
    uint8_t tmpBuf[32];
    uint32_t readLen = 0;
    uint32_t i = 0, cnt = 0;

    while (readLen < len) {
        if (Sec_Eng_Trng_Read(tmpBuf) != SUCCESS) {
            return ERROR;
        }

        cnt = len - readLen;

        if (cnt > sizeof(tmpBuf)) {
            cnt = sizeof(tmpBuf);
        }

        for (i = 0; i < cnt; i++) {
            data[readLen + i] = tmpBuf[i];
        }

        readLen += cnt;
    }

    return 0;
}

void Sec_Eng_Trng_Int_Read_Trigger(void)
{
    uint32_t TRNGx = SEC_ENG_BASE;
    uint32_t tmpVal;

    Sec_Eng_Trng_Int_Enable();

    tmpVal = readl(TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
    /* Trigger */
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_TRNG_0_TRIG_1T);
    writel(tmpVal, TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
}

void Sec_Eng_Trng_Int_Read(uint8_t data[32])
{
    uint8_t *p = (uint8_t *)data;
    uint32_t TRNGx = SEC_ENG_BASE;
    uint32_t tmpVal;

    tmpVal = readl(TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    /* copy trng value */
    DRV_WORD_TO_BYTES(p, readl(TRNGx + SEC_ENG_SE_TRNG_0_DOUT_0_OFFSET));
    p += 4;
    DRV_WORD_TO_BYTES(p, readl(TRNGx + SEC_ENG_SE_TRNG_0_DOUT_1_OFFSET));
    p += 4;
    DRV_WORD_TO_BYTES(p, readl(TRNGx + SEC_ENG_SE_TRNG_0_DOUT_2_OFFSET));
    p += 4;
    DRV_WORD_TO_BYTES(p, readl(TRNGx + SEC_ENG_SE_TRNG_0_DOUT_3_OFFSET));
    p += 4;
    DRV_WORD_TO_BYTES(p, readl(TRNGx + SEC_ENG_SE_TRNG_0_DOUT_4_OFFSET));
    p += 4;
    DRV_WORD_TO_BYTES(p, readl(TRNGx + SEC_ENG_SE_TRNG_0_DOUT_5_OFFSET));
    p += 4;
    DRV_WORD_TO_BYTES(p, readl(TRNGx + SEC_ENG_SE_TRNG_0_DOUT_6_OFFSET));
    p += 4;
    DRV_WORD_TO_BYTES(p, readl(TRNGx + SEC_ENG_SE_TRNG_0_DOUT_7_OFFSET));
    p += 4;

    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_TRNG_0_TRIG_1T);
    writel(tmpVal, TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    /* Clear data */
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_TRNG_0_DOUT_CLR_1T);
    writel(tmpVal, TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_TRNG_0_DOUT_CLR_1T);
    writel(tmpVal, TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
}

void Sec_Eng_Trng_Disable(void)
{
    uint32_t TRNGx = SEC_ENG_BASE;
    uint32_t tmpVal;

    tmpVal = readl(TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_TRNG_0_EN);
    //tmpVal=reg_clr_bit(tmpVal, SEC_ENG_SE_TRNG_0_RESEED_N);
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_TRNG_0_INT_CLR_1T);

    writel(tmpVal, TRNGx + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
}

int Sec_Eng_Group0_Request_Trng_Access(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

    if (((tmpVal >> 4) & 0x03) == 0x03) {
        /* SHA control idle, write to request*/
        writel(0x02, SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_PROT_OFFSET);

        /* Check request result */
        tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

        if (((tmpVal >> 4) & 0x03) == 0x01) {
            return SUCCESS;
        }
    }

    return ERROR;
}

int Sec_Eng_Group0_Release_Trng_Access(void)
{
    writel(0x06, SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_PROT_OFFSET);

    return SUCCESS;
}

int Sec_Eng_Group1_Request_Trng_Access(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

    if (((tmpVal >> 4) & 0x03) == 0x03) {
        /* SHA control idle, write to request*/
        writel(0x04, SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_PROT_OFFSET);

        /* Check request result */
        tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

        if (((tmpVal >> 4) & 0x03) == 0x02) {
            return SUCCESS;
        }
    }

    return ERROR;
}

int Sec_Eng_Group1_Release_Trng_Access(void)
{
    writel(0x06, SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_PROT_OFFSET);

    return SUCCESS;
}

void Sec_Eng_PKA_Reset(void)
{
    uint8_t val;

    //Disable sec engine
    writel(0, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);

    //Enable sec engine
    val = 1 << 3;
    writel(val, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
}

void Sec_Eng_PKA_BigEndian_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_PKA_0_ENDIAN);
    writel(tmpVal, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_PKA_IRQHandler);
    Interrupt_Handler_Register(SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_PKA_IRQHandler);
#endif
}

void Sec_Eng_PKA_LittleEndian_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_PKA_0_ENDIAN);
    writel(tmpVal, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_PKA_IRQHandler);
    Interrupt_Handler_Register(SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_PKA_IRQHandler);
#endif
}

void Sec_Eng_PKA_GetStatus(SEC_Eng_PKA_Status_Type *status)
{
    uint32_t tmpVal;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
    *(uint16_t *)status = (uint16_t)reg_get_bits(tmpVal, SEC_ENG_SE_PKA_0_STATUS);
}

void Sec_Eng_PKA_Clear_Int(void)
{
    uint32_t ctrl;

    ctrl = readl(SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
    ctrl = reg_set_bit(ctrl, SEC_ENG_SE_PKA_0_INT_CLR_1T);

    writel(ctrl, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);

    ctrl = readl(SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
    ctrl = reg_clr_bit(ctrl, SEC_ENG_SE_PKA_0_INT_CLR_1T);
    writel(ctrl, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
}

static uint16_t Sec_Eng_PKA_Get_Reg_Size(SEC_ENG_PKA_REG_SIZE_Type reg_type)
{
    switch (reg_type) {
        case SEC_ENG_PKA_REG_SIZE_8:
            return 8;

        case SEC_ENG_PKA_REG_SIZE_16:
            return 16;

        case SEC_ENG_PKA_REG_SIZE_32:
            return 32;

        case SEC_ENG_PKA_REG_SIZE_64:
            return 64;

        case SEC_ENG_PKA_REG_SIZE_96:
            return 96;

        case SEC_ENG_PKA_REG_SIZE_128:
            return 128;

        case SEC_ENG_PKA_REG_SIZE_192:
            return 192;

        case SEC_ENG_PKA_REG_SIZE_256:
            return 256;

        case SEC_ENG_PKA_REG_SIZE_384:
            return 384;

        case SEC_ENG_PKA_REG_SIZE_512:
            return 512;

        default:
            return 0;
    }
}

static void Sec_Eng_PKA_Write_Pld_Cfg(uint16_t size, uint8_t regIndex, SEC_ENG_PKA_REG_SIZE_Type regType, SEC_ENG_PKA_OP_Type op, uint8_t lastOp)
{
    struct pka0_pld_cfg cfg;

    cfg.value.BF.size = size;
    cfg.value.BF.d_reg_index = regIndex;
    cfg.value.BF.d_reg_type = regType;
    cfg.value.BF.op = op;
    cfg.value.BF.last_op = lastOp;

    writel(cfg.value.WORD, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);
}

static void Sec_Eng_PKA_Write_Common_OP_First_Cfg(uint8_t s0RegIndex, uint8_t s0RegType, uint8_t dRegIndex, uint8_t dRegType,
                                                  uint8_t op, uint8_t lastOp)
{
    struct pka0_common_op_first_cfg cfg;

    cfg.value.BF.s0_reg_idx = s0RegIndex;
    cfg.value.BF.s0_reg_type = s0RegType;

    if (op != SEC_ENG_PKA_OP_LCMP) {
        cfg.value.BF.d_reg_idx = dRegIndex;
        cfg.value.BF.d_reg_type = dRegType;
    }

    cfg.value.BF.op = op;
    cfg.value.BF.last_op = lastOp;

    writel(cfg.value.WORD, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);
}

static void Sec_Eng_PKA_Write_Common_OP_Snd_Cfg_S1(uint8_t s1RegIndex, uint8_t s1RegType)
{
    struct pka0_common_op_snd_cfg_S1_only cfg;

    cfg.value.BF.s1_reg_idx = s1RegIndex;
    cfg.value.BF.s1_reg_type = s1RegType;

    writel(cfg.value.WORD, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);
}

static void Sec_Eng_PKA_Write_Common_OP_Snd_Cfg_S2(uint8_t s2RegIndex, uint8_t s2RegType)
{
    struct pka0_common_op_snd_cfg_S2_only cfg;

    cfg.value.BF.s2_reg_idx = s2RegIndex;
    cfg.value.BF.s2_reg_type = s2RegType;

    writel(cfg.value.WORD, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);
}

static void Sec_Eng_PKA_Write_Common_OP_Snd_Cfg_S1_S2(uint8_t s1RegIndex, uint8_t s1RegType, uint8_t s2RegIndex, uint8_t s2RegType)
{
    struct pka0_common_op_snd_cfg_S1_S2 cfg;

    cfg.value.BF.s1_reg_idx = s1RegIndex;
    cfg.value.BF.s1_reg_type = s1RegType;
    cfg.value.BF.s2_reg_idx = s2RegIndex;
    cfg.value.BF.s2_reg_type = s2RegType;

    writel(cfg.value.WORD, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);
}

static int Sec_Eng_PKA_Wait_ISR(void)
{
    uint32_t pka0_ctrl;
    uint32_t timeoutCnt = SEC_ENG_PKA_INT_TIMEOUT_COUNT;

    do {
        pka0_ctrl = readl(SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (!reg_get_bits(pka0_ctrl, SEC_ENG_SE_PKA_0_INT));

    return SUCCESS;
}

#ifdef ARCH_ARM
#ifndef __GNUC__
__ASM void Sec_Eng_PKA_Read_Block(uint32_t *dest, const uint32_t *src, uint32_t len)
{
    PUSH{ R3 - R6, LR } Start0
        CMP R2,
# 4 BLT Finish0
        LDR R3,
        [R1] LDR R4, [R1] LDR R5, [R1] LDR R6, [R1] STMIA R0 !, { R3 - R6 } SUBS R2, R2, #4 B Start0 Finish0 POP
    {
        R3 - R6, PC
    }
}
#else
void Sec_Eng_PKA_Read_Block(uint32_t *dest, const uint32_t *src, uint32_t len)
{
    __asm__ __volatile__("push {r3-r6,lr}\n\t"
                         "Start0 :"
                         "cmp   r2,#4\n\t"
                         "blt   Finish0\n\t"
                         "ldr   r3,[r1]\n\t"
                         "ldr   r4,[r1]\n\t"
                         "ldr   r5,[r1]\n\t"
                         "ldr   r6,[r1]\n\t"
                         "stmia r0!,{r3-r6}\n\t"
                         "sub   r2,r2,#4\n\t"
                         "b     Start0\n\t"
                         "Finish0 :"
                         "pop   {r3-r6,pc}\n\t");
}
#endif
#endif
#ifdef ARCH_RISCV
void Sec_Eng_PKA_Read_Block(uint32_t *dest, const uint32_t *src, uint32_t len)
{
    uint32_t wrLen = len - len % 4;
    uint32_t i;

    for (i = 0; i < wrLen; i++) {
        dest[i] = *src;
    }
}
#endif

#ifdef ARCH_ARM
#ifndef __GNUC__
__ASM void Sec_Eng_PKA_Write_Block(uint32_t *dest, const uint32_t *src, uint32_t len)
{
    PUSH{ R3 - R6, LR } Start1
        CMP R2,
# 4 BLT Finish1
        LDMIA R1 !,
        { R3 - R6 } STR R3, [R0] STR R4, [R0] STR R5, [R0] STR R6, [R0] SUBS R2, R2, #4 B Start1 Finish1 POP
    {
        R3 - R6, PC
    }
}
#else
void Sec_Eng_PKA_Write_Block(uint32_t *dest, const uint32_t *src, uint32_t len)
{
    __asm__ __volatile__("push {r3-r6,lr}\n\t"
                         "Start1 :"
                         "cmp   r2,#4\n\t"
                         "blt   Finish1\n\t"
                         "ldmia r1!,{r3-r6}\n\t"
                         "str   r3,[r0]\n\t"
                         "str   r4,[r0]\n\t"
                         "str   r5,[r0]\n\t"
                         "str   r6,[r0]\n\t"
                         "sub   r2,r2,#4\n\t"
                         "b     Start1\n\t"
                         "Finish1 :"
                         "pop   {r3-r6,pc}\n\t");
}
#endif
#endif
#ifdef ARCH_RISCV
void Sec_Eng_PKA_Write_Block(uint32_t *dest, const uint32_t *src, uint32_t len)
{
    uint32_t wrLen = len - len % 4;
    uint32_t i;

    for (i = 0; i < wrLen; i++) {
        *dest = src[i];
    }
}
#endif

static void Sec_Eng_PKA_Get_Result(uint32_t *result, uint8_t retSize, uint16_t regLen)
{
    uint32_t ret_data = 0x00;
    int index = 0x00;

    /* Wait for the result */
    Sec_Eng_PKA_Wait_ISR();
    Sec_Eng_PKA_Clear_Int();
    Sec_Eng_PKA_Read_Block(result, (uint32_t *)(SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET), retSize);
    index = retSize - (retSize % 4);

    while (index < retSize) {
        ret_data = readl(SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);
        result[index] = ret_data;
        index++;
    }
}

void Sec_Eng_PKA_Write_Data(SEC_ENG_PKA_REG_SIZE_Type regType, uint8_t regIndex, const uint32_t *data, uint16_t size, uint8_t lastOp)
{
    int index = 0x00;
    uint16_t regLen = Sec_Eng_PKA_Get_Reg_Size(regType);

    Sec_Eng_PKA_Write_Pld_Cfg(size, regIndex, regType, SEC_ENG_PKA_OP_CTLIR_PLD, lastOp);

    if (size > regLen / 4) {
        size = regLen / 4;
    }

    Sec_Eng_PKA_Write_Block((uint32_t *)(SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET), data, size);
    index = size - (size % 4);

    while (index < size) {
        writel(data[index], SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);
        index++;
    }
}

void Sec_Eng_PKA_Read_Data(SEC_ENG_PKA_REG_SIZE_Type regType, uint8_t regIdx, uint32_t *result, uint8_t retSize)
{
    uint16_t regSize;
    uint32_t dummyData = 0;

    regSize = Sec_Eng_PKA_Get_Reg_Size(regType);

    if (retSize > regSize / 4) {
        result = NULL;
        return;
    }

    Sec_Eng_PKA_Write_Pld_Cfg(retSize, regIdx, regType, SEC_ENG_PKA_OP_CFLIR_BUFFER, 1);

    writel(dummyData, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);

    Sec_Eng_PKA_Get_Result(result, retSize, regSize);
}

void Sec_Eng_PKA_CREG(SEC_ENG_PKA_REG_SIZE_Type dRegType, uint8_t dRegIdx, uint8_t size, uint8_t lastOp)
{
    uint32_t dummyData = 0;

    Sec_Eng_PKA_Write_Pld_Cfg(size, dRegIdx, dRegType, SEC_ENG_PKA_OP_CLIR, lastOp);
    writel(dummyData, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_Write_Immediate(SEC_ENG_PKA_REG_SIZE_Type regType, uint8_t regIndex, uint32_t data, uint8_t lastOp)
{
    struct pka0_pldi_cfg cfg;

    cfg.value.BF.rsvd = 0;
    cfg.value.BF.d_reg_index = regIndex;
    cfg.value.BF.d_reg_type = regType;
    cfg.value.BF.op = SEC_ENG_PKA_OP_SLIR;
    cfg.value.BF.last_op = lastOp;

    writel(cfg.value.WORD, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);
    writel(data, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_NREG(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx, uint8_t lastOp)
{
    uint32_t dummyData = 0;

    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_NLIR, lastOp);
    writel(dummyData, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_Move_Data(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx, uint8_t lastOp)
{
    uint32_t dummyData = 0;

    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_MOVDAT, lastOp);
    writel(dummyData, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_RESIZE(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx, uint8_t lastOp)
{
    uint32_t dummyData = 0;

    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_RESIZE, lastOp);
    writel(dummyData, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_MADD(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx,
                      uint8_t s1RegType, uint8_t s1RegIdx, uint8_t s2RegType, uint8_t s2RegIdx, uint8_t lastOp)
{
    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_MADD, lastOp);
    Sec_Eng_PKA_Write_Common_OP_Snd_Cfg_S1_S2(s1RegIdx, s1RegType, s2RegIdx, s2RegType);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_MSUB(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx,
                      uint8_t s1RegType, uint8_t s1RegIdx, uint8_t s2RegType, uint8_t s2RegIdx, uint8_t lastOp)
{
    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_MSUB, lastOp);
    Sec_Eng_PKA_Write_Common_OP_Snd_Cfg_S1_S2(s1RegIdx, s1RegType, s2RegIdx, s2RegType);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_MREM(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx,
                      uint8_t s2RegType, uint8_t s2RegIdx, uint8_t lastOp)
{
    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_MREM, lastOp);
    Sec_Eng_PKA_Write_Common_OP_Snd_Cfg_S2(s2RegIdx, s2RegType);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_MMUL(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx,
                      uint8_t s1RegType, uint8_t s1RegIdx, uint8_t s2RegType, uint8_t s2RegIdx, uint8_t lastOp)
{
    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_MMUL, lastOp);
    Sec_Eng_PKA_Write_Common_OP_Snd_Cfg_S1_S2(s1RegIdx, s1RegType, s2RegIdx, s2RegType);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_MSQR(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx,
                      uint8_t s2RegType, uint8_t s2RegIdx, uint8_t lastOp)
{
    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_MSQR, lastOp);
    Sec_Eng_PKA_Write_Common_OP_Snd_Cfg_S2(s2RegIdx, s2RegType);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_MEXP(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx,
                      uint8_t s1RegType, uint8_t s1RegIdx, uint8_t s2RegType, uint8_t s2RegIdx, uint8_t lastOp)
{
    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_MEXP, lastOp);
    Sec_Eng_PKA_Write_Common_OP_Snd_Cfg_S1_S2(s1RegIdx, s1RegType, s2RegIdx, s2RegType);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_MINV(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx,
                      uint8_t s2RegType, uint8_t s2RegIdx, uint8_t lastOp)
{
    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_MINV, lastOp);
    Sec_Eng_PKA_Write_Common_OP_Snd_Cfg_S2(s2RegIdx, s2RegType);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_LCMP(uint8_t *cout, uint8_t s0RegType, uint8_t s0RegIdx, uint8_t s1RegType, uint8_t s1RegIdx)
{
    uint32_t pka0_ctrl = 0x00;

    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, 0, 0, SEC_ENG_PKA_OP_LCMP, 1);
    Sec_Eng_PKA_Write_Common_OP_Snd_Cfg_S1(s1RegIdx, s1RegType);

    Sec_Eng_PKA_Wait_ISR();
    Sec_Eng_PKA_Clear_Int();
    pka0_ctrl = readl(SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);

    *cout = (pka0_ctrl & SEC_ENG_PKA_STATUS_LAST_OPC_MASK) >> SEC_ENG_PKA_STATUS_LAST_OPC_OFFSET;
}

void Sec_Eng_PKA_LADD(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx,
                      uint8_t s1RegType, uint8_t s1RegIdx, uint8_t lastOp)
{
    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_LADD, lastOp);
    Sec_Eng_PKA_Write_Common_OP_Snd_Cfg_S1(s1RegIdx, s1RegType);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_LSUB(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx,
                      uint8_t s1RegType, uint8_t s1RegIdx, uint8_t lastOp)
{
    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_LSUB, lastOp);
    Sec_Eng_PKA_Write_Common_OP_Snd_Cfg_S1(s1RegIdx, s1RegType);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_LMUL(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx,
                      uint8_t s1RegType, uint8_t s1RegIdx, uint8_t lastOp)
{
    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_LMUL, lastOp);
    Sec_Eng_PKA_Write_Common_OP_Snd_Cfg_S1(s1RegIdx, s1RegType);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_LSQR(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx, uint8_t lastOp)
{
    uint32_t dummyData = 0;

    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_LSQR, lastOp);
    writel(dummyData, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_LDIV(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx,
                      uint8_t s2RegType, uint8_t s2RegIdx, uint8_t lastOp)
{
    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_LDIV, lastOp);
    Sec_Eng_PKA_Write_Common_OP_Snd_Cfg_S2(s2RegIdx, s2RegType);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_LMUL2N(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx,
                        uint16_t bit_shift, uint8_t lastOp)
{
    struct pka0_bit_shift_op_cfg cfg;

    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_LMUL2N, 0);

    cfg.value.BF.bit_shift = bit_shift;
    writel(cfg.value.WORD, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_LDIV2N(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx,
                        uint16_t bit_shift, uint8_t lastOp)
{
    struct pka0_bit_shift_op_cfg cfg;

    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_LDIV2N, 0);

    cfg.value.BF.bit_shift = bit_shift;
    writel(cfg.value.WORD, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_LMOD2N(uint8_t dRegType, uint8_t dRegIdx, uint8_t s0RegType, uint8_t s0RegIdx,
                        uint16_t bit_shift, uint8_t lastOp)
{
    struct pka0_bit_shift_op_cfg cfg;

    Sec_Eng_PKA_Write_Common_OP_First_Cfg(s0RegIdx, s0RegType, dRegIdx, dRegType, SEC_ENG_PKA_OP_MOD2N, lastOp);

    cfg.value.BF.bit_shift = bit_shift;
    writel(cfg.value.WORD, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_RW_OFFSET);

    if (lastOp) {
        Sec_Eng_PKA_Wait_ISR();
        Sec_Eng_PKA_Clear_Int();
    }
}

void Sec_Eng_PKA_GF2Mont(uint8_t dRegType, uint8_t dRegIdx, uint8_t sRegType, uint8_t sRegIdx, uint32_t size,
                         uint8_t tRegType, uint8_t tRegIdx, uint8_t pRegType, uint8_t pRegIdx)
{
    Sec_Eng_PKA_LMUL2N(tRegType, tRegIdx, sRegType, sRegIdx, size, 0);
    Sec_Eng_PKA_MREM(dRegType, dRegIdx, tRegType, tRegIdx, pRegType, pRegIdx, 1);
}

void Sec_Eng_PKA_Mont2GF(uint8_t dRegType, uint8_t dRegIdx, uint8_t aRegType, uint8_t aRegIdx, uint8_t invrRegType, uint8_t invrRegIdx,
                         uint8_t tRegType, uint8_t tRegIdx, uint8_t pRegType, uint8_t pRegIdx)
{
    Sec_Eng_PKA_LMUL(tRegType, tRegIdx, aRegType, aRegIdx, invrRegType, invrRegIdx, 0);
    Sec_Eng_PKA_MREM(dRegType, dRegIdx, tRegType, tRegIdx, pRegType, pRegIdx, 1);
}

int Sec_Eng_Group0_Request_PKA_Access(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

    if (((tmpVal >> 6) & 0x03) == 0x03) {
        /* SHA control idle, write to request*/
        writel(0x02, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_PROT_OFFSET);

        /* Check request result */
        tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

        if (((tmpVal >> 6) & 0x03) == 0x01) {
            return SUCCESS;
        }
    }

    return ERROR;
}

int Sec_Eng_Group0_Release_PKA_Access(void)
{
    writel(0x06, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_PROT_OFFSET);

    return SUCCESS;
}

int Sec_Eng_Group1_Request_PKA_Access(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

    if (((tmpVal >> 6) & 0x03) == 0x03) {
        /* SHA control idle, write to request*/
        writel(0x04, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_PROT_OFFSET);

        /* Check request result */
        tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

        if (((tmpVal >> 6) & 0x03) == 0x02) {
            return SUCCESS;
        }
    }

    return ERROR;
}

int Sec_Eng_Group1_Release_PKA_Access(void)
{
    writel(0x06, SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_PROT_OFFSET);

    return SUCCESS;
}

void Sec_Eng_GMAC_Enable_LE(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_GMAC_0_T_ENDIAN);
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_GMAC_0_H_ENDIAN);
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_GMAC_0_X_ENDIAN);
    writel(tmpVal, SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_GMAC_IRQHandler);
    Interrupt_Handler_Register(SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_GMAC_IRQHandler);
#endif
}

void Sec_Eng_GMAC_Enable_BE(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_GMAC_0_T_ENDIAN);
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_GMAC_0_H_ENDIAN);
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_GMAC_0_X_ENDIAN);
    writel(tmpVal, SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_GMAC_IRQHandler);
    Interrupt_Handler_Register(SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn, SEC_GMAC_IRQHandler);
#endif
}

void Sec_Eng_GMAC_Enable_Link(void)
{
    uint32_t tmpVal;

    /* Enable gmac link mode */
    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
    writel(reg_set_bit(tmpVal, SEC_ENG_SE_GMAC_0_EN), SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
}

void Sec_Eng_GMAC_Disable_Link(void)
{
    uint32_t tmpVal;

    /* Disable gmac link mode */
    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
    writel(reg_clr_bit(tmpVal, SEC_ENG_SE_GMAC_0_EN), SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
}

int Sec_Eng_GMAC_Link_Work(uint32_t linkAddr, const uint8_t *in, uint32_t len, uint8_t *out)
{
    uint32_t GMACx = SEC_ENG_BASE;
    uint32_t tmpVal;
    uint32_t timeoutCnt = SEC_ENG_GMAC_BUSY_TIMEOUT_COUNT;

    /* Link address should word align */
    if ((linkAddr & 0x03) != 0 || len % 16 != 0) {
        return ERROR;
    }

    /* Wait finished */
    do {
        tmpVal = readl(GMACx + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_GMAC_0_BUSY));

    /* Set link address */
    writel(linkAddr, GMACx + SEC_ENG_SE_GMAC_0_LCA_OFFSET);

    /* Change source buffer address */
    *(uint32_t *)(uintptr_t)(linkAddr + 4) = (uint32_t)(uintptr_t)in;

    /* Set data length */
    *((uint16_t *)(uintptr_t)linkAddr + 1) = len / 16;

    /* Start gmac engine and wait finishing */
    tmpVal = readl(GMACx + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
    writel(reg_set_bit(tmpVal, SEC_ENG_SE_GMAC_0_TRIG_1T), GMACx + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
    timeoutCnt = SEC_ENG_GMAC_BUSY_TIMEOUT_COUNT;

    do {
        tmpVal = readl(GMACx + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    } while (reg_is_bit_set(tmpVal, SEC_ENG_SE_GMAC_0_BUSY));

    /* Get result */
    ARCH_MemCpy_Fast(out, (uint8_t *)(uintptr_t)(linkAddr + 0x18), 16);

    return SUCCESS;
}

#ifndef IPRO_USE_HAL_DRIVER

static void SEC_Eng_IntHandler(SEC_ENG_INT_Type intType)
{
    uint32_t tmpVal;

    /* Check the parameters */

    switch (intType) {
        case SEC_ENG_INT_TRNG:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

            if (reg_is_bit_set(tmpVal, SEC_ENG_SE_TRNG_0_INT)) {
                /* Clear interrupt */
                writel(reg_set_bit(tmpVal, SEC_ENG_SE_TRNG_0_INT_CLR_1T), SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

                /* Call the callback function */
                if (secEngIntCbfArra[SEC_ENG_INT_TRNG] != NULL) {
                    secEngIntCbfArra[SEC_ENG_INT_TRNG]();
                }
            }

            break;

        case SEC_ENG_INT_AES:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_OFFSET);

            if (reg_is_bit_set(tmpVal, SEC_ENG_SE_AES_0_INT)) {
                /* Clear interrupt */
                writel(reg_set_bit(tmpVal, SEC_ENG_SE_AES_0_INT_CLR_1T), SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_OFFSET);

                /* Call the callback function */
                if (secEngIntCbfArra[SEC_ENG_INT_AES] != NULL) {
                    secEngIntCbfArra[SEC_ENG_INT_AES]();
                }
            }

            break;

        case SEC_ENG_INT_SHA:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

            if (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_INT)) {
                /* Clear interrupt */
                writel(reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_INT_CLR_1T), SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

                /* Call the callback function */
                if (secEngIntCbfArra[SEC_ENG_INT_SHA] != NULL) {
                    secEngIntCbfArra[SEC_ENG_INT_SHA]();
                }
            }

            break;

        case SEC_ENG_INT_PKA:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);

            if (reg_is_bit_set(tmpVal, SEC_ENG_SE_PKA_0_INT)) {
                /* Clear interrupt */
                writel(reg_set_bit(tmpVal, SEC_ENG_SE_PKA_0_INT_CLR_1T), SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);

                /* Call the callback function */
                if (secEngIntCbfArra[SEC_ENG_INT_PKA] != NULL) {
                    secEngIntCbfArra[SEC_ENG_INT_PKA]();
                }
            }

            break;

        case SEC_ENG_INT_CDET:

            /* Call the callback function */
            if (secEngIntCbfArra[SEC_ENG_INT_CDET] != NULL) {
                secEngIntCbfArra[SEC_ENG_INT_CDET]();
            }

            break;

        case SEC_ENG_INT_GMAC:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);

            if (reg_is_bit_set(tmpVal, SEC_ENG_SE_GMAC_0_INT)) {
                /* Clear interrupt */
                writel(reg_set_bit(tmpVal, SEC_ENG_SE_GMAC_0_INT_CLR_1T), SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);

                /* Call the callback function */
                if (secEngIntCbfArra[SEC_ENG_INT_GMAC] != NULL) {
                    secEngIntCbfArra[SEC_ENG_INT_GMAC]();
                }
            }

            break;

        default:
            break;
    }
}
#endif

int Sec_Eng_Group0_Request_GMAC_Access(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

    if (((tmpVal >> 10) & 0x03) == 0x03) {
        /* SHA control idle, write to request*/
        writel(0x02, SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_PROT_OFFSET);

        /* Check request result */
        tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

        if (((tmpVal >> 10) & 0x03) == 0x01) {
            return SUCCESS;
        }
    }

    return ERROR;
}

int Sec_Eng_Group0_Release_GMAC_Access(void)
{
    writel(0x06, SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_PROT_OFFSET);

    return SUCCESS;
}

int Sec_Eng_Group1_Request_GMAC_Access(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

    if (((tmpVal >> 10) & 0x03) == 0x03) {
        /* SHA control idle, write to request*/
        writel(0x04, SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_PROT_OFFSET);

        /* Check request result */
        tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

        if (((tmpVal >> 10) & 0x03) == 0x02) {
            return SUCCESS;
        }
    }

    return ERROR;
}

int Sec_Eng_Group1_Release_GMAC_Access(void)
{
    writel(0x06, SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_PROT_OFFSET);

    return SUCCESS;
}

void SEC_Eng_IntMask(SEC_ENG_INT_Type intType, int intMask)
{
    uint32_t tmpVal;

    /* Check the parameters */

    switch (intType) {
        case SEC_ENG_INT_TRNG:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                writel(reg_clr_bit(tmpVal, SEC_ENG_SE_TRNG_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
            } else {
                /* MASK(Disable) this interrupt */
                writel(reg_set_bit(tmpVal, SEC_ENG_SE_TRNG_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
            }

            break;

        case SEC_ENG_INT_AES:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_OFFSET);

            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                writel(reg_clr_bit(tmpVal, SEC_ENG_SE_AES_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_OFFSET);
            } else {
                /* MASK(Disable) this interrupt */
                writel(reg_set_bit(tmpVal, SEC_ENG_SE_AES_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_OFFSET);
            }

            break;

        case SEC_ENG_INT_SHA:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                writel(reg_clr_bit(tmpVal, SEC_ENG_SE_SHA_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            } else {
                /* MASK(Disable) this interrupt */
                writel(reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            }

            break;

        case SEC_ENG_INT_PKA:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);

            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                writel(reg_clr_bit(tmpVal, SEC_ENG_SE_PKA_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
            } else {
                /* MASK(Disable) this interrupt */
                writel(reg_set_bit(tmpVal, SEC_ENG_SE_PKA_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
            }

            break;

        case SEC_ENG_INT_CDET:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);

            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                writel(reg_clr_bit(tmpVal, SEC_ENG_SE_CDET_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);
            } else {
                /* MASK(Disable) this interrupt */
                writel(reg_set_bit(tmpVal, SEC_ENG_SE_CDET_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);
            }

            break;

        case SEC_ENG_INT_GMAC:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);

            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                writel(reg_clr_bit(tmpVal, SEC_ENG_SE_GMAC_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
            } else {
                /* MASK(Disable) this interrupt */
                writel(reg_set_bit(tmpVal, SEC_ENG_SE_GMAC_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
            }

            break;

        case SEC_ENG_INT_ALL:
            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
                writel(reg_clr_bit(tmpVal, SEC_ENG_SE_TRNG_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
                tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_OFFSET);
                writel(reg_clr_bit(tmpVal, SEC_ENG_SE_AES_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_OFFSET);
                tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
                writel(reg_clr_bit(tmpVal, SEC_ENG_SE_SHA_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
                tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
                writel(reg_clr_bit(tmpVal, SEC_ENG_SE_PKA_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
                tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
                writel(reg_clr_bit(tmpVal, SEC_ENG_SE_GMAC_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
            } else {
                /* MASK(Disable) this interrupt */
                tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
                writel(reg_set_bit(tmpVal, SEC_ENG_SE_TRNG_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
                tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_OFFSET);
                writel(reg_set_bit(tmpVal, SEC_ENG_SE_AES_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_OFFSET);
                tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
                writel(reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
                tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
                writel(reg_set_bit(tmpVal, SEC_ENG_SE_PKA_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
                tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
                writel(reg_set_bit(tmpVal, SEC_ENG_SE_GMAC_0_INT_MASK), SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
            }

            break;

        default:
            break;
    }
}

void SEC_Eng_ClrIntStatus(SEC_ENG_INT_Type intType)
{
    uint32_t tmpVal;

    /* Check the parameters */

    switch (intType) {
        case SEC_ENG_INT_AES:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_OFFSET);
            writel(reg_set_bit(tmpVal, SEC_ENG_SE_AES_0_INT_CLR_1T), SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_OFFSET);
            break;

        case SEC_ENG_INT_SHA:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            writel(reg_set_bit(tmpVal, SEC_ENG_SE_SHA_0_INT_CLR_1T), SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            break;

        case SEC_ENG_INT_TRNG:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
            writel(reg_set_bit(tmpVal, SEC_ENG_SE_TRNG_0_INT_CLR_1T), SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
            break;

        case SEC_ENG_INT_PKA:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
            writel(reg_set_bit(tmpVal, SEC_ENG_SE_PKA_0_INT_CLR_1T), SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
            break;

        case SEC_ENG_INT_CDET:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);
            writel(reg_set_bit(tmpVal, SEC_ENG_SE_CDET_0_INT_CLR), SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);
            while (reg_get_bits(readl(SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET), SEC_ENG_SE_CDET_0_INT));
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);
            writel(reg_clr_bit(tmpVal, SEC_ENG_SE_CDET_0_INT_CLR), SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);
            break;

        case SEC_ENG_INT_GMAC:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
            writel(reg_set_bit(tmpVal, SEC_ENG_SE_GMAC_0_INT_CLR_1T), SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
            break;

        default:
            break;
    }
}

#ifndef IPRO_USE_HAL_DRIVER
void SEC_Eng_Int_Callback_Install(SEC_ENG_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */

    secEngIntCbfArra[intType] = cbFun;
}
#endif

int SEC_Eng_GetIntStatus(SEC_ENG_INT_Type intType)
{
    uint32_t tmpVal;
    int status = RESET;

    /* Check the parameters */

    switch (intType) {
        case SEC_ENG_INT_AES:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_OFFSET);

            if (reg_is_bit_set(tmpVal, SEC_ENG_SE_AES_0_INT)) {
                status = SET;
            }

            break;

        case SEC_ENG_INT_SHA:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

            if (reg_is_bit_set(tmpVal, SEC_ENG_SE_SHA_0_INT)) {
                status = SET;
            }

            break;

        case SEC_ENG_INT_TRNG:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

            if (reg_is_bit_set(tmpVal, SEC_ENG_SE_TRNG_0_INT)) {
                status = SET;
            }

            break;

        case SEC_ENG_INT_PKA:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);

            if (reg_is_bit_set(tmpVal, SEC_ENG_SE_PKA_0_INT)) {
                status = SET;
            }

            break;

        case SEC_ENG_INT_CDET:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);

            if (reg_is_bit_set(tmpVal, SEC_ENG_SE_CDET_0_INT)) {
                status = SET;
            }
            break;

        case SEC_ENG_INT_GMAC:
            tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);

            if (reg_is_bit_set(tmpVal, SEC_ENG_SE_GMAC_0_INT)) {
                status = SET;
            }

            break;

        default:
            break;
    }

    return status;
}

#ifndef IPRO_USE_HAL_DRIVER

void SEC_TRNG_IRQHandler(void)
{
    SEC_Eng_IntHandler(SEC_ENG_INT_TRNG);
}

void SEC_PKA_IRQHandler(void)
{
    SEC_Eng_IntHandler(SEC_ENG_INT_PKA);
}

void SEC_AES_IRQHandler(void)
{
    SEC_Eng_IntHandler(SEC_ENG_INT_AES);
}

void SEC_SHA_IRQHandler(void)
{
    SEC_Eng_IntHandler(SEC_ENG_INT_SHA);
}

void SEC_CDET_IRQHandler(void)
{
    SEC_Eng_IntHandler(SEC_ENG_INT_CDET);
}

void SEC_GMAC_IRQHandler(void)
{
    SEC_Eng_IntHandler(SEC_ENG_INT_GMAC);
}
#endif

void ATTR_TCM_SECTION SEC_Eng_Turn_On_Sec_Ring(void)
{
    uint32_t tmpVal = 0;

    /* Turn-on Sec Ring Oscillation */
    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_3_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_TRNG_0_ROSC_EN);
    writel(tmpVal, SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_3_OFFSET);
}

void ATTR_TCM_SECTION SEC_Eng_Turn_Off_Sec_Ring(void)
{
    uint32_t tmpVal = 0;

    /* Turn-off Sec Ring Oscillation */
    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_3_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_TRNG_0_ROSC_EN);
    writel(tmpVal, SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_3_OFFSET);
}

int Sec_Eng_Group0_Request_CDET_Access(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

    if (((tmpVal >> 8) & 0x03) == 0x03) {
        /* CDET control idle, write to request*/
        writel(0x02, SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_PROT_OFFSET);

        /* Check request result */
        tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

        if (((tmpVal >> 8) & 0x03) == 0x01) {
            return SUCCESS;
        }
    }

    return ERROR;
}

int Sec_Eng_Group0_Release_CDET_Access(void)
{
    /* CDET control idle, write to request*/
    writel(0x06, SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_PROT_OFFSET);

    return SUCCESS;
}

int Sec_Eng_Group1_Request_CDET_Access(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

    if (((tmpVal >> 8) & 0x03) == 0x03) {
        /* SHA control idle, write to request*/
        writel(0x04, SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_PROT_OFFSET);

        /* Check request result */
        tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);

        if (((tmpVal >> 8) & 0x03) == 0x02) {
            return SUCCESS;
        }
    }

    return ERROR;
}

int Sec_Eng_Group1_Release_CDET_Access(void)
{
    writel(0x06, SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_PROT_OFFSET);

    return SUCCESS;
}

void Sec_Eng_CDET_Init(uint16_t xtalNum)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_CDET_0_EN);
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_CDET_0_MODE);
    writel(tmpVal, SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);

    writel(0x0000FFFF, SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_1_OFFSET);

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SEC_ENG_SE_CDET_0_T_LOOP_N, xtalNum - 1);
    writel(tmpVal, SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_2_OFFSET);
}

void Sec_Eng_CDET_Start(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_CDET_0_EN);
    writel(tmpVal, SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);

    while (1) {
        tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);
        tmpVal = reg_get_bits(tmpVal, SEC_ENG_SE_CDET_0_BUSY);
        if (tmpVal) {
            break;
        } else {

        }
    }
}

uint32_t Sec_Eng_CDET_GetXtalFreq(void)
{
    uint32_t tmpVal = 0;
    uint64_t xtalNum, rcNum;

    while (1) {
        tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);
        tmpVal = reg_get_bits(tmpVal, SEC_ENG_SE_CDET_0_BUSY);
        if (tmpVal) {

        } else {
            break;
        }
    }

    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_3_OFFSET);
    xtalNum = tmpVal & 0xFFFF;
    rcNum = (tmpVal >> 16) & 0xFFFF;

    return (uint32_t)((32000000 * xtalNum +  rcNum / 2)/ rcNum);
}


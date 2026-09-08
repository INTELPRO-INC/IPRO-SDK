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

#ifndef __UTILS_CRC_H__
#define __UTILS_CRC_H__
#include <stdint.h>

extern const uint32_t utils_crc_crc32_tab[256];
uint16_t utils_crc16(const void *dataIn, uint32_t len);
uint32_t utils_crc32_accumulate(uint32_t crc, const void *dataIn, uint32_t len);
uint32_t utils_crc32(const void *dataIn, uint32_t len);
uint16_t utils_crc16_ccitt(const void *dataIn, uint32_t len);

struct crc32_stream_ctx {
    uint32_t crc;
};

void utils_crc32_stream_init(struct crc32_stream_ctx *ctx);
void utils_crc32_stream_feed(struct crc32_stream_ctx *ctx, uint8_t data);
void utils_crc32_stream_feed_block(struct crc32_stream_ctx *ctx, const uint8_t *data, uint32_t len);
uint32_t utils_crc32_stream_results(struct crc32_stream_ctx *ctx);

#endif

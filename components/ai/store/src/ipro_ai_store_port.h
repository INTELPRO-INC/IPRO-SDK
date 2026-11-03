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
 * @file ipro_ai_store_port.h
 * @brief What the model store needs from the platform: partitions, flash, CRC.
 *
 * ipro_ai_store_port_ipro7.c implements it with hal_boot2, hal_flash and
 * utils_crc32; test_host/fake_flash.c with a NOR flash in RAM that can lose
 * power after any operation.
 */
#ifndef IPRO_AI_STORE_PORT_H
#define IPRO_AI_STORE_PORT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Physical address and size of a partition; 0 on success. */
int store_port_partition(const char *name, uint32_t *addr, uint32_t *size);
/** XIP pointer for a physical address, NULL when it is not mapped. */
const uint8_t *store_port_xip(uint32_t addr);

int store_port_erase(uint32_t addr, uint32_t len);            /* sector aligned */
int store_port_write(uint32_t addr, const uint8_t *data, uint32_t len);
int store_port_read(uint32_t addr, uint8_t *data, uint32_t len);

uint32_t store_port_crc32(uint32_t crc, const uint8_t *data, uint32_t len);  /* zlib CRC-32, chained */
void     store_port_yield(void);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_AI_STORE_PORT_H */

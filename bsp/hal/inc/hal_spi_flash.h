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

#ifndef __HAL_SPI_FLASH__H__
#define __HAL_SPI_FLASH__H__

#include "hal_common.h"

void spi_flash_set_gpio(uint8_t clk, uint8_t cs, uint8_t mosi, uint8_t miso);
int spi_flash_init(void);
int spi_flash_read_jedec_id(uint8_t *data);
int spi_flash_read(uint32_t addr, uint8_t *data, uint32_t len);
int spi_flash_write(uint32_t addr, uint8_t *data, uint32_t len);
int spi_flash_erase(uint32_t startaddr, uint32_t len);

#endif

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

#ifndef __LMAC154_PDS_H__
#define __LMAC154_PDS_H__

/****************************************************************************//**
 * @brief  Store registers to backup memory
 *
 * @param  retentionMem: retention memory to hold registers configurations
 *
 * @return None
 *
*******************************************************************************/
void lmac154_sleepStoreRegs(uint32_t *retentionMem);

/****************************************************************************//**
 * @brief  Restore registers from backup memory
 *
 * @param  retentionMem: retention memory hold register configurations
 *
 * @return None
 *
*******************************************************************************/
void lmac154_sleepRestoreRegs(uint32_t *retentionMem);

#endif
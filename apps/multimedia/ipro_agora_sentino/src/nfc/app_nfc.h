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

#ifndef APP_NFC_H
#define APP_NFC_H

/**
 * @brief Initialize NFC reader hardware
 * @return 0 on success, negative on failure
 */
int app_nfc_init(void);

/**
 * @brief NFC tag callback type
 * @param tag_id NFC tag identifier string
 */
typedef void (*nfc_tag_cb_t)(const char *tag_id);

/**
 * @brief Set callback for NFC tag detection
 */
void app_nfc_set_callback(nfc_tag_cb_t cb);

#endif /* APP_NFC_H */

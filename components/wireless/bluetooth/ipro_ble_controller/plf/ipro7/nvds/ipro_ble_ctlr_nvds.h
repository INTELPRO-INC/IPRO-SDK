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
 ****************************************************************************************
 * @file ipro_ble_ctlr_nvds.h
 *
 * @brief NVDS 種入 —— 把 IPRO7 的裝置身分與時脈精度交給 controller。
 ****************************************************************************************
 */

#ifndef IPRO_BLE_CTLR_NVDS_H_
#define IPRO_BLE_CTLR_NVDS_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * 初始化 NVDS 並種入必要的參數。
 *
 * 必須在 `rwip_init()` **之前**呼叫 —— `rwip_init()` 會把 `rwip_param.get`
 * 綁到 `nvds_get`（`upstream/.../rwip.c:782`），而 `llm_init()` 緊接著就讀
 * `PARAM_ID_BD_ADDRESS`。`ipro_ble_ctlr_controller_init()` 已經處理了順序。
 *
 * ## 為什麼是 RAM 模式
 *
 * 見 `CMakeLists.txt` 的 `CFG_NVDS_RAM` 註解：上游的 flash 後端會把
 * `flash_identify()` 的結果比對三個**寫死的**型號常數，IPRO7 的 flash
 * 不是其中任何一個，走 flash 模式就得謊報型號。
 *
 * RAM 模式下 `nvds_init()` 發現魔術字不對會自己 `nvds_init_memory()`
 * 建一份空的，所以開機時 RAM 是垃圾也沒關係。
 *
 * `PARAM_ID_SLEEP_ENABLE=1` 啟用的是 RivieraWaves BLE IP 自身深睡，
 * 不是 IPRO7 platform PDS。平台 PDS 由 APP/FreeRTOS tickless policy 先確認
 * BLE full idle window，再交給 `ipro_ble_ctlr_plf_deep_sleep()` 做最後 admission；
 * 未 grant、profile 不支援或 timing 不安全時一律回到 core-retentive WFI。
 *
 * @return true 表示 BD address 來自 efuse（每顆晶片唯一）；
 *         false 表示用了開發用的固定位址（見 .c 的說明）。
 */
bool ipro_ble_ctlr_nvds_init(void);

/// 取得目前生效的 BD address（6 bytes，BLE 的 LSB-first 排列）。
void ipro_ble_ctlr_nvds_bd_addr_get(uint8_t addr[6]);

#endif /* IPRO_BLE_CTLR_NVDS_H_ */

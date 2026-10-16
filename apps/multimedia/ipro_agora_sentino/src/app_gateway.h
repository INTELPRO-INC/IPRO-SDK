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
 * @file app_gateway.h
 * @brief Gateway orchestrator — manages the startup sequence and module lifecycle
 */

#ifndef APP_GATEWAY_H
#define APP_GATEWAY_H

#include "app_module.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Module lifecycle state */
typedef enum {
    MODULE_STATE_IDLE    = 0,
    MODULE_STATE_INIT    = 1,
    MODULE_STATE_RUNNING = 2,
    MODULE_STATE_ERROR   = 3,
} module_state_t;

/** Gateway configuration — holds all module ops */
typedef struct {
    const network_ops_t      *network;
    const audio_input_ops_t  *audio_in;
    const audio_output_ops_t *audio_out;
    const rtc_ops_t          *rtc;
    const cloud_ops_t        *cloud;
} gateway_config_t;

/**
 * @brief Initialize and start the gateway.
 *
 * Creates the gateway FreeRTOS task that executes the startup sequence:
 *  Phase 1: BLE provisioning (if needed)
 *  Phase 2: Network (LTE/EMAC) connect
 *  Phase 3: Cloud (MQTT) connect + bind report
 *  Phase 4: Audio hardware init
 *  Phase 5: RTC join (Agora)
 *  Phase 6: Wire audio route + start capture
 *
 * @param cfg Gateway configuration with all module ops
 * @return APP_OK on success
 */
app_err_t app_gateway_start(const gateway_config_t *cfg);

/**
 * @brief Get the current state of a specific module phase.
 * @param phase Phase index (0-5)
 */
module_state_t app_gateway_get_state(int phase);

#ifdef __cplusplus
}
#endif

#endif /* APP_GATEWAY_H */

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

#pragma once

/* Controller initialization before scheduler; host starts after Wi-Fi RF init. */
int br_ble_controller_prepare(void);
int br_ble_management_start(void);
int br_ble_management_ready(void);
/* Called only right before hal_reboot(): after the reboot response has been
 * delivered, or once a committed factory reset makes the reboot unconditional. */
void br_ble_management_quiesce(void);

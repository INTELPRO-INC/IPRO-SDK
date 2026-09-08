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

#include "atbm_hal.h"
int atbm_config_dpll(struct atbmwifi_common *hw_priv,char* value,int prjType,int dpllClock);
int atbm_config_dcxo(struct atbmwifi_common *hw_priv,char *value,int prjType,int dcxoType,int dpllClock);
int atbm_wait_wlan_rdy(struct atbmwifi_common *hw_priv);
int atbm_system_done(struct atbmwifi_common *hw_priv);
int atbm_config_jtag_mode(struct atbmwifi_common *hw_priv);
void atbm_set_config_to_smu(struct atbmwifi_common *hw_priv,int dpllClock);
void atbm_set_config_to_smu_apolloB(struct atbmwifi_common *hw_priv,int dpllClock);

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

#ifndef __WL_BOARD_CFG_H__
#define __WL_BOARD_CFG_H__

enum board_ctl_ops {
  /* @ rf configuration start {  */
  BRD_CTL_RF_RESET_DEFAULT,
  BRD_CTL_RF_RESTORE_LP_BZ,

  BRD_CTL_RF_INIT_WLAN,
  BRD_CTL_RF_DEINIT_WLAN,

  BRD_CTL_RF_INIT_BZ,
  BRD_CTL_RF_DEINIT_BZ,

  BRD_CTL_RF_INIT_ALL,
  BRD_CTL_RF_DEINIT_ALL,

  BRD_CTL_RF_SET_XTAL,
  BRD_CTL_RF_SET_CAPCODE,
  /* } rf configuration end @ */

};

int hal_board_ctl(enum board_ctl_ops ops, ...);

#endif

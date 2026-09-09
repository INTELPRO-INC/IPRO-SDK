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

#include <stdint.h>
#include <stdarg.h>
#include <errno.h>

#include "wl_board_cfg.h"

#include "wl_api.h"
#include "rfparam_adapter.h"

/* IPRO6 uses IPRO7 drivers which don't have HBN_Get_Xtal_Value */
/* Use default 40MHz xtal frequency */
#define IPRO6_DEFAULT_XTAL_FREQ_HZ  40000000

#define USER_UNUSED(a) ((void)(a))

#if defined(WL_API_RMEM_EN) && WL_API_RMEM_EN
/* WL/RF cal RMEM address is owned by the chip-level HBN RAM manager, not hardcoded.
 * Must match the RF CLI (cli_app.c) so both paths share one cal slot. The old
 * 0x20090600 collided with the HBN sysinfo/wake-ctx/boot2 regions. Pre-seed
 * HBN_RAM_BASE so the HAL header does not pull in ipro6.h (RF_BASE clash). */
#define HBN_RAM_BASE  ((uint32_t)0x20090000)
#include "hal_hbnram_mgnt.h"
#define WL_API_RMEM_ADDR    HAL_HBNRAM_WLCAL_ADDR
#endif

static int ctl_rf_configuration(enum board_ctl_ops ops, va_list args)
{
  struct wl_cfg_t *wl_cfg;
  int ret;

#if defined(WL_API_RMEM_EN) && WL_API_RMEM_EN
  wl_cfg = wl_cfg_get((uint8_t *)WL_API_RMEM_ADDR);
#else
  wl_cfg = wl_cfg_get();
#endif

  switch (ops) {
  case BRD_CTL_RF_RESET_DEFAULT:
      do {
        uint32_t xtal_value = IPRO6_DEFAULT_XTAL_FREQ_HZ;
        int full_cal = va_arg(args, int);

        /* reset to default param */
        wl_cfg->en_param_load = full_cal ? 1 : 0;
        wl_cfg->en_full_cal = full_cal ? 1 : 0;
        wl_cfg->mode = WL_API_MODE_ALL;
        wl_cfg->param.xtalfreq_hz = xtal_value;
        //wl_cfg->param.xtalcapcode_in = 32;
        //wl_cfg->param.xtalcapcode_out = 32;
        wl_cfg->capcode_set = rfparam_set_capcode;
        wl_cfg->capcode_get = rfparam_get_capcode;
#ifdef BL_PSM_RFPARAM 
        if(wl_cfg->en_param_load){
            extern char *ef_get_env(const char *key);
            char *psm_rfparam_enable = ef_get_env("psm_rfparam_enable");
            if((NULL != psm_rfparam_enable) && (strlen(psm_rfparam_enable) == 1) && (*psm_rfparam_enable == '1')){
                wl_cfg->param_load = rfparam_load_psm;
            }else{
                wl_cfg->param_load = rfparam_load;
            }   
        }
#else  
        if(wl_cfg->en_param_load){
            wl_cfg->param_load = rfparam_load;        
        }
#endif
      } while(0);
      break;

  case BRD_CTL_RF_RESTORE_LP_BZ:
      do {
        struct wl_cfg_t *wl_cfg;
        wl_cfg = wl_cfg_get((uint8_t *)WL_API_RMEM_ADDR);
        wl_cfg->mode = WL_API_MODE_BZ;
        wl_lp_init((uint8_t*)WL_API_RMEM_ADDR,2412);
      }while(0);
      break;

  case BRD_CTL_RF_INIT_WLAN:
    wl_cfg->mode = WL_API_MODE_WLAN;
    break;

  case BRD_CTL_RF_INIT_BZ:
    wl_cfg->mode = WL_API_MODE_BZ;
    break;

  case BRD_CTL_RF_INIT_ALL:
    wl_cfg->mode = WL_API_MODE_ALL;
    break;

  case BRD_CTL_RF_SET_XTAL:
    do {
      int xtal = va_arg(args, int);
      wl_cfg->param.xtalfreq_hz = xtal;
    } while (0);
    break;

  case BRD_CTL_RF_SET_CAPCODE:
    do {
      int cap_in = va_arg(args, int);
      int cap_out = va_arg(args, int);
      wl_cfg->param.xtalcapcode_in = cap_in;
      wl_cfg->param.xtalcapcode_out = cap_out;
    } while (0);
    break;

  default:
    break;
  }

  ret = wl_init();
  if (ret != WL_API_STATUS_OK) {
    return -EINVAL;
  }
  wl_cfg->en_param_load = 0;
  wl_cfg->en_full_cal = 0;

  return 0;
}

/* board configuration */
int hal_board_ctl(enum board_ctl_ops ops, ...)
{
  va_list ops_arg;
  int ret = -ENOSYS;

  va_start(ops_arg, ops);

  if (ops >= BRD_CTL_RF_RESET_DEFAULT && ops <= BRD_CTL_RF_SET_CAPCODE) {
    ret = ctl_rf_configuration(ops, ops_arg);
  } else {
    va_end(ops_arg);
    return -ENOSYS;
  }

  va_end(ops_arg);
  return ret;
}

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

#ifndef _IPRO7_RF_PRIVATE_H_
#define _IPRO7_RF_PRIVATE_H_

#include <stdint.h>

//#define DBG_ROSCAL

/* RF_SRAM_START_ADDR (the OCRAM word offset the modem DMAs cal IQ samples to)
 * is no longer a fixed 0x2000 — that offset (CPU 0x11018000) was never reserved
 * and collided with FreeRTOS idle/timer stacks. It is now derived at runtime
 * from a dedicated linker-reserved buffer; see g_rf_cal_scratch /
 * rf_cal_scratch_word() in ipro7_rf_private.c. */
#define OCRAM_BASE_ADDR    0x42020000   /* OCRAM as seen by the RF modem bus */
#define ROSCAL_NRECORD     256
//add from rccal
//#define DBG_RCCAL
#define RCCAL_NRECORD     256
#define RBB_GAIN_NUM 16
#define BT_CHANNEL_NUM 40
#define ZB_CHANNEL_NUM 8
//#define PWR_TBL_LEN 19
// #define GC_TBL_LEN 18
#define TX_PWR_OS_MFG_LEN 4
#define TX_PWR_OS_ZB_MFG_LEN 16

typedef struct
{
    uint32_t gain_ctrl_rosdac_q_bw0        :  6; /* [ 5: 0],        r/w,       0x20 */
    uint32_t gain_ctrl_rosdac_i_bw0        :  6; /* [11: 6],        r/w,       0x20 */
    uint32_t gain_ctrl_rosdac_q_bw1        :  6; /* [17:12],        r/w,       0x20 */
    uint32_t gain_ctrl_rosdac_i_bw1        :  6; /* [23:18],        r/w,       0x20 */
    uint32_t gain_ctrl_g_rbb2              :  3; /* [26:24],        r/w,        0x0 */
    uint32_t reserved_27                   :  1; /* [   27],       rsvd,        0x0 */
    uint32_t gain_ctrl_g_rbb1              :  2; /* [29:28],        r/w,        0x0 */
    uint32_t reserved_30_31                :  2; /* [31:30],       rsvd,        0x0 */
} rosdac_gain_ctrl_tag;

typedef struct
{
    uint32_t rbb_cap2_fc_q     : 6;
    uint32_t reserved_6_7      : 2;
    uint32_t rbb_cap2_fc_i     : 6;
    uint32_t reserved_14_15    : 2;
    uint32_t rbb_cap1_fc_q     : 6;
    uint32_t reserved_22_23    : 2;
    uint32_t rbb_cap1_fc_i     : 6;
    uint32_t reserved_30_31    : 2;
} rf_rccal_tag;

typedef struct
{
    uint32_t rf_lo_kcal_ratio       : 10;
    uint32_t reserved_10_11         : 2;
    uint32_t adpll_sdm_dither_en    : 1;
    uint32_t reserved_13_31         : 19;
} rf_lo_config_tag;

typedef struct 
{
    uint32_t lo_vco_idac_ll     : 6;
    uint32_t reserved_6_7       : 2;
    uint32_t lo_vco_idac_lh     : 6;
    uint32_t reserved_14_15     : 2;
    uint32_t lo_vco_idac_hl     : 6;
    uint32_t reserved_22_23     : 2;
    uint32_t lo_vco_idac_hh     : 6;
    uint32_t reserved_30_31     : 2;
} rf_lo_acal_tag;

typedef volatile struct
{
    rosdac_gain_ctrl_tag rx_offset_data[RBB_GAIN_NUM];
    // 
    rf_rccal_tag rccal_data;
    // 
    rf_lo_config_tag bt_lo_config_data[BT_CHANNEL_NUM];
    rf_lo_config_tag zb_lo_config_data[ZB_CHANNEL_NUM];
    // 
    rf_lo_acal_tag lo_acal_data;
	// 
    int16_t tsen_value_init;
	uint8_t tsen_value_status;
} rf_calibration_data_tag;

typedef struct
{
    /*
    uint32_t RF_PA;
    uint32_t RF_TRX_GAIN_BW;
    uint32_t RF_LODIST;
    uint32_t RF_PUCR_TX;
    uint32_t BZ_PHY_PHY_TX_DFE_1;
    // 
    uint32_t pa_dac_code_init;
    */
    int32_t tx_pwr_offset_zb;
    int32_t tx_target_power;
    int32_t tx_target_power_zb;
    int8_t tx_pwr_os_mfg[TX_PWR_OS_MFG_LEN];
    int8_t tx_pwr_os_zb_mfg[TX_PWR_OS_ZB_MFG_LEN];
    // 
    // do not need init
    uint8_t tx_pwr_table_index;
    uint8_t tx_pwr_table_index_zb;
    uint8_t pa_ref_dac_zb;
} rf_power_data_tag;

enum{
	MODE_IDLE = 0,
	MODE_TX,
	MODE_RX,
	MODE_LO_ACAL,
	MODE_LO_KCAL,
	MODE_RCCAL,
	MODE_ROSCAL
};

enum{
	RATE_BLE1M = 0,
    RATE_BLE2M = 1,
    RATE_ZB250K = 2,
};

/* public api */
/* Init, Self-cal, Optimize */
void rf_init(uint32_t force_init);
void rf_init_lp(uint32_t flag);
void rf_update_or(uint32_t flag);

void rf_full_calibration(void);

uint32_t rf_acal(void);
uint16_t acal_core(void);
void rf_set_vco_idac(uint8_t idac);

uint32_t rf_kcal(void);
uint16_t kcal_core(void);

uint32_t rf_roscal(void);
uint32_t roscal_core(void);
void ros_result(int32_t *ros_iq, int32_t *average_i, int32_t *average_q);
void rf_set_rosdac(uint8_t rosdac_i, uint8_t rosdac_q);
void rf_sram_dump(void);
void rf_enable_dfe(void);
void rf_disable_dfe(void);

uint32_t rf_rccal(void);
uint32_t rccal_get_vpp(uint32_t iq);
void rf_set_rccap(uint8_t cap);

void rf_restore_calibration_value(void);
void rf_save_state_for_cal(void);
void rf_pucr_manually_for_cal(uint8_t mode);
void rf_switch_channel_manually(double lo_freq);
void rf_restore_state_for_cal(void);

void rf_optimize_by_rate(uint8_t rate_index);
void rf_optimize_by_channel(uint32_t channel);       //unit is MHz
void rf_set_channel_rx_sw(uint32_t channel, uint8_t rate_index);
void rf_set_channel_tx_sw(uint32_t channel);
void rf_set_channel_hw(void);

/* Power Control */
void rf_tx_pwr_init(uint8_t init, uint8_t lpmd);
void rf_set_pa_ref_dac_adj(void);
void rf_set_tx_power_table_regs(uint8_t index);
void rf_set_tx_power_regs_zb(uint8_t index, uint8_t pa_ref_dac);
uint8_t rf_set_tx_power(int32_t target_power);
uint8_t rf_set_tx_power_zb(int32_t target_power);
int32_t rf_query_tx_power();
int32_t rf_query_tx_power_zb();
// void rf_tx_power_store(void);
// void rf_tx_power_restore(void);
// uint32_t rf_get_txgain_index(int32_t target_power);
// float rf_gain_dB_to_ceof(float gain_dB);
void rf_set_pwr_offset(int8_t tx_pwr_os[TX_PWR_OS_MFG_LEN]);
void rf_set_pwr_offset_zb(int8_t tx_pwr_os[TX_PWR_OS_ZB_MFG_LEN]);
void rf_set_pwr_offset_regs();
// int32_t rf_get_pwr_offset();

/* Temperature Compensation */
uint32_t rf_set_init_tsen_value(int16_t tsen_value);
int16_t rf_get_init_tsen_value(void);
uint32_t rf_inc_cal_tsen_based(int16_t tsen_value);
void rf_inc_acal(int16_t tsen_value);
void rf_inc_power_cal(int16_t tsen_value);

/* Test API */
//void rf_xtal32m_offset(uint32_t capcode_in,uint32_t capcode_out);
void rf_enable_cont_zb_rx();
void rf_disable_cont_zb_rx();
void rf_enable_cont_tx();
void rf_disable_cont_tx();

/* External API */
extern uint32_t ipro_timer_now_us(void);

#endif

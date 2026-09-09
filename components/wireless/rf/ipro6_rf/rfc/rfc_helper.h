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

#ifndef _RFC_HELPER_H_
#define _RFC_HELPER_H_

#include "co_int.h"
#include "reg_rf.h"
#include "wl_config.h"

enum
{
    RFC_OFF = 0,
    RFC_ON  = 1
};

enum {
    RFC_FORMATMOD_11B = 0,
    RFC_FORMATMOD_11G = 1,
    RFC_FORMATMOD_11N = 2,
    RFC_FORMATMOD_11AC = 3,
    RFC_FORMATMOD_11AX = 4
};

enum 
{
    RFC_PM_MUX_TO_ADC = 0,
    RFC_PM_MUX_TO_IQCOMP = 1,
    RFC_PM_MUX_TO_BLE = 2
};

enum 
{
    RFC_TXDFE_MUX_TO_BB = 0,
    RFC_TXDFE_MUX_TO_SRAM = 1,
    RFC_TXDFE_MUX_TO_SINGEN = 2,
    RFC_TXDFE_MUX_TO_PAD = 3
};

enum
{
    RFC_RC_IDLE = 0,
    RFC_RC_RX2ON = 1,
    RFC_RC_TX2ON = 2,
    RFC_RC_RXON = 3,
    RFC_RC_TX2PAON = 4,
    RFC_RC_TXPAON = 7,
    RFC_RC_FORCE_OFF = 15
};


enum 
{
    RFC_FSM_PD = 0,
    RFC_FSM_SB = 1,
    RFC_FSM_LO = 2,
    RFC_FSM_RX = 3,
    RFC_FSM_TX = 4,
    RFC_FSM_FORCE_OFF = 15
};

enum BBMODE_enum
{
    RFC_BBMODE_WLAN = 0,
    RFC_BBMODE_BLE = 1
};

enum 
{
	RFC_SG_SINGLE_TONE = 0,
	RFC_SG_TWO_TONE,
	RFC_SG_RAMP
};

enum {
    RFC_PC_AUTO = 0,
    RFC_PC_WLAN_11B,
    RFC_PC_WLAN_11G,
    RFC_PC_WLAN_11N,
    RFC_PC_BT_BLE
};

enum {
    RFC_BW_NA  = 0,
    RFC_BW_5M  = 1,
    RFC_BW_10M = 2,
    RFC_BW_20M = 3
};

struct rfc_status_tag {
    /** test read 1 */
    uint32_t pkdet_out_raw            : 1;  //[0]
    uint32_t dig_xtal_clk_dbg         : 1;  //[1]
    uint32_t clk_ble_16m_dbg          : 1;  //[2]
    uint32_t clk_rc_dbg0              : 1;  //[3]
    uint32_t clk_adcpow_dbg           : 1;  //[4]
    uint32_t clk_fetx_dbg             : 1;  //[5]
    uint32_t clk_ferx_dbg             : 1;  //[6]
    uint32_t clkpll_postdiv_outclk_dbg: 1;  //[7]
    uint32_t clk_soc_480m_dbg         : 1;  //[8]
    uint32_t clk_soc_240m_dbg         : 1;  //[9]
    uint32_t clk_soc_192m_dbg         : 1;  //[10]
    uint32_t clk_soc_160m_dbg         : 1;  //[11]
    uint32_t clk_soc_120m_dbg         : 1;  //[12]
    uint32_t clk_soc_96m_dbg          : 1;  //[13]
    uint32_t clk_soc_80m_dbg          : 1;  //[14]
    uint32_t clk_soc_48m_dbg          : 1;  //[15]
    uint32_t clk_soc_32m_dbg          : 1;  //[16]
    uint32_t pad_pkdet_out            : 1;  //[17]
    uint32_t pad_agc_ctrl             :10;  //[27:18]
    uint32_t rf_pkdet_rst_hw          : 1;  //[28]
    uint32_t rf_cbw_wifi              : 2;  //[30:29]
    uint32_t lo_unlocked              : 1;  //[31]

    /** test read 2 */
    uint32_t fsm_pu_txbuf             : 1;  //[0]
    uint32_t fsm_pu_rxbuf             : 1;  //[1]
    uint32_t fsm_pu_tosdac            : 1;  //[2]
    uint32_t fsm_pu_dac               : 1;  //[3]
    uint32_t fsm_trsw_en              : 1;  //[4]
    uint32_t fsm_pu_adc               : 1;  //[5]
    uint32_t fsm_pu_pkdet             : 1;  //[6]
    uint32_t fsm_pu_rbb               : 1;  //[7]
    uint32_t fsm_pu_rmx               : 1;  //[8]
    uint32_t fsm_pu_rmxgm             : 1;  //[9]
    uint32_t fsm_pu_lna               : 1;  //[10]
    uint32_t clk_rc_dbg2              : 1;  //[11]
    uint32_t rf_lna_ind_hw            : 4;  //[15:12]
    uint32_t rf_rbb_ind_hw            : 4;  //[19:16]
    uint32_t rf_tx_pow_lvl_hw         : 4;  //[23:20]
    uint32_t rf_rc_lo_rdy             : 1;  //[24]
    uint32_t rf_fsm_state             : 3;  //[27:25]
    uint32_t rf_rc_state              : 3;  //[30:28]
    uint32_t clk_rc_dbg               : 1;  //[31]
};

union rfc_pwritem_tag {
    struct {
        uint32_t iqc_gain       : 11;
        uint32_t gc_dg_qdb      : 7;
        uint32_t gc_tmx         : 3;
        uint32_t gc_tbb         : 5;
        uint32_t gc_tbb_boost   : 2;
        uint32_t gc_dac_bias    : 2;
        uint32_t ax_submode     : 2;

        uint32_t dpd_sel        : 2;
        uint32_t tosdac_q       : 6;
        uint32_t tosdac_i       : 6;
        uint32_t iqc_phase      : 10;
        uint32_t gc_pa          : 3;
        uint32_t pa_lp_en       : 1;
        uint32_t dpd_en_b       : 1;
        uint32_t dpd_en_g       : 1;
        uint32_t dpd_en_n       : 1;
        uint32_t dpd_en_ax      : 1;
    } bf;
    uint32_t word[2];
};

struct rfc_pwrtbl_tag {
    union {
        struct {
            uint32_t pwr_0 : 6;
            uint32_t pwr_1 : 6;
            uint32_t pwr_2 : 6;
            uint32_t pwr_3 : 6;
            uint32_t m154_tx_pwr        : 8;

            uint32_t pwr_4 : 6;
            uint32_t pwr_5 : 6;
            uint32_t pwr_6 : 6;
            uint32_t pwr_7 : 6;
            uint32_t wifi_tx_corr       : 8;

            uint32_t pwr_8 : 6;
            uint32_t pwr_9 : 6;
            uint32_t pwr_10 : 6;
            uint32_t pwr_11 : 6;
            uint32_t pwr_12 : 6;
            uint32_t unused_31_30 : 2;
        } bf;

        uint32_t word[3];
    } wifi_nominal_pwr;

    union rfc_pwritem_tag wifi_pwrtbl[13];

    uint32_t bz_tx_backoff[3];
    
    uint32_t bz_tx_channel_corr;

    uint32_t bz_tx_temperature_corr;

    union {
        struct {
            uint32_t pwr_0 : 6;
            uint32_t pwr_1 : 6;
            uint32_t pwr_2 : 6;
            uint32_t pwr_3 : 6;
            uint32_t reserved_pwrctrl23_24_31 : 8;
        } bf;

        uint32_t word[1];
    } bz_nominal_pwr;

    union rfc_pwritem_tag bz_pwrtbl[4];

    union {
        struct {
            uint32_t dsss_corr      : 8;
            uint32_t dg0_qdb        : 8;
            uint32_t pwr_req_qdb    : 8;
            uint32_t tbl_idx        : 4;
            uint32_t reserved_28_30 : 3;
            uint32_t tx_reduce_pwr_ena : 1;
        } bf;
        uint32_t word;
     } rf_tx_pwr_sts;
};

/*
 * DECLARATIONS
 ****************************************************************************************
 */
void wait_us(uint32_t);
void wait_ms(uint32_t);
uint32_t read_reg(uint32_t addr);
void write_reg(uint32_t addr,uint32_t value);
void write_reg_bits(uint32_t addr, uint8_t high_bit, uint8_t low_bit, uint32_t wdata);

/*
 * TXDFE DECLARATIONS
 ****************************************************************************************
 */
static inline void rfc_txdfe_start()
{
    rf_tx_dfe_en_4s_en_setf(1);
    rf_tx_dfe_en_4s_setf(0); wait_us(10);
    rf_tx_dfe_en_4s_setf(1);
}

static inline void rfc_txdfe_stop()
{
    rf_tx_dfe_en_4s_setf(0);
    rf_tx_dfe_en_4s_en_setf(0);
}

static inline void rfc_txdfe_source_mux(int8_t signal_source)
{
    rf_tx_test_sel_setf(signal_source);
}

static inline void rfc_txdfe_set_dvga0(int8_t dvga_qdb)  // DVGA apply only at packet start
{
    if (dvga_qdb < -48 || dvga_qdb > +24) {
        wl_logprintf(DBG_RFC "dvga_qdb out of range -48~+24,skip\r\n");
        return;
    }
    rf_tx_dvga_0_qdb_setf(dvga_qdb);
}

static inline void rfc_txdfe_set_dvga1(int8_t dvga_qdb)
{
    if (dvga_qdb < -48 || dvga_qdb > +24) {
        wl_logprintf(DBG_RFC "dvga_qdb out of range -48~+24,skip\r\n");
        return;
    }
    rf_tx_dvga_1_qdb_setf(dvga_qdb);
}

static inline void rfc_txdfe_set_dvga2(int8_t dvga_qdb)
{
    if (dvga_qdb < -48 || dvga_qdb > +24) {
        wl_logprintf(DBG_RFC "dvga_qdb out of range -48~+24,skip\r\n");
        return;
    }
    rf_tx_dvga_2_qdb_setf(dvga_qdb);
}

static inline void rfc_txdfe_set_iqgaincomp(uint8_t en,uint16_t coeff)
{
    rf_tx_iqc_gain_en_setf(en);
	rf_tx_iqc_gain_setf(coeff);
}

static inline void rfc_txdfe_set_iqphasecomp(uint8_t en,int16_t coeff)
{
    rf_tx_iqc_phase_en_setf(en);
	rf_tx_iqc_phase_setf(coeff);
}

static inline void rfc_txdfe_set_dccomp(int16_t dcc_i,int16_t dcc_q)
{
    rf_tx_dac_os_i_setf(dcc_i);
    rf_tx_dac_os_q_setf(dcc_q);
}

static inline void rfc_hwctrl_txdvga0(int8_t hwctrl)
{
    rf_tx_dvga_0_ctrl_hw_setf(hwctrl);
}

static inline void rfc_hwctrl_txdvga1(int8_t hwctrl)
{
    rf_tx_dvga_1_ctrl_hw_setf(hwctrl);
}

/*
 * RXDFE DECLARATIONS
 ****************************************************************************************
 */
static inline void rfc_rxdfe_start()
{
    rf_rx_dfe_en_4s_setf(1);
    rf_rx_dfe_en_4s_en_setf(1);
}

static inline void rfc_rxdfe_stop()
{
    rf_rx_dfe_en_4s_setf(0);
	rf_rx_dfe_en_4s_en_setf(0);
}

static inline void rfc_rxdfe_set_iqgaincomp(uint8_t en,uint16_t coeff)
{
    rf_rx_iqc_gain_en_setf(en);
	rf_rx_iqc_gain_setf(coeff);
}

static inline void rfc_rxdfe_set_iqphasecomp(uint8_t en,int16_t coeff)
{
	rf_rx_iqc_phase_en_setf(en);
	rf_rx_iqc_phase_setf(coeff);
}

static inline void rfc_rxdfe_set_notch1(uint8_t en,uint8_t alpha,int16_t nrmfc)
{
    rf_rx_notch1_alpha_setf(alpha);
    rf_rx_notch1_fnrm_setf(nrmfc);
    rf_rx_notch1_en_setf(en);
}

static inline void rfc_rxdfe_set_notch2(uint8_t en,uint8_t alpha,int16_t nrmfc)
{
    rf_rx_notch2_alpha_setf(alpha);
    rf_rx_notch2_fnrm_setf(nrmfc);
    rf_rx_notch2_en_setf(en);
}

/*
 * SG DECLARATIONS
 ****************************************************************************************
 */
void rfc_sg_start(int32_t freq_hz,uint32_t gain,uint32_t cplx_flag);
void rfc_sg_stop();

/*
 * PM DECLARATIONS
 ****************************************************************************************
 */
uint32_t rfc_power_meas(uint32_t insel,int32_t freq_cw,uint32_t acclen,uint32_t rshift,
                        int32_t *raw_acc_i,int32_t *raw_acc_q);

/*
 * FSM CONTROL
 ****************************************************************************************
 */
void rfc_rc_fsm_force(uint8_t state);
void rfc_rf_fsm_force(uint8_t state);
void rfc_coex_force(uint32_t force_enable, uint32_t bbmode);

/*
 * LO CONTROL
 ****************************************************************************************
 */

/*
 * POWER CONTROL
 ****************************************************************************************
 */
static inline void rfc_hwctrl_txgain(uint8_t hwctrl_on)
{
    rf_tx_gain_ctrl_hw_setf(hwctrl_on);
}

static inline void rfc_hwctrl_rxgain(uint8_t hwctrl_on)
{
    rf_rx_gain_ctrl_hw_setf(hwctrl_on);
}

static inline void rfc_hwctrl_txcal(uint8_t hwctrl_on)
{
    rf_txcal_ctrl_hw_setf(hwctrl_on); // FIXME: rf_txcal_ctrl_hw_setf(hwctrl_on);
}

static inline void rfc_hwctrl_rxcal(uint8_t hwctrl_on)
{
    rf_rxcal_ctrl_hw_setf(hwctrl_on); // FIXME: rf_rxcal_ctrl_hw_setf(hwctrl_on);
}

/*
 * NOTCH CONTROL
 ****************************************************************************************
 */
static inline void rfc_hwctrl_notch(uint8_t hwctrl_on)
{
    // TODO
}

static inline void rfc_set_notch_table()
{
    // TODO
}

/*
 * MISC (Diag/Coex...)
 ****************************************************************************************
 */
void rfc_wlan_mode_force(uint32_t force_mode);
void rfc_dump();
void rfc_apply_tx_dvga_offset(int8_t offset);
void rfc_apply_tx_dvga(int8_t dvga_qdb[16]);
void rfc_config_bandwidth(uint8_t mode);
// private narrowband channel (CBW10/CBW5) via whole-clock-tree scaling.
// cbw = 5 or 10 enters narrowband, 20 restores standard mode; idempotent.
// Callers must ensure TX/RX are stopped and the PHY is otherwise in the
// standard 20MHz channel configuration before switching.
void rfc_narrowband_set(uint8_t cbw);
uint8_t rfc_narrowband_get(void);   // 0 = standard, else 5/10
// TX power compensation applied on entry (dvga2, quarter-dB units); defaults
// -12 (10M) / -24 (5M). Set before or while in narrowband; re-entering applies it.
void rfc_narrowband_pwr_comp(uint8_t cbw, int8_t qdb);
int8_t rfc_narrowband_pwr_comp_get(uint8_t cbw);
void rfc_config_power(uint32_t mode,uint32_t tbb_boost,uint32_t tbb,uint32_t tmx);


#endif

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
 * @file shell_phy.c
 * @brief PHY/RF shell commands for IPRO6
 *
 * This file provides shell interface for PHY/RF testing and configuration.
 * It wraps the existing CLI command implementations with shell-compatible
 * function signatures.
 *
 * NOTE: This file must be compiled into the shell library (not ipro6_rf)
 * to ensure FSymTab symbols are included via whole-archive linking.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "cli_app.h"

/*
 * Shell command signature: int cmd(int argc, char **argv)
 * CLI command signature:   void cmd(char *buf, int len, int argc, char **argv)
 *
 * Wrapper macro to adapt CLI commands to shell interface
 */
#define SHELL_PHY_CMD(shell_name, cli_func, desc) \
    static int shell_##shell_name(int argc, char **argv) { \
        cli_func(NULL, 0, argc, argv); \
        return 0; \
    } \
    SHELL_CMD_EXPORT_ALIAS(shell_##shell_name, shell_name, desc)

/* PHY Initialization */
SHELL_PHY_CMD(phy_init, cmd_phy_init_register, init rf driver);
SHELL_PHY_CMD(phy_channel, cmd_channel_register, configure work channel frequency);
SHELL_PHY_CMD(phy_bandwidth, cmd_bandwidth_register, configure phy bandwidth);
SHELL_PHY_CMD(ltfp, cmd_ltfp_register, private LTF polarity table);
SHELL_PHY_CMD(rf_channel, cmd_rf_channel_register, configure rf channel hw control);

/* Crystal configuration */
SHELL_PHY_CMD(xtalcapcode, cmd_xtalcapcode, configure xtal capcode);

/* TX Test Commands */
SHELL_PHY_CMD(txsin_start, cmd_txsin_start_register, transmit single tone);
SHELL_PHY_CMD(txsin_stop, cmd_txsin_stop_register, stop transmit single tone);
SHELL_PHY_CMD(tx11b_start, cmd_tx11b_start_register, start transmit 11b frames);
SHELL_PHY_CMD(tx11g_start, cmd_tx11g_start_register, start transmit 11g frames);
SHELL_PHY_CMD(tx11n_start, cmd_tx11n_start_register, start transmit 11n frames);
SHELL_PHY_CMD(tx11ax_su_start, cmd_tx11ax_su_start_register, start transmit 11ax SU frames);
SHELL_PHY_CMD(tx11ax_er_start, cmd_tx11ax_er_start_register, start transmit 11ax ER frames);
SHELL_PHY_CMD(tx11ax_mu_start, cmd_tx11ax_mu_start_register, start transmit 11ax MU frames);
SHELL_PHY_CMD(tx11ax_tb_start, cmd_tx11ax_tb_start_register, start transmit 11ax TB frames);
SHELL_PHY_CMD(tx_stop, cmd_tx_stop_register, stop transmitting);

/* RX Test Commands */
SHELL_PHY_CMD(at_rx_start, cmd_rx_start_at, start receiver for auto test);
SHELL_PHY_CMD(at_rx_poll, cmd_rx_poll_stat_at, pull receiver state for auto test);
SHELL_PHY_CMD(at_rx_clr, cmd_rx_clear_stat_at, clear receiver state for auto test);
SHELL_PHY_CMD(at_rx_stop, cmd_rx_stop_at, stop receiver state for auto test);
SHELL_PHY_CMD(rx_start_ate, cmd_rx_start_ate, start ate receiver);

/* CW Test */
SHELL_PHY_CMD(cw_start, cmd_cw_start_register, rf cw start);
SHELL_PHY_CMD(cw_stop, cmd_cw_stop_register, rf cw stop);

/* Chip and Power Configuration */
SHELL_PHY_CMD(chip_info, cmd_chip_info_input_register, rf chip info input);
SHELL_PHY_CMD(bz_set_target_power, cmd_bz_set_target_power_register, set target power of bz);
SHELL_PHY_CMD(ch_pwr_comp, cmd_ch_pwr_comp_register, channel power compensation);
SHELL_PHY_CMD(ch_lp_pwr_comp, cmd_ch_lp_pwr_comp_register, channel lp power compensation);
SHELL_PHY_CMD(bz_ch_pwr_comp, cmd_bz_ch_pwr_comp_register, channel power compensation of bz);
SHELL_PHY_CMD(temp_pwr_comp, cmd_temp_pwr_comp_register, temp power compensation);
SHELL_PHY_CMD(bz_temp_pwr_comp, cmd_bz_temp_pwr_comp_register, temp power compensation of bz);

/* RX Calibration */
SHELL_PHY_CMD(rxgain, cmd_rxgain_register, rxgain is controlled by hw);
SHELL_PHY_CMD(rxcal_hw, cmd_rxcal_hw_driven_register, rxcal is controlled by hw);
SHELL_PHY_CMD(rxcal_sw, cmd_rxcal_sw_driven_register, rxcal is controlled by sw);
SHELL_PHY_CMD(rxcal_iq, cmd_rxcal_iq_configure_register, set rx iqcomp parameters);
SHELL_PHY_CMD(rxcal_os, cmd_rxcal_os_configure_register, set rx offset parameters);
SHELL_PHY_CMD(rxcal_mode, cmd_rxcal_mode_configure_register, set rx pu for calibration mode);

/* TX Calibration */
SHELL_PHY_CMD(txgain, cmd_txgain_register, txgain is controlled by hw);
SHELL_PHY_CMD(force_txgain, cmd_force_txgain_register, DEBUG force analog tx gain regs);
SHELL_PHY_CMD(txcal_hw, cmd_txcal_hw_driven_register, txcal is controlled by hw);
SHELL_PHY_CMD(txcal_sw, cmd_txcal_sw_driven_register, txcal is controlled by sw);
SHELL_PHY_CMD(txcal_iq, cmd_txcal_iq_configure_register, configure tx iqcomp coeff);
SHELL_PHY_CMD(txcal_os, cmd_txcal_os_configure_register, configure tx offset comp coeff);

/* Debug and Dump Commands */
SHELL_PHY_CMD(dump_rfc, cmd_dump_rfc_register, dump rfc state);
SHELL_PHY_CMD(dump_rf, cmd_dump_rf_register, dump rf state);
SHELL_PHY_CMD(dump_txvec, cmd_dump_txvec_register, dump wlan tx vector);
SHELL_PHY_CMD(dump_txgain, cmd_dump_txgain_register, dump wlan tx gain);

/* TPC Commands */
SHELL_PHY_CMD(tpc_dump_wlan, cmd_tpc_dump_pwrtbl_wlan_register, dump wifi power table);
SHELL_PHY_CMD(tpc_dump_bz, cmd_tpc_dump_pwrtbl_bz_register, dump bz power table);
SHELL_PHY_CMD(tpc_update_wlan, cmd_tpc_update_pwrtbl_wlan_register, update wifi power table);

/* RC State Commands */
SHELL_PHY_CMD(rc_state_force, cmd_rc_state_force, force RC state machine);
SHELL_PHY_CMD(rc_adc_dc_meas, cmd_rc_adc_dc_meas, ADC DC measure);
SHELL_PHY_CMD(rc_adc_pwr_meas, cmd_rc_adc_pwr_meas, ADC Power measure);

/* SRAM Commands */
SHELL_PHY_CMD(sram_rx, cmd_sram_rx, write waveform to sram);
SHELL_PHY_CMD(sram_rx_addr, cmd_sram_rx_addr, query sram rx buffer address);
SHELL_PHY_CMD(rf_tx2rx, cmd_rf_tx2rx, rf loopback test);
SHELL_PHY_CMD(rf_rx, cmd_rf_rx, rf loopback test);

/* WL Driver Commands */
SHELL_PHY_CMD(wl_init, cmd_wl_init, init wl driver);
SHELL_PHY_CMD(wl_restore, cmd_wl_restore, restore wl driver);
SHELL_PHY_CMD(lp_reset, cmd_lp_reset, reset mac/phy/rf to model reboot);
SHELL_PHY_CMD(bbmode, cmd_bbmode, select WIFI or BZ baseband for the shared RF);
SHELL_PHY_CMD(bz_agc_preset, cmd_bz_agc_preset, apply BZ AGC parameter preset rtl or alt);
SHELL_PHY_CMD(lp_init, cmd_lp_init, init wl in low power mode);
SHELL_PHY_CMD(lp_dump_rmem, cmd_lp_dump_rmem, dump rmem for debug purpose);

/* RF SRAM Dump Commands */
SHELL_PHY_CMD(rf_sram_dump, cmd_rf_sram_dump, dump rf sram);

/* Read/Write Register Commands */
SHELL_PHY_CMD(rr, cmd_read_register, read register);
SHELL_PHY_CMD(rb, cmd_read_reg_partial_bits, read partial register bits);
SHELL_PHY_CMD(wr, cmd_write_register, write register);
SHELL_PHY_CMD(wb, cmd_write_reg_partial_bits, write partial register bits);

#if CFG_FPGA
SHELL_PHY_CMD(rtrf, cmd_cci_read_T1_register, read T1 CR);
SHELL_PHY_CMD(wtrf, cmd_cci_write_T1_register, write T1 CR);
SHELL_PHY_CMD(rbtrf, cmd_cci_read_T1_reg_partial_bits, read T1 CR partial bits);
SHELL_PHY_CMD(wbtrf, cmd_cci_write_T1_reg_partial_bits, write T1 CR partial bits);
#endif

#if 0 /* Zigbee commands - disabled by default */
SHELL_PHY_CMD(zb_tstmode_enter, cmd_zb_tstmode_enter, enable zigbee test mode);
SHELL_PHY_CMD(zb_tstmode_exit, cmd_zb_tstmode_exit, disable zigbee test mode);
SHELL_PHY_CMD(zb_tx_start, cmd_zb_tx_start, start zigbee tx);
SHELL_PHY_CMD(zb_tx_stop, cmd_zb_tx_stop, stop zigbee tx);
SHELL_PHY_CMD(zb_rx_start, cmd_zb_rx_start, start zigbee rx);
SHELL_PHY_CMD(zb_rx_read, cmd_zb_rx_status_read, read rx status);
SHELL_PHY_CMD(zb_rx_clr, cmd_zb_rx_status_clear, clear rx status);
#endif

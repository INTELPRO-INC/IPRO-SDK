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

#ifndef _PHYCLI_H_
#define _PHYCLI_H_

#include <stdint.h>

#define HE_USER_MAX (4)

#if WL_NIC
/* Structure for registering CLI commands */
struct cli_command 
{
    const char *name;
    const char *help;

    void (*function)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
};
#endif

#pragma pack(push, 1)
union TxVectorCommon
{
	uint8_t vector[8];
	struct
	{
		// txv0
		uint8_t format : 4;
		uint8_t cbw : 3;
		uint8_t preamble_type : 1;
		// txv1
		uint8_t antenna_set : 8;
		// txv2
		uint8_t txpwr_level : 8;
		// txv3
		uint8_t n_tx : 3;
		uint8_t txv3_rsvd_3_5 : 3;
		uint8_t time_of_departure_req : 1;
		uint8_t continuous_tx : 1;
		// txv4, txv5
		uint16_t l_length : 12;
		uint16_t l_rate : 4;
		// txv6, txv7
		uint16_t service : 16;
	} bf;
};
#pragma pack(pop)

#pragma pack(push, 1)
union TxVectorNonHT
{
	uint8_t vector[2];
	struct
	{
		// txv8
		uint8_t trigger_responding : 8;
		// txv9
		uint8_t smmindex : 8;
	} bf;
};
#pragma pack(pop)

#pragma pack(push, 1)
union TxVectorHT
{
	uint8_t vector[5];
	struct
	{
		// txv8
		uint8_t sounding : 1;
		uint8_t smoothing : 1;
		uint8_t gi_type : 1;
		uint8_t aggregation : 1;
		uint8_t stbc : 1;
		uint8_t num_ext_ss : 2;
		uint8_t txv8_rsvd_7 : 1;
		// txv9
		uint8_t smmindex : 8;
		// txv10
		uint8_t mcs : 7;
		uint8_t fec : 1;
		// txv11, txv12
		uint16_t length : 16;
	} bf;
};
#pragma pack(pop)

#pragma pack(push, 1)
union TxVectorHECommon
{
	uint8_t vector[5];
	struct
	{
		// txv8
		uint8_t sounding : 1;
		uint8_t beamformed : 1;
		uint8_t gi_type : 2;
		uint8_t stbc : 1;
		uint8_t txv8_rsvd_5_7 : 3;
		// txv9
		uint8_t uplink_flag : 1;
		uint8_t beam_change : 1;
		uint8_t dcm : 1;
		uint8_t he_ltf_type : 2;
		uint8_t doppler : 1;
		uint8_t midamble : 1;
		uint8_t txv9_rsvd_7 : 1;
		// txv10
		uint8_t bss_color : 6;
		uint8_t txv10_rsvd_6_7 : 2;
		// txv11
		uint8_t txop_duration : 7;
		uint8_t txv11_rsvd_7 : 1;
		// txv12
		uint8_t spatial_reuse : 4;
		uint8_t spatial_reuse2: 4;
	} bf;
};
#pragma pack(pop)

#pragma pack(push, 1)
union TxVectorHESU 
{
	uint8_t vector[5];
	struct
	{
		// txv13
		uint8_t smmindex : 8;
		// txv14
		uint8_t mcs : 4;
		uint8_t nss : 3;
		uint8_t fec : 1;
		// txv15,txv16,txv17
		uint32_t length : 20;
		uint32_t packet_extension : 3;
		uint32_t txv17_rsvd_7 : 1;
	} bf;
};
#pragma pack(pop)

#pragma pack(push, 1)
union TxVectorHETB
{
	uint8_t vector[11];
	struct
	{
		// txv13
		uint8_t spatial_reuse3 : 4;
		uint8_t spatial_reuse4 : 4;
		// txv14
		uint8_t he_siga_reserved_7_0 : 8;
		// txv15
		uint8_t he_siga_reserved_8 : 1;
		uint8_t he_ltf_num 		   : 3;
		uint8_t he_ltf_mode 	   : 1;
		uint8_t ldpc_extra_symbol  : 1;
		uint8_t txv15_rsvd_7_6	   : 2;
		// txv16
		uint8_t starting_sts_num   : 3;
		uint8_t txv16_rsvd_7_3     : 5;
		// txv17
		uint8_t ru_allocation      : 8;
		// txv18
		uint8_t smmindex           : 8;
		// txv19
		uint8_t mcs  			: 4;
		uint8_t nss  			: 3;
		uint8_t fec  			: 1;
		// txv20/txv21/txv22
		uint32_t length 		: 20;
		uint32_t pe     		: 3;
		uint32_t trigger_method : 1;
		// txv23
		uint8_t ru_tone_set_index : 7;
		uint8_t feedback_status   : 1;
	} bf;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TxVectorUser{
	// 0
	uint8_t smmindex : 8;

	// 1
	uint8_t mcs : 4;
	uint8_t nss : 3;
	uint8_t fec : 1;

	// 2,3,4
	uint32_t length : 20;
	uint32_t pe : 3;
	uint32_t txv3_rsvd_7 : 1;

	// 5,6
	uint16_t staid : 11;
	uint16_t user_position : 5;
};

#pragma pack(pop)

#pragma pack(push, 1)
union TxVectorHEMU
{
	uint8_t vector[6+HE_USER_MAX*7];

	struct
	{
		// txv13
		uint8_t sigb_compression_mode : 1;
		uint8_t sigb_dcm : 1;
		uint8_t sigb_mcs : 3;
		uint8_t txv13_rsvd_7_5 : 3;
		// txv14,txv15,txv16,txv17
		uint8_t ru_allocation[4];
		// txv18
		uint8_t n_user;

		struct TxVectorUser user[HE_USER_MAX];
	} bf;
};
#pragma pack(pop)

#pragma pack(push, 1)
union RxVectorCommon
{
	uint8_t vector[6];
	struct
	{
		// rxv0
		uint8_t format : 4;
		uint8_t cbw : 3;
		uint8_t preamble_type : 1;
		// rxv1
		uint8_t antenna_set : 8;
		// rxv2
		int8_t rssi_legacy : 8;
		// rxv3, rxv4
		uint16_t l_length : 12;
		uint16_t l_rate   : 4;
		// rxv5
		int8_t rssi : 8;
	} bf;
};
#pragma pack(pop)

#pragma pack(push, 1)
union RxVectorNonHT
{
	uint8_t vector[1];
	struct
	{
		// rxv6
		uint8_t dynbw_in_nonht : 1;
		uint8_t cbw_in_nonht : 2;
		uint8_t rxv6_reserved_6_3 : 4;
		uint8_t l_sig_valid : 1; 
	} bf;
};
#pragma pack(pop)

#pragma pack(push, 1)
union RxVectorHT
{
	uint8_t vector[4];
	struct
	{
		// rxv6
		uint8_t sounding : 1;
		uint8_t smoothing : 1;
		uint8_t gi_type : 1;
		uint8_t aggregation : 1;
		uint8_t stbc : 1;
		uint8_t num_ess : 2;
		uint8_t  l_sig_valid : 1;

		// rxv7
		uint8_t mcs : 7;
		uint8_t fec : 1;

		// rxv8, rxv9
		uint16_t length : 16;
	} bf;
};
#pragma pack(pop)

#pragma pack(push, 1)
union RxVectorVHT
{
	uint8_t vector[7];
	struct
	{
		// rxv6
		uint8_t sounding : 1;
		uint8_t beamformed : 1;
		uint8_t gi_type : 1;
		uint8_t rxv6_reserved_3 : 1;
		uint8_t stbc : 1;
		uint8_t doze_not_allowed : 1;
		uint8_t first_user : 1;
		uint8_t rxv6_reserved_7 : 1;

		// rxv7, rxv8
		uint16_t partial_aid : 9;
		uint16_t group_id : 6;
		uint16_t rxv8_reserved_7 : 1;

		// rxv9
		uint8_t mcs : 4;
		uint8_t nss : 3;
		uint8_t fec : 1;

		// rxv10,rxv11,rxv12
		uint32_t length : 20;
		uint32_t rxv12_served_7_4 : 4; 
	} bf;
};
#pragma pack(pop)

#pragma pack(push, 1)
union RxVectorHECommon
{
	uint8_t vector[4];
	struct
	{
		// rxv6
		uint8_t sounding : 1;
		uint8_t beamformed : 1;
		uint8_t gi_type : 2;
		uint8_t stbc : 1;
		uint8_t rxv6_reserved_7_5 : 3;

		// rxv7
		uint8_t uplink_flag : 1;
		uint8_t beam_change : 1;
		uint8_t dcm : 1;
		uint8_t he_ltf_type : 2;
		uint8_t doppler : 1;
		uint8_t rxv7_reserved_7_6 : 2;

		// rxv8
		uint8_t bss_color : 6;
		uint8_t rxv8_reserved_7_6 : 2;

		// rxv9
		uint8_t txop_duration : 7;
		uint8_t rxv9_reserved_7 : 1; 
	} bf;
};
#pragma pack(pop)

#pragma pack(push, 1)
union RxVectorHESU
{
	uint8_t vector[6];
	struct
	{
		// rxv10
		uint8_t pe_duration : 4;
		uint8_t spatial_reuse : 4;
		
		// rxv11
		uint8_t rxv11_reserved;

		// rxv12
		uint8_t mcs : 4;
		uint8_t nss : 3;
		uint8_t fec : 1;

		// rxv13,rxv14,rxv15
		uint32_t length : 20;
		uint32_t rxv15_reserved_7_4 : 4; 
	} bf;
};
#pragma pack(pop)

#pragma pack(push, 1)
union RxVectorHEMU
{
	uint8_t vector[6];
	struct
	{
		// rxv10
		uint8_t pe_duration : 4;
		uint8_t spatial_reuse : 4;
		
		// rxv11
		uint8_t sigb_compression_mode : 1;
		uint8_t sigb_dcm : 1;
		uint8_t sigb_mcs : 3;
		uint8_t ru_size : 3;

		// rxv12
		uint8_t mcs : 4;
		uint8_t nss : 3;
		uint8_t fec : 1;

		// rxv13,rxv14,rxv15
		uint32_t length : 20;
		uint32_t rxv15_reserved_7_4 : 4; 
	} bf;
};
#pragma pack(pop)

#define CLI_EXPORT
#define NOT_SET     INT32_MIN
enum {
    MACBYP_OFF = 0,
    MACBYP_ON = 1
};

typedef enum {
    MACBYP_IDLE = 0,
    MACBYP_RX   = 1,
    MACBYP_TX_SINGLE = 2,
    MACBYP_TX_BURST = 3,
    /* Legacy value: NOT decoded by the macbypass mode field on this chip (only
     * 1/2/3 exist) — writing it results in no transmission at all. Continuous
     * transmission = MACBYP_TX_BURST + frameperburst=0 (+ interframe_delay=0
     * for back-to-back). Kept only so old references fail loudly here. */
    MACBYP_TX_CONT = 4
} MacbypassMode_t;

typedef enum {
    MACBYP_PLD_PRBS = 0,
    MACBYP_PLD_ALLONE = 1,
    MACBYP_PLD_RAMP = 2,
    MACBYP_PLD_01 = 3,
    MACBYP_PLD_10 = 4
} MacbypassPldType_t;

typedef enum {
    FORMAT_NONHT = 0,
    FORMAT_NONHT_DUP = 1,
    FORMAT_HT_MM = 2,
    FORMAT_HT_GF = 3,
	FORMAT_VHT = 4,
	FORMAT_HE_SU = 5,
	FORMAT_HE_MU = 6,
	FORMAT_HE_SU_ER = 7,
	FORMAT_HE_TB = 8
} FormatMode_t;

typedef enum {
    LM_1M      = 0x0,
    LM_2M      = 0x1,
    LM_5_5M    = 0x2,
    LM_11M     = 0x3,
    LM_6MBPS   = 0xb, // LEG_6=0xb,
    LM_9MBPS   = 0xf, // LEG_9=0xf,
    LM_12MBPS  = 0xa, // LEG_12=0xa,
    LM_18MBPS  = 0xe, // LEG_18=0xe,
    LM_24MBPS  = 0x9, // LEG_24=0x9,
    LM_36MBPS  = 0xd, // LEG_36=0xd,
    LM_48MBPS  = 0x8, // LEG_48=0x8,
    LM_54MBPS  = 0xc  // LEG_54=0xc
} LegRate_t;

typedef enum {
    SHORT_PREAMBLE = 0,
    LONG_PREAMBLE = 1
} PreambleType_t;

typedef enum {
    SRAM_LINKMODE_INDEP= 0,
    SRAM_LINKMODE_ADC_TRIG_DAC = 1,
    SRAM_LINKMODE_DAC_TRIG_ADC = 2
} SramLinkMode_t;

typedef enum {
	HE_LTF_TYPE_1X = 0,
	HE_LTF_TYPE_2X = 1,
	HE_LTF_TYPE_4X = 2
} HE_LtfType_t;

typedef enum {
	HE_PE0_0US = 0,
	HE_PE2_8US = 2,
	HE_PE4_16US = 4,
} HE_PacketExtension_t;

typedef struct  {
    const char * name;
    int32_t    * value;
} arg_t;

void cli_phy_isr(void);

/* CLI command function declarations for shell wrapper */
CLI_EXPORT void cmd_xtalcapcode(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_phy_init_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_channel_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_bandwidth_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_ltfp_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_rf_channel_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_txsin_start_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_txsin_stop_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_tx11b_start_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_tx11g_start_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_tx11n_start_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_tx11ax_su_start_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_tx11ax_er_start_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_tx11ax_mu_start_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_tx11ax_tb_start_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_tx_stop_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_rx_start_at(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_rx_poll_stat_at(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_rx_clear_stat_at(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_rx_stop_at(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_rx_start_ate(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_cw_start_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_cw_stop_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_chip_info_input_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_bz_set_target_power_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_ch_pwr_comp_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_ch_lp_pwr_comp_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_bz_ch_pwr_comp_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_temp_pwr_comp_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_bz_temp_pwr_comp_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_rxgain_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_rxcal_hw_driven_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_rxcal_sw_driven_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_rxcal_iq_configure_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_rxcal_os_configure_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_rxcal_mode_configure_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_txgain_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_force_txgain_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_txcal_hw_driven_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_txcal_sw_driven_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_txcal_iq_configure_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_txcal_os_configure_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_dump_rfc_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_dump_rf_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_dump_txvec_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_dump_txgain_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_tpc_dump_pwrtbl_wlan_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_tpc_dump_pwrtbl_bz_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_tpc_update_pwrtbl_wlan_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_rc_state_force(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_rc_adc_dc_meas(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_rc_adc_pwr_meas(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_sram_rx(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_sram_rx_addr(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_rf_tx2rx(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_rf_rx(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_wl_init(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_wl_restore(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_lp_reset(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_bbmode(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_bz_agc_preset(char *buf, int len, int argc, char **argv);
/* B2: BZ ownership of the shared RF (save on enter, restore on exit) */
void rf_bz_enter(void);
void rf_bz_exit(void);
CLI_EXPORT void cmd_lp_init(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_lp_dump_rmem(char *buf, int len, int argc, char **argv);

CLI_EXPORT void cmd_rf_sram_dump(char *buf, int len, int argc, char **argv);

CLI_EXPORT void cmd_read_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_read_reg_partial_bits(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_write_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_write_reg_partial_bits(char *buf, int len, int argc, char **argv);

#if CFG_FPGA
CLI_EXPORT void cmd_cci_read_T1_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_cci_read_T1_reg_partial_bits(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_cci_write_T1_register(char *buf, int len, int argc, char **argv);
CLI_EXPORT void cmd_cci_write_T1_reg_partial_bits(char *buf, int len, int argc, char **argv);
#endif

#endif

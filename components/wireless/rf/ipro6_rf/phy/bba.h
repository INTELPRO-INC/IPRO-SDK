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

#ifndef _BBA_H_
#define _BBA_H_

#include <stdint.h>
#include "compiler.h" // for __PACKED

// enum 
// {
//     BBA_PD_HG_PDE = 0,
//     BBA_PD_HG,
//     BBA_PD_MG,
//     BBA_PD_LG,
//     BBA_PD_ULG,
//     BBA_PD_MAX
// };

enum 
{
    BBA_GAIN_HG = 0,
    BBA_GAIN_HG7,
    BBA_GAIN_MG,
    BBA_GAIN_LG,
    BBA_GAIN_HG6,
    BBA_GAIN_ML,
    BBA_GAIN_MAX
};

/// Receive Vector specific part for NON-HT and NON-HT-DUP-OFDM frames
struct rx_vect_1_leg
{
    /// Dynamic Bandwidth
    uint8_t    dyn_bw_in_non_ht     : 1;
    /// Channel Bandwidth
    uint8_t    chn_bw_in_non_ht     : 2;
    /// Not used (offset only)
    uint8_t    rsvd_nht             : 4;
    /// L-SIG Valid
    uint8_t    lsig_valid           : 1;
} __PACKED;

/// Receive Vector specific part for HT frames
struct rx_vect_1_ht
{
    /// Sounding bit
    uint16_t   sounding             : 1;
    /// Smoothing bit
    uint16_t   smoothing            : 1;
    /// Guard Interval Type bit
    uint16_t   short_gi             : 1;
    /// MPDU Aggregate bit
    uint16_t   aggregation          : 1;
    /// Space Time Block Coding bit
    uint16_t   stbc                 : 1;
    /// Number of Extension Spatial Streams
    uint16_t   num_extn_ss          : 2;
    /// L-SIG Valid
    uint16_t   lsig_valid           : 1;
    /// Modulation Coding Scheme
    uint16_t   mcs                  : 7;
    /// FEC Coding
    uint16_t   fec                  : 1;
    /// Lenght of HT PPDU
    uint16_t   length               :16;
} __PACKED;

/// Receive Vector specific part for VHT frames
struct rx_vect_1_vht
{
    /// Sounding bit
    uint8_t   sounding              : 1;
    /// BeamFormed bit
    uint8_t   beamformed            : 1;
    /// Guard Interval Type
    uint8_t   short_gi              : 1;
    /// Not used (offset only)
    uint8_t   rsvd_vht1             : 1;
    /// Space Time Block Coding
    uint8_t   stbc                  : 1;
    /// TXOP PS Not Allowed
    uint8_t   doze_not_allowed      : 1;
    /// First User
    uint8_t   first_user            : 1;
    /// Not used (offset only)
    uint8_t   rsvd_vht2             : 1;
    /// Partial AID
    uint16_t  partial_aid           : 9;
    /// Group ID
    uint16_t  group_id              : 6;
    /// Not used (offset only)
    uint16_t  rsvd_vht3             : 1;
    /// Modulation Coding Scheme
    uint32_t  mcs                   : 4;
    /// Number of Spatial Streams
    uint32_t  nss                   : 3;
    /// FEC Coding
    uint32_t  fec                   : 1;
    /// Lenght of VHT PPDU
    uint32_t  length                :20;
    /// Not used (offset only)
    uint32_t  rsvd_vht4             : 4;
} __PACKED;

/// Receive Vector specific part for HE frames
struct rx_vect_1_he
{
    /// Sounding bit
    uint8_t   sounding              : 1;
    /// BeamFormed bit
    uint8_t   beamformed            : 1;
    /// Guard Interval Type
    uint8_t   gi_type               : 2;
    /// Space Time Block Coding
    uint8_t   stbc                  : 1;
    /// Not Used (offset only)
    uint8_t   rsvd_he1              : 3;
    /// UP link Flag
    uint8_t   uplink_flag           : 1;
    /// Beam Change
    uint8_t   beam_change           : 1;
    /// Dual Carrier Modulation
    uint8_t   dcm                   : 1;
    /// Type of HE-LTF
    uint8_t   he_ltf_type           : 2;
    /// Doppler bit
    uint8_t   doppler               : 1;
    /// Not Used (offset only)
    uint8_t   rsvd_he2              : 2;
    /// BSS Color
    uint8_t   bss_color             : 6;
    /// Not Used (offset only)
    uint8_t   rsvd_he3              : 2;
    /// Duration of TX OP
    uint8_t   txop_duration         : 7;
    /// Not Used (offset only)
    uint8_t   rsvd_he4              : 1;
    /// Packet Extension Duration
    uint8_t   pe_duration           : 4;
    /// Spatial Reuse
    uint8_t   spatial_reuse         : 4;

    /// SIG-B Compression Mode
    uint8_t   sig_b_comp_mode       : 1;
    /// SIG-B Dual Carrier Modulation
    uint8_t   dcm_sig_b             : 1;
    /// SIG-B Modulation Coding Scheme
    uint8_t   mcs_sig_b             : 3;
    /// RU Size
    uint8_t   ru_size               : 3;

    /// Modulation Coding Scheme
    uint32_t  mcs                   : 4;
    /// Number of Spatial Streams
    uint32_t  nss                   : 3;
    /// FEC Coding
    uint32_t  fec                   : 1;
    /// Length of PPDU
    uint32_t  length                :20;
    /// Not Used (offset only)
    uint32_t  rsvd_he6              : 4;
} __PACKED;

/// Structure for receive Vector 1
struct rx_vector_1
{
    /// Format Modulation
    uint8_t     format_mod         : 4;
    /// Channel Bandwidth
    uint8_t     ch_bw              : 3;
    /// Preamble Type
    uint8_t     pre_type           : 1;
    /// Antenna Set
    uint8_t     antenna_set        : 8;
    /// RSSI Legacy
    int32_t     rssi_leg           : 8;
    /// Legacy Length
    uint32_t    leg_length         :12;
    /// Legacy rate
    uint32_t    leg_rate           : 4;
    /// RSSI
    int32_t     rssi1              : 8;
    union
    {
        /// non-ht and non-ht-dup-ofdm bitmap
        struct rx_vect_1_leg leg;
        /// ht-mm and ht-gf bitmap
        struct rx_vect_1_ht ht;
        /// vht bitmap
        struct rx_vect_1_vht vht;
        /// he bitmap
        struct rx_vect_1_he he;
    };
} __PACKED;

/// Structure for receive Vector 2
struct rx_vector_2
{
    /// Contains the bytes 4 - 1 of Receive Vector 2
    uint32_t            recvec2a;
    ///  Contains the bytes 8 - 5 of Receive Vector 2
    uint32_t            recvec2b;
};

struct rvec_t
{
    /// Receive Vector 1
    struct rx_vector_1  rx_vec_1;
    /// Receive Vector 2
    struct rx_vector_2  rx_vec_2;
};

int8_t calc_ppm_ofdm(uint16_t rxv_freqoff);
int8_t calc_ppm_dsss(uint8_t rxv_freqoff);
int8_t calc_ppm(struct rvec_t *rvec);

void bba_init();
void bba_reset();
void bba_reset_full();
void bba_reset_level1();
void bba_reset_level2();
void bba_update(struct rvec_t *rvec);
uint8_t bba_get_pd_state();
uint8_t bba_get_pd_gain();
uint8_t bba_get_pd_mile();
void bba_set_pd_state(uint8_t state);
void bba_set_pd_gain(uint8_t gain);
void bba_set_pd_mile(uint8_t on);
void bba_set_capcode(uint8_t capcode_new);
void bba_set_pd_ofdm(uint8_t ofdm);
void bba_set_pd_dsss(uint8_t dsss);
void bba_set_pd_rssi(int8_t rssi);
void bba_get_pd_cfg(uint8_t *ofdm, uint8_t *dsss, int8_t *rssi);
void bba_rssi_correction(void *rvec_ptr);
void bba_loop(void* rvec_ptr, uint16_t type_subtype, int8_t gainopt);

#endif

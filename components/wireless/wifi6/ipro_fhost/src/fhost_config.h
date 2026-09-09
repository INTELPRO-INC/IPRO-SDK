/**
 ****************************************************************************************
 *
 * @file fhost_config.h
 *
 * @brief Definition of configuration for Fully Hosted firmware.
 *
 * Copyright (C) RivieraWaves 2017-2021
 *
 ****************************************************************************************
 */
#ifndef _FHOST_CONFIG_H_
#define _FHOST_CONFIG_H_

/**
 ****************************************************************************************
 * @addtogroup FHOST
 * @{
 ****************************************************************************************
 */

#include "cfgrwnx.h"

#if NX_FHOST_RX_STATS
struct fhost_rx_conf_tag
{
    /// Number of BW
    uint8_t n_bw;

    /// First HT rate index
    uint16_t first_ht;
    /// Rates per HT MCS
    uint8_t ht_rates_per_mcs;

    /// First VHT rate index
    uint16_t first_vht;
    /// Max VHT MCS
    uint8_t max_vht_mcs;
    /// Rates per VHT MCS
    uint8_t vht_rates_per_mcs;

    /// First HE SU rate index
    uint16_t first_he_su;
    /// Rates per HE SU MCS
    uint8_t he_su_rates_per_mcs;

    /// First HE MU rate index
    uint16_t first_he_mu;
    /// Rates per HE MU MCS
    uint8_t he_mu_rates_per_mcs;

    /// First HE ER rate index
    uint16_t first_he_er;
    /// Rates per HE ER MCS
    uint8_t he_er_rates_per_mcs;

    /// Max HE MCS
    uint8_t max_he_mcs;
    /// Total number of rates
    uint16_t n_rates;
};

extern struct fhost_rx_conf_tag fhost_conf_rx;
#endif

extern struct me_chan_config_req fhost_chan;

/**
 ****************************************************************************************
 * @brief Initialize wifi configuration structure from fhost configuration
 *
 * To be called before initializing the wifi stack.
 * Can also be used to retrieve firmware feature list at run-time. In this case @p init
 * is false.
 *
 * @param[out] me_config     Configuration structure for the UMAC (i.e. ME task)
 * @param[out] start         Configuration structure for the LMAC (i.e. MM task)
 * @param[out] base_mac_addr Base MAC address of the device (from which all VIF MAC
 *                           addresses are computed)
 * @param[in]  init          Whether it is called before firmware initialization or not.
 ****************************************************************************************
 */
void fhost_config_prepare(struct fhost_me_config_req *me_config, struct fhost_mm_start_req *start,
                          bool init);

/**
 ****************************************************************************************
 * @brief Return the channel associated to a given frequency
 *
 * @param[in] freq Channel frequency
 *
 * @return Channel definition whose primary frequency is the requested one and NULL if
 * there no such channel.
 ****************************************************************************************
 */
struct mac_chan_def *fhost_chan_get(int freq);

/**
 ****************************************************************************************
 * @brief Toggle state of HT40 mode, configuration takes effect only before the
 *        initiation of the WiFi task.
 *
 * Enabling only permits HT40; the modem capability read by phy_get_bw() still
 * has the final say in fhost_config_prepare(), so this can never widen a part
 * that has no 40 MHz support.
 *
 * @param[in] value   Decision on whether to enable HT40 mode (1 = enable, 0 = disable)
 *
 * @return            0: Success, other: Failure
 ****************************************************************************************
 */
int fhost_set_ht40_mode_enable(uint8_t value);

/**
 ****************************************************************************************
 * @brief Get the requested HT40 mode state.
 *
 * This is the request, not the negotiated width - read `rc` (bw_max) or the
 * phy_hw_set_channel chantype trace for what the link actually runs at.
 *
 * @return            1 = HT40 permitted, 0 = forced to 20 MHz
 ****************************************************************************************
 */
uint8_t fhost_get_ht40_mode_enable(void);

#if CONFIG_COEX_WIFI_MODE
/**
 ****************************************************************************************
 * @brief Set wifi coex enable state
 *
 * @param[in] en   (1 = enable, 0 = disable)
 ****************************************************************************************
 */
void fhost_set_coex_enable(bool en);

/**
 ****************************************************************************************
 * @brief Get wifi coex enable state
 *
 * @return            (1 = enable, 0 = disable)
 ****************************************************************************************
 */
uint8_t fhost_get_coex_enable(void);
#endif
/**
 * @}
 */
#endif /* _FHOST_CONFIG_H_ */

/**
 * @file ieee802154_hal.h
 * @brief IEEE 802.15.4 Hardware Abstraction Layer Interface
 * 
 * Platform-specific HAL interface for IEEE 802.15.4 driver
 * Implement these functions for each hardware platform
 * 
 * @copyright Copyright (c) 2025 INTELPRO Inc.
 */

#ifndef __IEEE802154_HAL_H__
#define __IEEE802154_HAL_H__

#include "ieee802154.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup IEEE802154_HAL Hardware Abstraction Layer
 * @{
 */

/**
 * @brief HAL initialization
 * 
 * Initialize hardware-specific components (clocks, GPIOs, IRQs)
 * 
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_init(void);

/**
 * @brief HAL deinitialization
 * 
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_deinit(void);

/**
 * @brief Reset radio hardware
 * 
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_reset(void);

/**
 * @brief Re-register only the M154 ECLIC vector after a PDS wake.
 *
 * For LP apps that keep the Misc power domain alive across PDS31
 * (M154 state preserved) but where CPU-domain reset has wiped the
 * per-IRQ ECLIC binding. Single ECLIC register write — safe in
 * suspended-scheduler wake-hook context.
 */
void ieee802154_hal_lp_rearm_eclic(void);

/**
 * @brief Full M154 baseband resync after a PDS warm-boot.
 *
 * For LP apps that let the Misc power domain be power-cycled by PDS31
 * (M154 hardware fully reset on wake). Re-runs the essential parts of
 * ieee802154_hal_init() plus a re-apply of the cached
 * ieee802154_config_t (channel/PAN/short_addr/ext_addr/tx_power/auto_ack
 * /CCA mode/CCA threshold) and the cached FPT slot software state, then
 * re-registers the M154 ECLIC vector with the proper handler pointer.
 *
 * Covers M154 register file beyond the 0x000–0x180 region that a
 * register-level snapshot can safely cover (NBR_CTRL at 0x380, FPT
 * tables at 0x620+, COEX at 0x180, TX_PPDU_CTRL at 0x300+ etc.).
 *
 * Must be called AFTER GLB-level module reset / RF analog restore.
 * Safe in suspended-scheduler wake-hook context (no allocations, no
 * mutexes — every call inside is register-level or simple memcpy).
 *
 * @return IEEE802154_OK on success.
 *         IEEE802154_ERROR_NOT_INITIALIZED if cold-boot ieee802154_init()
 *         hasn't run yet.
 */
ieee802154_error_t ieee802154_hal_lp_resync(void);

/**
 * @brief Is a TX still in flight at the HAL/lmac154 layer?
 *
 * Returns true if either:
 *   - an ACK-requested TX is still waiting for ACK reception
 *     (s_tx_ack_pending: cleared only when lmac154_ackEvent /
 *     lmac154_ackFrameEvent dispatches the result), or
 *   - a TX completion has not yet been reported up to the driver
 *     (tx_done_reported still false after triggerTx)
 *
 * Note: lmac154_getRFState() can return IDLE the moment the TX HW
 * has finished sending (or right after CSMA fails) — but the upper
 * layers haven't seen the TX_DONE / NO_ACK / CSMA_FAILED event yet
 * because the M154 IRQ still has to be serviced. Sleeping in that
 * window clock-gates M154 mid-IRQ, swallowing the event so OT keeps
 * retrying a poll the radio actually already finished.
 */
bool ieee802154_hal_is_tx_pending(void);

/**
 * @brief True while an FP=1 data-poll ACK has opened an indirect-frame RX window.
 *
 * A Thread sleepy end device polls its parent; if the ACK has Frame-Pending=1 the
 * parent's buffered downlink arrives a few ms LATER. The LP-sleep PDS gate must
 * not power-gate the radio while this is true, or that buffered frame (e.g. a CASE
 * Sigma1 / command — the first downlink during PDS idle) is dropped on-air and the
 * Matter session desyncs. Self-clears after ~120 ms so a never-arriving frame
 * cannot wedge PDS off.
 */
bool ieee802154_hal_is_rx_pending(void);

/* ieee802154_hal_pds_busy() bits: why the radio cannot go through a PDS now */
#define IEEE802154_HAL_PDS_BUSY_TX          (1U << 0) /* TX, its ACK wait or a timed TX */
#define IEEE802154_HAL_PDS_BUSY_FP_WINDOW   (1U << 1) /* FP=1 ACK: buffered downlink inbound */
#define IEEE802154_HAL_PDS_BUSY_RX_FRAME    (1U << 2) /* a frame or our Enh-ACK on the air */
#define IEEE802154_HAL_PDS_BUSY_RX_ON       (1U << 3) /* receiver on (a window, or on when idle) */
#define IEEE802154_HAL_PDS_BUSY_MAC_HW      (1U << 4) /* the MAC hardware still receiving / sending */
#define IEEE802154_HAL_PDS_BUSY_ACK_WINDOW  (1U << 5) /* Enh-ACK acceptance window armed */
#define IEEE802154_HAL_PDS_BUSY_IRQ         (1U << 6) /* MAC interrupt not taken yet */

/**
 * @brief Why the radio cannot sleep through a platform PDS now (0: it can).
 *
 * Lock-free, IRQs may be masked.
 */
uint32_t ieee802154_hal_pds_busy(void);

/**
 * @brief Right before a platform PDS (IRQs masked): capture the MAC
 *        configuration registers and the symbol counter.
 */
void ieee802154_hal_pds_suspend(void);

/* ieee802154_hal_pds_resume() results */
#define IEEE802154_HAL_PDS_RETAINED 0    /* the MAC kept its configuration */
#define IEEE802154_HAL_PDS_RESTORED 1    /* reset by the PDS, configuration written back */
#define IEEE802154_HAL_PDS_LOST     (-1) /* not restored: ieee802154_reset() from task context */

/**
 * @brief Right after a platform PDS (IRQs masked, MTIME already rebased):
 *        write the MAC configuration back if the PDS reset it (register
 *        file, frame-pending table, symbol counter continued on the
 *        microsecond clock, interrupt vector).
 *
 * The RF is not touched: warm it up separately when the PDS powered it down.
 *
 * @return IEEE802154_HAL_PDS_RETAINED, _RESTORED or _LOST.
 */
int ieee802154_hal_pds_resume(void);

/**
 * @brief After the RF / BZ PHY came back from a reset (rf_init() after a PDS
 *        that did not keep the BZ domain): re-apply the PHY settings the HAL
 *        owns (CCA mode, ED threshold) from the current configuration.
 */
void ieee802154_hal_phy_restore(void);

/**
 * @brief Keep the RF / BZ PHY out of the next resets (lmac154_init() without
 *        its rf_init()): a MAC recovery while another radio (BLE) is live on
 *        the shared RF, which the PDS it ran kept powered.
 */
void ieee802154_hal_keep_rf_on_reset(bool keep);

/**
 * @brief IRQs masked: turn off a receiver the MAC reopened by itself while
 *        the driver has it off (the PTA's rx-on-after-abort); counted in
 *        ieee802154_hal_rx_stray_off.
 */
void ieee802154_hal_rx_stray_check(void);
extern volatile uint32_t ieee802154_hal_rx_stray_off;

/**
 * @brief Force-clear the sticky TX-pending flags.
 *
 * Normally s_tx_in_flight / s_tx_ack_pending clear only via the lmac154
 * terminal ISRs (TX_DONE / ACK / NO_ACK). If a TX is ABANDONED before its
 * terminal event fires — e.g. OT's DataPollSender is stopped while one of its
 * ACK-requested Data Requests is still in flight — the flags can stay TRUE
 * forever, so ieee802154_hal_is_tx_pending() never goes false and wedges any
 * caller that gates on it (the SED fast-poll engine then spins in a sub-gate
 * WFI loop: never polling, never entering PDS). This lets such a caller
 * reclaim the radio. tx_done_reported is set so the abandoned TX's late ISR is
 * swallowed; the next ieee802154_hal_transmit() resets it.
 */
void ieee802154_hal_clear_tx_pending(void);

/**
 * @brief Poll for hardware events (TX/RX completion)
 * 
 * Call this periodically when interrupts are not reliable.
 * Checks hardware status and triggers callbacks if events occurred.
 */
void ieee802154_hal_poll(void);

/**
 * @brief Configure radio parameters
 * 
 * @param config Configuration structure
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_configure(const ieee802154_config_t *config);

/**
 * @brief Switch the TX data rate and multi-rate RX only
 *
 * Writes just the rate registers (M154 TX PPDU control, BZ PHY P-mode), so
 * a caller changing rate per frame does not re-apply the whole config.
 *
 * @param rate IEEE802154_DATA_RATE_*
 * @param rx_multi_rate receive all four rates
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_set_data_rate(ieee802154_data_rate_t rate, bool rx_multi_rate);

/**
 * @brief Enable radio transceiver
 * 
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_enable(void);

/**
 * @brief Disable radio transceiver
 * 
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_disable(void);

/**
 * @brief Enter sleep mode
 * 
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_sleep(void);

/**
 * @brief Start RX mode
 * 
 * @param channel Channel number
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_rx_start(uint8_t channel);

/**
 * @brief Stop RX mode
 * 
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_rx_stop(void);

/**
 * @brief Transmit frame
 * 
 * @param frame Frame to transmit
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_transmit(const ieee802154_frame_t *frame);

/**
 * @brief Perform CCA (Clear Channel Assessment)
 * 
 * @param mode CCA mode
 * @param threshold Energy threshold
 * @return true if channel is clear, false otherwise
 */
bool ieee802154_hal_cca(ieee802154_cca_mode_t mode, int8_t threshold);

/**
 * @brief Perform energy detection
 * 
 * @param channel Channel to scan
 * @param duration_ms Scan duration
 * @param energy Pointer to store energy level
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_energy_detect(uint8_t channel, uint16_t duration_ms, int8_t *energy);

/**
 * @brief Get current RSSI
 * 
 * @return RSSI value in dBm
 */
int8_t ieee802154_hal_get_rssi(void);

/**
 * @brief Set TX power
 * 
 * @param power TX power in dBm
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_set_tx_power(int8_t power);

/**
 * @brief Set channel
 * 
 * @param channel Channel number (11-26)
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_set_channel(uint8_t channel);

/**
 * @brief Set PAN ID
 * 
 * @param pan_id PAN identifier
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_set_pan_id(uint16_t pan_id);

/**
 * @brief Set short address
 * 
 * @param short_addr 16-bit short address
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_set_short_addr(uint16_t short_addr);

/**
 * @brief Set extended address
 * 
 * @param ext_addr 64-bit extended address
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_set_ext_addr(const ieee802154_ext_addr_t *ext_addr);

/**
 * @brief Set promiscuous mode
 * 
 * @param enable true to enable, false to disable
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_set_promiscuous(bool enable);

/**
 * @brief Enable/disable auto ACK
 * 
 * @param enable true to enable, false to disable
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_set_auto_ack(bool enable);

/**
 * @brief Set frame pending for next ACK
 * 
 * Used for indirect transmission (coordinator has data pending for device)
 * 
 * @param addr Destination address
 * @param addr_mode Address mode (short or extended)
 * @param pending true if frame is pending, false otherwise
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_set_frame_pending(const uint8_t *addr, 
                                                      ieee802154_addr_mode_t addr_mode,
                                                      bool pending);

/**
 * @brief Check if frame pending bit should be set for address
 * 
 * @param addr Destination address
 * @param addr_mode Address mode
 * @return true if frame is pending for this address
 */
bool ieee802154_hal_get_frame_pending(const uint8_t *addr, ieee802154_addr_mode_t addr_mode);

/**
 * @brief Configure CSL (Coordinated Sampled Listening)
 * 
 * @param period CSL period in 10-symbol units (0 = disabled)
 * @param phase CSL phase offset
 * @param accuracy CSL timing accuracy
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_set_csl_config(uint16_t period, 
                                                   uint16_t phase,
                                                   uint8_t accuracy);

/**
 * @brief Enable/disable Enhanced ACK
 * 
 * @param enable true to enable Enhanced ACK (IEEE 802.15.4-2015)
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_set_enhanced_ack(bool enable);

/**
 * @brief Set Enhanced ACK payload
 * 
 * Configure Information Elements (IE) to be included in Enhanced ACK
 * 
 * @param ie_data IE data buffer
 * @param ie_len IE data length
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_set_ack_ie_data(const uint8_t *ie_data, uint8_t ie_len);

/**
 * @brief Get current timestamp
 * 
 * @return Timestamp in microseconds
 */
uint64_t ieee802154_hal_get_timestamp(void);

/**
 * @brief Current time on the same MAC clock at 1 us resolution
 *
 * ieee802154_hal_get_timestamp() counts whole 16 us symbols; this adds the
 * symbol fraction. Same epoch, so either works as a transmit_delay() base
 * and against RX timestamps.
 *
 * @return Timestamp in microseconds
 */
uint64_t ieee802154_hal_get_time_us(void);

/**
 * @brief Get current symbol counter
 * 
 * Symbol period = 16 µs for 2.4 GHz O-QPSK
 * 
 * @return Symbol counter value
 */
uint32_t ieee802154_hal_get_symbol_counter(void);

/**
 * @brief Transmit frame at specific time
 * 
 * Hardware-accelerated timed transmission for CSL and Thread timing
 * 
 * @param frame Frame to transmit
 * @param target_symbol_count Target symbol counter value for TX start
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_transmit_at(const ieee802154_frame_t *frame,
                                               uint32_t target_symbol_count);

/**
 * @brief Transmit frame with delay
 * 
 * Alternative API using base time and delay
 * 
 * @param frame Frame to transmit
 * @param base_time_us Base time in microseconds (0 = current time)
 * @param delay_us Delay from base time in microseconds
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_transmit_delay(const ieee802154_frame_t *frame,
                                                  uint64_t base_time_us,
                                                  uint32_t delay_us);

/**
 * @brief Transmit at a time fixed before the frame is finalized, CSMA-CA in software
 *
 * For frames whose content depends on the exact send time (a CSL receiver's
 * CSL IE). Per attempt: random backoff, pick the send time, prepare() writes and
 * secures the frame for its SFD time (HAL clock, us), CCA, timed trigger.
 * Completion arrives through the normal TX events.
 *
 * @return IEEE802154_OK, IEEE802154_ERROR_CHANNEL_BUSY after max_csma_backoffs + 1
 *         busy attempts, or IEEE802154_ERROR_SECURITY when prepare() failed.
 */
ieee802154_error_t ieee802154_hal_transmit_exact(ieee802154_frame_t *frame,
                                                 int (*prepare)(uint8_t *psdu, uint8_t len, uint64_t sfd_us, void *ctx),
                                                 void *ctx);

/**
 * @brief Check if timed TX is in progress
 * 
 * @return true if timed TX is pending
 */
bool ieee802154_hal_is_timed_tx_pending(void);

/**
 * @brief Cancel pending timed TX
 * 
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_cancel_timed_tx(void);

/**
 * @brief Get current HAL state
 * 
 * @return Current state
 */
ieee802154_state_t ieee802154_hal_get_state(void);

/**
 * @brief Register IRQ handler
 * 
 * Called by HAL to notify driver of events
 * 
 * @param event Event type
 * @param frame Frame data (for RX events)
 */
void ieee802154_hal_irq_handler(ieee802154_event_type_t event, ieee802154_frame_t *frame);

/**
 * @brief AES encryption (if hardware supported)
 * 
 * @param key Encryption key (16 bytes)
 * @param nonce Nonce (13 bytes)
 * @param input Input data
 * @param input_len Input length
 * @param output Output buffer
 * @param mic MIC buffer (optional)
 * @param mic_len MIC length
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_aes_encrypt(const uint8_t *key,
                                                const uint8_t *nonce,
                                                const uint8_t *input,
                                                uint16_t input_len,
                                                uint8_t *output,
                                                uint8_t *mic,
                                                uint8_t mic_len);

/**
 * @brief AES decryption (if hardware supported)
 * 
 * @param key Decryption key (16 bytes)
 * @param nonce Nonce (13 bytes)
 * @param input Input data
 * @param input_len Input length
 * @param output Output buffer
 * @param mic MIC to verify
 * @param mic_len MIC length
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_hal_aes_decrypt(const uint8_t *key,
                                                const uint8_t *nonce,
                                                const uint8_t *input,
                                                uint16_t input_len,
                                                uint8_t *output,
                                                const uint8_t *mic,
                                                uint8_t mic_len);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __IEEE802154_HAL_H__ */

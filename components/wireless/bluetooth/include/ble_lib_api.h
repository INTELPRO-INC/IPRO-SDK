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

#ifndef BLE_LIB_API_H_
#define BLE_LIB_API_H_

#include <stdbool.h>
#include <stdint.h>

// Forward declaration
struct rwip_eif_api;

/**
 * @brief Initialize BLE controller
 * @param task_priority FreeRTOS task priority for BLE controller task
 * @note Application must call ble_controller_set_eif_api() before calling this function
 */
void ble_controller_init(uint8_t task_priority);

/**
 * @brief Set external interface API for BLE controller
 * @param p_api Pointer to the EIF API structure (UART, SPI, etc.)
 * @note Must be called before ble_controller_init()
 */
void ble_controller_set_eif_api(const struct rwip_eif_api* p_api);

/**
 * @brief Reinitialize BLE hardware after PDS wakeup.
 * @note BLE IP registers are lost during PDS31. This re-enables clock,
 *       does IP core soft reset, reprograms registers, and reinits RF.
 *       Software state (kernel, heaps, tasks) is preserved.
 */
void ble_controller_reinit_hw(void);

/**
 * @brief Restore BLE hardware after PDS wakeup (save/restore path).
 * @note Restores saved registers + RF. Preserves scheduler state (advertising, etc.).
 *       Requires ble_controller_sleep_store() before PDS entry.
 *       Follow with ble_controller_sleep_wakeup() for time compensation.
 */
void ble_controller_restore_hw(void);

/**
 * @brief Get BLE loop iteration count (for debug)
 */
uint32_t ble_controller_get_loop_count(void);

int bt_mfg_cli_register(void);

/**
 * @brief Get current prevent_sleep flags (for debug)
 * @return Bitmask of active prevent_sleep flags
 */
uint32_t ble_controller_get_prevent_sleep(void);

/**
 * @brief Read the BLE base time counter, in half-slots.
 *
 * Crystal-derived, so it is the right reference against which to measure the
 * 32 kHz sleep clock that PDS uses to report slept time.
 *
 * @param[out] hus  half-microseconds within the current half-slot, or NULL
 * @return base time in half-slots
 */
uint32_t ble_controller_get_clock_hs(uint16_t *hus);

/**
 * @brief Override the declared sleep-clock drift, in ppm.
 *
 * Widens or narrows every connection's receive window proportionally, since
 * rx_win ~ 2 * time_since_sync * (this + peer_sca).
 *
 * Do not exceed 500. That is BLE_MAX_DRIFT_SLEEP and the default, and it is
 * also the worst case the spec's sleep-clock-accuracy enumeration can even
 * express — there is no defined value above it. Past that the window stops
 * being a harmless margin and starts changing behaviour:
 *
 *   - beyond EM_BLE_RXWINSZ_MASK/2 (16383 us) the hardware switches to
 *     wide-open mode and counts the window in whole slots (lld_con.c:2042)
 *   - once the half-window reaches half the connection interval the event is
 *     not scheduled at all (lld_con.c:1204)
 *
 * And the window grows with every missed event, so an inflated value is
 * self-amplifying: more receiver-on time invites the correlator to lock on
 * noise, which spends the event, which widens the window again. Tried at
 * 5000 ppm to buy margin for PDS timing error; it bought a runaway instead.
 */
void ble_controller_set_sleep_drift_ppm(uint16_t ppm);

/**
 * @brief Connection events lost to a false sync on noise.
 *
 * The correlator locked but no valid packet followed, so the event was spent
 * demodulating garbage. Climbs in step with widening receive windows.
 */
uint32_t ble_controller_get_falsesync(void);

/**
 * @brief Times the next anchor was already in the past when rescheduling.
 *
 * lld_con_sched() requires the anchor it is about to schedule to still be ahead
 * of the clock. Enabling controller autosleep breaks that by a near-constant
 * offset; a constant offset points at the timebase restoration rather than at
 * drift, so the inputs that produce the target are exported alongside the
 * count. target should equal sync_hs plus a whole number of intervals — if it
 * does and sync_hs is stale, the deep-sleep clock correction moved SLOTCLK
 * without shifting the stored sync reference.
 *
 * @param[out] clock     the clock reading that was too late, half-slots, or NULL
 * @param[out] target    the anchor being scheduled, half-slots, or NULL
 * @param[out] sync_hs   last_sync_ts.hs at that moment, or NULL
 * @param[out] interval  connection interval in half-slots, or NULL
 * @return number of occurrences (values above are the most recent one)
 */
uint32_t ble_controller_get_late_anchor(uint32_t *clock, uint32_t *target,
                                        uint32_t *sync_hs, uint32_t *interval);

/**
 * @brief How much longer the IP actually slept than it was told to.
 *
 * The sleep duration handed to the hardware is the whole remaining time to the
 * next target, less three small software margins; nothing subtracts the
 * oscillator start-up. If the hardware counts the programmed cycles and only
 * then begins waking, every wake lands late by that start-up and every anchor
 * is already behind the clock when the arbiter is reprogrammed.
 *
 * The two endpoint pairs tell the two candidates apart: a constant overshoot
 * across very different sleep lengths is the wake-up sequence; one that scales
 * with the sleep is a low-power clock mismatch between the conversion used to
 * program the sleep and the one used to correct the clock afterwards.
 *
 * @param[out] last,min,max  overshoot in low-power cycles, or NULL
 * @param[out] short_lpc,short_ovr  shortest sleep programmed and its overshoot
 * @param[out] long_lpc,long_ovr    longest sleep programmed and its overshoot
 * @return number of wake-ups measured
 */
uint32_t ble_controller_get_sleep_overshoot(int32_t *last, int32_t *min, int32_t *max,
                                            uint32_t *short_lpc, int32_t *short_ovr,
                                            uint32_t *long_lpc, int32_t *long_ovr);

/**
 * @brief How far the arbiter deadline had slipped into the past at wake-up end.
 *
 * Positive means the wake-up completed after the event it woke for was due.
 * This is the quantity that actually breaks a connection: a missed anchor
 * cannot be recovered once it is behind the clock.
 *
 * @param[out] last,min,max  lateness in half-slots, or NULL
 * @return number of wake-ups that had an arbiter target pending
 */
uint32_t ble_controller_get_wake_lateness(int32_t *last, int32_t *min, int32_t *max);

/**
 * @brief Low-power cycles currently held back from each programmed sleep.
 *
 * Seeded from the declared oscillator start-up and grown to the largest
 * overshoot the hardware has actually shown. A sleep whose window cannot
 * absorb it is refused rather than shortened, so on short connection
 * intervals the controller may correctly decline to sleep at all.
 */
uint32_t ble_controller_get_wakeup_reserve(void);

/**
 * @brief Tell the controller how far the low-power oscillator really is from
 *        32.768 kHz, in ppm, positive when it runs fast.
 *
 * The clock correction applied after every deep sleep converts the hardware's
 * slept-cycle count using the nominal rate. On this platform the source is an
 * RC oscillator with a measured residual of 45 to 1190 ppm, and at a one
 * second connection interval that becomes up to 1.2 ms of misplacement per
 * sleep — against a receive window of +-554 us. The application already
 * measures the residual for its own sleep accounting; this hands the same
 * figure to the controller so the correction is right too.
 *
 * Separate from ble_controller_set_sleep_drift_ppm(), which only widens the
 * window to tolerate error. This removes the error.
 */
void ble_controller_set_lp_clock_ppm(int32_t ppm);
int32_t ble_controller_get_lp_clock_ppm(void);

/**
 * @brief Which BLE core registers the IP's deep sleep changed.
 *
 * The whole register block is snapshotted going into the first sleep after a
 * reset and diffed coming out. Counters and interrupt status differ
 * legitimately; anything else names the receive state that a sleep destroys,
 * which no amount of reasoning about timing, radio power or clock correction
 * has been able to identify.
 *
 * @param[in]  n       entry index, below ble_controller_get_reg_diff_count()
 * @param[out] addr    absolute register address, or NULL
 * @param[out] before  value before the sleep, or NULL
 * @param[out] after   value after the wake-up, or NULL
 * @return total number of registers that differed (may exceed the entries kept)
 */
uint32_t ble_controller_get_reg_diff(uint32_t n, uint32_t *addr, uint32_t *before,
                                     uint32_t *after);
uint32_t ble_controller_get_reg_diff_count(void);

/**
 * @brief Why receive descriptor checks are failing.
 *
 * An event that reports no sync looks identical whether the hardware received
 * nothing or wrote the packet into a descriptor the software has stopped
 * tracking. notdone is the former, badlabel the latter; sw_ptr and hw_ptr are
 * the two ring positions that should agree.
 *
 * @param[out] notdone   checks where the descriptor was never filled
 * @param[out] badlabel  checks where it was filled for another activity
 * @param[out] sw_ptr    software index in the high half, its EM address in the low
 * @param[out] hw_ptr    the hardware CURRENTRXDESCPTR
 * @param[out] want_got  last mismatch: wanted label in bits 15:8, found in 7:0
 * @return checks that succeeded
 */
uint32_t ble_controller_get_rxdesc_diag(uint32_t *notdone, uint32_t *badlabel,
                                        uint32_t *sw_ptr, uint32_t *hw_ptr,
                                        uint32_t *want_got);

/**
 * @brief Whether the connection event counter still matches elapsed time.
 *
 * Taken between the last two CRC-OK receptions. The peer advances the counter
 * once per interval by definition, so d_hs divided by the interval must equal
 * d_ec. Under channel selection algorithm 2 the radio frequency comes from
 * that counter, so any disagreement puts the receiver on a pseudorandom
 * channel while timing, descriptors and registers all stay clean.
 *
 * @param[out] d_ec  event counters between the two receptions
 * @param[out] d_hs  half-slots between them
 * @param[out] ch    channel index the hardware used for the later one
 * @param[out] interval  the interval both samples were taken at, half-slots
 * @return false if the two receptions are not comparable: no first sample yet,
 *         or a parameter update changed the interval between them, in which
 *         case the counts refer to intervals of different lengths and the
 *         comparison is meaningless.
 */
bool ble_controller_get_evtcnt_check(uint32_t *d_ec, uint32_t *d_hs, uint32_t *ch,
                                     uint32_t *interval);

/**
 * @brief Where the last connection event's receive window was placed.
 *
 * Sampled each time a peripheral connection event is armed. sync_hus is the
 * sub-half-slot phase of the last successful reception, hus is where the
 * window was actually put, and size is its width in half-microseconds.
 *
 * These are the only quantities in the receive path that carry sub-half-slot
 * phase, and phase is the one thing a deep sleep redefines: the clock
 * correction reloads the fine counter, so a phase recorded before the sleep
 * refers to a different origin afterwards. Everything coarser — the anchor
 * half-slot, the event counter, the descriptor ring, the register block — has
 * already been measured intact across a sleep.
 */
void ble_controller_get_rx_window(int32_t *hus, int32_t *sync_hus, uint32_t *size,
                                  uint32_t *size_max);

/**
 * @brief The same window, but for the first event armed after a wake-up.
 *
 * The running sample above is read seconds later, once dozens of events have
 * already missed and widened the window themselves — it describes the
 * aftermath. Only this one is upstream of the failure.
 *
 * @param[out] clkdiff  half-slots from the last successful sync to the target,
 *                      which is what sets the width
 * @return number of wake-ups captured
 */
uint32_t ble_controller_get_postwake_window(uint32_t *size, int32_t *hus,
                                            int32_t *sync_hus, uint32_t *clkdiff);

/**
 * @brief Discard any captured post-wake window so the next wake is the one kept.
 *
 * Without this the advertising sleep, which happens long before a connection
 * exists, consumes the capture and the connection's own sleep is never seen.
 */
void ble_controller_reset_postwake_window(void);

/**
 * @brief Signal strength of the packets that were received, in dBm.
 *
 * The last domain not yet measured across a sleep. Everything that could put
 * the receiver in the wrong moment or on the wrong frequency has been shown
 * intact, and a window three half-slots wide still hears nothing five times in
 * six — so if what does arrive comes in markedly weaker after a sleep, the
 * loss is receiver gain rather than anything to do with scheduling.
 *
 * Reset together with the connection diagnostics, so the probe's baseline and
 * post-sleep windows are directly comparable.
 */
uint32_t ble_controller_get_rssi_diag(int8_t *last, int8_t *min, int8_t *max,
                                      int8_t *avg);

/**
 * @brief What the radio was tuned to on the events that heard nothing.
 *
 * The channel has only ever been sampled on successful receptions, which is
 * exactly the sample that cannot reveal a tuning fault. The event counter is
 * known to track elapsed time exactly, but that only settles which channel the
 * hardware was asked for.
 *
 * @param[out] nb_channels  how many distinct channels the misses used; a
 *                          healthy hop sequence spreads over the whole map,
 *                          while a stuck radio collapses to one or two
 */
uint32_t ble_controller_get_fail_diag(uint32_t *ch, uint32_t *evt_cnt,
                                      uint32_t *rxstat, uint32_t *nb_channels);

/**
 * @brief Which RF/PHY registers the sleep changed.
 *
 * The only block never examined across a sleep. Every software-visible
 * quantity is now measured intact — arrival 3 us from target at -51 dBm,
 * misses spread over 24 of 37 channels, event counter exact, descriptor ring
 * in step, all 133 BLE core registers unchanged, rxstat 0x0001 sync error
 * alone. Right moment, right frequency, full signal, window three half-slots
 * wide, and still nothing heard.
 *
 * @param[out] kept  how many entries are retrievable
 * @return total number of PHY registers that differed
 */
uint32_t ble_controller_get_phy_diff(uint32_t n, uint32_t *addr, uint32_t *before,
                                     uint32_t *after, uint32_t *kept);

/**
 * @brief Which Exchange Memory control structure fields the sleep changed.
 *
 * The register diffs cleared the BLE core block and the PHY block, but neither
 * covers the control structures: access address, CRC init, channel map,
 * whitening seed, hop control and the event counter that selects the channel
 * all live in Exchange Memory. No radio event can run between the snapshot and
 * the wake-up, so every difference reported here is the work of the sleep.
 *
 * @param[out] cs      which control structure, by activity index
 * @param[out] offset  byte offset of the 16-bit field within it
 * @param[out] kept    how many entries are retrievable
 * @return total number of fields that differed
 */
uint32_t ble_controller_get_emcs_diff(uint32_t n, uint32_t *cs, uint32_t *offset,
                                      uint32_t *before, uint32_t *after,
                                      uint32_t *kept);

/**
 * @brief How far the post-sleep clock correction lands from real time.
 *
 * Everything else that says the timebase is right after a sleep is measured
 * with that same timebase — the arbiter's lateness, the event counter step,
 * the anchor error — so a correction that is wrong by a whole number of
 * half-slots is consistent with all of them and still misses every anchor.
 * This compares the corrected BT clock against the free-running machine timer,
 * which the correction does not touch.
 *
 * Report bt_hus and ref_hus alongside the error: if the reference delta is
 * zero the machine timer stopped too and the error means nothing.
 *
 * @param[out] last     error of the most recent wake-up, half-microseconds,
 *                      positive when the BT clock advanced more than real time
 * @param[out] min      smallest error seen
 * @param[out] max      largest error seen
 * @param[out] bt_hus   raw BT clock delta of the last wake-up
 * @param[out] ref_hus  raw machine timer delta of the last wake-up
 * @return number of wake-ups measured
 */
uint32_t ble_controller_get_clock_xcheck(int32_t *last, int32_t *min, int32_t *max,
                                         int32_t *bt_hus, int32_t *ref_hus);

/**
 * @brief Snapshot the BLE core and PHY register blocks now, and diff them now.
 *
 * The two register diffs only ever ran across a sleep, because a sleep was the
 * suspected fault. It is not: the link freezes at 48.75 ms too, an interval
 * shorter than the wake-up reserve where the controller never sleeps once. The
 * fault to diff across is receiving normally for hundreds of events and then
 * never again, and only the application can time that, because only it can see
 * crcok stop advancing.
 *
 * Capture while the link is healthy, diff once it has frozen; the result comes
 * back through ble_controller_get_reg_diff() and ble_controller_get_phy_diff()
 * as before. Capturing also stops the sleep path from touching the buffers, so
 * the advertising sleeps that follow a disconnect cannot overwrite the
 * evidence.
 */
void ble_controller_capture_regs(void);
void ble_controller_diff_regs(void);

/**
 * @brief Receptions and missed events since boot, never zeroed.
 *
 * The probe restarts the connection diagnostics every couple of seconds while
 * it waits for a healthy link, so those counters cannot answer "is this link
 * receiving right now" — and getting that wrong is how a snapshot ends up
 * taken from an already-broken link and diffed after the disconnect.
 */
void ble_controller_get_link_totals(uint32_t *crcok, uint32_t *nosync);

/**
 * @brief Read one word of the BLE core (block 0) or PHY (block 1) block.
 *
 * Roughly half of cold boots come up with the receiver 15 to 20 dB worse and
 * the link broken from its very first connection event; the other half run for
 * a minute at 1010 receptions out of 1014. Nothing the connection does can
 * account for that — it is decided before the first packet, so the state to
 * compare is the state at boot, between a good boot and a bad one.
 *
 * @param[out] count  words in the block, valid even for an out-of-range idx
 */
uint32_t ble_controller_reg_block(uint32_t block, uint32_t idx, uint32_t *count);

/** @brief Rearm the register diff so the next sleep is the one captured. */
void ble_controller_reset_reg_diff(void);

/**
 * @brief Limit how many more deep sleeps the controller may take.
 *
 * Pass 0xFFFFFFFF for unlimited (the default), 0 to hold it awake, or a small
 * count to allow exactly that many.
 *
 * Intended for isolating what a single sleep costs. With the controller
 * sleeping continuously the link is already broken by the time anything can be
 * measured, so every anchor sample comes from a dead connection. Letting a
 * connection settle at budget 0, resetting the anchor statistics, then
 * granting one sleep makes the before-and-after difference attributable to
 * that one clock correction.
 */
void ble_controller_set_sleep_budget(uint32_t sleeps);
uint32_t ble_controller_get_sleep_budget(void);
uint32_t ble_controller_get_sleep_budget_used(void);

/**
 * @brief Count of lld_con.c timing assertions.
 *
 * These are treated as warnings and no longer printed — three lines of UART
 * from the frame ISR cost more than a quarter of a short connection interval,
 * so printing them ends the link faster than the fault being reported.
 */
uint32_t ble_controller_get_lld_con_asserts(void);

/**
 * @brief Sleeps taken between consecutive connection events, split by outcome.
 *
 * With a clean awake baseline at 415 ms — 12 of 12 CRC-OK — allowing the
 * controller to sleep loses roughly two events in three, while the arbiter
 * deadline is still met every time and the packets that do arrive land 30 us
 * from target at full strength. A partial failure at that ratio is not a
 * corrupted state, which would be all or nothing; it is probabilistic.
 *
 * The interval is being chopped into five or six sleeps of two or three
 * granules rather than one, so the obvious variable is how many times the IP
 * stopped and restarted before each event. If the successful events are the
 * ones preceded by fewer sleeps, that is both the mechanism and the fix.
 *
 * Averages are scaled by ten to keep integer precision.
 */
/**
 * @brief Event counter advance against anchor advance, at every event.
 *
 * Recorded at event start, so unlike every earlier version of this check it
 * does not need a reception to anchor it. That distinction is the whole point:
 * no event that follows a sleep ever succeeds, so any comparison keyed on
 * CRC-OK can only ever span two awake events and is structurally blind to
 * exactly the transition under investigation.
 *
 * Between consecutive events the counter must advance by the same number of
 * intervals the anchor did. Under channel selection algorithm 2 the radio
 * frequency is derived from that counter, so a step that disagrees is an event
 * listening on the wrong channel — which matches the observed failure exactly:
 * deterministic, timing-independent, full signal strength, misses spread over
 * the whole hop map.
 *
 * @param[out] bad  steps where the two disagreed
 * @return steps measured
 */
uint32_t ble_controller_get_evtcnt_step(uint32_t *d_ec, uint32_t *d_hs,
                                        uint32_t *interval, uint32_t *sleeps,
                                        uint32_t *bad);

/**
 * @brief What happened at the connection parameter update instant.
 *
 * The short interval receives 92 packets out of 97 and the long one receives
 * none, with no sleep taken on either side, so the transition is the only
 * event between working and dead. An instant applied one event early or late,
 * or a dropped transmit window offset, displaces the anchor permanently while
 * leaving the event counter, the channel sequence, the descriptor ring and the
 * arbiter all self-consistent — which is why nothing else has caught it.
 *
 * step is the half-slots from the previous anchor to the first anchor of the
 * new interval. It must equal itv_old + win_off. Equal to itv_new instead
 * means the new interval was applied one event too early.
 *
 * @param[out] instant   event counter the update was scheduled for
 * @param[out] evtcnt    event counter it was actually applied at
 * @param[out] itv_old   interval before, half-slots
 * @param[out] itv_new   interval after, half-slots
 * @param[out] win_off   transmit window offset, half-slots
 * @param[out] win_size  transmit window size, half-slots
 * @param[out] step      half-slots from the previous anchor
 * @return updates applied on this link
 */
uint32_t ble_controller_get_param_update(uint32_t *instant, uint32_t *evtcnt,
                                         uint32_t *itv_old, uint32_t *itv_new,
                                         uint32_t *win_off, uint32_t *win_size,
                                         uint32_t *step);

void ble_controller_get_sleep_vs_outcome(uint32_t *ok_avg_x10, uint32_t *ok_max,
                                         uint32_t *bad_avg_x10, uint32_t *bad_max,
                                         uint32_t *ok_n, uint32_t *bad_n);

/**
 * @brief Diagnostics for the IP's own hardware sleep path.
 *
 * The return value is the thing to look at first: rwip_wakeup() is reachable
 * only from the IP's sleep interrupt, which only fires if the IP actually
 * entered its own deep sleep. Zero means the hardware never corrected the
 * timebase, no matter what the software sleep bookkeeping reports.
 *
 * @param[out] last_slp    most recent DEEPSLSTAT reading, lp cycles, or NULL
 * @param[out] min_slp     smallest seen, or NULL
 * @param[out] max_slp     largest seen, or NULL
 * @param[out] programmed  most recent value handed to the wake counter
 * @return number of hardware wake-ups handled
 */
uint32_t ble_controller_get_sleep_hw_diag(uint32_t *last_slp, uint32_t *min_slp,
                                          uint32_t *max_slp, uint32_t *programmed);

/**
 * @brief Release the deep-sleep flag when the IP's own wake-up cannot arrive.
 *
 * Call once after every PDS wake. Does nothing when the IP is still sleeping
 * normally; only acts when its deep-sleep state was destroyed rather than
 * completed, which would otherwise wedge the controller's sleep bookkeeping.
 */
void ble_controller_sleep_reconcile(void);

/** @brief How many times the above had to act. */
uint32_t ble_controller_get_reconcile_cnt(void);

/** @brief DEEPSLCNTL as observed by the last reconcile. */
uint32_t ble_controller_get_deepslcntl(void);

/**
 * @brief The deadline comparisons the wake-up completion is making.
 *
 * A correct timebase and a delivered slot interrupt still leave the scheduler
 * dead if every timer target is judged "not yet due"; these are the operands.
 * RWIP_INVALID_TARGET_TIME in a target means nothing is armed on it.
 */
void ble_controller_get_timer_diag(uint32_t *now_hs, uint32_t *arb_hs, uint32_t *alarm_hs,
                                   uint32_t *us_hs, uint32_t *fired);

/**
 * @brief Every place RW_DEEP_SLEEP is taken or released.
 *
 * It is a counter rather than a flag, so an unmatched set anywhere blocks deep
 * sleep permanently with nothing to show for it. Sets minus clears is the
 * whole diagnosis.
 *
 * @return the controller's own software deep-sleep flag, which should track
 *         the counter exactly — a disagreement is itself the fault
 */
uint32_t ble_controller_get_ds_diag(uint32_t *set_sleep, uint32_t *clr_wakeup,
                                    uint32_t *clr_reconcile, uint32_t *legacy);

/**
 * @brief Both representations of "a wake-up is in progress".
 *
 * The completion path is gated on the bool; the sleep decision reads the
 * prevent-counter bit. One site clears the bool alone by design, so they can
 * disagree — and when they do, slot interrupts arrive and are discarded while
 * the bit blocks sleep forever.
 *
 * @param[out] isr_skipped  slot interrupts the bool turned away
 * @return the bool
 */
uint32_t ble_controller_get_wo_diag(uint32_t *set_wakeup, uint32_t *clr_end,
                                    uint32_t *clr_init, uint32_t *legacy,
                                    uint32_t *isr_skipped);

/**
 * @brief What the last clock correction was built from.
 *
 * @param[out] base        saved pre-sleep SLOTCLK, or NULL
 * @param[out] dur         half-slots derived from the hardware sleep counter
 * @param[out] target      value handed to CLKNCNTCORR
 * @param[out] clk_before  SLOTCLK as found on entry to the wake handler
 */
void ble_controller_get_corr_diag(uint32_t *base, uint32_t *dur, uint32_t *target,
                                  uint32_t *clk_before);

/**
 * @brief How often the wake-up actually completed.
 *
 * The clock correction landing is not the end of a wake: a slot interrupt has
 * to follow and re-check the timer targets, or the scheduler never re-arms
 * even with a perfectly corrected timebase.
 *
 * @param[out] clknint_cnt  slot interrupts seen, or NULL — a count here with
 *                          a zero return means they arrive but are not routed
 * @return completions handled
 */
uint32_t ble_controller_get_wakeup_end_cnt(uint32_t *clknint_cnt);

/**
 * @brief Zero the connection diagnostics.
 *
 * Call on every parameter change so each interval phase is measured on its
 * own; cumulative counts across phases hide short-lived failures — a phase
 * that misses everything for six seconds disappears inside the hundred
 * healthy events that preceded it.
 */
void ble_controller_reset_conn_diag(void);

/**
 * @brief Most recent connection anchor error, for closed-loop trimming.
 *
 * @param[out] err_hus  latest error in half-us, positive = arrived late
 * @return running sample count — use it as a sequence number to tell a fresh
 *         sample from a repeat of the last one
 */
uint32_t ble_controller_get_anchor_last(int32_t *err_hus);

/**
 * @brief Connection-event diagnostics.
 *
 * The anchor error is measured against the PEER's clock (expected vs actual
 * packet arrival), so unlike every other timing figure available to a sleep
 * manager it cannot be fooled by a locally mis-restored timebase.
 *
 * Only samples backed by a CRC-OK are counted. A sync with no valid packet
 * behind it is a false detection on noise and its apparent timing error is
 * meaningless — a phase receiving nothing at all otherwise reports a handful
 * of wild "anchor errors" and reads as though it had timing data.
 *
 * @param[out] anchor_min_hus  smallest anchor error seen, half-us, or NULL
 * @param[out] anchor_max_hus  largest, or NULL
 * @param[out] anchor_avg_hus  mean, or NULL
 * @param[out] evt_run         connection events the scheduler started
 * @param[out] evt_nosync      of those, how many never heard the peer
 * @param[out] evt_crcok       of those, how many reloaded supervision
 * @param[out] sch_rejected    events the arbiter refused to place at all
 * @return number of anchor-error samples
 */
uint32_t ble_controller_get_conn_diag(int32_t *anchor_min_hus, int32_t *anchor_max_hus,
                                      int32_t *anchor_avg_hus, uint32_t *evt_run,
                                      uint32_t *evt_nosync, uint32_t *evt_crcok,
                                      uint32_t *sch_rejected);

void ble_controller_set_full_rf_restore(bool enable);

/**
 * @brief Snapshot of the controller's sleep bookkeeping (debug).
 *
 * prevent_sleep is a counter, so its value alone does not identify which id
 * leaked a set() without a clear(). Under RW_DEBUG the per-id tallies do.
 *
 * @param[out] wake_ongoing  wake_up_ongoing flag, or NULL
 * @param[out] deep_active   deep-sleep-active flag, or NULL
 * @param[out] per_id        per-id tallies, or NULL
 * @param[in]  per_id_max    capacity of per_id
 * @return the prevent_sleep counter
 */
uint32_t ble_controller_get_sleep_diag(uint8_t *wake_ongoing, uint8_t *deep_active,
                                       int8_t *per_id, uint32_t per_id_max);

/**
 * @brief Initialize BLE controller sleep parameters
 * @note Configures sleep timing but keeps sleep DISABLED.
 *       Call ble_controller_sleep_enable(true) to actually allow deep sleep.
 *       Must be called after ble_controller_init().
 */
void ble_controller_sleep_init(void);

/**
 * @brief Enable or disable BLE controller deep sleep
 * @param enable true to allow deep sleep, false to prevent it
 * @note Must be called after ble_controller_sleep_init()
 */
void ble_controller_sleep_enable(bool enable);

/**
 * @brief Restore BLE time base after PDS wakeup
 * @param actual_lpcycles Actual sleep duration in LP clock cycles (32.768kHz)
 */
void ble_controller_sleep_wakeup(uint32_t actual_lpcycles);

/**
 * @brief Get BLE programmed sleep duration
 * @return Sleep duration in microseconds, or 0 if not programmed
 */
uint32_t ble_controller_get_sleep_duration_us(void);

/**
 * @brief Save BLE core registers before PDS
 * @note Must be called before hal_lp_enter() when BLE domain may lose power
 */
void ble_controller_sleep_store(void);

/**
 * @brief Restore BLE core registers after PDS wakeup
 * @note Must be called after ble_clk_enable(1) and before ble_controller_sleep_wakeup()
 */
void ble_controller_sleep_restore(void);

/**
 * @brief Lightweight RF reinitialization after PDS wakeup
 * @note Resets RF PHY and restores TX power settings
 */
void ble_rf_reinit_lp(void);

/**
 * @brief Read a BLE core register (for debug/verification)
 * @param index Register index (0-based, each index = 4 bytes)
 * @return Register value
 */
uint32_t ble_controller_read_reg(uint32_t index);

/**
 * @brief Write a BLE core register (for debug/verification)
 * @param index Register index (0-based, each index = 4 bytes)
 * @param value Value to write
 */
void ble_controller_write_reg(uint32_t index, uint32_t value);

/**
 ****************************************************************************************
 * @brief Veto the BLE IP's own deep sleep from the platform.
 *
 * Weak, default returns true. Override in an application to keep the IP awake
 * while doing platform work in the window it would otherwise have slept
 * through. Called with interrupts masked, from the controller thread, after
 * the sleep window is known but before anything is armed -- returning false
 * costs nothing to unwind.
 *
 * @param window_us  length of the sleep the IP was about to take.
 * @return true to allow the deep sleep, false to stay awake (RWIP_IDLE).
 ****************************************************************************************
 */
bool ble_platform_presleep_hook(uint32_t window_us);

/**
 ****************************************************************************************
 * @brief Tell the controller the platform keeps the BLE core alive across its sleep.
 *
 * When set, the PDS wake path skips the SLOTCLK reload and the sub-half-slot
 * phase correction entirely: the core never stopped counting, so reloading it
 * can only make it wrong. Set this whenever PDS_CTL4.BzRst / .BzGateClk and
 * the BZ_BLE domain fields are all clear.
 *
 * @param kept true if the core keeps running through platform sleep.
 ****************************************************************************************
 */
void ble_controller_set_core_kept_alive(bool kept);

/**
 ****************************************************************************************
 * @brief Skip ble_controller_sleep_restore() entirely.
 *
 * Only valid when the core stayed powered and clocked through platform sleep:
 * nothing was lost, so replaying the pre-sleep register snapshot can only
 * rewind state that is already correct. Separate from
 * ble_controller_set_core_kept_alive() so the two can be bisected.
 *
 * @param skip true to make sleep_restore a no-op.
 ****************************************************************************************
 */
void ble_controller_set_skip_reg_restore(bool skip);

/**
 ****************************************************************************************
 * @brief Skip all time compensation in the PDS wake path.
 *
 * Valid only alongside ble_controller_set_core_kept_alive() and
 * ble_controller_set_skip_reg_restore(): the clock counted the sleep itself,
 * so no measured duration needs to be credited to anything. Only the wake
 * bookkeeping the CPU reset actually broke still runs.
 *
 * @param skip true to skip the compensation.
 ****************************************************************************************
 */
void ble_controller_set_skip_time_credit(bool skip);

/**
 ****************************************************************************************
 * @brief Stop arming the IP's hardware clock correction on wake.
 *
 * Valid only when the core keeps running through sleep: the clock counted the
 * sleep itself, so CLKNCNTCORR + DEEP_SLEEP_CORR_EN would credit it twice.
 * Affects every IP deep-sleep wake, not only PDS ones.
 *
 * @param skip true to leave the correction unarmed.
 ****************************************************************************************
 */
void ble_controller_set_skip_hw_corr(bool skip);

/**
 ****************************************************************************************
 * @brief Mark the next controller wake as one that carried a platform sleep.
 *
 * Call from the platform wake path. Consumed once. Only wakes so marked can
 * have the hardware clock correction skipped -- the IP's own deep sleep does
 * stop its clock and does need the correction, so the skip must not apply to
 * every wake. See ble_controller_set_skip_hw_corr().
 ****************************************************************************************
 */
void ble_controller_note_pds_wake(void);

/**
 * @brief Wake ble_loop from inactive state
 * @note Call after enabling PDS to prevent ble_loop from blocking
 *       indefinitely in rwip_process() (rwip_inactive=true).
 */
void ble_controller_wake(void);

/**
 * @brief Check if BLE HW is actively processing a frame.
 * @return true if any ET entry is READY/UNDER_PROCESS/STARTED.
 * @note Do NOT enter PDS when busy — aborting mid-event permanently
 *       loses advertising/connection scheduling.
 */
bool ble_controller_is_busy(void);

/**
 * @brief Raw Exchange Table snapshot, for diagnosing ble_controller_is_busy().
 *
 * @param[out] out  receives the raw ET halfword for each entry
 * @param[in]  n    number of entries to read (clamped to the table length)
 * @return bit31 set if the BLE kernel still has pending events
 */
uint32_t ble_controller_et_snapshot(uint16_t *out, uint32_t n);

/**
 * @brief Get time until next scheduled BLE event.
 * @return Microseconds until next BLE timer target, or large value if none.
 * @note Use before PDS entry to cap sleep duration so we wake before
 *       the next advertising/connection event.
 */
uint32_t ble_controller_get_next_event_us(void);

#endif

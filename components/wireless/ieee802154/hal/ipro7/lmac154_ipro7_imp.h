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

#ifndef __LMAC154_IPRO7_IMP_H__
#define __LMAC154_IPRO7_IMP_H__

#include "ipro7_rf_private.h"
#include "stdint.h"

// reset rx/tx/csma in lmac154_monitor, if (bl_timer_now - rxEndSynCnt) >= RX_FINISHED_CNT
#define RX_FINISHED_CNT (5 * 1000 * 1000)
static uint32_t rxEndSynCnt = 0;

static uint8_t lmac154_tx_delay_post_fill_mpdu_guide_time = LMAC154_PREAMBLE_LEN;
static uint8_t lmac154_rx_off_before_delay_tx = 1;
static uint8_t lmac154_ack_seq_num = 0;
static bool lmac154_rx_on_when_idle = true;
static uint8_t lmac154_enh_ack_triggered = 0;

static uint8_t lmac154_tx_delay_trigger_guide_time = (LMAC154_TX_START_OFFSET_VALUE / LMAC154_US_PER_SYMBOL + 1) << LMAC154_US_PER_SYMBOL_BITS;
static uint8_t lmac154_tx_power_value = 15;
extern void rf_init(uint32_t xtalfreq_hz);
#define MTIMER_TICKS_PER_US (1)
static inline uint64_t bl_timer_now()
{
    uint32_t tick_low, tick_high, tick_tmp;
    uint64_t tick64;

    do {
        tick_high = *(volatile uint32_t *)0xE000BFFC;
        tick_low = *(volatile uint32_t *)0xE000BFF8;

        tick_tmp = *(volatile uint32_t *)0xE000BFFC; //make sure no overflow
    } while (tick_high != tick_tmp);

    tick64 = (((uint64_t)tick_high) << 32) | tick_low;

    return tick64 / MTIMER_TICKS_PER_US;
}

static inline void LMAC154_SET_TX_POWER(uint8_t txPower)
{
    // wl_rf_set_154_tx_power(txPower);
    lmac154_tx_power_value = txPower;
}

static inline void LMAC154_RFPHY_INIT(void)
{
    extern void rf_set_channel(uint8_t bandwidth, uint16_t channel_freq);
    extern void phy_init_lmac154_test(void);

    // TODO add define
#ifdef FPGA
#ifdef RFDTB9361
    // set default to CH11: 2405
    spi_9361_set_ch(2405);
#else
    // rf initialization
    rf_init(384 * 100 * 1000);
    phy_init_lmac154_test();
    rf_set_channel(0, 2405);
#endif
#endif
#ifdef CONFIG_IEEE802154_LP_SKIP_RF_CAL_ON_WAKE
    /*
     * Skip full RF calibration on PDS warm wake.
     *
     * rf_init(0) consults `inited` (DTCM retention) — on cold boot it is
     * still 0 so a full cal runs; after that `inited=1` persists across
     * PDS31, so subsequent wakes fall through to rf_restore_calibration_value()
     * using the cached `rf_calibration_data` (.noinit_data retention).
     * Saves ~1-2 ms of wake latency and the associated RF cal current.
     */
    rf_init(0);
#else
    rf_init(1);
#endif
    LMAC154_SET_TX_POWER(15);
}

static inline void LMAC154_MODULE_CLOCK_INIT(void)
{
    uint32_t tmpVal;

    /*
     * Step 1: enable clock (the M154 peripheral needs a clock before
     * its SWRST register is honoured; REGS_CMD would clear the clock
     * enable bit so we skip it).
     */
    tmpVal = BL_RD_REG(MAC154_BASE, M154_GLB_CFG);
    tmpVal = BL_SET_REG_BIT(tmpVal, M154_CLK_EN);
    tmpVal = BL_CLR_REG_BIT(tmpVal, M154_BEACON_MODE_EN);
    BL_WR_REG(MAC154_BASE, M154_GLB_CFG, tmpVal);

    /*
     * Step 2: reset TX/RX/TCON FSMs. Needed for PDS31 warm-boot path —
     * after MISC domain power-cycle, the FSMs may land in a non-IDLE
     * state so LMAC154_TX_FORBIDDEN() loops forever. We skip
     * REGS_CMD so M154_CLK_EN is preserved.
     */
    BL_WR_REG(MAC154_BASE, M154_SWRST,
              M154_SOFT_RESET_TCON_CMD_MSK |
              M154_SOFT_RESET_RX_CMD_MSK |
              M154_SOFT_RESET_TX_CMD_MSK);
    for (volatile int i = 0; i < 32; i++) { }
    BL_WR_REG(MAC154_BASE, M154_SWRST, 0);
}

IEEE802154_HOT_SECTION
static void LMAC154_ALARM_TIMER_ON(uint8_t id, uint32_t sym_cnt)
{
    uint32_t tmpVal;

    if (id > LMAC154_ALARM_TIMER_ID_MAX) {
        return;
    }

    tmpVal = BL_RD_REG(MAC154_BASE, M154_SYM_CNT);
    tmpVal += sym_cnt;
    BL_WR_WORD(MAC154_BASE + M154_ALARM_00_TARGET_OFFSET + id * 4, tmpVal);

    tmpVal = BL_RD_REG(MAC154_BASE, M154_INT_EN);
    tmpVal |= (1U << (M154_ALARM_00_INT_EN_POS + id));
    BL_WR_REG(MAC154_BASE, M154_INT_EN, tmpVal);
}

static inline void LMAC154_SET_COEX(bool isEnable)
{
    uint32_t tmpVal;

    if (isEnable) {
        *(volatile uint32_t *)0x30001220 = (~((uint32_t)0x3 << 26)) & *(volatile uint32_t *)0x30001220;
        *((volatile uint32_t *)BL_PTA_CONFIG_ADDR) = 0x50000001;
        *((volatile uint32_t *)BL_PTA_CONFIG2_ADDR) = BL_PTA_CONFIG2_ADDR_BZ_PTI_MSK;
    } else {
        *(volatile uint32_t *)0x30001220 |= ((uint32_t)0x3 << 26);
        *((volatile uint32_t *)BL_PTA_CONFIG2_ADDR) = 0;
    }

    tmpVal = BL_RD_REG(MAC154_BASE, M154_INT_EN);
    tmpVal = BL_SET_REG_BIT(tmpVal, M154_RX_FILTER_FAIL_INT_EN);
    tmpVal = BL_SET_REG_BIT(tmpVal, M154_RX_END_ACK_OTHERS_INT_EN);
    BL_WR_REG(MAC154_BASE, M154_INT_EN, tmpVal);
}

static inline void LMAC154_MONITOR(void)
{
    static uint32_t loopTimes = 0;

    if (loopTimes++ > 1000) {
        loopTimes = 0;

        uint32_t lastRxFinished = bl_timer_now() - rxEndSynCnt;
        if (lastRxFinished >= RX_FINISHED_CNT) {
            extern void ieee802154_hal_note_mon_swrst(void);
            ieee802154_hal_note_mon_swrst();
            // reset rx, tx and csma
            BL_WR_REG(MAC154_BASE, M154_SWRST, M154_SOFT_RESET_RX_CMD_MSK | M154_SOFT_RESET_TX_CMD_MSK | M154_SOFT_RESET_CSMA_CA_CMD_MSK);
        }
    }
}

static inline uint32_t lmac154_isTimeGuarded(uint32_t guardCnt, uint32_t baseCnt, uint32_t delayCnt)
{
    if (0 == guardCnt) {
        return 1;
    }

    if (guardCnt >= delayCnt) {
        return 0;
    }

    guardCnt = delayCnt - guardCnt;
    uint32_t now = BL_RD_REG(MAC154_BASE, M154_SYM_CNT);
    if (now - baseCnt <= guardCnt) {
        return 1;
    }

    return 0;
}

IEEE802154_HOT_SECTION
static int LMAC154_TX_TRIGGER_DELAY(uint64_t baseUs, uint32_t delayUs)
{
    uint32_t tmpVal;
    uint32_t timeout = 500000 / 16;
    uint32_t intDelay = 320 / 16;
    uint32_t baseCnt = (baseUs + LMAC154_US_PER_SYMBOL_BITS - 1) >> LMAC154_US_PER_SYMBOL_BITS;
    uint32_t delayCnt = (delayUs + LMAC154_US_PER_SYMBOL_BITS - 1) >> LMAC154_US_PER_SYMBOL_BITS;

    tmpVal = BL_RD_REG(MAC154_BASE, M154_SYM_CNT);

    while (LMAC154_TX_FORBIDDEN()) {
        if (BL_RD_REG(MAC154_BASE, M154_SYM_CNT) - tmpVal > timeout) {
            // reset tx and csma
            BL_WR_REG(MAC154_BASE, M154_SWRST, M154_SOFT_RESET_TX_CMD_MSK | M154_SOFT_RESET_CSMA_CA_CMD_MSK);

            // set alarm time to raise event
            LMAC154_ALARM_TIMER_ON(LMAC154_ALARM_TIMER_ID_HW_ERR, intDelay);

            // for debug
            lmac154_tx_hw_error_cnt++;
            return -1;
        }
    }

    // enable tx abort interrupt if coex enabled (for each tx trigger)
    if (LMAC154_COEX_ENABLED()) {
        LMAC154_TX_ABORT_INT_ON();
    }

    if (baseCnt || delayCnt) {
        if (0 == baseCnt) {
            baseCnt = BL_RD_REG(MAC154_BASE, M154_SYM_CNT);
        }

        /* The MPDU copy and the TX_FORBIDDEN wait above run after the guide
         * check in lmac154_triggerTxDelay(), so check the same guide time
         * again and arm with interrupts off: a trigger time closer than the
         * TX start offset was armed anyway before, and the radio went silent
         * (bench 2026-09-26, 2M, slave answering at RX + 300 us). */
        uint32_t guardCnt = lmac154_tx_delay_trigger_guide_time >> LMAC154_US_PER_SYMBOL_BITS;
        rv_csr_t irq = __RV_CSR_READ_CLEAR(CSR_MSTATUS, MSTATUS_MIE);
        uint32_t delta = BL_RD_REG(MAC154_BASE, M154_SYM_CNT) - baseCnt;
        if (delta + guardCnt >= delayCnt) {
            __RV_CSR_SET(CSR_MSTATUS, irq & MSTATUS_MIE);
            return -2;
        }

        BL_WR_REG(MAC154_BASE, M154_TX_2, baseCnt + delayCnt);

        tmpVal = BL_RD_REG(MAC154_BASE, M154_TX_1);
        tmpVal = BL_SET_REG_BIT(tmpVal, M154_TX_TRIG_CMD);
        tmpVal = BL_SET_REG_BIT(tmpVal, M154_TX_TRIG_TIME_SEL);
        tmpVal = BL_CLR_REG_BIT(tmpVal, M154_TX_MPDU_SRC_SEL);

        BL_WR_REG(MAC154_BASE, M154_TX_1, tmpVal);
        __RV_CSR_SET(CSR_MSTATUS, irq & MSTATUS_MIE);
    } else {
        tmpVal = BL_RD_REG(MAC154_BASE, M154_TX_1);
        tmpVal = BL_SET_REG_BIT(tmpVal, M154_TX_TRIG_CMD);
        tmpVal = BL_CLR_REG_BIT(tmpVal, M154_TX_TRIG_TIME_SEL);
        tmpVal = BL_CLR_REG_BIT(tmpVal, M154_TX_MPDU_SRC_SEL);
        BL_WR_REG(MAC154_BASE, M154_TX_1, tmpVal);
    }

    return 0;
}

static inline uint32_t lmac154_isTimeGuided(uint32_t guideTime, uint64_t baseTimeUs, uint32_t delayUs)
{
    if (0 == guideTime) {
        return 1;
    }

    if (guideTime >= delayUs) {
        return 0;
    }

    guideTime = delayUs - guideTime;
    uint64_t now = zb_timer_get_current_time_us();
    if (now - baseTimeUs <= guideTime) {
        return 1;
    }

    return 0;
}

static inline void LMAC154_ISR_HIT(uint32_t intStatus)
{
    // record rx end time
    if (intStatus & 0x01FE0000) {
        rxEndSynCnt = bl_timer_now();
    }
}

static inline bool LMAC154_ISR_RX_END(uint32_t framectrl)
{
    if (LMAC154_RX_PROMISCUOUS()) {
        return false;
    }

    if (LMAC154_FRAME_IS_ENH_ACK_REQ(framectrl)) {
        lmac154_reqEnhAckEvent();
    }

    // return LMAC154_FRAME_IS_ACK_REQ(framectrl);
    return true;
}

static inline void LMAC154_ISR_TX_DONE(void)
{
    uint32_t tmpVal;

    tmpVal = BL_GET_REG_BITS_VAL(BL_RD_REG(MAC154_BASE, M154_RX_15), M154_RX_ACK_WAIT_TIME_US);
    tmpVal = (tmpVal + (1 << LMAC154_US_PER_SYMBOL_BITS) - 1) >> LMAC154_US_PER_SYMBOL_BITS;
    LMAC154_ALARM_TIMER_ON(LMAC154_ALARM_TIMER_ID_ACK, tmpVal);
}

void lmac154_setRxStateWhenIdle(bool isRxOnWhenIdle)
{
    /** bit1: whether to disable "rx off after rx interrupts triger" feature.
     *  bit0: whether to turn on Rx after ACK frame is received.
     *
     *  bit1 = 0, whatever bit0 is, rx off after rx done
     *  bit1 = 1, bit0 = 1, rx on after ack received, whatever ack_frame.pending_bit is;
     *  bit1 = 1, bit0 = 0, rx on after ack received if ack_frame.pending_bit is ture; otherwise, rx is off after ack received;
     *  */

    uint32_t rx17 = BL_RD_REG(MAC154_BASE, M154_RX_17);

    lmac154_rx_on_when_idle = isRxOnWhenIdle;

    if (lmac154_rx_on_when_idle) {
        /** set both of bit0 and bit1 to 1 */
        rx17 = rx17 | 0x3;
    } else {
        rx17 = BL_SET_REG_BITS_VAL(rx17, M154_RX_ON_AFTER_RX_ACK, isRxOnWhenIdle);
        rx17 |= 2;
    }

    BL_WR_REG(MAC154_BASE, M154_RX_17, rx17);
}

int lmac154_triggerTxDelay(uint8_t *DataPtr, uint32_t length, uint32_t csma,
                           uint64_t baseTimeUs, uint32_t delayUs)
{
    uint32_t tmpVal;
    int iret = 0;
    bool is_tx_abort_by_tx_ack = false;

    if (baseTimeUs || delayUs) {
        if (!lmac154_isTimeGuided(lmac154_tx_delay_trigger_guide_time, baseTimeUs, delayUs)) {
            return -1;
        }

        if (NULL == DataPtr && BL_GET_REG_BITS_VAL(BL_RD_REG(MAC154_BASE, M154_TX_3), M154_TX_FSM_CURR) != 1) {
            lmac154_resetTx();
            is_tx_abort_by_tx_ack = true;
        }
        if (BL_GET_REG_BITS_VAL(BL_RD_REG(MAC154_BASE, M154_TX_3), M154_TX_FSM_CURR) != 1) {
            return -2;
        }

        if (lmac154_rx_off_before_delay_tx) {
            /** two reasons to set RX off:
             * 1. rx => tx needs longer delay time than idle => tx. 12 vs 7.
             * 2. consider tx + delay as show tx path */

            BL_WR_REG(MAC154_BASE, M154_RX_09, BL_SET_REG_BIT(BL_RD_REG(MAC154_BASE, M154_RX_09), M154_RX_OFF_CMD));
        }

        csma = 0;
    }

    // configure CSMA-CA engine
    tmpVal = BL_RD_REG(MAC154_BASE, M154_CSMA_0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, M154_CSMA_CA_EN, csma);
    BL_WR_REG(MAC154_BASE, M154_CSMA_0, tmpVal);

    // copy mpdu
    LMAC154_SET_MPDU(DataPtr, length);

    // start tx
    if ((iret = LMAC154_TX_TRIGGER_DELAY(baseTimeUs, delayUs)) != 0) {
        return -4 * 10 + iret;
    }

    lmac154_enh_ack_triggered = 0;
    if (DataPtr) {
        lmac154_ack_seq_num = DataPtr[2];
        lmac154_ack_request = LMAC154_FRAME_IS_ACK_REQ(DataPtr[0]);
    }

    // clear retry count
    lmac154_tx_retry_cnt = 0;

    if (baseTimeUs || delayUs) {
        /* LMAC154_ALARM_TIMER_ON() adds the symbol counter itself: passing
         * SYM_CNT + ... here put the alarm one uptime too far away, so a
         * timed trigger that never went out (e.g. armed while RX was still
         * turning on) was never timed out and the TX stayed pending. */
        LMAC154_ALARM_TIMER_ON(LMAC154_ALARM_TIMER_ID_TX_TIMEOUT,
                               (delayUs >> LMAC154_US_PER_SYMBOL_BITS) +
                                   (length + 2 + 6) * 2 + (1000 >> LMAC154_US_PER_SYMBOL_BITS));
    }

    if (is_tx_abort_by_tx_ack) {
        /** there is a tx abort, let upper layer know this  */
        lmac154_ackFrameEvent(0, 0, 0);
    }

    return 0;
}

/****************************************************************************/ /**
 * @brief  File MPDU data after tx trigger requested when lmac154_triggerTx or 
 *         lmac154_triggerTxDelay without data specified.
 *
 * @param  DataPtr: pointer to data buffer
 * @param  length: data length in bytes
 *
 * @return 0 is Success
 *
*******************************************************************************/
IEEE802154_HOT_SECTION
uint32_t lmac154_postFillMPDU(uint8_t *DataPtr, uint32_t length)
{
    /** this function is dedicated to fill enh-ack packet to mpdu, and normally operate under interrupt context */

    uint32_t tmpVal = 0, now = 0;

    if (lmac154_isTriggerTimeSelected()) {
        if (lmac154_tx_delay_post_fill_mpdu_guide_time) {
            tmpVal = BL_RD_REG(MAC154_BASE, M154_TX_2) + lmac154_tx_delay_post_fill_mpdu_guide_time;

            now = BL_RD_REG(MAC154_BASE, M154_SYM_CNT);
            if (!(now <= tmpVal && tmpVal - now <= 1000000 / 16)) {
                /** m154 hardware fills PHY header to PHY at M154_TX_2 + M154_TX_START_OFFSET + Preambles + SFD, 
                 * so PHY packet must get ready before this time.
                 * ideally lmac154_tx_delay_post_fill_mpdu_guide_time = M154_TX_START_OFFSET + Preambles.*/
                return -1;
            }
        }

        LMAC154_SET_MPDU(DataPtr, length);

        if (LMAC154_FRAME_IS_ACK(DataPtr[0])) {
            lmac154_ack_request = 0;
            lmac154_enh_ack_triggered = 1;
        } else {
            // store ack_request flag
            lmac154_ack_request = (DataPtr[0] >> 5) & 0x01;
            lmac154_enh_ack_triggered = 0;
        }
        lmac154_ack_seq_num = DataPtr[2];

        return 0;
    }

    return -2;
}

/****************************************************************************/ /**
 * @brief  File MPDU data after tx trigger requested when lmac154_triggerTx or 
 *         lmac154_triggerTxDelay without data specified.
 *
 * @param  DataPtr: pointer to data buffer
 * @param  length: data length in bytes
 *
 * @return 1 is selected
 *
*******************************************************************************/
uint32_t lmac154_isTriggerTimeSelected(void)
{
    return BL_IS_REG_BIT_SET(BL_RD_REG(MAC154_BASE, M154_TX_1), M154_TX_TRIG_TIME_SEL);
}

/****************************************************************************/ /**
 * @brief  Get event timestamps
 *
 * @param  type: event timestamp type
 *
 * @return timestamp
 *
*******************************************************************************/
IEEE802154_HOT_SECTION
uint64_t lmac154_getEventTimeUs(lmac154_eventTimeType_t type)
{
    uint64_t eventtime = *((uint32_t *)(MAC154_BASE + M154_SYM_CNT_RX_START_OFFSET) + type);
    return eventtime << LMAC154_US_PER_SYMBOL_BITS;
}

#endif

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

/* FreeRTOS UART transport for the upstream OpenThread RadioSpinel driver. */
#ifndef IPRO_SPINEL_UART_HPP
#define IPRO_SPINEL_UART_HPP

#include "lib/hdlc/hdlc.hpp"
#include "lib/spinel/spinel_interface.hpp"
#ifdef CONFIG_THREAD_RCP_UART_FAULT_INJECTION
#include "spinel_fault_injector.hpp"
#endif

class IproSpinelUart final : public ot::Spinel::SpinelInterface
{
public:
    otError Init(ReceiveFrameCallback callback, void *context, RxFrameBuffer &buffer) override;
    void Deinit() override;
    otError SendFrame(const uint8_t *frame, uint16_t length) override;
    // SendFrame() even after Release(): the last frame of a stop (radio off).
    otError SendFinalFrame(const uint8_t *frame, uint16_t length) { return Write(frame, length); }
    otError WaitForFrame(uint64_t timeoutUs) override;
    void UpdateFdSet(void *) override {}
    void Process(const void *) override;
    uint32_t GetBusSpeed() const override { return CONFIG_THREAD_RCP_UART_BAUD; }
#if defined(CONFIG_THREAD_RCP_RESET_GPIO) && CONFIG_THREAD_RCP_RESET_GPIO >= 0
    otError HardwareReset() override;
#else
    otError HardwareReset() override { return OT_ERROR_NOT_IMPLEMENTED; }
#endif
    const otRcpInterfaceMetrics *GetRcpInterfaceMetrics() const override { return &mMetrics; }
    uint32_t GetHardwareResetCount() const { return mHardwareResets; }
    // Spinel owns UART1 (not a bench console that took it over).
    bool OwnsUart() const { return !sReleased && sActive == this; }
    // A bench console took UART1 for good: give it up without touching its
    // interrupts (the new owner set them) and never route, read or write it
    // again - Init() fails, SendFrame() drops. Also valid before any Init().
    static void Release()
    {
        sReleased = true;
        sActive = nullptr;
    }
    static bool IsReleased() { return sReleased; }
    // Before Init(): reboot the RCP with an in-band Spinel platform reset and
    // wait up to waitMs for its reset notification. True when it came back.
    bool PlatformReset(uint32_t waitMs, uint32_t &elapsedMs);
#ifdef CONFIG_THREAD_RCP_UART_FAULT_INJECTION
    // All test controls require the same OpenThread lock as SendFrame.
    bool StartFaultInjection(uint64_t now, uint32_t seed) { return mFault.Start(now, seed); }
    void StopFaultInjection(uint64_t now) { mFault.Stop(now); }
    IproSpinelFaultInjector::Stats GetFaultStats(uint64_t now) { return mFault.Snapshot(now); }
#endif

#ifdef CONFIG_THREAD_RCP_UART_DIAGNOSTICS
    // Instance-lifetime counters modulo 2^32. Poll at least every five minutes
    // and accumulate unsigned deltas externally for long-running tests.
    struct RxDiagnostics {
        uint32_t uartReadBytes;
        uint32_t uartReadErrors;
        uint32_t ringDropBytes;
        uint32_t ringOverflowEvents;
        uint32_t drainDropBytes;
        uint32_t overflowDrains;
        uint32_t decodeBytes;
        uint32_t validFrames;
        uint32_t parseErrors; // Includes short frames as well as invalid FCS.
        uint32_t noBufferErrors;
        uint32_t otherErrors;
    };
    // Call from the OpenThread owner, or while holding its lock. Masks the
    // UART ISR while copying; this is not a lock against other task callers.
    RxDiagnostics GetRxDiagnostics() const;
#endif

private:
    static void RouteUart();
    static void ReceiveIsr(void *, void *, uint32_t, uint32_t);
    static void FrameDone(void *context, otError error);
    otError Write(const uint8_t *frame, uint16_t length);
    bool Drain();
    static constexpr unsigned kRingSize = 4096;
    static IproSpinelUart *sActive;
    static volatile bool sReleased;
    uint8_t mRing[kRingSize] = {};
    volatile unsigned mHead = 0;
    volatile unsigned mTail = 0;
    volatile bool mOverflow = false;
    ReceiveFrameCallback mCallback = nullptr;
    void *mContext = nullptr;
    RxFrameBuffer *mBuffer = nullptr;
    ot::Hdlc::Decoder mDecoder;
    // TX holds one Spinel frame, not the aggregate multi-frame RX queue.
    // Each payload/FCS byte may escape to two bytes, plus two HDLC flags.
    ot::Spinel::FrameBuffer<2 * SPINEL_FRAME_MAX_SIZE + 6> mTx;
    otRcpInterfaceMetrics mMetrics = {};
    uint32_t mHardwareResets = 0;
#ifdef CONFIG_THREAD_RCP_UART_FAULT_INJECTION
    IproSpinelFaultInjector mFault;
#endif
#ifdef CONFIG_THREAD_RCP_UART_DIAGNOSTICS
    volatile RxDiagnostics mRxDiagnostics = {};
#endif
};
#endif

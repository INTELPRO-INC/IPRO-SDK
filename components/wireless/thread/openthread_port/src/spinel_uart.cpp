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

#include "spinel_uart.hpp"

#include <string.h>

extern "C" {
#include "FreeRTOS.h"
#include "task.h"
#include "drv_glb.h"
#include "drv_gpio.h"
#include "hal_uart.h"
#include "openthread_port.h"
#include "ot_rcp_health.h"
#include "ipro_log.h"
}
#include <openthread/platform/time.h>

IproSpinelUart *IproSpinelUart::sActive = nullptr;
volatile bool IproSpinelUart::sReleased = false;

#ifdef CONFIG_THREAD_RCP_UART_FRAME_LOG
// Spinel packed unsigned int (7 bits per byte, LSB first); returns bytes used, 0 if cut short.
static unsigned UnpackUint(const uint8_t *p, uint16_t len, uint32_t &value)
{
    value = 0;
    for (unsigned i = 0; i < len && i < 4; ++i) {
        value |= static_cast<uint32_t>(p[i] & 0x7f) << (7 * i);
        if ((p[i] & 0x80) == 0) return i + 1;
    }
    return 0;
}

// Header, command and property id only: property values can carry MAC keys and are never logged.
static void LogFrame(const char *dir, const uint8_t *frame, uint16_t length, int error)
{
    uint32_t cmd = 0, prop = 0;
    unsigned n = (length > 1) ? UnpackUint(frame + 1, length - 1, cmd) : 0;
    if (n != 0 && length > 1 + n) UnpackUint(frame + 1 + n, length - 1 - n, prop);
    IPRO_LOGI("SpinelUART", "%s len=%u err=%d hdr=%02x cmd=%lu prop=0x%lx", dir, length, error,
              length ? frame[0] : 0, (unsigned long)cmd, (unsigned long)prop);
}
#endif

extern "C" __attribute__((weak)) void ot_spinel_uart_get_link(uint8_t *tx_pin, uint8_t *rx_pin, uint32_t *baud)
{
    *tx_pin = CONFIG_THREAD_RCP_UART_TX_PIN;
    *rx_pin = CONFIG_THREAD_RCP_UART_RX_PIN;
    *baud = CONFIG_THREAD_RCP_UART_BAUD;
}

otError IproSpinelUart::Init(ReceiveFrameCallback callback, void *context, RxFrameBuffer &buffer)
{
    if (sReleased) return OT_ERROR_INVALID_STATE; // a bench console owns UART1
    if (sActive != nullptr) return OT_ERROR_ALREADY;
    mCallback = callback;
    mContext = context;
    mBuffer = &buffer;
    mHead = mTail = 0;
    mOverflow = false;
    mDecoder.Init(buffer, FrameDone, this);
    sActive = this;
    RouteUart();
    return OT_ERROR_NONE;
}

void IproSpinelUart::RouteUart()
{
    uint8_t pins[2];
    uint32_t baud;
    ot_spinel_uart_get_link(&pins[0], &pins[1], &baud);
    GLB_GPIO_Func_Init(GPIO_FUN_UART, pins, 2);
    GLB_UART_Fun_Sel_Swap(pins[0] % 8, GLB_UART_SIG_FUN_UART1_TXD);
    GLB_UART_Fun_Sel_Swap(pins[1] % 8, GLB_UART_SIG_FUN_UART1_RXD);
    uart_set_baudrate(1, baud);
    uart_init(1, 0);
    uart_ioctl(1, UART_CTRL_SET_CALLBACK, reinterpret_cast<void *>(ReceiveIsr));
    uart_ioctl(1, UART_CTRL_SET_INT,
               reinterpret_cast<void *>(UART_RX_END_IT | UART_RX_FIFO_IT | UART_RTO_IT));
}

// SpinelDriver only ever sends a stack reset, which leaves the RCP's radio
// driver and RF firmware as they were (an RCP wedged in RX enable stays
// wedged across BR reboots). A platform reset reboots the RCP's MCU. The
// reply is an unsolicited LAST_STATUS RESET_* (7e 8x 06 00 7x ...); any other
// LAST_STATUS means the RCP does not support it.
bool IproSpinelUart::PlatformReset(uint32_t waitMs, uint32_t &elapsedMs)
{
    static const uint8_t kReset[] = {SPINEL_HEADER_FLAG, SPINEL_CMD_RESET, SPINEL_RESET_PLATFORM};
    uint8_t win[5] = {};
    bool reset = false, answered = false;

    elapsedMs = 0;
    if (sReleased) return false;
    mHead = mTail = 0;
    mOverflow = false;
    sActive = this;
    RouteUart();
    mTx.Clear();
    ot::Hdlc::Encoder encoder(mTx);
    if (encoder.BeginFrame() != OT_ERROR_NONE || encoder.Encode(kReset, sizeof(kReset)) != OT_ERROR_NONE ||
        encoder.EndFrame() != OT_ERROR_NONE) {
        Deinit();
        elapsedMs = 0;
        return false;
    }
    const uint64_t start = otPlatTimeGet();
    uart_write(1, mTx.GetFrame(), mTx.GetLength());
    ++mMetrics.mTxFrameCount;
    unsigned scan = mTail;
    while (!answered && !sReleased && otPlatTimeGet() - start < static_cast<uint64_t>(waitMs) * 1000u) {
        while (scan != mHead && !answered) {
            memmove(win, win + 1, sizeof(win) - 1);
            win[sizeof(win) - 1] = mRing[scan];
            scan = (scan + 1) % kRingSize;
            if (win[0] == 0x7e && (win[1] & 0xf0) == 0x80 && win[2] == SPINEL_CMD_PROP_VALUE_IS &&
                win[3] == SPINEL_PROP_LAST_STATUS) {
                answered = true;
                reset = (win[4] & 0xf0) == 0x70;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    elapsedMs = static_cast<uint32_t>((otPlatTimeGet() - start) / 1000u);
    Deinit(); // Init() starts again from an empty ring
    return reset;
}

#if defined(CONFIG_THREAD_RCP_RESET_GPIO) && CONFIG_THREAD_RCP_RESET_GPIO >= 0
#ifndef CONFIG_THREAD_RCP_RESET_PULSE_MS
#define CONFIG_THREAD_RCP_RESET_PULSE_MS 20
#endif
// Called by SpinelDriver when the RCP ignored a Spinel stack reset (a hung
// RCP never processes one); it then waits for the RCP's RESET notification.
otError IproSpinelUart::HardwareReset()
{
    GLB_GPIO_Cfg_Type cfg = {};

    if (sActive != this) return OT_ERROR_NOT_IMPLEMENTED; // UART1 was handed to someone else

    cfg.gpioPin = CONFIG_THREAD_RCP_RESET_GPIO;
    cfg.gpioFun = GPIO_FUN_GPIO;
    cfg.gpioMode = GPIO_MODE_OUTPUT;
    cfg.pullType = GPIO_PULL_NONE;
    cfg.smtCtrl = 1;
    GLB_GPIO_Init(&cfg);
    GLB_GPIO_Write(CONFIG_THREAD_RCP_RESET_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(CONFIG_THREAD_RCP_RESET_PULSE_MS));
    // Release (open-drain style) so it never fights a debugger's reset line.
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_UP;
    GLB_GPIO_Init(&cfg);
    // Reclaims the UART pads too (the reset pin may be one of them on a bench
    // RCP fed through its RX pin, or rcpcon may have taken UART1).
    RouteUart();
    taskENTER_CRITICAL();
    mTail = mHead;
    mOverflow = false;
    taskEXIT_CRITICAL();
    mBuffer->DiscardFrame();
    mDecoder.Init(*mBuffer, FrameDone, this);
    ++mHardwareResets;
    IPRO_LOGW("SpinelUART", "RCP hardware reset %lu on GPIO%d (%d ms)", (unsigned long)mHardwareResets,
              CONFIG_THREAD_RCP_RESET_GPIO, CONFIG_THREAD_RCP_RESET_PULSE_MS);
    return OT_ERROR_NONE;
}
#endif

void IproSpinelUart::Deinit()
{
    if (sActive == this) {
        uart_ioctl(1, UART_CTRL_CLR_INT,
                   reinterpret_cast<void *>(UART_RX_END_IT | UART_RX_FIFO_IT | UART_RTO_IT));
        sActive = nullptr;
    }
}

void IproSpinelUart::ReceiveIsr(void *, void *, uint32_t, uint32_t)
{
    uint8_t data[64];
    int count = uart_read(1, data, sizeof(data));
    IproSpinelUart *self = sActive;
    if (self == nullptr) return;
#ifdef CONFIG_THREAD_RCP_UART_DIAGNOSTICS
    if (count > 0) self->mRxDiagnostics.uartReadBytes += static_cast<uint32_t>(count);
    if (count < 0) ++self->mRxDiagnostics.uartReadErrors;
#endif
    for (int i = 0; i < count; ++i) {
        unsigned next = (self->mHead + 1) % kRingSize;
        if (next == self->mTail) {
#ifdef CONFIG_THREAD_RCP_UART_DIAGNOSTICS
            ++self->mRxDiagnostics.ringOverflowEvents;
            self->mRxDiagnostics.ringDropBytes += static_cast<uint32_t>(count - i);
#endif
            self->mOverflow = true;
            break;
        }
        self->mRing[self->mHead] = data[i];
        __asm__ volatile ("fence rw,rw" ::: "memory");
        self->mHead = next;
    }
    ot_system_event_var = static_cast<ot_system_event_t>(
        ot_system_event_var | OT_SYSTEM_EVENT_UART_RXD);
    otSysEventSignalPending();
}

bool IproSpinelUart::Drain()
{
    if (mOverflow) {
        taskENTER_CRITICAL();
#ifdef CONFIG_THREAD_RCP_UART_DIAGNOSTICS
        mRxDiagnostics.drainDropBytes += (mHead + kRingSize - mTail) % kRingSize;
        ++mRxDiagnostics.overflowDrains;
#endif
        mTail = mHead;
        mOverflow = false;
        taskEXIT_CRITICAL();
        mBuffer->DiscardFrame();
        mDecoder.Init(*mBuffer, FrameDone, this);
        ++mMetrics.mTransferredGarbageFrameCount;
    }
    bool received = false;
    while (mTail != mHead) {
        uint8_t byte = mRing[mTail];
        mTail = (mTail + 1) % kRingSize;
#ifdef CONFIG_THREAD_RCP_UART_DIAGNOSTICS
        ++mRxDiagnostics.decodeBytes;
#endif
        mDecoder.Decode(&byte, 1);
        received = true;
    }
    return received;
}

void IproSpinelUart::Process(const void *) { Drain(); }

otError IproSpinelUart::WaitForFrame(uint64_t timeoutUs)
{
    uint64_t start = otPlatTimeGet();
    do {
        if (Drain()) return OT_ERROR_NONE;
        vTaskDelay(1);
    } while (otPlatTimeGet() - start < timeoutUs);
    IPRO_LOGW("SpinelUART", "timeout tx=%lu rx=%lu bad=%lu head=%u tail=%u",
              (unsigned long)mMetrics.mTxFrameCount, (unsigned long)mMetrics.mRxFrameCount,
              (unsigned long)mMetrics.mTransferredGarbageFrameCount, mHead, mTail);
    return OT_ERROR_RESPONSE_TIMEOUT;
}

otError IproSpinelUart::SendFrame(const uint8_t *frame, uint16_t length)
{
    if (sReleased) return OT_ERROR_INVALID_STATE; // UART1 belongs to a bench console
    return Write(frame, length);
}

otError IproSpinelUart::Write(const uint8_t *frame, uint16_t length)
{
    if (length > SPINEL_FRAME_MAX_SIZE) return OT_ERROR_NO_BUFS;
    if (frame == nullptr && length != 0) return OT_ERROR_INVALID_ARGS;
    if (length >= 2 && frame[1] == SPINEL_CMD_RESET) ot_rcp_note_reset();
    mTx.Clear();
    ot::Hdlc::Encoder encoder(mTx);
    otError error = encoder.BeginFrame();
    if (error == OT_ERROR_NONE) error = encoder.Encode(frame, length);
    if (error == OT_ERROR_NONE) error = encoder.EndFrame();
    if (error != OT_ERROR_NONE) return error;
#ifdef CONFIG_THREAD_RCP_UART_FAULT_INJECTION
    // Test-only: corrupt a complete encoded TX frame, preserving boundaries.
    mFault.Apply(mTx.GetFrame(), mTx.GetLength(), otPlatTimeGet());
#endif
#ifdef CONFIG_THREAD_RCP_UART_FRAME_LOG
    LogFrame("TX", frame, length, 0);
#endif
    uart_write(1, mTx.GetFrame(), mTx.GetLength());
    ++mMetrics.mTxFrameCount;
    mMetrics.mTxFrameByteCount += length;
    return OT_ERROR_NONE;
}

void IproSpinelUart::FrameDone(void *context, otError error)
{
    auto &self = *static_cast<IproSpinelUart *>(context);
    ++self.mMetrics.mTransferredFrameCount;
#ifdef CONFIG_THREAD_RCP_UART_FRAME_LOG
    LogFrame("RX", self.mBuffer->GetFrame(), self.mBuffer->GetLength(), error);
#endif
    if (error == OT_ERROR_NONE) {
#ifdef CONFIG_THREAD_RCP_UART_DIAGNOSTICS
        ++self.mRxDiagnostics.validFrames;
#endif
        ++self.mMetrics.mRxFrameCount;
        ++self.mMetrics.mTransferredValidFrameCount;
        self.mMetrics.mRxFrameByteCount += self.mBuffer->GetLength();
        self.mCallback(self.mContext);
    } else {
#ifdef CONFIG_THREAD_RCP_UART_DIAGNOSTICS
        if (error == OT_ERROR_PARSE) ++self.mRxDiagnostics.parseErrors;
        else if (error == OT_ERROR_NO_BUFS) ++self.mRxDiagnostics.noBufferErrors;
        else ++self.mRxDiagnostics.otherErrors;
#endif
        ++self.mMetrics.mTransferredGarbageFrameCount;
        self.mBuffer->DiscardFrame();
    }
}

#ifdef CONFIG_THREAD_RCP_UART_DIAGNOSTICS
IproSpinelUart::RxDiagnostics IproSpinelUart::GetRxDiagnostics() const
{
    RxDiagnostics snapshot;
    taskENTER_CRITICAL();
    snapshot.uartReadBytes = mRxDiagnostics.uartReadBytes;
    snapshot.uartReadErrors = mRxDiagnostics.uartReadErrors;
    snapshot.ringDropBytes = mRxDiagnostics.ringDropBytes;
    snapshot.ringOverflowEvents = mRxDiagnostics.ringOverflowEvents;
    snapshot.drainDropBytes = mRxDiagnostics.drainDropBytes;
    snapshot.overflowDrains = mRxDiagnostics.overflowDrains;
    snapshot.decodeBytes = mRxDiagnostics.decodeBytes;
    snapshot.validFrames = mRxDiagnostics.validFrames;
    snapshot.parseErrors = mRxDiagnostics.parseErrors;
    snapshot.noBufferErrors = mRxDiagnostics.noBufferErrors;
    snapshot.otherErrors = mRxDiagnostics.otherErrors;
    taskEXIT_CRITICAL();
    return snapshot;
}
#endif

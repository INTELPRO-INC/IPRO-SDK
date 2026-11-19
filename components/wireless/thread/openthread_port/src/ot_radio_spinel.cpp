/*
 *  Copyright (c) 2020, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the radio apis for the IPRO6S FreeRTOS RadioSpinel host (adapted from POSIX).
 */


#include <assert.h>
#include <new>
#include <string.h>
#include <openthread/platform/radio.h>
#include <openthread/platform/diag.h>
#include <openthread/platform/misc.h>
#include <openthread/platform/time.h>
#include "lib/platform/exit_code.h"
#include "lib/spinel/radio_spinel.hpp"
#include "lib/spinel/spinel_driver.hpp"
#include "spinel_uart.hpp"
extern "C" {
#include <stdio.h>
#include "openthread_port.h"
#include "ot_rcp_health.h"
#include "ipro_log.h"
#ifdef CONFIG_SHELL
#include "shell.h"
#endif
#include <stdlib.h>
}
alignas(IproSpinelUart) static unsigned char sUartStorage[sizeof(IproSpinelUart)];
alignas(ot::Spinel::SpinelDriver) static unsigned char sDriverStorage[sizeof(ot::Spinel::SpinelDriver)];
alignas(ot::Spinel::RadioSpinel) static unsigned char sRadioStorage[sizeof(ot::Spinel::RadioSpinel)];
#define sUart (*reinterpret_cast<IproSpinelUart *>(sUartStorage))
#define sDriver (*reinterpret_cast<ot::Spinel::SpinelDriver *>(sDriverStorage))
#define sRadio (*reinterpret_cast<ot::Spinel::RadioSpinel *>(sRadioStorage))
static ot::Spinel::RadioSpinel &GetRadioSpinel() { return sRadio; }
#ifndef CONFIG_THREAD_RCP_BOOT_RESET_WAIT_MS
#define CONFIG_THREAD_RCP_BOOT_RESET_WAIT_MS 3000
#endif
#ifndef IPRO_RCP_BUS_LATENCY_US
#define IPRO_RCP_BUS_LATENCY_US 5000u
#endif
static bool sRadioStarted;

// An error answer from the RCP to a radio command (a timeout goes through
// RadioSpinel's own recovery instead). Stops OpenThread; see ot_rcp_health.h.
static void RadioCommandOrDie(otError error, int line)
{
    if (error != OT_ERROR_NONE) {
        IPRO_LOGE("RadioSpinel", "Radio command failed: %d", error);
        ot_rcp_fatal(OT_EXIT_FAILURE, "radio-cmd", __FILE__, line);
    }
}
#define RadioOrDie(aError) RadioCommandOrDie((aError), __LINE__)

// Frames moving both ways mean the RCP works: resets before were transient.
static void ReceiveDone(otInstance *aInstance, otRadioFrame *aFrame, otError aError)
{
    if (aError == OT_ERROR_NONE) {
        ot_rcp_note_progress();
    }
    otPlatRadioReceiveDone(aInstance, aFrame, aError);
}

static void TransmitDone(otInstance *aInstance, otRadioFrame *aFrame, otRadioFrame *aAckFrame, otError aError)
{
    if (aError == OT_ERROR_NONE) {
        ot_rcp_note_progress();
    }
    otPlatRadioTxDone(aInstance, aFrame, aAckFrame, aError);
}

extern "C" void ot_radioInit(otInstance *)
{
    if (IproSpinelUart::IsReleased()) {
        // The console took UART1 before OpenThread started: leave it there.
        // (Later, Init() and SendFrame() refuse and RadioSpinel gives up.)
        ot_rcp_fatal(OT_EXIT_FAILURE, "uart-released", __FILE__, __LINE__);
    }
    // Explicit construction also supports BootROM entry paths that bypass
    // the application's C++ init_array. These objects live for the firmware lifetime.
    new (sUartStorage) IproSpinelUart;
    new (sDriverStorage) ot::Spinel::SpinelDriver;
    new (sRadioStorage) ot::Spinel::RadioSpinel;
    sRadioStarted = true;
#ifdef CONFIG_THREAD_RCP_BOOT_PLATFORM_RESET
    {
        uint32_t ms = 0;
        if (sUart.PlatformReset(CONFIG_THREAD_RCP_BOOT_RESET_WAIT_MS, ms)) {
            IPRO_LOGI("RadioSpinel", "RCP platform reset: back after %lu ms", (unsigned long)ms);
        } else {
            IPRO_LOGW("RadioSpinel", "RCP platform reset: no reset notification (%lu ms)", (unsigned long)ms);
        }
    }
#endif
    const spinel_iid_t iid = 0;
    auto type = sDriver.Init(sUart, true, &iid, 1);
    if (type != OT_COPROCESSOR_RCP) {
        IPRO_LOGE("RadioSpinel", "Co-processor is not an RCP (type %d): %s", type, sDriver.GetVersion());
        ot_rcp_fatal(OT_EXIT_RADIO_SPINEL_INCOMPATIBLE, "not-rcp", __FILE__, __LINE__);
    }
    ot::Spinel::RadioSpinelCallbacks callbacks = {};
    callbacks.mEnergyScanDone = otPlatRadioEnergyScanDone;
    callbacks.mBusLatencyChanged = otPlatRadioBusLatencyChanged;
    callbacks.mReceiveDone = ReceiveDone;
    callbacks.mTransmitDone = TransmitDone;
    callbacks.mTxStarted = otPlatRadioTxStarted;
#if OPENTHREAD_CONFIG_DIAG_ENABLE
    callbacks.mDiagReceiveDone = otPlatDiagRadioReceiveDone;
    callbacks.mDiagTransmitDone = otPlatDiagRadioTransmitDone;
#endif
    sRadio.SetCallbacks(callbacks);
    sRadio.Init(false, true, &sDriver,
                OT_RADIO_CAPS_ACK_TIMEOUT | OT_RADIO_CAPS_TRANSMIT_RETRIES |
                OT_RADIO_CAPS_CSMA_BACKOFF | OT_RADIO_CAPS_TRANSMIT_SEC |
                OT_RADIO_CAPS_TRANSMIT_TIMING, true);
    // Radio time = RCP time (host clock + an offset measured with
    // SPINEL_PROP_RCP_TIMESTAMP). The POSIX platform switches the sync on when
    // the Thread netif comes up; without it RadioSpinel::GetNow() stays
    // UINT64_MAX and the first CSL child's schedule spins forever in
    // CslTxScheduler::GetNextCslTransmissionDelay() (BR hung, 2026-09-23).
    sRadio.SetTimeSyncState(true);
    // Host -> RCP pipeline (OT task wake, spinel encode, UART, the RCP's own
    // task) measured 3-5 ms on the IPRO6S BR. OpenThread requests a CSL
    // frame only OPENTHREAD_CONFIG_MAC_CSL_REQUEST_AHEAD_US (2 ms) + bus
    // transfer + this latency ahead of the child's sample; with 0 the frame
    // reached the RCP after its target and went out late. Set before the
    // instance is created: CslTxScheduler reads it in its constructor.
    sRadio.SetBusLatency(IPRO_RCP_BUS_LATENCY_US);
    IPRO_LOGI("RadioSpinel", "RCP ready: %s caps=0x%x", sDriver.GetVersion(), sRadio.GetRadioCaps());
}
extern "C" void ot_radioTask(ot_system_event_t) { sDriver.Process(nullptr); sRadio.Process(nullptr); }
extern "C" bool ot_radio_drain_pending_reinit(void) { return false; }
extern "C" void ot_radio_dbg_dump(void) { IPRO_LOGI("RadioSpinel", "state=%d", sRadio.GetState()); }

extern "C" void ot_radio_spinel_stop(void)
{
    // A live RCP keeps its PAN, addresses and keys and would keep auto-ACKing
    // frames for this BR (a "ghost ACKer"); turn its radio off first.
    static const uint8_t kPhyOff[] = {SPINEL_HEADER_FLAG, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_PHY_ENABLED, 0};

    if (!sRadioStarted) {
        return;
    }
    // TX still reaches the RCP when a bench console took the RX side over.
    (void)sUart.SendFinalFrame(kPhyOff, sizeof(kPhyOff));
    if (sUart.OwnsUart()) {
        sUart.Deinit();
    }
}

extern "C" void ot_radio_spinel_release_uart(void)
{
    IproSpinelUart::Release();
}

// Shell diagnostics without the OpenThread lock: a racy snapshot of counters
// and cached values only, so it still works after a fatal error.
extern "C" void ot_rcp_print_status(void)
{
    ot_rcp_fault_t fault;
    const bool failed = ot_rcp_get_fault(&fault);

    printf("OpenThread: %s\r\n",
           failed ? "stopped (fatal error)" : (otrGetInstanceIfReady() ? "running" : "starting"));
    if (failed) {
        const char *file = fault.file ? strrchr(fault.file, '/') : nullptr;
        printf("  fault: exit=%d cause=%s at %s:%d task=%s uptime=%lu ms\r\n", fault.exit_code, fault.cause,
               file ? file + 1 : (fault.file ? fault.file : "-"), fault.line, fault.task,
               (unsigned long)fault.uptime_ms);
    }
    if (!sRadioStarted) {
        printf("RCP: not started\r\n");
        return;
    }
    const otRadioSpinelMetrics &m = sRadio.GetRadioSpinelMetrics();
    const otRcpInterfaceMetrics *u = sUart.GetRcpInterfaceMetrics();
    printf("RCP: %s\r\n  caps=0x%x timeouts=%lu unexpected_resets=%lu restorations=%lu parse_errors=%lu\r\n",
           sDriver.GetVersion(), sRadio.GetRadioCaps(), (unsigned long)m.mRcpTimeoutCount,
           (unsigned long)m.mRcpUnexpectedResetCount, (unsigned long)m.mRcpRestorationCount,
           (unsigned long)m.mSpinelParseErrorCount);
    printf("  uart: tx=%lu rx=%lu garbage=%lu hw_resets=%lu\r\n", (unsigned long)u->mTxFrameCount,
           (unsigned long)u->mRxFrameCount, (unsigned long)u->mTransferredGarbageFrameCount,
           (unsigned long)sUart.GetHardwareResetCount());
}
// CSL parent: advertise the RCP's clock accuracy and TX timing uncertainty to
// SSED children. OpenThread's weak defaults claim the worst case (255 ppm,
// 2.55 ms), which widens every child's receive window and costs it power.
#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
// 0 = ask the RCP. A board whose crystal and CSL TX timing were characterised
// can set them (Kconfig, or `rcp_csl` at run time); children learn new values
// at their next MLE exchange with this parent.
#ifndef CONFIG_THREAD_RCP_CSL_ACCURACY_PPM
#define CONFIG_THREAD_RCP_CSL_ACCURACY_PPM 0
#endif
#ifndef CONFIG_THREAD_RCP_CSL_UNCERTAINTY_10US
#define CONFIG_THREAD_RCP_CSL_UNCERTAINTY_10US 0
#endif
static uint8_t sCslAccuracy = CONFIG_THREAD_RCP_CSL_ACCURACY_PPM;
static uint8_t sCslUncertainty = CONFIG_THREAD_RCP_CSL_UNCERTAINTY_10US;
uint8_t otPlatRadioGetCslAccuracy(otInstance *) { return sCslAccuracy ? sCslAccuracy : sRadio.GetCslAccuracy(); }
uint8_t otPlatRadioGetCslUncertainty(otInstance *)
{
    return sCslUncertainty ? sCslUncertainty : sRadio.GetCslUncertainty();
}
#ifdef CONFIG_SHELL
static void cmd_rcp_csl(int argc, char **argv)
{
    if (argc == 3) {
        sCslAccuracy = static_cast<uint8_t>(strtoul(argv[1], nullptr, 0));
        sCslUncertainty = static_cast<uint8_t>(strtoul(argv[2], nullptr, 0));
    }
    printf("CSL parent accuracy %u ppm, uncertainty %u x 10 us (0 = from the RCP)\r\n", sCslAccuracy,
           sCslUncertainty);
}
SHELL_CMD_EXPORT_ALIAS(cmd_rcp_csl, rcp_csl, CSL parent accuracy ppm and uncertainty x10us [acc unc] (0 = RCP));
#endif
#endif
uint32_t otPlatRadioGetSupportedChannelMask(otInstance *) { return sRadio.GetRadioChannelMask(false); }
uint32_t otPlatRadioGetPreferredChannelMask(otInstance *) { return sRadio.GetRadioChannelMask(true); }
void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64)
{
    OT_UNUSED_VARIABLE(aInstance);
    RadioOrDie(GetRadioSpinel().GetIeeeEui64(aIeeeEui64));
}

void otPlatRadioSetPanId(otInstance *aInstance, uint16_t panid)
{
    OT_UNUSED_VARIABLE(aInstance);
    RadioOrDie(GetRadioSpinel().SetPanId(panid));
}

void otPlatRadioSetExtendedAddress(otInstance *aInstance, const otExtAddress *aAddress)
{
    OT_UNUSED_VARIABLE(aInstance);
    otExtAddress addr;

    for (size_t i = 0; i < sizeof(addr); i++)
    {
        addr.m8[i] = aAddress->m8[sizeof(addr) - 1 - i];
    }

    RadioOrDie(GetRadioSpinel().SetExtendedAddress(addr));
}

void otPlatRadioSetShortAddress(otInstance *aInstance, uint16_t aAddress)
{
    OT_UNUSED_VARIABLE(aInstance);
    RadioOrDie(GetRadioSpinel().SetShortAddress(aAddress));
}

void otPlatRadioSetAlternateShortAddress(otInstance *aInstance, uint16_t aAddress)
{
    OT_UNUSED_VARIABLE(aInstance);
    RadioOrDie(GetRadioSpinel().SetAlternateShortAddress(aAddress));
}

void otPlatRadioSetPromiscuous(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);
    RadioOrDie(GetRadioSpinel().SetPromiscuous(aEnable));
}

bool otPlatRadioIsEnabled(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().IsEnabled();
}

otError otPlatRadioEnable(otInstance *aInstance) { return GetRadioSpinel().Enable(aInstance); }

otError otPlatRadioDisable(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().Disable();
}

otError otPlatRadioSleep(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().Sleep();
}

otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError error;

    SuccessOrExit(error = GetRadioSpinel().Receive(aChannel));

exit:
    return error;
}

otError otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().Transmit(*aFrame);
}

otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return &GetRadioSpinel().GetTransmitFrame();
}

int8_t otPlatRadioGetRssi(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().GetRssi();
}

otRadioCaps otPlatRadioGetCaps(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().GetRadioCaps();
}

const char *otPlatRadioGetVersionString(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().GetVersion();
}

bool otPlatRadioGetPromiscuous(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().IsPromiscuous();
}

void otPlatRadioEnableSrcMatch(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);
    RadioOrDie(GetRadioSpinel().EnableSrcMatch(aEnable));
}

otError otPlatRadioAddSrcMatchShortEntry(otInstance *aInstance, uint16_t aShortAddress)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().AddSrcMatchShortEntry(aShortAddress);
}

otError otPlatRadioAddSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    OT_UNUSED_VARIABLE(aInstance);
    otExtAddress addr;

    for (size_t i = 0; i < sizeof(addr); i++)
    {
        addr.m8[i] = aExtAddress->m8[sizeof(addr) - 1 - i];
    }

    return GetRadioSpinel().AddSrcMatchExtEntry(addr);
}

otError otPlatRadioClearSrcMatchShortEntry(otInstance *aInstance, uint16_t aShortAddress)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().ClearSrcMatchShortEntry(aShortAddress);
}

otError otPlatRadioClearSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    OT_UNUSED_VARIABLE(aInstance);
    otExtAddress addr;

    for (size_t i = 0; i < sizeof(addr); i++)
    {
        addr.m8[i] = aExtAddress->m8[sizeof(addr) - 1 - i];
    }

    return GetRadioSpinel().ClearSrcMatchExtEntry(addr);
}

void otPlatRadioClearSrcMatchShortEntries(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    RadioOrDie(GetRadioSpinel().ClearSrcMatchShortEntries());
}

void otPlatRadioClearSrcMatchExtEntries(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    RadioOrDie(GetRadioSpinel().ClearSrcMatchExtEntries());
}

otError otPlatRadioEnergyScan(otInstance *aInstance, uint8_t aScanChannel, uint16_t aScanDuration)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().EnergyScan(aScanChannel, aScanDuration);
}

otError otPlatRadioGetTransmitPower(otInstance *aInstance, int8_t *aPower)
{
    OT_UNUSED_VARIABLE(aInstance);
    assert(aPower != nullptr);
    return GetRadioSpinel().GetTransmitPower(*aPower);
}

otError otPlatRadioSetTransmitPower(otInstance *aInstance, int8_t aPower)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().SetTransmitPower(aPower);
}

otError otPlatRadioGetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t *aThreshold)
{
    OT_UNUSED_VARIABLE(aInstance);
    assert(aThreshold != nullptr);
    return GetRadioSpinel().GetCcaEnergyDetectThreshold(*aThreshold);
}

otError otPlatRadioSetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t aThreshold)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().SetCcaEnergyDetectThreshold(aThreshold);
}

otError otPlatRadioGetFemLnaGain(otInstance *aInstance, int8_t *aGain)
{
    OT_UNUSED_VARIABLE(aInstance);
    assert(aGain != nullptr);
    return GetRadioSpinel().GetFemLnaGain(*aGain);
}

otError otPlatRadioSetFemLnaGain(otInstance *aInstance, int8_t aGain)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().SetFemLnaGain(aGain);
}

int8_t otPlatRadioGetReceiveSensitivity(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().GetReceiveSensitivity();
}

otRadioState otPlatRadioGetState(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().GetState();
}

void otPlatRadioSetMacKey(otInstance             *aInstance,
                          uint8_t                 aKeyIdMode,
                          uint8_t                 aKeyId,
                          const otMacKeyMaterial *aPrevKey,
                          const otMacKeyMaterial *aCurrKey,
                          const otMacKeyMaterial *aNextKey,
                          otRadioKeyType          aKeyType)
{
    RadioOrDie(GetRadioSpinel().SetMacKey(aKeyIdMode, aKeyId, aPrevKey, aCurrKey, aNextKey));
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aKeyType);
}

void otPlatRadioSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    RadioOrDie(GetRadioSpinel().SetMacFrameCounter(aMacFrameCounter, /* aSetIfLarger */ false));
    OT_UNUSED_VARIABLE(aInstance);
}

void otPlatRadioSetMacFrameCounterIfLarger(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    RadioOrDie(GetRadioSpinel().SetMacFrameCounter(aMacFrameCounter, /* aSetIfLarger */ true));
    OT_UNUSED_VARIABLE(aInstance);
}

uint64_t otPlatRadioGetNow(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    const uint64_t now = GetRadioSpinel().GetNow();

    // UINT64_MAX = not synced to the RCP yet. Never hand that to the core:
    // CSL scheduling loops "while (window < now + ahead)" on it. Host time is
    // wrong by the (unknown) offset but finite; the sync fixes it shortly.
    return now != UINT64_MAX ? now : otPlatTimeGet();
}

uint32_t otPlatRadioGetBusSpeed(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().GetBusSpeed();
}

uint32_t otPlatRadioGetBusLatency(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return GetRadioSpinel().GetBusLatency();
}

#if defined(CONFIG_THREAD_RCP_UART_DIAGNOSTICS) && defined(CONFIG_SHELL)
// Read-only test command. Do not print while holding the OpenThread mutex.
static void cmd_spinel_rx_stats(int argc, char **argv)
{
    (void)argv;
    if (argc != 1) {
        IPRO_LOGW("SpinelUART", "usage: spinel_rx_stats");
        return;
    }
    if (otrGetInstanceIfReady() == nullptr) {
        IPRO_LOGW("SpinelUART", "rx_v1 not-ready");
        return;
    }
    if (!otrTryLock()) {
        IPRO_LOGW("SpinelUART", "rx_v1 busy");
        return;
    }
    const auto d = sUart.GetRxDiagnostics();
    const auto metrics = *sUart.GetRcpInterfaceMetrics();
    otrUnlock();
    IPRO_LOGI("SpinelUART",
              "rx_v1 read=%lu read_err=%lu ring_drop=%lu ring_overflow=%lu "
              "drain_drop=%lu overflow_drain=%lu decode=%lu valid=%lu "
              "parse=%lu no_buf=%lu other=%lu",
              (unsigned long)d.uartReadBytes, (unsigned long)d.uartReadErrors,
              (unsigned long)d.ringDropBytes, (unsigned long)d.ringOverflowEvents,
              (unsigned long)d.drainDropBytes, (unsigned long)d.overflowDrains,
              (unsigned long)d.decodeBytes, (unsigned long)d.validFrames,
              (unsigned long)d.parseErrors, (unsigned long)d.noBufferErrors,
              (unsigned long)d.otherErrors);
    // Payload counts exclude HDLC framing/escaping. TX means submitted to
    // uart_write, not independently verified delivery to the RCP.
    IPRO_LOGI("SpinelUART",
              "frames_v1 tx_submitted=%llu tx_payload_bytes=%llu "
              "rx_valid=%llu rx_payload_bytes=%llu garbage=%llu",
              (unsigned long long)metrics.mTxFrameCount,
              (unsigned long long)metrics.mTxFrameByteCount,
              (unsigned long long)metrics.mRxFrameCount,
              (unsigned long long)metrics.mRxFrameByteCount,
              (unsigned long long)metrics.mTransferredGarbageFrameCount);
}
SHELL_CMD_EXPORT_ALIAS(cmd_spinel_rx_stats, spinel_rx_stats, Read host UART RX diagnostic counters);
#endif

#if defined(CONFIG_THREAD_RCP_UART_FAULT_INJECTION) && defined(CONFIG_SHELL)
static bool ParseFaultSeed(const char *text, uint32_t &value)
{
    if (text == nullptr || *text == '\0') return false;
    uint32_t parsed = 0;
    for (; *text; ++text) {
        if (*text < '0' || *text > '9') return false;
        const uint32_t digit = static_cast<uint32_t>(*text - '0');
        if (parsed > (UINT32_MAX - digit) / 10u) return false;
        parsed = parsed * 10u + digit;
    }
    value = parsed;
    return true;
}

static void cmd_spinel_fault(int argc, char **argv)
{
    uint32_t seed = 0;
    const bool start = argc == 3 && strcmp(argv[1], "start") == 0 && ParseFaultSeed(argv[2], seed);
    const bool stop = argc == 2 && strcmp(argv[1], "stop") == 0;
    const bool status = argc == 2 && strcmp(argv[1], "status") == 0;
    if (!start && !stop && !status) {
        IPRO_LOGW("SpinelUART", "usage: spinel_fault start SEED | status | stop");
        return;
    }
    if (otrGetInstanceIfReady() == nullptr) {
        IPRO_LOGW("SpinelUART", "fault_v1 not-ready");
        return;
    }
    if (!otrTryLock()) {
        IPRO_LOGW("SpinelUART", "fault_v1 busy");
        return;
    }
    const uint64_t now = otPlatTimeGet();
    const bool accepted = !start || sUart.StartFaultInjection(now, seed);
    if (stop) sUart.StopFaultInjection(now);
    const auto d = sUart.GetFaultStats(now);
    otrUnlock();
    IPRO_LOGI("SpinelUART",
              "fault_v1 direction=host-to-rcp accepted=%u active=%u seed=%lu "
              "start_us=%llu deadline_us=%llu stopped_observed_us=%llu "
              "frames=%llu corrupted=%llu malformed=%llu",
              accepted ? 1u : 0u, d.active ? 1u : 0u, (unsigned long)d.seed,
              (unsigned long long)d.startedUs, (unsigned long long)d.deadlineUs,
              (unsigned long long)d.stoppedObservedUs, (unsigned long long)d.frames,
              (unsigned long long)d.corrupted, (unsigned long long)d.malformed);
}
SHELL_CMD_EXPORT_ALIAS(cmd_spinel_fault, spinel_fault, Test-only HDLC TX fault injection);
#endif

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

/* Dedicated FTD measurement workload. Never linked into Matter Light or BR. */
#include <generated/autoconf.h>
#ifdef CONFIG_SHELL
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "shell.h"
#include "ipro_log.h"
#include "openthread_port.h"
#include <openthread/udp.h>
#include <openthread/message.h>
#include <openthread/platform/time.h>
#include "udp_load_wire.h"

#define TAG "UDP_LOAD"
#define MAX_COUNT 200000u
/* All mutable state is protected by the OT lock. The UDP callback already
 * owns that lock; it must not acquire the non-recursive mutex again. */
static otUdpSocket sSocket;
static otMessageInfo sPeer;
static otInstance *sInstance;
static TaskHandle_t sTask;
static bool sOpen;
static unsigned sMode, sDirection; /* 0 idle, 1 transmit, 2 receive */
static uint64_t sRun, sStart, sEnd, sFirst, sLast, sMaxLag, sDeadline;
static uint32_t sCount, sRate, sAttempted, sSent, sSendErrors;
static uint32_t sUnique, sDuplicate, sReordered, sForeign, sInvalid, sRange, sHighest;
static uint8_t sBitmap[(MAX_COUNT + 7u) / 8u];

static int number(const char *text, uint64_t limit, uint64_t *value)
{
    if (!text || text[0] < '0' || text[0] > '9') return -1;
    char *end;
    errno = 0;
    unsigned long long parsed = strtoull(text, &end, 0);
    if (errno || *end || parsed > limit) return -1;
    *value = (uint64_t)parsed;
    return 0;
}

static void report(void)
{
    IPRO_LOGI(TAG, "mode=%u direction=%u run=%llu count=%lu attempted=%lu sent=%lu send_errors=%lu",
              sMode, sDirection, (unsigned long long)sRun, (unsigned long)sCount,
              (unsigned long)sAttempted, (unsigned long)sSent, (unsigned long)sSendErrors);
    if (sDirection == 2) IPRO_LOGI(TAG, "unique=%lu missing=%lu duplicate=%lu reordered=%lu foreign=%lu invalid=%lu range=%lu",
              (unsigned long)sUnique, (unsigned long)(sCount - sUnique),
              (unsigned long)sDuplicate, (unsigned long)sReordered,
              (unsigned long)sForeign, (unsigned long)sInvalid, (unsigned long)sRange);
    IPRO_LOGI(TAG, "start_us=%llu end_us=%llu first_us=%llu last_us=%llu max_lag_us=%llu rate=%lu",
              (unsigned long long)sStart, (unsigned long long)sEnd,
              (unsigned long long)sFirst, (unsigned long long)sLast,
              (unsigned long long)sMaxLag, (unsigned long)sRate);
}

static void finish(const char *reason)
{
    sMode = 0;
    sEnd = otPlatTimeGet();
    if (sOpen) {
        otError error = otUdpClose(sInstance, &sSocket);
        if (error == OT_ERROR_NONE) sOpen = false;
        else IPRO_LOGE(TAG, "socket close error=%d", error);
    }
    IPRO_LOGI(TAG, "finished reason=%s", reason);
    report();
}

static void receive(void *context, otMessage *message, const otMessageInfo *info)
{
    (void)context; (void)info;
    if (sMode != 2) return;
    /* Do not count packets after the observation deadline if the worker was
     * delayed acquiring the OT mutex to close the socket. */
    uint64_t now = otPlatTimeGet();
    if (now >= sDeadline) return;
    uint8_t packet[UDP_LOAD_PACKET_SIZE];
    uint16_t offset = otMessageGetOffset(message);
    uint16_t length = otMessageGetLength(message);
    uint64_t run, sequence, stamp;
    if (length < offset || length - offset != sizeof(packet) ||
        otMessageRead(message, offset, packet, sizeof(packet)) != sizeof(packet) ||
        udp_load_decode(packet, sizeof(packet), &run, &sequence, &stamp) != 0) {
        ++sInvalid;
        return;
    }
    if (run != sRun) { ++sForeign; return; }
    if (sequence >= sCount) { ++sRange; return; }
    uint32_t seq = (uint32_t)sequence;
    uint8_t bit = (uint8_t)(1u << (seq & 7u));
    if (sBitmap[seq / 8u] & bit) { ++sDuplicate; return; }
    if (sUnique && seq < sHighest) ++sReordered;
    if (seq > sHighest) sHighest = seq;
    sBitmap[seq / 8u] |= bit;
    if (!sUnique) sFirst = now;
    ++sUnique;
    sLast = now;
}

static unsigned step(void)
{
    otrLock();
    unsigned mode = sMode;
    uint64_t now = otPlatTimeGet();
    if (mode && now >= sDeadline) {
        finish(mode == 1 ? "deadline-abort" : "observation-complete");
    } else if (mode == 1) {
        uint64_t due = sStart + (uint64_t)sAttempted * 1024000000ull / sRate;
        if (now >= due) {
            uint8_t packet[UDP_LOAD_PACKET_SIZE];
            (void)udp_load_encode(packet, sizeof(packet), sRun, sAttempted, now * 1000u);
            uint64_t lag = now - due;
            if (lag > sMaxLag) sMaxLag = lag;
            otMessage *message = otUdpNewMessage(sInstance, NULL);
            otError error = message ? otMessageAppend(message, packet, sizeof(packet)) : OT_ERROR_NO_BUFS;
            if (error == OT_ERROR_NONE) error = otUdpSend(sInstance, &sSocket, message, &sPeer);
            /* OT owns the message only after a successful send. */
            if (error != OT_ERROR_NONE) {
                if (message) otMessageFree(message);
                ++sSendErrors;
            } else {
                uint64_t completed = otPlatTimeGet();
                if (!sSent) sFirst = completed;
                sLast = completed;
                ++sSent;
            }
            ++sAttempted;
            if (sAttempted == sCount) finish("count-complete");
        }
    }
    mode = sMode;
    otrUnlock();
    return mode;
}

static void worker(void *argument)
{
    (void)argument;
    for (;;) {
        if (!step()) ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        else vTaskDelay(1); /* Bounded work per wake; actual pacing is measured. */
    }
}

static void command(int argc, char **argv)
{
    if (argc == 4 && !strcmp(argv[1], "bitmap")) {
        uint64_t offset, length;
        if (number(argv[2], MAX_COUNT / 8u, &offset) || number(argv[3], 64, &length) || !length) return;
        otrLock();
        if (!sMode && sDirection == 2 && offset + length <= (sCount + 7u) / 8u) {
            static const char hex[] = "0123456789abcdef";
            char text[129];
            for (unsigned i = 0; i < length; ++i) {
                uint8_t value = sBitmap[offset + i];
                text[2u * i] = hex[value >> 4];
                text[2u * i + 1u] = hex[value & 15u];
            }
            text[length * 2u] = '\0';
            IPRO_LOGI(TAG, "bitmap offset=%lu hex=%s", (unsigned long)offset, text);
        } else IPRO_LOGE(TAG, "bitmap requires completed RX and in-range byte slice");
        otrUnlock();
        return;
    }
    if (argc == 2 && (!strcmp(argv[1], "status") || !strcmp(argv[1], "stop"))) {
        otrLock();
        if (!strcmp(argv[1], "stop") && (sMode || sOpen)) finish("operator-stop");
        else report();
        otrUnlock();
        return;
    }
    bool tx = argc == 7 && !strcmp(argv[1], "tx");
    bool rx = argc == 6 && !strcmp(argv[1], "rx");
    uint64_t port, run, count, setting;
    int base = tx ? 3 : 2;
    otMessageInfo peer = {0};
    if ((!tx && !rx) || number(argv[base], 65535, &port) || !port ||
        number(argv[base + 1], UINT64_MAX, &run) ||
        number(argv[base + 2], MAX_COUNT, &count) || !count ||
        number(argv[base + 3], tx ? 250000 : 3600, &setting) || !setting ||
        (tx && (setting < 10000 || (count - 1) * 1024000000ull / setting > 3600000000ull ||
                otIp6AddressFromString(argv[2], &peer.mPeerAddr) != OT_ERROR_NONE))) {
        IPRO_LOGI(TAG, "udp_load tx IPv6 PORT RUN COUNT RATE_BPS | rx PORT RUN COUNT SECONDS | status | stop");
        return;
    }
    otrLock();
    otInstance *instance = otrGetInstanceIfReady();
    if (!instance || sMode || sOpen) {
        IPRO_LOGE(TAG, "not ready or previous workload/socket still active");
        otrUnlock();
        return;
    }
    sInstance = instance;
    if (!sTask && xTaskCreate(worker, "udpLoad", 1024, NULL, 1, &sTask) != pdPASS) {
        IPRO_LOGE(TAG, "worker allocation failed");
        otrUnlock();
        return;
    }
    memset(&sSocket, 0, sizeof(sSocket));
    otError error = otUdpOpen(sInstance, &sSocket, receive, NULL);
    if (error == OT_ERROR_NONE) {
        sOpen = true;
        otSockAddr local = {0};
        local.mPort = rx ? (uint16_t)port : 0;
        error = otUdpBind(sInstance, &sSocket, &local, OT_NETIF_THREAD_INTERNAL);
    }
    if (error != OT_ERROR_NONE) {
        IPRO_LOGE(TAG, "socket setup error=%d", error);
        if (sOpen) finish("setup-error");
        otrUnlock();
        return;
    }
    memset(sBitmap, 0, sizeof(sBitmap));
    sRun = run; sCount = (uint32_t)count; sRate = tx ? (uint32_t)setting : 0;
    sAttempted = sSent = sSendErrors = 0;
    sUnique = sDuplicate = sReordered = sForeign = sInvalid = sRange = sHighest = 0;
    sFirst = sLast = sMaxLag = 0;
    sPeer = peer; sPeer.mPeerPort = (uint16_t)port;
    sStart = otPlatTimeGet(); sEnd = 0;
    sDeadline = sStart + (rx ? setting * 1000000u :
                         (count - 1) * 1024000000ull / setting + 30000000ull);
    sMode = sDirection = tx ? 1 : 2;
    IPRO_LOGI(TAG, "started mode=%u port=%u", sMode, (unsigned)port);
    report();
    xTaskNotifyGive(sTask);
    otrUnlock();
}
SHELL_CMD_EXPORT_ALIAS(command, udp_load, Dedicated FTD UDP measurement workload);
#endif

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

/**
 * @file ot_stub.c
 * @brief OpenThread port layer initialization for IPRO7
 * 
 * This file provides the initialization and glue code between OpenThread
 * and the IPRO7 hardware platform.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include "openthread/instance.h"
#include "openthread/thread.h"
#include "openthread/ip6.h"
#include "openthread/cli.h"
#include "openthread/tasklet.h"
#include "openthread/platform/logging.h"
#include "openthread/platform/misc.h"
#include "openthread_port.h"
#ifdef CONFIG_THREAD_RADIO_SPINEL
#include "ot_rcp_health.h"
#endif
#include "hal_sec_trng.h"
#include "hal_sys.h"
#include "ipro_log.h"
#include "ipro_log.h"

/* Global OpenThread instance management is in ot_freertos.c */
extern otInstance *otrGetInstance(void);
extern void otrStackInit(void);
extern void otrStart(void);

#ifdef CONFIG_OPENTHREAD_CLI
#if defined(CONFIG_THREAD_WIFI_BORDER_ROUTER) || defined(CONFIG_NEWLIBC_CONSOLE_GUARD)
#include "ot_cli_line.h"
static struct ot_cli_line sCliLine;

static void cliEmitLine(const char *text)
{
    /* SDK puts is guarded and does not append a newline. Unlike printf's
     * static formatter, it can emit the full 511-byte line without clipping. */
    puts(text);
}

void otrCliOutputFlush(void)
{
    ot_cli_line_flush(&sCliLine, cliEmitLine);
}
#endif
/**
 * OpenThread CLI output callback - prints CLI output to console
 */
static int cliOutputCallback(void *aContext, const char *aFormat, va_list aArguments)
{
    char buffer[256];
    int ret = vsnprintf(buffer, sizeof(buffer), aFormat, aArguments);
    if (ret > 0)
    {
#if defined(CONFIG_THREAD_WIFI_BORDER_ROUTER) || defined(CONFIG_NEWLIBC_CONSOLE_GUARD)
        ot_cli_line_write(&sCliLine, buffer, cliEmitLine);
        /* Upstream CLI prompt is the sole unterminated standard CLI output.
         * Flush immediately, including startup and asynchronous completion. */
        if (strcmp(buffer, "> ") == 0) otrCliOutputFlush();
#else
        printf("%s", buffer);
#endif
    }
    return ret;
}
#endif /* CONFIG_OPENTHREAD_CLI */

/* otrStart() is implemented in ot_freertos.c */
extern void otrStart(void);

/* Helper function for ot_freertos.c to use. The CLI bring-up only links
 * cleanly when CONFIG_OPENTHREAD_CLI=y (which pulls in libopenthread-cli-*
 * providing otCliInit). Apps that don't enable OT CLI — e.g. the Matter
 * lighting-app — would otherwise see an undefined reference at link time. */
#ifdef CONFIG_OPENTHREAD_CLI
void otrCliInitialize(otInstance *aInstance)
{
    IPRO_LOGI("OpenThread", "Starting OpenThread...");

    if (aInstance == NULL)
    {
        IPRO_LOGE("OpenThread", "Instance not initialized!");
        return;
    }

    // Initialize OpenThread CLI with output callback
    // This allows OpenThread CLI to send output to the console
    otCliInit(aInstance, cliOutputCallback, NULL);

    IPRO_LOGI("OpenThread", "CLI initialized");
}
#endif /* CONFIG_OPENTHREAD_CLI */

void otrInitUser(otInstance *instance)
{
    if (instance == NULL) {
        IPRO_LOGE("OpenThread", "User init: instance is NULL!");
        return;
    }

#ifdef CONFIG_OPENTHREAD_RADIO
    extern void otRcpUartInit(otInstance *instance);
    otRcpUartInit(instance);
#elif defined(CONFIG_OPENTHREAD_CLI)
    // Initialize CLI
    otrCliInitialize(instance);
#endif
}

/* Additional helper implementations */
// Removed duplicate functions - now provided by OpenThread library

//=============================================================================
// Platform Assert & Logging Implementation
//=============================================================================

void otPlatAssertFail(const char *aFilename, int aLineNumber)
{
    IPRO_LOGE("OT-Assert", "Assert failed at %s:%d", aFilename, aLineNumber);
#ifdef CONFIG_THREAD_RADIO_SPINEL
    /* Stop OpenThread and let the application's policy act (a spin here, at
     * the OpenThread task's priority, starved the BR's management tasks). */
    if (!xPortIsInsideInterrupt()) {
        ot_rcp_fatal(1 /* OT_EXIT_FAILURE */, "assert", aFilename, aLineNumber);
    }
#endif
    // In production, could trigger a reboot or enter safe mode
    while (1) { /* Hang for debugging */ }
}

void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    va_list args;
    char buffer[256];
    const char *regionStr = "OT";
    
    // Map region to string
    switch (aLogRegion) {
        case OT_LOG_REGION_API:      regionStr = "OT-API"; break;
        case OT_LOG_REGION_MLE:      regionStr = "OT-MLE"; break;
        case OT_LOG_REGION_ARP:      regionStr = "OT-ARP"; break;
        case OT_LOG_REGION_NET_DATA: regionStr = "OT-NetData"; break;
        case OT_LOG_REGION_ICMP:     regionStr = "OT-ICMP"; break;
        case OT_LOG_REGION_IP6:      regionStr = "OT-IP6"; break;
        case OT_LOG_REGION_MAC:      regionStr = "OT-MAC"; break;
        case OT_LOG_REGION_MEM:      regionStr = "OT-MEM"; break;
        case OT_LOG_REGION_NCP:      regionStr = "OT-NCP"; break;
        case OT_LOG_REGION_MESH_COP: regionStr = "OT-MeshCoP"; break;
        case OT_LOG_REGION_NET_DIAG: regionStr = "OT-NetDiag"; break;
        case OT_LOG_REGION_PLATFORM: regionStr = "OT-Platform"; break;
        case OT_LOG_REGION_COAP:     regionStr = "OT-CoAP"; break;
        case OT_LOG_REGION_CLI:      regionStr = "OT-CLI"; break;
        case OT_LOG_REGION_CORE:     regionStr = "OT-Core"; break;
        case OT_LOG_REGION_UTIL:     regionStr = "OT-Util"; break;
        default:                     regionStr = "OT"; break;
    }
    
    va_start(args, aFormat);
    vsnprintf(buffer, sizeof(buffer), aFormat, args);
    va_end(args);
    
    // Map OpenThread log level to IPRO log level
    switch (aLogLevel) {
        case OT_LOG_LEVEL_NONE:
            break;
        case OT_LOG_LEVEL_CRIT:
            IPRO_LOGE(regionStr, "%s", buffer);
            break;
        case OT_LOG_LEVEL_WARN:
            IPRO_LOGW(regionStr, "%s", buffer);
            break;
        case OT_LOG_LEVEL_NOTE:
        case OT_LOG_LEVEL_INFO:
            IPRO_LOGI(regionStr, "%s", buffer);
            break;
        case OT_LOG_LEVEL_DEBG:
            IPRO_LOGD(regionStr, "%s", buffer);
            break;
    }
}

void otPlatReset(otInstance *aInstance)
{
    (void)aInstance;
    IPRO_LOGI("OpenThread", "Platform reset requested - rebooting...");
    /* IPRO7 software reboot: marks reset reason then triggers POR.
     * Required for `ot factoryreset` and `ot reset` to take effect —
     * Settings::Wipe() clears on-disk settings, but MLE in-memory
     * state (RLOC, parent cache, mLastSavedRole) only resets via a
     * real chip reboot. Without this, factoryreset wipes the disk
     * but the running MLE keeps using stale RLOC/parent IID and the
     * SED gets stuck doing Child Update Request to a dead parent. */
    hal_reboot();
    /* Unreachable: hal_reboot() does not return. */
    while (1) { }
}

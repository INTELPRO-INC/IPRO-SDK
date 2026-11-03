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
 * @file lp_tcpdtim.c
 * @brief DTIM standby test over a live TCP connection.
 *
 * ping/iperf-style host polling wakes the STA on the very first beacon (the AP
 * buffers the probe and sets TIM immediately), so those flows never exercise
 * real DTIM cycling. Here the HOST is silent except for one wake message per
 * round, sent a requested delay after the DUT asks for it; every wake that
 * carries no TCP data just re-enters PDS.
 *
 * Protocol (line-based; host side: scratchpad/tcpdtim_server.py):
 *   DUT -> "HI\n"          host -> "ECHO\n"      connectivity check
 *   DUT -> "REQ <sec>\n"   host sends "WAKE <n>\n" <sec> seconds later
 *
 * Round:
 *   REQ -> settle (let the host TCP ACK drain, or it sets TIM and wakes us
 *   instantly) -> PDS sleep -> on wake poll the socket:
 *     no data  -> re-sleep, same REQ still pending          (step 4a)
 *     got data -> print it, wake_count++, next round        (step 4b)
 * Stops after max_wakes rounds; a round that re-sleeps past its cap without
 * ever seeing data is counted as a MISS and a fresh REQ is issued.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lp_demo.h"

#if defined(CONFIG_WIFI6) && defined(CONFIG_LWIP)

#include <sys/socket.h>
#include <FreeRTOS.h>
#include <task.h>
#include "hal_lp.h"

/* Give the host's TCP ACK (to our REQ/HI uplink) time to arrive BEFORE PDS
 * entry — a buffered ACK sets TIM and converts the whole round into an
 * immediate wake, exactly the failure mode this test exists to avoid. */
#define TCPDTIM_ACK_SETTLE_MS   500
/* Post-wake window for the AP to flush the buffered segment after PM=0. */
#define TCPDTIM_RX_POLL_MS      100
#define TCPDTIM_RX_POLL_WIFI    30      /* WIFI wake: data is expected, 3 s  */
#define TCPDTIM_RX_POLL_RTC     5       /* RTC wake: just catch races, .5 s  */

static int tcpdtim_recv_poll(int sock, char *buf, size_t len, int polls)
{
    for (int i = 0; i < polls; i++) {
        int n = recv(sock, buf, len - 1, MSG_DONTWAIT);
        if (n > 0) {
            buf[n] = '\0';
            return n;
        }
        vTaskDelay(pdMS_TO_TICKS(TCPDTIM_RX_POLL_MS));
    }
    return 0;
}

int lp_tcpdtim_run(const char *host_ip, uint16_t port, uint32_t period_s,
                   uint32_t max_wakes, uint32_t dtim_ms, uint32_t cycles)
{
    char buf[128];
    struct sockaddr_in addr;
    uint32_t wakes = 0, misses = 0, resleeps_total = 0;

    /* One round sleeps cycles*dtim_ms per PDS entry; allow the round to span
     * the full requested period plus generous margin before calling it a MISS. */
    uint32_t sleep_span_ms = dtim_ms * (cycles ? cycles : 60);
    uint32_t resleep_cap = (period_s * 1000 / sleep_span_ms) + 4;

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        printf("[TCPDTIM] socket() failed\r\n");
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host_ip);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        printf("[TCPDTIM] connect %s:%u failed\r\n", host_ip, (unsigned)port);
        close(sock);
        return -1;
    }

    /* Step 1: hi -> echo connectivity check */
    send(sock, "HI\n", 3, 0);
    if (tcpdtim_recv_poll(sock, buf, sizeof(buf), TCPDTIM_RX_POLL_WIFI) <= 0) {
        printf("[TCPDTIM] no ECHO from host\r\n");
        close(sock);
        return -1;
    }
    printf("[TCPDTIM] connected %s:%u (%s), period=%lus max_wakes=%lu "
           "dtim=%lums x%lu (resleep cap %lu)\r\n",
           host_ip, (unsigned)port, buf,
           (unsigned long)period_s, (unsigned long)max_wakes,
           (unsigned long)dtim_ms, (unsigned long)cycles,
           (unsigned long)resleep_cap);

    while (wakes < max_wakes) {
        /* Step 2: ask the host for a delayed wake message */
        int len = snprintf(buf, sizeof(buf), "REQ %lu\n", (unsigned long)period_s);
        if (send(sock, buf, len, 0) < 0) {
            printf("[TCPDTIM] REQ send failed, aborting\r\n");
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(TCPDTIM_ACK_SETTLE_MS));
        /* drain anything unexpected so the wake poll below sees only fresh data */
        while (recv(sock, buf, sizeof(buf) - 1, MSG_DONTWAIT) > 0) {}

        /* Steps 3/4: sleep, and re-sleep every wake that brought no data */
        uint32_t resleeps = 0;
        for (;;) {
            lp_demo_sleep(dtim_ms, cycles);                     /* step 3 */

            int32_t reason;
            lp_demo_get_stats(NULL, NULL, NULL, &reason);
            int polls = (reason == HAL_LP_WAKEUP_WIFI) ? TCPDTIM_RX_POLL_WIFI
                                                       : TCPDTIM_RX_POLL_RTC;
            int n = tcpdtim_recv_poll(sock, buf, sizeof(buf), polls);
            if (n > 0) {                                        /* step 4b */
                wakes++;
                printf("[TCPDTIM] wake #%lu/%lu (reason=0x%lx, resleeps=%lu): %s\r\n",
                       (unsigned long)wakes, (unsigned long)max_wakes,
                       (unsigned long)reason, (unsigned long)resleeps, buf);
                break;
            }
            resleeps++;                                         /* step 4a */
            resleeps_total++;
            printf("[TCPDTIM] wake w/o data (reason=0x%lx), re-sleep %lu/%lu\r\n",
                   (unsigned long)reason, (unsigned long)resleeps,
                   (unsigned long)resleep_cap);
            if (resleeps >= resleep_cap) {
                misses++;
                printf("[TCPDTIM] MISS: no host message within cap, new REQ\r\n");
                break;
            }
        }
    }

    /* Step 5: done — back to shell idle */
    printf("[TCPDTIM] DONE: wakes=%lu/%lu misses=%lu resleeps=%lu\r\n",
           (unsigned long)wakes, (unsigned long)max_wakes,
           (unsigned long)misses, (unsigned long)resleeps_total);
    close(sock);
    return (misses == 0) ? 0 : -1;
}

#else /* !(CONFIG_WIFI6 && CONFIG_LWIP) */

int lp_tcpdtim_run(const char *host_ip, uint16_t port, uint32_t period_s,
                   uint32_t max_wakes, uint32_t dtim_ms, uint32_t cycles)
{
    (void)host_ip; (void)port; (void)period_s;
    (void)max_wakes; (void)dtim_ms; (void)cycles;
    printf("[TCPDTIM] not available: No-WiFi/No-lwIP build\r\n");
    return -1;
}

#endif /* CONFIG_WIFI6 && CONFIG_LWIP */

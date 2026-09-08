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
 * @file gtag_gw_service.h
 * @brief G-TAG gateway discovery via the Thread Network Data "Service" TLV.
 *
 * The gateway (FTD) registers this service in Network Data (otServerAddService);
 * necks resolve it (otNetDataGetNextService) to the gateway's CURRENT RLOC for
 * /reg + /fall + /diag — instead of assuming "gateway == Leader". The leader
 * assumption breaks whenever another FTD wins leadership (e.g. after a gateway
 * reboot, or with a thread_router in the mesh): /reg then lands on a node with
 * no handler and the CoAP CON silently times out. A registered service always
 * tracks the gateway's real RLOC, even across reboots / RLOC changes.
 *
 * Requires OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE=1 (set in the shared
 * openthread-core-ipro7-config.h). Shared by the neck + gateway apps.
 */
#ifndef GTAG_GW_SERVICE_H
#define GTAG_GW_SERVICE_H

/* Private/vendor IANA Enterprise Number ("GTAG"). Any non-Thread enterprise
 * number marks a vendor-specific service, so it never collides with Thread's
 * own service numbers (which live under enterprise number 44970). */
#define GTAG_GW_SERVICE_ENTERPRISE 0x47544147u  /* 'G''T''A''G' */

/* 1-byte service data identifying the G-TAG gateway service. */
#define GTAG_GW_SERVICE_DATA0      0x01u
#define GTAG_GW_SERVICE_DATA_LEN   1

#endif /* GTAG_GW_SERVICE_H */

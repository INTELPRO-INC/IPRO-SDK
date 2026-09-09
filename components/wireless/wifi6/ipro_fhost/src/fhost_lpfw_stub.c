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

/* Placeholder TU for the LP beacon-RX build (CONFIG_PHY_LPFW).
 *
 * The real fhost sources need the full UMAC task enum (export_enum.h
 * TASK_FTM/APM/SCANU/SM/ME), which the structural WiFi trim removes, and the
 * lp_fw link never uses this archive — but the component target must survive
 * because its public include dir feeds wifi6_lwip_adapter (lwipopts.h includes
 * fhost.h). Keep the library non-empty with this stub only.
 */
typedef int fhost_lpfw_stub_t;

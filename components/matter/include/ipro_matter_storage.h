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

#ifndef IPRO_MATTER_STORAGE_H
#define IPRO_MATTER_STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/* SDK-private lifecycle hooks, not an application command API. Results are
 * EasyFlash error codes (zero = success). One reset coordinator serializes
 * calls; storage must be initialized and all credential writers quiesced
 * before completion. No function reboots or releases live callback resources.
 * This is logical deletion, not secure physical erasure of Flash payloads. */
int ipro_matter_storage_begin_reset(void);
int ipro_matter_storage_complete_reset(void);

/* Initialize storage and finish pending reset BEFORE BLE/Thread/Matter start.
 * Any error must stop startup. A valid READY sentinel performs no writes;
 * missing sentinel (including first install) clears reset-owned namespaces
 * before creating READY. Provisioning/migration must account for this policy. */
int ipro_matter_storage_boot_recover(void);

/* Required transport-specific SDK hook. It must verify removal of network
 * credentials and preserve unrelated/factory data; there is no success stub.
 * The Thread app clears its ot- namespace; a Wi-Fi app supplies its own hook. */
int ipro_matter_reset_network_storage(void);

#ifdef __cplusplus
}
#endif
#endif

/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_BLE_CONTROLLER_H_
#define IPRO_BLE_CONTROLLER_H_

/*
 * Public SDK entry point for the IPRO BLE controller component.
 *
 * Applications should include this header instead of reaching into a platform
 * directory directly.  IPRO6S currently keeps the btdm_sdk compatibility API
 * while its RW v13.0.18 controller is migrated into this package; IPRO7
 * exports the byte-stream HCI and low-power controller API.
 */
#if defined(CONFIG_BTDM_ENABLE) || defined(CONFIG_IPRO6)
#include "btdm_sdk.h"
#include "ipro_ble_ctlr_hcitl.h"
#else
#include "../plf/ipro7/arch/main/ipro_ble_ctlr_arch.h"
#include "../hcitl/ipro_ble_ctlr_hcitl.h"
#endif

#endif /* IPRO_BLE_CONTROLLER_H_ */

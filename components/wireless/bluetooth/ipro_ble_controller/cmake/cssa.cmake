# SPDX-License-Identifier: Apache-2.0
set(IPRO_BLE_CTLR_CSSA_DEFS "")
set(IPRO_BLE_CTLR_CSSA_SUFFIX "")
if(CONFIG_IPRO_BLE_CTLR_CSSA)
    if(NOT IPRO_BLE_CTLR_FEATURE STREQUAL "bt54_iso2")
        message(FATAL_ERROR "IPRO7 CSSA requires the bt54_iso2 profile")
    endif()
    set(IPRO_BLE_CTLR_CSSA_DEFS CFG_CSSA)
    set(IPRO_BLE_CTLR_CSSA_SUFFIX "_cssa")
    # Experimental until radio qualification; never reuse the base archive.
    set(CONFIG_IPRO_BLE_CTLR_FORCE_SOURCE_BUILD ON)
endif()

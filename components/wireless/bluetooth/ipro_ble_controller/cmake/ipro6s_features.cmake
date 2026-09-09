# SPDX-License-Identifier: Apache-2.0
# Keep optional controller functionality separate from Host feature selection.
set(IPRO6S_OPTIONAL_BLE_DEFS "")
if(CONFIG_BTDM_PER_ADV OR CONFIG_BTDM_PAST OR CONFIG_BTDM_PAWR)
    list(APPEND IPRO6S_OPTIONAL_BLE_DEFS CFG_PER_ADV)
endif()
if(CONFIG_BTDM_PAST OR CONFIG_BTDM_PAWR)
    list(APPEND IPRO6S_OPTIONAL_BLE_DEFS CFG_PAST)
endif()
if(CONFIG_BTDM_PAWR)
    if(NOT CONFIG_BTDM_BLE_ENABLE)
        message(FATAL_ERROR "IPRO6S PAwR requires BTDM_BLE_ENABLE")
    endif()
    # RW v13 BLE_PAWR requires periodic advertising and PAST, not only BT54.
    list(APPEND IPRO6S_OPTIONAL_BLE_DEFS CFG_PAWR)
endif()

# BLE stack - uses bluetooth directory
if(CONFIG_BLUETOOTH_LE_ENABLE)
add_subdirectory(bluetooth)
endif()

if(CONFIG_IPRO_BLE_LIB)
add_subdirectory(ipro_ble_lib)
endif()

if(CONFIG_IEEE802154_ENABLE)
endif()

if(CONFIG_ZIGBEE_STACK)
endif()

if(CONFIG_OPENTHREAD_ENABLE)
endif()

if(CONFIG_BTBLECONTROLLER_LIB OR CONFIG_BLUETOOTH_LE_ENABLE OR CONFIG_IEEE802154_ENABLE OR CONFIG_ZIGBEE_STACK OR CONFIG_OPENTHREAD_ENABLE OR CONFIG_WIFI6 OR CONFIG_BTDM_ENABLE OR CONFIG_PHY_CLI)
add_subdirectory(rf)
endif()

if(CONFIG_WIFI6)
if(CONFIG_USE_MACSW)
# WiFi/BT PTA arbiter. Gated on exactly what gates CFG_COEX in wifi6's
# CMakeLists (and therefore RW_WLAN_COEX_EN, and therefore the hal_machw_init()
# call site): every build that compiles the coex block must link this, and the
# PHY_LPFW beacon-RX trim, which drops coex entirely, must not.
if(NOT CONFIG_PHY_LPFW)
add_subdirectory(ipro_coex)
endif()
add_subdirectory(macsw)
add_subdirectory(wifi6)
else()
endif()
endif()

# ATBM6162 (Altobeam OCEANUS) external Wi-Fi 6 over SDIO — independent of the
# on-chip RivieraWaves WIFI6 stack above. ATBM has its own RF; no IPRO7 rf component.
if(CONFIG_WIFI_ATBM6162)
add_subdirectory(wifi_atbm6162)
endif()

if(CONFIG_BTDM_ENABLE AND NOT CONFIG_BLUETOOTH_LE_ENABLE)
# IPRO6S controller code is packaged by the same component as IPRO7.  The
# CONFIG_BTDM_* names remain as compatibility configuration until applications
# migrate, but components/wireless/btdm no longer owns an implementation.
add_subdirectory(bluetooth/ipro_ble_controller)
endif()

if(CONFIG_LPAPP)
endif()

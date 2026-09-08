# ATBM6162 (Altobeam OCEANUS) Wi-Fi 6 SDIO driver — IPRO7 port
# Phase 0 scaffolding: WiFi-only SDIO subset, no wpa / no BLE / no data-path yet.
# See .claude/plans/atbm6162-integration.md.

__generate_library(1)

# ---- Sources --------------------------------------------------------------
# HAL core (exclude BLE combo sources: CONFIG_WIFI_BT_COMB=0 -> unreachable).
file(GLOB ATBM_HAL_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/hal/*.c)
list(REMOVE_ITEM ATBM_HAL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/atbm_ble.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/atbm_ble_driver_api.c)
__component_add_sources(${ATBM_HAL_SRCS})

# HAL SDIO transport + firmware download.
file(GLOB ATBM_SDIO_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/hal/sdio/*.c)
__component_add_sources(${ATBM_SDIO_SRCS})

# 802.11 net / data-path core.
file(GLOB ATBM_NET_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/net/*.c)
__component_add_sources(${ATBM_NET_SRCS})

# Phase 2: minimal real WPA core. The STA connect path is event-driven and cannot
# be stubbed (priv->appdata = init_wpa_supplicant() or it NULL-faults). SAE/WPS/P2P
# stay off (atbm_config.h) so net/wpa/{sae,p2p,wps} + most crypto stay out.
# sha1.c/aes_core.c are link-needed by compiled-but-unused secured key paths.
__component_add_sources(net/wpa/wpa_main.c)
__component_add_sources(net/wpa/wpa_event.c)
__component_add_sources(net/wpa/wpa_common.c)
__component_add_sources(net/wpa/wpabuf.c)
__component_add_sources(net/wpa/wpa_debug.c)
__component_add_sources(net/wpa/sha1.c)
__component_add_sources(net/wpa/aes_core.c)

# API surface.
__component_add_sources(api/atbm_api.c)

# OS abstraction (fh_rtt base, run on FreeRTOS via the RT-Thread compat shim).
__component_add_sources(os/fh_rtt/atbm_os_mem.c)
__component_add_sources(os/fh_rtt/atbm_os_atomic.c)
__component_add_sources(os/fh_rtt/atbm_os_mutex.c)
__component_add_sources(os/fh_rtt/atbm_os_msgQ.c)
__component_add_sources(os/fh_rtt/atbm_os_thread.c)
__component_add_sources(os/fh_rtt/atbm_os_timer.c)
__component_add_sources(os/fh_rtt/atbm_os_spinlock.c)
__component_add_sources(os/fh_rtt/atbm_os_workqueue.c)
# Phase 2 data path: real netdev/skbuf glue + lwIP netif bring-up (SDIO os-api).
__component_add_sources(os/fh_rtt/atbm_os_skbuf.c)
__component_add_sources(os/fh_rtt/sdio/atbm_os_api.c)

# IPRO7 port glue: RT-Thread->FreeRTOS shim, SDIO host binding, bring-up entry.
__component_add_sources(port_ipro7/rtthread_compat.c)
__component_add_sources(port_ipro7/atbm_os_sdio.c)
__component_add_sources(port_ipro7/atbm_bringup.c)
__component_add_sources(port_ipro7/atbm_sta_api.c)
__component_add_sources(port_ipro7/atbm_phase0_stubs.c)

# ---- Include dirs ---------------------------------------------------------
# Vendor headers are PRIVATE so their unprefixed typedefs (u8, min, bool ...) do
# not leak into the app or other SDK components. Only the clean bring-up header
# (port_ipro7) is exported.
__component_add_private_include_dirs(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/api
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/include
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/compress
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/sdio
    ${CMAKE_CURRENT_SOURCE_DIR}/net/include
    ${CMAKE_CURRENT_SOURCE_DIR}/net/include/proto
    ${CMAKE_CURRENT_SOURCE_DIR}/net/wpa/crypto/include
    ${CMAKE_CURRENT_SOURCE_DIR}/os/include
    ${CMAKE_CURRENT_SOURCE_DIR}/os/fh_rtt/include
    ${CMAKE_CURRENT_SOURCE_DIR}/port_ipro7/public)

# port_ipro7/ holds both the RT-Thread compat shim (rtdef.h/rtthread.h, used
# internally) and the public bring-up header — keep it PRIVATE for the vendor
# build and export only the public subdir.
__component_add_private_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/port_ipro7)
__component_add_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/port_ipro7/public)

# ---- Compile flags (PRIVATE — must not leak to app/other components) ------
# The vendor mac80211-derived C trips GCC-14's strict-C errors (implicit decls,
# int/pointer conversions). Start permissive for Phase-0 velocity, scoped PRIVATE;
# ratchet back on file-by-file in later phases (cf. commit c6e017d72 for macsw).
__component_add_private_compile_options(
    -w
    -fpermissive
    -fcommon
    -Wno-error=implicit-function-declaration
    -Wno-error=int-conversion
    -Wno-error=incompatible-pointer-types
    -Wno-error=implicit-int)

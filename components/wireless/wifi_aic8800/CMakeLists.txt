# AIC8800 D80 (AICSemi) external Wi-Fi 5 + BT combo over SDIO — IPRO7 port.
#
# aic8800_release/ is the vendor's IPRO7 customer release, consumed as ONE prebuilt
# archive: the FullMAC host driver, wpa_supplicant/hostapd, AIC's mbedTLS wrappers,
# the IPRO7 platform layer (SDIO shim, FreeRTOS/lwIP glue, Wi-Fi CLI) and the SDIO
# HCI mailbox transport are all inside lib/libsoftutil-aic_wifi.a. The package is
# committed byte-for-byte with its SHA256SUMS.txt, so nothing here may edit it; the
# SDIO HAL sources the vendor tested are under aic8800_release/integration/ and were
# merged into bsp/ by hand (see README.md for which hunks were taken and which not).
set(AIC8800_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/aic8800_release)
set(AIC8800_LIB ${AIC8800_RELEASE}/lib/libsoftutil-aic_wifi.a)
if(NOT EXISTS ${AIC8800_LIB})
    message(FATAL_ERROR "CONFIG_WIFI_AIC8800: ${AIC8800_LIB} not found")
endif()

set(CURRENT_STATIC_LIBRARY aic8800)
add_library(${CURRENT_STATIC_LIBRARY} STATIC IMPORTED GLOBAL)
set_target_properties(${CURRENT_STATIC_LIBRARY} PROPERTIES IMPORTED_LOCATION ${AIC8800_LIB})
set_property(GLOBAL APPEND PROPERTY SDK_LIBS ${CURRENT_STATIC_LIBRARY})
target_link_libraries(${CURRENT_STATIC_LIBRARY} INTERFACE __component_lib)
message(STATUS "AIC8800: using prebuilt archive ${AIC8800_LIB}")

# Public: our bring-up header plus the vendor's two BT headers, which sit directly in
# include/ next to aic_chip_desc.h and collide with nothing in the SDK.
# NOT public: include/utils/inc and include/drv_api — the vendor's co_list.h /
# co_math.h / compiler.h have the same names as the IPRO BLE controller's.
# An app that needs the full Wi-Fi API (wifi_api.h, netif_api.h, ...) adds
# AIC8800_VENDOR_INCLUDE_DIRS to its own PRIVATE include dirs.
__component_add_include_dirs(include)
__component_add_include_dirs(${AIC8800_RELEASE}/include)
set(AIC8800_VENDOR_INCLUDE_DIRS
    ${AIC8800_RELEASE}/include
    ${AIC8800_RELEASE}/include/drv_api/inc
    ${AIC8800_RELEASE}/include/drv_wifi
    ${AIC8800_RELEASE}/include/utils/inc
    ${AIC8800_RELEASE}/port/inc
    CACHE INTERNAL "AIC8800 vendor API include dirs (opt-in per app)")

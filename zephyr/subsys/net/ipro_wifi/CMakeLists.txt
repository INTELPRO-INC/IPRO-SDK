# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0
zephyr_library_named(ipro_wifi_net)
zephyr_include_directories(${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_LIST_DIR}
  ${IPRO_SDK_ROOT}/components/wireless/wifi6/wifi6_lwip_adapter/include)
zephyr_library_sources(net_al_zephyr.c net_al_zephyr_iface.c sdk_stubs.c)
zephyr_library_include_directories(${IPRO_SDK_ROOT}/components/wireless/wifi6/ipro_fhost/src
  ${IPRO_SDK_ROOT}/components/wireless/wifi6/ipro_wpa_supplicant/src/utils)
zephyr_library_link_libraries(__component_lib)

# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0
zephyr_library()
zephyr_library_sources(ipro_wifi.c)
zephyr_library_include_directories(${CMAKE_CURRENT_LIST_DIR}/../../subsys/net/ipro_wifi)
zephyr_library_include_directories(${IPRO_SDK_ROOT}/components/wireless/macsw/include/export)
zephyr_library_link_libraries(__component_lib)
zephyr_library_sources_ifdef(CONFIG_SHELL ipro_wifi_shell.c)
zephyr_library_sources_ifdef(CONFIG_WIFI_IPRO_TCP_TRACE ipro_wifi_tcptrace.c)


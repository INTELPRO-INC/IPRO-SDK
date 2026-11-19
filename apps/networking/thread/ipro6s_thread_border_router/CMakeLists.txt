__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/thread/openthread/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/thread/openthread_port/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/apps/platform/ipro6_if_wifi)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/matter/include)
include(${CMAKE_CURRENT_LIST_DIR}/domain_config.cmake)
if(CONFIG_THREAD_BR_BLE_MANAGEMENT)
    if(NOT CONFIG_BTDM_ENABLE OR NOT CONFIG_BT_SMP OR NOT CONFIG_BT_SETTINGS OR NOT CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
        message(FATAL_ERROR "BR BLE management requires BTDM, authenticated pairing and persistent bonds")
    endif()
    target_sources(app PRIVATE br_ble_management.c br_ble_protocol.c br_management.c br_em_recovery.c)
endif()
if(CONFIG_THREAD_BR_LAN_MANAGEMENT)
    if(NOT CONFIG_THREAD_BR_BLE_MANAGEMENT OR CONFIG_MATTER_ENABLE)
        message(FATAL_ERROR "LAN management requires BLE ownership and the standalone BR profile")
    endif()
    target_sources(app PRIVATE br_lan_management.c)
endif()
if(NOT "${IPRO_THREAD_BR_DOMAIN_PREFIX}" STREQUAL "")
    target_sources(app PRIVATE br_domain.c)
    target_compile_definitions(app PRIVATE IPRO_THREAD_BR_DOMAIN_PREFIX="${IPRO_THREAD_BR_DOMAIN_PREFIX}")
endif()
# BR-only: allow the command bridge to run alongside OT (priority 20).
# With priority 19, command entry stalled for seconds during BBR/reattachment.
# Use the shared component interface so shell_freertos.c receives the override.
__component_add_compile_definitions(SHELL_EXEC_THREAD_PRIO=20)
# `ot dataset ... -x <hex>` with a full operational dataset is ~300 chars.
# 384 x 3 history lines keeps the shell's RAM (256 x 5). The Matter BR's shell
# policy admits no such line: it keeps the defaults.
if(NOT CONFIG_MATTER_ENABLE)
    __component_add_compile_definitions(SHELL_CMD_SIZE=384 SHELL_HISTORY_LINES=3)
endif()
# The USB console drain task's stack is static .bss taken out of the heap
# region; its 4 KB default is far more than a ring copy + IN transfer needs.
__component_add_compile_definitions(USB_CONSOLE_TASK_STACK_WORDS=384)
target_sources(app PRIVATE wifi_backhaul.c br_matter_wifi.c br_matter_mdns.c br_matter_publish.c br_infra.c br_nd.c br_route.c br_thread_netif.c br_mdns.c br_udp.c
    br_onlink.c
    br_matter_dnssd_access.c
    br_dhcp6_pd.c
    br_nat64.c
    br_dns_upstream.c
    br_dns64.c
    br_matter_thread.c
    br_thread_activation.c
    br_shell_policy.c
    br_rcp_console.c
    br_rcp_recovery.c
    br_rcp_select.c
    br_matter_browse.c
    br_matter_resolve.c
    br_wifi_storage.c
    br_wifi_transaction.c
    br_wifi_connect_operation.c
    br_matter_wifi_connect.c
    br_matter_wifi_profile.c
    br_matter_wifi_scan.c
    br_matter_wifi_link.c
    br_wifi_scan_operation.c
    br_matter_reset.c
    br_reset_storage.c
    br_factory_reset.c
    br_network_startup.c
    br_cpp_runtime.c
    br_cpp_constructors.c
    ${CMAKE_SOURCE_DIR}/components/matter/matter_reset_storage.c
    br_multicast.c br_mld_sync.c br_multicast_policy.c br_nd_proxy_packet.c br_dua_table.c br_dua_proxy.c
    br_trace.c br_trace_store.c br_domain_ra.c br_cli_timing_store.c br_wifi_profile.c
    ${CMAKE_SOURCE_DIR}/apps/platform/ipro6_if_wifi/wl_board_cfg.c)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(main.c)
setup_project(ipro6s_thread_border_router)
target_compile_definitions(ipro6s_thread_border_router_IPRO6.elf PRIVATE OT_FREERTOS_ENABLE=1)
# OpenThread's DieNow() -> exit(): br_rcp_recovery.c takes it over before atexit handlers run.
target_link_options(ipro6s_thread_border_router_IPRO6.elf PRIVATE -Wl,--wrap=exit)
if(IPRO_MATTER_BR_PACKAGE_ACTIVE)
    target_sources(app PRIVATE br_matter_runtime.c br_wifi_provision.c br_wifi_provision_shell.c br_abort.c)
    target_link_options(ipro6s_thread_border_router_IPRO6.elf PRIVATE
        -Wl,--wrap=abort -Wl,--wrap=__assert_func)
else()
    # assert() in OpenThread code: stop OpenThread with the location (br_assert.c).
    target_sources(app PRIVATE br_assert.c)
    target_link_options(ipro6s_thread_border_router_IPRO6.elf PRIVATE -Wl,--wrap=__assert_func)
endif()

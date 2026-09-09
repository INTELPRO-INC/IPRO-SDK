# IPRO6 BTDM Demo Application
# ===========================
# BLE Controller demo application for IPRO6 FPGA platform

# Use generated linker script (IPRO6 uses IPRO7's BSP)
set(CONFIG_USE_GENERATED_LINKER_SCRIPT ON)

__component_add_include_dirs(.)
__component_add_include_dirs(./include)

target_compile_options(app PRIVATE -Werror)
if(CONFIG_IPRO_BT_HOST_PAWR_TEST AND CONFIG_SHELL AND CONFIG_BT_PER_ADV_RSP AND CONFIG_BT_PER_ADV_SYNC_RSP)
    target_sources(app PRIVATE ${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/tests/board/pawr_probe.c)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)
endif()
if(CONFIG_IPRO_BT_HOST_EAD_TEST AND CONFIG_SHELL AND CONFIG_BT_EAD)
    target_sources(app PRIVATE ${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/tests/board/ead_probe.c)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)
endif()
if(CONFIG_IPRO_BT_HOST_ADI_TEST AND CONFIG_SHELL AND CONFIG_BT_PER_ADV)
    target_sources(app PRIVATE ${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/tests/board/adi_probe.c)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)
endif()
if(CONFIG_IPRO_BT_HOST_PAST_TEST AND CONFIG_SHELL)
    target_sources(app PRIVATE ${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/tests/board/past_probe.c)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)
endif()
if(CONFIG_IPRO_BT_HOST_GATT_SECURITY_TEST AND CONFIG_SHELL AND CONFIG_BT_GATT_CLIENT)
    target_sources(app PRIVATE
        ${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/tests/board/gatt_security_probe.c)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)
endif()
if(CONFIG_IPRO_BLE_CTLR_CH_CLASS_TEST AND CONFIG_SHELL)
    target_sources(app PRIVATE
        ${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/tests/board/ch_class_peripheral_probe.c)
    __component_add_include_dirs(
        ${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_ble_controller/include
        ${CMAKE_SOURCE_DIR}/components/utils/include)
endif()
if(CONFIG_IPRO_BT_HOST_COC_TEST)
    if(NOT CONFIG_SHELL OR NOT CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
        message(FATAL_ERROR "CoC fixture requires SHELL and L2CAP_DYNAMIC_CHANNEL")
    endif()
    target_sources(app PRIVATE ${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/tests/board/coc_probe.c)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)
endif()

# =============================================================================
# BTDM Application Sources
# =============================================================================
if (CONFIG_BTDM_ENABLE)
    # BTDM application logic
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/btdm_app.c
    )

    if(CONFIG_BTDM_VOICE_OVER_HCI)
        target_sources(app PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src/ipro6_sco_i2s_tx.c
        )
        __component_add_include_dirs(
            ${CMAKE_SOURCE_DIR}/components/audio/include
            ${CMAKE_SOURCE_DIR}/components/utils/include
        )
    endif()

    # CLI commands for BTDM testing
    if (CONFIG_SHELL)
        target_sources(app PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src/btdm_cli.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/bt_capture.c
        )
    endif()
endif()

if(CONFIG_IPRO6_CLASSIC_BRINGUP)
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ipro6_classic_bringup.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ipro6_classic_port.c
    )
endif()

# =============================================================================
# Build Configuration
# =============================================================================

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
# Force whole-archive linking for app to include shell commands
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(main.c)
setup_project(ipro6_btdm_phyrf)

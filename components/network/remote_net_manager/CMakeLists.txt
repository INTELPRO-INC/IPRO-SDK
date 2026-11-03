__generate_library()

__component_add_include_dirs(inc)

__component_add_sources(src/rnm_server.c)
__component_add_sources(src/rnm_server_cmd_handler.c)
__component_add_sources(src/rnm_common.c)
__component_add_sources(src/rnm_utils.c)
# Host-driven firmware OTA (IPRO_CMD_OTA) over the ipro6 ROM flash API.
if(CONFIG_RNM_OTA)
    __component_add_sources(src/rnm_ota.c)
    __component_add_sources(src/rnm_ota_core.c)
    __component_add_sources(src/rnm_ota_flash.c)
    __component_add_compile_definitions(-DRNMS_OTA_ENABLE)
endif()
__component_add_sources(src/rnm_user_ext.c)

__component_add_compile_definitions(-DCFG_IPRO_WIFI)
# AP vif MAC = STA MAC with byte 5 bit 0 flipped (wifi_mgmr_ext.c)
__component_add_compile_definitions(-DWIFI_MGMR_AP_MAC_SPLIT)

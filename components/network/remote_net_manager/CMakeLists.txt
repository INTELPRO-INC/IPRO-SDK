__generate_library()

__component_add_include_dirs(inc)

__component_add_sources(src/rnm_server.c)
__component_add_sources(src/rnm_server_cmd_handler.c)
__component_add_sources(src/rnm_common.c)
__component_add_sources(src/rnm_utils.c)
# OTA is out-of-scope for the initial IPRO6LE bring-up; see Open Question #8 /
# DEV-3 in USBWIFI_PORTING_PLAN.md. The OTA path depends on bl_mtd/HAL APIs
# that do not exist on IPRO6LE. Excluded for now; rnms_handle_ota() is stubbed.
# __component_add_sources(src/rnm_ota.c)
__component_add_sources(src/rnm_user_ext.c)

__component_add_compile_definitions(-DCFG_IPRO_WIFI)

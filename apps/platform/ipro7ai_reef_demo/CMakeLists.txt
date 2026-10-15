# IPRO7AI Reef carrier board demo: WS2812 LED, keys, DAC speaker/headphone,
# PDM / headset / AUX-IN capture, SD slot, OV5640/OV2640 camera stream, Wi-Fi
# fin gate.

__component_add_include_dirs(.)
__component_add_include_dirs(src)

# main.c is registered only via __set_main_file() below so it compiles into the
# ELF target alone - listing it here as well would double-define main() once the
# app archive is force-linked with --whole-archive.
target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rgb_led.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/keys.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sdcard.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cam.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_cli.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_cli.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sd_cli.c
)

# Wi-Fi through the IPRO6S_FIN in CARD1 (ipronet_host over SDIO).
if(CONFIG_WIFI_USE_IPRO6S)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/wifi_ipro6s.c)
    # ipro6s_ota shell command (HTTP fetch + ipronet_ota_update)
    set(IPRONET_OTA_CLI_DIR ${IPRO_SDK_BASE}/apps/platform/common/ipronet_ota_cli)
    __component_add_include_dirs(${IPRONET_OTA_CLI_DIR})
    target_sources(app PRIVATE
        ${IPRONET_OTA_CLI_DIR}/ipronet_ota_cli.c
        ${IPRONET_OTA_CLI_DIR}/ota_http_fetch.c
        ${IPRONET_OTA_CLI_DIR}/ota_http_parse.c
    )
endif()

# RTSP MJPEG camera stream over that Wi-Fi; camera ch0 at sensor / 2.
if(CONFIG_WIFI_USE_IPRO6S AND CONFIG_RTSP_SERVER)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/wifi_rtsp.c)
    target_compile_definitions(app PRIVATE CAM_SCALE_DIV=2u)
endif()

# SHELL_CMD_EXPORT_ALIAS casts int(*)(int,char**) to the shell's generic
# syscall_func; that is the shell's contract, not something this app can fix,
# and a per-source property does not reach the `app` target from this scope.
target_compile_options(app PRIVATE -Wall -Wextra -Wno-cast-function-type)

# The shell commands are reachable only through their FSymTab section, never
# referenced from main, so the app archive must be force-linked whole.
set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)

__set_main_file(main.c)
setup_project(ipro7ai_reef_demo)

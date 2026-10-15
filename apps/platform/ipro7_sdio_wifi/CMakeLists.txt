__component_add_include_dirs(.)

# ipro6s_ota shell command (HTTP fetch + ipronet_ota_update)
set(IPRONET_OTA_CLI_DIR ${IPRO_SDK_BASE}/apps/platform/common/ipronet_ota_cli)
__component_add_include_dirs(${IPRONET_OTA_CLI_DIR})
target_sources(app PRIVATE
    ${IPRONET_OTA_CLI_DIR}/ipronet_ota_cli.c
    ${IPRONET_OTA_CLI_DIR}/ota_http_fetch.c
    ${IPRONET_OTA_CLI_DIR}/ota_http_parse.c
)

# main.c builds into the ELF through __set_main_file(); the app archive
# holds commands reached only through FSymTab, so link it whole.
set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)

__set_main_file(main.c)
setup_project(ipro7_sdio_wifi)

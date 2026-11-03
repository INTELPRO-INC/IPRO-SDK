__generate_library()

__component_add_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/esp_common/include)
__component_add_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/esp_iperf/include)
__component_add_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/esp_log/include)

__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/esp_iperf/iperf.c)
# iperf_cli.c: shell system (CONFIG_SHELL) or legacy CLI (CONFIG_COMPONENTS_CLI_CMD_ENABLE)
if(CONFIG_SHELL OR CONFIG_COMPONENTS_CLI_CMD_ENABLE)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/esp_iperf/iperf_cli.c)
endif()

__component_add_private_compile_options(-Wno-error=format=)

if(CONFIG_SHELL)
    __component_add_link_libraries(-ucmd_iperf)
endif()
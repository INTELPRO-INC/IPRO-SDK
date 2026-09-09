__generate_library(1)

# Force whole-archive linking to include WiFi CLI commands (SHELL_CMD_EXPORT_ALIAS)
__component_whole_archive()

__component_add_sources_ifdef(CONFIG_SHELL cli_al.c)
__component_add_sources(net_al.c)
__component_add_sources(pbuf_custom_ref.c)
__component_add_sources_ifdef(CONFIG_DHCPD net_al_ext.c)
__component_add_sources(wifi_pkt_hooks.c)
__component_add_sources(tx_buffer_copy.c)

__component_add_sources_ifdef(CONFIG_WIFI_TG net_tg_al.c net_tg_al_priv.c net_ping_al_priv.c)
__component_add_sources_ifdef(CONFIG_WIFI_IPERF net_iperf_al.c)

if (CONFIG_WIFI_MAT)
    __component_add_compile_definitions(-DCONFIG_MAT)
    __component_add_sources(mat.c)
    if (CONFIG_WIFI_MAT_FULL_COPY)
        __component_add_compile_definitions(-DCONFIG_MAT_FULL_COPY)
    endif()
endif()
__component_add_include_dirs(include)
__component_add_include_dirs(.)

__component_add_compile_definitions(-DPLATFORM_PRIVDE_ERRNO)
# Use shell instead of aos cli
# __component_add_compile_definitions(-DIOT_SDK_ADAPTER)

# IPRO6 SDIO-WiFi component, ported from
# bl_iot_sdk/components/network/host_router/dev_net_sdio.
# Mirrors components/network/usbwifi/CMakeLists.txt structure.

__generate_library()

__component_add_include_dirs(.)
__component_add_include_dirs(adapter)

# Chip-agnostic glue (upper layers + manager + buffers + control plane).
__component_add_sources(net_wifi_transceiver.c)
# DEBUG: gated DMA / cached-read inject-copy experiment.
__component_add_sources(sdio_dma_copy.c)
__component_add_sources(sdio_msg_ext_frame.c)
__component_add_sources(sdiowifi_mgmr.c)
__component_add_sources(sdiowifi_tty.c)
__component_add_sources(sdiowifi_tx_buf.c)
__component_add_sources(rx_profile.c)
__component_add_sources(hostrouter_init.c)
__component_add_sources(hostrouter_log.c)
__component_add_sources(adapter/sdiowifi_freertos.c)

# Hardware-specific transport: SMID (SDIO 3.0 device) controller —
# queue/ADMA model (ipro6_sdio3).
__component_add_sources(trcver_sdio_ipro6.c)
__component_add_sources(sdio_port_ipro6.c)
__component_add_compile_definitions(-DIPRO6)

# bl616_demo_sdiowifi proj_config.mk:184-185 sets these. Wire-protocol header
# magic + buffer accounting depend on these constants matching the legacy FW.
__component_add_compile_definitions(-DIPRO_HOSTROUTER_ENABLE)
__component_add_compile_definitions(-DBL_SDIO_RX_MAX_PORT_NUM=8)

if(CONFIG_DHCP_IN_EMB)
    __component_add_compile_definitions(-DDHCP_IN_EMB)
endif()

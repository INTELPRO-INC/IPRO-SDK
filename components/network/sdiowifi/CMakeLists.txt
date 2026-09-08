# IPRO6LE SDIO-WiFi component, ported from
# bl_iot_sdk/components/network/host_router/dev_net_sdio.
# Mirrors components/network/usbwifi/CMakeLists.txt structure.

__generate_library()

__component_add_include_dirs(.)
__component_add_include_dirs(adapter)

# Chip-agnostic glue (upper layers + manager + buffers + control plane).
__component_add_sources(net_wifi_transceiver.c)
# DEBUG: gated DMA / cached-read inject-copy experiment. IPRO6-specific body is
# #ifdef IPRO6 guarded; IPRO6LE gets legacy-equivalent stubs so it still links.
__component_add_sources(sdio_dma_copy.c)
__component_add_sources(sdio_msg_ext_frame.c)
__component_add_sources(sdiowifi_mgmr.c)
__component_add_sources(sdiowifi_tty.c)
__component_add_sources(sdiowifi_tx_buf.c)
__component_add_sources(rx_profile.c)
__component_add_sources(hostrouter_init.c)
__component_add_sources(hostrouter_log.c)
__component_add_sources(adapter/sdiowifi_freertos.c)

# Hardware-specific transport: one clean implementation per SDIO-device IP,
# selected at compile time (NOT #if-littered into one file).
#   IPRO6   : SMID (SDIO 3.0 device) controller — queue/ADMA model (ipro6_sdio3),
#             files trcver_sdio_ipro6.c / sdio_port_ipro6.c.
#   IPRO6LE : legacy SDU bitmap-mailbox IP (matches BL616's SDU peripheral,
#             SDU_BASE 0x2000d000, SDIO_IRQn, AHB SW reset GLB_AHB_MCU_SW_SDU),
#             files trcver_sdio.c / sdio_port.c (left byte-for-byte untouched).
# IPRO6 must NOT define IPRO6LE/CFG_IPRO_WIFI.
if("${CONFIG_CHIP}" STREQUAL "IPRO6")
    __component_add_sources(trcver_sdio_ipro6.c)
    __component_add_sources(sdio_port_ipro6.c)
    __component_add_compile_definitions(-DIPRO6)
else()
    __component_add_sources(trcver_sdio.c)
    __component_add_sources(sdio_port.c)
    __component_add_compile_definitions(-DCFG_IPRO_WIFI)
    __component_add_compile_definitions(-DIPRO6LE)
endif()

# bl616_demo_sdiowifi proj_config.mk:184-185 sets these. Wire-protocol header
# magic + buffer accounting depend on these constants matching the legacy FW.
__component_add_compile_definitions(-DIPRO_HOSTROUTER_ENABLE)
__component_add_compile_definitions(-DBL_SDIO_RX_MAX_PORT_NUM=8)

if(CONFIG_DHCP_IN_EMB)
    __component_add_compile_definitions(-DDHCP_IN_EMB)
endif()

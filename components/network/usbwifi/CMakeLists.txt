__generate_library()

__component_add_include_dirs(.)

__component_add_sources(usbwifi.c)
__component_add_sources(usb.c)
__component_add_sources(txrx.c)
__component_add_sources(data_channel.c)

__component_add_compile_definitions(-DUSBWIFI_ENABLE)

if(CONFIG_USBWIFI_DHCP_IN_EMB)
    __component_add_compile_definitions(-DDHCP_IN_EMB)
endif()

if(CONFIG_IPRO_DUAL_STACK)
    __component_add_compile_definitions(-DIPRO_DUAL_STACK)
endif()

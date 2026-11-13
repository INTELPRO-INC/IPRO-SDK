# ipro_ota: the OTA core every transport shares (see include/ipro_ota.h).
if(CONFIG_IPRO_OTA)

__generate_library()

__component_add_include_dirs(include)

__component_add_sources(
    src/ipro_ota_pkg.c
    src/ipro_ota.c
)

# OTA over the console UART: the `ota_uart` shell command.
__component_add_sources_ifdef(CONFIG_IPRO_OTA_UART src/ipro_ota_uart_frame.c src/ipro_ota_uart.c)
if(CONFIG_IPRO_OTA_UART)
    # The shell finds commands by their section, not by a call: keep the whole archive.
    set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS ipro_ota)
endif()

# OTA over BLE: the GATT service 0xFF60 (ipro_ota_ble.h). Needs the Bluetooth host.
__component_add_sources_ifdef(CONFIG_IPRO_OTA_BLE src/ipro_ota_ble.c)

# Signed packages need mbedTLS (ECDSA P-256, SHA-256).
__component_add_sources_ifdef(CONFIG_IPRO_OTA_SIGNATURE src/ipro_ota_sig.c)

endif()

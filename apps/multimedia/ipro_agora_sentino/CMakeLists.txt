# IPRO Agora Sentino AI Gateway Application
# Agora RTC voice AI with Sentino MQTT platform integration

# Include directories
__component_add_include_dirs(.)
__component_add_include_dirs(./src)
__component_add_include_dirs(./src/sentino)
__component_add_include_dirs(./src/nfc)
__component_add_include_dirs(./src/ble)

# LVGL port includes (only when LVGL is enabled)
if(CONFIG_COMPONENTS_LVGL_ENABLE)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/3rdparty/lvgl/port)
endif()

# IPRO Log System includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)

# LTE Modem includes (when CONFIG_LTE_MODEM is enabled)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/network/lte_modem/include)

# BLE stack includes (when CONFIG_BLUETOOTH_LE_ENABLE)
if(CONFIG_BLUETOOTH_LE_ENABLE)
    # Zephyr v4.4 host (components/wireless/bluetooth/ipro_bt_host). Its public
    # headers and the HCI-transport entry point; no controller-private includes.
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/include)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/port/include)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/upstream/zephyr/include)
endif()

# Agora SDK includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/3rdparty/agora/sdk/include)

# AOSL API includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/3rdparty/aosl/include/api)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/3rdparty/aosl/platform/include/hal)

# Application source files
target_sources(app PRIVATE
    src/main.c
    src/network_emac.c
    src/app_agora.c
    src/rtc_agora.c
    src/rtc_agora_audio.c
    src/rtc_agora_stream.c
    src/audio_pdm.c
    src/audio_dac.c
    src/app_audio_route.c
    src/cloud_sentino.c
    src/app_issue.c
    src/app_gateway.c
)
# Network backends beyond EMAC are opt-in: the LTE one drags the AT client
# and PPP in, the Wi-Fi one the prebuilt AIC8800 archive (~1.2 MB flash).
if(CONFIG_LTE_MODEM)
    target_sources(app PRIVATE src/network_lte.c)
endif()
if(CONFIG_WIFI_AIC8800)
    target_sources(app PRIVATE src/network_wifi_aic.c)
endif()
# IPRO6S_FIN over SDIO (IPRO7AI Reef card slot): components/network/ipronet_host.
if(CONFIG_COMPONENTS_IPRONET_HOST_ENABLE)
    target_sources(app PRIVATE src/network_wifi_ipro6s.c)
endif()

# Sentino MQTT platform modules
target_sources(app PRIVATE
    src/sentino/app_sentino.c
    src/sentino/sentino_auth.c
    src/sentino/sentino_protocol.c
    src/sentino/sentino_lifecycle.c
    src/sentino/sentino_property.c
    src/sentino/sentino_ota.c
    src/sentino/sentino_ai.c
    src/sentino/sentino_cmd.c
)

# NFC reader module
target_sources(app PRIVATE
    src/nfc/app_nfc.c
)

# LCD display module (ILI9341 SPI + LVGL + UI)
if(CONFIG_APP_LCD_ENABLE)
    __component_add_include_dirs(./src/lcd)
    __component_add_include_dirs(./src/ui)
    target_sources(app PRIVATE
        src/app_lcd.c
        src/lcd/lcd.c
        src/lcd/font.c
        src/lcd/spi/ili9341_spi.c
        src/lcd/lcd_test.c
        src/ui/ui_screen.c
        src/ui/ui_monk.c
        src/ui/monk_img.c
        src/fonts/lv_font_buddhist_16.c
    )
endif()

# BLE provisioning module
if(CONFIG_BLUETOOTH_LE_ENABLE)
    target_sources(app PRIVATE
        src/ble/app_ble_prov.c
    )
endif()

# Override mbedTLS config globally to use minimal TLS 1.2 client config.
# Replace the default full config on __component_lib INTERFACE to reduce
# mbedTLS flash footprint from ~220KB to ~80-100KB.
get_target_property(_iface_defs __component_lib INTERFACE_COMPILE_DEFINITIONS)
if(_iface_defs)
    list(FILTER _iface_defs EXCLUDE REGEX "MBEDTLS_CONFIG_FILE")
    set_target_properties(__component_lib PROPERTIES INTERFACE_COMPILE_DEFINITIONS "${_iface_defs}")
endif()
target_compile_definitions(__component_lib INTERFACE
    -DMBEDTLS_CONFIG_FILE="mbedtls_agora_config.h"
)
# Add app directory to include path so mbedtls sources can find our config header
if(TARGET mbedtls)
    target_include_directories(mbedtls PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# lwIP MQTT client (not compiled by default in SDK lwIP component)
target_sources(app PRIVATE
    ${CMAKE_SOURCE_DIR}/components/network/lwip/lwip/src/apps/mqtt/mqtt.c
)
# MQTT buffer sizes increased in lwipopts.h: MQTT_VAR_HEADER_BUFFER_LEN=512, MQTT_OUTPUT_RINGBUF_SIZE=1024

# Compiler options - Aggressive size optimization
target_compile_options(app PRIVATE 
    -Werror 
    -Os                      # Optimize for size
    -ffunction-sections      # Each function in separate section
    -fdata-sections          # Each data in separate section
)

# Linker options - Keep shell command sections
target_link_options(app PRIVATE
    -Wl,--gc-sections        # Garbage collect unused sections
)

# Set main file and project setup
__set_main_file(src/main.c)
setup_project(ipro_agora_sentino)

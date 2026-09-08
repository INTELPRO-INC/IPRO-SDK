# IPRO PDM DMIC Recording Demo
# PDM mic → PSRAM buffer → BLE NUS transfer to phone

__component_add_include_dirs(.)
__component_add_include_dirs(./src)

# IPRO Log System includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)

# BLE stack includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_ble_controller/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_ble_controller/plf/ipro7/lowpower)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/port/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/upstream/zephyr/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/upstream/zephyr/subsys/bluetooth/host)

# LC3 codec includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/3rdparty/liblc3/include)

# AI / NPU engine (on-device speaker ID) includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/ai)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/ai/src/core/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/ai/src/inst/include)

# Audio output framework (DAC sink) — used by src/dac_test.c
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/audio/include)

# CherryUSB device stack — used by src/uac_mic.c (USB Audio Class mic)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/cherryusb/common)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/cherryusb/core)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/cherryusb/class/audio)

# Application source files
target_sources(app PRIVATE
    src/ble_audio.c
    src/ble_tickless_pds.c
    src/pdm_buttons.c
    src/spk_ai.c
    src/dac_test.c
    src/pdm_export.c
    src/uac_mic.c
    src/usbd_event_common.c

    src/pdm_sleep.c)

target_compile_options(app PRIVATE -Werror -Os)

# Board-dependent pins. Each falls back to the EVB value in the source, so a
# .config that does not mention them keeps the previous behaviour.
foreach(_pin CLK_PIN DATA_PIN SPK_GPIO SPK_UNMUTE BUTTONS)
    if(DEFINED CONFIG_IPRO_PDM_MIC_${_pin})
        target_compile_definitions(app PRIVATE
            CONFIG_IPRO_PDM_MIC_${_pin}=${CONFIG_IPRO_PDM_MIC_${_pin}})
    endif()
endforeach()

if(CONFIG_IPRO_PDM_MIC_AUTO_PDS)
    target_compile_definitions(app PRIVATE CONFIG_IPRO_PDM_MIC_AUTO_PDS=1)
else()
    target_compile_definitions(app PRIVATE CONFIG_IPRO_PDM_MIC_AUTO_PDS=0)
endif()

if(CONFIG_IPRO_PDM_MIC_WFI_ONLY)
    target_compile_definitions(app PRIVATE CONFIG_IPRO_PDM_MIC_WFI_ONLY=1)
else()
    target_compile_definitions(app PRIVATE CONFIG_IPRO_PDM_MIC_WFI_ONLY=0)
endif()

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(src/main.c)
setup_project(ipro_pdm_mic_demo)

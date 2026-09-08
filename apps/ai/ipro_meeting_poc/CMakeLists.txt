# IPRO Meeting POC
# PDM DMIC + NPU Speaker Recognition + BLE NUS streaming
# Model (mbv2.inm) is loaded at boot from the 'mfg' flash partition —
# flash it with the IProCube tool's --ai-model option.

__component_add_include_dirs(.)
__component_add_include_dirs(./src)

# IPRO Log System includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)

# BLE stack includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/btble_inc)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/rwip/api)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/common/api)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/ke/api)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/plf/ipro7/src/arch)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/plf/ipro7/src/arch/compiler)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/plf/ipro7/src/arch/compiler/riscvgcc)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/blestack/src/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/blestack/src/host)

# LC3 codec includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/3rdparty/liblc3/include)

# AI / NPU engine (on-device speaker ID) includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/ai)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/ai/src/core/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/ai/src/inst/include)

# Application source files
# Audio output framework (DAC sink) — used by src/dac_test.c
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/audio/include)

# .incbin needs the C preprocessor run over the .S (for autoconf.h) - the
# toolchain does that for .S but not .s, so keep the capital extension.
if(CONFIG_MEETING_EMBED_MODEL)
    target_sources(app PRIVATE src/model_blob.S)
    # CONFIG_MEETING_EMBED_MODEL_PATH is resolved relative to src/.
    set_property(SOURCE src/model_blob.S APPEND PROPERTY COMPILE_OPTIONS
                 -I${CMAKE_CURRENT_SOURCE_DIR}/src)
endif()

target_sources(app PRIVATE
    src/dac_test.c
    src/ble_audio.c
)

target_compile_options(app PRIVATE -Werror -Os)

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(src/main.c)
setup_project(ipro_meeting_poc)

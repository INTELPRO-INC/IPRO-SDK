__generate_library()

__component_add_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/inc)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/src/drv_mjpeg.c)

# SDH and PSRAM only available on chips that define their base addresses
if(CONFIG_CHIP STREQUAL "IPRO7" OR CONFIG_CHIP STREQUAL "IPRO6S")
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/src/drv_sdh.c)
endif()
if(CONFIG_USE_PSRAM)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/src/drv_psram.c)
endif()

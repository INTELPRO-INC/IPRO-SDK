# Auto-generated for library release
# Original source code has been replaced with pre-compiled library

# Register pre-compiled library with SDK build system
set_property(GLOBAL APPEND PROPERTY SDK_LIBS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libai.a
)

# Export include directories for other components
target_include_directories(__component_lib INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inst/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inst/include/detect
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inst/include/audio
)

# Preserved from the component's original CMakeLists.txt
target_include_directories(__component_lib INTERFACE ${IPRO_SDK_BASE}/bsp/drivers/ipro7_soc/DSP/include)
set_property(GLOBAL APPEND PROPERTY SDK_LIBS ${IPRO_SDK_BASE}/bsp/drivers/ipro7_soc/DSP/lib/GCC/libnmsis_dsp_${IPRO_MARCH}${IPRO_MARCH_EXT_DSP_LIB}.a)

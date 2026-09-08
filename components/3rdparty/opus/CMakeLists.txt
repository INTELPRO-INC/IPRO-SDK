__generate_library(1)

__component_add_include_dirs(src include)
__component_add_include_dirs(celt)
__component_add_private_include_dirs(silk silk/fixed)

__component_add_private_compile_definitions(-DHAVE_CONFIG_H)

# Aggressive size optimization
__component_add_private_compile_options(-Os)  # Optimize for size
__component_add_private_compile_options(-Wno-incompatible-pointer-types)
__component_add_private_compile_options(-ffunction-sections)  # Enable function sections
__component_add_private_compile_options(-fdata-sections)      # Enable data sections

# Core OPUS sources (fixed-point only, no multistream/projection/demo)
file(GLOB OPUS_CELT_FILES ${CMAKE_CURRENT_SOURCE_DIR}/celt/*.c)
file(GLOB OPUS_SILK_FILES ${CMAKE_CURRENT_SOURCE_DIR}/silk/*.c)
file(GLOB OPUS_SILK_FIXED_FILES ${CMAKE_CURRENT_SOURCE_DIR}/silk/fixed/*.c)

set(OPUS_CORE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opus.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opus_encoder.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opus_decoder.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/repacketizer.c
)

__component_add_sources(
    ${OPUS_CELT_FILES}
    ${OPUS_SILK_FILES}
    ${OPUS_SILK_FIXED_FILES}
    ${OPUS_CORE_FILES}
)

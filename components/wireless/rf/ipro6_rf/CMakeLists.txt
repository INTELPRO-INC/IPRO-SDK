# Auto-generated for library release
# Original source code has been replaced with pre-compiled library

# Register pre-compiled library with SDK build system
set_property(GLOBAL APPEND PROPERTY SDK_LIBS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libipro6_rf.a
)

# Export include directories for other components
target_include_directories(__component_lib INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/phy
    ${CMAKE_CURRENT_SOURCE_DIR}/rfc
    ${CMAKE_CURRENT_SOURCE_DIR}/crm
    ${CMAKE_CURRENT_SOURCE_DIR}/arch
    ${CMAKE_CURRENT_SOURCE_DIR}/reg
    ${CMAKE_CURRENT_SOURCE_DIR}/reg/mac154
    ${CMAKE_CURRENT_SOURCE_DIR}/reg/rf
    ${CMAKE_CURRENT_SOURCE_DIR}/reg/phybz
    ${CMAKE_CURRENT_SOURCE_DIR}/reg/wlan11
    ${CMAKE_CURRENT_SOURCE_DIR}/rf/fpga
    ${CMAKE_CURRENT_SOURCE_DIR}/rf/asic/inc
)

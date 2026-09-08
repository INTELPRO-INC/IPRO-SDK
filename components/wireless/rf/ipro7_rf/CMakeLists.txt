# Auto-generated for library release
# Original source code has been replaced with pre-compiled library

# Register pre-compiled library with SDK build system
set_property(GLOBAL APPEND PROPERTY SDK_LIBS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libipro7_rf.a
)

# Export include directories for other components
target_include_directories(__component_lib INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/api
    ${CMAKE_CURRENT_SOURCE_DIR}/reg
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

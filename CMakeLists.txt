cmake_minimum_required(VERSION 3.15)

find_package(base_sdk REQUIRED HINTS $ENV{IPRO_SDK_BASE})

project(${CONFIG_BUILD_PORJECT} LANGUAGES C CXX ASM)
ENABLE_LANGUAGE(C CXX ASM)
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-Os)
## Auto-enable component dependencies for app-level CONFIG options
# PIRCAM_TOOL_UART requires EVFS and CherryUSB CDC
if(CONFIG_PIRCAM_TOOL_UART)
    set(CONFIG_EVFS 1)
    set(CONFIG_CHERRYUSB_DEVICE_CDC 1)
endif()

add_subdirectory(bsp)
add_subdirectory(components)

# Support for nested app directories (e.g., apps/bluetooth/ble/ipro_ble_remote)
# CONFIG_APP_CATEGORY can be set to specify subdirectory (e.g., "bluetooth")
if(DEFINED CONFIG_APP_CATEGORY AND NOT CONFIG_APP_CATEGORY STREQUAL "")
    set(APP_FULL_PATH "${CMAKE_CURRENT_LIST_DIR}/apps/${CONFIG_APP_CATEGORY}/${CONFIG_BUILD_PORJECT}")
else()
    set(APP_FULL_PATH "${CMAKE_CURRENT_LIST_DIR}/apps/${CONFIG_BUILD_PORJECT}")
endif()

__component_add_subdirectory_ifdef(CONFIG_APPS ${APP_FULL_PATH})
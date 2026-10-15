cmake_minimum_required(VERSION 3.15)

find_package(base_sdk REQUIRED HINTS $ENV{IPRO_SDK_BASE})

project(${CONFIG_BUILD_PORJECT} LANGUAGES C CXX ASM)
ENABLE_LANGUAGE(C CXX ASM)
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-Os)

# Resolve optional Matter packages before wireless/crypto export legacy headers.
if(CONFIG_MATTER_ENABLE AND CONFIG_MATTER_APP_VARIANT STREQUAL "wifi-lighting-app")
    include("${CMAKE_CURRENT_LIST_DIR}/components/matter/source_wifi_light.cmake")
    ipro_matter_load_wifi_light()
elseif(CONFIG_MATTER_ENABLE)
    # Matter is always consumed as an ipro_matter_upstream package. Both
    # inputs may come from the environment so a plain `make -C apps/matter/...`
    # works: MATTER_PACKAGE_DIR (package directory) and
    # MATTER_ALLOW_DEVELOPMENT_PACKAGE=ON (explicit opt-in the importer requires).
    if(NOT DEFINED MATTER_PACKAGE_DIR AND DEFINED ENV{MATTER_PACKAGE_DIR})
        set(MATTER_PACKAGE_DIR "$ENV{MATTER_PACKAGE_DIR}" CACHE PATH "Matter package from ipro_matter_upstream")
    endif()
    if(NOT DEFINED MATTER_ALLOW_DEVELOPMENT_PACKAGE AND DEFINED ENV{MATTER_ALLOW_DEVELOPMENT_PACKAGE})
        set(MATTER_ALLOW_DEVELOPMENT_PACKAGE "$ENV{MATTER_ALLOW_DEVELOPMENT_PACKAGE}" CACHE BOOL "Explicit development-package opt-in")
    endif()
    # No explicit package: use the bundled one from the packages submodule.
    include("${CMAKE_CURRENT_LIST_DIR}/components/matter/packages.cmake")
    ipro_matter_resolve_package_dir()
    if(DEFINED MATTER_PACKAGE_DIR AND NOT MATTER_PACKAGE_DIR STREQUAL "")
        if(CONFIG_MATTER_APP_VARIANT STREQUAL "thread-br-app")
            include("${CMAKE_CURRENT_LIST_DIR}/components/matter/package_br.cmake")
            ipro_matter_load_br_package()
        else()
            include("${CMAKE_CURRENT_LIST_DIR}/components/matter/package.cmake")
            ipro_matter_load_package()
        endif()
    endif()
endif()
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

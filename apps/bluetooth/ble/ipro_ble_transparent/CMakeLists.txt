# IPRO BLE Transparent Module
# UART-BLE bridge with AT command support
# Transport: UART (default) or USB CDC ACM

# Add project include directories
__component_add_include_dirs(.)
__component_add_include_dirs(include)
__component_add_include_dirs(src)

# Add new Bluetooth stack includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/include)

# Add BLE controller includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/btble_inc)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/rwip/api)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/common/api)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/ke/api)

# Add platform/compiler includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/plf/ipro7/src/arch)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/plf/ipro7/src/arch/compiler)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/plf/ipro7/src/arch/compiler/riscvgcc)

# Add host stack includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/blestack/src/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/blestack/src/host)

# Add IPRO Log System includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)

# Add VFS includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/fs/vfs/include)

# Core source files (always compiled)
target_sources(app PRIVATE
    src/ble_transparent.c
    src/at_command.c
    src/uart_bridge.c
    src/cli_commands.c
)

# CDC transport: add USB event handler source
if(CONFIG_AT_TRANSPORT_CDC)
    target_sources(app PRIVATE
        src/usbd_event_common.c
    )
    target_compile_definitions(app PRIVATE CONFIG_AT_TRANSPORT_CDC)
else()
    # Default: UART transport
    target_compile_definitions(app PRIVATE CONFIG_AT_TRANSPORT_UART)
endif()

# Set compiler options
target_compile_options(app PRIVATE -Werror)

# Set main file and project setup
set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(main.c)
setup_project(ipro_ble_transparent)

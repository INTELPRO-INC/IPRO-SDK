# BLE HID Keyboard Application
# Uses Zephyr blestack host (pure BLE stack, no ipro_ble_lib)

__component_add_include_dirs(.)
__component_add_include_dirs(./include)

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
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/blestack/src/services)

# Add IPRO Log System includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)

# Application sources
# Note: main.c is NOT added to app library - it's only compiled for the ELF target
# via __set_main_file() to avoid duplicate symbol errors with --whole-archive
target_sources(app PRIVATE
    hid_keyboard_service.c
)

# Set main file and project setup
set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
# Force whole-archive linking for app to include shell commands
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(main.c)
setup_project(ipro_ble_hid_keyboard)

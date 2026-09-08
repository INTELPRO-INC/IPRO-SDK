# IPRO BLE Remote Control Application
# Demonstrates interactive BLE communication with mobile phones

# Add project include directories
__component_add_include_dirs(.)
__component_add_include_dirs(include)
__component_add_include_dirs(src/data)

# Add Bluetooth public API includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/include)

# New controller + Zephyr 4.4 host port. Keep app code on public Zephyr and
# controller component headers.
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_ble_controller/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/port/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/upstream/zephyr/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/upstream/zephyr/subsys/bluetooth/host)

# Add IPRO Log System includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)

# Add VFS includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/fs/vfs/include)

# Add application source files
# Note: main.c is NOT added to app library - it's only compiled for the ELF target
# via __set_main_file() to avoid duplicate symbol errors with --whole-archive
target_sources(app PRIVATE
    src/app/remote_app.c
    src/services/remote_services.c
    src/services/pir_image_service.c
    src/services/image_upload_service.c
)

if(CONFIG_SHELL)
    target_sources(app PRIVATE src/app/remote_cli.c)
endif()

if(CONFIG_IPRO_BLE_REMOTE_OTA)
    target_sources(app PRIVATE src/services/ota_service.c)
endif()

# Set compiler options
target_compile_options(app PRIVATE -Werror)

# Set main file and project setup
set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
# Force whole-archive linking for app to include shell commands
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(main.c)
setup_project(ipro_ble_remote)

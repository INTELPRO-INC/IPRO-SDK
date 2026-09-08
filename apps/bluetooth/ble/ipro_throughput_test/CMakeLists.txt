# IPRO BLE Throughput Test Application
# Demonstrates BLE throughput testing between two IPRO7 boards (master/slave)

__component_add_include_dirs(.)
__component_add_include_dirs(./include)

# Add Zephyr blestack includes (native bt_* APIs)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/blestack/src/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/blestack/src/host)

# Add BLE controller/platform includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/include)

# Add IPRO Log System includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)

# Add VFS includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/fs/vfs/include)

# Add application source files
# Note: main.c is NOT added to app library - it's only compiled for the ELF target
# via __set_main_file() to avoid duplicate symbol errors with --whole-archive
target_sources(app PRIVATE
    throughput_app.c
    throughput_cli.c
    throughput_service.c
    throughput_client.c
)

# Throughput optimization: TX buffer counts are configured in .config:
# CONFIG_BT_L2CAP_TX_BUF_COUNT=10  - host L2CAP TX buffers
# CONFIG_BT_CONN_TX_MAX=10         - host connection TX max
# Controller-side optimizations (BLE_NB_TX_DESC_PER_CON=4) are baked into the
# prebuilt link2 library. Rebuild with: build_all_prebuilts.sh link2

# Set compiler options
target_compile_options(app PRIVATE -Werror)

# Set main file and project setup
set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
# Force whole-archive linking for app to include shell commands
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(main.c)
setup_project(ipro_throughput_test)

set(AIC_FEATURE_BT ON CACHE BOOL "D80 SDIO HCI" FORCE)
set(AIC_FEATURE_P2P OFF CACHE BOOL "P2P disabled" FORCE)
set(AIC_CHIP_MODEL D80 CACHE STRING "AIC8800 D80" FORCE)
set(AIC_USE_PREBUILT_HOST ON CACHE STRING "Customer prebuilt release" FORCE)
set(CONFIG_DHCPD ON CACHE BOOL "DHCP server" FORCE)
add_subdirectory(${IPRO_SDK_BASE}/aic8800-20260907/aic8800_release_d80_20261002
                 ${CMAKE_CURRENT_BINARY_DIR}/aic8800)
__component_add_include_dirs(.)
target_compile_definitions(app PRIVATE AIC_RELEASE_TEST)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main.c
                           ${CMAKE_CURRENT_SOURCE_DIR}/hci_release_test.c)
target_include_directories(app PRIVATE
    ${IPRO_SDK_BASE}/aic8800-20260907/aic8800_release_d80_20261002/include)
set_property(GLOBAL APPEND PROPERTY SDK_LIBS softutil-aic_wifi)
__set_main_file(main.c)
setup_project(ipro7_aic8800_wifi)
target_compile_definitions(ipro7_aic8800_wifi_IPRO7.elf PRIVATE AIC_RELEASE_TEST)

# IPRO7 RF driver (IPRO7 chip only)
if("${CONFIG_CHIP}" STREQUAL "IPRO7")
    add_subdirectory(ipro7_rf)
endif()

# IPRO6 RF drivers
if("${CONFIG_CHIP}" STREQUAL "IPRO6")
    if(CONFIG_BTDM_ENABLE AND CONFIG_FPGA)
        # BTDM FPGA: use ipro6_phyrf_fpga (T2/AD9361/GPM RF boards)
    else()
        # WiFi6/BLE: use ipro6_rf (comprehensive RF driver)
        add_subdirectory(ipro6_rf)
    endif()
endif()

if (CONFIG_FPGA)
__component_add_compile_definitions(-DCFG_FPGA)
endif()

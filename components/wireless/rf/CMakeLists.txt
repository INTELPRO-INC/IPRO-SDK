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
    endif()
endif()

# Optional: use prebuilt archive instead of sources
if(CONFIG_RF_PREBUILT)
    set(CURRENT_STATIC_LIBRARY rf)
    add_library(${CURRENT_STATIC_LIBRARY} STATIC IMPORTED GLOBAL)
    set_target_properties(${CURRENT_STATIC_LIBRARY} PROPERTIES IMPORTED_LOCATION ${CONFIG_RF_PREBUILT_PATH})
    set_property(GLOBAL APPEND PROPERTY SDK_LIBS ${CURRENT_STATIC_LIBRARY})
    target_link_libraries(${CURRENT_STATIC_LIBRARY} PUBLIC __component_lib)
    message(STATUS "RF: using prebuilt archive at ${CONFIG_RF_PREBUILT_PATH}")
    return()
endif()

if (CONFIG_FPGA)
__component_add_compile_definitions(-DCFG_FPGA)
endif()

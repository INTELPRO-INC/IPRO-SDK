add_subdirectory(board)
add_subdirectory(common)
add_subdirectory(hal)
add_subdirectory(drivers/soc_common)

string(TOLOWER "${CONFIG_CHIP}" CONFIG_CHIP_LOWER)

# Each chip uses its own SoC driver directory
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/drivers/${CONFIG_CHIP}_soc")
    set(SOC_DIR "${CONFIG_CHIP}_soc")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/drivers/${CONFIG_CHIP_LOWER}_soc")
    set(SOC_DIR "${CONFIG_CHIP_LOWER}_soc")
else()
    message(FATAL_ERROR "'${CONFIG_CHIP} or ${CONFIG_CHIP_LOWER}' not found")
endif()

add_subdirectory(drivers/${SOC_DIR})

# IPRO6 WiFi6 support: phyrf driver now in components/wireless/rf/ipro6_rf

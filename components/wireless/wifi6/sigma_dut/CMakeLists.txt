if(CONFIG_SIGMA_DUT)
    __generate_library()

    __component_add_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/include)
    __component_add_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/../../macsw/include/export)

    __component_add_sources(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sigma_dut_server.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sigma_dut_parser.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sigma_dut_cmd_device.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sigma_dut_cmd_sta.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sigma_dut_cmd_he.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sigma_dut_cmd_traffic.c
    )

    if(CONFIG_SHELL)
        __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/src/sigma_dut_cli.c)
        __component_add_link_libraries(-u__fsym_sigma_dut)
    endif()
endif()

cmake_minimum_required(VERSION 3.15)
add_library(softutil-aic_wifi STATIC IMPORTED GLOBAL)
set_target_properties(softutil-aic_wifi PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/libsoftutil-aic_wifi.a)
target_include_directories(softutil-aic_wifi INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/include/drv_api/inc
    ${CMAKE_CURRENT_LIST_DIR}/include/drv_wifi
    ${CMAKE_CURRENT_LIST_DIR}/include/utils/inc
    ${CMAKE_CURRENT_LIST_DIR}/port/inc)
target_link_libraries(softutil-aic_wifi INTERFACE __component_lib)

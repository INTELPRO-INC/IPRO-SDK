__component_add_include_dirs(.)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/imu/include)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main.c)

__set_main_file(main.c)
setup_project(mpu6500_test)

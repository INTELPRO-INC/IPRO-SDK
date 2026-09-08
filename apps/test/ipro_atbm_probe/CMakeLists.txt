__component_add_include_dirs(.)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main.c)

__set_main_file(main.c)
setup_project(ipro_atbm_probe)

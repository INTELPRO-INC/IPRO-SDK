__component_add_include_dirs(.)
__component_add_include_dirs(./src)

target_compile_options(app PRIVATE -Werror)

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
__set_main_file(main.c)
setup_project(ipro_tinymaix_demo)

__component_add_include_dirs(.)

target_compile_options(app PRIVATE -Werror)

__set_main_file(main.c)
setup_project(ipro7_uart_rcp)

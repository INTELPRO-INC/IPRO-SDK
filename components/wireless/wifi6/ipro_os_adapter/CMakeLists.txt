__generate_library(1)

__component_add_sources(src/rtos_al.c)
__component_add_sources_ifdef(CONFIG_IPRO_EVENT src/platform_iotsdk.c)

__component_add_include_dirs(include)

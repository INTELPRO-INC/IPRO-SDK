__generate_library()

# Expose updated getopt headers globally for other components
__component_add_include_dirs(.)

__component_add_sources(utils_getopt.c)
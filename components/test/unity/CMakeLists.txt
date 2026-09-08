__generate_library()

# Export public headers
__component_add_include_dirs(include)

# Add sources
__component_add_sources(src/unity.c)
__component_add_sources(src/test_framework.c)

__generate_library()

__component_add_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/include)

__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/iperf.c)
# iperf_cli.c supports both the FinSH shell (CONFIG_SHELL) and the legacy CLI;
# it picks the backend itself, so build it whenever the component is enabled.
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/iperf_cli.c)

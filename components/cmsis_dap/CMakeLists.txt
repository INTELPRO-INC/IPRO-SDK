__generate_library()

__component_add_include_dirs(inc)
__component_add_sources(src/DAP.c)
__component_add_sources(src/DAP_vendor.c)

# Suppress unused-variable warning in upstream ARM CMSIS-DAP DAP.c (DAP_SWJ_Clock
# declares 'delay' but only uses it indirectly via Set_Clock_Delay).  This is a
# cosmetic issue in the vendored code; do not modify upstream sources.
target_compile_options(cmsis_dap PRIVATE -Wno-unused-variable)

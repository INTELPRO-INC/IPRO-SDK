__component_add_include_dirs(.)
__component_add_include_dirs(./src)

# components/ai exposes a FLAT riscv_nn_types.h whose nmsis_nn_conv_params
# differs from upstream's. It reaches bench_u8.c through the global
# __component_lib INTERFACE path; bench_s8.c must only ever see the prefixed
# "nmsis_nn/..." copy. Keeping them in separate TUs is what makes this legal.
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/ai)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/ai/src/inst/include)

target_sources(app PRIVATE
    src/bench_s8.c
    src/bench_u8.c
    src/nn_bench_arena.c
    src/cpuclk.c
)

target_compile_options(app PRIVATE -Werror)

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
__set_main_file(main.c)
setup_project(ipro_nmsis_nn_test)

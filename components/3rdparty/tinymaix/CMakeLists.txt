# TinyMaix - Lightweight Neural Network Inference
# https://github.com/sipeed/TinyMaix
# License: Apache 2.0

__generate_library(1)

__component_add_include_dirs(port include src)

__component_add_private_compile_options(-Os)
__component_add_private_compile_options(-ffunction-sections)
__component_add_private_compile_options(-fdata-sections)
__component_add_private_compile_options(-Wno-unused-parameter)

__component_add_sources(
    src/tm_model.c
    src/tm_layers.c
    src/tm_stat.c
)

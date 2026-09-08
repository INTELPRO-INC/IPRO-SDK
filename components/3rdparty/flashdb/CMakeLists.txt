# FlashDB - Lightweight key-value and time-series database
# https://github.com/armink/FlashDB
# License: Apache 2.0

__generate_library(1)

__component_add_include_dirs(inc port)

__component_add_private_compile_options(-Os)
__component_add_private_compile_options(-ffunction-sections)
__component_add_private_compile_options(-fdata-sections)
__component_add_private_compile_options(-Wno-unused-parameter)

__component_add_sources(
    src/fdb.c
    src/fdb_kvdb.c
    src/fdb_tsdb.c
    src/fdb_utils.c
)

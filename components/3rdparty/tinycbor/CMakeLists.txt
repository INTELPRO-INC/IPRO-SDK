__generate_library(1)
__component_add_include_dirs(tinycbor/src)

__component_add_sources(
    tinycbor/src/cborencoder.c
    tinycbor/src/cborencoder_close_container_checked.c
    tinycbor/src/cborencoder_float.c
    tinycbor/src/cborerrorstrings.c
    tinycbor/src/cborparser.c
    tinycbor/src/cborparser_dup_string.c
    tinycbor/src/cborparser_float.c
    tinycbor/src/cborpretty.c
    tinycbor/src/cborvalidation.c
)

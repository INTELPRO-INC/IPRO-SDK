__generate_library(1)
__component_add_include_dirs(llhttp/include)

__component_add_sources(
    llhttp/src/llhttp.c
    llhttp/src/api.c
    llhttp/src/http.c
)

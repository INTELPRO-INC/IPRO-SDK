# mbedtls 3.6 LTS (CONFIG_MBEDTLS_V3) for the PSA apps. The app gives
# mbedtls3_config.h on its include path. Our code is CMakeLists.txt,
# port/ and UPSTREAM.md; mbedtls/ is a verbatim upstream subset.
__generate_library()

__component_add_include_dirs(mbedtls/include)
__component_add_private_include_dirs(mbedtls/library)
__component_add_compile_definitions(
    MBEDTLS_CONFIG_FILE=\"mbedtls3_config.h\")

# The crypto library/*.c: no TLS, X.509 or host-only files; the config
# selects what each file compiles to
file(GLOB MBEDTLS3_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls/library/*.c)
list(FILTER MBEDTLS3_SRCS EXCLUDE REGEX
    "/(ssl_[^/]*|x509[^/]*|mps_[^/]*|debug|pkcs7|entropy_poll|net_sockets|timing)\\.c$")
__component_add_sources(${MBEDTLS3_SRCS} port/entropy_hw_poll.c)

# TF-PSA-Crypto 1.2 (CONFIG_TF_PSA_CRYPTO) for the PSA apps. The app
# gives tfpsa_config.h on its include path. Our code is CMakeLists.txt,
# port/ and UPSTREAM.md; tf-psa-crypto/ is a verbatim release subset.
__generate_library()

set(TFPSA ${CMAKE_CURRENT_SOURCE_DIR}/tf-psa-crypto)
__component_add_include_dirs(${TFPSA}/include ${TFPSA}/drivers/builtin/include)
__component_add_private_include_dirs(${TFPSA}/core ${TFPSA}/dispatch
    ${TFPSA}/drivers/builtin/src ${TFPSA}/extras ${TFPSA}/platform
    ${TFPSA}/utilities)
__component_add_compile_definitions(
    TF_PSA_CRYPTO_CONFIG_FILE=\"tfpsa_config.h\")

# Every crypto source but the host file-based ITS; the config selects
# what each file compiles to
file(GLOB TFPSA_SRCS ${TFPSA}/core/*.c ${TFPSA}/drivers/builtin/src/*.c
    ${TFPSA}/extras/*.c ${TFPSA}/platform/*.c ${TFPSA}/utilities/*.c)
list(FILTER TFPSA_SRCS EXCLUDE REGEX "/psa_its_file\\.c$")
# Marker of the upstream test-driver build, tested in an #if
__component_add_private_compile_definitions(
    LIBTESTDRIVER1_TF_PSA_CRYPTO_MARKER=0)
__component_add_sources(${TFPSA_SRCS} port/entropy_get.c)

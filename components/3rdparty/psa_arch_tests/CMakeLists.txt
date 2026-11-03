# Arm psa-arch-tests dev_apis suites for the IPRO7 U-mode NSPE
# (CONFIG_COMPONENTS_PSA_ARCH_TESTS_ENABLE): one suite per build over
# components/psa/client, test list generated at configure time.
__generate_library()

set(PSA_ARCH_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/psa_arch_tests/api-tests)
set(PSA_ARCH_CVAL ${CMAKE_CURRENT_SOURCE_DIR}/psa_arch_tests/val_common)
set(PSA_ARCH_PAL ${PSA_ARCH_ROOT}/platform/targets/common/nspe)
set(PSA_ARCH_PORT ${CMAKE_CURRENT_SOURCE_DIR}/port/nspe)
set(PSA_ARCH_QCBOR ${CMAKE_CURRENT_SOURCE_DIR}/qcbor)
set(PSA_ARCH_GEN ${CMAKE_CURRENT_BINARY_DIR}/psa_arch_gen)

# Suite: macro, test directory, test list, sources and include dirs
if(CONFIG_PSA_ARCH_TEST_SUITE_CRYPTO)
    set(PSA_ARCH_SUITE CRYPTO)
    set(PSA_ARCH_SUITE_DIR ${PSA_ARCH_ROOT}/dev_apis/crypto)
    set(PSA_ARCH_SUITE_SRCS ${PSA_ARCH_SUITE_DIR}/common/test_crypto_common.c)
    set(PSA_ARCH_SUITE_PORT_SRCS ${PSA_ARCH_PORT}/pal_crypto_intf.c)
    set(PSA_ARCH_SUITE_INCS ${PSA_ARCH_SUITE_DIR}/common)
    if(CONFIG_TF_PSA_CRYPTO)
        # The 1.1.0 list with the API 1.2 tests (README.md, Tests)
        set(PSA_ARCH_DB ${CMAKE_CURRENT_SOURCE_DIR}/port/crypto_1.2_tfpsa_testsuite.db)
        set(PSA_ARCH_SUITE_DEFS PSA_ARCH_MBEDTLS_V3 PSA_ARCH_API_1_2
            PSA_API_MODE_ON=0)
    elseif(CONFIG_MBEDTLS_V3)
        # The 1.0.0 list with c019 and multipart AEAD (README.md, Tests)
        set(PSA_ARCH_DB ${CMAKE_CURRENT_SOURCE_DIR}/port/crypto_1.1.0_m3_testsuite.db)
        set(PSA_ARCH_SUITE_DEFS PSA_ARCH_MBEDTLS_V3 PSA_API_MODE_ON=0)
    else()
        # The 1.0.0 list with c019 marked skip (README.md, Tests)
        set(PSA_ARCH_DB ${CMAKE_CURRENT_SOURCE_DIR}/port/crypto_1.0.0_testsuite.db)
        set(PSA_ARCH_SUITE_DEFS)
    endif()
elseif(CONFIG_PSA_ARCH_TEST_SUITE_ITS)
    set(PSA_ARCH_SUITE INTERNAL_TRUSTED_STORAGE)
    set(PSA_ARCH_SUITE_DIR ${PSA_ARCH_ROOT}/dev_apis/storage)
    set(PSA_ARCH_DB ${PSA_ARCH_SUITE_DIR}/its_testsuite.db)
    set(PSA_ARCH_SUITE_SRCS
        ${PSA_ARCH_SUITE_DIR}/common/test_storage_common.c
        ${PSA_ARCH_PAL}/internal_trusted_storage/pal_internal_trusted_storage_intf.c)
    set(PSA_ARCH_SUITE_PORT_SRCS)
    set(PSA_ARCH_SUITE_INCS
        ${PSA_ARCH_SUITE_DIR}/common
        ${PSA_ARCH_PAL}/internal_trusted_storage)
    set(PSA_ARCH_SUITE_DEFS)
elseif(CONFIG_PSA_ARCH_TEST_SUITE_ATTEST)
    set(PSA_ARCH_SUITE INITIAL_ATTESTATION)
    set(PSA_ARCH_SUITE_DIR ${PSA_ARCH_ROOT}/dev_apis/initial_attestation)
    set(PSA_ARCH_DB ${PSA_ARCH_SUITE_DIR}/testsuite.db)
    set(PSA_ARCH_SUITE_SRCS
        ${PSA_ARCH_PAL}/initial_attestation/pal_attestation_intf.c
        ${PSA_ARCH_PAL}/initial_attestation/pal_attestation_crypto.c
        ${PSA_ARCH_QCBOR}/src/UsefulBuf.c
        ${PSA_ARCH_QCBOR}/src/ieee754.c
        ${PSA_ARCH_QCBOR}/src/qcbor_decode.c
        ${PSA_ARCH_QCBOR}/src/qcbor_encode.c)
    set(PSA_ARCH_SUITE_PORT_SRCS ${PSA_ARCH_PORT}/pal_attest_pk.c)
    set(PSA_ARCH_SUITE_INCS
        ${PSA_ARCH_PAL}/initial_attestation
        ${PSA_ARCH_QCBOR}/inc)
    set(PSA_ARCH_SUITE_DEFS PSA_ATTESTATION_PROFILE_1 PLATFORM_HAS_ATTEST_PK)
    if(CONFIG_TF_PSA_CRYPTO)
        # pal_attestation_crypto.c names the key psa_key_handle_t, which
        # TF-PSA-Crypto 1.x no longer declares
        list(APPEND PSA_ARCH_SUITE_DEFS psa_key_handle_t=psa_key_id_t)
    endif()
else()
    message(FATAL_ERROR "psa_arch_tests: no CONFIG_PSA_ARCH_TEST_SUITE_* set")
endif()

# Test range: 0 = no bound ("None" for gen_tests_list.py)
__component_default_def(CONFIG_PSA_ARCH_TEST_RANGE_START 0)
__component_default_def(CONFIG_PSA_ARCH_TEST_RANGE_END 0)
__component_default_def(CONFIG_PSA_ARCH_TEST_VERBOSITY 3)
set(PSA_ARCH_RANGE)
foreach(bound ${CONFIG_PSA_ARCH_TEST_RANGE_START} ${CONFIG_PSA_ARCH_TEST_RANGE_END})
    if(bound EQUAL 0)
        list(APPEND PSA_ARCH_RANGE None)
    else()
        list(APPEND PSA_ARCH_RANGE ${bound})
    endif()
endforeach()

# test_entry_list.inc (val_dispatcher.c) and
# test_entry_fn_declare_list.inc (val_interfaces.h)
string(TOLOWER ${PSA_ARCH_SUITE} PSA_ARCH_SUITE_LOWER)
file(MAKE_DIRECTORY ${PSA_ARCH_GEN})
set(PSA_ARCH_GEN_PY ${PSA_ARCH_ROOT}/tools/scripts/gen_tests_list.py)
execute_process(
    COMMAND /usr/bin/python3 ${PSA_ARCH_GEN_PY}
            ${PSA_ARCH_SUITE_LOWER} ${PSA_ARCH_DB} 0 ALL
            ${PSA_ARCH_GEN}/testlist.txt
            ${PSA_ARCH_GEN}/test_entry_list.inc
            ${PSA_ARCH_GEN}/test_entry_fn_declare_list.inc
            ${PSA_ARCH_GEN}/client_tests_list_declare.inc
            ${PSA_ARCH_GEN}/client_tests_list.inc
            ${PSA_ARCH_GEN}/server_tests_list_declare.inc
            ${PSA_ARCH_GEN}/server_tests_list.inc
            ${PSA_ARCH_RANGE}
    RESULT_VARIABLE PSA_ARCH_GEN_RC
    OUTPUT_QUIET)
if(NOT PSA_ARCH_GEN_RC EQUAL 0)
    message(FATAL_ERROR "psa_arch_tests: gen_tests_list.py failed (${PSA_ARCH_GEN_RC})")
endif()
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    ${PSA_ARCH_DB} ${PSA_ARCH_GEN_PY})
file(STRINGS ${PSA_ARCH_GEN}/testlist.txt PSA_ARCH_TESTS)
if(NOT PSA_ARCH_TESTS)
    message(FATAL_ERROR
        "psa_arch_tests: no ${PSA_ARCH_SUITE} test in range ${PSA_ARCH_RANGE}")
endif()
list(LENGTH PSA_ARCH_TESTS PSA_ARCH_TEST_COUNT)
message(STATUS "psa_arch_tests: ${PSA_ARCH_SUITE}, ${PSA_ARCH_TEST_COUNT} tests")

# Each test.cmake appends its sources to CC_SOURCE
set(PSA_ARCH_TEST_SRCS)
foreach(test ${PSA_ARCH_TESTS})
    set(CC_SOURCE)
    set(CC_OPTIONS)
    set(AS_SOURCE)
    include(${PSA_ARCH_SUITE_DIR}/${test}/test.cmake)
    if(CC_OPTIONS OR AS_SOURCE)
        message(FATAL_ERROR "psa_arch_tests: ${test} needs CC_OPTIONS or AS_SOURCE")
    endif()
    foreach(src ${CC_SOURCE})
        list(APPEND PSA_ARCH_TEST_SRCS ${PSA_ARCH_SUITE_DIR}/${test}/${src})
    endforeach()
    __component_add_private_include_dirs(${PSA_ARCH_SUITE_DIR}/${test})
endforeach()

set(PSA_ARCH_VENDOR_SRCS
    ${PSA_ARCH_ROOT}/val/nspe/val_attestation.c
    ${PSA_ARCH_ROOT}/val/nspe/val_crypto.c
    ${PSA_ARCH_ROOT}/val/nspe/val_dispatcher.c
    ${PSA_ARCH_ROOT}/val/nspe/val_entry.c
    ${PSA_ARCH_ROOT}/val/nspe/val_framework.c
    ${PSA_ARCH_ROOT}/val/nspe/val_interfaces.c
    ${PSA_ARCH_ROOT}/val/nspe/val_peripherals.c
    ${PSA_ARCH_ROOT}/val/nspe/val_platform.c
    ${PSA_ARCH_ROOT}/val/nspe/val_storage.c
    ${PSA_ARCH_CVAL}/src/val_common_framework.c
    ${PSA_ARCH_CVAL}/src/val_common_log.c
    ${PSA_ARCH_CVAL}/src/val_common_peripherals.c
    ${PSA_ARCH_CVAL}/src/val_common_status.c
    ${PSA_ARCH_PAL}/pal_weak.c
    ${PSA_ARCH_SUITE_SRCS}
    ${PSA_ARCH_TEST_SRCS})

set_source_files_properties(${PSA_ARCH_VENDOR_SRCS}
    PROPERTIES COMPILE_OPTIONS -Wno-unused-variable)

__component_add_sources(
    ${PSA_ARCH_VENDOR_SRCS}
    ${PSA_ARCH_SUITE_PORT_SRCS}
    ${PSA_ARCH_PORT}/pal_driver_intf.c)

# Exported: val_entry.h and pal_common.h (pal_nvmem_reset) for the app
__component_add_include_dirs(
    ${PSA_ARCH_GEN}
    ${PSA_ARCH_ROOT}/val/common
    ${PSA_ARCH_ROOT}/val/nspe
    ${PSA_ARCH_CVAL}/inc
    ${PSA_ARCH_PAL}
    ${PSA_ARCH_PAL}/crypto
    ${PSA_ARCH_PORT})
__component_add_compile_definitions(
    ARCH_TEST_SUITE_NAME=\"${PSA_ARCH_SUITE}\")

__component_add_private_include_dirs(
    ${PSA_ARCH_ROOT}/platform/drivers/nvmem
    ${PSA_ARCH_SUITE_INCS})

__component_add_private_compile_definitions(
    VAL_NSPE_BUILD
    NONSECURE_TEST_BUILD
    PSA_CMAKE_BUILD
    VERBOSITY=${CONFIG_PSA_ARCH_TEST_VERBOSITY}
    PLATFORM_PSA_ISOLATION_LEVEL=2
    STATELESS_ROT=0
    STATIC_ASSERT_CHECKS
    ${PSA_ARCH_SUITE}
    ${PSA_ARCH_SUITE_DEFS})

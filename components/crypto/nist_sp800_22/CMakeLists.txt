# SPDX-FileCopyrightText: 2025 IPRO Technology Co., Ltd.
# SPDX-License-Identifier: Apache-2.0
#
# NIST SP 800-22 Statistical Test Suite for Random Number Generators
#
# This library provides all 15 statistical tests defined in NIST SP 800-22 Rev 1a.
# It is hardware-independent and can be used with any TRNG by providing a HAL function.

__generate_library()

__component_add_include_dirs(include)

# Export test directory path for apps to use
# Test files are not compiled into the library (they depend on app-level shell/HAL)
set(NIST_SP800_22_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test CACHE PATH "NIST SP 800-22 test directory")

__component_add_sources(
    nist_sp800_22.c
)

# Link math library for sqrt, log, exp, etc.
target_link_libraries(nist_sp800_22 PRIVATE m)

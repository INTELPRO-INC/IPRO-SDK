# IPRO SDK Unit Test Project
# Comprehensive SDK validation framework

message(STATUS "========================================")
message(STATUS "Building IPRO Unit Test Project")
message(STATUS "========================================")

__component_add_include_dirs(.)
__component_add_include_dirs(./include)
__component_add_include_dirs(./tests)

# BLE stack include directories (needed for bluetooth/bluetooth.h)
if (CONFIG_BLUETOOTH_LE_ENABLE)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/include)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/blestack/src/include)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/blestack/src/include/bluetooth)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/blestack/src/host)
endif()

# Compiler options
target_compile_options(app PRIVATE -Wall)
target_compile_options(app PRIVATE -Wextra)


# Main application sources
target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test_runner.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freertos_hooks.c
)

# =============================================================================
# Unit Test Framework
# =============================================================================
if (CONFIG_UNIT_TEST_FRAMEWORK)
    # Unity provided as component library (components/test/unity)
endif()

# =============================================================================
# Software Tests (TEST_GROUP_SW)
# =============================================================================

if (CONFIG_TEST_UTILS)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_utils.c)
endif()

if (CONFIG_TEST_MEMORY)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_memory.c)
endif()

if (CONFIG_TEST_RTOS)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_rtos.c)
endif()

if (CONFIG_TEST_RINGBUF)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_ringbuf.c)
endif()

if (CONFIG_TEST_RBTREE)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_rbtree.c)
endif()

if (CONFIG_TEST_SOFTCRC)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_softcrc.c)
endif()

if (CONFIG_TEST_GETOPT)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_getopt.c)
endif()

if (CONFIG_TEST_EVENT)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_event.c)
endif()

if (CONFIG_TEST_BASE64)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_base64.c)
endif()

if (CONFIG_TEST_LIST)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_list.c)
endif()

if (CONFIG_TEST_HEX)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_hex.c)
endif()

if (CONFIG_TEST_SHA256)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_sha256.c)
endif()

if (CONFIG_TEST_RTOS_ADV)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_rtos_adv.c)
endif()

if (CONFIG_TEST_MEMP)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_memp.c)
    target_sources(app PRIVATE ${CMAKE_SOURCE_DIR}/components/utils/src/utils_memp.c)
endif()

if (CONFIG_TEST_MD5)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_md5.c)
endif()

if (CONFIG_TEST_SHA1)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_sha1.c)
endif()

if (CONFIG_TEST_HMAC)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_hmac.c)
endif()

if (CONFIG_TEST_DNS)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_dns.c)
    target_sources(app PRIVATE ${CMAKE_SOURCE_DIR}/components/utils/src/utils_dns.c)
endif()

if (CONFIG_TEST_NOTIFIER)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_notifier.c)
    target_sources(app PRIVATE ${CMAKE_SOURCE_DIR}/components/utils/src/utils_notifier.c)
endif()

if (CONFIG_TEST_BITMAP)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_bitmap.c)
    target_sources(app PRIVATE ${CMAKE_SOURCE_DIR}/components/utils/src/utils_bitmap_window.c)
endif()

# =============================================================================
# HAL Hardware Tests (TEST_GROUP_HAL)
# =============================================================================

if (CONFIG_TEST_HAL_GPIO)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_hal_gpio.c)
endif()

if (CONFIG_TEST_HAL_TIMER)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_hal_timer.c)
endif()

if (CONFIG_TEST_HAL_FLASH)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_hal_flash.c)
endif()

if (CONFIG_TEST_HAL_DMA)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_hal_dma.c)
endif()

if (CONFIG_TEST_HAL_WDT)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_hal_wdt.c)
endif()

if (CONFIG_TEST_HAL_RTC)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_hal_rtc.c)
endif()

if (CONFIG_TEST_HAL_ADC)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_hal_adc.c)
endif()

# =============================================================================
# HW Crypto Engine Tests (TEST_GROUP_CRYPTO)
# =============================================================================

if (CONFIG_TEST_SEC_AES)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_sec_aes.c)
endif()

if (CONFIG_TEST_SEC_HASH)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_sec_hash.c)
endif()

if (CONFIG_TEST_SEC_TRNG)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_sec_trng.c)
endif()

if (CONFIG_TEST_SEC_GMAC)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_sec_gmac.c)
endif()

# =============================================================================
# BLE Tests (TEST_GROUP_BLE)
# =============================================================================

if (CONFIG_TEST_BLE)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_ble_core.c)
endif()

# =============================================================================
# Legacy test entries (sources don't exist, kept for reference)
# =============================================================================
if (CONFIG_TEST_CRYPTO)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_crypto.c)
endif()
if (CONFIG_TEST_NETWORK)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_network.c)
endif()
if (CONFIG_TEST_DRIVERS)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_drivers.c)
endif()
if (CONFIG_TEST_CUSTOM)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_custom.c)
endif()

message(STATUS "Unit Test configuration complete")

# =============================================================================
# Setup Final Executable
# =============================================================================
__set_main_file(main.c)
setup_project(ipro_unit_test)

# Linker: allow multiple definitions (shell and platform_utils both provide ring_buffer)
target_link_options(ipro_unit_test_${CONFIG_CHIP}.elf PRIVATE -Wl,--allow-multiple-definition)

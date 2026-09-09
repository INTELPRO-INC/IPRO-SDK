#======================================================================
# Copyright (c) 2026 IntelPro Technology Co., Ltd.
# All rights reserved.
#
# This software and associated documentation files (the "Software") are
# proprietary and confidential to IntelPro Technology Co., Ltd..
#
# Access to and use of this Software is limited exclusively to authorized 
# customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
# valid license or non-disclosure agreement.
#
# Unauthorized copying, modification, distribution, or reverse engineering 
# of this Software, via any medium, is strictly prohibited without 
# prior written permission from IntelPro Technology Co., Ltd..
#
# SPDX-License-Identifier: Proprietary
#======================================================================

# BTDM Stack - Main CMakeLists.txt
# =================================
#
# This is the main entry point for the BTDM (Bluetooth Dual Mode) stack build system.
# Implements the BLE controller and, when CONFIG_BTDM_BT_ENABLE is selected,
# the BR/EDR controller from the same RW v13.0.18 release.
#
# Stack Structure (using unmodified CEVA code):
# ---------------------------------------------
# 1. ip/         - CEVA original IP code (hci, ll/ble, ll/sch)
# 2. modules/    - CEVA original modules (ke, common, rwip, security)
# 3. port/ipro6s/ - IPRO6S OS and hardware abstraction
# 4. plf/ipro6s/  - IPRO6S platform code (arch, registers, drivers)
#
# Usage:
#   - Controller-only mode: Enable CONFIG_BTDM_ENABLE
#   - BLE only: Enable CONFIG_BTDM_BLE_ENABLE

# Only build if BTDM is enabled
if(NOT CONFIG_BTDM_ENABLE)
    return()
endif()

message(STATUS "[ipro_ble_controller] Building IPRO6S RW v13.0.18 / BT 5.4 backend")

set(IPRO_BLE_CTLR_IPRO6S_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(IPRO_BLE_CTLR_IPRO6S_UPSTREAM ${IPRO_BLE_CTLR_IPRO6S_ROOT}/upstream/ipro6s)
set(IPRO_BLE_CTLR_IPRO6S_PLF ${IPRO_BLE_CTLR_IPRO6S_ROOT}/plf/ipro6s)
set(IPRO_BLE_CTLR_IPRO6S_PORT ${IPRO_BLE_CTLR_IPRO6S_ROOT}/port/ipro6s)
set(IPRO_BLE_CTLR_IPRO6S_COMPAT ${IPRO_BLE_CTLR_IPRO6S_ROOT}/compat/ipro6s)

# ---------------------------------------------------------------------------
# Archive identity
#
# The archive name has to encode everything that changes its ABI or its
# resource footprint, because a customer picks one prebuilt off the shelf and
# links it: connection count (CFG_CON sizes the environment arrays), whether
# BR/EDR is compiled in at all, whether the periodic-advertising family is
# present, and whether CFG_DBG is on. Two builds that differ in any of these
# are not interchangeable, so they must not share a file name.
#
# This mirrors the IPRO7 naming in ../CMakeLists.txt (linkN + feature suffixes
# + _release) so one release config can describe both platforms, but the
# dimensions are IPRO6S's own: IPRO7 has no BR/EDR and carries its 5.4 feature
# set in bt52/bt54_iso2 instead.
# ---------------------------------------------------------------------------
if(DEFINED CONFIG_BTDM_MAX_ACL_CONNECTIONS)
    set(IPRO_BLE_CTLR_IPRO6S_CON ${CONFIG_BTDM_MAX_ACL_CONNECTIONS})
else()
    set(IPRO_BLE_CTLR_IPRO6S_CON 4)   # matches the Kconfig default
endif()
set(IPRO_BLE_CTLR_IPRO6S_LIBRARY_NAME
    "ipro_ble_controller_ipro6s_link${IPRO_BLE_CTLR_IPRO6S_CON}")
if(CONFIG_BTDM_BT_ENABLE)
    string(APPEND IPRO_BLE_CTLR_IPRO6S_LIBRARY_NAME "_br")
endif()
if(CONFIG_BTDM_PAWR)
    string(APPEND IPRO_BLE_CTLR_IPRO6S_LIBRARY_NAME "_pawr")
elseif(CONFIG_BTDM_PER_ADV OR CONFIG_BTDM_PAST)
    string(APPEND IPRO_BLE_CTLR_IPRO6S_LIBRARY_NAME "_peradv")
endif()
if(NOT CONFIG_BTDM_DEBUG)
    string(APPEND IPRO_BLE_CTLR_IPRO6S_LIBRARY_NAME "_release")
endif()
set(IPRO_BLE_CTLR_IPRO6S_PREBUILT
    "${IPRO_BLE_CTLR_IPRO6S_ROOT}/lib/lib${IPRO_BLE_CTLR_IPRO6S_LIBRARY_NAME}.a")

# Generate library
__generate_library(0 ${IPRO_BLE_CTLR_IPRO6S_LIBRARY_NAME})

# ============================================================================
# Compiler Options (C only flags)
# ============================================================================
# NOTE ON SCOPE: these go out through __component_add_compile_options, i.e.
# PUBLICLY, so they reach every consumer -- including ipro_bt_host. The Host
# has bare `#if CONFIG_X` sites that only survive because -Wno-undef is in
# effect here, so a prebuilt build has to apply the same flags or the Host
# stops compiling. That is why this block sits ahead of the prebuilt branch
# rather than with the source list below.
#
# It is worth recording that this is broader than intended: the flags exist to
# compile CEVA's controller sources, but as public options they also switch off
# -Wundef, -Wsign-compare and friends for the entire Host. Narrowing them to
# PRIVATE would be correct, and would surface the Host's bare #if sites the way
# CONFIG_BT_PER_ADV_SYNC_BUF_SIZE surfaced on IPRO7.
# Use $<COMPILE_LANGUAGE:C> to apply C-specific flags
# Note: -Wno-undef is needed because CEVA code uses undefined macros as 0
set(BTDM_C_FLAGS
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-missing-field-initializers
    -Wno-pointer-sign
    -Wno-sign-compare
    -Wno-undef
    # GCC 14 diagnoses the v13 P-192 implementation's generic 192/384-bit
    # helper cast even though the selected type bounds each access correctly.
    -Wno-array-bounds
)
foreach(flag ${BTDM_C_FLAGS})
    __component_add_compile_options($<$<COMPILE_LANGUAGE:C>:${flag}>)
endforeach()

# ---------------------------------------------------------------------------
# Prebuilt archive
#
# Same contract as IPRO7: if the matching archive is present and the build has
# not explicitly asked for sources, link it and stop. This is what lets the
# RivieraWaves v13.0.18 sources stay out of a customer SDK -- without it,
# shipping IPRO6S means shipping upstream/ipro6s/ in full.
#
# --whole-archive for the same reason as IPRO7: the controller registers HCI
# command descriptors and kernel tasks through tables that nothing references
# directly, so ordinary archive semantics would drop those members and the
# prebuilt would behave differently from the source build.
# ---------------------------------------------------------------------------
if(EXISTS "${IPRO_BLE_CTLR_IPRO6S_PREBUILT}" AND NOT CONFIG_IPRO_BLE_CTLR_FORCE_SOURCE_BUILD)
    target_sources(${IPRO_BLE_CTLR_IPRO6S_LIBRARY_NAME} PRIVATE
        ${IPRO_BLE_CTLR_IPRO6S_ROOT}/prebuilt_anchor.c)
    # Exactly the include set the source build exports publicly -- no more.
    # The upstream modules/*/api headers are PRIVATE for a reason: co_bt.h and
    # co_hci.h only expand correctly under the controller's own CFG_* defines,
    # all 33 of which are private here. Exporting them publicly makes
    # components/wireless/rf/ipro6_rf (whose reg_access.h includes co_utils.h)
    # resolve to those headers without the defines, and it fails to compile on
    # HOST_ACTIVITY_MAX. A prebuilt must present the same interface as the
    # source build, not a larger one.
    __component_add_include_dirs(
        ${IPRO_BLE_CTLR_IPRO6S_ROOT}/include
        ${IPRO_BLE_CTLR_IPRO6S_ROOT}/hcitl
        ${IPRO_BLE_CTLR_IPRO6S_COMPAT})
    __component_add_link_options(-Wl,--whole-archive ${IPRO_BLE_CTLR_IPRO6S_PREBUILT} -Wl,--no-whole-archive)
    message(STATUS "[ipro_ble_controller] Using prebuilt ${IPRO_BLE_CTLR_IPRO6S_PREBUILT}")
    return()
endif()

# Preserve the controller's pre-migration __FILE__ strings.  RivieraWaves
# assertions compile absolute source paths into the image; without these maps a
# packaging-only directory move adds roughly 2 KB of path text to flash and
# makes binary comparison with the known IPRO6S baseline unnecessarily noisy.
__component_add_private_compile_options(
    -ffile-prefix-map=${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}=${CMAKE_SOURCE_DIR}/components/wireless/btdm
    -ffile-prefix-map=${IPRO_BLE_CTLR_IPRO6S_PLF}=${CMAKE_SOURCE_DIR}/components/wireless/btdm/plf/ipro6
    -ffile-prefix-map=${IPRO_BLE_CTLR_IPRO6S_PORT}=${CMAKE_SOURCE_DIR}/components/wireless/btdm/btdm_port
    -ffile-prefix-map=${IPRO_BLE_CTLR_IPRO6S_COMPAT}=${CMAKE_SOURCE_DIR}/components/wireless/btdm/api)

# ============================================================================
# Public Include Directories
# ============================================================================
# Export api/ directory for public API access
__component_add_include_dirs(
    ${IPRO_BLE_CTLR_IPRO6S_ROOT}/include
    ${IPRO_BLE_CTLR_IPRO6S_ROOT}/hcitl
    ${IPRO_BLE_CTLR_IPRO6S_COMPAT})

# ============================================================================
# Private Include Directories
# ============================================================================
# Note: rwip_config.h is in modules/rwip/api/ - configured via CFG_* defines
# Platform architecture
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_PLF}/arch)
# Platform registers
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_PLF}/driver/reg)
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_PLF}/driver/reg/ble)
if(CONFIG_BTDM_BT_ENABLE)
    __component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_PLF}/driver/reg/bt)
endif()
# Platform drivers (RF, LED stubs)
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_PLF}/driver/rf)
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_PLF}/driver/led)
if(CONFIG_BTDM_BT_ENABLE)
    __component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_PLF}/driver/dma)
endif()
# BTDM Port
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_PORT})
# CEVA modules API
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/ke/api)
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/common/api)
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/rwip/api)
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/security/api)
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/dbg/api)
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/h4tl/api)
if(CONFIG_BTDM_BT_ENABLE)
    __component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/data_path/api)
endif()
# CEVA modules src (internal headers)
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/ke/src)
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/common/src)
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/rwip/src)
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/h4tl/src)
# CEVA IP API
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/ip/hci/api)
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/ip/ll/ble/api)
if(CONFIG_BTDM_BT_ENABLE)
    __component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/ip/ll/bt/api)
endif()
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/ip/ll/sch/api)
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/ip/ll/em/api)
# CEVA IP src (internal headers)
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/ip/hci/src)
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/ip/ll/ble/src)
if(CONFIG_BTDM_BT_ENABLE)
    __component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/ip/ll/bt/src)
endif()
__component_add_private_include_dirs(${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/ip/ll/sch/src)

# ============================================================================
# Compiler Definitions
# ============================================================================

# Core feature flags
include(${CMAKE_CURRENT_LIST_DIR}/ipro6s_features.cmake)
foreach(_ipro6s_feature IN LISTS IPRO6S_OPTIONAL_BLE_DEFS)
    __component_add_private_compile_definitions(-D${_ipro6s_feature})
endforeach()
if(CONFIG_BTDM_BLE_ENABLE)
    __component_add_private_compile_definitions(-DCFG_BLE_EMB)
    # BLE roles
    __component_add_private_compile_definitions(-DCFG_BROADCASTER)
    __component_add_private_compile_definitions(-DCFG_OBSERVER)
    __component_add_private_compile_definitions(-DCFG_PERIPHERAL)
    __component_add_private_compile_definitions(-DCFG_CENTRAL)
    # IPRO6S has four hardware scheduler activity contexts. Advertising data
    # buffer capacity is configured separately below for periodic updates.
    __component_add_private_compile_definitions(-DCFG_ACT=4)
    __component_add_private_compile_definitions(-DCFG_RAL=3)
endif()

if(CONFIG_BTDM_BT_ENABLE)
    __component_add_private_compile_definitions(-DCFG_BT_EMB)
    __component_add_private_compile_definitions(-DCFG_CON_ACL=${CONFIG_BTDM_MAX_BR_ACL_CONNECTIONS})
    # The official v13 build always supplies CFG_CON_SCO, including zero for
    # ACL-only products. Keep the resource count APP-configurable so enabling
    # an HFP Host role does not silently advertise a controller with no
    # synchronous-link capability.
    if(DEFINED CONFIG_BTDM_MAX_SCO_CONNECTIONS)
        __component_add_private_compile_definitions(
            -DCFG_CON_SCO=${CONFIG_BTDM_MAX_SCO_CONNECTIONS})
    else()
        __component_add_private_compile_definitions(-DCFG_CON_SCO=0)
    endif()
    if(CONFIG_BTDM_VOICE_OVER_HCI)
        __component_add_private_compile_definitions(-DCFG_VOHCI)
    endif()

    # RW-BTDM v13 requires the software P-256/SHA implementations for Secure
    # Simple Pairing.  Without these definitions rwip_config.h intentionally
    # omits ECC_MULT_ALGO_TYPE and the official ECC sources cannot compile.
    # Use the release default 32-bit multiplication implementation on IPRO6S.
    __component_add_private_compile_definitions(-DCFG_ECC_P256_SUPPORT)
    __component_add_private_compile_definitions(-DCFG_SHA_256_SUPPORT)
endif()

# Platform flags
if(CONFIG_BTDM_FPGA)
    __component_add_private_compile_definitions(-DCFG_DBG_RUN_ON_FPGA)
endif()

# RTOS flag
if(CONFIG_FREERTOS)
    __component_add_private_compile_definitions(-DCFG_OS)
    __component_add_private_compile_definitions(-DCFG_FREERTOS)
endif()

# Connection configuration (use Kconfig values if defined, else use defaults from BLE role section)
if(DEFINED CONFIG_BTDM_MAX_ACL_CONNECTIONS)
    __component_add_private_compile_definitions(-DCFG_CON=${CONFIG_BTDM_MAX_ACL_CONNECTIONS})
endif()

# Debug configuration
if(CONFIG_BTDM_DEBUG)
    __component_add_private_compile_definitions(-DCFG_DBG)
endif()

# HCI Transport Layer
__component_add_private_compile_definitions(-DCFG_HCITL)

# BLE Manufacturing Test mode - allows direct handler calls from CLI
# (handlers return status directly, skip llm_cmd_cmp_send / hci_send_2_host)
__component_add_private_compile_definitions(-DCONFIG_BLE_MFG)

# BT version (BLE 5.4 - also includes 5.3 and 5.2 features)
__component_add_private_compile_definitions(-DCFG_BT_52)
__component_add_private_compile_definitions(-DCFG_BT_53)
__component_add_private_compile_definitions(-DCFG_BT_54)
__component_add_private_compile_definitions(-DCFG_BT_VERSION=54)

# Features not supported
__component_add_private_compile_definitions(-DGAIA_SUPPORT=0)

# IPRO6 platform identifier
__component_add_private_compile_definitions(-DCFG_IPRO6)
__component_add_private_compile_definitions(-DARCH_RISCV)

# Enable ipro6_phyrf_fpga integration for FPGA RF boards (T2/AD9361/GPM)
if(CONFIG_FPGA)
    if(CONFIG_RF_T2)
        __component_add_private_compile_definitions(-DCONFIG_RF_T2)
    elseif(CONFIG_RF_AD9361)
        __component_add_private_compile_definitions(-DCONFIG_RF_AD9361)
    elseif(CONFIG_RF_GPM)
        __component_add_private_compile_definitions(-DCONFIG_RF_GPM)
    endif()
endif()


# ============================================================================
# Platform Sources
# ============================================================================
__component_add_sources(
    ${IPRO_BLE_CTLR_IPRO6S_PLF}/arch/arch_main.c
    ${IPRO_BLE_CTLR_IPRO6S_PLF}/driver/rf/rf.c
)
if(CONFIG_IPRO_BLE_CTLR_CH_CLASS_TEST)
    __component_add_sources(
        ${IPRO_BLE_CTLR_IPRO6S_ROOT}/tests/channel_classification_override.c)
    __component_add_private_compile_definitions(
        -DCONFIG_IPRO_BLE_CTLR_CH_CLASS_TEST=1)
endif()
if(CONFIG_BTDM_BT_ENABLE)
    __component_add_sources(${IPRO_BLE_CTLR_IPRO6S_PLF}/driver/dma/dma.c)
endif()

# ============================================================================
# BTDM Port Sources
# ============================================================================
__component_add_sources(
    ${IPRO_BLE_CTLR_IPRO6S_PORT}/btdm_port.c
    ${IPRO_BLE_CTLR_IPRO6S_PORT}/btdm_port_freertos.c
)

# ============================================================================
# BTDM SDK API Sources
# ============================================================================
__component_add_sources(
    ${IPRO_BLE_CTLR_IPRO6S_COMPAT}/btdm_sdk.c
    ${IPRO_BLE_CTLR_IPRO6S_ROOT}/hcitl/ipro_ble_ctlr_hcitl.c
)

# ============================================================================
# CEVA Modules Sources
# ============================================================================

# Kernel (KE) module
file(GLOB KE_SOURCES ${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/ke/src/*.c)
foreach(src ${KE_SOURCES})
    __component_add_sources(${src})
endforeach()

# Common module
file(GLOB COMMON_SOURCES ${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/common/src/*.c)
foreach(src ${COMMON_SOURCES})
    __component_add_sources(${src})
endforeach()

# RWIP module
file(GLOB RWIP_SOURCES ${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/rwip/src/*.c)
foreach(src ${RWIP_SOURCES})
    __component_add_sources(${src})
endforeach()

# Security module (AES) - exclude BHSU (hardware AES) files as we use software AES
file(GLOB AES_SOURCES ${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/security/src/aes/*.c)
list(FILTER AES_SOURCES EXCLUDE REGEX ".*_bhsu\\.c$")
foreach(src ${AES_SOURCES})
    __component_add_sources(${src})
endforeach()

# The official v13 BR/EDR build requires both P-192/P-256 ECC and SHA for
# Secure Simple Pairing.  Keep the software implementations in the controller
# so the IPRO6S port does not depend on an unqualified crypto accelerator.
if(CONFIG_BTDM_BT_ENABLE)
    file(GLOB ECC_SOURCES ${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/security/src/ecc/*.c)
    foreach(src ${ECC_SOURCES})
        __component_add_sources(${src})
    endforeach()

    file(GLOB SHA_SOURCES ${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/security/src/sha/*.c)
    list(FILTER SHA_SOURCES EXCLUDE REGEX ".*_bhsu\\.c$")
    foreach(src ${SHA_SOURCES})
        __component_add_sources(${src})
    endforeach()
endif()

# Debug module
file(GLOB DBG_SOURCES ${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/dbg/src/*.c)
foreach(src ${DBG_SOURCES})
    __component_add_sources(${src})
endforeach()

# H4TL module
file(GLOB H4TL_SOURCES ${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/h4tl/src/*.c)
foreach(src ${H4TL_SOURCES})
    __component_add_sources(${src})
endforeach()

# BR/EDR ACL/SCO payload routing.
if(CONFIG_BTDM_BT_ENABLE)
    __component_add_sources(
        ${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/data_path/src/data_path.c
        ${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/modules/data_path/src/codec_db.c
    )
endif()

# ============================================================================
# CEVA IP Sources
# ============================================================================

# HCI
file(GLOB HCI_SOURCES ${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/ip/hci/src/*.c)
foreach(src ${HCI_SOURCES})
    __component_add_sources(${src})
endforeach()

# Link Layer - BLE.  The BL616D reference treats CFG_BLE_EMB and CFG_BT_EMB
# as independent build axes; do not compile the BLE LL into a Classic-only
# archive merely because both modes share the scheduler and HCI modules.
if(CONFIG_BTDM_BLE_ENABLE)
    file(GLOB_RECURSE LL_BLE_SOURCES ${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/ip/ll/ble/src/*.c)
    foreach(src ${LL_BLE_SOURCES})
        __component_add_sources(${src})
    endforeach()
endif()

# Link Layer - BR/EDR.  These are the unmodified v13.0.18 sources supplied in
# rw-btdm-sw-v13_0_18; generated register headers live in plf/ipro6s/driver/reg/bt.
if(CONFIG_BTDM_BT_ENABLE)
    file(GLOB_RECURSE LL_BT_SOURCES ${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/ip/ll/bt/src/*.c)
    foreach(src ${LL_BT_SOURCES})
        __component_add_sources(${src})
    endforeach()
endif()

# Scheduler
file(GLOB_RECURSE SCH_SOURCES ${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/ip/ll/sch/src/*.c)
foreach(src ${SCH_SOURCES})
    __component_add_sources(${src})
endforeach()

# Exchange Memory
file(GLOB EM_SOURCES ${IPRO_BLE_CTLR_IPRO6S_UPSTREAM}/ip/ll/em/src/*.c)
foreach(src ${EM_SOURCES})
    __component_add_sources(${src})
endforeach()

if(CONFIG_BTDM_BT_ENABLE AND CONFIG_BTDM_BLE_ENABLE)
    message(STATUS "[ipro_ble_controller] IPRO6S dual-mode backend configured as ${IPRO_BLE_CTLR_IPRO6S_LIBRARY_NAME}")
elseif(CONFIG_BTDM_BT_ENABLE)
    message(STATUS "[ipro_ble_controller] IPRO6S Classic-only backend configured as ${IPRO_BLE_CTLR_IPRO6S_LIBRARY_NAME}")
else()
    message(STATUS "[ipro_ble_controller] IPRO6S BLE backend configured as ${IPRO_BLE_CTLR_IPRO6S_LIBRARY_NAME}")
endif()

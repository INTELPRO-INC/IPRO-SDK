# ipro_ble_controller — shared IPRO BLE controller package.
#
# IPRO6S keeps the already-running RivieraWaves v13.0.18 / BT 5.4 controller
# while its platform integration is migrated out of components/wireless/btdm.
# Keep that backend isolated from the IPRO7 v14.0.2 source and patch set: a
# platform packaging change must not silently become a controller-core upgrade.
if(CONFIG_BTDM_ENABLE OR CONFIG_IPRO_BLE_CTLR_PLATFORM_IPRO6S)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ipro6s.cmake)
    return()
endif()

# IPRO7 BLE controller, built from RivieraWaves v14.0.2
#
# 取代原廠的 SCons 建置。來源清單與 CFG_* 定義的權威是
# upstream/src/*/config/{sourcelist.txt,SCompile}；這裡是照抄，
# 條件式用 CMake if() 鏡射原本的 python 條件並在註解裡標出原式。
#
# Customer archives encode both resource sizing and the controller feature set.

# ---------------------------------------------------------------------------
# 註冊成 SDK 元件
#
# 🔴 allow_warning = **1**（不加 -Werror）。
#
# upstream/ 是原廠原始碼且**唯讀**。原廠的程式碼會產生警告（未使用參數、
# 有號/無號比較之類），而規則 1 說我們不能改它。把警告當錯誤的話，
# 唯一的出路就是去改上游 —— 那正是這棵樹存在要避免的事。
#
# 我們自己寫的移植層要不要 -Werror 是另一回事，將來可以對
# plf/ ‧ hcitl/ ‧ rf/ 個別加 target_compile_options，不必連累 upstream/。
# ---------------------------------------------------------------------------
set(IPRO_BLE_CTLR_ROOT      ${CMAKE_CURRENT_SOURCE_DIR})
set(IPRO_BLE_CTLR_UPSTREAM  ${IPRO_BLE_CTLR_ROOT}/upstream/src)
set(IPRO_BLE_CTLR_PLF       ${IPRO_BLE_CTLR_ROOT}/plf/ipro7)
set(IPRO_BLE_CTLR_HCITL     ${IPRO_BLE_CTLR_ROOT}/hcitl)

if(DEFINED CONFIG_IPRO_BLE_CTLR_LIB)
    set(IPRO_BLE_CTLR_VARIANT "${CONFIG_IPRO_BLE_CTLR_LIB}")
else()
    # Backward-compatible default for existing .config files created before
    # IPRO_BLE_CTLR_VARIANT existed. Do not inherit the first-generation variant.
    set(IPRO_BLE_CTLR_VARIANT "link1_periph")
endif()

set(IPRO_BLE_CTLR_SUPPORTED_VARIANTS link1_periph link1 link2 link4)
if(NOT IPRO_BLE_CTLR_VARIANT IN_LIST IPRO_BLE_CTLR_SUPPORTED_VARIANTS)
    message(FATAL_ERROR "[ipro_ble_controller] unsupported variant '${IPRO_BLE_CTLR_VARIANT}'; supported: ${IPRO_BLE_CTLR_SUPPORTED_VARIANTS}")
endif()

if(DEFINED CONFIG_IPRO_BLE_CTLR_FEATURE)
    set(IPRO_BLE_CTLR_FEATURE "${CONFIG_IPRO_BLE_CTLR_FEATURE}")
else()
    set(IPRO_BLE_CTLR_FEATURE "bt52")
endif()
set(IPRO_BLE_CTLR_SUPPORTED_FEATURES bt52 bt54_iso2)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/cssa.cmake)
if(CONFIG_IPRO_BLE_CTLR_SUBRATING)
    if(NOT IPRO_BLE_CTLR_FEATURE STREQUAL "bt54_iso2")
        message(FATAL_ERROR "IPRO7 subrating requires the bt54_iso2 profile")
    endif()
    # Do not silently link an archive built without CFG_LE_SUBRATING.
    set(CONFIG_IPRO_BLE_CTLR_FORCE_SOURCE_BUILD ON)
endif()
if(NOT IPRO_BLE_CTLR_FEATURE IN_LIST IPRO_BLE_CTLR_SUPPORTED_FEATURES)
    message(FATAL_ERROR "[ipro_ble_controller] unsupported feature profile '${IPRO_BLE_CTLR_FEATURE}'; supported: ${IPRO_BLE_CTLR_SUPPORTED_FEATURES}")
endif()

if(NOT DEFINED CONFIG_IPRO_BLE_CTLR_CFG_DBG)
    # Customer SDK default is the no-CFG_DBG release controller profile.
    # Bring-up/test applications that need RW_DEBUG/PLF_DEBUG paths must opt in
    # explicitly in their .config so archive identity and validation stay clear.
    set(CONFIG_IPRO_BLE_CTLR_CFG_DBG n)
endif()
if(CONFIG_IPRO_BLE_CTLR_TRACE_BUFFERS)
    add_compile_definitions(IPRO_BLE_CTLR_TRACE_BUFFERS=1)
else()
    add_compile_definitions(IPRO_BLE_CTLR_TRACE_BUFFERS=0)
endif()
if(DEFINED CONFIG_IPRO_BLE_CTLR_TRACE_DEPTH)
    # lld_cis.c consumes IPRO_CIS_TRACE_DEPTH.  Keep the Kconfig-facing name
    # controller-specific, and feed both controller trace implementations.
    add_compile_definitions(IPRO_BLE_CTLR_TRACE_DEPTH=${CONFIG_IPRO_BLE_CTLR_TRACE_DEPTH})
    add_compile_definitions(IPRO_CIS_TRACE_DEPTH=${CONFIG_IPRO_BLE_CTLR_TRACE_DEPTH})
else()
    # Diagnostic buffers are opt-in and bounded.  A normal build uses only
    # one-entry ABI sentinels; diagnostic builds without an explicit depth use
    # the smallest supported capture instead of silently reserving 4096 rows.
    add_compile_definitions(IPRO_BLE_CTLR_TRACE_DEPTH=256)
    add_compile_definitions(IPRO_CIS_TRACE_DEPTH=256)
endif()
if(DEFINED CONFIG_IPRO_BLE_CTLR_CIS_RXWIN_DIAG_EXTRA_US)
    add_compile_definitions(IPRO_CIS_RXWIN_DIAG_EXTRA_US=${CONFIG_IPRO_BLE_CTLR_CIS_RXWIN_DIAG_EXTRA_US})
endif()
if(CONFIG_IPRO_BLE_CTLR_CIS_DIAG)
    add_compile_definitions(IPRO_BLE_CTLR_CIS_DIAG=1)
else()
    add_compile_definitions(IPRO_BLE_CTLR_CIS_DIAG=0)
endif()
if(CONFIG_IPRO_BLE_CTLR_NATIVE_SLEEP_DEFAULT)
    add_compile_definitions(IPRO_BLE_CTLR_NATIVE_SLEEP_DEFAULT=1)
else()
    add_compile_definitions(IPRO_BLE_CTLR_NATIVE_SLEEP_DEFAULT=0)
endif()
if(CONFIG_IPRO_BLE_CTLR_32K_FORCE_RC32K)
    add_compile_definitions(IPRO_BLE_CTLR_32K_FORCE_RC32K=1)
else()
    add_compile_definitions(IPRO_BLE_CTLR_32K_FORCE_RC32K=0)
endif()
if(DEFINED CONFIG_IPRO_BLE_CTLR_32K_CORRECTED_DRIFT_PPM)
    add_compile_definitions(IPRO_BLE_CTLR_32K_CORRECTED_DRIFT_PPM=${CONFIG_IPRO_BLE_CTLR_32K_CORRECTED_DRIFT_PPM})
else()
    add_compile_definitions(IPRO_BLE_CTLR_32K_CORRECTED_DRIFT_PPM=0)
endif()

if(NOT DEFINED CONFIG_EM_SIZE)
    message(FATAL_ERROR "[ipro_ble_controller] CONFIG_EM_SIZE must match the IPRO7 hardware EM selection")
endif()
if(IPRO_BLE_CTLR_FEATURE STREQUAL "bt54_iso2" AND CONFIG_EM_SIZE LESS 32)
    message(FATAL_ERROR
        "[ipro_ble_controller] bt54_iso2 needs at least 32 KB EM; "
        "its fixed layout is larger than the 16 KB retained-PDS profile")
endif()

# Keep the PDS ABI and diagnostics in every archive. Runtime entry is accepted
# only by the qualified 16 KB retained-EM profile; larger ISO profiles return
# an explicit unsupported result while retaining native controller sleep.
add_compile_definitions(IPRO_BLE_CTLR_PDS_CAPABLE=1)

set(IPRO_BLE_CTLR_LIBRARY_NAME "ipro_ble_controller_ipro7_${IPRO_BLE_CTLR_VARIANT}")
if(NOT IPRO_BLE_CTLR_FEATURE STREQUAL "bt52")
    string(APPEND IPRO_BLE_CTLR_LIBRARY_NAME "_${IPRO_BLE_CTLR_FEATURE}")
endif()
if(NOT CONFIG_IPRO_BLE_CTLR_CFG_DBG)
    string(APPEND IPRO_BLE_CTLR_LIBRARY_NAME "_release")
endif()
string(APPEND IPRO_BLE_CTLR_LIBRARY_NAME "${IPRO_BLE_CTLR_CSSA_SUFFIX}")
set(IPRO_BLE_CTLR_PREBUILT "${IPRO_BLE_CTLR_ROOT}/lib/lib${IPRO_BLE_CTLR_LIBRARY_NAME}.a")
__generate_library(1 ${IPRO_BLE_CTLR_LIBRARY_NAME})

if(EXISTS "${IPRO_BLE_CTLR_PREBUILT}" AND NOT CONFIG_IPRO_BLE_CTLR_FORCE_SOURCE_BUILD)
    target_sources(${IPRO_BLE_CTLR_LIBRARY_NAME} PRIVATE ${IPRO_BLE_CTLR_ROOT}/prebuilt_anchor.c)
    __component_add_include_dirs(
        ${IPRO_BLE_CTLR_ROOT}/include
        ${IPRO_BLE_CTLR_UPSTREAM}/modules/rwip/api
        ${IPRO_BLE_CTLR_UPSTREAM}/modules/common/api
        ${IPRO_BLE_CTLR_UPSTREAM}/ip/hci/api
        ${IPRO_BLE_CTLR_PLF}/arch/main
        ${IPRO_BLE_CTLR_PLF}/nvds
        ${IPRO_BLE_CTLR_PLF}/lowpower
        ${IPRO_BLE_CTLR_HCITL})
    __component_add_link_options(-Wl,--whole-archive ${IPRO_BLE_CTLR_PREBUILT} -Wl,--no-whole-archive)
    message(STATUS "[ipro_ble_controller] Using prebuilt ${IPRO_BLE_CTLR_PREBUILT}")
    return()
endif()

# ---------------------------------------------------------------------------
# Isolated upstream patches
#
# `upstream/` remains byte-for-byte pristine.  IPRO7 does not promptly end a
# legacy advertising event after CONNECT_IND, unlike the reference hardware;
# waiting for EOF makes the first connection anchor expire.  Build a patched
# copy outside upstream so the silicon adaptation is explicit and reviewable.
# ---------------------------------------------------------------------------
set(IPRO_BLE_CTLR_PATCHED_DIR ${CMAKE_CURRENT_BINARY_DIR}/patched)
file(MAKE_DIRECTORY ${IPRO_BLE_CTLR_PATCHED_DIR})
file(GLOB IPRO_BLE_CTLR_PATCH_FILES CONFIGURE_DEPENDS
    "${IPRO_BLE_CTLR_ROOT}/patches/*.patch")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    ${IPRO_BLE_CTLR_PATCH_FILES})
set(IPRO_BLE_CTLR_LLD_CORE_SRC ${IPRO_BLE_CTLR_PATCHED_DIR}/lld.c)
set(IPRO_BLE_CTLR_LLD_ADV_SRC ${IPRO_BLE_CTLR_PATCHED_DIR}/lld_adv.c)
set(IPRO_BLE_CTLR_LLD_CON_SRC ${IPRO_BLE_CTLR_PATCHED_DIR}/lld_con.c)
set(IPRO_BLE_CTLR_LLD_ISO_SRC ${IPRO_BLE_CTLR_PATCHED_DIR}/lld_iso.c)
set(IPRO_BLE_CTLR_LLD_ISOAL_SRC ${IPRO_BLE_CTLR_PATCHED_DIR}/lld_isoal.c)
set(IPRO_BLE_CTLR_LLD_CIS_SRC ${IPRO_BLE_CTLR_PATCHED_DIR}/lld_cis.c)
set(IPRO_BLE_CTLR_LLD_TEST_SRC ${IPRO_BLE_CTLR_PATCHED_DIR}/lld_test.c)
set(IPRO_BLE_CTLR_LLI_CIS_SRC ${IPRO_BLE_CTLR_PATCHED_DIR}/lli_cis.c)
set(IPRO_BLE_CTLR_ISOOHCI_OUT_SRC ${IPRO_BLE_CTLR_PATCHED_DIR}/isoohci_out.c)
set(IPRO_BLE_CTLR_RWIP_DRIVER_SRC ${IPRO_BLE_CTLR_PATCHED_DIR}/rwip_driver.c)
set(IPRO_BLE_CTLR_LLC_SRC ${IPRO_BLE_CTLR_PATCHED_DIR}/llc.c)
set(IPRO_BLE_CTLR_LLC_TASK_SRC ${IPRO_BLE_CTLR_PATCHED_DIR}/llc_task.c)
set(IPRO_BLE_CTLR_LLC_LLCP_SRC ${IPRO_BLE_CTLR_PATCHED_DIR}/llc_llcp.c)
set(IPRO_BLE_CTLR_LLC_DL_UPD_SRC ${IPRO_BLE_CTLR_PATCHED_DIR}/llc_dl_upd.c)
set(IPRO_BLE_CTLR_LLM_SRC ${IPRO_BLE_CTLR_PATCHED_DIR}/llm.c)
set(IPRO_BLE_CTLR_SCH_ARB_SRC ${IPRO_BLE_CTLR_PATCHED_DIR}/sch_arb.c)
set(IPRO_BLE_CTLR_SCH_PROG_SRC ${IPRO_BLE_CTLR_PATCHED_DIR}/sch_prog.c)
set(IPRO_BLE_CTLR_RWIP_CONFIG ${IPRO_BLE_CTLR_PATCHED_DIR}/rwip_config.h)

function(ipro_ble_ctlr_copy_upstream_source src dst)
    configure_file(${src} ${dst} COPYONLY)
    # Some imported RW files use CRLF.  Normalize only the generated build copy
    # so local IPRO7 patches are portable while upstream/ remains pristine.
    file(READ ${dst} _ipro_ble_ctlr_src_text)
    string(REPLACE "\r\n" "\n" _ipro_ble_ctlr_src_text "${_ipro_ble_ctlr_src_text}")
    file(WRITE ${dst} "${_ipro_ble_ctlr_src_text}")
    unset(_ipro_ble_ctlr_src_text)
endfunction()

function(ipro_ble_ctlr_apply_file_patch target patch_file label)
    execute_process(
        COMMAND patch --batch --silent ${target} ${patch_file}
        RESULT_VARIABLE _ipro_ble_ctlr_patch_result
        OUTPUT_VARIABLE _ipro_ble_ctlr_patch_output
        ERROR_VARIABLE _ipro_ble_ctlr_patch_error)
    if(NOT _ipro_ble_ctlr_patch_result EQUAL 0)
        message(FATAL_ERROR
            "[ipro_ble_controller] failed to apply ${label}: "
            "${_ipro_ble_ctlr_patch_output}${_ipro_ble_ctlr_patch_error}")
    endif()
endfunction()

ipro_ble_ctlr_copy_upstream_source(
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lld/lld.c
    ${IPRO_BLE_CTLR_LLD_CORE_SRC})
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_CORE_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0043-ipro7-lld-rxwin-and-drift-diagnostics.patch
    "LLD core RX window/drift diagnostics patch")

ipro_ble_ctlr_copy_upstream_source(
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lld/lld_test.c
    ${IPRO_BLE_CTLR_LLD_TEST_SRC})
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_TEST_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0098-ipro7-port-dtm-counter-and-timing-workarounds.patch
    "IPRO7 DTM counter and RF timing workarounds patch")

configure_file(
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lld/lld_adv.c
    ${IPRO_BLE_CTLR_LLD_ADV_SRC}
    COPYONLY)
configure_file(
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_llcp.c
    ${IPRO_BLE_CTLR_LLC_LLCP_SRC}
    COPYONLY)
configure_file(
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_task.c
    ${IPRO_BLE_CTLR_LLC_TASK_SRC}
    COPYONLY)
ipro_ble_ctlr_copy_upstream_source(
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc.c
    ${IPRO_BLE_CTLR_LLC_SRC})
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLC_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0051-ipro7-let-hci-host-own-feature-exchange.patch
    "HCI-host-owned startup LLCP patch")
configure_file(
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_dl_upd.c
    ${IPRO_BLE_CTLR_LLC_DL_UPD_SRC}
    COPYONLY)
configure_file(
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llm/llm.c
    ${IPRO_BLE_CTLR_LLM_SRC}
    COPYONLY)
# This upstream unit contains CRLF in the DLE block.  Normalize only the
# generated build copy so portable unified-diff patches remain reviewable.
file(READ ${IPRO_BLE_CTLR_LLC_DL_UPD_SRC} IPRO_BLE_CTLR_LLC_DL_UPD_TEXT)
string(REPLACE "\r\n" "\n" IPRO_BLE_CTLR_LLC_DL_UPD_TEXT "${IPRO_BLE_CTLR_LLC_DL_UPD_TEXT}")
file(WRITE ${IPRO_BLE_CTLR_LLC_DL_UPD_SRC} "${IPRO_BLE_CTLR_LLC_DL_UPD_TEXT}")
unset(IPRO_BLE_CTLR_LLC_DL_UPD_TEXT)
configure_file(
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/sch/src/sch_arb.c
    ${IPRO_BLE_CTLR_SCH_ARB_SRC}
    COPYONLY)
configure_file(
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/sch/src/sch_prog.c
    ${IPRO_BLE_CTLR_SCH_PROG_SRC}
    COPYONLY)
configure_file(
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/rwip/api/rwip_config.h
    ${IPRO_BLE_CTLR_RWIP_CONFIG}
    COPYONLY)
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_RWIP_CONFIG}
            ${IPRO_BLE_CTLR_ROOT}/patches/0018-size-ecc-heap-for-dhkey-table.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_ECC_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_ECC_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_ECC_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_ECC_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply ECC heap patch: "
        "${IPRO_BLE_CTLR_ECC_PATCH_OUTPUT}${IPRO_BLE_CTLR_ECC_PATCH_ERROR}")
endif()
execute_process(
COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLC_LLCP_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0017-reject-unconfigured-llcp-opcode.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_LLCP_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_LLCP_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_LLCP_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_LLCP_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply LLCP validation patch: "
        "${IPRO_BLE_CTLR_LLCP_PATCH_OUTPUT}${IPRO_BLE_CTLR_LLCP_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent -i ${IPRO_BLE_CTLR_ROOT}/patches/0052-ipro7-llcp-live-diagnostics.patch
    WORKING_DIRECTORY ${IPRO_BLE_CTLR_PATCHED_DIR}
    RESULT_VARIABLE IPRO_BLE_CTLR_LLCP_DIAG_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_LLCP_DIAG_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_LLCP_DIAG_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_LLCP_DIAG_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply LLCP live diagnostics patch: "
        "${IPRO_BLE_CTLR_LLCP_DIAG_PATCH_OUTPUT}${IPRO_BLE_CTLR_LLCP_DIAG_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLC_DL_UPD_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0019-bound-uncoded-dle-effective-time.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_DLE_TIME_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_DLE_TIME_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_DLE_TIME_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_DLE_TIME_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply uncoded DLE effective-time patch: "
        "${IPRO_BLE_CTLR_DLE_TIME_PATCH_OUTPUT}${IPRO_BLE_CTLR_DLE_TIME_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLM_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0053-ipro7-onchip-hci-conservative-local-features.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_LLM_FEATURE_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_LLM_FEATURE_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_LLM_FEATURE_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_LLM_FEATURE_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply on-chip HCI local feature policy patch: "
        "${IPRO_BLE_CTLR_LLM_FEATURE_PATCH_OUTPUT}${IPRO_BLE_CTLR_LLM_FEATURE_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_SCH_ARB_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0021-ipro7-expose-next-arbiter-slack.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_ARB_SLACK_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_ARB_SLACK_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_ARB_SLACK_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_ARB_SLACK_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply arbiter slack API patch: "
        "${IPRO_BLE_CTLR_ARB_SLACK_PATCH_OUTPUT}${IPRO_BLE_CTLR_ARB_SLACK_PATCH_ERROR}")
endif()
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_SCH_ARB_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0060-ipro7-rearm-arbiter-timer-after-platform-pds.patch
    "arbiter timer post-PDS re-arm patch")
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_SCH_PROG_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0023-ipro7-rearm-programmer-after-platform-reset.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_PROG_REARM_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_PROG_REARM_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_PROG_REARM_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_PROG_REARM_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply scheduler programmer re-arm patch: "
        "${IPRO_BLE_CTLR_PROG_REARM_PATCH_OUTPUT}${IPRO_BLE_CTLR_PROG_REARM_PATCH_ERROR}")
endif()
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_SCH_PROG_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0058-ipro7-retime-programmed-et-after-platform-pds.patch
    "scheduler programmer post-PDS retime patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_SCH_PROG_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0075-ipro7-retain-sch-prog-push-et-latch.patch
    "post-PDS scheduler ET push retained latch patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_SCH_PROG_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0079-ipro7-skip-duplicate-pds-recovery-et-push.patch
    "post-PDS duplicate scheduler ET push guard patch")
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_ADV_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0001-ipro7-handle-connect-ind-on-rx.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply CONNECT_IND RX patch: "
        "${IPRO_BLE_CTLR_PATCH_OUTPUT}${IPRO_BLE_CTLR_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_ADV_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0013-ipro7-advertising-event-diagnostics.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_ADV_DIAG_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_ADV_DIAG_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_ADV_DIAG_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_ADV_DIAG_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply advertising diagnostics patch: "
        "${IPRO_BLE_CTLR_ADV_DIAG_PATCH_OUTPUT}${IPRO_BLE_CTLR_ADV_DIAG_PATCH_ERROR}")
endif()
ipro_ble_ctlr_copy_upstream_source(
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lld/lld_con.c
    ${IPRO_BLE_CTLR_LLD_CON_SRC})
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_CON_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0044-ipro7-catch-up-expired-acl-anchor-after-cis-sync.patch
    "ACL anchor catch-up after CIS sync patch")
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0004-ipro7-connection-diagnostics.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_CON_DIAG_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_CON_DIAG_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_CON_DIAG_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_CON_DIAG_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply connection diagnostics patch: "
        "${IPRO_BLE_CTLR_CON_DIAG_PATCH_OUTPUT}${IPRO_BLE_CTLR_CON_DIAG_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0005-ipro7-reject-false-sync-in-wide-window.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_CON_SYNC_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_CON_SYNC_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_CON_SYNC_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_CON_SYNC_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply wide-window sync patch: "
        "${IPRO_BLE_CTLR_CON_SYNC_PATCH_OUTPUT}${IPRO_BLE_CTLR_CON_SYNC_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0008-ipro7-connection-timing-diagnostics.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_CON_TIMING_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_CON_TIMING_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_CON_TIMING_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_CON_TIMING_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply connection timing diagnostics patch: "
        "${IPRO_BLE_CTLR_CON_TIMING_PATCH_OUTPUT}${IPRO_BLE_CTLR_CON_TIMING_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0009-ipro7-instant-diagnostics.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_INSTANT_DIAG_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_INSTANT_DIAG_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_INSTANT_DIAG_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_INSTANT_DIAG_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply instant diagnostics patch: "
        "${IPRO_BLE_CTLR_INSTANT_DIAG_PATCH_OUTPUT}${IPRO_BLE_CTLR_INSTANT_DIAG_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0010-ipro7-guard-deep-sleep-across-connection-update.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_UPDATE_GUARD_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_UPDATE_GUARD_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_UPDATE_GUARD_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_UPDATE_GUARD_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply connection-update sleep guard: "
        "${IPRO_BLE_CTLR_UPDATE_GUARD_PATCH_OUTPUT}${IPRO_BLE_CTLR_UPDATE_GUARD_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0011-ipro7-recover-missed-event-without-deep-sleep.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_RECOVERY_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_RECOVERY_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_RECOVERY_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_RECOVERY_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply missed-event recovery patch: "
        "${IPRO_BLE_CTLR_RECOVERY_PATCH_OUTPUT}${IPRO_BLE_CTLR_RECOVERY_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0015-ipro7-catch-up-connection-after-platform-pds.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_PDS_CON_CATCHUP_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_PDS_CON_CATCHUP_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_PDS_CON_CATCHUP_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_PDS_CON_CATCHUP_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply connected PDS catch-up patch: "
        "${IPRO_BLE_CTLR_PDS_CON_CATCHUP_PATCH_OUTPUT}${IPRO_BLE_CTLR_PDS_CON_CATCHUP_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0016-ipro7-release-pds-recovery-on-connection-event.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_PDS_CON_RELEASE_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_PDS_CON_RELEASE_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_PDS_CON_RELEASE_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_PDS_CON_RELEASE_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply connected PDS recovery-release patch: "
        "${IPRO_BLE_CTLR_PDS_CON_RELEASE_PATCH_OUTPUT}${IPRO_BLE_CTLR_PDS_CON_RELEASE_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0020-ipro7-connection-cancel-timing-diagnostics.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_CON_CANCEL_DIAG_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_CON_CANCEL_DIAG_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_CON_CANCEL_DIAG_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_CON_CANCEL_DIAG_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply connection cancel timing diagnostics: "
        "${IPRO_BLE_CTLR_CON_CANCEL_DIAG_PATCH_OUTPUT}${IPRO_BLE_CTLR_CON_CANCEL_DIAG_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0022-ipro7-release-pds-recovery-on-crc-valid.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_PDS_CRC_RELEASE_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_PDS_CRC_RELEASE_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_PDS_CRC_RELEASE_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_PDS_CRC_RELEASE_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply CRC-valid PDS recovery release: "
        "${IPRO_BLE_CTLR_PDS_CRC_RELEASE_PATCH_OUTPUT}${IPRO_BLE_CTLR_PDS_CRC_RELEASE_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0024-ipro7-capture-post-pds-ble-diag-waveform.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_DIAG_BURST_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_DIAG_BURST_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_DIAG_BURST_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_DIAG_BURST_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply post-PDS BLE waveform patch: "
        "${IPRO_BLE_CTLR_DIAG_BURST_PATCH_OUTPUT}${IPRO_BLE_CTLR_DIAG_BURST_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0025-ipro7-trace-post-pds-reschedule-chain.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_RESCHED_TRACE_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_RESCHED_TRACE_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_RESCHED_TRACE_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_RESCHED_TRACE_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply post-PDS reschedule trace patch: "
        "${IPRO_BLE_CTLR_RESCHED_TRACE_PATCH_OUTPUT}${IPRO_BLE_CTLR_RESCHED_TRACE_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0026-ipro7-capture-pds-connection-software-state.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_CON_STATE_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_CON_STATE_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_CON_STATE_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_CON_STATE_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply PDS connection-state capture patch: "
        "${IPRO_BLE_CTLR_CON_STATE_PATCH_OUTPUT}${IPRO_BLE_CTLR_CON_STATE_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0027-ipro7-capture-first-post-pds-connection-callback.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_FIRST_CALLBACK_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_FIRST_CALLBACK_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_FIRST_CALLBACK_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_FIRST_CALLBACK_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply first post-PDS callback capture patch: "
        "${IPRO_BLE_CTLR_FIRST_CALLBACK_PATCH_OUTPUT}${IPRO_BLE_CTLR_FIRST_CALLBACK_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0028-ipro7-retain-first-post-pds-radio-boundary.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_RADIO_BOUNDARY_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_RADIO_BOUNDARY_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_RADIO_BOUNDARY_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_RADIO_BOUNDARY_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply post-PDS radio-boundary capture patch: "
        "${IPRO_BLE_CTLR_RADIO_BOUNDARY_PATCH_OUTPUT}${IPRO_BLE_CTLR_RADIO_BOUNDARY_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0029-ipro7-retain-post-pds-cs-boundary.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_CS_BOUNDARY_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_CS_BOUNDARY_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_CS_BOUNDARY_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_CS_BOUNDARY_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply post-PDS CS-boundary capture patch: "
        "${IPRO_BLE_CTLR_CS_BOUNDARY_PATCH_OUTPUT}${IPRO_BLE_CTLR_CS_BOUNDARY_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0030-ipro7-post-reset-phase-bias-diagnostic.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_PHASE_BIAS_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_PHASE_BIAS_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_PHASE_BIAS_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_PHASE_BIAS_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply post-reset phase-bias diagnostic: "
        "${IPRO_BLE_CTLR_PHASE_BIAS_PATCH_OUTPUT}${IPRO_BLE_CTLR_PHASE_BIAS_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0033-ipro7-guard-large-first-post-reset-reanchor.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_REANCHOR_GUARD_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_REANCHOR_GUARD_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_REANCHOR_GUARD_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_REANCHOR_GUARD_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply first post-reset re-anchor guard: "
        "${IPRO_BLE_CTLR_REANCHOR_GUARD_PATCH_OUTPUT}${IPRO_BLE_CTLR_REANCHOR_GUARD_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0034-ipro7-commit-anchor-only-on-valid-rx.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_VALID_ANCHOR_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_VALID_ANCHOR_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_VALID_ANCHOR_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_VALID_ANCHOR_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply valid-anchor commit patch: "
        "${IPRO_BLE_CTLR_VALID_ANCHOR_PATCH_OUTPUT}${IPRO_BLE_CTLR_VALID_ANCHOR_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0035-ipro7-flush-stale-rxdesc-on-connection-start.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_RXDESC_FLUSH_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_RXDESC_FLUSH_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_RXDESC_FLUSH_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_RXDESC_FLUSH_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply stale RX descriptor flush patch: "
        "${IPRO_BLE_CTLR_RXDESC_FLUSH_PATCH_OUTPUT}${IPRO_BLE_CTLR_RXDESC_FLUSH_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0036-ipro7-reload-supervision-only-on-clean-rx.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_CLEAN_RX_SUPERVISION_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_CLEAN_RX_SUPERVISION_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_CLEAN_RX_SUPERVISION_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_CLEAN_RX_SUPERVISION_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply clean-RX supervision patch: "
        "${IPRO_BLE_CTLR_CLEAN_RX_SUPERVISION_PATCH_OUTPUT}${IPRO_BLE_CTLR_CLEAN_RX_SUPERVISION_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0037-ipro7-latch-sync-only-on-valid-rx-anchor.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_SYNC_LATCH_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_SYNC_LATCH_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_SYNC_LATCH_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_SYNC_LATCH_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply valid-RX sync latch patch: "
        "${IPRO_BLE_CTLR_SYNC_LATCH_PATCH_OUTPUT}${IPRO_BLE_CTLR_SYNC_LATCH_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0038-ipro7-anchor-only-first-sync-descriptor.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_FIRST_SYNC_DESC_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_FIRST_SYNC_DESC_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_FIRST_SYNC_DESC_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_FIRST_SYNC_DESC_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply first-sync-descriptor anchor patch: "
        "${IPRO_BLE_CTLR_FIRST_SYNC_DESC_PATCH_OUTPUT}${IPRO_BLE_CTLR_FIRST_SYNC_DESC_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0039-ipro7-event-level-sync-latch.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_EVENT_SYNC_LATCH_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_EVENT_SYNC_LATCH_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_EVENT_SYNC_LATCH_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_EVENT_SYNC_LATCH_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply event-level sync latch patch: "
        "${IPRO_BLE_CTLR_EVENT_SYNC_LATCH_PATCH_OUTPUT}${IPRO_BLE_CTLR_EVENT_SYNC_LATCH_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0040-ipro7-debug-force-software-csa2.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_FORCE_CSA2_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_FORCE_CSA2_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_FORCE_CSA2_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_FORCE_CSA2_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply force software CSA#2 diagnostic patch: "
        "${IPRO_BLE_CTLR_FORCE_CSA2_PATCH_OUTPUT}${IPRO_BLE_CTLR_FORCE_CSA2_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0057-ipro7-pds-csa2-use-air-event-counter.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_PDS_CSA2_COUNTER_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_PDS_CSA2_COUNTER_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_PDS_CSA2_COUNTER_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_PDS_CSA2_COUNTER_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply PDS CSA#2 air-event-counter patch: "
        "${IPRO_BLE_CTLR_PDS_CSA2_COUNTER_PATCH_OUTPUT}${IPRO_BLE_CTLR_PDS_CSA2_COUNTER_PATCH_ERROR}")
endif()
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_CON_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0065-ipro7-release-pds-recovery-on-event-level-clean-rx.patch
    "post-PDS event-level clean RX recovery release patch")
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0041-ipro7-persist-first-sync-descriptor-latch.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_FIRST_SYNC_LATCH_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_FIRST_SYNC_LATCH_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_FIRST_SYNC_LATCH_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_FIRST_SYNC_LATCH_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply persistent first-sync latch patch: "
        "${IPRO_BLE_CTLR_FIRST_SYNC_LATCH_PATCH_OUTPUT}${IPRO_BLE_CTLR_FIRST_SYNC_LATCH_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_LLD_CON_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0054-ipro7-lld-rx-pdu-diagnostics.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_LLD_RX_PDU_DIAG_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_LLD_RX_PDU_DIAG_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_LLD_RX_PDU_DIAG_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_LLD_RX_PDU_DIAG_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply LLD RX PDU diagnostics patch: "
        "${IPRO_BLE_CTLR_LLD_RX_PDU_DIAG_PATCH_OUTPUT}${IPRO_BLE_CTLR_LLD_RX_PDU_DIAG_PATCH_ERROR}")
endif()
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_CON_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0059-ipro7-retime-waiting-connection-after-platform-pds.patch
    "waiting connection post-PDS retime patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_CON_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0062-ipro7-bound-post-pds-recovery-rxwin-drift.patch
    "bounded post-PDS recovery RX window drift patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_CON_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0063-ipro7-retain-first-nonsync-rx-descriptor.patch
    "first non-sync RX descriptor retained trace patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_CON_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0055-ipro7-rebase-post-pds-anchor-on-clean-crc-rx.patch
    "post-PDS timestamp-valid anchor rebase patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_CON_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0064-ipro7-restore-csa2-fhen-after-pds-clean-rx.patch
    "post-PDS CSA#2 FHEN restore after clean RX patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_CON_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0066-ipro7-release-pds-recovery-in-rx-trace-clean-path.patch
    "post-PDS RX trace clean-path recovery release patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_CON_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0067-ipro7-do-not-reenter-pds-recovery-after-clean-release.patch
    "post-PDS recovery latch re-entry guard patch")
	ipro_ble_ctlr_apply_file_patch(
	    ${IPRO_BLE_CTLR_LLD_CON_SRC}
	    ${IPRO_BLE_CTLR_ROOT}/patches/0068-ipro7-trace-pds-rxtrace-release-gates.patch
	    "post-PDS RX trace release gate diagnostics patch")
	ipro_ble_ctlr_apply_file_patch(
	    ${IPRO_BLE_CTLR_LLD_CON_SRC}
	    ${IPRO_BLE_CTLR_ROOT}/patches/0069-ipro7-defer-csa2-fhen-rearm-after-pds-clean-rx.patch
	    "post-PDS deferred CSA#2 FHEN rearm patch")
	ipro_ble_ctlr_apply_file_patch(
	    ${IPRO_BLE_CTLR_LLD_CON_SRC}
	    ${IPRO_BLE_CTLR_ROOT}/patches/0070-ipro7-adapt-pds-recovery-phase-from-crc-sync.patch
	    "post-PDS adaptive recovery phase patch")
	ipro_ble_ctlr_apply_file_patch(
	    ${IPRO_BLE_CTLR_LLD_CON_SRC}
	    ${IPRO_BLE_CTLR_ROOT}/patches/0071-ipro7-release-pds-recovery-only-on-clean-event.patch
	    "post-PDS clean-event-only recovery release patch")
	ipro_ble_ctlr_apply_file_patch(
	    ${IPRO_BLE_CTLR_LLD_CON_SRC}
	    ${IPRO_BLE_CTLR_ROOT}/patches/0072-ipro7-bound-pds-adaptive-phase-samples.patch
	    "post-PDS bounded adaptive phase sample patch")
	ipro_ble_ctlr_apply_file_patch(
	    ${IPRO_BLE_CTLR_LLD_CON_SRC}
	    ${IPRO_BLE_CTLR_ROOT}/patches/0074-ipro7-narrow-post-pds-recovery-rxwin-from-clock-error.patch
	    "post-PDS measured-error RX window drift patch")
	ipro_ble_ctlr_apply_file_patch(
	    ${IPRO_BLE_CTLR_LLD_CON_SRC}
	    ${IPRO_BLE_CTLR_ROOT}/patches/0073-ipro7-preload-event-counter-before-pds-recovery-event.patch
	    "post-PDS CSA#2 event-counter preload patch")
	ipro_ble_ctlr_apply_file_patch(
	    ${IPRO_BLE_CTLR_LLD_CON_SRC}
	    ${IPRO_BLE_CTLR_ROOT}/patches/0076-ipro7-post-pds-csa2-event-counter-offset-diag.patch
	    "post-PDS CSA#2 event-counter offset diagnostic patch")
	ipro_ble_ctlr_apply_file_patch(
	    ${IPRO_BLE_CTLR_LLD_CON_SRC}
	    ${IPRO_BLE_CTLR_ROOT}/patches/0077-ipro7-release-pds-recovery-only-after-post-enter-clean-event.patch
	    "post-PDS release only after post-enter clean event patch")
	ipro_ble_ctlr_apply_file_patch(
	    ${IPRO_BLE_CTLR_LLD_CON_SRC}
	    ${IPRO_BLE_CTLR_ROOT}/patches/0056-ipro7-flush-post-pds-stale-rxdesc-before-event.patch
	    "post-PDS stale RX descriptor flush patch")
	ipro_ble_ctlr_apply_file_patch(
	    ${IPRO_BLE_CTLR_LLD_CON_SRC}
	    ${IPRO_BLE_CTLR_ROOT}/patches/0078-ipro7-latch-post-pds-event-start-sleep-state.patch
	    "post-PDS event-start sleep/control state latch patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_CON_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0099-ipro7-gate-connection-trace-storage.patch
    "connection trace storage gating patch")
configure_file(
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/rwip/src/rwip_driver.c
    ${IPRO_BLE_CTLR_RWIP_DRIVER_SRC}
    COPYONLY)
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_RWIP_DRIVER_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0002-ipro7-compensate-rc32k-residual.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_RC32K_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_RC32K_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_RC32K_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_RC32K_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply RC32K compensation patch: "
        "${IPRO_BLE_CTLR_RC32K_PATCH_OUTPUT}${IPRO_BLE_CTLR_RC32K_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_RWIP_DRIVER_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0003-ipro7-sleep-diagnostics.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_SLEEP_DIAG_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_SLEEP_DIAG_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_SLEEP_DIAG_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_SLEEP_DIAG_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply sleep diagnostics patch: "
        "${IPRO_BLE_CTLR_SLEEP_DIAG_PATCH_OUTPUT}${IPRO_BLE_CTLR_SLEEP_DIAG_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_RWIP_DRIVER_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0006-ipro7-learn-wakeup-reserve.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_RESERVE_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_RESERVE_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_RESERVE_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_RESERVE_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply learned wake reserve patch: "
        "${IPRO_BLE_CTLR_RESERVE_PATCH_OUTPUT}${IPRO_BLE_CTLR_RESERVE_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_RWIP_DRIVER_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0042-ipro7-use-active-clock-when-native-sleep-disabled.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_ACTIVE_CLOCK_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_ACTIVE_CLOCK_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_ACTIVE_CLOCK_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_ACTIVE_CLOCK_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply active-clock no-sleep patch: "
        "${IPRO_BLE_CTLR_ACTIVE_CLOCK_PATCH_OUTPUT}${IPRO_BLE_CTLR_ACTIVE_CLOCK_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_RWIP_DRIVER_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0012-ipro7-complete-platform-pds-wake.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_PDS_WAKE_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_PDS_WAKE_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_PDS_WAKE_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_PDS_WAKE_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply platform PDS wake patch: "
        "${IPRO_BLE_CTLR_PDS_WAKE_PATCH_OUTPUT}${IPRO_BLE_CTLR_PDS_WAKE_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_RWIP_DRIVER_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0014-ipro7-rearm-arbiter-after-platform-pds.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_PDS_ARB_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_PDS_ARB_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_PDS_ARB_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_PDS_ARB_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply PDS arbiter re-arm patch: "
        "${IPRO_BLE_CTLR_PDS_ARB_PATCH_OUTPUT}${IPRO_BLE_CTLR_PDS_ARB_PATCH_ERROR}")
endif()
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_RWIP_DRIVER_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0031-ipro7-ignore-stale-arbiter-target-irq.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_STALE_ARB_IRQ_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_STALE_ARB_IRQ_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_STALE_ARB_IRQ_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_STALE_ARB_IRQ_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply stale arbiter IRQ guard: "
        "${IPRO_BLE_CTLR_STALE_ARB_IRQ_PATCH_OUTPUT}${IPRO_BLE_CTLR_STALE_ARB_IRQ_PATCH_ERROR}")
endif()
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_RWIP_DRIVER_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0080-ipro7-use-platform-elapsed-for-retained-native-wake.patch
    "retained native wake platform-elapsed correction patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_RWIP_DRIVER_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0081-ipro7-use-low-power-clock-units-for-native-sleep.patch
    "native sleep low-power-clock counter unit fix")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_RWIP_DRIVER_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0082-ipro7-initialize-native-sleep-engine-independent-of-policy.patch
    "native sleep engine initialization independent of APP policy")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_RWIP_DRIVER_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0083-ipro7-publish-native-sleep-counter-at-program-time.patch
    "publish current native sleep counter before platform PDS consumes it")
execute_process(
    COMMAND patch --batch --silent ${IPRO_BLE_CTLR_SCH_ARB_SRC}
            ${IPRO_BLE_CTLR_ROOT}/patches/0032-ipro7-trace-arbiter-start-decision.patch
    RESULT_VARIABLE IPRO_BLE_CTLR_ARB_TRACE_PATCH_RESULT
    OUTPUT_VARIABLE IPRO_BLE_CTLR_ARB_TRACE_PATCH_OUTPUT
    ERROR_VARIABLE IPRO_BLE_CTLR_ARB_TRACE_PATCH_ERROR)
if(NOT IPRO_BLE_CTLR_ARB_TRACE_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR
        "[ipro_ble_controller] failed to apply arbiter decision trace patch: "
        "${IPRO_BLE_CTLR_ARB_TRACE_PATCH_OUTPUT}${IPRO_BLE_CTLR_ARB_TRACE_PATCH_ERROR}")
endif()
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_SCH_ARB_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0061-ipro7-latch-arbiter-start-after-platform-pds.patch
    "arbiter post-PDS start latch patch")

ipro_ble_ctlr_copy_upstream_source(
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lld/lld_iso.c
    ${IPRO_BLE_CTLR_LLD_ISO_SRC})
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_ISO_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0045-ipro7-iso-hopping-sync-workaround.patch
    "ISO hopping synchronous completion patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_ISO_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0085-ipro7-trace-iso-hop-accelerator.patch
    "ISO hopping accelerator trace patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_ISO_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0086-ipro7-generate-iso-csa2-subevents-in-software.patch
    "ISO CSA2 software subevent generation patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_ISO_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0087-ipro7-bypass-broken-iso-freqsel-accelerator.patch
    "ISO frequency-selection accelerator bypass patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_ISO_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0088-ipro7-fix-iso-csa2-retransmission-sequence.patch
    "ISO CSA2 retransmission sequence patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_ISO_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0089-ipro7-use-freqsel-contiguous-output.patch
    "ISO FREQSEL contiguous hardware output patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_ISO_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0091-ipro7-publish-corrected-software-iso-csa2.patch
    "ISO corrected software CSA2 publication patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_ISO_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0097-ipro7-publish-freqsel-csa2-sequence.patch
    "ISO FREQSEL CSA2 sequence publication patch")

ipro_ble_ctlr_copy_upstream_source(
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lld/lld_isoal.c
    ${IPRO_BLE_CTLR_LLD_ISOAL_SRC})
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_ISOAL_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0046-ipro7-isoal-rx-diagnostics.patch
    "ISOAL RX diagnostics patch")

ipro_ble_ctlr_copy_upstream_source(
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lli/lli_cis.c
    ${IPRO_BLE_CTLR_LLI_CIS_SRC})
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLI_CIS_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0047-ipro7-cis-start-diagnostics.patch
    "CIS start diagnostics patch")

ipro_ble_ctlr_copy_upstream_source(
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/data_path/isoohci/isoohci_out.c
    ${IPRO_BLE_CTLR_ISOOHCI_OUT_SRC})
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_ISOOHCI_OUT_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0048-ipro7-isoohci-out-diagnostics.patch
    "ISOOHCI output diagnostics patch")

ipro_ble_ctlr_copy_upstream_source(
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lld/lld_cis.c
    ${IPRO_BLE_CTLR_LLD_CIS_SRC})
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_CIS_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0049-ipro7-cis-rx-anchor-and-diagnostics.patch
    "CIS RX anchor and diagnostics patch")

# rwble_config.h is a header, so the patched copy works by shadowing: the
# patched directory is first on the include path, and rwip_config.h's quoted
# include finds this copy before upstream's. Nothing else in upstream/ is
# touched, and the .c patches above are unaffected.
set(IPRO_BLE_CTLR_RWBLE_CONFIG_HDR ${IPRO_BLE_CTLR_PATCHED_DIR}/rwble_config.h)
ipro_ble_ctlr_copy_upstream_source(
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/api/rwble_config.h
    ${IPRO_BLE_CTLR_RWBLE_CONFIG_HDR})
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_RWBLE_CONFIG_HDR}
    ${IPRO_BLE_CTLR_ROOT}/patches/0055-ipro7-four-tx-descriptors-per-connection.patch
    "four TX descriptors per connection patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_CIS_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0084-ipro7-freeze-first-cis-sync-burst-and-trace-chmap.patch
    "CIS first sync-burst freeze and channel-map trace patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_CIS_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0090-ipro7-reanchor-cis-only-on-clean-sync.patch
    "CIS clean-sync-only re-anchor patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_CIS_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0095-ipro7-trace-cis-hardware-event-counter.patch
    "CIS hardware event-counter trace patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_CIS_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0096-ipro7-quarantine-implausible-acl-cis-reanchor.patch
    "CIS implausible ACL re-anchor quarantine patch")
ipro_ble_ctlr_apply_file_patch(
    ${IPRO_BLE_CTLR_LLD_CIS_SRC}
    ${IPRO_BLE_CTLR_ROOT}/patches/0092-ipro7-keep-cis-drift-out-of-acl-anchor.patch
    "Keep CIS descriptor drift out of ACL anchor patch")
# ---------------------------------------------------------------------------
# 功能開關 —— 對應原廠 SCons 的同名引數
# ---------------------------------------------------------------------------
set(IPRO_BLE_CTLR_BT        52)
if(IPRO_BLE_CTLR_VARIANT STREQUAL "link1_periph")
    set(IPRO_BLE_CTLR_CON 1)
    set(IPRO_BLE_CTLR_ACT 5)
    set(IPRO_BLE_CTLR_TX_BUF 2)
    set(IPRO_BLE_CTLR_ROLE_DEFS CFG_PERIPHERAL CFG_BROADCASTER)
elseif(IPRO_BLE_CTLR_VARIANT STREQUAL "link1")
    set(IPRO_BLE_CTLR_CON 1)
    set(IPRO_BLE_CTLR_ACT 7)
    set(IPRO_BLE_CTLR_TX_BUF 2)
    set(IPRO_BLE_CTLR_ROLE_DEFS CFG_ALLROLES CFG_CENTRAL CFG_PERIPHERAL CFG_OBSERVER CFG_BROADCASTER)
elseif(IPRO_BLE_CTLR_VARIANT STREQUAL "link2")
    set(IPRO_BLE_CTLR_CON 2)
    set(IPRO_BLE_CTLR_ACT 7)
    set(IPRO_BLE_CTLR_TX_BUF 4)
    set(IPRO_BLE_CTLR_ROLE_DEFS CFG_ALLROLES CFG_CENTRAL CFG_PERIPHERAL CFG_OBSERVER CFG_BROADCASTER)
else()
    set(IPRO_BLE_CTLR_CON 4)
    set(IPRO_BLE_CTLR_ACT 8)
    set(IPRO_BLE_CTLR_TX_BUF 6)
    set(IPRO_BLE_CTLR_ROLE_DEFS CFG_ALLROLES CFG_CENTRAL CFG_PERIPHERAL CFG_OBSERVER CFG_BROADCASTER)
endif()

if(DEFINED CONFIG_BT_CONN AND NOT CONFIG_BT_CONN EQUAL IPRO_BLE_CTLR_CON)
    message(FATAL_ERROR "[ipro_ble_controller] host CONFIG_BT_CONN=${CONFIG_BT_CONN} does not match ${IPRO_BLE_CTLR_VARIANT} capacity ${IPRO_BLE_CTLR_CON}")
endif()
set(IPRO_BLE_CTLR_RAL        3)   # BLE_RAL_MAX
set(IPRO_BLE_CTLR_ISO_CON    0)
set(IPRO_BLE_CTLR_ECC_P256  ON)   # → CFG_ECC_P256_SUPPORT（LE Secure Connections 需要）
set(IPRO_BLE_CTLR_RF    ipro7)   # 對應原廠的 RF=<name>
                        #   ipro7 → 我們自己的 rf/rf_ipro7.c
                        #   其他  → upstream/src/modules/rf/src/rf_<name>.c
                        #
                        # 🔴 原本計畫「Phase 1 先用 extrc 驗鏈路」不可行：
                        #    rf_extrc.c 需要 reg_iqgen.h（DF/IQ 產生器），
                        #    而那個標頭**整份原廠 drop 裡都沒有**，IPRO7 的
                        #    38 個生成標頭也沒有。extrc 對這顆矽編不出來。
set(IPRO_BLE_CTLR_PAST      OFF)
set(IPRO_BLE_CTLR_PAWR      OFF)
set(IPRO_BLE_CTLR_PER_ADV   OFF)
set(IPRO_BLE_CTLR_CIS       OFF)
set(IPRO_BLE_CTLR_BIS       OFF)
set(IPRO_BLE_CTLR_ADV_EXT   OFF)

if(IPRO_BLE_CTLR_FEATURE STREQUAL "bt54_iso2")
    set(IPRO_BLE_CTLR_BT       54)
    set(IPRO_BLE_CTLR_ISO_CON   2)
    set(IPRO_BLE_CTLR_PAST      ON)
    set(IPRO_BLE_CTLR_PAWR      ON)
    set(IPRO_BLE_CTLR_PER_ADV   ON)
    set(IPRO_BLE_CTLR_CIS       ON)
    set(IPRO_BLE_CTLR_BIS       ON)
    set(IPRO_BLE_CTLR_ADV_EXT   ON)
endif()

# ---------------------------------------------------------------------------
# 來源 —— upstream/src/modules
# ---------------------------------------------------------------------------
set(IPRO_BLE_CTLR_SRC
    ${IPRO_BLE_CTLR_PLF}/arch/main/ipro_ble_ctlr_em_layout.c
    # modules/rwip —— rwip_chsd.c / rwip_clock_capture.c 見下方存在性過濾
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/rwip/src/rwip.c
    ${IPRO_BLE_CTLR_RWIP_DRIVER_SRC}
    # modules/ke
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/ke/src/ke.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/ke/src/ke_event.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/ke/src/ke_mem.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/ke/src/ke_msg.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/ke/src/ke_queue.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/ke/src/ke_task.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/ke/src/ke_timer.c
    # modules/common
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/common/src/co_buf.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/common/src/co_djob.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/common/src/co_list.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/common/src/co_time.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/common/src/co_utils.c
    # modules/rwtl —— HCI 傳輸層。mbtl.c 只在 MBTL_SUPPORT 時需要，我們用 H4
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/rwtl/src/rwtl.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/rwtl/src/h4tl.c
    # modules/nvds
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/nvds/src/nvds.c
    # modules/dbg
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/dbg/src/dbg.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/dbg/src/dbg_task.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/dbg/src/dbg_swdiag.c
    # ip/hci
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/hci/src/hci.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/hci/src/hci_tl.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/hci/src/hci_msg.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/hci/src/hci_fc.c
    # ip/ll/sch
    ${IPRO_BLE_CTLR_SCH_ARB_SRC}
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/sch/src/sch_alarm.c
    ${IPRO_BLE_CTLR_SCH_PROG_SRC}
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/sch/src/sch_plan.c
    # ip/ll/ble/co
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/co/rwble.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/co/ble_util.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/co/ble_util_buf.c
    # ip/ll/ble/llm —— Link Manager
    ${IPRO_BLE_CTLR_LLM_SRC}
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llm/llm_task.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llm/llm_hci.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llm/llm_adv.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llm/llm_scan.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llm/llm_init.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llm/llm_test.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llm/llm_ch_scan.c
    # ip/ll/ble/llc —— Link Controller
    ${IPRO_BLE_CTLR_LLC_SRC}
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_hci.c
    ${IPRO_BLE_CTLR_LLC_TASK_SRC}
    ${IPRO_BLE_CTLR_LLC_LLCP_SRC}
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_disconnect.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_ver_exch.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_encrypt.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_le_ping.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_feat_exch.c
    ${IPRO_BLE_CTLR_LLC_DL_UPD_SRC}
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_con_upd.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_chmap_upd.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_phy_upd.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_cte.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_clk_acc.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_dbg.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_pwr.c
    # ip/ll/ble/lld —— Link Driver
    ${IPRO_BLE_CTLR_LLD_CORE_SRC}
    ${IPRO_BLE_CTLR_LLD_ADV_SRC}
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lld/lld_per_adv.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lld/lld_scan.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lld/lld_sync.c
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lld/lld_init.c
    ${IPRO_BLE_CTLR_LLD_CON_SRC}
    ${IPRO_BLE_CTLR_LLD_TEST_SRC}
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lld/lld_ch_scan.c
)

# BT >= 53 才有的 LLC 檔（原式：#if (int(env['BT']) >= 53)）
if(IPRO_BLE_CTLR_BT GREATER_EQUAL 53)
    list(APPEND IPRO_BLE_CTLR_SRC
        ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_subrate.c
        ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_ch_class.c)
endif()

# 原式：#if (env['PAST'] == 'on')
if(IPRO_BLE_CTLR_PAST)
    list(APPEND IPRO_BLE_CTLR_SRC ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_past.c)
endif()

# 原式：#if (env['PAWR'] == 'on')
if(IPRO_BLE_CTLR_PAWR)
    list(APPEND IPRO_BLE_CTLR_SRC
        ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lld/lld_pawr.c
        ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lld/lld_sync_pawr.c)
endif()

# 原式：#if (int(env['ISO_CON']) > 0) / CIS / BIS
if(IPRO_BLE_CTLR_ISO_CON GREATER 0)
    list(APPEND IPRO_BLE_CTLR_SRC
        ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lli/lli.c
        ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lli/lli_task.c
        ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lli/lli_data_path.c
        ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lli/lli_test.c
        ${IPRO_BLE_CTLR_UPSTREAM}/modules/data_path/src/data_path.c
        ${IPRO_BLE_CTLR_UPSTREAM}/modules/data_path/src/codec_db.c)
endif()
if(IPRO_BLE_CTLR_CIS OR IPRO_BLE_CTLR_BIS)
    list(APPEND IPRO_BLE_CTLR_SRC
        ${IPRO_BLE_CTLR_LLD_ISO_SRC}
        ${IPRO_BLE_CTLR_LLD_ISOAL_SRC}
        ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/data_path/isoohci/isoohci_in.c
        ${IPRO_BLE_CTLR_ISOOHCI_OUT_SRC})
endif()
if(IPRO_BLE_CTLR_CIS)
    list(APPEND IPRO_BLE_CTLR_SRC
        ${IPRO_BLE_CTLR_LLI_CIS_SRC}
        ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc/llc_cis.c
        ${IPRO_BLE_CTLR_LLD_CIS_SRC})
endif()
if(IPRO_BLE_CTLR_BIS)
    list(APPEND IPRO_BLE_CTLR_SRC
        ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lli/lli_bis.c
        ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lld/lld_bis.c)
endif()

# ---------------------------------------------------------------------------
# 來源 —— 移植層（plf/ipro7）
# ---------------------------------------------------------------------------
list(APPEND IPRO_BLE_CTLR_SRC
    ${IPRO_BLE_CTLR_PLF}/arch/main/arch_main.c
    ${IPRO_BLE_CTLR_PLF}/driver/ble_clk/ble_clk.c
    ${IPRO_BLE_CTLR_PLF}/driver/ble_irq/ble_irq.c
    ${IPRO_BLE_CTLR_HCITL}/ipro_ble_ctlr_hcitl.c
    ${IPRO_BLE_CTLR_PLF}/nvds/ipro_ble_ctlr_nvds.c
    ${IPRO_BLE_CTLR_PLF}/lowpower/ipro_ble_ctlr_lowpower.c
    ${IPRO_BLE_CTLR_PLF}/lowpower/ipro_ble_ctlr_pds_snapshot.c
)

# ---------------------------------------------------------------------------
# 來源 —— security（照抄 upstream/src/modules/security/config/sourcelist.txt）
#
# 我們的組態：BLE_EMB_PRESENT=on、**BLE_HOST_PRESENT=off**、ECC_P256_PRESENT=on，
# BHSU / MESH / EAD / GFP_PROVIDER / SHA / RSA / CHSD / SEC_TEST_API / BT_EMB 全關。
#
# 因此**不編**（每一項都對得上 sourcelist.txt 的條件）：
#   aes_s1/k1/c1/f4/f5/f6/g2   → #if BLE_HOST_PRESENT == "on"（SMP 配對用，host 側）
#   aes_ccm                    → #if BLE_HOST_PRESENT and (MESH or EAD)
#   aes_128_sw                 → #if SW_AES_CIPHER or SW_AES_DECIPHER（我們用 BLE 核心的硬體 AES）
#   ecc_p192                   → #if BT_EMB_PRESENT == "on"（傳統藍牙）
#   sha/* rsa/*                → #if SHA_PRESENT / RSA_PRESENT
#   security_dbg_test.c        → #if SEC_TEST_API and BLE_HOST_PRESENT
# ---------------------------------------------------------------------------
list(APPEND IPRO_BLE_CTLR_SRC
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/security/src/aes/aes.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/security/src/aes/aes_cmac.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/security/src/aes/aes_h6.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/security/src/aes/aes_h7.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/security/src/aes/aes_h8.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/security/src/aes/aes_h9.c
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/security/src/aes/aes_rpa.c
)
if(IPRO_BLE_CTLR_ECC_P256)
    list(APPEND IPRO_BLE_CTLR_SRC ${IPRO_BLE_CTLR_UPSTREAM}/modules/security/src/ecc/ecc_p256.c)
endif()

# ---------------------------------------------------------------------------
# 來源 —— RF（原廠 sourcelist.txt 就一行：rf_${RF}.c）
# ---------------------------------------------------------------------------
if(IPRO_BLE_CTLR_RF STREQUAL "ipro7")
    list(APPEND IPRO_BLE_CTLR_SRC ${IPRO_BLE_CTLR_ROOT}/rf/rf_ipro7.c)
else()
    list(APPEND IPRO_BLE_CTLR_SRC ${IPRO_BLE_CTLR_UPSTREAM}/modules/rf/src/rf_${IPRO_BLE_CTLR_RF}.c)
endif()

# 存在性過濾：原廠的 build_regs/sourcelist 會靜默略過不存在的檔
# （例如這份 drop 沒有 CHSD，rwip_chsd.c 不存在）。同樣行為，但**出聲**——
# 靜默略過正是舊樹「以為編到了其實沒有」那類問題的來源。
set(IPRO_BLE_CTLR_SRC_FINAL "")
foreach(f ${IPRO_BLE_CTLR_SRC})
    if(EXISTS ${f})
        list(APPEND IPRO_BLE_CTLR_SRC_FINAL ${f})
    else()
        message(WARNING "[ipro_ble_controller] source listed but missing, skipped: ${f}")
    endif()
endforeach()

__component_add_sources(${IPRO_BLE_CTLR_SRC_FINAL})

# ---------------------------------------------------------------------------
# Include 路徑
# ---------------------------------------------------------------------------
__component_add_include_dirs(
    ${IPRO_BLE_CTLR_PATCHED_DIR}
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/rwip/api
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/common/api
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/hci/api
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lld # patched lld_adv.c keeps source-local includes
    ${IPRO_BLE_CTLR_PLF}/arch
    ${IPRO_BLE_CTLR_PLF}/arch/main       # ipro_ble_ctlr_arch.h —— 啟動與喚醒
    ${IPRO_BLE_CTLR_PLF}/nvds            # ipro_ble_ctlr_nvds.h —— BD address 查詢
    ${IPRO_BLE_CTLR_PLF}/lowpower        # ipro_ble_ctlr_lowpower.h —— 保活低功耗策略/診斷
    ${IPRO_BLE_CTLR_HCITL}               # ipro_ble_ctlr_hcitl.h —— host 端的位元組流介面
)

__component_add_private_include_dirs(
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/rwip/src
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/ke/api
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/rwtl/api
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/nvds/api
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/dbg/api
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/rf/api
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/data_path/api
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/security/api
    ${IPRO_BLE_CTLR_UPSTREAM}/modules/display/api
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/hci/src
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/api
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llm # patched llm.c keeps source-local includes
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/lli # patched lli_cis.c keeps source-local includes
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/llc # patched llc_llcp.c keeps source-local includes
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/ble/src/data_path/isoohci/inc
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/em/api
    ${IPRO_BLE_CTLR_UPSTREAM}/ip/ll/sch/api
    ${IPRO_BLE_CTLR_PLF}/arch/compiler            # 讓 "riscvgcc/compiler.h" 解析得到
    ${IPRO_BLE_CTLR_PLF}/arch/compiler/riscvgcc
    ${IPRO_BLE_CTLR_PLF}/arch/ll
    ${IPRO_BLE_CTLR_PLF}/arch/boot
    ${IPRO_BLE_CTLR_PLF}/arch/ll/riscvgcc
    ${IPRO_BLE_CTLR_PLF}/arch/boot/riscvgcc
    ${IPRO_BLE_CTLR_PLF}/driver/reg
    ${IPRO_BLE_CTLR_PLF}/driver/ble_clk
    ${IPRO_BLE_CTLR_PLF}/driver/ble_irq
    ${IPRO_BLE_CTLR_PLF}/driver/dma
    ${IPRO_BLE_CTLR_PLF}/driver/led
    ${IPRO_BLE_CTLR_PLF}/driver/flash
)

__component_add_include_dirs(${IPRO_BLE_CTLR_ROOT}/include)

# ---------------------------------------------------------------------------
# CFG_* 定義 —— 對應 upstream/src/*/config/SCompile 的推導
# ---------------------------------------------------------------------------
set(IPRO_BLE_CTLR_DEFS
    CFG_BLE CFG_BLE_ENABLE CFG_BLE_EMB      # → BLE_EMB_PRESENT=1, EMB_PRESENT=1
    CFG_ONCHIP_HCI CFG_HCITL CFG_H4TL      # → on-chip byte-stream HCI TL
    CFG_RTOS CFG_FREERTOS CFG_OS=1          # → RTOS_SUPPORT
    CFG_NVDS                                # → NVDS_SUPPORT
    # NVDS 放 RAM。上游 SCons 也有這個一級選項（scutils.py:780 NVDS_RAM）。
    #
    # 🔑 這一項省掉一整個 flash 後端。RAM 模式下 nvds.c 的 nvds_flash_init()
    #    整段編不進去，而那個函式會把 flash_identify() 的結果比對三個**寫死的**
    #    型號常數（NUMONYX_M25P128 / INTEL_28F320C3 / INTEL_28F800C3），
    #    對不上就 nvds_null_init() + NVDS_FAIL。IPRO7 的 flash 不是那三個裡的
    #    任何一個，走 flash 模式就得謊報型號才能過關。
    #
    #    持久化改由移植層用公開的 nvds_get()/nvds_put() 對 SDK 的 flash
    #    分割區存取（Phase 4），完全在 upstream 之外。
    CFG_NVDS_RAM                            # → NVDS_RAM_MODE=1
    CFG_ECC_P256_SUPPORT
    # Upstream rf.h requires one of its built-in RF selector macros.  IPRO7's
    # actual RF implementation is rf/rf_ipro7.c below; CFG_RF_EXTRC is kept
    # only to select the minimal upstream RF vtable header and must not be
    # interpreted as using upstream rf_extrc.c or external-RF timing.
    CFG_RF_EXTRC
    ARCH_RISCV IPRO_BLE IPRO7 CFG_IPRO7
    ${IPRO_BLE_CTLR_ROLE_DEFS}
    # 尺寸
    CFG_CON=${IPRO_BLE_CTLR_CON}
    CFG_ACT=${IPRO_BLE_CTLR_ACT}
    CFG_RAL=${IPRO_BLE_CTLR_RAL}
    CFG_BLE_TX_BUFF_DATA=${IPRO_BLE_CTLR_TX_BUF}
)
# 注意：**沒有** CFG_BLE_HOST → HOST_PRESENT=0，RivieraWaves host 不編。
# host 是 Zephyr blestack，透過 H4 位元組流 HCI 溝通（見 PROVENANCE.md 規則 3）。

if(CONFIG_IPRO_BLE_CTLR_CFG_DBG)
    list(APPEND IPRO_BLE_CTLR_DEFS CFG_DBG) # → RW_DEBUG/RW_SWDIAG/KE_PROFILING, PLF_DEBUG
endif()

if(IPRO_BLE_CTLR_BT GREATER_EQUAL 53)
    list(APPEND IPRO_BLE_CTLR_DEFS CFG_BT_53)
    if(CONFIG_IPRO_BLE_CTLR_SUBRATING)
        list(APPEND IPRO_BLE_CTLR_DEFS CFG_LE_SUBRATING)
    endif()
endif()
if(IPRO_BLE_CTLR_BT GREATER_EQUAL 54)
    list(APPEND IPRO_BLE_CTLR_DEFS CFG_BT_54)
    list(APPEND IPRO_BLE_CTLR_DEFS ${IPRO_BLE_CTLR_CSSA_DEFS})
endif()
if(IPRO_BLE_CTLR_PER_ADV)
    list(APPEND IPRO_BLE_CTLR_DEFS CFG_PER_ADV)
endif()
if(IPRO_BLE_CTLR_PAST)
    list(APPEND IPRO_BLE_CTLR_DEFS CFG_PAST)
endif()
if(IPRO_BLE_CTLR_PAWR)
    list(APPEND IPRO_BLE_CTLR_DEFS CFG_PAWR)
endif()
if(IPRO_BLE_CTLR_ADV_EXT)
    list(APPEND IPRO_BLE_CTLR_DEFS CFG_ADV_EXT)
endif()
if(IPRO_BLE_CTLR_ISO_CON GREATER 0)
    list(APPEND IPRO_BLE_CTLR_DEFS CFG_ISO_CON=${IPRO_BLE_CTLR_ISO_CON} CFG_ISOOHCI)
endif()
if(IPRO_BLE_CTLR_CIS)
    list(APPEND IPRO_BLE_CTLR_DEFS CFG_CIS)
endif()
if(IPRO_BLE_CTLR_BIS)
    list(APPEND IPRO_BLE_CTLR_DEFS CFG_BIS)
endif()

__component_add_private_compile_definitions(${IPRO_BLE_CTLR_DEFS})

# ---------------------------------------------------------------------------
# 🔴 rf_init 名稱衝突 —— 不動上游的解法
#
# 上游呼叫 rf_init(struct rwip_rf_api *)（rwip.c:831）。
# 但 SDK 的 components/wireless/rf/ipro7_rf 也有一個 rf_init(uint32_t)，
# 是**射頻前端**的初始化，由 bz_phy_reset() 呼叫 —— 完全不同的函式、
# 同一個名字，連結時直接 multiple definition。
#
# 舊樹的解法是改上游：btblecontroller/rwip/src/rwip.c 第 101 行 extern、
# 第 875 行呼叫，都被改成 btble_rf_init。那正是規則 1 要避免的事。
#
# 這裡改用編譯期改名。這是 **private** 定義，只作用在 ipro_ble_controller 自己的
# 編譯單元：上游的呼叫端與我們 rf/rf_ipro7.c 的定義端**同時**被改成
# ipro_ble_ctlr_rf_init，兩邊一致，upstream/ 一個字都沒動。
#
# 前提（已確認）：ipro7_phy.h 只宣告 bz_phy_*，不宣告 rf_init，
# 所以我們的 rf_ipro7.c 同時 include 兩者也不會產生衝突的宣告。
# 若日後 ipro7_phy.h 加進 rf_init 的宣告，這裡會編譯失敗而不是靜默出錯。
# ---------------------------------------------------------------------------
__component_add_private_compile_definitions(-Drf_init=ipro_ble_ctlr_rf_init)

message(STATUS "[ipro_ble_controller] RivieraWaves v14.0.2 + plf/ipro7 + IPRO patches")
message(STATUS "[ipro_ble_controller]   LIB=${IPRO_BLE_CTLR_LIBRARY_NAME} FEATURE=${IPRO_BLE_CTLR_FEATURE} BT=${IPRO_BLE_CTLR_BT} ACT=${IPRO_BLE_CTLR_ACT} CON=${IPRO_BLE_CTLR_CON} ISO_CON=${IPRO_BLE_CTLR_ISO_CON}")
list(LENGTH IPRO_BLE_CTLR_SRC_FINAL _n)
message(STATUS "[ipro_ble_controller]   ${_n} source files")

# ipro_ble_controller — shared IPRO BLE controller package.
#
# IPRO6S keeps its already-running BT 5.4 controller backend while its
# platform integration is migrated out of components/wireless/btdm.
# Keep that backend isolated from the IPRO7 source and patch set: a
# platform packaging change must not silently become a controller-core upgrade.
if(CONFIG_BTDM_ENABLE OR CONFIG_IPRO_BLE_CTLR_PLATFORM_IPRO6S)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ipro6s.cmake)
    return()
endif()

# IPRO7 BLE controller
#
# 取代上游的 SCons 建置。來源清單與 CFG_* 定義的權威是
# upstream/src/*/config/{sourcelist.txt,SCompile}；這裡是照抄，
# 條件式用 CMake if() 鏡射原本的 python 條件並在註解裡標出原式。
#
# Customer archives encode both resource sizing and the controller feature set.

# ---------------------------------------------------------------------------
# 註冊成 SDK 元件
#
# 🔴 allow_warning = **1**（不加 -Werror）。
#
# upstream/ 是上游原始碼且**唯讀**。上游的程式碼會產生警告（未使用參數、
# 有號/無號比較之類），而規則 1 說我們不能改它。把警告當錯誤的話，
# 唯一的出路就是去改上游 —— 那正是這棵樹存在要避免的事。
#
# 我們自己寫的移植層要不要 -Werror 是另一回事，將來可以對
# plf/ ‧ hcitl/ ‧ rf/ 個別加 target_compile_options，不必連累 upstream/。
# ---------------------------------------------------------------------------
# The build description (variant/feature selection, patched sources, include
# directories, defines) lives in cmake/ipro7_build.cmake so the Zephyr module
# can build the same controller from source; this file only wires it to the
# SDK component macros.
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ipro7_build.cmake)

foreach(_d ${IPRO_BLE_CTLR_GLOBAL_DEFS})
    add_compile_definitions(${_d})
endforeach()

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


__component_add_sources(${IPRO_BLE_CTLR_SRC_FINAL})
__component_add_include_dirs(${IPRO_BLE_CTLR_PUBLIC_INC})
__component_add_private_include_dirs(${IPRO_BLE_CTLR_PRIVATE_INC})
__component_add_private_compile_definitions(${IPRO_BLE_CTLR_DEFS})
__component_add_private_compile_options(${IPRO_BLE_CTLR_COMPILE_OPTIONS})


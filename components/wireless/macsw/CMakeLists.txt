## Public include paths exported to project level
__component_add_include_dirs(include)

# ---------------------------------------------------------------------------
# PUBLIC-scope knobs
#
# These have to be emitted on BOTH the prebuilt and the source path. They are
# PUBLIC precisely because consumers outside macsw (wifi6, ipro6le_demo)
# declare the same objects, and the note below spells out what happens when
# only one side sees them: the link succeeds and the two sides disagree about
# an object's size. A prebuilt build that skipped them would reintroduce
# exactly that, with no diagnostic.
# ---------------------------------------------------------------------------
#
# PUBLIC vs PRIVATE is not a style choice. A knob that DIMENSIONS AN OBJECT a consumer
# outside macsw also declares must be PUBLIC: defining it for macsw alone links cleanly
# and leaves the two sides disagreeing about the size of one object. PRIVATE is only for
# a knob no translation unit outside this component reads. Each call says which and why.
function(__macsw_debug_knob name scope)
    set(val "${${name}}")
    if(val STREQUAL "" AND DEFINED ENV{${name}})
        set(val "$ENV{${name}}")
    endif()
    if(val STREQUAL "")
        return()
    endif()
    if(scope STREQUAL "PUBLIC")
        __component_add_compile_definitions(-D${name}=${val})
    else()
        __component_add_private_compile_definitions(-D${name}=${val})
    endif()
endfunction()

# AB_TRACE ring depth, entries, power of 2. Default 16 fits the tightest config
# (defconfig_ipro6_sdio has ~1.1 KB above its ram_tcm heap floor). RC_P9_0809.md
__macsw_debug_knob(AB_TRACE_SIZE PUBLIC)          # also read by wifi6, ipro6le_demo

# Spare A-MPDU descriptors per data AC, parked on a reserve list and moved by the
# `aggpool` command so both A/B arms come from one image. (N x 4) x 376 B ram_wifi.
__macsw_debug_knob(TX_AGG_POOL_EXTRA PUBLIC)      # also read by wifi6 cli_al.c

# How many RX-ring desync rejections to record WHOLE rather than just count. The
# range guards themselves are unconditional - they are the fix. RC_P27_0810.md
__macsw_debug_knob(RXL_DESYNC_LOG PUBLIC)         # also read by wifi6 cli_al.c

# P28 only: move RX buffer 1 by N bytes to decide whether the fault follows the
# ADDRESS or the offset. Single-file, so it stays inside macsw.

# LP beacon-RX firmware trim. PUBLIC, and it changes what is compiled into the
# archive, so a prebuilt built without it cannot serve a CONFIG_PHY_LPFW build
# -- the check in the prebuilt branch below refuses that combination instead of
# letting it link.
if(CONFIG_PHY_LPFW)
    __component_add_compile_definitions(-DIPRO_WIFI_LP_FW=1)
endif()

# ---------------------------------------------------------------------------
# Prebuilt archive
#
# Same contract as ipro_ble_controller: an explicit path wins, otherwise the
# component's own lib/ is used when present, and CONFIG_MACSW_FORCE_SOURCE_BUILD
# always forces sources. This is what keeps the RivieraWaves Wi-Fi MAC sources
# out of a customer SDK.
# ---------------------------------------------------------------------------
if(DEFINED CONFIG_MACSW_PREBUILT_PATH AND NOT "${CONFIG_MACSW_PREBUILT_PATH}" STREQUAL "")
    set(MACSW_PREBUILT "${CONFIG_MACSW_PREBUILT_PATH}")
elseif(CONFIG_PHY_LPFW)
    set(MACSW_PREBUILT "${CMAKE_CURRENT_SOURCE_DIR}/lib/libmacsw_lpfw.a")
else()
    set(MACSW_PREBUILT "${CMAKE_CURRENT_SOURCE_DIR}/lib/libmacsw.a")
endif()

if((CONFIG_MACSW_PREBUILT OR EXISTS "${MACSW_PREBUILT}") AND NOT CONFIG_MACSW_FORCE_SOURCE_BUILD)
    if(NOT EXISTS "${MACSW_PREBUILT}")
        message(FATAL_ERROR
            "[macsw] prebuilt requested but not found: ${MACSW_PREBUILT}\n"
            "        CONFIG_PHY_LPFW builds need the _lpfw archive; set "
            "CONFIG_MACSW_FORCE_SOURCE_BUILD=y to build from sources instead.")
    endif()
    set(CURRENT_STATIC_LIBRARY macsw)
    add_library(${CURRENT_STATIC_LIBRARY} STATIC IMPORTED GLOBAL)
    set_target_properties(${CURRENT_STATIC_LIBRARY} PROPERTIES IMPORTED_LOCATION ${MACSW_PREBUILT})
    set_property(GLOBAL APPEND PROPERTY SDK_LIBS ${CURRENT_STATIC_LIBRARY})
    # INTERFACE, not PUBLIC: CMake rejects PUBLIC/PRIVATE on an IMPORTED target.
    # The original line here said PUBLIC, which means this branch had never once
    # been executed -- the prebuilt path existed but nothing ever took it.
    target_link_libraries(${CURRENT_STATIC_LIBRARY} INTERFACE __component_lib)
    message(STATUS "MACSW: using prebuilt archive at ${MACSW_PREBUILT}")
    return()
endif()

__generate_library(1)

__component_add_private_include_dirs(.)
__component_add_private_include_dirs(plf_build_reg)

# Tell export headers to skip re-definitions when compiling macsw internally
# Use PRIVATE so this define does NOT leak to wifi6 and other consumers
__component_add_private_compile_options(-DMACSW_INTERNAL)

# WiFi datapath resume across PDS (CONFIG_IPRO_WIFI_PS, lpwifi carrier build).
# Enables the ported keyram save/restore + mm_ps_init + rwnxl_resume_wifi datapath
# so the full FW can re-program the MAC HW after a lp_fw WIFI-wake. PRIVATE: the
# lp_demo caller reaches rwnxl_pds_store/rwnxl_resume_wifi via extern decls, so this
# gate stays internal to macsw and does not leak to fhost/wifi6 consumers.
if(CONFIG_IPRO_WIFI_PS)
    __component_add_private_compile_options(-DCFG_IPRO_WIFI_PS_ENABLE)
endif()

# Bench debug knobs. Taken from -D<NAME>= or from the environment, so an A/B can be
# built without editing a defconfig:  AB_TRACE_SIZE=512 make
#
# UNSET is the shipped state for every one of them: the header keeps the default and
# the build is byte-identical, which is why these live here rather than in a defconfig.
# (moved above the prebuilt branch: see the PUBLIC-scope note there)

__macsw_debug_knob(RXL_BUF1_SHIFT PRIVATE)

# Release module include paths FIRST to win guard collisions vs export headers
__component_add_private_include_dirs(modules/common/src)
__component_add_private_include_dirs(modules/ke/src)
__component_add_private_include_dirs(modules/dbg/src)
__component_add_private_include_dirs(modules/macif/src)
__component_add_private_include_dirs(modules/mac/src)
__component_add_private_include_dirs(modules/adapter/src)
__component_add_private_include_dirs(modules/coexist)

# LMAC internal include paths (before export to let release headers win)
__component_add_private_include_dirs(ip/lmac/src/rwnx)
__component_add_private_include_dirs(ip/lmac/src/rc)
__component_add_private_include_dirs(ip/lmac/src/tx)
__component_add_private_include_dirs(ip/lmac/src/tx/txl)
__component_add_private_include_dirs(ip/lmac/src/rx)
__component_add_private_include_dirs(ip/lmac/src/rx/rxl)
__component_add_private_include_dirs(ip/lmac/src/mm)
__component_add_private_include_dirs(ip/lmac/src/chan)
__component_add_private_include_dirs(ip/lmac/src/tdls)
__component_add_private_include_dirs(ip/lmac/src/p2p)
__component_add_private_include_dirs(ip/lmac/src/ps)
__component_add_private_include_dirs(ip/lmac/src/sta)
__component_add_private_include_dirs(ip/lmac/src/scan)
__component_add_private_include_dirs(ip/lmac/src/vif)
__component_add_private_include_dirs(ip/lmac/src/hal)
__component_add_private_include_dirs(ip/lmac/src/rd)
__component_add_private_include_dirs(ip/lmac/src/uf)
__component_add_private_include_dirs(ip/lmac/src/td)
__component_add_private_include_dirs(ip/lmac/src/tpc)
__component_add_private_include_dirs(ip/lmac/src/bfr)

# UMAC internal include paths (before export)
__component_add_private_include_dirs(ip/umac/src/scanu)
__component_add_private_include_dirs(ip/umac/src/rxu)
__component_add_private_include_dirs(ip/umac/src/me)
__component_add_private_include_dirs(ip/umac/src/sm)
__component_add_private_include_dirs(ip/umac/src/apm)
__component_add_private_include_dirs(ip/umac/src/txu)
__component_add_private_include_dirs(ip/umac/src/llc)
__component_add_private_include_dirs(ip/umac/src/bam)
__component_add_private_include_dirs(ip/umac/src/mfp)
__component_add_private_include_dirs(ip/umac/src/hsu)
__component_add_private_include_dirs(ip/umac/src/rm)
__component_add_private_include_dirs(ip/umac/src/twt)
__component_add_private_include_dirs(ip/umac/src/ftm)

# Export paths LAST (fallback for headers not in release modules)
__component_add_private_include_dirs(include/export)
__component_add_private_include_dirs(include/export/common)
__component_add_private_include_dirs(include/export/ke)
__component_add_private_include_dirs(include/export/mac)
__component_add_private_include_dirs(include/export/adapter)
__component_add_private_include_dirs(include/export/coexist)
__component_add_private_include_dirs(include/export/dbg)

# Platform include paths
__component_add_private_include_dirs(plf/refip/src/arch)
__component_add_private_include_dirs(plf/refip/src/arch/risc-v)
__component_add_private_include_dirs(plf/refip/src/arch/risc-v/boot)
__component_add_private_include_dirs(plf/refip/src/arch/risc-v/boot/riscv32)
__component_add_private_include_dirs(plf/refip/src/arch/risc-v/ll/riscv32)
__component_add_private_include_dirs(plf/refip/src/arch/risc-v/ll/thead_e907)
__component_add_private_include_dirs(plf/refip/src/driver)
__component_add_private_include_dirs(plf/refip/src/driver/reg)
__component_add_private_include_dirs(plf/refip/src/driver/crm)
__component_add_private_include_dirs(plf/refip/src/driver/phy)
__component_add_private_include_dirs(plf/refip/src/driver/intc/risc-v)
__component_add_private_include_dirs(plf/refip/src/driver/extint)
__component_add_private_include_dirs(plf/refip/src/driver/sysctrl)
__component_add_private_include_dirs(plf/refip/src/driver/ipc_fhost)
__component_add_private_include_dirs(plf/refip/src/driver/dma)
__component_add_private_include_dirs(plf/refip/src/driver/la)
__component_add_private_include_dirs(plf/refip/src/driver/time)
__component_add_private_include_dirs(plf/refip/src/driver/button)
__component_add_private_include_dirs(plf/refip/src/driver/led)

# FHOST private headers needed by release macif_fhost.c
__component_add_private_include_dirs(../wifi6/ipro_fhost/src)

# Export layer (SDK-specific API bridge)
__component_add_sources(include/export/export_func.c)
# LP beacon-RX firmware trim (CONFIG_PHY_LPFW): these three are pure UMAC message
# marshalling (export_struct/export_malloc) and fhost-TX descriptor glue
# (export_tx). The upper-MAC host API and the fhost TX path do not exist in the
# no-UMAC / no-host-networking LP build, so they cannot compile there — the
# reference lp_fw source set builds none of the export/*.c shims either. Normal
# builds compile them as before.
if(NOT CONFIG_PHY_LPFW)
__component_add_sources(include/export/export_struct.c)
__component_add_sources(include/export/export_malloc.c)
__component_add_sources(include/export/export_tx.c)
endif()

# LMAC - TX path
__component_add_sources(ip/lmac/src/tx/tx_swdesc.c)
__component_add_sources(ip/lmac/src/tx/txl/txl_buffer_shared.c)
__component_add_sources(ip/lmac/src/tx/txl/txl_buffer.c)
__component_add_sources(ip/lmac/src/tx/txl/txl_cntrl.c)
__component_add_sources(ip/lmac/src/tx/txl/txl_cfm.c)
__component_add_sources(ip/lmac/src/tx/txl/txl_frame.c)
__component_add_sources(ip/lmac/src/tx/txl/txl_frame_shared.c)
__component_add_sources(ip/lmac/src/tx/txl/txl_agg.c)
# DEBUG 2026-07-25: wedge #2 probe. Kept out of the OCRAM relocation globs on
# purpose (cold code must not spend the hot-code RAM budget).
__component_add_sources(ip/lmac/src/tx/txl/txl_wedge_dbg.c)
__component_add_sources(ip/lmac/src/tx/txl/txl_he.c)
__component_add_sources(ip/lmac/src/tx/txl/txl_mu.c)

# LMAC - RX path
__component_add_sources(ip/lmac/src/rx/rxl/rxl_cntrl.c)
__component_add_sources(ip/lmac/src/rx/rxl/v20/rxl_hwdesc.c)
__component_add_sources(ip/lmac/src/rx/rxl/rxl_tb.c)

# LMAC - core modules
__component_add_sources(ip/lmac/src/rwnx/rwnx.c)
__component_add_sources(ip/lmac/src/rd/rd.c)
__component_add_sources(ip/lmac/src/uf/uf.c)
__component_add_sources(ip/lmac/src/sta/sta_mgmt.c)
__component_add_sources(ip/lmac/src/vif/vif_mgmt.c)
__component_add_sources(ip/lmac/src/mm/mm.c)
__component_add_sources(ip/lmac/src/mm/mm_task.c)
__component_add_sources(ip/lmac/src/mm/mm_timer.c)
__component_add_sources(ip/lmac/src/mm/mm_bcn.c)
__component_add_sources(ip/lmac/src/chan/chan.c)
__component_add_sources(ip/lmac/src/scan/scan_shared.c)
__component_add_sources(ip/lmac/src/scan/scan_task.c)
__component_add_sources(ip/lmac/src/scan/scan.c)
__component_add_sources(ip/lmac/src/hal/hal_machw.c)
__component_add_sources(ip/lmac/src/hal/hal_desc.c)
__component_add_sources(ip/lmac/src/hal/hal_mib.c)
__component_add_sources(ip/lmac/src/hal/hal_dma.c)
__component_add_sources(ip/lmac/src/tpc/tpc.c)
__component_add_sources(ip/lmac/src/tdls/tdls.c)
__component_add_sources(ip/lmac/src/tdls/tdls_task.c)
__component_add_sources(ip/lmac/src/ps/ps.c)
__component_add_sources(ip/lmac/src/p2p/p2p.c)
__component_add_sources(ip/lmac/src/td/td.c)
__component_add_sources(ip/lmac/src/bfr/bfr.c)
__component_add_sources(ip/lmac/src/rc/rc.c)
__component_add_sources(ip/lmac/src/rc/lgov.c)

# Kernel Engine
__component_add_sources(modules/ke/src/ke_timer.c)
__component_add_sources(modules/ke/src/ke_msg.c)
__component_add_sources(modules/ke/src/ke_queue.c)
__component_add_sources(modules/ke/src/ke_task.c)
__component_add_sources(modules/ke/src/ke_event.c)
__component_add_sources(modules/ke/src/ke_env.c)
__component_add_sources(modules/ke/src/ke_mem.c)

# Common modules
__component_add_sources(modules/coexist/coexist.c)
__component_add_sources(modules/common/src/co_dlist.c)
__component_add_sources(modules/common/src/co_list.c)
__component_add_sources(modules/common/src/co_math.c)
__component_add_sources(modules/common/src/co_pool.c)
__component_add_sources(modules/common/src/co_ring.c)

# MAC modules
__component_add_sources(modules/mac/src/mac_ie.c)
__component_add_sources(modules/mac/src/mac.c)

# Debug modules
__component_add_sources(modules/dbg/src/dbg_print.c)
__component_add_sources(modules/dbg/src/dbg_profiling.c)
__component_add_sources(modules/dbg/src/dbg_task.c)
__component_add_sources(modules/dbg/src/dbg_assert.c)
__component_add_sources(modules/dbg/src/dbg.c)
__component_add_sources(modules/dbg/src/trace.c)

# MACIF (FHOST interface) - re-enabled for Phase 2
# LP beacon-RX firmware trim (CONFIG_PHY_LPFW): macif_fhost bridges the UMAC RX
# (rxu_stat_desc) / fhost TX (umacdesc) path to the host stack, which does not
# exist in the no-UMAC / no-host-networking LP build. Excluded there, matching
# the reference lp_fw (which uses the IPC macif variant, not the fhost one).
if(NOT CONFIG_PHY_LPFW)
__component_add_sources(modules/macif/src/macif_fhost.c)
endif()

# LP beacon-RX firmware (CONFIG_PHY_LPFW): the standalone lp_fw has NO host/IPC
# peer, so neither macif_fhost.c (fhost queues) nor macif_ipc.c (IPC transport)
# applies. macif_lp.c provides minimal, side-effect-free MAC<->host stubs: RX
# host-upload reports "no host buffer" (beacon read directly), and the never-TX
# path is no-op / "empty". Only built here; normal builds are unaffected.
if(CONFIG_PHY_LPFW)
__component_add_sources(modules/macif/src/macif_lp.c)
endif()

# RTOS adapter
__component_add_sources(modules/adapter/src/rtos.c)

# UMAC — excluded entirely from the LP beacon-RX firmware (CONFIG_PHY_LPFW).
# The standalone lp_fw never runs the upper-MAC (association/scan/AP/rate/MFP/
# TWT/FTM): with CFG_UMAC undefined these sources would not even compile, and the
# reference lp_fw source set compiles none of ip/umac/src/*. Normal builds
# compile the full UMAC as before.
if(NOT CONFIG_PHY_LPFW)
# UMAC - Management Entity
__component_add_sources(ip/umac/src/me/me_mgmtframe.c)
__component_add_sources(ip/umac/src/me/me_task.c)
__component_add_sources(ip/umac/src/me/me.c)
__component_add_sources(ip/umac/src/me/me_utils.c)
__component_add_sources(ip/umac/src/me/me_mic.c)

# UMAC - State Machine
__component_add_sources(ip/umac/src/sm/sm_task.c)
__component_add_sources(ip/umac/src/sm/sm.c)

# UMAC - AP Management
__component_add_sources(ip/umac/src/apm/apm_task.c)
__component_add_sources(ip/umac/src/apm/apm.c)

# UMAC - Scan
__component_add_sources(ip/umac/src/scanu/scanu_task.c)
__component_add_sources(ip/umac/src/scanu/scanu.c)
__component_add_sources(ip/umac/src/scanu/scanu_shared.c)

# UMAC - RX/TX upper
__component_add_sources(ip/umac/src/rxu/rxu_cntrl.c)
__component_add_sources(ip/umac/src/txu/txu_cntrl.c)

# UMAC - LLC, BAM, TWT, MFP, HSU, RM, FTM
__component_add_sources(ip/umac/src/llc/llc.c)
__component_add_sources(ip/umac/src/bam/bam_task.c)
__component_add_sources(ip/umac/src/bam/bam.c)
__component_add_sources(ip/umac/src/twt/twt.c)
__component_add_sources(ip/umac/src/twt/twt_task.c)
__component_add_sources(ip/umac/src/mfp/mfp.c)
__component_add_sources(ip/umac/src/mfp/mfp_bip.c)
__component_add_sources(ip/umac/src/hsu/hsu.c)
__component_add_sources(ip/umac/src/rm/rm_task.c)
__component_add_sources(ip/umac/src/rm/rm.c)
__component_add_sources(ip/umac/src/ftm/ftm.c)
__component_add_sources(ip/umac/src/ftm/ftm_task.c)
endif()

# Platform drivers
__component_add_sources(plf/refip/src/driver/rwnx_platform.c)
__component_add_sources(plf/refip/src/driver/crm/v30/crm.c)
__component_add_sources(plf/refip/src/driver/phy/phy_ipro6rf.c)
__component_add_sources(plf/refip/src/driver/intc/risc-v/intc.c)
__component_add_sources(plf/refip/src/driver/extint/extint.c)
__component_add_sources(plf/refip/src/driver/sysctrl/sysctrl.c)
__component_add_sources(plf/refip/src/driver/dma/dma.c)
__component_add_sources(plf/refip/src/driver/la/la.c)
__component_add_sources(plf/refip/src/driver/la/la_shared.c)
__component_add_sources(plf/refip/src/driver/la/la_mem_mac.c)
__component_add_sources(plf/refip/src/driver/time/nx_time.c)
__component_add_sources(plf/refip/src/driver/button/button.c)
__component_add_sources(plf/refip/src/driver/led/led.c)
__component_add_sources(plf/refip/src/driver/csidma/csidma_cli.c)
__component_add_sources(plf/refip/src/driver/csidma/csidma.c)
__component_add_sources(plf/refip/src/driver/csidma/csidata.c)

# Architecture
__component_add_sources(plf/refip/src/arch/risc-v/arch_main.c)

# IPC (FHOST variant)
__component_add_sources(plf/refip/src/driver/ipc_fhost/ipc_emb.c)
__component_add_sources(plf/refip/src/driver/ipc_fhost/ipc_shared.c)

# Beacon-RX LP firmware trim: activate the ~12 IPRO_WIFI_LP_FW guard sites
# that strip IPC/DMA init, static RX buffers, rate-control, and CLI paths.
# Guard ensures normal (non-LP) builds are completely unaffected.
# (IPRO_WIFI_LP_FW moved above the prebuilt branch)

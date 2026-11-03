# IPRO6 PHY/RF Driver
# Full implementation replacing the stub driver

__generate_library()

# Include directories
__component_add_include_dirs(.)
__component_add_include_dirs(inc)
__component_add_include_dirs(phy)
__component_add_include_dirs(rfc)
__component_add_include_dirs(crm)
__component_add_include_dirs(arch)
__component_add_include_dirs(reg)
__component_add_include_dirs(reg/mac154)
__component_add_include_dirs(reg/rf)
__component_add_include_dirs(reg/phybz)
__component_add_include_dirs(reg/wlan11)

# FPGA or ASIC specific includes
if(CONFIG_FPGA)
    __component_add_include_dirs(rf/fpga)
else()
    __component_add_include_dirs(rf/asic/inc)
    __component_add_private_include_dirs(rf/asic/private)
endif()

# Core source files
__component_add_sources(phy/wl_api.c)
__component_add_sources(phy/wl_internal.c)
# phy.c and phy_lp.c are two PHY variants that define the same symbols. Build
# exactly one: phy.c is the normal-fw PHY; phy_lp.c is the low-power-firmware copy,
# STARTED as a byte-copy of phy.c (which brings up RX correctly) and then trimmed for
# a faster PDS-wake init. (The old phy_lp.c did not bring up the RX front-end -- adcpow
# stayed 0 -- so it was re-based on phy.c.) The blob build (CONFIG_PHY_LPFW) uses
# phy_lp.c so its init flow can be optimized without touching the normal-fw phy.c.
if(CONFIG_PHY_LPFW)
    __component_add_sources(phy/phy_lp.c)
else()
    __component_add_sources(phy/phy.c)
endif()
__component_add_sources(phy/bba.c)
# Bluetooth/Zigbee modem/DFE reset programming is required by the IPRO6S
# controller RF callback. The implementation is part of the master RF source
# tree; include it only as the BT controller integration dependency.
__component_add_sources(phy/phy_bz.c)
__component_add_sources(phy/phy_trpc.c)
__component_add_sources(phy/phyrf_utils.c)
# LP warm-restore PHY (B2): real lp_phy_init/lp_default_init/wl_lp_config that
# override the weak no-op stubs in wl_api.c. Built alongside phy.c/phy_lp.c for
# any ASIC build (self-contained: own static rc/agc/mdm helpers, reads RF cal
# from wl_cal->lp_lo which lp_rf_pri_init already restored from WLCAL). Ported
# from the vendor ipro6_rf drop.
if(NOT CONFIG_FPGA)
    __component_add_sources(phy/lp_phy.c)
endif()
__component_add_sources(phy/wireless_config.c)
__component_add_sources(crm/crm.c)
__component_add_sources(rfc/rf.c)
__component_add_sources(rfc/rfc.c)
__component_add_sources(rfc/rfc_helper.c)

# Architecture support (when not WLAN app). phy_lp.c (the ASIC/phyrf PHY) is
# selected above in place of phy.c.
if(NOT CONFIG_APP_WLAN)
    __component_add_sources(arch/arch.c)
endif()

# AGC memory. The carrier links the 512-word agcmem.c; the blob
# (CONFIG_PHY_LPFW) links the LP program agcmem_lp.c, whose copy loops are
# bounded by its own agcmem_lp_words.
#
# Do not read the STA associating as evidence a blob table works: association
# runs on the carrier's phy.c with the full table, and never executes the blob's
# copy. Only a blob beacon-loss run tests the LP program.
if(CONFIG_PHY_LPFW)
    __component_add_sources(phy/agcmem_lp.c)
else()
    __component_add_sources(phy/agcmem.c)
endif()

# LP warm-restore RF glue: weak no-op wl_lp_config() so the standalone
# beacon-RX firmware links. TODO(B2): real warm-restore RF config.
if(CONFIG_PHY_LPFW)
    __component_add_sources(wl_lp_glue.c)
    # LPFW_CFG_RF=1 activates the rf_pri.c built-in LP trim: the wake path does
    # RF warm-restore (rf_pri_restore_cal_reg reads cal from retention) and never
    # runs the full cold cal (rf_pri_full_cal / txcal / rxcal / rccal / pwr-table),
    # which the #if LPFW_CFG_RF==0 blocks drop. Default (0) keeps the full cal for
    # normal builds. See rf_pri.c:152 ("only 11b 1M rx in LPFW").
    __component_add_compile_definitions(-DLPFW_CFG_RF=1)
endif()

# FPGA or ASIC RF implementation
if(CONFIG_FPGA)
    __component_add_sources(rf/fpga/rf_fpga.c)
else()
    __component_add_sources(rf/asic/private/rf_pri.c)
    __component_add_sources(rf/asic/private/rf_calib_data.c)
endif()

# CLI/Shell support
if((CONFIG_WIFI6 AND NOT CONFIG_IPRO6_FMAC) OR CONFIG_PHY_CLI)
    __component_add_sources(cli/cli_app.c)
    # NOTE: shell_phy.c is compiled in components/sys/shell/ to ensure
    # FSymTab symbols are linked via whole-archive
    if(CONFIG_PHY_CLI)
        __component_add_sources(cli/cli_intc.c)
    endif()
endif()

# Compile options - private to avoid affecting other components
__component_add_private_compile_options(-std=gnu99)
__component_add_private_compile_options(-ffunction-sections -fdata-sections)
__component_add_private_compile_options(-fstrict-volatile-bitfields)
__component_add_private_compile_options(-fshort-enums)
__component_add_private_compile_options(-fno-common)
__component_add_private_compile_options(-fno-builtin)
__component_add_private_compile_options(-fno-jump-tables)
__component_add_private_compile_options(-ffreestanding -fno-strict-aliasing)

# Suppress warnings
__component_add_private_compile_options(-Wno-implicit-fallthrough)
__component_add_private_compile_options(-Wno-error=empty-body)
__component_add_private_compile_options(-Wno-error=unused-function)
__component_add_private_compile_options(-Wno-error=unused-but-set-variable)
__component_add_private_compile_options(-Wno-error=unused-variable)
__component_add_private_compile_options(-Wno-error=deprecated-declarations)
__component_add_private_compile_options(-Wno-error=absolute-value)
__component_add_private_compile_options(-Wno-error=type-limits)
__component_add_private_compile_options(-Wno-error=cpp)
__component_add_private_compile_options(-Wno-unused-parameter)
__component_add_private_compile_options(-Wno-sign-compare)
__component_add_private_compile_options(-Wno-error=format=)
__component_add_private_compile_options(-Wno-error=parentheses)
__component_add_private_compile_options(-Wno-error=implicit-function-declaration)
__component_add_private_compile_options(-Wno-error=undef)

# Preprocessor definitions - chip configuration
__component_add_compile_definitions(-DWL_IPRO6=1)
__component_add_compile_definitions(-DVERSION_PHYRF="NULL")

# PHY/RF configuration
__component_add_compile_definitions(-DCFG_BZ_ONLY=0)
if(CONFIG_FPGA)
    __component_add_compile_definitions(-DCFG_FPGA=1)
else()
    __component_add_compile_definitions(-DCFG_FPGA=0)
endif()
__component_add_compile_definitions(-DCFG_PHY_ANT2_EN=0)
__component_add_compile_definitions(-DCFG_COEX_TEST=0)
__component_add_compile_definitions(-DCFG_LPFW=0)
__component_add_compile_definitions(-DCFG_PHY_CLI_SRAM=0)
__component_add_compile_definitions(-DCFG_CHIP_VERSION=1)
# ASIC-path build defines (PHY/RF config macros not otherwise provided)
__component_add_compile_definitions(-DPHYRF_LOG_EN=0)
__component_add_compile_definitions(-DCFG_RFBOARD_U=0)
__component_add_compile_definitions(-DCFG_HUALAI_T41SC401AI_OTA=0)

# Logging configuration. wl_config.h used to map wl_logprintf to printf even
# with WL_BB_LOG_EN=0, so the log was on everywhere; keep it on unless the app
# asks for it off (CONFIG_WL_BB_LOG_DISABLE, e.g. the Thread BR).
if(CONFIG_WL_BB_LOG_DISABLE)
    __component_add_compile_definitions(-DWL_BB_LOG_EN=0)
else()
    __component_add_compile_definitions(-DWL_BB_LOG_EN=1)
endif()
__component_add_compile_definitions(-DWL_RF_LOG_EN=0)
__component_add_compile_definitions(-DWL_BB_PD=1)
__component_add_compile_definitions(-DWL_BB_CE=1)
__component_add_compile_definitions(-DWL_BB_CE_LOG=0)
__component_add_compile_definitions(-DWL_NIC=0)

# RW configuration - private to avoid conflicts with macsw
__component_add_private_compile_definitions(-DRW_NX_DERIV_80211B=1)
__component_add_private_compile_definitions(-DRW_NX_IQ_COMP_EN=1)
__component_add_private_compile_definitions(-DRW_NX_FIQ_COMP_EN=0)
__component_add_private_compile_definitions(-DRW_NX_AGC_SNR_EN=1)
__component_add_private_compile_definitions(-DRW_NX_LDPC_DEC=1)
__component_add_private_compile_definitions(-DRW_BFMEE_EN=1)
__component_add_private_compile_definitions(-DRW_MUMIMO_TX_EN=0)
__component_add_private_compile_definitions(-DRW_MUMIMO_RX_EN=1)
__component_add_private_compile_definitions(-DRW_MUMIMO_SEC_USER1_EN=0)
__component_add_private_compile_definitions(-DRW_MUMIMO_SEC_USER2_EN=0)
__component_add_private_compile_definitions(-DRW_MUMIMO_SEC_USER3_EN=0)

# Version configuration - private to avoid conflicts with macsw
__component_add_private_compile_definitions(-DNX_MDM_VER=32)
__component_add_private_compile_definitions(-DNX_MAC_VER=21)
__component_add_private_compile_definitions(-DNX_PLF_VER=30)
__component_add_private_compile_definitions(-DNX_UF_EN=0)
__component_add_private_compile_definitions(-DNX_GP_DMA=0)

# Bluetooth: the shared ipro_ble_controller component and unified Zephyr 4.4
# host. IPRO7 uses the IPRO7 backend; IPRO6S uses the isolated dual-mode
# backend selected by CONFIG_BTDM_ENABLE.
#
# The previous btblecontroller/blestack/btblestack trees were removed. Any
# configuration that still selects them fails explicitly instead of silently
# falling back to an incomplete legacy tree.

# Shared configuration used by both controller backends and the host.
include(options.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/ble_common.cmake)

# ============================================================================
# Compiler Options
# ============================================================================
__component_add_compile_options(-Wno-expansion-to-defined)

# ============================================================================
# Build Sub-libraries
# ============================================================================

# ----------------------------------------------------------------------------
# Controller
# ----------------------------------------------------------------------------
if(CONFIG_BT_CTLR_IPRO OR CONFIG_BTDM_ENABLE)
    if(CONFIG_BTDM_ENABLE)
        message(STATUS "[bluetooth] Using ipro_ble_controller IPRO6S backend (dual mode)")
    else()
        message(STATUS "[bluetooth] Using ipro_ble_controller IPRO7 backend (byte-stream HCI)")
    endif()
    add_subdirectory(ipro_ble_controller)
elseif(CONFIG_BT_CTLR_AIC8800_SDIO)
    message(STATUS "[bluetooth] Using the external AIC8800 D80 controller over the SDIO mailbox HCI (aic_hci_transport.h) (no on-chip controller)")
else()
    message(FATAL_ERROR
        "[bluetooth] No controller selected. Set CONFIG_BT_CTLR_IPRO=y for "
        "IPRO7, CONFIG_BTDM_ENABLE=y for IPRO6S or CONFIG_BT_CTLR_AIC8800_SDIO=y "
        "for the AIC8800 combo chip. The legacy btblecontroller tree is no "
        "longer part of master.")
endif()

# ----------------------------------------------------------------------------
# Host Stack
# ----------------------------------------------------------------------------
# Skip host stack in controller-only mode (e.g., HCI transport mode)
if(NOT CONFIG_BLE_HOST_DISABLE)
    if(CONFIG_BT_HOST_IPRO_V44)
        message(STATUS "[bluetooth] Using unified Zephyr 4.4 host (ipro_bt_host)")
        add_subdirectory(ipro_bt_host)
    else()
        message(FATAL_ERROR
            "[bluetooth] No host selected. Set CONFIG_BT_HOST_IPRO_V44=y, "
            "or CONFIG_BLE_HOST_DISABLE=y for controller-only builds. The "
            "legacy blestack/btblestack trees are no longer part of master.")
    endif()
else()
    message(STATUS "[bluetooth] Host stack disabled (controller-only mode)")
endif()

# ----------------------------------------------------------------------------
# BLE Mesh Support
# ----------------------------------------------------------------------------
if(CONFIG_BT_MESH)
    message(FATAL_ERROR
        "[bluetooth] BLE Mesh has not yet been ported to ipro_bt_host; the "
        "legacy blemesh trees are no longer part of master.")
endif()

# ----------------------------------------------------------------------------
# Application Framework (TODO: rebuild for blestack)
# ----------------------------------------------------------------------------
# app_framework was removed as it depended on the removed legacy host stack
# Will be rebuilt for blestack in future

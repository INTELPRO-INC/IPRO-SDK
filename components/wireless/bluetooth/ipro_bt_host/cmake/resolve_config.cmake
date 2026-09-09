# SPDX-License-Identifier: Apache-2.0
#
# Resolve the subset of Zephyr's Bluetooth Kconfig select/imply graph that is
# required by the standalone IPRO SDK build.  The SDK consumes a checked-in
# .config directly; it does not run Zephyr Kconfig before CMake.  Consequently
# profile symbols must be expanded before CMake chooses source files.

macro(ipro_bt_host_enable symbol)
    if(NOT ${symbol})
        set(${symbol} 1)
    endif()
endmacro()

macro(ipro_bt_host_default symbol value)
    if(NOT DEFINED ${symbol})
        set(${symbol} ${value})
    endif()
endmacro()

# BR/EDR protocols and profiles.
if(CONFIG_BT_A2DP OR CONFIG_BT_A2DP_SOURCE OR CONFIG_BT_A2DP_SINK)
    ipro_bt_host_enable(CONFIG_BT_A2DP)
    ipro_bt_host_enable(CONFIG_BT_AVDTP)
    ipro_bt_host_enable(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
endif()
if(CONFIG_BT_AVRCP OR CONFIG_BT_AVRCP_CONTROLLER OR CONFIG_BT_AVRCP_TARGET)
    ipro_bt_host_enable(CONFIG_BT_AVRCP)
    ipro_bt_host_enable(CONFIG_BT_AVCTP)
    ipro_bt_host_enable(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
endif()
if(CONFIG_BT_AVRCP_TG_COVER_ART OR CONFIG_BT_AVRCP_CT_COVER_ART)
    ipro_bt_host_enable(CONFIG_BT_BIP)
endif()
if(CONFIG_BT_AVRCP_BROWSING OR CONFIG_BT_GOEP)
    ipro_bt_host_enable(CONFIG_BT_L2CAP_ENH_RET)
    ipro_bt_host_enable(CONFIG_BT_L2CAP_RET_FC)
endif()
if(CONFIG_BT_HFP OR CONFIG_BT_HFP_HF OR CONFIG_BT_HFP_AG OR CONFIG_BT_SPP)
    ipro_bt_host_enable(CONFIG_BT_RFCOMM)
    ipro_bt_host_enable(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
endif()
if(CONFIG_BT_GOEP OR CONFIG_BT_BIP)
    ipro_bt_host_enable(CONFIG_BT_GOEP)
    ipro_bt_host_enable(CONFIG_BT_RFCOMM)
    ipro_bt_host_enable(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
endif()

# LE Audio service/profile symbols all share audio.c and codec.c.
set(_ipro_audio_profile_symbols
    CONFIG_BT_BAP_STREAM
    CONFIG_BT_BAP_BASE
    CONFIG_BT_BAP_UNICAST_SERVER
    CONFIG_BT_BAP_UNICAST_CLIENT
    CONFIG_BT_BAP_BROADCAST_SOURCE
    CONFIG_BT_BAP_BROADCAST_SINK
    CONFIG_BT_BAP_SCAN_DELEGATOR
    CONFIG_BT_BAP_BROADCAST_ASSISTANT
    CONFIG_BT_ASCS
    CONFIG_BT_PACS
    CONFIG_BT_VCP_VOL_REND
    CONFIG_BT_VCP_VOL_CTLR
    CONFIG_BT_VOCS
    CONFIG_BT_VOCS_CLIENT
    CONFIG_BT_AICS
    CONFIG_BT_AICS_CLIENT
    CONFIG_BT_MICP_MIC_DEV
    CONFIG_BT_MICP_MIC_CTLR
    CONFIG_BT_CSIP_SET_MEMBER
    CONFIG_BT_CSIP_SET_COORDINATOR
    CONFIG_BT_CAP_ACCEPTOR
    CONFIG_BT_CAP_INITIATOR
    CONFIG_BT_CAP_COMMANDER
    CONFIG_BT_CAP_HANDOVER
    CONFIG_BT_TBS
    CONFIG_BT_TBS_CLIENT
    CONFIG_BT_CCP_CALL_CONTROL_SERVER
    CONFIG_BT_CCP_CALL_CONTROL_CLIENT
    CONFIG_BT_MCS
    CONFIG_BT_MCC
    CONFIG_BT_MPL
    CONFIG_MCTL
    CONFIG_BT_HAS
    CONFIG_BT_HAS_CLIENT
    CONFIG_BT_TMAP
    CONFIG_BT_GMAP
    CONFIG_BT_PBP
)
foreach(_ipro_symbol IN LISTS _ipro_audio_profile_symbols)
    if(${_ipro_symbol})
        ipro_bt_host_enable(CONFIG_BT_AUDIO)
    endif()
endforeach()

# BAP unicast dependencies.  Direction and GAP role remain application policy;
# the profile selection only brings in protocol machinery shared by both.
if(CONFIG_BT_BAP_UNICAST_SERVER)
    ipro_bt_host_enable(CONFIG_BT_BAP_UNICAST)
    ipro_bt_host_enable(CONFIG_BT_BAP_STREAM)
    ipro_bt_host_enable(CONFIG_BT_ASCS)
    ipro_bt_host_enable(CONFIG_BT_PACS)
    ipro_bt_host_enable(CONFIG_BT_ISO)
    ipro_bt_host_enable(CONFIG_BT_ISO_UNICAST)
endif()
if(CONFIG_BT_BAP_UNICAST_CLIENT)
    ipro_bt_host_enable(CONFIG_BT_BAP_UNICAST)
    ipro_bt_host_enable(CONFIG_BT_BAP_STREAM)
    ipro_bt_host_enable(CONFIG_BT_PACS)
    ipro_bt_host_enable(CONFIG_BT_ISO)
    ipro_bt_host_enable(CONFIG_BT_ISO_UNICAST)
endif()

# In Zephyr 4.4 the CIG/CIS central QoS validation helpers are part of the ISO
# TX compilation unit.  Kconfig normally resolves this role dependency; mirror
# it here before source selection for the SDK's direct-.config build.
if(CONFIG_BT_ISO_CENTRAL)
    ipro_bt_host_enable(CONFIG_BT_ISO_TX)
endif()

# BAP broadcast dependencies.
if(CONFIG_BT_BAP_BROADCAST_SOURCE)
    ipro_bt_host_enable(CONFIG_BT_BAP_BASE)
    ipro_bt_host_enable(CONFIG_BT_BAP_STREAM)
    ipro_bt_host_enable(CONFIG_BT_ISO)
    ipro_bt_host_enable(CONFIG_BT_ISO_BROADCAST)
    ipro_bt_host_enable(CONFIG_BT_ISO_BROADCASTER)
    ipro_bt_host_enable(CONFIG_BT_ISO_TX)
endif()
if(CONFIG_BT_BAP_BROADCAST_SINK)
    ipro_bt_host_enable(CONFIG_BT_BAP_BASE)
    ipro_bt_host_enable(CONFIG_BT_BAP_STREAM)
    ipro_bt_host_enable(CONFIG_BT_ISO)
    ipro_bt_host_enable(CONFIG_BT_ISO_BROADCAST)
    ipro_bt_host_enable(CONFIG_BT_ISO_SYNC_RECEIVER)
    ipro_bt_host_enable(CONFIG_BT_ISO_RX)
    ipro_bt_host_enable(CONFIG_BT_PER_ADV_SYNC)
endif()
if(CONFIG_BT_BAP_SCAN_DELEGATOR OR CONFIG_BT_BAP_BROADCAST_ASSISTANT)
    ipro_bt_host_enable(CONFIG_BT_PER_ADV_SYNC)
endif()

# Included-service and higher-level profile dependencies.
if(CONFIG_BT_VCP_VOL_REND_VOCS_INSTANCE_COUNT GREATER 0)
    ipro_bt_host_enable(CONFIG_BT_VOCS)
endif()
if(CONFIG_BT_VCP_VOL_REND_AICS_INSTANCE_COUNT GREATER 0 OR
   CONFIG_BT_MICP_MIC_DEV_AICS_INSTANCE_COUNT GREATER 0)
    ipro_bt_host_enable(CONFIG_BT_AICS)
endif()
if(CONFIG_BT_VCP_VOL_CTLR_MAX_VOCS_INST GREATER 0)
    ipro_bt_host_enable(CONFIG_BT_VOCS_CLIENT)
endif()
if(CONFIG_BT_VCP_VOL_CTLR_MAX_AICS_INST GREATER 0 OR
   CONFIG_BT_MICP_MIC_CTLR_MAX_AICS_INST GREATER 0)
    ipro_bt_host_enable(CONFIG_BT_AICS_CLIENT)
endif()
if(CONFIG_BT_CAP_ACCEPTOR OR CONFIG_BT_CAP_INITIATOR OR CONFIG_BT_CAP_COMMANDER OR
   CONFIG_BT_CAP_HANDOVER)
    ipro_bt_host_enable(CONFIG_BT_CAP)
endif()
if(CONFIG_BT_CAP_INITIATOR)
    ipro_bt_host_enable(CONFIG_BT_CAP_INITIATOR_UNICAST)
endif()
if(CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER)
    ipro_bt_host_enable(CONFIG_BT_CAP_ACCEPTOR)
    ipro_bt_host_enable(CONFIG_BT_CSIP_SET_MEMBER)
endif()
if(CONFIG_MCTL)
    ipro_bt_host_enable(CONFIG_BT_MCS)
    ipro_bt_host_enable(CONFIG_BT_MCC)
endif()
if(CONFIG_BT_CCP_CALL_CONTROL_SERVER)
    ipro_bt_host_enable(CONFIG_BT_TBS)
endif()
if(CONFIG_BT_CCP_CALL_CONTROL_CLIENT)
    # BT_TBS_CLIENT is a derived Kconfig symbol.  Enabling it directly does
    # not compile either discovery implementation, so choose GTBS as the
    # interoperable baseline when an application did not select GTBS/TBS.
    if(NOT CONFIG_BT_TBS_CLIENT_GTBS AND NOT CONFIG_BT_TBS_CLIENT_TBS)
        ipro_bt_host_enable(CONFIG_BT_TBS_CLIENT_GTBS)
    endif()
    ipro_bt_host_enable(CONFIG_BT_TBS_CLIENT)
endif()

if(CONFIG_BT_TBS_CLIENT AND
   NOT CONFIG_BT_TBS_CLIENT_GTBS AND NOT CONFIG_BT_TBS_CLIENT_TBS)
    ipro_bt_host_enable(CONFIG_BT_TBS_CLIENT_GTBS)
endif()
if(CONFIG_BT_TBS_CLIENT_GTBS OR CONFIG_BT_TBS_CLIENT_TBS)
    ipro_bt_host_enable(CONFIG_BT_TBS_CLIENT)
    ipro_bt_host_enable(CONFIG_BT_GATT_CLIENT)
    ipro_bt_host_enable(CONFIG_BT_GATT_AUTO_DISCOVER_CCC)
    ipro_bt_host_enable(CONFIG_BT_SMP)
endif()
if(CONFIG_BT_TBS)
    ipro_bt_host_enable(CONFIG_BT_GATT_DYNAMIC_DB)
    ipro_bt_host_enable(CONFIG_BT_SMP)
endif()

# Media Control and Object Transfer.  Zephyr Kconfig normally rejects or
# expands these combinations; the SDK consumes .config directly, so mirror
# the same dependency graph here before selecting source files.
if(CONFIG_BT_MCS)
    ipro_bt_host_enable(CONFIG_MCTL)
    ipro_bt_host_enable(CONFIG_MCTL_LOCAL_PLAYER_CONTROL)
    ipro_bt_host_enable(CONFIG_MCTL_LOCAL_PLAYER_REMOTE_CONTROL)
    ipro_bt_host_enable(CONFIG_BT_GATT_DYNAMIC_DB)
    ipro_bt_host_enable(CONFIG_BT_SMP)
endif()
if(CONFIG_BT_MCC)
    ipro_bt_host_enable(CONFIG_BT_GATT_CLIENT)
    ipro_bt_host_enable(CONFIG_BT_GATT_AUTO_DISCOVER_CCC)
    ipro_bt_host_enable(CONFIG_BT_SMP)
endif()
if(CONFIG_BT_MPL)
    ipro_bt_host_enable(CONFIG_MCTL)
    ipro_bt_host_enable(CONFIG_MCTL_LOCAL_PLAYER_CONTROL)
    # Matches Kconfig's default-y local application controller.  MPL wires
    # its callbacks through this member of media_proxy's mprx structure.
    ipro_bt_host_enable(CONFIG_MCTL_LOCAL_PLAYER_LOCAL_CONTROL)
endif()
if(CONFIG_BT_MPL_OBJECTS)
    ipro_bt_host_enable(CONFIG_BT_MPL)
    ipro_bt_host_enable(CONFIG_BT_MCS)
    ipro_bt_host_enable(CONFIG_BT_OTS)
endif()
if(CONFIG_BT_MCC_OTS)
    ipro_bt_host_enable(CONFIG_BT_MCC)
    ipro_bt_host_enable(CONFIG_BT_OTS_CLIENT)
endif()
if(CONFIG_MCTL_REMOTE_PLAYER_CONTROL)
    ipro_bt_host_enable(CONFIG_MCTL)
    ipro_bt_host_enable(CONFIG_BT_MCC)
endif()
if(CONFIG_MCTL_REMOTE_PLAYER_CONTROL_OBJECTS)
    ipro_bt_host_enable(CONFIG_MCTL_REMOTE_PLAYER_CONTROL)
    ipro_bt_host_enable(CONFIG_BT_MCC_OTS)
endif()
if(CONFIG_BT_OTS)
    ipro_bt_host_enable(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
    ipro_bt_host_enable(CONFIG_BT_GATT_DYNAMIC_DB)
    ipro_bt_host_enable(CONFIG_BT_SMP)
    ipro_bt_host_enable(CONFIG_BT_OTS_OACP_READ_SUPPORT)
    ipro_bt_host_enable(CONFIG_BT_OTS_OLCP_GO_TO_SUPPORT)
endif()
if(CONFIG_BT_OTS_CLIENT)
    ipro_bt_host_enable(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
    ipro_bt_host_enable(CONFIG_BT_GATT_CLIENT)
    ipro_bt_host_enable(CONFIG_BT_GATT_AUTO_DISCOVER_CCC)
endif()

# Zephyr's GATT service-client Kconfigs select automatic CCC discovery.  The
# symbol changes the public bt_gatt_subscribe_params layout, so omitting the
# select is a compile-time ABI mismatch rather than merely lost convenience.
if(CONFIG_BT_VCP_VOL_CTLR OR CONFIG_BT_VOCS_CLIENT OR CONFIG_BT_AICS_CLIENT OR
   CONFIG_BT_MICP_MIC_CTLR OR CONFIG_BT_CSIP_SET_COORDINATOR OR
   CONFIG_BT_BAP_UNICAST_CLIENT OR CONFIG_BT_BAP_BROADCAST_ASSISTANT OR
   CONFIG_BT_TBS_CLIENT OR CONFIG_BT_CCP_CALL_CONTROL_CLIENT OR
   CONFIG_BT_MCC OR CONFIG_BT_HAS_CLIENT)
    ipro_bt_host_enable(CONFIG_BT_GATT_CLIENT)
    ipro_bt_host_enable(CONFIG_BT_GATT_AUTO_DISCOVER_CCC)
endif()

# Host feature dependencies outside Audio.
if(CONFIG_BT_EXT_ADV_CODING_SELECTION)
    ipro_bt_host_enable(CONFIG_BT_EXT_ADV)
endif()
if(CONFIG_BT_GATT_SECURITY_LEVELS)
    ipro_bt_host_enable(CONFIG_BT_GATT_GAP_SERVICE)
    ipro_bt_host_enable(CONFIG_BT_GATT_GAP_SVC_VALIDATE)
endif()
if(CONFIG_BT_EAD)
    ipro_bt_host_enable(CONFIG_BT_HOST_CCM)
endif()
if(CONFIG_BT_SUBRATING AND NOT (CONFIG_BT_CENTRAL OR CONFIG_BT_PERIPHERAL))
    message(FATAL_ERROR "[ipro_bt_host] SUBRATING needs CENTRAL or PERIPHERAL")
endif()
if(CONFIG_BT_PER_ADV_RSP)
    ipro_bt_host_enable(CONFIG_BT_EXT_ADV)
    ipro_bt_host_enable(CONFIG_BT_PER_ADV)
endif()
if(CONFIG_BT_PER_ADV_SYNC_RSP OR CONFIG_BT_PER_ADV_SYNC_TRANSFER_RECEIVER OR
   CONFIG_BT_PER_ADV_SYNC_TRANSFER_SENDER)
    ipro_bt_host_enable(CONFIG_BT_EXT_ADV)
    ipro_bt_host_enable(CONFIG_BT_PER_ADV_SYNC)
endif()
if(CONFIG_BT_PER_ADV_SYNC)
    ipro_bt_host_enable(CONFIG_BT_EXT_ADV)
    ipro_bt_host_enable(CONFIG_BT_OBSERVER)
endif()
if(CONFIG_BT_PER_ADV)
    ipro_bt_host_enable(CONFIG_BT_EXT_ADV)
    ipro_bt_host_enable(CONFIG_BT_BROADCASTER)
endif()
if(CONFIG_BT_EATT)
    ipro_bt_host_enable(CONFIG_BT_GATT_READ_MULT_VAR_LEN)
    ipro_bt_host_enable(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
    ipro_bt_host_enable(CONFIG_BT_L2CAP_ECRED)
    ipro_bt_host_enable(CONFIG_BT_SMP)
endif()

# Bluetooth 5.1 Direction Finding role dependencies.
if(CONFIG_BT_DF_CONNECTIONLESS_CTE_RX OR CONFIG_BT_DF_CONNECTIONLESS_CTE_TX OR
   CONFIG_BT_DF_CONNECTION_CTE_RX OR CONFIG_BT_DF_CONNECTION_CTE_TX)
    ipro_bt_host_enable(CONFIG_BT_DF)
endif()
if(CONFIG_BT_DF_CONNECTIONLESS_CTE_RX)
    ipro_bt_host_enable(CONFIG_BT_PER_ADV_SYNC)
endif()
if(CONFIG_BT_DF_CONNECTIONLESS_CTE_TX)
    ipro_bt_host_enable(CONFIG_BT_PER_ADV)
endif()

# Capacity defaults used by source selection and compile definitions.
if(CONFIG_BT_EXT_ADV)
    ipro_bt_host_default(CONFIG_BT_BUF_EVT_DISCARDABLE_SIZE 255)
else()
    ipro_bt_host_default(CONFIG_BT_BUF_EVT_DISCARDABLE_SIZE 43)
endif()
if(CONFIG_BT_BUF_EVT_DISCARDABLE_SIZE LESS 43 OR CONFIG_BT_BUF_EVT_DISCARDABLE_SIZE GREATER 255)
    message(FATAL_ERROR "[ipro_bt_host] EVT_DISCARDABLE_SIZE must be in 43..255")
endif()
ipro_bt_host_default(CONFIG_BT_ISO_MAX_CHAN 2)
ipro_bt_host_default(CONFIG_BT_ISO_MAX_CIG 1)
ipro_bt_host_default(CONFIG_BT_ISO_MAX_BIG 1)
ipro_bt_host_default(CONFIG_BT_ISO_TX_BUF_COUNT 5)
ipro_bt_host_default(CONFIG_BT_ISO_TX_MTU 251)
ipro_bt_host_default(CONFIG_BT_ISO_RX_BUF_COUNT 4)
ipro_bt_host_default(CONFIG_BT_ISO_RX_MTU 251)
ipro_bt_host_default(CONFIG_BT_PER_ADV_SYNC_MAX 1)
ipro_bt_host_default(CONFIG_BT_PER_ADV_SYNC_BUF_SIZE 0)
ipro_bt_host_default(CONFIG_BT_EATT_MAX 3)
if(CONFIG_BT_EATT AND (CONFIG_BT_EATT_MAX LESS 1 OR CONFIG_BT_EATT_MAX GREATER 8))
    message(FATAL_ERROR "[ipro_bt_host] EATT_MAX must be in 1..8")
endif()
ipro_bt_host_default(CONFIG_BT_L2CAP_MPS 48)
ipro_bt_host_default(CONFIG_BT_L2CAP_MAX_WINDOW_SIZE 1)
ipro_bt_host_default(CONFIG_BT_L2CAP_CONN_RTX_TIMEOUT 40)

ipro_bt_host_default(CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_COUNT 1)
ipro_bt_host_default(CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_STREAM_COUNT 2)
ipro_bt_host_default(CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK_COUNT 2)
ipro_bt_host_default(CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT 0)
ipro_bt_host_default(CONFIG_BT_BAP_BASS_MAX_SUBGROUPS 1)
ipro_bt_host_default(CONFIG_BT_BAP_BROADCAST_SRC_COUNT 1)
ipro_bt_host_default(CONFIG_BT_BAP_BROADCAST_SRC_SUBGROUP_COUNT 1)
ipro_bt_host_default(CONFIG_BT_BAP_BROADCAST_SRC_STREAM_COUNT 1)
ipro_bt_host_default(CONFIG_BT_BAP_BROADCAST_SNK_COUNT 1)
ipro_bt_host_default(CONFIG_BT_BAP_BROADCAST_SNK_SUBGROUP_COUNT 1)
ipro_bt_host_default(CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT 1)
ipro_bt_host_default(CONFIG_BT_BAP_SCAN_DELEGATOR_RECV_STATE_COUNT 1)
ipro_bt_host_default(CONFIG_BT_BAP_SCAN_DELEGATOR_BUF_TIMEOUT 50)
ipro_bt_host_default(CONFIG_BT_BAP_BROADCAST_ASSISTANT_RECV_STATE_COUNT 1)

ipro_bt_host_default(CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT 2)
ipro_bt_host_default(CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT 0)
ipro_bt_host_default(CONFIG_BT_ASCS_MAX_ACTIVE_ASES ${CONFIG_BT_ISO_MAX_CHAN})
ipro_bt_host_default(CONFIG_BT_VCP_VOL_REND_VOCS_INSTANCE_COUNT 0)
ipro_bt_host_default(CONFIG_BT_VCP_VOL_REND_AICS_INSTANCE_COUNT 0)
ipro_bt_host_default(CONFIG_BT_VCP_VOL_CTLR_MAX_VOCS_INST 0)
ipro_bt_host_default(CONFIG_BT_VCP_VOL_CTLR_MAX_AICS_INST 0)
ipro_bt_host_default(CONFIG_BT_MICP_MIC_DEV_AICS_INSTANCE_COUNT 0)
ipro_bt_host_default(CONFIG_BT_MICP_MIC_CTLR_MAX_AICS_INST 0)

# Reject combinations that Kconfig would normally make impossible.  Failing at
# configure time is preferable to a smaller image that silently lacks a role.
if(CONFIG_BT_A2DP AND NOT (CONFIG_BT_A2DP_SOURCE OR CONFIG_BT_A2DP_SINK))
    message(FATAL_ERROR "[ipro_bt_host] A2DP needs SOURCE and/or SINK")
endif()
if(CONFIG_BT_AVRCP AND NOT (CONFIG_BT_AVRCP_CONTROLLER OR CONFIG_BT_AVRCP_TARGET))
    message(FATAL_ERROR "[ipro_bt_host] AVRCP needs CONTROLLER and/or TARGET")
endif()
if(CONFIG_BT_HFP AND NOT (CONFIG_BT_HFP_HF OR CONFIG_BT_HFP_AG))
    message(FATAL_ERROR "[ipro_bt_host] HFP needs HF and/or AG")
endif()
if(CONFIG_BT_BREDR AND NOT CONFIG_IPRO6)
    message(FATAL_ERROR "[ipro_bt_host] BR/EDR is supported only by the IPRO6S controller")
endif()
if(CONFIG_BT_ISO AND CONFIG_BT_CTLR_IPRO AND
   NOT CONFIG_IPRO_BLE_CTLR_FEATURE STREQUAL "bt54_iso2")
    message(FATAL_ERROR
        "[ipro_bt_host] ISO requires CONFIG_IPRO_BLE_CTLR_FEATURE=\"bt54_iso2\"")
endif()
if(CONFIG_BT_ASCS_MAX_ACTIVE_ASES GREATER CONFIG_BT_ISO_MAX_CHAN)
    message(FATAL_ERROR
        "[ipro_bt_host] ASCS active ASE count exceeds CONFIG_BT_ISO_MAX_CHAN")
endif()

unset(_ipro_audio_profile_symbols)
unset(_ipro_symbol)

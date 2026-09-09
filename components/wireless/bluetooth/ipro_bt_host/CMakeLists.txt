# SPDX-License-Identifier: Apache-2.0

# The v4.4 source import is deliberately separate from the IPRO port.  Start
# with the dependency-light data/UUID layer; additional host modules are added
# only after their FreeRTOS adapter gate compiles and passes unit tests.
__generate_library()

set(IPRO_BT_HOST_ZEPHYR ${CMAKE_CURRENT_SOURCE_DIR}/upstream/zephyr)
set(IPRO_BT_HOST_ZEPHYR_AUDIO ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/audio)
set(IPRO_BT_HOST_ZEPHYR_OTS ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/services/ots)
# tinycrypt used to be pulled from ../blestack/src/common/tinycrypt while the
# rest of that tree was still present. It is the only thing the new host ever
# needed from the old stack, so it now lives here and blestack can go.
set(IPRO_BT_HOST_TINYCRYPT ${CMAKE_CURRENT_SOURCE_DIR}/tinycrypt)

# Resolve Zephyr-style select/imply dependencies before choosing source files.
# IPRO SDK .config files are consumed directly and do not otherwise receive
# Kconfig's dependency expansion.
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/resolve_config.cmake)

# Application autoconf may explicitly undefine dependencies enabled above.
# Include resolved role declarations after it in the Zephyr port wrapper.
configure_file(cmake/periodic_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/ipro_bt_host_periodic_config.h)
__component_add_include_dirs(${CMAKE_CURRENT_BINARY_DIR}/generated)

# Mirror Zephyr Host's default TX processing model.  Without a dedicated TX
# workqueue, synchronous HCI commands issued by bt_enable() from sysworkq must
# self-drain the command queue from the same work item, which is fragile during
# controller bring-up.
sdk_ifndef(CONFIG_BT_TX_PROCESSOR_THREAD 1)
sdk_ifndef(CONFIG_BT_TX_PROCESSOR_THREAD_PRIO 6)
sdk_ifndef(CONFIG_BT_TX_PROCESSOR_STACK_SIZE 1024)
sdk_ifndef(CONFIG_BT_RX_STACK_SIZE 2048)
if(CONFIG_BT_SMP)
    # The SDK consumes .config directly rather than resolving Zephyr's Kconfig
    # defaults. Preserve upstream's default: authenticated I/O capabilities
    # must advertise MITM, or a responder silently makes Level 4 unreachable.
    sdk_ifndef(CONFIG_BT_SMP_ENFORCE_MITM 1)
endif()

__component_add_include_dirs(
    port/include
    ${IPRO_BT_HOST_ZEPHYR}/include
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host
    ${IPRO_BT_HOST_ZEPHYR_AUDIO}
    ${IPRO_BT_HOST_TINYCRYPT}/include
)

__component_add_sources(
    port/kernel.c
    port/assert.c
    port/crypto.c
    port/ecc.c
    port/hci_ipro.c
    port/channel_map.c
    port/util.c
    port/uuid_port.c
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/uuid.c
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/addr.c
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/data.c
    ${IPRO_BT_HOST_ZEPHYR}/lib/net_buf/buf_simple.c
    ${IPRO_BT_HOST_ZEPHYR}/lib/net_buf/buf.c
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/buf.c
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/hci_common.c
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/hci_core.c
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/id.c
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/adv.c
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/scan.c
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/conn.c
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/l2cap.c
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/att.c
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/gatt.c
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/common/addr.c
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/common/bt_str.c
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/crypto/bt_crypto.c
    ${IPRO_BT_HOST_TINYCRYPT}/source/aes_encrypt.c
    ${IPRO_BT_HOST_TINYCRYPT}/source/cmac_mode.c
    ${IPRO_BT_HOST_TINYCRYPT}/source/utils.c
    ${IPRO_BT_HOST_TINYCRYPT}/source/ecc.c
    ${IPRO_BT_HOST_TINYCRYPT}/source/ecc_dh.c
)

if(CONFIG_BT_GATT_GAP_SERVICE)
    __component_add_sources(
        port/gatt_security_levels_value.c
        port/gatt_gap_service.c
        ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/gatt_gap_svc_validate.c
    )
endif()

# Host-side EAD uses the upstream software CCM implementation and IPRO AES.
if(CONFIG_BT_EAD)
    __component_add_sources(${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/lib/ead.c)
    __component_add_compile_definitions(-DCONFIG_BT_EAD=1 -DCONFIG_BT_EAD_LOG_LEVEL=2)
endif()
if(CONFIG_BT_HOST_CCM)
    __component_add_sources(${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/aes_ccm.c)
    __component_add_compile_definitions(-DCONFIG_BT_HOST_CCM=1)
endif()

# Match Zephyr's host build graph: conn.c always calls the SMP interface, but
# the full pairing/key implementation is valid only when CONFIG_BT_SMP is on.
# Basic, unsecured peripheral builds use the upstream null implementation.
if(CONFIG_BT_SMP)
    __component_add_sources(
        ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/smp.c
        ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/keys.c
    )
else()
    __component_add_sources(
        ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/smp_null.c
    )
endif()

# The SDK historically exposes BR/EDR as CONFIG_BT_BREDR, while the Zephyr
# v4.4 Host renamed its gate to CONFIG_BT_CLASSIC.  Mirror that Kconfig select
# here and use the exact upstream Classic source graph.  Without this bridge
# the dual-mode controller advertises BR/EDR support, but the Host silently
# builds without Connection Request/Complete handling, page accept, BR L2CAP,
# SSP, or SCO support.
if(CONFIG_BT_BREDR)
    __component_add_sources(
        ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/br.c
        ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/conn_br.c
        ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/keys_br.c
        ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/l2cap_br.c
        ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/sdp.c
        ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/ssp.c
        ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/sco.c
    )

    sdk_ifndef(CONFIG_BT_LIMITED_DISCOVERABLE_DURATION 60)
    sdk_ifndef(CONFIG_BT_BR_MIN_ENC_KEY_SIZE 16)
    sdk_ifndef(CONFIG_BT_MAX_SCO_CONN 1)
    sdk_ifndef(CONFIG_BT_L2CAP_RX_FLUSH_TO 0xFFFF)
    sdk_ifndef(CONFIG_BT_L2CAP_BR_RET_TIMEOUT 2000)
    sdk_ifndef(CONFIG_BT_L2CAP_BR_MONITOR_TIMEOUT 2000)
    sdk_ifndef(CONFIG_BT_PAGE_TIMEOUT 0x2000)
    sdk_ifndef(CONFIG_BT_COD 0)
    sdk_ifndef(CONFIG_BT_L2CAP_CONN_RTX_TIMEOUT 40)

    # Keep optional Classic profiles on the same source/configuration graph as
    # upstream Zephyr.  CONFIG_BT_RFCOMM alone previously exposed the public
    # API to applications, while omitting its implementation from this SDK
    # component and therefore failing only at final link time.
    if(CONFIG_BT_RFCOMM)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/rfcomm.c
        )
        sdk_ifndef(CONFIG_BT_RFCOMM_L2CAP_MTU 255)
        sdk_ifndef(CONFIG_BT_RFCOMM_LOG_LEVEL 3)
    endif()

    if(CONFIG_BT_AVDTP)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/avdtp.c
        )
        sdk_ifndef(CONFIG_BT_AVDTP_RTP_VERSION 2)
        sdk_ifndef(CONFIG_BT_AVDTP_SIGNAL_SDU_MAX 512)
        sdk_ifndef(CONFIG_BT_AVDTP_LOG_LEVEL 3)
    endif()

    if(CONFIG_BT_A2DP)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/a2dp.c
            ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/a2dp_codec_sbc.c
        )
        sdk_ifndef(CONFIG_BT_A2DP_LOG_LEVEL 3)
    endif()

    if(CONFIG_LIBSBC)
        set(IPRO_LIBSBC ${CMAKE_SOURCE_DIR}/components/3rdparty/libsbc)
        __component_add_include_dirs(
            ${IPRO_LIBSBC}/encoder/include
            ${IPRO_LIBSBC}/decoder/include
            ${IPRO_LIBSBC}/decoder/srce
        )
        __component_add_sources(
            ${IPRO_LIBSBC}/sbc.c
            ${IPRO_LIBSBC}/decoder/srce/alloc.c
            ${IPRO_LIBSBC}/decoder/srce/bitalloc.c
            ${IPRO_LIBSBC}/decoder/srce/bitalloc-sbc.c
            ${IPRO_LIBSBC}/decoder/srce/bitstream-decode.c
            ${IPRO_LIBSBC}/decoder/srce/decoder-oina.c
            ${IPRO_LIBSBC}/decoder/srce/decoder-private.c
            ${IPRO_LIBSBC}/decoder/srce/decoder-sbc.c
            ${IPRO_LIBSBC}/decoder/srce/dequant.c
            ${IPRO_LIBSBC}/decoder/srce/framing.c
            ${IPRO_LIBSBC}/decoder/srce/framing-sbc.c
            ${IPRO_LIBSBC}/decoder/srce/oi_codec_version.c
            ${IPRO_LIBSBC}/decoder/srce/synthesis-8-generated.c
            ${IPRO_LIBSBC}/decoder/srce/synthesis-dct8.c
            ${IPRO_LIBSBC}/decoder/srce/synthesis-sbc.c
        )
    endif()

    if(CONFIG_BT_AVCTP)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/avctp.c
        )
        sdk_ifndef(CONFIG_BT_AVCTP_LOG_LEVEL 3)
    endif()

    if(CONFIG_BT_AVRCP)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/avrcp.c
        )
        sdk_ifndef(CONFIG_BT_AVRCP_LOG_LEVEL 3)
        sdk_ifndef(CONFIG_BT_AVRCP_VD_RX_SIZE 1024)
        sdk_ifndef(CONFIG_BT_AVRCP_BROWSING_L2CAP_MTU 339)
    endif()

    if(CONFIG_BT_AVRCP_TG_COVER_ART OR CONFIG_BT_AVRCP_CT_COVER_ART)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/avrcp_cover_art.c
        )
    endif()

    if(CONFIG_BT_HFP_HF)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/hfp_hf.c
            ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/at.c
        )
        sdk_ifndef(CONFIG_BT_HFP_HF_LOG_LEVEL 3)
        sdk_ifndef(CONFIG_BT_HFP_HF_MAX_CALLS 1)
    endif()

    if(CONFIG_BT_HFP_AG)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/hfp_ag.c
        )
        sdk_ifndef(CONFIG_BT_HFP_AG_LOG_LEVEL 3)
        sdk_ifndef(CONFIG_BT_HFP_AG_MAX_CALLS 1)
        sdk_ifndef(CONFIG_BT_HFP_AG_TX_BUF_COUNT 7)
        sdk_ifndef(CONFIG_BT_HFP_AG_OUTGOING_TIMEOUT 3)
        sdk_ifndef(CONFIG_BT_HFP_AG_INCOMING_TIMEOUT 3)
        sdk_ifndef(CONFIG_BT_HFP_AG_ALERTING_TIMEOUT 60)
        sdk_ifndef(CONFIG_BT_HFP_AG_PHONE_NUMBER_MAX_LEN 32)
        sdk_ifndef(CONFIG_BT_HFP_AG_RING_NOTIFY_INTERVAL 3)
        sdk_ifndef(CONFIG_BT_HFP_AG_GET_ONGOING_CALL_TIMEOUT 10000)
    endif()

    if(CONFIG_BT_GOEP)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/obex.c
            ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/goep.c
        )
        sdk_ifndef(CONFIG_BT_GOEP_LOG_LEVEL 3)
        sdk_ifndef(CONFIG_BT_GOEP_RFCOMM_MTU 1021)
        sdk_ifndef(CONFIG_BT_GOEP_L2CAP_MTU 1021)
    endif()

    if(CONFIG_BT_BIP)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/bip.c
        )
        sdk_ifndef(CONFIG_BT_BIP_LOG_LEVEL 3)
    endif()

    if(CONFIG_BT_DID)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/did.c
        )
        sdk_ifndef(CONFIG_BT_DEVICE_VENDOR_ID 0)
        sdk_ifndef(CONFIG_BT_DEVICE_PRODUCT_ID 0)
        sdk_ifndef(CONFIG_BT_DEVICE_VERSION 0)
    endif()
endif()

# Applications own the public Bluetooth identity.  Keep the historical name
# as a compatibility default, but do not hard-code it in the Host component:
# dual-mode products need a meaningful BR/EDR name and Class of Device before
# Inquiry Scan is enabled.
sdk_ifndef(CONFIG_BT_DEVICE_NAME IPRO-BT)

# Zephyr adds rpa.c through CONFIG_BT_RPA.  Without SMP or privacy its only
# public operation is the ENOTSUP stub, while the private ah() helper becomes
# dead code under this component's -Werror policy.
if(CONFIG_BT_SMP OR CONFIG_BT_PRIVACY OR CONFIG_BT_CTLR_PRIVACY)
    __component_add_sources(
        ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/common/rpa.c
    )
endif()

if(CONFIG_BT_ISO)
    __component_add_sources(
        ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/iso.c
    )
endif()

if(CONFIG_BT_DF)
    __component_add_sources(
        ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/direction.c
    )
    sdk_ifndef(CONFIG_BT_DF_LOG_LEVEL 3)
endif()

if(CONFIG_BT_OTS)
    __component_add_sources(
        ${IPRO_BT_HOST_ZEPHYR_OTS}/ots.c
        ${IPRO_BT_HOST_ZEPHYR_OTS}/ots_l2cap.c
        ${IPRO_BT_HOST_ZEPHYR_OTS}/ots_obj_manager.c
        ${IPRO_BT_HOST_ZEPHYR_OTS}/ots_oacp.c
        ${IPRO_BT_HOST_ZEPHYR_OTS}/ots_olcp.c
    )
    if(CONFIG_BT_OTS_DIR_LIST_OBJ)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_OTS}/ots_dir_list.c)
    endif()
endif()
if(CONFIG_BT_OTS_CLIENT)
    __component_add_sources(${IPRO_BT_HOST_ZEPHYR_OTS}/ots_client.c)
    if(NOT CONFIG_BT_OTS)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_OTS}/ots_l2cap.c)
    endif()
endif()

if(CONFIG_BT_AUDIO)
    __component_add_sources(
        ${IPRO_BT_HOST_ZEPHYR_AUDIO}/audio.c
        ${IPRO_BT_HOST_ZEPHYR_AUDIO}/codec.c
    )

    if(CONFIG_BT_CONN)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/ccid.c
        )
    endif()

    if(CONFIG_BT_PACS)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/pacs.c
        )
    endif()

    if(CONFIG_BT_ASCS)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/ascs.c
        )
    endif()

    if(CONFIG_BT_BAP_STREAM OR CONFIG_BT_BAP_UNICAST_SERVER OR CONFIG_BT_BAP_UNICAST_CLIENT)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/bap_stream.c
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/bap_iso.c
        )
    endif()

    if(CONFIG_BT_BAP_UNICAST_SERVER)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/bap_unicast_server.c
        )
    endif()

    if(CONFIG_BT_BAP_UNICAST_CLIENT)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/bap_unicast_client.c
        )
    endif()

    if(CONFIG_BT_VCP_VOL_REND)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/vcp_vol_rend.c
        )
    endif()

    if(CONFIG_BT_VCP_VOL_CTLR)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/vcp_vol_ctlr.c
        )
    endif()

    if(CONFIG_BT_VOCS OR CONFIG_BT_VOCS_CLIENT)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/vocs.c
        )
    endif()
    if(CONFIG_BT_VOCS_CLIENT)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/vocs_client.c
        )
    endif()

    if(CONFIG_BT_AICS OR CONFIG_BT_AICS_CLIENT)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/aics.c
        )
    endif()
    if(CONFIG_BT_AICS_CLIENT)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/aics_client.c
        )
    endif()

    if(CONFIG_BT_MICP_MIC_DEV)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/micp_mic_dev.c
        )
    endif()
    if(CONFIG_BT_MICP_MIC_CTLR)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/micp_mic_ctlr.c
        )
    endif()

    if(CONFIG_BT_CAP_ACCEPTOR)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/cap_acceptor.c
        )
    endif()

    if(CONFIG_BT_CAP_INITIATOR)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/cap_stream.c
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/cap_initiator.c
        )
    endif()

    if(CONFIG_BT_CAP_COMMANDER)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/cap_commander.c
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/cap_common.c
        )
    elseif(CONFIG_BT_CAP_INITIATOR)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/cap_common.c
        )
    endif()

    if(CONFIG_BT_CAP_HANDOVER)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/cap_handover.c
        )
    endif()

    if(CONFIG_BT_CSIP_SET_MEMBER)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/csip_set_member.c
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/csip_crypto.c
        )
    endif()

    if(CONFIG_BT_CSIP_SET_COORDINATOR)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/csip_set_coordinator.c
        )
        if(NOT CONFIG_BT_CSIP_SET_MEMBER)
            __component_add_sources(
                ${IPRO_BT_HOST_ZEPHYR_AUDIO}/csip_crypto.c
            )
        endif()
    endif()

    if(CONFIG_BT_BAP_BASE)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_AUDIO}/bap_base.c)
    endif()
    if(CONFIG_BT_BAP_BROADCAST_SOURCE)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_AUDIO}/bap_broadcast_source.c)
    endif()
    if(CONFIG_BT_BAP_BROADCAST_SINK)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_AUDIO}/bap_broadcast_sink.c)
    endif()
    if(CONFIG_BT_BAP_SCAN_DELEGATOR)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_AUDIO}/bap_scan_delegator.c)
    endif()
    if(CONFIG_BT_BAP_BROADCAST_ASSISTANT)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_AUDIO}/bap_broadcast_assistant.c)
    endif()

    if(CONFIG_BT_TBS)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_AUDIO}/tbs.c)
    endif()
    if(CONFIG_BT_TBS_CLIENT)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_AUDIO}/tbs_client.c)
    endif()
    if(CONFIG_BT_CCP_CALL_CONTROL_SERVER)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_AUDIO}/ccp_call_control_server.c)
    endif()
    if(CONFIG_BT_CCP_CALL_CONTROL_CLIENT)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_AUDIO}/ccp_call_control_client.c)
    endif()
    if(CONFIG_BT_MCS)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_AUDIO}/mcs.c)
    endif()
    if(CONFIG_BT_MCC)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_AUDIO}/mcc.c)
    endif()
    if(CONFIG_BT_MPL)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_AUDIO}/mpl.c)
    endif()
    if(CONFIG_MCTL)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_AUDIO}/media_proxy.c)
    endif()
    if(CONFIG_BT_HAS)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_AUDIO}/has.c)
    endif()
    if(CONFIG_BT_HAS_CLIENT)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_AUDIO}/has_client.c)
    endif()
    if(CONFIG_BT_TMAP)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_AUDIO}/tmap.c)
    endif()
    if(CONFIG_BT_GMAP)
        __component_add_sources(
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/gmap_client.c
            ${IPRO_BT_HOST_ZEPHYR_AUDIO}/gmap_server.c
        )
    endif()
    if(CONFIG_BT_PBP)
        __component_add_sources(${IPRO_BT_HOST_ZEPHYR_AUDIO}/pbp.c)
    endif()
endif()

if(CONFIG_BT_SETTINGS)
    __component_add_sources(
        port/settings_ipro.c
        ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/settings.c
    )
endif()

if(NOT DEFINED CONFIG_BT_MAX_PAIRED)
    set(CONFIG_BT_MAX_PAIRED 1)
endif()

if(NOT DEFINED CONFIG_BT_L2CAP_TX_BUF_COUNT)
    set(CONFIG_BT_L2CAP_TX_BUF_COUNT 10)
endif()

if(NOT DEFINED CONFIG_BT_BUF_ACL_TX_COUNT)
    if(DEFINED IPRO_BLE_CTLR_ACT)
        # IPRO controller reports BLE_ACL_BUF_NB_TX, defined upstream as
        # BLE_ACTIVITY_MAX + 2.  Keep Zephyr host TX contexts aligned with the
        # actual controller HCI credits so boot does not depend on over/under
        # provisioned host pools.
        math(EXPR CONFIG_BT_BUF_ACL_TX_COUNT "${IPRO_BLE_CTLR_ACT} + 2")
    else()
        set(CONFIG_BT_BUF_ACL_TX_COUNT 7)
    endif()
endif()

# RFCOMM's upstream default follows the resolved ACL credit count.  Resolve it
# here (rather than in the Classic source block above) so apps that leave the
# ACL count at this component's controller-derived default get a valid value.
if(CONFIG_BT_RFCOMM)
    sdk_ifndef(CONFIG_BT_RFCOMM_TX_MAX ${CONFIG_BT_BUF_ACL_TX_COUNT})
endif()

if(NOT DEFINED CONFIG_BT_CONN_TX_MAX)
    set(CONFIG_BT_CONN_TX_MAX ${CONFIG_BT_L2CAP_TX_BUF_COUNT})
    if(CONFIG_BT_CONN_TX_MAX LESS CONFIG_BT_BUF_ACL_TX_COUNT)
        set(CONFIG_BT_CONN_TX_MAX ${CONFIG_BT_BUF_ACL_TX_COUNT})
    endif()
endif()

if(NOT DEFINED CONFIG_BT_BUF_EVT_RX_COUNT)
    # Zephyr needs at least one extra event buffer for HCI Number Of Completed
    # Packets relative to the ACL TX credit count.
    math(EXPR CONFIG_BT_BUF_EVT_RX_COUNT "${CONFIG_BT_BUF_ACL_TX_COUNT} + 1")
endif()

if(NOT DEFINED CONFIG_BT_BUF_ACL_RX_SIZE)
    if(CONFIG_BT_BREDR)
        # IPRO6S Read Buffer Size reports DH5/3-DH5's 1021-byte ACL data
        # length and may deliver a complete 598-byte Android SBC packet in a
        # single Controller-to-Host HCI ACL packet.  A LE-oriented 255-byte
        # pool makes the HCI adapter discard those packets before L2CAP.
        set(CONFIG_BT_BUF_ACL_RX_SIZE 1021)
    else()
        set(CONFIG_BT_BUF_ACL_RX_SIZE 255)
    endif()
endif()

if(NOT DEFINED CONFIG_SYS_CLOCK_TICKS_PER_SEC)
    set(CONFIG_SYS_CLOCK_TICKS_PER_SEC 1000)
endif()

if(NOT DEFINED CONFIG_BT_RPA_TIMEOUT)
    set(CONFIG_BT_RPA_TIMEOUT 900)
endif()

if(CONFIG_BT_SETTINGS AND NOT DEFINED CONFIG_BT_SETTINGS_LOG_LEVEL)
    set(CONFIG_BT_SETTINGS_LOG_LEVEL 3)
endif()

if(CONFIG_BT_SETTINGS AND NOT DEFINED CONFIG_BT_SETTINGS_CCC_STORE_MAX)
    set(CONFIG_BT_SETTINGS_CCC_STORE_MAX 48)
endif()

if(NOT DEFINED CONFIG_BT_ATT_PREPARE_COUNT)
    set(CONFIG_BT_ATT_PREPARE_COUNT 0)
endif()

if(CONFIG_BT_AUDIO)
    if(NOT DEFINED CONFIG_BT_AUDIO_LOG_LEVEL)
        set(CONFIG_BT_AUDIO_LOG_LEVEL 3)
    endif()
    if(NOT DEFINED CONFIG_BT_AUDIO_CODEC_LOG_LEVEL)
        set(CONFIG_BT_AUDIO_CODEC_LOG_LEVEL 3)
    endif()
    if(NOT DEFINED CONFIG_BT_AUDIO_CODEC_CFG_MAX_DATA_SIZE)
        set(CONFIG_BT_AUDIO_CODEC_CFG_MAX_DATA_SIZE 19)
    endif()
    if(NOT DEFINED CONFIG_BT_AUDIO_CODEC_CFG_MAX_METADATA_SIZE)
        set(CONFIG_BT_AUDIO_CODEC_CFG_MAX_METADATA_SIZE 4)
    endif()
    if(NOT DEFINED CONFIG_BT_AUDIO_CODEC_CAP_MAX_DATA_SIZE)
        set(CONFIG_BT_AUDIO_CODEC_CAP_MAX_DATA_SIZE 19)
    endif()
    if(NOT DEFINED CONFIG_BT_AUDIO_CODEC_CAP_MAX_METADATA_SIZE)
        set(CONFIG_BT_AUDIO_CODEC_CAP_MAX_METADATA_SIZE 4)
    endif()
    if(NOT DEFINED CONFIG_BT_AUDIO_NOTIFY_RETRY_DELAY)
        set(CONFIG_BT_AUDIO_NOTIFY_RETRY_DELAY 20)
    endif()
    if(NOT DEFINED CONFIG_BT_ISO_MAX_CHAN)
        set(CONFIG_BT_ISO_MAX_CHAN 2)
    endif()
    if(NOT DEFINED CONFIG_BT_ISO_MAX_CIG)
        set(CONFIG_BT_ISO_MAX_CIG 1)
    endif()
    if(NOT DEFINED CONFIG_BT_ISO_MAX_BIG)
        set(CONFIG_BT_ISO_MAX_BIG 0)
    endif()
    if(NOT DEFINED CONFIG_BT_ISO_LOG_LEVEL)
        set(CONFIG_BT_ISO_LOG_LEVEL 3)
    endif()
    if(NOT DEFINED CONFIG_BT_ISO_TX_BUF_COUNT)
        # IPRO controller's LE Read Buffer Size V2 currently reports five ISO
        # HCI data packet credits (BLE_HCI_ISO_IN_SDU_BUF_NB).
        set(CONFIG_BT_ISO_TX_BUF_COUNT 5)
    endif()
    if(NOT DEFINED CONFIG_BT_ISO_TX_MTU)
        set(CONFIG_BT_ISO_TX_MTU 251)
    endif()
    if(NOT DEFINED CONFIG_BT_ISO_RX_BUF_COUNT)
        set(CONFIG_BT_ISO_RX_BUF_COUNT 4)
    endif()
    if(NOT DEFINED CONFIG_BT_ISO_RX_MTU)
        set(CONFIG_BT_ISO_RX_MTU 251)
    endif()
    if(CONFIG_BT_BAP_UNICAST_SERVER AND NOT DEFINED CONFIG_BT_BAP_STREAM)
        set(CONFIG_BT_BAP_STREAM 1)
    endif()
    if(CONFIG_BT_BAP_UNICAST_CLIENT AND NOT DEFINED CONFIG_BT_BAP_STREAM)
        set(CONFIG_BT_BAP_STREAM 1)
    endif()
    if(CONFIG_BT_BAP_UNICAST_CLIENT AND NOT DEFINED CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_COUNT)
        set(CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_COUNT 1)
    endif()
    if(CONFIG_BT_BAP_UNICAST_CLIENT AND NOT DEFINED CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_STREAM_COUNT)
        set(CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_STREAM_COUNT 1)
    endif()
    if(CONFIG_BT_BAP_UNICAST_CLIENT AND NOT DEFINED CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK_COUNT)
        set(CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK_COUNT 2)
    endif()
    if(CONFIG_BT_BAP_UNICAST_CLIENT AND NOT DEFINED CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT)
        set(CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT 0)
    endif()
    if(NOT DEFINED CONFIG_BT_BAP_UNICAST_CLIENT_LOG_LEVEL)
        set(CONFIG_BT_BAP_UNICAST_CLIENT_LOG_LEVEL 3)
    endif()
    if(NOT DEFINED CONFIG_BT_BAP_ISO_LOG_LEVEL)
        set(CONFIG_BT_BAP_ISO_LOG_LEVEL 3)
    endif()
    if(NOT DEFINED CONFIG_BT_BAP_STREAM_LOG_LEVEL)
        set(CONFIG_BT_BAP_STREAM_LOG_LEVEL 3)
    endif()
    if(NOT DEFINED CONFIG_BT_BAP_UNICAST_SERVER_LOG_LEVEL)
        set(CONFIG_BT_BAP_UNICAST_SERVER_LOG_LEVEL 3)
    endif()
    if(NOT DEFINED CONFIG_BT_BAP_BASS_MAX_SUBGROUPS)
        set(CONFIG_BT_BAP_BASS_MAX_SUBGROUPS 1)
    endif()
    if(NOT DEFINED CONFIG_BT_ASCS_LOG_LEVEL)
        set(CONFIG_BT_ASCS_LOG_LEVEL 3)
    endif()
    if(CONFIG_BT_ASCS AND CONFIG_BT_ATT_PREPARE_COUNT LESS 4)
        set(CONFIG_BT_ATT_PREPARE_COUNT 4)
    endif()
    if(NOT DEFINED CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT)
        set(CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT 2)
    endif()
    if(NOT DEFINED CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT)
        set(CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT 0)
    endif()
    if(NOT DEFINED CONFIG_BT_ASCS_MAX_ACTIVE_ASES)
        set(CONFIG_BT_ASCS_MAX_ACTIVE_ASES ${CONFIG_BT_ISO_MAX_CHAN})
    endif()
    if(NOT DEFINED CONFIG_BT_ASCS_ISO_DISCONNECT_DELAY)
        set(CONFIG_BT_ASCS_ISO_DISCONNECT_DELAY 500)
    endif()
    if(NOT DEFINED CONFIG_BT_ASCS_ASE_BUF_TIMEOUT)
        set(CONFIG_BT_ASCS_ASE_BUF_TIMEOUT 50)
    endif()
    if(CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT GREATER 0)
        set(CONFIG_BT_ASCS_ASE_SNK 1)
    endif()
    if(CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT GREATER 0)
        set(CONFIG_BT_ASCS_ASE_SRC 1)
    endif()
    if(NOT DEFINED CONFIG_BT_PACS_LOG_LEVEL)
        set(CONFIG_BT_PACS_LOG_LEVEL 3)
    endif()
    if(NOT DEFINED CONFIG_BT_PACS_SUPPORTED_CONTEXT_NOTIFIABLE)
        set(CONFIG_BT_PACS_SUPPORTED_CONTEXT_NOTIFIABLE 1)
    endif()
    if(NOT DEFINED CONFIG_BT_PAC_SNK_LOC)
        set(CONFIG_BT_PAC_SNK_LOC 1)
    endif()
    if(NOT DEFINED CONFIG_BT_VCP_VOL_REND_LOG_LEVEL)
        set(CONFIG_BT_VCP_VOL_REND_LOG_LEVEL 3)
    endif()
    if(NOT DEFINED CONFIG_BT_VCP_VOL_REND_VOCS_INSTANCE_COUNT)
        set(CONFIG_BT_VCP_VOL_REND_VOCS_INSTANCE_COUNT 0)
    endif()
    if(NOT DEFINED CONFIG_BT_VCP_VOL_REND_AICS_INSTANCE_COUNT)
        set(CONFIG_BT_VCP_VOL_REND_AICS_INSTANCE_COUNT 0)
    endif()
    if(NOT DEFINED CONFIG_BT_CAP_ACCEPTOR_LOG_LEVEL)
        set(CONFIG_BT_CAP_ACCEPTOR_LOG_LEVEL 3)
    endif()
    if(NOT DEFINED CONFIG_BT_CAP_INITIATOR_LOG_LEVEL)
        set(CONFIG_BT_CAP_INITIATOR_LOG_LEVEL 3)
    endif()
    if(CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER AND NOT DEFINED CONFIG_BT_CSIP_SET_MEMBER)
        set(CONFIG_BT_CSIP_SET_MEMBER 1)
    endif()
    if(NOT DEFINED CONFIG_BT_CSIP_SET_MEMBER_LOG_LEVEL)
        set(CONFIG_BT_CSIP_SET_MEMBER_LOG_LEVEL 3)
    endif()
    if(NOT DEFINED CONFIG_BT_CSIP_SET_MEMBER_CRYPTO_LOG_LEVEL)
        set(CONFIG_BT_CSIP_SET_MEMBER_CRYPTO_LOG_LEVEL 3)
    endif()
    if(NOT DEFINED CONFIG_BT_CSIP_SET_MEMBER_MAX_INSTANCE_COUNT)
        set(CONFIG_BT_CSIP_SET_MEMBER_MAX_INSTANCE_COUNT 1)
    endif()
    if(NOT DEFINED CONFIG_BT_CSIP_SET_MEMBER_ENC_SIRK_SUPPORT)
        set(CONFIG_BT_CSIP_SET_MEMBER_ENC_SIRK_SUPPORT 1)
    endif()
    foreach(_ipro_audio_log
            BT_VCP_VOL_CTLR BT_VOCS BT_VOCS_CLIENT BT_AICS BT_AICS_CLIENT
            BT_MICP_MIC_DEV BT_MICP_MIC_CTLR BT_CSIP_SET_COORDINATOR
            BT_CAP_COMMANDER BT_CAP_COMMON BT_CAP_HANDOVER BT_CAP_STREAM
            BT_BAP_BASE BT_BAP_BROADCAST_SOURCE BT_BAP_BROADCAST_SINK
            BT_BAP_SCAN_DELEGATOR BT_BAP_BROADCAST_ASSISTANT
            BT_TBS BT_TBS_CLIENT BT_CCP_CALL_CONTROL_SERVER
            BT_CCP_CALL_CONTROL_CLIENT BT_MCS BT_MCC BT_MPL BT_HAS
            BT_HAS_CLIENT BT_TMAP BT_GMAP BT_PBP)
        if(NOT DEFINED CONFIG_${_ipro_audio_log}_LOG_LEVEL)
            set(CONFIG_${_ipro_audio_log}_LOG_LEVEL 3)
        endif()
    endforeach()
    if(NOT DEFINED CONFIG_BT_VOCS_MAX_INSTANCE_COUNT)
        set(CONFIG_BT_VOCS_MAX_INSTANCE_COUNT 1)
    endif()
    if(NOT DEFINED CONFIG_BT_VOCS_CLIENT_MAX_INSTANCE_COUNT)
        set(CONFIG_BT_VOCS_CLIENT_MAX_INSTANCE_COUNT 1)
    endif()
    if(NOT DEFINED CONFIG_BT_VOCS_MAX_OUTPUT_DESCRIPTION_SIZE)
        set(CONFIG_BT_VOCS_MAX_OUTPUT_DESCRIPTION_SIZE 16)
    endif()
    if(NOT DEFINED CONFIG_BT_AICS_MAX_INSTANCE_COUNT)
        set(CONFIG_BT_AICS_MAX_INSTANCE_COUNT 1)
    endif()
    if(NOT DEFINED CONFIG_BT_AICS_CLIENT_MAX_INSTANCE_COUNT)
        set(CONFIG_BT_AICS_CLIENT_MAX_INSTANCE_COUNT 1)
    endif()
    if(NOT DEFINED CONFIG_BT_AICS_MAX_INPUT_DESCRIPTION_SIZE)
        set(CONFIG_BT_AICS_MAX_INPUT_DESCRIPTION_SIZE 16)
    endif()
    if(NOT DEFINED CONFIG_BT_CSIP_SET_COORDINATOR_MAX_CSIS_INSTANCES)
        set(CONFIG_BT_CSIP_SET_COORDINATOR_MAX_CSIS_INSTANCES 1)
    endif()
    if(NOT DEFINED CONFIG_BT_CSIP_SET_COORDINATOR_ENC_SIRK_SUPPORT)
        set(CONFIG_BT_CSIP_SET_COORDINATOR_ENC_SIRK_SUPPORT 1)
    endif()
    if(NOT DEFINED CONFIG_BT_TBS_BEARER_COUNT)
        set(CONFIG_BT_TBS_BEARER_COUNT 1)
    endif()
    if(NOT DEFINED CONFIG_BT_TBS_MAX_CALLS)
        set(CONFIG_BT_TBS_MAX_CALLS 1)
    endif()
    if(NOT DEFINED CONFIG_BT_TBS_MAX_URI_LENGTH)
        set(CONFIG_BT_TBS_MAX_URI_LENGTH 32)
    endif()
    if(NOT DEFINED CONFIG_BT_TBS_MAX_PROVIDER_NAME_LENGTH)
        set(CONFIG_BT_TBS_MAX_PROVIDER_NAME_LENGTH 32)
    endif()
    if(NOT DEFINED CONFIG_BT_TBS_MAX_FRIENDLY_NAME_LENGTH)
        set(CONFIG_BT_TBS_MAX_FRIENDLY_NAME_LENGTH 32)
    endif()
    if(NOT DEFINED CONFIG_BT_TBS_MAX_SCHEME_LIST_LENGTH)
        set(CONFIG_BT_TBS_MAX_SCHEME_LIST_LENGTH 32)
    endif()
    if(NOT DEFINED CONFIG_BT_TBS_LOCK_TIMEOUT)
        set(CONFIG_BT_TBS_LOCK_TIMEOUT 500)
    endif()
    if(NOT DEFINED CONFIG_BT_TBS_CLIENT_MAX_CALLS)
        set(CONFIG_BT_TBS_CLIENT_MAX_CALLS 1)
    endif()
    if(NOT DEFINED CONFIG_BT_TBS_CLIENT_MAX_TBS_INSTANCES)
        set(CONFIG_BT_TBS_CLIENT_MAX_TBS_INSTANCES 1)
    endif()
    if(NOT DEFINED CONFIG_BT_CCP_CALL_CONTROL_CLIENT_BEARER_COUNT)
        set(CONFIG_BT_CCP_CALL_CONTROL_CLIENT_BEARER_COUNT 1)
    endif()
    if(NOT DEFINED CONFIG_BT_CCP_CALL_CONTROL_SERVER_BEARER_COUNT)
        set(CONFIG_BT_CCP_CALL_CONTROL_SERVER_BEARER_COUNT 1)
    endif()
    if(NOT DEFINED CONFIG_BT_CCP_CALL_CONTROL_SERVER_PROVIDER_NAME_MAX_LENGTH)
        set(CONFIG_BT_CCP_CALL_CONTROL_SERVER_PROVIDER_NAME_MAX_LENGTH 32)
    endif()
    if(NOT DEFINED CONFIG_BT_GATT_DYNAMIC_DB)
        set(CONFIG_BT_GATT_DYNAMIC_DB 1)
    endif()
    if(NOT DEFINED CONFIG_BT_MCS_LOG_LEVEL)
        set(CONFIG_BT_MCS_LOG_LEVEL 3)
    endif()
    if(NOT DEFINED CONFIG_BT_MCC_LOG_LEVEL)
        set(CONFIG_BT_MCC_LOG_LEVEL 3)
    endif()
    if(NOT DEFINED CONFIG_BT_MCC_MEDIA_PLAYER_NAME_MAX)
        set(CONFIG_BT_MCC_MEDIA_PLAYER_NAME_MAX 20)
    endif()
    if(NOT DEFINED CONFIG_BT_MCC_ICON_URL_MAX)
        set(CONFIG_BT_MCC_ICON_URL_MAX 40)
    endif()
    if(NOT DEFINED CONFIG_BT_MCC_TRACK_TITLE_MAX)
        set(CONFIG_BT_MCC_TRACK_TITLE_MAX 40)
    endif()
    if(NOT DEFINED CONFIG_BT_MCC_SEGMENT_NAME_MAX)
        set(CONFIG_BT_MCC_SEGMENT_NAME_MAX 25)
    endif()
    if(NOT DEFINED CONFIG_BT_MCC_OTC_OBJ_BUF_SIZE)
        set(CONFIG_BT_MCC_OTC_OBJ_BUF_SIZE 512)
    endif()
    if(NOT DEFINED CONFIG_BT_MCC_TOTAL_OBJ_CONTENT_MEM)
        set(CONFIG_BT_MCC_TOTAL_OBJ_CONTENT_MEM 1)
    endif()
    if(NOT DEFINED CONFIG_BT_MCC_TRACK_SEGS_MAX_CNT)
        set(CONFIG_BT_MCC_TRACK_SEGS_MAX_CNT 25)
    endif()
    if(NOT DEFINED CONFIG_BT_MCC_GROUP_RECORDS_MAX)
        set(CONFIG_BT_MCC_GROUP_RECORDS_MAX 25)
    endif()
    if(NOT DEFINED CONFIG_BT_MPL_LOG_LEVEL)
        set(CONFIG_BT_MPL_LOG_LEVEL 3)
    endif()
    if(NOT DEFINED CONFIG_BT_MPL_MEDIA_PLAYER_NAME)
        set(CONFIG_BT_MPL_MEDIA_PLAYER_NAME Player0)
    endif()
    if(NOT DEFINED CONFIG_BT_MPL_MEDIA_PLAYER_NAME_MAX)
        set(CONFIG_BT_MPL_MEDIA_PLAYER_NAME_MAX 20)
    endif()
    if(NOT DEFINED CONFIG_BT_MPL_ICON_URL)
        set(CONFIG_BT_MPL_ICON_URL http://server.some.where/path/icon.png)
    endif()
    if(NOT DEFINED CONFIG_BT_MPL_ICON_URL_MAX)
        set(CONFIG_BT_MPL_ICON_URL_MAX 40)
    endif()
    if(NOT DEFINED CONFIG_BT_MPL_TRACK_TITLE_MAX)
        set(CONFIG_BT_MPL_TRACK_TITLE_MAX 40)
    endif()
    if(NOT DEFINED CONFIG_BT_MPL_SEGMENT_NAME_MAX)
        set(CONFIG_BT_MPL_SEGMENT_NAME_MAX 25)
    endif()
    if(NOT DEFINED CONFIG_BT_MPL_GROUP_TITLE_MAX)
        set(CONFIG_BT_MPL_GROUP_TITLE_MAX ${CONFIG_BT_MPL_TRACK_TITLE_MAX})
    endif()
    if(NOT DEFINED CONFIG_BT_MPL_MAX_OBJ_SIZE)
        set(CONFIG_BT_MPL_MAX_OBJ_SIZE 127)
    endif()
    if(NOT DEFINED CONFIG_BT_MPL_ICON_BITMAP_SIZE)
        set(CONFIG_BT_MPL_ICON_BITMAP_SIZE 127)
    endif()
    if(NOT DEFINED CONFIG_BT_MPL_TRACK_MAX_SIZE)
        set(CONFIG_BT_MPL_TRACK_MAX_SIZE 127)
    endif()
    if(NOT DEFINED CONFIG_MCTL_LOG_LEVEL)
        set(CONFIG_MCTL_LOG_LEVEL 3)
    endif()
endif()

if(CONFIG_BT_OTS OR CONFIG_BT_OTS_CLIENT)
    sdk_ifndef(CONFIG_BT_OTS_LOG_LEVEL 3)
    sdk_ifndef(CONFIG_BT_OTS_CLIENT_LOG_LEVEL 3)
    sdk_ifndef(CONFIG_BT_OTS_MAX_INST_CNT 1)
    sdk_ifndef(CONFIG_BT_OTS_MAX_OBJ_CNT 0x05)
    sdk_ifndef(CONFIG_BT_OTS_L2CAP_CHAN_TX_MTU 256)
    sdk_ifndef(CONFIG_BT_OTS_L2CAP_CHAN_RX_MTU ${CONFIG_BT_BUF_ACL_RX_SIZE})
    sdk_ifndef(CONFIG_BT_OTS_OBJ_MAX_NAME_LEN 120)
endif()

# Zephyr's central-only extended-create path computes num_phys for a debug
# assertion.  This port intentionally compiles Zephyr assertions out, leaving
# the local set-but-unused in pristine hci_core.c.  Scope the exception to that
# upstream translation unit instead of weakening warnings for the Host port.
set_source_files_properties(
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/hci_core.c
    PROPERTIES COMPILE_OPTIONS "-Wno-unused-but-set-variable"
)

# RFCOMM parses the MCC length into a local used only by upstream debug
# assertions/logging.  The IPRO log shim compiles that debug path out, so keep
# this exception local to the imported source file.
set_source_files_properties(
    ${IPRO_BT_HOST_ZEPHYR}/subsys/bluetooth/host/classic/rfcomm.c
    PROPERTIES COMPILE_OPTIONS "-Wno-unused-but-set-variable"
)

__component_add_compile_definitions(
    -DIPRO_BT_HOST_ZEPHYR_VERSION=0x040400
    # This component is the HCI Host.  Zephyr's public buffer helpers gate
    # BT_BUF_ACL_RX_COUNT on CONFIG_BT_HCI_HOST; omitting it made RFCOMM derive
    # RFCOMM_MAX_CREDITS from zero and advertise an underflowed 255 credits.
    -DCONFIG_BT_HCI_HOST=1
    -DCONFIG_RISCV=1
    -DCONFIG_LITTLE_ENDIAN=1
    -DCONFIG_NET_BUF_ALIGNMENT=0
    -DCONFIG_NET_BUF_WARN_ALLOC_INTERVAL=0
    -DCONFIG_ATOMIC_OPERATIONS_BUILTIN=1
    -DCONFIG_SYS_CLOCK_TICKS_PER_SEC=${CONFIG_SYS_CLOCK_TICKS_PER_SEC}
    -DCONFIG_BT_BUF_ACL_TX_COUNT=${CONFIG_BT_BUF_ACL_TX_COUNT}
    -DCONFIG_BT_BUF_EVT_RX_COUNT=${CONFIG_BT_BUF_EVT_RX_COUNT}
    -DCONFIG_BT_BUF_ACL_RX_COUNT_EXTRA=1
    -DCONFIG_BT_BUF_ACL_RX_SIZE=${CONFIG_BT_BUF_ACL_RX_SIZE}
    -DCONFIG_BT_BUF_EVT_RX_SIZE=255
    -DCONFIG_BT_BUF_EVT_DISCARDABLE_COUNT=3
    -DCONFIG_BT_BUF_EVT_DISCARDABLE_SIZE=${CONFIG_BT_BUF_EVT_DISCARDABLE_SIZE}
    -DCONFIG_BT_BUF_SYNC_EVT_POOL_COUNT=1
    -DCONFIG_BT_ID_MAX=1
    -DCONFIG_BT_MAX_CONN=${CONFIG_BT_CONN}
    -DCONFIG_BT_CREATE_CONN_TIMEOUT=3
    -DCONFIG_BT_BACKGROUND_SCAN_INTERVAL=2048
    -DCONFIG_BT_BACKGROUND_SCAN_WINDOW=18
    -DCONFIG_BT_EXT_SCAN_BUF_SIZE=229
    -DCONFIG_BT_BUF_CMD_TX_COUNT=2
    -DCONFIG_BT_BUF_CMD_TX_SIZE=255
    -DCONFIG_BT_EXT_ADV_MAX_ADV_SET=1
    -DCONFIG_BT_LIM_ADV_TIMEOUT=60
    -DCONFIG_BT_RECV_WORKQ_BT=1
    -DCONFIG_BT_RX_STACK_SIZE=${CONFIG_BT_RX_STACK_SIZE}
    -DCONFIG_BT_RX_PRIO=6
    -DCONFIG_BT_DEVICE_NAME=\"${CONFIG_BT_DEVICE_NAME}\"
    -DCONFIG_BT_DEVICE_APPEARANCE=0
    # This SDK imports Zephyr Host sources directly instead of running Zephyr's
    # full Kconfig select graph for each app.  Mirror the dependencies selected
    # by CONFIG_BT_SMP in upstream Kconfig so SMP authreq and implementation
    # stay coherent: SC pairing requires CONFIG_BT_ECC, and the local port
    # provides TinyCrypt/TRNG backed ECC and crypto primitives.
    -DCONFIG_BT_CRYPTO=1
    -DCONFIG_BT_RPA=1
    -DCONFIG_BT_ECC=1
    -DCONFIG_BT_CONN_TX_USER_DATA_SIZE=16
    -DCONFIG_BT_CONN_TX=1
    -DCONFIG_BT_CONN_TX_MAX=${CONFIG_BT_CONN_TX_MAX}
    -DCONFIG_BT_CONN_DYNAMIC_CALLBACKS=1
    # Exercise a full data-length packet. 247-byte ATT MTU fits the
    # controller's 251-byte ACL payload once the four-byte L2CAP header is
    # included.
    -DCONFIG_BT_L2CAP_TX_MTU=247
    -DCONFIG_BT_CONN_PARAM_UPDATE_TIMEOUT=5000
    -DCONFIG_BT_CONN_FRAG_COUNT=1
    -DCONFIG_BT_MAX_PAIRED=${CONFIG_BT_MAX_PAIRED}
    -DCONFIG_BT_L2CAP_TX_BUF_COUNT=${CONFIG_BT_L2CAP_TX_BUF_COUNT}
    -DCONFIG_BT_RPA_TIMEOUT=${CONFIG_BT_RPA_TIMEOUT}
    -DCONFIG_BT_ATT_TX_COUNT=10
    -DCONFIG_BT_ATT_PREPARE_COUNT=${CONFIG_BT_ATT_PREPARE_COUNT}
    -DCONFIG_BT_SMP_MIN_ENC_KEY_SIZE=16
    -DCONFIG_ASSERT=1
    -DCONFIG_ASSERT_LEVEL=2
    -DCONFIG_ASSERT_VERBOSE=1
    -DK_HEAP_MEM_POOL_SIZE=0
)

if(CONFIG_BT_EXT_ADV)
    __component_add_compile_definitions(-DCONFIG_BT_EXT_ADV=1)
endif()

# These feature switches may have been derived by resolve_config.cmake rather
# than appearing literally in autoconf.h. Forward them uniformly so source
# selection and preprocessor selection always describe the same Host image.
foreach(_ipro_host_bool
        CONFIG_BT_GATT_GAP_SERVICE CONFIG_BT_GATT_GAP_SVC_VALIDATE
        CONFIG_BT_GATT_SECURITY_LEVELS
        CONFIG_BT_SMP_ENFORCE_MITM
        CONFIG_BT_EXT_ADV_CODING_SELECTION
        CONFIG_BT_SUBRATING
        CONFIG_BT_L2CAP_DYNAMIC_CHANNEL
        CONFIG_BT_L2CAP_ECRED
        CONFIG_BT_L2CAP_ENH_RET CONFIG_BT_L2CAP_RET_FC
        CONFIG_BT_GATT_CLIENT CONFIG_BT_GATT_AUTO_DISCOVER_CCC
        CONFIG_BT_GATT_AUTO_UPDATE_MTU CONFIG_BT_GATT_DYNAMIC_DB
        CONFIG_BT_EATT CONFIG_BT_EATT_AUTO_CONNECT
        CONFIG_BT_GATT_READ_MULT_VAR_LEN
        CONFIG_BT_PER_ADV CONFIG_BT_PER_ADV_SYNC
        CONFIG_BT_PER_ADV_RSP CONFIG_BT_PER_ADV_SYNC_RSP
        CONFIG_BT_PER_ADV_SYNC_TRANSFER_SENDER
        CONFIG_BT_PER_ADV_SYNC_TRANSFER_RECEIVER
        CONFIG_BT_DF CONFIG_BT_DF_CONNECTIONLESS_CTE_RX
        CONFIG_BT_DF_CONNECTIONLESS_CTE_TX CONFIG_BT_DF_CONNECTION_CTE_RX
        CONFIG_BT_DF_CONNECTION_CTE_TX CONFIG_BT_DF_CONNECTION_CTE_REQ
        CONFIG_BT_DF_CONNECTION_CTE_RSP CONFIG_BT_DF_CTE_RX_AOA
        CONFIG_BT_DF_CTE_TX_AOD
        CONFIG_BT_ISO CONFIG_BT_ISO_RX CONFIG_BT_ISO_TX
        CONFIG_BT_ISO_CENTRAL CONFIG_BT_ISO_PERIPHERAL
        CONFIG_BT_ISO_UNICAST CONFIG_BT_ISO_BROADCAST
        CONFIG_BT_ISO_BROADCASTER CONFIG_BT_ISO_SYNC_RECEIVER
        CONFIG_BT_AUDIO CONFIG_BT_AUDIO_RX CONFIG_BT_AUDIO_TX
        CONFIG_BT_BAP_STREAM CONFIG_BT_BAP_BASE CONFIG_BT_BAP_UNICAST
        CONFIG_BT_BAP_UNICAST_SERVER CONFIG_BT_BAP_UNICAST_CLIENT
        CONFIG_BT_BAP_BROADCAST_SOURCE CONFIG_BT_BAP_BROADCAST_SINK
        CONFIG_BT_BAP_SCAN_DELEGATOR CONFIG_BT_BAP_BROADCAST_ASSISTANT
        CONFIG_BT_ASCS CONFIG_BT_PACS CONFIG_BT_PAC_SNK CONFIG_BT_PAC_SRC
        CONFIG_BT_VCP_VOL_REND CONFIG_BT_VCP_VOL_CTLR
        CONFIG_BT_VOCS CONFIG_BT_VOCS_CLIENT CONFIG_BT_AICS CONFIG_BT_AICS_CLIENT
        CONFIG_BT_MICP_MIC_DEV CONFIG_BT_MICP_MIC_CTLR
        CONFIG_BT_CSIP_SET_MEMBER CONFIG_BT_CSIP_SET_COORDINATOR
        CONFIG_BT_CAP CONFIG_BT_CAP_ACCEPTOR CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER
        CONFIG_BT_CAP_INITIATOR CONFIG_BT_CAP_INITIATOR_UNICAST
        CONFIG_BT_CAP_COMMANDER CONFIG_BT_CAP_HANDOVER
        CONFIG_BT_TBS CONFIG_BT_TBS_CLIENT
        CONFIG_BT_CCP_CALL_CONTROL_SERVER CONFIG_BT_CCP_CALL_CONTROL_CLIENT
        CONFIG_BT_MCS CONFIG_BT_MCC CONFIG_BT_MPL CONFIG_MCTL
        CONFIG_MCTL_LOCAL_PLAYER_CONTROL CONFIG_MCTL_LOCAL_PLAYER_LOCAL_CONTROL
        CONFIG_MCTL_LOCAL_PLAYER_REMOTE_CONTROL CONFIG_MCTL_REMOTE_PLAYER_CONTROL
        CONFIG_MCTL_REMOTE_PLAYER_CONTROL_OBJECTS CONFIG_BT_MPL_OBJECTS
        CONFIG_BT_MCC_OTS CONFIG_BT_OTS CONFIG_BT_OTS_CLIENT
        CONFIG_BT_OTS_DIR_LIST_OBJ CONFIG_BT_TBS_CLIENT_GTBS
        CONFIG_BT_TBS_CLIENT_TBS CONFIG_BT_OTS_OACP_READ_SUPPORT
        CONFIG_BT_OTS_OLCP_GO_TO_SUPPORT
        CONFIG_BT_HAS CONFIG_BT_HAS_CLIENT CONFIG_BT_TMAP CONFIG_BT_GMAP
        CONFIG_BT_PBP)
    if(${_ipro_host_bool})
        __component_add_compile_definitions(-D${_ipro_host_bool}=1)
    endif()
endforeach()

if(CONFIG_BT_DF)
    __component_add_compile_definitions(-DCONFIG_BT_DF_LOG_LEVEL=${CONFIG_BT_DF_LOG_LEVEL})
endif()

if(CONFIG_BT_PER_ADV_SYNC)
    __component_add_compile_definitions(
        -DCONFIG_BT_PER_ADV_SYNC_MAX=${CONFIG_BT_PER_ADV_SYNC_MAX}
        -DCONFIG_BT_PER_ADV_SYNC_BUF_SIZE=${CONFIG_BT_PER_ADV_SYNC_BUF_SIZE}
    )
endif()
if(CONFIG_BT_EATT)
    __component_add_compile_definitions(-DCONFIG_BT_EATT_MAX=${CONFIG_BT_EATT_MAX})
endif()
if(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
    __component_add_compile_definitions(
        -DCONFIG_BT_L2CAP_CONN_RTX_TIMEOUT=${CONFIG_BT_L2CAP_CONN_RTX_TIMEOUT}
    )
endif()
if(CONFIG_BT_L2CAP_RET_FC)
    __component_add_compile_definitions(
        -DCONFIG_BT_L2CAP_MPS=${CONFIG_BT_L2CAP_MPS}
        -DCONFIG_BT_L2CAP_MAX_WINDOW_SIZE=${CONFIG_BT_L2CAP_MAX_WINDOW_SIZE}
    )
endif()

if(CONFIG_BT_BREDR)
    __component_add_compile_definitions(
        -DCONFIG_BT_CLASSIC=1
        -DCONFIG_BT_LIMITED_DISCOVERABLE_DURATION=${CONFIG_BT_LIMITED_DISCOVERABLE_DURATION}
        -DCONFIG_BT_BR_MIN_ENC_KEY_SIZE=${CONFIG_BT_BR_MIN_ENC_KEY_SIZE}
        -DCONFIG_BT_MAX_SCO_CONN=${CONFIG_BT_MAX_SCO_CONN}
        -DCONFIG_BT_L2CAP_RX_FLUSH_TO=${CONFIG_BT_L2CAP_RX_FLUSH_TO}
        -DCONFIG_BT_L2CAP_BR_RET_TIMEOUT=${CONFIG_BT_L2CAP_BR_RET_TIMEOUT}
        -DCONFIG_BT_L2CAP_BR_MONITOR_TIMEOUT=${CONFIG_BT_L2CAP_BR_MONITOR_TIMEOUT}
        -DCONFIG_BT_PAGE_TIMEOUT=${CONFIG_BT_PAGE_TIMEOUT}
        -DCONFIG_BT_COD=${CONFIG_BT_COD}
        -DCONFIG_BT_L2CAP_CONN_RTX_TIMEOUT=${CONFIG_BT_L2CAP_CONN_RTX_TIMEOUT}
    )

    if(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
        __component_add_compile_definitions(-DCONFIG_BT_L2CAP_DYNAMIC_CHANNEL=1)
    endif()

    if(CONFIG_BT_RFCOMM)
        __component_add_compile_definitions(
            -DCONFIG_BT_RFCOMM=1
            -DCONFIG_BT_RFCOMM_L2CAP_MTU=${CONFIG_BT_RFCOMM_L2CAP_MTU}
            -DCONFIG_BT_RFCOMM_TX_MAX=${CONFIG_BT_RFCOMM_TX_MAX}
            -DCONFIG_BT_RFCOMM_LOG_LEVEL=${CONFIG_BT_RFCOMM_LOG_LEVEL}
        )
    endif()

    if(CONFIG_BT_AVDTP)
        __component_add_compile_definitions(
            -DCONFIG_BT_AVDTP=1
            -DCONFIG_BT_AVDTP_RTP_VERSION=${CONFIG_BT_AVDTP_RTP_VERSION}
            -DCONFIG_BT_AVDTP_SIGNAL_SDU_MAX=${CONFIG_BT_AVDTP_SIGNAL_SDU_MAX}
            -DCONFIG_BT_AVDTP_LOG_LEVEL=${CONFIG_BT_AVDTP_LOG_LEVEL}
        )
    endif()

    if(CONFIG_BT_A2DP)
        __component_add_compile_definitions(
            -DCONFIG_BT_A2DP=1
            -DCONFIG_BT_A2DP_LOG_LEVEL=${CONFIG_BT_A2DP_LOG_LEVEL}
        )
        if(CONFIG_BT_A2DP_SOURCE)
            __component_add_compile_definitions(-DCONFIG_BT_A2DP_SOURCE=1)
        endif()
        if(CONFIG_BT_A2DP_SINK)
            __component_add_compile_definitions(-DCONFIG_BT_A2DP_SINK=1)
        endif()
    endif()

    if(CONFIG_BT_AVCTP)
        __component_add_compile_definitions(
            -DCONFIG_BT_AVCTP=1
            -DCONFIG_BT_AVCTP_LOG_LEVEL=${CONFIG_BT_AVCTP_LOG_LEVEL}
        )
    endif()

    if(CONFIG_BT_AVRCP)
        __component_add_compile_definitions(
            -DCONFIG_BT_AVRCP=1
            -DCONFIG_BT_AVRCP_LOG_LEVEL=${CONFIG_BT_AVRCP_LOG_LEVEL}
            -DCONFIG_BT_AVRCP_VD_RX_SIZE=${CONFIG_BT_AVRCP_VD_RX_SIZE}
            -DCONFIG_BT_AVRCP_BROWSING_L2CAP_MTU=${CONFIG_BT_AVRCP_BROWSING_L2CAP_MTU}
        )
        if(CONFIG_BT_AVRCP_CONTROLLER)
            __component_add_compile_definitions(-DCONFIG_BT_AVRCP_CONTROLLER=1)
        endif()
        if(CONFIG_BT_AVRCP_TARGET)
            __component_add_compile_definitions(-DCONFIG_BT_AVRCP_TARGET=1)
        endif()
        if(CONFIG_BT_AVRCP_BROWSING)
            __component_add_compile_definitions(-DCONFIG_BT_AVRCP_BROWSING=1)
        endif()
        if(CONFIG_BT_AVRCP_TG_COVER_ART)
            __component_add_compile_definitions(-DCONFIG_BT_AVRCP_TG_COVER_ART=1)
        endif()
        if(CONFIG_BT_AVRCP_CT_COVER_ART)
            __component_add_compile_definitions(-DCONFIG_BT_AVRCP_CT_COVER_ART=1)
        endif()
    endif()

    if(CONFIG_BT_HFP)
        __component_add_compile_definitions(-DCONFIG_BT_HFP=1)
    endif()
    if(CONFIG_BT_HFP_HF)
        __component_add_compile_definitions(
            -DCONFIG_BT_HFP_HF=1
            -DCONFIG_BT_HFP_HF_LOG_LEVEL=${CONFIG_BT_HFP_HF_LOG_LEVEL}
            -DCONFIG_BT_HFP_HF_MAX_CALLS=${CONFIG_BT_HFP_HF_MAX_CALLS}
        )
        foreach(_ipro_hfp_hf_opt
                CLI VOLUME CODEC_NEG CODEC_MSBC CODEC_LC3_SWB ECNR
                3WAY_CALL ECS ECC VOICE_RECG ENH_VOICE_RECG VOICE_RECG_TEXT
                HF_INDICATORS HF_INDICATOR_ENH_SAFETY HF_INDICATOR_BATTERY)
            if(CONFIG_BT_HFP_HF_${_ipro_hfp_hf_opt})
                __component_add_compile_definitions(
                    -DCONFIG_BT_HFP_HF_${_ipro_hfp_hf_opt}=1)
            endif()
        endforeach()
    endif()
    if(CONFIG_BT_HFP_AG)
        __component_add_compile_definitions(
            -DCONFIG_BT_HFP_AG=1
            -DCONFIG_BT_HFP_AG_LOG_LEVEL=${CONFIG_BT_HFP_AG_LOG_LEVEL}
            -DCONFIG_BT_HFP_AG_MAX_CALLS=${CONFIG_BT_HFP_AG_MAX_CALLS}
            -DCONFIG_BT_HFP_AG_TX_BUF_COUNT=${CONFIG_BT_HFP_AG_TX_BUF_COUNT}
            -DCONFIG_BT_HFP_AG_OUTGOING_TIMEOUT=${CONFIG_BT_HFP_AG_OUTGOING_TIMEOUT}
            -DCONFIG_BT_HFP_AG_INCOMING_TIMEOUT=${CONFIG_BT_HFP_AG_INCOMING_TIMEOUT}
            -DCONFIG_BT_HFP_AG_ALERTING_TIMEOUT=${CONFIG_BT_HFP_AG_ALERTING_TIMEOUT}
            -DCONFIG_BT_HFP_AG_PHONE_NUMBER_MAX_LEN=${CONFIG_BT_HFP_AG_PHONE_NUMBER_MAX_LEN}
            -DCONFIG_BT_HFP_AG_RING_NOTIFY_INTERVAL=${CONFIG_BT_HFP_AG_RING_NOTIFY_INTERVAL}
            -DCONFIG_BT_HFP_AG_GET_ONGOING_CALL_TIMEOUT=${CONFIG_BT_HFP_AG_GET_ONGOING_CALL_TIMEOUT}
        )
        foreach(_ipro_hfp_ag_opt
                EXT_ERR CODEC_NEG CODEC_MSBC CODEC_LC3_SWB ECNR 3WAY_CALL
                ECS ECC VOICE_RECG ENH_VOICE_RECG VOICE_RECG_TEXT VOICE_TAG
                HF_INDICATORS HF_INDICATOR_ENH_SAFETY HF_INDICATOR_BATTERY
                REJECT_CALL INBAND_RINGTONE)
            if(CONFIG_BT_HFP_AG_${_ipro_hfp_ag_opt})
                __component_add_compile_definitions(
                    -DCONFIG_BT_HFP_AG_${_ipro_hfp_ag_opt}=1)
            endif()
        endforeach()
    endif()

    if(CONFIG_BT_GOEP)
        __component_add_compile_definitions(
            -DCONFIG_BT_GOEP=1
            -DCONFIG_BT_GOEP_LOG_LEVEL=${CONFIG_BT_GOEP_LOG_LEVEL}
            -DCONFIG_BT_GOEP_RFCOMM_MTU=${CONFIG_BT_GOEP_RFCOMM_MTU}
            -DCONFIG_BT_GOEP_L2CAP_MTU=${CONFIG_BT_GOEP_L2CAP_MTU}
        )
    endif()
    if(CONFIG_BT_BIP)
        __component_add_compile_definitions(
            -DCONFIG_BT_BIP=1
            -DCONFIG_BT_BIP_LOG_LEVEL=${CONFIG_BT_BIP_LOG_LEVEL}
        )
    endif()
    if(CONFIG_BT_DID)
        # Zephyr v4.4's public ABI contains the historical VEDNOR typo.
        __component_add_compile_definitions(
            -DCONFIG_BT_DID=1
            -DCONFIG_BT_DEVICE_VEDNOR_ID=${CONFIG_BT_DEVICE_VENDOR_ID}
            -DCONFIG_BT_DEVICE_PRODUCT_ID=${CONFIG_BT_DEVICE_PRODUCT_ID}
            -DCONFIG_BT_DEVICE_VERSION=${CONFIG_BT_DEVICE_VERSION}
        )
    endif()
endif()

if(CONFIG_PHY_UPDATE)
    __component_add_compile_definitions(
        -DCONFIG_BT_PHY_UPDATE=1
        -DCONFIG_BT_USER_PHY_UPDATE=1
    )
    foreach(_ipro_phy_role CENTRAL PERIPHERAL)
        foreach(_ipro_phy_pref NONE 1M 2M CODED)
            if(CONFIG_BT_AUTO_PHY_${_ipro_phy_role}_${_ipro_phy_pref})
                __component_add_compile_definitions(
                    -DCONFIG_BT_AUTO_PHY_${_ipro_phy_role}_${_ipro_phy_pref}=1
                )
            endif()
        endforeach()
    endforeach()
endif()

if(CONFIG_BT_TX_PROCESSOR_THREAD)
    __component_add_compile_definitions(
        -DCONFIG_BT_TX_PROCESSOR_THREAD=1
        -DCONFIG_BT_TX_PROCESSOR_THREAD_PRIO=${CONFIG_BT_TX_PROCESSOR_THREAD_PRIO}
        -DCONFIG_BT_TX_PROCESSOR_STACK_SIZE=${CONFIG_BT_TX_PROCESSOR_STACK_SIZE}
    )
endif()

if(CONFIG_BT_AUDIO)
    __component_add_compile_definitions(
        -DCONFIG_BT_AUDIO_LOG_LEVEL=${CONFIG_BT_AUDIO_LOG_LEVEL}
        -DCONFIG_BT_AUDIO_CODEC_LOG_LEVEL=${CONFIG_BT_AUDIO_CODEC_LOG_LEVEL}
        -DCONFIG_BT_AUDIO_CODEC_CFG_MAX_DATA_SIZE=${CONFIG_BT_AUDIO_CODEC_CFG_MAX_DATA_SIZE}
        -DCONFIG_BT_AUDIO_CODEC_CFG_MAX_METADATA_SIZE=${CONFIG_BT_AUDIO_CODEC_CFG_MAX_METADATA_SIZE}
        -DCONFIG_BT_AUDIO_CODEC_CAP_MAX_DATA_SIZE=${CONFIG_BT_AUDIO_CODEC_CAP_MAX_DATA_SIZE}
        -DCONFIG_BT_AUDIO_CODEC_CAP_MAX_METADATA_SIZE=${CONFIG_BT_AUDIO_CODEC_CAP_MAX_METADATA_SIZE}
        -DCONFIG_BT_AUDIO_NOTIFY_RETRY_DELAY=${CONFIG_BT_AUDIO_NOTIFY_RETRY_DELAY}
        -DCONFIG_BT_ISO_LOG_LEVEL=${CONFIG_BT_ISO_LOG_LEVEL}
        -DCONFIG_BT_ISO_MAX_CHAN=${CONFIG_BT_ISO_MAX_CHAN}
        -DCONFIG_BT_ISO_MAX_CIG=${CONFIG_BT_ISO_MAX_CIG}
        -DCONFIG_BT_ISO_MAX_BIG=${CONFIG_BT_ISO_MAX_BIG}
        -DCONFIG_BT_ISO_TX_BUF_COUNT=${CONFIG_BT_ISO_TX_BUF_COUNT}
        -DCONFIG_BT_ISO_TX_MTU=${CONFIG_BT_ISO_TX_MTU}
        -DCONFIG_BT_ISO_RX_BUF_COUNT=${CONFIG_BT_ISO_RX_BUF_COUNT}
        -DCONFIG_BT_ISO_RX_MTU=${CONFIG_BT_ISO_RX_MTU}
        -DCONFIG_BT_BAP_ISO_LOG_LEVEL=${CONFIG_BT_BAP_ISO_LOG_LEVEL}
        -DCONFIG_BT_BAP_STREAM_LOG_LEVEL=${CONFIG_BT_BAP_STREAM_LOG_LEVEL}
        -DCONFIG_BT_BAP_UNICAST_SERVER_LOG_LEVEL=${CONFIG_BT_BAP_UNICAST_SERVER_LOG_LEVEL}
        -DCONFIG_BT_BAP_UNICAST_CLIENT_LOG_LEVEL=${CONFIG_BT_BAP_UNICAST_CLIENT_LOG_LEVEL}
        -DCONFIG_BT_BAP_UNICAST_CLIENT_GROUP_COUNT=${CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_COUNT}
        -DCONFIG_BT_BAP_UNICAST_CLIENT_GROUP_STREAM_COUNT=${CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_STREAM_COUNT}
        -DCONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK_COUNT=${CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK_COUNT}
        -DCONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT=${CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT}
        -DCONFIG_BT_BAP_BASS_MAX_SUBGROUPS=${CONFIG_BT_BAP_BASS_MAX_SUBGROUPS}
        -DCONFIG_BT_BAP_BROADCAST_SRC_COUNT=${CONFIG_BT_BAP_BROADCAST_SRC_COUNT}
        -DCONFIG_BT_BAP_BROADCAST_SRC_SUBGROUP_COUNT=${CONFIG_BT_BAP_BROADCAST_SRC_SUBGROUP_COUNT}
        -DCONFIG_BT_BAP_BROADCAST_SRC_STREAM_COUNT=${CONFIG_BT_BAP_BROADCAST_SRC_STREAM_COUNT}
        -DCONFIG_BT_BAP_BROADCAST_SNK_COUNT=${CONFIG_BT_BAP_BROADCAST_SNK_COUNT}
        -DCONFIG_BT_BAP_BROADCAST_SNK_SUBGROUP_COUNT=${CONFIG_BT_BAP_BROADCAST_SNK_SUBGROUP_COUNT}
        -DCONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT=${CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT}
        -DCONFIG_BT_BAP_SCAN_DELEGATOR_RECV_STATE_COUNT=${CONFIG_BT_BAP_SCAN_DELEGATOR_RECV_STATE_COUNT}
        -DCONFIG_BT_BAP_BROADCAST_ASSISTANT_RECV_STATE_COUNT=${CONFIG_BT_BAP_BROADCAST_ASSISTANT_RECV_STATE_COUNT}
        -DCONFIG_BT_ASCS_LOG_LEVEL=${CONFIG_BT_ASCS_LOG_LEVEL}
        -DCONFIG_BT_ASCS_MAX_ASE_SNK_COUNT=${CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT}
        -DCONFIG_BT_ASCS_MAX_ASE_SRC_COUNT=${CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT}
        -DCONFIG_BT_ASCS_MAX_ACTIVE_ASES=${CONFIG_BT_ASCS_MAX_ACTIVE_ASES}
        -DCONFIG_BT_ASCS_ISO_DISCONNECT_DELAY=${CONFIG_BT_ASCS_ISO_DISCONNECT_DELAY}
        -DCONFIG_BT_ASCS_ASE_BUF_TIMEOUT=${CONFIG_BT_ASCS_ASE_BUF_TIMEOUT}
        -DCONFIG_BT_BAP_SCAN_DELEGATOR_BUF_TIMEOUT=${CONFIG_BT_BAP_SCAN_DELEGATOR_BUF_TIMEOUT}
        -DCONFIG_BT_PACS_LOG_LEVEL=${CONFIG_BT_PACS_LOG_LEVEL}
        -DCONFIG_BT_PACS_SUPPORTED_CONTEXT_NOTIFIABLE=${CONFIG_BT_PACS_SUPPORTED_CONTEXT_NOTIFIABLE}
        -DCONFIG_BT_PAC_SNK_LOC=${CONFIG_BT_PAC_SNK_LOC}
        -DCONFIG_BT_VCP_VOL_REND_LOG_LEVEL=${CONFIG_BT_VCP_VOL_REND_LOG_LEVEL}
        -DCONFIG_BT_VCP_VOL_REND_VOCS_INSTANCE_COUNT=${CONFIG_BT_VCP_VOL_REND_VOCS_INSTANCE_COUNT}
        -DCONFIG_BT_VCP_VOL_REND_AICS_INSTANCE_COUNT=${CONFIG_BT_VCP_VOL_REND_AICS_INSTANCE_COUNT}
        -DCONFIG_BT_VCP_VOL_CTLR_MAX_VOCS_INST=${CONFIG_BT_VCP_VOL_CTLR_MAX_VOCS_INST}
        -DCONFIG_BT_VCP_VOL_CTLR_MAX_AICS_INST=${CONFIG_BT_VCP_VOL_CTLR_MAX_AICS_INST}
        -DCONFIG_BT_VOCS_MAX_INSTANCE_COUNT=${CONFIG_BT_VOCS_MAX_INSTANCE_COUNT}
        -DCONFIG_BT_VOCS_CLIENT_MAX_INSTANCE_COUNT=${CONFIG_BT_VOCS_CLIENT_MAX_INSTANCE_COUNT}
        -DCONFIG_BT_VOCS_MAX_OUTPUT_DESCRIPTION_SIZE=${CONFIG_BT_VOCS_MAX_OUTPUT_DESCRIPTION_SIZE}
        -DCONFIG_BT_AICS_MAX_INSTANCE_COUNT=${CONFIG_BT_AICS_MAX_INSTANCE_COUNT}
        -DCONFIG_BT_AICS_CLIENT_MAX_INSTANCE_COUNT=${CONFIG_BT_AICS_CLIENT_MAX_INSTANCE_COUNT}
        -DCONFIG_BT_AICS_MAX_INPUT_DESCRIPTION_SIZE=${CONFIG_BT_AICS_MAX_INPUT_DESCRIPTION_SIZE}
        -DCONFIG_BT_MICP_MIC_DEV_AICS_INSTANCE_COUNT=${CONFIG_BT_MICP_MIC_DEV_AICS_INSTANCE_COUNT}
        -DCONFIG_BT_MICP_MIC_CTLR_MAX_AICS_INST=${CONFIG_BT_MICP_MIC_CTLR_MAX_AICS_INST}
        -DCONFIG_BT_CAP_ACCEPTOR_LOG_LEVEL=${CONFIG_BT_CAP_ACCEPTOR_LOG_LEVEL}
        -DCONFIG_BT_CAP_INITIATOR_LOG_LEVEL=${CONFIG_BT_CAP_INITIATOR_LOG_LEVEL}
        -DCONFIG_BT_CSIP_SET_MEMBER_LOG_LEVEL=${CONFIG_BT_CSIP_SET_MEMBER_LOG_LEVEL}
        -DCONFIG_BT_CSIP_SET_MEMBER_CRYPTO_LOG_LEVEL=${CONFIG_BT_CSIP_SET_MEMBER_CRYPTO_LOG_LEVEL}
        -DCONFIG_BT_CSIP_SET_MEMBER_MAX_INSTANCE_COUNT=${CONFIG_BT_CSIP_SET_MEMBER_MAX_INSTANCE_COUNT}
        -DCONFIG_BT_CSIP_SET_MEMBER_ENC_SIRK_SUPPORT=${CONFIG_BT_CSIP_SET_MEMBER_ENC_SIRK_SUPPORT}
        -DCONFIG_BT_CSIP_SET_COORDINATOR_MAX_CSIS_INSTANCES=${CONFIG_BT_CSIP_SET_COORDINATOR_MAX_CSIS_INSTANCES}
        -DCONFIG_BT_CSIP_SET_COORDINATOR_ENC_SIRK_SUPPORT=${CONFIG_BT_CSIP_SET_COORDINATOR_ENC_SIRK_SUPPORT}
        -DCONFIG_BT_TBS_BEARER_COUNT=${CONFIG_BT_TBS_BEARER_COUNT}
        -DCONFIG_BT_TBS_MAX_CALLS=${CONFIG_BT_TBS_MAX_CALLS}
        -DCONFIG_BT_TBS_MAX_URI_LENGTH=${CONFIG_BT_TBS_MAX_URI_LENGTH}
        -DCONFIG_BT_TBS_MAX_PROVIDER_NAME_LENGTH=${CONFIG_BT_TBS_MAX_PROVIDER_NAME_LENGTH}
        -DCONFIG_BT_TBS_MAX_FRIENDLY_NAME_LENGTH=${CONFIG_BT_TBS_MAX_FRIENDLY_NAME_LENGTH}
        -DCONFIG_BT_TBS_MAX_SCHEME_LIST_LENGTH=${CONFIG_BT_TBS_MAX_SCHEME_LIST_LENGTH}
        -DCONFIG_BT_TBS_LOCK_TIMEOUT=${CONFIG_BT_TBS_LOCK_TIMEOUT}
        -DCONFIG_BT_TBS_CLIENT_MAX_CALLS=${CONFIG_BT_TBS_CLIENT_MAX_CALLS}
        -DCONFIG_BT_TBS_CLIENT_MAX_TBS_INSTANCES=${CONFIG_BT_TBS_CLIENT_MAX_TBS_INSTANCES}
        -DCONFIG_BT_CCP_CALL_CONTROL_CLIENT_BEARER_COUNT=${CONFIG_BT_CCP_CALL_CONTROL_CLIENT_BEARER_COUNT}
        -DCONFIG_BT_CCP_CALL_CONTROL_SERVER_BEARER_COUNT=${CONFIG_BT_CCP_CALL_CONTROL_SERVER_BEARER_COUNT}
        -DCONFIG_BT_CCP_CALL_CONTROL_SERVER_PROVIDER_NAME_MAX_LENGTH=${CONFIG_BT_CCP_CALL_CONTROL_SERVER_PROVIDER_NAME_MAX_LENGTH}
        -DCONFIG_BT_GATT_DYNAMIC_DB=${CONFIG_BT_GATT_DYNAMIC_DB}
        -DCONFIG_BT_MCS_LOG_LEVEL=${CONFIG_BT_MCS_LOG_LEVEL}
        -DCONFIG_BT_MCC_LOG_LEVEL=${CONFIG_BT_MCC_LOG_LEVEL}
        -DCONFIG_BT_MCC_MEDIA_PLAYER_NAME_MAX=${CONFIG_BT_MCC_MEDIA_PLAYER_NAME_MAX}
        -DCONFIG_BT_MCC_ICON_URL_MAX=${CONFIG_BT_MCC_ICON_URL_MAX}
        -DCONFIG_BT_MCC_TRACK_TITLE_MAX=${CONFIG_BT_MCC_TRACK_TITLE_MAX}
        -DCONFIG_BT_MCC_SEGMENT_NAME_MAX=${CONFIG_BT_MCC_SEGMENT_NAME_MAX}
        -DCONFIG_BT_MCC_OTC_OBJ_BUF_SIZE=${CONFIG_BT_MCC_OTC_OBJ_BUF_SIZE}
        -DCONFIG_BT_MCC_TOTAL_OBJ_CONTENT_MEM=${CONFIG_BT_MCC_TOTAL_OBJ_CONTENT_MEM}
        -DCONFIG_BT_MCC_TRACK_SEGS_MAX_CNT=${CONFIG_BT_MCC_TRACK_SEGS_MAX_CNT}
        -DCONFIG_BT_MCC_GROUP_RECORDS_MAX=${CONFIG_BT_MCC_GROUP_RECORDS_MAX}
        -DCONFIG_BT_MPL_LOG_LEVEL=${CONFIG_BT_MPL_LOG_LEVEL}
        -DCONFIG_BT_MPL_MEDIA_PLAYER_NAME=\"${CONFIG_BT_MPL_MEDIA_PLAYER_NAME}\"
        -DCONFIG_BT_MPL_MEDIA_PLAYER_NAME_MAX=${CONFIG_BT_MPL_MEDIA_PLAYER_NAME_MAX}
        -DCONFIG_BT_MPL_ICON_URL=\"${CONFIG_BT_MPL_ICON_URL}\"
        -DCONFIG_BT_MPL_ICON_URL_MAX=${CONFIG_BT_MPL_ICON_URL_MAX}
        -DCONFIG_BT_MPL_TRACK_TITLE_MAX=${CONFIG_BT_MPL_TRACK_TITLE_MAX}
        -DCONFIG_BT_MPL_SEGMENT_NAME_MAX=${CONFIG_BT_MPL_SEGMENT_NAME_MAX}
        -DCONFIG_BT_MPL_GROUP_TITLE_MAX=${CONFIG_BT_MPL_GROUP_TITLE_MAX}
        -DCONFIG_BT_MPL_MAX_OBJ_SIZE=${CONFIG_BT_MPL_MAX_OBJ_SIZE}
        -DCONFIG_BT_MPL_ICON_BITMAP_SIZE=${CONFIG_BT_MPL_ICON_BITMAP_SIZE}
        -DCONFIG_BT_MPL_TRACK_MAX_SIZE=${CONFIG_BT_MPL_TRACK_MAX_SIZE}
        -DCONFIG_MCTL_LOG_LEVEL=${CONFIG_MCTL_LOG_LEVEL}
    )
    if(CONFIG_BT_AUDIO_RX)
        __component_add_compile_definitions(-DCONFIG_BT_AUDIO_RX)
    endif()
    if(CONFIG_BT_AUDIO_TX)
        __component_add_compile_definitions(-DCONFIG_BT_AUDIO_TX)
    endif()
    if(CONFIG_BT_ISO)
        __component_add_compile_definitions(-DCONFIG_BT_ISO)
    endif()
    if(CONFIG_BT_ISO_CENTRAL)
        __component_add_compile_definitions(-DCONFIG_BT_ISO_CENTRAL)
    endif()
    if(CONFIG_BT_ISO_RX)
        __component_add_compile_definitions(-DCONFIG_BT_ISO_RX)
    endif()
    if(CONFIG_BT_ISO_TX)
        __component_add_compile_definitions(-DCONFIG_BT_ISO_TX)
    endif()
    if(CONFIG_BT_ISO_PERIPHERAL)
        __component_add_compile_definitions(-DCONFIG_BT_ISO_PERIPHERAL)
    endif()
    if(CONFIG_BT_ISO_UNICAST)
        __component_add_compile_definitions(-DCONFIG_BT_ISO_UNICAST)
    endif()
    if(CONFIG_BT_BAP_STREAM)
        __component_add_compile_definitions(-DCONFIG_BT_BAP_STREAM)
    endif()
    if(CONFIG_BT_BAP_UNICAST_SERVER)
        __component_add_compile_definitions(-DCONFIG_BT_BAP_UNICAST -DCONFIG_BT_BAP_UNICAST_SERVER)
    endif()
    if(CONFIG_BT_BAP_UNICAST_CLIENT)
        __component_add_compile_definitions(-DCONFIG_BT_BAP_UNICAST -DCONFIG_BT_BAP_UNICAST_CLIENT)
    endif()
    if(CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK_COUNT GREATER 0)
        __component_add_compile_definitions(-DCONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK)
    endif()
    if(CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT GREATER 0)
        __component_add_compile_definitions(-DCONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC)
    endif()
    if(CONFIG_BT_ASCS)
        __component_add_compile_definitions(-DCONFIG_BT_ASCS)
    endif()
    if(CONFIG_BT_ASCS_ASE_SNK)
        __component_add_compile_definitions(-DCONFIG_BT_ASCS_ASE_SNK)
    endif()
    if(CONFIG_BT_ASCS_ASE_SRC)
        __component_add_compile_definitions(-DCONFIG_BT_ASCS_ASE_SRC)
    endif()
    if(CONFIG_BT_PACS)
        __component_add_compile_definitions(-DCONFIG_BT_PACS)
    endif()
    if(CONFIG_BT_PAC_SNK)
        __component_add_compile_definitions(-DCONFIG_BT_PAC_SNK)
    endif()
    if(CONFIG_BT_PAC_SRC)
        __component_add_compile_definitions(-DCONFIG_BT_PAC_SRC)
    endif()
    if(CONFIG_BT_VCP_VOL_REND)
        __component_add_compile_definitions(-DCONFIG_BT_VCP_VOL_REND)
    endif()
    if(CONFIG_BT_CAP_ACCEPTOR)
        __component_add_compile_definitions(-DCONFIG_BT_CAP -DCONFIG_BT_CAP_ACCEPTOR)
    endif()
    if(CONFIG_BT_CAP_INITIATOR)
        __component_add_compile_definitions(-DCONFIG_BT_CAP -DCONFIG_BT_CAP_INITIATOR -DCONFIG_BT_CAP_INITIATOR_UNICAST)
    endif()
    if(CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER)
        __component_add_compile_definitions(-DCONFIG_BT_CAP_ACCEPTOR_SET_MEMBER)
    endif()
    if(CONFIG_BT_CSIP_SET_MEMBER)
        __component_add_compile_definitions(-DCONFIG_BT_CSIP_SET_MEMBER)
    endif()
endif()

if(CONFIG_BT_OTS OR CONFIG_BT_OTS_CLIENT)
    __component_add_compile_definitions(
        -DCONFIG_BT_OTS_LOG_LEVEL=${CONFIG_BT_OTS_LOG_LEVEL}
        -DCONFIG_BT_OTS_CLIENT_LOG_LEVEL=${CONFIG_BT_OTS_CLIENT_LOG_LEVEL}
        -DCONFIG_BT_OTS_MAX_INST_CNT=${CONFIG_BT_OTS_MAX_INST_CNT}
        -DCONFIG_BT_OTS_MAX_OBJ_CNT=${CONFIG_BT_OTS_MAX_OBJ_CNT}
        -DCONFIG_BT_OTS_L2CAP_CHAN_TX_MTU=${CONFIG_BT_OTS_L2CAP_CHAN_TX_MTU}
        -DCONFIG_BT_OTS_L2CAP_CHAN_RX_MTU=${CONFIG_BT_OTS_L2CAP_CHAN_RX_MTU}
        -DCONFIG_BT_OTS_OBJ_MAX_NAME_LEN=${CONFIG_BT_OTS_OBJ_MAX_NAME_LEN}
    )
endif()

if(CONFIG_BT_SETTINGS)
    __component_add_compile_definitions(
        -DCONFIG_SETTINGS=1
        -DCONFIG_BT_SETTINGS_LOG_LEVEL=${CONFIG_BT_SETTINGS_LOG_LEVEL}
        -DCONFIG_BT_SETTINGS_CCC_STORE_MAX=${CONFIG_BT_SETTINGS_CCC_STORE_MAX}
    )
endif()

if(CONFIG_IPRO_BT_HOST_SMP_TRACE)
    __component_add_compile_definitions(-DCONFIG_IPRO_BT_HOST_SMP_TRACE)
endif()

if(CONFIG_IPRO_BT_HOST_HCI_TRACE)
    __component_add_compile_definitions(-DCONFIG_IPRO_BT_HOST_HCI_TRACE)
endif()

if(CONFIG_BT_DATA_LEN_UPDATE)
    __component_add_compile_definitions(
        -DCONFIG_BT_DATA_LEN_UPDATE=1
        -DCONFIG_BT_USER_DATA_LEN_UPDATE=1
    )
    if(CONFIG_BT_AUTO_DATA_LEN_UPDATE)
        __component_add_compile_definitions(-DCONFIG_BT_AUTO_DATA_LEN_UPDATE=1)
    endif()
endif()

if(CONFIG_IPRO_BT_HOST_SKIP_SET_PRIVACY_MODE)
    __component_add_compile_definitions(-DCONFIG_IPRO_BT_HOST_SKIP_SET_PRIVACY_MODE)
endif()

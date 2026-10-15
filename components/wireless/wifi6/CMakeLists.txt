# Public include directories - headers available for applications
# (subdirectories will add their own includes when building from source)

# Optional: use prebuilt archive instead of sources
if(CONFIG_WIFI6_PREBUILT)
    set(CURRENT_STATIC_LIBRARY wifi6)
    add_library(${CURRENT_STATIC_LIBRARY} STATIC IMPORTED GLOBAL)
    set_target_properties(${CURRENT_STATIC_LIBRARY} PROPERTIES IMPORTED_LOCATION ${CONFIG_WIFI6_PREBUILT_PATH})
    set_property(GLOBAL APPEND PROPERTY SDK_LIBS ${CURRENT_STATIC_LIBRARY})
    target_link_libraries(${CURRENT_STATIC_LIBRARY} PUBLIC __component_lib)
    message(STATUS "Wi-Fi 6: using prebuilt archive at ${CONFIG_WIFI6_PREBUILT_PATH}")
    return()
endif()

add_subdirectory(ipro_fhost)
# LP beacon-RX firmware (CONFIG_PHY_LPFW): wpa_supplicant does not compile
# against the trimmed UMAC-less headers and its archive is never linked by the
# lp_fw image — skip it so `make sdk DEFCONFIG=defconfig_ipro6_lpfw` completes.
if(NOT CONFIG_PHY_LPFW)
add_subdirectory(ipro_wpa_supplicant)
endif()
# LP beacon-RX firmware (CONFIG_PHY_LPFW): the lwIP adapter's lwipopts.h ->
# fhost.h include chain needs the full UMAC task enum, and the lp_fw link pulls
# no member from its archive (verified via the blob map) — skip it.
# Under Zephyr (IPRO_SDK_UNDER_ZEPHYR) the IP stack is Zephyr's own: the lwIP
# adapter and sigma_dut stay out, the RTOS abstraction comes from the Zephyr
# module (zephyr/hal_glue/ipro_wifi_rtos_al.c).
if(NOT CONFIG_PHY_LPFW AND NOT IPRO_SDK_UNDER_ZEPHYR)
add_subdirectory(wifi6_lwip_adapter)
endif()
if(NOT IPRO_SDK_UNDER_ZEPHYR)
add_subdirectory(ipro_os_adapter)

add_subdirectory(sigma_dut)
endif()

# Enable common section for wifi6
# __component_add_compile_options(-fcommon)

# disable undef warning
# __component_add_compile_options(-Wno-undef)

__component_add_compile_definitions(-DCFG_RTOS)
# LP beacon-RX firmware (CONFIG_PHY_LPFW) omits UMAC + FHOST entirely: the
# upper-MAC and fully-hosted paths are dead weight on a beacon-RX-only image and
# pull in the bulk of the stack. Normal builds keep them. This is one half of the
# structural WiFi trim (the other half is CFG_VIF_MAX/CFG_STA_MAX=0 below);
# CONFIG_PHY_LPFW is set only by the lp_fw build, so the demo path is unaffected.
if(NOT CONFIG_PHY_LPFW)
__component_add_compile_definitions(-DCFG_FHOST)
__component_add_compile_definitions(-DCFG_UMAC) # (NX_AMPDU_RX)CFG_BARX, (NX_AMPDU_RX_BUF_SIZE)CFG_REORD_BUF
endif()
# Phase 2A.53 NULL: CFG_STATS off regresses 0x24B40408 from 0x008 to 0x028.
# Counter-intuitive: legacy proj_config.mk has it commented but port needs it ON.
__component_add_compile_definitions(-DCFG_STATS)
__component_add_compile_definitions(-DCFG_WIFI_DBG=2)
# LP beacon-RX firmware (CONFIG_PHY_LPFW) config-align to the reference lp_fw
# (genlp REC=off, no COEX): the wake path never runs the recovery state machine
# nor Wi-Fi/BT coexistence arbitration, so NX_RECOVERY / RW_WLAN_COEX_EN collapse
# and coexist.c drops from the beacon-RX closure. Normal builds keep both.
if(NOT CONFIG_PHY_LPFW)
__component_add_compile_definitions(-DCFG_REC)
__component_add_compile_definitions(-DCFG_COEX)
endif()
__component_add_compile_definitions(-DCFG_BCN) # NX_BEACONING
__component_add_compile_definitions(-DCFG_AGG) # NX_MAX_BA_TX(CFG_BATX)
# LP beacon-RX firmware collapses the VIF/STA tables to zero (verified trim from
# the standalone lp_fw): no association/station state is retained in the image.
# Normal builds use 2/4. Second half of the structural trim (see UMAC/FHOST above).
if(CONFIG_PHY_LPFW)
__component_add_compile_definitions(-DCFG_VIF_MAX=0)
__component_add_compile_definitions(-DCFG_STA_MAX=0)
else()
__component_add_compile_definitions(-DCFG_VIF_MAX=2)
__component_add_compile_definitions(-DCFG_STA_MAX=4)
endif()
__component_add_compile_definitions(-DCFG_SPC=16)
if(CONFIG_WIFI_ETH_BRIDGE)
__component_add_compile_definitions(-DCFG_AMSDU_8K)
__component_add_compile_definitions(-DCFG_TXDESC0=12)
__component_add_compile_definitions(-DCFG_TXDESC1=64)
__component_add_compile_definitions(-DCFG_TXDESC2=12)
__component_add_compile_definitions(-DCFG_TXDESC3=12)
__component_add_compile_definitions(-DCFG_TXDESC4=2)
elseif(CONFIG_PHY_LPFW)
# LP beacon-RX firmware: AMSDU off + minimal TX descriptors, matching the
# reference lp_fw (genlp AMSDU=off, TXDESC1=8). The lp_fw does not transmit on
# the wake path, so the static txl_buffer_pool (was ~54K with AMSDU + TXDESC1=32)
# collapses. Normal builds keep AMSDU + TXDESC1=32.
__component_add_compile_definitions(-DCFG_AMSDU_4K)
__component_add_compile_definitions(-DCFG_TXDESC0=4)
__component_add_compile_definitions(-DCFG_TXDESC1=8)
__component_add_compile_definitions(-DCFG_TXDESC2=1)
__component_add_compile_definitions(-DCFG_TXDESC3=4)
__component_add_compile_definitions(-DCFG_TXDESC4=4)
else()
__component_add_compile_definitions(-DCFG_AMSDU_4K)
__component_add_compile_definitions(-DCFG_AMSDU)  # Phase 10G/H
__component_add_compile_definitions(-DCFG_TXDESC0=1)
__component_add_compile_definitions(-DCFG_TXDESC1=32)
__component_add_compile_definitions(-DCFG_TXDESC2=1)
__component_add_compile_definitions(-DCFG_TXDESC3=1)
#now txdesc4 isn't used beacause beacon change to VI
__component_add_compile_definitions(-DCFG_TXDESC4=4)
endif()
__component_add_compile_definitions(-DCFG_MAC_VER_V21)
__component_add_compile_definitions(-DCFG_MDM_VER_V30)
__component_add_compile_definitions(-DCFG_IPC_VER_V11)
__component_add_compile_definitions(-DCFG_PLF_VER_V30)
# LP beacon-RX firmware (CONFIG_PHY_LPFW) config-align to the reference lp_fw
# (genlp comments out #CFG_PS): the wake path never runs the legacy power-save
# state machine, so NX_POWERSAVE collapses and ps.c drops from the beacon-RX
# closure. Normal builds keep CFG_PS. (CFG_MU_CNT=1 already means RW_USER_MAX=1,
# i.e. MU-MIMO TX off, matching genlp MU=off in both builds.)
if(NOT CONFIG_PHY_LPFW)
__component_add_compile_definitions(-DCFG_PS)
endif()
__component_add_compile_definitions(-DCFG_MU_CNT=1)
#__component_add_compile_definitions(-DCFG_UAPSD)
__component_add_compile_definitions(-DCFG_KEYCFG)
__component_add_compile_definitions(-DCFG_P2P=0)
# LP beacon-RX firmware: CFG_P2P=0 already forces NX_P2P/NX_P2P_GO=0, so P2P is
# structurally absent; drop the now-inert CFG_P2P_GO define to match the
# reference lp_fw (genlp P2P=0). Normal builds keep it.
if(NOT CONFIG_PHY_LPFW)
__component_add_compile_definitions(-DCFG_P2P_GO)
endif()
__component_add_compile_definitions(-DCFG_BWLEN)
# LP beacon-RX firmware omits HE/802.11ax (CONFIG_PHY_LPFW): beacons are received
# at legacy rates, and HE pulls in a large PHY + txl_he + HE-MAC surface. Matches
# the reference lp_fw (built HE-off). Normal builds keep HE.
if(NOT CONFIG_PHY_LPFW)
__component_add_compile_definitions(-DCFG_HE)
endif()
# Enable TWT for sigma-dut HE certification
if(CONFIG_SIGMA_DUT)
__component_add_compile_definitions(-DCFG_TWT=8)
__component_add_compile_definitions(-DCONFIG_TWT)
endif()
# __component_add_compile_definitions(-DCFG_TWT=8)
# Standalone station memory profile (CONFIG_WIFI_MEM_PROFILE_*): reorder
# window, MAC RX buffer 1, RX buffers, TCP window, send buffer, lwIP heap.
if(CONFIG_WIFI_MEM_PROFILE_APP)
    set(STA_MEM_PROFILE 8 12288 10 8760 11680 24576)
elseif(CONFIG_WIFI_MEM_PROFILE_BALANCED)
    set(STA_MEM_PROFILE 8 12288 12 11680 23360 32768)
else()
    set(STA_MEM_PROFILE 16 20480 20 35040 49640 71680)
endif()
list(GET STA_MEM_PROFILE 0 STA_REORD_BUF)
list(GET STA_MEM_PROFILE 1 STA_RXL_BUFFER1_BYTES)
list(GET STA_MEM_PROFILE 2 STA_RX_BUF_CNT)
list(GET STA_MEM_PROFILE 3 STA_TCP_WND)
list(GET STA_MEM_PROFILE 4 STA_TCP_SND_BUF)
list(GET STA_MEM_PROFILE 5 STA_LWIP_HEAP_SIZE)

if(CONFIG_WIFI_ETH_BRIDGE)
__component_add_compile_definitions(-DCFG_BARX=12)
__component_add_compile_definitions(-DCFG_BATX=12)
__component_add_compile_definitions(-DCFG_REORD_BUF=12)
else()
__component_add_compile_definitions(-DCFG_BARX=2)
__component_add_compile_definitions(-DCFG_BATX=1)
if(CONFIG_LPM)
__component_add_compile_definitions(-DCFG_REORD_BUF=8)
__component_add_compile_definitions(-DCFG_LPM)
# CFG_HE_TB (HE trigger-based) follows CFG_HE — omitted in the LP beacon-RX build.
if(NOT CONFIG_PHY_LPFW)
__component_add_compile_definitions(-DCFG_HE_TB)
endif()
else()
# Zephyr's IP stack is its own (the lwIP knobs below do not apply); the
# reorder window, RX buffer 1 and the RX buffer count come from its Kconfig
# (CONFIG_IPRO_WIFI_REORD_BUF here, RXL_BUF1_BYTES and FHOST_RX_BUF_CNT in
# zephyr/CMakeLists.txt).
if(IPRO_SDK_UNDER_ZEPHYR)
__component_add_compile_definitions(-DCFG_REORD_BUF=${CONFIG_IPRO_WIFI_REORD_BUF})
elseif(CONFIG_COMPONENTS_SDIOWIFI_ENABLE OR CONFIG_COMPONENTS_USBWIFI_ENABLE)
__component_add_compile_definitions(-DCFG_REORD_BUF=12)
if(CONFIG_COMPONENTS_USBWIFI_ENABLE)
# USB bridge: 28 BE frames in flight, below the 32-MPDU TX BA window
__component_add_compile_definitions(-DFHOST_TX_HWQ_BE_MAX=28)
endif()
else()
# FreeRTOS standalone station: buffers from STA_MEM_PROFILE,
# 40 ms lwIP fast TCP timer.
__component_add_compile_definitions(-DCFG_REORD_BUF=${STA_REORD_BUF})
__component_add_compile_definitions(-DRXL_BUFFER1_BYTES=${STA_RXL_BUFFER1_BYTES})
__component_add_compile_definitions(-DFHOST_RX_BUF_CNT=${STA_RX_BUF_CNT})
__component_add_compile_definitions(-DTCP_WND=${STA_TCP_WND})
# Task stacks in words.
__component_add_compile_definitions(-DFHOST_TCPIP_STACK_WORDS=512)
__component_add_compile_definitions(-DFHOST_RX_STACK_WORDS=576)
__component_add_compile_definitions(-DFHOST_WIFI_STACK_WORDS=384)
# Send queue of 80 pbufs, low-water mark 68.
__component_add_compile_definitions(-DTCP_SND_QUEUELEN=80)
__component_add_compile_definitions(-DMEMP_NUM_TCP_SEG=80)
__component_add_compile_definitions(-DTCP_SNDQUEUELOWAT=68)
__component_add_compile_definitions(-DDNS_TABLE_SIZE=2)
__component_add_compile_definitions(-DTCP_SND_BUF=${STA_TCP_SND_BUF})
# TX headroom 460: fhost_tx_desc_tag 376 (txl_buffer_tag 200) + 3 align
# + NET_AL_TX_DESC_SHIFT 80. The bridges keep 500 (host arm offset).
__component_add_compile_definitions(-DFHOST_TX_BUF_HDR_RESERVE=200)
__component_add_compile_definitions(-DNET_AL_TX_HEADROOM=460)
__component_add_compile_definitions(-DPBUF_LINK_ENCAPSULATION_HLEN=460)
# TX confirm and push events above RX upload; the bridges keep the stock order
__component_add_compile_definitions(-DMACIF_TX_PUSH_FIRST=1)
__component_add_compile_definitions(-DLWIP_TCP_TIMER_WAKE_FIX=1)
__component_add_compile_definitions(-DLWIP_TCP_FAST_INTERVAL_MS=40)
endif()
endif()
endif()
#__component_add_compile_definitions(-DCFG_DBGDUMPKEY)
#__component_add_compile_definitions(-DCFG_TRACE)
__component_add_compile_definitions(-DCFG_UF)
__component_add_compile_definitions(-DCFG_MFP)
# Phase 2A.50 best: disable port-only CFG_FHOST_MONITOR + CFG_FTM_INIT + CFG_FTM_RSP
# to match legacy. Phase 2A.52 bisect: each individually breaks 0x24B40408 from
# legacy match (0x008): FTM alone → 0x608, FHOST_MONITOR alone → 0x028.
# Both off → 0x008 (legacy match). All 3 disabled is the correct fix.
# __component_add_compile_definitions(-DCFG_FTM_INIT)
# __component_add_compile_definitions(-DCFG_FTM_RSP)
__component_add_compile_definitions(-DCFG_HSU=0)
__component_add_compile_definitions(-DCFG_CRYPTO)
# __component_add_compile_definitions(-DCFG_FHOST_MONITOR)
# Phase 2A.52 NULL: CONFIG_NO_STDOUT_DEBUG regresses 0x24B40408 from 0x008 to 0x028.
# # __component_add_compile_definitions(-DCONFIG_NO_STDOUT_DEBUG)
# Sigma-DUT certification needs TG (traffic generator) which adds ~6KB
# to ram_wifi. Shrink lwIP heap from 64KB to 56KB to make room.
if(CONFIG_SIGMA_DUT AND CONFIG_WIFI_TG)
__component_add_compile_definitions(-DLWIP_HEAP_SIZE=57344)
elseif(CONFIG_COMPONENTS_SDIOWIFI_ENABLE OR CONFIG_COMPONENTS_USBWIFI_ENABLE)
# Bridge case: DUT lwIP only runs DHCP client + ARP under DHCP_IN_EMB
# (data traffic is bridged to the host via SDIO/USB and terminated
# there); the heap doesn't participate in throughput. Trade the extra
# heap back to ram_wifi for the transport TX queue and the WiFi pools.
# The 48 KB standalone heap does NOT fit alongside fhost RX + memp here:
# it starves _heap_wifi and the WiFi RX descriptor path faults during
# association (wild jump in rxu_cntrl_get_da_sa).
#
# 12288 -> 4096 (2026-07-26): measured, not estimated. On a bridge that had
# forwarded 766,193 frames, lwip_stats read back:
#     ip.recv 1127 (0.147% of RX)   tcp.* ALL ZERO   etharp.recv 24413
#     mem: avail=12288 used=60 max=1100 err=0
# i.e. the heap high-water is 1,100 B - ARP duplication and the DHCP client
# are its only users, exactly as the note above predicts. 4096 keeps 3.7x
# headroom over the observed peak and returns 8,192 B to ram_wifi.
# VERIFY AFTER ANY CHANGE HERE: lwip_stats.mem.err must stay 0 and mem.max
# must stay well under avail (mem block at lwip_stats+168; err +4, avail +6,
# used +8, max +10, all u16). A non-zero mem.err means roll this back.
__component_add_compile_definitions(-DLWIP_HEAP_SIZE=4096)
# The bridge never terminates TCP on the DUT (tcp stats all zero above), but
# the shared lwipopts derives the send path from the MAC TX queue depth:
#     TCP_SND_BUF      = 4 * MAC_TXQ_DEPTH * TCP_MSS = 186,880 B
#     TCP_SND_QUEUELEN = 4 * TCP_SND_BUF / TCP_MSS   = 512
#     MEMP_NUM_TCP_SEG = 4 * TCP_SND_BUF / TCP_MSS   = 512  -> 10,243 B ram_wifi
# A 186 KB send buffer cannot be backed by a 4 KB heap, so those 512 segment
# descriptors are unreachable by construction. Pin TCP_SND_BUF at 4 * TCP_MSS
# so QUEUELEN and MEMP_NUM_TCP_SEG both fall to 16 and lwIP's own sanity
# checks (init.c: MEMP_NUM_TCP_SEG >= TCP_SND_QUEUELEN >= 2*SND_BUF/MSS,
# TCP_SND_BUF >= 2*TCP_MSS) all still hold. Returns 9,920 B to ram_wifi.
__component_add_compile_definitions(-DTCP_SND_BUF=5840)
# Device-side DNS is the CLI's ping-by-name and nothing else; the bridged hosts
# resolve their own names. The table is 4 x 272 B of OCRAM to hold four
# concurrent lookups nobody issues.
__component_add_compile_definitions(-DDNS_TABLE_SIZE=1)
# memp pools. Careful with the justification here: "the bridge never enters
# lwIP" is true of the DATAPATH only (tcp stats read all zero after 766k
# bridged frames). The CONTROL plane does use lwIP -- CONFIG_DHCP_IN_EMB and
# CONFIG_DHCPD are both set and main.c connects with use_dhcp=1 -- and
# low-power operation additionally needs the device itself to do a DHCP lease
# and a simple TCP connect. So these are trimmed to a working minimum, not to
# the bone:
#   TCP_PCB 5 -> 3     an active connection plus TIME_WAIT, which draws from
#                      this SAME pool. At 2, one close-then-reconnect can
#                      exhaust it; the 198 B is not worth an intermittent
#                      reconnect failure.
#   UDP_PCB 8 -> 6     DHCP client (raw UDP) + DHCPD, both enabled.
#   NETCONN/NETBUF     only the socket API path; 8 and 16 are still generous
#                      for the one or two control connections that exist.
#   TCP_PCB_LISTEN     the device listens for nothing; 2 is already slack.
#   RAW_PCB 4 -> 2     ping.
# A memp exhaustion here is SILENT. After changing these, check the per-pool
# lwIP stats (memp_stats_<POOL>: err +4, avail +6, used +8, max +10, all u16)
# rather than trusting that it seemed to work.
__component_add_compile_definitions(-DMEMP_NUM_TCP_PCB=3)
__component_add_compile_definitions(-DMEMP_NUM_TCP_PCB_LISTEN=2)
__component_add_compile_definitions(-DMEMP_NUM_UDP_PCB=6)
__component_add_compile_definitions(-DMEMP_NUM_RAW_PCB=2)
__component_add_compile_definitions(-DMEMP_NUM_NETCONN=8)
__component_add_compile_definitions(-DMEMP_NUM_NETBUF=16)
# fhost RX buffer pool, 26 -> 18: 13,696 B of ram_wifi. The header default is
#     NX_MAX_BA_RX(2) * NX_AMPDU_RX_BUF_SIZE(12) + 2
# i.e. sized for EVERY RX Block-Ack session pinning a full reorder window at the
# same instant. Overridden here rather than in ipro_fhost/include/fhost.h, whose
# #ifndef default must keep working for lpfw's !NX_AMPDU_RX path (CNT=4).
#
# Measured on LAB02 (2026-07-29, 60 s downlink at a 20 M offer, 2 fresh boots),
# with the instruments in fhost_rx.c and rxu_cntrl.c:
#     concurrent RX BA sessions high-water = 1   (reorder-pool refusals = 0)
#     pinned buffers high-water            = 14  (of 26)
# so one session is what an AP actually opens, and 18 = one 12-deep window plus
# a 6 cushion, four above the measured peak.
#
# THE RISK IF THAT EVER CHANGES: with CFG_BARX=2 the theoretical worst case is
# 24 pinned. At 18 a genuine two-session stall would leave MAC RX with no upload
# buffer, forcing drops, which punch more reorder holes, which is how downlink
# TCP collapses rather than degrades. Re-measure g_ba_rx[0] before assuming this
# still holds on a different AP or a multi-TID load. Rollback is this one value.
__component_add_compile_definitions(-DFHOST_RX_BUF_CNT=18)
else()
# Standalone case (e.g. ipro6_demo): lwIP terminates the sockets on
# the DUT, so every in-flight TX pbuf is held in this heap until
# max(TX cfm, TCP ACK) — the heap IS the TX in-flight window. Each
# full-MSS pbuf costs 2,000 B with the 460 B TX headroom; the heap
# (STA_MEM_PROFILE) holds the profile's send buffer plus ACK and
# control pbufs. Must fit .wifibss within ram_wifi alongside the
# fhost RX pool and memp arrays.
__component_add_compile_definitions(-DLWIP_HEAP_SIZE=${STA_LWIP_HEAP_SIZE})
# The same standalone property, seen from the other side: because the heap IS
# the in-flight window, a sender that stops draining holds all of it. Stock lwIP
# backs the RTO off without bound (TCP_MAXRTX is 12 here: opt.h's stock
# default), so on IPRO-LAB10 at -23 dBm one loss burst walked rto 6 s -> 24 s -> 48 s with cwnd
# pinned at one MSS; the heap stayed full, the writer stayed blocked, and the
# run never recovered while ICMP to the same device answered 5/5. 4 ticks = 2 s,
# which removed every backed-off leg in a 10-leg alternating A/B. The bridge
# branches above stay stock: they do not terminate TCP on the DUT. Runtime
# override for A/B without a rebuild: lp rtomax.
# 2026-09-10: 1 tick (500 ms), not 4. The ceiling now also clamps the RTT
# estimator (tcp_in.c), and that is what sets how long a stalled uplink stays
# silent: measured from the air, seven stalls in one 60 s leg, each 1.1-1.9 s of
# silence in both directions ended by the retransmission, with the live rto at
# 3-6 ticks while the link's RTT was ~10 ms. At 4 ticks stage-0 was 0/10; at 1 it
# is 6/10 and 9/10 with the per-run hole count down from a median of 3 to 0 and
# throughput unchanged. 500 ms is 2.3x the worst RTT seen over a day (217 ms) and
# it is a CEILING, not the 1-tick FLOOR that wrecked the IPRO7 host on 08-28.
__component_add_compile_definitions(-DLP_TCP_RTO_MAX_TICKS=1)
# 2026-09-12: a TCP write that tcp_write() refuses with ERR_MEM is deferred with
# NOTHING queued, so no ack can arrive, sent_tcp() never fires, and api_msg.c's
# poll is the only retry path. tcp_poll()'s interval is in COARSE ticks and this
# build's slow tick measures 511 ms, so the stock NETCONN_TCP_POLL_INTERVAL of 2
# retries only about every 1.02 s. Measured on the 250 ms ring over 7 windows in
# 6 legs, dip and clean alike: ~15 ERR_MEM land in one sample, the writer stops,
# and the heap is ALREADY COMPLETELY FREE in that same sample -- yet nothing
# attempts a write for a further 0.75-1.00 s while the CPU sits 10-20x more idle
# than usual. The memory burst is only the trigger; the hole is retry latency,
# and a scored second breaks the 1 Mbps bar once ~860 ms of it is dead. 1 = one
# slow tick ~511 ms, the floor for this approach; going below it needs a retry
# driven by memory release. Runtime override for A/B without a rebuild:
# lp_netconn_poll_interval (.sbss), 0 = use the compile-time constant.
__component_add_compile_definitions(-DLP_NETCONN_POLL_INTERVAL=1)
# KNOWN CONSEQUENCE, deliberately not compensated here. lwIP gives up after a
# fixed COUNT of retransmissions, so a shorter RTO also shortens how long a
# connection survives a dead link. With the estimator's 3-tick base, stock walks
# 3+6+12+24+48+96+192+384x5 = 2301 ticks before TCP_MAXRTX (12) is reached, about
# 19 minutes; at a 1-tick ceiling the same 12 retries are 6 s. So a radio outage
# longer than 6 s now drops a TCP connection that stock rode out. Raising the
# count to restore the budget (120 = 60 s) needs the shared lwIP assertion in
# init.c relaxed, and how long a connection SHOULD outlive its link is a product decision, so it is left at the stock 12 that the
# 6/10 and 9/10 stage-0 runs were measured with. tcp_slowtmr() clamps the table
# index with LWIP_MIN(nrtx, sizeof(tcp_backoff)-1), so a larger count would be
# memory-safe; only the assertion stands in the way.
# SYN is unaffected either way: the ceiling is applied inside the
# state != SYN_SENT branch and in tcp_receive (established only), so
# TCP_SYNMAXRTX keeps its stock timing.
endif()
__component_add_compile_definitions_ifdef(CONFIG_WIFI_IPERF -DCFG_IPERF)
__component_add_compile_definitions_ifdef(CONFIG_WIFI_TG -DCFG_TG)
# Boot default for the fhost HT40 permission flag (fhost_config.c). Permission
# only - the read-only modem capability and the AP still decide the width.
__component_add_compile_definitions_ifdef(CONFIG_WIFI_HT40_ENABLE -DCFG_HT40)
#__component_add_compile_definitions(-DHIGH_PERFORMANCE_PBUF)
__component_add_compile_definitions(-DCFG_SMARTCONFIG)
__component_add_compile_definitions(-DWL_BB_TPC=1)
__component_add_compile_definitions(-DWL_BB_PD=1)

#__component_add_compile_definitions(-DCONFIG_VIF_MAX=${CONFIG_VIF_MAX})
#__component_add_compile_definitions(-DCONFIG_STA_MAX=${CONFIG_STA_MAX})
__component_add_compile_definitions(-DCONFIG_MAC_TXQ_DEPTH=${CONFIG_MAC_TXQ_DEPTH})
__component_add_compile_definitions(-DCONFIG_MAC_RXQ_DEPTH=${CONFIG_MAC_RXQ_DEPTH})
__component_add_compile_definitions(-D__FILENAME__=__FILE__)
__component_add_compile_definitions_ifdef(CONFIG_CSIDMA_CLI -DCFG_CSIDMA_CLI=1)

__component_add_compile_definitions_ifdef(CONFIG_IPV6 -DCFG_IPV6)

if(CONFIG_COEX_WIFI_MODE)
__component_add_compile_definitions(-DCONFIG_COEX_WIFI_MODE=${CONFIG_COEX_WIFI_MODE})
else()
__component_add_compile_definitions(-DCONFIG_COEX_WIFI_MODE=0)
endif()

if(CONFIG_WIFI_ETH_BRIDGE)
__component_add_compile_definitions(-DCONFIG_WIFI_ETH_BRIDGE=${CONFIG_WIFI_ETH_BRIDGE})
# __component_add_compile_definitions(-DCONFIG_FHOST_RX_BUF_SECTION=\".psram_noinit\")
__component_add_compile_definitions(-DCONFIG_FHOST_RX_BUF_SECTION=${CONFIG_FHOST_RX_BUF_SECTION})
endif()

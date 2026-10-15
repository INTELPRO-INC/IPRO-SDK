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

# IPRO6 Flash Configuration
# Flash-based boot configuration for IPRO6 platform
#
# IPRO6 uses:
# - IPRO7's CPU (toolchain, startup code)
# - IPRO6 specific memory layout (no PSRAM)
#
# Flash boot mode: Data sections have LMA in Flash, VMA in RAM.
# Startup code copies data from Flash to RAM before main().

set(LINKER_TEMPLATE "ipro6_common.ld.in")

# Flash boot - data sections have LMA in flash, VMA in RAM
# Startup code copies data from Flash to RAM before main()
set(DATA_LMA_REGION "AT > flash")

# Flash region
set(FLASH_ORIGIN    "0x13000000")
set(FLASH_SIZE      "2M")

# TCM RAM region (main RAM + nocache share same physical OCRAM)
# Non-cacheable access via 0x21xxxxxx alias (offset 0x10000000)
# Physical OCRAM is 320K. With CONFIG_WIFI_LP_APP_SUPPORT the top 40K
# (0x11046000-0x11050000) is reserved for the standalone lp_fw image, which lives
# in OCRAM (retained across PDS31 by the HW default), so ram_tcm shrinks to 280K;
# otherwise it keeps the full 320K. MUST match ipro6_lpfw.ld ORIGIN/LENGTH and
# LPFW_RUN_ADDR.
set(RAM_TCM_ORIGIN  "0x11000000")
if(CONFIG_WIFI_LP_APP_SUPPORT)
    set(RAM_TCM_SIZE    "280K")
else()
    set(RAM_TCM_SIZE    "320K")
endif()

# WiFi RAM region
set(RAM_WIFI_ORIGIN "0x21050000")
set(RAM_WIFI_SIZE   "160K")

# Objects run from TCM under CONFIG_IPRO6S_HOTCODE (flash builds only).
# port.c.o stays in flash: portasm.S reaches it with jal.
set(HOTCODE_OBJECTS
    "*liblwip.a:inet_chksum.c.o"
    "*libipro_fhost.a:fhost_rx.c.o"
    "*liblwip.a:timeouts.c.o"
    "*libfreertos.a:queue.c.o"
    "*liblwip.a:tcp_in.c.o"
    "*net_al.c.o"
    "*libmacsw.a:rxl_cntrl.c.o"
    "*libmacsw.a:rxu_cntrl.c.o"
    "*libipro_fhost.a:fhost_tx.c.o"
    "*liblwip.a:ip4.c.o"
    "*liblwip.a:pbuf.c.o"
    "*liblwip.a:mem.c.o"
    "*liblwip.a:tcp_out.c.o"
    "*liblwip.a:ethernet.c.o"
    "*liblwip.a:sys_arch.c.o"
    "*libipro_os_adapter.a:rtos_al.c.o"
    "*libc_nano.a:libc_a-memcpy.o"
    "*libc_nano.a:libc_a-memcmp.o"
    "*liblwip.a:def.c.o"
    "*liblwip.a:tcpip.c.o"
    "*liblwip.a:ip4_addr.c.o"
    "*liblwip.a:etharp.c.o"
    "*liblwip.a:memp.c.o"
    "*libfreertos.a:list.c.o"
    "*libfreertos.a:tasks.c.o"
    "*libmacsw.a:llc.c.o"
    "*libmacsw.a:co_list.c.o"
    "*libmacsw.a:export_tx.c.o"
    "*libmacsw.a:rtos.c.o"
    "*libmacsw.a:ke_event.c.o"
    "*libmacsw.a:ke_task.c.o"
    "*libmacsw.a:rxl_hwdesc.c.o"
    "*libmacsw.a:intc.c.o"
    "*libmacsw.a:macif_fhost.c.o"
    "*libmacsw.a:txl_cfm.c.o"
    "*libmacsw.a:ipc_emb.c.o"
    "*libmacsw.a:bam.c.o"
    "*libmacsw.a:txu_cntrl.c.o"
    "*libc_nano.a:libc_a-memset.o"
    "*liblwip.a:udp.c.o"
    "*libc_nano.a:libc_a-memmove-stub.o"
    "*libc_nano.a:libc_a-strcmp.o"
    "*libc_nano.a:libc_a-strlen.o"
    "*libc_nano.a:libc_a-strncmp.o"
    "*libc_nano.a:libc_a-memchr.o"
    "*libfreertos.a:port.c.o"
    "*libfreertos.a:portasm.S.o"
    "*libfreertos.a:timers.c.o"
    "*heap_5.c.o"
    "*libmacsw.a:txl_cntrl.c.o"
    "*libmacsw.a:txl_agg.c.o"
    "*libmacsw.a:rc.c.o"
    "*libmacsw.a:tx_swdesc.c.o"
    "*libmacsw.a:td.c.o"
    "*libmacsw.a:mfp.c.o"
    "*libmacsw.a:export_func.c.o"
    "*libmacsw.a:dma.c.o"
    "*libmacsw.a:nx_time.c.o"
)

# CONFIG_WIFI_MEM_PROFILE_BALANCED: the densest HOTCODE_OBJECTS only.
set(HOTCODE_OBJECTS_BALANCED
    "*liblwip.a:inet_chksum.c.o"
    "*liblwip.a:def.c.o"
    "*libc_nano.a:libc_a-memcmp.o"
    "*libc_nano.a:libc_a-memcpy.o"
    "*libmacsw.a:co_list.c.o"
    "*libmacsw.a:llc.c.o"
    "*libmacsw.a:ke_event.c.o"
    "*libc_nano.a:libc_a-memset.o"
    "*liblwip.a:sys_arch.c.o"
    "*libipro_os_adapter.a:rtos_al.c.o"
    "*libmacsw.a:intc.c.o"
    "*libipro_fhost.a:fhost_rx.c.o"
    "*libmacsw.a:rtos.c.o"
    "*liblwip.a:memp.c.o"
    "*liblwip.a:ethernet.c.o"
    "*libfreertos.a:list.c.o"
    "*liblwip.a:ip4.c.o"
    "*liblwip.a:pbuf.c.o"
    "*libmacsw.a:rxl_cntrl.c.o"
    "*liblwip.a:mem.c.o"
    "*liblwip.a:tcpip.c.o"
    "*libmacsw.a:rxl_hwdesc.c.o"
    "*libfreertos.a:queue.c.o"
    "*libmacsw.a:ke_task.c.o"
    "*liblwip.a:tcp_out.c.o"
    "*libwifi6_lwip_adapter.a:net_al.c.o"
    "*libmacsw.a:txu_cntrl.c.o"
    "*libmacsw.a:macif_fhost.c.o"
)
# CONFIG_WIFI_MEM_PROFILE_APP: libc memory functions only.
set(HOTCODE_OBJECTS_APP
    "*libc_nano.a:libc_a-memcpy.o"
    "*libc_nano.a:libc_a-memset.o"
    "*libc_nano.a:libc_a-memcmp.o"
)
if(CONFIG_WIFI_MEM_PROFILE_BALANCED)
    set(HOTCODE_OBJECTS ${HOTCODE_OBJECTS_BALANCED})
elseif(CONFIG_WIFI_MEM_PROFILE_APP)
    set(HOTCODE_OBJECTS ${HOTCODE_OBJECTS_APP})
endif()

# Sections of HOTCODE_OBJECTS that stay in flash, at the end of .text.
set(HOTCODE_COLD_SECTIONS
    "*libfreertos.a:portasm.S.o(.text.trap)"
    "*libipro_fhost.a:fhost_rx.c.o(.text.fhost_rx_set_mgmt_cb .text.fhost_rx_set_monitor_cb .text.fhost_rx_free_bufs)"
    "*liblwip.a:timeouts.c.o(.text.sys_timer_callback_for_add_timer .text.tcp_timer_calculate_next_wake .text.tcp_timer_lp_control)"
    "*liblwip.a:tcp_in.c.o(.text.tcp_get_next_optbyte .text.tcp_trigger_input_pcb_close)"
    "*libwifi6_lwip_adapter.a:net_al.c.o(.text.net_al_tcp_memkick_cb .text.net_l2_send_cfm .text.net_if_add .text.net_if_get_mac_addr .text.net_if_find_from_name .text.net_if_get_name .text.net_if_up_cb .text.net_al_tx_do_sta_del .text.net_al_tx_init .text.net_al_rx_resend .text.net_init .text.net_l2_send_do .text.net_l2_send .text.net_l2_socket_create .text.lock_net_tcpip_core .text.unlock_net_tcpip_core)"
    "*libmacsw.a:rxl_cntrl.c.o(.text.rxl_process_htc_he_unsupported .text.rxl_process_htc_he_om .text.rxl_process_htc_he_uph .text.rxl_init)"
    "*libmacsw.a:rxu_cntrl.c.o(.text.rxu_cntrl_spurious_check .text.rxu_mgt_frame_check .text.rxu_cntrl_reord_flush .text.rxu_cntrl_init .text.rxu_cntrl_reord_delete .text.rxu_cntrl_monitor_pm)"
    "*libipro_fhost.a:fhost_tx.c.o(.text.fhost_tx_amsdu_timer_cb .text.fhost_tx_ps_cleanup_timer_stop .text.fhost_tx_amsdu_commit_locked.isra.0 .text.fhost_tx_ps_cleanup_cb .text.fhost_tx_ac_min_set .text.fhost_tx_ac_min_get .text.fhost_tx_trace_print .text.fhost_tx_trace_reset .text.fhost_tx_txq_probe .text.fhost_tx_hwq_credits_get .text.fhost_tx_hwq_credits_max .text.fhost_tx_hwq_credits_set .text.fhost_tx_credits_update .text.fhost_tx_sta_add .text.fhost_tx_sta_del .text.fhost_tx_vif_txq_enable .text.fhost_tx_vif_txq_disable .text.fhost_tx_sta_ps_enable .text.fhost_tx_sta_ps_traffic_req .text.fhost_tx_vif_ps_enable)"
    "*liblwip.a:pbuf.c.o(.text.pbuf_take .text.pbuf_take_at .text.pbuf_clone .text.pbuf_put_at)"
    "*liblwip.a:tcp_out.c.o(.text.tcp_enqueue_flags .text.tcp_send_fin .text.tcp_rexmit_rto_prepare .text.tcp_rexmit .text.tcp_rexmit_fast .text.tcp_rexmit_rto_commit .text.tcp_rexmit_rto .text.tcp_keepalive)"
    "*liblwip.a:sys_arch.c.o(.text.sys_mbox_new .text.sys_mbox_free .text.sys_mbox_post .text.sys_mbox_trypost .text.sys_mbox_valid .text.sys_mbox_set_invalid .text.sys_sem_new .text.sys_arch_sem_wait .text.sys_sem_free .text.sys_sem_valid .text.sys_sem_set_invalid)"
    "*liblwip.a:tcpip.c.o(.text.tcpip_callback .text.tcpip_try_callback .text.tcpip_send_msg_wait_sem .text.tcpip_api_call .text.tcpip_callbackmsg_new .text.tcpip_callbackmsg_trycallback)"
    "*liblwip.a:ip4_addr.c.o(.text.ip4addr_aton .text.ipaddr_addr .text.ip4addr_ntoa_r .text.ip4addr_ntoa)"
    "*liblwip.a:etharp.c.o(.text.etharp_cleanup_netif .text.etharp_find_addr .text.etharp_request_dst .text.etharp_reply_dst .text.etharp_request)"
    "*liblwip.a:memp.c.o(.text.memp_init_pool)"
    "*libmacsw.a:co_list.c.o(.text.co_list_find .text.co_list_cnt .text.co_list_insert .text.co_list_insert_after_fast)"
    "*libmacsw.a:ke_task.c.o(.text.cmp_dest_id)"
    "*libmacsw.a:rxl_hwdesc.c.o(.text.rxl_hwdesc_ps_init)"
    "*libmacsw.a:macif_fhost.c.o(.text.macif_kmsg_push .text.macif_msg_evt .text.macif_kmsg_fwd)"
    "*libmacsw.a:txl_cfm.c.o(.text.txl_cfm_init .text.txl_cfm_deferred_repush_wake)"
    "*libmacsw.a:bam.c.o(.text.bam_init .text.bam_start_activity_timer .text.bam_param_sta_info_tab_reset .text.bam_get_avail_task .text.bam_search_task_wt_highest_inactivity_time .text.bam_get_task_to_process_rx .text.bam_delete_complete .text.bam_send_mm_ba_add_req .text.bam_send_mm_ba_del_req .text.bam_delete_all_ba_agg .text.bam_delba_status_cfm .text.bam_delete_ba_agg .text.bam_baw_init .text.bam_flush_baw)"
    "*libmacsw.a:txu_cntrl.c.o(.text.txu_cntrl_push_prof_reset)"
    "*liblwip.a:udp.c.o(.text.udp_init .text.udp_connect .text.udp_disconnect .text.udp_recv .text.udp_remove .text.udp_new .text.udp_new_ip_type .text.udp_netif_ip_addr_changed)"
)

# HBN RAM region (hibernate retention).
# Must match HBN_RAM_BASE in ipro6.h so the .hbn wakeup code links/executes and
# is addressed consistently with the C code that copies and jumps to it. The
# bootrom jumps here directly on PDS wake (from HBN_RSV1), so the wakeup code
# MUST be linked at this same VMA the silicon/bootrom expects (0x20090000).
set(HBNRAM_ORIGIN   "0x20090000")
set(HBNRAM_SIZE     "4K")

# Stack and heap
set(STACK_SIZE      "2K")
set(HEAP_SIZE       "2K")

# EM size for BLE controller (0K if not using BLE)
set(EM_SIZE         "0K")

# NVDS not used in IPRO6
set(ENABLE_NVDS     OFF)

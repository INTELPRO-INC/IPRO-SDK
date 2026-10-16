# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0
#
# The Wi-Fi per-packet path, relocated into OCRAM (CONFIG_IPRO_WIFI_HOT_CODE;
# docs/zephyr/known_issues.md, "Where the per-frame CPU goes"): it is several
# times larger than the instruction cache and otherwise runs at XIP flash-miss
# speed. Included from zephyr/CMakeLists.txt before the SDK libraries are
# finalised so the same list can lift the -Os pin from these files
# (CONFIG_IPRO_WIFI_HOT_CODE_O2).

  # The per-packet path, relocated into OCRAM (docs/zephyr/known_issues.md,
  # "Where the per-frame CPU goes"): it is several times larger than the
  # instruction cache and otherwise runs at XIP flash-miss speed.
  set(_znet ${ZEPHYR_BASE}/subsys/net)
  set(_zker ${ZEPHYR_BASE}/kernel)
  set(_sdk ${IPRO_SDK_ROOT}/components/wireless)
  set(_glue ${CMAKE_CURRENT_LIST_DIR}/..)
  # Zephyr: socket -> net_context -> net_pkt -> UDP/IPv4 -> Ethernet -> net_if
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES
    ${_znet}/ip/net_pkt.c ${_znet}/ip/udp.c ${_znet}/l2/ethernet/ethernet.c
    ${ZEPHYR_BASE}/lib/net_buf/buf.c)
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/ip/net_context.c
    FILTER "(context_sendto|context_alloc_pkt|context_setup_udp_packet|context_write_data|net_context_sendto|context_finalize|net_context_get_)")
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/ip/net_core.c
    FILTER "net_try_send_data")
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/ip/ipv4.c
    FILTER "net_ipv4_(create|finalize)")
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/ip/utils.c
    FILTER "chksum")
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/ip/net_if.c
    FILTER "net_if_(try_send_data|tx|ipv4_select_src_addr|ipv4_get_ll|ipv4_addr_lookup)")
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/lib/sockets/sockets.c ${_znet}/lib/sockets/sockets_inet.c
    FILTER "(zsock_sendto_ctx|send_check_and_wait|z_impl_zsock_sendto|zsock_sendto)")
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/l2/ethernet/arp.c
    FILTER "(net_arp_prepare|arp_entry_find)")
  # kernel primitives on the path
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES
    ${_zker}/sem.c ${_zker}/mutex.c ${_zker}/mem_slab.c ${_zker}/kheap.c ${_zker}/sched.c
    ${ZEPHYR_BASE}/lib/heap/heap.c
    ${ZEPHYR_BASE}/../modules/lib/picolibc/newlib/libc/machine/riscv/memset.S
    ${ZEPHYR_BASE}/arch/riscv/core/switch.S)
  # The trap path: with the ECLIC every interrupt and every context switch
  # (RISCV_ALWAYS_SWITCH_THROUGH_ECALL) goes through _isr_wrapper and the SoC
  # hooks, ~7 traps per received MPDU. _isr_wrapper is only relocatable once
  # the SDK's Zephyr patch (zephyr/patches.yml) puts it in .text; the hooks it
  # calls with `jal` must then move with it (and must stay in flash while it
  # does not: RAM and XIP are further apart than a jal reaches). mtvec points
  # into RAM from reset, so a fault before soc_prep_hook() has copied the code
  # hangs instead of printing.
  file(STRINGS ${ZEPHYR_BASE}/arch/riscv/core/isr.S _isr_in_text REGEX "CONFIG_RISCV_ISR_WRAPPER_IN_TEXT")
  # (un)patching isr.S must re-run this check, or a stale build relocates the
  # wrapper without its hooks
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${ZEPHYR_BASE}/arch/riscv/core/isr.S)
  if(_isr_in_text AND CONFIG_RISCV_ISR_WRAPPER_IN_TEXT)
    zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${ZEPHYR_BASE}/arch/riscv/core/isr.S
      ${_glue}/drivers/interrupt_controller/intc_ipro_eclic.S ${_glue}/soc/intelpro/ipro6s/soc_irq.S)
  else()
    message(STATUS "ipro_wifi_hot: Zephyr without the SDK patches (west patch apply): trap path stays in flash")
  endif()
  # What every context switch runs besides the switch itself.
  if(CONFIG_TIMESLICING)
    zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_zker}/timeslicing.c
      FILTER "^[.]text[.](z_reset_time_slice|z_time_slice|slice_timeout|z_time_slice_size)([.]|$)")
  endif()
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_zker}/usage.c
    FILTER "^[.]text[.](z_sched_usage_start|z_sched_usage_stop|usage_now|sched_cpu_update_usage)([.]|$)")
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_glue}/drivers/timer/ipro_systimer.c
    FILTER "^[.]text[.](sys_clock_set_timeout|set_mtimecmp|mtime|timer_isr)([.]|$)")
  # memcpy/memset are used before z_data_copy(); soc_prep_hook() installs the
  # relocated code first (ipro6s/soc.c).
  # driver + glue
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES
    ${_glue}/drivers/wifi/ipro_wifi.c
    ${_glue}/subsys/net/ipro_wifi/net_al_zephyr.c ${_glue}/subsys/net/ipro_wifi/net_al_zephyr_iface.c
    ${_glue}/hal_glue/ipro_wifi_rtos_al.c ${_glue}/hal_glue/ipro_wifi_platform.c)
  # fhost TX + MAC push / confirm / ISR
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/wifi6/ipro_fhost/src/fhost_tx.c
    FILTER "fhost_tx_(start|req|schedule|cfm|task|get_hw_desc_pointers|release_buf|get_txq|do_|amsdu)")
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES
    ${_sdk}/macsw/modules/macif/src/macif_fhost.c ${_sdk}/macsw/ip/umac/src/txu/txu_cntrl.c
    ${_sdk}/macsw/ip/lmac/src/tx/txl/txl_cfm.c ${_sdk}/macsw/ip/lmac/src/tx/tx_swdesc.c
    ${_sdk}/macsw/modules/ke/src/ke_event.c ${_sdk}/macsw/modules/common/src/co_list.c
    ${_sdk}/macsw/modules/adapter/src/rtos.c ${_sdk}/macsw/plf/refip/src/driver/intc/risc-v/intc.c)
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/ip/lmac/src/tx/txl/txl_cntrl.c
    FILTER "txl_(transmit_trigger|cntrl_push|cntrl_newhead|frame_exchange_chain|payload_handle|hwdesc_config_post|timer_|cntrl_pending_newhead_drain|pending_release_drain|no_advance_should_count)")
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/ip/lmac/src/tx/txl/txl_agg.c
    FILTER "txl_(agg_push_mpdu|agg_finish|agg_close_pending|ampdu_constraints_get|agg_hold_timer|agg_ba_decode|agg_chain|agg_split|agg_reconfig_ampdu|agg_change_to_singleton|agg_prep|agg_desc_)")
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/ip/lmac/src/hal/hal_machw.c
    FILTER "hal_machw_(gen_handler|abs_timer_handler|time)")
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/ip/lmac/src/rx/rxl/rxl_cntrl.c
    FILTER "rxl_(mpdu_isr|dma_evt|cntrl_evt|hwdesc_)")
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/ip/lmac/src/mm/mm_timer.c ${_sdk}/macsw/ip/umac/src/bam/bam.c)
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/ip/lmac/src/rc/rc.c
    FILTER "rc_(tx_check|check|update|get_)")
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/wifi6/ipro_fhost/src/fhost_tx.c
    FILTER "(prof_time|prof_cycle|tx_desc_init_for_fhost|fhost_tx_queue)")
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/ip/lmac/src/tx/txl/txl_agg.c
    FILTER "txl_agg_check")
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/modules/macif/src/macif_fhost.c ${_sdk}/macsw/ip/lmac/src/tx/tx_swdesc.c
    FILTER "(tx_desc_copy_from_shared|tx_txdesc_get|tx_txdesc_prep_for_push)")
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/ip/connection.c
    FILTER "net_conn_(change_local|change_remote|update)")
  zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/ip/net_context.c ${_znet}/ip/ipv4.c
    FILTER "(net_context_get_family|net_context_send|net_context_create_ipv4_new|ipv4_is_broadcast_address|net_if_ipv4_is_addr_bcast)")

# RX path (also carries the lab's broadcast noise: ~400 us per received frame
# when it runs from flash). A UDP download at 40 Mbps left the CPU 7 % idle
# (2026-09-29): RX is CPU-bound, so the whole per-MPDU chain goes to RAM.
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/wifi6/ipro_fhost/src/fhost_rx.c)
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/ip/umac/src/rxu/rxu_cntrl.c
  FILTER "rxu_(cntrl_frame_handle|msdu_upload_and_indicate|mpdu_upload_and_indicate|cntrl_reord|cntrl_mic_check|cntrl_protected_handle|cntrl_evt)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/ip/lmac/src/mm/mm.c
  FILTER "mm_update_buffer_control")
# Per received MPDU in the MAC, as FreeRTOS's IPRO6S_HOTCODE has them. The
# compiled descriptor file is rxl/v20/rxl_hwdesc.c: the old entry named
# rxl/rxl_hwdesc.c, which does not exist, so none of it was ever relocated.
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/ip/lmac/src/rx/rxl/v20/rxl_hwdesc.c
  FILTER "^[.]text[.]rxl_(mpdu_isr|rxdesc_get|desc_check|hw_addr_in_buffers|rxdesc_ready_for_processing|mpdu_transfer|frame_release|mpdu_release|mpdu_free|irq_clear)([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/ip/lmac/src/rx/rxl/rxl_cntrl.c
  FILTER "^[.]text[.]rxl_dma_int_handler([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/plf/refip/src/driver/dma/dma.c
  FILTER "^[.]text[.](dma_push|dma_sw_copy)([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/rf/ipro6_rf/phy/phy.c
  FILTER "^[.]text[.]phy_get_channel([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/rf/ipro6_rf/phy/wl_api.c
  FILTER "^[.]text[.]wl_wlan_bb_pre_proc([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/rf/ipro6_rf/phy/bba.c
  FILTER "^[.]text[.]bba_rssi_correction([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/ip/lmac/src/ps/ps.c
  FILTER "^[.]text[.]ps_check_frame([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/ip/umac/src/rxu/rxu_cntrl.c
  FILTER "^[.]text[.]rxu_(cntrl_desc_prepare|stat_desc_check|cntrl_duplicate_check)([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/ip/lmac/src/mm/mm.c
  FILTER "^[.]text[.]mm_ba_agmt_rx_exists([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/ip/umac/src/llc/llc.c
  ${ZEPHYR_PICOLIBC_MODULE_DIR}/newlib/libc/machine/riscv/memmove-stub.c)
# Zephyr RX: frame -> net_pkt -> IPv4 -> connection -> socket. (Zephyr 4.4
# names; the old filter's process_data/net_rx/net_if_l2/net_queue_rx match
# nothing any more.)
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/ip/net_core.c
  FILTER "^[.]text[.](net_recv_data|net_process_rx_packet)([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/ip/net_if.c
  FILTER "^[.]text[.]net_if_recv_data([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/ip/net_tc.c
  FILTER "^[.]text[.](net_tc_submit_to_rx_queue|tc_rx_handler|net_rx_priority2tc|net_tx_priority2tc)([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/ip/ipv4.c
  FILTER "^[.]text[.](net_ipv4_input|net_ipv4_prepare_for_send)([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/ip/connection.c
  FILTER "^[.]text[.]net_conn_input([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${ZEPHYR_BASE}/lib/net_buf/buf_simple.c
  FILTER "^[.]text[.]net_buf_simple_(add|add_mem|pull|remove_mem)([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/lib/sockets/sockets.c
  FILTER "^[.]text[.]z_impl_zsock_recvfrom([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/lib/sockets/sockets_inet.c
  FILTER "^[.]text[.](zsock_received_cb|zsock_recv_dgram|zsock_wait_data|sock_recvfrom_vmeth)([.]|$)")
# Small helpers every packet calls (checksum decisions go through the
# driver's get_capabilities, already in RAM with ipro_wifi.c).
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/ip/net_if.c
  FILTER "^[.]text[.](net_if_ipv4_is_addr_bcast_raw|net_if_need_calc_rx_checksum|net_if_need_calc_tx_checksum|need_calc_checksum|z_impl_net_if_get_by_index|net_if_get_by_iface|net_if_ipv4_select_src_iface|net_if_ipv4_get_ttl)([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_znet}/ip/utils.c
  FILTER "^[.]text[.]net_ipv4_(broadcast|unspecified)_address([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_glue}/hal_glue/ipro_irq_glue.c
  FILTER "^[.]text[.]ipro_irq_trampoline([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_sdk}/macsw/include/export/export_func.c
  FILTER "^[.]text[.](inline_hal_machw_time|inline_hal_machw_time_past|mac_vif_get_type|mac_vif_get_active|mac_vif_get_index|get_nx_txdesc_cnt)([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES
  ${ZEPHYR_PICOLIBC_MODULE_DIR}/newlib/libc/string/memcmp.c ${ZEPHYR_BASE}/lib/libc/picolibc/errno_wrap.c)
# Kernel IPC and time on the TX/RX hand-offs (msgq, queue, condvar, poll,
# timeouts) and the system timer read.
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_zker}/msg_q.c
  FILTER "^[.]text[.]z_impl_k_msgq_(put|get)([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_zker}/queue.c
  FILTER "^[.]text[.](queue_insert|z_impl_k_queue_get|k_queue_append|k_queue_prepend|z_impl_k_queue_peek_head|z_impl_k_queue_peek_tail)([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_zker}/condvar.c
  FILTER "^[.]text[.]z_impl_k_condvar_(signal|wait)([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_zker}/poll.c
  FILTER "^[.]text[.]z_handle_obj_poll_events([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_zker}/thread.c
  FILTER "^[.]text[.](k_is_in_isr|z_impl_k_thread_custom_data_get|z_impl_k_thread_custom_data_set)([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_zker}/timeout.c
  FILTER "^[.]text[.](sys_timepoint_calc|sys_timepoint_timeout|z_abort_timeout|z_add_timeout|remove_timeout|z_impl_k_uptime_ticks)([.]|$)")
zephyr_code_relocate(NOKEEP LOCATION RAM_TEXT FILES ${_glue}/drivers/timer/ipro_systimer.c
  FILTER "^[.]text[.](sys_clock_elapsed|sys_clock_cycle_get_32)([.]|$)")

set(IPRO_WIFI_HOT_SDK_SOURCES
  ${_sdk}/wifi6/ipro_fhost/src/fhost_rx.c
  ${_sdk}/macsw/ip/lmac/src/rx/rxl/v20/rxl_hwdesc.c
  ${_sdk}/macsw/ip/umac/src/rxu/rxu_cntrl.c
  ${_sdk}/macsw/ip/lmac/src/mm/mm.c
  ${_sdk}/macsw/ip/lmac/src/hal/hal_machw.c
  ${_sdk}/macsw/ip/lmac/src/mm/mm_timer.c
  ${_sdk}/macsw/ip/lmac/src/rc/rc.c
  ${_sdk}/macsw/ip/lmac/src/rx/rxl/rxl_cntrl.c
  ${_sdk}/macsw/ip/lmac/src/tx/tx_swdesc.c
  ${_sdk}/macsw/ip/lmac/src/tx/txl/txl_agg.c
  ${_sdk}/macsw/ip/lmac/src/tx/txl/txl_cfm.c
  ${_sdk}/macsw/ip/lmac/src/tx/txl/txl_cntrl.c
  ${_sdk}/macsw/ip/umac/src/bam/bam.c
  ${_sdk}/macsw/ip/umac/src/txu/txu_cntrl.c
  ${_sdk}/macsw/modules/adapter/src/rtos.c
  ${_sdk}/macsw/modules/common/src/co_list.c
  ${_sdk}/macsw/modules/ke/src/ke_event.c
  ${_sdk}/macsw/modules/macif/src/macif_fhost.c
  ${_sdk}/macsw/plf/refip/src/driver/intc/risc-v/intc.c
  ${_sdk}/wifi6/ipro_fhost/src/fhost_tx.c
)

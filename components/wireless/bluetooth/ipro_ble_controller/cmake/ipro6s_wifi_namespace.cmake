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

# RW Bluetooth and MACSW Wi-Fi carry different kernel/common implementations.
# Keep the Bluetooth implementation and all its callers in a private namespace.
# These definitions apply only to controller sources, never to Wi-Fi/the Host.
# AES entry points also differ from WPA supplicant's public crypto API.
set(_ipro6s_private_symbols
    aes_ccm
    aes_client_alloc
    aes_cmac
    aes_cmac_client_alloc
    aes_cmac_finished
    aes_cmac_start
    aes_cmac_zero
    aes_decrypt
    aes_encrypt
    aes_finished
    aes_gen_rand_addr
    aes_h6
    aes_h7
    aes_h8
    aes_h9
    aes_rand
    aes_rpa_gen
    aes_rpa_resolve
    aes_shift_left_128
    aes_start
    aes_start_with_same_key
    aes_xor_128
    co_list_extract
    co_list_extract_after
    co_list_extract_sublist
    co_list_find
    co_list_init
    co_list_insert_after
    co_list_insert_before
    co_list_merge
    co_list_pool_init
    co_list_pop_front
    co_list_push_back
    co_list_push_back_sublist
    co_list_push_front
    co_list_size
    ke_env
    ke_event_callback_set
    ke_event_clear
    ke_event_flush
    ke_event_get
    ke_event_get_all
    ke_event_init
    ke_event_process
    ke_event_process_single
    ke_event_set
    ke_flush
    ke_free
    ke_init
    ke_is_free
    ke_malloc_system
    ke_malloc_user
    ke_mem_init
    ke_mem_is_memory_heap_pool_used
    ke_msg_alloc
    ke_msg_dest_id_get
    ke_msg_discard
    ke_msg_forward
    ke_msg_forward_new_id
    ke_msg_free
    ke_msg_in_queue
    ke_msg_save
    ke_msg_send
    ke_msg_send_basic
    ke_msg_src_id_get
    ke_queue_extract
    ke_queue_insert
    ke_sleep_check
    ke_state_get
    ke_state_set
    ke_task_check
    ke_task_create
    ke_task_delete
    ke_task_init
    ke_task_msg_flush
    ke_timer_active
    ke_timer_clear
    ke_timer_flush
    ke_timer_get
    ke_timer_set
)
foreach(_symbol IN LISTS _ipro6s_private_symbols)
    __component_add_private_compile_definitions(${_symbol}=ipro6s_btdm_${_symbol})
endforeach()
unset(_symbol)
unset(_ipro6s_private_symbols)

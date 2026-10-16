/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * SDK RAM-resident code/data (compiler/common.h attributes). Placed inside
 * Zephyr's .data output section so the XIP data copy loads them from flash
 * into OCRAM exactly like early_section_copy() does in the SDK:
 *   ATTR_TCM_SECTION / ATTR_TCM_CONST_SECTION  -> .tcm_code.* / .tcm_const.*
 *   ATTR_CLOCK_SECTION / _CONST_SECTION         -> .sclock_rlt_code.* / _const.*
 *   ATTR_DTCM_SECTION                           -> .tcm_data*
 *   ATTR_OCRAM_SECTION                          -> .ocram_data*
 *   ATTR_SYSRAM_SECTION                         -> .system_ram*
 * Flash write/erase paths (hal_flash.c) are ATTR_TCM_SECTION and must not
 * execute from XIP while the SF controller is busy, hence RAM.
 * No KEEP: each function has its own section (.tcm_code.<file><line>), so
 * --gc-sections drops the ~400 ROM-API wrappers and flash helpers nothing
 * calls (wifi_sta: 16.6 KB of RAM code -> ~5 KB). Code reached through a
 * relocation is kept as usual.
 */
. = ALIGN(8);
__ipro_tcm_code_start = .;
*(.tcm_code.*)
*(.tcm_const.*)
*(.sclock_rlt_code.*)
*(.sclock_rlt_const.*)
. = ALIGN(8);
__ipro_tcm_code_end = .;
*(.tcm_data*)
*(.ocram_data*)
*(.system_ram*)
. = ALIGN(8);

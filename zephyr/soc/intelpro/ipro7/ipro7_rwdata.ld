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
 */
. = ALIGN(8);
__ipro_tcm_code_start = .;
KEEP(*(.tcm_code.*))
KEEP(*(.tcm_const.*))
KEEP(*(.sclock_rlt_code.*))
KEEP(*(.sclock_rlt_const.*))
. = ALIGN(8);
__ipro_tcm_code_end = .;
*(.tcm_data*)
*(.ocram_data*)
*(.system_ram*)
. = ALIGN(8);

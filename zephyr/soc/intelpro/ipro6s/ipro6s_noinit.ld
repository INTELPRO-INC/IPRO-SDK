/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * SDK non-initialised data:
 *   ATTR_NOINIT_DATA_SECTION      -> .noinit_data   (boot2 hand-off copy,
 *                                    written in soc_prep_hook before .bss
 *                                    is cleared, so it MUST be noinit)
 *   ATTR_NOCACHE_NOINIT_RAM_SECTION -> .nocache_noinit_ram
 *   ATTR_NOCACHE_RAM_SECTION      -> .nocache_ram (see known_issues.md:
 *                                    kept at the cacheable address, users
 *                                    call L1C_DCACHE_* around DMA)
 */
. = ALIGN(8);
*(.noinit_data*)
*(.nocache_noinit_ram*)
*(.nocache_ram*)
. = ALIGN(8);

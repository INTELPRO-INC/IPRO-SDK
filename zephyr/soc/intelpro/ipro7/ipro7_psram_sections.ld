/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * SDK ATTR_PSRAM_* placements when the board has PSRAM (device-tree
 * zephyr,memory-region "PSRAM" at 0x1E000000). Code/data are loaded from
 * flash and bss cleared by ipro7_psram_prep() in soc.c, after the PSRAM
 * controller has been trained.
 */
SECTION_PROLOGUE(.ipro_psram_data,,)
{
	. = ALIGN(8);
	__ipro_psram_data_start = .;
	KEEP(*(.psram_code.*))
	*(.psram_data*)
	. = ALIGN(8);
	__ipro_psram_data_end = .;
} GROUP_DATA_LINK_IN(PSRAM, ROMABLE_REGION)
__ipro_psram_data_load_start = LOADADDR(.ipro_psram_data);

SECTION_PROLOGUE(.ipro_psram_bss,(NOLOAD),)
{
	. = ALIGN(8);
	__ipro_psram_bss_start = .;
	*(.psram_bss*)
	. = ALIGN(8);
	__ipro_psram_bss_end = .;
	*(.psram_noinit*)
	. = ALIGN(8);
} GROUP_LINK_IN(PSRAM)

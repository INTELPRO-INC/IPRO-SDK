/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * SDK zero-initialised OCRAM data (ATTR_OCRAM_BSS_SECTION). Zephyr has no
 * hook inside its .bss output section, so this is a separate NOLOAD
 * section that soc_prep_hook() clears.
 */
SECTION_PROLOGUE(.ipro_bss,(NOLOAD),)
{
	. = ALIGN(8);
	__ipro_bss_start = .;
	*(.ocram_bss*)
#ifndef CONFIG_IPRO_HAL_USE_PSRAM
	*(.psram_bss*)
#endif
	. = ALIGN(8);
	__ipro_bss_end = .;
} GROUP_LINK_IN(RAMABLE_REGION)

/*
 * Retained diagnostics in the 4 KB always-on HBN RAM (0x20090000).
 * hal_hbnram_mgnt.h partitions that RAM: LP firmware +0x000..+0x3ff, LP
 * parameter block +0x400..+0x5ff, reset/wake/boot2 blocks through +0x8e3,
 * app-owned window +0x8e4..+0xeff (first word reserved for the bootloader
 * magic), iot2lp block +0xf00..+0xfff. The prebuilt BLE controller and
 * hal_pm.c place breadcrumbs in .hbn_noinit; keep the SDK's placement at
 * +0x8e8 so tooling that reads them (CCI) sees the same addresses. The
 * section is absolute (no Zephyr memory region needed) and never loaded.
 */
.hbn_noinit 0x200908e8 (NOLOAD) : ALIGN(4)
{
	__hbn_noinit_start = .;
	KEEP(*(.hbn_noinit))
	KEEP(*(.hbn_noinit.*))
	. = ALIGN(4);
	__hbn_noinit_end = .;
}
ASSERT(__hbn_noinit_end <= 0x20090f00, "HBN noinit exceeds the IPRO7 app-owned window")

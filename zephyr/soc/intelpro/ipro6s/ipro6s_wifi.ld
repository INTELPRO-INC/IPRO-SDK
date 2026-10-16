/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Wi-Fi MAC shared memory in WRAM (the non-cacheable 0x21050000 alias of
 * the 160 KB Wi-Fi RAM), as bsp/board/linker/ipro6/ipro6_common.ld.in lays
 * it out: RX buffer 1 first and 32 KB-aligned (a descriptor write that
 * crosses a 32 KB boundary is truncated by the MAC), then the IPC and the
 * rest of the shared RAM, the COMMON symbols of the shared-memory objects,
 * and the remainder as the firmware's shared-RAM heap.
 */
SECTION_PROLOGUE(.wifibss, (NOLOAD),)
{
	PROVIDE(__wifi_bss_start = .);
	_sshram = .;
	. = ALIGN(32768);
	PROVIDE(__rxl_buf1_start = .);
	*(SHAREDRAM_RXBUF)
	PROVIDE(__rxl_buf1_end = .);
	*(SHAREDRAMIPC)
	*(SHAREDRAM)
	_eshram = ORIGIN(WRAM) + LENGTH(WRAM);
	/* The SDK script lists these objects' COMMON symbols; Zephyr builds with
	 * -fno-common, so the same variables are ordinary .bss here. */
	*ipc_shared.c.obj(.bss .bss.* COMMON)
	*sdu_shared.c.obj(.bss .bss.* COMMON)
	*hal_desc.c.obj(.bss .bss.* COMMON)
	*txl_buffer_shared.c.obj(.bss .bss.* COMMON)
	*txl_frame_shared.c.obj(.bss .bss.* COMMON)
	*scan_shared.c.obj(.bss .bss.* COMMON)
	*scanu_shared.c.obj(.bss .bss.* COMMON)
	*mfp_bip.c.obj(.bss .bss.* COMMON)
	*me_mic.c.obj(.bss .bss.* COMMON)
	. = ALIGN(32);
	*(.wifi_txheap*)
	. = ALIGN(16);
	PROVIDE(__wifi_bss_end = .);
	PROVIDE(_heap_wifi_start = .);
	PROVIDE(_heap_wifi_size = ORIGIN(WRAM) + LENGTH(WRAM) - _heap_wifi_start);
} GROUP_LINK_IN(WRAM)
ASSERT((__rxl_buf1_start & 0x7fff) == 0 && __rxl_buf1_end - __rxl_buf1_start <= 0x8000 &&
       ((__rxl_buf1_end - __rxl_buf1_start) & 0x3ff) == 0,
       "MAC RX buffer 1 must be 32 KB-aligned, at most 32 KB and a multiple of 1 KB")
/* RX buffer 2 (680 B, rxl_hwdesc.c) takes BA and trigger frames; with a 16 KB
 * RX buffer 1 it ended up across 0x21058000. The TX fetch fault is proven at
 * 16 KB boundaries, so hold it to that. */
ASSERT((ipro_rxl_hw_buffer2 >> 14) == ((ipro_rxl_hw_buffer2 + 679) >> 14),
       "MAC RX buffer 2 crosses a 16 KB boundary: check CONFIG_IPRO_WIFI_RXL_BUF1_BYTES")

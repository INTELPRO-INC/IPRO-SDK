/* SDK ATTR_OCRAM_BSS_SECTION data (zeroed by soc_prep_hook) and the HBN RAM
 * no-init window, mirrored from the IPRO7 port. */
SECTION_PROLOGUE(.ipro_bss,(NOLOAD),)
{
	. = ALIGN(8);
	__ipro_bss_start = .;
	*(.ocram_bss*)
	*(.psram_bss*)
	. = ALIGN(8);
	__ipro_bss_end = .;
} GROUP_LINK_IN(RAMABLE_REGION)

.hbn_noinit 0x200908e8 (NOLOAD) : ALIGN(4)
{
	__hbn_noinit_start = .;
	KEEP(*(.hbn_noinit))
	KEEP(*(.hbn_noinit.*))
	. = ALIGN(4);
	__hbn_noinit_end = .;
}
ASSERT(__hbn_noinit_end <= 0x20090f00, "HBN noinit exceeds the IPRO6S app-owned window")

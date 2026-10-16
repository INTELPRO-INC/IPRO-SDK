/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * The rv32imafc_zba_zbb_zbc_zbs libgcc multilib carries an (empty) Zcmt
 * table-jump section in a few objects. Give it a home in flash so the
 * linker's orphan-section warning cannot trip a --fatal-warnings build.
 */
SECTION_PROLOGUE(.riscv.jvt,,)
{
	KEEP(*(.riscv.jvt))
} GROUP_LINK_IN(ROMABLE_REGION)

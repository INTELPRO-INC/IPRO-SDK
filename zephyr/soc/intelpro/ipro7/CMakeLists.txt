# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0

zephyr_include_directories(.)

zephyr_sources(
  entry.S
  soc.c
  soc_irq.S
)

# Reset entry first in flash: boot2 jumps to 0x13000000 + 0. The key sorts
# after arch/common's "0x0" rom_start_offset.ld (which must run first) and
# before the arch "0x0vectors" fragment that holds Zephyr's generic __start.
zephyr_linker_sources(ROM_START SORT_KEY 0x0entry init.ld)

# SDK section attributes (ATTR_TCM_SECTION, ATTR_OCRAM_SECTION, ...) mapped
# onto Zephyr's data / bss / noinit regions so the HAL links unchanged.
zephyr_linker_sources(ROM_SECTIONS ipro7_rom_sections.ld)
zephyr_linker_sources(RWDATA ipro7_rwdata.ld)
zephyr_linker_sources(NOINIT ipro7_noinit.ld)
zephyr_linker_sources(SECTIONS ipro7_sections.ld)
zephyr_linker_sources_ifdef(CONFIG_IPRO_HAL_USE_PSRAM SECTIONS ipro7_psram_sections.ld)

# Flash-programming and TCM code (ATTR_TCM_SECTION) is copied into OCRAM
# next to .data, so the RAM load segment is legitimately RWX. binutils
# >= 2.39 warns about that; the warning would be fatal under twister's
# --fatal-warnings and carries no information for this SoC.
zephyr_ld_options(-Wl,--no-warn-rwx-segments)

set(SOC_LINKER_SCRIPT ${ZEPHYR_BASE}/include/zephyr/arch/riscv/common/linker.ld CACHE INTERNAL "")

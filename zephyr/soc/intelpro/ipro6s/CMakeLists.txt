# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0

zephyr_include_directories(.)

zephyr_sources(
  entry.S
  soc.c
  soc_irq.S
)

# Reset entry first in flash (image offset 0), see the IPRO7 port.
zephyr_linker_sources(ROM_START SORT_KEY 0x0entry init.ld)
zephyr_linker_sources(ROM_SECTIONS ipro6s_rom_sections.ld)
zephyr_linker_sources(RWDATA ipro6s_rwdata.ld)
zephyr_linker_sources(NOINIT ipro6s_noinit.ld)
zephyr_linker_sources(SECTIONS ipro6s_sections.ld)

zephyr_ld_options(-Wl,--no-warn-rwx-segments)
# The SDK RAM code (ipro6s_rwdata.ld, RWDATA) sits between .sdata, where gp
# points, and .sbss. GP relaxation picks .sbss symbols that look in range,
# then alignment padding in the relaxed code pushes them out of it
# ("relocation truncated to fit: R_RISCV_GPREL_I") whenever that code
# shrinks. Before --gc-sections could trim it, it kept .sbss out of gp's reach
# anyway, so turning GP relaxation off costs nothing.
zephyr_ld_options(-Wl,--no-relax-gp)

set(SOC_LINKER_SCRIPT ${ZEPHYR_BASE}/include/zephyr/arch/riscv/common/linker.ld CACHE INTERNAL "")

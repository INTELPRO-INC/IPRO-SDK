/*
 * IPRO7 FPGA Board Linker Script Override
 * Copyright (c) 2025 IPRO
 * SPDX-License-Identifier: Apache-2.0
 * 
 * This file overrides default RISC-V memory permissions to eliminate
 * RWX security warnings by setting RAM to read-write only.
 */

#include <zephyr/devicetree.h>
#include <zephyr/linker/sections.h>
#include <zephyr/linker/linker-defs.h>
#include <zephyr/linker/linker-tool.h>

/* Board Memory Layout */
#define ROM_BASE        DT_REG_ADDR(DT_CHOSEN(zephyr_flash))
#define ROM_SIZE        DT_REG_SIZE(DT_CHOSEN(zephyr_flash))
#define RAM_BASE        DT_REG_ADDR(DT_CHOSEN(zephyr_sram))
#define RAM_SIZE        DT_REG_SIZE(DT_CHOSEN(zephyr_sram))

MEMORY
{
#ifdef CONFIG_XIP
    ROM (rx)  : ORIGIN = ROM_BASE, LENGTH = ROM_SIZE
#endif
    /* Change RAM from (rwx) to (rw) to eliminate security warning */
    RAM (rw) : ORIGIN = RAM_BASE, LENGTH = RAM_SIZE

    LINKER_DT_REGIONS()

    /* Used by and documented in include/linker/intlist.ld */
    IDT_LIST  (wx) : ORIGIN = 0xFFFFF7FF, LENGTH = 2K
}

/* Use standard RISC-V layout */
#define ROMABLE_REGION              RAM
#define RAMABLE_REGION              RAM

ENTRY(CONFIG_KERNEL_ENTRY)

#include <zephyr/arch/riscv/common/linker.ld>

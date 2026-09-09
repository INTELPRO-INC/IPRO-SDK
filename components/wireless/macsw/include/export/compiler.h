/**
 ****************************************************************************************
 *
 * @file riscv32/compiler.h
 *
 * @brief Definitions of compiler specific directives.
 *
 * Copyright (C) RivieraWaves 2011-2020
 *
 ****************************************************************************************
 */

#ifndef _COMPILER_H_
#define _COMPILER_H_

/// define the force inlining attribute for this compiler
#ifndef __INLINE
#define __INLINE static __attribute__((__always_inline__)) inline
#endif

/// define the IRQ handler attribute for this compiler
#define __IRQ __attribute__((interrupt))

/// function has no side effect and return depends only on arguments
#define __PURE __attribute__((const))

/// Align instantiated lvalue or struct member on 4 bytes
#define __ALIGN4 __attribute__((aligned(4)))

/// Pack a structure field
#define __PACKED16 __attribute__ ((__packed__))
/// Pack a structure field
#ifndef __PACKED
#define __PACKED __attribute__ ((__packed__))
#endif

/// __MODULE__ comes from the RVDS compiler that supports it
#define __MODULE__ __FILENAME__

/// define a variable as maybe unused, to avoid compiler warnings on it
#define __MAYBE_UNUSED __attribute__((unused))

#define __FALLTHROUGH __attribute__((fallthrough))

// Mapping of these different elements is already handled in the map.txt file, so no need
// to define anything here
/// SHARED RAM for IPC structure
#define __SHAREDRAMIPC __attribute__ ((section("SHAREDRAMIPC")))
/// SHARED RAM
#define __SHAREDRAM __attribute__ ((section("SHAREDRAM")))
/* SHARED RAM, but placed FIRST and 16 KB-aligned. Only the MAC RX ring uses it.
 *
 * The MAC truncates a descriptor write at a 32 KB boundary in ram_wifi: the fields below
 * the boundary land, the first word at or after it is never written and keeps whatever
 * the ring already held, and SW then follows that word as a payload pointer. Measured
 * 2026-08-10 - see RC_P27_0810.md, P28/P29.
 *
 * A 16 KB buffer contains an interior 32 KB boundary ONLY when it is not 16 KB-aligned,
 * so aligning it removes the fault by construction. Done by PLACEMENT rather than by an
 * aligned() attribute because the attribute pads: at ram_wifi's ORIGIN the alignment is
 * free, whereas `aligned(16384)` on the array costs up to 16 KB and overflows
 * defconfig_ipro6_sdio (measured 166,784 / 163,840).
 *
 * Its own section because `rxl_hwdesc.c`'s SHAREDRAM is one chunk with RX buffer 2
 * emitted ahead of buffer 1, so placing the chunk first would still land buffer 1 680
 * bytes in. Every linker script that collects SHAREDRAM must also collect this one, or
 * the buffer becomes an orphan section the linker places wherever it likes. */
#define __SHAREDRAM_RXBUF __attribute__ ((section("SHAREDRAM_RXBUF")))
/// LA RAM
#define __LARAMMAC __attribute__ ((section("LARAM")))
/// LA RAM PHY memory
#define __LARAMPHY __attribute__ ((section("LARAM2")))
/// MIB memory
#define __MIB __attribute__ ((section("MACHWMIB")))

#endif // _COMPILER_H_

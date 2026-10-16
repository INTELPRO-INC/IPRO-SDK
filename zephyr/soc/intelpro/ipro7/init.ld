/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Pin the reset entry (entry.S, section .init.__ipro7_start) to the very
 * start of ROM: the IPRO7 boot2 loader jumps to flash offset 0.
 */
KEEP(*(.init.*))

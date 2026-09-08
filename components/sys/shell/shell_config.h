/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __SHELL_CONFIG_H__
#define __SHELL_CONFIG_H__

#ifndef SHELL_DEFAULT_NAME
#define SHELL_DEFAULT_NAME "ipro "
#endif

#ifndef SHELL_CONSOLEBUF_SIZE
#define SHELL_CONSOLEBUF_SIZE 128
#endif

#ifndef SHELL_PROMPT_SIZE
#define SHELL_PROMPT_SIZE 20
#endif

#ifndef SHELL_HISTORY_LINES
#define SHELL_HISTORY_LINES 5
#endif

#ifndef SHELL_CMD_SIZE
#define SHELL_CMD_SIZE 256
#endif

#ifndef SHELL_ARG_NUM
#define SHELL_ARG_NUM 16
#endif

//#define SHELL_USING_FS
#define SHELL_USING_COLOR
#define SHELL_USING_DESCRIPTION

#ifndef SHELL_THREAD_STACK_SIZE
#if defined(CONFIG_BTCTRL2_PDS_SNAPSHOT_EXPERIMENTAL)
/* Measured btc2 shell peak use is 564 B; retain 1536 B for PDS diagnostics. */
#define SHELL_THREAD_STACK_SIZE 384
#else
/* 2048 words = 8 KB. FreeRTOS xTaskCreate size is in words (RISC-V word
 * = 4 B). 8 KB is the historical default and is sufficient under normal
 * prebuilt BLE controller link.
 *
 * The earlier 4096-word bump was a workaround for a source-built BLE
 * controller (2026-05-14) where the shell printf path appeared to
 * overflow at 8 KB. Root cause turned out to be a wild memory write in
 * the source-build path, not stack pressure — bumping to 4096 only
 * masked it. Reverted once the production prebuilt was rebuilt from
 * the new source. Bump again only if a future controller change
 * actually exhausts the printf call chain. */
/* [PERF 07-24] 2048 -> 1024 words (8K -> 4K): ps high-water shows <200
 * words used; freed .bss funds hot-code-to-RAM in ipro6_if_wifi. */
#define SHELL_THREAD_STACK_SIZE 768
#endif
#endif

#ifndef SHELL_THREAD_PRIO
#define SHELL_THREAD_PRIO 20
#endif

#ifndef SHELL_EXEC_THREAD_STACK_SIZE
/* 2048 words = 8 KB. shell_exec is allocated from FreeRTOS heap on every
 * shell command dispatch; oversizing it (e.g. 4096) exhausted heap
 * mid-session under the SIG ESL apps (~48 KB heap budget after BLE init).
 * See SHELL_THREAD_STACK_SIZE comment. */
#define SHELL_EXEC_THREAD_STACK_SIZE 1024
#endif

#ifndef SHELL_EXEC_THREAD_PRIO
#define SHELL_EXEC_THREAD_PRIO 19
#endif

#endif

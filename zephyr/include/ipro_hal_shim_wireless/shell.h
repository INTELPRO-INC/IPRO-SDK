/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * SDK CLI registration under Zephyr.
 *
 * The SDK wireless components register their debug commands with the SDK
 * shell (components/sys/shell/shell.h) under CONFIG_SHELL. Zephyr's own
 * CONFIG_SHELL enables a different shell, so when these components are
 * built for Zephyr the SDK registration macros become no-ops. Equivalent
 * Zephyr shell commands, where wanted, live in zephyr/drivers/.
 */

#ifndef IPRO_ZEPHYR_SDK_SHELL_SHIM_H_
#define IPRO_ZEPHYR_SDK_SHELL_SHIM_H_

#define SHELL_CMD_EXPORT(command, desc)
#define SHELL_CMD_EXPORT_ALIAS(command, alias, desc)

#endif /* IPRO_ZEPHYR_SDK_SHELL_SHIM_H_ */

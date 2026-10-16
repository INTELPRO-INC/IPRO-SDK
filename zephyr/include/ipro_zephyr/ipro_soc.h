/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * SoC-level services of the IPRO port that have no Zephyr API of their own.
 */
#ifndef IPRO_ZEPHYR_IPRO_SOC_H_
#define IPRO_ZEPHYR_IPRO_SOC_H_

#include <zephyr/toolchain.h>

/**
 * Reset the chip into the BootROM's UART download mode so the IntelProCube
 * flash tool can program it without the BOOT pin (the SDK's "reboot_dl").
 * Does not return.
 */
FUNC_NORETURN void ipro_soc_reboot_download(void);

#endif /* IPRO_ZEPHYR_IPRO_SOC_H_ */

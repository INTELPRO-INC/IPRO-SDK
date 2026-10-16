/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * The board-level shell commands every IPRO firmware carries, with the
 * same names as the FreeRTOS SDK shell so the flash and bench tooling
 * (tools/zephyr/flash.sh, board_test.sh) works on either RTOS:
 *
 *   reboot      normal reset (sys_reboot)
 *   reboot_dl   reset into the BootROM UART download mode
 *   ipro info   chip id, reset cause, build
 */
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/drivers/hwinfo.h>
#include <ipro_zephyr/ipro_soc.h>
#include <version.h>

#ifdef CONFIG_SOC_SERIES_IPRO7
/* Raw silicon reset events (HBN_GLB[12:6], sticky until cleared) so a reset
 * the HAL can only call "software" (HBN RAM intact, no WDT0/PDS flag) can be
 * told apart: BOR, external reset pin, power reset, GLB software reset. */
#include "ipro7.h"
#include "drv_hbn.h"
#include "drv_timer.h"
#endif

static int cmd_reboot(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	shell_print(sh, "rebooting");
	k_msleep(20); /* let the line leave the UART FIFO */
	sys_reboot(SYS_REBOOT_COLD);
	return 0;
}

static int cmd_reboot_dl(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	shell_print(sh, "reboot to download mode");
	k_msleep(20);
	ipro_soc_reboot_download();
	return 0;
}

static int cmd_ipro_info(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	uint8_t id[8] = {0};
	uint32_t cause = 0;
	ssize_t n = hwinfo_get_device_id(id, sizeof(id));

	if (n > 0) {
		shell_fprintf(sh, SHELL_NORMAL, "chip id      ");
		for (ssize_t i = 0; i < n; i++) {
			shell_fprintf(sh, SHELL_NORMAL, "%02x", id[i]);
		}
		shell_fprintf(sh, SHELL_NORMAL, "\n");
	}
	if (hwinfo_get_reset_cause(&cause) == 0) {
		shell_print(sh, "reset cause  0x%08x%s%s%s%s%s", cause,
			    (cause & RESET_POR) ? " POR" : "",
			    (cause & RESET_SOFTWARE) ? " SOFTWARE" : "",
			    (cause & RESET_WATCHDOG) ? " WATCHDOG" : "",
			    (cause & RESET_CPU_LOCKUP) ? " CPU_LOCKUP" : "",
			    (cause & RESET_LOW_POWER_WAKE) ? " LOW_POWER_WAKE" : "");
	}
#ifdef CONFIG_SOC_SERIES_IPRO7
	uint32_t ev = (sys_read32(HBN_BASE + HBN_GLB_OFFSET) >> HBN_RESET_EVENT_POS) &
		      BIT_MASK(HBN_RESET_EVENT_LEN);

	shell_print(sh, "aon-wdt      %s (HBN_WDT 0x%08x)",
		    (sys_read32(HBN_BASE + HBN_WDT_OFFSET) & HBN_CR_ANO_WDT_EN_MSK) ? "ARMED" : "off",
		    sys_read32(HBN_BASE + HBN_WDT_OFFSET));
	shell_print(sh, "hbn events   0x%02x%s%s%s%s%s  aon-wdt-rst %d  wdt0-rst %d", ev,
		    (ev & BIT(HBN_RST_EVENT_POR_OUT)) ? " POR_OUT" : "",
		    (ev & BIT(HBN_RST_EVENT_EXT_RST_N)) ? " EXT_RST_N" : "",
		    (ev & BIT(HBN_RST_EVENT_SW_RST)) ? " SW_RST" : "",
		    (ev & BIT(HBN_RST_EVENT_PWR_RST_N)) ? " PWR_RST_N" : "",
		    (ev & BIT(HBN_RST_EVENT_BOR_OUT)) ? " BOR_OUT" : "",
		    HBN_AON_WDT_Get_Reset_Status(), WDT_GetResetStatus(WDT0_ID));
	/* The events accumulate across resets; clear them so the next reading
	 * shows only what happened since this command ran. */
	(void)HBN_Clear_Reset_Event();
#endif
	shell_print(sh, "board        %s", CONFIG_BOARD_TARGET);
	shell_print(sh, "zephyr       %s", KERNEL_VERSION_STRING);
	shell_print(sh, "uptime       %lld ms", k_uptime_get());
	return 0;
}

SHELL_CMD_REGISTER(reboot, NULL, "Reboot the system", cmd_reboot);
SHELL_CMD_REGISTER(reboot_dl, NULL, "Reboot into BootROM download mode", cmd_reboot_dl);

SHELL_STATIC_SUBCMD_SET_CREATE(sub_ipro,
	SHELL_CMD(info, NULL, "Chip id, reset cause, build", cmd_ipro_info),
	SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(ipro, &sub_ipro, "IPRO SoC commands", NULL);

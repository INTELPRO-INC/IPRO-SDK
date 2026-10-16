# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0

# `west flash` -> zephyr/scripts/runners/ipro.py: download mode through the
# IPRO7 CMSIS-DAP probe (or the shell's reboot_dl with --boot shell), image
# written by the IntelProCube CLI ($IPRO_FLASH_TOOL or on PATH).
# The EVK is usually on its own USB console without a probe: the shell's
# reboot_dl puts it in download mode (override with -- --boot probe).
board_runner_args(ipro "--chipname=ipro7" "--pt=partition_cfg_1M_boot2.toml" "--boot=shell")
board_set_flasher_ifnset(ipro)
board_finalize_runner_args(ipro)

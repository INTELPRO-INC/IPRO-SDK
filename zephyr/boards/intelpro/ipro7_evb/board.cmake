# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0

# `west flash` -> zephyr/scripts/runners/ipro.py: download mode through the
# IPRO7 CMSIS-DAP probe (or the shell's reboot_dl with --boot shell), image
# written by the IntelProCube CLI ($IPRO_FLASH_TOOL or on PATH).
board_runner_args(ipro "--chipname=ipro7" "--pt=partition_cfg_2M.toml")
board_set_flasher_ifnset(ipro)
board_finalize_runner_args(ipro)

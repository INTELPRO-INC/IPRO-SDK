# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0

# `west flash` -> zephyr/scripts/runners/ipro.py: download mode through the
# IPRO7 CMSIS-DAP probe (or the shell's reboot_dl with --boot shell), image
# written by the IntelProCube CLI ($IPRO_FLASH_TOOL or on PATH). The partition tables live next to the board: the boot2 4 MB
# layout the IPRO6 boot2 v0.5.3 boards ship with, and the border router's
# BootROM-direct + PSM table for the plain board.
if(CONFIG_BOARD_IPRO6S_EVB_IPRO6S_BOOT2)
  board_runner_args(ipro "--chipname=ipro6" "--pt=${CMAKE_CURRENT_LIST_DIR}/partition/partition_cfg_4M_boot2.toml" "--flash-pin=0x2")
else()
  board_runner_args(ipro "--chipname=ipro6" "--pt=${CMAKE_CURRENT_LIST_DIR}/partition/partition_cfg_2M_fw_direct_psm.toml" "--flash-pin=0x2")
endif()
board_set_flasher_ifnset(ipro)
board_finalize_runner_args(ipro)

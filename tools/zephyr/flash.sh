#!/usr/bin/env bash
#======================================================================
# Copyright (c) 2026 IntelPro Technology Co., Ltd.
# All rights reserved.
#
# This software and associated documentation files (the "Software") are
# proprietary and confidential to IntelPro Technology Co., Ltd..
#
# Access to and use of this Software is limited exclusively to authorized 
# customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
# valid license or non-disclosure agreement.
#
# Unauthorized copying, modification, distribution, or reverse engineering 
# of this Software, via any medium, is strictly prohibited without 
# prior written permission from IntelPro Technology Co., Ltd..
#
# SPDX-License-Identifier: Proprietary
#======================================================================

# Flash a Zephyr image with the IntelProCube CLI (same tool as the FreeRTOS
# apps). `west flash` uses its own runner, zephyr/scripts/runners/ipro.py.
#
#   tools/zephyr/flash.sh [--port /dev/cu.usbserialXXXX] [--build-dir DIR | zephyr.bin]
#
# The image is a raw binary linked at the 0x13000000 XIP window; the tool
# wraps it and writes it to the FW partition, exactly like an SDK app.
# Remember: flash_pin lives in <tool>/chips/ipro7/eflash_loader/
# eflash_loader_cfg.ini (0x9 for EVB/EVK/Arduino boards).
set -euo pipefail

SDK_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
# shellcheck source=common.sh
source "$(dirname "$0")/common.sh"
PORT=""
BIN=""
BUILD_DIR=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        --port) PORT="$2"; shift 2 ;;
        --build-dir) BUILD_DIR="$2"; shift 2 ;;
        --board) shift 2 ;;                      # accepted from board.cmake
        --elf-file|--hex-file) shift 2 ;;        # runner args we do not use
        --bin-file) BIN="$2"; shift 2 ;;
        -h|--help) sed -n '2,11p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *.bin) BIN="$1"; shift ;;
        *) shift ;;
    esac
done

if [[ -z "${BIN}" ]]; then
    [[ -n "${BUILD_DIR}" ]] || BUILD_DIR="$(pwd)/build"
    BIN="${BUILD_DIR}/zephyr/zephyr.bin"
fi
[[ -f "${BIN}" ]] || { echo "image not found: ${BIN}" >&2; exit 1; }
FLASH_TOOL="$(ipro_find_flash_tool)"
[[ -n "${PORT}" ]] || PORT="$(ipro_find_port)"
[[ -x "${FLASH_TOOL}" ]] || { echo "flash tool not found: ${FLASH_TOOL} (set IPRO_FLASH_TOOL)" >&2; exit 1; }
[[ -e "${PORT}" ]] || { echo "serial port not found: ${PORT} (set IPRO_SERIAL_PORT)" >&2; exit 1; }

echo "[flash] ${BIN} -> ${PORT}"
# Ask a running SDK/Zephyr shell to enter download mode; harmless otherwise.
python3 "${SDK_ROOT}/tools/serial_monitor.py" "${PORT}" --send reboot_dl --timeout 1 >/dev/null 2>&1 || true
sleep 1

LOG="$(mktemp)"
"${FLASH_TOOL}" --chipname ipro7 --port "${PORT}" --firmware "${BIN}" --baudrate 921600 2>&1 | tee "${LOG}"
if grep -q "SHAKEHAND FAIL\|Burn return with retry fail\|ErrorCode" "${LOG}"; then
    rm -f "${LOG}"
    echo "[flash] FAILED (put the board in download mode: BOOT pin / reboot_dl)" >&2
    exit 1
fi
rm -f "${LOG}"
echo "[flash] done"

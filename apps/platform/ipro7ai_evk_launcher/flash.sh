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

# flash.sh - flash ipro7ai_evk_launcher (firmware + gesture classifier) onto the IPRO7AI EVK.
#
#   ./flash.sh            firmware only (partition table + model already there)
#   ./flash.sh --full     partition table + firmware + gesture classifier (the RPS app's model/rps_v3.inm)
#   ./flash.sh --model    model only (swap the .blai without touching firmware)
#
# --full is needed once per board: the 1 MiB flash is split FW 320 KB /
# mfg 688 KB by partition_cfg_1M_launcher.toml and the model lives in mfg
# at +0x1000. Every mode passes the table; only --full/--model write the model.
#
# Env: PORT (default /dev/cu.usbserial-2120), BAUD (console baud of the firmware
# currently on the board, for the reboot_dl nudge; this app runs 921600),
# IPRO_FLASH_TOOL (else ipro_iot_tool_macos / _linux on PATH), MODEL.
#
# The EVK module needs flash_pin = 0x9 in the tool's
# chips/ipro7/eflash_loader/eflash_loader_cfg.ini (shipped default 0x2). With
# 0x2 the handshake passes but the loader reads jedec id 00000000 and stops
# with "IPRO FLASH MATCH TYPE FAIL" - that is the pin, not the board.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SDK="$(cd "$HERE/../../.." && pwd)"

PORT="${PORT:-/dev/cu.usbserial-2120}"
BAUD="${BAUD:-921600}"
# IntelProCube v0.6.3's command-line flasher: IPRO_FLASH_TOOL, else this OS's
# binary on PATH - the same lookup as build_freertos.sh.
case "$(uname -s)" in
    Darwin) TOOL_NAME="ipro_iot_tool_macos" ;;
    Linux)  TOOL_NAME="ipro_iot_tool_linux" ;;
    *)      TOOL_NAME="ipro_iot_tool.exe" ;;
esac
TOOL="${IPRO_FLASH_TOOL:-$(command -v "$TOOL_NAME" || true)}"
MODEL="${MODEL:-$SDK/apps/ai/ipro7ai_evk_rps/model/rps_v3.inm}"
PT="$HERE/partition_cfg_1M_launcher.toml"
BIN="$HERE/build/build_out/ipro7ai_evk_launcher_IPRO7.bin"

MODE="fw"
case "${1:-}" in
    --full)  MODE="full" ;;
    --model) MODE="model" ;;
    "")      ;;
    *) echo "usage: $0 [--full|--model]"; exit 2 ;;
esac

if [[ ! -x "$TOOL" ]]; then
    echo "flash tool not found: ${TOOL:-$TOOL_NAME}"
    echo "set IPRO_FLASH_TOOL to IntelProCube's $TOOL_NAME, or put its directory on PATH"
    exit 1
fi
if [[ "$MODE" != "model" ]]; then
    [[ -f "$BIN" ]] || { echo "build first: make -C apps/platform/ipro7ai_evk_launcher"; exit 1; }
    # The FW partition is 0x50000; the tool refuses a larger image, but say so
    # here with the numbers instead of "fw bin size is overflow".
    fw_max=$((0x50000)); fw_size=$(( $(wc -c < "$BIN") ))
    if (( fw_size > fw_max )); then
        echo "firmware $fw_size B exceeds the FW partition ($fw_max B)"; exit 1
    fi
fi
if [[ "$MODE" != "fw" ]]; then
    [[ -f "$MODEL" ]] || { echo "model not found: $MODEL"; exit 1; }
    # mfg slot is 0xAC000 with a 0x1000 header in front of the blob.
    m_max=$((0xAC000 - 0x1000)); m_size=$(( $(wc -c < "$MODEL") ))
    if (( m_size > m_max )); then
        echo "model $m_size B exceeds the mfg slot payload ($m_max B)"; exit 1
    fi
fi

# --pt on every mode: without it the tool writes its own default table, which
# has no mfg entry, and the firmware boots to "NO MFG PARTITION". Rewriting
# the same table is idempotent and does not touch the mfg contents.
args=(--chipname ipro7 --port "$PORT" --baudrate 921600 --skip-ota --pt "$PT")
case "$MODE" in
    full)  args+=(--firmware "$BIN" --mfg "$MODEL") ;;
    fw)    args+=(--firmware "$BIN") ;;
    model) args+=(--mfg "$MODEL") ;;
esac

# Ask the running firmware to reboot into download mode and confirm the shell
# echoed the command. serial_monitor.py --send proved unreliable right after a
# baud change (the bytes went out before the port settled): the board kept
# running and the tool then read the shell's ANSI echo of its sync bytes as a
# random-looking ack ("SHAKEHAND FAIL"). enter_dl.py retries and only returns
# 0 when it saw the echo; 1 means no shell answered (already in DL, or dead).
python3 "$HERE/enter_dl.py" "$PORT" "$BAUD" || true

echo "flashing ($MODE) on $PORT ..."
log="$(mktemp)"
# The tool exits 0 even when flashing fails, and a non-zero exit must not end
# the script here (pipefail) before the hint below: "[All Success]" is the verdict.
"$TOOL" "${args[@]}" 2>&1 | tee "$log" || true
if ! grep -q "\[All Success\]" "$log"; then
    rm -f "$log"
    echo "flash FAILED - is the board in download mode (reboot_dl, or J6 pin 3 off + RST)?"
    exit 1
fi
rm -f "$log"
echo "flash done. Reset the board (RST, or reboot at its console)"

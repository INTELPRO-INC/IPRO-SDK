#!/bin/sh
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

# Build a boot2 OTA package (.xz.ota) of the current Reef build.
#
#   IPRO_FLASH_TOOL=<IntelProCube dir>/ipro_iot_tool_<os> sh make_reef_ota.sh [out.xz.ota]
#
# IPRO_FLASH_TOOL is the IntelProCube command-line tool, the same variable
# build_freertos.sh uses (ipro_iot_tool_macos, ipro_iot_tool_linux or
# ipro_iot_tool.exe); unset, the tool is looked up in PATH.
#
# The package payload is the tool's combined image (whole_img.bin: IPNP
# bootheader, boot2 and the partition_cfg_2M_remote_ab.toml layout),
# XZ-compressed by tools/create_ota.py with the 32 KB dictionary boot2's
# decoder takes. --chip-variant IPRO7AI is the Reef's variant; the tool also
# records that variant's flash_pin in its eflash_loader_cfg.ini, so pass the
# right --chip-variant the next time you flash a different board.
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
APP=$(dirname "$HERE")
SDK=$(cd "$APP/../../../.." && pwd)
BIN=$APP/build_reef/build_out/ipro_ble_remote_IPRO7.bin
OUT=${1:-$APP/build_reef/build_out/ipro_ble_remote_reef.xz.ota}

case "$(uname -s)" in
    Darwin)               TOOL_NAME=ipro_iot_tool_macos ;;
    MINGW*|MSYS*|CYGWIN*) TOOL_NAME=ipro_iot_tool.exe ;;
    *)                    TOOL_NAME=ipro_iot_tool_linux ;;
esac
TOOL=${IPRO_FLASH_TOOL:-$(command -v "$TOOL_NAME" || true)}
if [ -z "$TOOL" ] || [ ! -f "$TOOL" ]; then
    echo "IntelProCube tool not found: set IPRO_FLASH_TOOL to <IntelProCube dir>/$TOOL_NAME" >&2
    exit 1
fi
TOOL=$(cd "$(dirname "$TOOL")" && pwd)/$(basename "$TOOL")
CUBE=$(dirname "$TOOL")
PYTHON=$(command -v python3 || command -v python || true)
[ -n "$PYTHON" ] || { echo "python3 not found" >&2; exit 1; }
[ -f "$BIN" ] || { echo "build first: make -C apps/bluetooth/ble/ipro_ble_remote DEFCONFIG=defconfig_reef" >&2; exit 1; }

# The tool exits 0 even when the build fails; a finished build logs its
# partition check of whole_img.bin.
LOG=$(mktemp)
trap 'rm -f "$LOG"' EXIT
(cd "$CUBE" && "$TOOL" --chipname ipro7 --chip-variant IPRO7AI --build \
    --boot2 "$CUBE/chips/ipro7/builtin_imgs/boot2_v0.5.2/ipro_boot2_IPRO7_v0.5.2.bin" \
    --pt "$APP/partition_cfg_2M_remote_ab.toml" --firmware "$BIN") > "$LOG" 2>&1
if ! grep -q "PT Check: whole_img.bin" "$LOG"; then
    cat "$LOG" >&2
    echo "IntelProCube did not build whole_img.bin" >&2
    exit 1
fi
"$PYTHON" "$SDK/tools/create_ota.py" "$CUBE/chips/ipro7/img_create_iot/whole_img.bin" -o "$OUT" \
    --fw-ver "$(date +%Y%m%d%H%M)"

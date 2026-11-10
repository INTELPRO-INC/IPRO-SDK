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
# The tool's --build needs a boot2 image: the highest
# chips/ipro7/builtin_imgs/boot2_v*/ipro_boot2_IPRO7_v*.bin it ships (public
# IntelProCube 0.7.x: v0.6.0), or IPRO_BOOT2=<file>. The script prints the one
# it uses. boot2 is not in the package: v0.5.2 and v0.6.0 give the same payload.
#
# The package payload is the tool's whole_img.bin (the firmware behind its
# 4 KB IPNP bootheader, built for the partition_cfg_2M_remote_ab.toml layout),
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

# Highest boot2 version: "x.y.z path" lines sorted numerically by component.
if [ -n "$IPRO_BOOT2" ]; then
    BOOT2=$IPRO_BOOT2
else
    BOOT2=$(for f in "$CUBE"/chips/ipro7/builtin_imgs/boot2_v*/ipro_boot2_IPRO7_v*.bin; do
                [ -f "$f" ] || continue
                v=${f##*_v}
                echo "${v%.bin} $f"
            done | sort -t. -k1,1n -k2,2n -k3,3n | tail -n 1 | cut -d' ' -f2-)
fi
if [ -z "$BOOT2" ] || [ ! -f "$BOOT2" ]; then
    echo "boot2 image not found: ${IPRO_BOOT2:-$CUBE/chips/ipro7/builtin_imgs/boot2_v*/ipro_boot2_IPRO7_v*.bin}" >&2
    echo "set IPRO_BOOT2 to an ipro_boot2_IPRO7_v*.bin" >&2
    exit 1
fi
BOOT2=$(cd "$(dirname "$BOOT2")" && pwd)/$(basename "$BOOT2")
echo "boot2: $BOOT2"

# The tool exits 0 even when the build fails; a finished build logs its
# partition check of whole_img.bin.
LOG=$(mktemp)
trap 'rm -f "$LOG"' EXIT
(cd "$CUBE" && "$TOOL" --chipname ipro7 --chip-variant IPRO7AI --build \
    --boot2 "$BOOT2" \
    --pt "$APP/partition_cfg_2M_remote_ab.toml" --firmware "$BIN") > "$LOG" 2>&1
if ! grep -q "PT Check: whole_img.bin" "$LOG"; then
    cat "$LOG" >&2
    echo "IntelProCube did not build whole_img.bin" >&2
    exit 1
fi
"$PYTHON" "$SDK/tools/create_ota.py" "$CUBE/chips/ipro7/img_create_iot/whole_img.bin" -o "$OUT" \
    --fw-ver "$(date +%Y%m%d%H%M)"

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

# shellcheck shell=bash
# Sourced by flash.sh and monitor.sh: the flash tool and the serial port, from
# the environment or found on this machine - the same rules as the
# `west flash` runner (zephyr/scripts/runners/ipro.py).

# ipro_find_flash_tool: $IPRO_FLASH_TOOL (a path or a command name), else the
# IntelProCube CLI on PATH.
ipro_find_flash_tool() {
    if [[ -n "${IPRO_FLASH_TOOL:-}" ]]; then
        if [[ "${IPRO_FLASH_TOOL}" != */* ]] && command -v "${IPRO_FLASH_TOOL}" >/dev/null 2>&1; then
            command -v "${IPRO_FLASH_TOOL}"
        else
            echo "${IPRO_FLASH_TOOL}"
        fi
        return 0
    fi
    local name
    for name in ipro_iot_tool_macos ipro_iot_tool_linux ipro_iot_tool; do
        if command -v "${name}" >/dev/null 2>&1; then
            command -v "${name}"
            return 0
        fi
    done
    echo "no ipro_iot_tool found: set IPRO_FLASH_TOOL or put the IntelProCube CLI on PATH" >&2
    return 1
}

# ipro_find_port: $IPRO_SERIAL_PORT, else the only USB serial port present.
ipro_find_port() {
    if [[ -n "${IPRO_SERIAL_PORT:-}" ]]; then
        echo "${IPRO_SERIAL_PORT}"
        return 0
    fi
    local ports=() p
    for p in /dev/cu.usbmodem* /dev/cu.usbserial* /dev/ttyACM* /dev/ttyUSB*; do
        [[ -e "${p}" ]] && ports+=("${p}")
    done
    if [[ ${#ports[@]} -eq 1 ]]; then
        echo "${ports[0]}"
        return 0
    fi
    if [[ ${#ports[@]} -eq 0 ]]; then
        echo "no serial port found: pass --port or set IPRO_SERIAL_PORT" >&2
    else
        echo "several serial ports, pass --port or set IPRO_SERIAL_PORT: ${ports[*]}" >&2
    fi
    return 1
}

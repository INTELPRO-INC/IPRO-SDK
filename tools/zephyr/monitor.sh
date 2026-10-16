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

# Open the console of an IPRO board running Zephyr (2 Mbaud on UART0).
#
#   tools/zephyr/monitor.sh [--port /dev/cu.usbserialXXXX] [--baud 2000000] [--timeout S]
set -euo pipefail
SDK_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
# shellcheck source=common.sh
source "$(dirname "$0")/common.sh"
PORT=""
BAUD=2000000
TIMEOUT=0
while [[ $# -gt 0 ]]; do
    case "$1" in
        --port) PORT="$2"; shift 2 ;;
        --baud) BAUD="$2"; shift 2 ;;
        --timeout) TIMEOUT="$2"; shift 2 ;;
        *) shift ;;
    esac
done
[[ -n "${PORT}" ]] || PORT="$(ipro_find_port)"
exec python3 "${SDK_ROOT}/tools/serial_monitor.py" "${PORT}" --baud "${BAUD}" --timeout "${TIMEOUT}"

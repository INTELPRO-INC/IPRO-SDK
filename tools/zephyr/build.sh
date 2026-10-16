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

# Build a Zephyr application for an IPRO board.
#
#   tools/zephyr/build.sh [-b BOARD] [-d BUILD_DIR] [-p] APP [-- extra cmake args]
#
#   APP        path to the application (absolute, relative to the workspace,
#              or one of the SDK samples under zephyr/samples/<name>)
#   -b BOARD   ipro7_evb (default) | ipro7ai_evk
#   -p         pristine build
#
# Examples:
#   tools/zephyr/build.sh zephyr/samples/hello_world
#   tools/zephyr/build.sh -b ipro7ai_evk periph_smoke
#   tools/zephyr/build.sh zephyr/samples/bluetooth/peripheral_hr -- \
#       -DEXTRA_CONF_FILE=$PWD/zephyr/samples/bt_peripheral_hr/ipro7.conf \
#       -DEXTRA_DTC_OVERLAY_FILE=$PWD/zephyr/samples/bt_peripheral_hr/ipro7.overlay
set -euo pipefail

SDK_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
WS="${IPRO_ZEPHYR_WS:-${SDK_ROOT}/zephyr-workspace}"
BOARD="ipro7_evb"
BUILD_DIR=""
PRISTINE=""
APP=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        -b) BOARD="$2"; shift 2 ;;
        -d) BUILD_DIR="$2"; shift 2 ;;
        -p) PRISTINE="-p always"; shift ;;
        --) shift; break ;;
        -h|--help) sed -n '2,17p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) APP="$1"; shift ;;
    esac
done

[[ -n "${APP}" ]] || { echo "usage: $0 [-b BOARD] APP" >&2; exit 1; }
[[ -f "${WS}/env.sh" ]] || { echo "workspace missing, run tools/zephyr/bootstrap.sh" >&2; exit 1; }
# shellcheck disable=SC1091
source "${WS}/env.sh"

if [[ -d "${SDK_ROOT}/zephyr/samples/${APP}" ]]; then
    APP="${SDK_ROOT}/zephyr/samples/${APP}"
elif [[ -d "${APP}" ]]; then
    APP="$(cd "${APP}" && pwd)"          # absolute: west runs from the workspace
elif [[ -d "${WS}/${APP}" ]]; then
    APP="${WS}/${APP}"
fi
[[ -f "${APP}/CMakeLists.txt" ]] || { echo "not a Zephyr app: ${APP}" >&2; exit 1; }
[[ -n "${BUILD_DIR}" ]] || BUILD_DIR="${WS}/build/${BOARD}/$(basename "${APP}")"

cd "${WS}"
# shellcheck disable=SC2086
west build ${PRISTINE} -b "${BOARD}" -d "${BUILD_DIR}" "${APP}" -- "$@"
echo "[build] ${BUILD_DIR}/zephyr/zephyr.bin"

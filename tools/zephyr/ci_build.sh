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

# Build the Zephyr sample matrix the way CI does: pristine, with
# CONFIG_COMPILER_WARNINGS_AS_ERRORS=y (-Werror + linker --fatal-warnings),
# and print a size table. Exit code is non-zero if any entry fails.
#
#   tools/zephyr/ci_build.sh                 # full matrix, all boards
#   tools/zephyr/ci_build.sh -b ipro7_evb    # one board
#   tools/zephyr/ci_build.sh -q              # quick: hello_world + blinky only
#
# Requires a bootstrapped workspace (tools/zephyr/bootstrap.sh). Build
# directories go to <ws>/build/ci/<board>/<name>; the summary is also
# written to <ws>/build/ci/summary.md for CI artifacts.
set -uo pipefail

SDK_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
WS="${IPRO_ZEPHYR_WS:-${SDK_ROOT}/zephyr-workspace}"
BOARDS="ipro7_evb ipro7ai_evk ipro6s_evb"
QUICK=0

while [[ $# -gt 0 ]]; do
    case "$1" in
        -b) BOARDS="$2"; shift 2 ;;
        -q) QUICK=1; shift ;;
        -h|--help) sed -n '2,13p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) echo "unknown option $1" >&2; exit 1 ;;
    esac
done

[[ -f "${WS}/env.sh" ]] || { echo "workspace missing, run tools/zephyr/bootstrap.sh" >&2; exit 1; }
# shellcheck disable=SC1091
source "${WS}/env.sh"
cd "${WS}"

SDK_Z="${SDK_ROOT}/zephyr"
OUT="${WS}/build/ci"
mkdir -p "${OUT}"
SUMMARY="${OUT}/summary.md"
FAILED=0

# name | app path | extra cmake args | boards (empty = every board)
# IPRO6S has no SPI/I2C/PWM pins in its board file yet and no 802.15.4 / BLE
# controller integration, so the wireless and peripheral samples are IPRO7-only.
IPRO7_BOARDS="ipro7_evb ipro7ai_evk"
MATRIX=(
  "hello_world|zephyr/samples/hello_world||"
  "blinky|zephyr/samples/basic/blinky||"
)
if [[ "${QUICK}" == "0" ]]; then
# Upstream Zephyr samples with the port's overlays only: the port's own
# samples (periph_smoke, ieee802154_shell, mcuboot_app, wifi_*, LE audio...)
# carry a sample.yaml and are built by twister (test:zephyr_twister), so
# they are not repeated here.
MATRIX+=(
  "shell_module|zephyr/samples/subsys/shell/shell_module||"
  "blinky_pwm|zephyr/samples/basic/blinky_pwm|-DEXTRA_DTC_OVERLAY_FILE=${SDK_Z}/samples/periph_smoke/boards/@BOARD@.overlay|${IPRO7_BOARDS}"
  "bt_peripheral_hr|zephyr/samples/bluetooth/peripheral_hr|-DEXTRA_CONF_FILE=${SDK_Z}/samples/bt_peripheral_hr/ipro7.conf -DEXTRA_DTC_OVERLAY_FILE=${SDK_Z}/samples/bt_peripheral_hr/ipro7.overlay|${IPRO7_BOARDS}"
  "openthread_shell|zephyr/samples/net/openthread/shell|-DEXTRA_CONF_FILE=${SDK_Z}/samples/ieee802154_shell/ot.conf -DEXTRA_DTC_OVERLAY_FILE=${SDK_Z}/samples/ieee802154_shell/app.overlay|${IPRO7_BOARDS}"
)
fi

{
  echo "# Zephyr CI build matrix"
  echo
  echo "Zephyr $(git -C zephyr describe --tags --always 2>/dev/null), toolchain $(${CROSS_COMPILE}gcc -dumpversion), CONFIG_COMPILER_WARNINGS_AS_ERRORS=y"
  echo
  echo "| board | app | status | ROM | RAM |"
  echo "|---|---|---|---|---|"
} > "${SUMMARY}"

for BOARD in ${BOARDS}; do
  for entry in "${MATRIX[@]}"; do
    IFS='|' read -r NAME APP EXTRA ONLY <<< "${entry}"
    if [[ -n "${ONLY}" && " ${ONLY} " != *" ${BOARD} "* ]]; then
      continue
    fi
    EXTRA="${EXTRA//@BOARD@/${BOARD}}"
    WEST_OPTS=""
    if [[ "${EXTRA}" == *"@SYSBUILD@"* ]]; then
      WEST_OPTS="--sysbuild"
      EXTRA="${EXTRA//@SYSBUILD@/}"
    fi
    BD="${OUT}/${BOARD}/${NAME}"
    LOG="${BD}.log"
    mkdir -p "${OUT}/${BOARD}"
    printf "%-12s %-18s " "${BOARD}" "${NAME}"
    # shellcheck disable=SC2086
    if west build -p always -b "${BOARD}" ${WEST_OPTS} "${APP}" -d "${BD}" -- \
         -DCONFIG_COMPILER_WARNINGS_AS_ERRORS=y ${EXTRA} > "${LOG}" 2>&1; then
      # sysbuild prints one ROM/RAM pair per image in build order (not stable);
      # each pair is followed by "Generating files from <dir>/<image>/zephyr/zephyr.elf",
      # so keep the pair that belongs to the application image.
      read -r ROM RAM < <(awk -v app="/${NAME}/zephyr/zephyr.elf" '
        /^ *ROM:/ {rom=$2" "$3}
        /^ *RAM:/ {ram=$2" "$3}
        /^Generating files from / { if (index($0, app)) {print rom, ram; exit} }
        ' "${LOG}")
      if [[ -z "${ROM}" ]]; then
        ROM=$(grep -E "^ *ROM:" "${LOG}" | awk '{print $2" "$3}')
        RAM=$(grep -E "^ *RAM:" "${LOG}" | awk '{print $2" "$3}')
      fi
      echo "BUILD PASS  ROM ${ROM}  RAM ${RAM}"
      echo "| ${BOARD} | ${NAME} | BUILD PASS | ${ROM} | ${RAM} |" >> "${SUMMARY}"
    else
      echo "FAILED (see ${LOG})"
      # "error:" with the colon: a bare "error" matches every picolibc
      # strerror*.c / regerror.c compile line and hides the real message.
      grep -E "^FAILED:|error:|Error at|undefined reference" "${LOG}" | head -8 | sed 's/^/    /'
      echo "| ${BOARD} | ${NAME} | **FAILED** | | |" >> "${SUMMARY}"
      FAILED=$((FAILED + 1))
    fi
  done
done

echo
cat "${SUMMARY}"
echo
if [[ "${FAILED}" != "0" ]]; then
  echo "${FAILED} build(s) failed"
  exit 1
fi
echo "all builds passed"

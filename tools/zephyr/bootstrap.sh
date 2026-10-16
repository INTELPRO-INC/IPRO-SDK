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

# Create the Zephyr workspace for the IPRO SDK.
#
#   tools/zephyr/bootstrap.sh            # <sdk>/zephyr-workspace (gitignored)
#   tools/zephyr/bootstrap.sh -w DIR     # custom workspace directory
#
# Layout produced (Zephyr "T2" topology with the SDK as manifest repository):
#   <ws>/.venv                 python venv with west + Zephyr requirements
#   <ws>/zephyr                Zephyr v4.4.0 (pinned in <sdk>/west.yml)
#   <ws>/modules/...           only the modules west.yml allow-lists
#   <ws>/env.sh                `source` this before west build
#
# The SDK itself is the Zephyr module (zephyr/module.yml); west sees it as
# the manifest project at "..", so no symlinks or copies are made.
set -euo pipefail

SDK_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
WS="${SDK_ROOT}/zephyr-workspace"
TOOLCHAIN="${IPRO_TOOLCHAIN:-/opt/toolchain/riscv_ipro7}"
NARROW=1

while [[ $# -gt 0 ]]; do
    case "$1" in
        -w|--workspace) WS="$2"; shift 2 ;;
        --full)         NARROW=0; shift ;;
        -h|--help)      sed -n '2,16p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) echo "unknown option $1" >&2; exit 1 ;;
    esac
done

echo "[bootstrap] workspace: ${WS}"
mkdir -p "${WS}"
cd "${WS}"

# Create .venv with pip inside. `python3 -m venv` needs ensurepip, which
# Debian/Ubuntu ship separately (python3-venv) and the CI shell runner
# lacked; fall back to virtualenv, then to a pip-less venv bootstrapped
# with get-pip.py.
make_venv() {
    rm -rf .venv
    if python3 -m venv .venv 2>/dev/null && [[ -x .venv/bin/pip ]]; then
        return 0
    fi
    rm -rf .venv
    if python3 -m virtualenv .venv >/dev/null 2>&1 && [[ -x .venv/bin/pip ]]; then
        echo "[bootstrap] venv via virtualenv (python3-venv not installed)"
        return 0
    fi
    rm -rf .venv
    echo "[bootstrap] venv without ensurepip, fetching get-pip.py"
    python3 -m venv --without-pip .venv
    local getpip="${WS}/get-pip.py"
    curl -fsSL https://bootstrap.pypa.io/get-pip.py -o "${getpip}" ||
        wget -q -O "${getpip}" https://bootstrap.pypa.io/get-pip.py
    .venv/bin/python "${getpip}" -q
    rm -f "${getpip}"
    [[ -x .venv/bin/pip ]]
}

if [[ ! -x .venv/bin/west ]]; then
    echo "[bootstrap] creating python venv"
    make_venv || { echo "[bootstrap] could not create a venv with pip (install python3-venv or virtualenv)" >&2; exit 1; }
    .venv/bin/pip install -q --upgrade pip west
fi
export PATH="${WS}/.venv/bin:${PATH}"

if [[ ! -d .west ]]; then
    echo "[bootstrap] initialising west (manifest: ${SDK_ROOT}/west.yml)"
    mkdir -p .west
    # Relative path from the workspace to the SDK root.
    REL="$(python3 -c "import os,sys; print(os.path.relpath(sys.argv[1], sys.argv[2]))" "${SDK_ROOT}" "${WS}")"
    cat > .west/config <<CFG
[manifest]
path = ${REL}
file = west.yml

[zephyr]
base = zephyr
CFG
fi

# The SDK's own changes to the pinned modules (zephyr/patches.yml, applied with
# `west patch apply` below). Files they touched are listed in a stamp; they go
# back to the pinned revision before `west update`, together with every file
# the current patches touch, so a pin bump, an edited, added or removed patch
# and a tree patched by hand all start from pristine files. Local edits to
# those files are discarded.
PATCH_DIR="${SDK_ROOT}/zephyr/patches"
PATCH_STAMP="${WS}/.sdk-zephyr-patches"
patched_files() {
    local p
    for p in "${PATCH_DIR}"/zephyr/*.patch; do
        [[ -f "$p" ]] && sed -n 's|^+++ b/||p' "$p"
    done
}
if [[ -d zephyr/.git ]]; then
    { [[ -f "${PATCH_STAMP}" ]] && cat "${PATCH_STAMP}"; patched_files; } | sort -u | while read -r f; do
        [[ -n "$f" ]] && git -C zephyr checkout -q -- "$f" 2>/dev/null || true
    done
    rm -f "${PATCH_STAMP}"
fi

echo "[bootstrap] fetching Zephyr + allow-listed modules"
if [[ "${NARROW}" == "1" ]]; then
    west update --narrow --fetch-opt=--depth=1
else
    west update
fi

if [[ -f "${SDK_ROOT}/zephyr/patches.yml" ]]; then
    echo "[bootstrap] applying zephyr/patches.yml"
    west patch apply
    patched_files | sort -u > "${PATCH_STAMP}"
fi

echo "[bootstrap] installing Zephyr python requirements"
.venv/bin/pip install -q -r zephyr/scripts/requirements-base.txt \
    -r zephyr/scripts/requirements-run-test.txt \
    -r zephyr/scripts/requirements-build-test.txt \
    -r bootloader/mcuboot/scripts/requirements.txt  # twister, imgtool (MCUboot signing)

cat > env.sh <<ENV
# Source me: Zephyr build environment for the IPRO SDK workspace.
export ZEPHYR_TOOLCHAIN_VARIANT=cross-compile
export CROSS_COMPILE=${TOOLCHAIN}/bin/riscv64-unknown-elf-
export ZEPHYR_BASE="${WS}/zephyr"
export PATH="${WS}/.venv/bin:\$PATH"
ENV

echo
echo "[bootstrap] done. Next:"
echo "  source ${WS}/env.sh"
echo "  west build -b ipro7_evb zephyr/samples/hello_world"

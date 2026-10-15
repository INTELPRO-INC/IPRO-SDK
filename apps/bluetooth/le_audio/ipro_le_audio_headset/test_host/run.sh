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

# Host tests for the headset's DAC noise shaper. No target toolchain needed.
set -e
cd "$(dirname "$0")"
OUT="${TMPDIR:-/tmp}/ipro_le_audio_headset_test_dac_ns"
cc -std=c99 -O2 -Wall -Wextra -Werror -I../src -o "$OUT" test_dac_ns.c ../src/dac_ns.c -lm
"$OUT"

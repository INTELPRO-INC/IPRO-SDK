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

# Host tests for the launcher's pure-logic pieces - no board, no toolchain.
#
#   sh apps/platform/ipro7ai_evk_launcher/test_host/run.sh
set -e
cd "$(dirname "$0")/.."
OUT="${TMPDIR:-/tmp}"
CFLAGS="-std=c11 -Wall -Wextra -Werror -O1 -I src -I test_host"

cc $CFLAGS -o "$OUT/launcher_input_test" test_host/test_input_host.c src/input.c
"$OUT/launcher_input_test"

cc $CFLAGS -o "$OUT/launcher_softmax_test" test_host/test_softmax_host.c src/softmax_q.c -lm
"$OUT/launcher_softmax_test"

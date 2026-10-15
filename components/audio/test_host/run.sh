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

# Host tests for components/audio's pure-logic pieces - no board, no toolchain.
#
#   sh components/audio/test_host/run.sh
set -e
cd "$(dirname "$0")/.."
OUT="${TMPDIR:-/tmp}"
CFLAGS="-std=c11 -Wall -Wextra -Werror -O1 -I include -I test_host"

cc $CFLAGS -o "$OUT/audio_asrc_test" test_host/test_audio_asrc_host.c src/audio_asrc.c src/audio_asrc_coef.c -lm
"$OUT/audio_asrc_test"

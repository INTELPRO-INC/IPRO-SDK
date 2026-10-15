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

# Host tests for the IPRO7AI EVK board library - no board, no toolchain.
#
#   sh components/board/ipro7ai_evk/test_host/run.sh
#
# Only the pure-logic pieces run here (integer PCM math, text layout). The
# drivers need the silicon; the resampler moved to components/audio.
set -e
cd "$(dirname "$0")/.."
OUT="${TMPDIR:-/tmp}"
CFLAGS="-std=c11 -Wall -Wextra -Werror -O1 -I include -I test_host"

cc $CFLAGS -o "$OUT/evk_pcm_test" test_host/test_evk_pcm_host.c src/evk_pcm.c -lm
"$OUT/evk_pcm_test"

cc $CFLAGS -o "$OUT/evk_gfx_test" test_host/test_evk_gfx_host.c src/evk_gfx.c src/font5x7.c test_host/st7789p3_stub.c
"$OUT/evk_gfx_test"

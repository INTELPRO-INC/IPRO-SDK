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

# Host tests for the KW307 radar component - no board, no toolchain.
#
#   sh components/radar/test_host/run.sh
#
# The protocol vectors come from the vendor's own printed frames, so a pass
# means our framing matches Keywave's rather than merely being self-consistent.
set -e
cd "$(dirname "$0")/.."
OUT="${TMPDIR:-/tmp}"
CFLAGS="-std=c11 -Wall -Wextra -Werror -O1 -I include -I test_host"

cc $CFLAGS -o "$OUT/kw307_proto_test" test_host/test_kw307_proto_host.c src/kw307_proto.c
"$OUT/kw307_proto_test"

cc $CFLAGS -o "$OUT/kw307_map_test" test_host/test_map_host.c \
   src/kw307_history.c src/kw307_map.c src/kw307_proto.c
"$OUT/kw307_map_test"

# The spectrogram needs the DSP calls; test_host/riscv_math.h stubs them with a
# reference DFT so OUR arithmetic - decimation, bin mapping, log compression -
# is what gets tested rather than the vendor's transform.
cc $CFLAGS -DCONFIG_NMSIS_DSP -o "$OUT/kw307_spectro_test" test_host/test_spectro_host.c \
   src/kw307_spectro.c src/kw307_history.c src/kw307_proto.c -lm
"$OUT/kw307_spectro_test"

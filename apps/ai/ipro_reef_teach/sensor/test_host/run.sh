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

# Host check: sensor_front.c == IPRO AI Studio's numpy front end (within one int8 step).
#   sh test_host/run.sh [ipro_ai_studio dir]
set -e
here=$(cd "$(dirname "$0")" && pwd)
studio=${1:-$HOME/Works/ipro_ai_studio}
out=$(mktemp -d)
pre="$here/../../../../../components/ai/preproc"
cc -O2 -Wall -Wextra -I"$pre/include" "$here/front_cli.c" "$pre/src/sensor_front.c" -lm -o "$out/front_cli"
"$studio/.venv/bin/python" "$here/check_front.py" "$out/front_cli" "$studio"

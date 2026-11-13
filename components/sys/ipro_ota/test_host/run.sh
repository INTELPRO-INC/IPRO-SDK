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

# Host tests for ipro_ota - no board, no toolchain.
#
#   sh components/sys/ipro_ota/test_host/run.sh
#
# The package header, the sink (with and without erase-as-written), a
# package checked in flash and the frames of OTA over UART, against a NOR flash in RAM and a software SHA-256.
# The signature code needs mbedTLS and is tested apart from this script.
set -e
cd "$(dirname "$0")/.."
OUT="${TMPDIR:-/tmp}"
CFLAGS="-std=c11 -Wall -Wextra -Werror -O1 -g -fsanitize=address,undefined -fno-common -I include -I test_host/stub"

cc $CFLAGS -o "$OUT/ipro_ota_test" test_host/test_ipro_ota_host.c src/ipro_ota_pkg.c src/ipro_ota.c src/ipro_ota_uart_frame.c test_host/stub/stubs.c
"$OUT/ipro_ota_test"

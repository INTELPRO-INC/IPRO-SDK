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

# Host tests of the ipro_llm engine against the PyTorch reference - no board,
# no RISC-V toolchain.
#
#   sh components/ipro_llm/test_host/run.sh <blob.iplm> <golden dir>
#
# Make the inputs with tools/ipro_llm/export/export_blob.py and make_golden.py
# (same checkpoint). A sanitizer build checks the tokenizer, the logits and the
# first 500 decodes; an optimised build then decodes every golden row.
set -e
BLOB="$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"
GOLD="$(cd "$2" && pwd)"
cd "$(dirname "$0")/.."
OUT="${TMPDIR:-/tmp}"
SRC="src/llm_math.c src/llm_cq.c src/llm_tok.c src/llm_model.c src/llm_grammar.c src/llm_engine.c src/llm_accel.c test_host/llm_host.c"
CFLAGS="-std=c11 -Wall -Wextra -Werror -Wshadow -Wconversion -Wno-sign-conversion -fno-math-errno -ffp-contract=off -I include"

cc $CFLAGS -O1 -g -fsanitize=address,undefined -fno-sanitize-recover=all -o "$OUT/llm_host_san" $SRC -lm
cc $CFLAGS -O2 -o "$OUT/llm_host" $SRC -lm

"$OUT/llm_host_san" "$BLOB" tok "$GOLD/tok.tsv"
"$OUT/llm_host_san" "$BLOB" logits "$GOLD/logits.bin"
head -n 500 "$GOLD/decode.tsv" > "$OUT/llm_decode_head.tsv"
"$OUT/llm_host_san" "$BLOB" decode "$OUT/llm_decode_head.tsv"
"$OUT/llm_host" "$BLOB" decode "$GOLD/decode.tsv"
# the board's int8 kernel: not bit-exact, judged on accuracy
"$OUT/llm_host_san" "$BLOB" decode "$OUT/llm_decode_head.tsv" int8 > /dev/null
"$OUT/llm_host" "$BLOB" decode "$GOLD/decode.tsv" int8
# the NPU's arithmetic for prompt and decode (reference accelerator); skipped without an NPUS section
"$OUT/llm_host_san" "$BLOB" decode "$OUT/llm_decode_head.tsv" accel > /dev/null
"$OUT/llm_host" "$BLOB" decode "$GOLD/decode.tsv" accel

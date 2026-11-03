#!/usr/bin/env python3
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

"""The --mfg payload for the combined Teach firmware: the mode sector, then the
voiceprint model in its slot (src/reef_mode.h) with the model store's record
at the end of the slot, committed, so the board finds the model CRC-checked
(components/ai/store/include/ipro_ai_store.h has the record). IntelProCube
writes it at mfg + 0x1000; the image and sensor slots after it are not
touched (IPRO AI Studio deploys those models over USB).

    python3 mfg_payload.py --voice model_b.spkb [--mode image|voice|sensor] -o reef_teach_mfg.bin
"""
from __future__ import annotations

import argparse
import struct
import sys
import zlib
from pathlib import Path

MODES = ("image", "voice", "sensor")
MAGIC = 0x444D5452                       # "RTMD"
SECTOR = 0x1000
VOICE_OFFSET, VOICE_SIZE = 0x2000, 0x70000   # from the start of mfg (CMakeLists.txt)
WRITTEN_AT = 0x1000                      # where the tool puts the payload
# the store's record (ipro_ai_store.h): magic, version, kind, seq, bytes, crc, format, hdr crc, commit
RECORD_MAGIC, RECORD_VERSION, KIND_VOICE, COMMIT = 0x524D5049, 1, 2, 0x544D4F43
SPKB_FORMAT = 1


def record(voice: bytes) -> bytes:
    head = struct.pack("<IHHIIII", RECORD_MAGIC, RECORD_VERSION, KIND_VOICE, 1, len(voice),
                       zlib.crc32(voice) & 0xFFFFFFFF, SPKB_FORMAT)
    return head + struct.pack("<II", zlib.crc32(head) & 0xFFFFFFFF, COMMIT)


def payload(voice: bytes, mode: str) -> bytes:
    if voice[:4] != b"SPKB":
        raise ValueError("not a voiceprint model (no SPKB header)")
    room = VOICE_SIZE - SECTOR                    # the slot's last sector is the record
    if len(voice) > room:
        raise ValueError(f"voice model {len(voice)} B is larger than its slot's {room} B")
    m = MODES.index(mode)
    sector = struct.pack("<III", MAGIC, m, ~m & 0xFFFFFFFF).ljust(SECTOR, b"\xff")
    gap = b"\xff" * (VOICE_OFFSET - WRITTEN_AT - SECTOR)
    pad = b"\xff" * (room - len(voice))
    return sector + gap + voice + pad + record(voice)


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--voice", type=Path, required=True, help="model_b.spkb")
    ap.add_argument("--mode", choices=MODES, default="image", help="the mode to boot into")
    ap.add_argument("-o", "--out", type=Path, default=Path("reef_teach_mfg.bin"))
    args = ap.parse_args(argv)
    try:
        data = payload(args.voice.read_bytes(), args.mode)
    except (OSError, ValueError) as exc:
        print(f"mfg_payload: {exc}", file=sys.stderr)
        return 1
    args.out.write_bytes(data)
    print(f"{args.out}: {len(data)} B (mode {args.mode}, voice model {args.voice.stat().st_size} B)")
    return 0


if __name__ == "__main__":
    sys.exit(main())

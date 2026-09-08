#!/usr/bin/env python3
"""
Copyright (c) 2026 IntelPro Technology Co., Ltd.
All rights reserved.

This software and associated documentation files (the "Software") are
proprietary and confidential to IntelPro Technology Co., Ltd..

Access to and use of this Software is limited exclusively to authorized 
customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
valid license or non-disclosure agreement.

Unauthorized copying, modification, distribution, or reverse engineering 
of this Software, via any medium, is strictly prohibited without 
prior written permission from IntelPro Technology Co., Ltd..

SPDX-License-Identifier: Proprietary
"""

"""Pack a video into the flat container the EVK player reads off the SD card.

The board has no video decoder and a 16 MHz SPI panel, so everything that can
be done ahead of time is done here: scaling to the panel, RGB565 conversion,
and byte order. What lands on the card is exactly what gets pushed to the
panel, so playback is a read and a blit with no per-pixel work in between.

Layout, little-endian:

    magic   "IPV1"      4
    width   u16         2
    height  u16         2
    fps     u16         2
    arate   u16         2   audio sample rate
    frames  u32         4
    then per frame: arate/fps mono s16le samples, then width*height rgb565be

Audio first in each chunk so the player can keep the DAC fed before spending
the frame time on the blit.
"""
import argparse, os, struct, subprocess, sys, tempfile

MAGIC = b"IPV1"


def probe_duration(path):
    out = subprocess.run(
        ["ffprobe", "-v", "error", "-show_entries", "format=duration",
         "-of", "default=nw=1:nk=1", path],
        capture_output=True, text=True, check=True).stdout.strip()
    return float(out)


def convert(src, dst, w, h, fps, arate):
    with tempfile.TemporaryDirectory() as tmp:
        vraw = os.path.join(tmp, "v.raw")
        araw = os.path.join(tmp, "a.raw")

        # Fit inside the panel and pad rather than crop - these are 4:3 clips
        # on a 4:3 panel, so the pad is normally zero, but a stray aspect ratio
        # should letterbox instead of losing the edges.
        vf = (f"scale={w}:{h}:force_original_aspect_ratio=decrease,"
              f"pad={w}:{h}:(ow-iw)/2:(oh-ih)/2,fps={fps}")
        subprocess.run(["ffmpeg", "-v", "error", "-y", "-i", src,
                        "-vf", vf, "-pix_fmt", "rgb565be",
                        "-f", "rawvideo", vraw], check=True)
        subprocess.run(["ffmpeg", "-v", "error", "-y", "-i", src,
                        "-vn", "-ar", str(arate), "-ac", "1",
                        "-f", "s16le", araw], check=True)

        fsz = w * h * 2
        asz = (arate // fps) * 2
        nfrm = os.path.getsize(vraw) // fsz

        with open(vraw, "rb") as vf_, open(araw, "rb") as af, open(dst, "wb") as out:
            out.write(MAGIC + struct.pack("<HHHHI", w, h, fps, arate, nfrm))
            for _ in range(nfrm):
                a = af.read(asz)
                out.write(a + b"\0" * (asz - len(a)))   # pad a short tail
                out.write(vf_.read(fsz))
        return nfrm, os.path.getsize(dst)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("inputs", nargs="+")
    ap.add_argument("-o", "--outdir", required=True)
    ap.add_argument("-W", "--width", type=int, default=320)
    ap.add_argument("-H", "--height", type=int, default=240)
    ap.add_argument("-f", "--fps", type=int, default=10)
    ap.add_argument("-a", "--arate", type=int, default=32000,
                    help="must be the DAC's native rate or the board resamples")
    a = ap.parse_args()

    os.makedirs(a.outdir, exist_ok=True)
    total = 0
    for i, src in enumerate(a.inputs):
        dst = os.path.join(a.outdir, f"{i}.ipv")
        n, sz = convert(src, dst, a.width, a.height, a.fps, a.arate)
        total += sz
        print(f"{os.path.basename(dst)}: {n:4d} frames, {sz/1e6:6.2f} MB, "
              f"{probe_duration(src):.1f} s")
    print(f"\ntotal {total/1e6:.1f} MB at {a.width}x{a.height} {a.fps} fps, "
          f"audio {a.arate} Hz mono")
    print(f"per frame: {a.width*a.height*2/1024:.0f} KB video + "
          f"{a.arate//a.fps*2/1024:.1f} KB audio "
          f"-> {(a.width*a.height*2 + a.arate//a.fps*2)*a.fps/1e6:.2f} MB/s off the card")


if __name__ == "__main__":
    sys.exit(main())

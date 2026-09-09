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

"""Convert any video into the GH7007 panel's .ghv format.

Usage:
    python3 video_convert.py input.mp4 output.ghv [--fps 15]

Pipeline: ffmpeg scales/letterboxes to 320x240 RGB565LE raw frames; each
pixel is then repacked into the panel's WIRE format so the firmware plays
back with zero per-pixel work:

  1. G/B field swap  (chip decodes R5-B6-G5, bench truth 2026-07-29)
  2. per-field bit rotation: R5 rotR3, B6 rotR3, G5 rotR2 (pre-rotation that
     cancels the chip's rotate-left consumption; `lcd rot 3 3 2` defaults)
  3. MSB-first byte order (big-endian u16 on the wire)

Header (16 B, little-endian): "GHV1" u16 w, u16 h, u16 fps, u16 flags, u32 frames.

Copy the .ghv onto a FAT32 SD card, then on the board:
    video play output.ghv 1
"""

import argparse
import glob
import os
import struct
import subprocess
import sys
import tempfile

W, H = 320, 240
FRAME_BYTES = W * H * 2
ROT_R, ROT_B, ROT_G = 3, 3, 2

# The GH7007 scaler stretches the 320x240 input plane to 1024x600 glass:
# horizontal 3.2x vs vertical 2.5x => each plane pixel displays 1.28x WIDE.
# Pre-squeeze so content shows with the correct aspect on glass.
PIXEL_AR = (1024 / 320) / (600 / 240)   # 1.28


def build_lut():
    """Native RGB565 -> wire-format big-endian u16, as bytes pairs."""
    lut = bytearray(65536 * 2)
    for c in range(65536):
        r = (c >> 11) & 0x1F
        g6 = (c >> 5) & 0x3F
        b5 = c & 0x1F
        # G/B swap: B widens 5->6 (replicate MSB), G narrows 6->5
        b6 = ((b5 << 1) | (b5 >> 4)) & 0x3F
        g5 = g6 >> 1
        # pre-rotate right so the chip's rotate-left cancels
        r = ((r >> ROT_R) | (r << (5 - ROT_R))) & 0x1F
        b6 = ((b6 >> ROT_B) | (b6 << (6 - ROT_B))) & 0x3F
        g5 = ((g5 >> ROT_G) | (g5 << (5 - ROT_G))) & 0x1F
        px = (r << 11) | (b6 << 5) | g5
        lut[c * 2] = px >> 8          # MSB first on the wire
        lut[c * 2 + 1] = px & 0xFF
    return bytes(lut)


def probe_size(path):
    try:
        out = subprocess.check_output(
            ["ffprobe", "-v", "error", "-select_streams", "v:0",
             "-show_entries", "stream=width,height", "-of", "csv=p=0", path])
        w, h = out.decode().strip().split("\n")[0].split(",")
        return int(w), int(h)
    except Exception:
        return None


def plane_fit(src_w, src_h):
    """Largest w<=320, h<=240 whose ON-GLASS aspect matches the source."""
    ratio = (src_w / src_h) / PIXEL_AR      # target w/h inside the plane
    w, h = W, int(round(W / ratio / 2)) * 2
    if h > H:
        h = H
        w = int(round(H * ratio / 2)) * 2
    return max(2, min(W, w)), max(2, min(H, h))


def fetch_url(url, workdir):
    """Download a video URL (YouTube etc.) via yt-dlp; returns the local path.
    Video-only stream capped at 480p — the panel is 320x240, and we drop audio
    anyway. Any container works (ffmpeg reads webm/mp4/mkv alike)."""
    out_tpl = os.path.join(workdir, "ytvid.%(ext)s")
    print(f"[convert] downloading via yt-dlp: {url}")
    subprocess.run(["yt-dlp", "-f", "bv*[height<=480]/bv*/b",
                    "--no-playlist", "-o", out_tpl, url], check=True)
    files = glob.glob(os.path.join(workdir, "ytvid.*"))
    if not files:
        sys.exit("[convert] yt-dlp produced no file")
    return files[0]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("input", help="video file OR a URL (YouTube etc.)")
    ap.add_argument("output")
    ap.add_argument("--fps", type=int, default=15)
    ap.add_argument("--start", default=None, help="start time, e.g. 0:30")
    ap.add_argument("--duration", default=None, help="clip length in seconds")
    ap.add_argument("--warm", type=float, default=0.0,
                    help="0..1 warm shift to counter the panel's cool/blue "
                         "white tint (raises R gamma, lowers B). Try 0.3")
    ap.add_argument("--sat", type=float, default=1.0,
                    help="saturation multiplier (compressed sources often "
                         "benefit from 1.1)")
    args = ap.parse_args()

    tmpdir = None
    src = args.input
    if src.startswith(("http://", "https://")):
        tmpdir = tempfile.mkdtemp(prefix="ghv_")
        src = fetch_url(src, tmpdir)

    size = probe_size(src)
    if size:
        fw, fh = plane_fit(*size)
        print(f"[convert] source {size[0]}x{size[1]} -> plane {fw}x{fh} "
              f"(glass pixel AR {PIXEL_AR:.2f} compensated)")
    else:
        fw, fh = W, H
        print("[convert] ffprobe failed — no aspect compensation")
    vf = f"fps={args.fps},scale={fw}:{fh}:flags=lanczos"
    if args.warm > 0 or args.sat != 1.0:
        gr = 1.0 + 0.25 * args.warm
        gb = 1.0 - 0.20 * args.warm
        vf += f",eq=gamma_r={gr:.3f}:gamma_b={gb:.3f}:saturation={args.sat:.3f}"
    vf += f",pad={W}:{H}:(ow-iw)/2:(oh-ih)/2:black" 
    cmd = ["ffmpeg", "-v", "error"]
    if args.start:
        cmd += ["-ss", args.start]
    cmd += ["-i", src]
    if args.duration:
        cmd += ["-t", args.duration]
    cmd += ["-vf", vf, "-pix_fmt", "rgb565le", "-f", "rawvideo", "-"]
    print(f"[convert] ffmpeg: {src} -> {W}x{H}@{args.fps} rgb565 ...")
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)

    lut = build_lut()
    frames = 0
    try:
        import numpy as np
        nlut = np.frombuffer(lut, dtype=np.uint16)  # LE u16 view of byte pairs
        use_np = True
    except ImportError:
        use_np = False
        print("[convert] numpy not found — pure-python fallback (slow)")

    with open(args.output, "wb") as out:
        out.write(struct.pack("<4sHHHHI", b"GHV1", W, H, args.fps, 0, 0))
        while True:
            raw = proc.stdout.read(FRAME_BYTES)
            if len(raw) < FRAME_BYTES:
                break
            if use_np:
                px = np.frombuffer(raw, dtype=np.uint16)
                out.write(nlut[px].tobytes())
            else:
                fb = bytearray(FRAME_BYTES)
                for i in range(0, FRAME_BYTES, 2):
                    c = raw[i] | (raw[i + 1] << 8)
                    fb[i] = lut[c * 2]
                    fb[i + 1] = lut[c * 2 + 1]
                out.write(fb)
            frames += 1
            if frames % 100 == 0:
                print(f"[convert] {frames} frames ...")

        out.seek(12)
        out.write(struct.pack("<I", frames))

    proc.wait()
    dur = frames / args.fps if args.fps else 0
    size_mb = (16 + frames * FRAME_BYTES) / 1e6
    print(f"[convert] done: {frames} frames ({dur:.1f}s) -> {args.output} "
          f"({size_mb:.1f} MB)")
    if proc.returncode != 0:
        sys.exit(proc.returncode)


if __name__ == "__main__":
    main()

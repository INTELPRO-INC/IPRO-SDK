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

"""Talk to ipro7_jpeg_decode over the console UART. See HELP below."""
import argparse
import base64
import io
import re
import subprocess
import sys
import time

import numpy as np
import serial
from PIL import Image

CHUNK = 135  # bytes per `jdec put` line -> 180 base64 characters, under the 256-byte shell line

HELP = """Talk to ipro7_jpeg_decode over the console UART.

  jdec_host.py -p PORT [-b BAUD] shell "jdec run all"
      send one shell command and print what comes back

  jdec_host.py -p PORT upload photo.jpg [--go 565|565be|888]
      send a JPEG with `jdec rx` / `jdec put` (base64, 135 bytes per line),
      print `jdec info rx`, optionally decode it with `jdec go`

  jdec_host.py -p PORT dump <n|name|rx> [--fmt 565|888] [--scale 0..3] -o out.png [--ref photo.jpg]
      run `jdec dump`, rebuild the picture from the base64 text and save a PNG;
      with --ref (full size only), compare against the same JPEG decoded on
      the PC by libjpeg (no chroma smoothing) and print PSNR

Board console: 921600 baud (.config for the IPRO7AI EVK, defconfig_reef for the
Reef). Needs pyserial, numpy, Pillow; --ref also needs djpeg.
"""


class Board:
    def __init__(self, port, baud):
        self.s = serial.Serial(port, baud, timeout=0.05)
        time.sleep(0.05)
        self.s.reset_input_buffer()

    def send(self, line):
        data = (line + "\r\n").encode()
        for i in range(0, len(data), 64):  # small slices: the console has a short RX ring
            self.s.write(data[i:i + 64])
            self.s.flush()
            time.sleep(0.002)

    def read_until(self, pattern, timeout):
        buf, end, rx = "", time.time() + timeout, re.compile(pattern)
        while time.time() < end:
            chunk = self.s.read(65536)
            if chunk:
                start = max(0, len(buf) - 400)  # only the new text plus a line of overlap
                buf += chunk.decode(errors="replace")
                m = rx.search(buf, start)
                if m:
                    return buf, m
        raise TimeoutError("no %r within %ss, got:\n%s" % (pattern, timeout, buf[-600:]))

    def command(self, line, pattern, timeout=10):
        self.s.reset_input_buffer()
        self.send(line)
        return self.read_until(pattern, timeout)


def do_shell(b, args):
    out, _ = b.command(args.command, args.until, args.timeout)
    sys.stdout.write(out)


def do_upload(b, args):
    data = open(args.file, "rb").read()
    b.command("jdec rx %d" % len(data), r"(?<!jdec )rx (\d+)\r?\n")
    for i in range(0, len(data), CHUNK):
        line = base64.b64encode(data[i:i + CHUNK]).decode()
        _, m = b.command("jdec put " + line, r"(?<!jdec )put (\d+)/(\d+)\r?\n|put: \w+")
        if not m.group(1) or int(m.group(1)) != min(len(data), i + CHUNK):
            sys.exit("upload failed at byte %d: %s" % (i, m.group(0)))
        print("\r%d / %d bytes" % (min(len(data), i + CHUNK), len(data)), end="", flush=True)
    print()
    out, _ = b.command("jdec info rx", r"probe -?\d+ \([^)]*\)[^\n]*\n(  [^\n]*\n)?", 5)
    sys.stdout.write(out)
    if args.go:
        out, _ = b.command("jdec go " + args.go, r"(PASS|FAIL)[^\n]*\n", 60)
        sys.stdout.write(out)


def psnr(a, b):
    mse = np.mean((a.astype(np.float64) - b.astype(np.float64)) ** 2)
    return float("inf") if mse == 0 else 10 * np.log10(255.0 ** 2 / mse)


def do_dump(b, args):
    t0 = time.time()
    out, m = b.command("jdec dump %s %s %d" % (args.source, args.fmt, args.scale),
                       r"-----END JDEC fnv=(\w+)-----|error[ :][^\r\n]*", args.timeout)
    if not m.group(1):
        sys.exit(m.group(0))
    head = re.search(r"-----BEGIN JDEC fmt=(\w+) w=(\d+) h=(\d+) bytes=(\d+)-----", out)
    fmt, w, h, nbytes = head.group(1), *map(int, head.groups()[1:])
    body = out[head.end():m.start()]
    raw = base64.b64decode("".join(re.findall(r"^[A-Za-z0-9+/=]+\r?$", body, re.M)))
    if len(raw) != nbytes:
        sys.exit("got %d bytes, expected %d" % (len(raw), nbytes))
    print("%s %dx%d (%d bytes) in %.1f s, fnv %s" % (fmt, w, h, nbytes, time.time() - t0, m.group(1)))

    if fmt == "rgb888":
        rgb = np.frombuffer(raw, np.uint8).reshape(h, w, 3)
    else:
        v = np.frombuffer(raw, "<u2").reshape(h, w).astype(np.uint32)
        r, g, bl = v >> 11, (v >> 5) & 0x3F, v & 0x1F
        rgb = np.stack([(r << 3) | (r >> 2), (g << 2) | (g >> 4), (bl << 3) | (bl >> 2)], 2).astype(np.uint8)
    Image.fromarray(rgb, "RGB").save(args.out)
    print("saved", args.out)

    if args.ref:
        ppm = subprocess.run(["djpeg", "-nosmooth", "-dct", "int", "-pnm", args.ref],
                             capture_output=True, check=True).stdout
        ref = np.asarray(Image.open(io.BytesIO(ppm)).convert("RGB"))
        if ref.shape != rgb.shape:
            sys.exit("reference is %s, board gave %s" % (ref.shape, rgb.shape))
        d = np.abs(ref.astype(int) - rgb.astype(int))
        print("vs libjpeg: PSNR %.2f dB, max diff %d, %.3f %% of samples differ by more than 2"
              % (psnr(ref, rgb), d.max(), 100.0 * np.mean(d > 2)))


def main():
    ap = argparse.ArgumentParser(description=HELP, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-p", "--port", required=True)
    ap.add_argument("-b", "--baud", type=int, default=921600)
    sub = ap.add_subparsers(dest="cmd", required=True)

    s = sub.add_parser("shell")
    s.add_argument("command")
    s.add_argument("--until", default=r"(ALL PASS|FAIL|PASS)[^\n]*\n")
    s.add_argument("--timeout", type=float, default=60)

    u = sub.add_parser("upload")
    u.add_argument("file")
    u.add_argument("--go", choices=["565", "565be", "888"])

    d = sub.add_parser("dump")
    d.add_argument("source")
    d.add_argument("--fmt", default="888", choices=["565", "888"])
    d.add_argument("--scale", type=int, default=0, choices=[0, 1, 2, 3])
    d.add_argument("-o", "--out", required=True)
    d.add_argument("--ref")
    d.add_argument("--timeout", type=float, default=120)

    args = ap.parse_args()
    b = Board(args.port, args.baud)
    {"shell": do_shell, "upload": do_upload, "dump": do_dump}[args.cmd](b, args)


if __name__ == "__main__":
    main()

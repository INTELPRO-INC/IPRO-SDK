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

"""Check jdec_rgb.c + tjpgd/ (TJpgDec) on the PC before going to the board.

Everything runs on a build with AddressSanitizer and UndefinedBehaviorSanitizer.
For every image in host/images/:
  * jdec_probe() must return the code the generator expects;
  * RGB888 output is compared with libjpeg (djpeg -nosmooth): PSNR and max difference;
  * the frame and the block-callback paths must give identical pixels, for
    RGB565, RGB565 big-endian and RGB888, at scale 1/1, 1/2, 1/4 and 1/8. They
    start from differently filled buffers, so this also checks the output size.
Then: API edge cases (test_jdec_core selftest), a decoder built with
JD_FORMAT 1, and MUTATIONS damaged copies of the test images (truncated,
bytes and segment lengths changed), each of which must end in an error code
or a clean decode, and on which host/jpeg_check.py must agree with jdec_probe().

Needs cc, djpeg (libjpeg-turbo), Pillow and numpy.
"""
import io
import os
import random
import re
import subprocess
import sys
import tempfile

import numpy as np
from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from make_test_jpegs import HERE, build_host_decoder, host_decode  # noqa: E402
from jpeg_check import check as pc_check  # noqa: E402

EXPECT = {"progressive_420": "-4", "cmyk": "-7", "truncated_420": "-3"}
MUTATIONS = 500
MIN_PSNR = 30.0  # also for JD_FORMAT 1, where RGB888 is expanded from 16 bits


def djpeg(path):
    out = subprocess.run(["djpeg", "-nosmooth", "-dct", "int", "-pnm", path],
                         capture_output=True, check=True).stdout
    return np.asarray(Image.open(io.BytesIO(out)).convert("RGB")).astype(np.int32)


def psnr(a, b):
    mse = np.mean((a - b) ** 2.0)
    return float("inf") if mse == 0 else 10 * np.log10(255.0 ** 2 / mse)


def mutate(rng, data):
    d = bytearray(data)
    kind = rng.randrange(4)
    if kind == 0:                                   # truncated
        return bytes(d[:rng.randrange(len(d))])
    if kind == 1:                                   # random bytes
        for _ in range(rng.randint(1, 16)):
            d[rng.randrange(len(d))] = rng.randrange(256)
    elif kind == 2:                                 # header bytes that matter to the parser
        for _ in range(8):
            d[rng.randrange(2, min(700, len(d)))] = rng.choice(
                [0xFF, 0x00, 0x01, 0x02, 0x11, 0x21, 0x22, 0xC0, 0xC4, 0xD8, 0xDA, 0xDB, 0xDD])
    else:                                           # segment length fields
        for i in range(2, min(len(d) - 3, 800)):
            if d[i] == 0xFF and d[i + 1] not in (0x00, 0xFF) and rng.random() < 0.3:
                d[i + 2], d[i + 3] = rng.randrange(256), rng.randrange(256)
    return bytes(d)


def fuzz(exe, tmp):
    rng = random.Random(2026)
    srcs = [open(os.path.join(HERE, "images", f), "rb").read()
            for f in sorted(os.listdir(os.path.join(HERE, "images"))) if f.endswith(".jpg") and "hd_" not in f]
    path, codes = os.path.join(tmp, "mutated.jpg"), {}
    for n in range(MUTATIONS):
        data = mutate(rng, rng.choice(srcs))
        open(path, "wb").write(data)
        pc_ok = not pc_check(data)[0]
        for fmt, scale in (("565", 0), ("888", rng.randrange(4))):
            r = subprocess.run([exe, path, fmt, str(scale)], capture_output=True, text=True)
            kv = dict(re.findall(r'(\w+)=("[^"]*"|\S+)', r.stdout))
            # probe=0: jdec_probe() accepted the file, so the decoder must not reject its
            # header (SOF / TABLES) afterwards; DATA is fine, the image data is damaged.
            bad = r.returncode != 0 or "Sanitizer" in r.stderr or "runtime error" in r.stderr or (
                kv.get("ret") == "0" and kv.get("blocks") != "same") or (
                kv.get("probe") == "0" and kv.get("ret") in ("-5", "-9")) or (
                (kv.get("probe") == "0") != pc_ok)
            if bad:
                keep = os.path.join(tmp, "fuzz_fail_%d.jpg" % n)
                os.replace(path, keep)
                print("FAIL fuzz case %d (%s, scale %d): %s" % (n, fmt, scale, (r.stderr or r.stdout)[:400]))
                print("     input kept at %s" % keep)
                return 1
            codes[kv.get("ret", "?")] = codes.get(kv.get("ret", "?"), 0) + 1
    print("ok   fuzz (jpeg_check.py agrees with jdec_probe): %d damaged files, %d decodes, results %s" % (
        MUTATIONS, 2 * MUTATIONS, " ".join("%s:%d" % kv for kv in sorted(codes.items()))))
    return 0


def main():
    tmp = tempfile.mkdtemp(prefix="jdec_host_")
    exe = build_host_decoder(tmp, sanitize=True)
    failures = 0
    names = sorted(f[:-4] for f in os.listdir(os.path.join(HERE, "images")) if f.endswith(".jpg"))
    for name in names:
        src = os.path.join(HERE, "images", name + ".jpg")
        raw = os.path.join(tmp, name + ".rgb")
        kv = host_decode(exe, src, "888", 0, raw)
        want = EXPECT.get(name, "0")
        ok = kv["ret"] == want
        detail = kv["err"]
        if ok and want == "0":
            w, h = int(kv["w"]), int(kv["h"])
            got = np.fromfile(raw, np.uint8).reshape(h, w, 3).astype(np.int32)
            ref = djpeg(src)
            p, mx = psnr(got, ref), int(np.abs(got - ref).max())
            paths = []
            for fmt in ("565", "565be", "888"):
                for scale in range(4):
                    r = host_decode(exe, src, fmt, scale)
                    paths.append(r["ret"] == "0" and r["blocks"] == "same")
            ok = p >= MIN_PSNR and all(paths)
            detail = "%sx%s %-4s app %5s B  std_huff %s  PSNR %.1f dB (max %d)  frame==blocks %d/%d" % (
                w, h, kv["samp"], kv["app"], kv["std_huff"], p, mx, sum(paths), len(paths))
        print("%-4s %-16s ret=%-3s %s" % ("ok" if ok else "FAIL", name, kv["ret"], detail))
        failures += not ok

    r = subprocess.run([exe, "selftest", os.path.join(HERE, "images", "basic_420.jpg")],
                       capture_output=True, text=True)
    ok = r.returncode == 0
    print("%-4s selftest: %s" % ("ok" if ok else "FAIL", (r.stdout + r.stderr).strip()[-300:]))
    failures += not ok

    failures += fuzz(exe, tmp)

    # A decoder built with JD_FORMAT 1 (16-bit, as components/3rdparty/tjpgdec) must pass too.
    exe0 = build_host_decoder(tmp, jd_format=1)
    for name in ("basic_420", "full_444", "gray", "nodht_422", "odd_100x75"):
        src = os.path.join(HERE, "images", name + ".jpg")
        raw = os.path.join(tmp, name + "_fmt1.rgb")
        kv = host_decode(exe0, src, "888", 0, raw)
        w, h = int(kv["w"]), int(kv["h"])
        got = np.fromfile(raw, np.uint8).reshape(h, w, 3).astype(np.int32)
        p = psnr(got, djpeg(src))
        same = all(host_decode(exe0, src, f, s)["blocks"] == "same" for f in ("565", "888") for s in range(4))
        ok = kv["ret"] == "0" and p >= MIN_PSNR and same
        print("%-4s JD_FORMAT 1 %-16s PSNR %.1f dB  frame==blocks %s" % ("ok" if ok else "FAIL", name, p, same))
        failures += not ok
    print("host tests: %s" % ("PASS" if failures == 0 else "%d FAILED" % failures))
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())

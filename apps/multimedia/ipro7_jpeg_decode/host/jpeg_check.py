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

"""Check on the PC whether a JPEG can be decoded on IPRO7 (jdec_rgb + TJpgDec), and fix it if not. See HELP below."""
import argparse
import io
import shutil
import subprocess
import sys

SOF_NAMES = {0xC0: "baseline", 0xC1: "extended sequential", 0xC2: "progressive", 0xC3: "lossless",
             0xC9: "arithmetic", 0xCA: "arithmetic progressive", 0xCB: "arithmetic lossless"}
SZBUF = 512         # TJpgDec loads SOF, DHT, DQT, DRI and SOS segments whole into this buffer
MS_PER_MPIX = 650   # IPRO7 at 192 MHz, decoder built -O2 (about 1250 at -Os)

HELP = """Check on the PC whether a JPEG can be decoded on IPRO7 (jdec_rgb + TJpgDec), and fix it if not.

  python3 jpeg_check.py photo.jpg [more.jpg ...]
  python3 jpeg_check.py photo.jpg --fix photo_ipro7.jpg [--quality 90]

The rules are the ones jdec_probe() applies on the board: baseline (SOF0)
Huffman JPEG, 8-bit samples and quantisation tables, grey or YCbCr with
4:4:4 / 4:2:2 / 4:2:0 sampling, one scan, Huffman tables 0 (Y) and 1 (Cb/Cr),
header segments of at most 512 bytes, an EOI at the end of the scan.
--fix converts anything else: losslessly with jpegtran when only the coding is
the problem (progressive, table layout), otherwise by re-encoding with Pillow
(baseline, 4:2:0, rotated upright from the EXIF orientation).
Needs Python 3; --fix needs Pillow, and uses jpegtran (libjpeg-turbo) if found.
"""


class Broken(Exception):
    pass


def be16(b, i):
    if i + 2 > len(b):
        raise Broken("segment too short")
    return b[i] << 8 | b[i + 1]


def find_eoi(data, i):
    """Walk the entropy-coded data like jdec_rgb.c: None if EOI, else the problem."""
    while True:
        i = data.find(b"\xFF", i)
        if i < 0 or i + 1 >= len(data):
            return "truncated: no EOI marker after the image data"
        b = data[i + 1]
        if b == 0x00 or 0xD0 <= b <= 0xD7:
            i += 2
        elif b == 0xFF:
            i += 1
        elif b == 0xD9:
            return None
        else:
            return "another scan or marker 0x%02X inside the image data (multi-scan file)" % b


def check(data):
    """Return (problems, notes, info); info["lossless_fix"]: jpegtran alone can fix the problems."""
    problems, notes, info = [], [], {"lossless_fix": True}

    def lossy(msg):
        problems.append(msg)
        info["lossless_fix"] = False

    try:
        check_inner(data, problems, notes, info, lossy)
    except Broken as e:
        lossy("broken file: %s" % e)
    return problems, notes, info


def check_inner(data, problems, notes, info, lossy):
    if len(data) < 4 or data[:2] != b"\xFF\xD8":
        raise Broken("no SOI marker, not a JPEG")
    i, comps, sel, qt, ht = 2, [], {}, set(), set()
    while True:
        if i + 4 > len(data):
            lossy("truncated: the file ends inside the header")
            return
        if data[i] != 0xFF:
            raise Broken("no marker at byte %d" % i)
        m = data[i + 1]
        if m == 0xD9:
            lossy("EOI before any image data")
            return
        if m in (0xFF, 0xD8, 0x01, 0x00) or 0xD0 <= m <= 0xD7:
            raise Broken("fill bytes or marker 0x%02X without a length in the header (byte %d)" % (m, i))
        n = be16(data, i + 2)
        if n <= 2:
            raise Broken("empty segment 0x%02X at byte %d" % (m, i))
        if i + 2 + n > len(data):
            lossy("truncated: segment 0x%02X runs past the end" % m)
            return
        s, n = data[i + 4:i + 2 + n], n - 2
        if m in (0xC0, 0xC4, 0xDB, 0xDD, 0xDA) and n > SZBUF:
            problems.append("segment 0x%02X is %d bytes, the decoder takes at most %d" % (m, n, SZBUF))
        if 0xE0 <= m <= 0xEF or m == 0xFE:
            notes.append("%s segment, %d bytes (skipped by the decoder)"
                         % ("COM" if m == 0xFE else "APP%d" % (m - 0xE0), n + 4))
        elif m == 0xDB:
            k = 0
            while k < n:
                size = 129 if s[k] >> 4 else 65
                if n - k < size or (s[k] & 15) > 3:
                    raise Broken("bad DQT segment")
                if s[k] >> 4:
                    lossy("16-bit quantisation table")
                qt.add(s[k] & 15)
                k += size
        elif m == 0xC4:
            k = 0
            while k < n:
                if n - k < 17 or (s[k] >> 4) > 1 or (s[k] & 15) > 3:
                    raise Broken("bad DHT segment")
                cnt = sum(s[k + 1:k + 17])
                if n - k < 17 + cnt:
                    raise Broken("bad DHT segment")
                if s[k] & 0x0E:
                    problems.append("Huffman table id %d (the decoder keeps 0 and 1)" % (s[k] & 15))
                if (s[k] >> 4) == 0 and any(v > 11 for v in s[k + 17:k + 17 + cnt]):
                    problems.append("DC Huffman symbol above 11")
                ht.add(s[k])
                k += 17 + cnt
        elif m == 0xDD:
            if n < 2:
                raise Broken("bad DRI segment")
            notes.append("restart interval %d MCUs (supported)" % be16(s, 0))
        elif 0xC0 <= m <= 0xCF and m not in (0xC4, 0xC8, 0xCC):
            if comps:
                raise Broken("second frame header")
            if n < 6:
                raise Broken("bad SOF segment")
            info["sof"] = SOF_NAMES.get(m, "SOF%d" % (m - 0xC0))
            if m != 0xC0:
                problems.append("%s JPEG (SOF%d), only baseline (SOF0) is supported" % (info["sof"], m - 0xC0))
                if m not in (0xC1, 0xC2, 0xC9, 0xCA):
                    info["lossless_fix"] = False
            if s[0] != 8:
                lossy("%d-bit samples" % s[0])
            info["h"], info["w"] = be16(s, 1), be16(s, 3)
            if n < 6 + 3 * s[5]:
                raise Broken("bad SOF segment")
            comps = [(s[6 + 3 * c], s[7 + 3 * c] >> 4, s[7 + 3 * c] & 15, s[8 + 3 * c]) for c in range(s[5])]
        elif m == 0xDA:
            if not comps:
                raise Broken("image data before the frame header")
            ns = s[0]
            if n < 4 + 2 * ns:
                raise Broken("bad SOS segment")
            for k in range(ns):
                sel[s[1 + 2 * k]] = s[2 + 2 * k]
            if ns != len(comps):
                problems.append("one component per scan (multi-scan file)")
            ecs = i + 4 + n
            break
        i += 4 + n

    info["header"] = ecs
    baseline = info.get("sof") == "baseline"
    if info["w"] == 0 or info["h"] == 0:
        lossy("width or height is zero")
    need = set()  # DHT (class << 4 | id) the decoder needs
    if len(comps) not in (1, 3):
        lossy("%d components (%s), only grey or YCbCr" % (len(comps), "CMYK" if len(comps) == 4 else "?"))
    elif len(comps) == 3:
        y, cb, cr = comps
        if bytes(c[0] for c in comps) == b"RGB":
            lossy("RGB-coded JPEG (no YCbCr transform)")
        info["samp"] = {(1, 1): "4:4:4", (2, 1): "4:2:2", (2, 2): "4:2:0"}.get((y[1], y[2]), "other")
        if info["samp"] == "other" or (cb[1], cb[2], cr[1], cr[2]) != (1, 1, 1, 1):
            lossy("chroma sampling Y %dx%d Cb %dx%d Cr %dx%d" % (y[1], y[2], cb[1], cb[2], cr[1], cr[2]))
        if baseline and (sel.get(y[0]), sel.get(cb[0]), sel.get(cr[0])) != (0x00, 0x11, 0x11):
            problems.append("Huffman tables not 0 for Y and 1 for Cb/Cr")
        need = {0x00, 0x10, 0x01, 0x11}
    else:
        info["samp"] = "grey"
        if comps[0][1:3] != (1, 1):
            lossy("grey image with %dx%d sampling" % comps[0][1:3])
        if baseline and sel.get(comps[0][0]) != 0x00:
            problems.append("Huffman table 0 not used for Y")
        need = {0x00, 0x10}
    if any(c[3] not in qt for c in comps):
        lossy("a quantisation table is missing")
    if baseline and not need <= ht:
        notes.append("no Huffman tables in the file (camera MJPEG frame): jdec adds the standard ones")
    if baseline:
        p = find_eoi(data, ecs)
        if p and p.startswith("truncated"):
            lossy(p)
        elif p:
            problems.append(p)


def exif_orientation(data):
    try:
        from PIL import Image
        return Image.open(io.BytesIO(data)).getexif().get(0x0112, 1)
    except Exception:
        return 1


def fix(src, dst, quality):
    data = open(src, "rb").read()
    _, _, info = check(data)
    rotated = exif_orientation(data) not in (1, None)
    if info.get("lossless_fix") and not rotated and shutil.which("jpegtran"):
        out = subprocess.run(["jpegtran", "-copy", "none", "-optimize"], input=data,
                             capture_output=True, check=True).stdout
        how = "jpegtran (lossless)"
    else:
        if info.get("lossless_fix") and not rotated:
            print("note: jpegtran not found, re-encoding instead (lossy)")
        from PIL import Image, ImageOps
        img = ImageOps.exif_transpose(Image.open(io.BytesIO(data))).convert("RGB")
        buf = io.BytesIO()
        img.save(buf, "JPEG", quality=quality, subsampling=2, progressive=False, optimize=True)
        out = buf.getvalue()
        how = "re-encoded with Pillow, quality %d, 4:2:0%s" % (quality, ", rotated upright" if rotated else "")
    left = check(out)[0]
    if left:
        sys.exit("fix failed: %s" % "; ".join(left))
    open(dst, "wb").write(out)
    print("%s -> %s: %s, %d bytes" % (src, dst, how, len(out)))


def main():
    ap = argparse.ArgumentParser(description=HELP, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="+")
    ap.add_argument("--fix", metavar="OUT", help="write a decodable copy of the (single) input file")
    ap.add_argument("--quality", type=int, default=90)
    args = ap.parse_args()
    if args.fix and len(args.files) != 1:
        sys.exit("--fix takes one input file")
    bad = 0
    for f in args.files:
        data = open(f, "rb").read()
        problems, notes, info = check(data)
        w, h = info.get("w", 0), info.get("h", 0)
        orient = exif_orientation(data)
        print("%s: %d bytes, %dx%d %s %s" % (f, len(data), w, h, info.get("sof", "?"), info.get("samp", "?")))
        if orient not in (1, None):
            notes.append("EXIF orientation %d: the board decodes the picture as stored, not rotated" % orient)
        for n in notes:
            print("  note: " + n)
        for p in problems:
            print("  PROBLEM: " + p)
        if problems:
            bad += 1
            print("  -> cannot be decoded on IPRO7%s" % (" (fixable without quality loss)"
                                                        if info.get("lossless_fix") else ""))
        else:
            print("  -> OK. Output buffer: RGB565 %d bytes, RGB888 %d bytes; decode about %d ms"
                  % (w * h * 2, w * h * 3, w * h * MS_PER_MPIX // 1000000))
    if args.fix:
        fix(args.files[0], args.fix, args.quality)
        return 0
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())

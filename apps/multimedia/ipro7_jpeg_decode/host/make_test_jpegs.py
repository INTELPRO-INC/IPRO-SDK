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

"""Generate the test JPEGs embedded in ipro7_jpeg_decode and their reference data.

Every image is drawn here from a fixed seed (colour bars, ramps, a textured
area and a circle), so nothing third-party is shipped. Each variant exercises
one rule of the software decoder (TJpgDec, wrapped by jdec_rgb.c):

  basic_420        baseline 4:2:0, JFIF only                 -> decodes
  basic_422        baseline 4:2:2                            -> decodes
  full_444         baseline 4:4:4                            -> decodes
  gray             one component                             -> decodes
  exif_420         EXIF APP1 with thumbnail + ICC APP2       -> decodes
  optimized_420    image-specific Huffman tables             -> decodes
  nodht_422        no DHT segment (camera MJPEG frame)       -> decodes (standard tables added)
  odd_100x75       size not a multiple of the MCU            -> decodes
  restart_420      DRI + RSTn markers                        -> decodes
  progressive_420  SOF2                                      -> rejected
  cmyk             4 components (Adobe CMYK)                 -> rejected
  truncated_420    file cut short, no EOI                    -> rejected
  hd_1280x720      timing
  fhd_1920x1080    timing

Outputs (next to this script's parent directory):
  host/images/<name>.jpg   the JPEG files
  test_images.c            const arrays, RGB block means decoded by libjpeg
                           (16x16 blocks, 64x64 above 640 pixels wide) and the
                           FNV-1a hash of the RGB565 / RGB888 output of a host
                           build of the same decoder (jdec_rgb.c + tjpgd/)

Requires Pillow, numpy, cc, djpeg and jpegtran (libjpeg-turbo) on PATH.
"""
import io
import os
import struct
import subprocess
import sys

import re
import tempfile

import numpy as np
from PIL import Image, ImageCms, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
APP_DIR = os.path.dirname(HERE)
IMG_DIR = os.path.join(HERE, "images")
TJPGD_DIR = os.path.join(APP_DIR, "tjpgd")


def build_host_decoder(out_dir, jd_format=None, sanitize=False):
    """Compile jdec_rgb.c + tjpgd/ for the PC; return the executable.

    jd_format: None keeps tjpgd/tjpgdcnf.h, 0/1/2 builds a copy with that JD_FORMAT.
    sanitize: build with AddressSanitizer and UndefinedBehaviorSanitizer.
    """
    src = TJPGD_DIR
    exe = os.path.join(out_dir, "test_jdec_core")
    if jd_format is not None:
        src = os.path.join(out_dir, "tjpgd_fmt%d" % jd_format)
        os.makedirs(src, exist_ok=True)
        for f in ("tjpgd.c", "tjpgd.h", "tjpgdcnf.h"):
            text = open(os.path.join(TJPGD_DIR, f)).read()
            if f == "tjpgdcnf.h":
                text = re.sub(r"#define JD_FORMAT\s+\d+", "#define JD_FORMAT %d" % jd_format, text)
            open(os.path.join(src, f), "w").write(text)
        exe += "_fmt%d" % jd_format
    flags = ["-O2", "-Wall", "-Wextra", "-fwrapv"]  # -fwrapv as in CMakeLists.txt
    if sanitize:
        flags += ["-g", "-fsanitize=address,undefined", "-fno-sanitize-recover=undefined"]
        exe += "_asan"
    subprocess.run(["cc", *flags, "-I", src, "-o", exe,
                    os.path.join(HERE, "test_jdec_core.c"), os.path.join(src, "tjpgd.c")],
                   check=True)
    return exe


def host_decode(exe, path, fmt, scale=0, out=None):
    """Run the host decoder; return its key=value line as a dict."""
    args = [exe, path, fmt, str(scale)] + ([out] if out else [])
    line = subprocess.run(args, capture_output=True, text=True, check=True).stdout.strip()
    return dict(re.findall(r'(\w+)=("[^"]*"|\S+)', line))


# --------------------------------------------------------------------------
# Source picture
# --------------------------------------------------------------------------
def make_picture(w, h, seed=7):
    rng = np.random.default_rng(seed)
    img = np.zeros((h, w, 3), dtype=np.float32)

    # Top 40 %: eight colour bars.
    bars = [(235, 235, 235), (235, 235, 16), (16, 235, 235), (16, 235, 16),
            (235, 16, 235), (235, 16, 16), (16, 16, 235), (16, 16, 16)]
    top = int(h * 0.4)
    for i, c in enumerate(bars):
        x0, x1 = i * w // 8, (i + 1) * w // 8
        img[:top, x0:x1] = c

    # Next 20 %: R, G, B and grey ramps.
    mid = int(h * 0.6)
    ramp = np.linspace(0, 255, w, dtype=np.float32)
    rows = np.array_split(np.arange(top, mid), 4)
    for k, r in enumerate(rows):
        for ch in range(3):
            img[r, :, ch] = ramp if (k == 3 or k == ch) else 0

    # Bottom 40 %: smooth 2-D gradient with a texture and noise (photo-like).
    yy, xx = np.mgrid[mid:h, 0:w].astype(np.float32)
    img[mid:, :, 0] = 60 + 150 * xx / w
    img[mid:, :, 1] = 90 + 100 * (yy - mid) / max(1, h - mid)
    img[mid:, :, 2] = 160 - 120 * xx / w
    img[mid:] += (18 * np.sin(xx / 7.0) * np.cos(yy / 5.0))[..., None]
    img[mid:] += rng.normal(0, 6, size=img[mid:].shape)

    pil = Image.fromarray(np.clip(img, 0, 255).astype(np.uint8), "RGB")
    d = ImageDraw.Draw(pil)
    r = max(4, min(w, h) // 8)
    cx, cy = w * 3 // 4, (mid + h) // 2
    d.ellipse([cx - r, cy - r, cx + r, cy + r], fill=(250, 200, 40), outline=(0, 0, 0))
    if w >= 160:
        d.text((6, mid + 6), "IPRO7 JPEG", fill=(255, 255, 255))
    return pil


def save(pil, **kw):
    buf = io.BytesIO()
    pil.save(buf, "JPEG", **kw)
    return buf.getvalue()


# --------------------------------------------------------------------------
# Byte-level helpers
# --------------------------------------------------------------------------
def segments(jpg):
    """Yield (marker, start, end) for every segment up to and including SOS."""
    i = 2
    while i + 4 <= len(jpg):
        assert jpg[i] == 0xFF, "bad marker at %d" % i
        m = jpg[i + 1]
        if m == 0xFF:
            i += 1
            continue
        n = struct.unpack(">H", jpg[i + 2:i + 4])[0]
        yield m, i, i + 2 + n
        if m == 0xDA:
            return
        i += 2 + n


def strip_markers(jpg, drop):
    out = bytearray(jpg[:2])
    last = 2
    for m, s, e in segments(jpg):
        if m in drop:
            out += jpg[last:s]
            last = e
    out += jpg[last:]
    return bytes(out)


def insert_after_soi(jpg, blob):
    return jpg[:2] + blob + jpg[2:]


def exif_app1(thumb):
    """EXIF APP1 the way a phone writes it: IFD0 with make/model, IFD1 thumbnail."""
    make, model = b"IPRO\0", b"TEST-CAM\0"
    ifd0_n, ifd1_n = 3, 3
    ifd0 = 8
    ifd1 = ifd0 + 2 + ifd0_n * 12 + 4
    data = ifd1 + 2 + ifd1_n * 12 + 4
    make_off, model_off = data, data + len(make)
    thumb_off = model_off + len(model)
    t = bytearray(b"II*\0" + struct.pack("<I", ifd0))
    t += struct.pack("<H", ifd0_n)
    t += struct.pack("<HHII", 0x010F, 2, len(make), make_off)
    t += struct.pack("<HHII", 0x0110, 2, len(model), model_off)
    t += struct.pack("<HHIHH", 0x0112, 3, 1, 1, 0)
    t += struct.pack("<I", ifd1)
    t += struct.pack("<H", ifd1_n)
    t += struct.pack("<HHIHH", 0x0103, 3, 1, 6, 0)
    t += struct.pack("<HHII", 0x0201, 4, 1, thumb_off)
    t += struct.pack("<HHII", 0x0202, 4, 1, len(thumb))
    t += struct.pack("<I", 0)
    t += make + model + thumb
    payload = b"Exif\0\0" + bytes(t)
    return b"\xFF\xE1" + struct.pack(">H", len(payload) + 2) + payload


def icc_app2(profile):
    payload = b"ICC_PROFILE\0" + bytes([1, 1]) + profile
    return b"\xFF\xE2" + struct.pack(">H", len(payload) + 2) + payload


def jpegtran(jpg, *args):
    r = subprocess.run(["jpegtran", *args], input=jpg, capture_output=True, check=True)
    return r.stdout


# --------------------------------------------------------------------------
# Reference data: block means of the decoded RGB, computed by libjpeg without
# chroma smoothing (the same nearest-neighbour chroma the sample uses).
# --------------------------------------------------------------------------
def reference_rgb(jpg):
    r = subprocess.run(["djpeg", "-nosmooth", "-dct", "int", "-pnm"],
                       input=jpg, capture_output=True, check=True)
    img = Image.open(io.BytesIO(r.stdout)).convert("RGB")
    return np.asarray(img, dtype=np.float32)


def block_means(rgb, blk):
    h, w, _ = rgb.shape
    bw, bh = w // blk, h // blk
    a = rgb[:bh * blk, :bw * blk].reshape(bh, blk, bw, blk, 3).mean(axis=(1, 3))
    return np.rint(a).astype(np.uint8), bw, bh


# --------------------------------------------------------------------------
def build_variants():
    v = []
    pic = make_picture(320, 240)

    basic = save(pic, quality=85, subsampling=2)
    v.append(("basic_420", "baseline 4:2:0, JFIF only", basic, "JDEC_OK"))
    v.append(("basic_422", "baseline 4:2:2", save(pic, quality=85, subsampling=1), "JDEC_OK"))
    v.append(("full_444", "baseline 4:4:4", save(pic, quality=85, subsampling=0), "JDEC_OK"))
    v.append(("gray", "one component (Y only)", save(pic.convert("L"), quality=85), "JDEC_OK"))

    thumb = save(pic.resize((160, 120)), quality=80, subsampling=2)
    body = strip_markers(basic, {0xE0})
    icc = bytearray(ImageCms.ImageCmsProfile(ImageCms.createProfile("sRGB")).tobytes())
    icc[24:36] = struct.pack(">6H", 2026, 1, 1, 0, 0, 0)  # fixed creation date: reproducible output
    exif = insert_after_soi(body, exif_app1(thumb) + icc_app2(icc))
    v.append(("exif_420", "EXIF thumbnail + ICC profile, 4:2:0", exif, "JDEC_OK"))

    v.append(("optimized_420", "image-specific Huffman tables",
              save(pic, quality=85, subsampling=2, optimize=True), "JDEC_OK"))

    nodht = strip_markers(save(pic, quality=80, subsampling=1), {0xC4})
    v.append(("nodht_422", "no DHT (camera MJPEG frame), 4:2:2", nodht, "JDEC_OK"))

    v.append(("odd_100x75", "100x75, not a multiple of 16",
              save(make_picture(100, 75), quality=85, subsampling=2), "JDEC_OK"))

    v.append(("restart_420", "restart markers every MCU row",
              jpegtran(basic, "-copy", "none", "-restart", "1"), "JDEC_OK"))
    v.append(("progressive_420", "progressive (SOF2)",
              save(pic, quality=85, subsampling=2, progressive=True), "JDEC_ERR_PROGRESSIVE"))
    v.append(("cmyk", "CMYK, 4 components", save(pic.convert("CMYK"), quality=85), "JDEC_ERR_COMPONENTS"))
    v.append(("truncated_420", "cut at 60 %, no EOI", basic[:len(basic) * 6 // 10], "JDEC_ERR_TRUNCATED"))

    v.append(("hd_1280x720", "1280x720 4:2:0 (timing)",
              save(make_picture(1280, 720, seed=11), quality=85, subsampling=2), "JDEC_OK"))
    v.append(("fhd_1920x1080", "1920x1080 4:2:0 (timing)",
              save(make_picture(1920, 1080, seed=13), quality=85, subsampling=2), "JDEC_OK"))
    return v


def c_bytes(data, indent="    ", per_line=16):
    lines = []
    for i in range(0, len(data), per_line):
        lines.append(indent + ", ".join("0x%02X" % b for b in data[i:i + per_line]) + ",")
    return "\n".join(lines)


def main():
    os.makedirs(IMG_DIR, exist_ok=True)
    exe = build_host_decoder(tempfile.mkdtemp(prefix="jdec_gen_"))
    out = []
    out.append("/* Generated by host/make_test_jpegs.py - do not edit. */")
    out.append('#include "test_images.h"\n')
    table = []
    for name, what, jpg, expect in build_variants():
        with open(os.path.join(IMG_DIR, name + ".jpg"), "wb") as f:
            f.write(jpg)
        ref_name, blk, bw, bh, fnv565, fnv888 = "NULL", 0, 0, 0, "0", "0"
        path = os.path.join(IMG_DIR, name + ".jpg")
        if expect == "JDEC_OK":
            fnv565 = "0x" + host_decode(exe, path, "565")["fnv"]
            fnv888 = "0x" + host_decode(exe, path, "888")["fnv"]
            rgb = reference_rgb(jpg)
            blk = 16 if rgb.shape[1] <= 640 else 64
            means, bw, bh = block_means(rgb, blk)
            ref_name = "ref_" + name
            out.append("static const uint8_t %s[%d] = {" % (ref_name, means.size))
            out.append(c_bytes(means.tobytes()))
            out.append("};\n")
        out.append("static const uint8_t jpg_%s[%d] __attribute__((aligned(8))) = {" % (name, len(jpg)))
        out.append(c_bytes(jpg))
        out.append("};\n")
        table.append('    { "%s", "%s", jpg_%s, sizeof(jpg_%s), %s, %d, %d, %d, %s, %s, %s },'
                     % (name, what, name, name, expect, blk, bw, bh, ref_name, fnv565, fnv888))
        print("%-16s %7d B  %s" % (name, len(jpg), expect))
    out.append("const test_image_t g_test_images[] = {")
    out += table
    out.append("};\n")
    out.append("const unsigned g_test_image_count = sizeof(g_test_images) / sizeof(g_test_images[0]);")
    with open(os.path.join(APP_DIR, "test_images.c"), "w") as f:
        f.write("\n".join(out) + "\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())

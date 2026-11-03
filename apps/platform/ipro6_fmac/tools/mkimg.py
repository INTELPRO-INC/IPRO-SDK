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

"""Make an FMAC download image (fmac_dl.h fmac_img_hdr_t + bytes) from an
OCRAM-linked ELF: every loadable segment, placed at its load address."""
import struct
import sys
import time
import zlib

WIN_BASE = 0x11000000
WIN_SIZE = 0x00050000
MAGIC = 0x57463649          # "I6FW"
HDR_VER = 1
HDR_LEN = 64


def load_segments(path):
    data = open(path, 'rb').read()
    if data[:4] != b'\x7fELF' or data[4] != 1 or data[5] != 1:
        sys.exit('%s: not a 32-bit little-endian ELF' % path)
    entry, phoff = struct.unpack_from('<II', data, 24)
    phentsize, phnum = struct.unpack_from('<HH', data, 42)
    segs = []
    for i in range(phnum):
        p_type, off, _vaddr, paddr, filesz, _memsz = struct.unpack_from(
            '<IIIIII', data, phoff + i * phentsize)
        if p_type == 1 and filesz:
            segs.append((paddr, data[off:off + filesz]))
    return entry, segs


def main():
    if len(sys.argv) != 3:
        sys.exit('usage: mkimg.py <elf> <out.img>')
    entry, segs = load_segments(sys.argv[1])
    lo = min(a for a, _ in segs)
    hi = max(a + len(b) for a, b in segs)
    if lo != WIN_BASE or hi > WIN_BASE + WIN_SIZE:
        sys.exit('image 0x%08x-0x%08x outside the OCRAM window' % (lo, hi))
    if not WIN_BASE <= entry < hi:
        sys.exit('entry 0x%08x outside the image' % entry)
    size = (hi - lo + 3) & ~3
    img = bytearray(size)
    for a, b in segs:
        img[a - lo:a - lo + len(b)] = b
    crc = zlib.crc32(img) & 0xFFFFFFFF
    build = time.strftime('%Y%m%d-%H%M%S').encode()[:31]
    hdr = struct.pack('<IHHIIIIII32s', MAGIC, HDR_LEN, HDR_VER, lo, size,
                      entry, crc, 0, 0, build)
    assert len(hdr) == HDR_LEN
    with open(sys.argv[2], 'wb') as f:
        f.write(hdr + img)
    print('mkimg: %s load 0x%08x len %u entry 0x%08x crc %08x' %
          (sys.argv[2], lo, size, entry, crc))


if __name__ == '__main__':
    main()

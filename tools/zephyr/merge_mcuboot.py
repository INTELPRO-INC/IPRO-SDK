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

"""Concatenate MCUboot and a signed application into one boot2 "FW" image.

    merge_mcuboot.py <sysbuild-build-dir> [-o merged.bin]

boot2 loads the FW partition as a whole, so the bootloader (linked at the
XIP base) and the image in slot0 are flashed together: MCUboot's zephyr.bin
padded with 0xFF up to the slot0 offset (taken from the application's
devicetree: slot0_partition.reg - boot_partition.reg), then zephyr.signed.bin.
Flash the result with the IntelProCube tool as the firmware.
"""
import argparse, os, re, sys

ap = argparse.ArgumentParser()
ap.add_argument("build_dir")
ap.add_argument("-o", "--output", default=None)
ap.add_argument("--slot1", default=None,
                help="signed+padded upgrade image (imgtool --pad) to place in slot1 as a pending test upgrade")
a = ap.parse_args()

def find_app_dir(bd):
    for name in os.listdir(bd):
        p = os.path.join(bd, name, "zephyr", "zephyr.signed.bin")
        if name != "mcuboot" and os.path.exists(p):
            return os.path.join(bd, name)
    sys.exit("no <app>/zephyr/zephyr.signed.bin under %s" % bd)

app_dir = find_app_dir(a.build_dir)
boot_bin = os.path.join(a.build_dir, "mcuboot", "zephyr", "zephyr.bin")
app_bin = os.path.join(app_dir, "zephyr", "zephyr.signed.bin")
dts = open(os.path.join(app_dir, "zephyr", "zephyr.dts")).read()

def reg_of(label):
    m = re.search(r"%s:\s*partition@[0-9a-fA-F]+\s*\{[^}]*?reg\s*=\s*<\s*(0x[0-9a-fA-F]+|\d+)\s+(0x[0-9a-fA-F]+|\d+)\s*>" % label, dts, re.S)
    if not m:
        sys.exit("partition %s not found in %s" % (label, os.path.join(app_dir, "zephyr", "zephyr.dts")))
    return int(m.group(1), 0), int(m.group(2), 0)

boot_off, boot_size = reg_of("boot_partition")
slot0_off, slot0_size = reg_of("slot0_partition")
slot1_off, slot1_size = reg_of("slot1_partition")
boot = open(boot_bin, "rb").read()
app = open(app_bin, "rb").read()
pad = slot0_off - boot_off
if len(boot) > pad:
    sys.exit("MCUboot (%d B) does not fit before slot0 (%d B)" % (len(boot), pad))
if len(app) > slot0_size:
    sys.exit("application (%d B) does not fit slot0 (%d B)" % (len(app), slot0_size))
img = boot + b"\xff" * (pad - len(boot)) + app
note = ""
if a.slot1:
    up = open(a.slot1, "rb").read()
    if len(up) > slot1_size:
        sys.exit("upgrade image (%d B) does not fit slot1 (%d B)" % (len(up), slot1_size))
    img += b"\xff" * (slot1_off - boot_off - len(img)) + up
    note = " + slot1 upgrade %d B @0x%x" % (len(up), slot1_off)
out = a.output or os.path.join(a.build_dir, "merged.bin")
with open(out, "wb") as f:
    f.write(img)
print("%s: mcuboot %d B @0x%x + app %d B @0x%x (slot0 %d B)%s" % (out, len(boot), boot_off, len(app), slot0_off, slot0_size, note))

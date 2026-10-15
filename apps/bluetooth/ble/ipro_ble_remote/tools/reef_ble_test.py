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

"""
BLE central test for ipro_ble_remote on the IPRO7AI Reef (bleak; tested on macOS).

    python3 -m venv .venv && .venv/bin/pip install bleak
    .venv/bin/python reef_ble_test.py                  # info led env sound tone img
    .venv/bin/python reef_ble_test.py keys motion      # interactive: press keys / move
    KEY_SECS=30 MOTION_SECS=30 ...                     # longer interactive windows

Captured JPEGs land next to this script as capture_<time>.jpg.
"""
import asyncio
import os
import struct
import sys
import time

from bleak import BleakClient, BleakScanner

NAME = os.environ.get("REMOTE_NAME", "IPRO-Remote")
OUT = os.environ.get("OUT_DIR", os.path.dirname(os.path.abspath(__file__)))
KEYS = ["BOOT", "SW3", "SW4", "SW5"]
EVENTS = ["released", "pressed", "long", "double"]


def U(x):
    return f"0000{x:04x}-0000-1000-8000-00805f9b34fb"


async def t_info(c):
    print("board info:", (await c.read_gatt_char(U(0xFF85))).decode())


async def t_led(c):
    await c.write_gatt_char(U(0xFF81), bytes([255, 0, 0]), response=True)    # colour
    await c.write_gatt_char(U(0xFF11), b"\x01", response=True)               # on
    await c.write_gatt_char(U(0xFF12), bytes([50]), response=True)           # 50 %
    await c.write_gatt_char(U(0xFF13), bytes([2]), response=True)            # blink slow
    print("LED colour", list(await c.read_gatt_char(U(0xFF81))),
          "state", (await c.read_gatt_char(U(0xFF11)))[0],
          "brightness", (await c.read_gatt_char(U(0xFF12)))[0],
          "mode", (await c.read_gatt_char(U(0xFF13)))[0])


async def t_env(c):
    vals = []
    await c.start_notify(U(0xFF31), lambda h, d: vals.append(struct.unpack("<h", d)[0] / 100))
    await asyncio.sleep(3.5)
    await c.stop_notify(U(0xFF31))
    print("chip temperature (C):", vals, "humidity:", (await c.read_gatt_char(U(0xFF32)))[0])


async def t_sound(c):
    lv = []
    await c.start_notify(U(0xFF83), lambda h, d: lv.append(struct.unpack("b", d)[0]))
    await asyncio.sleep(4.5)
    await c.stop_notify(U(0xFF83))
    print("microphone level (dBFS):", lv)


async def t_tone(c):
    await c.write_gatt_char(U(0xFF84), struct.pack("<HHB", 1000, 400, 30), response=True)
    print("tone 1000 Hz, 400 ms, 30 % written")
    await asyncio.sleep(1)


async def t_img(c):
    info, chunks, done = {}, {}, asyncio.Event()

    def on_info(h, d):
        info.update(zip(("size", "count", "chunk", "format", "status"), struct.unpack("<IHHBB", d[:10])))

    def on_data(h, d):
        idx, n = struct.unpack("<HH", d[:4])
        chunks[idx] = bytes(d[4:4 + n])
        if info.get("count") and len(chunks) >= info["count"]:
            done.set()

    await c.start_notify(U(0xFF54), on_info)
    await c.start_notify(U(0xFF53), on_data)
    t0 = time.time()
    await c.write_gatt_char(U(0xFF52), b"\x01", response=False)              # CAPTURE
    try:
        await asyncio.wait_for(done.wait(), 20)
    except asyncio.TimeoutError:
        print("image: timeout, info", info, "chunks", len(chunks))
    if chunks:
        jpg = b"".join(chunks[i] for i in sorted(chunks))
        path = os.path.join(OUT, "capture_%d.jpg" % int(time.time()))
        open(path, "wb").write(jpg)
        ok = jpg[:2] == b"\xff\xd8" and jpg[-2:] == b"\xff\xd9"
        print(f"image: {len(jpg)} bytes, {len(chunks)} chunks in {time.time() - t0:.2f} s, "
              f"{'valid JPEG' if ok else 'NOT a complete JPEG'} -> {path}")
    await c.stop_notify(U(0xFF53))
    await c.stop_notify(U(0xFF54))


async def t_motion(c):
    ev = []
    await c.start_notify(U(0xFF51), lambda h, d: ev.append((d[0], struct.unpack("<I", d[5:9])[0])))
    secs = float(os.environ.get("MOTION_SECS", "15"))
    print(f"motion detection armed for {secs:.0f} s - move in front of the camera")
    await asyncio.sleep(secs)
    await c.stop_notify(U(0xFF51))
    print("motion notifications (state, total):", ev)


async def t_keys(c):
    ev = []
    await c.start_notify(U(0xFF82), lambda h, d: ev.append(struct.unpack("<BBI", d)))
    secs = float(os.environ.get("KEY_SECS", "20"))
    print(f"press the keys now ({secs:.0f} s)")
    await asyncio.sleep(secs)
    await c.stop_notify(U(0xFF82))
    for k, e, n in ev:
        print(f"  {KEYS[k] if k < len(KEYS) else k} {EVENTS[e] if e < len(EVENTS) else e} (count {n})")


TESTS = {"info": t_info, "led": t_led, "env": t_env, "sound": t_sound, "tone": t_tone,
         "img": t_img, "motion": t_motion, "keys": t_keys}


async def main(names):
    dev = await BleakScanner.find_device_by_name(NAME, timeout=15)
    if dev is None:
        sys.exit(f"{NAME} not found")
    async with BleakClient(dev) as c:
        print("connected, MTU", c.mtu_size, "services:",
              " ".join(sorted({s.uuid[4:8] for s in c.services})))
        for n in names:
            await TESTS[n](c)


if __name__ == "__main__":
    names = sys.argv[1:] or ["info", "led", "env", "sound", "tone", "img"]
    bad = [n for n in names if n not in TESTS]
    if bad:
        sys.exit(f"unknown test(s) {bad}; choose from {sorted(TESTS)}")
    asyncio.run(main(names))

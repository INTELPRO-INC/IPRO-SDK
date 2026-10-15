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
BLE OTA client for ipro_ble_remote (protocol as src/services/ota_service.c implements it).

    .venv/bin/python reef_ota.py <package.xz.ota>

On the IPRO7AI Reef the payload is a boot2 OTA package (make_reef_ota.sh),
not a raw .bin: the app stages it in FW slot B, boot2 checks and unpacks it
into slot A on the next boot.

Control 0xFF61: START 01 <size u32>, END 02 <crc32 u32>, ABORT 03, VERIFY 04, REBOOT 05
Data    0xFF62: <seq u16> <len u16> <reserved u16> <payload>, write without response
Status  0xFF63: <status> <error> <progress %> <reserved u16>
                status 0 idle, 1 receiving, 2 verifying, 3 ready, 4 complete, 0xFF error
"""
import asyncio
import os
import struct
import sys
import time
import zlib

from bleak import BleakClient, BleakScanner

NAME = os.environ.get("REMOTE_NAME", "IPRO-Remote")
STATUS = {0: "IDLE", 1: "RECEIVING", 2: "VERIFYING", 3: "READY", 4: "COMPLETE", 0x10: "RETX", 0xFF: "ERROR"}
BARRIER = 8      # packets between status reads: the device queues 20 and flashes as it goes


def U(x):
    return f"0000{x:04x}-0000-1000-8000-00805f9b34fb"


async def wait_status(st, ev, want, timeout):
    while st["s"] not in want:
        ev.clear()
        await asyncio.wait_for(ev.wait(), timeout)
        if st["s"] == 0xFF:
            sys.exit(f"device error {st['e']}")


async def ota(path):
    data = open(path, "rb").read()
    crc = zlib.crc32(data) & 0xFFFFFFFF
    dev = await BleakScanner.find_device_by_name(NAME, timeout=15)
    if dev is None:
        sys.exit(f"{NAME} not found")
    async with BleakClient(dev) as c:
        try:
            print("running:", (await c.read_gatt_char(U(0xFF85))).decode())
        except Exception:
            pass
        payload = c.mtu_size - 3 - 6
        st, ev = {"s": None, "e": 0, "p": 0}, asyncio.Event()

        def on_status(h, d):
            st.update(s=d[0], e=d[1], p=d[2])
            ev.set()

        await c.start_notify(U(0xFF63), on_status)
        print(f"{os.path.basename(path)}: {len(data)} bytes, CRC32 {crc:08x}, {payload} bytes/packet")
        t0 = time.time()
        await c.write_gatt_char(U(0xFF61), struct.pack("<BI", 1, len(data)), response=True)
        await wait_status(st, ev, (1,), 60)                       # erase done
        n = (len(data) + payload - 1) // payload
        for seq in range(n):
            chunk = data[seq * payload:(seq + 1) * payload]
            await c.write_gatt_char(U(0xFF62), struct.pack("<HHH", seq, len(chunk), 0) + chunk,
                                    response=False)
            if seq % BARRIER == BARRIER - 1:
                await c.read_gatt_char(U(0xFF63))                 # completes after the writes
                await asyncio.sleep(0.01)
            if st["s"] == 0xFF:
                sys.exit(f"device error {st['e']} at packet {seq}")
        print(f"sent in {time.time() - t0:.1f} s ({len(data) / (time.time() - t0) / 1024:.1f} KB/s)")
        await c.write_gatt_char(U(0xFF61), struct.pack("<BI", 2, crc), response=True)
        await wait_status(st, ev, (4,), 60)
        print("verified, slot switched; rebooting")
        try:
            await c.write_gatt_char(U(0xFF61), b"\x05", response=True)
        except Exception:
            pass                                                  # the link drops with the reset
    await asyncio.sleep(8)                                        # boot2 unpacks, then the app boots
    dev = await BleakScanner.find_device_by_name(NAME, timeout=30)
    if dev is None:
        sys.exit("not back after the reboot")
    async with BleakClient(dev) as c:
        print("running:", (await c.read_gatt_char(U(0xFF85))).decode())


if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    asyncio.run(ota(sys.argv[1]))

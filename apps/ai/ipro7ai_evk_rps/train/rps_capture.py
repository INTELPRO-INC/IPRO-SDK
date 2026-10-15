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

"""Collect labelled 128x128 training crops from the board's play box.

The firmware's `rps capture <label>` prints the frame the NPU just saw as
base64 between CAPBEGIN / CAPEND lines (RGB, 3 bytes per pixel, checksum in
the CAPEND line). This script asks for N of them and writes PNGs:

    python3 rps_capture.py rock 60              # hold a fist in the box
    python3 rps_capture.py paper 60
    python3 rps_capture.py scissors 60
    python3 rps_capture.py none 40              # empty box, your desk, a face...

Files land in data/board/<label>/<label>_<nnn>.png (next to this script by
default; --out changes it). Move the hand around between shots - distance,
angle, rotation, lighting - the model learns what you show it.

Port defaults to the CP2104 at 921600 (this app's console baud).
"""
import argparse, base64, fcntl, glob, os, re, select, struct, sys, termios, time

import numpy as np
from PIL import Image


def open_port(path, baud):
    fd = os.open(path, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
    attr = termios.tcgetattr(fd)
    attr[0] = 0
    attr[1] = 0
    attr[2] = termios.CS8 | termios.CREAD | termios.CLOCAL
    attr[3] = 0
    std = getattr(termios, f"B{baud}", None)
    attr[4] = attr[5] = std if std is not None else termios.B230400
    attr[6][termios.VMIN] = 0
    attr[6][termios.VTIME] = 0
    termios.tcsetattr(fd, termios.TCSANOW, attr)
    if std is None:
        # macOS termios has no B921600: set the real divisor with IOSSIOSPEED
        # (same trick as tools/serial_monitor.py).
        fcntl.ioctl(fd, 0x80085402, struct.pack("Q", baud))
    return fd


def read_until(fd, marker, timeout):
    """Bytes up to and including a line containing marker, or None."""
    buf = b""
    t0 = time.time()
    while time.time() - t0 < timeout:
        r, _, _ = select.select([fd], [], [], 0.1)
        if r:
            try:
                buf += os.read(fd, 65536)
            except BlockingIOError:
                pass
            if marker in buf:
                return buf
    return None


def strip_ansi(s):
    return re.sub(rb"\x1b\[[0-9;]*m", b"", s)


def capture_one(fd, label, timeout=15.0):
    # drain
    while True:
        r, _, _ = select.select([fd], [], [], 0.05)
        if not r:
            break
        try:
            os.read(fd, 65536)
        except BlockingIOError:
            break
    os.write(fd, f"rps capture {label}\r\n".encode())
    raw = read_until(fd, b"CAPEND", timeout)
    if raw is None:
        return None, "timeout"
    txt = strip_ansi(raw).decode("ascii", "replace")
    m = re.search(r"CAPBEGIN (\S+) (\d+) (\d+) (\d+)\s*\n(.*?)CAPEND (\d+)", txt, re.S)
    if not m:
        return None, "no frame in output"
    w, h, c = int(m.group(2)), int(m.group(3)), int(m.group(4))
    # Other tasks' console lines ([RPS] ..., [GAME] ...) can land inside the
    # block: keep only lines that are pure base64.
    b64 = "".join(l.strip() for l in m.group(5).splitlines()
                  if re.fullmatch(r"[A-Za-z0-9+/=]+", l.strip() or "x"))
    try:
        data = base64.b64decode(b64, validate=True)
    except Exception as e:
        return None, f"base64: {e}"
    if len(data) != w * h * c:
        return None, f"got {len(data)} bytes, expected {w * h * c}"
    if sum(data) & 0xFFFFFFFF != int(m.group(6)):
        return None, "checksum mismatch (UART noise) - retrying"
    return np.frombuffer(data, np.uint8).reshape(h, w, c), None


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("label", choices=["rock", "paper", "scissors", "none"])
    ap.add_argument("count", type=int, nargs="?", default=50)
    ap.add_argument("--port", default=os.environ.get("PORT", "/dev/cu.usbserial-02LWD648"))
    ap.add_argument("--baud", type=int, default=921600)
    ap.add_argument("--interval", type=float, default=0.4, help="seconds between shots")
    ap.add_argument("--out", default=os.path.join(os.path.dirname(os.path.abspath(__file__)), "data", "board"))
    a = ap.parse_args()

    out = os.path.join(a.out, a.label)
    os.makedirs(out, exist_ok=True)
    have = len(glob.glob(os.path.join(out, "*.png")))
    fd = open_port(a.port, a.baud)
    os.write(fd, b"rps print off\r\n")          # keep the console quiet
    time.sleep(0.3)
    print(f"{a.label}: capturing {a.count} frames into {out} (already {have}). "
          f"Move the hand a little between shots. Ctrl-C to stop.")
    ok = fail = 0
    try:
        while ok < a.count:
            img, err = capture_one(fd, a.label)
            if img is None:
                fail += 1
                print(f"  ! {err}")
                if fail > 10:
                    print("too many failures - is the NPU running? (`rps status`)")
                    break
                continue
            path = os.path.join(out, f"{a.label}_{have + ok:03d}.png")
            Image.fromarray(img).save(path)
            ok += 1
            print(f"  {ok}/{a.count} {os.path.basename(path)}  mean rgb {img.reshape(-1, 3).mean(0).round(0)}")
            time.sleep(a.interval)
    except KeyboardInterrupt:
        pass
    finally:
        os.close(fd)
    print(f"done: {ok} saved, {fail} failed")


if __name__ == "__main__":
    main()

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

"""Put the running firmware into the boot ROM's download mode: send reboot_dl
over the console and confirm the shell echoed it. Exit 0 = the board is in
download mode (or already was), 1 = no shell answered (already in DL, or dead).

    python3 enter_dl.py /dev/cu.usbserial-XXXX 921600
"""
import fcntl, os, select, struct, sys, termios, time

port, baud = sys.argv[1], int(sys.argv[2])
fd = os.open(port, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
a = termios.tcgetattr(fd)
a[0] = a[1] = a[3] = 0
a[2] = termios.CS8 | termios.CREAD | termios.CLOCAL
std = getattr(termios, f"B{baud}", None)
a[4] = a[5] = std if std is not None else termios.B230400
a[6][termios.VMIN] = 0
a[6][termios.VTIME] = 0
termios.tcsetattr(fd, termios.TCSANOW, a)
if std is None:
    fcntl.ioctl(fd, 0x80085402, struct.pack("Q", baud))     # IOSSIOSPEED
time.sleep(0.3)


def read_for(secs):
    buf = b""
    t0 = time.time()
    while time.time() - t0 < secs:
        r, _, _ = select.select([fd], [], [], 0.05)
        if r:
            try:
                buf += os.read(fd, 4096)
            except BlockingIOError:
                pass
    return buf


read_for(0.2)                                           # drain
for attempt in range(3):
    os.write(fd, b"\r\n")
    read_for(0.2)
    os.write(fd, b"reboot_dl\r\n")
    got = read_for(1.5)
    if b"reboot" in got:
        print("reboot_dl accepted by the shell - board entering download mode")
        time.sleep(0.8)                                 # let the boot ROM settle
        os.close(fd)
        sys.exit(0)
    print(f"attempt {attempt + 1}: no shell echo ({len(got)} bytes)")
os.close(fd)
print("no shell on this port - board may already be in download mode")
sys.exit(1)

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

"""CSL analysis of an nRF 802.15.4 sniffer pcap (DLT 283, TAP).

usage: csl_pcap.py FILE.pcap [--ts-end] [--dump]

Capture with the nRF 802.15.4 sniffer (nrf802154_sniffer.py --capture
--metadata ieee802154-tap). Its timestamps mark the SFD, which the bench
confirmed from ACK spacing after 28 B and 110 B frames; --ts-end treats them
as end-of-frame instead.

Only unencrypted MAC header fields are used (FC, seq, addresses, aux sec header,
header IEs), so no network key is needed.

- Finds frames carrying a CSL IE (id 0x1a) -> CSL receiver and its sample schedule.
- For every frame addressed to that receiver, reports the SFD offset against the
  predicted sample time (latest CSL IE: sfd_end + phase*160us + k*period*160us).
- Matches AR frames to their ACK (same seq, < 6 ms after the frame's SFD).
"""
import struct, sys

SYM_US = 16
UNIT_US = 160  # CSL phase/period unit = 10 symbols


def read_pcap(path):
    with open(path, "rb") as f:
        hdr = f.read(24)
        if len(hdr) < 24:
            return
        dlt = struct.unpack("<I", hdr[20:24])[0]
        while True:
            ph = f.read(16)
            if len(ph) < 16:
                return
            sec, usec, incl, _ = struct.unpack("<IIII", ph)
            data = f.read(incl)
            if dlt == 283:
                tlen = struct.unpack("<H", data[2:4])[0]
                data = data[tlen:]
            yield sec * 1_000_000 + usec, data


def parse(fr):
    """Return dict of MAC header fields or None."""
    if len(fr) < 3:
        return None
    fc = fr[0] | fr[1] << 8
    p = {"fc": fc, "type": fc & 7, "sec": bool(fc & 8), "fp": bool(fc & 0x10),
         "ar": bool(fc & 0x20), "comp": bool(fc & 0x40), "seqsup": bool(fc & 0x100),
         "iep": bool(fc & 0x200), "dm": (fc >> 10) & 3, "ver": (fc >> 12) & 3,
         "sm": (fc >> 14) & 3, "len": len(fr)}
    o = 2
    p["seq"] = None
    if not p["seqsup"]:
        p["seq"] = fr[o]; o += 1
    dm, sm, comp, ver = p["dm"], p["sm"], p["comp"], p["ver"]
    if ver < 2:
        dpan = dm != 0
        span = sm != 0 and not comp
    else:
        if dm == 0 and sm == 0:
            dpan, span = comp, False
        elif sm == 0:
            dpan, span = not comp, False
        elif dm == 0:
            dpan, span = False, not comp
        elif dm == 3 and sm == 3:
            dpan, span = not comp, False
        else:
            dpan, span = True, not comp
    try:
        if dpan:
            o += 2
        p["dst"] = fr[o:o + (2 if dm == 2 else 8 if dm == 3 else 0)][::-1].hex()
        o += 2 if dm == 2 else 8 if dm == 3 else 0
        if span:
            o += 2
        p["src"] = fr[o:o + (2 if sm == 2 else 8 if sm == 3 else 0)][::-1].hex()
        o += 2 if sm == 2 else 8 if sm == 3 else 0
        p["fcnt"] = None
        if p["sec"]:
            sc = fr[o]; o += 1
            if not sc & 0x20:
                p["fcnt"] = struct.unpack("<I", fr[o:o + 4])[0]; o += 4
            o += {0: 0, 1: 1, 2: 5, 3: 9}[(sc >> 3) & 3]
        p["csl"] = None
        if p["iep"]:
            while o + 2 <= len(fr) - 2:
                d = fr[o] | fr[o + 1] << 8
                ln, eid = d & 0x7f, (d >> 7) & 0xff
                if d & 0x8000:
                    break
                if eid == 0x1a and ln >= 4:
                    p["csl"] = struct.unpack("<HH", fr[o + 2:o + 6])
                o += 2 + ln
                if eid in (0x7e, 0x7f):
                    break
    except (IndexError, struct.error, KeyError):
        return None
    return p


def main():
    path = sys.argv[1]
    ts_end = "--ts-end" in sys.argv
    dump = "--dump" in sys.argv
    frames = []
    bad = 0
    for ts, fr in read_pcap(path):
        p = parse(fr)
        if p is None:
            bad += 1
            continue
        # SFD end: sniffer ts either at SFD or at frame end (PHR + PSDU after SFD)
        p["sfd"] = ts - (1 + len(fr)) * SYM_US * 2 if ts_end else ts
        p["ts"] = ts
        frames.append(p)
    if not frames:
        print("no frames"); return
    t0 = frames[0]["ts"]
    print("frames=%d unparsed=%d span=%.1fs" % (
        len(frames), bad,
        (frames[-1]["ts"] - t0) / 1e6))

    # CSL receiver(s): sources of frames with CSL IE; ACKs have no src -> use the acked frame's dst
    csl_ids = set()
    for i, f in enumerate(frames):
        if f["csl"] and f["src"]:
            csl_ids.add(f["src"])
    # ACK -> previous frame with same seq and AR, within 6 ms (127 B frame + ACK)
    ack_of = {}
    for i, f in enumerate(frames):
        if f["type"] != 2:
            continue
        for j in range(i - 1, max(-1, i - 6), -1):
            g = frames[j]
            if g["type"] != 2 and g["ar"] and g["seq"] == f["seq"] and f["ts"] - g["ts"] < 6000:
                ack_of[i] = j
                break
    for i, j in ack_of.items():
        if frames[i]["csl"]:
            csl_ids.add(frames[j]["dst"])
    print("CSL IE carriers (src / acked dst):", sorted(csl_ids))
    n_csl_ack = sum(1 for i in ack_of if frames[i]["csl"])
    n_enh = sum(1 for i in ack_of if frames[i]["ver"] == 2)
    print("acks matched=%d enh(v2015)=%d with CSL IE=%d secured=%d" % (
        len(ack_of), n_enh, n_csl_ack, sum(1 for i in ack_of if frames[i]["sec"])))

    acked = set(ack_of.values())
    sched = None  # (sfd_end_of_ie_frame + phase, period)
    stats = []
    for i, f in enumerate(frames):
        # the schedule is updated by any CSL IE from the receiver
        if f["csl"]:
            ph, per = f["csl"]
            sched = (f["sfd"] + ph * UNIT_US, per * UNIT_US)
        tag = ""
        if f["type"] != 2 and f["dst"] in csl_ids and sched and sched[1]:
            base, per = sched
            k = round((f["sfd"] - base) / per)
            off = f["sfd"] - (base + k * per)
            ok = i in acked
            stats.append((off, ok, f["ar"], (f["sfd"] - base) / 1e6))
            tag = " -> CSL rx off=%+6d us k=%d %s" % (off, k, "ACKED" if ok else ("NOACK" if f["ar"] else ""))
        if dump:
            print("%10.6f t%d v%d seq=%-4s %s->%s len=%3d%s%s%s%s" % (
                (f["ts"] - t0) / 1e6, f["type"], f["ver"], f["seq"], f["src"] or "-", f["dst"] or "-",
                f["len"], " SEC" if f["sec"] else "", " AR" if f["ar"] else "",
                " CSL(ph=%d,per=%d)" % f["csl"] if f["csl"] else "", tag))
    if stats:
        ar = [s for s in stats if s[2]]
        print("frames to CSL receiver: %d (AR %d, acked %d)" % (len(stats), len(ar), sum(s[1] for s in ar)))
        bins = [(-10**9, -2000), (-2000, -500), (-500, -200), (-200, 0), (0, 200), (200, 500), (500, 2000), (2000, 10**9)]
        for lo, hi in bins:
            sel = [s for s in ar if lo <= s[0] < hi]
            if sel:
                print("  off [%6d,%6d) us: %3d frames, acked %3d" % (lo, hi, len(sel), sum(s[1] for s in sel)))
        age = [s for s in ar if not s[1]]
        if age:
            print("  unacked: schedule age (s) =", ", ".join("%.1f" % s[3] for s in age[:30]))


if __name__ == "__main__":
    main()

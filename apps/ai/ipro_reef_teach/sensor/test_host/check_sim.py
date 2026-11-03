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

"""sensor_sim.c, compiled for the host: every simulated type's states must be
learnable by IPRO AI Studio's Sensor Teach trainer, from recordings it never
trained on.

    python3 check_sim.py <libsensorsim> <ipro_ai_studio dir> [--seconds 20] [--types sim_vib ...]
"""
import argparse
import ctypes
import sys
import time

import numpy as np

MAX_STATES = 8


class Info(ctypes.Structure):
    _fields_ = [("name", ctypes.c_char_p), ("rate", ctypes.c_uint32), ("min_rate", ctypes.c_uint32),
                ("max_rate", ctypes.c_uint32), ("channels", ctypes.c_uint32),
                ("lsb_per_unit", ctypes.c_float), ("unit", ctypes.c_char_p),
                ("n_states", ctypes.c_uint32), ("states", ctypes.c_char_p * MAX_STATES)]


class Params(ctypes.Structure):
    _fields_ = [("rate", ctypes.c_uint32), ("channels", ctypes.c_uint32), ("noise", ctypes.c_float),
                ("drift", ctypes.c_float), ("severity", ctypes.c_float), ("ramp_s", ctypes.c_float)]


class Sim:
    def __init__(self, path):
        self.lib = ctypes.CDLL(path)
        self.lib.sim_type_info.restype = ctypes.POINTER(Info)
        self.lib.sim_state.restype = ctypes.c_char_p

    def info(self, t):
        i = self.lib.sim_type_info(t).contents
        return {"name": i.name.decode(), "rate": i.rate, "channels": i.channels,
                "states": [i.states[k].decode() for k in range(i.n_states)]}

    def record(self, t, state, seconds, seed, **kw):
        p = Params(kw.get("rate", 0), kw.get("channels", 0), kw.get("noise", 1.0), kw.get("drift", 1.0),
                   kw.get("severity", 1.0), kw.get("ramp_s", 0.0))
        self.lib.sim_start(t, ctypes.byref(p), seed)
        assert self.lib.sim_set_state(state.encode()) == 0, state
        info = self.info(t)
        rate = kw.get("rate") or info["rate"]
        ch = kw.get("channels") or info["channels"]
        n = int(seconds * rate)
        buf = (ctypes.c_int16 * (n * ch))()
        self.lib.sim_generate(buf, n)
        return np.frombuffer(buf, np.int16).reshape(n, ch).astype(np.float32)


# window and view per type: what a customer would pick. The motor needs 2 s:
# a broken rotor bar shows as +-3.6 Hz sidebands of 60 Hz, a slow beat a
# short window cannot hold.
SETUP = {"sim_vib": (1.0, "spectrogram"), "sim_imu": (2.56, "spectrogram"),
         "sim_env": (60.0, "wave"), "sim_motor": (2.0, "spectrogram"), "sim_sound": (0.5, "spectrogram")}


# the healthy state anomaly mode learns
NORMAL = {"sim_vib": "normal", "sim_imu": "walk", "sim_env": "normal", "sim_motor": "normal",
          "sim_sound": "hum"}


def fresh_windows(sim, t, state, secs, seed, fe):
    x = sim.record(t, state, secs, seed)
    return [x[o:o + fe.window] for o in range(0, len(x) - fe.window + 1, max(1, fe.window // 2))]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("lib")
    ap.add_argument("studio")
    ap.add_argument("--seconds", type=float, default=20.0)
    ap.add_argument("--types", nargs="*")
    ap.add_argument("--mode", default="classify", choices=["classify", "unknown", "anomaly"])
    ap.add_argument("--size", default="medium")
    ap.add_argument("--augment", default="strong")
    ap.add_argument("--floors", action="store_true", help="unknown mode: also the per-class floors")
    ap.add_argument("--false-alarm", type=float, default=0.01, help="anomaly mode's target")
    args = ap.parse_args()
    sys.path.insert(0, args.studio)
    from ipro_ai_core import sensor_teach as st

    sim = Sim(args.lib)
    ok = True
    for t in range(5):
        info = sim.info(t)
        if args.types and info["name"] not in args.types:
            continue
        win, mode = SETUP[info["name"]]
        secs = max(args.seconds, 4 * win)
        fe = st.FrontEnd(rate=info["rate"], channels=info["channels"], window_s=win, mode=mode)
        states = info["states"]
        if args.mode == "classify":
            taught = states
        elif args.mode == "unknown":
            taught = states[:-2]                       # the last two are never shown
        else:
            taught = [NORMAL[info["name"]]]
        recs = {s: [sim.record(t, s, secs, 11 + k), sim.record(t, s, secs, 23 + k)]
                for k, s in enumerate(states) if s in taught}
        cfg = st.TrainConfig(size=args.size, augment=args.augment, anomaly=args.mode == "anomaly",
                             unknown=args.mode == "unknown", false_alarm=args.false_alarm)
        t0 = time.time()
        m = st.train(recs, fe, cfg)
        took = time.time() - t0
        q, calib = st.prepare_int8(m, recs, args.false_alarm)
        thr = st.unknown_thresholds(m, q)
        line = (f"{info['name']:9s} {args.size:6s} trained {took:4.1f} s "
                f"(best epoch {m.best_epoch + 1}/{len(m.history)}), held-out {m.val_acc:.3f}")
        rows = {}
        for k, s in enumerate(states):
            # three new takes per state, odd seeds (the generator ORs in a 1)
            wins = [w for j in range(3) for w in fresh_windows(sim, t, s, secs, 1001 + 200 * j + 2 * k, fe)]
            z = q.logits(st.quantize_input(np.stack([m.front.image(w) for w in wins])))
            # anomaly mode decides on the margin its bias was set for; floors on
            # the normal class's own score alarm on a new take's level
            use = thr if args.mode == "unknown" and args.floors else None
            pred = st.classify(z, use)
            if m.unknown:                              # the made-up class reads as UNKNOWN too
                pred = np.where(pred == len(m.labels) - 1, -1, pred)
            rows[s] = pred
        if args.mode == "classify":
            acc = np.mean([np.mean(rows[s] == k) for k, s in enumerate(states)])
            print(f"{line}, fresh takes int8 {acc:.3f}")
            for k, s in enumerate(states):
                bad = rows[s] != k
                if bad.any():
                    print(f"    {s:12s} -> " + str({states[j] if j >= 0 else 'UNKNOWN': int(v)
                                                    for j, v in zip(*np.unique(rows[s], return_counts=True))}))
            ok &= acc >= 0.9
        elif args.mode == "unknown":
            known = np.mean([np.mean(rows[s] == taught.index(s)) for s in taught])
            false_unk = np.mean([np.mean(rows[s] == -1) for s in taught])
            caught = {s: float(np.mean(rows[s] == -1)) for s in states if s not in taught}
            print(f"{line}; taught {taught}: right {known:.3f}, said UNKNOWN {false_unk:.3f}; "
                  f"never shown, said UNKNOWN: {caught}")
        else:
            normal = taught[0]
            false_alarm = float(np.mean(rows[normal] != 0))
            caught = {s: float(np.mean(rows[s] != 0)) for s in states if s != normal}
            print(f"{line}; trained on {normal} only: false alarms {false_alarm:.3f}; "
                  f"faults caught: {caught}")
    if args.mode == "classify":
        print("PASS" if ok else "FAIL")
        return 0 if ok else 1
    return 0


if __name__ == "__main__":
    sys.exit(main())

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

"""sensor_front.c against IPRO AI Studio's numpy FrontEnd on random windows.

    python3 check_front.py <front_cli binary> <ipro_ai_studio dir>
"""
import struct
import subprocess
import sys

import numpy as np

sys.path.insert(0, sys.argv[2])
from ipro_ai_core import sensor_sim as sim, sensor_teach as st  # noqa: E402


def run_c(exe, fe, x):
    mean = list(fe.mean) + [0.0] * (8 - fe.channels)
    std = list(fe.std) + [1.0] * (8 - fe.channels)
    hdr = struct.pack("<III8f8ffi", fe.channels, fe.window, st.MODES.index(fe.mode), *mean, *std,
                      st.IN_SCALE, 0)
    out = subprocess.run([exe], input=hdr + x.astype(np.float32).tobytes(), capture_output=True,
                         check=True).stdout
    return np.frombuffer(out, np.int8).reshape(64, 64, 8)


def main():
    exe = sys.argv[1]
    rng = np.random.default_rng(0)
    worst = 0
    cases = [("spectrogram", 1000, 3, 1.0), ("spectrogram", 16000, 1, 1.0), ("wave", 10, 2, 30.0),
             ("wave", 1000, 3, 2.0), ("spectrogram", 200, 8, 0.75)]
    for mode, rate, ch, win in cases:
        fe = st.FrontEnd(rate=rate, channels=ch, window_s=win, mode=mode)
        if rate == 1000 and ch == 3:
            x = sim.to_lsb(sim.simulate("bearing", win, rng)).astype(np.float32)
        else:
            x = (rng.normal(0, 3000, (fe.window, ch)) + np.linspace(0, 500, fe.window)[:, None]).astype(np.float32)
        fe = fe.fit_norm(fe.raw(x)[None])
        ref = st.quantize_input(fe.image(x))
        got = run_c(exe, fe, x)
        d = np.abs(ref.astype(int) - got.astype(int))
        worst = max(worst, int(d.max()))
        print(f"{mode:11s} {rate:5d} Hz x{ch} {win:g} s: max diff {d.max()}, "
              f"{100 * np.mean(d > 0):.3f} % of values differ")
    print("PASS" if worst <= 1 else "FAIL")
    return 0 if worst <= 1 else 1


if __name__ == "__main__":
    sys.exit(main())

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

"""Generate NIST SP 800-22 KAT data + golden values from the sts reference run.

Produces test/nist_kat_data.c containing:
  - the first 2^20 bits of the binary expansion of e (computed exactly here,
    cross-validated against NIST sts data/data.e), packed MSB-first per byte
    to match the library's GET_BIT macro
  - golden P-values parsed from an sts-2.1.2 run over the same bit stream

Usage:
  1. Build NIST sts-2.1.2 (csrc.nist.gov) and run it on the 2^20-bit e file
     this script can emit with --emit-bits:
       printf '0\\n<bits-file>\\n1\\n0\\n1\\n0\\n' | ./assess 1048576
  2. gen_kat_golden.py --sts-experiments <sts>/experiments/AlgorithmTesting \\
       --out nist_kat_data.c [--check-data-e <sts>/data/data.e]

sts defaults match the library defaults at n=2^20: BlockFrequency M=128,
templates m=9, Universal L=7/Q=1280, LinearComplexity M=500, Serial m=16,
ApEn m=10.
"""

import argparse
import math
import re
import sys
from pathlib import Path

N_BITS = 1 << 20

# The library's quick-mode non-overlapping templates (nist_sp800_22.c);
# all must be members of the official m=9 aperiodic set
QUICK_TEMPLATES = [0x001, 0x003, 0x005, 0x00B, 0x00D,
                   0x013, 0x025, 0x04B, 0x093, 0x128]

DFT_TOL = 0.02      # device FFT is f32; a few threshold-adjacent bins may flip
DEFAULT_TOL = 1e-4  # double math; covers the A&S-vs-Cephes erfc difference


def e_bits(n_bits):
    """Exact first n_bits of the binary expansion of e (starting '10.1011...')."""
    guard = 64
    shift = n_bits - 2 + guard
    t = 1 << shift
    s = 0
    k = 0
    while t:
        s += t
        k += 1
        t //= k
    bits = bin(s >> guard)[2:]
    assert len(bits) == n_bits
    return bits


def parse_pvalues(path, pattern=r"(?<![A-Za-z0-9])p_value\s*=\s*([0-9.eE+-]+)"):
    # lookbehind keeps "exp_value = ..." (Universal stats) from matching
    return [float(m) for m in re.findall(pattern, Path(path).read_text())]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--sts-experiments", required=False)
    ap.add_argument("--out", required=False)
    ap.add_argument("--check-data-e")
    ap.add_argument("--emit-bits", help="write ASCII bit file for sts input and exit")
    args = ap.parse_args()

    bits = e_bits(N_BITS)

    if args.check_data_e:
        ref = "".join(c for c in Path(args.check_data_e).read_text() if c in "01")
        # NIST's data.e tail has a 2-bit rounding artifact; require exact
        # match everywhere before that.
        mismatch = [i for i, (a, b) in enumerate(zip(bits, ref)) if a != b]
        bad = [i for i in mismatch if i < len(ref) - 2]
        if bad:
            sys.exit(f"e generator mismatch vs data.e at bit {bad[0]}")
        print(f"data.e cross-check OK ({min(len(bits), len(ref))} bits, "
              f"tail artifacts: {mismatch})")

    if args.emit_bits:
        Path(args.emit_bits).write_text(bits)
        print(f"wrote {N_BITS} bits to {args.emit_bits}")
        return

    if not args.sts_experiments or not args.out:
        sys.exit("--sts-experiments and --out are required to generate")

    exp = Path(args.sts_experiments)

    g = {}
    g["frequency"] = parse_pvalues(exp / "Frequency/stats.txt")[0]
    g["block_frequency"] = parse_pvalues(exp / "BlockFrequency/stats.txt")[0]
    g["runs"] = parse_pvalues(exp / "Runs/stats.txt")[0]
    g["longest_run"] = parse_pvalues(exp / "LongestRun/stats.txt")[0]
    g["rank"] = parse_pvalues(exp / "Rank/stats.txt")[0]
    g["dft"] = parse_pvalues(exp / "FFT/stats.txt")[0]
    g["universal"] = parse_pvalues(exp / "Universal/stats.txt")[0]
    g["approximate_entropy"] = parse_pvalues(exp / "ApproximateEntropy/stats.txt")[0]
    cusum = parse_pvalues(exp / "CumulativeSums/stats.txt")
    g["cusum_fwd"], g["cusum_bwd"] = cusum[0], cusum[1]   # sts order: FORWARD, REVERSE
    serial = re.findall(r"p_value[12]\s*=\s*([0-9.eE+-]+)",
                        (exp / "Serial/stats.txt").read_text())
    g["serial_p1"], g["serial_p2"] = float(serial[0]), float(serial[1])
    # Overlapping template: freq row ends "... chi2 pval SUCCESS|FAILURE";
    # linear complexity: freq row ends "... chi2 pval"
    ov = (exp / "OverlappingTemplate/stats.txt").read_text()
    g["overlap"] = float(re.findall(
        r"([0-9.]+)\s+(?:SUCCESS|FAILURE)", ov)[-1])
    # Linear Complexity: sts-2.1.2 hardcodes pi[0] = 0.01047, a typo for the
    # spec's theoretical 1/96 = 0.010417 (SP 800-22 Section 3.10). The
    # library uses the spec value, so recompute the golden P-value from the
    # sts frequency counts with the spec pi[]. Q(3, x) = e^-x (1 + x + x^2/2).
    lc = (exp / "LinearComplexity/stats.txt").read_text()
    lc_counts = [int(c) for c in re.findall(
        r"^\s*((?:\d+\s+){7})[0-9.]+\s+[0-9.]+\s*$", lc, re.MULTILINE)[-1].split()]
    lc_pi = [0.010417, 0.03125, 0.125, 0.5, 0.25, 0.0625, 0.020833]
    lc_n = sum(lc_counts)
    lc_chi2 = sum((lc_counts[i] - lc_n * lc_pi[i]) ** 2 / (lc_n * lc_pi[i])
                  for i in range(7))
    lc_x = lc_chi2 / 2.0
    g["linear_complexity"] = math.exp(-lc_x) * (1.0 + lc_x + lc_x * lc_x / 2.0)

    # Non-overlapping: per-template rows "TTTTTTTTT W1..W8 chi2 pval ASSIGN idx"
    rows = re.findall(r"^([01]{9})\s+(?:\d+\s+){8}[0-9.]+\s+([0-9.]+)\s+\w+\s+\d+",
                      (exp / "NonOverlappingTemplate/stats.txt").read_text(),
                      re.MULTILINE)
    tmpl_p = {int(t, 2): float(p) for t, p in rows}
    if len(tmpl_p) != 148:
        sys.exit(f"expected 148 non-overlap templates, got {len(tmpl_p)}")
    g["nonoverlap_full_minp"] = min(tmpl_p.values())
    g["nonoverlap_quick_minp"] = min(tmpl_p[t] for t in QUICK_TEMPLATES)

    re_p = parse_pvalues(exp / "RandomExcursions/stats.txt")
    rev_p = parse_pvalues(exp / "RandomExcursionsVariant/stats.txt",
                          r"p-value\s*=\s*([0-9.eE+-]+)")
    if len(re_p) != 8 or len(rev_p) != 18:
        sys.exit(f"RE/REV count mismatch: {len(re_p)}/{len(rev_p)}")
    g["random_excursions_minp"] = min(re_p)
    g["random_excursions_variant_minp"] = min(rev_p)

    # Pack bits MSB-first per byte (library GET_BIT order)
    data = bytearray(N_BITS // 8)
    for i in range(0, N_BITS, 8):
        data[i // 8] = int(bits[i:i + 8], 2)

    # Suite order: index, name, p1, p2 (-1 = none), tolerance
    entries = [
        ("Frequency (Monobit)", g["frequency"], -1, DEFAULT_TOL),
        ("Block Frequency", g["block_frequency"], -1, DEFAULT_TOL),
        ("Runs", g["runs"], -1, DEFAULT_TOL),
        ("Longest Run of Ones", g["longest_run"], -1, DEFAULT_TOL),
        ("Binary Matrix Rank", g["rank"], -1, DEFAULT_TOL),
        ("DFT (Spectral)", g["dft"], -1, DFT_TOL),
        ("Non-overlapping Template", None, -1, DEFAULT_TOL),  # quick/full split
        ("Overlapping Template", g["overlap"], -1, DEFAULT_TOL),
        ("Maurer's Universal", g["universal"], -1, DEFAULT_TOL),
        ("Linear Complexity", g["linear_complexity"], -1, DEFAULT_TOL),
        ("Serial", g["serial_p1"], g["serial_p2"], DEFAULT_TOL),
        ("Approximate Entropy", g["approximate_entropy"], -1, DEFAULT_TOL),
        ("Cumulative Sums (Fwd)", g["cusum_fwd"], -1, DEFAULT_TOL),
        ("Cumulative Sums (Bwd)", g["cusum_bwd"], -1, DEFAULT_TOL),
        ("Random Excursions", g["random_excursions_minp"], -1, DEFAULT_TOL),
        ("Random Excursions Variant", g["random_excursions_variant_minp"], -1,
         DEFAULT_TOL),
    ]

    out = []
    out.append("/*")
    out.append(" * NIST SP 800-22 KAT data + golden values. GENERATED FILE, do not edit.")
    out.append(" * Generator: tools/gen_kat_golden.py")
    out.append(" *")
    out.append(" * Data: first 2^20 bits of the binary expansion of e, computed exactly")
    out.append(" * and cross-validated against NIST sts-2.1.2 data/data.e (identical")
    out.append(" * except that file's 2-bit rounding tail).")
    out.append(" * Golden values: NIST sts-2.1.2 reference run over the same bits")
    out.append(" * (n=1048576, 1 stream, default parameters).")
    out.append(" */")
    out.append("")
    out.append('#include "nist_kat_data.h"')
    out.append("")
    out.append(f"const uint32_t nist_kat_n_bits = {N_BITS};")
    out.append("")
    out.append(f"const double nist_kat_nonoverlap_full_minp = "
               f"{g['nonoverlap_full_minp']:.6f};")
    out.append(f"const double nist_kat_nonoverlap_quick_minp = "
               f"{g['nonoverlap_quick_minp']:.6f};")
    out.append("")
    out.append("const nist_kat_expect_t nist_kat_expect[16] = {")
    for name, p1, p2, tol in entries:
        p1s = "-1.0" if p1 is None else f"{p1:.6f}"
        p2s = "-1.0" if p2 < 0 else f"{p2:.6f}"
        out.append(f'    {{ "{name}", {p1s}, {p2s}, {tol} }},')
    out.append("};")
    out.append("")
    out.append(f"const uint8_t nist_kat_e_bits[{N_BITS // 8}] = {{")
    for i in range(0, len(data), 16):
        out.append("    " + " ".join(f"0x{b:02x}," for b in data[i:i + 16]))
    out.append("};")
    out.append("")

    Path(args.out).write_text("\n".join(out))
    print(f"wrote {args.out}")
    for k in sorted(g):
        print(f"  {k:34s} {g[k]:.6f}")


if __name__ == "__main__":
    main()

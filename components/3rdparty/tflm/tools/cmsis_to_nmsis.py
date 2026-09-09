#!/usr/bin/env python3
"""Regenerate the NMSIS-NN kernel backend from TFLM's upstream CMSIS-NN one.

NMSIS-NN is Nuclei's RISC-V mirror of CMSIS-NN: same kernels, same signatures,
same semantics, with arm_/cmsis_nn_ renamed to riscv_/nmsis_nn_. Every one of
the 46 CMSIS-NN APIs the TFLM backend calls has an exact NMSIS-NN counterpart
(verified against NMSIS dc728b98), so this transform is purely mechanical -
which is why it is a script and not a hand-maintained fork.

Re-run after upgrading the vendored TFLM:

    python3 tools/cmsis_to_nmsis.py <tflite-micro>/tensorflow/lite/micro/kernels/cmsis_nn \\
                                    tensorflow/lite/micro/kernels/nmsis_nn
"""

import pathlib
import re
import sys

# Order matters: the longer, more specific patterns must be applied first, or
# e.g. "arm_cmsis_nn_status" would be half-rewritten by the bare "arm_" rule.
RULES = [
    (r'#include "Include/arm_nnfunctions\.h"', '#include "nmsis_nn/riscv_nnfunctions.h"'),
    (r'#include "Include/arm_nn_types\.h"', '#include "nmsis_nn/riscv_nn_types.h"'),
    (r'#include "Include/arm_nnsupportfunctions\.h"', '#include "nmsis_nn/riscv_nnsupportfunctions.h"'),
    (r'\bARM_CMSIS_NN_([A-Z_]+)\b', r'RISCV_NMSIS_NN_\1'),
    (r'\barm_cmsis_nn_status\b', 'riscv_nmsis_nn_status'),
    # Plain uppercase enum constants such as ARM_SIGMOID / ARM_TANH. Must run
    # after the ARM_CMSIS_NN_ rule above, which is the more specific prefix.
    (r'\bARM_([A-Z][A-Z_0-9]*)\b', r'RISCV_\1'),
    (r'\bCMSIS_NN_([A-Za-z_][A-Za-z_0-9]*)\b', r'NMSIS_NN_\1'),
    (r'\bcmsis_nn_([a-z_][a-z_0-9]*)\b', r'nmsis_nn_\1'),
    (r'\barm_([a-z_][a-z_0-9]*)\b', r'riscv_\1'),
]

BANNER = """// GENERATED - do not edit by hand.
// Produced from TFLM's cmsis_nn kernel backend by tools/cmsis_to_nmsis.py.
// See that script for why the CMSIS-NN -> NMSIS-NN mapping is mechanical.

"""


def convert(text: str) -> str:
    for pattern, repl in RULES:
        text = re.sub(pattern, repl, text)
    return text


def main() -> int:
    if len(sys.argv) != 3:
        print(__doc__)
        return 2

    src = pathlib.Path(sys.argv[1])
    dst = pathlib.Path(sys.argv[2])
    dst.mkdir(parents=True, exist_ok=True)

    count = 0
    for f in sorted(src.glob("*.cc")):
        out = dst / f.name
        body = convert(f.read_text(encoding="utf-8"))
        # Keep the upstream copyright header on top, banner right after it.
        out.write_text(BANNER + body, encoding="utf-8")
        count += 1

    leftovers = set()
    for f in sorted(dst.glob("*.cc")):
        leftovers |= set(re.findall(r"\b(?:arm_|cmsis_nn_|ARM_|CMSIS_NN_)[A-Za-z_0-9]*",
                                    f.read_text(encoding="utf-8")))
    if leftovers:
        print("WARNING: unconverted identifiers remain:")
        for x in sorted(leftovers):
            print("   ", x)
        return 1

    print(f"converted {count} files -> {dst}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

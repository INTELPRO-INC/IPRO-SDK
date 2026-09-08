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
Convert audio recognition weights from params.h to binary format.

Usage:
    python convert_audio_weights.py <params.h> <output.bin>

Binary output format (little-endian float32):
    W1[48][5120]       - 983,040 bytes
    b1[48]             - 192 bytes
    running_mean[48]   - 192 bytes
    running_var[48]    - 192 bytes
    W2[48][48]         - 9,216 bytes
    b2[48]             - 192 bytes
    Total: ~993 KB
"""

import re
import struct
import sys
import os

def parse_array(content, name, expected_size):
    """Parse a C array from the content."""
    # Find the array definition
    pattern = rf'float\s+{name}\s*\[[\d\]\[]*\]\s*=\s*\{{'
    match = re.search(pattern, content)
    if not match:
        print(f"ERROR: Array '{name}' not found!")
        return None

    start = match.end()

    # Find matching closing brace (handle nested braces)
    brace_count = 1
    pos = start
    while brace_count > 0 and pos < len(content):
        if content[pos] == '{':
            brace_count += 1
        elif content[pos] == '}':
            brace_count -= 1
        pos += 1

    array_content = content[start:pos-1]

    # Extract all floating point numbers
    numbers = re.findall(r'-?\d+\.?\d*(?:[eE][+-]?\d+)?', array_content)
    values = [float(x) for x in numbers]

    print(f"  {name}: found {len(values)} values (expected {expected_size})")

    if len(values) != expected_size:
        print(f"  WARNING: Size mismatch!")

    return values[:expected_size] if len(values) >= expected_size else values

def main():
    if len(sys.argv) < 3:
        print("Usage: python convert_audio_weights.py <params.h> <output.bin>")
        print("\nExample:")
        print("  python convert_audio_weights.py /path/to/params.h audio_weights.bin")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    print(f"=== Audio Weights Converter ===")
    print(f"Input:  {input_file}")
    print(f"Output: {output_file}")
    print()

    # Read params.h
    print("Reading params.h...")
    with open(input_file, 'r') as f:
        content = f.read()
    print(f"  File size: {len(content):,} bytes")
    print()

    # Define expected dimensions (optimized model)
    # POOL_DIM = 2*C*T = 2*128*20 = 5120
    EMB_SIZE = 48    # Optimized from 192
    POOL_DIM = 5120  # 2*C*T where C=128, T=20

    # Parse arrays
    print("Parsing arrays...")
    arrays = {}
    arrays['W1'] = parse_array(content, 'W1', EMB_SIZE * POOL_DIM)
    arrays['b1'] = parse_array(content, 'b1', EMB_SIZE)
    arrays['running_mean'] = parse_array(content, 'running_mean', EMB_SIZE)
    arrays['running_var'] = parse_array(content, 'running_var', EMB_SIZE)
    arrays['W2'] = parse_array(content, 'W2', EMB_SIZE * EMB_SIZE)
    arrays['b2'] = parse_array(content, 'b2', EMB_SIZE)
    print()

    # Check for errors
    for name, values in arrays.items():
        if values is None:
            print(f"ERROR: Failed to parse {name}")
            sys.exit(1)

    # Write binary file
    print(f"Writing binary file: {output_file}")
    with open(output_file, 'wb') as f:
        for name in ['W1', 'b1', 'running_mean', 'running_var', 'W2', 'b2']:
            values = arrays[name]
            # Pack as little-endian float32
            data = struct.pack(f'<{len(values)}f', *values)
            f.write(data)
            print(f"  {name}: {len(data):,} bytes")

    total_size = os.path.getsize(output_file)
    print()
    print(f"=== Done ===")
    print(f"Total size: {total_size:,} bytes ({total_size / 1024 / 1024:.2f} MB)")
    print()
    print("Copy the output file to SD card:")
    print(f"  cp {output_file} /sdcard/ai_model/audio_weights.bin")

if __name__ == '__main__':
    main()

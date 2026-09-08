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
Convert audio data from .h header file to raw PCM binary.

Usage:
    python convert_audio_input.py <input.h> <output.bin>

Example:
    python convert_audio_input.py wav_data_speaker1_0.h audio_input.bin

The .h file should contain an array like:
    int16_t audio_buffer[121619] = {...};
    or
    #define WAVE_DATA {...}

Output: Raw int16 PCM binary (little-endian)
"""

import re
import struct
import sys
import os

def parse_int16_array(content):
    """Parse int16 values from C array definition."""

    # Try to find array patterns
    patterns = [
        # Pattern 1: int16_t array_name[size] = {...}
        r'int16_t\s+\w+\s*\[[\d]*\]\s*=\s*\{([^}]+)\}',
        # Pattern 2: #define WAVE_DATA {...}
        r'#define\s+WAVE_DATA\s*\{([^}]+)\}',
        # Pattern 3: Generic array assignment
        r'=\s*\{([^}]+)\}',
    ]

    for pattern in patterns:
        match = re.search(pattern, content, re.DOTALL)
        if match:
            array_content = match.group(1)
            # Extract all integer numbers (including negative)
            numbers = re.findall(r'-?\d+', array_content)
            if numbers:
                print(f"Found {len(numbers)} values using pattern")
                return [int(x) for x in numbers]

    return None

def main():
    if len(sys.argv) < 3:
        print("Usage: python convert_audio_input.py <input.h> <output.bin>")
        print("\nExample:")
        print("  python convert_audio_input.py wav_data_speaker1_0.h audio_input.bin")
        print("\nThen copy to SD card:")
        print("  cp audio_input.bin /sdcard/ai_model/audio_input.bin")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    print(f"=== Audio Input Converter ===")
    print(f"Input:  {input_file}")
    print(f"Output: {output_file}")
    print()

    # Read header file
    print("Reading header file...")
    with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    print(f"  File size: {len(content):,} characters")
    print()

    # Parse array
    print("Parsing audio data...")
    values = parse_int16_array(content)

    if values is None or len(values) == 0:
        print("ERROR: Could not find audio data in header file!")
        print("Expected format: int16_t array[N] = {...} or #define WAVE_DATA {...}")
        sys.exit(1)

    print(f"  Found {len(values):,} samples")
    print(f"  Duration: {len(values) / 16000:.2f} seconds @ 16kHz")
    print()

    # Validate int16 range
    min_val = min(values)
    max_val = max(values)
    print(f"  Value range: [{min_val}, {max_val}]")

    if min_val < -32768 or max_val > 32767:
        print("WARNING: Values outside int16 range! Clipping...")
        values = [max(-32768, min(32767, v)) for v in values]

    # Write binary file
    print(f"\nWriting binary file: {output_file}")
    with open(output_file, 'wb') as f:
        for val in values:
            # Pack as little-endian int16
            f.write(struct.pack('<h', val))

    total_size = os.path.getsize(output_file)
    print(f"  Total size: {total_size:,} bytes")
    print()
    print(f"=== Done ===")
    print()
    print("To use with AI demo:")
    print(f"  1. Copy {output_file} to SD card as /sdcard/ai_model/audio_input.bin")
    print(f"  2. Set AUDIO_ENABLE_MFCC_PREPROCESS to 1 in demo_ai.c")
    print(f"  3. Run: ai_run_demo 4  (or ai_run_ext 4)")

if __name__ == '__main__':
    main()

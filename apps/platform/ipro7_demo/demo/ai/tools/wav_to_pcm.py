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
wav_to_pcm.py — Convert 16-bit mono WAV into the raw int16 PCM .bin that
ai_run_ext (AI_MODEL_SPEAKER_RECOG / AI_MODEL_AUDIO_RECOG) reads from the SD
card as /sdcard/ai_model/audio_input.bin.

The on-device fbank front-end consumes exactly the FIRST 300 frames:
    frame shift = 160 samples = 10 ms  ->  100 frames/sec
    300 frames  = 3.0 s  =  (300-1)*160 + 400 = 48240 samples
So the default trims each clip to 48240 samples (anything past ~3 s is ignored
by inference anyway). Use --full to keep the whole clip, or --seconds/--samples
to choose your own length.

Output is headerless little-endian int16 PCM (NOT a WAV).

Input requirements: PCM WAV, 16-bit. Mono is passed through; stereo is
downmixed (channel average). Sample rate should be 16000 Hz — a warning is
printed otherwise (this tool does NOT resample).

Usage:
    # single file -> single .bin
    python wav_to_pcm.py speaker1.wav -o audio_input.bin

    # single file, keep whole clip
    python wav_to_pcm.py speaker1.wav -o full.bin --full

    # custom length
    python wav_to_pcm.py speaker1.wav -o clip.bin --seconds 5
    python wav_to_pcm.py speaker1.wav -o clip.bin --samples 80000

    # batch: every *.wav in a dir -> <name>.bin in OUT dir
    python wav_to_pcm.py /path/to/wavs/ -o out_dir/

Then copy the .bin onto the SD card and run e.g.:
    ai_run_ext 5                              # identify (audio_input.bin)
    ai_run_ext 5 img_path /sdcard/ai_model/spk1_test.bin
"""

import argparse
import os
import struct
import sys

SAMPLE_RATE = 16000
DEFAULT_SAMPLES = 48240   # 300 fbank frames @ 16 kHz (3.015 s)


def read_wav_pcm16(path):
    """Return (pcm_int16_bytes_mono, sample_rate). Parses RIFF chunks; downmixes stereo."""
    with open(path, "rb") as f:
        b = f.read()
    if b[0:4] != b"RIFF" or b[8:12] != b"WAVE":
        raise ValueError(f"{path}: not a RIFF/WAVE file")

    fmt = None
    data = None
    i = 12
    while i + 8 <= len(b):
        cid = b[i:i + 4]
        csz = struct.unpack("<I", b[i + 4:i + 8])[0]
        body = b[i + 8:i + 8 + csz]
        if cid == b"fmt ":
            fmt = body
        elif cid == b"data":
            data = body
        i += 8 + csz + (csz & 1)   # chunks are word-aligned

    if fmt is None or data is None:
        raise ValueError(f"{path}: missing fmt/data chunk")

    audio_fmt, n_ch, fs, _byte_rate, _blk, bits = struct.unpack("<HHIIHH", fmt[:16])
    if audio_fmt != 1 or bits != 16:
        raise ValueError(f"{path}: only 16-bit PCM supported (fmt={audio_fmt} bits={bits})")

    if n_ch == 1:
        mono = data
    else:
        # downmix to mono by averaging channels
        import array
        s = array.array("h")
        s.frombytes(data[:len(data) - (len(data) % (2 * n_ch))])
        out = array.array("h", [0] * (len(s) // n_ch))
        for k in range(len(out)):
            out[k] = sum(s[k * n_ch:(k + 1) * n_ch]) // n_ch
        mono = out.tobytes()
    return mono, fs


def convert(in_path, out_path, n_samples):
    pcm, fs = read_wav_pcm16(in_path)
    if fs != SAMPLE_RATE:
        print(f"  WARNING: {os.path.basename(in_path)} is {fs} Hz, not {SAMPLE_RATE} Hz "
              f"(not resampled — feed 16 kHz audio for correct results)")
    total = len(pcm) // 2
    if n_samples is not None:
        pcm = pcm[:n_samples * 2]
    kept = len(pcm) // 2
    tag = ""
    if n_samples is not None and total < n_samples:
        tag = f"  (only {total} samples < {n_samples}; device fbank zero-pads)"
    with open(out_path, "wb") as f:
        f.write(pcm)
    print(f"  {os.path.basename(in_path):28s} -> {os.path.basename(out_path):20s} "
          f"{len(pcm):7d} bytes = {kept} samples ({kept/SAMPLE_RATE:.2f}s){tag}")


def main():
    ap = argparse.ArgumentParser(description="WAV -> raw int16 PCM .bin for ai_run_ext")
    ap.add_argument("inputs", nargs="+", help="WAV file(s) or a directory of WAVs")
    ap.add_argument("-o", "--out", required=True,
                    help="output .bin (single input) or output directory (multiple)")
    g = ap.add_mutually_exclusive_group()
    g.add_argument("--samples", type=int, help=f"keep first N samples (default {DEFAULT_SAMPLES})")
    g.add_argument("--seconds", type=float, help="keep first S seconds (S*16000 samples)")
    g.add_argument("--full", action="store_true", help="keep the entire clip")
    args = ap.parse_args()

    if args.full:
        n_samples = None
    elif args.seconds is not None:
        n_samples = int(round(args.seconds * SAMPLE_RATE))
    elif args.samples is not None:
        n_samples = args.samples
    else:
        n_samples = DEFAULT_SAMPLES

    # expand inputs (files or directories) to a wav list
    wavs = []
    for p in args.inputs:
        if os.path.isdir(p):
            wavs += [os.path.join(p, f) for f in sorted(os.listdir(p)) if f.lower().endswith(".wav")]
        else:
            wavs.append(p)
    if not wavs:
        sys.exit("no .wav inputs found")

    batch = len(wavs) > 1
    if batch:
        os.makedirs(args.out, exist_ok=True)

    print(f"Trim: {'full clip' if n_samples is None else f'{n_samples} samples'}  "
          f"({len(wavs)} file(s))")
    for w in wavs:
        if batch:
            out = os.path.join(args.out, os.path.splitext(os.path.basename(w))[0] + ".bin")
        else:
            out = args.out
        try:
            convert(w, out, n_samples)
        except ValueError as e:
            print(f"  SKIP {e}")


if __name__ == "__main__":
    main()

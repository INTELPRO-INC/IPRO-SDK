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
Create OTA package for IPRO7 boot2 OTA.

Format (matches the IPRO7 boot2 / ipro_iot_tool):
  [0-15]    Magic: "IPRO_OTA_Ver1.0\\0"
  [16-19]   Type:  "XZ  " or "RAW "
  [20-23]   Payload length (LE)
  [24-31]   Pad (0x01-0x08)
  [32-47]   HW version string
  [48-63]   FW version string
  [64-95]   SHA256 of payload
  [96-511]  Reserved (0xFF) — or, with --sign-key, a signature block
            (see apps/platform/common/ota/ota_sig.h):
            [96] "GSG1"  [100] product id  [104] hw id  [108] fw version
            [112] unpacked length  [116] key id
            [120-183] ECDSA-P256 r||s over SHA-256(header[0..120))
  [512+]    Payload (XZ compressed or raw FW combined image)

IMPORTANT: Input must be the combined FW image (with bootheader, starts with
"IPNP" magic), NOT the raw .bin from SDK build. Use the whole_img.bin generated
by ipro_iot_tool (in chips/ipro7/img_create_iot/whole_img.bin).

Usage:
  python3 create_ota.py <whole_img.bin> [--raw] [-o output.ota]
  python3 create_ota.py /path/to/iprocube/chips/ipro7/img_create_iot/whole_img.bin

By default creates XZ compressed OTA. Use --raw for uncompressed.
"""

import argparse
import hashlib
import lzma
import os
import struct
import subprocess
import sys
import tempfile

SIG_MAGIC = b"GSG1"

# Products that take signed packages, from ota_products.py next to this script:
#   PRODUCTS       name -> product id (must match OTA_PRODUCT_* in ota_sig.h)
#   VERSION_CONFIG name -> (.config path relative to the SDK root, version key)
# The firmware compares a package's version with its own build-time constant,
# so the package version comes from the same .config (one source of truth).
# Without that file, --sign-key has no product to sign for; unsigned packages
# are unaffected.
try:
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    from ota_products import PRODUCTS, VERSION_CONFIG
except ModuleNotFoundError as e:
    if e.name != "ota_products":
        raise
    PRODUCTS, VERSION_CONFIG = {}, {}
finally:
    sys.path.pop(0)


def config_version(product):
    rel, key = VERSION_CONFIG[product]
    path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", rel)
    with open(path) as f:
        for line in f:
            k, _, v = line.strip().partition("=")
            if k == key:
                return int(v, 0)
    raise SystemExit(f"ERROR: {key} not found in {rel}")


def _der_sig_to_rs(der):
    """ECDSA DER SEQUENCE{INTEGER r, INTEGER s} -> 64-byte r||s."""
    def read_int(buf, i):
        if buf[i] != 0x02:
            raise ValueError("bad DER integer")
        n = buf[i + 1]
        v = buf[i + 2:i + 2 + n]
        return int.from_bytes(v, "big"), i + 2 + n
    if der[0] != 0x30:
        raise ValueError("bad DER signature")
    i = 2 if der[1] < 0x80 else 2 + (der[1] & 0x7F)
    r, i = read_int(der, i)
    s, _ = read_int(der, i)
    return r.to_bytes(32, "big") + s.to_bytes(32, "big")


def sign_header(header, key_path, product_id, hw_id, fw_version, unpacked_len, key_id):
    """Fill the signature block of a 512-byte header (openssl does the ECDSA,
    so no Python crypto package is needed)."""
    h = bytearray(header)
    h[96:100] = SIG_MAGIC
    struct.pack_into("<IIIII", h, 100, product_id, hw_id, fw_version,
                     unpacked_len, key_id)
    with tempfile.TemporaryDirectory() as td:
        msg = os.path.join(td, "signed.bin")
        with open(msg, "wb") as f:
            f.write(bytes(h[0:120]))
        der = subprocess.run(["openssl", "dgst", "-sha256", "-sign", key_path, msg],
                             check=True, capture_output=True).stdout
    h[120:184] = _der_sig_to_rs(der)
    return bytes(h)


def create_ota_header(payload, use_xz, hw_ver="BFL_Module_v1.1", fw_ver="OTA_V1.0.0"):
    header = bytearray(512)

    # Magic (16 bytes)
    magic = b"IPRO_OTA_Ver1.0\x00"
    header[0:16] = magic

    # Type (4 bytes)
    header[16:20] = b"XZ  " if use_xz else b"RAW "

    # Payload length (4 bytes LE)
    struct.pack_into("<I", header, 20, len(payload))

    # Pad (8 bytes)
    header[24:32] = bytes([0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08])

    # HW version (16 bytes)
    hw = hw_ver.encode("utf-8")[:15] + b"\x00"
    header[32:32+len(hw)] = hw

    # FW version (16 bytes)
    fw = fw_ver.encode("utf-8")[:15] + b"\x00"
    header[48:48+len(fw)] = fw

    # SHA256 of payload (32 bytes)
    sha = hashlib.sha256(payload).digest()
    header[64:96] = sha

    # Fill rest with 0xFF
    for i in range(96, 512):
        header[i] = 0xFF

    return bytes(header)


def main():
    parser = argparse.ArgumentParser(description="Create IPRO7 OTA package")
    parser.add_argument("input", help="Input firmware binary (.bin)")
    parser.add_argument("--raw", action="store_true", help="No compression (RAW)")
    parser.add_argument("-o", "--output", help="Output file (default: <input>.xz.ota or <input>.ota)")
    parser.add_argument("--hw-ver", default="BFL_Module_v1.1", help="HW version string")
    parser.add_argument("--fw-ver", default="OTA_V1.0.0", help="FW version string")
    parser.add_argument("--sign-key", help="ECDSA P-256 private key (PEM) — adds the "
                        "signature block; keep the key OUTSIDE the repo")
    parser.add_argument("--product", choices=sorted(PRODUCTS),
                        help="product the package is for (required with --sign-key)")
    parser.add_argument("--hw-id", type=lambda v: int(v, 0), default=0,
                        help="hardware id (0 = any)")
    parser.add_argument("--fw-version", type=lambda v: int(v, 0),
                        help="monotonic firmware version (default/must equal the "
                        "version key in the product's .config)")
    parser.add_argument("--force-version", action="store_true",
                        help="allow --fw-version to differ from .config "
                        "(test packages only: rollback / anti-rollback checks)")
    parser.add_argument("--key-id", type=lambda v: int(v, 0), default=1,
                        help="id of the signing key in the device key table")
    args = parser.parse_args()

    with open(args.input, "rb") as f:
        fw_data = f.read()

    print(f"Input: {args.input} ({len(fw_data)} bytes)")

    # A signed package is for a known product whose boot2 wants the IPNP
    # combined image: wrap a raw SDK .bin the same way ipro_dl.py does.
    if args.sign_key and not PRODUCTS:
        print("ERROR: --sign-key: no signed-OTA products are configured "
              "(ota_products.py next to this script)")
        sys.exit(1)

    if args.sign_key and fw_data[:4] != b"IPNP":
        sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
        from ipro_dl import ipnp_wrap
        fw_data = ipnp_wrap(fw_data)
        print(f"Wrapped raw .bin in an IPNP bootheader ({len(fw_data)} bytes)")

    # Validate input is a combined image (with bootheader)
    if fw_data[:4] != b"IPNP":
        print("WARNING: Input does not start with IPNP bootheader magic.")
        print("  OTA payload must be the combined FW image (whole_img.bin),")
        print("  not the raw .bin from SDK build.")
        print("  Use: iprocube/chips/ipro7/img_create_iot/whole_img.bin")
        sys.exit(1)

    if args.raw:
        payload = fw_data
        suffix = ".ota"
        print(f"Mode: RAW (no compression)")
    else:
        xz_filters = [{"id": lzma.FILTER_LZMA2, "dict_size": 32768}]
        payload = lzma.compress(fw_data, format=lzma.FORMAT_XZ,
                                check=lzma.CHECK_CRC32, filters=xz_filters)
        suffix = ".xz.ota"
        ratio = len(payload) / len(fw_data) * 100
        print(f"Mode: XZ ({len(fw_data)} -> {len(payload)} bytes, {ratio:.1f}%)")

    header = create_ota_header(payload, not args.raw, args.hw_ver, args.fw_ver)
    if args.sign_key:
        if args.product is None:
            print("ERROR: --sign-key needs --product")
            sys.exit(1)
        built = config_version(args.product)
        if args.fw_version is None:
            args.fw_version = built
        elif args.fw_version != built and not args.force_version:
            # A package claiming another version than the image it carries
            # would defeat (or wrongly trip) the device's anti-rollback check.
            print(f"ERROR: --fw-version {args.fw_version} != {built} in "
                  f"{VERSION_CONFIG[args.product][0]} (the image's own version)")
            sys.exit(1)
        header = sign_header(header, args.sign_key, PRODUCTS[args.product],
                             args.hw_id, args.fw_version, len(fw_data), args.key_id)
        print(f"Signed: product={args.product} fw_version={args.fw_version} "
              f"hw_id={args.hw_id:#x} key_id={args.key_id} unpacked={len(fw_data)}")

    if args.output:
        out_path = args.output
    else:
        base = os.path.splitext(args.input)[0]
        out_path = base + suffix

    with open(out_path, "wb") as f:
        f.write(header)
        f.write(payload)

    total = len(header) + len(payload)
    sha_hex = hashlib.sha256(payload).hexdigest()[:16]
    print(f"Output: {out_path} ({total} bytes, header={len(header)}, payload={len(payload)})")
    print(f"SHA256: {sha_hex}...")


if __name__ == "__main__":
    main()

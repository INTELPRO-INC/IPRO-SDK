#!/usr/bin/env python3
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

"""IPRO u-boot host downloader.

Drives the boot2 USB CDC-ACM download protocol once the board is in rescue
mode (blank FW / forced via HBN magic). Pure stdlib — termios for serial,
hashlib for SHA256, no pyserial dependency.

Subcommands:
  info              handshake + basic connectivity check
  erase             erase a range [start, end] inclusive
  flash             erase + write + verify (SHA256) + optional boot
  read              read a range back to a file
  boot              send FLASH_BOOT (exit rescue, jump to FW)
  reset             send the RESET command (board reboots)
  monitor           115200 serial monitor, matches tools/serial_monitor.py

Protocol reference: apps/platform/ipro7_uboot_boot2/boot2_usb_download.c
"""
from __future__ import annotations

import argparse
import errno
import hashlib
import os
import select
import struct
import sys
import termios
import time
import zlib
from dataclasses import dataclass
from pathlib import Path

# ---- Protocol constants (must match boot2_usb_download.c) ---------------
HANDSHAKE_MAGIC     = bytes([0x42, 0x32, 0x55, 0x53])   # 'B','2','U','S' == 0x53553242 LE
HANDSHAKE_ACK       = b"OK\x00\x00"

CMD_RESET           = 0x21
CMD_FLASH_ERASE     = 0x30
CMD_FLASH_WRITE     = 0x31
CMD_FLASH_READ      = 0x32
CMD_FLASH_BOOT      = 0x33
CMD_FLASH_WRITE_CHK = 0x3A
CMD_FLASH_READSHA   = 0x3D
CMD_DIAG            = 0x40

# Stage names, must match uboot_stage_t in boot2_usb_download.c
UBOOT_STAGES = [
    "IDLE", "RX_WAIT", "RX_GOT", "PARSE", "DISPATCH",
    "FLASH_WRITE", "ACK", "DONE",
]

CMD_DATA_MAX_LEN    = 4092
# Defaults assume the boot2 with CONFIG_BOOT2_CDC_PACE_US > 0 — boot2 inserts
# a 500 μs delay after each FLASH_WRITE ACK to dodge macOS' CDC stall recovery
# under sustained transfers. With that fix on the device side we can run flat
# out from the host: 2048 B blocks, no inter-block sleep, no retries needed.
# Throughput ~150-180 KB/s.
WRITE_BLOCK_SIZE    = 2048
WRITE_RETRIES       = 1                     # first attempt only
INTER_BLOCK_SLEEP   = 0.0                   # device-side pacing handles it
READ_BLOCK_SIZE     = CMD_DATA_MAX_LEN      # reply fits CMD_DATA_MAX_LEN bytes

# boot2 rescue CDC identity — see apps/platform/ipro7_uboot_boot2/boot2_usb_download.c
# VID 0x3896 PID 0x0701, serial string "00000001". macOS embeds the serial in
# /dev/cu.usbmodem<serial><iface>, so any rescue port contains this substring.
BOOT2_RESCUE_SERIAL_PREFIX = "00000001"


# ---- IPNP wrapper (self-contained replacement for iprocube img_create) -----
#
# Boot2 expects FW partitions to begin with a 240-byte IPNP bootheader; the
# actual code starts at the "group_image_offset" the header declares (0x1000
# in the pre-built template below). The template is a byte-accurate snapshot
# of the header iprocube's ipro_img_create generates for ipro7 + u-boot
# partition layout (partition_cfg_1M_uboot.toml), with the three app-specific
# fields zeroed so we can patch them per call:
#
#   [0x78] img_len_cnt    uint32 LE — length of the 16-byte-aligned payload
#   [0x84] hash           32 bytes  — SHA-256 of that payload
#   [0xEC] header_crc     uint32 LE — CRC-32/IEEE over bytes [0..0xEC)
#
# The remaining bytes encode the ipro7 SF flash config, system clock config,
# AES/signing flags, PT addresses (0x10000 / 0x11000 for u-boot layout), and
# CPU-patches — all chip-specific constants that don't depend on the app.
#
# If you change partition layout, AES/signing, or target chip, regenerate
# this template by rebuilding via iprocube and re-extracting the first 240
# bytes of whole_img.bin with [0x78], [0x84..0x84+32], and [0xEC] zeroed.
IPNP_TEMPLATE = bytes.fromhex(
    "49504e500100000046434647040101016699ff039f009f0004c80001c72052d8"
    "060232000b010b013b01bb006b01eb02eb02025000010001010002010201ab01"
    "053500000101000038ffa0ff77030240770302f02c01b004b0040500ffff1400"
    "6cd95fd650434647010400010100000031b51bd8100308000000000000000000"
    "0010000000000000000000000000000000000000000000000000000000000000"
    "000000000000010000100100f000000000000000000000000000000000000000"
    "0000000000000000000000000000000000000000000000000000000000000000"
    "00000000000000000000000000000000"
)
IPNP_HEADER_SIZE      = 240
IPNP_IMAGE_OFFSET     = 0x1000   # group_image_offset baked into the template
IPNP_PAYLOAD_ALIGN    = 16       # iprocube pads the raw .bin up to this
IPNP_MAGIC_BYTES      = b"IPNP"


def ipnp_wrap(raw_bin: bytes) -> bytes:
    """Produce the 192-ish KB image boot2 expects from a raw ELF-derived .bin.

    Layout: [240-byte hdr] + [0xFF pad to 0x1000] + [raw_bin padded to 16 B].
    Byte-for-byte equivalent to iprocube's ipro_iot_tool --build output for
    the default ipro7 u-boot partition layout.
    """
    if not raw_bin:
        raise ValueError("ipnp_wrap: empty input")
    pad_len = (-len(raw_bin)) % IPNP_PAYLOAD_ALIGN
    image = raw_bin + b"\xFF" * pad_len

    hdr = bytearray(IPNP_TEMPLATE)
    struct.pack_into("<I", hdr, 0x78, len(image))
    hdr[0x84:0x84 + 32] = hashlib.sha256(image).digest()
    crc = zlib.crc32(bytes(hdr[:0xEC])) & 0xFFFFFFFF
    struct.pack_into("<I", hdr, 0xEC, crc)

    pad = b"\xFF" * (IPNP_IMAGE_OFFSET - IPNP_HEADER_SIZE)
    return bytes(hdr) + pad + image


def _looks_ipnp_wrapped(data: bytes) -> bool:
    return len(data) >= 4 and data[:4] == IPNP_MAGIC_BYTES

# Error code names (for friendly output)
ERR_NAMES = {
    0x0002: "FLASH_ERASE_PARA (protected range?)",
    0x0003: "FLASH_ERASE",
    0x0004: "FLASH_WRITE_PARA",
    0x0005: "FLASH_WRITE_ADDR (protected range?)",
    0x0006: "FLASH_WRITE",
    0x0101: "CMD_ID (unknown command)",
    0x0102: "CMD_LEN (data length out of bounds)",
    0x0103: "CMD_CRC (checksum mismatch)",
    0x0104: "CMD_SEQ",
}


# ---- Serial port helpers ------------------------------------------------

@dataclass
class Port:
    fd: int
    name: str

    def close(self) -> None:
        try:
            os.close(self.fd)
        except OSError:
            pass


def open_cdc(port_path: str, baud: int = 115200,
             retries: int = 8, retry_delay: float = 0.15) -> Port:
    """Open a CDC-ACM port in raw, non-blocking, 8N1 mode.

    CDC-ACM ignores the actual baud rate we set, but we still need valid
    termios attrs to put the port in raw mode. Transient EBUSY is common
    on macOS when a previous fd on the same /dev/cu.usbmodem* was just
    closed (the USB driver holds the device for ~100 ms) — retry a few
    times with a short sleep before giving up.
    """
    baud_map = {
        9600: termios.B9600, 19200: termios.B19200, 38400: termios.B38400,
        57600: termios.B57600, 115200: termios.B115200, 230400: termios.B230400,
    }
    baud_const = baud_map.get(baud, termios.B115200)

    last_err: OSError | None = None
    for attempt in range(retries):
        try:
            fd = os.open(port_path, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
        except OSError as e:
            last_err = e
            if e.errno in (errno.EBUSY, errno.ENOENT, errno.ENXIO, errno.EAGAIN):
                time.sleep(retry_delay)
                continue
            raise
        try:
            attrs = termios.tcgetattr(fd)
            attrs[0] = 0                                  # iflag
            attrs[1] = 0                                  # oflag
            attrs[2] = termios.CS8 | termios.CREAD | termios.CLOCAL
            attrs[3] = 0                                  # lflag
            attrs[4] = baud_const                         # ispeed
            attrs[5] = baud_const                         # ospeed
            attrs[6][termios.VMIN]  = 0
            attrs[6][termios.VTIME] = 1
            termios.tcsetattr(fd, termios.TCSANOW, attrs)
            termios.tcflush(fd, termios.TCIOFLUSH)
        except Exception:
            os.close(fd)
            raise
        return Port(fd=fd, name=port_path)

    assert last_err is not None
    raise last_err


def write_all(port: Port, data: bytes) -> None:
    """Write `data` in one or more chunks, honouring EAGAIN. Drains at the
    end so the OS USB layer has actually handed everything to the device
    before we expect a reply."""
    mv = memoryview(data)
    while mv:
        _, ready, _ = select.select([], [port.fd], [], 5.0)
        if not ready:
            raise TimeoutError(f"write timed out on {port.name}")
        try:
            n = os.write(port.fd, mv)
        except BlockingIOError:
            continue
        mv = mv[n:]
    try:
        termios.tcdrain(port.fd)
    except OSError:
        # tcdrain occasionally raises on transient USB hiccups; the next
        # read will surface any real problem, so don't treat this as fatal.
        pass


def read_exact(port: Port, nbytes: int, timeout: float = 10.0) -> bytes:
    """Read exactly `nbytes` or raise TimeoutError. Treats transient ENXIO
    (device briefly gone after a USB hiccup) as retryable within the timeout
    window — boot2 and macOS' CDC driver can both drop the fd for a few ms
    under sustained load, but the device comes right back."""
    buf = bytearray()
    deadline = time.monotonic() + timeout
    while len(buf) < nbytes:
        remaining = deadline - time.monotonic()
        if remaining <= 0:
            raise TimeoutError(
                f"read timed out waiting for {nbytes} bytes, got {len(buf)}"
            )
        try:
            ready, _, _ = select.select([port.fd], [], [], min(1.0, remaining))
        except OSError as e:
            # select itself can fail if the fd is invalidated by USB reset
            if e.errno in (errno.EBADF, errno.ENXIO):
                raise
            time.sleep(0.01)
            continue
        if not ready:
            continue
        try:
            chunk = os.read(port.fd, nbytes - len(buf))
        except BlockingIOError:
            continue
        except OSError as e:
            if e.errno in (errno.EAGAIN,):
                continue
            if e.errno == errno.ENXIO:
                # USB stack dropped the fd briefly — wait and retry
                time.sleep(0.02)
                continue
            raise
        if chunk:
            buf.extend(chunk)
    return bytes(buf)


# ---- Protocol framing ---------------------------------------------------

class ProtocolError(RuntimeError):
    pass


def send_cmd(port: Port, cmd_id: int, data: bytes = b"") -> None:
    if len(data) > CMD_DATA_MAX_LEN:
        raise ValueError(f"data too long: {len(data)} > {CMD_DATA_MAX_LEN}")
    data_len = len(data)
    len_lo = data_len & 0xFF
    len_hi = (data_len >> 8) & 0xFF
    chk = (len_lo + len_hi + sum(data)) & 0xFF
    frame = bytes([cmd_id, chk, len_lo, len_hi]) + data
    write_all(port, frame)


def read_response(port: Port, timeout: float = 10.0) -> bytes:
    """Read a response frame and return its payload bytes.

    Raises ProtocolError on 'FL' + err or unknown header.
    Returns b"" for bare "OK", or the variable-length data for ACK-with-data.
    """
    hdr = read_exact(port, 2, timeout=timeout)
    if hdr == b"OK":
        # May be followed by len+data or nothing. Peek by trying a short read.
        # Boot2 always sends at least 4 bytes for handshake ack, 2 bytes for bare
        # command ack, and 4+N for read-data. We disambiguate by the command
        # context — caller decides whether to read more via read_response_data.
        return b""
    if hdr == b"FL":
        err_bytes = read_exact(port, 2, timeout=timeout)
        err = err_bytes[0] | (err_bytes[1] << 8)
        name = ERR_NAMES.get(err, f"0x{err:04X}")
        raise ProtocolError(f"board returned error {name}")
    raise ProtocolError(f"unknown response header {hdr!r}")


def read_ack(port: Port, timeout: float = 10.0) -> None:
    """Expect a bare 'OK' response (2 bytes)."""
    _ = read_response(port, timeout=timeout)


def read_ack_with_data(port: Port, timeout: float = 10.0) -> bytes:
    """Expect 'OK' + len_le16 + data and return data bytes."""
    _ = read_response(port, timeout=timeout)
    len_bytes = read_exact(port, 2, timeout=timeout)
    data_len = len_bytes[0] | (len_bytes[1] << 8)
    if data_len == 0:
        return b""
    return read_exact(port, data_len, timeout=timeout)


def read_handshake_ack(port: Port, timeout: float = 10.0) -> None:
    """Handshake reply is 4 bytes: 'OK\\x00\\x00'."""
    ack = read_exact(port, 4, timeout=timeout)
    if ack != HANDSHAKE_ACK:
        raise ProtocolError(f"handshake reply mismatch: {ack!r}")


# ---- High-level operations ----------------------------------------------

def handshake(port: Port, timeout: float = 10.0) -> None:
    write_all(port, HANDSHAKE_MAGIC)
    read_handshake_ack(port, timeout=timeout)


def flash_erase(port: Port, start: int, end_inclusive: int, timeout: float = 30.0) -> None:
    data = struct.pack("<II", start, end_inclusive)
    send_cmd(port, CMD_FLASH_ERASE, data)
    read_ack(port, timeout=timeout)


def flash_write(port: Port, addr: int, data: bytes, timeout: float = 10.0) -> None:
    send_cmd(port, CMD_FLASH_WRITE, struct.pack("<I", addr) + data)
    read_ack(port, timeout=timeout)


def flash_read(port: Port, addr: int, length: int) -> bytes:
    out = bytearray()
    while length > 0:
        chunk = min(READ_BLOCK_SIZE, length)
        send_cmd(port, CMD_FLASH_READ, struct.pack("<II", addr, chunk))
        data = read_ack_with_data(port)
        if len(data) != chunk:
            raise ProtocolError(
                f"FLASH_READ short reply: asked {chunk}, got {len(data)}"
            )
        out.extend(data)
        addr += chunk
        length -= chunk
    return bytes(out)


def flash_read_sha(port: Port, addr: int, length: int) -> bytes:
    send_cmd(port, CMD_FLASH_READSHA, struct.pack("<II", addr, length))
    data = read_ack_with_data(port, timeout=60.0)
    if len(data) != 32:
        raise ProtocolError(f"FLASH_READSHA returned {len(data)} bytes, expected 32")
    return data


def read_diag(port: Port, timeout: float = 3.0) -> dict:
    """Query boot2's post-mortem diagnostic block.

    Layout must match uboot_diag_t in boot2_usb_download.c.
    """
    send_cmd(port, CMD_DIAG)
    data = read_ack_with_data(port, timeout=timeout)
    if len(data) < 56:  # head fields up to nack_count
        raise ProtocolError(f"DIAG reply too short: {len(data)} bytes")
    fields = struct.unpack_from("<14I", data, 0)
    stage = fields[3]
    stage_name = UBOOT_STAGES[stage] if stage < len(UBOOT_STAGES) else f"?({stage})"
    return {
        "magic":            fields[0],
        "version":          fields[1],
        "boot_count":       fields[2],
        "stage":            stage,
        "stage_name":       stage_name,
        "last_cmd_id":      fields[4],
        "cmds_total":       fields[5],
        "writes_started":   fields[6],
        "writes_completed": fields[7],
        "last_write_addr":  fields[8],
        "last_write_len":   fields[9],
        "last_flash_us":    fields[10],
        "max_flash_us":     fields[11],
        "last_err":         fields[12],
        "nack_count":       fields[13],
    }


def flash_boot(port: Port) -> None:
    send_cmd(port, CMD_FLASH_BOOT)
    read_ack(port)


def flash_reset(port: Port) -> None:
    send_cmd(port, CMD_RESET)
    # Board resets after sending ACK — don't assume the reply survives the link
    try:
        read_ack(port, timeout=2.0)
    except (TimeoutError, ProtocolError, OSError):
        pass


# ---- Monitor ------------------------------------------------------------

def monitor_loop(port: Port) -> None:
    print(f"[ipro_dl] monitor on {port.name} — Ctrl+C to exit", file=sys.stderr)
    try:
        while True:
            ready, _, _ = select.select([port.fd], [], [], 1.0)
            if not ready:
                continue
            try:
                chunk = os.read(port.fd, 1024)
            except BlockingIOError:
                continue
            except OSError as e:
                print(f"\n[ipro_dl] port error: {e}", file=sys.stderr)
                break
            if chunk:
                sys.stdout.buffer.write(chunk)
                sys.stdout.buffer.flush()
    except KeyboardInterrupt:
        print("\n[ipro_dl] monitor stopped", file=sys.stderr)


# ---- Subcommand handlers ------------------------------------------------

def cmd_info(args: argparse.Namespace) -> int:
    port = open_cdc(args.port)
    try:
        handshake(port)
        print(f"handshake OK on {port.name}")
    finally:
        port.close()
    return 0


def cmd_erase(args: argparse.Namespace) -> int:
    start, end_inclusive = int(args.start, 0), int(args.end, 0)
    if end_inclusive < start:
        print("error: end must be >= start", file=sys.stderr)
        return 2
    port = open_cdc(args.port)
    try:
        handshake(port)
        print(f"erase [0x{start:X}, 0x{end_inclusive:X}]")
        flash_erase(port, start, end_inclusive)
        print("erase OK")
    finally:
        port.close()
    return 0


def cmd_flash(args: argparse.Namespace) -> int:
    # --wait-rescue: scan for rescue, handshake, and reuse the open port for
    # the rest of the flash. This avoids the close/reopen race that can leave
    # boot2 in an inconsistent state or trip macOS' CDC EBUSY window.
    if args.wait_rescue > 0:
        try:
            pre_opened = _wait_rescue_port(
                timeout=args.wait_rescue,
                match=args.match,
                any_port=args.any_port,
            )
        except TimeoutError:
            print("[ipro_dl] no rescue handshake within --wait-rescue window",
                  file=sys.stderr)
            return 1
    else:
        # Refuse to flash onto an app CDC — the handshake would hang and
        # confuse whichever app is running. --force-port bypasses this.
        if not args.force_port and BOOT2_RESCUE_SERIAL_PREFIX not in args.port:
            other = [p for p in _find_usbmodem_ports()
                     if BOOT2_RESCUE_SERIAL_PREFIX in p]
            print(f"error: selected port {args.port} doesn't look like a boot2 "
                  f"rescue CDC (expected path containing "
                  f"'{BOOT2_RESCUE_SERIAL_PREFIX}').", file=sys.stderr)
            if other:
                print(f"       did you mean --port {other[0]}?", file=sys.stderr)
            else:
                print(f"       reboot the board into rescue first — run "
                      f"'ipro_dl wait-rescue' and power-cycle or send REBOOT, "
                      f"or pass --wait-rescue SECONDS to combine both steps.",
                      file=sys.stderr)
            print(f"       (override with --force-port)", file=sys.stderr)
            return 2
        pre_opened = None

    raw = open(args.file, "rb").read()
    if not raw:
        print("error: input file is empty", file=sys.stderr)
        if pre_opened:
            pre_opened.close()
        return 2

    # Auto-wrap: boot2 expects an IPNP-wrapped payload at the FW partition.
    # If the input is a raw app .bin (no IPNP magic), wrap it in-process so
    # the user doesn't need iprocube installed.
    if _looks_ipnp_wrapped(raw):
        payload = raw
        if args.wrap:
            print("[ipro_dl] note: --wrap requested but input already IPNP-wrapped, skipping",
                  file=sys.stderr)
    elif args.raw:
        print("[ipro_dl] warning: --raw set; flashing bytes as-is (no IPNP header)",
              file=sys.stderr)
        payload = raw
    else:
        print(f"[ipro_dl] input looks raw (no IPNP magic) — wrapping in-process")
        payload = ipnp_wrap(raw)
        if args.save_wrapped:
            Path(args.save_wrapped).write_bytes(payload)
            print(f"[ipro_dl] wrote wrapped image to {args.save_wrapped}")

    addr = int(args.addr, 0)
    end_inclusive = addr + len(payload) - 1

    # Round erase range up to 4KB sector boundary for safety
    sector = 0x1000
    erase_end = ((end_inclusive // sector) + 1) * sector - 1

    if pre_opened is not None:
        port = pre_opened
        t0 = time.monotonic()
    else:
        port = open_cdc(args.port)
        t0 = time.monotonic()
    try:
        if pre_opened is None:
            print(f"[ipro_dl] opening {port.name}")
            handshake(port)
            print(f"[ipro_dl] handshake OK")

        print(f"[ipro_dl] erase  0x{addr:X}..0x{erase_end:X}"
              f" ({(erase_end - addr + 1) // 1024} KB)")
        flash_erase(port, addr, erase_end)

        total = len(payload)
        written = 0
        block_size = args.block_size
        print(f"[ipro_dl] write  {total} bytes in {block_size}-byte blocks")
        retries = args.retries
        slowest_ms = 0.0
        slowest_addr = 0
        blocks_done = 0
        while written < total:
            chunk = payload[written : written + block_size]
            last_err: Exception | None = None
            for attempt in range(1, retries + 1):
                t_block = time.monotonic()
                try:
                    flash_write(port, addr + written, chunk)
                    dt_ms = (time.monotonic() - t_block) * 1000.0
                    if args.trace_slow_ms > 0 and dt_ms >= args.trace_slow_ms:
                        sys.stderr.write(
                            f"\n[ipro_dl] slow @0x{addr + written:X} "
                            f"block {blocks_done} took {dt_ms:.1f} ms\n"
                        )
                    if dt_ms > slowest_ms:
                        slowest_ms = dt_ms
                        slowest_addr = addr + written
                    last_err = None
                    break
                except (TimeoutError, ProtocolError) as e:
                    last_err = e
                    if retries > 1:
                        sys.stderr.write(
                            f"\n[ipro_dl] write @0x{addr + written:X} attempt "
                            f"{attempt}/{retries} failed: {e}\n"
                        )
                        time.sleep(0.1)
            if last_err is not None:
                sys.stderr.write(
                    f"\n[ipro_dl] last successful block took "
                    f"{slowest_ms:.1f} ms @0x{slowest_addr:X}\n"
                )
                # Try to pull the boot2 diag block so we see where it hung.
                # Device is probably still stuck in flash_write; give it some
                # time to recover (WDT or finish flash op) before asking.
                try:
                    sys.stderr.write(
                        "[ipro_dl] attempting post-mortem diag read "
                        "(may wait for AON WDT, up to ~20 s)...\n"
                    )
                    d = read_diag(port, timeout=20.0)
                    sys.stderr.write("\n[ipro_dl] boot2 diag after failure:\n")
                    _print_diag(d, prefix="    ")
                except Exception as diag_err:
                    sys.stderr.write(
                        f"[ipro_dl] diag read also failed: {diag_err}\n"
                    )
                raise last_err
            written += len(chunk)
            blocks_done += 1
            pct = 100 * written // total
            sys.stderr.write(
                f"\r[ipro_dl] write  {written}/{total} ({pct}%)"
            )
            sys.stderr.flush()
            if args.inter_block_ms > 0:
                time.sleep(args.inter_block_ms / 1000.0)
        sys.stderr.write("\n")

        if not args.no_verify:
            print(f"[ipro_dl] verify SHA256 over {total} bytes...")
            remote = flash_read_sha(port, addr, total)
            local = hashlib.sha256(payload).digest()
            if remote != local:
                print(f"[ipro_dl] VERIFY FAIL")
                print(f"  local:  {local.hex()}")
                print(f"  remote: {remote.hex()}")
                return 3
            print(f"[ipro_dl] verify OK ({local.hex()[:16]}...)")

        elapsed = time.monotonic() - t0
        kbps = (total / 1024) / max(elapsed, 0.001)
        print(f"[ipro_dl] done in {elapsed:.1f}s ({kbps:.1f} KB/s)")

        if args.boot:
            print(f"[ipro_dl] sending BOOT — device will reset and jump to FW")
            flash_boot(port)
    finally:
        port.close()
    return 0


def cmd_read(args: argparse.Namespace) -> int:
    addr, length = int(args.addr, 0), int(args.length, 0)
    port = open_cdc(args.port)
    try:
        handshake(port)
        print(f"[ipro_dl] read   0x{addr:X} ({length} bytes)")
        data = flash_read(port, addr, length)
        with open(args.output, "wb") as f:
            f.write(data)
        print(f"[ipro_dl] wrote  {args.output} ({len(data)} bytes)")
    finally:
        port.close()
    return 0


def cmd_boot(args: argparse.Namespace) -> int:
    port = open_cdc(args.port)
    try:
        handshake(port)
        flash_boot(port)
        print("[ipro_dl] boot sent")
    finally:
        port.close()
    return 0


def cmd_reset(args: argparse.Namespace) -> int:
    port = open_cdc(args.port)
    try:
        handshake(port)
        flash_reset(port)
        print("[ipro_dl] reset sent")
    finally:
        port.close()
    return 0


def _wait_rescue_port(timeout: float, match: str, any_port: bool,
                      quiet: bool = False) -> Port:
    """Scan for a boot2 rescue CDC, handshake, and return the live Port.

    Caller owns the returned Port and is responsible for closing it. Raises
    TimeoutError if no rescue handshake within `timeout` seconds.
    """
    deadline = time.monotonic() + timeout
    target_label = (
        "any /dev/cu.usbmodem*" if any_port
        else f"/dev/cu.usbmodem*{match}*"
    )
    if not quiet:
        print(f"[ipro_dl] scanning {target_label} for rescue handshake "
              f"(timeout {timeout}s)...")

    last_seen: set[str] = set()
    while time.monotonic() < deadline:
        all_ports = _find_usbmodem_ports()
        candidates = all_ports if any_port else _filter_rescue(all_ports, match)
        current = set(candidates)
        if not quiet:
            for p in sorted(current - last_seen):
                print(f"[ipro_dl]   + {p}")
            for p in sorted(last_seen - current):
                print(f"[ipro_dl]   - {p}")
        last_seen = current

        for p in candidates:
            try:
                port = open_cdc(p)
            except OSError:
                continue
            try:
                handshake(port, timeout=0.5)
                if not quiet:
                    print(f"[ipro_dl] rescue handshake OK on {p}")
                return port
            except (TimeoutError, ProtocolError):
                port.close()
        time.sleep(0.2)

    # Timeout — leave a breadcrumb for the user about ports we ignored.
    if not any_port:
        leftover = [p for p in _find_usbmodem_ports() if match not in p]
        if leftover and not quiet:
            print(f"[ipro_dl] note: skipped non-rescue ports: "
                  f"{', '.join(leftover)} (retry with --any-port to include)",
                  file=sys.stderr)
    raise TimeoutError("no rescue handshake within timeout")


def cmd_wait_rescue(args: argparse.Namespace) -> int:
    """Poll for boot2 rescue CDC and complete its handshake.

    Filters by the rescue serial string so we only touch ports that look
    like they belong to boot2 — app CDCs (different serial, different VID/PID)
    are left alone. `--any-port` opts out of the filter and tries everything.
    """
    try:
        port = _wait_rescue_port(args.timeout, args.match, args.any_port)
    except TimeoutError:
        print("[ipro_dl] no rescue handshake within timeout", file=sys.stderr)
        return 1
    port.close()
    return 0


def _print_diag(d: dict, prefix: str = "") -> None:
    magic_ok = (d["magic"] == 0x44494147)  # 'DIAG'
    print(f"{prefix}magic             0x{d['magic']:08X} "
          f"{'OK' if magic_ok else 'INVALID — diag not initialised?'}")
    if not magic_ok:
        return
    print(f"{prefix}version           {d['version']}")
    print(f"{prefix}boot_count        {d['boot_count']}  "
          f"(rescue sessions since power on)")
    print(f"{prefix}stage             {d['stage']} = {d['stage_name']}")
    print(f"{prefix}last_cmd_id       0x{d['last_cmd_id']:02X}")
    print(f"{prefix}cmds_total        {d['cmds_total']}")
    print(f"{prefix}writes_started    {d['writes_started']}")
    print(f"{prefix}writes_completed  {d['writes_completed']}  "
          f"(delta {d['writes_started'] - d['writes_completed']})")
    print(f"{prefix}last_write_addr   0x{d['last_write_addr']:08X}")
    print(f"{prefix}last_write_len    {d['last_write_len']}")
    print(f"{prefix}last_flash_us     {d['last_flash_us']}  "
          f"({d['last_flash_us']/1000:.1f} ms)")
    print(f"{prefix}max_flash_us      {d['max_flash_us']}  "
          f"({d['max_flash_us']/1000:.1f} ms)")
    print(f"{prefix}last_err          0x{d['last_err']:08X}")
    print(f"{prefix}nack_count        {d['nack_count']}")


def cmd_diag(args: argparse.Namespace) -> int:
    port = open_cdc(args.port)
    try:
        handshake(port)
        d = read_diag(port)
        _print_diag(d)
    finally:
        port.close()
    return 0


def cmd_monitor(args: argparse.Namespace) -> int:
    port = open_cdc(args.port, baud=args.baud)
    try:
        monitor_loop(port)
    finally:
        port.close()
    return 0


def _touch_1200(port_path: str, hold_ms: int = 100) -> None:
    """1200-baud touch: open CDC at 1200, briefly hold, close so DTR drops.

    The firmware-side uboot_trigger component reboots into boot2 rescue
    when it observes this exact sequence (see components/boot/uboot_trigger).
    """
    port = open_cdc(port_path, baud=1200)
    try:
        time.sleep(hold_ms / 1000.0)
    finally:
        port.close()


def cmd_reboot(args: argparse.Namespace) -> int:
    """Host-side 1200-baud touch. Confirms the device actually rebooted by
    watching the original CDC node disappear, then waits for a new (possibly
    renamed) CDC node to re-appear, then optionally handshakes.

    If the node never disappears, the running firmware almost certainly lacks
    CONFIG_BOOT_UBOOT_TRIGGER_ENABLE — surface that as the error.
    """
    if not args.port:
        print("error: no --port provided and auto-detect failed", file=sys.stderr)
        return 2

    import glob
    original = args.port
    print(f"[ipro_dl] 1200-baud touch on {original}")
    try:
        _touch_1200(original, hold_ms=args.hold_ms)
    except OSError as e:
        print(f"[ipro_dl] touch open failed: {e}", file=sys.stderr)
        return 1

    if args.wait_rescue <= 0:
        return 0

    # Step 1: wait for the original port to disappear — this is proof that the
    # device actually reset. Without disappearance, the running firmware is not
    # honouring the 1200-touch.
    disappear_budget = min(3.0, args.wait_rescue)
    deadline = time.monotonic() + disappear_budget
    disappeared = False
    while time.monotonic() < deadline:
        if not os.path.exists(original):
            disappeared = True
            break
        time.sleep(0.1)

    if not disappeared:
        print(f"[ipro_dl] {original} did not disappear — device did not reset.\n"
              f"         Running firmware probably lacks CONFIG_BOOT_UBOOT_TRIGGER_ENABLE.",
              file=sys.stderr)
        return 1
    print(f"[ipro_dl] {original} vanished — device reset")

    # Step 2: wait for ANY CDC node to come back. macOS may renumber under a
    # new serial string (app vs rescue expose different Product/Serial). If the
    # original comes back, assume same-path reconnect; otherwise take whatever
    # usbmodem port is present.
    remaining = deadline + (args.wait_rescue - disappear_budget) - time.monotonic()
    remaining = max(remaining, 2.0)
    deadline = time.monotonic() + remaining
    rescue_port = None
    while time.monotonic() < deadline:
        candidates = sorted(glob.glob("/dev/cu.usbmodem*"))
        if original in candidates:
            rescue_port = original
            break
        if candidates:
            rescue_port = candidates[0]
            break
        time.sleep(0.2)
    if rescue_port is None:
        print("[ipro_dl] no CDC port came back within the wait window",
              file=sys.stderr)
        return 1

    # Small extra delay — CDC needs a moment to finish enumeration.
    time.sleep(0.5)
    print(f"[ipro_dl] rescue port up: {rescue_port}")
    if args.verify:
        port = open_cdc(rescue_port)
        try:
            handshake(port)
            print("[ipro_dl] rescue handshake OK")
        finally:
            port.close()
    return 0


# ---- CLI ----------------------------------------------------------------

def _find_usbmodem_ports() -> list[str]:
    import glob
    return sorted(glob.glob("/dev/cu.usbmodem*"))


def _filter_rescue(ports: list[str], match: str = BOOT2_RESCUE_SERIAL_PREFIX) -> list[str]:
    """Keep only ports whose path contains the boot2 rescue serial substring.

    On macOS, /dev/cu.usbmodem<serial><interface> encodes the USB serial
    descriptor. Boot2's rescue CDC advertises "00000001"; app CDCs advertise
    other serials (e.g. "IMU_0001"). Filtering lets wait-rescue skip app
    ports entirely, avoiding spurious handshake writes into running apps.
    """
    return [p for p in ports if match in p]


def _auto_detect_port() -> str | None:
    """Default port for subcommands that expect a live CDC.

    Prefers rescue ports (so `ipro_dl info`, `flash`, etc. land on boot2
    rather than an unrelated app CDC that happens to be enumerated).
    """
    all_ports = _find_usbmodem_ports()
    rescue = _filter_rescue(all_ports)
    return rescue[0] if rescue else (all_ports[0] if all_ports else None)


def main(argv: list[str] | None = None) -> int:
    default_port = _auto_detect_port()
    p = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument("--port", default=default_port,
                   help=f"CDC port (default auto: {default_port})")
    sub = p.add_subparsers(dest="cmd", required=True)

    sp = sub.add_parser("info", help="handshake test")
    sp.set_defaults(func=cmd_info)

    sp = sub.add_parser("erase", help="erase a range")
    sp.add_argument("--start", required=True, help="start address (hex/dec)")
    sp.add_argument("--end",   required=True, help="end address, inclusive")
    sp.set_defaults(func=cmd_erase)

    sp = sub.add_parser("flash", help="erase + write + verify [+ boot]")
    sp.add_argument("file",
                    help="raw .bin (auto-wrapped in IPNP) or pre-wrapped whole_img.bin")
    sp.add_argument("--addr", default="0x12000",
                    help="flash address (default 0x12000 = FW start in u-boot layout)")
    sp.add_argument("--no-verify", action="store_true", help="skip SHA256 verify")
    sp.add_argument("--boot", action="store_true",
                    help="send FLASH_BOOT after a successful flash")
    sp.add_argument("--raw", action="store_true",
                    help="flash the file bytes as-is without any IPNP wrapping "
                         "(expert use; boot2 will refuse unless the file already "
                         "carries a valid bootheader)")
    sp.add_argument("--wrap", action="store_true",
                    help="force IPNP wrap even if the input has IPNP magic "
                         "(useful when your input is a partial header)")
    sp.add_argument("--save-wrapped", metavar="PATH",
                    help="after wrapping a raw .bin, also save the wrapped image to PATH")
    sp.add_argument("--block-size", type=lambda x: int(x, 0), default=WRITE_BLOCK_SIZE,
                    help=f"bytes of flash payload per USB write (default {WRITE_BLOCK_SIZE}; "
                         f"max 4088). Smaller = slower but more robust on flaky USB links.")
    sp.add_argument("--inter-block-ms", type=float, default=INTER_BLOCK_SLEEP * 1000.0,
                    help=f"sleep between write frames in ms (default "
                         f"{INTER_BLOCK_SLEEP * 1000:.0f}). Raise if you see mid-flash timeouts "
                         f"on a boot2 that lacks the rx_scratch fix.")
    sp.add_argument("--retries", type=int, default=WRITE_RETRIES,
                    help=f"per-block attempts before failing the flash "
                         f"(default {WRITE_RETRIES}). Increase only if your link is noisy; "
                         f"retries papering over errors often mean a firmware bug.")
    sp.add_argument("--trace-slow-ms", type=float, default=0,
                    help="log any block write that takes longer than this many ms "
                         "(default 0 = disabled). Useful for diagnosing erratic "
                         "per-block latency patterns.")
    sp.add_argument("--force-port", action="store_true",
                    help="skip the 'port must look like boot2 rescue' safety check")
    sp.add_argument("--wait-rescue", type=float, default=0.0, metavar="SECONDS",
                    help="before flashing, scan for a boot2 rescue CDC and "
                         "handshake on it (combines wait-rescue + flash; "
                         "keeps the same fd so macOS EBUSY races can't trip).")
    sp.add_argument("--match", default=BOOT2_RESCUE_SERIAL_PREFIX,
                    help=f"rescue port name filter, used with --wait-rescue "
                         f"(default '{BOOT2_RESCUE_SERIAL_PREFIX}')")
    sp.add_argument("--any-port", action="store_true",
                    help="with --wait-rescue, ignore --match and try every port")
    sp.set_defaults(func=cmd_flash)

    sp = sub.add_parser("read", help="read range back to a file")
    sp.add_argument("--addr",   required=True, help="start address")
    sp.add_argument("--length", required=True, help="bytes to read")
    sp.add_argument("--output", required=True, help="output file")
    sp.set_defaults(func=cmd_read)

    sp = sub.add_parser("boot", help="send FLASH_BOOT — jump to FW")
    sp.set_defaults(func=cmd_boot)

    sp = sub.add_parser("reset", help="send CMD_RESET — reboot board")
    sp.set_defaults(func=cmd_reset)

    sp = sub.add_parser("monitor", help="115200 serial monitor")
    sp.add_argument("--baud", type=int, default=115200)
    sp.set_defaults(func=cmd_monitor)

    sp = sub.add_parser("diag",
                        help="dump boot2's post-mortem diagnostic block")
    sp.set_defaults(func=cmd_diag)

    sp = sub.add_parser("wait-rescue",
                        help="poll for CDC + boot2 handshake (catch countdown window)")
    sp.add_argument("--timeout", type=float, default=20.0,
                    help="seconds to wait (default 20, > boot2 countdown)")
    sp.add_argument("--match", default=BOOT2_RESCUE_SERIAL_PREFIX,
                    help=f"substring that must appear in /dev/cu.usbmodem* "
                         f"to be treated as a rescue candidate "
                         f"(default '{BOOT2_RESCUE_SERIAL_PREFIX}')")
    sp.add_argument("--any-port", action="store_true",
                    help="ignore --match and try every usbmodem port "
                         "(useful if you have customised boot2's serial string)")
    sp.set_defaults(func=cmd_wait_rescue)

    sp = sub.add_parser("reboot",
                        help="1200-baud touch: reboot app into boot2 USB rescue")
    sp.add_argument("--hold-ms", type=int, default=100,
                    help="how long to keep the 1200-baud port open (default 100)")
    sp.add_argument("--wait-rescue", type=int, default=5,
                    help="seconds to wait for rescue CDC to re-appear (0 disables)")
    sp.add_argument("--verify", action="store_true",
                    help="after rescue appears, do a handshake to confirm")
    sp.set_defaults(func=cmd_reboot)

    args = p.parse_args(argv)

    # wait-rescue tolerates a missing port (it will poll until one appears);
    # every other subcommand requires a live port up front.
    if not args.port and args.cmd != "wait-rescue":
        print("error: no --port given and no /dev/cu.usbmodem* auto-detected",
              file=sys.stderr)
        return 2

    try:
        return args.func(args)
    except ProtocolError as e:
        print(f"[ipro_dl] protocol error: {e}", file=sys.stderr)
        return 1
    except TimeoutError as e:
        print(f"[ipro_dl] timeout: {e}", file=sys.stderr)
        return 1
    except FileNotFoundError as e:
        print(f"[ipro_dl] {e}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())

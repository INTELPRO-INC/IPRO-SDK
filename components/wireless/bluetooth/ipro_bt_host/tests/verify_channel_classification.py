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

"""Verify BLE 5.3 peripheral channel classification with two boards."""

from __future__ import annotations

import argparse
import json
import os
import re
import select
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[5]
sys.path.insert(0, str(ROOT / "tools"))
from serial_monitor import open_serial, serial_write


ANSI_RE = re.compile(r"\x1b\[[0-9;?]*[ -/]*[@-~]")
CENTRAL_RE = re.compile(
    r"CH_CLASS_TEST:\s+CENTRAL\s+map=(?P<map>[0-9a-fA-F]{10})\s+"
    r"expect_bad=(?P<expected>-?\d+)\s+(?P<status>PASS|FAIL)\s+"
    r"rc=(?P<rc>-?\d+)\s+sec=(?P<security>\d+)"
)
PERIPHERAL_RE = re.compile(
    r"CH_CLASS_TEST:\s+PERIPHERAL\s+"
    r"(?:(?:force_bad=(?P<channel>\d+)\s+rc=(?P<rc>-?\d+))|"
    r"(?:clear\s+rc=(?P<clear_rc>-?\d+))|(?:status))\s+"
    r"controller_reads=(?P<reads>\d+)"
)


def parse_central_line(line: str) -> dict[str, int | str] | None:
    match = CENTRAL_RE.search(ANSI_RE.sub("", line))
    if match is None:
        return None
    result: dict[str, int | str] = match.groupdict()
    for name in ("expected", "rc", "security"):
        result[name] = int(str(result[name]), 10)
    result["map"] = str(result["map"]).lower()
    return result


def parse_peripheral_line(line: str) -> dict[str, int | None] | None:
    match = PERIPHERAL_RE.search(ANSI_RE.sub("", line))
    if match is None:
        return None
    groups = match.groupdict()
    return {
        name: (int(value, 10) if value is not None else None)
        for name, value in groups.items()
    }


def channel_enabled(map_hex: str, channel: int) -> bool:
    channel_map = bytes.fromhex(map_hex)
    return bool(channel_map[channel // 8] & (1 << (channel % 8)))


class SerialEndpoint:
    def __init__(self, port: str, baud: int, log_path: Path):
        self.fd = open_serial(port, baud)
        self.log_file = log_path.open("wb")
        self.pending = bytearray()

    def close(self) -> None:
        self.log_file.close()
        os.close(self.fd)

    def send(self, command: str) -> None:
        if not serial_write(self.fd, command + "\r\n"):
            raise OSError(f"serial write failed: {command}")

    def receive(self, predicate, timeout: float) -> str | None:
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            remaining = max(0.0, deadline - time.monotonic())
            ready, _, _ = select.select([self.fd], [], [], min(0.1, remaining))
            if not ready:
                continue
            chunk = os.read(self.fd, 512)
            if not chunk:
                continue
            self.log_file.write(chunk)
            self.log_file.flush()
            self.pending.extend(chunk)
            while b"\n" in self.pending:
                raw, _, self.pending = self.pending.partition(b"\n")
                line = raw.decode("utf-8", errors="replace").rstrip("\r")
                if predicate(line):
                    return line
        return None


def read_peripheral(endpoint: SerialEndpoint, command: str,
                    timeout: float) -> dict[str, int | None] | None:
    endpoint.send(command)
    line = endpoint.receive(lambda value: parse_peripheral_line(value) is not None,
                            timeout)
    return parse_peripheral_line(line) if line is not None else None


def read_central(endpoint: SerialEndpoint, command: str,
                 timeout: float) -> dict[str, int | str] | None:
    endpoint.send(command)
    line = endpoint.receive(lambda value: parse_central_line(value) is not None,
                            timeout)
    return parse_central_line(line) if line is not None else None


def poll_channel(endpoint: SerialEndpoint, channel: int, enabled: bool,
                 timeout: float, poll_interval: float) -> tuple[dict | None, float]:
    start = time.monotonic()
    while time.monotonic() - start < timeout:
        remaining = timeout - (time.monotonic() - start)
        # Keep individual shell transactions bounded.  Passing the complete
        # transition timeout here lets one dropped UART response overrun the
        # whole state transition and leaves a late response for the next step.
        result = read_central(endpoint, "chclass map", min(1.5, remaining))
        if (result is not None and result["status"] == "PASS" and
                result["rc"] == 0 and
                channel_enabled(str(result["map"]), channel) == enabled):
            return result, (time.monotonic() - start) * 1000.0
        time.sleep(poll_interval)
    return None, (time.monotonic() - start) * 1000.0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--peripheral-port", default="/dev/cu.usbmodem21202")
    parser.add_argument("--central-port", default="/dev/cu.usbmodem31302")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--channels", default="7,36")
    parser.add_argument("--cycles", type=int, default=3)
    parser.add_argument("--timeout", type=float, default=8.0)
    parser.add_argument("--poll-interval", type=float, default=0.75)
    parser.add_argument("--peripheral-log", type=Path, required=True)
    parser.add_argument("--central-log", type=Path, required=True)
    parser.add_argument("--json", type=Path, required=True)
    args = parser.parse_args()

    channels = [int(value) for value in args.channels.split(",")]
    if not channels or any(channel < 0 or channel > 36 for channel in channels):
        parser.error("channels must be comma-separated values in range 0..36")

    report = {
        "peripheral_port": args.peripheral_port,
        "central_port": args.central_port,
        "baud": args.baud,
        "channels": channels,
        "cycles": args.cycles,
        "requested_transitions": args.cycles * len(channels),
        "completed_transitions": 0,
        "failures": [],
        "results": [],
    }
    peripheral = SerialEndpoint(args.peripheral_port, args.baud,
                                args.peripheral_log)
    central = SerialEndpoint(args.central_port, args.baud, args.central_log)
    try:
        central.send("cstat")
        connected = central.receive(
            lambda line: "state=connected" in ANSI_RE.sub("", line), args.timeout)
        report["connected_at_start"] = connected is not None
        if connected is None:
            report["failures"].append("central not connected at start")

        initial = read_peripheral(peripheral, "chclass clear", args.timeout)
        if initial is None or initial["clear_rc"] != 0:
            report["failures"].append("initial peripheral clear failed")

        for cycle in range(args.cycles):
            for channel in channels:
                item = {"cycle": cycle, "channel": channel}
                baseline = read_peripheral(peripheral, "chclass status", args.timeout)
                forced = read_peripheral(peripheral, f"chclass bad {channel}",
                                         args.timeout)
                bad_map, bad_latency = poll_channel(
                    central, channel, False, args.timeout, args.poll_interval)
                observed = read_peripheral(peripheral, "chclass status", args.timeout)
                cleared = read_peripheral(peripheral, "chclass clear", args.timeout)
                good_map, good_latency = poll_channel(
                    central, channel, True, args.timeout, args.poll_interval)

                item.update({
                    "baseline_reads": None if baseline is None else baseline["reads"],
                    "forced": forced,
                    "bad_map": bad_map,
                    "bad_latency_ms": round(bad_latency, 3),
                    "observed_reads": None if observed is None else observed["reads"],
                    "cleared": cleared,
                    "restored_map": good_map,
                    "restore_latency_ms": round(good_latency, 3),
                })
                item["pass"] = all((
                    baseline is not None,
                    forced is not None and forced["rc"] == 0,
                    bad_map is not None,
                    observed is not None and
                    observed["reads"] > baseline["reads"],
                    cleared is not None and cleared["clear_rc"] == 0,
                    good_map is not None,
                ))
                report["results"].append(item)
                if item["pass"]:
                    report["completed_transitions"] += 1
                else:
                    report["failures"].append(
                        f"cycle {cycle} channel {channel} transition failed")
                    break
            if report["failures"]:
                break

        central.send("cstat")
        connected = central.receive(
            lambda line: "state=connected" in ANSI_RE.sub("", line), args.timeout)
        report["connected_at_end"] = connected is not None
        if connected is None:
            report["failures"].append("central disconnected at end")
    finally:
        peripheral.close()
        central.close()

    report["pass"] = (
        report.get("connected_at_start", False) and
        report.get("connected_at_end", False) and
        report["completed_transitions"] == report["requested_transitions"] and
        not report["failures"]
    )
    args.json.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({key: report[key] for key in (
        "requested_transitions", "completed_transitions", "failures", "pass")},
        indent=2))
    return 0 if report["pass"] else 1


if __name__ == "__main__":
    raise SystemExit(main())

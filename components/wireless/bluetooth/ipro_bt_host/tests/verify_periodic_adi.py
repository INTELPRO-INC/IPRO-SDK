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

"""Verify BLE 5.3 periodic advertising ADI duplicate filtering on two boards."""

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
STATUS_RE = re.compile(
    r"ADI_TEST:\s+(?P<operation>\w+)\s+rc=(?P<rc>-?\d+)\s+"
    r"adv=(?P<adv>\d+)\s+adi=(?P<adi>\d+)\s+seeking=(?P<seeking>\d+)\s+"
    r"sync=(?P<sync>\d+)\s+filter=(?P<filter>\d+)\s+tx_seq=(?P<tx_seq>\d+)\s+"
    r"rx=(?P<rx>\d+)\s+good=(?P<good>\d+)\s+bad=(?P<bad>\d+)\s+"
    r"same=(?P<same>\d+)\s+changed=(?P<changed>\d+)\s+last=(?P<last>\d+)\s+"
    r"lost=(?P<lost>\d+)\s+err=(?P<err>\d+)"
)


def parse_status_line(line: str) -> dict[str, int | str] | None:
    match = STATUS_RE.search(ANSI_RE.sub("", line))
    if match is None:
        return None
    result: dict[str, int | str] = {"operation": match.group("operation")}
    for name, value in match.groupdict().items():
        if name != "operation":
            result[name] = int(value, 10)
    return result


def sequence_transition_ok(before: dict, after: dict, sequence: int) -> bool:
    """Return true only for one new exact report and no duplicate/error."""
    return all((
        after["sync"] == 1,
        after["rx"] == before["rx"] + 1,
        after["good"] == before["good"] + 1,
        after["bad"] == before["bad"],
        after["same"] == before["same"],
        after["changed"] == before["changed"] + 1,
        after["last"] == sequence,
        after["err"] == before["err"],
        after["lost"] == before["lost"],
    ))


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

    def receive_status(self, operation: str, timeout: float) -> dict | None:
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            remaining = max(0.0, deadline - time.monotonic())
            ready, _, _ = select.select([self.fd], [], [], min(0.1, remaining))
            if not ready:
                continue
            chunk = os.read(self.fd, 1024)
            if not chunk:
                continue
            self.log_file.write(chunk)
            self.log_file.flush()
            self.pending.extend(chunk)
            while b"\n" in self.pending:
                raw, _, self.pending = self.pending.partition(b"\n")
                parsed = parse_status_line(
                    raw.decode("utf-8", errors="replace").rstrip("\r"))
                if parsed is not None and parsed["operation"] == operation:
                    return parsed
        return None

    def command(self, command: str, operation: str | None = None,
                timeout: float = 3.0) -> dict | None:
        self.send(command)
        return self.receive_status(operation or command.split()[1], timeout)


def poll_status(endpoint: SerialEndpoint, predicate, timeout: float,
                interval: float = 0.25) -> dict | None:
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        status = endpoint.command("adi status", timeout=min(1.0, timeout))
        if status is not None and predicate(status):
            return status
        time.sleep(interval)
    return None


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--advertiser-port", default="/dev/cu.usbmodem31302")
    parser.add_argument("--scanner-port", default="/dev/cu.usbmodem21202")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--updates", type=int, default=10)
    parser.add_argument("--advertiser-mode", choices=("on", "bare"),
                        default="on")
    parser.add_argument("--scanner-mode", choices=("filter", "any"),
                        default="filter")
    parser.add_argument("--timeout", type=float, default=8.0)
    parser.add_argument("--idle-seconds", type=float, default=1.0)
    parser.add_argument("--advertiser-log", type=Path, required=True)
    parser.add_argument("--scanner-log", type=Path, required=True)
    parser.add_argument("--json", type=Path, required=True)
    args = parser.parse_args()

    report = {
        "advertiser_port": args.advertiser_port,
        "scanner_port": args.scanner_port,
        "baud": args.baud,
        "advertiser_mode": args.advertiser_mode,
        "scanner_mode": args.scanner_mode,
        "requested_updates": args.updates,
        "completed_updates": 0,
        "results": [],
        "failures": [],
    }
    advertiser = SerialEndpoint(args.advertiser_port, args.baud,
                                args.advertiser_log)
    scanner = SerialEndpoint(args.scanner_port, args.baud, args.scanner_log)
    try:
        scanner.command("adi stop")
        advertiser.command("adi stop")
        adv_start = advertiser.command(f"adi adv {args.advertiser_mode}",
                                       "adv", args.timeout)
        scan_start = scanner.command(f"adi scan {args.scanner_mode}",
                                     "scan", args.timeout)
        report["advertiser_start"] = adv_start
        report["scanner_start"] = scan_start
        if adv_start is None or adv_start["rc"] != 0:
            report["failures"].append("advertiser start failed")
        if scan_start is None or scan_start["rc"] != 0:
            report["failures"].append("scanner start failed")

        initial = None
        if not report["failures"]:
            initial = poll_status(
                scanner,
                lambda item: item["sync"] == 1 and item["rx"] == 1 and
                item["good"] == 1 and item["bad"] == 0 and
                item["same"] == 0 and item["last"] == 0,
                args.timeout,
            )
        report["initial"] = initial
        if initial is None:
            report["failures"].append("initial sequence 0 not received exactly")

        before = initial
        for sequence in range(1, args.updates + 1):
            if before is None or report["failures"]:
                break
            tx = advertiser.command(f"adi update {sequence}", "update",
                                    args.timeout)
            after = poll_status(
                scanner,
                lambda item, b=before, s=sequence:
                    sequence_transition_ok(b, item, s),
                args.timeout,
            )
            item = {"sequence": sequence, "tx": tx, "rx": after}
            item["pass"] = bool(
                tx is not None and tx["rc"] == 0 and
                tx["tx_seq"] == sequence and after is not None
            )
            report["results"].append(item)
            if not item["pass"]:
                report["failures"].append(
                    f"sequence {sequence} DID transition failed")
                break
            report["completed_updates"] += 1
            before = after

        if before is not None and not report["failures"]:
            time.sleep(args.idle_seconds)
            idle = scanner.command("adi status", timeout=args.timeout)
            report["idle"] = idle
            if idle is None or any((
                    idle["rx"] != before["rx"],
                    idle["good"] != before["good"],
                    idle["same"] != before["same"],
                    idle["bad"] != 0,
                    idle["sync"] != 1,
                    idle["err"] != 0,
                    idle["lost"] != 0,
            )):
                report["failures"].append(
                    "duplicate filter did not remain quiet after final update")
    finally:
        scanner.command("adi stop")
        advertiser.command("adi stop")
        scanner.close()
        advertiser.close()

    report["pass"] = (
        report["completed_updates"] == report["requested_updates"] and
        not report["failures"]
    )
    args.json.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({key: report[key] for key in (
        "requested_updates", "completed_updates", "failures", "pass")},
        indent=2))
    return 0 if report["pass"] else 1


if __name__ == "__main__":
    raise SystemExit(main())

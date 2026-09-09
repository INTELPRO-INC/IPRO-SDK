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

"""Stress the board-to-board LE GATT Security Levels characteristic read."""

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
READ_RE = re.compile(
    r"GSL_TEST:\s+READ\s+(?P<status>PASS|FAIL)\s+"
    r"rc=(?P<rc>-?\d+)\s+att_err=0x(?P<att_err>[0-9a-fA-F]{2})\s+"
    r"sec=(?P<security>\d+)\s+values=(?P<values>\d+)\s+"
    r"len=(?P<length>\d+)\s+data=(?P<data>[0-9a-fA-F]+)\s+"
    r"pass=(?P<pass_count>\d+)\s+fail=(?P<fail_count>\d+)"
)
EXPECTED_VALUE = "01040202"


def parse_read_line(line: str) -> dict[str, int | str] | None:
    match = READ_RE.search(ANSI_RE.sub("", line))
    if match is None:
        return None
    result: dict[str, int | str] = match.groupdict()
    for name in ("rc", "security", "values", "length", "pass_count", "fail_count"):
        result[name] = int(str(result[name]), 10)
    result["att_err"] = int(str(result["att_err"]), 16)
    result["data"] = str(result["data"]).lower()
    return result


def read_result_passes(result: dict[str, int | str] | None) -> bool:
    return result is not None and all((
        result["status"] == "PASS",
        result["rc"] == 0,
        result["att_err"] == 0,
        result["security"] == 1,
        result["values"] == 1,
        result["length"] == 4,
        result["data"] == EXPECTED_VALUE,
        result["fail_count"] == 0,
    ))


def receive_until(fd: int, predicate, timeout: float, log_file) -> str | None:
    deadline = time.monotonic() + timeout
    pending = bytearray()
    while time.monotonic() < deadline:
        remaining = max(0.0, deadline - time.monotonic())
        ready, _, _ = select.select([fd], [], [], min(0.1, remaining))
        if not ready:
            continue
        chunk = os.read(fd, 512)
        if not chunk:
            continue
        log_file.write(chunk)
        log_file.flush()
        pending.extend(chunk)
        while b"\n" in pending:
            raw, _, pending = pending.partition(b"\n")
            line = raw.decode("utf-8", errors="replace").rstrip("\r")
            if predicate(line):
                return line
    return None


def send_command(fd: int, command: str) -> None:
    if not serial_write(fd, command + "\r\n"):
        raise OSError("serial write failed")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", default="/dev/cu.usbmodem31302")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--count", type=int, default=50)
    parser.add_argument("--timeout", type=float, default=5.0,
                        help="per-command response timeout in seconds")
    parser.add_argument("--log", type=Path, required=True)
    parser.add_argument("--json", type=Path, required=True)
    args = parser.parse_args()

    report = {
        "port": args.port,
        "baud": args.baud,
        "requested": args.count,
        "completed": 0,
        "passed": 0,
        "failed": 0,
        "timeout": 0,
        "expected": {"security": 1, "values": 1, "length": 4,
                     "data": EXPECTED_VALUE},
        "results": [],
    }

    fd = open_serial(args.port, args.baud)
    try:
        with args.log.open("wb") as log_file:
            send_command(fd, "cstat")
            connected = receive_until(
                fd, lambda line: "state=connected" in ANSI_RE.sub("", line),
                args.timeout, log_file)
            report["connected"] = connected is not None
            if connected is None:
                report["failed"] = 1
            else:
                for index in range(args.count):
                    command_start = time.monotonic()
                    send_command(fd, "gsl read")
                    line = receive_until(
                        fd, lambda value: parse_read_line(value) is not None,
                        args.timeout, log_file)
                    if line is None:
                        report["timeout"] += 1
                        report["failed"] += 1
                        report["results"].append({"index": index, "timeout": True})
                        break
                    result = parse_read_line(line)
                    result["index"] = index
                    result["latency_ms"] = round(
                        (time.monotonic() - command_start) * 1000.0, 3)
                    report["results"].append(result)
                    report["completed"] += 1
                    if read_result_passes(result):
                        report["passed"] += 1
                    else:
                        report["failed"] += 1
                        break
    finally:
        os.close(fd)

    report["pass"] = (
        report.get("connected", False) and report["completed"] == args.count and
        report["passed"] == args.count and report["failed"] == 0
    )
    args.json.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({key: report[key] for key in
                      ("connected", "requested", "completed", "passed", "failed", "timeout", "pass")},
                     indent=2))
    return 0 if report["pass"] else 1


if __name__ == "__main__":
    raise SystemExit(main())

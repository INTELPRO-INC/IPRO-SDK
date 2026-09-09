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

"""Verify declared and enforced BLE 5.4 GATT Security Levels policy."""

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
PROTECTED_RE = re.compile(
    r"GSL_TEST:\s+PROTECTED\s+rc=(?P<rc>-?\d+)\s+"
    r"att_err=0x(?P<att_err>[0-9a-fA-F]{2})\s+"
    r"sec=(?P<security>\d+)\s+len=(?P<length>\d+)\s+"
    r"exact=(?P<exact>[01])\s+data=(?P<data>[0-9a-fA-F]{16})"
)
SECURITY_CHANGED_RE = re.compile(
    r"\[ipro-central-smp\]\s+security_changed\s+peer=.*?\s+"
    r"level=(?P<level>\d+)\s+err=(?P<err>-?\d+)\s+count=(?P<count>\d+)"
)
DECLARATION_RE = re.compile(
    r"GSL_TEST:\s+READ\s+(?P<status>PASS|FAIL)\s+"
    r"rc=(?P<rc>-?\d+)\s+att_err=0x(?P<att_err>[0-9a-fA-F]{2})\s+"
    r"sec=(?P<security>\d+)\s+values=(?P<values>\d+)\s+"
    r"len=(?P<length>\d+)\s+data=(?P<data>[0-9a-fA-F]+)"
)
POLICY_VALUE = "4950524f2d534334"
UNPAIR_RE = re.compile(r"GSL_TEST:\s+UNPAIR\s+rc=(?P<rc>-?\d+)")


def parse_protected_line(line: str) -> dict[str, int | str] | None:
    match = PROTECTED_RE.search(ANSI_RE.sub("", line))
    if match is None:
        return None
    result: dict[str, int | str] = match.groupdict()
    for name in ("rc", "security", "length", "exact"):
        result[name] = int(str(result[name]), 10)
    result["att_err"] = int(str(result["att_err"]), 16)
    result["data"] = str(result["data"]).lower()
    return result


def parse_security_changed_line(line: str) -> dict[str, int] | None:
    match = SECURITY_CHANGED_RE.search(ANSI_RE.sub("", line))
    if match is None:
        return None
    return {name: int(value, 10) for name, value in match.groupdict().items()}


def parse_declaration_line(line: str) -> dict[str, int | str] | None:
    match = DECLARATION_RE.search(ANSI_RE.sub("", line))
    if match is None:
        return None
    result: dict[str, int | str] = match.groupdict()
    for name in ("rc", "security", "values", "length"):
        result[name] = int(str(result[name]), 10)
    result["att_err"] = int(str(result["att_err"]), 16)
    result["data"] = str(result["data"]).lower()
    return result


def declaration_passes(result: dict[str, int | str] | None) -> bool:
    return result is not None and all((
        result["status"] == "PASS", result["rc"] == 0,
        result["att_err"] == 0, result["security"] == 1,
        result["values"] == 1, result["length"] == 2,
        str(result["data"]).startswith("0104"),
    ))


def denial_passes(result: dict[str, int | str] | None) -> bool:
    return result is not None and all((
        result["rc"] == 0, result["att_err"] in (0x05, 0x0F),
        result["security"] == 1, result["length"] == 0,
        result["exact"] == 0, result["data"] == "0000000000000000",
    ))


def protected_read_passes(result: dict[str, int | str] | None) -> bool:
    return result is not None and all((
        result["rc"] == 0, result["att_err"] == 0,
        result["security"] == 4, result["length"] == 8,
        result["exact"] == 1, result["data"] == POLICY_VALUE,
    ))


class SerialEndpoint:
    def __init__(self, port: str, baud: int, log_path: Path):
        self.port = port
        self.fd = open_serial(port, baud)
        self.log_file = log_path.open("wb")
        self.pending = bytearray()

    def close(self) -> None:
        self.log_file.close()
        os.close(self.fd)

    def send(self, command: str) -> None:
        if not serial_write(self.fd, command + "\r\n"):
            raise OSError(f"serial write failed on {self.port}: {command}")

    def lines(self) -> list[str]:
        chunk = os.read(self.fd, 1024)
        if not chunk:
            return []
        self.log_file.write(chunk)
        self.log_file.flush()
        self.pending.extend(chunk)
        result = []
        while b"\n" in self.pending:
            raw, _, self.pending = self.pending.partition(b"\n")
            result.append(raw.decode("utf-8", errors="replace").rstrip("\r"))
        return result


def receive_until(endpoints: list[SerialEndpoint], predicate, timeout: float):
    deadline = time.monotonic() + timeout
    by_fd = {endpoint.fd: endpoint for endpoint in endpoints}
    while time.monotonic() < deadline:
        remaining = max(0.0, deadline - time.monotonic())
        ready, _, _ = select.select(list(by_fd), [], [], min(0.1, remaining))
        for fd in ready:
            endpoint = by_fd[fd]
            for line in endpoint.lines():
                result = predicate(endpoint, line)
                if result is not None:
                    return result
    return None


def command_result(endpoint: SerialEndpoint, command: str, parser,
                   timeout: float):
    endpoint.send(command)
    return receive_until(
        [endpoint], lambda _endpoint, line: parser(line), timeout)


def drain(endpoints: list[SerialEndpoint], quiet_seconds: float = 0.2) -> None:
    """Drain already queued UART lines until all endpoints stay quiet."""
    by_fd = {endpoint.fd: endpoint for endpoint in endpoints}
    deadline = time.monotonic() + quiet_seconds
    while time.monotonic() < deadline:
        ready, _, _ = select.select(list(by_fd), [], [], quiet_seconds)
        if not ready:
            return
        for fd in ready:
            by_fd[fd].lines()
        deadline = time.monotonic() + quiet_seconds


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--peer-port", default="/dev/cu.usbmodem21202")
    parser.add_argument("--central-port", default="/dev/cu.usbmodem31302")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--reads", type=int, default=10)
    parser.add_argument("--timeout", type=float, default=12.0)
    parser.add_argument("--peer-log", type=Path, required=True)
    parser.add_argument("--central-log", type=Path, required=True)
    parser.add_argument("--json", type=Path, required=True)
    args = parser.parse_args()

    report = {
        "peer_port": args.peer_port,
        "central_port": args.central_port,
        "baud": args.baud,
        "requested_secure_reads": args.reads,
        "secure_reads": [],
        "failures": [],
    }
    peer = SerialEndpoint(args.peer_port, args.baud, args.peer_log)
    central = SerialEndpoint(args.central_port, args.baud, args.central_log)
    try:
        # Make repeated runs deterministic.  Both errors permitted by Core
        # 5.4 (Authentication with no key, Encryption with a retained LTK)
        # are validated by the parser, but the full bench starts keyless.
        unpaired: set[str] = set()
        peer.send("gsl unpair")
        central.send("gsl unpair")

        def unpair_predicate(endpoint: SerialEndpoint, line: str):
            match = UNPAIR_RE.search(ANSI_RE.sub("", line))
            if match is not None and int(match.group("rc"), 10) == 0:
                unpaired.add(endpoint.port)
            return True if len(unpaired) == 2 else None

        unpair_ok = receive_until([peer, central], unpair_predicate,
                                  args.timeout)
        report["unpaired"] = unpair_ok is not None
        if unpair_ok is None:
            report["failures"].append("both peers did not clear test bonds")

        # bt_unpair() may report before the controller's disconnect event.
        # Do not mistake that old ACL (or a queued heartbeat describing it)
        # for the fresh keyless connection used by the negative read.
        time.sleep(1.0)
        drain([peer, central])

        peer.send("le_adv start")
        advertising = receive_until(
            [peer, central],
            lambda endpoint, line: line if endpoint is peer and
            "LE advertising started" in ANSI_RE.sub("", line) else None,
            args.timeout,
        )
        report["advertising"] = advertising is not None
        if advertising is None:
            report["failures"].append("peer LE advertising did not start")

        central.send("cstat")
        connected = receive_until(
            [peer, central],
            lambda endpoint, line: line if endpoint is central and
            "state=connected" in ANSI_RE.sub("", line) else None,
            args.timeout,
        )
        report["connected"] = connected is not None
        if connected is None and not report["failures"]:
            report["failures"].append("central did not report connected")

        if not report["failures"]:
            declaration = command_result(
                central, "gsl read", parse_declaration_line, args.timeout)
            report["declaration"] = declaration
            if not declaration_passes(declaration):
                report["failures"].append(
                    "0x2BF5 did not declare exactly Mode 1 Level 4 at security 1")

        if not report["failures"]:
            denied = command_result(
                central, "gsl protected", parse_protected_line, args.timeout)
            report["unsecured_read"] = denied
            if not denial_passes(denied):
                report["failures"].append(
                    "protected characteristic was not rejected at security 1")

        if not report["failures"]:
            central.send("gsl secure 4")
            passkey_events: list[str] = []

            def security_predicate(endpoint: SerialEndpoint, line: str):
                clean = ANSI_RE.sub("", line)
                if "passkey_confirm" in clean.lower() or \
                        "PASSKEY_CONFIRM" in clean:
                    passkey_events.append(f"{endpoint.port}: {clean}")
                if endpoint is central:
                    changed = parse_security_changed_line(clean)
                    if changed is not None:
                        return changed
                return None

            changed = receive_until([peer, central], security_predicate,
                                    args.timeout)
            report["passkey_events"] = passkey_events
            report["security_changed"] = changed
            if changed is None or changed["level"] != 4 or changed["err"] != 0:
                report["failures"].append(
                    "LE Secure Connections did not reach security level 4")

        if not report["failures"]:
            for index in range(args.reads):
                result = command_result(
                    central, "gsl protected", parse_protected_line,
                    args.timeout)
                item = {"index": index, "result": result,
                        "pass": protected_read_passes(result)}
                report["secure_reads"].append(item)
                if not item["pass"]:
                    report["failures"].append(
                        f"protected level-4 read {index} was not exact")
                    break
    finally:
        central.close()
        peer.close()

    report["pass"] = (
        not report["failures"] and
        len(report["secure_reads"]) == args.reads and
        all(item["pass"] for item in report["secure_reads"])
    )
    args.json.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({
        "connected": report.get("connected", False),
        "secure_reads": len(report["secure_reads"]),
        "failures": report["failures"],
        "pass": report["pass"],
    }, indent=2))
    return 0 if report["pass"] else 1


if __name__ == "__main__":
    raise SystemExit(main())

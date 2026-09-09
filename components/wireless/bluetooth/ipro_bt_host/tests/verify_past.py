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

"""Local-set PAST air gate. Start from fresh, connected PAST fixture images."""
import argparse
import json
import os
from pathlib import Path
import re
import select
import sys
import time

sys.path.insert(0, str(Path(__file__).resolve().parents[5] / "tools"))
from serial_monitor import open_serial, serial_write

ANSI = re.compile(r"\x1b\[[0-9;?]*[ -/]*[@-~]")
STATUS = re.compile(r"PAST_TEST: (\w+) rc=(-?\d+) transfers=(\d+) syncs=(\d+) "
                    r"reports=(\d+) bad=(\d+) lost=(\d+) err=(\d+)")
FIELDS = ("operation", "rc", "transfers", "syncs", "reports", "bad", "lost", "err")


def parse_status(line):
    match = STATUS.search(ANSI.sub("", line))
    if not match:
        return None
    return {key: value if key == "operation" else int(value)
            for key, value in zip(FIELDS, match.groups())}


def healthy(status):
    return status is not None and all(status[k] == 0 for k in ("rc", "bad", "err"))


class Lab:
    def __init__(self, ports, directory):
        self.fds, self.files, self.pending, self.statuses = {}, {}, {}, {}
        self.text = {name: "" for name in ports}
        self.steps = []
        self.observation_retries = []
        try:
            for name, port in ports.items():
                self.fds[name] = open_serial(port, 115200)
                self.files[name] = (directory / f"{name}.log").open("wb")
                self.pending[name] = b""
                self.statuses[name] = []
        except BaseException:
            self.close()
            raise

    def close(self):
        for file in self.files.values():
            file.close()
        for fd in self.fds.values():
            os.close(fd)

    def pump(self, seconds):
        deadline = time.monotonic() + seconds
        while time.monotonic() < deadline:
            ready, _, _ = select.select(list(self.fds.values()), [], [],
                                        min(0.1, max(0, deadline - time.monotonic())))
            for name, fd in self.fds.items():
                if fd not in ready:
                    continue
                chunk = os.read(fd, 4096)
                self.files[name].write(chunk)
                self.files[name].flush()
                self.pending[name] += chunk
                while b"\n" in self.pending[name]:
                    line, self.pending[name] = self.pending[name].split(b"\n", 1)
                    line = ANSI.sub("", line.decode(errors="replace"))
                    self.text[name] += line + "\n"
                    status = parse_status(line)
                    if status:
                        self.statuses[name].append(status)

    def command(self, board, operation):
        self.pump(0.05)
        self.statuses[board].clear()
        if not serial_write(self.fds[board], f"past {operation}\r\n"):
            raise RuntimeError("UART write failed")
        deadline = time.monotonic() + 3
        retried = False
        while True:
            if time.monotonic() >= deadline:
                # Only a read-only observation may be resent. Never retry
                # adv/transfer/stop and accidentally hide an HCI failure.
                if operation != "status" or retried:
                    break
                retried = True
                self.observation_retries.append({"board": board, "operation": operation,
                                                 "reason": "no UART response in 3 seconds"})
                if not serial_write(self.fds[board], "past status\r\n"):
                    raise RuntimeError("UART write failed")
                deadline = time.monotonic() + 3
            self.pump(0.05)
            for status in self.statuses[board]:
                if status["operation"] == operation:
                    self.steps.append({"board": board, **status})
                    if not healthy(status):
                        raise RuntimeError(f"unhealthy {board}: {status}")
                    return status
        raise TimeoutError(f"{board} {operation}")

    def await_reports(self, syncs, minimum, lost):
        deadline = time.monotonic() + 35
        while time.monotonic() < deadline:
            status = self.command("receiver", "status")
            if status["lost"] != lost:
                raise RuntimeError(f"unexpected sync loss: {status}")
            if status["syncs"] == syncs and status["reports"] >= minimum:
                return status
            self.pump(1)
        raise TimeoutError("exact periodic reports not received")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sender-port", required=True)
    parser.add_argument("--receiver-port", required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=False)
    report = {"pass": False, "ports": vars(args) | {"output": str(args.output)},
              "scope": "3 local-set transfers, exact payload, recreate, remote timeout"}
    lab = None
    try:
        lab = Lab({"sender": args.sender_port, "receiver": args.receiver_port}, args.output)
        for board in ("sender", "receiver"):
            status = lab.command(board, "status")
            if any(status[k] for k in ("transfers", "syncs", "reports", "lost")):
                raise RuntimeError("Requires fresh fixtures; refusing to hide previous results")
        lab.command("receiver", "negative")
        lab.command("receiver", "subscribe")
        total = 0
        for cycle in range(3):
            lab.command("sender", "adv")  # Exactly one attempt: no retry workaround.
            lab.command("sender", "transfer")
            status = lab.await_reports(cycle + 1, total + 100, cycle)
            total = status["reports"]
            if cycle < 2:
                lab.command("receiver", "stop")
                lab.command("sender", "stop")
        lab.command("sender", "stop")
        lab.pump(12)  # Receiver timeout is 10 s, not a timing-accuracy measurement.
        final = lab.command("receiver", "status")
        if final["syncs"] != 3 or final["lost"] != 3 or final["reports"] < 300:
            raise RuntimeError(f"final counters: {final}")
        if "SYNC_TERM reason=31" not in lab.text["receiver"]:
            raise RuntimeError("missing remote timeout event")
        for name, text in lab.text.items():
            if re.search(r"fatal|assertion|Disconnected|disconnected|BTDM err", text):
                raise RuntimeError(f"unexpected link/fatal event on {name}")
        report.update({"pass": True, "receiver": final,
                       "sender": lab.command("sender", "status")})
    except (OSError, RuntimeError, TimeoutError) as exc:
        report["failure"] = str(exc)
    finally:
        if lab:
            report["steps"] = lab.steps
            report["observation_retries"] = lab.observation_retries
            lab.close()
        (args.output / "result.json").write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps({key: value for key, value in report.items() if key != "steps"}, indent=2))
    return 0 if report["pass"] else 1


if __name__ == "__main__":
    raise SystemExit(main())

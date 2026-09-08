#!/usr/bin/env python3
"""Drive the IPRO7AI EVK over BLE from a terminal.

The board exposes its shell over the Nordic UART Service, so this is a thin
transport: whatever the serial console accepts, this sends. That is also why
there is no per-peripheral API here - `lcd`, `cam`, `spk`, `mic`, `vbat`, `sd`,
`vid` and `bt` all work, and anything added to the firmware later works with no
change to this script.

Web Bluetooth is the other client, but Safari has never supported it and an
embedded preview blocks it, so this exists to be the one that always works.

    evk.py                      interactive console
    evk.py "bt rf"              run one command, print the reply, exit
    evk.py -f commands.txt      run a file of commands in order
    evk.py --scan               list nearby boards and exit

Requires bleak:  python3 -m pip install bleak
"""
import argparse
import asyncio
import sys

from bleak import BleakClient, BleakScanner

NUS_RX = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"   # we write commands here
NUS_TX = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"   # the board answers here

DEFAULT_NAME = "IPRO7AI-EVK"
# How long to wait for output to stop arriving before treating a reply as done.
# Commands like `vid play` and `spk tone` are silent for seconds while they run,
# so this is a gap timer, not a total timeout.
QUIET_S = 0.6


class Board:
    def __init__(self, client):
        self.client = client
        self._buf = bytearray()
        self._last = None

    def _on_notify(self, _sender, data: bytearray):
        self._buf.extend(data)
        self._last = asyncio.get_event_loop().time()
        sys.stdout.write(data.decode("utf-8", "replace"))
        sys.stdout.flush()

    async def start(self):
        await self.client.start_notify(NUS_TX, self._on_notify)

    async def run(self, cmd: str, settle: float = QUIET_S, limit: float = 90.0):
        """Send one command and wait for its output to go quiet."""
        self._buf.clear()
        self._last = asyncio.get_event_loop().time()
        payload = (cmd + "\n").encode()

        # Write without response where the peer allows it; the firmware accepts
        # both and this avoids a round trip per command.
        try:
            await self.client.write_gatt_char(NUS_RX, payload, response=False)
        except Exception:
            await self.client.write_gatt_char(NUS_RX, payload, response=True)

        started = asyncio.get_event_loop().time()
        while True:
            await asyncio.sleep(0.05)
            now = asyncio.get_event_loop().time()
            if self._buf and (now - self._last) > settle:
                break
            if not self._buf and (now - started) > settle * 4:
                break            # command produced nothing at all
            if (now - started) > limit:
                print("\n[timed out waiting for output]", file=sys.stderr)
                break
        return self._buf.decode("utf-8", "replace")


async def find(name: str, timeout: float):
    print(f"scanning for {name!r} ...", file=sys.stderr)
    dev = await BleakScanner.find_device_by_filter(
        lambda d, _ad: bool(d.name and name.lower() in d.name.lower()),
        timeout=timeout,
    )
    if dev is None:
        print(
            f"no board called {name!r} found.\n"
            "Run `bt init` on the serial console once so it advertises, and\n"
            "check it is not already connected to something else - the\n"
            "firmware allows a single connection.",
            file=sys.stderr,
        )
    return dev


async def scan(timeout: float):
    devs = await BleakScanner.discover(timeout=timeout)
    named = [d for d in devs if d.name]
    if not named:
        print("nothing advertising nearby.")
        return
    width = max(len(d.name) for d in named)
    for d in sorted(named, key=lambda x: x.name):
        print(f"  {d.name:<{width}}  {d.address}")


async def interactive(board: Board):
    print(
        "connected. Type a shell command, or `quit`.\n"
        "Everything the serial console takes works here - try `bt rf`, "
        "`vid status`, `sd mount`.\n"
    )
    loop = asyncio.get_event_loop()
    while True:
        try:
            line = await loop.run_in_executor(None, lambda: input("evk> "))
        except (EOFError, KeyboardInterrupt):
            print()
            return
        line = line.strip()
        if not line:
            continue
        if line in ("quit", "exit"):
            return
        await board.run(line)
        print()


async def main_async(args):
    if args.scan:
        await scan(args.timeout)
        return 0

    dev = await find(args.name, args.timeout)
    if dev is None:
        return 1

    async with BleakClient(dev) as client:
        board = Board(client)
        await board.start()

        if args.file:
            with open(args.file) as fh:
                cmds = [c.strip() for c in fh if c.strip()
                        and not c.lstrip().startswith("#")]
            for c in cmds:
                print(f"\nevk> {c}")
                await board.run(c)
            return 0

        if args.command:
            await board.run(" ".join(args.command))
            print()
            return 0

        await interactive(board)
        return 0


def main():
    ap = argparse.ArgumentParser(
        description="Drive the IPRO7AI EVK over BLE.",
        epilog="With no command, opens an interactive console.",
    )
    ap.add_argument("command", nargs="*", help="a single shell command to run")
    ap.add_argument("-n", "--name", default=DEFAULT_NAME,
                    help=f"advertised name to match (default {DEFAULT_NAME})")
    ap.add_argument("-f", "--file", help="run a file of commands, one per line")
    ap.add_argument("-t", "--timeout", type=float, default=10.0,
                    help="scan timeout in seconds")
    ap.add_argument("--scan", action="store_true",
                    help="list nearby devices and exit")
    args = ap.parse_args()

    try:
        return asyncio.run(main_async(args))
    except KeyboardInterrupt:
        return 130


if __name__ == "__main__":
    sys.exit(main())

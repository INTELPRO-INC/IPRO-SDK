#!/usr/bin/env python3
"""
Serial monitor for IPRO7 board test automation.
Uses termios (stdlib) for reliable serial I/O on macOS - no pyserial needed.

Usage:
  serial_monitor.py <port> [--baud 115200] [--timeout 30]
                           [--send CMD] [--wait-for PATTERN]

Examples:
  # Send reboot and wait for test results
  serial_monitor.py /dev/cu.usbserial-2120 --send reboot --wait-for "TESTS PASSED|TESTS FAILED"

  # Just monitor serial output
  serial_monitor.py /dev/cu.usbserial-2120 --timeout 10
"""

import sys
import os
import re
import errno
import time
import fcntl
import termios
import signal
import select
import argparse

# Global fd for cleanup in signal handler
_cleanup_fd = None
_cleanup_log = None


def _signal_handler(signum, frame):
    """Clean up and exit on SIGINT/SIGTERM."""
    if _cleanup_log:
        try:
            _cleanup_log.close()
        except Exception:
            pass
    if _cleanup_fd is not None:
        try:
            os.close(_cleanup_fd)
        except Exception:
            pass
    sig_name = "SIGINT" if signum == signal.SIGINT else "SIGTERM"
    print(f"\n[serial_monitor] {sig_name} received, exiting.", file=sys.stderr)
    sys.exit(130 if signum == signal.SIGINT else 143)


def open_serial(port, baud=115200, retries=3, retry_delay=1.0):
    """Open serial port with proper settings using termios.

    Retries on transient failures (port busy, device not ready).
    """
    baud_map = {
        9600: termios.B9600,
        19200: termios.B19200,
        38400: termios.B38400,
        57600: termios.B57600,
        115200: termios.B115200,
        230400: termios.B230400,
    }

    baud_const = baud_map.get(baud)
    if baud_const is None:
        print(f"Unsupported baud rate: {baud}", file=sys.stderr)
        sys.exit(1)

    last_err = None
    for attempt in range(1, retries + 1):
        try:
            # Open with O_RDWR | O_NOCTTY | O_NONBLOCK to avoid blocking on DCD
            fd = os.open(port, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
        except OSError as e:
            last_err = e
            if e.errno in (errno.EBUSY, errno.ENOENT, errno.ENXIO) and attempt < retries:
                print(
                    f"[serial_monitor] Port {port} not ready ({e}), "
                    f"retry {attempt}/{retries}...",
                    file=sys.stderr,
                )
                time.sleep(retry_delay)
                continue
            raise

        try:
            # Get current settings
            attrs = termios.tcgetattr(fd)

            # Set baud rate
            attrs[4] = baud_const  # ispeed
            attrs[5] = baud_const  # ospeed

            # Raw mode: no echo, no signals, no processing
            # cflag: 8N1, no flow control, enable receiver, local mode
            attrs[2] = termios.CS8 | termios.CREAD | termios.CLOCAL
            # iflag: no parity check, no flow control, no special handling
            attrs[0] = 0
            # oflag: no output processing
            attrs[1] = 0
            # lflag: no echo, no canonical mode, no signals
            attrs[3] = 0

            # cc: VMIN=0, VTIME=1 (100ms timeout per read)
            attrs[6][termios.VMIN] = 0
            attrs[6][termios.VTIME] = 1

            termios.tcsetattr(fd, termios.TCSANOW, attrs)

            # Keep O_NONBLOCK set — we use select() for readiness and
            # non-blocking read to avoid hanging if the port disappears.

            # Flush any stale data
            termios.tcflush(fd, termios.TCIOFLUSH)

            return fd

        except Exception:
            os.close(fd)
            raise

    # Should not reach here, but just in case
    raise last_err


def serial_write(fd, data, timeout=5.0):
    """Write data to serial port with timeout protection."""
    raw = data.encode() if isinstance(data, str) else data
    try:
        os.write(fd, raw)
    except OSError as e:
        print(f"[serial_monitor] Write failed: {e}", file=sys.stderr)
        return False

    # tcdrain can block forever if the port disappears.
    # Use a thread-based timeout to avoid hanging.
    import threading

    drain_done = threading.Event()

    def _drain():
        try:
            termios.tcdrain(fd)
        except OSError:
            pass
        drain_done.set()

    t = threading.Thread(target=_drain, daemon=True)
    t.start()
    if not drain_done.wait(timeout):
        print(
            f"[serial_monitor] Warning: tcdrain timed out after {timeout}s",
            file=sys.stderr,
        )
    return True


def serial_monitor(fd, timeout=30, wait_for=None, log_file=None):
    """
    Read serial output until timeout or pattern match.
    Returns (output_text, matched_pattern_or_None).
    """
    output_parts = []
    output_len = 0  # Track total length for efficient pattern search
    start = time.time()
    pattern = re.compile(wait_for) if wait_for else None
    consecutive_errors = 0
    max_consecutive_errors = 10

    while True:
        elapsed = time.time() - start
        if elapsed >= timeout:
            break

        # Wait for data with 0.5s poll interval
        remaining = timeout - elapsed
        try:
            ready, _, _ = select.select([fd], [], [], min(0.5, remaining))
        except (ValueError, OSError) as e:
            # fd became invalid (port disconnected)
            print(
                f"\n[serial_monitor] Port disconnected during select: {e}",
                file=sys.stderr,
            )
            break

        if not ready:
            consecutive_errors = 0  # Reset on successful select (just timeout)
            continue

        try:
            chunk = os.read(fd, 4096)
        except OSError as e:
            if e.errno == errno.EAGAIN:
                # Non-blocking read, no data available yet
                continue
            consecutive_errors += 1
            if consecutive_errors >= max_consecutive_errors:
                print(
                    f"\n[serial_monitor] Too many read errors ({e}), exiting.",
                    file=sys.stderr,
                )
                break
            continue

        if not chunk:
            # Empty read = EOF / port disconnected
            consecutive_errors += 1
            if consecutive_errors >= max_consecutive_errors:
                print(
                    "\n[serial_monitor] Port returned EOF repeatedly, "
                    "device may have disconnected.",
                    file=sys.stderr,
                )
                break
            time.sleep(0.1)
            continue

        # Successfully read data
        consecutive_errors = 0
        text = chunk.decode("utf-8", errors="replace")
        output_parts.append(text)
        prev_len = output_len
        output_len += len(text)

        # Print to stdout in real-time
        sys.stdout.write(text)
        sys.stdout.flush()

        # Write to log file if specified
        if log_file:
            log_file.write(text)
            log_file.flush()

        # Check for pattern match — only search the new data plus overlap
        if pattern:
            # Search with overlap to catch patterns split across chunks
            overlap = min(256, prev_len)
            if overlap > 0:
                # Reconstruct just the tail portion for searching
                search_start = prev_len - overlap
                # Build search text from relevant parts
                search_text = "".join(output_parts)
                search_region = search_text[search_start:]
            else:
                search_region = text

            m = pattern.search(search_region)
            if m:
                # Give a moment for trailing output
                time.sleep(0.5)
                _drain_remaining(fd, output_parts, log_file)
                return "".join(output_parts), m.group(0)

    return "".join(output_parts), None


def _drain_remaining(fd, output_parts, log_file, drain_time=0.3):
    """Read any remaining data after a pattern match."""
    deadline = time.time() + drain_time
    while time.time() < deadline:
        try:
            ready, _, _ = select.select([fd], [], [], 0.1)
        except (ValueError, OSError):
            break
        if not ready:
            break
        try:
            tail = os.read(fd, 4096)
            if not tail:
                break
            text = tail.decode("utf-8", errors="replace")
            output_parts.append(text)
            sys.stdout.write(text)
            sys.stdout.flush()
            if log_file:
                log_file.write(text)
                log_file.flush()
        except OSError:
            break


def main():
    global _cleanup_fd, _cleanup_log

    parser = argparse.ArgumentParser(description="Serial monitor for IPRO7 board")
    parser.add_argument("port", help="Serial port (e.g., /dev/cu.usbserial-2120)")
    parser.add_argument("--baud", type=int, default=115200, help="Baud rate")
    parser.add_argument("--timeout", type=int, default=30, help="Timeout in seconds")
    parser.add_argument("--send", help="Command to send before monitoring")
    parser.add_argument("--wait-for", help="Regex pattern to wait for")
    parser.add_argument("--log", help="Save raw serial output to file")
    args = parser.parse_args()

    # Install signal handlers for clean shutdown
    signal.signal(signal.SIGINT, _signal_handler)
    signal.signal(signal.SIGTERM, _signal_handler)

    try:
        fd = open_serial(args.port, args.baud)
    except OSError as e:
        print(f"Failed to open {args.port}: {e}", file=sys.stderr)
        sys.exit(3)

    _cleanup_fd = fd

    log_fh = None
    if args.log:
        os.makedirs(os.path.dirname(args.log) or ".", exist_ok=True)
        log_fh = open(args.log, "w")
        _cleanup_log = log_fh

    try:
        # Send command if specified
        if args.send:
            if not serial_write(fd, f"{args.send}\r\n"):
                print(
                    f"[serial_monitor] Failed to send command: {args.send}",
                    file=sys.stderr,
                )
                sys.exit(3)
            print(f"[serial_monitor] Sent: {args.send}", file=sys.stderr)

        # Monitor output
        output, match = serial_monitor(fd, args.timeout, args.wait_for, log_file=log_fh)

        if match:
            if "PASSED" in match:
                print(f"\n[serial_monitor] MATCHED: {match}", file=sys.stderr)
                sys.exit(0)
            else:
                print(f"\n[serial_monitor] MATCHED: {match}", file=sys.stderr)
                sys.exit(1)
        else:
            print(f"\n[serial_monitor] TIMEOUT after {args.timeout}s", file=sys.stderr)
            sys.exit(2)
    finally:
        _cleanup_fd = None
        _cleanup_log = None
        if log_fh:
            log_fh.close()
        os.close(fd)


if __name__ == "__main__":
    main()

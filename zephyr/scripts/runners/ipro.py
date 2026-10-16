# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0
'''
`west flash` runner for IPRO boards: puts the board in download mode through
an IPRO7 CMSIS-DAP probe (in-band IPRODL_x / IPRORST_x keywords on the
probe's UART bridge) or through the running shell's `reboot_dl`, then writes
zephyr.bin with the IntelProCube CLI (ipro_iot_tool), and resets the board.

    west flash                                   # board defaults from board.cmake
    west flash -- --port /dev/cu.usbmodem31202   # probe bridge = flash UART
    west flash -- --boot shell                   # no probe: `reboot_dl` on the console
    west flash -- --boot manual                  # you hold BOOT and reset
    west flash -- --no-reset                     # stay in download mode afterwards

Registered through zephyr/module.yml (runners:), configured per board with
board_runner_args(ipro "--chipname=ipro7" "--pt=partition_cfg_2M.toml").
'''

import os
import shutil
import subprocess
import sys
import termios
import time
from pathlib import Path

from runners.core import RunnerCaps, RunnerConfig, ZephyrBinaryRunner

# IntelProCube CLI names looked up on PATH when neither --tool nor
# $IPRO_FLASH_TOOL names the binary.
DEFAULT_TOOL_NAMES = ('ipro_iot_tool_macos', 'ipro_iot_tool_linux', 'ipro_iot_tool')
PROBE_BAUD = 115200
# Probe strap timing (tools/security/security_cert_run.py::dut_download):
# settle before the reset edge, keep the strap through the probe's 100 ms
# pulse plus a hold so the bootrom samples it.
SETTLE_S = 0.05
PULSE_S = 0.10
HOLD_S = 0.3
TOOL_FAIL_MARKERS = ('SHAKEHAND FAIL', 'Burn return with retry fail', 'ErrorCode', 'Error')
TOOL_OK_MARKER = 'All Success'


def _open_serial(port, baud):
    """termios open; tools/serial_monitor.py knows the macOS IOSSIOSPEED ioctl
    for non-standard rates (2 Mbaud console), fall back to plain termios."""
    sdk_tools = Path(__file__).resolve().parents[3] / 'tools'
    if str(sdk_tools) not in sys.path:
        sys.path.insert(0, str(sdk_tools))
    try:
        from serial_monitor import open_serial  # noqa: PLC0415
        return open_serial(port, baud, retries=3, retry_delay=0.5)
    except ImportError:
        pass
    fd = os.open(port, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
    attrs = termios.tcgetattr(fd)
    attrs[0] = 0
    attrs[1] = 0
    attrs[2] = termios.CS8 | termios.CREAD | termios.CLOCAL
    attrs[3] = 0
    speed = getattr(termios, f'B{baud}', None)
    if speed is None:
        raise ValueError(f'unsupported baud {baud}')
    attrs[4] = speed
    attrs[5] = speed
    termios.tcsetattr(fd, termios.TCSANOW, attrs)
    return fd


def _serial_send(fd, text, gap):
    os.write(fd, text.encode())
    time.sleep(gap)


class IproBinaryRunner(ZephyrBinaryRunner):
    '''IPRO CMSIS-DAP probe + IntelProCube CLI runner.'''

    def __init__(self, cfg, chipname, port, probe_port, tool, pt, flash_pin, baudrate, boot,
                 console_baud, no_reset, attempts, extra_tool_args):
        super().__init__(cfg)
        self.chipname = chipname
        self.port = port
        self.probe_port = probe_port or port  # None until autodetected
        self.tool = tool
        self.pt = pt
        self.flash_pin = flash_pin
        self.baudrate = baudrate
        self.boot = boot
        self.console_baud = console_baud
        self.no_reset = no_reset
        self.attempts = attempts
        self.extra_tool_args = extra_tool_args

    @classmethod
    def name(cls):
        return 'ipro'

    @classmethod
    def capabilities(cls):
        return RunnerCaps(commands={'flash'}, file=True, reset=True)

    @classmethod
    def do_add_parser(cls, parser):
        parser.add_argument('--chipname', default='ipro7',
                            help='IntelProCube chip name: ipro7 (default) or ipro6')
        parser.add_argument('--port', default=os.environ.get('IPRO_SERIAL_PORT'),
                            help='flash UART (/dev/cu.usbmodem*, /dev/cu.usbserial*); '
                                 'default $IPRO_SERIAL_PORT')
        parser.add_argument('--probe-port', default=os.environ.get('IPRO_PROBE_PORT'),
                            help='probe UART bridge that takes the IPRODL/IPRORST keywords; '
                                 'default: same as --port ($IPRO_PROBE_PORT)')
        parser.add_argument('--tool', default=os.environ.get('IPRO_FLASH_TOOL'),
                            help='ipro_iot_tool binary; default $IPRO_FLASH_TOOL or the '
                                 'IntelProCube CLI on PATH')
        parser.add_argument('--pt', default=None,
                            help='partition table (.toml, name under chips/<chip>/partition '
                                 'of the tool, or a path)')
        parser.add_argument('--console-baud', type=int, default=2000000,
                            help='console baud for --boot shell (default 2000000)')
        parser.add_argument('--flash-pin', default='0x9',
                            help='flash pin config of the eflash loader (0x9 EVB/EVK/Arduino, '
                                 '0x2 unit-test board; default 0x9)')
        parser.add_argument('--baudrate', type=int, default=2000000,
                            help='flash UART baud (default 2000000)')
        parser.add_argument('--boot', choices=('probe', 'shell', 'manual'), default='probe',
                            help='how the board enters download mode: probe strap keywords '
                                 '(default), the running shell\'s reboot_dl, or by hand')
        parser.add_argument('--attempts', type=int, default=3,
                            help='flash attempts before giving up (default 3)')
        parser.add_argument('--tool-arg', action='append', default=[],
                            help='extra argument passed to the tool (repeatable)')

    @classmethod
    def do_create(cls, cfg, args):
        return IproBinaryRunner(cfg, args.chipname, args.port, args.probe_port, args.tool,
                                args.pt, args.flash_pin, args.baudrate, args.boot,
                                args.console_baud, not args.reset,
                                args.attempts, args.tool_arg)

    # ---- helpers ---------------------------------------------------------

    def sdk_root(self):
        return Path(__file__).resolve().parents[3]

    def find_tool(self):
        if self.tool:
            # a path, or a command name on PATH
            found = self.tool if '/' in self.tool else shutil.which(self.tool) or self.tool
            tool = Path(found)
            if not tool.is_file():
                raise RuntimeError(f'flash tool not found: {self.tool}')
            return tool
        for name in DEFAULT_TOOL_NAMES:
            found = shutil.which(name)
            if found:
                return Path(found)
        raise RuntimeError('no ipro_iot_tool found: pass --tool, set IPRO_FLASH_TOOL '
                           'or put the IntelProCube CLI on PATH')

    def resolve_pt(self, tool):
        if self.pt is None:
            return None
        pt = Path(self.pt)
        if pt.is_file():
            return pt.resolve()
        looked = []
        for base in (tool.parent,):
            candidate = base / 'chips' / self.chipname / 'partition' / self.pt
            if candidate.is_file():
                return candidate
            looked.append(str(candidate.parent))
        raise RuntimeError(f'partition table not found: {self.pt} (looked in {", ".join(looked)})')

    def pick_image(self):
        """The image to write: --file, or zephyr.bin, or - under sysbuild with
        MCUboot - the bootloader and the signed application merged into the
        one FW image boot2 loads (tools/zephyr/merge_mcuboot.py); the
        mcuboot domain itself is then skipped."""
        build_dir = Path(self.cfg.build_dir).resolve()
        if self.cfg.file:
            return Path(self.cfg.file)
        sysbuild_dir = build_dir.parent
        if (sysbuild_dir / 'domains.yaml').is_file() and (sysbuild_dir / 'mcuboot').is_dir():
            if build_dir.name == 'mcuboot':
                self.logger.info('mcuboot domain: written together with the application '
                                 'as one merged image')
                return None
            merged = sysbuild_dir / 'merged.bin'
            merge = self.sdk_root() / 'tools' / 'zephyr' / 'merge_mcuboot.py'
            self.check_call([sys.executable, str(merge), str(sysbuild_dir), '-o', str(merged)])
            return merged
        image = Path(self.cfg.bin_file)
        if not image.is_file():
            raise RuntimeError(f'image not found: {image}')
        return image

    def autodetect_port(self):
        import glob

        ports = sorted(glob.glob('/dev/cu.usbmodem*') + glob.glob('/dev/cu.usbserial*') +
                       glob.glob('/dev/ttyACM*') + glob.glob('/dev/ttyUSB*'))
        if len(ports) == 1:
            self.logger.info(f'using {ports[0]}')
            return ports[0]
        if not ports:
            raise RuntimeError('no serial port found: pass --port or set IPRO_SERIAL_PORT')
        raise RuntimeError('several serial ports, pass --port or set IPRO_SERIAL_PORT: ' +
                           ' '.join(ports))

    def probe(self, *words, gaps=()):
        fd = _open_serial(self.probe_port, PROBE_BAUD)
        try:
            for i, w in enumerate(words):
                _serial_send(fd, w + '\r\n', gaps[i] if i < len(gaps) else 0.0)
        finally:
            os.close(fd)

    def enter_download(self):
        if self.boot == 'probe':
            self.logger.info(f'probe {self.probe_port}: download strap + reset')
            self.probe('IPRODL_1', 'IPRORST_RESET', 'IPRODL_0',
                       gaps=(SETTLE_S, PULSE_S + HOLD_S, 0.0))
        elif self.boot == 'shell':
            self.logger.info(f'{self.port}: reboot_dl on the console')
            fd = _open_serial(self.port, self.console_baud)
            try:
                _serial_send(fd, '\r\nreboot_dl\r\n', 1.0)
            finally:
                os.close(fd)
        else:
            self.logger.info('hold BOOT, reset the board, then press Enter')
            sys.stdin.readline()

    def reset(self):
        if self.boot == 'probe':
            self.probe('IPRORST_RESET', gaps=(PULSE_S,))

    def run_tool(self, tool, pt, image):
        cmd = [str(tool), '--chipname', self.chipname, '--interface', 'uart', '--port', self.port,
               '--baudrate', str(self.baudrate), '--firmware', str(image.resolve()),
               '--skip-ota']
        # Only the lite tool takes the flash pin on the command line; the
        # full IntelProCube CLI reads it from its eflash_loader_cfg.ini.
        if 'lite' in tool.name:
            cmd += ['--flash-pin', self.flash_pin]
        if pt is not None:
            cmd += ['--pt', str(pt)]
        cmd += self.extra_tool_args
        self.logger.info(' '.join(cmd))
        proc = subprocess.run(cmd, cwd=tool.parent, capture_output=True, text=True)
        out = proc.stdout + proc.stderr
        for line in out.splitlines():
            if any(k in line for k in ('Chip name', 'Firmware', 'Partition', 'Success', 'FAIL',
                                       'fail', 'Error', 'error', 'time cost')):
                self.logger.info(line.strip())
        ok = TOOL_OK_MARKER in out and not any(m in out for m in TOOL_FAIL_MARKERS[:3])
        if not ok:
            log = Path(self.cfg.build_dir) / 'ipro_flash.log'
            log.write_text(out)
            self.logger.warning(f'tool output kept in {log}')
        return ok

    # ---- run -------------------------------------------------------------

    def do_run(self, command, **kwargs):
        if command != 'flash':
            raise ValueError(f'unsupported command {command}')
        if not self.port:
            self.port = self.autodetect_port()
            if self.probe_port is None:
                self.probe_port = self.port
        if not os.path.exists(self.port):
            raise RuntimeError(f'serial port not found: {self.port}')
        image = self.pick_image()
        if image is None:
            return
        tool = self.find_tool()
        pt = self.resolve_pt(tool)
        self.logger.info(f'{image.name} ({image.stat().st_size} B) -> {self.chipname} on '
                         f'{self.port} @ {self.baudrate}, pt {pt.name if pt else "(tool default)"}')
        for attempt in range(1, self.attempts + 1):
            self.enter_download()
            if self.run_tool(tool, pt, image):
                break
            self.logger.warning(f'flash attempt {attempt}/{self.attempts} failed')
            time.sleep(1.0)
        else:
            raise RuntimeError('flashing failed (is the board in download mode? '
                               'try --boot shell or --boot manual)')
        if self.no_reset:
            self.logger.info('leaving the board in download mode (--no-reset)')
        else:
            self.reset()
            self.logger.info('flashed and reset')

# IPRO Unit Test - Board Testing Workflow

## Quick Reference

```bash
# Build
make -C apps/test/ipro_unit_test

# Full cycle: build + flash + test on board
tools/board_test.sh --project apps/test/ipro_unit_test

# Skip build (flash + test only)
tools/board_test.sh --project apps/test/ipro_unit_test --skip-build

# Quick retest (reboot board and capture results, no build/flash)
python3 tools/serial_monitor.py /dev/cu.usbserial-2120 --send reboot --timeout 30 --wait-for "ALL TESTS PASSED|SOME TESTS FAILED"
```

## Hardware Setup

- **Board**: IPRO7 EVB connected via USB serial
- **Serial port**: `/dev/cu.usbserial-2120` (115200 baud) — **must use `cu.*` on macOS** (`tty.*` blocks on DCD)
- **Flash tool**: `tools/ipro_iot_tool_lite/ipro_iot_tool_lite` (PyInstaller-bundled native binary)
- **Board commands**: `reboot` (normal reset), `reboot_dl` (enter download mode for flashing)

## Test Behavior

- `CONFIG_TEST_AUTO_RUN=y` is enabled, so tests run automatically on boot
- Output goes to UART at 115200 baud
- Look for `ALL TESTS PASSED` or `SOME TESTS FAILED` in serial output
- `board_test.sh` exit codes: 0 = pass, 1 = fail, 2 = timeout

## Test Groups

Tests are organized into 4 groups:

| Group | Shell Command | Description | Auto-Skip |
|-------|--------------|-------------|-----------|
| `sw` | `test_group sw` | Software tests (no HW dependency) | Never |
| `hal` | `test_group hal` | HAL driver tests (GPIO, Timer, Flash, etc.) | When HW not detected |
| `crypto` | `test_group crypto` | HW security engine (AES, SHA, TRNG, GMAC) | When engine init fails |
| `ble` | `test_group ble` | Bluetooth stack tests | When BLE init fails |

### Shell Commands

- `test_all` — Run all test suites (all groups)
- `test_group <name>` — Run specific group (`sw`, `hal`, `crypto`, `ble`)
- `test_run <suite>` — Run specific suite by name
- `test_list` — Show all available suites grouped by category
- `test_status` — Show test results summary

## Serial Communication

- **`tools/serial_monitor.py`** — Python3 termios-based serial I/O (no pyserial dependency)
- Critical: Must use a single process to open port, configure, and read/write (macOS resets port settings on close)
- The monitor opens port with `O_NOCTTY | O_NONBLOCK` flags to avoid DCD blocking

## Adding Tests

1. Create `tests/test_<name>.c` with Unity test functions using `TEST_CASE()` and `RUN_TEST()` macros
2. Add `CONFIG_TEST_<NAME>=y` to `.config`
3. Register in `test_runner.c`:
   - Add `#ifdef CONFIG_TEST_<NAME>` / `extern void test_<name>_run(void);` / `#endif`
   - Add entry in `test_suites[]` with `.group = TEST_GROUP_<GROUP>`
4. Add source file to `CMakeLists.txt` inside `if (CONFIG_TEST_<NAME>)` block

### Available Unity Macros

```c
TEST_ASSERT(condition)
TEST_ASSERT_TRUE(condition)
TEST_ASSERT_FALSE(condition)
TEST_ASSERT_EQUAL_INT(expected, actual)
TEST_ASSERT_EQUAL_UINT(expected, actual)
TEST_ASSERT_EQUAL_PTR(expected, actual)
TEST_ASSERT_EQUAL_STRING(expected, actual)
TEST_ASSERT_EQUAL_MEMORY(expected, actual, len)
TEST_ASSERT_NOT_NULL(ptr)
TEST_ASSERT_NULL(ptr)
TEST_FAIL(msg)
TEST_PASS()
TEST_IGNORE_MESSAGE(msg)
```

**Note**: This SDK's Unity is a simplified version. Do NOT use `TEST_ASSERT_MESSAGE`, `TEST_FAIL_MESSAGE`, `TEST_ASSERT_EQUAL_HEX8`, `TEST_ASSERT_GREATER_THAN`, or other extended macros.

### Known Limitations

- `utils_md5.h` and `utils_sha1.h` require `iot_import.h` (Alibaba IoT platform) — not available, so MD5/SHA1/HMAC tests are disabled
- Shell + platform_utils have duplicate `Ring_Buffer` symbols — handled via `--allow-multiple-definition` linker flag

## Makefile Targets

- `make` - Build the project
- `make flash` - Flash firmware to board
- `make monitor` - Open serial monitor (Ctrl-C to exit)
- `make test` - Full cycle: build + flash + capture results

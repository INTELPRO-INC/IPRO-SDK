# IPRO BLE Throughput Test

A two-board BLE throughput benchmark for IPRO7. One board is the peripheral (slave), the
other the central (master); either side can flood the other through a custom GATT
service. On two IPRO7AI Reef boards it reaches **1.34 Mbps on 2M PHY** and **747 kbps on
1M PHY**, in either direction.

> **`ipro_throughput_test` vs `ipro_throughput_demo`.** Same CLI and GATT service.
> This app is the plain shell-driven benchmark shipped in the SDK release packages and
> the one tuned below (`CONFIG_IPRO7_BLE_HOTCODE`). `apps/bluetooth/ble/ipro_throughput_demo`
> adds a framed protocol (`throughput_protocol.{c,h}`) for the Android
> `IproThroughputDemo` companion app and keeps its own configuration; the numbers in this
> README do not apply to it.

## Features

- Peripheral (slave) and central (master) roles
- 1M / 2M / Coded PHY switching at run time
- Data Length Extension (251 octets) and ATT MTU 247
- Connection interval 7.5 ms to 4 s, changeable while connected
- Flood mode or a fixed TX interval; notifications slave → master, writes without
  response master → slave, or both at once
- Statistics every second and a summary at the end of each test
- `tp cpu`: per-task CPU share and stack headroom, to tell a radio limit from a CPU limit
- Interactive shell CLI

## Architecture

```
ipro_throughput_test/
├── main.c                  # Platform init, BLE controller start-up, FreeRTOS scheduler
├── throughput_app.c/.h     # Connection management, state machine, TX task, statistics
├── throughput_service.c/.h # GATT service (slave side): TX notify + RX write
├── throughput_client.c/.h  # GATT client (master side): discovery, subscription, writes
├── throughput_cli.c/.h     # `tp` shell commands
├── .config                 # IPRO7 EVB configuration (default)
├── defconfig_reef          # IPRO7AI Reef configuration
├── CMakeLists.txt          # Build configuration
└── OPERATION_MANUAL.md     # Operation manual (Chinese)
```

### Modules

| Module | Responsibility |
|--------|---------------|
| `throughput_app` | BLE initialization, GAP connection callbacks, PHY/DLE/MTU negotiation, TX task, statistics |
| `throughput_service` | Slave-side GATT service with a custom UUID: TX (notify) and RX (write without response) characteristics |
| `throughput_client` | Master-side GATT client: service discovery, notification subscription, writes |
| `throughput_cli` | Shell command interface for all `tp` subcommands |

### Data Flow

```
Slave (Peripheral)                    Master (Central)
┌─────────────────┐                  ┌─────────────────┐
│  throughput_app  │                  │  throughput_app  │
│    TX Task       │                  │    TX Task       │
│       │          │                  │       │          │
│       ▼          │                  │       ▼          │
│  throughput_     │   BLE Notify     │  throughput_     │
│  service_notify ─┼─────────────────►│  client (RX cb)  │
│                  │                  │                  │
│  throughput_     │   BLE Write      │  throughput_     │
│  service (RX cb)◄┼──────────────────┼─ client_write    │
└─────────────────┘                  └─────────────────┘
```

## Getting Started

### Prerequisites

- Two IPRO7 boards: IPRO7 EVB, or IPRO7AI Reef with an IPRO Debug Probe on each
- RISC-V cross-compiler (`/opt/toolchain/riscv_ipro7/`)
- IntelProCube v0.6.3 (`ipro_iot_tool_macos`) for flashing
- macOS or Linux host

Each Debug Probe enumerates as `IPRO7 CMSIS_DAP` with two CDC ports. The one whose name
ends in `02` (for example `/dev/cu.usbmodem11622102`) is the UART bridge to the board's
console (Reef UART0, GPIO20/21); the `04` port is the CCI bridge. The console runs at
115200 baud. On macOS always open the `/dev/cu.*` node, and close any terminal app that
holds the port before flashing.

### Build

```bash
make -C apps/bluetooth/ble/ipro_throughput_test                            # IPRO7 EVB (.config)
make -C apps/bluetooth/ble/ipro_throughput_test DEFCONFIG=defconfig_reef   # IPRO7AI Reef
```

Output: `build/build_out/ipro_throughput_test_IPRO7.bin` (EVB) or
`build_reef/build_out/ipro_throughput_test_IPRO7.bin` (Reef). Both use about 360 KB of
flash and 205 KB of the 224 KB OCRAM region.

### Flash

Use the IntelProCube `ipro_iot_tool_macos`; the SDK's `tools/ipro_iot_tool_lite` is
retired. The tool has no `--flash-pin` option: set `flash_pin` in
`<IntelProCube>/chips/ipro7/eflash_loader/eflash_loader_cfg.ini` to `0x2` for the EVB or
`0x9` for the Reef (QFN56), and put it back afterwards because the ini is shared by every
board you flash.

1. Put the board in download mode. While the firmware runs, type `reboot_dl` in its
   shell. If the firmware does not answer, hold BOOT (SW1), tap RESET (SW2) and release
   BOOT; or write `IPRODL_1` and then `IPRORST_RESET` to the probe's `02` port and keep
   DL high until the flash completes (`IPRODL_0` and `IPRORST_RESET` afterwards).
2. Flash:

   ```bash
   <IntelProCube>/ipro_iot_tool_macos --chipname ipro7 \
       --port /dev/cu.usbmodemXXXX02 \
       --firmware apps/bluetooth/ble/ipro_throughput_test/build_reef/build_out/ipro_throughput_test_IPRO7.bin \
       --baudrate 921600
   ```

3. Check the log for `[All Success]`. The tool exits with status 0 even when a step fails.

### Run Test

Open one terminal per board:

```bash
python3 tools/serial_monitor.py /dev/cu.usbmodemXXXX02   # Slave
python3 tools/serial_monitor.py /dev/cu.usbmodemYYYY02   # Master
```

A Reef board prints `BLE initialized` about 7 s after reset. Most of that is the
controller qualifying its 32 kHz clock: the board has no 32.768 kHz crystal, so the log
shows `XTAL32K rejected` and the controller falls back to RC32K.

```
[Slave]  tp slave              # Advertise as IPRO-TP-TEST
[Master] tp master             # Scan for IPRO-TP-TEST and connect
         (wait for "Notifications enabled - ready for test" on the master)
[Master] tp conn 40 40         # 50 ms connection interval for bulk data
[Master] tp phy 2              # 2M PHY
[Slave]  tp start 244 10       # Slave -> master: 244 B notifications for 10 s
[Master] tp start 244 10       # Master -> slave: 244 B writes without response for 10 s
```

- Start both sides at the same time for a bidirectional test.
- Each test ends with a summary on the sending board. Its TX figure counts packets the
  host stack accepted, which is within a few packets of what was delivered.
- The receiving board's RX counters accumulate across tests and its RX rate is not
  meaningful. To measure what actually arrived, run `tp stats` on the receiver before
  and after the test and divide the byte difference by the sender's test duration.
- With more than one `IPRO-TP-TEST` advertiser in range, give the master an address:
  `tp master XX:XX:XX:XX:XX:XX`.

## CLI Commands

| Command | Description |
|---------|-------------|
| `tp slave` | Start as peripheral and advertise |
| `tp master [addr]` | Start as central, scan and connect (optionally to one address) |
| `tp start [size] [sec] [interval_ms]` | Start a test from this board |
| `tp stop` | Stop the test, or disconnect when idle |
| `tp stats` | Print statistics |
| `tp status` | Print role, state, connection interval and PHY |
| `tp phy <1\|2\|3>` | Request 1M, 2M or Coded PHY |
| `tp conn <min> <max>` | Request a connection interval in 1.25 ms units (latency 0, supervision timeout 1 s) |
| `tp interval <ms>` | Change the TX interval of a running test |
| `tp cpu` | CPU share per task since the previous `tp cpu` (the first call covers everything since boot), and each task's minimum free stack |
| `tp help` | Print command help |

### `tp start` Parameters

| Parameter | Default | Range | Description |
|-----------|---------|-------|-------------|
| size | 244 | 20–244 | Payload per notification or write, in bytes |
| sec | 10 | 0 = until `tp stop` | Test duration in seconds |
| interval_ms | flood | 1–1000 | Gap between packets. Leave it out for flood mode: an explicit `0` becomes 1 ms |

## Performance Benchmarks

Two IPRO7AI Reef boards on a bench, prebuilt `link2` controller,
ATT MTU 247, DLE 251 octets, 8 s flood. The rate is counted from the packets the
receiver got (2026-10-07). "Down" is slave → master notifications, "up" is
master → slave writes without response.

| PHY | Interval | Payload | Down / up, `CONFIG_IPRO7_BLE_HOTCODE=y` | Down / up, option off (code in flash) |
|-----|----------|---------|------------------------------------------|----------------------------------------|
| 2M | 7.5 ms | 244 B | 1055 / 1056 kbps | 891 / 957 kbps |
| 2M | 15 ms | 244 B | 1180 / 1180 kbps | 921 / 930 kbps |
| 2M | 30 ms | 244 B | 1315 / 1315 kbps | 737 / 788 kbps |
| 2M | 50 ms | 244 B | **1342 / 1333 kbps** | 593 / 642 kbps |
| 1M | 7.5 ms | 244 B | 526 / 525 kbps | 437 / 492 kbps |
| 1M | 15 ms | 244 B | 656 / 656 kbps | 559 / 554 kbps |
| 1M | 30 ms | 244 B | 722 / 721 kbps | 584 / 551 kbps |
| 1M | 50 ms | 244 B | **747 / 746 kbps** | 535 / 568 kbps |
| 1M | 7.5 ms | 20 B (MTU 23 sized) | 165 / 165 kbps | — |
| 1M | 15 ms | 20 B (MTU 23 sized) | 163 / 163 kbps | — |
| Coded S8 | 7.5 ms | 244 B | 34 kbps (down) | — |

Both directions at once, `CONFIG_IPRO7_BLE_HOTCODE=y`, rate per direction:

| PHY | Interval | Payload | Down | Up |
|-----|----------|---------|------|----|
| 2M | 15 ms | 244 B | 653 kbps | 654 kbps |
| 2M | 30 ms | 244 B | 719 kbps | 719 kbps |
| 2M | 50 ms | 244 B | **782 kbps** | **783 kbps** |
| 1M | 50 ms | 244 B | 389 kbps | 390 kbps |

With the option on, repeated runs agree within about 1 % (2M, 30 ms: 1314–1317 kbps
over four runs). With it off the same case spread by up to 25 % (2M, 30 ms down:
737–934 kbps over five runs).

### What limits each case

- **Air time.** One 251-octet data PDU plus the empty acknowledgment and two inter-frame
  spaces takes 1.39 ms on 2M and 2.47 ms on 1M, so 244-byte payloads top out at about
  1.40 Mbps on 2M and 790 kbps on 1M. Both directions at once exchange 251 octets each
  way per round trip, about 815 kbps per direction on 2M. At 50 ms the measured rates
  are 4–6 % below these figures.
- **Connection event length.** The controller ends each event early enough to program
  the next one, so a 7.5 ms interval carries about four 2M packets (about 1.05 Mbps)
  and two 1M packets. Longer intervals lose a smaller share to the gap.
- **CPU.** With the code in flash, the controller, the Zephyr host and the FreeRTOS
  kernel spent about 1.8 ms of CPU time per packet in I-cache misses on the sending
  board, more than the 1.39 ms the packet spends on air. Throughput was then set by
  the CPU, and longer intervals made it worse. `CONFIG_IPRO7_BLE_HOTCODE` moves that
  path to OCRAM; at 1.3 Mbps the sender's CPU is then about half idle. 20-byte packets
  are still CPU-bound at roughly 1000 packets per second.
- **Coded PHY is not tuned.** The app asks for a 2120 µs maximum TX time, which on Coded
  S8 fits only about 27 octets per PDU.

## Tuning

- **Bulk data:** 2M PHY, 30–50 ms interval, 244-byte payloads. 15 ms still gives
  1.18 Mbps when latency matters more.
- **1M only:** use 30–50 ms; 15 ms gives 656 kbps.
- **Small packets:** with an ATT MTU of 23 (20-byte payloads) the interval hardly
  matters; expect about 165 kbps on 1M.
- **Check the CPU before changing buffers.** Run `tp cpu` on the sender, start a test,
  and run `tp cpu` again when it ends. An `IDLE` share far below 50 % means the CPU is
  the limit; more host or controller buffers will not help.
- **Other apps** on IPRO7 can enable `CONFIG_IPRO7_BLE_HOTCODE` if they have about 88 KB
  of OCRAM to spare. The linker script
  generator rejects the option with the AI+MM split layout and with PDS / low-power
  configurations (`CONFIG_HAL_LP`, `CONFIG_IPRO_PM_*`, `CONFIG_OCRAM_NORET_SPLIT`).

## Configuration

Key `.config` entries:

```ini
CONFIG_IPRO_BLE_CTLR_LIB="link2"          # prebuilt controller: 2 links, 9 ACL buffers
CONFIG_IPRO_BLE_CTLR_FEATURE="bt52"
CONFIG_BT_CONN=2
CONFIG_EM_SIZE=32
CONFIG_BT_L2CAP_TX_BUF_COUNT=10
CONFIG_BT_CONN_TX_MAX=10
CONFIG_IPRO7_BLE_HOTCODE=y                # BLE per-packet path runs from OCRAM
CONFIG_FREERTOS_TIMER_TASK_STACK_WORDS=1024
```

- `CONFIG_IPRO7_BLE_HOTCODE` takes its object list from `IPRO7_BLE_HOTCODE_OBJECTS` in
  `bsp/board/linker/configs/ipro7_flash.cmake`. `port.c` and `memcpy` stay in flash on
  purpose; the comment there explains why.
- Without the timer-stack line the timer task keeps a 16 KB stack, and the heap left
  after the hot code is too small for the app's start-up allocations. Under a flood the
  timer task used less than 400 bytes of the 4 KB.
- `defconfig_reef` is `.config` with `CONFIG_EVB=y` replaced by the Reef board
  selection; keep the two in step.

## Troubleshooting

| Symptom | Cause and fix |
|---------|---------------|
| `IPRO FLASH WRITE CHECK FAIL` on every retry | Seen with some images. Set `decompress_write = false` in the IntelProCube ini, flash, then restore it. |
| `flash jedec id: 00000000`, `IPRO FLASH MATCH TYPE FAIL` | Wrong `flash_pin` in the ini (`0x9` for the Reef, `0x2` for the EVB). |
| No console output after flashing a custom build | Something called before `.tcmcode` is copied was moved into it. Keep `memcpy` and `port.c` out of `IPRO7_BLE_HOTCODE_OBJECTS`. |
| `Malloc failed ... task: TPTest` at start-up | Timer task stack too large for the heap left by the hot code; set `CONFIG_FREERTOS_TIMER_TASK_STACK_WORDS=1024`. |
| `relocation truncated to fit: R_RISCV_JAL` at link time | An object reached with `jal` from flash was moved to OCRAM; keep it in flash. |
| Throughput far below the table with the option on | Check `tp status` for the interval and PHY, then `tp cpu` on the sender. |
| Link drops with reason 0x08 about 1 s after `tp phy 3` | Seen in 2 of 4 switches from 2M to Coded on a build without the option, and in none of 3 since. Reconnect and switch again. |

## Related Documentation

- [OPERATION_MANUAL.md](OPERATION_MANUAL.md) — operation manual in Chinese: hardware
  setup, test workflow, troubleshooting. Its flashing steps still use the retired
  `ipro_iot_tool_lite`; use the Flash section above instead.

## API Reference

### throughput_app

```c
int  throughput_app_init(void);                              // BLE enable + TX task
int  throughput_app_start_slave(void);                       // Advertise as IPRO-TP-TEST
int  throughput_app_start_master(const char *target_addr);   // Scan and connect (NULL = by name)
int  throughput_app_start_test(const throughput_test_config_t *config);
int  throughput_app_stop_test(void);
int  throughput_app_stop(void);                              // Stop and disconnect
int  throughput_app_get_stats(throughput_stats_t *stats);
void throughput_app_print_stats(void);
throughput_app_ctx_t *throughput_app_get_ctx(void);
int  throughput_app_update_conn_params(uint16_t interval_min, uint16_t interval_max,
                                       uint16_t latency, uint16_t timeout);
int  throughput_app_set_tx_interval(uint32_t interval_ms);
int  throughput_app_set_phy(uint8_t phy);                    // 1 = 1M, 2 = 2M, 3 = Coded
```

### throughput_service (slave side)

```c
int  throughput_service_init(const throughput_svc_config_t *config);  // Register the GATT service
int  throughput_service_notify(struct bt_conn *conn,
                               const uint8_t *data, uint16_t length); // Send a notification
bool throughput_service_is_notify_enabled(void);                      // CCCD state
```

### throughput_client (master side)

```c
int  throughput_client_init(const throughput_client_config_t *config);
int  throughput_client_discover(struct bt_conn *conn);                  // Service discovery
int  throughput_client_enable_notify(struct bt_conn *conn);             // Subscribe
int  throughput_client_write(struct bt_conn *conn,
                             const uint8_t *data, uint16_t length);     // Write without response
const throughput_client_handles_t *throughput_client_get_handles(void);
bool throughput_client_is_ready(void);
int  throughput_client_deinit(void);
```

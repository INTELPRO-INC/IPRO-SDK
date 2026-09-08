# IPRO BLE Throughput Test

A BLE throughput testing application for measuring data transfer rates between two IPRO7 development boards. Supports 1M/2M/Coded PHY with configurable connection parameters, achieving up to **999 Kbps** peak throughput (2M PHY).

## Features

- Dual-role support: Slave (Peripheral) and Master (Central)
- PHY switching: 1M / 2M / Coded
- Data Length Extension (DLE), up to 251 bytes
- ATT MTU 247 bytes
- Configurable connection interval (7.5ms ~ 4s)
- Flood mode and timed interval mode
- Real-time throughput statistics (updated every second)
- Interactive Shell CLI

## Architecture

```
ipro_throughput_test/
├── main.c                  # Entry point: platform init, BLE controller startup, FreeRTOS scheduler
├── throughput_app.c/.h     # Application core: connection management, state machine, TX task, statistics
├── throughput_service.c/.h # GATT Service (Slave side): TX Notify + RX Write
├── throughput_client.c/.h  # GATT Client (Master side): service discovery, notification subscription
├── throughput_cli.c/.h     # Shell command registration and parsing
├── .config                 # Kconfig configuration
├── CMakeLists.txt          # Build configuration
├── OPERATION_MANUAL.md     # Detailed operation manual (Chinese)
└── THROUGHPUT_REPORT.md    # Test report and performance analysis
```

### Modules

| Module | Responsibility |
|--------|---------------|
| `throughput_app` | Application core: BLE initialization, GAP connection callbacks, PHY/DLE/MTU negotiation, TX send task, statistics calculation |
| `throughput_service` | Slave-side GATT Service with custom UUID, providing TX (Notify) and RX (Write Without Response) characteristics |
| `throughput_client` | Master-side GATT Client, performs service discovery, enables notification subscription, writes data |
| `throughput_cli` | Shell command interface, handles all `tp` subcommands |

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

- 2x IPRO7 EVB development boards
- RISC-V cross-compiler (`/opt/toolchain/riscv_ipro7/`)
- macOS or Linux host

### Build

```bash
make -C apps/bluetooth/ble/ipro_throughput_test
```

Output: `apps/bluetooth/ble/ipro_throughput_test/build/build_out/ipro_throughput_test_IPRO7.bin`

### Flash

Each board must enter download mode before flashing:

```bash
# Enter download mode (via serial command or manual BOOT+RESET)
python3 tools/serial_monitor.py /dev/cu.usbserial-XXXX --send reboot_dl --timeout 5
sleep 2

# Flash firmware
tools/ipro_iot_tool_lite/ipro_iot_tool_lite \
    --chipname ipro7 \
    --port /dev/cu.usbserial-XXXX \
    --firmware apps/bluetooth/ble/ipro_throughput_test/build/build_out/ipro_throughput_test_IPRO7.bin \
    --baudrate 921600 --flash-pin 0x02
```

### Run Test

Open two terminals, each connected to a board's serial port:

```bash
# Terminal 1 (Slave)
python3 tools/serial_monitor.py /dev/cu.usbserial-2120

# Terminal 2 (Master)
python3 tools/serial_monitor.py /dev/cu.usbserial-2130
```

Execute the test:

```
[Slave]  tp slave              # Start advertising
[Master] tp master             # Scan and connect
         (Wait for connection...)
[Either] tp phy 2              # Switch to 2M PHY
[Slave]  tp start 244 10       # Start test: 244B packets, 10 seconds
```

## CLI Commands

| Command | Description |
|---------|-------------|
| `tp slave` | Start as Peripheral, begin BLE advertising |
| `tp master [addr]` | Start as Central, scan and connect (optionally specify target address) |
| `tp start [size] [sec] [interval_ms]` | Start throughput test |
| `tp stop` | Stop test / disconnect |
| `tp stats` | Show statistics |
| `tp status` | Show connection status |
| `tp phy <1\|2\|3>` | Switch PHY (1=1M, 2=2M, 3=Coded) |
| `tp conn <min> <max>` | Update connection interval (units of 1.25ms) |
| `tp interval <ms>` | Change TX interval |
| `tp help` | Show command help |

### `tp start` Parameters

| Parameter | Default | Range | Description |
|-----------|---------|-------|-------------|
| size | 244 | 20-244 | Packet size (bytes) |
| sec | 10 | 0=infinite | Test duration (seconds) |
| interval_ms | 0 | 0=flood | TX interval (ms), 0 for maximum rate |

## Performance Benchmarks

| PHY | Interval | Payload | Throughput |
|-----|----------|---------|------------|
| 1M | 7.5 ms | 244 B | 507 Kbps |
| 2M | 7.5 ms | 244 B | **999 Kbps** |
| 2M | 15 ms | 244 B | 984 Kbps |
| 2M | 7.5 ms | 100 B | 610 Kbps |
| 2M | 7.5 ms | 20 B | 187 Kbps |

See [THROUGHPUT_REPORT.md](THROUGHPUT_REPORT.md) for detailed analysis.

## Configuration

Key `.config` entries:

```ini
# BLE Controller
CONFIG_BTBLECONTROLLER_LIB="link2"       # Controller variant (uses prebuilt library)

# BLE Host
CONFIG_BT_L2CAP_TX_BUF_COUNT=10         # L2CAP TX buffer count
CONFIG_BT_CONN_TX_MAX=10                # Connection-level TX limit

# PHY
CONFIG_CFG_LE_2M_PHY=y                  # 2M PHY support
CONFIG_CFG_LE_CODED_PHY=y               # Coded PHY support
CONFIG_CFG_DLE=y                         # Data Length Extension
```

## BLE Stack Optimizations

This application includes the following BLE stack-level throughput optimizations:

1. **max_ce_len configuration** (`hci_core.c`, `conn.c`) — Sets `max_ce_len = 2 * interval_max` in LE Create Connection and Connection Update commands, allowing the controller to fully utilize the connection event window.

2. **TX descriptor increase** (prebuilt `link2` library) — `BLE_NB_TX_DESC_PER_CON` increased from 2 to 4, eliminating descriptor starvation as a throughput bottleneck. Baked into the prebuilt controller library.

3. **Per-app TX buffer override** (`ble_common.cmake`) — Allows each application to independently configure `CONFIG_APP_BLE_TX_BUFF_DATA` without modifying shared headers.

### Rebuilding the Prebuilt Controller Library

If controller-level changes are needed, rebuild the prebuilt library:

```bash
components/wireless/bluetooth/build_all_prebuilts.sh link2
```

## Related Documentation

- [OPERATION_MANUAL.md](OPERATION_MANUAL.md) — Complete operation manual (hardware setup, flashing, test workflow, troubleshooting)
- [THROUGHPUT_REPORT.md](THROUGHPUT_REPORT.md) — Test report (benchmark results, bottleneck analysis, optimization details)

## API Reference

### throughput_app

```c
int  throughput_app_init(void);                              // Initialize (BLE enable + create TX task)
int  throughput_app_start_slave(void);                       // Start Slave mode
int  throughput_app_start_master(const char *target_addr);   // Start Master mode
int  throughput_app_start_test(const throughput_test_config_t *config);  // Start test
int  throughput_app_stop_test(void);                         // Stop test
int  throughput_app_stop(void);                              // Stop and disconnect
int  throughput_app_set_phy(uint8_t phy);                    // Switch PHY
int  throughput_app_update_conn_params(uint16_t min, uint16_t max,
                                       uint16_t latency, uint16_t timeout);
void throughput_app_print_stats(void);                       // Print statistics
```

### throughput_service (Slave side)

```c
int  throughput_service_init(const throughput_svc_config_t *config);  // Register GATT service
int  throughput_service_notify(struct bt_conn *conn,
                               const uint8_t *data, uint16_t length); // Send notification
bool throughput_service_is_notify_enabled(void);                      // Query CCCD state
```

### throughput_client (Master side)

```c
int  throughput_client_init(const throughput_client_config_t *config);  // Initialize client
int  throughput_client_discover(struct bt_conn *conn);                  // Service discovery
int  throughput_client_enable_notify(struct bt_conn *conn);            // Enable notifications
int  throughput_client_write(struct bt_conn *conn,
                             const uint8_t *data, uint16_t length);    // Write without response
```

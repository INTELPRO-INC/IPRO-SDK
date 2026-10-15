# IPRO7 Demo Application

A comprehensive demo application for the IPRO7 platform, showcasing peripheral drivers, system services, and hardware capabilities.

## Supported Features

### RTOS & System Services
- FreeRTOS with POSIX support
- Shell (RT-Thread FinSH based)
- Event loop system (ipro_event)
- Command scheduler

### Memory Configuration
- PSRAM support (16MB)
- Flash boot mode
- Heap management with multiple regions

### Bluetooth Low Energy (BLE)

Off by default (`CONFIG_BLUETOOTH_LE_ENABLE` is not set in `.config` or any
`defconfig_*`). The dedicated BLE applications under `apps/bluetooth/` are the
reference; this is a minimal peripheral / central demo on the same stack: the
Zephyr Bluetooth host (`ipro_bt_host`) over the IPRO BLE controller
(`ipro_ble_controller`).

| Demo | Description | Shell Command |
|------|-------------|---------------|
| BLE Peripheral | Advertise as `IPRO7_DEMO`, restart after disconnect | `ble_peripheral` |
| BLE Central | Scan 1 s, auto-connect to an `IPRO7_DEMO` peripheral | `ble_central` |
| Scan results | List the devices found by the last scan | `ble_list` |
| Connect | Connect to device `<n>` of `ble_list` | `ble_connect <n>` |
| Disconnect | Drop the current link | `ble_disconnect` |

**Enabling it**: in `.config`, replace `# CONFIG_BLUETOOTH_LE_ENABLE is not set`
with the block below (a controller library with both roles, the host, and
central + peripheral). The demo already fills most of the 1 MB flash, so the
BLE host only fits once something else is switched off - for example
`CONFIG_COMPONENTS_IPERF_ENABLE`, which builds at about 99% of flash.

```
CONFIG_BLUETOOTH_LE_ENABLE=y
CONFIG_BT_CTLR_IPRO=y
CONFIG_IPRO_BLE_CTLR_LINK1=y
CONFIG_IPRO_BLE_CTLR_LIB="link1"
CONFIG_IPRO_BLE_CTLR_FEATURE_BT52=y
CONFIG_IPRO_BLE_CTLR_FEATURE="bt52"
CONFIG_EM_SIZE=32
CONFIG_BT_HOST_IPRO_V44=y
CONFIG_BT_CONN=1
CONFIG_BT_CENTRAL=y
CONFIG_BT_PERIPHERAL=y
CONFIG_BT_OBSERVER=y
CONFIG_BT_BROADCASTER=y
CONFIG_PHY_UPDATE=y
CONFIG_BT_AUTO_PHY_PERIPHERAL_NONE=y
CONFIG_BT_DATA_LEN_UPDATE=y
CONFIG_BT_AUTO_DATA_LEN_UPDATE=y
CONFIG_BT_SMP=y
CONFIG_BT_GATT_CLIENT=y
```

**Implementation** (`demo/bluetooth/`):
- `ble_demo.c`: `ipro_ble_ctlr_controller_init()` → `hci_driver_ipro_ble_ctlr_init()`
  → `bt_enable()`, run from the `ble_fsm` task (it blocks until the host is up),
  and the shell commands above
- `ble_demo_app.c`: advertising, scanning and connections on the `bt_*` API,
  with `bt_conn_cb` callbacks driving a small state machine
  (INIT → READY → ADVERTISING / SCANNING → CONNECTED)

### Peripheral Demos
| Demo | Description | Shell Command |
|------|-------------|---------------|
| PWM | PWM output control | `demo_pwm` |
| SPI | SPI master/slave | `demo_spi` |
| I2S | Audio interface | `demo_i2s` |
| LCD | SPI LCD display | `demo_lcd` |
| Low Power | Sleep modes | `demo_lowpower` |
| ROM API | ROM function calls | `demo_romapi` |
| SPI Camera | Camera sensor | `demo_spicam` |

### Audio Demos
| Demo | Description | Shell Command |
|------|-------------|---------------|
| ADC | Audio ADC input | `demo_adc` |
| DAC | Audio DAC output | `demo_dac` |

### Security & Crypto
| Demo | Description | Shell Command |
|------|-------------|---------------|
| TRNG NIST | Random number test (SP800-22) | `demo_trng_nist` |

### DSP
| Demo | Description | Shell Command |
|------|-------------|---------------|
| DSP | DSP acceleration | `demo_dsp` |

### File System
| Demo | Description | Shell Command |
|------|-------------|---------------|
| FS | FatFS & LittleFS | `demo_fs` |

### USB (CherryUSB)
- USB Device CDC
- USB Device HID
- USB Device MSC
- USB Device Audio
- USB Device Video (UVC)

### Multimedia
- SPI sensor interface
- AI/NPU support

## Build Instructions

### Prerequisites
- RISC-V toolchain: `/opt/toolchain/riscv_ipro7/`
- CMake 3.15+
- Ninja build system

### Method 1: Makefile (Recommended)

```bash
# From SDK root
cd apps/platform/ipro7_demo

# Build
make

# Clean build
make clean && make

# Output: build/build_out/ipro7_demo_IPRO7.bin
```

### Method 2: CMake Direct

```bash
# From SDK root
cmake -S . -B build -G "Ninja" \
    -DIPRO_SDK_BASE=$(pwd) \
    -DCONFIG_BUILD_PORJECT=ipro7_demo \
    -DCMAKE_PREFIX_PATH=$(pwd)/cmake \
    -DCUSTOM_CONFIG_DIR=$(pwd)/apps/platform/ipro7_demo \
    -DCONFIG_APPS=1 \
    -DCROSS_COMPILE=/opt/toolchain/riscv_ipro7/bin/riscv64-unknown-elf-

cmake --build build

# Output: build/build_out/ipro7_demo_IPRO7.bin
```

## Configuration

### Modify Configuration
```bash
cd apps/platform/ipro7_demo

# Interactive menu (requires kconfiglib)
make menuconfig

# Show current config
make show-config

# Load default config
make defconfig
```

### Key Configuration Options
| Option | Description |
|--------|-------------|
| `CONFIG_USE_PSRAM` | Enable PSRAM (16MB) |
| `CONFIG_LINK_FLASH` | Boot from flash |
| `CONFIG_SHELL` | Enable shell interface |
| `CONFIG_CHERRYUSB` | Enable USB support |
| `CONFIG_AI_SUPPORT` | Enable AI/NPU |

## Output Files

After successful build:
```
build/build_out/
├── ipro7_demo_IPRO7.bin    # Binary for flashing
├── ipro7_demo_IPRO7.elf    # ELF with debug symbols
├── ipro7_demo_IPRO7.map    # Memory map
└── ipro7_demo_IPRO7.asm    # Disassembly
```

## Memory Usage (Typical)

| Region | Used | Total | Usage |
|--------|------|-------|-------|
| Flash | ~756KB | 1MB | ~72% |
| PSRAM | ~49KB | 8MB | <1% |
| OCRAM | ~39KB | 48KB | ~79% |

## Debug

### UART Console
- Baudrate: 115200
- Data bits: 8
- Stop bits: 1
- Parity: None

## Usage Examples

### BLE Peripheral Demo

With BLE enabled (see above), start advertising:
```bash
ipro /> ble_peripheral
I (xxx) BLE_APP: Advertising started as "IPRO7_DEMO"
```

From a second board running the demo, scan and connect:
```bash
ipro /> ble_central
I (xxx) BLE_APP: Scanning started for 1000 ms...
I (xxx) BLE_APP: [1] IPRO7_DEMO (xx:xx:xx:xx:xx:xx (random))
I (xxx) BLE_APP: Scan complete. Found 1 devices.
I (xxx) BLE_APP: Auto-connecting to IPRO7_DEMO...
```

**Expected Behavior**:
1. Device appears as "IPRO7_DEMO" in BLE scanner apps
2. Advertising with flags 0x06 (General Discoverable, BR/EDR not supported)
3. Advertising interval: 100-200ms
4. Device is connectable; advertising restarts after a disconnect

**Troubleshooting**:
- `BLE not initialized`: the BLE task failed to start the stack - look for
  `Controller init failed` / `HCI driver init failed` / `Bluetooth init failed`
  earlier in the boot log
- `region 'flash' overflowed` when building: switch another feature off (see
  "Enabling it" above)

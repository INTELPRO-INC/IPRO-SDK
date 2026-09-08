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

**BLE 5.4 Stack**: RivieraWaves BLE 5.4 with LE Audio support

| Demo | Description | Shell Command |
|------|-------------|---------------|
| BLE Peripheral | Simple peripheral advertising | `ble_peripheral` |
| BLE Status | Show BLE connection status | `ble_status` |

**Architecture**:
- Direct API calls (no ke_msg system)
- Callback-based event handling
- Simple state machine (INIT → READY → ADVERTISING → CONNECTED)

**Features**:
- Device name: `IPRO7_DEMO`
- Advertising interval: 100-200ms
- Connectable and discoverable
- Automatic advertising start after configuration

**Implementation** (`demo/bluetooth/ble_demo_app.c`):
- Uses `gapm_reset()` → `gapm_configure()` → `gapm_le_create_adv_legacy()` sequence
- Hardware error callback for stack monitoring
- Activity callbacks for advertising lifecycle
- Supports connection events (ready for future expansion)

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

Start BLE advertising:
```bash
ipro /> ble_peripheral

=== BLE Peripheral (Direct API) ===
Device: IPRO7_DEMO
Interval: 100-200 ms

I (xxx) BLE_APP: Resetting GAPM...
I (xxx) BLE_APP: GAPM reset complete, now configuring...
I (xxx) BLE_APP: Configuring GAP device...
I (xxx) BLE_APP: Device configured successfully
I (xxx) BLE_APP: Creating advertising activity...
I (xxx) BLE_APP: Activity created: actv_idx=0, tx_pwr=0
I (xxx) BLE_APP: Set advertising data (len=14)
I (xxx) BLE_APP: Advertising data set, now starting advertising...
I (xxx) BLE_APP: Advertising started
```

Check BLE status:
```bash
ipro /> ble_status

=== BLE Status ===
State: ADVERTISING
Activity: 0
Connection: 255
```

**Expected Behavior**:
1. Device appears as "IPRO7_DEMO" in BLE scanner apps
2. Advertising with flags 0x06 (General Discoverable, BR/EDR not supported)
3. Advertising interval: 100-200ms
4. Device is connectable and scannable

**Troubleshooting**:
- If `GAP configure failed: 0x0040` appears, ensure BLE controller is initialized
- Check that no other BLE operations are in progress
- Verify `gapm_reset()` completes successfully before configuration

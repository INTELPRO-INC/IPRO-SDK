# IPRO SDK

**Professional embedded development kit for IPRO7 RISC-V platforms with BLE 5.4 and LE Audio support.**

---

## Quick Start

### Prerequisites

```bash
# RISC-V toolchain
/opt/toolchain/riscv_ipro7/bin/riscv64-unknown-elf-gcc

# Build tools
cmake >= 3.19
ninja
python3 >= 3.8
```

### Build & Run

```bash
# List available projects
./build_freertos.sh -l

# Build a BLE application
./build_freertos.sh ipro_ble_remote build

# Build + flash + monitor serial output
./build_freertos.sh ipro_ble_remote build flash monitor

# Clean
./build_freertos.sh ipro_ble_remote clean
```

Build output: `apps/<category>/<project>/build/build_out/<project>_IPRO7.bin`

---

## Applications

### BLE Applications

| Application | Description | Documentation |
|-------------|-------------|---------------|
| `ipro_ble_remote` | GATT services (LED, Button, UART, OTA) + PIR image transfer | [README](apps/bluetooth/ble/ipro_ble_remote/README.md) |
| `ipro_throughput_test` | Bidirectional throughput testing, up to 999 Kbps | [README](apps/bluetooth/ble/ipro_throughput_test/README.md) |
| `ipro_ble_hid_keyboard` | HID over GATT (HOGP) keyboard | - |
| `ipro_ble_transparent` | BLE transparent data transfer | - |

### LE Audio Applications

| Application | Description | Documentation |
|-------------|-------------|---------------|
| `ipro_le_audio_headset` | GAF/BAP unicast/broadcast headset with LC3 codec | [README](apps/bluetooth/le_audio/ipro_le_audio_headset/README.md) |



### Platform Applications

| Application | Description | Documentation |
|-------------|-------------|---------------|
| `ipro7_demo` | Platform demo (GPIO, UART, SPI, USB, ISP, Audio, Ethernet) | [README](apps/platform/ipro7_demo/README.md) |



---

## Architecture

```
apps/              Application projects (each with .config + CMakeLists.txt)
  bluetooth/         BLE and LE Audio applications
  multimedia/        Audio/video applications (Agora, PIR camera)
  networking/        Thread, Matter networking apps
  platform/          Platform demos (ipro6, ipro7)
  test/              Unit tests and validation
components/        Middleware & libraries
  wireless/          BLE 5.4: ipro_bt_host (Zephyr 4.4 host) + ipro_ble_controller
  3rdparty/          Agora SDK, liblc3, mbedTLS (via crypto/)
  cherryusb/         USB device classes (CDC, MSC, HID, UAC, UVC)
  network/           lwIP networking
  os/                FreeRTOS kernel + shared app hooks
  audio/             Audio processing
  fs/                FatFS, SPIFFS
bsp/               Board Support Package (HAL, drivers, linker scripts)
cmake/             Build system (toolchain.cmake, extension.cmake)
tools/             Flash tool, serial monitor, release scripts
```

### BLE Stack

The BLE stack is a **Zephyr 4.4 host** (`components/wireless/bluetooth/ipro_bt_host`)
over the **IPRO BLE controller** (`components/wireless/bluetooth/ipro_ble_controller`),
joined by an in-memory HCI byte stream:

```
Application (Zephyr bt_* APIs)
    |
    v
ipro_bt_host -- Zephyr 4.4: GAP, GATT, L2CAP, SMP, ISO, LE Audio (source)
    |  HCI byte stream (ipro_ble_ctlr_hcitl)
    v
ipro_ble_controller -- prebuilt archive, selected per app (lib/)
    |
    v
RF Drivers (ipro7_rf)
```

Applications use the Zephyr `bt_*` callback-based API. The controller ships as
a matrix of prebuilt archives; an app selects one with three config keys:

| Key | Values | Meaning |
|-----|--------|---------|

`CONFIG_BT_CONN` must match the variant's connection count. The application
brings the controller up itself, then the HCI driver, then the host:

```c
ipro_ble_ctlr_controller_init(configMAX_PRIORITIES - 1);
vTaskDelay(pdMS_TO_TICKS(50));
hci_driver_ipro_ble_ctlr_init();
vTaskDelay(pdMS_TO_TICKS(50));
bt_enable(bt_ready_cb);          /* with CONFIG_BT_SETTINGS: settings_load_subtree("bt") in bt_ready_cb, before advertising */
```

`apps/bluetooth/ble/ipro_ble_remote` is the reference for GATT + OTA;
`apps/multimedia/ipro_pdm_mic_demo` for SMP bonding persisted to flash,
PDS31 sleep with bonded reconnect on wake, and 2M/DLE bulk transfer.

### Configuration System

Linux kernel-style Kconfig. Each app has `.config` with `CONFIG_*` variables:

```conf
CONFIG_IPRO7=y
CONFIG_BLUETOOTH_LE_ENABLE=y
CONFIG_BT_CTLR_IPRO=y
CONFIG_IPRO_BLE_CTLR_LIB="link1_periph"
CONFIG_IPRO_BLE_CTLR_FEATURE="bt52"
CONFIG_BT_HOST_IPRO_V44=y
CONFIG_FREERTOS_SUPPORT=y
```

The build generates `generated/autoconf.h` from `.config`.

### Hardware Specs (IPRO7)

| Feature | Details |
|---------|---------|
| Processor | RISC-V RV32IMAFC with custom DSP extensions |
| Flash | 1 MB @ 0x13000000 |
| PSRAM | 8 MB @ 0x1e000000 |
| OCRAM | 256 KB @ 0x11010000 |
| HBNRAM | 4 KB @ 0x20090000 |
| Wireless | BLE 5.4 with LE Audio |
| USB | CDC, MSC, HID, UAC, UVC via CherryUSB |

---

## Tools

| Tool | Path | Description |
|------|------|-------------|
| Build script | `build_freertos.sh` | Unified build/flash/monitor/test CLI |
| Flash tool | `tools/ipro_iot_tool_lite/` | Firmware download via serial |
| Serial monitor | `tools/serial_monitor.py` | Python3 termios-based monitor |
| Board test | `tools/board_test.sh` | Automated build + flash + test |
| Release tool | `tools/release/release.py` | SDK release packaging |

### Flashing & Monitoring

```bash
# Flash firmware
tools/ipro_iot_tool_lite/ipro_iot_tool_lite \
  --chipname ipro7 --port /dev/cu.usbserial-2120 \
  --firmware apps/bluetooth/ble/ipro_ble_remote/build/build_out/ipro_ble_remote_IPRO7.bin \
  --baudrate 921600 --flash-pin 0x02

# Monitor serial output (macOS: always use /dev/cu.*, not /dev/tty.*)
python3 tools/serial_monitor.py /dev/cu.usbserial-2120

# Or use build_freertos.sh shortcuts
./build_freertos.sh ipro_ble_remote flash monitor
```

---

## Documentation

| Topic | Location |
|-------|----------|
| FreeRTOS app development | [docs/development/FreeRTOS_App_Guide.md](docs/development/FreeRTOS_App_Guide.md) |
| BLE development overview | [docs/ble/README.md](docs/ble/README.md) |
| BLE Host architecture | [docs/ble/guides/BLE_HOST_ARCHITECTURE_GUIDE.md](docs/ble/guides/BLE_HOST_ARCHITECTURE_GUIDE.md) |
| Configuration system | [docs/Configuration_System.md](docs/Configuration_System.md) |
| Configuration quick reference | [docs/Configuration_Quick_Reference.md](docs/Configuration_Quick_Reference.md) |
| Hardware adaptation | [docs/IPRO7_HARDWARE_ADAPTATION.md](docs/IPRO7_HARDWARE_ADAPTATION.md) |
| FreeRTOS hooks migration plan | [docs/plans/freertos-hooks-migration.md](docs/plans/freertos-hooks-migration.md) |
| CI/CD pipeline | [docs/CI_CD_README_EN.md](docs/CI_CD_README_EN.md) |
| CI quick start | [docs/CI_QUICK_START.md](docs/CI_QUICK_START.md) |

---

## Creating a New Application

All FreeRTOS apps use a shared hooks component (`freertos_app_hooks`) that provides default heap setup, FreeRTOS hooks, and platform init. See [FreeRTOS App Development Guide](docs/development/FreeRTOS_App_Guide.md) for details.

1. Create directory and files:
   ```
   apps/<category>/my_app/
     CMakeLists.txt
     .config
     main.c
     Makefile          # copy from ipro_ble_remote as template
   ```

2. Configure `.config` (copy from a similar app as template)

3. Write a minimal `main.c`:
   ```c
   #include "freertos_app_hooks.h"
   #include <FreeRTOS.h>
   #include <task.h>

   static void app_task(void *arg) {
       // Your application code
   }

   int main(void) {
       ipro_platform_init();  // heap, boot2, log, shell — all handled
       xTaskCreate(app_task, "app", 1024, NULL, 5, NULL);
       vTaskStartScheduler();
       while (1) {}
   }
   ```

4. Build:
   ```bash
   make -C apps/<category>/my_app
   ```

---

## SDK Release

Package the BLE SDK for distribution (strips source, includes prebuilt libraries):

```bash
python3 tools/release/release.py \
  -c tools/release/config/release_config_ble_sdk.yaml \
  -o /tmp/ble_sdk_release -v
```

The release tool builds all apps, extracts library `.a` files, adds license headers, strips development-only config keys, and verifies all apps build successfully in the released SDK.

---

## License

Proprietary. See [LICENSE](LICENSE) for details.

**Last Updated**: March 7, 2026

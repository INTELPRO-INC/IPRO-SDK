# IPRO SDK

**Embedded development kit for the IntelPro IPRO7 and IPRO6S RISC-V SoCs:
FreeRTOS, Bluetooth 5.4 with LE Audio, BR/EDR, Wi-Fi 6, and on-device AI.**

---

## Supported SoCs

| | IPRO7 | IPRO6S |
|---|---|---|
| Core | RISC-V RV32IMAFC + DSP extensions | RISC-V RV32IMAFC + DSP extensions |
| Bluetooth | BLE 5.4, LE Audio (ISO) | BLE 5.4 + BR/EDR: A2DP, AVRCP, HFP-HF, SCO |
| Wi-Fi | external, over SDIO (an IPRO6S as the radio, or an ATBM6162 module) | on-chip Wi-Fi 6 |
| NPU / ISP | yes | - |
| Reference boards | IPRO7 EVB, IPRO7AI EVK, IPRO7AI Reef EVK | IPRO6S EVB |

Both SoCs build from the same tree with the same toolchain. They share the
RTOS, the Bluetooth host, the HAL and the utility components; they differ in
the SoC driver layer, the RF driver and the Bluetooth controller.

---

## Quick Start

### Prerequisites

```bash
# RISC-V toolchain - obtain it from IntelPro; it is not part of the SDK
/opt/toolchain/riscv_ipro7/bin/riscv64-unknown-elf-gcc     # GCC 14, rv32imafc / ilp32f

# Build tools
cmake >= 3.19          # tools/cmake/ carries a Linux x86-64 build
ninja or GNU make
python3 >= 3.8
```

`cmake/toolchain.cmake` looks for the toolchain at the path above; set
`CROSS_COMPILE` to point elsewhere.

### Build

```bash
# List all projects
./build_freertos.sh -l

# Build a project (the name is the last path segment)
./build_freertos.sh ipro_ble_remote build
make -C apps/bluetooth/ble/ipro_ble_remote        # equivalent

# Clean
./build_freertos.sh ipro_ble_remote clean
```

Build output: `apps/<category>/<project>/build/build_out/<project>_<SOC>.bin`,
with `<SOC>` = `IPRO7` or `IPRO6`. The linker prints flash and RAM usage at
the end of every build.

Some projects carry more than one board configuration and select it with
`BOARD=`:

```bash
make -C apps/multimedia/ipro_pdm_mic_demo BOARD=evk    # IPRO7AI EVK; uses config_evk, builds in build_config_evk/
```

Others carry alternate configurations as `defconfig_*` files and select one
with `DEFCONFIG=`; the build goes to `build_<name>/`:

```bash
make -C apps/multimedia/ipro7_uvc_cam DEFCONFIG=defconfig_reef_rps    # IPRO7AI Reef EVK; builds in build_reef_rps/
```

### Flash and monitor

Firmware is downloaded over the serial port with the command-line tool of
IntelProCube v0.6.3 (obtained separately from the SDK): `ipro_iot_tool.exe`,
`ipro_iot_tool_macos` or `ipro_iot_tool_linux`.

```bash
ipro_iot_tool_macos --chipname ipro7 --chip-variant IPRO7 --port /dev/cu.usbserial-XXXX \
    --baudrate 2000000 --firmware apps/bluetooth/ble/ipro_ble_remote/build/build_out/ipro_ble_remote_IPRO7.bin
```

The tool has no `--flash-pin` option: `--chip-variant` writes `flash_pin` in
`<IntelProCube dir>/chips/ipro7/eflash_loader/eflash_loader_cfg.ini` (`IPRO7`
→ `0x2`; `IPRO7AI` → `0x9`, for the IPRO7AI EVK). It exits 0 even when
flashing fails, so look for `[All Success]` in its output.

The console baud rate is set per application by
`CONFIG_PLATFORM_UART_BAUDRATE` in its `.config` (115200 when unset): most
applications use 115200, while `ipro7ai_evk_demo`, `ipro7ai_evk_launcher`,
`ipro7ai_evk_rps`, `ipro_meeting_poc`, `ipro7_uvc_cam` and `ipro_epd_el036`
use 921600. On macOS use `/dev/cu.*`, not `/dev/tty.*` (the latter blocks on
carrier detect).

---

## Applications

### Platform references

| Application | Description | SoC |
|-------------|-------------|-----|
| `ipro7_demo` | Full-chip reference: GPIO, UART, SPI, USB, ISP camera, audio, Ethernet, NPU person detection | IPRO7 |
| `ipro7ai_evk_demo` | IPRO7AI EVK board: ST7789 panel, camera, PDM mic, speaker, SD card, BLE, LE Audio; prebuilt images and an Android control app under `release/` | IPRO7 |
| `ipro7ai_reef_demo` | IPRO7AI Reef EVK board: a shell command per block - WS2812 LED, keys, DAC speaker and headphone, PDM / headset / AUX-IN capture, SD card (FatFS), OV5640 / OV2640 camera; `defconfig_wifi_ipro6s` adds Wi-Fi through the IPRO6S_FIN and an RTSP camera stream; console at 115200 baud | IPRO7 |
| `ipro7ai_evk_launcher` | IPRO7AI EVK board: camera, LCD, microphone and speaker in one image; pages are switched by hand gestures recognised on the NPU | IPRO7 |
| `ipro7_sdio_wifi` | SDIO host for an IPRO6S Wi-Fi radio (`components/network/ipronet_host`): transport, lwIP netif `iproeth0`, DHCP, iperf. The IPRO6S runs `ipro6_if_wifi`, which is not part of this package | IPRO7 |

### Bluetooth LE

| Application | Description | SoC |
|-------------|-------------|-----|
| `ipro_ble_remote` | GATT services (LED, button, UART, OTA) - the reference for GATT + OTA | IPRO7 |
| `ipro_ble_transparent` | Transparent data pipe over BLE with an AT-style interface | IPRO7 |
| `ipro_ble_hid_keyboard` | HID over GATT (HOGP) keyboard | IPRO7 |
| `ipro_throughput_test` | Bidirectional throughput test, 2M PHY + DLE | IPRO7 |
| `ipro_le_audio_headset` | LE Audio unicast headset: GAF / BAP, VCP, MCP, LC3 | IPRO7 |

### Audio and AI

| Application | Description | SoC |
|-------------|-------------|-----|
| `ipro_pdm_mic_demo` | PDM capture, DAC playback, BLE audio transfer with SMP bonding, PDS31 sleep with bonded reconnect, 2M bulk transfer; EVB and EVK boards | IPRO7 |
| `ipro_meeting_poc` | Speaker recognition on the NPU; the model is loaded at boot from the `mfg` flash partition | IPRO7 |
| `ipro_agora_sentino` | Sentino cloud AI voice on the IPRO7AI Reef EVK: BLE provisioning, Wi-Fi through the IPRO6S FIN card, Sentino MQTT login, Agora RTC voice. Needs a Sentino device triple and the FIN-card firmware, which do not ship with the SDK, and the AOSL core: `git submodule update --init components/3rdparty/agora/aosl` (github.com/AgoraIO-Community/aosl at `d825607`) - see the app's README | IPRO7 |
| `ipro_npu_yolo` | YOLOv8 person detection on the NPU | IPRO7 |
| `ipro7ai_evk_person_detect` | IPRO7AI EVK: camera preview on the LCD with YOLOv8 person / pet boxes from the NPU | IPRO7 |
| `ipro7ai_evk_rps` | IPRO7AI EVK: rock-paper-scissors against a 4-class int8 CNN on the NPU (model and training scripts included) | IPRO7 |
| `ipro_ai_bench` | NPU vs CPU inference timing | IPRO7 |
| `ipro_ai_link` | Board firmware for IPRO AI Studio "Run on board": models and inputs from the SD card (`sd:/`), raw outputs back to the host over USB | IPRO7 |
| `ipro_tflm_test` | TensorFlow Lite Micro with NMSIS-NN kernels | IPRO7 |
| `ipro_tflm_yolo` | YOLO on TensorFlow Lite Micro | IPRO7 |
| `ipro_nmsis_nn_test` | NMSIS-NN operator tests | IPRO7 |
| `ipro_tinymaix_demo` | TinyMaix inference | IPRO7 |

### Displays, sensors and peripherals

| Application | Description | SoC |
|-------------|-------------|-----|
| `ipro7_uvc_cam` | USB webcam: DVP sensor, ISP, hardware MJPEG, USB Video Class. `.config` is the IPRO7 EVB with SC101IOT; `defconfig_reef_ov5640` the IPRO7AI Reef EVK with OV5640; `defconfig_reef_rps` adds rock-paper-scissors on the NPU, drawn into the stream | IPRO7 |
| `ipro_gh7007_test` | GH7007 5" SPI LCD bring-up | IPRO7 |
| `ipro_gh7007_video` | Video playback on the GH7007 panel | IPRO7 |
| `ipro_epd_test` | E-paper display driver | IPRO7 |
| `ipro_epd_el036` | EL036 e-paper panel | IPRO7 |
| `kw307_radar_test` | KW307 24 GHz radar module over UART | IPRO7 |
| `mpu6500_test` | MPU6500 IMU over I2C | IPRO7 |
| `ipro_enc28j60_test` | ENC28J60 Ethernet over SPI | IPRO7 |

### Storage, SDIO and low power

| Application | Description | SoC |
|-------------|-------------|-----|
| `ipro_pio_sdh_test` | Software (PIO) SD host validation | IPRO7 |
| `ipro_sdio_probe` | SDIO bring-up probe | IPRO7 |
| `ipro_atbm_probe` | ATBM6162 SDIO Wi-Fi module probe | IPRO7 |
| `ipro_lp_test` | PDS entry/exit and wake sources | IPRO7 |

### Test

| Application | Description | SoC |
|-------------|-------------|-----|
| `ipro_unit_test` | Unit tests for the HAL and the shared components | IPRO7 |

---

## Zephyr RTOS

The SDK is also a Zephyr v4.4.0 module (`zephyr/module.yml`): the same HAL and
SoC drivers behind Zephyr's device driver API. Supported boards: `ipro7_evb`
(IPRO7 EVB) and `ipro7ai_evk` (IPRO7AI EVK). The `ipro6s_evb` board files are
included, but IPRO6S is not supported under Zephyr in this release.

```bash
tools/zephyr/bootstrap.sh          # west workspace in zephyr-workspace/
source zephyr-workspace/env.sh     # ZEPHYR_BASE, CROSS_COMPILE, west
cd zephyr-workspace
west build -b ipro7_evb zephyr/samples/hello_world        # a Zephyr sample
west build -b ipro7_evb ../zephyr/samples/periph_smoke -d build/smoke   # an SDK sample
west flash                         # IntelProCube CLI: $IPRO_FLASH_TOOL or on PATH
```

The SDK is the west manifest repository. The first bootstrap downloads
Zephyr v4.4.0 and the modules `west.yml` lists (mbedTLS, OpenThread, MCUboot,
LC3, zcbor, picolibc, SEGGER RTT) from GitHub, and applies the Zephyr patches
in `zephyr/patches.yml`. `west flash` puts the board in download mode through
the IPRO7 CMSIS-DAP probe; without one, use `west flash -- --boot shell` (the
running image's `reboot_dl`) or `--boot manual`. The console runs at
2,000,000 baud on UART0.

| Area | Under Zephyr |
|---|---|
| Drivers | GPIO, UART (incl. async), SPI, I2C, PWM, ADC, DAC, counter/RTC, watchdog, flash, DMA, entropy, hwinfo, I2S/PDM |
| Bluetooth LE | Zephyr host on the IPRO BLE controller (the prebuilt archives in `components/wireless/bluetooth/ipro_ble_controller/lib/`) |
| IEEE 802.15.4 | Zephyr IEEE 802.15.4 driver and OpenThread |
| Boot and update | MCUboot through sysbuild; image upload over BLE (MCUmgr SMP) |

SDK samples in `zephyr/samples/`: `periph_smoke`, `mcuboot_app`, `ble_ota`,
`bap_broadcast_source`, `bap_broadcast_sink`, `le_audio_headset`, `evk_demo`
(IPRO7AI EVK LCD, microphone, speaker, camera preview), and configuration for
two Zephyr samples:

```bash
west build -b ipro7_evb zephyr/samples/bluetooth/peripheral_hr -- \
    -DEXTRA_CONF_FILE=$PWD/../zephyr/samples/bt_peripheral_hr/ipro7.conf \
    -DEXTRA_DTC_OVERLAY_FILE=$PWD/../zephyr/samples/bt_peripheral_hr/ipro7.overlay
west build -b ipro7_evb zephyr/samples/net/openthread/shell -- \
    -DEXTRA_CONF_FILE=$PWD/../zephyr/samples/ieee802154_shell/ot.conf \
    -DEXTRA_DTC_OVERLAY_FILE=$PWD/../zephyr/samples/ieee802154_shell/app.overlay
```

Driver tests are in `zephyr/tests/`. The RF driver and the IEEE 802.15.4 MAC
link as prebuilt archives (`components/wireless/rf/ipro7_rf/lib/zephyr/`,
`components/wireless/ieee802154/lib/zephyr/`); the build picks the variant
that matches the image's enum size (`-fshort-enums` when the BLE controller is
linked).

Not available under Zephyr yet: low-power modes (no PDS/HBN through
`pm_state`, so the BLE controller does not sleep either), and drivers for the
display, camera, audio codec, USB, SD host, Ethernet and NPU. The IPRO7AI
EVK's LCD works through Zephyr's own ST7789V driver.

---

## Architecture

```
apps/               Application projects (each with .config + CMakeLists.txt + Makefile)
  ai/                 NPU applications
  bluetooth/          BLE and LE Audio applications
  multimedia/         Audio applications
  platform/           SoC references (ipro7, ipro6), bootloaders
  test/               Driver examples, probes, unit tests
  platform/common/    Shared OTA helpers
  turnkey/common/     Shared USB console
components/         Middleware and libraries
  wireless/bluetooth/ipro_bt_host          Bluetooth host (Zephyr 4.4)
  wireless/bluetooth/ipro_ble_controller   Bluetooth controllers (prebuilt archives)
  wireless/macsw                           Wi-Fi 6 MAC (prebuilt archive)
  wireless/wifi6                           Wi-Fi 6 upper layers: fhost, wpa_supplicant, lwIP adapter
  wireless/rf                              RF drivers (prebuilt archives)
  ai/                                      NPU inference engine (libai.a + headers)
  network/                                 lwIP 2.1.2, SDIO Wi-Fi host, network utilities
  os/                                      FreeRTOS V11 kernel + shared application hooks
  sys/                                     Shell, logging, USB console
  audio/                                   Audio output, resampling, speaker correction
  fs/                                      FatFS, EasyFlash
  crypto/                                  mbedTLS
  cherryusb/                               USB device classes (CDC, MSC, HID, UAC, UVC)
  3rdparty/                                LC3, TensorFlow Lite Micro, NMSIS-NN, TinyMaix, zmodem, ...
bsp/                Board Support Package
  board/              Board files and linker scripts (ipro7, ipro6)
  drivers/            SoC drivers (IPRO7_soc, ipro6_soc, soc_common)
  hal/                HAL; lp_framework/ipro7 is the low-power (PDS) framework
cmake/              Toolchain and build-system macros
tools/              Bundled CMake, Kconfig parser, Android companion app
```

### Prebuilt libraries

A few components are provided as prebuilt archives with their public
headers rather than as source: the Bluetooth controllers, the Wi-Fi 6 MAC,
the RF drivers, the low-power framework, the AI engine, the ISP algorithms,
the LC3 codec and the third-party Agora RTC SDK. The build picks them up
automatically; nothing has to be
configured to use them. Debug information is stripped from the archives;
symbol names are kept so map files and `nm` still work.

### Bluetooth stack

A **Zephyr 4.4 host** (`components/wireless/bluetooth/ipro_bt_host`) runs
over the **IntelPro Bluetooth controller**
(`components/wireless/bluetooth/ipro_ble_controller`), joined by an in-memory
HCI byte stream:

```
Application (Zephyr bt_* APIs)
    |
    v
ipro_bt_host -- Zephyr 4.4: GAP, GATT, L2CAP, SMP, ISO, LE Audio, BR/EDR profiles (source)
    |  HCI byte stream (ipro_ble_ctlr_hcitl)
    v
ipro_ble_controller -- prebuilt archive, one per configuration (lib/)
    |
    v
RF driver (ipro7_rf / ipro6_rf)
```

Applications use the Zephyr `bt_*` callback-based API. The controller is a
matrix of prebuilt archives; the archive is chosen by config keys, and the
build stops at configure time if no archive matches the configuration.

**IPRO7** - `libipro_ble_controller_ipro7_<link>[_<feature>][_release].a`

| Key | Values | Meaning |
|-----|--------|---------|
| `CONFIG_IPRO_BLE_CTLR_LIB` | `link1_periph`, `link1`, `link2`, `link4` | connection count and roles; `_periph` is peripheral + broadcaster only |
| `CONFIG_IPRO_BLE_CTLR_FEATURE` | `bt52`, `bt54_iso2` | feature profile; `bt54_iso2` adds ISO (LE Audio) and needs `CONFIG_EM_SIZE=32` |
| `CONFIG_IPRO_BLE_CTLR_CFG_DBG` | set / not set | controller debug profile; not set selects the `_release` archive |

`CONFIG_BT_CONN` must match the variant's connection count. The application
brings the controller up, then the HCI driver, then the host:

```c
ipro_ble_ctlr_controller_init(configMAX_PRIORITIES - 1);
vTaskDelay(pdMS_TO_TICKS(50));
hci_driver_ipro_ble_ctlr_init();
vTaskDelay(pdMS_TO_TICKS(50));
bt_enable(bt_ready_cb);   /* with CONFIG_BT_SETTINGS: settings_load_subtree("bt") in bt_ready_cb, before advertising */
```

`ipro_ble_remote` is the reference for GATT + OTA; `ipro_pdm_mic_demo` for
SMP bonding persisted to flash, PDS31 sleep with bonded reconnect, and 2M/DLE
bulk transfer.

**IPRO6S** - `libipro_ble_controller_ipro6s_link<N>[_br][_pawr][_release].a`

| Key | Values | Meaning |
|-----|--------|---------|
| `CONFIG_BTDM_ENABLE` | y | selects the IPRO6S dual-mode controller |
| `CONFIG_BTDM_MAX_ACL_CONNECTIONS` | 1, 2, 4 | `link<N>` - sizes the controller's connection environment |
| `CONFIG_BTDM_BT_ENABLE` | y | `_br` - BR/EDR compiled in (all provided archives are `_br`) |
| `CONFIG_BTDM_PAWR` | y | `_pawr` - periodic advertising with responses |
| `CONFIG_BTDM_DEBUG` | set / not set | controller debug profile; not set selects `_release` |

`ipro6_btdm_phyrf` is the reference for the BR/EDR path, including the SCO
audio bridge to I2S.

### Wi-Fi 6 (IPRO6S)

```conf
CONFIG_WIFI6=y
CONFIG_USE_MACSW=y
```

The MAC (`components/wireless/macsw`) is a prebuilt archive. Everything above
it - the fhost API, wpa_supplicant, the lwIP adapter under
`components/wireless/wifi6` - is source, so the network stack and supplicant
configuration can be adapted. `CONFIG_PHY_LPFW` (the beacon-RX low-power
firmware trim) requires the `libmacsw_lpfw.a` variant of the MAC archive.

`ipro6_demo` is the reference: station and soft-AP bring-up, DHCP, and the
shell commands that drive them.

### Configuration system

Linux kernel-style Kconfig. Each application has a `.config` of `CONFIG_*`
variables, from which the build generates `generated/autoconf.h`. The SoC is
one of those variables; IPRO6S is IPRO6 with the dual-mode controller enabled:

```conf
# IPRO7
CONFIG_IPRO7=y
CONFIG_CPU_ID="intelpro_ipro7"
CONFIG_BOARD="EVB"                 # or "IPRO7AI_EVK"
CONFIG_FREERTOS_SUPPORT=y

# IPRO6S
CONFIG_IPRO6=y
CONFIG_CPU_ID="intelpro_ipro7"     # same toolchain
CONFIG_BOARD="EVB"
CONFIG_BTDM_ENABLE=y
CONFIG_FREERTOS_SUPPORT=y
```

Start a new project from the `.config` of the application closest to it
rather than from scratch; Kconfig `select` relations are not resolved by a
per-project build, so dependent options such as `CONFIG_USE_PSRAM` must be
stated explicitly.

### Hardware specs

**IPRO7**

| Region | Size | Address | Notes |
|--------|------|---------|-------|
| Flash | 1 MB | `0x13000000` | 2 / 4 / 8 MB with an external SF3 flash (`CONFIG_IPRO7_FLASH_2M` etc.) |
| PSRAM | 8 MB | `0x1e000000` | required by AI/NPU and ISP applications (`CONFIG_USE_PSRAM`) |
| OCRAM | 256 KB | `0x11010000` | fully retained through PDS31 |
| HBNRAM | 4 KB | `0x20090000` | retained through hibernate |

**IPRO6S**

| Region | Size | Address | Notes |
|--------|------|---------|-------|
| Flash | 2 MB | `0x13000000` | |
| TCM RAM | 320 KB | `0x11000000` | code + data; 280 KB in the reduced layout |
| Wi-Fi RAM | 160 KB | `0x21050000` | the Bluetooth exchange memory (`CONFIG_EM_SIZE`) is carved from the top |
| HBNRAM | 4 KB | `0x20090000` | |

---

## Tools

| Tool | Path | Description |
|------|------|-------------|
| Build script | `build_freertos.sh` | Build / clean front end for every project |
| CMake | `tools/cmake/` | Bundled CMake (Linux x86-64), for hosts without one |
| Kconfig parser | `tools/script/parse_kconfig_dynamic.py` | Used by the build to turn `.config` into `autoconf.h` |
| Android companion app | `tools/android/IproMicDemo/` | Pairing, bonded reconnect, 2M bulk transfer and LC3 streaming against `ipro_pdm_mic_demo` (source) |
| Zephyr workspace and flashing | `tools/zephyr/` | `bootstrap.sh` (west workspace), `build.sh`, `flash.sh`, `monitor.sh`, `ci_build.sh` (the sample matrix) |
| Serial monitor | `tools/serial_monitor.py` | Console reader used by `tools/zephyr/monitor.sh` and `flash.sh` |

---

## Creating a New Application

All FreeRTOS applications share `components/os/freertos/freertos_app_hooks`,
which provides the heap, the FreeRTOS hooks, assert and platform init as
weak defaults.

1. Create the project directory:
   ```
   apps/<category>/my_app/
     CMakeLists.txt
     .config           # copy from the closest shipped application
     Makefile          # copy from ipro_ble_remote
     main.c
   ```

2. Write a minimal `main.c`:
   ```c
   #include "freertos_app_hooks.h"
   #include <FreeRTOS.h>
   #include <task.h>

   static void app_task(void *arg)
   {
       for (;;) {
           /* your code */
       }
   }

   int main(void)
   {
       ipro_platform_init();   /* heap, boot2, log, shell */
       xTaskCreate(app_task, "app", 1024, NULL, 5, NULL);
       vTaskStartScheduler();
       for (;;) {}
   }
   ```

3. Build:
   ```bash
   make -C apps/<category>/my_app
   ```

Do not add heap, hook or assert boilerplate to `main.c`. To override a hook
(for example `vApplicationIdleHook`), define a non-weak version in your own
source; the linker picks it automatically.

---

## Testing

`apps/test/ipro_unit_test` exercises the HAL and the shared components and
prints `ALL TESTS PASSED` or `SOME TESTS FAILED` on the console. Build and
run it after porting to a new board.

---

## License

This SDK is provided under the terms of your IntelPro license agreement.
Source files carry an IntelPro copyright header; third-party components keep
their own notices (lwIP, mbedTLS, Zephyr, wpa_supplicant, FreeRTOS,
TensorFlow Lite Micro and others are under their respective open-source
licenses). The Agora RTC SDK in `components/3rdparty/agora/sdk` is
redistributed with Agora's permission and is subject to the Agora Terms of
Service (https://www.agora.io/en/terms-of-service/); see
`components/3rdparty/agora/sdk/LICENSE`. The IPRO7 port of AOSL in
`components/3rdparty/agora/port` is under the AOSL license in that directory,
with the further permissions Agora granted in that LICENSE file: shipping the
port in this SDK, and using AOSL on devices sold to end users (Powered by AOSL).
The AOSL core is not part of the SDK: `components/3rdparty/agora/aosl` is a git
submodule of https://github.com/AgoraIO-Community/aosl.

**SDK version**: IPRO-SDK-V2.1.0 (2026-10-16)

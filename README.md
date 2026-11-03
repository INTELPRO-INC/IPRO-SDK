# IPRO SDK

**Embedded development kit for the INTELPRO IPRO7, IPRO7AI and IPRO6S RISC-V
SoCs: FreeRTOS and Zephyr, Bluetooth 5.4 with LE Audio, Wi-Fi 6, and
on-device AI.**

The reference board in this guide is the **IPRO7AI Reef EVK V4.0**. Every
step below was written for it; applications for other boards are listed in
[Other applications](#other-applications).

Full documentation, with step-by-step tutorials:
https://intelpro-inc.github.io/ipro-sdk-docs/en/ (English) and
https://intelpro-inc.github.io/ipro-sdk-docs/ (Traditional Chinese).

| I want to | Go to |
|---|---|
| bring up the Reef and run the out-of-box firmware | [Quick Start](#quick-start-on-the-ipro7ai-reef-evk-v40) |
| teach the Reef to recognise images, voices or sensor states without code | [IPRO AI Teach](#ipro-ai-teach) |
| write my own application | [Develop your own application](#develop-your-own-application) |
| test it, by hand or in a script | [Test your application](#test-your-application) |

---

## Supported SoCs

| | IPRO7 / IPRO7AI | IPRO6S |
|---|---|---|
| Core | RISC-V RV32IMAFC + DSP extensions | RISC-V RV32IMAFC + DSP extensions |
| Radio | BLE 5.4, LE Audio (ISO), IEEE 802.15.4 | Wi-Fi 6 |
| Wi-Fi | external radio, e.g. an IPRO6S over SDIO | on-chip |
| NPU / ISP | IPRO7AI | - |
| RTOS | FreeRTOS V11, Zephyr v4.4.0 | FreeRTOS V11 |
| On the Reef EVK | the main SoC | the FIN card in the microSD slot (Wi-Fi for the Reef) |

Both SoCs build from the same tree with the same toolchain. They share the
RTOS, the HAL and the utility components; they differ in the SoC driver
layer, the RF driver and the radio stack.

---

## Quick Start on the IPRO7AI Reef EVK V4.0

At the end of this section the Reef runs the out-of-box firmware
`ipro7ai_reef_demo` and answers on its console.

### 1. What you need

| Item | Notes |
|---|---|
| IPRO7AI Reef EVK V4.0 | silkscreen "IPRO7AI REEF EVK V4.0" |
| USB-C cable | powers the board; the only supply input |
| Console cable | a 3.3 V USB-to-UART adapter with an SH1.0 6-pin plug for U6, or the IPRO Debug Probe |
| Host | macOS or Linux with a bash shell |
| RISC-V toolchain | from INTELPRO, not part of the SDK (GCC 14, rv32imafc / ilp32f) |
| IntelProCube v0.6.3 | the flash tool, from INTELPRO, not part of the SDK: `ipro_iot_tool_macos` or `ipro_iot_tool_linux` |
| CMake >= 3.15, GNU make, Python >= 3.8 | Zephyr builds need CMake >= 3.20 |

### 2. Install the host tools

```bash
# macOS (Homebrew)
brew install cmake python3
xcode-select --install                 # GNU make

# Ubuntu / Debian
sudo apt install cmake make python3
```

On Linux x86-64 the build falls back to the CMake 3.15 in `tools/cmake/` when
no `cmake` is on `PATH`.

Unpack the toolchain to `/opt/toolchain/riscv_ipro7/` and check it:

```bash
/opt/toolchain/riscv_ipro7/bin/riscv64-unknown-elf-gcc --version
```

To keep it elsewhere, pass the prefix on every build:
`make -C <app> CROSS_COMPILE=<dir>/bin/riscv64-unknown-elf-`. The build takes
the prefix from `CROSS_COMPILE` (environment or command line), then from
`CONFIG_CROSS_COMPILE` in the application's `.config`, then from the default
path above.

Unpack IntelProCube to a directory of your choice (`<ICUBE>` below). Its
command-line tool is run from that directory, because it reads its chip files
from `chips/`.

### 3. Get the SDK

Download `IPRO-SDK-V2.2.0.tar.gz` from the release page, compare its checksum
with the one published there, and unpack it:

```bash
shasum -a 256 IPRO-SDK-V2.2.0.tar.gz      # Linux: sha256sum
tar xzf IPRO-SDK-V2.2.0.tar.gz
cd IPRO-SDK-V2.2.0
cat VERSION.txt
```

All commands below run from the SDK root.

### 4. Connect the board

1. Set SW7 to **CAM** and SW8 to **PWR**.
2. Connect the console cable to **U6**, the 6-pin SH1.0 debug connector:

   | U6 pin | Signal | Connect to |
   |---|---|---|
   | 1 | NC | - |
   | 2 | GND | adapter GND |
   | 3 | TX (GPIO20, Reef output) | adapter RX |
   | 4 | RX (GPIO21, Reef input) | adapter TX |
   | 5 | BOOT (1 kΩ to GPIO41) | optional: adapter DTR |
   | 6 | RESET (1 kΩ, active low) | optional: adapter RTS |

   The IPRO Debug Probe connects with a pin-reversed cable (probe pin n to U6
   pin 7 - n). A straight cable puts the probe's 3.3 V on RESET.
3. Plug in USB-C. U6 does not power the board.

The console is UART0: 115200 baud, 8N1, no flow control. Flashing uses the
same UART at 921600 baud. On macOS use `/dev/cu.*`, not `/dev/tty.*` (the
latter blocks on carrier detect).

### 5. Build

```bash
make -C apps/platform/ipro7ai_reef_demo
```

The image is
`apps/platform/ipro7ai_reef_demo/build/build_out/ipro7ai_reef_demo_IPRO7.bin`;
the linker prints flash and RAM usage at the end of the build. Remove the
build with `make -C apps/platform/ipro7ai_reef_demo clean`.

### 6. Flash

1. Enter download mode: hold **SW1 (BOOT)**, press and release **SW2
   (RESET)**, then release SW1. The console stays silent in download mode.
   When the board already runs SDK firmware, typing `reboot_dl` at its console
   does the same.
2. From `<ICUBE>`, flash the image (Linux: `./ipro_iot_tool_linux`):

   ```bash
   cd <ICUBE>
   ./ipro_iot_tool_macos --chipname ipro7 --chip-variant IPRO7AI \
       --port /dev/cu.usbserial-XXXX --baudrate 921600 \
       --pt chips/ipro7/partition/partition_cfg_2M.toml \
       --firmware <SDK>/apps/platform/ipro7ai_reef_demo/build/build_out/ipro7ai_reef_demo_IPRO7.bin
   ```

   `--chip-variant IPRO7AI` selects the flash pins of the IPRO7AI (`0x9`).
   The tool exits 0 even when flashing fails: look for `[All Success]` in its
   output.
3. Press SW2 (RESET) to start the new firmware.

### 7. Run

Open the console at 115200 baud with any terminal program, for example
`screen /dev/cu.usbserial-XXXX 115200`, or send one command with the
bundled monitor:

```bash
python3 tools/serial_monitor.py /dev/cu.usbserial-XXXX --baud 115200 --send "reef selftest" --timeout 10
```

What you should see:

- at power-on the RGB LED breathes blue once, then the `ipro />` prompt;
- SW3, SW4, SW5 and BOOT light the LED red, green, blue and white;
- `reef selftest` cycles the LED, plays a 1 kHz tone on the speaker, checks
  the SD card detect and probes the camera, in about 3 seconds.

`help` lists every console command. The demo's command groups:

| Commands | Block |
|---|---|
| `reef status`, `reef selftest` | board state, self-test |
| `led <r> <g> <b>`, `led rainbow`, `led off` | WS2812 RGB LED |
| `btn status`, `btn watch` | keys |
| `spk tone <hz> <ms>`, `hp` | speaker and headset |
| `mic rec pdm|headset <s>`, `mic play`, `mic loop`, `mic sdrec`, `mic usb` | microphone; `mic usb` turns the Reef into a USB microphone, `mic sdrec` records WAV files to the SD card |
| `sd mount`, `sd ls`, `sd test` | microSD card (FatFS at `sd:`) |
| `cam probe`, `cam start`, `cam status`, `cam snap`, `cam bar on|off` | OV5640 camera (SW7 on CAM) |
| `reboot`, `reboot_dl` | restart; restart into download mode |

### 8. One command: build, flash, monitor

`build_freertos.sh` chains the steps above. With firmware already running on
the board it enters download mode by itself (`reboot_dl`); the first time,
put the board in download mode by hand.

```bash
export IPRO_FLASH_TOOL=<ICUBE>/ipro_iot_tool_macos
./build_freertos.sh -p /dev/cu.usbserial-XXXX \
    -P <ICUBE>/chips/ipro7/partition/partition_cfg_2M.toml \
    build flash monitor ipro7ai_reef_demo
```

It selects `--chip-variant IPRO7AI` from the application's configuration and
opens the console at the application's own baud rate. Other options:

| Option | Meaning |
|---|---|
| `-l` | list every project |
| `-d defconfig_<name>` | build an alternate configuration (same as `DEFCONFIG=`) |
| `-P <table>` | partition table, a `.toml` path or a table name under the tool's `chips/ipro7/partition/` |
| `-p <port>` | serial port (or `IPRO_SERIAL_PORT`; the only USB serial port is used when there is one) |
| `-t <s>` | monitor time-out, 30 s by default |
| `clean`, `size`, `test` | remove the build; print section sizes; build + flash + wait for a test verdict |

It cannot pass a boot2 image or an `mfg` payload: flash those with the
IntelProCube command line, as the application's README shows.

---

## IPRO AI Teach

IPRO AI Teach teaches the Reef to recognise things without writing code: you
collect data with the board's own camera, microphone or a sensor signal,
train on your computer in IPRO AI Studio, deploy over the USB-C cable, and the
IPRO7AI's NPU runs the model. The three tools live in one Edge AI firmware,
`apps/ai/ipro_reef_teach`, which runs one tool at a time.

| | Image recognition | Voiceprint recognition | Sensor recognition |
|---|---|---|---|
| Recognises | the object in the box at the centre of the camera view | who is speaking, once a second | the state of a machine or environment from a sensor signal |
| Data source | OV5640 camera module (SW7 on CAM) | the board's PDM microphone | five simulated sensors in the firmware, the microphone, or a sensor you add |
| Firmware mode | `image` | `voice` | `sensor` |
| USB device (VID:PID) | IPRO7 UVC Cam (3896:0705): camera + serial | IPRO Reef Voice (3896:0720): serial | IPRO Reef Sensor (3896:0721): serial |
| IPRO AI Studio page | **Teach** | **Audio** | **Sensor** |

IPRO AI Studio (macOS and Windows) and the ready-to-flash firmware files come
in the separate IPRO Edge AI package; contact INTELPRO. This SDK carries the
firmware source and `tools/teach_web/`, a browser version of image Teach.

### The flow

1. **Install IPRO AI Studio** from the IPRO Edge AI package and allow it to
   use the camera.
2. **Flash the Edge AI firmware once**, with its own partition table and the
   `mfg` payload (start-up mode + voiceprint model):

   ```bash
   make -C apps/ai/ipro_reef_teach
   python3 apps/ai/ipro_reef_teach/mfg_payload.py \
       --voice apps/ai/ipro_reef_teach/model/model_b.spkb --mode image -o reef_teach_mfg.bin
   cd <ICUBE>
   ./ipro_iot_tool_macos --chipname ipro7 --chip-variant IPRO7AI --port /dev/cu.usbserial-XXXX \
       --baudrate 921600 --skip-ota --pt <SDK>/apps/ai/ipro_reef_teach/partition_reef_teach.toml \
       --firmware <SDK>/apps/ai/ipro_reef_teach/build/build_out/ipro_reef_teach_IPRO7.bin \
       --mfg <SDK>/reef_teach_mfg.bin
   ```

   Only `[All Success]` in the output means it worked. To update the
   firmware later and keep the mode and the models, leave out `--mfg`.
3. **Connect USB-C to the computer.** After a reset the console (115200)
   shows `RTCH:   mode: image …` and the computer finds **IPRO7 UVC Cam**.
   Studio's pages find the board by its USB device; when the board is in
   another mode, the page offers a **Switch to …** button. At the console,
   `mode image`, `mode voice` or `mode sensor` switches too (the board restarts
   in about 2 s; all three models stay on the board).
4. **Collect data.** Image: put the object in the box and hold **Hold to
   record**, once per class. Voiceprint: type a name under **TEACH A VOICE**,
   press **Record** and read aloud. Sensor: choose a data source and hold
   **Hold to record** for each state.
5. **Train.** Image and sensor: press **Train** (seconds to about 90 s on the
   computer's CPU). A voiceprint is ready as soon as it is enrolled.
6. **Deploy.** Press **Deploy**: Studio compiles the model, writes it to the
   board over USB-C and restarts the board.
7. **Check it on the board.** Image results are drawn on the camera stream;
   for voiceprints press **Start listening** and watch the timeline; sensor
   results update four times a second and the RGB LED shows the class.

Each tool has a step-by-step page, with troubleshooting, in the
documentation's IPRO AI Teach section:
https://intelpro-inc.github.io/ipro-sdk-docs/en/teach/

Without Studio, `python3 tools/teach_web/serve.py` serves image Teach at
http://localhost:8780 for Chrome or Edge (Web Serial); see
`tools/teach_web/README.md`.

---

## Develop your own application

A first application for the Reef that logs a line and blinks the RGB LED.
It reuses the LED driver of the out-of-box demo in place. The
documentation's tutorial "Create your first application" continues it with
the keys, a shell command of your own and a self-test.

1. Create the directory and four files:

   ```
   apps/custom/my_app/
     Makefile
     CMakeLists.txt
     .config
     main.c
   ```

2. `Makefile`:

   ```make
   IPRO_SDK_BASE ?= $(abspath ../../../)
   export IPRO_SDK_BASE

   all:
   	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR)

   clean:
   	@rm -rf build

   .PHONY: all clean
   ```

3. `CMakeLists.txt`:

   ```cmake
   # The Reef's LED driver from the out-of-box demo, used in place.
   set(REEF_DEMO_SRC ${CMAKE_SOURCE_DIR}/apps/platform/ipro7ai_reef_demo/src)
   __component_add_include_dirs(${REEF_DEMO_SRC})
   target_sources(app PRIVATE ${REEF_DEMO_SRC}/rgb_led.c)

   __set_main_file(main.c)
   setup_project(my_app)
   ```

4. `.config`: the SoC, the Reef board, PSRAM, FreeRTOS, the shell and logging.
   Kconfig `select` is not applied, so every option is stated:

   ```conf
   CONFIG_IPRO7=y
   CONFIG_CHIP="IPRO7"
   CONFIG_CPU_ID="intelpro_ipro7"
   CONFIG_CROSS_COMPILE="/opt/toolchain/riscv_ipro7/bin/riscv64-unknown-elf-"
   CONFIG_IPRO7AI_REEF=y
   CONFIG_BOARD="IPRO7AI_REEF"
   CONFIG_LINK_FLASH=y
   CONFIG_PARTITION=y
   CONFIG_ENABLE_FRAME_POINTER=y
   CONFIG_EXCEPTION_DUMP=y
   CONFIG_USE_PSRAM=y
   # CONFIG_AI_SUPPORT is not set
   CONFIG_PLATFORM_UART_BAUDRATE=115200
   CONFIG_FREERTOS_SUPPORT=y
   CONFIG_COMPONENTS_FREERTOS_ENABLE=y
   CONFIG_FREERTOS=y
   CONFIG_IPRO_OSAL=y
   CONFIG_FREERTOS_HZ=1000
   CONFIG_MAIN_TASK_STACK_SIZE=4096
   CONFIG_FREERTOS_TIMER_TASK_STACK_WORDS=1024
   CONFIG_SHELL=y
   CONFIG_SHELL_EXEC_THREAD=y
   CONFIG_COMPONENTS_SOFTCRC_ENABLE=y
   CONFIG_COMPONENTS_UTILITY_API_ENABLE=y
   CONFIG_COMPONENTS_NEW_LIBC_ENABLE=y
   CONFIG_IPRO_LOG_ENABLE=y
   CONFIG_LOG_BUFFER_SIZE=256
   CONFIG_LOG_COLORS=y
   CONFIG_LOG_DEFAULT_LEVEL_INFO=y
   CONFIG_LOG_DEFAULT_LEVEL=3
   CONFIG_LOG_OUTPUT_UART=y
   CONFIG_LOG_TIMESTAMP=y
   CONFIG_LOG_USE_MUTEX=y
   ```

5. `main.c`:

   ```c
   #include <FreeRTOS.h>
   #include <task.h>

   #include "freertos_app_hooks.h"
   #include "ipro_log.h"
   #include "rgb_led.h"

   #define LOG_TAG "MYAPP"

   static void app_task(void *arg)
   {
       (void)arg;
       rgb_led_init();
       for (;;) {
           rgb_led_set(0, 40, 0);              /* green */
           vTaskDelay(pdMS_TO_TICKS(500));
           rgb_led_set(0, 0, 0);               /* off */
           vTaskDelay(pdMS_TO_TICKS(500));
       }
   }

   int main(void)
   {
       ipro_platform_init();                   /* heap, boot2, log, shell */
       IPRO_LOGI(LOG_TAG, "Hello from my_app");
       xTaskCreate(app_task, "my_app", 1024, NULL, 5, NULL);
       vTaskStartScheduler();
       for (;;) {}
   }
   ```

6. Build. The end of the output shows the memory use and the image:

   ```bash
   make -C apps/custom/my_app
   ```

   ```
   Memory region         Used Size  Region Size  %age Used
              flash:       94856 B         1 MB      9.05%
   ...
   Generate .../apps/custom/my_app/build/build_out/my_app_IPRO7.bin
   ```

7. Flash `apps/custom/my_app/build/build_out/my_app_IPRO7.bin` as in
   [Quick Start step 6](#6-flash), press SW2 (RESET), and open the console:
   the log shows `Hello from my_app`, the LED blinks green once a second,
   and `help` lists the built-in shell commands.

`ipro_platform_init()` brings up the heap, boot2, logging and the shell; the
heap, the FreeRTOS hooks and assert are weak defaults in
`components/os/freertos/freertos_app_hooks.c`. Do not add that boilerplate to
`main.c`; to override a hook (for example `vApplicationIdleHook`), define a
non-weak version in your own source and the linker picks it.

---

## Test your application

| Method | Needs | Verdict |
|---|---|---|
| Console by hand | the board and a terminal | you read it |
| `tools/serial_monitor.py` | the board, macOS or Linux | exit code |
| `./build_freertos.sh test` | the board, IntelProCube, macOS or Linux | exit code 0 / 1 / 2 |
| `test_host/run.sh` | `cc` on your computer, no board | exit code |

**A self-test that prints a verdict.** Let your application check what it
can (LED, keys, free heap) and end with exactly one of these lines; the test
tools wait for them:

```c
printf("%s\n", failed ? "SOME TESTS FAILED" : "ALL TESTS PASSED");
```

Run it at boot, or from a shell command of your own, for example
`myapp selftest` registered with `SHELL_CMD_EXPORT_ALIAS` (guard it with
`#ifdef CONFIG_SHELL`).

**Script it.** `serial_monitor.py` sends a command and waits for a pattern:

```bash
python3 tools/serial_monitor.py /dev/cu.usbserial-XXXX --send "myapp selftest" \
    --wait-for "ALL TESTS PASSED|SOME TESTS FAILED" --timeout 15 --log selftest.log
echo "exit=$?"
```

Exit codes: 0 = the match contains `PASSED`, 1 = something else matched,
2 = nothing matched before the time-out, 3 = the port could not be opened or
written. It is not an interactive terminal; close any terminal program that
holds the port first.

**Build, flash and test in one command.** `build_freertos.sh test` builds,
flashes (sending `reboot_dl` first), sends `reboot` and waits for
`ALL TESTS PASSED|SOME TESTS FAILED`, so the application must print the
verdict by itself after a reset:

```bash
export IPRO_FLASH_TOOL=<ICUBE>/ipro_iot_tool_macos
./build_freertos.sh -p /dev/cu.usbserial-XXXX \
    -P <ICUBE>/chips/ipro7/partition/partition_cfg_2M.toml test my_app
echo "exit=$?"     # 0 pass, 1 some tests failed, 2 no verdict within -t seconds (30)
```

**Hardware smoke test.** The out-of-box firmware's `reef selftest` checks the
LED, the speaker, the SD card detect and the camera in about 3 seconds.

**Host-side unit tests.** Logic that does not touch hardware is fastest to
test on your computer. Several components ship a `test_host/run.sh` that
compiles with the host `cc` and exits non-zero on a failure, for example:

```bash
sh components/audio/test_host/run.sh
```

To test a module of your own the same way, follow `components/audio/test_host/`:
a `test_*.c` with `main()`, compiled together with the `.c` under test.
`apps/test/ipro_unit_test` is the SDK's on-target unit test; it is configured
for the IPRO7 EVB.

---

## Applications for the Reef EVK

Every application below builds for the Reef. Alternate configurations are
built with `DEFCONFIG=` into `build_<name>/` (the `defconfig_` prefix
dropped); the application's `.config` is left as it is.

| Application | Build | What it does | Console | Flash notes |
|---|---|---|---|---|
| `ipro7ai_reef_demo` | `make -C apps/platform/ipro7ai_reef_demo` | out-of-box firmware: a shell command per board block | 115200 | `--pt partition_cfg_2M.toml` |
| `ipro7ai_reef_demo` + Wi-Fi | `make -C apps/platform/ipro7ai_reef_demo DEFCONFIG=defconfig_wifi_ipro6s` | Wi-Fi through the IPRO6S FIN card, RTSP camera stream at `rtsp://<ip>:8554/av0_0` | 115200 | needs a FIN card running `ipro6_if_wifi` (`defconfig_ipro6_sdio`); JP3 on GPIO |
| `ipro_reef_teach` | `make -C apps/ai/ipro_reef_teach` | Edge AI firmware: `image`, `voice` and `sensor` Teach tools, driven from IPRO AI Studio | 115200 | own partition table and `mfg` payload, see below |
| `ipro_ble_remote` | `make -C apps/bluetooth/ble/ipro_ble_remote DEFCONFIG=defconfig_reef` | GATT services (LED, button, UART, image upload) and BLE OTA with A/B images | 115200 | first flash with boot2 and `partition_cfg_2M_remote_ab.toml`, see its README |
| `ipro_throughput_test` | `make -C apps/bluetooth/ble/ipro_throughput_test DEFCONFIG=defconfig_reef` | BLE throughput between two boards, 2M PHY + DLE | 115200 | - |
| `ipro_le_audio_headset` | `make -C apps/bluetooth/le_audio/ipro_le_audio_headset DEFCONFIG=defconfig_reef` | LE Audio unicast headset: BAP, CAP, CSIP, VCP, LC3 | 115200 | see its README for the current status |
| `ipro7_uvc_cam` | `make -C apps/multimedia/ipro7_uvc_cam DEFCONFIG=defconfig_reef_ov5640` | USB webcam: OV5640, ISP, hardware MJPEG, UVC; 1280x720 or 640x360 | 115200 | `--pt partition_cfg_2M.toml` |
| `ipro7_uvc_cam` | `make -C apps/multimedia/ipro7_uvc_cam DEFCONFIG=defconfig_reef_rps` | rock-paper-scissors on the NPU, drawn into the webcam stream | 115200 | model linked into the image |
| `ipro7_jpeg_decode` | `make -C apps/multimedia/ipro7_jpeg_decode DEFCONFIG=defconfig_reef` | JPEG to RGB565 / RGB888 in software (TJpgDec); IPRO7's MJPEG block encodes only | **921600** | - |
| `ipro_ai_link` | `make -C apps/test/ipro_ai_link DEFCONFIG=defconfig_reef` | board side of IPRO AI Studio "Run on board": models and inputs from `sd:/` | 115200 | - |
| `ipro_agora_sentino` | `make -C apps/multimedia/ipro_agora_sentino` | Sentino cloud voice: BLE provisioning, Wi-Fi through the FIN card, Agora RTC | 115200 | needs the AOSL core, a Sentino device triple and the FIN-card firmware, see below |

`ipro_reef_teach` is flashed and used as described in
[IPRO AI Teach](#ipro-ai-teach).

### Wi-Fi on the Reef: the IPRO6S FIN card

The FIN card carries an IPRO6S on a card edge that plugs into the Reef's
microSD slot and talks SDIO. Its firmware is `ipro6_if_wifi` built with
`defconfig_ipro6_sdio`:

```bash
make -C apps/platform/ipro6_if_wifi DEFCONFIG=defconfig_ipro6_sdio   # build_ipro6_sdio/build_out/ipro6_if_wifi_IPRO6.bin
```

On the Reef, `defconfig_wifi_ipro6s` of `ipro7ai_reef_demo` brings it up:
`ipronet_up <ssid> <psk>`, `ipro6s_scan`, `ipro6s_link_status`,
`rtsp_cam on`, and `ipro6s_ota <http url>` to update the card's firmware over
Wi-Fi.

### `ipro_agora_sentino`: the AOSL core

The AOSL core is not part of the SDK. Fetch it once, at the commit the Agora
RTC library was built against:

```bash
git clone https://github.com/AgoraIO-Community/aosl components/3rdparty/agora/aosl
git -C components/3rdparty/agora/aosl checkout d825607
```

The build stops with these steps while the directory is empty.

---

## Other applications

These build for other INTELPRO boards; their READMEs describe the board.
Contact INTELPRO for the hardware.

| Application | Board | Description |
|---|---|---|
| `ipro7_demo` | IPRO7 EVB | full-chip reference: GPIO, UART, SPI, USB, ISP camera, audio |
| `ipro7ai_evk_demo` | IPRO7AI EVK | ST7789 panel, camera, PDM mic, speaker, SD card; BLE and LE Audio in `config_bt` |
| `ipro7ai_evk_launcher` | IPRO7AI EVK | camera, LCD, microphone and speaker in one image, pages switched by hand gestures on the NPU |
| `ipro7ai_evk_person_detect` | IPRO7AI EVK | camera preview with YOLOv8 person / pet boxes from the NPU |
| `ipro7ai_evk_rps` | IPRO7AI EVK | rock-paper-scissors against an int8 CNN on the NPU (model and training scripts included) |
| `ipro7_sdio_wifi` | IPRO7 EVB | SDIO host for an IPRO6S Wi-Fi radio: lwIP netif `iproeth0`, DHCP, iperf |
| `ipro_ble_transparent` | IPRO7 EVB | transparent data pipe over BLE with an AT-style interface |
| `ipro_ble_hid_keyboard` | IPRO7 EVB | HID over GATT keyboard |
| `ipro_pdm_mic_demo` | IPRO7 EVB, IPRO7AI EVK (`BOARD=evk`) | PDM capture, DAC playback, BLE audio with SMP bonding, PDS31 sleep with bonded reconnect |
| `ipro_speaker_recog` | IPRO7 EVB | speaker recognition on the NPU, model from the `mfg` partition |
| `ipro_npu_yolo`, `ipro_ai_bench` | IPRO7 EVB | YOLOv8 on the NPU; NPU vs CPU timing |
| `ipro_tflm_test`, `ipro_tflm_yolo`, `ipro_nmsis_nn_test`, `ipro_tinymaix_demo` | IPRO7 EVB | TensorFlow Lite Micro, NMSIS-NN, TinyMaix |
| `ipro_gh7007_test`, `ipro_gh7007_video` | IPRO7 EVB | GH7007 5" SPI LCD bring-up and video playback |
| `ipro_epd_test`, `ipro_epd_el036` | IPRO7 EVB | e-paper displays |
| `kw307_radar_test` | IPRO7AI EVK | KW307 24 GHz radar module over UART |
| `mpu6500_test` | IPRO7 EVB | MPU6500 IMU over I2C |
| `ipro_enc28j60_test` | IPRO7 Arduino board | ENC28J60 Ethernet over SPI |
| `ipro_pio_sdh_test`, `ipro_sdio_probe`, `ipro_atbm_probe` | IPRO7 EVB | PIO SD host, SDIO bring-up, ATBM6162 SDIO Wi-Fi probe |
| `ipro_lp_test` | IPRO7 EVB | PDS entry/exit and wake sources |
| `ipro_unit_test` | IPRO7 EVB | unit tests for the HAL and the shared components |
| `ipro6_demo` | IPRO6S EVB | Wi-Fi 6 station / soft-AP; `defconfig_ipro6_lpwifi` adds DTIM power save, `defconfig_ipro6_lp` is the low-power demo |
| `ipro6_if_wifi` | IPRO6S EVB, FIN card | Wi-Fi 6 bridge to a host over SDIO (`defconfig_ipro6_sdio`) or USB (`defconfig_ipro6_usb`) |
| `ipro6_fmac` | IPRO6S EVB | FullMAC firmware for a Linux cfg80211 host (`defconfig_ipro6_fmac`) and its flash loader (`defconfig_ipro6_fmac_loader`); the Linux driver is not part of this package |

`ipro7_jpeg_decode/.config` and `ipro7_uvc_cam/.config` build the same
applications for the IPRO7AI EVK and the IPRO7 EVB.

---

## Zephyr RTOS

The SDK is also a Zephyr v4.4.0 module (`zephyr/module.yml`): the same HAL and
SoC drivers behind Zephyr's device driver API. The Zephyr boards in this
release are `ipro7_evb` and `ipro7ai_evk`; there is no Reef board definition
yet. The `ipro6s_evb` board files are included, but IPRO6S is not supported
under Zephyr in this release.

```bash
tools/zephyr/bootstrap.sh          # west workspace in zephyr-workspace/ (needs git and network access)
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
  multimedia/         Camera, audio and cloud-voice applications
  platform/           SoC and board references (ipro7, ipro7ai, ipro6)
  test/               Driver examples, probes, unit tests
  platform/common/    Shared OTA helpers
  turnkey/common/     Shared USB console
components/         Middleware and libraries
  wireless/bluetooth/ipro_bt_host          Bluetooth host (Zephyr 4.4)
  wireless/bluetooth/ipro_ble_controller   Bluetooth controller (prebuilt archives)
  wireless/macsw                           Wi-Fi 6 MAC (prebuilt archive)
  wireless/wifi6                           Wi-Fi 6 upper layers: fhost, wpa_supplicant, lwIP adapter
  wireless/rf                              RF drivers (prebuilt archives)
  ai/                                      NPU inference engine (libai.a + headers)
  network/                                 lwIP 2.1.2, SDIO Wi-Fi host (ipronet_host), RTSP server, network utilities
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
tools/              Serial monitor, OTA packager, browser Teach, Zephyr scripts, bundled CMake, Android companion app
```

### Prebuilt libraries

A few components are provided as prebuilt archives with their public
headers rather than as source: the Bluetooth controller, the Wi-Fi 6 MAC,
the RF drivers, the low-power framework, the AI engine, the ISP algorithms,
the LC3 codec and the third-party Agora RTC SDK. The build picks them up
automatically; nothing has to be configured to use them. Debug information is
stripped from the archives; symbol names are kept so map files and `nm` still
work.

### Bluetooth stack

A **Zephyr 4.4 host** (`components/wireless/bluetooth/ipro_bt_host`) runs
over the **INTELPRO Bluetooth controller**
(`components/wireless/bluetooth/ipro_ble_controller`), joined by an in-memory
HCI byte stream:

```
Application (Zephyr bt_* APIs)
    |
    v
ipro_bt_host -- Zephyr 4.4: GAP, GATT, L2CAP, SMP, ISO, LE Audio (source)
    |  HCI byte stream (ipro_ble_ctlr_hcitl)
    v
ipro_ble_controller -- prebuilt archive, one per configuration (lib/)
    |
    v
RF driver (ipro7_rf)
```

Applications use the Zephyr `bt_*` callback-based API. The controller is a
matrix of prebuilt archives,
`libipro_ble_controller_ipro7_<link>[_<feature>][_release].a`, chosen by config
keys; the build stops at configure time if no archive matches the
configuration.

| Key | Values | Meaning |
|-----|--------|---------|
| `CONFIG_IPRO_BLE_CTLR_LIB` | `link1_periph`, `link1`, `link2`, `link4` | connection count and roles; `_periph` is peripheral + broadcaster only |
| `CONFIG_IPRO_BLE_CTLR_FEATURE` | `bt52`, `bt54_iso2` | feature profile; `bt54_iso2` adds ISO (LE Audio) and needs `CONFIG_EM_SIZE=32` |
| `CONFIG_IPRO_BLE_CTLR_CFG_DBG` | set / not set | controller debug profile; not set selects the `_release` archive |

`CONFIG_BT_CONN` must not exceed the variant's connection count. The application
brings the controller up, then the HCI driver, then the host:

```c
ipro_ble_ctlr_controller_init(configMAX_PRIORITIES - 1);
vTaskDelay(pdMS_TO_TICKS(50));
hci_driver_ipro_ble_ctlr_init();
vTaskDelay(pdMS_TO_TICKS(50));
bt_enable(bt_ready_cb);   /* with CONFIG_BT_SETTINGS: settings_load_subtree("bt") in bt_ready_cb, before advertising */
```

`ipro_ble_remote` is the reference for GATT and, in `defconfig_reef`, BLE
OTA. The IPRO6S Bluetooth controller is not part of this package; the IPRO6S
applications here are Wi-Fi only.

### Wi-Fi (IPRO7)

IPRO7 has no on-chip Wi-Fi. The package carries four ways to add it:

| Radio | Interface | Where |
|---|---|---|
| IPRO6S | SDIO, lwIP netif `iproeth0` | `components/network/ipronet_host`; `ipro7ai_reef_demo/defconfig_wifi_ipro6s`, `ipro_agora_sentino`, `ipro7_sdio_wifi` |
| ATBM6162 | SDIO | `components/wireless/wifi_atbm6162`; `ipro_atbm_probe` |
| AIC8800 | SDIO (prebuilt vendor archive) | `components/wireless/wifi_aic8800`; `ipro_agora_sentino/config_evk` |
| ESP32 | UART AT commands | `components/network/esp32_wifi_at`; `ipro7_demo` (`CONFIG_ESP32_WIFI_AT`) |

### Wi-Fi 6 (IPRO6S)

```conf
CONFIG_WIFI6=y
CONFIG_USE_MACSW=y
```

The MAC (`components/wireless/macsw`), the host layer (`components/wireless/wifi6`:
fhost API, wpa_supplicant, lwIP adapter) and the RF driver
(`components/wireless/rf/ipro6_rf`) are prebuilt archives. They are compiled
per application configuration, so each verified `defconfig_*` of an IPRO6S
application has its own set under `lib/<app>-<config>/`, selected by the
`CONFIG_WIRELESS_LIB_VARIANT` line in that defconfig. Build these applications
with one of these defconfigs:

| Application | Defconfigs |
|---|---|
| `ipro6_demo` | `defconfig_ipro6_lpwifi`, `defconfig_ipro6_lp` |
| `ipro6_if_wifi` | `defconfig_ipro6_sdio`, `defconfig_ipro6_usb`, `defconfig_ipro6_lp` |
| `ipro6_fmac` | `defconfig_ipro6_fmac`, `defconfig_ipro6_fmac_loader` |

```bash
make -C apps/platform/ipro6_if_wifi DEFCONFIG=defconfig_ipro6_sdio    # builds in build_ipro6_sdio/
```

A plain `make` uses `.config`, which is a copy of the first defconfig. A new
configuration must keep the Wi-Fi options of one of these defconfigs and its
`CONFIG_WIRELESS_LIB_VARIANT`; the archives exist only for those. IPRO6S
images are flashed with `--chipname ipro6`; `build_freertos.sh flash` handles
IPRO7 only.

The DTIM low-power firmware runs from OCRAM while the main core sleeps. It
ships as an image, `bsp/hal/lp_framework/ipro6/prebuilt/ipro6_lpfw.bin`, which
the low-power configurations embed.

### Configuration system

Linux kernel-style Kconfig. Each application has a `.config` of `CONFIG_*`
variables, from which the build generates `generated/autoconf.h`
(`cmake/generate_autoconf.cmake`). The SoC and the board are among those
variables:

```conf
# IPRO7AI on the Reef EVK
CONFIG_IPRO7=y
CONFIG_CHIP="IPRO7"
CONFIG_CPU_ID="intelpro_ipro7"
CONFIG_IPRO7AI_REEF=y
CONFIG_BOARD="IPRO7AI_REEF"
CONFIG_USE_PSRAM=y
CONFIG_FREERTOS_SUPPORT=y

# IPRO6S
CONFIG_IPRO6=y
CONFIG_CHIP="IPRO6"
CONFIG_CPU_ID="intelpro_ipro7"     # same toolchain
CONFIG_FREERTOS_SUPPORT=y
```

The `Kconfig` files document the options; the build reads `.config` as it is
and does not apply Kconfig defaults. Start a new project from the `.config`
of the application closest to it rather than from scratch; Kconfig `select`
relations are not resolved by a per-project build, so dependent options such
as `CONFIG_USE_PSRAM` must be stated explicitly. On the Reef the build sets the
crystal trim `CONFIG_IPRO7_XTAL_CAPCODE` to 95 unless the `.config` sets it.

### Hardware specs

**IPRO7 / IPRO7AI**

| Region | Size | Address | Notes |
|--------|------|---------|-------|
| Flash | 1 MB | `0x13000000` | 2 / 4 / 8 MB with `CONFIG_IPRO7_FLASH_2M` etc. |
| PSRAM | 8 MB | `0x1e000000` | required by AI/NPU and ISP applications (`CONFIG_USE_PSRAM`) |
| OCRAM | 256 KB | `0x11010000` | the first 64 KB is retained through PDS31; the rest only while the MISC domain stays powered (`pm_pds_set_misc_keep()`, see `ipro_lp_test`) |
| HBNRAM | 4 KB | `0x20090000` | retained through hibernate |

**IPRO6S**

| Region | Size | Address | Notes |
|--------|------|---------|-------|
| Flash | 2 MB | `0x13000000` | |
| TCM RAM | 320 KB | `0x11000000` | code + data; 280 KB in the reduced layout |
| Wi-Fi RAM | 160 KB | `0x21050000` | |
| HBNRAM | 4 KB | `0x20090000` | |

---

## Tools

| Tool | Path | Description |
|------|------|-------------|
| Build script | `build_freertos.sh` | Build / clean / flash / monitor / size / test front end for every project |
| Serial monitor | `tools/serial_monitor.py` | Console reader (`--baud`, `--send`, `--wait-for`, `--log`), used by `build_freertos.sh` and `tools/zephyr/`; not an interactive terminal |
| OTA packager | `tools/create_ota.py` | OTA package from IntelProCube's whole image (`chips/ipro7/img_create_iot/whole_img.bin`) |
| Browser Teach | `tools/teach_web/` | Image Teach in Chrome or Edge for `ipro_reef_teach` (`serve.py`) |
| Teach payload | `apps/ai/ipro_reef_teach/mfg_payload.py` | Builds the `mfg` payload (mode + voice model) for `ipro_reef_teach` |
| Reef BLE OTA | `apps/bluetooth/ble/ipro_ble_remote/tools/` | `make_reef_ota.sh` (OTA image), `reef_ota.py` (push over BLE); `ipro_remote_demo.apk` Android demo |
| CMake | `tools/cmake/` | Bundled CMake 3.15 (Linux x86-64), for hosts without one |
| Kconfig dump | `tools/script/parse_kconfig_dynamic.py` | Prints a Kconfig tree as JSON (needs `kconfiglib`). Not used by the build |
| Android companion app | `tools/android/IproMicDemo/` | Pairing, bonded reconnect, 2M bulk transfer and LC3 streaming against `ipro_pdm_mic_demo` (source) |
| Zephyr workspace and flashing | `tools/zephyr/` | `bootstrap.sh` (west workspace), `build.sh`, `flash.sh`, `monitor.sh`, `ci_build.sh` (the sample matrix) |

---

## Troubleshooting

| Symptom | Cause and fix |
|---|---|
| `No cmake found` | install CMake >= 3.15 (Linux x86-64 also uses `tools/cmake/`) |
| `riscv64-unknown-elf-gcc: No such file or directory` | toolchain not at `/opt/toolchain/riscv_ipro7/`: pass `CROSS_COMPILE=<dir>/bin/riscv64-unknown-elf-` |
| Flash tool prints no `[All Success]` | the board is not in download mode (hold SW1, tap SW2, release SW1), or another program holds the serial port |
| `flash jedec id: 00000000` or `FLASH MATCH TYPE FAIL` | wrong flash pins: use `--chip-variant IPRO7AI` on the Reef |
| Console shows garbage | wrong baud rate: 115200 for most applications, 921600 for `ipro7_jpeg_decode` |
| macOS terminal hangs on open | use `/dev/cu.*`, not `/dev/tty.*` |
| Camera not found (`cam probe`) | SW7 must be on CAM; unplug USB-C before reseating the camera cable |

---

## License

This SDK is provided under the terms of your INTELPRO license agreement.
Source files carry an INTELPRO copyright header; third-party components keep
their own notices (lwIP, mbedTLS, Zephyr, wpa_supplicant, FreeRTOS,
TensorFlow Lite Micro and others are under their respective open-source
licenses). The Agora RTC SDK in `components/3rdparty/agora/sdk` is
redistributed with Agora's permission and is subject to the Agora Terms of
Service (https://www.agora.io/en/terms-of-service/); see
`components/3rdparty/agora/sdk/LICENSE`. The IPRO7 port of AOSL in
`components/3rdparty/agora/port` is under the AOSL license in that directory,
with the further permissions Agora granted in that LICENSE file: shipping the
port in this SDK, and using AOSL on devices sold to end users (Powered by AOSL).
The AOSL core is not part of the SDK; fetch it from
https://github.com/AgoraIO-Community/aosl as shown above.

**SDK version**: IPRO-SDK-V2.2.0 (2026-11-03)

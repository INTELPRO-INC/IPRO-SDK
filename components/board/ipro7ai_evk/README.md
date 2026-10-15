# components/board/ipro7ai_evk — IPRO7AI EVK board library

The M5Stack-style board layer for the IPRO7AI EVK sub-board
(`CONFIG_IPRO7AI_EVK`, schematic IPRO7AI_MODULE_SUB_0812A): one include, one
`evk_begin()`, and the drivers for everything soldered to the board.

```c
#include "evk.h"

evk_begin();                         /* camera parked, amp off, panel up   */
evk_gfx_splash("HELLO", "IPRO7AI EVK");
evk_cam_start();                     /* ISP ch0 -> preview frames          */
evk_mic_start();  evk_mic_read(frame, 100);
evk_spk_tone(1000, 500, 30);         /* 1 kHz, 0.5 s, 30 % of full scale   */
evk_sd_mount();   evk_sd_exists("sd:/models/gesture.inm");
```

| Block | Header | Hardware | Notes |
|---|---|---|---|
| Panel | `st7789p3.h`, `st7789p3_port.h` | HEM LCM-T2D4BP-084, 2.4" 240×320 IPS, ST7789P3, 4-line SPI0 at 16 MHz | Row-at-a-time blocking blits, no framebuffer, no DMA. `st7789p3_bus_lock()` around any burst you drive yourself. |
| Gfx | `evk_gfx.h`, `font5x7.h` | — | Row-compositing text (`evk_text_t`), outlined rectangles, YUYV→RGB565 rows, bars, splash. Everything works on one row buffer at a time. |
| Camera | `evk_cam.h`, `evk_cam_port.h` | SmartSens SC101IOT 1280×720 DVP | ISP channel 0: YUYV letterbox 320×180 (or centre-crop 320×240) into PSRAM ping-pong, frame callback at the preview rate. Channel 1: a sensor ROI scaled to the NPU input, one frame in flight (`evk_cam_set_npu_input()`, `evk_cam_npu_rearm()`). |
| Mic | `evk_mic.h`, `evk_audio_port.h` | Infineon IM69D129 PDM on GPIO18/19 → AUADC | 20 ms frames at a nominal 16 kHz (15 875 real) through a stream buffer; AON pads handed back to the GLB mux first (the "mic silent from cold boot" fix). |
| Speaker | `evk_spk.h`, `evk_pcm.h` | On-chip DAC GPIO23 → TPA2037D1, EN GPIO22 **active high** | `components/audio` DAC sink opened with the right polarity; Q12 volume; tone generator. `evk_pcm.h` is the integer-only PCM math (level, dBFS, gain, sine) — no float anywhere in the audio path. |
| micro-SD | `evk_sd.h` | SDH, GPIO27-34, 4-bit | FatFS at `sd:/`. With `CONFIG_AI_MODEL_FROM_FILE=y` the NPU runtime loads models from the card. |
| Shell | — | — | `lcd`, `cam`, `mic`, `spk`, `sd` bench commands (`CONFIG_EVK_SHELL`). |

## Configuration

Raw per-app `.config` files do not resolve Kconfig `depends on`, so an app
lists what it needs:

```
CONFIG_COMPONENTS_IPRO7AI_EVK_ENABLE=y     # the library: panel + gfx + pcm, always
CONFIG_EVK_CAMERA=y                        # + CONFIG_MM_SUPPORT=y CONFIG_COMPONENTS_SENSOR_ENABLE=y CONFIG_USE_PSRAM=y
CONFIG_EVK_AUDIO=y                         # + CONFIG_AUDIO_OUTPUT=y CONFIG_DMA_CHANNEL_ALLOCATOR=y
CONFIG_EVK_SDCARD=y                        # + CONFIG_EVFS=y CONFIG_EVFS_USE_FATFS=y CONFIG_EVFS_POSIX=y
                                           #   CONFIG_COMPONENTS_FATFS_ENABLE=y CONFIG_FATFS_CODE_PAGE_437=y CONFIG_FATFS_CODE_PAGE_VALUE=437
CONFIG_EVK_SHELL=y                         # only if the app does not register its own lcd / cam / mic / spk / sd
```

The CMake file fails loudly when a block is asked for without its
dependencies. `CONFIG_EVK_SHELL` force-links the archive whole (the shell
commands live only in their FSymTab section); two commands with the same
name do not link, which is why apps that keep their own `cam`/`lcd`
commands (`ipro7ai_evk_demo`, `ipro7ai_evk_rps`, `ipro7ai_evk_person_detect`)
leave it off.

## Who uses it

| App | Blocks |
|---|---|
| `apps/platform/ipro7ai_evk_launcher` | all of them |
| `apps/ai/ipro7ai_evk_rps` | panel, gfx, camera |
| `apps/ai/ipro7ai_evk_person_detect` | panel, gfx, camera |
| `apps/platform/ipro7ai_evk_demo` | panel only — its `cam.c` / `audio.c` / `sdcard.c` are bring-up tools with diagnostics the library deliberately does not carry |

The drivers were lifted from those apps after they were validated on the
board (panel and camera 2026-08-28, NPU feed 2026-09-18, mic/speaker
2026-09-16 in `ipro_voice_device`); the `evk_cam_*` API is the RPS app's
`cam_pipe` with the prefix changed.

## Rules the board imposes

- **J6**: VDDIO1 (pin 3) powers the camera's pad bank, and on V1 boards the
  only usable supply is 2.8 V: wire pin 3 to pin 7 (2V8_CAM; not adjacent, a
  jumper cap does not fit). The old PIN1/PIN3 (1.8 V) and PIN3/PIN5 (3.3 V)
  settings do nothing on V1 — those pins are not connected. VDDIO1 also
  powers the in-package Flash (minimum about 2.7 V), so 1.8 V is not an option,
  and with VDDIO1 unpowered at reset the boot ROM cannot read the Flash and
  enters download mode; the download procedure is in
  `bsp/board/ipro7/IPRO7AI_EVK.md`. V1 limitation: the board feeds the
  sensor's DOVDD/DVDD 1.8 V although the SC101IOT wants 2.7–2.9 V, so the
  2.8 V bus drives an under-powered sensor — works on the bench, out of spec.
  The LCD bank is 3.3 V. `evk_cam_start()` failing
  with no sensor on SCCB is J6 nine times in ten.
- **GPIO22 is the amplifier enable AND DAC channel B**, so the board is mono.
- **The polling SPI panel path spins the CPU** for the whole blit (~58 ms for
  320×180 at 16 MHz). Put the display task below anything with a deadline
  (NPU dispatch, mic drain); the launcher uses NPU −3 > audio −4 > UI −6.
- **AI + BLE do not fit together** (64 KB OCRAM with AI on, the BLE stack
  needs ~67 KB) — see `apps/platform/ipro7ai_evk_demo/README.md`.
- **Float beside the NPU task**: until 2026-10-09 the FreeRTOS port did not
  save the FPU across context switches, so tasks beside the NPU task had to
  be integer-only; `evk_pcm.h` exists for that. The port saves the FPU
  registers on task switches and interrupts since then
  (`apps/test/ipro_fpu_ctx_test`), and the integer helpers stay as they are.

## Host tests

```
sh components/board/ipro7ai_evk/test_host/run.sh
```

Runs the integer PCM math against double-precision references (20 k
checks) and the text-layout helpers against a panel stub. The drivers need
the silicon.

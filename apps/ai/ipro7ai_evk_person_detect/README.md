# ipro7ai_evk_person_detect — person detection on the IPRO7AI EVK LCD

Live camera preview on the EVK's 2.4" ST7789P3 panel with YOLOv8 person / pet
bounding boxes from the IPRO7 NPU drawn over it, plus a HUD with the person
count, NPU time and frame rates.

```
SC101IOT 1280x720 DVP
   └─ ISP ─┬─ ch0  YUYV 320x180 @10 fps ─► CPU: RGB565 + boxes + HUD ─► SPI ─► ST7789P3
           └─ ch1  ARGB 640x360, one frame in flight ─► NPU person_pet_detect ─► boxes
```

Everything starts at power-on. No shell interaction is needed; `det`, `cam`
and `lcd` are there for tuning.

## Hardware

IPRO7AI EVK sub-board (`CONFIG_IPRO7AI_EVK`, wiring in
`bsp/board/ipro7/IPRO7AI_EVK.md`) with the LCM-T2D4BP-084 panel and the
SC101IOT camera module on their FPCs.

**Wire J6 before power-on: pin 3 (VDDIO1) to pin 7 (2V8_CAM).** VDDIO_1 is
the bank GPIO0-9 + 36-41, i.e. every camera pin, and on V1 boards 2.8 V is its
only usable supply: J6 pins 1/2 (1V8_SOC) and 4/5 (3V3_SOC) are not
connected. Pins 3 and 7 are not adjacent, so use a jumper wire; a jumper cap
does not fit. VDDIO1 also powers the in-package Flash (minimum about 2.7 V),
so with VDDIO1 unpowered at reset the boot ROM cannot read the Flash and enters
download mode instead of booting, and 1.8 V is not an option. The camera is
wired below its spec on V1: the SC101IOT wants DOVDD 2.7–2.9 V, but the board
feeds it 1.8 V, so the 2.8 V bus drives an under-powered sensor. It works on
the bench. The LCD is on
another bank and stays at 3.3 V. BOOT is no longer on J6; the full J6 table
and the download procedure are in `bsp/board/ipro7/IPRO7AI_EVK.md`.

Console: platform UART, 115200 8N1.

## Flash layout — the model does not fit next to the firmware

The EVK module's flash is **1 MiB** (JEDEC EF4014) and `person_pet_detect`
is 700,088 bytes, so the model goes into its own `mfg` partition and is read
over XIP at boot. `partition_cfg_1M_person_lcd.toml`:

| Partition | Address | Size | Contents |
|---|---|---|---|
| FW | 0x003000 | 320 KB | this firmware (~190 KB) |
| mfg | 0x053000 | 688 KB | `person_pet_detect` at +0x1000 (whole_img_mfg.bin is 704,192 B, 320 B spare) |
| factory | 0x0FF000 | 4 KB | |

No PSM / OTA slot: the app has no settings store, and a second FW copy would
not fit anyway. The `.ld` is the SDK's generated 1 MB script; `flash.sh`
refuses a firmware over 320 KB before the flash tool does.

## Build and flash

```bash
make -C apps/ai/ipro7ai_evk_person_detect          # → build/build_out/ipro7ai_evk_person_detect_IPRO7.bin

cd apps/ai/ipro7ai_evk_person_detect
./flash.sh --full        # first time: partition table + firmware + model
./flash.sh               # afterwards: firmware only
./flash.sh --model       # swap the model only
```

`flash.sh` drives the IntelProCube v0.6.3 command-line tool: `IPRO_FLASH_TOOL`,
else `ipro_iot_tool_macos` / `ipro_iot_tool_linux` on `PATH` (port with
`PORT`, model with `MODEL`). **The EVK module
needs `flash_pin = 0x9`** in the tool's
`chips/ipro7/eflash_loader/eflash_loader_cfg.ini` (the shipped default is
`0x2`): with `0x2` the handshake succeeds but the loader reads
`flash jedec id: 00000000` and stops with `IPRO FLASH MATCH TYPE FAIL`. That
error means the pin setting, not a dead board. The board has to be in the
boot ROM's download mode — `reboot_dl` from the shell (the script sends it),
or by hand: take the wire off J6 pin 3, press RST (SW1), put the wire back on
pin 3 – pin 7, then run the flash tool. A board sitting in download mode is
silent on the console, which is easy to mistake for a dead one. The model it
flashes is `apps/platform/ipro7_demo/demo/ai/ai_model/person_pet_detect.inm` —
the same 6-output 640x360 YOLOv8 that `ipro7_demo`'s AI demo runs (`.inm` and
`.blai` are the same format).

Under the hood, `--full` is:

```bash
ipro_iot_tool_macos --chipname ipro7 --port /dev/cu.usbserial-2120 --baudrate 921600 \
    --skip-ota --pt partition_cfg_1M_person_lcd.toml \
    --firmware build/build_out/ipro7ai_evk_person_detect_IPRO7.bin \
    --mfg ../../platform/ipro7_demo/demo/ai/ai_model/person_pet_detect.inm
```

Build footprint (2026-09-18, clean build):

| Region | Used | Available | |
|---|---|---|---|
| flash | 189,608 B | 320 KB (FW partition) | 57.9 % |
| OCRAM | 43,152 B | 64 KB | 65.8 % (AI+MM layout, `ipro7_flash.cmake` Case 1) |
| PSRAM static | 33,896 B | 8 MB | heap use is at runtime, see below |
| HBNRAM | 2,460 B | 4 KB | |

## What you should see

1. Splash: `PERSON DETECT / IPRO7AI EVK LOADING MODEL` (~1 s while the NPU
   takes the weights).
2. Live preview, 320x180 letterboxed in the 320x240 landscape panel, 30
   black rows above and below.
3. Green boxes on people, yellow on pets. HUD top-left:
   `PERSON 2  PET 0` and `NPU 102MS  9.8FPS  LCD 10FPS`.

Console at boot, in order (board, 2026-09-18):

```
[HEAP] 8158KB @ 0x1e008468
jedec id   0xEF4014
DETECT: model: 700088 B at flash 0x054000 (partition 'mfg' 688 KB)
DETECT: loading into the NPU (about a second)...
DETECT: loaded in 1109 ms: input 640x353x3, 6 outputs, ext_input 1, 102 layers
DETECT: PSRAM heap left after the model: 4294 KB
CAM: sensor __rti_name_sc101iot @0x68, 1280x720, pclk 72000000 Hz, total 1600x750
CAM: preview 320x180 at (0,30), letterbox, 10 fps, two buffer(s); NPU feed 640x360 ARGB, one frame in flight
```

Note the model's own header says **640x353**, not 360: its first layer pads
353 rows, so the NPU reads the top 353 of the 360 rows the ISP writes. That
is how the SDK's camera demos have always fed it; `detect.c` accepts any
height up to 360.

## Measured on the board (2026-09-18)

`det status` after ten minutes of running, one person in front of the camera,
then stepping out and back in (HUD went `PERSON 1` → `0` → `1`):

```
model   : 700088 B from 'mfg', loaded in 1109 ms, 4294 KB PSRAM left
detect  : running, conf 25%, 5.9 fps, 3823 frames, 0 errors
last    : #147 1 box (1 person, 0 pet), NPU 103 ms, decode 16 ms
          PERSON  50%  [367,208]-[515,351]
preview : 10.4 fps drawn, 76 ms/frame, 6372 drawn, 0 dropped
isp     : ch0 6378 frames, ch1 3824 frames (3824 to NPU), errors fifo 0 vsync 0 hsync 0
```

| | |
|---|---|
| NPU inference | 103 ms (the `ipro_npu_yolo` benchmark says 102) |
| box decode + NMS | 16 ms |
| detection rate | **5.9 fps** — see below for where the other 50 ms go |
| preview | 10.4 fps at the 10 fps setting, 76 ms/frame, 0 dropped |
| PSRAM after model | 4.29 MB free (the earlier 1.4 MB estimate was far too pessimistic) |
| ISP errors | none over 6,378 + 3,824 frames |

**Why 5.9 and not 8.4 fps.** One inference is 103 + 16 = 119 ms of work, but
the one-frame-in-flight gate serialises the *capture* with the *inference*:
after the done callback re-opens channel 1 the ISP waits for the next frame
start (0–33 ms) and then spends 33 ms capturing before the callback fires —
≈ 50 ms per cycle in which the NPU idles. Two ways to get the missing frames
back, neither done yet:

- run channel 1 continuously into its own ping-pong and `memcpy` the newest
  completed frame into the NPU buffer at the start of each cycle (~0.9 MB,
  ~6 ms + cache maintenance; the copy is done long before the ISP comes back
  round to that buffer). Unambiguous, costs 1.8 MB of the 4.3 MB PSRAM left.
- capture-ahead with `pp_num = 2` and gate only after the *second* frame.
  Cheaper, but it depends on whether a dropped frame advances the ISP's
  ping-pong pointer, which is not documented — verify on the bench before
  trusting it.

## Shell

```
det status              model, detection rate, last boxes, preview rate, ISP counters
det start | stop        feed the NPU / stop (preview continues, HUD says DETECT OFF)
det conf <1..99>        confidence threshold in %, default 25
det pets <on|off>       draw pet boxes too (default on; persons always)
det hud <on|off>        HUD text band
det print <on|off>      print every detection list to the console
det uvswap <on|off>     swap U/V if skin tones come out blue

cam status | start | stop
cam fit <box|crop>      box = whole frame 320x180 (default), crop = centre 960x720 fills 320x240
cam fps <1..30>         preview rate, default 10 (see "Why 10 fps")
cam rot <0..3>          panel orientation, 1 and 3 landscape; restart the preview
cam rst <0|1>           sensor RESET line by hand

lcd clk <hz>            SPI clock, default 16 MHz; 32000000 halves the blit time
lcd bl <0|1>            backlight enable
lcd bgr <0|1>           MADCTL colour order
lcd id                  RDDID read-back over the shared SDA line
```

## Design notes

**Two ISP channels, independent rates.** Channel 0 is the preview and runs
continuously. Channel 1 is the NPU input: it starts gated (`dst_fps = 0`),
is opened once AE has settled (6 frames), and after every completed frame the
ISP callback gates it again before handing the buffer to the detector. The
detector's done callback re-opens it. So exactly one 640x360 frame is ever in
flight to the NPU and the ISP can never overwrite a buffer the NPU is
reading — the same one-frame-in-flight scheme `ipro7_uvc_cam` validated.
Detection runs at the NPU's own rate (~10 fps for this model); the preview
does not wait for it, the boxes drawn on a preview frame are from the newest
finished inference.

**Model input is the raw ISP frame.** `person_pet_detect` is a uint8
`ext_input` model: `NPU_Img_Ext_Addr_Cfg()` is pointed at the ARGB frame and
the NPU reads it directly — no CPU conversion, no channel reorder (the `.blai`
was compiled for the ISP's native byte order). The frame buffer is also the
`imgBuffer` the AI runtime programs into the NPU at load time, which is why
it is allocated before the model.

**Boxes are remapped through the geometry, not scaled by a constant.** Model
pixels are sensor/2; the preview is a crop window of the sensor scaled to the
image. `display.c: map_x/map_y` handle both `fit` modes.

**No framebuffer.** The display task converts one YUYV row to RGB565 into a
640-byte row buffer, overlays the box edges and the HUD glyphs that intersect
that row, and pushes it. A 320x180 frame is 115 KB; two ISP ping-pong buffers
already cost 230 KB of PSRAM and there is no reason to add a third copy.

**Why 10 fps.** The ST7789P3 driver (copied from `ipro7ai_evk_demo`) pushes
pixels through the polling SPI driver, so a 320x180 frame at the proven
16 MHz SPI clock is ~58 ms of CPU. At 10 fps that leaves ~40 % of the CPU
for the ~100 NPU layer dispatches per inference, the box decode and the
shell. The detect task therefore runs at a *higher* priority than the display
task: with the order reversed, every layer dispatch would queue behind a
blit and a 100 ms inference would stretch to most of a second. `lcd clk
32000000` then `cam fps 15` if the panel wiring takes the faster clock — the
datasheet gives no maximum, the 16 MHz figure is what was measured.

**PSRAM heap budget (measured).** Heap at boot 8,158 KB; after the ch1 frame
(0.92 MB) and the model, 4,294 KB are left, so the model is ~2.9 MB resident
(NPU activation patches + weights + layer tables). The two ch0 frames are
0.23 MB and the ISP's own allocations come on top. Allocation order: ch1
frame, model, then camera. `det status` prints the heap left after the
model. If the camera ever reports a preview-buffer allocation failure it
drops to one buffer with a warning.

**Priorities.** app 30 (idle after init) > detect 29 > display 26 > shell 20.

## Troubleshooting

| Symptom | Cause |
|---|---|
| HUD `NO MODEL / NO MFG PARTITION` | Board still has the default partition table. `./flash.sh --full`. |
| HUD `NO MODEL / NO MODEL IN MFG` | Table is right, partition empty or garbage. `./flash.sh --model`. |
| HUD `NO MODEL / WRONG MODEL` | A model that is not 640x360 ext_input was flashed (e.g. `person_detect.blai`, 4 outputs). Use `person_pet_detect`. |
| `NO CAMERA` splash, "no sensor answered on SCCB" | The FPC, or the J6 pin 3 – pin 7 wire came loose after boot (with VDDIO1 unpowered at reset the board would not have booted at all). `cam start` retries. |
| Flash: handshake OK, `flash jedec id: 00000000`, `FLASH MATCH TYPE FAIL` | `flash_pin` in the loader ini is `0x2`; the EVK needs `0x9`. |
| Console silent, `det status` gets nothing | Board is in download mode (boot ROM does not talk) — at reset VDDIO1 had no supply. Flash it, or check J6 pin 3 is wired to pin 7 and press RST. |
| `cam status` shows 0 frames and 0 errors | The MM clock was not un-gated before the ISP was touched. `cam_isp_up()` does it; if you reordered anything, put `GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM)` back first. |
| Picture but colours wrong | `det uvswap on` (chroma order), `lcd bgr 1` (panel order). |
| Boxes offset from people | You changed `cam fit` without `cam stop` / `cam start`. |
| `LCD` fps well under `cam fps` | SPI-bound; `lcd clk 32000000` or accept it. `det status` shows `dropped`. |
| Detection fps far below ~9 | Something is starving the detect task — check nothing was moved above priority 29. |

## Files

```
main.c                  boot sequence: panel → model → camera → display
src/detect.[ch]         mfg partition → XIP → NPU load; inference task; result snapshots
src/display.[ch]        render task: YUYV→RGB565, box overlay, HUD, splash
src/app_cli.c           det / cam / lcd shell commands
components/board/ipro7ai_evk/   ST7789P3 driver + panel wiring (st7789p3.*),
                        camera pipeline (evk_cam.[ch] - sensor probe, ISP ch0
                        + ch1, channel-1 gating, start/stop hygiene), camera
                        wiring and this board's three quirks (evk_cam_port.h),
                        5x7 font - shared with ipro7ai_evk_rps and the launcher
partition_cfg_1M_person_lcd.toml
flash.sh
```

The start/stop sequence in `evk_cam.c` (MM block reset with the sensor held
in reset, sensor stopped before the ISP, `imgproc_stop()` never called) is
the set of conditions found on this board in `ipro7ai_evk_demo`; its README
has the full story.

## Not done / next

- Detection at the NPU's full ~8 fps instead of 5.9 (see "Why 5.9" above).
- SPI DMA for the panel would give the CPU back and allow 30 fps preview.
- Per-box confidence labels (the font is there; each label is one more
  `hud_line_t` per row).
- Model swap to a person-only or smaller model is a `--model` flash away as
  long as it stays 640x360 ext_input with 6 heads; anything else needs the
  post-processor in `detect.c` changed too.

# ipro7ai_evk_launcher — camera + LCD + mic + speaker, one image

The IPRO7AI EVK presented the way an M5Stack presents itself: every
peripheral on the board alive in one firmware, a few pages to flip through,
and the flipping done by the NPU — hand gestures in front of the camera are
the buttons.

```
SC101IOT 1280x720 DVP
   └─ ISP ─┬─ ch0  YUYV 320x180 @10 fps ─┬──────────► preview (CAMERA / DETECT pages) ──► ST7789P3
           │                             └─ centre 128x128, CPU YUYV→ARGB ─► NPU gesture classifier (7 ms)
           │                                                                    PAPER    = next page
           │                                                                    ROCK     = the page's action
           │                                                                    SCISSORS = the page's alternate
           └─ ch1  1280x720 → 640x360 ARGB ───────────► NPU YOLOv8 person/pet (~100 ms) ─► DETECT page
IM69D129 PDM mic ─► AUADC ─► PSRAM clip (≤10 s) ─► DAC ─► TPA2037D1 speaker                (VOICE page)
```

| Page | Shows | ROCK | SCISSORS |
|---|---|---|---|
| 1 CAMERA | live preview, the gesture box, the live class | freeze / resume the preview | mirror on/off |
| 2 DETECT | live preview with person (green) / pet (yellow) boxes from the second model | pet boxes on/off | mirror on/off |
| 3 VOICE | scrolling microphone waveform, peak in dBFS | record 5 s then play it back through the speaker | replay the last clip |
| 4 ABOUT | what every block is doing: model sources, NPU/detector/camera/LCD rates, mic frames, speaker gain, SD card, heap | — | — |

Two models share the one NPU. The gesture classifier no longer needs an ISP
channel of its own: in the 320×180 letterbox preview (sensor ÷ 4) the
512×512 gesture box is exactly the 128×128 window the model wants, so the UI
task cuts it out of every preview frame and converts YUYV→ARGB on the CPU
(~1 ms). That leaves ISP channel 1 for the detector, which gets the whole
frame at 640×360 like the person-detect app. `npu_share.c` serialises the two
inference tasks and re-applies the per-model NPU state (external-input
address, net parameters) that the runtime otherwise writes only at load.

Hold a gesture in the box for half a second; the yellow line along the top
of the page fills as the hold progresses and the page reacts once. Take the
hand out before the next gesture — a hand that stays in the box does nothing
more, so waving does not page through everything.

Everything starts at power-on. Without a camera the voice and about pages
still work and `app next` / `app select` / `app alt` stand in for the hand.

## Build, flash, run

```bash
make -C apps/platform/ipro7ai_evk_launcher
cd apps/platform/ipro7ai_evk_launcher
./flash.sh --full      # partition table + firmware + gesture model into mfg   (first time)
./flash.sh             # firmware only
./flash.sh --model     # model only
```

Same 1 MiB layout as the person-detect and RPS apps (FW 320 KB at 0x3000,
mfg 688 KB at 0x53000), so the three swap with a firmware + model flash.
The gesture model is the RPS app's `apps/ai/ipro7ai_evk_rps/model/rps_v3.inm`
(299 KB, 11 layers all on the NPU, 92 % on people it never saw).

**Models on the SD card** (`sd:/models/`, FAT):

| File on the card | Source in the SDK | Role |
|---|---|---|
| `models/gesture.inm` | `apps/ai/ipro7ai_evk_rps/model/rps_v3.inm` (299 KB) | optional — same model as in mfg; the card wins when present |
| `models/person.inm` | `apps/platform/ipro7_demo/demo/ai/ai_model/person_pet_detect.inm` (700 KB) | **required for the DETECT page** — it does not fit next to the gesture model in the 688 KB mfg slot |

The runtime reads a whole `.inm` into PSRAM at boot (`CONFIG_AI_MODEL_FROM_FILE`),
so the card is only touched then; `app status` and the ABOUT page say where
each model came from. No card: gestures from mfg, DETECT page says
`COPY person.inm TO SD:/MODELS/`.

Console is 921600 8N1. J6: pin 3 (VDDIO1) wired to pin 7 (**2.8 V**) for the
camera; `flash_pin = 0x9` in the IntelProCube loader ini. Flashing lessons
(download mode entry, two hosts on one UART, silence is not death) are in the
RPS README and apply unchanged; the J6 table and the manual download
procedure are in `bsp/board/ipro7/IPRO7AI_EVK.md`.

## Footprint

Clean build, 2026-09-22: flash 271,416 B (of the 320 KB FW partition),
OCRAM 33,056 B of the 64 KB AI + MM layout, PSRAM 46 KB static plus, from
the heap, the gesture model (~0.5 MB), the detector (~3 MB activation +
921 KB input frame), the ISP frames (2 × 115 KB) and the voice clip
(320 KB). No Bluetooth: with AI on the OCRAM region is 64 KB and the BLE
stack alone needs ~67.

Half of the OCRAM figure went away when the timer-task stack key was fixed:
the EVK configs had been writing `CONFIG_FREERTOS_TIMER_TASK_STACK_DEPTH`,
which no Kconfig reads — the real key is
`CONFIG_FREERTOS_TIMER_TASK_STACK_WORDS`, and without it the timer task
took 16 KB. The RPS, person-detect, EVK demo and Reef demo configs carried
the same no-op and were fixed alongside.

## Task layout

| Task | Priority | Why |
|---|---|---|
| app | max−2 | boot sequence only, then sleeps |
| gesture, detect (NPU) | max−3 | the NPU is dispatched layer by layer from the CPU; it must never queue behind a blit. `npu_share` serialises the two |
| voice | max−4 | a 20 ms mic frame must be drained before the stream buffer (8 frames) fills |
| ui | max−6 | the polling SPI panel path spins the CPU for ~58 ms per 320×180 frame — everyone else preempts it |
| shell | 20 | |

Exactly one task uses floating point after boot: the detector's box decode
(`yolov8_process_6out` in `components/ai`). Until 2026-10-09 the FreeRTOS
port on IPRO7 did not save the FPU across a context switch (it does now,
`apps/test/ipro_fpu_ctx_test`): a single FPU user was safe and a second
would corrupt it silently — which is why the audio path is integer
(`evk_pcm.h`), the gesture softmax is integer (`softmax_q.c`), the mic gain
writes the AUADC register directly, and the detector's confidence knob is an
integer the detect task folds into its float config itself. The gesture
model's float output scale is folded into Q16 once, at load, on the app
task.

## Shell

`app` (this file) plus the board library's `lcd`, `cam`, `mic`, `spk`, `sd`:

```
app status               everything, one screen
app page <1..4>          jump to a page
app det <1..99>          detector confidence threshold (default 25)
app detrate <ms>         minimum ms between detections (default 200)
app detprint <on|off>    print every detection list
app trace <on|off>       periodic [TRACE] health + run-time stats (auto for the first minute)
app next | select | alt  what PAPER / ROCK / SCISSORS would do
app rec [seconds]        record and play back (voice page)
app play | stop
app gain <0..40>         playback make-up gain, dB (default 24)
app hold <ms> <ms> <pct> gesture hold, release, confidence floor (500 300 60)
app feed <raw|xor|copy>  how the frame reaches the NPU (bench knob)
app print <on|off>       print every classification
app mirror | uvswap | npu <on|off>

spk tone [hz] [ms] [amp%]   the speaker without the mic
mic level [seconds]         the mic without the speaker (peak / rms in dBFS)
sd ls [path]                what is on the card
```

## Files

```
main.c                 boot: evk_begin → model → voice → camera → ui
src/npu_share.[ch]     one NPU, two models: inference lock + per-model state re-apply
src/gesture.[ch]       gesture classifier: sd:/models/gesture.inm or mfg → XIP; CPU-fed from the preview
src/softmax_q.[ch]     integer softmax (host-tested against double precision)
src/detect.[ch]        YOLOv8 person/pet from sd:/models/person.inm on ISP channel 1
src/input.[ch]         gesture → event debounce: hold, release, confidence (host-tested)
src/voice.[ch]         mic → PSRAM clip → speaker worker; waveform ring
src/ui.[ch]            render task, page table, bars, hold-progress line
src/page_camera.c      preview + box, freeze, mirror
src/page_detect.c      preview + person / pet boxes
src/page_voice.c       waveform, record / play back
src/page_about.c       live status lines
src/app_cli.c          the `app` shell command
partition_cfg_1M_launcher.toml, flash.sh, enter_dl.py
test_host/             host tests: sh test_host/run.sh
```

The board itself — panel driver, camera → ISP → NPU feed, mic, speaker, SD —
is `components/board/ipro7ai_evk` (see its README); this app is what sits
on top of `evk_begin()`.

## Bench log

- 2026-09-22, first image (gesture model on ISP ch1, three pages): boots in
  0.9 s; gesture 14.9 fps / 6 ms, preview 9.8 fps / 96 ms per frame, ISP 0
  errors, mic 15 875 Hz with 0 dropped frames while listening; `app rec 3`
  recorded and played back 3 s, 0 clipped; a PAPER in front of the camera
  flipped to the VOICE page. Mic frames dropped during playback (nobody read
  them) → the mic is now stopped for the duration of a clip.
- 2026-09-22, two-model image, first run with `person.inm` on the card:
  the detector loads and detects (1–3 objects), the models do share the
  NPU — but the gesture output froze at one value (`raw 84 5 85 84`) and
  the shell starved (the YOLOv8 decoder's three console lines per
  detection plus back-to-back ~100 ms detections). Follow-up image: the
  decoder's prints are compiled out, detections are paced (200 ms minimum,
  `app detrate`), and the boot task prints a `[TRACE]` health line with
  per-task CPU and a fingerprint of the gesture input every 5 s (`app
  trace`), so "constant input" and "clobbered NPU state" can be told
  apart. Pending a run.

## Not done / next

- Touch (the panel's FPC has it, the board does not wire it) would make the
  gestures optional.
- The person model could also come from mfg when there is no gesture model
  there (a `--model` flash of person.inm); today the mfg slot is gesture-only.

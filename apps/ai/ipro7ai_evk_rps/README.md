# ipro7ai_evk_rps — rock-paper-scissors against the NPU

Hold your hand in the box on the EVK's LCD, the board counts 3-2-1, you both
throw, it scores. A 4-class int8 CNN on the IPRO7 NPU (rock / paper /
scissors / none) classifies the box 15 times a second at 7 ms an inference.

```
SC101IOT 1280x720 DVP
   └─ ISP ─┬─ ch0  YUYV 320x180 @10 fps ─► preview + play box + scores ─► ST7789P3
           └─ ch1  centre 512x512 ROI -> 128x128 ARGB, one frame in flight ─► NPU ─► game
```

Built on `apps/ai/ipro7ai_evk_person_detect` (same board, same camera and
panel code, same 1 MiB flash layout). What is new: the channel-1 ROI, the
classifier, the game state machine and the game screen.

## Hardware, flash layout, flashing

Identical to the person-detect app: IPRO7AI EVK, J6 pin 3 (VDDIO1) wired to
pin 7 (**2.8 V**) for the camera, `flash_pin = 0x9` in the IntelProCube loader ini.

**Model linked into the firmware (default, `CONFIG_RPS_EMBED_MODEL=y`).**
`src/model_blob.S` pulls `model/rps_v3.inm` into `.rodata` with `.incbin`; the
NPU reads it over XIP exactly as it read the mfg copy. The image is ~485 KB
(~186 KB without the model), so `flash.sh` switches to
`partition_cfg_1M_rps_embed.toml` (FW 1008 KB, no mfg). One file to flash; a
retrained model means a rebuild.

```bash
make -C apps/ai/ipro7ai_evk_rps
cd apps/ai/ipro7ai_evk_rps
./flash.sh             # table + firmware (model included); --full is the same
```

**Model in the mfg partition (comment out `CONFIG_RPS_EMBED_MODEL`).** Table
FW 320 KB / mfg 688 KB (`partition_cfg_1M_rps.toml` is the person-detect
app's table under another name, so the two demos swap with firmware + model
only), and the model can be swapped without rebuilding:

```bash
./flash.sh --full      # table + firmware + model/rps_v3.inm   (first time)
./flash.sh             # firmware only
./flash.sh --model     # model only
```

**Console is 921600 8N1**, not 115200: `rps capture` streams 64 KB training
frames over it. `flash.sh` sends `reboot_dl` at `BAUD` (default 921600; pass
`BAUD=115200` when the board still runs the person app) and then the flash
tool. Board flashing lessons from this app's bring-up, all reproducible:

- `reboot_dl` sent blind (`serial_monitor.py --send`) right after opening the
  port at 921600 often never reached the shell; the board kept running and
  the tool read the shell's ANSI-coloured echo of its 0x55 sync bytes as a
  random-looking ack → `SHAKEHAND FAIL`. `enter_dl.py` (used by `flash.sh`)
  waits for the port to settle, sends the command and only returns 0 once
  the shell echoed `reboot`. The tell in a tool log is `ack is <random hex>`;
  a board really in download mode answers `4f 4b` (`OK`, sometimes with a
  leading `f8`) or nothing.
- Two hosts on one UART (a CP2104 dongle *and* the CMSIS-DAP probe both wired
  to the board's RX) → the boot ROM NAKs every corrupted byte and the tool
  sees an endless `f8 f8 f8…`. Wire one of them.
- Through the CMSIS-DAP probe's UART bridge the boot ROM handshake did not
  succeed on this bench at any baud (the DL-mode beacon floods the probe's
  RX; the CP2104 has a pull-up and sees only sparse glitches). Console works
  over the probe; flashing goes through the CP2104.
- A board in download mode is silent. Silence is not a dead board.

## What you see

Landscape 320x240. Top bar: `YOU n  ME n  DRAW n` and, during a round's
result, `ME: PAPER` (otherwise `NPU 7MS`). Middle: mirrored live preview with
the play box — white idle, yellow while counting/capturing, green / red /
cyan for win / loss / draw — the live class label in its corner (`ROCK 96%`),
the 3-2-1 digits and `GO` in its centre, then your captured move. Bottom bar:
`SHOW YOUR HAND` → `HOLD IT...` → `READY...` → `GO!` → `YOU WIN!` / `I WIN!`
/ `DRAW` → `REMOVE HAND FOR NEXT ROUND`.

The preview is mirrored like a selfie camera (`rps mirror off` for the raw
view); the NPU input is not mirrored (the model was trained with flips).

## Shell

```
rps status              model, NPU rate, last logits, game state, preview rate
rps reset               scores to zero
rps go                  start a round now
rps conf <10..99>       confidence needed to count a hand (default 60)
rps feed <raw|xor|copy> how the frame reaches the NPU - see "Feed"
rps print <on|off>      one line per classification
rps mirror <on|off>     selfie mirror (default on)
rps uvswap <on|off>     chroma order if skin looks blue
rps npu <on|off>        stop / resume feeding the NPU (preview stays)
rps capture <label>     dump the next NPU frame as base64 (see train/)
cam start|stop|fps|rot|rst, lcd clk|bl|bgr
```

`[GAME] round n: you ROCK, me PAPER -> I WIN (you 3 me 4 draw 2)` is printed
once per round.

## Model

`model/rps_v3.inm` (default; `rps_v1.inm` kept for comparison) — 299,160 B,
11 layers, all on the NPU, 7 ms.

| | |
|---|---|
| input | 128x128x3 int8, scale 1/255, zero point −128 (the person_pet / big_a convention: the NPU reads the ISP's ARGB bytes directly through its external-input port) |
| output | 1x1x4 int8 logits (v3: scale 0.2048 zp 13; the runtime presents them uint8 with the zero point shifted by 128, so the board reports zp 141). `rps_model.c` reads scale/zp from the model header and softmaxes on the CPU — a retrained model needs no firmware edit |
| architecture | 3x3 convs (16,16 / 32,32 / 64,64 / 96,96 with stride-2 between groups), 2x2 max-pool, 3x3 stride-2 VALID conv 4x4→1x1, 1x1 conv to 4 logits. BatchNorm folded. 295 K params |
| why no dense / no global-pool | the CIFAR model card in `ipro_ai_studio/examples` measured one FC layer eating 62 % of an inference on the CPU; keeping the head as convolutions keeps it on the NPU |
| training data v1 | TensorFlow `rock_paper_scissors` CGI hands cut out and composited on flower photos / procedural backgrounds; 15,200 train / 1,900 val; int8 val acc 88.9 % (synthetic) |
| training data v3 | HaGRID 30k sample (`cj-mills/hagrid-sample-30k-384p` on Hugging Face, CC BY-SA 4.0; real people, webcams, indoor) + 5,000 of the v1 composites: fist → rock; palm/stop(_inverted) → paper; peace(_inverted)/two_up(_inverted) → scissors; other gestures, no_gesture hands and hand-free crops (faces, torsos, rooms) → none. Crops are squares of 1.1–3.0× the hand box, dropped when another class's hand would be in frame. 30,136 train / 5,040 val, **person-disjoint** (split by `user_id`) |
| v3 int8 accuracy | **92.2 %** on the real val set. Recall: rock 96.8 %, paper 97.2 %, scissors 92.9 %, none 91.0 % (the none misses are mostly other HaGRID gestures such as "three" and "ok" read as scissors/paper — hands a player does not show) |

Conversion: `python3 -m ipro_ai convert rps_int8.tflite -o rps.inm` with the
native `ips_aitool` from the IPRO AI Studio app (≈1 s). `preflight` predicted
11/11 operators on the accelerator; the board agreed.

### Feed

Three ways to get the frame into the NPU, switchable live because the right
one depends on how a model was quantised and whether its first layer is the
external-input conv:

| `rps feed` | what happens | when |
|---|---|---|
| `raw` (default) | `NPU_Img_Ext_Addr_Cfg(frame)` — the NPU reads the ISP ARGB bytes as they are | ext_input model, int8 zp −128 or uint8: what person_pet uses and what v1 was verified with |
| `xor` | every byte `^ 0x80` in place, then as raw | if a model turned out to need explicit u−128 |
| `copy` | CPU packs u−128 into the model's own input buffer | models whose layer 0 is not the ext-input conv |

## On the board (2026-09-18)

**v1** (CGI hands): rock / paper / scissors 95–100 % when shown as trained
(hand + wrist, centred), but in free play a bare fist held close was `NONE`
73 % of the time (20 s recording) and a face in the box read as a gesture.
Both are what a CGI-trained model does on a real camera.

**v3** (HaGRID): fist recognised 60–98 % across angles and distances, the
player reports rounds trigger reliably; a face alone in the box is `NONE`;
palm 84–98 %. `train/rps_capture.py` remains the tool for tuning to one user
or one room if a future setup needs it.

Pipeline numbers: NPU 7 ms / 14.9 fps (one frame in flight), preview
10 fps at 73–96 ms/frame, 0 ISP errors, 7.3 MB PSRAM free after the model.

## Files

```
main.c                boot: panel → model → camera (ROI 512x512 → 128x128) → UI
src/rps_model.[ch]    firmware or mfg → XIP → NPU; inference task; softmax; capture hook
src/model_blob.S      .incbin model/rps_v3.inm (CONFIG_RPS_EMBED_MODEL)
src/rps_game.[ch]     state machine, pure logic (host-simulated: 3000 rounds uniform)
src/rps_ui.[ch]       render task: preview, box, labels, countdown, bars; mirror
src/app_cli.c         rps / cam / lcd; base64 frame capture
components/board/ipro7ai_evk/   panel (st7789p3), camera → ISP → NPU feed
                      (evk_cam, the former cam_pipe), font — shared with
                      ipro7ai_evk_person_detect and ipro7ai_evk_launcher
model/                rps_v3.inm (default), rps_v1.inm (+ .tflite, numbers.json)
train/gen_data.py     CGI hands → composites (v1)
train/gen_hagrid.py   HaGRID crops (v3)
train/train_rps.py    Keras model, int8 export, val confusion, device samples
train/rps_capture.py  board frames → labelled PNGs
partition_cfg_1M_rps{,_embed}.toml, flash.sh, enter_dl.py
```

Training ran on an Apple M4 host (TF 2.20 CPU, ~40 s/epoch on 30 k images);
the scripts assume `~/Work/rps/{data,out}` on the training machine.

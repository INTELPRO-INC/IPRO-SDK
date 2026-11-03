# IPRO AI Teach (web) — train an image classifier for the IPRO7AI Reef in the browser

Show your objects to the Reef's camera, train on this computer, and deploy the
model to the board over the same USB cable. Nothing to install: the page runs
in **Chrome or Edge** on Windows, macOS or Linux. It is the same flow as the
Teach page of IPRO AI Studio, and builds the same model, byte for byte.

```
Collect ─► Train ─► Deploy ─► Run
camera     in the    USB serial   on the board's NPU,
crops      browser   (Web Serial) result drawn into the stream
```

## What you need

- An IPRO7AI Reef with the OV5640 camera, flashed with the Edge AI firmware
  [`apps/ai/ipro_reef_teach`](../../apps/ai/ipro_reef_teach/README.md) in `image` mode.
- One USB-C cable from the Reef to the computer. It powers the board and
  carries both the video and the model.
- Chrome or Edge (they have Web Serial; Safari and Firefox do not).
- Python 3.8 or newer, only to serve the page on this computer.

## Start

```bash
python3 tools/teach_web/serve.py
```

It serves this folder on `http://localhost:8780` (this computer only) and opens
it in the default browser; if that is not Chrome or Edge, paste the address
into one. The camera and the serial port need a page from `localhost` or
`https`; a file opened from disk cannot reach them.

The first time, the browser asks for the camera (allow it) and, on **Connect…**
or the first **Deploy**, which serial port to use: pick **IPRO7 UVC Cam**.

The page runs its neural-network backbone with LiteRT.js. It downloads it from
the jsDelivr CDN unless a local copy exists. For a computer without internet,
fetch the copy once (about 22 MB, checked against the npm integrity hash):

```bash
python3 tools/teach_web/serve.py --fetch-runtime
```

## Use it

1. **Collect.** Pick the Reef's camera (**IPRO7 UVC Cam**). The bright square
   is what the model sees. Select a class, put the object in the square, and
   hold **Hold to record** (or the space bar) for a few seconds. Keep the
   **BACKGROUND** class, recorded with nothing in the square, so the board can
   say "nothing there". Up to 8 classes; names use A–Z, 0–9 and up to 15
   characters, because the board draws them in its own font. **Review** drops
   bad pictures, for example those where the object is half out of the square.
2. **Train.** Choose a backbone and press **Train**. It takes a second or two.
   Every fourth picture of a class is held out and reported as a score; the
   pictures of one recording look alike, so read it as "the classes are
   separable", not as accuracy on new scenes.
3. **Deploy.** Press **Deploy**. The page builds the model, sends it with the
   labels, the board checks and writes it and restarts; the camera comes back
   by itself. A refused or interrupted transfer never touches the model
   already on the board.
4. **Run.** The board classifies the square on its own NPU and draws the label,
   its confidence and every class's probability beside the square.

**Save…** and **Open…** keep a set of pictures in a `.teach` file, to add to it
or train it again later.

| Backbone | On the board's NPU | Model size |
|---|---|---|
| MobileNetV2 0.35 (more accurate) | 12 ms, about 10 per second | 461 KB |
| MobileNetV1 0.25 (faster) | 5 ms, about 15 per second | 237 KB |

## How it works

| Step | Where | What |
|---|---|---|
| Crop | `app.js` | the centre square of the 640 × 360 stream (x 192–447, y 52–307), scaled to 128 × 128: what the board's ISP gives its NPU |
| Features | `backbone.js` | a frozen ImageNet MobileNet, int8, run by LiteRT.js (WebAssembly) up to the head's 4 × 4 × C input; the board feeds it `pixel − 128` in R, G, B order, and so does the page |
| Head | `train.js`, `train_worker.js` | softmax regression on the features averaged over the 4 × 4 positions (a mirror and two exposure changes per picture), in a worker |
| Model | `teach_inm.js` | no converter: the trained head (a 1 × 1 convolution) is quantised and written straight into a template `.inm` converted once per backbone; the result is identical, byte for byte, to what the converter makes |
| Deploy | `board.js` | Web Serial to the board's CDC port (VID 0x3896, PID 0x0705): `TDEP` with size and CRC-32, then the model and an 8-label block; `TINF` asks what the board runs |

The wire format and the label block are described in the firmware's
[`uvc_teach_link.h`](../../apps/ai/ipro_reef_teach/image/uvc_teach_link.h) and
[`teach_model.h`](../../apps/ai/ipro_reef_teach/image/teach_model.h).

## Voice page

`voice.html` teaches the Reef people's voices instead of objects: enroll a
person by reading a passage to the board, then watch who is speaking, live.
It needs the Reef flashed with
[`apps/ai/ipro_reef_teach`](../../apps/ai/ipro_reef_teach/README.md) and its voice
model, in `voice` mode; **Connect…** then lists **IPRO Reef Voice**. Everything runs
on the board: the page only sends commands and draws the labels it gets back.
`voice.html?demo` replays `test/voice_demo.log` (lines a board sent) without a
board.

## Files

| File | Role |
|---|---|
| `index.html`, `app.css`, `app.js` | the image page |
| `voice.html`, `voice.js` | the voice page |
| `voice_link.js` | Web Serial for the voice firmware (line protocol) |
| `backbone.js` | LiteRT.js loading (local copy or CDN) and feature extraction |
| `train.js`, `train_worker.js` | head training |
| `teach_inm.js` | head quantisation and `.inm` patching, label block |
| `board.js` | Web Serial: board info and deploy |
| `serve.py` | local server; `--fetch-runtime` for offline use |
| `assets/` | per backbone: `<name>_features.tflite` (the backbone without its head), `<name>.inm` (the template), `teach_assets.json` |

## If something is wrong

| Symptom | What to do |
|---|---|
| "This browser has no Web Serial" | use Chrome or Edge |
| "Open this page from http://localhost" | start it with `serve.py`, not by opening the file |
| No **IPRO7 UVC Cam** in the camera list | the board is not on the Teach firmware, or the cable is charge-only |
| "The camera went away" after a deploy | normal while the board restarts; it comes back by itself, otherwise unplug and replug |
| No picture in any program (macOS) | another program still holds the camera; quit it, unplug and replug |
| "The model runtime did not load" | no internet and no local copy: run `serve.py --fetch-runtime` on a computer with internet, or copy `vendor/` over |

## Tests

```bash
node tools/teach_web/test/patch.test.mjs   # .inm patching vs IPRO AI Studio's teach_inm.py (golden vectors)
node tools/teach_web/test/train.test.mjs   # head training vs Studio's teach.fit()
python3 tools/teach_web/serve.py           # then open http://localhost:8780/test/features.html:
                                           # the browser backbone vs Studio's LiteRT on the same crops
```

The browser runs the backbone with XNNPACK, Studio with TFLite's reference
kernels; their int8 feature maps differ by a few counts, which changed no
prediction when heads trained on either were compared (2026-10-30, the
3-class Reef set, both backbones). The golden vectors and `assets/` are
generated by IPRO AI Studio's asset builder (in its source repository).

# ipro_speaker_recog: On-Device Speaker Recognition

Identify who is speaking, on the IPRO7 itself. Audio from a PDM digital
microphone goes through a speaker-embedding model on the NPU, and the result is
matched against the voices enrolled on the device. The same audio can be
streamed to a phone over BLE (LC3), and a BLE central can enroll speakers and
receive the recognition results.

## Features

- Speaker (voiceprint) recognition on the NPU: 3 s of 16 kHz audio → 256-d
  embedding → cosine match against an enrolled gallery
- Enrollment with the device's own microphone, from the shell or over BLE;
  several clips per speaker are averaged into one voiceprint
- Continuous recognition every 3 s while audio is captured, with an energy gate
  that skips silent windows
- The gallery (names, voiceprints, match threshold) is saved to flash and
  restored at boot
- PDM DMIC capture at 16 kHz, 16-bit mono; recordings of up to 30 s in PSRAM
- BLE peripheral with the Nordic UART Service: command protocol, recording
  download, live LC3 (32 kbps) or raw PCM streaming, recognition results
- On-chip DAC playback of recordings and a live mic-to-speaker loopback

## How Speaker Recognition Works

```
PDM DMIC ─► AUADC (16 kHz, +18 dB) ─► 3 s window (48000 samples)
                                          │
                       energy gate: RMS < vi_vad threshold ─► skip
                                          │
     fbank: 300 frames x 80 mel bins ─► INT8 ─► mbv2.inm on the NPU
                                          │
              dequantise ─► L2-normalise ─► 256-d embedding
                                          │
   cosine similarity against every enrolled speaker (RAM gallery)
                                          │
        best score >= threshold (0.42) ─► ACCEPT <name>, else REJECT
```

- **Front end.** Each clip is turned into a log-mel filterbank of 300 frames x
  80 bins (3 s at 100 frames/s). Clips longer than 3 s are cut to their first
  3 s, shorter ones are zero-padded; anything under 0.5 s is refused.
- **Model.** `mbv2.inm`, a MobileNetV2-style speaker-embedding network
  quantised to INT8, runs on the NPU and produces a 256-dimensional embedding,
  which is L2-normalised.
- **Gallery.** Enrolling a speaker stores the normalised mean of the
  embeddings of all clips enrolled under that name. Identification computes
  the cosine similarity (a dot product of unit vectors) against every stored
  voiceprint and accepts the best match if it reaches the threshold (default
  0.42, adjustable over BLE). The gallery lives in RAM (up to 64 speakers);
  the first 15 are saved to the `media` flash partition.
- **Continuous mode.** While audio is being captured with voice ID enabled,
  the capture interrupt fills a 3 s accumulator in PSRAM. When it is full, the
  voice ID task checks its RMS level against the energy gate, runs the
  embedding if the window holds speech, reports the result, and starts the
  next window.

All embedding work runs in one low-priority task (`voice_id`, 32 KB stack), so
it never stalls audio capture or BLE streaming.

## Hardware Setup

### Board

- IPRO7 EVB with in-package 2 MB flash and 8 MB PSRAM (the image is built with
  `CONFIG_IPRO7_FLASH_2M=y` and `CONFIG_USE_PSRAM=y`)
- Serial console: **921600** baud, 8N1
- A phone or other BLE central (optional: needed only to receive audio and
  results over the air; everything else works from the shell)

### PDM microphone

| GPIO  | Function | Direction | Notes                                        |
|-------|----------|-----------|----------------------------------------------|
| GPIO0 | PDM CLK  | Output    | Clock to the DMIC (valid PDM CLK pins: 0, 2, 8) |
| GPIO1 | PDM DATA | Input     | Data from the DMIC (valid PDM DATA pins: 1, 3, 7) |

Any standard PDM MEMS microphone works. The pins are set by `PDM_CLK_PIN` and
`PDM_DATA_PIN` in `src/main.c`; the left PDM channel is used.

### Speaker (optional)

Playback goes through the on-chip DAC to an external audio amplifier. The
amplifier enable pin defaults to GPIO3, driven high to unmute. On the IPRO7
EVB, whose amplifier is enabled by GPIO22 driven low, run `dac_spk 22 0` once
after boot.

## Build

```bash
# From the SDK root
make -C apps/ai/ipro_speaker_recog

# Clean build
make -C apps/ai/ipro_speaker_recog clean && make -C apps/ai/ipro_speaker_recog
```

Output: `apps/ai/ipro_speaker_recog/build/build_out/ipro_speaker_recog_IPRO7.bin`

| Region | Used | Notes |
|--------|------|-------|
| Flash (image) | ~615 KB | FW partition of `partition_cfg_2M_meeting.toml` is 980 KB |
| OCRAM | ~61 KB of 224 KB | |
| PSRAM, static | ~202 KB | |
| PSRAM, heap | — | 960 KB recording buffer, 96 KB voice ID accumulator, fbank buffers, and the NPU runtime's model and activation buffers; the boot log prints the heap left over |

## Flash the Firmware and the Model

The speaker model is **not** part of the firmware image. At boot the firmware
reads `mbv2.inm` from the `mfg` flash partition, so the model is flashed once,
separately, and can be replaced without rebuilding the firmware.

| File | Where | Size |
|------|-------|------|
| `mbv2.inm` | `apps/platform/ipro7_demo/demo/ai/ai_model/mbv2.inm` | 1,032,080 bytes |
| Partition table | `apps/ai/ipro_speaker_recog/partition_cfg_2M_meeting.toml` | FW 980 KB, `mfg` 1 MB, `media` 16 KB |

Use the partition table in this directory. The `mfg` partition of the stock
`partition_cfg_2M.toml` is 824 KB, too small for the model (it needs 1,032,080
bytes plus a 4 KB partition header). The table keeps PSM, backup, media and
factory at the stock addresses.

Flash with IntelProCube, from its command line (`ipro_iot_tool_macos`,
`ipro_iot_tool_linux` or `ipro_iot_tool.exe`) or its GUI; see the SDK README for
the tool, the flash pin (`--chip-variant`) and download mode. The tool's `--mfg`
option writes a file to the `mfg` partition with the 4 KB header the firmware
expects. Pass `--pt` with this table on **every** flash: without it the tool
writes its default table, which has no `mfg` partition. `--skip-ota` is
optional. The table has no room for an OTA copy of the firmware; without the
option the tool prints `Warn: fw1 xz bin size is overflow with partition table,
don't create ota bin` and carries on.

### Flash from the command line

```bash
# First time: partition table + firmware + model
<IntelProCube dir>/ipro_iot_tool_macos --chipname ipro7 --chip-variant IPRO7 \
    --port /dev/cu.usbserial-XXXX --baudrate 2000000 --skip-ota \
    --pt apps/ai/ipro_speaker_recog/partition_cfg_2M_meeting.toml \
    --firmware apps/ai/ipro_speaker_recog/build/build_out/ipro_speaker_recog_IPRO7.bin \
    --mfg apps/platform/ipro7_demo/demo/ai/ai_model/mbv2.inm

# Firmware only (model already in mfg)
<IntelProCube dir>/ipro_iot_tool_macos --chipname ipro7 --chip-variant IPRO7 \
    --port /dev/cu.usbserial-XXXX --baudrate 2000000 --skip-ota \
    --pt apps/ai/ipro_speaker_recog/partition_cfg_2M_meeting.toml \
    --firmware apps/ai/ipro_speaker_recog/build/build_out/ipro_speaker_recog_IPRO7.bin

# Model only
<IntelProCube dir>/ipro_iot_tool_macos --chipname ipro7 --chip-variant IPRO7 \
    --port /dev/cu.usbserial-XXXX --baudrate 2000000 --skip-ota \
    --pt apps/ai/ipro_speaker_recog/partition_cfg_2M_meeting.toml \
    --mfg apps/platform/ipro7_demo/demo/ai/ai_model/mbv2.inm
```

Every command also writes boot2 and the partition table, the model-only one
included; rewriting them is harmless. The tool exits 0 even when flashing
fails; check its output for `[All Success]`. Reset the board after flashing.

If the tool stops with `Error: mfg bin size is overflow with partition table`
and `Error: IPRO PT CHECK FAIL`, the model was flashed with a table whose `mfg`
is too small, such as the stock `partition_cfg_2M.toml`. Use
`partition_cfg_2M_meeting.toml`.

### Flash with the IntelProCube GUI

1. Start IntelProCube, choose chip IPRO7 and open the IoT page. Select the
   serial port and baud rate.
2. Next to **partition table**, click **Browse** and choose
   `apps/ai/ipro_speaker_recog/partition_cfg_2M_meeting.toml` in the SDK (the
   dialog opens in the tool's own `chips/ipro7/partition` folder). The page then
   shows a row per partition, including **firmware** and **mfg**.
3. Tick **firmware** and browse to
   `build/build_out/ipro_speaker_recog_IPRO7.bin`. Tick **mfg** and browse to
   `mbv2.inm`; the dialog lists only `*.bin` files until you set its filter to
   **All files (\*.\*)**. To replace only the model, tick only **mfg**.
4. Click **Create & Download** and wait for `[All Success]`, then reset the
   board.

The GUI keeps the chosen table, so later flashes use it too. It has no option
for skipping OTA; with this table it prints the OTA warning above and flashes
normally.

### Check the boot log

A board with the model in place logs (tag `MEETING`):

```
model in 'mfg': base=0xf8000 size=1032080
AI ready: speaker-embed engine, threshold 0.42 (heap free: ...)
no saved gallery in 'media' — starting empty
```

On later boots the last line reads `Gallery restored from flash: N speaker(s),
threshold 0.42` instead.

If the model cannot be loaded, the firmware still boots — audio capture and
BLE work — but speaker recognition is disabled: `vi_reg`, `vi_id` and
`vi_start` answer `AI not ready`. The boot log shows why:

| Message | Cause |
|---------|-------|
| `partition 'mfg' not found` | The board's partition table has no `mfg` entry. Re-flash with `--pt partition_cfg_2M_meeting.toml`. |
| `bad model size ... (part ...) — is mbv2.inm flashed to 'mfg'?` | `mfg` does not hold a valid model: nothing was flashed there, or its header describes a model larger than the partition. Flash `mbv2.inm` with the table above. |
| `AI init failed — flash mbv2.inm to 'mfg' partition; voice ID disabled` | Printed after any of the above, and after `model load failed` when the NPU runtime rejects what it read from `mfg`. |

### Alternative: link the model into the firmware

For bench work where flashing a second file is a nuisance, set
`CONFIG_MEETING_EMBED_MODEL=y` (menu "IPRO speaker recognition demo"). `src/model_blob.S`
then includes the file named by `CONFIG_MEETING_EMBED_MODEL_PATH` (default
`../../platform/ipro7_demo/demo/ai/ai_model/mbv2.inm`, relative to `src/`) in
the image, and the firmware loads it from there instead of from `mfg`.

The cost: the image grows to about 1.65 MB. It no longer fits the 980 KB FW
partition of `partition_cfg_2M_meeting.toml` or the 1180 KB FW partition of the
stock OTA-capable 2 MB tables, so it needs a table whose FW partition spans
nearly the whole flash (for example FW at `0x3000` with size `0x1F5000`, no
`mfg`, no OTA copy). That gives up OTA, and the model can then only be changed
with a full firmware update. Keep the default for anything you ship.

### Which model file

The speaker-embedding engine in the AI library (`libai.a`, headers in
`components/ai/src/inst/include/audio/`) is built for `mbv2.inm`: its INT8
input and output quantisation constants are those of that model. Use only
`mbv2.inm` with this application.

`res2dnet.inm`, in the same directory, is a different speaker-embedding model
with different quantisation. It is the model of the speaker-ID mode in the
`ipro7_demo` application, which reads it from an SD card, and it does not work
with this application's engine.

## Shell Commands

All commands are on the serial console (921600 baud).

### Voice ID

| Command | Description |
|---------|-------------|
| `vi_reg <name>` | Record 3 s with the DMIC and enroll it as `<name>` (up to 31 characters). Enrolling the same name again adds a clip; three clips per speaker give a steadier voiceprint than one. A clip quieter than the energy gate is refused. |
| `vi_id [seconds]` | Record 1–5 s (default 3) and identify the speaker. Only the first 3 s are used. Prints `ACCEPT speaker=<name> score=<cosine>` or `REJECT (best score=<cosine>)`. |
| `vi_start` | Start continuous recognition, one result per 3 s window. Needs capture running (`pdm_stream on`). Results go to the log and, if a phone is connected, to BLE. |
| `vi_stop` | Stop continuous recognition. |
| `vi_list` | List enrolled speakers: slot, name, number of clips. |
| `vi_clear` | Delete all speakers, including the copy saved in flash. |
| `vi_vad [rms]` | Show or set the energy gate (default 250). Windows and enrollment clips with a lower RMS level are treated as silence. |
| `ai_cycles [runs]` | Run 1–50 inferences (default 5) on synthetic audio and print how the time splits between NPU and CPU. Needs `CONFIG_AI_CYCLE_PROFILE=y` (on in `.config`). |

`vi_reg` and `vi_id` pause any BLE stream or continuous recognition while they
record, then restore it.

### Recording and streaming

| Command | Description |
|---------|-------------|
| `pdm_rec [seconds]` | Record 1–30 s (default 5) into PSRAM. Blocks until done. |
| `pdm_dump [offset] [count]` | Print PCM sample values (default: the first 64). |
| `pdm_stats` | Show the last recording: bytes, duration, frames, peak level and peak dBFS. |
| `pdm_gain [dB]` | Show or set the AUADC digital gain (default +18, the hardware maximum). Takes effect at the next recording or stream start. See [Accuracy notes](#accuracy-notes) before changing it. |
| `pdm_ch [left\|right]` | Show the PDM channel (left). The channel is applied when the AUADC is initialised, which this example does once at boot, so changing it at runtime has no effect. |
| `pdm_stream on\|off` | Start or stop audio capture and BLE streaming. |
| `pdm_lc3 on\|off` | Stream LC3 (default) or raw PCM. |

### DAC playback and test

| Command | Description |
|---------|-------------|
| `pdm_play [gain_dB]` | Play the last `pdm_rec` recording through the DAC. |
| `pdm_level [gain_dB]` | Report the levels `pdm_play` would produce, without playing. |
| `pdm_loop [seconds] [gain_dB]` | Live microphone-to-speaker loopback, 1–120 s (default 10). |
| `dac_tone [freq] [seconds] [amp%]` | Play a sine tone (100–12000 Hz, 1–60 s). |
| `dac_gain [dB]` | Show or set the software playback gain (-20 to +48 dB). |
| `dac_dc <int16>` | Hold a constant DAC code, for measuring the output range with a meter. |
| `dac_spk <gpio> <unmute_level>` | Set the amplifier enable pin; `dac_spk 22 0` on the IPRO7 EVB. |
| `dac_stop` | Stop a tone, playback or loopback. |

`pdm_play` and `pdm_loop` refuse to start while recording or streaming.

### Example: enroll two speakers, then identify

Each `vi_reg` and `vi_id` records for 3 s after the command is entered; speak
normally, about an arm's length from the microphone. Log lines are left out
and the scores are illustrative.

```
vi_reg alice
Recording 3 sec for registration...
Speaker 'alice' registered at slot 0 (1 clip)
vi_reg alice
Recording 3 sec for registration...
Speaker 'alice' registered at slot 0 (2 clips)
vi_reg alice
Recording 3 sec for registration...
Speaker 'alice' registered at slot 0 (3 clips)

vi_reg bob
Recording 3 sec for registration...
Speaker 'bob' registered at slot 1 (1 clip)
...

vi_list
  0: alice (3 clips)
  1: bob (3 clips)

vi_id
Recording 3 sec for identification...

=== Voice ID Result ===
ACCEPT speaker=alice score=0.612

vi_id
Recording 3 sec for identification...

=== Voice ID Result ===
REJECT (best score=0.287)
```

The second `vi_id` was a voice that is not enrolled. For continuous
recognition:

```
pdm_stream on
vi_start
Voice ID enabled (every 3 sec)
```

Every 3 s the log then shows `[VI] Silence (rms=...), skip` for a quiet window,
or `[VI] Speech detected (rms=...), inferring...` followed by
`[VI] <name> score=<cosine> rms=<level> (embed <ms> ms)`, where `<name>` is
`unknown` for a rejected window. `vi_stop` ends it and `pdm_stream off` stops
capture. If a clip is too quiet to enroll, `vi_reg` prints
`Too quiet (rms=... < 250) — '<name>' NOT enrolled; speak louder or raise pdm_gain`.

### Accuracy notes

- **Enroll and identify with the same microphone.** The embedding captures the
  microphone and its signal chain as well as the voice. Audio recorded on a
  phone and compared against audio from the device's DMIC scores far lower for
  the same person, often below the threshold. Enroll on the device (`vi_reg`,
  or BLE command `0x0A`).
- **Keep the capture gain fixed.** The model's INT8 input scale is fixed and
  there is no feature normalisation, so the absolute signal level shifts every
  feature. Enrollment and identification must use the same `pdm_gain`; after
  changing it, re-enroll.
- **The energy gate depends on the gain.** On the board this example was tuned
  on, at +18 dB, the noise floor measured ~150 RMS and speech ~550 RMS; the
  default gate of 250 sits between them. After a gain change, measure both
  with `pdm_rec` and `pdm_stats` and set `vi_vad` between them.
- **Threshold.** 0.42 is the default decision threshold. Raise it to reject
  unknown voices more strictly, lower it to accept enrolled speakers more
  readily; set it over BLE (`0x14`). It is saved together with the gallery.

## BLE Protocol

The device advertises as **`IPRO-MIC`** with the Nordic UART Service (NUS).

| UUID | Characteristic |
|------|----------------|
| `6E400001-B5A3-F393-E0A9-E50E24DCCA9E` | NUS service |
| `6E400002-B5A3-F393-E0A9-E50E24DCCA9E` | RX: the phone writes commands here (write, write without response) |
| `6E400003-B5A3-F393-E0A9-E50E24DCCA9E` | TX: the device sends notifications here |

Enable notifications on TX before sending commands; nothing is sent until they
are on. Multi-byte fields are little-endian.

### Connection

- On connection the device requests Data Length Extension (251 octets,
  2120 µs) and a 7.5–15 ms connection interval (latency 0, 4 s timeout).
- Pairing is accepted automatically, with fixed passkey 123456.
- Advertising restarts 200 ms after a disconnect. A disconnect also stops
  capture: streaming, a meeting and continuous recognition.
- The device does not start an ATT MTU exchange. Ask for a larger MTU from the
  phone (on Android, `requestMtu()`): LC3 notifications are 81 bytes and need an
  MTU of at least 84, and the speaker list, bulk audio and raw PCM chunks all
  use the negotiated MTU.

### Commands (phone → device)

Write `[command ID][payload]` to RX.

| ID | Name | Payload | Reply |
|----|------|---------|-------|
| `0x01` | START_REC | `[seconds u8]` (1–30; absent = 5) | `0x81`, and `0x81` again when the recording ends |
| `0x02` | STOP_REC | — | `0x81` |
| `0x03` | GET_AUDIO | — | `0x82`, `0x83`..., `0x84` (or `0x85`) |
| `0x04` | GET_STATUS | — | `0x81` |
| `0x05` | SET_GAIN | `[gain_db i8]` (applies at the next capture start) | `0x81` |
| `0x06` | START_STREAM | — | `0x81`, then `0x86` (or `0x83`) frames |
| `0x07` | STOP_STREAM | — | `0x81` |
| `0x08` | START_VOICE_ID | — (needs capture running) | `0x87` per 3 s window with speech |
| `0x09` | STOP_VOICE_ID | — | — |
| `0x0A` | REG_SPEAKER | `[name ASCII]` (up to 31 bytes) | `0x81` RECORDING (the "speak now" cue), `0x81` when the 3 s recording ends, then one `0x87` |
| `0x0B` | LIST_SPEAKERS | — | `0x88` |
| `0x0C` | CLEAR_SPEAKERS | — | — |
| `0x0D` | START_MEETING | `[with_stream u8]` (optional, default 0) | `0x87` per 3 s window with speech; with `with_stream` = 1 also `0x86` frames |
| `0x0E` | STOP_MEETING | — | — |
| `0x0F` | REG_PCM_START | `[name_len u8][name][total_bytes u32]` | — |
| `0x10` | REG_PCM_DATA | `[PCM bytes]` | one `0x87` after the last chunk |
| `0x11` | DELETE_SPEAKER | `[slot u8]` | `0x88` (preceded by `0x85 03` on a bad slot) |
| `0x12` | RENAME_SPEAKER | `[slot u8][new name ASCII]` (up to 31 bytes) | `0x88` (preceded by `0x85 03` on a bad slot or duplicate name) |
| `0x13` | GET_CONFIG | — | `0x89` |
| `0x14` | SET_CONFIG | `[mask u8][gain_db i8][vad_rms u16][threshold_x100 u8]` | `0x89` with the values now in effect |
| `0x15` | GET_HEALTH | — | `0x8A` |

- **START_MEETING** starts capture with continuous recognition in one step.
  Without `with_stream` no audio goes over the air, which saves the LC3
  encoding and radio time; with it, the phone also gets the audio, for example
  to transcribe it while the device names the speakers.
- **REG_SPEAKER** records 3 s with the device's DMIC, exactly like `vi_reg`.
  Speak after the `0x81` with state RECORDING arrives.
- **REG_PCM_START / REG_PCM_DATA** enroll from audio uploaded by the phone:
  16 kHz, 16-bit mono PCM, at most 960,000 bytes (30 s). Capture is paused
  during the upload. If no chunk arrives for 2 s, the device enrolls what it
  has if that is at least 1.5 s, otherwise it reports `xfer_failed`. Because of
  the microphone mismatch described in [Accuracy notes](#accuracy-notes),
  prefer REG_SPEAKER.
- **SET_CONFIG** `mask`: bit 0 = gain (applied immediately if capture is
  running), bit 1 = energy gate, bit 2 = match threshold (`threshold_x100` /
  100, saved with the gallery). Fields whose bit is clear are ignored.

### Notifications (device → phone)

| ID | Name | Payload |
|----|------|---------|
| `0x81` | STATUS | `[state u8][recorded_bytes u32]` |
| `0x82` | AUDIO_HDR | `[sample_rate u16][total_bytes u32]` |
| `0x83` | AUDIO_DATA | `[raw PCM bytes]`: one chunk of a GET_AUDIO download or of a raw PCM stream |
| `0x84` | AUDIO_DONE | `[bytes_sent u32]` |
| `0x85` | ERROR | `[code u8]` |
| `0x86` | AUDIO_LC3 | `[LC3 frame, 40 bytes][LC3 frame, 40 bytes]`: 20 ms of audio |
| `0x87` | SPEAKER_ID | `[slot u8][score_x100 u16][name_len u8][name]` |
| `0x88` | SPEAKER_LIST | `[count u8]` then per speaker `[slot u8][name_len u8][name]` |
| `0x89` | CONFIG | `[version u8 = 1][gain_db i8][vad_rms u16][threshold_x100 u8]` |
| `0x8A` | HEALTH | `[rssi i8 (0x7F = unknown)][temp_c_x10 i16][vbat_mv u16][model_ready u8][speaker_count u8]` |

**STATUS state:** `0x00` idle, `0x01` recording, `0x03` capturing (a stream or
a meeting is running). `0x02` (sending) is defined in `ble_audio.h` but not
reported.

**ERROR codes:** `0x01` not ready to send audio, `0x02` no recording to send,
`0x03` bad argument, bad slot or AI not ready, `0xFF` unknown command.

**SPEAKER_ID:** `slot` is the gallery slot of the match, or `0xFF` for no
match or a failure. `score_x100` is the cosine similarity x 100. `name` is:

| `slot` | `name` | Meaning |
|--------|--------|---------|
| 0–63 | speaker name | Recognised (score = similarity), or enrolled (score = 100) |
| `0xFF` | `unknown` | Speech, but no enrolled speaker reached the threshold |
| `0xFF` | `too_quiet` | Enrollment clip below the energy gate, not enrolled |
| `0xFF` | `no_audio` | Less than 0.5 s of audio captured |
| `0xFF` | `inf_failed` | Embedding failed |
| `0xFF` | `reg_failed` | Gallery full |
| `0xFF` | `not_ready` | Model not loaded |
| `0xFF` | `xfer_failed` | Phone PCM upload stalled with less than 1.5 s received |

**HEALTH:** `rssi` of the connection in dBm, chip temperature in 0.1 °C, battery
voltage in mV, `model_ready` = 1 when the model is loaded, and the number of
enrolled speakers.

### Testing with nRF Connect

1. Connect to `IPRO-MIC`, request an MTU of 247, and enable notifications on
   the TX characteristic.
2. Write `15` to RX: in the `8A` reply, the second-to-last byte
   (`model_ready`) is `01` if the model is loaded.
3. Write `0A 61 6C 69 63 65` (REG_SPEAKER "alice"). When `81 01 ...`
   arrives, speak for 3 s. `81 00 ...` marks the end of the recording, and
   `87 00 64 00 05 61 6C 69 63 65` confirms "alice" in slot 0.
4. Write `0D 00` (START_MEETING, no stream) and talk: an `87` notification
   arrives for every 3 s window with speech. Write `0E` to stop.
5. Write `06` to stream and `07` to stop: `86` notifications, 81 bytes each,
   50 per second.

No phone application for this example ships with the SDK; any BLE central that
can write to NUS and parse the notifications above can drive it.

## LC3 Streaming

| Parameter | Value |
|-----------|-------|
| Codec | LC3 (liblc3, `components/3rdparty/liblc3`) |
| Frame duration | 10 ms |
| Sample rate | 16 kHz |
| Bytes per frame | 40 (32 kbps) |
| Frames per notification | 2 (one 20 ms capture frame) |
| Notification size | 81 bytes, 50 per second |

Each 20 ms capture frame (320 samples) is encoded as two 10 ms LC3 frames,
which are sent together in one `0x86` notification: 4 kB/s on air instead of
32 kB/s of raw PCM. With `pdm_lc3 off` the stream carries raw PCM in `0x83`
notifications instead, split to fit the MTU, which needs DLE and a large MTU to
keep up.

## Architecture

```
                                  IPRO7 firmware
 PDM DMIC ─► AUADC ─► DMA ping-pong (OCRAM, 2 x 640 B, 20 ms frames)
                              │
                     pdm_frame_callback (ISR)
        ┌──────────────┬──────┴───────┬────────────────┐
        ▼              ▼              ▼                ▼
   recording       stream ring    voice ID          DAC loopback
   buffer          + queue        accumulator       (pdm_loop)
   (PSRAM, 30 s)   (PSRAM)        (PSRAM, 3 s)
        │              │              │
   ble_send task   ble_strm task  voice_id task (32 KB stack, low priority)
   (GET_AUDIO)     LC3 encode     energy gate ─► fbank ─► NPU (mbv2.inm)
        │              │          ─► 256-d embedding ─► gallery match
        │              │              │                     │
        │              │              │              'media' partition
        └──────────────┴──────────────┴─► BLE NUS TX notifications
```

- **DMA buffers in OCRAM.** The AUADC DMA needs its buffers in OCRAM, not
  PSRAM: a 1280-byte non-cached static array.
- **Copy out of the ISR.** The ISR copies each frame into the stream ring, the
  voice ID accumulator or the recording buffer, so the consumers never race
  the DMA overwriting the next frame.
- **Heavy work outside the ISR.** LC3 encoding runs in the stream task and
  every embedding in the voice ID task, both below the capture interrupt and at
  priorities that keep BLE streaming ahead of recognition.
- **One engine, serialised.** The embedding engine shares its fbank and NPU
  buffers between calls, so all callers go through one mutex; the gallery has
  its own short-held mutex, so BLE commands can read it while an embedding is
  running.
- **Flash writes in one place.** Saving the gallery erases flash, which stalls
  execute-in-place code. Changes are collected and written by the voice ID
  task 1.5 s after the last one, never from the BLE thread.
- **AUADC initialised once.** The AUADC is set up at boot; recordings and
  streams only start and stop it. The first 5 frames (100 ms) of each
  recording are dropped while the microphone settles.

## Files

| File | Description |
|------|-------------|
| `src/main.c` | PDM capture, voice ID task, gallery and its flash persistence, model loading, shell commands, BLE callbacks |
| `src/ble_audio.c` | BLE NUS service, command protocol, LC3 encoding, audio download |
| `src/ble_audio.h` | Protocol constants and the callback interface between the two |
| `src/dac_test.c` | DAC playback, loopback and test commands |
| `src/model_blob.S` | Links the model into the image when `CONFIG_MEETING_EMBED_MODEL=y` |
| `partition_cfg_2M_meeting.toml` | 2 MB partition table with a 1 MB `mfg` partition for the model |
| `Kconfig` | `CONFIG_MEETING_EMBED_MODEL` and `CONFIG_MEETING_EMBED_MODEL_PATH` |
| `.config` | IPRO7, 2 MB flash, PSRAM, FreeRTOS, BLE, LC3, AI/NPU |
| `CMakeLists.txt`, `Makefile` | Build |

The speaker-embedding engine and the gallery are part of the AI component,
which ships as the prebuilt `libai.a`; their APIs are in
`components/ai/src/inst/include/audio/speaker_embed.h` and `gallery.h`.

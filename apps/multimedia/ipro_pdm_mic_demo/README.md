# PDM DMIC Recording Demo

Record audio from a PDM digital DMIC on IPRO7, store it in PSRAM, transfer or stream it to a phone via BLE, with optional LC3 compression.

## Features

- PDM DMIC capture at 16 kHz, 16-bit mono via DMA ping-pong
- Up to 4 hours recording (`START_REC` u16 LE seconds). Physical PSRAM ring is 240 s (~7.68 MB); older audio wraps. Every 30 s the device notifies `CHUNK_READY` so the phone can `GET_CHUNK` that slice while recording continues.
- BLE NUS (Nordic UART Service) for phone control
- Real-time audio streaming over BLE (~20ms frame latency)
- LC3 codec compression (8:1 ratio, 32 kbps) for streaming
- Shell commands for standalone debugging
- Android companion app with LC3 decoder and audio playback

## Hardware Setup

### IPRO7 EVB Wiring

| GPIO | Function | Direction | Notes |
|------|----------|-----------|-------|
| GPIO0 | PDM CLK | Output | Clock to DMIC (valid PDM CLK pins: 0, 2, 8) |
| GPIO1 | PDM DATA | Input | Data from DMIC (valid PDM DATA pins: 1, 3, 7) |

Any standard PDM DMIC module works (e.g., 3SM222, SPH0645, INMP441 with PDM output). Power the DMIC from a dedicated 3.3V supply.

### Board Requirements

- IPRO7 EVB with PSRAM (8 MB)
- Serial console at 921600 baud (for shell commands)
- Phone with BLE 4.2+ (for BLE control)

## Build

```bash
# From SDK root
make -C apps/multimedia/ipro_pdm_mic_demo

# Clean build
make -C apps/multimedia/ipro_pdm_mic_demo clean && make -C apps/multimedia/ipro_pdm_mic_demo
```

Output binary: `build/build_out/ipro_pdm_mic_demo_IPRO7.bin`

### Memory Usage

| Region | Used | Available | Usage |
|--------|------|-----------|-------|
| Flash | ~473 KB | 1024 KB | 46% |
| OCRAM (BSS) | ~68 KB | 224 KB | 30% |
| PSRAM | heap recording ring ~7.68 MB (240 s) | 8192 KB | ~94% |

The 7.68 MB recording ring and 5 KB stream ring are allocated from PSRAM heap at runtime. Logical recordings longer than 240 s wrap; `GET_AUDIO` then returns the last 240 s.

## Shell Commands

All commands are available over the serial console (921600 baud).

| Command | Description |
|---------|-------------|
| `pdm_rec [seconds]` | Record audio (1-14400s / 4 h, default 5s). Blocks until done. |
| `pdm_dump [offset] [count]` | Print PCM sample values (default: first 64 samples) |
| `pdm_stats` | Show recording statistics: duration, peak, RMS, dBFS |
| `pdm_gain <dB>` | Set PDM digital gain (-96 to +18 dB, default 0) |
| `pdm_ch left\|right` | Select PDM channel (default: left) |
| `pdm_stream on\|off` | Start/stop real-time BLE audio streaming |
| `pdm_lc3 on\|off` | Toggle LC3 compression for streaming (default: on) |

### Example Session

```
# Record 3 seconds
pdm_rec 3
Recording 3 seconds (ch=LEFT, gain=0 dB, 96000 bytes)...
Recording done: 96000 bytes, 150 frames

# Check stats
pdm_stats
=== PDM Recording Stats ===
Recorded:     96000 bytes (48000 samples, 3.00 sec)
Frames:       150
Peak sample:  4521 (of 32767)
RMS level:    312.5 (of 32767)
Peak dBFS:    -17.2 dB

# Dump first 20 samples
pdm_dump 0 20
Samples [0..19] of 48000:
   -12    34   -56    78   102   -45    23   -89   156  -203
    45   -67    89  -134   201  -178    56   -23    90  -145
```

## BLE Protocol

The device advertises as **"IPRO-MIC"** using the Nordic UART Service (NUS).

### NUS UUIDs

| UUID | Characteristic |
|------|---------------|
| `6E400001-B5A3-F393-E0A9-E50E24DCCA9E` | NUS Service |
| `6E400002-B5A3-F393-E0A9-E50E24DCCA9E` | RX (phone writes commands here) |
| `6E400003-B5A3-F393-E0A9-E50E24DCCA9E` | TX (device sends notifications here) |

### Commands (Phone to Device)

Write to the RX characteristic. Format: `[1 byte command ID] [payload...]`

| ID | Name | Payload | Description |
|----|------|---------|-------------|
| `0x01` | START_REC | `[uint16 LE seconds]` (legacy: 1 byte) | Start recording (1-14400 s) |
| `0x02` | STOP_REC | — | Stop recording early |
| `0x03` | GET_AUDIO | — | Download recorded audio as raw PCM (last 240 s if wrapped) |
| `0x04` | GET_STATUS | — | Query device state |
| `0x05` | SET_GAIN | `[int8 gain_db]` | Set PDM digital gain |
| `0x06` | START_STREAM | — | Start real-time audio streaming |
| `0x07` | STOP_STREAM | — | Stop streaming |
| `0x0C` | GET_CHUNK | — | Next unsent 30 s slice (live STT) |

### Responses (Device to Phone)

Received as notifications on the TX characteristic. Format: `[1 byte response ID] [payload...]`

| ID | Name | Payload | Description |
|----|------|---------|-------------|
| `0x81` | STATUS | `[uint8 state] [uint32 recorded_bytes LE]` | Current state |
| `0x82` | AUDIO_HDR | `[uint16 sample_rate LE] [uint32 total_bytes LE]` | Metadata before bulk transfer |
| `0x83` | AUDIO_DATA | `[raw PCM bytes]` | Chunked raw PCM data |
| `0x84` | AUDIO_DONE | `[uint32 bytes_sent LE]` | Bulk transfer complete |
| `0x85` | ERROR | `[uint8 error_code]` | Error occurred |
| `0x86` | AUDIO_LC3 | `[40 bytes LC3 frame]` | LC3-compressed 10ms audio frame |

### Status Byte Values

| Value | State |
|-------|-------|
| `0x00` | IDLE — ready for commands |
| `0x01` | RECORDING — PDM capture in progress |
| `0x02` | SENDING — bulk audio transfer in progress |
| `0x03` | STREAMING — real-time audio streaming active |
| `0x04` | RECORDED — local recording finished; phone should GET_AUDIO / GET_CHUNK remainder |
| `0x06` | CHUNK_READY — 30 s slice ready; phone should GET_CHUNK |

### State Machine

```
         START_REC             STOP_REC / timeout
  IDLE ───────────> RECORDING ──────────────────> IDLE
   │                                                │
   │  START_STREAM              STOP_STREAM         │
   └──────────────> STREAMING ──────────────────────┘
   │                                                │
   │  GET_AUDIO                 AUDIO_DONE          │
   └──────────────> SENDING   ──────────────────────┘
```

Recording completion automatically sends a STATUS notification to the phone so it knows data is ready for download.

### BLE Connection Parameters

On connection, the device automatically requests:
- **Data Length Extension (DLE)**: 251 octets / 2120 us for ~800 Kbps throughput
- **Fast connection interval**: 7.5-15 ms for low latency
- **Pairing**: auto-accept with passkey 123456

### Testing with nRF Connect

1. Scan and connect to "IPRO-MIC"
2. Find the NUS service, enable notifications on the TX characteristic (0x0003)
3. Write to RX characteristic (0x0002):
   - `01 05 00` = start 5-second recording (u16 LE; `01 05` still works)
   - Wait for `81 04 ...` notification (status = RECORDED)
   - `03` = download audio
   - Receive `82` header, then `83` data chunks, then `84` done
4. For streaming: write `06` to start, `07` to stop
   - Receive `86` LC3 frames (41 bytes each, 100 per second)

## LC3 Codec

The firmware includes Google's liblc3 encoder for compressed audio streaming.

| Parameter | Value |
|-----------|-------|
| Frame duration | 10 ms |
| Sample rate | 16 kHz |
| Input samples per frame | 160 |
| Output bytes per frame | 40 |
| Bitrate | 32 kbps |
| Compression ratio | 8:1 (320 bytes PCM to 40 bytes LC3) |

Each 20ms DMA frame (320 samples) is split into two 10ms LC3 frames (160 samples each), encoded to 40 bytes, and sent as individual `0x86` BLE notifications (41 bytes including header).

LC3 streaming fits within standard BLE notification throughput (~36 Kbps) without requiring DLE, making it compatible with any BLE 4.2+ phone.

Toggle LC3 on/off via shell (`pdm_lc3 on|off`). When LC3 is off, raw PCM frames are sent as `0x83` notifications (requires DLE for full throughput).

## Android Companion App

Located at `tools/android/IproMicDemo/`.

### Structure

```
app/src/main/java/com/ipro/micdemo/
├── MainActivity.kt      — App entry point, permission handling
├── BleManager.kt         — BLE scanning, connection, NUS read/write
├── AudioPlayer.kt        — AudioTrack playback of decoded PCM
├── Lc3Decoder.kt         — LC3 frame decoding (JNI wrapper for liblc3)
└── ui/MainScreen.kt      — Compose UI: connect, record, play, stream controls
```

### Building

1. Open `tools/android/IproMicDemo/` in Android Studio
2. Sync Gradle (requires Android SDK 33+)
3. Build and install on an Android device with BLE support

### App Features

- BLE scan and connect to "IPRO-MIC"
- Record button: sends START_REC, waits for completion, downloads audio
- Play button: plays downloaded PCM through AudioTrack
- Stream button: starts real-time streaming with LC3 decode and playback
- Gain slider: sends SET_GAIN commands
- Status display: shows device state, recording duration, transfer progress

## Architecture

```
                              IPRO7 Firmware
┌──────────────────────────────────────────────────────────────────┐
│                                                                  │
│  PDM DMIC ──> AUADC ──> DMA ping-pong (OCRAM, 2 x 640B)        │
│                              │                                   │
│                     pdm_frame_callback (ISR)                     │
│                         │              │                         │
│                    Recording       Streaming                     │
│                    ┌────┴────┐    ┌────┴────┐                    │
│                    │ memcpy  │    │ ring buf │                    │
│                    │ → PSRAM │    │ → queue  │                    │
│                    │ (320KB) │    │ (PSRAM)  │                    │
│                    └────┬────┘    └────┬────┘                    │
│                         │              │                         │
│                  pdm_monitor     stream_send_task                │
│                    task               │                          │
│                         │        ┌────┴────┐                     │
│                  ble_audio_      │ LC3 enc │ (2 x 160 samples)   │
│                  notify_done     │ → 40B   │                     │
│                         │        └────┬────┘                     │
│                         │              │                         │
│                  audio_send_task  bt_gatt_notify                 │
│                  (bulk transfer)  (0x86 LC3 or 0x83 PCM)         │
│                         │                                        │
│                    bt_gatt_notify                                 │
│                    (0x83 PCM chunks)                              │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘
                              │
                         BLE NUS
                              │
┌──────────────────────────────────────────────────────────────────┐
│                       Phone App                                  │
│                                                                  │
│   NUS RX ──> command dispatch ──> reassemble/decode ──> play     │
└──────────────────────────────────────────────────────────────────┘
```

### Key Design Decisions

1. **DMA buffers in OCRAM** — The AUADC DMA peripheral requires buffers in OCRAM (not PSRAM). The DMA double-buffer is placed in a `ATTR_NOCACHE_NOINIT_RAM_SECTION` static array (1280 bytes).

2. **Stream ring buffer in PSRAM** — The ISR copies each DMA frame to a ring buffer in PSRAM before queuing the pointer. This prevents the stream_send_task from racing with DMA overwrites.

3. **No cache invalidation on PSRAM ring buffer** — The ISR writes to the ring buffer via CPU (which goes through cache). The stream_send_task reads it also via CPU. Since both sides use the cache, invalidation would discard valid data.

4. **LC3 encoding in the BLE task (not ISR)** — LC3 encode takes ~1-2 ms per frame. It runs in `stream_send_task` which has lower priority than the ISR, keeping the audio capture path jitter-free.

5. **PDM initialized once** — The HAL AUADC is initialized once at boot. Subsequent recordings use `hal_auadc_start()`/`hal_auadc_stop()` without reinitializing, because reinit breaks the DMA configuration.

6. **DMIC startup discard** — The first 5 DMA frames (~100ms) are discarded after each `hal_auadc_start()` to allow the DMIC's internal oscillator to stabilize.

## Files

| File | Description |
|------|-------------|
| `src/main.c` | PDM capture, recording/streaming API, shell commands |
| `src/ble_audio.c` | BLE NUS service, command protocol, LC3 encoding, audio transfer |
| `src/ble_audio.h` | Public API and protocol constants |
| `.config` | Kconfig: IPRO7 + PSRAM + FreeRTOS + BLE + LC3 |
| `CMakeLists.txt` | Build: source files, BLE stack includes, LC3 includes |
| `Makefile` | Top-level make wrapper |

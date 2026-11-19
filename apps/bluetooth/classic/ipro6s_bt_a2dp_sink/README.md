# IPRO6S Classic Bluetooth: A2DP sink (Bluetooth speaker) with AVRCP

The IPRO6S as a Bluetooth speaker. A phone or computer pairs with the board
and plays audio to it over A2DP; the board decodes the SBC stream and plays
16-bit PCM on its I2S port for an external DAC or amplifier. Over AVRCP the
board shows the track title and sends play / pause / next / previous to the
phone.

| Item | Value |
|---|---|
| Board | IPRO6S EVK (V1.0 or V2 board), console and flashing through J_UART |
| Bluetooth name | `IPRO6S-Speaker` (Class of Device: loudspeaker) |
| Profiles | A2DP 1.4 sink, SBC (44.1 / 48 kHz, mono or stereo); AVRCP controller and target |
| Pairing | Secure Simple Pairing, no PIN; bonds kept in flash |
| Audio out | I2S master, standard I2S, 16-bit slots (BCLK = 32 × sample rate), no MCLK |
| Controller | prebuilt `libipro_ble_controller_ipro6s_link1_br_release.a` (BR/EDR + LE) |

## 1. Optional: connect an I2S DAC or amplifier

Without one the board still receives and decodes the stream, and `a2dp`
shows it; you just hear nothing. Any I2S DAC or amplifier board that takes
16-bit standard I2S as a slave without MCLK will do.

| I2S signal | IPRO6S | EVK V1.0 board (H1) | EVK V2 board (J_GPIO) |
|---|---|---|---|
| BCLK | GPIO0 | pin 1 | pin 10 |
| FS (LRCLK, WS) | GPIO1 | pin 3 | pin 11 |
| DO → DAC data in | GPIO3 | pin 7 | pin 12 |
| GND | - | pin 2 | pin 2 |

Power the DAC board as its own instructions say (the V2 board's J_GPIO pin 1
is 3.3 V for small loads). `CONFIG_APP_I2S_PIN_BASE` moves the group to
another set of four pins (BCLK, FS, -, DO = base + 0, 1, 2, 3).

## 2. Build

```bash
make -C apps/bluetooth/classic/ipro6s_bt_a2dp_sink DEFCONFIG=defconfig_evk
```

About 710 KB of flash. Image:
`apps/bluetooth/classic/ipro6s_bt_a2dp_sink/build_evk/build_out/ipro6s_bt_a2dp_sink_IPRO6.bin`.

## 3. Flash

IPRO Debug Probe on J_UART (pin-reversed cable); the probe's UART port ends in
`02`. From the IntelProCube folder:

```bash
./ipro_iot_tool_macos --chipname ipro6 --chip-variant IPRO6S \
    --port /dev/cu.usbmodemXXXX02 --baudrate 2000000 \
    --pt chips/ipro6/partition/partition_cfg_fw_direct.toml \
    --firmware <SDK>/apps/bluetooth/classic/ipro6s_bt_a2dp_sink/build_evk/build_out/ipro6s_bt_a2dp_sink_IPRO6.bin
```

Only `[All Success]` at the end means success; the tool resets the board.

## 4. Start

Console at 115200 baud. You should see:

```
I (...) A2DP: IPRO6S Classic Bluetooth A2DP sink example
I (...) BT: controller ready (BR/EDR + LE)
I (...) I2S: format 44100 Hz, 2 channel(s)
I (...) I2S: ready: BCLK GPIO0, FS GPIO1, DO GPIO3
I (...) A2DP: A2DP sink (SBC) and AVRCP ready
I (...) BT: name "IPRO6S-Speaker", address XX:XX:XX:XX:XX:XX
I (...) BT: discoverable, waiting for a connection
```

## 5. Play from a phone or a Mac

1. In the phone's Bluetooth settings (or **System Settings → Bluetooth** on
   a Mac), pair with **IPRO6S-Speaker**. The console prints
   `BT: connected`, `BT: paired, bond saved`, then `A2DP: A2DP connected`
   and `AVRCP: remote control connected`.
2. Choose IPRO6S-Speaker as the audio output (on a Mac: **Control Center →
   Sound**) and play music. The console prints:

   ```
   I (...) A2DP: configured: SBC 44100 Hz, 2 channel(s)
   I (...) I2S: format 44100 Hz, 2 channel(s)
   I (...) A2DP: stream open
   I (...) A2DP: playing
   I (...) AVRCP: phone is playing
   I (...) AVRCP: title: <the track's title>
   ```

   With a DAC connected you hear the music.
3. Check the stream:

   ```
   ipro /> a2dp
   I (...) A2DP: link up, stream playing, SBC 44100 Hz x 2
   I (...) A2DP: packets 1520 (...), SBC frames 10640, decode errors 0, sequence gaps 0
   I (...) A2DP: I2S on, ... bytes out, buffered ..., underruns 0, drops 0
   ```

   Run it again: packets and SBC frames keep growing while music plays;
   decode errors stay 0.
4. Control the phone from the board:

   ```
   ipro /> avrcp pause
   ```

   The phone pauses; the console prints `A2DP: paused` and
   `AVRCP: phone is paused`. `avrcp play`, `avrcp next` and `avrcp prev`
   work the same way.

A phone or Mac suspends the stream about 10 seconds after playback stops
(`A2DP: paused`); that is normal.

## Console commands

| Command | What it does |
|---|---|
| `a2dp` | link, stream state, packet / decoder / I2S counters |
| `avrcp play` \| `pause` \| `next` \| `prev` | send the key to the phone |
| `bt_unpair` | forget every paired device (disconnect first) |
| `reboot` | restart |

## Use it in your product

- PCM leaves `a2dp_decode()` in `main.c` (16-bit, interleaved); replace
  `a2dp_i2s_write()` to send it elsewhere, or add volume, EQ or mixing there.
- `a2dp_i2s_out.c` drives the I2S port: a 32 KB ring buffer, DMA ping-pong,
  playback starts once 4 KB is buffered. 48 MHz is not a whole multiple of
  44.1 kHz × 32, so BCLK alternates between two adjacent dividers to keep
  the long-term sample rate exact.
- `CONFIG_APP_I2S_OUTPUT=n` removes the I2S part (decode only).
- `bt_classic_common.c` is the bring-up shared with the SPP example: RF and
  controller start, host start, bonds in flash at 0x1E8000, and the scan
  policy (one BR/EDR link; not discoverable while it is up, still
  connectable).

## Troubleshooting

| Symptom | Cause | Fix |
|---|---|---|
| The phone pairs but does not offer the board as audio output | A2DP was not connected | disconnect and reconnect from the phone; check for `A2DP: A2DP connected` |
| `packets` grow, `SBC frames` stay 0 | the decoder was not set up | look for `configured: SBC ...`; reconnect |
| Music but crackles | DAC wiring or clock format | short wires, common GND; the DAC must accept 32 BCLK per frame (16-bit slots) |
| `underruns` grows quickly | the radio link drops packets | move the phone closer; `sequence gaps` counts lost packets |
| `AVRCP: no remote control connection` | the phone did not open AVRCP | reconnect from the phone |
| Pairing fails after re-flashing | old bond on the phone | remove `IPRO6S-Speaker` on the phone, `bt_unpair` on the board, pair again |

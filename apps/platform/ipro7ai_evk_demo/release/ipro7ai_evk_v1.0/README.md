# IPRO7AI EVK — bring-up firmware v1.0

Panel, camera, speaker, microphone, SD card, video playback, BLE and LE Audio,
all driven from one shell. Four ways in reach that same shell, so a command
added to the firmware works everywhere at once.

```
evk_manual.bin     the firmware
COMMANDS.html      full command reference — open in a browser
EVKBench.apk       Android control app
tools/evk.py       terminal control over BLE
tools/mkvid.py     pack video for the board
```

## Flash it

From the SDK root, with the board on USB serial:

```bash
python3 tools/serial_monitor.py /dev/cu.usbserial-XXXX --send reboot_dl --timeout 4

tools/ipro_iot_tool_lite/ipro_iot_tool_lite --chipname ipro7 \
    --port /dev/cu.usbserial-XXXX --baudrate 921600 --flash-pin 0x02 \
    --firmware evk_manual.bin
```

The serial port name changes when the USB adapter is swapped — check
`ls /dev/cu.*` rather than assuming. Use `cu.*` and not `tty.*`: the latter
blocks waiting for carrier detect.

Console is 115200 on the platform UART. The panel initialises and draws colour
bars during boot, so a correctly wired board shows something before you type
anything.

## Connecting without a cable

The radio starts on its own at boot. It takes a little over ten seconds — RF
calibration and the 32 kHz qualification — and says so when it is ready:

```
BT: advertising as "IPRO7AI-EVK" - the Android app, tools/evk.py and
    the web panel can connect now
```

Then any of:

```bash
adb install -r EVKBench.apk          # phone
python3 tools/evk.py                 # terminal, needs: pip install bleak
python3 tools/evk.py "bt rf"         # one command and exit
python3 tools/evk.py -f checks.txt   # replay a whole test run
```

**One connection at a time.** If a client cannot connect, the first thing to
rule out is another one already holding the link.

## Video playback

Clips are packed on the host — the board has no video decoder and the panel sits
behind a 16 MHz SPI bus, so scaling, RGB565 conversion and byte order are all
done ahead of time. What lands on the card is exactly what gets pushed to the
panel.

```bash
python3 tools/mkvid.py clip.mp4 -o out/ -f 8      # 320x240, 8 fps
```

Copy the `.ipv` files to the card root, then `sd mount` and `vid play 0`.

Two limits are worth knowing before packing anything:

- **Keep a clip under about 6 MiB.** The whole thing is loaded into PSRAM before
  playback starts, and the largest block the heap will hand out is well short of
  the 8 MB total — `vid mem` reports the real figure.
- **Play the largest clip first.** The buffer is claimed once and never resized,
  so whichever clip loads first fixes the reservation for the session.

A variant that plays automatically on card insertion exists for unattended
demos — it starts the same watcher `vid auto on` does. It is not part of this
release and not the default, because the clip buffer takes most of the PSRAM
heap and the camera's ISP frames come from the same place. Build it with
`cp config_autoplay .config`.

## When something is wrong

The order matters more than the individual commands, because the first check
invalidates every one below it if it fails.

1. **`bt rf`** — `roscal` must read 3. An uncalibrated receiver reports no error;
   it drops packets, and the result imitates every other fault in the chain.
   Measured loss on this board ran from 1.2 % to 30 % on nothing else.
2. **`spk tone 1000 2 30`** — no decoder, no radio, no resampling. Clean here
   means the converter, amplifier and speaker are fine and the fault is upstream.
3. **`bt rate 16`**, then reconnect — at 16 kHz the decoder upsamples internally,
   so no anti-alias filter runs and the rate matcher sits at unity. Still wrong
   at 16 kHz means the conversion was never the problem.
4. **`bt capture 5`** during a stream, then **`mic play`** — a clean replay says
   the decoder output was always right and the fault is in delivery.
5. **`sd bench sd:/0.ipv`** — reads a file straight through with nothing else
   running, which separates a card that cannot sustain a rate from one that only
   fails while the panel is also being driven.

`COMMANDS.html` has the full list with the same reasoning attached to each
peripheral.

## Board notes that cost time to find

- **GPIO41 is both the boot strap and the camera MCLK.** A jumper left on J6
  silences the sensor clock while SCCB carries on answering, which reads as a
  dead camera rather than a wiring choice.
- **Zero camera frames *and* zero errors** means no pixel data arrives at all,
  not corrupted data. With SCCB working, look at the DVP side.
- **VDDIO1 must be 1.8 V for the camera** — J6 PIN1/PIN3. That bank carries the
  DVP and SCCB pins; the panel sits in a different bank at 3.3 V.
- **The speaker amplifier enable is active high.** The shared audio component
  assumes active low and drives it the other way, so the app re-asserts it.
- **The firmware cannot read the battery.** The charger's status outputs drive
  LEDs and reach no GPIO, and the cell never meets an ADC pin. The channel named
  "VBAT/2" taps the SoC's own supply, which here is the 3V3 rail. What *is*
  readable is whether the connector has power: `vbus` reads the USB
  transceiver's comparator. That is connector power, not charge state.
- **There is no 32.768 kHz crystal** — those pads are the PDM microphone pins.
  The radio runs from RC32K, whose calibration residual is redrawn every boot.
- **SD paths need the `sd:` prefix.** A bare filename goes to drive 0, which has
  no work area and returns `FR_NOT_ENABLED` rather than "file not found".

## Rebuilding

```bash
cp config_bt .config          # manual (this build)
cp config_autoplay .config    # plays on card insertion
make -C apps/platform/ipro7ai_evk_demo
```

The Android project is `tools/android/`; open it in Android Studio, or build it
with any Gradle 8.11+ against the installed SDK.

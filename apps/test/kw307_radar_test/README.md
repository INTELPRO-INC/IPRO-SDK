# KW307 24 GHz presence radar — IPRO7 bring-up

Keywave KW307-P (HW `KW307-P-M1`, FW 2.7.6, datasheet v2.5, SDK guide v2.5.1).
The module streams a 27-byte detection frame at 20 Hz over UART; this app puts
that stream on IPRO7 UART1 and exposes it through the shell on UART0.

Build: `make -C apps/test/kw307_radar_test`
(flash 182 KB / 1 MB, OCRAM 41 KB / 256 KB. The history ring — 457 KB at the
default 900 s — and the FFT scratch come from the PSRAM heap at runtime.)

The driver, history ring and analysis live in `components/radar/` so a product
app can use them without copying this probe — see that README for the protocol,
the distance correction, and the range-time tensor contract. This app is the
shell around them.

## Read this before you wire anything

**The KW307 is 3.3 V logic and is explicitly NOT 5 V tolerant** (datasheet,
"Configuration and monitoring tool"). Its `VDIO` absolute maximum is 3.6 V.

IPRO7 pads are grouped into three banks, each with its own VDDIO:

| Bank     | Pads                  | On the IPRO7AI EVK |
|----------|-----------------------|--------------------|
| VDDIO_1  | GPIO0–9, GPIO36–41    | **2.8 V** (J6 pin 3 wired to pin 7) — the only usable supply on V1 boards |
| VDDIO_2  | GPIO10–19             | 3.3 V |
| VDDIO_3  | GPIO20–35             | 3.3 V |

So on a V1 EVK, **GPIO0–7 should not be wired straight to the KW307**:

- the module's 3.3 V TX drives a 2.8 V pad above its own supply, into the ESD
  clamp — treat that as a damage path, not a warning;
- our 2.8 V TX is below the module's 3.3 V logic level; whether it meets the
  input threshold has not been checked.

`kw_init` prints this warning when the pads you pick fall in VDDIO_1 and the
build has `CONFIG_IPRO7AI_EVK`.

### Two ways to use GPIO0–7 anyway

J6 cannot put this bank at 3.3 V on a V1 board. The old advice — "move J6 to
3.3 V (short PIN3/PIN5)" — came from the IPRO7AI_MODULE_SUB_0805 draft: on V1
boards J6 pins 4/5 (3V3_SOC) measure 0 V, so that jumper leaves VDDIO1
unpowered and the chip enters download mode instead of booting (see
`bsp/board/ipro7/IPRO7AI_EVK.md`). This README used to call it "the
configuration in use"; on a V1 board it cannot have been, so the VDDIO1
voltage of the earlier GPIO0–2 bench runs is not known.

Pin conflicts are not the problem: with the camera unused nothing else on the
board collides — radar on GPIO0–2, USB DP/DM on GPIO8/9, LCD on GPIO10–17, PDM
mic on GPIO18/19, console on GPIO20/21 and JTAG on GPIO11/24–26 all coexist.

1. **Level shifter.** A TXS0102/TXS0108E (or two-FET translator) between the
   module and GPIO0–7, 2.8 V on the SoC side and 3.3 V on the module side. Keeps
   the camera. Costs a part and a small board.
2. **Use a 3.3 V bank instead.** Any pad can carry UART1, so GPIO10–35 works
   with no extra hardware. This is the fastest path if GPIO0–7 is not a fixed
   requirement.

## Wiring

| KW307 pin | Signal | To IPRO7 | Note |
|-----------|--------|----------|------|
| P1 | 5V   | 5 V (3.6–5.5 V) | on-module LDO; add 10 µF near the pin |
| P2 | GND  | GND | |
| P3 | RX   | our **TX** pad | 3.3 V logic |
| P4 | TX   | our **RX** pad | 3.3 V logic |
| P5 | GPO  | any input pad   | presence flag; needs a MOSFET to drive a load |
| P6 | ICLK | leave open | programming only |
| P7 | IDAT | leave open | programming only |
| P8 | nRST | leave open (or a spare pad) | active low |

Pin pitch is 2.54 mm. The datasheet asks for **≥8 mm total clearance** in front
of and behind the antenna, mounting at 1.5–2 m, and no metal in the field of
view.

UART1 signal routing: a pad reaches signal slot `gpio % 12`, and that slot is
pointed at UART1_TXD/RXD. **TX and RX must differ modulo 12** or they fight over
one slot — `kw_init` rejects that pairing rather than letting it fail silently.

## Do not start a second shell

`ipro_platform_init()` already calls `shell_init_with_task(UART0_INDEX)` itself
(`components/os/freertos/freertos_app_hooks.c`). Older apps such as
`apps/test/uart1_test` use the pre-consolidation skeleton — `ipro_setup_heap()`
+ `platform_init(0)` + `hal_boot2_init()` — which does *not* start the shell, so
they call it from their own task. Copying that call alongside the new
`ipro_platform_init()` starts a second shell task: the banner prints, a second
`ipro />` prompt appears, and the board faults with an Illegal Instruction at a
garbage PC with every register reading `0xffffffff`. This app therefore has no
task of its own at all — every command is shell-driven.

## Bring-up order

```
kw_init 0 1 2       # or your chosen pads; prints the VDDIO_1 warning if relevant
kw_sniff 5          # read-only: does anything arrive, and does it frame?
kw_mon 10           # decoded presence + targets
kw_stat             # frame rate, missed frames, CRC errors
```

`kw_sniff` is the first-contact tool and distinguishes the three usual failures:

| Symptom | Meaning |
|---------|---------|
| no bytes at all | ground, the module's 5 V rail, or our RX is not on the module's P4 |
| bytes but no frames | baud, level mismatch, or TX/RX swapped |
| frames at ~20 Hz | link is good — move on to detection tests |

Configuration (all take effect immediately, none are persisted unless the
module's own save command is issued — which this SDK release does not document):

```
kw_gain             # read back
kw_gain 5           # index 0..8 = -3..+21 dB, boot default 5 (+12 dB)
kw_range 50 1000    # cm, boot default 50/1000
kw_fov 120          # degrees, boot default 120
kw_power 1          # 0 = deep sleep, 1 = normal
kw_gpo              # read the presence pin
kw_dist 5           # average one measurement point into a single line
kw_scale            # show the distance divisor (1 = raw wire value)
```

## Analysis

The radar runs in its own task from `kw_init` onward and every frame lands in
the PSRAM history ring, whether or not a command is watching. That is what lets
the windowed analysis work at all — the breathing estimate alone needs 51.2 s
of uninterrupted history.

```
kw_hist                 # ring status: how much history is banked
kw_act 10               # activity over the last 10 s: empty/moving/still/settling
kw_breath               # breathing estimate (needs ~52 s of stationary history)
kw_map 60               # range-time waterfall, drawn on the console
kw_capture walking 60   # the same map as one CSV line, labelled, for training
```

`kw_map` prints the exact tensor a model would be fed, as a waterfall — range
down the rows, time left to right, newest on the right. A person walking away
shows as a track descending to the right; walking towards the sensor, ascending.

## Output over USB

`CONFIG_DEBUG_CONSOLE_USB=y` mirrors printf to a USB CDC-ACM port on the
board's USB-C connector and lets the shell read from it, so a single cable to a
PC carries both the commands and the data. The UART console keeps working
unchanged. `kw_stat` reports whether the host has opened the port.

That is the collection path for `kw_capture`: log the CDC port to a file, and
every `MAP,<label>,...` line is one labelled training example in the same
layout the NPU will later be fed.

## What the module does and does not give you

The only output is CMD `0xF0`: a frame index, one presence flag, two moving
target slots and one stationary slot, each carrying distance / angle /
magnitude / state / counter. That is **540 B/s**. There is no raw ADC, no IQ,
no range-Doppler and no point cloud in the documented command set — the
detection decisions are made inside the module.

### 🔴 The reported distance is twice the true range

**Measured 2026-08-31, and it contradicts the vendor documentation.** SDK User
Guide §2.1 states `Distance_mm ... (2000 mm = 200 cm)`. A two-point test with
targets at a tape-measured 1.0 m and 3.0 m showed the module reporting exactly
double — the *difference* was 4.0 m, not 2.0 m, so this is a scale error and not
a constant offset.

Physically that is the round-trip path length: range = c·τ/2, and the /2 is
missing. So `kw307_tracker_t.distance_raw` keeps the wire value untouched and
`kw307_distance_mm()` applies the correction in one named place. Never fold it
into the decoder — a Keywave firmware update could remove it, and then a silent
/2 becomes a silent halving of every reading.

`kw_scale` shows the divisor and `kw_scale 1` reverts to raw wire units, so the
measurement can be repeated on a new module or a new firmware without a rebuild.

**Re-run the two-point test on any new module or firmware version before
trusting a distance.** Note also that the *angle* scale has not been verified
independently — only the distance was measured.

Still open: the module silkscreen says `2T3R` while datasheet p.3 draws 3× LNA
and 1× PA (1T3R).

Also note the datasheet's own power figures disagree: the cover claims 12 mW at
20 Hz, while the DC table gives 4.0 mA — which at the 3.6 V supply minimum is
14.4 mW and at 5 V is 20 mW.

## Files

| File | Role |
|------|------|
| `main.c`      | shell commands — the whole app |
| `usb_config.h`| cherryusb configuration (CDC-ACM only) |

Everything else is in `components/radar/`.

Run the host tests on the dev machine before touching hardware:

```
sh components/radar/test_host/run.sh
```

The protocol vectors are the complete frames printed in the vendor guide §1.8,
so a pass means our framing matches Keywave's rather than merely being
self-consistent. The map tests pin the age-to-column arithmetic: a waterfall
drawn backwards in time still looks like a plausible picture, so only a test
catches it.

A macOS/Linux host tool speaking the same protocol over a USB-UART adapter is in
`tools/kw307/` — useful for characterising the module before the firmware path
is wired up.

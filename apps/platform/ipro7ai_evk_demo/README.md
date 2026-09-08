# ipro7ai_evk_demo — IPRO7AI EVK board demo

Board demo and bring-up tool for the **IPRO7AI EVK sub-board**
(`CONFIG_IPRO7AI_EVK`, schematic IPRO7AI_MODULE_SUB_0805). Board-level details —
power tree, J6 jumpers, what firmware can and cannot see — live in
`bsp/board/ipro7/IPRO7AI_EVK.md`; this README covers the drivers.

Seven blocks, each with its own shell command:

- **Display** — HEM LCM-T2D4BP-084, 2.4" IPS, Sitronix **ST7789P3**, 240×320,
  4-line SPI at 3.3 V. Driver + `lcd` shell command + test patterns.
- **Camera** — SmartSens **SC101IOT**, 1280×720 DVP at 1.8 V, live-previewed on
  the panel. `cam` shell command.
- **Speaker** — on-chip DAC (GPIO23) into a **TPA2037D1** mono class-D driving
  J1. `spk` shell command.
- **Microphone** — Infineon **IM69D129FV01** PDM MEMS mic on GPIO18/19 through
  the AUADC. `mic` shell command, including a live waveform on the panel.
- **Supply rail** — the SoC's own VBAT read through the internal VBAT/2 tap,
  plus a generic GPADC reader. `vbat` and `adc` shell commands.
- **micro-SD** — SDH controller with FatFS at `sd:`. `sd` shell command.
- **Bluetooth LE + LE Audio** — new controller (`ipro_ble_controller`,
  RivieraWaves v14.0.2 from source) and new host (`ipro_bt_host`, upstream
  Zephyr v4.4.0), configured as an LE Audio unicast **sink**. `bt` command.

The panel path uses no LVGL, no framebuffer and no DMA — row-at-a-time blocking
transfers out of one static buffer. The camera path needs PSRAM for the ISP
output frames.

## Build / flash / run

```bash
make -C apps/platform/ipro7ai_evk_demo
# or: ./build_freertos.sh ipro7ai_evk_demo build flash monitor

tools/ipro_iot_tool_lite/ipro_iot_tool_lite --chipname ipro7 \
    --port /dev/cu.usbserial-2120 --baudrate 921600 --flash-pin 0x02 \
    --firmware apps/platform/ipro7ai_evk_demo/build/build_out/ipro7ai_evk_demo_IPRO7.bin

python3 tools/serial_monitor.py /dev/cu.usbserial-2120
```

Console + shell are on the platform UART at 115200. The app initialises the
panel and draws colour bars on boot, so a correctly wired module shows first
light without typing anything.

## Build variants — AI and Bluetooth cannot share this SoC's OCRAM

`.config` is a copy of one of these files. Switch with:

```bash
cp config_bt    .config && make clean && make   # Bluetooth + LE Audio
cp config_ai    .config && make clean && make   # AI/NPU
cp config_video .config && make clean && make   # SD video kiosk, no radio
```

| | flash | OCRAM | Camera | AI | BT |
|---|---|---|---|---|---|
| `config_ai` | 255,536 B (24.4 %) | 45,632 B / 64 KB (69.6 %) | yes | **yes** | no |
| `config_bt` | 809,184 B (77.2 %) | 43,936 B / 48 KB (89.4 %) | yes | no | **yes** |
| `config_video` | 261,840 B (25.0 %) | 30,848 B / 256 KB (11.8 %) | yes | no | no |

`config_video` is the demo-loop build: it comes up, draws the panel, and plays
the clips off the SD card on repeat with no sound. Dropping both radios and the
NPU puts it on the linker's plain layout (`AI=n`, no EM reservation), which is
where the 256 KB OCRAM figure comes from — the other two are squeezed because
the NPU remap and the controller's Exchange Memory both eat into that region.

Muted playback still opens the DAC and still writes to it, one silent sample
for every real one. That is not waste: the DAC ring is what paces the frame
loop, so a player that skipped the audio would blit as fast as the SPI bus
allows and run the clip at several times its own frame rate. What mute changes
is the samples (zeros) and the class-D amplifier (left off, so there is no idle
hiss and no open/close pop). `vid mute off` restores sound at runtime, and
`vid clip <n>` pins autoplay to a single clip instead of walking the list.

The camera is in both — only the NPU is dropped. What forces the split is the
OCRAM region size, which the linker derives from the feature set
(`bsp/board/linker/configs/ipro7_flash.cmake`):

| Config | OCRAM region |
|---|---|
| AI + MM, `EM_SIZE` = 0 | 64 KB |
| AI + MM, `EM_SIZE` > 0 (i.e. with BLE) | **48 KB** |
| AI off | 256 KB − `EM_SIZE` |

The BLE controller and host need ~67 KB of OCRAM. With AI on that budget is
48 KB, so the two genuinely do not fit; with AI off it is 224 KB with room to
spare.

⚠ **A build that links is not proof the stack is in the image.** An earlier
attempt appeared to fit — everything on, OCRAM at 93 % — because nothing
referenced `ipro_ble_ctlr_controller_init()` and `--gc-sections` had quietly
dropped the entire controller. The tell was flash growing by 32 bytes after
adding a Bluetooth stack. Check that a symbol you expect is actually present
before believing a footprint.

## Display wiring

The module's 17-pin FPC, and how this app uses it:

| FPC | Signal | GPIO | Notes |
|----:|--------|------|-------|
| 1, 13, 17 | GND | — | |
| 2–6 | TP_SDA / TP_SCL / TP_RST / TP_INT / TP_VDD | — | capacitive touch, **not connected** |
| 7 | TE | — | **not connected** — no tearing-effect sync |
| 8 | RESET | **14** | active low |
| 9 | SDA | **16** | SPI0 MOSI, also the read-back line |
| 10 | SCL | **15** | SPI0 SCLK |
| 11 | CS | **10** | active low, software GPIO |
| 12 | D/C | **17** | 0 = command, 1 = data |
| 14 | VDD | — | 3.3 V (2.8–3.3 V) |
| 15, 16 | LEDA / LEDK | — | backlight rail — **not** a GPIO. 4 LEDs, Vf 3.0 V @ 80 mA |
| — | backlight enable | **12** | board-side enable only; there is no PWM dimming pin |

IPRO7 SPI0 pad roles follow `pin % 4`: 0 = MOSI, 1 = MISO, 2 = SS, 3 = SCLK.
So GPIO16/GPIO15 are the real SPI pads; GPIO10 and GPIO14 sit on SS positions
and GPIO17 on the MISO position, all used as plain GPIOs. Only SCLK and MOSI
are muxed to `GPIO_FUN_SPI`.

This is the **same header the SDK's existing `ili9341_spi.c` already drives**
(`apps/test/ipro_lvgl_demo`, `apps/multimedia/ipro_agora_*`), so the wiring is
proven on this board — only the panel is new.

## Datasheet findings

Facts established from the ST7789P3 datasheet (Version 0.0, 2021/11) and the
module spec (LCM-T2D4BP-084 V1.0, 2024-09-11), not from folklore:

**The module vendor supplies no init sequence.** Its section 8 reads, verbatim,
"Reference Resources ST7789P3 Datasheet". Both sequences here are derived from
the IC datasheet.

**Every serial AC timing in the datasheet is "TBD"** — TCSS, TSCYCW, TSDS, and
the reset widths TRW/TRT all included. There is no published maximum SCLK to
design against. The default 16 MHz was picked because it is the top of the
IPRO7 XCLK-sourced SPI range (`SPI_SetClock` switches XCLK → BCLK above
16 MHz). Reset timing instead uses the datasheet's spike-rejection rule: a RESX
pulse under 5 µs is *rejected*, over 9 µs *resets*; the driver holds 10 ms.

**⚠ There is no C4h (VDVS) command on the ST7789P3.** "VDVS" and "C4h" appear
zero times in all 276 pages. Nearly every ST7789**V** init sequence in the wild
has `0xC4, 0x20` right after `0xC3 VRHS` — pasting one in here writes an
undefined command. Neither sequence in this app contains C4h; don't "restore" it.

**The silicon defaults are already right for this glass**, which is why the
default init is minimal:

| Reg | Default | Meaning |
|-----|---------|---------|
| E4h GATECTRL | `27 00 10` | NL=27h → 320 gate lines, SCN=0 → **no CASET/RASET offset needed** |
| E7h SPI2EN | `00` | 2-data-lane mode off → single SDA works as shipped |
| B2h PORCTRL | `0C 0C 00 33 33` | |
| B7h GCTRL | `35` | VGH 13.26 V / VGL −10.43 V |
| BBh VCOMS | `20` | 0.9 V |
| C0h LCMCTRL | `2C` | |
| C3h VRHS | `0B` | |
| C6h FRCTRL2 | `0F` | 60 Hz |
| D0h PWCTRL1 | `A4 81` | AVDD 6.8 V / AVCL −4.4 V / VDS 2.3 V |
| E0h/E1h gamma | a real monotonic curve, **not** zeros | transcribed in `st7789p3_init_seq.h` |

Only three things genuinely *must* be written: `3Ah COLMOD = 55h` (the H/W-reset
default is 18 bit/pixel, not 16), `36h MADCTL`, and `21h INVON` (ST7789 drives
normally-black IPS glass inverted).

## Camera wiring

CN3 = FH34SRJ-24S-0.5SH, 24-pin 0.5 mm FPC. Full pinout in `src/cam_port.h`.

| Signal | GPIO | Notes |
|--------|------|-------|
| SCL | **36** | I2C0; silicon fixes the role by pin parity (even = SCL) |
| SDA | **37** | odd = SDA |
| D2–D9 → DVP_D0–D7 | **0–7** | sensor D0/D1 are **not connected** |
| PCLK | **38** | |
| VSYNC | **39** | |
| HSYNC | **40** | |
| MCLK | **41** | `GPIO_FUN_CAM_MISC` + `GLB_Set_CAM_CLK`, PLL48M/2 = 24 MHz |
| RESET | **35** | active low, **level-shifted** through Q3 |

### Three things this board does differently

**1. The camera bus is 1.8 V while the LCD bus is 3.3 V.** DOVDD is 1.8 V, so
the DVP and SCCB lines run at 1.8 V. On IPRO7 the VDDIO_1 bank covers GPIO0–9
and GPIO36–41 — exactly the camera pins — and the SoC brings that bank out on
its own supply pin (U5 pin 36, `VDDIO1`). The LCD pins (GPIO10–17) are in
VDDIO_2 at 3.3 V. **VDDIO1 must be fed 1.8 V** or the camera cannot work; that
split is the whole reason the pin has a separate net.

**2. RESET is level-shifted, not direct.** GPIO35 is in the VDDIO_3 bank
(GPIO20–35) at 3.3 V and cannot drive a 1.8 V input. Q3 (PJA7002H, gate on
DOVDD1V8, drain 3.3 V side, source 1.8 V side) plus R56 4.7 k to DOVDD1V8
translates it. R55 — the 3.3 V-side pull-up — is DNI, so the link is one-way:
the SoC drives GPIO35 push-pull and the sensor sees the same polarity at 1.8 V.

**3. There is no PWDN pin.** The SDK's stock EVB camera code (`uvc_isp.c`,
`mc_camera.c`) drives GPIO34 as sensor power/PWDN and GPIO35 as reset. Here
GPIO34 is **SD_DET**, so only reset exists — this app never touches GPIO34.

### 8 data lines out of the sensor's 10

Sensor D0/D1 are left unconnected and D2–D9 land on DVP_D0–D7. That is the
standard MSB-aligned 8-bit tap on a 10-bit output: the two least-significant
bits are discarded, so the bus carries the top 8 bits at full scale — no
shifting or scaling in firmware.

### One SDK label that is backwards (harmless)

`components/sensor/sccb.h` defines, for `CONFIG_EVB` + `CONFIG_IPRO7`,
`SCCB_SDA_PIN = GPIO36` and `SCCB_SCL_PIN = GPIO37`. On IPRO7 silicon the I2C0
role is fixed by pin parity — **even = SCL, odd = SDA** — so GPIO36 really is
SCL, as this schematic wires it. The header's two labels are simply swapped.
It changes nothing in practice: `SCCB_Init()` only uses them to build a
2-entry list and muxes both pins to `GPIO_FUN_I2C`, and nothing reads the
SDA/SCL distinction. Don't "fix" the shared header on account of this board.

## Audio wiring

| Signal | GPIO | Notes |
|--------|------|-------|
| SPK_DAC_OUT | **23** | DAC channel A → R27 470R + C20 3.3nF (fc ≈ 102 kHz) → C19 2.2µF → R26 4.7k → TPA2037D1 IN+ |
| SPK_EN | **22** | TPA2037D1 EN, **active HIGH**, via R24 1k / R25 10k divider |
| DMIC_CLK | **18** | even pin → PDM_CLK |
| DMIC_DAT | **19** | odd pin → PDM_DIN |

The amp runs off `PA_VDD` ← VBAT (not 3V3), outputs go through FB3/FB4 ferrites
and D4/D5 TVS to J1. The mic's SELECT pin is tied to GND, so it drives the
**left** PDM slot.

### ⚠ SPK_EN is active HIGH and the SDK assumes active LOW

R25 10k pulls the EN node down, so the amp is off until GPIO22 goes high. But
`components/audio/src/audio_output_dac.c` hardcodes the IPRO7 EVB's **LM4871**,
whose SHUTDOWN is active *low*: it writes the enable GPIO **LOW on
`audio_output_start()`** and **HIGH on stop**. On this board that is exactly
inverted — muted while playing, unmuted while idle. `audio_output_cfg_t` has a
`dac_spk_gpio` field but **no polarity field**, so the only fix is to re-assert
GPIO22 after every start and stop. `audio.c` does that in `dac_open()` /
`dac_close()`; don't remove it.

GPIO22 is also the SoC's DAC channel B pin, so using it as the amp enable makes
this board mono — the same trade-off the EVB makes.

### ⚠ The mic is on a 1.8 V rail but shares a VDDIO bank with the 3.3 V LCD

On IPRO7 the pad banks are **VDDIO_1 = GPIO0–9 + GPIO36–41**, **VDDIO_2 =
GPIO10–19**, **VDDIO_3 = GPIO20–35**. `DMIC_CLK`/`DMIC_DAT` are GPIO18/19 —
VDDIO_2, the *same bank* as the LCD on GPIO10–17, which needs 3.3 V logic.
`DMIC_1V8` comes from `1V8_SOC` through FB2.

So if VDDIO_2 sits at 3.3 V, the SoC clocks a 1.8 V-supplied microphone at
3.3 V and reads its 1.8 V data back against a 3.3 V input threshold. R29/R31
are both 0R, so nothing limits it. Two things worth checking on the board
before blaming firmware for a silent mic:

1. What VDDIO_2 is actually fed. The package brings out `VDDIO1` (U5 pin 36)
   separately — that is what lets the camera bank run at 1.8 V — but only
   `3V3_SOC` (pin 37) otherwise.
2. Whether the IM69D129 could simply be supplied from 3V3 instead. Its
   operating supply range is wider than 1.8 V, so moving FB2's input from
   `1V8_SOC` to `3V3_SOC` may be all that is needed — **confirm against the
   Infineon datasheet before changing anything.**

I have not been able to verify the bank assignment from a datasheet in this
repo, so treat the above as a lead, not a finding.

### Neither side runs at exactly 16 kHz

Bench-measured on this SoC, from the SDK's `ipro_pdm_mic_demo` work:

- **PDM capture: 15875 Hz** (2.032 MHz / 128), 0.8 % slow of nominal.
- **DAC: `rate = 1992465 / div`**, because the DAC root clock is really
  ~31.88 MHz, not the nominal 32 MHz. 16000 Hz needs `div = 124.53` —
  unreachable; `div = 125` gives 15938 Hz.

Nothing here needs sample-accurate sync, so both stay at nominal settings —
just don't expect a recording saved as "16 kHz" to play back at exactly pitch.

### 🔴 The AON block's pad-ownership bits are split across TWO registers

**This is what kept the microphone silent, and it is easy to look straight
past.** GPIO18/19 are AON pads. The always-on block's per-pad "I own this pin"
field lives in two places:

| Register | Field | Covers |
|---|---|---|
| `HBN_PAD_CTRL_0` [27:20] | `reg_en_aon_ctrl_gpio_7_0` | the low 8 AON pads |
| `HBN_PAD_CTRL_2` [11:10] | `reg_en_aon_ctrl_gpio_msb` | the **top 2** AON pads |

On this board, straight out of boot, they read:

```
HBN_PAD_CTRL_0 0x00000003  en_aon_ctrl=0x00     <- looks like "AON owns nothing"
HBN_PAD_CTRL_2 0x00000C00  en_aon_ctrl_msb=0b11 <- but the top two ARE claimed
                           ie_smt_msb=0b00      <- with their input buffers OFF
```

**Bench-confirmed 2026-08-28.** Clearing `en_aon_ctrl_msb` flipped GPIO19 from
"will not follow its own pull-up" to `pull-up 1 / pull-down 0`, and the mic
immediately appeared: **49 % ones density with 12 952 transitions over 20 000
samples**, which is a textbook idle sigma-delta stream. Either `0x000` or
`0x300` works — both clear bits [11:10]; `ie_smt_msb` is irrelevant once AON
control is off.

Check only `PAD_CTRL_0` and you conclude the AON block is not involved. It is.
A pad in that state reads 0 whatever happens and **will not even follow its own
internal pull-up** — which is exactly what `mic probe` measured on GPIO19,
while the GLB side was perfectly configured (`func=5` PDM, `ie=1`, `oe=0`).

`pdm_claim_pads()` in `audio.c` clears `en_aon_ctrl_msb`, sets `ie_smt_msb`,
and does the same for the low-8 field. `mic probe` runs the pull-up test both
before and after that call, so the two readings are the proof of who owned the
pins.

**A dead end worth recording:** GPIO18/19 are also the 32.768 kHz crystal pads,
and `HBN_Power_Off_Xtal_32K()` genuinely is incomplete — it clears
`PU_XTAL32K` and `PU_XTAL32K_BUF` but never sets `XTAL32K_HIZ_EN`, while
`HBN_Power_On_Xtal_32K()` explicitly clears that bit on the way in. That looked
like a perfect explanation. It was not the cause here: the register already
read `0x00030229` (HIZ_EN set, both PU bits clear) straight out of boot, so the
pads were long since released. `pdm_claim_pads()` still fixes it up for a build
that boots with the crystal running, but do not mistake it for the answer.

Any peripheral that wants GPIO18/19 on this SoC needs the AON handover; it is
not specific to PDM.

### ⚠ The PDM DMA buffers must be in non-cached OCRAM, not PSRAM

The AUADC DMA cannot be pointed at PSRAM. With `CONFIG_USE_PSRAM=y` the heap
can hand back PSRAM for any allocation, so the capture buffers are declared
statically instead:

```c
static int16_t ATTR_NOCACHE_NOINIT_RAM_SECTION
    __attribute__((aligned(32))) s_dma_raw[AUDIO_FRAME_SAMP * 2];
```

That lands them in the non-cached OCRAM alias (`__nocache_ram_start`
= `0x210198b0`, mirroring `__ocram_cacheable_end` = `0x110198b0`). Verify with
`nm` if capture ever goes quiet again — an address in the `0x1e……` range means
PSRAM and the mic will read nothing.

For the same reason this app does **not** use the `audio_capture` component,
which allocates its ping-pong buffers with `pvPortMallocCacheAligned()`. Two
further constraints made the raw `hal_auadc_*` path the right one:

- **`hal_auadc_pdm_init()` may be called only once per boot.** The SDK's PDM
  demo states that "reinit breaks DMA on subsequent calls", but
  `audio_capture_init()`/`deinit()` re-runs it every session. `audio.c`
  configures once and gates capture with `hal_auadc_start()`/`stop()`, which is
  why the DMA channel is deliberately never released.
- **The first ~5 frames after the clock starts are garbage** while the mic
  settles, and are discarded.

### Gain budget — silence is the expected default

A PDM MEMS mic sits near −26 dBFS at 94 dB SPL, so normal speech at 30 cm lands
around **−54 dBFS**. With every stage at unity that is inaudible, which reads
like a broken microphone but is arithmetic. **+18 dB of AUADC digital gain**
(`mic gain`) is the hardware maximum and the only stage applied before the
16-bit truncation, so it is the only one that improves resolution — take it
first, then make up the rest in software.

Measured on this board (2026-08-28), with the AUADC gain read back from the
hardware to confirm it stuck:

| | quiet room | normal speech |
|---|---|---|
| peak | −47.2 dBFS | **−35.9 dBFS** |
| rms | −60.7 dBFS | −53.9 dBFS |

−35.9 dBFS sits right on the SDK's own speech reference of −35.2 dBFS, so the
microphone performs exactly as expected — and it leaves ~36 dB of headroom.

**The SDK's +16 dB make-up default is wrong for this board and sounds quiet.**
That figure was matched to a recording peaking at −17.9 dBFS; here it throws
away most of the available range. `SPK_MAKEUP_GAIN_DB` is 30 dB, which keeps
about 6 dB of margin against a −36 dBFS peak.

Better still, `mic play auto` derives the gain from the take that was actually
recorded, holding back 6 dB for resampler overshoot — the per-sample peak is
not the peak of the reconstructed waveform. `mic stats` reports the remaining
headroom and, after a playback, how many samples clipped (`gain_apply()`
saturates rather than compressing, so clipping is deliberately audible).

⚠ Digital make-up does not improve SNR — it raises the noise floor with the
signal. If the peak barely moves when you speak, the problem is acoustic
(port, placement) and no amount of gain will fix it.

## Supply voltage

### ⚠ GPADC channel 18 is not the board's VBAT net

`ADC_CHANNEL_VABT_HALF` (note the SDK's typo) is an **internal** node.
`ADC_Vbat_Enable()` sets `GLB_GPADC_VBAT_EN` — bit 4 of
`GLB_GPADC_REG_CONFIG2` — which switches **the SoC's own supply** into the
converter through an on-die 2:1 divider. No GPIO is involved; channels 0–11
are the pin-routed ones.

On this 49-pin package the only supply pins brought out are `VDDIO1` (36),
`3V3_SOC` (37) and `VBUS` (35), so channel 18 is almost certainly reading
`3V3_SOC`. **It is not the schematic's `VBAT` net** — the one reaching
`PA_VDD` through R32 to power the class-D amplifier. Those two share a name
and nothing else, and there is no charger circuit on a pin to make the second
one a battery.

`vbat` says all of this in its own output and interprets the reading: steady
and near 3.3 V means a regulated supply; anything in Li-ion range above a
3.3 V regulator would mean the package does bring VBAT out separately, which
is worth confirming against the board before relying on it.

The divider is **gated**: `ADC_CTRL_VBAT_ON` before the read,
`ADC_CTRL_VBAT_OFF` after so it stops loading the rail. Working config:
vref 3.2 V, gain 1, clk/32, 16-bit with 256× hardware averaging,
single-ended, continuous. Eight samples, **trimmed mean** with the raw spread
reported — the class-D amplifier's switching current modulates whatever rail
it shares, so a wide spread is a hint to retry with `spk off`.

### Measuring the real battery rail

That needs an external divider into one of GPADC channels 0–11:

```
adc <channel 0..11> [ratio_x100]
```

`ratio_x100` scales the pin reading back up through the divider — two equal
resistors (1:2) is `200`. vref is 3.2 V, so keep the divided voltage below
that. GPADC channels map to a specific set of pins; check the pinmux
spreadsheet, since on this board most of the candidates are already taken by
the LCD, UART, speaker and JTAG.

## Two SDK fixes this app needed

**`CONFIG_NVDS_SIZE` never worked.** `generate_linker_script.cmake` documented
the override in a comment but tested `NVDS_SIZE` — no `CONFIG_` prefix — which
no `.config` can set, so setting it did nothing. Now implemented.

**`.nvds` was pinned to OCRAM to dodge a bug rather than fix it.** The comment
explained why: a PSRAM-placed NVDS overlapped the heap, because
`__heap_psram_start` was anchored on `_bss_psram_end`, which is emitted *before*
`.nvds` in section order. FreeRTOS then handed out NVDS memory as task stacks
and stamped 0xa5 canaries over it, and the controller's `nvds_walk_tag` read a
canary as a tag length and asserted in `nvds.c:473`. OCRAM "had plenty of
headroom" — true at 224 KB, false at the 48 KB an AI+MM+EM build gets.

The root fix is the anchor, not the region:

```ld
PROVIDE( __heap_psram_start = MAX(_bss_psram_end, __nvds_end) );
```

Now the heap cannot start inside `.nvds` whatever order the sections land in.
Opt in with `CONFIG_NVDS_IN_PSRAM=y`; the default is unchanged, so no existing
app is affected.

**And one in the host component:** `uuid_to_string()` is declared in
`zephyr/sys/uuid.h` but upstream's `lib/utils/uuid.c` was never imported, so
`bt_uuid_to_str()`'s 128-bit branch fails to link — which LE Audio reaches.
Since `upstream/` is read-only by project rule, the implementation was added to
the port layer as `port/uuid_port.c`.

## Bluetooth LE and LE Audio

New stack on both sides:

| | Component | Config |
|---|---|---|
| Controller | `ipro_ble_controller` — RivieraWaves v14.0.2, upstream kept read-only, H4 byte-stream HCI | `CONFIG_BT_CTLR_IPRO=y`, `CONFIG_IPRO_BLE_CTLR_FORCE_SOURCE_BUILD=1`, `LIB="link1"`, `FEATURE="bt54_iso2"` |
| Host | `ipro_bt_host` — upstream Zephyr v4.4.0 | `CONFIG_BT_HOST_IPRO_V44=y` |

Headers are `<zephyr/bluetooth/...>`, **not** the old `<bluetooth/...>` of
`blestack`. Mixing the two does not work.

LE Audio is set up as a **unicast sink** (the headset role): PACS publishes an
LC3 sink capability at 16/32/48 kHz with 7.5 and 10 ms frames, and a BAP
unicast server with one sink ASE lets a phone configure and start a stream.

```
bt init            enable controller + host, register PACS/ASCS
bt adv on          advertise with the ASCS/PACS UUIDs
bt status          link state, address, connection interval
bt audio           stream state, RX counters and decode statistics
```

### Playback path

Received frames are decoded and played, in `src/bt_audio_out.c`:

```
ISO SDU -> stream_recv() -> 8-frame ring -> bt_audio task
                                              -> lc3_decode() -> audio_output_write() -> DAC -> TPA2037D1
```

`stream_recv()` runs on the BT RX thread, which must not block, so it only
copies the payload and signals the decode task. The codec parameters are read
back from `stream->codec_cfg` when the stream starts rather than assumed — the
phone may pick any rate PACS advertised.

### The DAC does not run at 32 kHz — and nothing above it knows

`hal_dac.c` programs the GPDAC clock with divider 62 and then divides by 16 for
the "32 kHz" setting. Bench measurement of this DAC (two cross-checked points,
div 124 → 16070 Hz and div 125 → 15938 Hz, consistent to 0.02%) gives

```
rate = 1992465 / div_written        →  1992465 / 62 = 32136 Hz
```

That is **+4266 ppm fast**, and every layer above it — including
`audio_output_dac.c`'s `DAC_RATE` constant — assumes it got the 32000 it asked
for. Feeding 32000 samples per second into a sink that drains 32136 runs the
ring dry by a whole 10 ms frame every ~2.3 s, and because the DMA free-runs
cyclically with no underrun detection it replays a stale segment rather than
going quiet. That is the intermittent glitching, and **deeper buffers do not
fix it** — the error accumulates without bound, so a bigger ring only changes
how often you hear it.

So the decoded PCM is resampled 32000 → the true rate before it reaches the
DAC, with a phase accumulator that persists across frames. A host-side test of
that resampler leaves ≤ 6.3 ppm residual (from 4266) with no discontinuity at
frame boundaries.

Two cautions here:

- **Do not compute the rate from `Clock_Peripheral_Clock_Get()`.** It applies
  `clock / (div + 1)`, which disagrees with the measurement — and disagrees in
  *sign*, giving 31626 Hz where the hardware does 32136. Correcting with the
  wrong sign makes the drift 1.5× worse, not better.
- The constant is only a starting point. A stream that runs a few seconds
  measures the real rate (`audio_output_write()` blocks, so samples handed over
  per unit time *is* the drain rate) and later streams use the measured value.
  `bt audio` prints both; `bt dacrate <hz>` pins it by hand.

### 48 kHz needs a real filter, not interpolation

The first thing a phone actually picked was **48 kHz**, and the resampler then
had to take 48000 down to 32136 — a 1.494× decimation done with plain linear
interpolation, which is not an anti-alias filter in any useful sense. Content
between 16068 Hz (the output Nyquist) and 24 kHz folded straight back into the
audible band, and it sounded rough.

There is a 104-tap Hamming windowed-sinc low-pass in front of the decimation
now, engaged only when the stream rate is above the DAC rate. Measured
response: flat to 13 kHz, −6 dB at 14.5 kHz, **−71 dB at 16068 Hz**.

It runs through NMSIS-DSP's `riscv_fir_q15`, which the IPRO7 BSP already links
(`bsp/drivers/ipro7_soc/CMakeLists.txt` adds `libnmsis_dsp_*_xxldsp.a`
unconditionally). q15 rather than f32 is deliberate: the q15 path uses the DSP
extension's integer SIMD and never touches `f0`–`f31`, so it stays clear of the
missing FPU context save.

A stream at or below 32 kHz skips the filter entirely — liblc3 upsamples to the
DAC rate itself, properly, and the rate matcher is then running at ~1:1 where
linear interpolation is appropriate. That is the better path on this hardware;
48 kHz is supported, not preferred.

### Conceal only what is actually broken

`stream_recv()` first required `BT_ISO_FLAGS_VALID` to be set before accepting
a frame. The reference headset does not test the flags at all — it conceals
only on a NULL or zero-length payload. Requiring VALID throws away every good
frame on any path that does not populate that bit, and concealing a frame that
did arrive is worse than the packet loss it is meant to hide.

The test is now for `BT_ISO_FLAGS_LOST | BT_ISO_FLAGS_ERROR` being explicitly
set. `bt audio` reports concealed frames and, separately, how many of them the
controller actually flagged — if those two numbers diverge, the flags are the
problem and not the link.

Two more details worth keeping:

- **liblc3 upsamples during decode.** `lc3_setup_decoder()`'s `sr_pcm_hz`
  argument sets the PCM output rate independently of the stream rate, so a
  16 kHz stream is decoded straight to 32 kHz — the DAC's native rate — and no
  resampler of ours sits in the path. Only a stream already above 32 kHz is
  decoded natively and resampled by the `audio_output` component.
- **Lost SDUs are concealed, not skipped.** An empty SDU, or one the controller
  flagged as invalid, is pushed as a zero-length frame; `lc3_decode(NULL, 0)`
  runs packet-loss concealment. Dropping it instead leaves a hole that becomes
  an audible click.

The DAC ring is 4 × 64 ms. `audio.c`'s own 4 × 20 ms was sized for locally
generated frames that arrive on time; an ISO stream arrives in bursts and needs
the slack. The LC3 ring in front of the decoder holds 32 frames — enough to
cover one whole DAC segment, which is how long the decode task can sit blocked
inside `audio_output_write()` while frames keep arriving — and lives on the
PSRAM heap, because OCRAM is at 93%.

### ⚠ Playback cannot run alongside the mic or the camera

The IPRO7 FreeRTOS port does **not** save the float registers `f0`–`f31` across
a context switch, yet everything is built hard-float (`ilp32f`). liblc3 is
float-heavy, and so are `audio.c`'s level metering and the camera's 3A — two
of them running concurrently corrupt each other's registers silently, with no
fault and no log line, just wrong numbers.

`bt_audio_out_start()` therefore calls `audio_stop()` first. Do not start a
camera preview while a stream is playing.

## Preview pipeline

```
SC101IOT 1280x720 DVP -> ISP -> scaler ch1 (ARGB) -> PSRAM
                                 -> CPU packs 8-8-8 to 5-6-5 -> SPI -> panel
```

Channel 1 is used rather than channel 0 because the ISP's hardware YUV-to-RGB
block sits on that path, so the CPU only repacks bit fields instead of doing a
full YUV422 colour conversion per pixel.

**⚠ The ch1 ARGB channel order was not stable until the pipeline start was
made idempotent.** Three bench measurements, two of them misleading:

| Condition | byte 0 |
|---|---|
| IQ table silently missing (before the `-usc101iot_img_param` fix) — ISP on the hardcoded `hal_isp.c` matrix | RED |
| IQ table loaded — **first** start after boot | swapped |
| IQ table loaded — **second** start, same binary and settings | correct |

Same image, different answer depending on how many times the pipeline had been
started. Only the second start was preceded by `cam_stop()`'s MM block reset,
which is what made it differ. So the order is not a property of the ISP at all
— it depends on the state the YUV-to-RGB block is in when `isp_init()` and the
IQ colour matrix are applied.

`cam_isp_up()` now resets `DVP_AS`, `DVP2AXI_C` and `ISP` **before** it
configures anything, so the first start and the fifth are identical, and byte 0
is RED. `cam order rgb|bgr` still switches it live and `cam pixels` reads the
answer off a red scene.

Note that `apps/multimedia/ipro7_uvc_cam/uvc_ai.c` reads the same buffer with
the **opposite** convention (`CLS_ARGB_R=2, CLS_ARGB_G=1, CLS_ARGB_B=0`) — and
carries its own comment saying to flip those constants if results look wrong.
Treat it as an unverified guess, not as evidence. `cam order rgb|bgr` switches
this live and `cam pixels` prints the per-byte values against a red scene, so
neither needs a rebuild to settle.

The sensor is 16:9 and the panel in landscape is 4:3, so `cam fit` picks:

- `box` (default) — the whole frame scaled 4× to 320×180, letterboxed with
  30 black rows above and below.
- `crop` — the centre 960×720 scaled 3× to 320×240, filling the screen.

Both are clean integer ratios, so neither introduces scaler distortion.

**⚠ The multimedia AHB clock must be un-gated before ANY ISP register write.**
`GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM)` — every working camera app in the SDK
does this first (`uvc_isp.c:883`, `mc_camera.c:952`, `pircam_isp.c:725`).
Without it the failure is silent and very misleading: SCCB is on I2C0, a
different clock domain, so the sensor still probes and resets perfectly, and
`isp_init()` still returns success — but every write into the MM domain is
dropped, nothing streams, and no completion interrupt ever fires. The symptom
is `cam status` sitting at **0 frames with 0 errors**, which reads like a
wiring fault and sends you to the connector instead of the clock tree.
`cam_isp_up()` un-gates it; `cam_stop()` gates it back off.

**AE/AWB depends on the SEOF interrupt.** The ISP firmware registers its
statistics handler through `ipro_isp_stats_ready_config()`, which the
*application* must define, and it has to be called once per start-of-frame from
the SEOF0 ISR. Without that hook 3A never ticks and every frame comes out at
the sensor's power-on exposure — a plausible-looking but permanently wrong
image. `cam.c` implements both.

## First light

```
lcd status              # what the driver thinks is configured
lcd id                  # read the chip back over SDA — is it alive at all?
lcd bars                # colour bars
lcd rgbtest             # R/G/B/white/black bands

cam probe               # SCCB + MCLK + reset, then look for the sensor
cam start               # live preview
cam status              # geometry, frame counters, ISP error counters

spk tone 1000 2         # 1 kHz sine — proves DAC + amp + speaker
mic rec 3               # record 3 s
mic stats               # peak/rms dBFS — proves the mic without feedback
mic play                # play it back
mic scope               # live waveform on the panel
```

`lcd id` is the highest-value first command: it bit-bangs the datasheet's
"Interface-I" read (command byte out, **one** dummy clock, then the chip drives
the same SDA line) and needs nothing to be visually correct. `RDDPM (0Ah)` bit 4
set means sleep-out succeeded and bit 2 means display-on — proof the command
path works even if the screen is blank.

## Troubleshooting

| Symptom | Try |
|---------|-----|
| Screen completely dark | Backlight rail first — LEDA/LEDK need ~80 mA at 3.0 V from a boost/CC driver, GPIO12 is only an enable. Then `lcd id`. |
| `lcd id` all `00` or all `FF` | The chip is not driving SDA back. Check nothing else holds GPIO16; a unidirectional level shifter on SDA also breaks reads while writes still work. |
| Picture is a photographic negative | `lcd inv off` (this glass disagrees with the IPS default). |
| Red and blue swapped | `lcd bgr on` — sets MADCTL bit 3. |
| Corrupt / smeared / doubled columns | `lcd clk 8000000`, then `lcd clk 4000000`. If a slower clock fixes it, it is signal integrity, not the sequence. |
| Init lands but pixels don't | `lcd framing byte` — falls back to one CS window per byte, the framing the proven `ili9341_spi.c` uses. |
| Washed out, dim, or flickery | `lcd init tuned` — adds the conventional power/gamma block (VCOMS 0.725 V, VRHS ~4.45 V, AVCL −4.8 V). `lcd gamma default` restores the silicon curve. |
| Image offset or edges missing | `lcd frame` then `lcd ruler`. All four corner blocks should be visible; this panel needs no offset, so a missing corner means a window bug, not a panel quirk. |

`lcd pin <gpio> [0|1]` drives or reads any pin for multimeter checks — it
releases the SPI mux, so re-run `lcd init` afterwards.

### Camera

| Symptom | Try |
|---------|-----|
| `cam probe` finds nothing | The SCCB scan output shows every address that ACKed. SC101IOT answers at **0x68**. No ACK at all usually means the 1.8 V VDDIO1 rail or the sensor's own rails are missing, or MCLK is not running — the sensor needs its clock before it will talk. |
| Probe works, `cam status` shows **0 frames and 0 errors** | The multimedia AHB clock is gated — see the note below. Bench-confirmed 2026-08-28. |
| Probe works, frames stay 0 but error counters climb | PCLK/VSYNC/HSYNC. Check GPIO38/39/40 reach the connector and that VDDIO1 is 1.8 V, then `cam rst 0` / `cam rst 1` to re-pulse the sensor. |
| Frames arrive but nothing is drawn | `lcd init` has to have run first — the preview blits through the same panel driver. |
| Red and blue swapped | `cam order bgr` (or back to `rgb`). Bench-confirmed default is `rgb`; `cam pixels` against a red scene proves which is right. |
| Image is a photographic negative | That is the panel, not the camera — `lcd inv off`. |
| Image is black or blown out and never settles | AE is not ticking — that is the SEOF hook, not the sensor. |
| High `dropped` count in `cam status` | The panel blit is slower than the ISP. `cam fps 5`, or raise `lcd clk`. |
| `fifo`/`vsync`/`hsync` errors climbing | DVP timing mismatch: PCLK too fast for the ISP, or the sync polarities are wrong for this module. |

### Audio

| Symptom | Try |
|---------|-----|
| `spk tone` visible on a scope at GPIO23 but no sound | The amplifier enable. `spk on` forces GPIO22 high; if that fixes it, something reset the polarity workaround. |
| Recording plays back far too quiet | Check `mic status` gains are +18 / +16 dB. Unity everywhere is ~34 dB short — see the gain budget above. |
| `mic loop` howls | Acoustic feedback, not excess gain. An input peak of exactly **32768** is the signature (int16 only reaches that on the negative rail). Use `mic rec` + `mic stats` with the speaker silent to measure honestly. |
| Mic silent from a cold boot | Almost certainly the XTAL32K pads — see above. `mic probe` bit-bangs the pins with the AUADC out of the picture and says outright whether the mic is driving DATA. |
| `mic rec` appears to do nothing | Run `mic status` and read the `auadc:` line. **0 frames from the DMA ISR** means nothing is clocking the mic at all — that is upstream of every gain setting. Non-zero means the hardware runs and the fault is above it. Capture loops now give up after 2 s and print which case it is. |
| Mic reads all zeros or all ones | Check the 1.8 V vs 3.3 V bank question above before touching firmware. |
| `spk dc` sweep reboots the board | Expected if the amplifier is on — a full-scale step is ~1.5 V through C19 and the class-D turns it into a rail-to-rail transient. `audio_dac_dc()` leaves the amp off deliberately; measure before C19. |

**Concurrency note.** The preview task, `mic scope` and the `lcd` drawing
commands share one SPI bus. The driver serialises them with a recursive mutex
(`st7789p3_bus_lock()`), so a pattern drawn while the preview runs is atomic —
but the next camera frame will simply paint over it. Run `cam stop` before
using the `lcd` test patterns.

## Caveats

- **The tuned sequence's gamma table is generic.** It is the widely used
  ST7789 module curve, *not* one characterised for this glass — the module
  vendor supplies none. If colours look wrong under `lcd init tuned`, the
  minimal sequence is the honest baseline.
- **No TE line**, so full-frame writes can shear against the panel scan. Wire
  FPC pin 7 to a GPIO and use TEON (35h) if that ever matters.
- **No touch.** FPC pins 2–6 carry a capacitive controller the module spec
  neither names nor documents.
- **No dimming.** The module has no LEDPWM pin; brightness is a property of the
  board's LED driver.
- Command-Table-2 registers are not readable unless SPIRD (E7h bit 0) is set.
  Command-Table-1 reads (04h, 09h, 0Ah, DAh–DCh) work as shipped.

## Files

```
main.c                       UART shell + auto-init + colour bars on boot
src/st7789p3_port.h          panel pin map, SPI settings, geometry, FPC pinout
src/st7789p3_init_seq.h      the two init sequences + datasheet default gamma
src/st7789p3.{c,h}           panel driver: bus, reset, init, MADCTL, blit,
                             bit-bang read, bus mutex
src/st7789p3_patterns.{c,h}  bars, rgbtest, checker, gradient, gray, frame, ruler
src/st7789p3_cli.c           the `lcd` shell command
src/cam_port.h               camera pin map, CN3 FPC pinout, the board's quirks
src/cam.{c,h}                SCCB probe, ISP pipeline, ARGB->RGB565 preview task
src/cam_cli.c                the `cam` shell command
src/audio_port.h             speaker/mic pin map, amp polarity, rates, gains
src/audio.{c,h}              DAC output, PDM capture, tone/record/loop/scope
src/audio_cli.c              the `spk` and `mic` shell commands
src/vbat.{c,h}               SoC supply via the internal VBAT/2 ADC tap, plus
                             a generic GPADC reader (`vbat` and `adc`)
src/sdcard.{c,h}             micro-SD + FatFS (`sd`)
src/bt.{c,h}                 BLE host bring-up + LE Audio unicast sink
src/bt_audio_out.{c,h}       LC3 decode task -> DAC playback
src/bt_cli.c                 the `bt` shell command
```

Audio needs `CONFIG_AUDIO_OUTPUT=y`, `CONFIG_AUDIO_CAPTURE=y` and
**`CONFIG_DMA_CHANNEL_ALLOCATOR=y`**. The last one is not optional for
`mic loop`: the legacy `dma_init()` does a controller-wide `DMA_Disable()` on
every call, so bringing up the DAC channel aborts the in-flight AUADC
transfer — `hal_dma.c` calls this out in its own comment as "the bug that
blocks duplex audio". Different channel numbers do not save you; the disable is
controller-level.

Camera support needs `CONFIG_MM_SUPPORT=y`, `CONFIG_COMPONENTS_SENSOR_ENABLE=y`
and `CONFIG_USE_PSRAM=y` in `.config`. The sensor descriptor is registered into
the `.camera_desc` linker section by `components/sensor/sc101iot.c`; the stock
IPRO7 linker script already provides `cam_desc_start`/`cam_desc_end`, so no
custom `.ld` is needed.

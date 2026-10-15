# IPRO7AI EVK sub-board

Board support for the IPRO7AI EVK, schematic **IPRO7AI_MODULE_SUB_0812A**
(2026-08-12), the revision the boards were produced from. J6 was checked on a
V1 board on 2026-10-01.

The support was first written from the **IPRO7AI_MODULE_SUB_0805** draft
(IPRO7 module, POC, 2026-08-05; six sheets: `01_POWER`, `02_MCU_MODULE`,
`03_SPK_SD_CARD`, `04_LCM_CAMERA`, `05_BATTERY_CHARGER`, `06_HEADERS`). The
draft differs on J6, on Q1 (draft DMG3401LSN, built MSYL3401A-PA), on Q3
(draft PJA7002H, built MSL2310-N) and on the camera FPC designator (draft CN3,
built CN5).

Select with `CONFIG_IPRO7AI_EVK=y` (implies `CONFIG_USE_PSRAM`). Pin map and
init macros live in `ipro7ai_evk_board_config.h`.

## Peripherals

| Block | Part | Pins |
|---|---|---|
| Display | HEM LCM-T2D4BP-084, ST7789P3, 2.4" IPS 240×320, 4-wire SPI | CS 10, BL 12, RST 14, SCLK 15, MOSI 16, DC 17 |
| Camera | SmartSens SC101IOT, 1280×720 DVP, SCCB 0x68 | D0–D7 = GPIO0–7, SCL 36, SDA 37, PCLK 38, VSYNC 39, HSYNC 40, MCLK 41, RST 35 |
| Speaker | on-chip DAC → TPA2037D1 mono class-D → J1 | DAC 23, EN 22 |
| Microphone | Infineon IM69D129FV01 PDM | CLK 18, DAT 19 |
| Storage | micro-SD on the SDH controller | PWR_EN 27, D1 28, D0 29, CLK 30, CMD 31, D3 32, D2 33, DET 34 |
| Console | UART0 | TX 20, RX 21 |
| JTAG | | TMS 24, TDI 25, TCK 26, TDO 11 |
| Spare | header J3 only | 13 |

## Power tree

```
USB-C CN1 → KTS1671 OVP (U1, OVLO 5.49 V) → VBUS
VBUS      → RT9527GQW  (U7) Li-ion charger, Ichg 400 mA → VBAT → CN4
VBAT      → RT6158AWSC (U2) buck-boost 2 A              → 3V3_SOC
3V3_SOC   → RT9080N-08GJ5 (U3) → 2V8_CAM   (camera AVDD; VDDIO1 on V1, by J6 wire)
3V3_SOC   → RT9080N-08GJ5 (U4) → 1V8_SOC   (camera DOVDD/DVDD via FB6, DMIC)
VBAT      → R32 0R → PA_VDD  (the amplifier runs off the cell)
```

## Wire J6 before powering up

J6 is a 2×5 header. As measured on a V1 board (2026-10-01):

| Pin | Net | On a V1 board |
|---|---|---|
| 1, 2 | 1V8_SOC | 0 V — not connected |
| 3 | VDDIO1 | supply **input** of the SoC's VDDIO_1 pad bank |
| 4, 5 | 3V3_SOC | 0 V — not connected |
| 7 | 2V8_CAM | 2.8 V |
| 9 | VBUS | 5 V |
| 6, 8, 10 | GND | (per schematic) |

**The only usable VDDIO1 supply on V1 is 2.8 V: connect pin 3 to pin 7 with a
jumper wire.** They are not adjacent, so a jumper cap does not fit. The old
settings — 1.8 V = short PIN1/PIN3, 3.3 V = short PIN3/PIN5 — came from the
0805 draft and do nothing on a V1 board.

VDDIO1 feeds the VDDIO_1 pad bank (GPIO0–9, GPIO36–41) — exactly the camera's
DVP and SCCB pins — and also the in-package Flash (SF_CLK/CS/IO0–3 are in that
bank). The Flash sets the minimum, about 2.7 V (IPRO7 datasheet V1.5, note 101),
so 1.8 V on VDDIO1 is not an option on IPRO7AI at all, and with VDDIO1
unpowered at reset the boot ROM cannot read the Flash and enters download mode
(below). The LCD is in a different bank at 3.3 V.

**V1 limitation: the camera is powered below its spec.** The SC101IOT
datasheet (V0.6) specifies DOVDD 2.7–2.9 V (typ. 2.8 V) and inputs up to
DOVDD + 0.3 V, but V1 feeds the sensor's DOVDD and DVDD from the fixed 1V8_SOC
rail (via FB6). The SoC side of the DVP/SCCB pins runs at VDDIO1 = 2.8 V, the
voltage the sensor should have, so it drives a sensor powered at 1.8 V. It
works on the bench; it is outside the sensor's datasheet. The fix belongs on
the board: DOVDD from 2V8_CAM.

### Entering download mode

BOOT is no longer on J6 (in 0812A pins 6/8/10 are all GND). Download mode now
comes from VDDIO1: with it unpowered at reset the boot ROM cannot read the
in-package Flash and falls back to download mode; with it powered the chip
boots normally. On a V1 board (verified on hardware):

1. Move the wire on J6 pin 3 from pin 7 to pin 1 (pins 1 and 3 are adjacent,
   so a jumper cap works too). Pin 1 is dead on V1, so VDDIO1 is unpowered;
   simply pulling the wire off pin 3 does the same.
2. Press RST (SW1, the only button).
3. Move the wire back to pin 3 – pin 7: the Flash needs VDDIO1 to be written.
4. Run the flash tool.

This relies on pin 1 being unconnected; on a board where J6 pin 1 really
carries 1V8_SOC, VDDIO1 would sit at 1.8 V, below the Flash's minimum, and the
result is not defined.
With firmware running, `reboot_dl` in the shell gets to download mode
without touching J6 (the EVK apps' `flash.sh` scripts do that).

The board also has test pads TP5/TP6/TP7 (TP6 = GPIO41, TP5 → R66 → GND,
TP7 → R67 → VDDIO1), but they are not needed for this.

**GPIO41 is still also the camera MCLK.** Once the application runs, the
CAM_MISC function drives it as DVP_REF_CLK.

## Things that will catch you out

**Firmware cannot see the battery.** The charger's PGOOD and CHG outputs drive
LEDs D14/D15 — neither reaches a GPIO, so charge state is not readable. And
VBAT appears on sheets 01, 03 and 05 only (buck-boost input, amplifier supply,
charger output); it never reaches an ADC pin and no divider is fitted. GPADC
channel 18 (`ADC_CHANNEL_VABT_HALF`) is an **on-die tap of the SoC's own
supply**, which here is 3V3_SOC out of the buck-boost — not the cell. Reading
the actual battery needs a divider added into one of GPADC channels 0–11.
`BOARD_HAS_BATTERY_SENSE` and `BOARD_HAS_CHARGE_STATUS` are both 0 to say so.

**The speaker enable is ACTIVE HIGH** (TPA2037D1 EN, with R25 pulling down),
while `components/audio/src/audio_output_dac.c` hardcodes the EVB's LM4871 and
writes the enable GPIO LOW on `audio_output_start()` and HIGH on stop — exactly
inverted. `audio_output_cfg_t` has a `dac_spk_gpio` field but no polarity
field, so re-assert the pin after every start and stop. GPIO22 is also DAC
channel B, so using it as the enable makes the board mono.

**GPIO18/19 (the PDM mic) are AON pads,** and the always-on block's per-pad
ownership bits are split across two registers: `HBN_PAD_CTRL_0[27:20]` for the
low 8 pads and `HBN_PAD_CTRL_2[11:10]` for the top 2. This board boots with
`PAD_CTRL_2 = 0xC00` — the top two claimed, input buffers off. Such a pad reads
0 forever and will not follow its own internal pull-up. Clear
`en_aon_ctrl_msb` before muxing PDM. Checking only `PAD_CTRL_0` gives the
opposite (wrong) answer.

**MM is not un-gated at board init.** `BOARD_CLOCK_GATING_INIT()` deliberately
leaves `GLB_AHB_CLOCK_MM` gated: it is a large block and most apps do not use
the camera. A camera pipeline that forgets
`GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM)` gets zero frames and zero errors,
which reads like a wiring fault.

**SD_PWR_EN is active low — by schematic, not yet by bench test.** Q1 is an
MSYL3401A-PA P-channel high-side switch. In 0812A R46 (2 kΩ) pulls its gate up
to 3V3_SOC and GPIO27 drives it through R50 (100 Ω), so the card is unpowered
until GPIO27 drives low; that topology confirms `BOARD_SD_PWR_ON_LEVEL` 0. The
reverse test on hardware (GPIO27 = 1 cuts the card) has still not been done.

## Reference app

`apps/platform/ipro7ai_evk_demo` exercises the display, camera, speaker and
microphone on this board and documents each bring-up in its own README.

# IPRO7AI EVK sub-board

Board support for the IPRO7AI EVK, schematic **IPRO7AI_MODULE_SUB_0805**
(IPRO7 module, POC, 2026-08-05). Six sheets: `01_POWER`, `02_MCU_MODULE`,
`03_SPK_SD_CARD`, `04_LCM_CAMERA`, `05_BATTERY_CHARGER`, `06_HEADERS`.

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
3V3_SOC   → RT9080N-08GJ5 (U3) → 2V8_CAM   (camera AVDD)
3V3_SOC   → RT9080N-08GJ5 (U4) → 1V8_SOC   (camera DOVDD/DVDD, DMIC, VDDIO1)
VBAT      → R32 0R → PA_VDD  (the amplifier runs off the cell)
```

## Set the J6 jumpers before powering up

```
VDDIO1 supply:  1.8 V = short PIN1/PIN3     <- required for the camera
                3.3 V = short PIN3/PIN5
BOOT (GPIO41):  pull LOW  = PIN6/PIN8       <- normal boot
                pull HIGH = PIN8/PIN10      (ISP download)
```

VDDIO1 supplies the VDDIO_1 pad bank (GPIO0–9, GPIO36–41) — exactly the
camera's DVP and SCCB pins — so it must be 1.8 V to match the sensor's DOVDD.
The LCD is in a different bank at 3.3 V.

**GPIO41 is both the BOOT strap and the camera MCLK.** J6 sets it at reset;
the CAM_MISC function drives it afterwards. Leaving the ISP jumper fitted and
then wondering why the sensor will not clock is an easy afternoon to lose.

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

**SD_PWR_EN polarity is unconfirmed.** `BOARD_SD_PWR_ON_LEVEL` is 0 on the
reasoning that a DMG3401LSN P-channel high-side switch conducts with its gate
low, but the schematic does not settle the drive polarity. Verify on hardware.

## Reference app

`apps/platform/ipro7ai_evk_demo` exercises the display, camera, speaker and
microphone on this board and documents each bring-up in its own README.

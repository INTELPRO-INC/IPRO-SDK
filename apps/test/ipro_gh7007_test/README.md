# GH7007-01 SPI LCD bring-up test (IPRO7 Arduino board)

Bench bring-up for a **5″ 1024×600** panel driven by a **GoHi GH7007-01**
single-chip LCD driver over **4-wire SPI**. The GH7007 has an internal GRAM +
scaler: firmware writes a small **320×240 RGB565** image over SPI and the chip
upscales it to the native **1024×600** panel.

This app applies the vendor power-on / scaler / GIP / gamma init sequence and
then paints RGB565 test patterns — no LVGL yet (added in a later pass once the
raw panel is verified).

## Wiring

> ⚠ **The current bench harness attaches the five signal wires in REVERSED
> FPC order** (pins were counted from the wrong end when the wires were glued
> — root cause of the entire 2026-07 bring-up saga). The module pinout itself
> is the standard GND/RS/CS/SCL/SDA/RESET/VCC/GND/LEDA/LEDK; the table below
> is the TRUE function-to-GPIO map for this harness (matches `gh7007_port.h`):

| Module signal | IPRO7 GPIO | Note                                        |
|---------------|-----------:|---------------------------------------------|
| RESET         | **13**     | active low (bench-proven: low kills panel)  |
| SDA           | **14**     | non-MOSI pad → bus is **bit-banged**        |
| SCL           | **15**     | center wire — unaffected by the mirror      |
| CS            | **16**     | software GPIO, active low                   |
| DC (RS)       | **17**     | 0 = command, 1 = data                       |
| VCC / GND     | —          | 3.3 V / GND (power pins are wired correctly)|
| LEDA/LEDK     | —          | backlight LED string on the board's boost/constant-current rail — not a GPIO |

Because SDA sits on GPIO14 (an SS-position pad, not HW-MOSI-capable), the bus
runs **bit-banged 4-wire** by default (`GH_BUS_BITBANG=1` in `gh7007_port.h`).
To restore ~7 MHz hardware SPI, physically swap the G14/G16 wires at the SoC
end and set `GH_BUS_BITBANG=0`.

GPIO12 (`GH_PIN_BL`) is a board-side backlight-enable output driven high at
init (for rails with an EN input); it is not a module wire and is harmless if
left unconnected.

This is a **write-only** link — the module exposes no read-back (MISO) line, so
there is no chip-ID read. Only MOSI + SCLK are muxed to SPI0.

> This TM050DYGP23 module's 10-pin FPC has **no CMD_SEL / SPI_CSB straps** —
> those exist on the bare GH7007 pinout only. Do not move CS/SCLK to GPIO18/19:
> with the SPI0 pad group active those pins cannot be driven as plain GPIOs.

## SPI parameters
- **Mode 3 (CPOL=1, CPHA=1)** for the GH7007 (hardcoded in `gh7007.c`, matches
  the vendor driver); the ILI9341 fallback branch uses Mode 0. MSB-first,
  4 MHz default (GH7007 max write ≈ 10 MHz, change with `lcd clk <hz>`).
- Command = DC low; parameters/pixels = DC high. Pixel format RGB565
  (COLMOD `0x3A`=`0x75`), 2 bytes/pixel, sent MSB byte first.

## Build / flash

```bash
make -C apps/test/ipro_gh7007_test                 # build
./build_freertos.sh ipro_gh7007_test build flash    # build + flash
```

Console + shell appear on the USB CDC port (enumerates as "IPRO GH7007 LCD
Test"); no UART adapter needed.

## Shell commands

```
lcd init                 reset + vendor init + clear black
lcd fill <color|0xRGB>   solid fill (black/white/red/green/blue/yellow/
                         cyan/magenta/gray, or 0xRRGGBB, or 0xRGB565)
lcd bars                 8 vertical colour bars
lcd checker [sq]         checkerboard (square px, default 16)
lcd grad                 R/G/B/gray gradient bands
lcd frame [color]        border + crosshair (verify scaler reaches all edges)
lcd box x0 y0 x1 y1 c    fill a rectangle (window test)
lcd dims <w> <h>         override GRAM input plane (default 320x240)
lcd cmd <hex> [p..]      raw command + parameter bytes (debug)
lcd pin <gpio> [0|1]     drive/read a pin for multimeter wire checks
```

### Typical bring-up flow
1. `lcd init` — backlight should light; screen clears to black.
2. `lcd fill red` / `green` / `blue` — confirm a clean solid colour fills the
   whole panel. If red shows as **blue**, the R/B order is swapped — flip
   MADCTL `BGR` (try `lcd cmd 0x36 0x0A`) or byte order.
3. `lcd bars`, `lcd frame` — confirm geometry and that the scaler reaches every
   edge. If the image doesn't fill the panel, adjust the input plane with
   `lcd dims` (the scaler input is assumed 320×240).

## Files
- `src/gh7007_port.h` — pin map, SPI config, panel geometry.
- `src/gh7007.{c,h}` — SPI/GPIO, reset, init runner, window, blit, fills.
- `src/gh7007_init_seq.h` — vendor init sequence (verbatim transcription).
- `src/gh7007_patterns.{c,h}` — test patterns.
- `src/gh7007_cli.c` — `lcd` shell commands.

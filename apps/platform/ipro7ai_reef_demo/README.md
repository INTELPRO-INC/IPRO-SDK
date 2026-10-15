# ipro7ai_reef_demo — IPRO7AI Reef EVK demo

Out-of-box firmware for the **IPRO7AI Reef EVK**: one shell command per block
on the board, including a headless OV5640 / OV2640 camera stream, and in the
`defconfig_wifi_ipro6s` build Wi-Fi through the IPRO6S_FIN with an RTSP stream
of the camera. Board support: `bsp/board/ipro7/ipro7ai_reef_board_config.h`
(`CONFIG_IPRO7AI_REEF`). The full pin map, power tree and jumpers are in the
IPRO7AI Reef EVK Board User Guide.

This is a different board from `ipro7ai_evk_demo` (the SoM sub-board with the
ST7789 LCD and SC101IOT camera). On the Reef the IPRO7AI QFN56 sits directly on
the carrier; there is no LCD, no battery, and the camera connector is the
24-pin OmniVision FLEX-24 (OV5640 / OV2640 modules).

## Build / flash

```bash
make -C apps/platform/ipro7ai_reef_demo                                  # -> build/build_out/ipro7ai_reef_demo_IPRO7.bin
make -C apps/platform/ipro7ai_reef_demo DEFCONFIG=defconfig_wifi_ipro6s  # + Wi-Fi and RTSP -> build_wifi_ipro6s/build_out/
```

Flash with the command-line tool of IntelProCube (`ipro_iot_tool.exe`,
`ipro_iot_tool_macos` or `ipro_iot_tool_linux`), run from the IntelProCube
directory:

```bash
ipro_iot_tool_macos --chipname ipro7 --chip-variant IPRO7AI --port /dev/cu.usbserial-XXXX \
    --baudrate 921600 --pt chips/ipro7/partition/partition_cfg_2M.toml \
    --firmware <SDK>/apps/platform/ipro7ai_reef_demo/build/build_out/ipro7ai_reef_demo_IPRO7.bin
```

`--chip-variant IPRO7AI` selects the QFN56 flash pins. The tool exits 0 even
when flashing fails, so look for `[All Success]` in its output.

Console: 115200 8N1 on the U6 SH1.0 wafer (GPIO20 TX / GPIO21 RX). The wafer's
DTR and RTS pull BOOT (GPIO41) and RESET through 1 k, so an auto-download
cable works; by hand it is hold **SW1** (BOOT), tap **SW2** (RESET).

## Before powering up

| Jumper | Setting |
|---|---|
| JP1 | VDDIO1: 1-2 = 3.3 V, 2-3 = 2.8 V. **JP1 must be fitted**, or the VDDIO1 bank (camera, USB, BOOT) is unpowered. |
| JP2 | 1-2 (3V3 from the buck). R40 0R is the same link; fit one. |
| JP3 | 1-2 = SD_VDD always on. 2-3 = switched by GPIO14 — use this with the IPRO6S_FIN so `fin cycle` works. |

Power is USB-C only. The WS2812 and the class-D amplifier run from VBUS, so
a current-limited bench supply on 3V3 alone leaves the LED and speaker dead.

## What happens at power-on

1. Camera parked (PWDN high, RESETB low, XCLK off).
2. RGB LED breathes blue once, then off.
3. Keys drive the LED: SW3 red, SW4 green, SW5 blue, BOOT white.
4. Shell prompt. `reef selftest` runs LED, a 1 kHz tone on the speaker, the
   slot detect and the camera probe in ~3 s.

## Shell commands

```
reef status                    pins, key states, LED, amp, detect lines
reef selftest                  LED r/g/b, 1 kHz 300 ms, SD detect, cam probe

led <r> <g> <b>                WS2812 colour (0..255), e.g. led 40 0 0
led rainbow [sec] | led off | led status

btn status                     debounced state + press counts
btn watch [sec]                print press/release events

spk tone <hz> <ms> [amp%] [hp] sine on DAC A; 'hp' keeps the amplifier muted
spk on | spk off | spk stop | spk status

mic rec <pdm|headset|aux> <s>  record up to 10 s at 16 kHz into PSRAM
mic play [hp]                  play it back (+20 dB make-up by default)
mic loop <src> [sec] [hp]      live monitor; omit sec for "until mic stop"
mic gain <db> | mic ch <0|1> | mic stop | mic status

sd status | sd mount | sd ls [path] | sd test | sd unmount
sd bench [kib]                 write, read back and verify (default 1024 KiB)
sd power on|off|cycle          SD_VDD gate on GPIO14 (JP3 2-3)

fin                            IPRO6S_FIN slot status
fin cycle | fin power on|off   power-cycle the fin (JP3 2-3)

cam probe                      power sequence + SCCB through components/sensor
cam start | cam stop           YUV422 -> ISP ch0 (1/4 size YUYV in PSRAM)
cam status                     frames, measured fps, sync/fifo errors, mean luma
cam snap [cols]                ASCII thumbnail of the latest frame
cam bar on|off                 sensor colour bars - proves the pipeline without optics
cam reg <addr> [val]           sensor register (OV2640: 0x1xx = sensor bank)
cam order lower|higher         DVP byte order (next start)
cam sync <high|low> <high|low> VSYNC / HSYNC polarity (next start)
cam pins [ms]                  DVP pad activity: edges/s and duty per pin
cam park | cam xclk on|off

hp                             read the jack-detect level on GPIO26
```

## How each block is wired

| Block | Pins | Notes |
|---|---|---|
| RGB LED | DI GPIO24 | Driven from SPI0 MOSI: 4 SPI bits per WS2812 bit at 3.2 MHz. GPIO24 % 4 == 0 is MOSI with `pin_swap_enable = 0` (the SDK default swap 1 makes it MISO and the LED stays dark). Supply is VBUS − D3. |
| Keys | SW1 41 (**active high**), SW3 10, SW4 27, SW5 11 | Polled at 10 ms, 30 ms debounce. SW3/SW5 are released while AUX-IN captures. |
| Speaker | DAC A GPIO23 → PAM8302A, SD# GPIO25 | Enable is **active high**; the board header's `BOARD_SPK_EN_ACTIVE 1` is honoured by `audio_output_dac.c`. Gain 30 V/V — the demo defaults to 20 % amplitude. |
| Headphone | same DAC → TLV9061 → J2 (PJ-342, OMTP) | Always connected; `hp` in a command just mutes the amp. Detect GPIO26 LOW = plugged. |
| PDM mic | CLK 16, DATA 17 | MSM261DGT003 on 3V3, L/R = GND → left slot (`mic ch 1` to try the other). |
| Headset mic | AIN7 (GPIO13) +, AIN6 (GPIO12) − | Differential AUADC, 24 dB PGA. Bias 2.2 k to 3V3 on the jack. |
| AUX-IN | AIN4 (GPIO10) L, AIN5 (GPIO11) R | Single-ended, 6 dB PGA, left channel only in this demo. |
| SD slot | 28–33 on SDH_0, DET 15, PWR 14 | 4-bit FatFS at `sd:`. DET low = seated. PWR low = on (reset default). |
| Camera | D0-7 = 0-7, PCLK 38, VSYNC 39, HREF 40, XCLK 22, RST 34, PWDN 35, SCCB 36/37 | OV5640 / OV2640 YUV422 720p → ISP ch0, see below. |

## Camera

`components/sensor/ov5640.c` and `ov2640.c` (`CONFIG_SENSOR_OV5640` /
`CONFIG_SENSOR_OV2640`, opt-in) drive the FLEX-24 module in YUV422 YUYV at
1280x720 — OV5640 2x-binned through a 16:9 window, OV2640 from its UXGA base
through a 1600x900 window and DSP scaler. Both keep AEC/AGC/AWB on the sensor,
so the demo never starts the ISP firmware's imgproc 3A (the descriptor hooks
it would call are no-ops).

There is no display, so `cam start` runs the ISP with channel 0 only: YUYV
scaled 4x down (320x180) into a PSRAM ping-pong pair, counted in the ISP
callback. `cam status` gives frames, fps, the ISP's VSYNC/HSYNC/FIFO error
counters and the mean luma; `cam snap` prints the frame as ASCII; `cam bar on`
switches the sensor to colour bars so the pipeline can be judged without
lens or light.

The bring-up sequence: `cam probe` (identity over SCCB), `cam start`,
`cam status` twice a second apart (frames climbing, errors flat), `cam bar on`
then `cam snap` (vertical bands), `cam bar off`, `cam snap` (the scene).

If the camera does not stream, these are the things to check:

- The DVP pixel clock. `hal_isp` sizes the DVP_AS FIFO threshold as
  `width x (1 - (pclk/2) / BCLK) + 10` (BCLK 96 MHz, FIFO 1024 entries), so a
  720p stream needs a DVP PCLK of at least ~40 MHz. Below that `isp_init`
  prints `[WARN] dvp as threshold over 1024` and the ISP delivers 0 frames
  with 0 errors. The descriptor `pclk` must be the real DVP PCLK - reporting
  low is not the safe side. The OV5640 driver runs an 84 MHz DVP PCLK with
  OmniVision's 720p line timing: 30 fps, down to 15 in dim light.
- Orientation: the Reef's OV5640 module reads out top-to-bottom mirrored.
  `cam.c` flips in the capture DMA (DVP2AXI flip mode, `CAM_DVP_VFLIP`), not
  in the sensor - sensor flip / mirror in the binned mode puts the demosaic a
  Bayer phase off. Flip mode writes the first line at the start address and
  each next one a line lower, so the DMA is given the buffer's last line.
- Whether anything reaches the SoC at all. `cam pins [ms]` samples D0-7,
  PCLK, VSYNC and HREF (rising edges per second, duty): VSYNC edges/s is the
  sensor frame rate, HREF edges/s the line rate, HREF duty tells the DVP
  clock against the line length. It switches the pads to plain GPIO inputs
  for the measurement (with the camera function selected they read 0) and
  hands them back to the camera block after.
- Sync polarity and byte order. VSYNC/HSYNC errors that climb with the frame
  count mean the polarity is wrong: `cam sync low high` / `cam sync high low`
  and restart. A picture with Y/UV swapped (grey-green, doubled edges in
  `cam snap`) means `cam order higher`.
- Whether `camera_module_init()`'s 8-bit SCCB scan sees the OV5640 (16-bit
  registers). It should — the scan only needs the address ACK — and if it
  does not, `cam probe` says so: it re-reads both OmniVision IDs directly and
  logs "framework scan missed it" when the sensor is there after all.

## Wi-Fi and RTSP (`defconfig_wifi_ipro6s`)

The IPRO6S_FIN is an SDIO slave on the card slot's six lines, powered from
SD_VDD. With JP3 on 2-3, `fin cycle` power-cycles it through GPIO14. The
`defconfig_wifi_ipro6s` build adds the host side (`components/network/ipronet_host`
over SDIO, lwIP) and an RTSP MJPEG stream of the camera
(`components/network/rtsp_server`):

```
ipronet_up [<ssid> <psk>]      ipronet_start() on the fin, then ipronet_connect() if given an AP
ipronet_down                   ipronet_stop()
ipro6s_scan | ipro6s_scan_last scan for APs; print the last result
ipro6s_connect <ssid> <psk>    associate and run DHCP
ipro6s_link_status             STA link status
ipro6s_disconnect              disconnect from the AP
rtsp_cam on [fps] | off | stats  camera stream at rtsp://<ip>:8554/av0_0 (ch0 at sensor / 2)
ipro6s_ota <http url>          update the IPRO6S_FIN firmware over HTTP
```

`apps/platform/ipro7_sdio_wifi` is the stand-alone Wi-Fi reference app; build it
with `CONFIG_IPRO7AI_REEF=y` to run it on this board.

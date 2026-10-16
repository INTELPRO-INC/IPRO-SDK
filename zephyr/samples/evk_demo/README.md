# IPRO7AI EVK demo (Zephyr)

One image that exercises the EVK's peripherals from the shell:

| command | what it does |
|---|---|
| `evk lcd bars` / `checker [px]` / `gradient` / `fill <color>` / `bl on|off` | ST7789P3 240x320 over SPI0 through Zephyr's display API |
| `evk mic level [s]` | PDM microphone (IM69D129, GPIO18/19) RMS meter on the console and as a bar on the LCD |
| `evk mic rec [s]` / `evk mic play` | record up to 20 s (16 kHz mono) into PSRAM, play it back on the speaker |
| `evk mic loop [s]` | live microphone -> speaker |
| `evk spk tone [hz] [ms]` | sine on the speaker (GPDAC channel A, GPIO23, TPA2037D1 enable GPIO22) |
| `evk cam start|stop|snap` | SC101IOT DVP camera preview on the LCD (`CONFIG_EVK_DEMO_CAMERA=y`) |

Build and flash (1 MB boot2 layout, see `docs/zephyr/getting_started.md`):

```bash
west build -p always -b ipro7ai_evk ../zephyr/samples/evk_demo
```

Speaker playback uses the GPDAC DMA stream (`ipro_zephyr/ipro_dac_stream.h`, DMA channel 1);
the microphone path is the DMIC driver over the AUADC HAL (DMA channel 0, HAL-owned). The
AUADC's real rate for "16 kHz" is 15875 Hz, so a recording plays back ~0.8 % fast.

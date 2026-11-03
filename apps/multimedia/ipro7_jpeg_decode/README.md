# JPEG to RGB565 / RGB888 on IPRO7

Decode a JPEG picture into RGB565 or RGB888 pixels on IPRO7 with a software decoder,
and check the result.

IPRO7's MJPEG hardware block is supported for **encoding only**. `hal_mjdec.h` exists in
the SDK, but on IPRO7 the decoder never completes a frame (no frame-done interrupt, no
output), so JPEG pictures are decoded on the CPU, by TJpgDec R0.03 (BSD-style licence, see
the header of `tjpgd/tjpgd.c`). The decoder sources are in `tjpgd/` because the SDK
release packages do not carry `components/3rdparty/tjpgdec`; `tjpgd.c` / `tjpgd.h` are that
component's files unchanged, `tjpgdcnf.h` selects 24-bit output. To use it in your own
project copy `jdec_rgb.c`, `jdec_rgb.h` and `tjpgd/`.

## What the wrapper adds to TJpgDec

| Topic | TJpgDec on its own | `jdec_rgb` |
|---|---|---|
| Unsupported file | `JDR_FMT1` / `JDR_FMT3` after the decoder has started | `jdec_probe()` names the reason first (progressive, CMYK, 4:1:1, truncated, ...) |
| Colour order | writes B, G, R; with `JD_FORMAT 1` its "RGB565" is really BGR565 | true RGB565 (LE or BE) or RGB888, whatever `JD_FORMAT` is |
| Camera MJPEG frame without DHT | fails (`JDR_FMT1`) | standard Huffman tables inserted |
| Output | callback per MCU block | whole picture into a buffer (`jdec_decode`) or per block (`jdec_decode_blocks`) |
| Output size with scaling | to be worked out | `jdec_out_dims()` / `jdec_out_size()` |

## API

```c
#include "jdec_rgb.h"

jdec_info_t info;
int err = jdec_probe(jpg, len, &info);            /* can it be decoded, how big is it? */
if (err != JDEC_OK) {
    printf("%s\r\n", jdec_strerror(err));
    return;
}
uint32_t size = jdec_out_size(&info, JDEC_RGB565, 0);   /* scale 0..3 = 1/1, 1/2, 1/4, 1/8 */
uint8_t *rgb  = pvPortMalloc(size);                     /* PSRAM when CONFIG_USE_PSRAM=y */
err = jdec_decode(jpg, len, JDEC_RGB565, 0, rgb, size, &info);
```

Formats: `JDEC_RGB565` (uint16_t, R in bits 15..11, little-endian), `JDEC_RGB565_BE`
(same value, high byte first, as SPI LCD controllers expect), `JDEC_RGB888` (R, G, B bytes).
For an LCD, `jdec_decode_blocks()` passes each decoded block (at most 16 x 16 pixels) to a
callback, so no frame buffer is needed; a callback writing into its own buffer must keep
`x + w` and `y + h` within `jdec_out_dims()`. The decoder allocates an 8 KB work area per
call (16 KB with `JD_FASTDECODE 2`; typically 3.5 KB / 9.7 KB is used, 5.4 KB for a valid file
at the format limits); the input JPEG can be anywhere the CPU can read, flash included.
`jdec_out_size()` returns 0 when the picture cannot be decoded or needs 4 GB or more.

Accepted: baseline (SOF0) Huffman JPEG, grey or YCbCr 4:4:4 / 4:2:2 / 4:2:0, 8-bit tables,
any size, EXIF / ICC / other APPn segments, restart markers, missing DHT.
Rejected: progressive, other SOF types (extended, lossless, arithmetic), 12-bit, 4:1:1 / 4:4:0,
CMYK / RGB-coded, 16-bit quantisation tables, files without EOI.
`host/jpeg_check.py photo.jpg [--fix out.jpg]` checks a file on the PC and converts it if needed.

## Build and flash

```bash
make -C apps/multimedia/ipro7_jpeg_decode                            # IPRO7AI EVK, console 921600
make -C apps/multimedia/ipro7_jpeg_decode DEFCONFIG=defconfig_reef   # IPRO7AI Reef (build_reef/)
```

Needs PSRAM (`CONFIG_USE_PSRAM=y`) for the output buffers. Image: 482 KB flash (360 KB of
it test JPEGs), 41 KB OCRAM. With CMake 3.18 or later `jdec_rgb.c` and `tjpgd/tjpgd.c` are
compiled at `-O2` (about 1.9x faster than the SDK's `-Os`, same output); older CMake keeps `-Os`.

## Shell

| Command | |
|---|---|
| `jdec list` | built-in test images and what is expected of each |
| `jdec info <n\|rx>` | what `jdec_probe()` reports |
| `jdec mem` | stack and heap used around a few decodes |
| `jdec run <n\|all> [565\|565be\|888] [scale] [blocks]` | decode, check, time |
| `jdec dump <n\|rx> [565\|888] [scale]` | print the pixels as base64 |
| `jdec rx <bytes>`, `jdec put <base64>`, `jdec go [fmt] [scale]` | receive a JPEG over the console and decode it |

Checks on the built-in images: the output hash must equal that of the same decoder built on
the PC (`=host`, bit-exact), and the RGB mean of every 16 x 16 block (64 x 64 above 640
pixels wide) must be within 10 of libjpeg.

Host tools (Python 3, pyserial, Pillow, numpy):

```bash
python3 host/jdec_host.py -p /dev/cu.usbmodemXXXX02 upload photo.jpg --go 565
python3 host/jdec_host.py -p /dev/cu.usbmodemXXXX02 dump rx --fmt 888 -o board.png --ref photo.jpg
python3 host/run_host_tests.py      # decoder + wrapper on the PC against libjpeg (needs cc, djpeg)
python3 host/make_test_jpegs.py     # regenerate test_images.c (needs cc, djpeg, jpegtran)
```

## Results (IPRO7AI Reef, CPU 192 MHz, 2026-10-30)

`jdec run all` with 565, 888, 565be, and 565 / 888 in block mode: 14 / 14 PASS each; all 11
decodable images bit-exact with the PC build; 3 rejected with the expected reason
(progressive, CMYK, truncated). Against libjpeg (RGB888, PC build): PSNR 44.7-45.3 dB on the
test images (grey 49.6 dB); a 640 x 480 photo-like JPEG with EXIF + ICC sent over the console:
46.2 dB. With `JD_FORMAT 1` (16-bit, as `components/3rdparty/tjpgdec`) RGB888 is expanded
from 16 bits: 38.3-39.1 dB. `host/run_host_tests.py` runs on an AddressSanitizer +
UndefinedBehaviorSanitizer build and decodes damaged copies of the test images; 1500 of them
(3000 decodes) gave no memory error, and `host/jpeg_check.py` agreed with `jdec_probe()` on all.
Stack: about 1 KB at peak (decode plus `printf`); heap back to the same value after each decode.

Decode time, output to PSRAM, code in flash:

| Picture | RGB565 `-O2` | RGB888 `-O2` | RGB565 `-Os` | RGB888 `-Os` |
|---|---|---|---|---|
| 320 x 240 4:2:0 | 53 ms | 53 ms | 95 ms | 91 ms |
| 320 x 240 4:2:2 / 4:4:4 / grey | 58 / 67 / 50 ms | 57 / 66 / 48 ms | 110 ms (4:4:4) | |
| 640 x 480 4:2:0 | | 204 ms | | |
| 1280 x 720 4:2:0 | 571 ms | 564 ms | | |
| 1920 x 1080 4:2:0 | 1.27 s | 1.25 s | 2.41 s | 2.31 s |
| 1920 x 1080 at 1/2, 1/4, 1/8 | 1.08 s, 1.03 s, 0.45 s | | | |

`-O2` applies with CMake 3.18 or later (see `CMakeLists.txt`). Block mode adds 3-4 %.
`JD_FASTDECODE 2` (16 KB work area) saved a further 5 % in a test with the 16-bit build; it is
left at 1. `-fwrapv` keeps corrupt data from causing undefined behaviour in the decoder's
integer IDCT (it costs 3-5 % on RGB888).

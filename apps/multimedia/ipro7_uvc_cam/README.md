# ipro7_uvc_cam — USB webcam with on-NPU AI

A USB Video Class camera: DVP sensor → ISP → hardware MJPEG → isochronous
endpoint at USB Full Speed. Hosts open it with their built-in UVC driver;
nothing to install (verified on macOS). The NPU works on a
second ISP channel at the same time, and what it finds is drawn into the
stream the host receives.

| Build | Board, sensor | Stream (MJPEG) | On the NPU |
|---|---|---|---|
| `.config` | IPRO7 EVB, SC101IOT | 320 × 240 | object detector, boxes drawn as OSD ([EVB build](#evb-build-config)) |
| `defconfig_reef_ov5640` | IPRO7AI Reef EVK, OV5640 | 1280 × 720 or 640 × 360 | nothing (plain webcam) |
| `defconfig_reef_rps` | IPRO7AI Reef EVK, OV5640 | 640 × 360 | rock-paper-scissors game |

## Build

```bash
make -C apps/multimedia/ipro7_uvc_cam                                  # EVB    -> build/build_out/ipro7_uvc_cam_IPRO7.bin
make -C apps/multimedia/ipro7_uvc_cam DEFCONFIG=defconfig_reef_ov5640  # Reef   -> build_reef_ov5640/build_out/
make -C apps/multimedia/ipro7_uvc_cam DEFCONFIG=defconfig_reef_rps     # Reef   -> build_reef_rps/build_out/
```

## Flash (IPRO7AI Reef EVK)

Board setup (jumpers, power, console) is in the IPRO7AI Reef user guide; in short, power
is USB-C only. Flash with IntelProCube's command-line tool from the
IntelProCube directory, using its standard 2 MB table:

```bash
ipro_iot_tool_macos --chipname ipro7 --chip-variant IPRO7AI --port /dev/cu.usbserial-XXXX \
    --baudrate 921600 --pt chips/ipro7/partition/partition_cfg_2M.toml \
    --firmware <SDK>/apps/multimedia/ipro7_uvc_cam/build_reef_ov5640/build_out/ipro7_uvc_cam_IPRO7.bin
```

| Partition | Offset | Size | Used by |
|---|---|---|---|
| FW | 0x003000 | 1180 KB | the firmware |
| mfg | 0x12A000 | 824 KB | not used by these builds |

The tool exits 0 even when flashing fails; look for `[All Success]`. Console:
115200 8N1 on the U6 SH1.0 wafer (the EVB build uses 921600). Download mode by hand: hold SW1 (BOOT),
tap SW2 (RESET).

## Image Teach

Image Teach (a classifier trained in IPRO AI Studio or the browser, deployed over USB) is the
`image` mode of the Edge AI firmware `apps/ai/ipro_reef_teach`; this app has no Teach build.
Its camera pipeline (`uvc.c`, `uvc_isp.c`, `uvc_draw.c`, `uvc_iq_sc101iot.c`) is built from
`apps/ai/ipro_reef_teach/image/`, the same files for both.

## Rock-paper-scissors build (`defconfig_reef_rps`)

The classifier, game and model of `apps/ai/ipro7ai_evk_rps` (full IPRO SDK)
on the Reef, drawn into the stream instead of on an LCD: a play box, the live
label, a 3-2-1 countdown, both moves and the score. The model is linked into
the firmware, so there is nothing else to flash. Shell: `rps
[reset|go|mirror|print|conf|dump]`.

## Plain webcam build (`defconfig_reef_ov5640`)

Two frame sizes, the host picks: 1280 × 720 (about 15 fps for a busy scene,
limited by Full-Speed USB) and 640 × 360 (30 fps). Sensor tuning from the
shell: `cam rd <reg> [n]`, `cam wr <reg> <hexbytes>`, `cam grid [cols rows]`.

## EVB build (`.config`)

IPRO7 EVB with the SC101IOT sensor, 320 × 240 preview (the detector's buffers
leave too little PSRAM for more). Channel 1 feeds a YOLOv8-family detector on
the NPU and its boxes are drawn on the preview. The detector is chosen at
compile time in `uvc_ai.c`:

| Switch | Detector |
|---|---|
| `UVC_AI_MODEL_GESTURE 1` (default) | hand signs 0–9 |
| `UVC_AI_MODEL_GESTURE 0` | YOLOv8 person + pet |

The model is read from the `mfg` partition at +0x1000, so it is flashed
separately from the firmware. The person + pet model is
`apps/platform/ipro7_demo/demo/ai/ai_model/person_pet_detect.inm`.

## Files

| File | Role |
|---|---|
| `main.c` | platform init, sensor and ISP bring-up, UVC start |
| `uvc_ai.c` | EVB detector |
| `uvc_rps.c` | Reef rock-paper-scissors overlay |
| `../../ai/ipro_reef_teach/image/uvc.c` | USB descriptors and the frame pump |
| `../../ai/ipro_reef_teach/image/uvc_isp.c`, `uvc_isp.h` | DVP, ISP channels, MJPEG ring, NPU hand-off hooks |
| `../../ai/ipro_reef_teach/image/uvc_iq_sc101iot.c` | SC101IOT image tuning |
| `../../ai/ipro_reef_teach/image/uvc_draw.c` | shared text and bar drawing on the frame |

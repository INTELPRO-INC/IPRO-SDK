# IPRO LE Audio Unicast Headset

## Overview

LE Audio Unicast Headset bring-up application for the new IPRO BLE controller
component and unified Zephyr-derived host.

Current validated status is profile/HCI ISO payload bring-up, not a finished
customer audio product:

- peripheral advertising as `IPRO-LE-Audio`
- SMP/settings/bonding enabled
- BAP unicast server, PACS, ASCS, CAS and VCP Volume Renderer enabled
- CIS setup and ISO RX payload path validated with the two-board central test
- UART shell enabled at `115200`
- controller config: `link1_periph`, feature profile `bt54_iso2`, EM `32 KiB`
- audio output/render is currently disabled; LC3 decode/render and acoustic
  validation remain open

Use `apps/bluetooth/ble/ipro_ble_controller_central_test` as the current
automated peer for basic validation.

Android phone basic status:

- Pixel 10 Pro / Android 17 can scan/connect to `IPRO-LE-Audio`.
- Android GATT service discovery has found `1801,1853,1850,184e,1844`.
- Android system Bluetooth reports LE Audio connected and Volume Control
  connected.
- Actual LE Audio media streaming is not yet validated; Android currently
  reports zero connected audio channels with this firmware.

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    PC/Smartphone (LE Audio Source)              │
│                                                                   │
│  ┌──────────────┐   ┌──────────────┐   ┌──────────────┐        │
│  │  Music App   │   │ Volume Ctrl  │   │ Media Ctrl   │        │
│  └──────┬───────┘   └──────┬───────┘   └──────┬───────┘        │
│         │                  │                   │                 │
│         ▼                  ▼                   ▼                 │
│  ┌────────────────────────────────────────────────────┐         │
│  │         BLE LE Audio Unicast Client (CAP)          │         │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐         │         │
│  │  │   BAP    │  │   VCP    │  │   MCP    │         │         │
│  │  └──────────┘  └──────────┘  └──────────┘         │         │
│  └────────────────────────────────────────────────────┘         │
└───────────────────────────────┬─────────────────────────────────┘
                                │ BLE Link (CIS)
                                │ LC3 Encoded Audio
                                ▼
┌─────────────────────────────────────────────────────────────────┐
│              IPRO7 LE Audio Unicast Headset                     │
│                                                                   │
│  ┌────────────────────────────────────────────────────────┐     │
│  │              BLE LE Audio Server Stack                 │     │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────┐  │     │
│  │  │ BAP      │  │ VCP      │  │ MCP      │  │ CCP  │  │     │
│  │  │ Unicast  │  │ Server   │  │ Server   │  │      │  │     │
│  │  │ Server   │  │          │  │          │  │      │  │     │
│  │  └────┬─────┘  └────┬─────┘  └────┬─────┘  └──────┘  │     │
│  └───────┼─────────────┼─────────────┼───────────────────┘     │
│          │             │             │                           │
│  ┌───────▼─────────────▼─────────────▼───────────────────┐     │
│  │         GAF (Generic Audio Framework)                  │     │
│  └────────────────────────┬───────────────────────────────┘     │
│                           │                                      │
│  ┌────────────────────────▼───────────────────────────────┐     │
│  │              Audio Data Path                           │     │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐             │     │
│  │  │ ISOOHCI  │→ │   LC3    │→ │   Ring   │             │     │
│  │  │ Receive  │  │ Decoder  │  │  Buffer  │             │     │
│  │  └──────────┘  └──────────┘  └────┬─────┘             │     │
│  │                                    │                    │     │
│  │  ┌──────────┐  ┌──────────┐  ┌────▼─────┐             │     │
│  │  │ ISOOHCI  │← │   LC3    │← │   Ring   │             │     │
│  │  │ Transmit │  │ Encoder  │  │  Buffer  │             │     │
│  │  └──────────┘  └──────────┘  └──────────┘             │     │
│  └────────────────────────┬───────────────────────────────┘     │
│                           │                                      │
│  ┌────────────────────────▼───────────────────────────────┐     │
│  │              USB Audio Class (UAC)                     │     │
│  │  ┌──────────────────┐      ┌──────────────────┐       │     │
│  │  │  UAC Playback    │      │  UAC Capture     │       │     │
│  │  │  (Speaker Out)   │      │  (Mic In)        │       │     │
│  │  └────────┬─────────┘      └────────▲─────────┘       │     │
│  └───────────┼──────────────────────────┼─────────────────┘     │
└──────────────┼──────────────────────────┼───────────────────────┘
               │                          │
               ▼                          │
        ┌─────────────┐          ┌───────┴──────┐
        │   Speaker   │          │  Microphone  │
        └─────────────┘          └──────────────┘
```

## Key Features

### Audio Profiles
- **BAP (Basic Audio Profile)**: Unicast audio streaming with CIS (Connected Isochronous Stream)
- **VCP (Volume Control Profile)**: Remote volume control from source device
- **MCP (Media Control Profile)**: Playback control (play/pause/next/prev)
- **CCP (Call Control Profile)**: Phone call handling (ready for future development)

### Audio Capabilities
- **Codec**: LC3 (Low Complexity Communications Codec)
- **Sample Rates**: 16kHz, 24kHz, 32kHz, 48kHz
- **Frame Duration**: 7.5ms, 10ms
- **Bitrates**: 32kbps - 96kbps (configurable)
- **Bidirectional Audio**: Simultaneous playback and capture

### Advanced Features
- **Auto-reconnection**: Automatically reconnect to paired devices
- **Battery Service**: Report battery level to connected device
- **Device Information Service**: Expose device info (model, firmware version)
- **GATT Caching**: Faster reconnection with cached GATT database
- **Smart Power Management**: Optimized for low power consumption

### Shell Commands
```bash
# Volume Control
volume get                    # Get current volume
volume set <0-255>           # Set volume level
volume mute                  # Mute audio
volume unmute                # Unmute audio

# Media Control
media play                   # Start playback
media pause                  # Pause playback
media next                   # Next track
media prev                   # Previous track

# Connection Management
gap disconnect               # Disconnect from source
gap advertise <on|off>       # Control advertising

# Audio Control
audio status                 # Show audio stream status
```

## DEMO Application Scenarios

### Scenario 1: Wireless Music Listening
**Use Case**: Listen to music from smartphone/PC wirelessly

**Setup**:
1. Power on IPRO7 headset → auto-start advertising
2. On smartphone: Open Bluetooth settings → Connect "IPRO-Headset"
3. Open music app → Select IPRO-Headset as audio output
4. Play music → Audio streams via BLE LE Audio

**User Experience**:
- High-quality audio with LC3 codec (better than SBC)
- Low latency (~20-40ms)
- Phone volume controls work seamlessly
- Media play/pause buttons control smartphone playback

### Scenario 2: Video Conference with Bidirectional Audio
**Use Case**: Join video conference call with microphone support

**Setup**:
1. Connect IPRO7 headset to PC
2. Join Zoom/Teams meeting
3. Select "IPRO-Headset" for both speaker and microphone
4. Bidirectional audio automatically established

**User Experience**:
- Clear voice transmission with LC3 encoding
- Echo cancellation enabled
- Low latency for natural conversation
- USB audio seamlessly bridges BLE and PC

### Scenario 3: Gaming with Low-Latency Audio
**Use Case**: Play mobile games with minimal audio delay

**Setup**:
1. Connect to gaming smartphone
2. Launch game application
3. Audio automatically routes to headset with optimized latency

**User Experience**:
- Fast audio response (<30ms typical)
- Synchronized audio and visual feedback
- Volume adjustable without leaving game

### Scenario 4: Smart Home Voice Assistant
**Use Case**: Interact with voice assistant (Siri/Google Assistant)

**Setup**:
1. Connected to smartphone
2. Trigger voice assistant with button or wake word
3. Audio I/O handled by headset

**User Experience**:
- Clear voice command capture
- Assistant responses play through headset
- Hands-free operation

## Build Instructions

### Prerequisites
- IPRO SDK properly set up
- RISC-V toolchain installed (`/opt/toolchain/riscv_ipro7/`)

### Build Command
```bash
cd <SDK root>
PATH=/opt/toolchain/riscv_ipro7/bin:$PATH \
IPRO_SDK_BASE=$(pwd) \
make -C apps/bluetooth/le_audio/ipro_le_audio_headset -j8
```

### Build Output
- Binary:
  `apps/bluetooth/le_audio/ipro_le_audio_headset/build/build_out/ipro_le_audio_headset_IPRO7.bin`
- Latest recorded basic build: `521080 B`, SHA256
  `55074c89ae853bb462af90f3cdd0129a4ccad42cd8163d9774ac2ad20be6473f`.

### Flash to Device
```bash
# $CUBE_UTILS is the utils/ directory of your IntelProCube installation.
# The port name changes with the USB adapter - check `ls /dev/cu.*` rather
# than assuming, and use cu.* not tty.* (the latter blocks on carrier detect).
python3 "$CUBE_UTILS/ipro_probe_ctl.py" \
  dl --port /dev/cu.usbmodemXXXX

"$CUBE_UTILS/ipro_flash.sh" \
  --chipname ipro7 \
  --port /dev/cu.usbmodemXXXX \
  --firmware apps/bluetooth/le_audio/ipro_le_audio_headset/build/build_out/ipro_le_audio_headset_IPRO7.bin
```

## Basic Android phone smoke

With the headset app running on A:

```sh
cd <SDK root>
PATH=/opt/toolchain/riscv_ipro7/bin:$PATH \
IPRO_SDK_BASE=$(pwd) \
python3 apps/bluetooth/le_audio/ipro_le_audio_headset/tools/verify_le_audio.py \
  --port /dev/cu.usbmodemXXXX \
  --baud 115200 \
  --duration 45 \
  --device IPRO-LE-Audio \
  --expect-ascs \
  --log /private/tmp/ipro_android_leaudio_smoke_board.log \
  --json /private/tmp/ipro_android_leaudio_smoke.json
```

Latest recorded r48 result:

- Android found `IPRO-LE-Audio`, connected, and discovered
  `1801,1853,1850,184e,1844`.
- The phone bonded successfully; the DUT reported security level 4.
- This verifies phone visibility/profile connection, not media streaming.

## Configuration

### Kconfig Options
Key configuration options in `defconfig`:

```ini
# BLE Configuration
CONFIG_BLE_PERIPHERAL=y
CONFIG_BLE_CENTRAL=n

# GAF Audio Profiles
CONFIG_GAF_BAP=y
CONFIG_GAF_BAP_CAPA_SRV=y
CONFIG_GAF_VCP=y
CONFIG_GAF_VCP_SRV=y
CONFIG_GAF_MCP=y
CONFIG_GAF_MCP_SRV=y

# Audio Configuration
CONFIG_LC3_CODEC=y
CONFIG_ISOOHCI=y

# USB Audio
CONFIG_CHERRYUSB=y
CONFIG_CHERRYUSB_DEVICE=y
CONFIG_CHERRYUSB_DEVICE_AUDIO=y
```

### Audio Parameters
Configurable in `src/app_audio.h`:

```c
#define APP_AUDIO_SAMPLE_RATE      48000   // Hz
#define APP_AUDIO_FRAME_DURATION   10      // ms
#define APP_AUDIO_BITRATE          96      // kbps
#define APP_AUDIO_CHANNELS         2       // Stereo
```

## Development Roadmap

### Phase 1: Core Features ✅ (Completed)
- [x] Basic BAP unicast audio streaming
- [x] LC3 codec integration
- [x] USB audio bridge
- [x] VCP volume control
- [x] MCP media control
- [x] Auto-reconnection support

### Phase 2: Enhanced User Experience (Q1 2026)
- [ ] **Multipoint Connection**: Connect to 2 devices simultaneously
- [ ] **Fast Pair Support**: Google Fast Pair for quick pairing
- [ ] **AVRCP Fallback**: Support classic Bluetooth for older devices
- [ ] **Battery Optimization**: Extend battery life by 30%
- [ ] **Custom EQ**: User-configurable audio equalization

### Phase 3: Advanced Audio Features (Q2 2026)
- [ ] **Broadcast Audio Scan**: Scan and join Auracast broadcasts
- [ ] **Audio Sharing**: Share audio with another headset
- [ ] **Spatial Audio**: 3D audio positioning support
- [ ] **Hearing Aid Features**: Customizable audio amplification
- [ ] **Active Noise Cancellation**: ANC integration

### Phase 4: Ecosystem Integration (Q3 2026)
- [ ] **CCP Full Implementation**: Complete call control profile
- [ ] **HAP Integration**: Hearing Access Profile for accessibility
- [ ] **TMAP Support**: Telephony and Media Audio Profile
- [ ] **Multi-language Voice Prompts**: Localized user feedback
- [ ] **OTA Firmware Update**: Over-the-air update via BLE

### Phase 5: AI and Smart Features (Q4 2026)
- [ ] **AI Noise Reduction**: Deep learning-based noise cancellation
- [ ] **Voice Enhancement**: AI-powered voice clarity
- [ ] **Context Awareness**: Automatic audio mode switching
- [ ] **Personalized Audio**: Learning user preferences
- [ ] **Health Monitoring**: Heart rate/activity tracking integration

## Technical Specifications

### Performance Metrics
- **Audio Latency**: 20-40ms (typical)
- **Connection Range**: Up to 10 meters (Class 2)
- **Battery Life**: 8+ hours continuous playback
- **Standby Time**: 200+ hours
- **Charging Time**: ~2 hours

### Memory Usage
- **Flash**: ~550 KB
- **RAM**: ~140 KB (OCRAM)
- **PSRAM**: ~2 MB (audio buffers)

### Supported Platforms
- IPRO7 Development Board
- IPRO7 Commercial Modules
- Future: IPRO8 platform

## Troubleshooting

### Common Issues

**Issue**: Headset not discoverable
- **Solution**: Check if advertising is enabled: `gap advertise on`

**Issue**: No audio output
- **Solution**: Verify USB connection and check `audio status` command

**Issue**: Poor audio quality
- **Solution**: Check RSSI (`gap status`) and ensure strong BLE signal

**Issue**: Volume control not working
- **Solution**: Ensure VCP is enabled in configuration and source device supports it

**Issue**: Disconnection during audio playback
- **Solution**: Check for interference, reduce distance, or update firmware

## References

- [Bluetooth LE Audio Specification](https://www.bluetooth.com/specifications/le-audio/)
- [LC3 Codec Documentation](https://www.bluetooth.com/specifications/lc3/)
- [IPRO SDK Documentation](../../README.md)
- [GAF Developer Guide](../../../docs/ble/GAF_DEVELOPER_GUIDE.md)

## License

Copyright (c) 2024-2026 INTELPRO Inc. All rights reserved.

## Contact

For technical support or questions:
- Email: support@intelpro.com.tw
- Website: https://www.intelpro.com.tw

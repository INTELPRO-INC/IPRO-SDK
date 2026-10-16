# LE Audio unicast headset on the IPRO7AI EVK

A phone's LE Audio unicast client (Android "LE Audio" headphones flow) pairs with the
EVK and streams LC3 over a CIS to the on-board speaker. Port of the FreeRTOS
`apps/bluetooth/le_audio/ipro_le_audio_headset` (same controller archive, PACS
capability and service set) onto Zephyr's own CAP/BAP/VCP host.

```bash
west build -p always -b ipro7ai_evk ../zephyr/samples/le_audio_headset
```

What it registers: CAP acceptor (CAS + CSIP set member, set size 1), PACS sink (LC3
16/32/48 kHz, 7.5/10 ms, 1 channel, 26..155 octets, front-left), ASCS with 2 sink ASEs,
VCP volume renderer (drives the output gain). Advertises as `IPRO-LE-Audio` with the
ASCS unicast announcement, Earbud appearance, CAS/ASCS/PACS/VCS UUIDs and a CSIS RSI;
security L2 is requested on connect and the bond is kept in NVS (`storage_partition`).

Audio: SDUs are queued to a decoder thread (liblc3); a lost SDU replays the last good
frame (halving per consecutive loss); PCM goes through `samples/common/audio/pcm_out.c`
(GPDAC at 32 kHz for 32/48 kHz streams, 16 kHz for 16 kHz, level-servoed resampling,
hardware DMA ping-pong).

Phone: Bluetooth settings -> pair new device -> `IPRO-LE-Audio`. Android shows it as an
LE Audio device; play music. Shell: `pcm status` (ring/underrun/step/peaks), `pcm vol
<0-255>`, `pcm tone [ms]` (local 1 kHz, only while not streaming), `hs unpair` (forget
all bonds - then forget the EVK on the phone too), `hs disconnect`.

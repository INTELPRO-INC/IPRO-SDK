# BAP broadcast sink on the IPRO7AI EVK speaker

Zephyr's `samples/bluetooth/bap_broadcast_sink` (upstream sources, unmodified) with the
decoded LC3 PCM played through the GPDAC stream (`src/hw_codec_dac.c`) on the EVK's
class-D speaker, and a Kconfig that lets `ENABLE_LC3` be selected on this SoC.

```bash
west build -p always -b ipro7ai_evk ../zephyr/samples/bap_broadcast_sink
```

Pair it with `../zephyr/samples/bap_broadcast_source` on the EVB (LC3 sine, 16 kHz,
2 BIS). The sink scans for `CONFIG_TARGET_BROADCAST_NAME` ("IPRO7 Broadcast Source"),
syncs to the periodic advertising and the BIG, decodes and plays. Shell: `codec status`
(pad state, ring/refill statistics), `codec tone [ms]` (local 1 kHz through the same
DAC path), `codec spk on|off`.

Notes learnt on the way (all in the glue or the port):

- the GPDAC's 16 kHz is not the source's 16 kHz (different crystals; measured within
  0.1 %, not the 15873 Hz the AUADC runs at): the refill resamples with linear
  interpolation, the step servoed +-1 % on the ring-buffer level (dropping one sample in
  126 was a 127 Hz buzz, and drained the ring);
- the sample opens the codec once per stream and forwards channel 0 of every stream
  through one buffer: the sink syncs to the FRONT_LEFT BIS only
  (`CONFIG_TARGET_BROADCAST_CHANNEL=1`) and the DAC is set up once;
- GPIO22 is both the speaker enable and DAC channel B: it is re-muxed as a GPIO after
  every GPDAC (re)initialisation;
- a lost SDU is concealed by replaying the last good PCM frame (`src/lc3_plc.c`, in front
  of liblc3 through a `-D` on the upstream lc3.c): liblc3's spec PLC randomises the
  spectrum's signs, a 10 ms burst of noise on tonal audio;
- the port's HCI driver needed `BT_BUF_EVT_DISCARDABLE_SIZE` 255 (extended advertising
  reports longer than ~31 bytes of AD were dropped) and a 2 ms gap after command events
  (this controller answers LE Create BIG with the BIG Complete event within a millisecond).

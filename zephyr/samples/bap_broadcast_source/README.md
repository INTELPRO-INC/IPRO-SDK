# BAP broadcast source on IPRO7

Zephyr's `samples/bluetooth/bap_broadcast_source` (upstream sources, unmodified) built
with the IPRO7 ISO controller (`bt54_iso2`, 32 KB Exchange Memory) and LC3 enabled;
broadcasts a 16 kHz LC3 sine on 2 BIS as "IPRO7 Broadcast Source" (`BT_LE_EXT_ADV` +
periodic advertising + BIG). Verified 2026-09-21 on ipro7_evb, received by the EVK
sink (`../bap_broadcast_sink`) and by an Android phone's Auracast receiver.

```bash
west build -p always -b ipro7_evb ../zephyr/samples/bap_broadcast_source
```

`../bt_le_audio/iso.conf` holds the controller settings for any other ISO / LE Audio
sample (`iso_broadcast`, `iso_receive`, `cap_*`, `tmap_*`).

`CONFIG_BAP_BROADCAST_HIGH_RELIABILITY` (default y) broadcasts with the "_2" twin of the
chosen preset (RTN 4, 60 ms latency instead of RTN 2, 10 ms): on the bench the "_1" preset
lost ~3 % of the SDUs (a 10 ms PLC gap each), audible as an occasional hiccup on the sink.

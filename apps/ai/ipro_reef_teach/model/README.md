# Voiceprint model

`model_b.spkb` is the voiceprint model for the voice mode of the Teach firmware
(`apps/ai/ipro_reef_teach`).

| | |
|---|---|
| Name in the blob | `b3-e12` |
| Size | 437,200 B |
| CRC-32 (what the board and IPRO AI Studio report) | `db073b9e` |
| SHA-256 | `3d1937399e6aa3a4a1df4e4356a994c6d023885f7c0e26d41b69328ffcbf2b9b` |
| Format | SPKB: `.inm` trunk for the NPU (16 of 16 layers), float16 head, quantisation parameters, CRC (`spkb_hdr_t` in `../src/spk_model.c`) |
| Voiceprint | 192 values (`CONFIG_AI_SPK_EMBEDDING_DIM=192`) |

Flash it with the firmware: build the mfg payload with
`python3 apps/ai/ipro_reef_teach/mfg_payload.py --voice apps/ai/ipro_reef_teach/model/model_b.spkb --mode image -o reef_teach_mfg.bin`
and pass it to IntelProCube with `--mfg` (`../../ipro_reef_teach/README.md`, *Build and flash*).

Or deploy it over USB with the IPRO AI Studio command line: `ipro-ai model pack`
makes a package from the `.spkb`, and `ipro-ai deploy` sends it; the board checks
its CRC and keeps it through the model store.

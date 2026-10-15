# Radar sensors

Keywave KW307-P 24 GHz human-presence module, plus the on-board analysis that
the module itself does not provide.

```
CONFIG_COMPONENTS_RADAR_KW307_ENABLE=y
CONFIG_RADAR_KW307_HISTORY_SECONDS=900
CONFIG_NMSIS_DSP=y                      # required by the breathing estimate
```

| File | Role |
|------|------|
| `kw307_proto.[ch]`    | portable codec — CRC-16/CMS, framing, output-frame decode |
| `kw307_dev.[ch]`      | UART1 binding, pad routing, command/ACK layer, statistics |
| `kw307_history.[ch]`  | PSRAM ring of past frames |
| `kw307_analysis.[ch]` | activity classification and the q15 breathing estimate |
| `kw307_map.[ch]`      | range-time map — the NPU input |

Host tests, no board needed: `sh components/radar/test_host/run.sh`

## What the module gives you, and what it does not

One CMD `0xF0` frame every 50 ms: a frame index, a presence flag, two moving
target slots and one stationary slot, each carrying distance, angle, magnitude
and tracker state. **540 B/s.** There is no raw ADC, no IQ, no range-Doppler
and no point cloud in the documented command set — the detection decisions are
made inside the module.

Two consequences worth stating before anyone plans a product on it:

- **No elevation.** The module reports range and azimuth only. A fall is a
  vertical event, so this part cannot detect one; "a moving target stopped" is
  all the geometry supports, and that is also what sitting down looks like.
- **Three targets, hard.** Two moving and one stationary. It is not a people
  counter.

## 🔴 The reported distance is twice the true range

Measured 2026-08-31 by a two-point test at a tape-measured 1.0 m and 3.0 m: the
*difference* came out at 4.0 m, so this is a scale error, not an offset.
Physically it is the round-trip path — range = c·τ/2 with the /2 missing.

This contradicts SDK User Guide §2.1, which states `Distance_mm ... (2000 mm =
200 cm)`. So `kw307_tracker_t.distance_raw` keeps the wire value untouched and
`kw307_distance_mm()` applies the correction in one named place. Never fold it
into the decoder: a Keywave firmware fix would turn a silent /2 into a silent
halving of every reading. `kw307_dist_divisor` is writable at runtime
(`kw_scale 1` shows raw wire units) so the measurement can be repeated on a new
module without a rebuild.

**Re-run the two-point test on any new module or firmware.** The *angle* scale
has never been checked independently.

## Why the history ring exists

The module vendor's pitch is that its algorithms are strong enough that the
customer only needs a Cortex-M0. That is true for "is someone there" and it is
also the ceiling: an M0 holds seconds. At 26 bytes a sample and 20 Hz, an hour
of history is 1.79 MB, and this part has 8 MB of PSRAM.

Everything with a window longer than one frame reads that ring — dwell
statistics, the breathing estimate, behavioural baselines, and the time axis of
the range-time map.

## 🔴 Range-time is the wrong picture for this module

A range-against-time image is the obvious 2D view of a radar, and
`kw307_map.[ch]` builds one. **On hardware it runs at about 2% occupancy** —
31 of its 32 rows are empty (measured 2026-08-31). The reason is structural:
this module reports a *tracked point*, not a range profile, so exactly one
range bin is lit per column. It is a good picture of a trajectory and a poor
tensor — a network would be learning from 97% zeros.

The information that is actually present is in how the magnitude of that one
track **fluctuates**: breathing, postural sway, limb motion. `kw307_spectro.[ch]`
turns that into a time-frequency image where every cell carries energy. Use the
spectrogram as the NPU input; keep the map for reading trajectories by eye.

## The range-time map, and the NPU

A single frame is three numbers; there is nothing for a convolutional network
to convolve over. A minute of frames stacked into range × time is a 2D image,
which is exactly what a small conv net eats. That is the argument for this part
over the M0: **the memory buys the time axis, and the time axis is what the NPU
needs.**

Tensor contract (also in `kw307_map.h`, keep the two in step):

```
shape   32 range bins x 64 time steps, uint8, row-major
        row = range bin, near to far, 250 mm each (8 m of coverage)
        column = time, oldest to newest
value   target magnitude, saturating at 4096 counts
0       no target in that bin at that time
```

⚠️ **The model must use Conv2D over this map, not Conv1D over time.** A Conv1D
with a non-square kernel does not map onto this NPU — a silicon limit, not a
converter gap — and would silently fall back to the CPU.

## The spectrogram — the dense tensor

```
shape   32 frequency bins x 64 time steps, uint8, row-major
        row = frequency, slowest at the top; bins 1..32 of a 128-point
              transform at 5 Hz, i.e. 2.3 .. 75 cycles/min
        column = time, oldest to newest, 1.6 s apart
value   log-compressed band energy over 12 octaves
```

The magnitude series is decimated 20 Hz → 5 Hz first. At the full rate a
128-point transform spreads 0..10 Hz over 64 bins and puts the entire breathing
band inside the first three — the band of interest would be quantised away. At
5 Hz the same transform gives 0.039 Hz per bin, so breathing (0.2–0.5 Hz) lands
across bins 5–13 with room to move.

It needs **126 s of history** — which is the whole point of the ring.

Per-column gain is undone after the transform so columns stay comparable: a
per-column normalisation would erase exactly the amplitude differences the
image exists to show.

`kw_capture <label>` (map) and `kw_spectro <label>` (spectrogram) emit exactly
these tensors as CSV over the USB console, so training data and inference input
cannot drift apart. Training the model, and converting it to `.inm` with the
licensed `iproai-quantize`, is the remaining gate.

## Fixed point throughout

`kw307_analysis.c` uses NMSIS-DSP's q15 transforms and no float anywhere. It was
written when this FreeRTOS port did not save the FPU context (f0–f31) across a
task switch, although the build is hard-float `ilp32f`: a float in a preemptible
task was silently corrupted as soon as a second task touched one. The port saves
it since 2026-10-09 (`apps/test/ipro_fpu_ctx_test`). The module's magnitudes are
`uint16` to begin with, so q15 is the natural domain anyway.

## Breathing: what the estimate is and is not

`kw307_breath()` runs a 1024-point q15 FFT over the stationary magnitude series
— a 51.2 s window at 20 Hz, 1.17 breaths/min resolution — and reports the peak
in the 6–36 /min band with an SNR against the band median.

**It is an amplitude proxy.** 24 GHz breathing detection normally works on
*phase* (λ/2 = 6.21 mm is a full turn, so one degree is 17.2 µm), and this
module exposes magnitude only, after its own CFAR and tracker and possibly its
own gain control. Treat a low SNR as "no reading", not as a slow breath, and do
not quote a rate from it against a reference belt until someone has actually
run that comparison.

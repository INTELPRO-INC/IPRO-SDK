# PIO SD Host — User Guide (`ipro_pio_sdh_test` v1.0)

A software SD host built on the IPRO7 PIO state machines (GPIO22-27), for
**demo, evaluation, and benchmarking**. Runs alongside the hardware SDH
(GPIO28-33) so both a PIO card and a HW-SDH card can be used at the same time.

> **Why a software SD host?** It gives near-hardware SD throughput using only
> spare GPIOs and firmware — no dedicated SDH block needed, freeing the hardware
> SDH for another card (e.g. an SDIO WiFi module).

---

## 1. What you get

| Capability | Command |
|------------|---------|
| Identify the card | `pio_sdcard info` |
| Format (exFAT) | `pio_sdcard format` |
| Mount existing FS | `pio_sdcard mount` |
| One-shot benchmark | `pio_sdcard benchmark [mb] [dual]` |
| Live progress of a running test | `pio_sdcard status` |
| Endurance / soak | `pio_sdcard fstress_batch`, `fstress_mix` |
| Dual-card (interleaved / concurrent) | `pio_sdcard dual`, `concurrent` |
| Audio + WiFi + SD-write coexistence | `pio_sdcard audiotest` |
| Signal-integrity / write-path knobs | `pio_sdcard pad`, `wauto` |

All heavy work runs in a worker task, so output streams live and the shell
stays responsive. `pio_sdcard help` lists everything.

---

## 2. Hardware setup

### Board layout

```text
        ┌──────────────────────────────┐
        │  ▄▀▄▀▄  antenna              │
        │                          ┌─┐ │
   (1)──┼─▶ ▪▪▪▪▪▪   signal        │ │ │
        │              ◆           │ │ │  ← castellated
        │            IPRO7         │ │ │     pad rows
        │  ▪▪▪▪                    │ │ │
        │  ▪▪▪▪   ▢ CN1   ▢ CN3    │ │ │
   (2)──┼─▶ ▪▪     ⊙RST ⊙BOOT ▪LED └─┘ │
        │                              │
        │  ┌────────────┐              │
   (3)──┼─▶│  microSD   │      ┌─────┐ │
        │  └────────────┘      │USB-C│◀┼──(4)
        └──────────────────────┴─────┴─┘
```

| # | Connector | Purpose |
|---|-----------|---------|
| **1** | PIO SD **signal** connector | CLK, CMD, D0–D3 → GPIO22–27 |
| **2** | Card **power** connector | 3.3 V + GND for the breakout |
| **3** | On-board microSD socket | Hardware SDH (GPIO28–33, CD = GPIO34) |
| **4** | USB-C | Power + USB CDC console |

### The microSD breakout (supplied)

```text
            ┌──────────────────────┐
            │   ┌──────────────┐   │
            │   │   microSD    │   │   ← card goes here,
            │   │    socket    │   │     contacts facing down
            │   └──────────────┘   │
            │  ▫ ▫ ▫ ▫ ▫ ▫ ▫ ▫     │   ← on-board pull-up resistors
            │  ╷ ╷ ╷ ╷ ╷ ╷ ╷ ╷ ╷   │
            └──┴─┴─┴─┴─┴─┴─┴─┴─┴───┘
               │ │ │ │ │ │ │ │ └── CD    leave UNCONNECTED
               │ │ │ │ │ │ │ └──── D1  → GPIO25
               │ │ │ │ │ │ └────── D2  → GPIO26
               │ │ │ │ │ └──────── D3  → GPIO27
               │ │ │ │ └────────── CLK → GPIO22
               │ │ │ └──────────── CMD → GPIO23
               │ │ └────────────── D0  → GPIO24
               │ └──────────────── GND ─┐ power
               └────────────────── 3.3V ┘ connector (2)
```

> **The pin order is not the logical order** — `3.3V GND D0 CMD CLK D3 D2 D1 CD`
> left to right. Read the silkscreen label on every pin; don't count positions.

The breakout carries its own pull-up resistors, which is why the IPRO7 pads are
configured **pull-none** (see below).

### Pinout — microSD breakout ↔ IPRO7

Both sides use the same signal names. **Connect name to name**; the wire colours
in your harness don't matter.

| Breakout pin | Signal | IPRO7 | Goes to |
|--------------|--------|-------|---------|
| `3.3V` | Card power | 3V3 | power connector **(2)** |
| `GND` | Ground | GND | power connector **(2)** |
| `CLK` | Clock (24 MHz) | **GPIO22** | signal connector **(1)** |
| `CMD` | Command | **GPIO23** | signal connector **(1)** |
| `D0` | Data 0 (also busy) | **GPIO24** | signal connector **(1)** |
| `D1` | Data 1 | **GPIO25** | signal connector **(1)** |
| `D2` | Data 2 | **GPIO26** | signal connector **(1)** |
| `D3` | Data 3 | **GPIO27** | signal connector **(1)** |
| `CD` | Card detect | — | **leave unconnected** (PIO host doesn't use it) |

### Wiring steps

1. **Power the board down first** — hot-plugging these signals can damage the GPIO pads.
2. Run `3.3V` and `GND` to the power connector **(2)**. Getting these backwards is
   the one mistake the card will not survive — check twice.
3. Run the six signals to the connector **(1)**, matching name to name.
4. Leave `CD` empty; keep leads short with a GND return alongside the bundle.
5. Power up and verify with `pio_sdcard info` before trusting any result.

### Notes

- **Board 3.3V powers the card directly — no external supply needed.** The pads
  ship at **drive 0 with internal pull-ups off** (the adapter carries external
  pull-ups); this is the validated configuration.
- **Two hosts, two cards**: the PIO host (breakout) mounts at `/piosd`; the
  on-board socket runs the hardware SDH and mounts at `/sdcard`. Both can be
  mounted at once — see §6.

---

## 3. Connect

Console runs over **USB CDC — no UART adapter needed**. Plug the board in and
open the port; it enumerates as `IPRO PIO SDH Test`.

```bash
# macOS / Linux — any serial terminal works
python3 tools/serial_monitor.py /dev/cu.usbmodem*
```

You should see the banner and the `ipro />` prompt.

---

## 4. Quick start (3 steps)

```text
pio_sdcard info          # confirm the card is detected
pio_sdcard format        # fresh exFAT + mount   (ERASES the card)
pio_sdcard benchmark     # write / read / small-file suite + report
```

During any long test, type `pio_sdcard status` to see live progress.

> **Always `format` before benchmarking** a card that may hold an old small
> partition — otherwise the small-file stage can hit ENOSPC. `info` shows the
> *card* capacity, not the *partition* size.

---

## 5. Reading a benchmark report

```
============ PIO SD Host Benchmark ============
 Card : SanDisk SN64G   59.4 GB   4-bit SDHC/SDXC
 Sequential write ..........    7.10 MB/s     <- bulk write throughput
 Sequential read ...........    9.83 MB/s     <- bulk read throughput
 Small-file write ..........      20 files/s  <- 256 KB files (open/close/FAT cost)
 Small-file read ...........      33 files/s
 Data integrity ............ PASS (all verified)   <- MUST be PASS
 RAM: heap free 8152 KB, peak use 257 KB
 RESULT: PASS
```

**Dual-card compare** (`benchmark dual`, needs both cards mounted):
```
                          PIO        HW SDH
 Sequential write     7.10 MB/s    7.50 MB/s
 Sequential read      9.83 MB/s   10.50 MB/s
 Small-file write       20 f/s       22 f/s
 Small-file read        33 f/s       35 f/s
 Data integrity         PASS         PASS
```

### Reference figures (SanDisk SN64G, healthy bench)
| | PIO | HW SDH |
|---|-----|--------|
| Sequential write | ~7.0 MB/s | ~7.5 MB/s |
| Sequential read | ~9.8 MB/s | ~10.5 MB/s |

HW SDH is slightly faster; **PIO reaches ~94% of hardware throughput in
software.** Read-throughput jitter under ~0.1% is the healthy signature.

---

## 6. Dual-card testing

Two cards, one in each socket, both mounted:
```text
sdcard                   # HW SDH -> /sdcard
pio_sdcard mount         # PIO   -> /piosd   (or `format` if no FS)
pio_sdcard dual 32       # interleaved on ONE thread: both cards write/read/verify
pio_sdcard concurrent 64 # SEPARATE tasks at once: each card on its own thread
```
- `dual`: proves both volumes coexist; single-threaded interleave.
- `concurrent`: true two-task parallel. Reports each card's throughput under
  contention + aggregate. (Uses an FS lock — see §8.)

---

## 7. Running audio / WiFi at the same time

The PIO host runs its transfers on a worker task, so it coexists with other
real-time work (audio playback, SDIO WiFi RX). Two rules make it robust:

**Rule 1 — priority:**
```
Audio refill / WiFi RX   HIGH priority   (real-time, must not glitch)
PIO SD write worker      LOW  priority   (yields to the real-time work)
```
A high-priority task **preempts** the PIO write, so audio/WiFi are never
starved. The cost shows up only as reduced PIO write throughput.

**Rule 2 — use the `wauto` write path for concurrent scenarios.**
The default (CPU-paced) write clocks the CRC-token/busy handshake in software;
a long high-priority burst that preempts it mid-block can break the SD timing
and **fail the write**. The SM-autonomous path (`wauto`) self-clocks the
handshake, tolerates preemption, and is faster under load.

```text
pio_sdcard audiotest 25       # music + WiFi-feed write; tests BOTH write paths
pio_sdcard wauto 1            # switch PIO writes to the preemption-tolerant path
```
`audiotest` output (typical):
```
 Scenario                    sustained   audio    write
 feed only ................. 4.00 MB/s   -        OK
 feed + load (CPU-paced) ... 2.77 MB/s   0 miss   WRITE-FAIL  <- fragile
 feed + load (wauto) ....... 4.00 MB/s   0 miss   OK          <- robust
```
Set the `25` to your codec's real load (PCM playback ≈ 0%, MP3 decode ≈ 10-30%).
**Audio never glitches** (higher priority); the item to protect is the PIO
write — `wauto` does that. `WRITE-FAIL` on the CPU-paced row is the expected
demonstration of why `wauto` is required, not a bench fault.

**Takeaway for "WiFi download → store to PIO SD, while playing music":**
feasible — audio and WiFi at high priority, PIO SD write low priority **in
`wauto` mode**. Even a GC-degraded card (§9) at 3.73 MB/s = 30 Mbps keeps up
with typical SDIO WiFi download rates.

---

## 8. Pass criteria

| Item | Healthy |
|------|---------|
| Data integrity | **PASS (required)** |
| Sequential write / read | ~7.0 / ~9.8 MB/s |
| Throughput jitter | < 1% (wild swings = power/contact problem) |
| `dual` / `concurrent` | BOTH VERIFIED |
| `audiotest` audio | 0 missed deadlines |

**Golden rule:** after any physical action (swap card, touch a wire), run
`pio_sdcard fspeed 4096` and confirm the number before continuing. Don't chase
software problems on a marginal bench.

---

## 9. Troubleshooting

| Symptom | Likely cause | Fix |
|---------|--------------|-----|
| Throughput swings wildly | bad contact / weak supply path | reseat the card and wiring; check `pad 0 0` (drive 0, pull-none) is in effect |
| Read OK but write fails | host→card contact (D0/CMD) | check GPIO24 (D0) / GPIO23 (CMD) wiring & solder |
| Slow enumerate passes, full-speed soak fails | supply/contact can't hold sustained current | shorten and double up the VDD/GND jumpers |
| **Sustained write halved (e.g. 3.7 vs 7.0), small write still fast** | **card FTL heavy GC after lots of writes** | `format` (TRIM) or let the card rest 10-30 min |
| `no filesystem` | not formatted / FS damaged | `pio_sdcard format` |
| Small-file ENOSPC | mounted an old small partition | `pio_sdcard format` (full-card exFAT) |
| Write fails under audio/WiFi | preemption breaks CPU-paced write | `pio_sdcard wauto 1` |
| `concurrent` verify BAD | two FatFS users (FF_FS_REENTRANT=0) | the built-in FS lock handles it; in production only ONE task uses FatFS |
| Card not detected after swap | needs re-enumeration | re-run `pio_sdcard mount` / `info` |

---

## 10. Command reference

| Command | Purpose |
|---------|---------|
| `info` | vendor / capacity / bus / heap |
| `format` | exFAT format + mount (**erases**, shows progress) |
| `mount` | mount an existing filesystem |
| `benchmark [mb] [dual]` | full suite; `dual` = PIO vs HW SDH side-by-side |
| `status` | live progress of the running test |
| `fspeed [kb]` | single-file write/read + verify |
| `fstress <w\|r\|x> <idx> [mb]` | large-file write / verify-read / delete |
| `fstress_batch <n> [mb]` | N-file soak + full byte verify |
| `fstress_mix [secs]` | mixed large+small time-bounded soak |
| `dual [mb]` | interleaved both cards (one thread) |
| `concurrent [mb]` | both cards on separate tasks at once |
| `audiotest [load%] [feed_mbps] [mb]` | coexistence: paced WiFi-feed write + music; tests both write paths |
| `wauto <0\|1>` | write path: 0 = CPU-paced (default), 1 = SM-autonomous (preemption-tolerant) |
| `pad <drive 0-3> <pull 0\|1> [smt]` | GPIO pad drive / internal pull (sticky) |
| `help` | full list |

HW SDH commands: `sdcard` (mount /sdcard), `format_sd`, `sdh_speed_test`,
`sdcd` (card-detect auto-mount on GPIO34: `sdcd` verify → `sdcd pol <0|1>` →
`sdcd auto 1`; insert → auto mount, remove → unmount).

⚠ **Destructive:** `format` / `format_sd` / `fstress w` / `dual` / `concurrent`
/ `audiotest` write to the card.

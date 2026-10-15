# wifi_aic8800 — AIC8800 D80 Wi-Fi 5 + BT combo over SDIO (IPRO7 host)

IPRO7 drives an AICSemi **AIC8800 D80** module as SDIO host: the module is the
2.4/5 GHz Wi-Fi 5 + Bluetooth radio, IPRO7 owns the IP stack (lwIP) and — for
Bluetooth — the host stack. The example app is
[`apps/platform/ipro7_aic8800_wifi`](../../../apps/platform/ipro7_aic8800_wifi/README.md).

```
IPRO7 (lwIP + BT host, this component) --SDIO GPIO28-33--> AIC8800 D80 (FullMAC fw) --Wi-Fi/BT-->
                                        WIFI_PWR_EN = GPIO16
```

Enable with `CONFIG_WIFI_AIC8800=y` (needs `CONFIG_COMPONENTS_LWIP_ENABLE`,
`CONFIG_ETHERNET`, `CONFIG_DHCPD`, `CONFIG_USE_PSRAM`, `CONFIG_IPRO7_FLASH_2M`
— the app's `.config` says why each one).

## Vendor package: `aic8800_release/`

AICSemi's IPRO7 customer release **`aic8800_release_d80_20261002`** (received
2026-10-02; the previous drop was 2026-09-07). It is committed byte-for-byte,
including the vendor's own `README.md` and `SHA256SUMS.txt`, and **nothing in the
SDK may edit it** — all 49 hashes must keep verifying:

```bash
cd components/wireless/wifi_aic8800/aic8800_release
tr -d '\r' < SHA256SUMS.txt | awk '{print tolower($1)"  "$2}' | shasum -a 256 -c
```

(`SHA256SUMS.txt` is CRLF and upper-case hex, so `shasum -c` needs that filter.)
35 of the 49 files are CRLF, and the repository root sets `* text=auto eol=lf`,
which would rewrite them on checkout and break every hash — so this directory
carries its own `.gitattributes` with `* -text`. Keep it.

| Path | Role |
|------|------|
| `lib/libsoftutil-aic_wifi.a` | 177 objects, 1.31 MB text: FullMAC host driver (`rwnx_*`/`fhost_*`), wpa_supplicant + hostapd, AIC's mbedTLS wrappers, the IPRO7 platform layer (SDIO shim, FreeRTOS/lwIP glue, Wi-Fi CLI), the SDIO HCI mailbox transport **and** the embedded Wi-Fi/BT firmware. Debug info stripped by the vendor. GCC 13.1.1, `rv32imafc_zba_zbb_zbc_zbs` / `ilp32f`. |
| `include/aic_hci_transport.h`, `include/aic_hci_mailbox.h` | Public Bluetooth transport API (see below). |
| `include/`, `port/inc/` | Wi-Fi API and RTOS/SDIO porting headers. |
| `integration/bsp/` | The SDIO HAL / PIO sources the vendor actually tested, for hand-merging into `bsp/` — see "SDK-side merge" below. |
| `example/ipro7_aic8800_wifi/` | Application-side release smoke test. `hci_release_test.c` is copied into our app; `main.c`/`.config` are their copy of ours and are not used. |

What changed against the 2026-09-07 package (`git show b6c5c1705`, which also
still holds that drop's `platform/*.c` reference sources and the `fw/*.h`
firmware arrays):

- **Bluetooth became a public transport API.** `aic_btsdio.c.obj` is gone,
  replaced by `aic_hci_mailbox.c.obj` + `aic_hci_transport.c.obj`. Twelve new
  symbols (`aic_hci_transport_attach/send/detach/stats/buffers`, …); five gone
  (`aic_ble_adv_start/stop`, `aic_ble_test_get/set`, `aic_bt_hci_cmd` — the BLE
  advertising/ATT demo and the raw `aic bthci` command path). Wi-Fi text is
  unchanged to within 510 bytes; `.bss` grows ~17 KB for the HCI queues.
- **The platform sources and the firmware `.h`/`.bin` trees are no longer
  shipped** (−7.7 MB in the repo). The firmware is only inside the archive, so
  `CONFIG_LOAD_FW=1` (load from a flash partition) is not reachable from this
  package.
- **The default SDIO transport is now conservative**: the shim comes up at
  **1-bit for reads *and* writes, 10 MHz, with single-block (512 B) reads through
  PIO** — it announces itself as `diagnostic v2` / `v3`. The previous drop came up
  at 25 MHz with 4-bit writes and the hybrid 1-bit/4-bit read rule. Expect much
  lower throughput out of the box; the tuned path is still there but is now opt-in
  at run time (`aic_ipro_sdio_set_rx_width(4, …)` / `set_rx_phase()` /
  `set_clock()`, or `aic sdio_rx 4 …` / `aic sdio_clk 25`).

Archive build configuration (vendor `CMakeLists.txt` / example): `AIC_CHIP_MODEL=D80`,
`AIC_FEATURE_BT=ON`, `AIC_FEATURE_P2P=OFF`, `AIC_USE_PREBUILT_HOST=ON`,
`CONFIG_DHCPD=ON`.

## Bluetooth: transport only

The driver carries HCI **Command / Event / ACL** over the SDIO mailbox and nothing
else. The host stack is the application's.

```c
#include "aic_hci_mailbox.h"     /* aic_btsdio_init() after Wi-Fi/SDIO is up */
#include "aic_hci_transport.h"   /* attach / send / detach + stats */
```

- Sequence: `aic_wifi_demo_test()` → `aic_btsdio_init()` →
  `aic_hci_transport_attach(&cb, ctx, AIC_HCI_FLOW_HOST)` → host-stack HCI init.
- `AIC_HCI_FLOW_HOST` is the normal choice: the host stack owns command and ACL
  flow control. `AIC_HCI_FLOW_DRIVER` exists only for a lightweight BR/EDR-only
  host with no flow control of its own — never let both sides subtract the same
  credits.
- Max HCI packet is 1023 bytes excluding the H4 type byte, so the **effective ACL
  data MTU is 1019**. The controller reports 1021; the host adapter must clamp
  its own fragment size, because the driver never fragments silently.
- **No SCO/eSCO/ISO transport, and no A2DP/HFP/PCM codec.** Callbacks run on the
  transport worker (never an ISR), RX buffers are borrowed for the duration of the
  callback, and callbacks must not block or detach.
- After a `fault` callback, detach and re-initialise the controller/board for real
  before attaching again. Nothing resets the module automatically, and the module's
  reset is shared with Wi-Fi — the application has to sequence it.

`CONFIG_AIC8800_HCI_SMOKE=y` builds the vendor's acceptance test into the app as
the `hci_info` shell command (BT Reset + version / features / buffer-size /
BD_ADDR). Bring-up only: it must not run while a real host stack is attached.

**Zephyr host on this controller** — `CONFIG_BT_CTLR_AIC8800_SDIO=y` builds
`components/wireless/bluetooth/ipro_bt_host/port/hci_aic_sdio.c`, a Zephyr HCI
driver that is a client of this transport (host flow control). It rewrites the
Controller's Read Buffer Size answer on the way through — ACL MTU 1021 → 1019,
SCO buffers → 0 — because the transport rejects anything larger and has no SCO
path, and it latches a `fault` and stops sending until the module is
re-initialised. That is what gives IPRO7 BR/EDR profiles; the first user is
[`apps/bluetooth/classic/ipro7_aic8800_a2dp`](../../../apps/bluetooth/classic/ipro7_aic8800_a2dp/README.md).
The 2026-09 package's BLE demo had to be cut out of the archive for this
(`ar d aic_btsdio.c.obj`) and BR/EDR needed the chip's BT UART; both are gone
with this release. Verified on IPRO7AI Reef 2026-10-06: the mailbox firmware
reports BR/EDR (LMP byte 4 = 0xd8) and A2DP SBC streams from a macOS source
with no transport errors on the default 1-bit / 10 MHz bus.

## IPRO7 SDIO transport (inside the archive)

- **No card interrupt.** IPRO7's SDH `CARD_INT` does not reach the CPU on the
  tested silicon, so a task polls CCCR `INT_PENDING` (CMD52) every 1 ms while
  traffic is active, 5 ms when idle (`aic_ipro_sdio_set_poll_ms`). Routing DAT1
  through `GPIO_INT0` locks the chip in an interrupt storm while the pads are in
  SDC function and is disabled in the vendor source.
- **Bus defaults** (this release): 1-bit RX+TX, 10 MHz, single-block RX via PIO.
- **The 4-bit path, when enabled:** reads of ≤ 4096 B go 1-bit
  (`sdio_set_read_1bit_max`), larger aggregates go 4-bit with read-CRC ignored
  (`sdio_set_ignore_rd_crc(1)`) and per-line capture taps
  `clk=3 d0..d3=1/1/2/1 inv=1 samp=1`. **Those taps were measured on the fly-wire
  bring-up board — re-sweep them on a real PCB.**
- Pins: SDC D0/D1/CMD/CLK/D3/D2 = GPIO28/29/30/31/32/33 (`GPIO_FUN_SDC`, 96 MHz
  SDH source), `WIFI_PWR_EN` = GPIO16 (active high). No RESET# is driven.
- Driver task priorities 29..31 (`port/inc/plat_port.h`, `TCPIP_PRIORITY=29`);
  `configMAX_PRIORITIES` is 32 on IPRO7, so app tasks that touch the network must
  stay below 29.

Public header for apps: [`include/aic8800_ipro7.h`](include/aic8800_ipro7.h)
(bring-up entry, CLI forward, transport tunables, counters). The vendor's two BT
headers are on the public path as well. The rest of the vendor API is **not**:
its `co_list.h` / `co_math.h` / `compiler.h` have the same names as the BLE
controller's, so an app that needs `wifi_api.h` & co. adds
`${AIC8800_VENDOR_INCLUDE_DIRS}` to its own private include dirs.

## SDK-side merge (`integration/bsp/`)

The vendor ships the three SDIO sources they tested. They are based on a release
package, not on master, so they were merged by hand. Taken:

- `bsp/drivers/soc_common/src/drv_sdh.c` — bound the PIO `BUFFER_READ_READY`
  poll (it could spin for ever) and, after draining the FIFO, **wait for
  `DATA_COMPLETED` before returning**: the card is still clocking out CRC and the
  end bit, and letting the next CMD52/CMD53 onto the bus there corrupts the
  follow-on transfer. This is the "PIO receive fix" the vendor README names.
- `bsp/hal/src/hal_sdio.c` — `sdio_set_small_pio(2)` (single-block reads through
  PIO, TX and bulk DMA untouched); zero `txDataLen`/`rxDataLen` for a PIO transfer,
  because `drv_sdh` uses them as a **post-incremented word index**, not a length,
  so the FIFO drain otherwise wrote 512 bytes past the caller's buffer (this also
  fixes the pre-existing mode-1 path that `atbm_pio 1` could reach); skip the
  post-read cache invalidate for PIO, where the CPU itself stored the bytes.
- Their extra register values in the SDH command-timeout message, folded into the
  one master already prints.

Rejected (taking them would regress the SDK):

- Deleting `SDH_WaitDataHook` / `sdio_set_wait_demotion` —
  `components/network/ipronet_host` registers it, and without it the IPRO6S SDIO
  bridge drops received datagrams under load.
- Deleting the stale data-phase status clear in `SDH_WaitCommandDone`'s caller —
  that is the fix for the first CMD53 read after a write losing its first 32-byte
  burst (SMID `GET_MAC` desync).
- Deleting `host->high_speed = 0` in `sdio_host_reset`, and their revert of
  several comments.
- Their unconditional `[sdio] CMD52 failed` / `R5 error` prints: `hal_sdio` is
  shared with the SD-card and ipronet paths, and the AIC warm-reset probe expects
  CMD52 failures. Callers already log a failed status.

Also note the five root-level SDK files in the drop (`cmake/extension.cmake`,
`bsp/hal/CMakeLists.txt`, the lwIP `CMakeLists.txt`, `lwipopts.h`,
`dhcp_server_raw.c`) are byte-identical to the 2026-09 drop and still carry the
release package's older content — nothing to take from them.

## Open items

- **Partly re-qualified.** On IPRO7AI Reef (2026-10-06) the release brings the
  module up, and BR/EDR + A2DP streaming over the mailbox works. Wi-Fi STA/AP +
  IP traffic and the vendor `hci_info` test have not been re-run with this
  package, and the transport defaults have not been re-tuned for 4-bit.
- **Images do not fit the flash tool's default 2 MB partition table** (FW slot
  0x127000). Both example apps carry `partition_cfg_2M_aic8800.toml` (no OTA,
  FW 0x1F5000); flash with `--skip-ota --pt <it>`.
- The platform layer (pin map, poll cadence, tap defaults) is inside the archive.
  Run-time knobs cover taps/clock/poll/width; changing the pin map needs the
  vendor's platform objects, which this package no longer ships.
- Redistribution terms for the AICSemi archive and firmware are still not
  confirmed — keep this component out of customer release packages until they are.

Background: `docs/wifi-companion/aic8800d80_porting.md` (SDIO ABI reference),
`aic8800d80_hw_bringup.md` (wiring), `aic8800d80_storage_decision.md` (firmware
placement options).

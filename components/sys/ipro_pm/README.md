# ipro_pm — automatic low-power mode selection (FreeRTOS)

`ipro_pm` owns the FreeRTOS tickless-idle hooks and picks, for every idle
window, the deepest mode that the window, the application and the radio allow:

| Mode | What sleeps | Typical use |
|------|-------------|-------------|
| `IPRO_PM_MODE_WFI` | CPU clock gated | short gaps, busy links |
| `IPRO_PM_MODE_NATIVE` | WFI while the BLE core sleeps on its 32 kHz timer | reserved |
| `IPRO_PM_MODE_PDS` | platform PDS31 between BLE connection events | long connection intervals |

Applications never call "sleep". They say what must not happen right now
(hold reasons, vetoes, devices) and the idle task does the rest.

Architecture, flows and timing of the whole connected-PDS path (HAL,
controller, this framework): [`docs/ble/ipro7_connected_pds_architecture.md`](../../../docs/ble/ipro7_connected_pds_architecture.md).

## Enabling it

Low-power defconfig fragment (see `apps/bluetooth/ble/ipro_ble_remote/defconfig_lowpower`):

```
CONFIG_FREERTOS_USE_TICKLESS_IDLE=1
CONFIG_FREERTOS_IDLE_HOOK=y
CONFIG_IPRO_PM=y
CONFIG_IPRO_PM_BLE=y
CONFIG_IPRO_PM_MAX_MODE=3
CONFIG_IPRO_PM_PDS_CONNECTED=y          # forces the controller source build
CONFIG_IPRO_PM_BLE_IDLE_INTERVAL_MS=1000 # ask the central for ~1 s once the link settles
CONFIG_OCRAM_NORET_SPLIT=y
CONFIG_OCRAM_NORET_AS_RAM=y
CONFIG_IPRO_BLE_PDS_WAKE_BUDGET_US=27000
CONFIG_IPRO_BLE_PDS_DEADLINE_GUARD_US=2000
CONFIG_IPRO_BLE_PDS_RXWINSZDEF=16
CONFIG_IPRO_BLE_PDS_RXPATHDLY0=7
CONFIG_IPRO_BLE_PDS_RXPWRUP0=88
```

`ipro_pm_init()` runs from `ipro_platform_init()`, or lazily from the idle hook.
Bring the controller up with `ipro_pm_ble_controller_init(prio)` instead of
`ipro_ble_ctlr_controller_init()`; the framework then follows every connection
through the host's connection callbacks.

## Application API (`include/ipro_pm.h`, `include/ipro_pm_ble.h`)

- `ipro_pm_reason_register(name, max_mode)` + `ipro_pm_hold()/release()`
  (`_isr` variants, `ipro_pm_hold_for_ms()`): cap the depth while something is
  in progress (e.g. an image transfer holds `WFI`).
- `ipro_pm_veto_register(fn, ctx)`: refuse one PDS window (IRQs masked, lock-free).
- `ipro_pm_device_register()`: `suspend` (controller task, may veto),
  `resume_early` (IRQs masked, right after PDS), `resume_late` (timer task).
- `ipro_pm_notifier_register()`: PDS enter/exit/failed, demoted/promoted.
- `ipro_pm_policy_set(mode)`: application ceiling; `ipro_pm_stats_get()` /
  `pm stats` shell: residency per mode and an energy estimate.
- `ipro_pm_ble_conn_param_hint(idle_ms, &p)` and `ipro_pm_ble_params_request(idle_ms)`:
  Apple-compatible parameters (interval multiple of 15 ms, max <= 1995 ms,
  timeout 6 s); the framework requests them itself `CONFIG_IPRO_PM_BLE_IDLE_DELAY_MS`
  after each connection when `CONFIG_IPRO_PM_BLE_IDLE_INTERVAL_MS` is set.
- `ipro_pm_ble_diag_get()`: where the PDS windows of a connection went (grants,
  cycles run, recoveries, and each reject reason, including the ones
  `pds_rejects` does not count); `ipro_ble_lowpower_test` logs it per window.

Rules for applications that want PDS: no periodic polling task (block on a
notification or an IRQ instead), and keep periodic timers as slow as the
product allows — an RTOS deadline inside a connection interval ends that
interval's PDS early. `CONFIG_IPRO_PM_CONSOLE_VETO` keeps a log line from
being cut by PDS; a console command typed while the device is in PDS is lost
(the UART is off).

## How a connected PDS cycle is decided

1. After each connection event the controller publishes the slack to its
   next radio target (a "full window").
2. The FreeRTOS port projects the idle time excluding the controller's 1 ms
   poll, and in `PreSleepRevalidate` offers the window to the BLE backend.
3. The backend revalidates it with the controller (HCI idle, link not in
   recovery, token fresh), sizes the sleep to the joint RTOS/radio budget minus
   the wake reserve, checks holds, vetoes and devices, and grants it.
4. The controller runs snapshot → native sleep commit → PDS31 → restore, and
   reports `cycle_consumed`, `recovered` (first post-PDS anchor) or
   `recovery_failed`. Two failures in a row demote to WFI for 10 s (doubling,
   max 10 min).

## Measured (IPRO7 EVB, RC32K only, central = IPRO7AI on XTAL, 2026-09-27)

| Connection interval | PDS per interval | PDS residency (defaults) | Link loss |
|---|---|---|---|
| 125 ms (controller_test) | ~74 % | 59 % | 0 |
| 500 ms (controller_test) | ~93 % | 66–72 % | 0 |
| 1 s (controller_test) | ~90 % | 66–69 % | 0 |
| 1 s (`ipro_ble_remote` + ipro_pm, 10 min) | ~90 % | **75–78 %** | 0 / 492 cycles |

(controller_test feeds its watchdog every 3 s and prints a summary, which
cuts some intervals short; `ipro_ble_remote` has no such task.)

Model estimate for `ipro_ble_remote` at 1 s: ~245 µA average vs ~655 µA with
WFI only (coefficients in Kconfig; WFI current not yet measured).

## Thread (802.15.4) and several radios

Radios register as *participants* (`ipro_pm_radio.h`); an arbiter decides,
per idle window, who runs the PDS:

| Situation | Executor | Thread's part |
|---|---|---|
| BLE link in the PDS profile | BLE controller task (the cycle above) | device: deadline shortens the sleep, MAC suspended/restored around it |
| no BLE controller | idle task (`CONFIG_IPRO_PM_IDLE_PDS`, `src/pm_exec_idle.c`) | participant: READY / RETRY / BUSY, deadline |
| BLE advertising / short interval | none (WFI) | — |

- Time: MTIME (OpenThread's microsecond clock and the RTOS tick authority)
  is carried across every PDS from HBN RTC edge-synced pairs, the 32 kHz
  clock calibrated against MTIME while awake (`src/pm_lpclk.c`, 250 ms
  windows). The FreeRTOS tickless exit is corrected on the same MTIME epoch
  (`xPortApplicationCorrectElapsedTicks`); without it the port's clock ran
  up to 10 % fast with a receiver on.
- Idle-executor PDS uses domain policy `0xE9` (SoC reset on): without the
  SoC reset a PDS rarely never wakes. The reset clears the 802.15.4 MAC and
  the TIMER clock select: `ot_pm.c` snapshots M154 before each PDS and
  restores it (~100 us), `alarm.c` restores TCCR/TCDR.
- BLE + Thread on one radio: `components/wireless/coex_bz` owns the PTA
  (GLB `BZ_COEX_CTRL`, M154 `COEX_PRIORITY`). A sleepy Thread device needs
  `CONFIG_COEX_BZ_RX_ON_AFTER_ABORT=n` (otherwise the MAC reopens a receive
  window BLE cut short and leaves the receiver on). Start 802.15.4 after the
  BLE host's HCI reset (it re-runs the shared RF bring-up). Both ends of a
  BLE link need the board XTAL trim (`CONFIG_IPRO7_XTAL_CAPCODE`): the ROM
  default is ~68 ppm off and a 1 s interval times out against a trimmed peer.

Configurations: `apps/networking/thread/ipro_thread_ftd_cli/defconfig_ssed_pds`
(SSED, idle executor), `apps/networking/thread/ipro_thread_mtd/defconfig_sed_pm`
(SED), `apps/networking/thread/ipro_thread_ble_lp` (BLE peripheral + Thread).

Measured (IPRO7 EVB, RC32K, nRF52840 parent, 2026-09-28):

| Setup | PDS residency | Notes |
|---|---|---|
| SSED CSL 500 ms, idle executor | 95 % | Thread conformance cases pass under PDS; e2 Parent Request gaps 750/1250 ms |
| SED 1 s poll + BLE 1005 ms | 71–75 % | 0 BLE disconnects, pings 100 % |
| SSED CSL 1 s + BLE 1005 ms | 30–36 % | one PDS per BLE interval (below) |
| SSED CSL 500 ms + BLE 1005 ms | 16 % | same |

`CONFIG_IPRO_PM_THREAD_ALIGN` (experimental, off) moves an SSED's CSL phase
next to the BLE event; it measured worse (0–8 %) because the controller has no
room before its anchor and refuses a late entry after the event.

## Known limits

- RC32K short-term stability on the EVB is ~110–130 ppm per second (measured
  against the BLE XTAL clock), so the first post-PDS window is opened with a
  1000 ppm floor and the residual is learned in flight from the air anchor.
  An XTAL32K board should do better.
- A first post-PDS event lost to ordinary RF interference keeps the link in
  recovery for that interval (no PDS until it re-anchors).
- Only one PDS per connection interval: an RTOS deadline inside the interval
  ends it early and the rest of the interval stays at WFI.
- GPIO wake from PDS is limited to GPIO0–31 by `hal_lp`.
- The console UART cannot wake PDS (GPIO21 is not seen by the PDS pad
  logic): a character received while awake holds WFI for
  `CONFIG_IPRO_PM_CONSOLE_RX_HOLD_MS`. With Thread stopped the part sleeps to
  the next RTOS timer (the OT heartbeat). The UART can also lose the RX
  interrupt of a line that arrives right behind its own output (the bytes
  wait for the next one); a bench script should resend a CR when no echo
  comes (see the bench notes).
- With BLE the controller runs one PDS per connection interval: a Thread CSL
  window or a data poll inside the interval ends it early. More needs the
  controller to publish a fresh window after an early wake.
- Moving data in OCRAM (`.tcmcode` growth ahead of `.data_ocram`/`lp_stack`)
  brought back rare PDS no-wakes under stress on the SSED image; keep the
  layout and re-run the parent-loss stress after changing TCM code size.

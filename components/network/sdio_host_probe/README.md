# sdio_host_probe

Shared IPRO7 SDIO **host** bring-up layer for external-module probe apps.

IPRO7 drives its SDH controller as an SDIO host. Every external module brought
up on it — AIC8800D80, ATBM6162, the IPRO6S ipronet bridge — starts from the
same three steps: put GPIO28-33 on the SDC function, sequence the module's
power/reset lines, then enumerate and negotiate the bus. Three probe apps each
carried their own copy of those steps; this component owns them, so a new
module's probe app is a pin table plus that module's own quirks.

Public API: `sdio_probe.h`. Bench and bring-up only — a shipping module driver
owns its own host (see `components/network/ipronet_host`).

## Two layers, separately selectable

| Layer | Kconfig | Contents |
|---|---|---|
| BOARD | `COMPONENTS_SDIO_HOST_PROBE_ENABLE` | `sdio_probe_board_init` / `_power` / `_reset` / `_power_cycle` / `_wake`. SDH pinmux + module control GPIOs. No card I/O, no host descriptor. |
| BUS | `SDIO_HOST_PROBE_BUS` (default y) | `sdio_probe_start` / `_enumerate` / `_negotiate` / `_run` / `_stop` + the CCCR/CIS/function dumps. Owns one `SDIO_Host_Type` on SDH controller 0. |
| SHELL | `SDIO_HOST_PROBE_SHELL` (default y) | The generic `sdio_*` command table. |

> **Select BOARD only (`CONFIG_SDIO_HOST_PROBE_BUS=n`) for a module whose vendor
> driver calls `sdio_host_init()` itself.** ATBM6162 keeps its own descriptor in
> `atbm_os_sdio.c`; a second one on the same controller would race it.

## Using it

```c
sdio_probe_config_t cfg;
sdio_probe_config_default(&cfg);          /* EVB: PWR/RST/WAKE = GPIO16/17/18 */
cfg.pin_reset_n = SDIO_PROBE_PIN_NONE;    /* this carrier has no RESET# */
cfg.data_func   = SDIO_FUNC_1;

/* Module-specific bring-up registers go in on_post_enable — they run after IO
 * Enable of the data function and before the sequence reports success. */
const sdio_probe_callbacks_t cb = { .on_post_enable = my_module_quirks };

sdio_probe_shell_bind(&cfg, &cb);         /* what sdio_init / sdio_probe use */
```

`sdio_probe_config_default()` gives PWR/RST/WAKE on GPIO16/17/18 (active
high/low), a 5 ms reset with 20 ms settle, and F1 @ 512 B, 4-bit, 25 MHz, high
speed off.

`sdio_probe_power_cycle()` is the only thing that clears a module still running
wedged firmware from a previous run — plain re-enumeration does not. It reuses
`reset_low_ms` as the de-asserted window and `reset_post_ms` as the settle, so a
board with only an enable line still shapes its cycle through those two.

## Shell commands

Setup: `sdio_pins [pwr rst wake]` · `sdio_init` · `sdio_pwr <on|off>` ·
`sdio_reset [low_ms] [post_ms]` · `sdio_cycle` · `sdio_stop` · `sdio_enum` ·
`sdio_width <1|4>` · `sdio_clk <hz>` · `sdio_hs <0|1>` ·
`sdio_blksz <func> <bytes>` · `sdio_en <func>` · `sdio_dis <func>`

Inspection: `sdio_cccr` · `sdio_cis [func]` · `sdio_func <n>`

Transfers: `sdio_readb <func> <addr>` · `sdio_writeb <func> <addr> <val>` ·
`sdio_rdbyte <func> <addr> <len> [fixed]` ·
`sdio_rdblk <func> <addr> <blks> [fixed]` ·
`sdio_wrblk <func> <addr> <blks> [fixed]`

Full sequence: `sdio_probe` — power on, reset, enumerate, negotiate (bus width,
clock, high speed, block size, IO Enable), run `on_post_enable`, then dump the
enumeration, CCCR, F0 CIS and the data function.

`sdio_stop` then `sdio_init` is the supported restart: the second `sdio_init`
goes through `sdio_host_reinit()`, so a bench that re-enumerates repeatedly does
not leak a mutex and a semaphore per cycle.

Arguments parse with `strtoul`, so `0x` forms work everywhere.

## Apps on it

| App | Layers | Keeps |
|---|---|---|
| `apps/test/ipro_sdio_host_probe` | BOARD + BUS + SHELL | Nothing — it *is* the example: a pin table and a banner. |
| `apps/test/ipro_sdio_probe` | BOARD + BUS + SHELL | AIC8800D80 quirks (`FN0[0xF2]=0x7F` and friends) as `on_post_enable`, plus `aic_quirks` to re-apply them alone. |
| `apps/test/ipro_atbm_probe` | BOARD only | Its 25 `atbm_*` A/B tuning commands, which drive the vendor driver. |

## Known loose ends

- **A fourth copy of the pinmux** lives in `ipronet_host`'s `ipronet_sdio.c`. It
  is deliberately not switched over: a production data path should not depend on
  a bench component. Its version was the best of the four (it re-asserts OE after
  `GLB_GPIO_Init()`), so that is the body this component adopted.
- **GPIO30's role is recorded inconsistently** in the comments this was collected
  from: `ipro_sdio_probe` and `ipro_sdio_host_probe` called it CMD, `ipro_atbm_probe`
  called it CLK (carrier wiring confirmed 2026-06-25). All four agree it is the
  one line that must not get a host pull-up, so the code is identical either way
  — but do not quote either label as authoritative without the pinmux xlsx.

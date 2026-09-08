# ipronet_host

FreeRTOS host side of the ipronet SDIO-WiFi bridge. IPRO7 is the SDIO host;
an IPRO6S running `ipro6_if_wifi` is the radio. The component drives the
device over the IPRO7 SDH controller (SMID wire protocol), presents the link
as the lwIP netif `iproeth0` and reports WiFi events through callbacks. Once
`on_got_ip` has fired, the link is a normal netif: use BSD sockets.

```
IPRO7 (your app + ipronet_host) --SDIO 4-bit--> IPRO6S (ipro6_if_wifi) --WiFi--> AP
```

## Minimal program

```c
#include "ipronet_host.h"

static void got_ip(void *arg, const ipronet_ip_info_t *ip)
{
    /* ip->ip4 / mask4 / gw4 / dns1 / dns2 are octets a.b.c.d - open sockets now */
}

void app_main(void)
{
    ipronet_config_t cfg;
    ipronet_config_default(&cfg);                 /* EVB: GPIO15 rail, 25 MHz, reconnect on */
    ipronet_callbacks_t cb = { .on_got_ip = got_ip };
    while (ipronet_start(&cfg, &cb) != IPRONET_OK)   /* device may still be booting */
        vTaskDelay(pdMS_TO_TICKS(1000));
    ipronet_connect("SSID", "psk");
}
```

`ipronet_start()` is synchronous (~1.5 s, ~3.5 s when it power-cycles the
device): SDH init, enumerate, 4-bit / 25 MHz / F1 block 512, RX task, HELLO,
GET_MAC, zero-copy opt-in, `iproeth0` up with the device STA MAC. Every
failure rolls back to idle, so a retry loop is the normal way to wait for a
booting device. `ipronet_stop()` undoes it.

## Prerequisites

- Device: `ipro6_if_wifi` built with `CONFIG_IF_WIFI_AUTO_START` (default y),
  so nothing has to be typed on its console.
- App `.config`, written literally (this build system copies it verbatim):
  `CONFIG_COMPONENTS_LWIP_ENABLE=y`, `CONFIG_ETHERNET=y`,
  `CONFIG_COMPONENTS_IPRONET_HOST_ENABLE=y`, `CONFIG_USE_PSRAM=y`.
- Copy `apps/platform/ipro7_sdio_wifi/lwipopts_app.h` into your app. Without
  it (PSRAM pools, window scale, the fork's TCP loss recovery with dupack
  threshold 3) TCP uplink drops from ~8 Mbps to ~2 Mbps.
- SDH bus pins GPIO28-33 are silicon-fixed and muxed by the component.

## Config (`ipronet_config_t`)

| field | EVB default | meaning |
| --- | --- | --- |
| `pwr_en_gpio` | 15 | IPRO7 GPIO switching the device 3.3 V rail; -1 = host does not control it (then `start()` never power-cycles) |
| `pwr_active_low` | 1 | writing 0 turns the rail ON |
| `bus_hz` | 25000000 | or 50000000 (High Speed) |
| `auto_reconnect` | 1 | re-issue STA_CONNECT after a disconnect until an IP is obtained |
| `tx_pace_kbps` | 16000 | egress pacer; 0 = off (do not: overload wedges the device MAC) |

## Callbacks (`ipronet_callbacks_t`, any member may be NULL)

| callback | when |
| --- | --- |
| `on_connected(arg)` | STA associated |
| `on_disconnected(arg)` | STA lost the AP; also right before the component's own reconnect loop runs |
| `on_got_ip(arg, ip)` | device DHCP finished, `iproeth0` carries the address |
| `on_scan_result(arg, aps, n)` | ~3 s after `ipronet_scan()` |

Link recovery needs exactly one owner. With `auto_reconnect = 1` that owner is
this component (retry until an IP arrives); with 0 it is your application,
which must call `ipronet_connect()` again from `on_disconnected`.
`ipronet_connect()` therefore asks the device to switch its own autoconnect
off (RNM `STA_SET_AUTO_RECONNECT`). Measured 2026-09-02: the `ipro6_if_wifi`
firmware ACKs that request with status 0 and does not act on it, so the device
retries as well and the two can collide after a deauth (the device's
association attempt is torn down, and recovery then takes minutes). Until the
device side is fixed, run `wifi_sta_autoconnect_disable` on the device console
once per boot if you need deterministic recovery.

Rules: callbacks run in the component's RX task, never in ISR context. They
must not block and must not call `ipronet_connect/disconnect/scan/stop/
device_power` (the RX task holds the SDIO host around the read that produced
the event). The state getters (`ipronet_is_connected`, `ipronet_has_ip`,
`ipronet_get_mac`, `ipronet_get_ip`) are safe. Hand the event to your own task
if you need to act on it.

## Return codes

| code | meaning |
| --- | --- |
| `IPRONET_OK` | 0 |
| `IPRONET_ERR_STATE` | wrong order (connect before start, start twice) |
| `IPRONET_ERR_NOMEM` | task / semaphore allocation failed |
| `IPRONET_ERR_SETUP` | SDH host init or bus setup failed |
| `IPRONET_ERR_NO_DEVICE` | enumerate failed or FBR std-IF != WLAN (device silent or still booting) |
| `IPRONET_ERR_NO_MAC` | GET_MAC reply did not arrive within 1 s |
| `IPRONET_ERR_IO` | CMD53 write failed |
| `IPRONET_ERR_PARAM` | bad argument |

## Not provided

- Transport-death detection: a powered-off device makes TX return
  `IPRONET_ERR_IO` and TCP time out; recover with `ipronet_stop()` then
  `ipronet_start()`.
- AP / monitor mode, more than one netif.

## Example and bench

`apps/platform/ipro7_sdio_wifi` is the example app: `ipronet_up <ssid> <psk>`
is the program above as a shell command, and its `ipro6s_*` commands expose
every bring-up stage, counter and A/B knob through `ipronet_host_diag.h`
(bench access, no API promise).

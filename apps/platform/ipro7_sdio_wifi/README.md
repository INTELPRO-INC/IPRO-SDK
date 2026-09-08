# IPRO7 SDIO WiFi Host — example app for `ipronet_host`

IPRO7 runs as the **SDIO host**. It drives an IPRO6S running `ipro6_if_wifi` over
the SDH controller (SMID protocol) and presents the link as the lwIP netif
`iproeth0`, so the IPRO6S is the radio and IPRO7 owns the IP stack.

```
IPRO7 (this app, SDIO host) --SDIO 4-bit--> IPRO6S (ipro6_if_wifi) --WiFi--> AP
```

The host flow itself lives in `components/network/ipronet_host` (public API
`ipronet_host.h`, see its README). This app is the example of using it and the
bench: every stage, counter and knob is a shell command.

## 1. Build both images

Two chips, two images. Run both from the SDK root.

```bash
# IPRO6S device (the radio)
./build_freertos.sh -d defconfig_ipro6_sdio build ipro6_if_wifi
#   -> apps/platform/ipro6_if_wifi/build_ipro6_sdio/build_out/ipro6_if_wifi_IPRO6.bin

# IPRO7 host (this app)
./build_freertos.sh build ipro7_sdio_wifi
#   -> apps/platform/ipro7_sdio_wifi/build/build_out/ipro7_sdio_wifi_IPRO7.bin
```

## 2. Flash

The IPRO6S has no reset button of its own: IPRO7 owns its 3.3 V rail and its BROM
boot strap, so both are driven from the IPRO7 console. Point each `iot.toml` at
the `.bin` built above, then:

```bash
# IPRO6S: put it in BROM download from the IPRO7 shell, then flash its own UART
#   IPRO7 console:  ipro6_dl
cd <flashtool>/ && ./ipro_iot_tool_macos --chipname ipro6 \
    --config chips/ipro6/conf/iot.toml --skip-ota
#   IPRO7 console:  ipro6_rst      # power-cycle back into the new firmware

# IPRO7: the CMSIS-DAP probe is the reset/download line - see the ipro7-probe skill
cd <flashtool>/ && ./ipro_iot_tool_macos --chipname ipro7 \
    --config chips/ipro7/conf/iot.toml --skip-ota
```

Pass is `[All Success]` on both.

## 3. Run

**Nothing has to be typed on the IPRO6S console.** `CONFIG_IF_WIFI_AUTO_START`
(default y) makes the device start its WiFi firmware task and then the SDIO
bridge by itself, about 600 ms after reset.

### The customer path (one command)

```
ipronet_up INTELPRO-LAB02 20210101
```

This is exactly the minimal program from `ipronet_host.h`: `ipronet_start()`
with the EVB config (GPIO15 rail, so it power-cycles the device itself and
waits for it to boot) followed by `ipronet_connect()`. `ipronet_down` is
`ipronet_stop()`.

### Step by step (the bench path)

```
ipro6_rst                                 # power-cycle the device explicitly
ipro6s_probe                              # ipronet_start() without a power-cycle: init -> enum -> setup -> rx_start -> HELLO -> GET_MAC -> ZCEN3 -> iproeth0 up
ipro6s_connect INTELPRO-LAB02 20210101    # host drives assoc + DHCP on the device
```

`ipro6s_net_up` is no longer needed: `start()` brings `iproeth0` up. What a
good run looks like, either way:

```
[ctrl] MAC sta=C2:74:15:00:00:15 ap=C2:74:15:00:00:15
[ctrl] STA CONNECTED
[ctrl] IP 192.168.1.14 / 255.255.255.0 gw 192.168.1.1
```

Those lines are printed by this app's callbacks (`on_connected`, `on_got_ip`).
The MAC line is the GET_MAC handshake — `iproeth0` takes the device's STA MAC,
it is never hardcoded here. The IP line arrives asynchronously afterwards
(`IP_UPDATE_IND`), so `ipro6s_connect` returning is not yet "ready"; wait for it.

Then the link is a normal netif — `iperf -c <server>` to measure it.

## Troubleshooting the first minute

| Symptom | Cause |
|---------|-------|
| `ipro6s_probe` fails with `-4` (`IPRONET_ERR_NO_DEVICE`) | device silent — it has not reached the SDIO bridge, or the host was reset and the device still holds the old session. `ipro6_rst` (or `ipronet_up`, which power-cycles) and retry |
| `iproeth0` never gets an IP | association or DHCP failed on the device side, not on the bus. `ipro6s_link_status` and the device console tell them apart |
| Nothing works after an IPRO7 reset | any IPRO7 or IPRO6S reset invalidates the SDIO session. `ipronet_up` recovers by itself; on the bench path run `ipro6_rst` then `ipro6s_probe` again. Never continue a measurement across one |

## Bench commands

| group | commands |
| --- | --- |
| device reset / download | `ipro6_rst [off_ms]`, `ipro6_dl [off_ms]` (BROM strap on SD_D0), `ipro6_pwr <0\|1>` |
| bring-up stages | `ipro6s_init`, `ipro6s_enum`, `ipro6s_setup [hs]`, `ipro6s_rx_start`, `ipro6s_send_hello`, `ipro6s_get_mac`, `ipro6s_mac`, `ipro6s_net_up`, `ipro6s_probe` |
| control plane | `ipro6s_connect`, `ipro6s_disconnect`, `ipro6s_scan`, `ipro6s_link_status`, `ipro6s_reconn [0\|1]` |
| counters | `ipro6s_tx_stats`, `ipro6s_rx_stats`, `tcpstat` |
| knobs (A/B) | `ipro6s_aggmax <N>`, `ipro6s_zcen <1\|2\|3>`, `ipro6s_pace [kbps]`, `ipro6s_girq <0\|1>`, `tcprto <ticks> [dupack]` |
| ceiling tests | `ipro6s_flood_rx [secs]` (with the device `smid_flood`), `ipro6s_tx_ramp <count> <len>`, `ipro6s_cpu [secs]` |
| raw SDH probes | `ipro6s_doorbell`, `ipro6s_cccr`, `ipro6s_func1`, `ipro6s_readb`, `ipro6s_writeb`, `ipro6s_cmd53_*`, `ipro6s_irq_on`, `ipro6s_irq_wait`, `ipro6s_sdh_int`, `ipro6s_peek`, `ipro6s_poke` |

Bring-up stages, counters and knobs go through `ipronet_host_diag.h` (bench
access, no API promise); the customer never needs it.

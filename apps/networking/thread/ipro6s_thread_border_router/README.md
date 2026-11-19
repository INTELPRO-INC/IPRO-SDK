# IPRO6S Thread Border Router

A Thread Border Router (BR) that connects a Thread network to your Wi-Fi
network. The IPRO6S runs the OpenThread host (Thread 1.4, FTD) and the Wi-Fi
station; an IPRO7AI Reef V4 running the OpenThread RCP firmware is its
802.15.4 radio, connected over UART (Spinel). A second Reef V4 runs a Thread
device, so you can check every step on the bench:

```
 computer ── Wi-Fi AP ──(Wi-Fi)── IPRO6S EVK ──(UART, Spinel)── Reef V4 #1 ~~(Thread)~~ Reef V4 #2
                                  this app                      ipro7_uart_rcp          ipro_thread_ftd_cli
```

What the BR does in this SDK:

- forms or joins a Thread network and routes IPv6 between it and the Wi-Fi
  network (Border Routing: OMR prefix, on-link prefix, route advertisements);
- mDNS / DNS-SD on the Wi-Fi side, SRP server and advertising proxy, DNS-SD
  discovery proxy, Border Agent (MeshCoP service), Backbone Router;
- keeps its Wi-Fi profile and Thread network in flash and resumes both after
  a reboot.

The configuration in this package is the console (standalone) profile: you
set up Wi-Fi and Thread from the BR's console. It is a reference design, not
a Thread-certified product.

## 1. What you need

| Item | Notes |
|---|---|
| IPRO6S EVK (V1.0 or V2 board) | runs this app |
| 2 × IPRO7AI Reef V4 | #1 runs `ipro7_uart_rcp` (the radio), #2 runs `ipro_thread_ftd_cli` (the Thread device) |
| 3 × IPRO Debug Probe, or USB-serial adapters (3.3 V) | console and flashing for each board |
| 3 female-female jumper wires | EVK ↔ Reef #1 UART |
| 2.4 GHz Wi-Fi access point (WPA2-Personal) | the computer joins the same network |
| IntelProCube 0.7.0.1 or later | the flashing tool; `<ICUBE>` below is its folder |

Leave the microSD slot of Reef #1 empty: the RCP's UART uses the slot's
power-switch and card-detect pins.

## 2. Wire the EVK to Reef #1

UART1 of the IPRO6S, 1,000,000 baud, 8N1, no flow control. TX goes to RX.

| Signal | IPRO6S EVK V1.0 board (H1) | IPRO6S EVK V2 board (J_GPIO) | Reef V4 #1 (J_IO) |
|---|---|---|---|
| BR TX → RCP RX | pin 3 (GPIO1) | pin 11 (GPIO1) | pin 15 (GPIO15) |
| BR RX ← RCP TX | pin 1 (GPIO0) | pin 10 (GPIO0) | pin 13 (GPIO14) |
| GND | pin 2 | pin 2 | pin 14 |

Connect each board's console as its user guide shows: the EVK's J_UART and
each Reef's U6, with the IPRO Debug Probe's pin-reversed cable. On macOS use
the `/dev/cu.*` ports, not `/dev/tty.*`.

## 3. Build the three applications

From the SDK root:

```bash
make -C apps/networking/thread/ipro6s_thread_border_router DEFCONFIG=defconfig_evk
make -C apps/networking/thread/ipro7_uart_rcp DEFCONFIG=defconfig_reef
make -C apps/networking/thread/ipro_thread_ftd_cli DEFCONFIG=defconfig_reef
```

Each build ends with the linker's memory table, then `Built target`. The
images are:

| Board | Image |
|---|---|
| IPRO6S EVK | `apps/networking/thread/ipro6s_thread_border_router/build_evk/build_out/ipro6s_thread_border_router_IPRO6.bin` |
| Reef #1 | `apps/networking/thread/ipro7_uart_rcp/build_reef/build_out/ipro7_uart_rcp_IPRO7.bin` |
| Reef #2 | `apps/networking/thread/ipro_thread_ftd_cli/build_reef/build_out/ipro_thread_ftd_cli_IPRO7.bin` |

The BR image is about 1.5 MB; it fits the 2 MB flash of the EVK.

## 4. Flash

**Reef #1 and Reef #2.** Put the board in download mode (hold SW1 BOOT, press
and release SW2 RESET, release SW1), then from `<ICUBE>`:

```bash
./ipro_iot_tool_macos --chipname ipro7 --chip-variant IPRO7AI \
    --port /dev/cu.usbserial-XXXX --baudrate 921600 \
    --pt chips/ipro7/partition/partition_cfg_2M.toml \
    --firmware <SDK>/apps/networking/thread/ipro7_uart_rcp/build_reef/build_out/ipro7_uart_rcp_IPRO7.bin
```

Use the FTD image for Reef #2. On Linux the tool is `./ipro_iot_tool_linux`.
The tool exits 0 even when flashing fails: only `[All Success]` at the end
means success. Press SW2 to start the firmware.

**IPRO6S EVK.** With the IPRO Debug Probe, use the probe's UART port, the one
whose name ends in `02`; the tool enters download mode through the probe:

```bash
./ipro_iot_tool_macos --chipname ipro6 --chip-variant IPRO6S \
    --port /dev/cu.usbmodemXXXX02 --baudrate 2000000 \
    --pt chips/ipro6/partition/partition_cfg_fw_direct.toml \
    --firmware <SDK>/apps/networking/thread/ipro6s_thread_border_router/build_evk/build_out/ipro6s_thread_border_router_IPRO6.bin
```

The BR keeps its Wi-Fi profile and Thread network at flash address 0x1F8000,
outside the firmware partition, so flashing a new BR image keeps them.

## 5. Start the boards

Open three consoles at 115200 baud, for example
`screen /dev/cu.usbmodemXXXX02 115200`.

1. **Reef #1 (RCP).** Press SW2. You should see:

   ```
   IPRO7 OpenThread UART RCP: UART1 TX=GPIO14 RX=GPIO15 @ 1000000
   ```

   The RCP prints nothing else: from here on it talks only to the BR.

2. **Reef #2 (Thread device).** Press SW2. You should see the banner
   `IPRO Thread FTD CLI Application`, then
   `THREAD_DSET: applying compiled-in defaults (first boot)`. The device
   starts the network `IPRO-Thread` on channel 15. After a few seconds:

   ```
   ipro /> ot state
   leader
   Done
   ```

3. **IPRO6S EVK (BR).** Reset the EVK (the flashing tool already did). Within
   a few seconds the BR finds the RCP and prints:

   ```
   IPRO6S RCP host ready, Thread version 5
   ```

   Version 5 is Thread 1.4. If you see
   `IPRO6S BR up without Thread: RCP failure (rcp_status)` instead, go to
   Troubleshooting.

## 6. Connect the BR to Wi-Fi

On the BR console:

```
ipro /> wifi_sta_connect <your SSID> <your password>
```

You should see `Backhaul: Wi-Fi address acquired` after a few seconds.
`wifi_state` shows the connection. Save it so the BR reconnects after a
reboot:

```
ipro /> br_wifi_save
```

You should see `Save result=0 (0=stored)`. The profile is stored without
encryption; `br_wifi_clear` removes it.

## 7. Join the Thread network

Give the BR the same network as the Thread device (these are the device's
compiled-in demo values; use your own for anything other than a bench test):

```
ipro /> ot dataset init new
ipro /> ot dataset networkname IPRO-Thread
ipro /> ot dataset channel 15
ipro /> ot dataset panid 0x1234
ipro /> ot dataset extpanid dead00beef00cafe
ipro /> ot dataset networkkey 00112233445566778899aabbccddeeff
ipro /> ot dataset meshlocalprefix fd11:1111:2200::
ipro /> ot dataset commit active
ipro /> ot ifconfig up
ipro /> ot thread start
```

Each command answers `Done`. Then check, about once a minute:

```
ipro /> ot state
router
Done
```

The BR attaches as `child` first and becomes `router` (or `leader`) within
about two minutes. Check Border Routing:

```
ipro /> ot br state
running
Done
ipro /> ot br omrprefix
Local: fd5a:3d1b:9e2c:1::/64
...
Done
```

Your prefix differs: the BR generates it. That OMR (off-mesh routable) prefix
is what Thread devices use to be reached from the Wi-Fi network.

## 8. Verify

**Inside the Thread network.** On the Thread device, read its addresses:

```
ipro /> ot ipaddr
fd5a:3d1b:9e2c:1:2d3e:...      <- OMR address (the BR's OMR prefix)
fd11:1111:2200:0:...           <- mesh-local addresses
fe80:0:0:0:...                 <- link-local
Done
```

From the BR, ping the device's OMR address:

```
ipro /> ot ping fd5a:3d1b:9e2c:1:2d3e:...
16 bytes from fd5a:3d1b:9e2c:1:2d3e:...: icmp_seq=1 hlim=64 time=40ms
1 packets transmitted, 1 packets received. Packet loss = 0.0%. ...
Done
```

**From the computer, through the BR.** The BR advertises a route to the OMR
prefix on Wi-Fi. On a computer on the same Wi-Fi network:

```bash
ping6 -c 5 fd5a:3d1b:9e2c:1:2d3e:...      # macOS
ping -6 -c 5 fd5a:3d1b:9e2c:1:2d3e:...    # Linux
```

You should see five replies. macOS installs the route by itself. Linux accepts
the route only with `accept_ra_rt_info_max_plen` set:

```bash
sudo sysctl -w net.ipv6.conf.wlan0.accept_ra_rt_info_max_plen=64
```

(replace `wlan0` with your Wi-Fi interface), then wait up to a minute for the
next router advertisement.

**After a reboot.** Type `reboot` on the BR. It reconnects to Wi-Fi with the
saved profile and resumes the Thread network by itself
(`Backhaul: Thread resume result=0`); `ot state` returns to `router` or
`leader` without typing the dataset again.

## Console commands

| Command | What it does |
|---|---|
| `ot <command>` | OpenThread CLI (`ot state`, `ot dataset ...`, `ot ipaddr`, `ot br ...`, `ot ping ...`, `ot srp server ...`) |
| `wifi_sta_connect <ssid> [password]`, `wifi_sta_disconnect`, `wifi_scan`, `wifi_state` | Wi-Fi station |
| `br_wifi_save`, `br_wifi_clear` | save / remove the Wi-Fi profile the BR uses at boot |
| `rcp_status` | RCP link state and recovery counters |
| `rcp_select ipro7` \| `rcp_select <tx> <rx> [baud]` | choose the RCP UART pins (saved; the BR reboots) |
| `br_factory_reset confirm` | erase the saved Wi-Fi profile and Thread network, then reboot |
| `reboot` | restart |

## Configuration

`defconfig_evk` is this package's configuration (`.config` is the same file).
The keys you are most likely to change:

| Key | Default | Meaning |
|---|---|---|
| `CONFIG_THREAD_RCP_UART_TX_PIN` / `_RX_PIN` | 1 / 0 | IPRO6S pins of the RCP UART |
| `CONFIG_THREAD_RCP_UART_BAUD` | 1000000 | Spinel UART baud rate; the RCP must match |
| `CONFIG_WIFI_MEM_PROFILE_BALANCED` | y | Wi-Fi station memory profile; see `apps/platform/ipro6_demo/profile_selection_readme.md`. The PERF profile does not fit next to the BR's network pools |
| `CONFIG_OPENTHREAD_NUM_MESSAGE_BUFFERS` | 256 | OpenThread message buffers |
| `CONFIG_EASYFLASH4_FIXED_PARTITION_OFFSET` | 0x1F8000 | flash area for the Wi-Fi profile and the Thread network |

Any OpenThread RCP that speaks Spinel over UART (RCP API version 4 to 11)
can replace Reef #1: wire it to the same pins and match the baud rate.

## Troubleshooting

| Symptom | Cause | Fix |
|---|---|---|
| `IPRO6S BR up without Thread: RCP failure (rcp_status)` | the BR got no Spinel reply | check TX→RX crossing and GND; Reef #1 runs `ipro7_uart_rcp` from `defconfig_reef`, printed its banner and has no microSD card; same baud on both sides; reset Reef #1, then the BR |
| `ot state` stays `detached` | different network parameters, or the device is off | compare `ot dataset active` on both boards; the channel, PAN ID, extended PAN ID and network key must match |
| `ot br state` is `disabled` or `stopped` | Wi-Fi not connected | `wifi_state`; connect again and `br_wifi_save` |
| The computer gets no reply, the BR does | the computer has no route to the OMR prefix | macOS: wait for the next router advertisement or turn Wi-Fi off and on; Linux: the `accept_ra_rt_info_max_plen` setting above; check that the AP does not block IPv6 multicast between Wi-Fi clients |
| The BR does not reconnect after `reboot` | no saved profile | `br_wifi_save` while connected |
| Flashing ends without `[All Success]` | wrong port, or the board is not in download mode | use the probe port ending in `02` for the EVK; Reef: SW1 + SW2 sequence |

## Files

| File | Contents |
|---|---|
| `main.c` | start-up, OpenThread CLI bridge |
| `wifi_backhaul.c`, `br_wifi_*.c` | Wi-Fi station and its saved profile |
| `br_infra.c`, `br_nd*.c`, `br_route.c`, `br_onlink.c`, `br_thread_netif.c` | infrastructure interface, ND / RA, routing between Thread and Wi-Fi |
| `br_mdns.c`, `br_udp.c`, `br_multicast*.c`, `br_mld_sync.c`, `br_dua_*.c` | mDNS, OpenThread platform UDP, multicast and DUA proxy |
| `br_dhcp6_pd.c`, `br_nat64.c`, `br_dns*.c` | DHCPv6 prefix delegation, NAT64, DNS (off in this configuration) |
| `br_rcp_*.c` | RCP selection, recovery after an RCP reset |
| `br_factory_reset.c`, `br_reset_storage.c` | factory reset |
| `br_matter_*.c` | interfaces for a Matter controller on the same BR; not started in this configuration |
| `partition.toml` | the flash layout this app assumes (same as IntelProCube's `partition_cfg_fw_direct.toml`, plus the 0x1F8000 settings area) |

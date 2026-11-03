# Wi-Fi memory profiles for the IPRO6S standalone station

In `ipro6_demo` the IPRO6S runs the whole Wi-Fi station and the lwIP stack itself.
The same RAM holds the Wi-Fi buffers, the per-packet code that runs from TCM, and
the FreeRTOS heap that your application uses. `CONFIG_WIFI_MEM_PROFILE_*` selects
how that RAM is split. Each profile is one setting, and it sets every Wi-Fi memory
parameter together.

| Profile | Use it when | Free heap after connect | Wi-Fi throughput |
|---|---|---|---|
| `PERF` (default) | Wi-Fi throughput matters most | about 36 KB | 100 % |
| `BALANCED` | general products | about 170 KB | 40 to 75 % |
| `APP` | applications that need most of the RAM | about 210 KB | about 25 to 50 % |

## How to select a profile

Add one line to the defconfig of your build, for example
`apps/platform/ipro6_demo/defconfig_ipro6_lpwifi`:

```
CONFIG_WIFI_MEM_PROFILE_BALANCED=y
```

or

```
CONFIG_WIFI_MEM_PROFILE_APP=y
```

- If there is no such line, the profile is `PERF`.
- The build uses the defconfig as the `.config` without changes. Set at most one
  of the three options.
- In menuconfig, the choice is at Component Modules → Wireless Configuration →
  IPRO6 standalone station memory profile.
- After you change the profile, delete the build directory (for example
  `build_ipro6_lpwifi`) and build again. The profile also changes the generated
  linker script.

## Measured results

| Profile | TCP TX | TCP RX | UDP RX | UDP TX | Free heap after boot | after connect | lowest during the test |
|---|---|---|---|---|---|---|---|
| `PERF` | 30.6 | 32.0 | 47.3 | 44.1 | 71.0 KB | 36.4 KB | 30.7 KB |
| `BALANCED` | 12.6 | 15.2 | 35.5 | 23.1 | 206.0 KB | 171.6 KB | 166.0 KB |
| `APP` | 7.5 | 9.8 | 23.9 | 12.6 | 247.8 KB | 213.5 KB | 207.9 KB |

Throughput is in Mbit/s. TX is from the IPRO6S to the network.

Test conditions:

- Shielding box with one 802.11ax AP on 2.4 GHz, channel 1, 20 MHz, HE20.
- The iperf peer (iperf 2.0.5) is on wired Ethernet.
- The IPRO6S runs the `rw_iperf` shell command.
- 60 s per direction. UDP: 50 Mbit/s offered, 1200 B datagrams.
- Each value is the mean of three rounds.
- CPU clock 240 MHz, the product default.

Free heap is the FreeRTOS heap that the `free` shell command reports. The "total"
value that `free` prints is a configuration constant. The boot log line
`Heap Info` shows the real heap regions. With all three profiles, the `iperf` shell
command (socket-based) also runs in all four directions.

In a real installation, throughput depends on the AP, the signal level and the
interference. Use the percentages to compare the profiles, not the absolute values.

## What each profile changes

| Parameter | PERF | BALANCED | APP | Effect |
|---|---|---|---|---|
| Per-packet code in TCM | all hot objects (about 125 KB) | the densest objects (about 50 KB) | only functions marked `HOTCODE` (about 20 KB) | Code that runs from TCM is much faster than code that runs from flash. This parameter has the largest effect on throughput and on free heap. |
| lwIP heap (`LWIP_HEAP_SIZE`) | 70 KB | 32 KB | 24 KB | Holds every TX packet until it is sent and acknowledged. It sets the TCP send window. |
| TCP send buffer | 34 MSS | 16 MSS | 8 MSS | Must fit in the lwIP heap. |
| TCP receive window | 24 MSS | 8 MSS | 6 MSS | The larger the window, the more RX buffers stay in use. |
| RX buffers | 20 | 12 | 10 | 1.7 KB each. |
| MAC RX buffer | 20 KB | 12 KB | 12 KB | Absorbs bursts of received frames. |
| Block Ack reorder window | 16 | 8 | 8 | |

The SDIO and USB network-card builds (`ipro6_if_wifi`) do not use these profiles.

## Notes for application developers

- Your sockets share the lwIP heap. In `BALANCED` and `APP`, many parallel TCP
  connections that send at the same time can run out of lwIP heap. A write then
  returns `ERR_MEM` and is retried later. This lowers the throughput, but the
  connection stays up.
- `rw_iperf` in UDP mode needs at least 20 KB of lwIP heap. All three profiles
  have this.
- Low-power operation (`wifi_sta_ps_on`) works with all three profiles.

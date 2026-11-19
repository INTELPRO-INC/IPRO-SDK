# IPRO Thread FTD CLI

A Full Thread Device (router-capable) on the IPRO7 with the OpenThread CLI on
the console. It is the Thread device of the IPRO6S Thread Border Router demo
(`apps/networking/thread/ipro6s_thread_border_router`), and a general
starting point for a Thread product: form or join networks, ping, run SRP
and DNS-SD, inspect neighbors and routes.

On first boot it starts a compiled-in demo network; after that it keeps the
active network in flash and rejoins it on every boot.

| Default network (compiled in) | Value |
|---|---|
| Network name | `IPRO-Thread` |
| Channel | 15 |
| PAN ID | `0x1234` |
| Extended PAN ID | `dead00beef00cafe` |
| Network key | `00112233445566778899aabbccddeeff` |
| Mesh-local prefix | `fd11:1111:2200::/64` |

The network key is public: use these values only on a bench. Set your own in
the defconfig (`CONFIG_THREAD_DEFAULT_*`) or with `ot dataset ...` on the
console.

## Build (IPRO7AI Reef V4)

```bash
make -C apps/networking/thread/ipro_thread_ftd_cli DEFCONFIG=defconfig_reef
```

Image: `apps/networking/thread/ipro_thread_ftd_cli/build_reef/build_out/ipro_thread_ftd_cli_IPRO7.bin`.

## Flash

Download mode: hold SW1 (BOOT), press and release SW2 (RESET), release SW1.
From the IntelProCube folder:

```bash
./ipro_iot_tool_macos --chipname ipro7 --chip-variant IPRO7AI \
    --port /dev/cu.usbserial-XXXX --baudrate 921600 \
    --pt chips/ipro7/partition/partition_cfg_2M.toml \
    --firmware <SDK>/apps/networking/thread/ipro_thread_ftd_cli/build_reef/build_out/ipro_thread_ftd_cli_IPRO7.bin
```

Only `[All Success]` at the end means success. Press SW2.

## What you should see

Console on U6 at 115200:

```
=================================================
  IPRO Thread FTD CLI Application
=================================================
...
THREAD_DSET: applying compiled-in defaults (first boot)
```

After a few seconds, alone on the channel, the device is the leader of its
own partition:

```
ipro /> ot state
leader
Done
ipro /> ot networkname
IPRO-Thread
Done
```

On later boots the second line reads
`dataset already commissioned, keeping existing`.

## Try it

| Task | Commands |
|---|---|
| State and role | `ot state`, `ot rloc16`, `ot leaderdata` |
| Addresses | `ot ipaddr`, `ot ipaddr mleid` |
| Neighbors and routers | `ot neighbor table`, `ot router table` |
| Ping | `ot ping <ipv6 address>` |
| Join a different network | `ot thread stop`, `ot dataset clear`, `ot dataset networkkey ...` (and the other fields), `ot dataset commit active`, `ot thread start` |
| Start over with the defaults | `ot factoryreset` (the board restarts and applies the defaults again) |

Two devices with this firmware form one network by themselves. With an
IPRO6S Border Router on the same network, `ot ipaddr` also lists an address
in the Border Router's OMR prefix, which the Wi-Fi network can reach.

## Troubleshooting

| Symptom | Cause | Fix |
|---|---|---|
| `ot state` stays `detached` | another network parameter than its peers | compare `ot dataset active` on both devices |
| Two devices each say `leader` | not on the same network, or out of range | compare the datasets; partitions with the same parameters merge within a minute or two |
| No OMR address in `ot ipaddr` | no Border Router on the network, or it has no Wi-Fi | check `ot br state` on the Border Router |

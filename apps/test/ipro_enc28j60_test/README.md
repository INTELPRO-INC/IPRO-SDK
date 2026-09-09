# ipro_enc28j60_test

ENC28J60 SPI Ethernet **bring-up / raw-frame** test for the **IPRO7 Arduino** board.

This proves the chip is alive over SPI (read silicon + PHY id), configures the
MAC/PHY, and sends/receives **raw Ethernet frames** from an interactive shell.
lwIP / TCP-IP integration is intentionally *not* part of this app — it is the
next step once the bench bring-up is confirmed.

## Wiring

GPIO16/17/19 are a **hardware SPI0 group** (the SPI signal each pin carries is
fixed by its position in the SoC pinmux). CS/RST/INT are plain GPIOs.

| IPRO7 GPIO | Signal | Dir | ENC28J60 pin |
|-----------:|--------|-----|--------------|
| GPIO19 | SPI0 SCLK | → | SCK |
| GPIO16 | SPI0 MOSI | → | SI  |
| GPIO17 | SPI0 MISO | ← | SO  |
| GPIO18 | CS (software) | → | CS  |
| GPIO14 | Reset | → | RST |
| GPIO15 | Interrupt (polled) | ← | INT |
| 3.3–5V | Power | → | VCC |
| GND | Ground | — | GND |

- Module VCC accepts 3.3–5V (onboard 3.3V LDO). IPRO7 GPIOs are 3.3V; the
  ENC28J60 signal pins are 3.3V-safe.
- INT is **polled**, not wired to an interrupt — leave it connected (or floating;
  it has an internal pull-up) and it won't affect the test.
- If `enc id` reports `EREVID=0x00`/`0xFF` with the wiring verified, the data
  lines are most likely crossed: set `ENC_SPI_PIN_SWAP` to `1` in
  `src/enc28j60_port.h`, or physically swap GPIO16/17.

Console + shell run over **USB CDC** — the board enumerates as a USB serial
port (`/dev/cu.usbmodem*` on macOS). No UART adapter needed. A 1200-baud touch
on that port drops into the boot2 USB downloader, so firmware is flashed over
the same USB cable.

## Build / flash

```bash
# from SDK root
./build_freertos.sh test/ipro_enc28j60_test build
# or
make -C apps/test/ipro_enc28j60_test

# flash + monitor (over the USB CDC port)
./build_freertos.sh test/ipro_enc28j60_test flash monitor
```

Binary: `apps/test/ipro_enc28j60_test/build/build_out/ipro_enc28j60_test_IPRO7.bin`

Open the console with any serial monitor on the `/dev/cu.usbmodem*` port (baud
is irrelevant for USB CDC), e.g.:

```bash
python3 tools/serial_monitor.py /dev/cu.usbmodemXXXX
```

## Shell commands

```
enc init [half|full]   reset + configure the chip (default: half-duplex)
enc id                 read EREVID + PHY id + link (SPI sanity check)
enc stat               EPKTCNT / EIR / ESTAT / ECON1 / link snapshot
enc link               PHY link up/down
enc reg  <hex>         read one control register (packed addr, e.g. 0x72)
enc phy  <hex>         read one PHY register (raw addr, e.g. 0x11)
enc prom <on|off>      promiscuous receive (accept every frame)
enc tx   [count]       send broadcast test frame(s) (EtherType 0x88B5)
enc rx                 drain + dump all pending received frames
enc mon  [secs]        poll + dump received frames for N seconds
```

### Typical bench session

```
enc init            # expect rev=0x06, PHY id=0x0083_14xx, link=UP if cabled
enc id
enc tx 3            # send 3 broadcast frames — watch them on a PC with
                    # `sudo tcpdump -i <iface> ether proto 0x88b5`
enc prom on         # accept everything, then
enc mon 10          # dump whatever the chip hears for 10 s
```

## What this does *not* do (yet)

- No lwIP / IP stack, no DHCP, no ping. RX/TX is raw L2 only.
- INT is polled, not interrupt-driven.

These are the natural follow-ups: wrap `enc28j60_send`/`enc28j60_recv` in a
lwIP `netif` (`linkoutput` → `enc28j60_send`; an RX task pumping
`enc28j60_recv` into `netif->input`), enable `CONFIG_LWIP_ENABLE`, and start
DHCP.

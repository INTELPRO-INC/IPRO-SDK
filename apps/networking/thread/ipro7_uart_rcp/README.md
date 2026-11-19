# IPRO7 OpenThread RCP over UART

OpenThread Radio Co-Processor (RCP) firmware: the IPRO7's 802.15.4 radio,
driven by a Thread host over Spinel on UART. The host runs the whole Thread
stack; this firmware only sends, receives, acknowledges and times frames.
The reference host is the IPRO6S Thread Border Router
(`apps/networking/thread/ipro6s_thread_border_router`), whose README walks
through the complete three-board demo.

## Link

| Item | Value |
|---|---|
| Protocol | Spinel over HDLC-lite, OpenThread RCP |
| UART | UART1, 1,000,000 baud, 8N1, no flow control |
| Pins on the IPRO7AI Reef V4 (`defconfig_reef`) | J_IO pin 13 = GPIO14 = RCP TX, pin 15 = GPIO15 = RCP RX, pin 14 = GND |
| Console | UART0 (U6), 115200 baud; prints one line at start |

GPIO14 / GPIO15 are the microSD power switch and card detect on the Reef V4:
leave the microSD slot empty while the RCP runs.

## Build

```bash
make -C apps/networking/thread/ipro7_uart_rcp DEFCONFIG=defconfig_reef
```

Image: `apps/networking/thread/ipro7_uart_rcp/build_reef/build_out/ipro7_uart_rcp_IPRO7.bin`
(about 190 KB).

## Flash

Download mode: hold SW1 (BOOT), press and release SW2 (RESET), release SW1.
From the IntelProCube folder:

```bash
./ipro_iot_tool_macos --chipname ipro7 --chip-variant IPRO7AI \
    --port /dev/cu.usbserial-XXXX --baudrate 921600 \
    --pt chips/ipro7/partition/partition_cfg_2M.toml \
    --firmware <SDK>/apps/networking/thread/ipro7_uart_rcp/build_reef/build_out/ipro7_uart_rcp_IPRO7.bin
```

Only `[All Success]` at the end means success. Press SW2.

## What you should see

On the console (115200):

```
IPRO7 OpenThread UART RCP: UART1 TX=GPIO14 RX=GPIO15 @ 1000000
```

Nothing more is printed. On the host, the IPRO6S Border Router prints
`IPRO6S RCP host ready, Thread version 5` once it has reset the RCP and read
its version and capabilities.

## Use another host or other pins

- Any OpenThread host with a Spinel UART driver (for example `ot-daemon` or
  the OpenThread Border Router on Linux through a USB-UART adapter at
  1,000,000 baud) can drive this firmware.
- To move the UART, change `CONFIG_THREAD_RCP_UART_TX_PIN`,
  `CONFIG_THREAD_RCP_UART_RX_PIN` and `CONFIG_THREAD_RCP_UART_BAUD` in the
  defconfig. On IPRO7 the UART signal of a pin is its GPIO number modulo 12:
  TX and RX must not share that number with the console pins (GPIO20 / GPIO21
  on the Reef V4).

## Troubleshooting

| Symptom | Cause | Fix |
|---|---|---|
| No banner | wrong console port or the board is still in download mode | press SW2; use U6's port at 115200 |
| Banner shows other pins | the `.config` (IPRO7 EVB pins) was built instead of `defconfig_reef` | build with `DEFCONFIG=defconfig_reef` and flash the image under `build_reef/` |
| The host reports no RCP | TX and RX not crossed, no common GND, baud mismatch, or a microSD card in the slot | fix the wiring; remove the card; reset the RCP, then the host |

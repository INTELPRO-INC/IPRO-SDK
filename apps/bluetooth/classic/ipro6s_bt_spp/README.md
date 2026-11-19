# IPRO6S Classic Bluetooth: Serial Port Profile (SPP)

A Bluetooth Classic (BR/EDR) serial port on the IPRO6S. A phone or computer
pairs with the board, opens the serial port, and everything it sends is
printed on the board's console and sent straight back (echo). The console
command `spp send <text>` sends text the other way.

| Item | Value |
|---|---|
| Board | IPRO6S EVK (V1.0 or V2 board), console and flashing through J_UART |
| Bluetooth name | `IPRO6S-SPP` |
| Profile | SPP 1.2: SDP record "IPRO6S Serial Port" (UUID 0x1101) on RFCOMM channel 5 |
| Pairing | Secure Simple Pairing, no PIN (the phone may ask you to confirm); bonds kept in flash |
| Links | one phone or computer at a time |
| Controller | prebuilt `libipro_ble_controller_ipro6s_link1_br_release.a` (BR/EDR + LE) |

## 1. Build

From the SDK root:

```bash
make -C apps/bluetooth/classic/ipro6s_bt_spp DEFCONFIG=defconfig_evk
```

The build ends with the memory table (about 660 KB of flash) and
`Built target ipro6s_bt_spp_IPRO6.elf`. Image:
`apps/bluetooth/classic/ipro6s_bt_spp/build_evk/build_out/ipro6s_bt_spp_IPRO6.bin`.

## 2. Flash

Connect the IPRO Debug Probe to the EVK's J_UART (pin-reversed cable). From
the IntelProCube folder, using the probe's UART port (name ends in `02`):

```bash
./ipro_iot_tool_macos --chipname ipro6 --chip-variant IPRO6S \
    --port /dev/cu.usbmodemXXXX02 --baudrate 2000000 \
    --pt chips/ipro6/partition/partition_cfg_fw_direct.toml \
    --firmware <SDK>/apps/bluetooth/classic/ipro6s_bt_spp/build_evk/build_out/ipro6s_bt_spp_IPRO6.bin
```

Only `[All Success]` at the end means success; the tool resets the board.

## 3. Start

Open the console at 115200 baud (`screen /dev/cu.usbmodemXXXX02 115200`).
You should see, in this order:

```
I (...) SPP: IPRO6S Classic Bluetooth SPP example
I (...) BT: controller ready (BR/EDR + LE)
I (...) SPP: SPP server on RFCOMM channel 5
I (...) BT: name "IPRO6S-SPP", address XX:XX:XX:XX:XX:XX
I (...) BT: discoverable, waiting for a connection
```

## 4. Connect from an Android phone

1. Install a Bluetooth serial terminal app, for example "Serial Bluetooth
   Terminal".
2. In Android **Settings → Bluetooth**, pair with **IPRO6S-SPP**. Confirm if
   the phone asks. The console prints:

   ```
   I (...) BT: connected: XX:XX:XX:XX:XX:XX
   I (...) BT: encrypted, security level 2
   I (...) BT: paired, bond saved
   ```

3. In the terminal app, choose **Devices → Bluetooth Classic → IPRO6S-SPP**
   and connect. The console prints `SPP: serial port open`.
4. Type `hello` in the app and send it. The console prints

   ```
   I (...) SPP: rx 7 bytes: hello..
   ```

   (the two dots are the CR LF the app adds), and the app shows `hello`
   again: that is the board's echo.
5. On the board's console type:

   ```
   ipro /> spp send hi from IPRO6S
   ```

   The app shows `hi from IPRO6S`.
6. Disconnect in the app. The console prints
   `SPP: serial port closed (rx ... bytes, tx ... bytes)`; the board becomes
   discoverable again.

## 5. Connect from a Linux computer (BlueZ)

```bash
bluetoothctl
[bluetooth]# scan on                 # wait for IPRO6S-SPP, note its address
[bluetooth]# pair XX:XX:XX:XX:XX:XX
[bluetooth]# trust XX:XX:XX:XX:XX:XX
[bluetooth]# quit
sudo rfcomm connect /dev/rfcomm0 XX:XX:XX:XX:XX:XX 5
```

In a second terminal, `screen /dev/rfcomm0` (or `picocom /dev/rfcomm0`):
what you type comes back, and the board's console prints it. `Ctrl-C` on
`rfcomm` closes the port.

## Console commands

| Command | What it does |
|---|---|
| `spp status` | link and serial-port state, byte counters |
| `spp send <text>` | send `<text>` plus CR LF to the phone |
| `bt_unpair` | forget every paired device (disconnect first) |
| `reboot` | restart |

## Use it in your product

- Replace the echo in `spp_recv()` (`main.c`) with your protocol; send with
  `spp_send()`.
- `CONFIG_APP_SPP_MTU` (256) is the largest frame either side may send;
  `CONFIG_BT_DEVICE_NAME` and `CONFIG_BT_COD` set what phones display.
- `bt_classic_common.c` holds the bring-up every IPRO6S Classic example
  shares: RF and controller start, host start, bonds in flash at 0x1E8000
  (`CONFIG_EASYFLASH4_FIXED_PARTITION_OFFSET`), and the scan policy. The
  controller carries one BR/EDR link; while it is up the board stops being
  discoverable (Inquiry Scan) but stays connectable, so a bonded phone can
  reconnect.

## Troubleshooting

| Symptom | Cause | Fix |
|---|---|---|
| The phone does not list `IPRO6S-SPP` | the board is connected to another device, or not running | `spp status`; disconnect the other device; check the start-up lines |
| Pairing fails after re-flashing | the phone kept an old bond, the board did not | remove `IPRO6S-SPP` in the phone's Bluetooth settings, `bt_unpair` on the board, pair again |
| The app connects, then `serial port open` never appears | the app connected to a different profile | choose the Bluetooth Classic serial device in the app |
| `serial port is not open` on `spp send` | no app has the port open | connect the terminal app first |
| `rx` lines appear but no echo arrives | the text was longer than the MTU | keep frames under 256 bytes, or raise `CONFIG_APP_SPP_MTU` |

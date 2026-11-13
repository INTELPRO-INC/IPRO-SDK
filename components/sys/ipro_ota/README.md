# ipro_ota

The OTA core every transport shares. A transport receives a boot2 OTA package
(`tools/create_ota.py`: a 512-byte `IPRO_OTA_Ver1.0` header, then the
XZ-compressed image) and hands it to this component. The component stages it
in the FW partition's second slot, checks it, and switches the partition table;
boot2 installs it on the next boot.

| Call | What it does |
|---|---|
| `ota_pkg_parse()` | Reads a package header: type, payload length, versions, SHA-256, whether it is signed. No HAL. |
| `ota_get_fw_b()` | The staging slot (FW slot B) from the running partition table. Fails on a table with one FW slot. |
| `ota_sink_begin_ex()`, `ota_sink()`, `ota_sink_flush()`, `ota_sink_end()` | A sequential stream into the slot, hashed as it goes. With `OTA_SINK_ERASE_AS_WRITTEN` the sink erases each 4 KB right before writing it. |
| `ota_validate()` | The stream just received against its header. |
| `ota_validate_flash()` | A package a transport wrote to the slot itself (out of order, with retransmission) against its header, read back from flash. |
| `ota_mark_fwb_active()` | Switches the FW entry so boot2 applies the package. |
| `ota_sig_*()` (`ipro_ota_sig.h`, `CONFIG_IPRO_OTA_SIGNATURE`) | The signature block of a signed package: policy checks and ECDSA verification. |

Enable it with `CONFIG_IPRO_OTA=y` in the application's `.config` and include
`ipro_ota.h`. The board needs boot2 and a partition table with a second FW slot.

## Transports in the component

Both stage the package in FW slot B, have the core check it, and switch the
partition table. An application may enable both.

### Console UART: `CONFIG_IPRO_OTA_UART`

Adds the shell command `ota_uart <package bytes> [noapply]` (`ipro_ota_uart.h`).
It needs no second UART and no baud rate change, and no code in the application.

    host:  ota_uart 200776
    board: ==OTA_UART_READY:1024==                    the largest payload it takes
    host:  A5 5A  seq(u16)  len(u16)  payload  crc16   one frame, then wait
    board: A5 06  seq(u16)                             taken; A5 15 seq: send it again
    board: ==OTA_UART_OK==                             checked, switched, rebooting
           ==OTA_UART_FAIL:<reason>==

Integers are little-endian; the CRC is CRC-16/CCITT-FALSE over seq, len and
payload. IPRO Workbench's OTA Package page sends a package this way (Send to
Board): 200 KB takes about 22 s at 115200. A host that writes its own sender
must type the command line a character at a time: the shell drops a line that
arrives in one piece.

### BLE: `CONFIG_IPRO_OTA_BLE`

The GATT service 0xFF60 (`ipro_ota_ble.h`): control 0xFF61, data 0xFF62
(numbered packets, written without response), status 0xFF63 (read, notify).
The application registers it once Bluetooth is up and tells it the link:

    #include "ipro_ota_ble.h"

    static void bt_ready(int err)
    {
        ...
        ota_service_init(NULL);             /* or a callback for progress */
        ota_service_register_zephyr();
    }

    static void connected(struct bt_conn *conn, uint8_t err)   { ... ota_service_set_conn(conn); }
    static void disconnected(struct bt_conn *conn, uint8_t r)   { ... ota_service_set_conn(NULL); }

`CONFIG_IPRO_OTA_BLE_RAW_IMAGE` is for a layout without boot2's unpacking: the
transfer is a firmware image and only its CRC32 is checked.

A central caches a peripheral's GATT table by its address, and the host does
not send Service Changed. After firmware with other services ran on the same
board, a Mac kept the old table: its write to the control characteristic
failed with "the handle is invalid", and the same firmware under a new address
took the package. On macOS, turn Bluetooth off and on.

## Users

- `apps/bluetooth/ble/ipro_ble_remote` (`defconfig_reef`): the BLE service and
  the UART command of this component. `include/ota_service.h` forwards to
  `ipro_ota_ble.h`.
- `apps/platform/common/ota/ota_zmodem.c`, `ota_usb.c` (UART, USB CDC): the sink.
- `apps/platform/common/ota/ota_common.[ch]`, `ota_sig.[ch]` are forwarding
  files for applications that still add the old paths to their sources.

A/B execution slots with trial and rollback are a different model and are not
in this component.

## Host tests

    sh components/sys/ipro_ota/test_host/run.sh

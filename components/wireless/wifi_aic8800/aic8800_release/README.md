# AIC8800 D80 / IPRO7 customer release

This is an IPRO7-specific binary driver, not a generic CPU archive.

Kept: D80 Wi-Fi/BT firmware, SDIO 1-bit/10 MHz/PIO receive fixes, Wi-Fi STA/AP,
SDIO HCI Command/Event/ACL transport, queues, flow control, fault reporting.
Removed from the library: manual BT pairing, BLE ATT demo, inquiry/name/connect
CLI, SDP/AVDTP/A2DP diagnostics, SBC test tone, ACL echo tests, fake Controller
selftest and destructive firmware upload probes. Debug symbols are stripped.
The existing SDIO tuning CLI is retained for board integration (not BT tests).
Vendor/platform startup and error logs remain; this is not a silent build.

Files:
- `lib/libsoftutil-aic_wifi.a`: combined platform + private host archive.
- `include/aic_hci_transport.h`, `include/aic_hci_mailbox.h`: BT public interfaces.
- `include/`, `port/inc/`: Wi-Fi API and RTOS/SDIO port interfaces.
- `integration/bsp/`: exact tested IPRO7 SDIO HAL/PIO sources, requiring SDK merge.
- `example/ipro7_aic8800_wifi/`: application-side release smoke test; not in `.a`.
- `SHA256SUMS.txt`: integrity hashes (not an authenticity signature).

BT belongs to the customer Host stack. After SDIO initialization call
`aic_btsdio_init`, attach callbacks in `AIC_HCI_FLOW_HOST`, then initialize HCI.
Only a lightweight BR/EDR-only Host without its own flow control should use
`AIC_HCI_FLOW_DRIVER`; never let both driver and Host subtract the same credits.
Maximum HCI packet is 1023 bytes excluding H4 type; effective ACL data MTU is
1019 bytes. Host adapter must clamp fragmentation size despite Controller
reporting 1021. No SCO/eSCO/ISO transport is included. A2DP/HFP/PCM encoding is
not provided. Runtime callbacks/timeout/recovery contracts are in the public header.
On fault, detach and perform real Controller/board reinitialization before attach.
Coordinate any shared Wi-Fi/BT reset with the application; no automatic board reset.

Release example test sequence:
1. Flash the release project's generated binary.
2. `aic_start`, wait for initialization success.
3. `hci_info`: uses only public Host transport callbacks, sends BT Reset then
   version/features/BR-EDR buffer/BD_ADDR queries. Expected `PASS ret=0`.
4. Wi-Fi STA/AP connectivity and IP traffic tests using existing Wi-Fi CLI.

`hci_info` is an application example, not a product Host. It must not run alongside
an already attached customer stack. Production firmware may omit that source.
`aic bthci ...`/tone-test/transport-test are intentionally absent from this library.
Earlier A10 60-second large-packet and 10-minute uninterrupted sound results
were obtained on the diagnostic build; they are not runtime proof of this newly
assembled release SDK. Release on-board validation and real Host audio integration
must be completed before treating this package as production-qualified.

Licensing: retain existing AICSemi/IntelPro and third-party notices and confirm
authorization for customer distribution. This package does not grant new rights.

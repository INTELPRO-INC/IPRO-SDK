# BLE OTA on IPRO7: MCUboot + MCUmgr SMP over Bluetooth LE

The device advertises as `IPRO7 OTA` with the MCUmgr SMP service; a phone (nRF Connect
Device Manager, iOS/Android) or the `mcumgr` / `smpmgr` CLI uploads a signed image into
slot1, marks it for test and resets; MCUboot (swap-move) swaps it in and the new image
confirms itself on its first boot (`boot_write_img_confirmed()`), so a later reset keeps it.
Boot chain: boot2 -> MCUboot (boot_partition) -> image-0 (slot0).

```bash
# v1 (bootloader + signed app in one FW image, written by west flash)
west build -p always -b ipro7ai_evk --sysbuild ../zephyr/samples/ble_ota -d build/ota
west flash -d build/ota                 # merges mcuboot + zephyr.signed.bin, flashes, resets

# v2 = the OTA payload: build/ota_v2/ble_ota/zephyr/zephyr.signed.bin
west build -p always -b ipro7ai_evk --sysbuild ../zephyr/samples/ble_ota -d build/ota_v2 \
    -- -Dble_ota_CONFIG_MCUBOOT_IMGTOOL_SIGN_VERSION=\"2.0.0\"
```

Phone (nRF Connect Device Manager): connect to `IPRO7 OTA` -> Image -> Upgrade -> pick
`zephyr.signed.bin` -> mode "Test and confirm" (or "Test only": the app confirms itself)
-> Start. Console shows the upload, then after the reset MCUboot's `Swap type: test`
/ `Image version: v2.0.0`, then `IPRO7 BLE OTA, image version 2.0.0` and
`first boot of this image: confirmed`.

Verified 2026-09-21 on ipro7_evb with a Pixel 10 Pro running nRF Connect Device Manager
3.4.1: 324 KB uploaded in ~20 s, "Test only" and "Test and Confirm" both end in
`Swap type: test` -> swap-move (11 s) -> `Image version: v2.0.0` -> `first boot of this
image: confirmed (0)`; the phone reconnects to the new image.

Images are signed with MCUboot's development key (`bootloader/mcuboot/root-rsa-2048.pem`);
replace it for anything real. The image group, OS group (echo, reset, bootloader info)
and BLE packet reassembly (498-byte MTU) are enabled; no file system, no shell group.

Shell: `ota selftest [kb]` erases/writes/verifies the start of slot1 the way the image
upload does (bench check of the flash path while BLE runs). `CONFIG_MCUMGR_TRANSPORT_SHELL`
is also on, so `smpmgr --port <console> --baudrate 2000000 os echo hi` works, but image
upload over the 2 Mbaud console is unreliable on this port (the second chunk goes
unanswered) - use BLE for uploads.

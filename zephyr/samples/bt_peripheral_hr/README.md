# Bluetooth on IPRO7 with Zephyr

The upstream Zephyr Bluetooth samples build unchanged; only the board
overlay/prj fragments in this directory are needed:

    west build -b ipro7_evb zephyr/samples/bluetooth/peripheral_hr -- \
        -DEXTRA_DTC_OVERLAY_FILE=$IPRO_SDK/zephyr/samples/bt_peripheral_hr/ipro7.overlay \
        -DEXTRA_CONF_FILE=$IPRO_SDK/zephyr/samples/bt_peripheral_hr/ipro7.conf

`ipro7.conf` selects the controller archive (`link1_periph`, BT 5.2 profile)
and the 16 KB Exchange Memory it needs; the overlay enables the `bt_hci` node.

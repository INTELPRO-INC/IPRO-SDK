/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 *
 * IPRO6S Classic Bluetooth bring-up shared by the classic examples:
 * radio and controller start, Bluetooth host start, BR/EDR discoverability
 * and bonds kept in flash.
 */
#ifndef BT_CLASSIC_COMMON_H
#define BT_CLASSIC_COMMON_H

#include <stdbool.h>

/* Called once, from the Bluetooth host's context, after bt_enable()
 * succeeded and the bonds were loaded. Register profiles and SDP records
 * here; the board becomes discoverable right after it returns. */
typedef void (*bt_classic_ready_cb_t)(void);

/*
 * Bring up the shared radio front end and the BR/EDR + LE controller.
 * Call from main() after ipro_platform_init() and before the scheduler
 * starts. Returns 0 or the controller's error code.
 */
int bt_classic_controller_init(void);

/*
 * Start the Bluetooth host from a task. on_ready runs when the host is up.
 * Returns 0 or a negative errno.
 */
int bt_classic_host_start(bt_classic_ready_cb_t on_ready);

/* true while a phone or computer holds the BR/EDR link. */
bool bt_classic_connected(void);

#endif /* BT_CLASSIC_COMMON_H */

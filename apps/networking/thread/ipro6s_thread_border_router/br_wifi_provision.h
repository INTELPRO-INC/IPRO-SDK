/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

#pragma once
#include "br_wifi_profile.h"

/* Local, trusted UART bootstrap only, before Matter claims the profile store.
 * The startup task is the sole take/finish/seal caller. Shell input only fills
 * a bounded mailbox; no network, logging or flash operations in the RX path.
 * open is one-shot; a sealed boot cannot reopen. Not an authentication scheme.
 */
void br_wifi_provision_open(void);
int br_wifi_provision_arm(void); /* 0=armed; host must await READY before data. */
/* Exact BW01 record as 208 hexadecimal characters followed by LF (CR ignored).
 * Nonzero consumes the byte BEFORE shell echo/history/dispatch. Invalid/long
 * input is discarded through LF, not reinterpreted as a shell command.
 * Ctrl-C cancels and wipes. No automatic timeout may expose a delayed tail. */
int br_wifi_provision_input(uint8_t data);
/* Output always cleared on no record/error. 0=none, 1=valid, -1=bad record.
 * For a delivered record (valid or bad), finish must follow. */
int br_wifi_provision_take(struct br_wifi_profile *output);
void br_wifi_provision_finish(bool saved);
bool br_wifi_provision_reboot_required(void);
/* 0=permanently sealed, 1=input/storage work or reboot is pending. */
int br_wifi_provision_seal(void);

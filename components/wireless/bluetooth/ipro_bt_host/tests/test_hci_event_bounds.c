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

#include <assert.h>
#include "../port/hci_event_bounds.h"
int main(void)
{
    for (unsigned room = 0; room <= 258; room++)
        for (unsigned payload = 0; payload <= 255; payload++)
            assert(ipro_hci_event_fits(room, payload) == (room >= payload + 2));
    assert(!ipro_hci_event_fits(45, 44));
    assert(ipro_hci_event_fits(257, 255));
    return 0;
}

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

/*
 * IPRO6S platform overrides for the isolated legacy BR/EDR controller.
 *
 * The imported BL616D controller archive deliberately carries weak platform
 * hooks.  Its default IRQ implementation calls bflb_irq_* APIs which are not
 * part of this SDK.  These strong definitions bind the three controller IRQs
 * to IPRO6S ECLIC without changing the proven BLE-only controller backend.
 */

#include <stdint.h>

#include "ipro6.h"
#include "drv_ef_ctrl.h"

#define IPRO6_BTDM_IRQ_LEVEL     3u
#define IPRO6_BTDM_IRQ_PRIORITY  0u

extern int32_t ECLIC_Register_IRQ(IRQn_Type IRQn, uint8_t shv,
                                  ECLIC_TRIGGER_Type trig_mode,
                                  uint8_t lvl, uint8_t priority,
                                  void *handler);

static void ipro6_classic_irq_init(IRQn_Type irq, void *handler)
{
    /* CPU/probe reset does not guarantee that the radio interrupt latch is
     * clear.  Mask and clear before registration, then clear once more before
     * unmasking so a stale controller instance cannot enter the new ISR. */
    ECLIC_DisableIRQ(irq);
    ECLIC_ClearPendingIRQ(irq);
    (void)ECLIC_Register_IRQ(irq, ECLIC_NON_VECTOR_INTERRUPT,
                            ECLIC_LEVEL_TRIGGER, IPRO6_BTDM_IRQ_LEVEL,
                            IPRO6_BTDM_IRQ_PRIORITY, handler);
    ECLIC_ClearPendingIRQ(irq);
    ECLIC_EnableIRQ(irq);
}

void btblecontroller_ble_irq_init(void *handler)
{
    ipro6_classic_irq_init(BLE_IRQn, handler);
}

void btblecontroller_bt_irq_init(void *handler)
{
    ipro6_classic_irq_init(BT_IRQn, handler);
}

void btblecontroller_dm_irq_init(void *handler)
{
    ipro6_classic_irq_init(DM_IRQn, handler);
}

void btblecontroller_ble_irq_enable(uint8_t enable)
{
    if (enable)
        ECLIC_EnableIRQ(BLE_IRQn);
    else
        ECLIC_DisableIRQ(BLE_IRQn);
}

void btblecontroller_bt_irq_enable(uint8_t enable)
{
    if (enable)
        ECLIC_EnableIRQ(BT_IRQn);
    else
        ECLIC_DisableIRQ(BT_IRQn);
}

void btblecontroller_dm_irq_enable(uint8_t enable)
{
    if (enable)
        ECLIC_EnableIRQ(DM_IRQn);
    else
        ECLIC_DisableIRQ(DM_IRQn);
}

int btblecontroller_efuse_read_mac(uint8_t mac[6])
{
    return EF_Ctrl_Read_MAC_Address(mac);
}

uint8_t btblecontrolller_get_chip_version(void)
{
    /* This return value selects a BL616 controller workaround, not the IPRO6S
     * silicon revision.  Never select the BL616-A0 idle path here: it pulses
     * the live BTDM reset whenever the legacy sleep state says no activity,
     * which violates the qualified IPRO6S active-clock baseline.  The A1
     * compatibility path performs an ordinary software wake instead. */
    return 1u;
}

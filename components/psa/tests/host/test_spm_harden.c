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
 * Host test of spm_irq_ok and spm_psm_layout_check on the partition
 * tables of components/psa/README.md and on refused layouts.
 */
#include <string.h>

#include "check.h"
#include "spm_harden.h"

/* FW-A at 0x14000: boot2 maps flash 0x15000 to PSA_XIP_BASE */
#define IMG_OFF 0x15000u

static int layout(uint32_t psm, uint32_t psm_size, uint32_t fw0,
                  uint32_t fw0_size, uint32_t fw1, uint32_t fw1_size)
{
    struct spm_psm_layout l = {
        psm, psm_size, IMG_OFF, { fw0, fw1 }, { fw0_size, fw1_size },
    };

    return spm_psm_layout_check(&l);
}

int main(void)
{
    check(spm_irq_ok(0, 0xC0) && spm_irq_ok(1, 0xC0) &&
          spm_irq_ok(0, 0xC1) && spm_irq_ok(1, 0xC2),
          "M-mode: off, on, off vectored, on with trig bits");
    check(!spm_irq_ok(1, 0xC1), "on and vectored refused");
    check(!spm_irq_ok(0, 0x40) && !spm_irq_ok(1, 0x00) &&
          !spm_irq_ok(0, 0x80),
          "mode S, U or reserved refused, also when off");

    check(layout(0xC4000, 0x20000, 0x14000, 0x60000, 0x74000, 0x50000) ==
              SPM_PSM_OK,
          "1M OTA table: PSM 0xC4000 (alias 0x130AF000)");
    check(layout(0xDA000, 0x20000, 0x14000, 0xC6000, 0, 0) == SPM_PSM_OK,
          "1M boot2 table: PSM 0xDA000 (alias 0x130C5000)");
    check(layout(0x1D4000, 0x20000, 0x14000, 0x100000, 0x114000, 0xC0000) ==
              SPM_PSM_OK,
          "2M OTA table: PSM 0x1D4000");

    check(layout(0x44000, 0x20000, 0x14000, 0x30000, 0, 0) ==
              SPM_PSM_FW_WINDOW,
          "alias 0x1302F000 in the SPE flash window");
    check(layout(0x60000, 0x20000, 0x14000, 0x30000, 0, 0) ==
              SPM_PSM_FW_WINDOW,
          "alias 0x1304B000 in the NSPE flash window");
    check(layout(0x8A000, 0x20000, 0x14000, 0x30000, 0, 0) ==
              SPM_PSM_FW_WINDOW,
          "alias ends 4 KB inside the NSPE window");
    check(layout(0x95000, 0x20000, 0x14000, 0x30000, 0, 0) == SPM_PSM_OK,
          "alias starts at the end of the NSPE window");
    check(layout(0xA0000, 0x20000, 0x14000, 0x60000, 0x74000, 0x50000) ==
              SPM_PSM_FW_ENTRY,
          "PSM inside FW-B, alias outside the window");
    check(layout(0xA0000, 0x20000, 0x100000, 0x10000, 0, 0) ==
              SPM_PSM_BOOT_AREA,
          "PSM below the FW entries");
    check(layout(0x0, 0x12000, 0x14000, 0x60000, 0, 0) == SPM_PSM_NO_ALIAS &&
              layout(0x12000, 0x2000, 0x14000, 0x60000, 0, 0) ==
                  SPM_PSM_NO_ALIAS,
          "PSM on boot2 or the tables: no XIP alias");
    check(layout(0xC4000, 0, 0x14000, 0x60000, 0, 0) == SPM_PSM_SIZE &&
              layout(0xC4800, 0x20000, 0x14000, 0x60000, 0, 0) ==
                  SPM_PSM_SIZE &&
              layout(0xC4000, 0x1F800, 0x14000, 0x60000, 0, 0) ==
                  SPM_PSM_SIZE &&
              layout(0xFFFFF000u, 0x2000, 0x14000, 0x60000, 0, 0) ==
                  SPM_PSM_SIZE,
          "size 0, unaligned address or size, end wraps");
    check(layout(0xF0000000u, 0x0F000000u, 0x14000, 0x60000, 0, 0) ==
              SPM_PSM_NO_ALIAS,
          "alias past the 32-bit space");
    check(layout(0xC4000, 0x20000, 0, 0, 0, 0) == SPM_PSM_NO_ENTRY,
          "no FW entry");
    check(layout(0xC4000, 0x20000, 0xFFFFF000u, 0x2000, 0, 0) ==
              SPM_PSM_FW_ENTRY,
          "FW entry end wraps");
    check(strcmp(spm_psm_reason(SPM_PSM_OK), "ok") == 0 &&
              strcmp(spm_psm_reason(-1), "?") == 0 &&
              strcmp(spm_psm_reason(99), "?") == 0,
          "reason strings, out of range");
    return check_summary();
}

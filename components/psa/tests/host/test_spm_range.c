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
 * Host test for spm_nspe_range_ok, spm_nspe_entry_ok and the copy
 * helpers. Runs every check; exits 1 when any check failed.
 */
#include "check.h"
#include "spm.h"

int main(void)
{
    uint32_t ocram_end = PSA_NSPE_OCRAM_BASE + PSA_NSPE_OCRAM_SIZE;
    uint8_t dst[4] = { 0xA5, 0xA5, 0xA5, 0xA5 };
    const uint8_t src[4] = { 1, 2, 3, 4 };

    check(spm_nspe_range_ok(PSA_NSPE_OCRAM_BASE, 4, 1) == 1,
          "NSPE OCRAM start, write");
    check(spm_nspe_range_ok(0x1101FF00u, 0x200u, 0) == 0,
          "straddles SPE RAM / NSPE OCRAM");
    check(spm_nspe_range_ok(0x21020000u, 4, 0) == 0,
          "non-cacheable alias 0x21020000");
    check(spm_nspe_range_ok(PSA_NSPE_FLASH_BASE, 4, 1) == 0,
          "NSPE flash, write");
    check(spm_nspe_range_ok(PSA_NSPE_FLASH_BASE, PSA_NSPE_FLASH_SIZE, 0) == 1,
          "NSPE flash, read whole region");
    check(spm_nspe_range_ok(0x1e7FFFF0u, 0x20u, 1) == 0,
          "crosses PSRAM end");
    check(spm_nspe_range_ok(PSA_NSPE_OCRAM2_BASE + 0x10u, 0, 1) == 1,
          "len 0 at a valid base");
    check(spm_nspe_range_ok(PSA_SPE_RAM_BASE, 0, 0) == 0,
          "len 0 in SPE RAM");
    check(spm_nspe_range_ok(0xFFFFFFF0u, 0x20u, 0) == 0,
          "base + len overflows");
    check(spm_nspe_range_ok(ocram_end - PSA_NSPE_FRAME_SIZE,
                            PSA_NSPE_FRAME_SIZE, 1) == 1,
          "frame at the end of NSPE OCRAM");
    check(spm_nspe_range_ok(ocram_end - 0x100u, PSA_NSPE_FRAME_SIZE, 1) == 0,
          "frame straddles the end of NSPE OCRAM");

    check(spm_nspe_entry_ok(PSA_NSPE_FLASH_BASE + 0x100u) == 1,
          "entry in NSPE flash");
    check(spm_nspe_entry_ok(PSA_NSPE_OCRAM_BASE) == 1,
          "entry in NSPE OCRAM");
    check(spm_nspe_entry_ok(ocram_end - 2u) == 0,
          "entry straddles the end of NSPE OCRAM");
    check(spm_nspe_entry_ok(PSA_NSPE_OCRAM2_BASE) == 0,
          "entry in NSPE OCRAM2 refused");
    check(spm_nspe_entry_ok(PSA_NSPE_PSRAM_BASE) == 0,
          "entry in PSRAM refused");
    check(spm_nspe_entry_ok(PSA_SPE_FLASH_BASE + 0x100u) == 0,
          "entry in SPE flash refused");
    check(spm_nspe_entry_ok(PSA_SPE_RAM_BASE) == 0,
          "entry in SPE RAM refused");

    check(spm_copy_in(dst, PSA_SPE_RAM_BASE, sizeof(dst)) == -1 &&
          dst[0] == 0xA5,
          "copy_in from SPE RAM refused, dst untouched");
    check(spm_copy_out(PSA_NSPE_FLASH_BASE, src, sizeof(src)) == -1,
          "copy_out to NSPE flash refused");

    return check_summary();
}

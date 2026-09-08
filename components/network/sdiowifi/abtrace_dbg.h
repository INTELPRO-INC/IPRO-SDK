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

/* DEBUG (uncommitted): sdiowifi-side access to the macsw AB_TRACE ring so the
 * SDIO arrival / inject / pool events land in the SAME timeline as the txl
 * trace points. Uses a function shim (abdbg_trace, defined in fhost_tx.c where
 * export_macsw.h is visible) instead of redeclaring ab_trace here — some
 * sdiowifi TUs already see the real struct via transitive includes and a
 * mirror-struct extern would clash.
 *
 * IDs used from sdiowifi (layer 5, keep unique vs macsw side):
 *   0x52 inject start (smid_rx_process entry, val = CMD53 payload len)
 *   0x53 inject done  (val = frames de-aggregated from this buffer)
 *   0x54 tx pool free (val = free_size after ++)
 *   0x55 tx pool drop (val = cumulative drop count low16)
 *   0x58 SMID DNLD_CPL IRQ (val = data_len)
 */
#ifndef ABTRACE_DBG_H
#define ABTRACE_DBG_H

#include <stdint.h>

void abdbg_trace(uint16_t id, uint16_t val);

#define ABDBG_TRACE(tid, tval) abdbg_trace((uint16_t)(tid), (uint16_t)(tval))

#endif /* ABTRACE_DBG_H */

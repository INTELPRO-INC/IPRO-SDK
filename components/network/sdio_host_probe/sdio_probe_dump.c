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
 * sdio_probe_dump.c - console dumps of what enumeration found.
 *
 * The enum / CCCR / function dumps were byte-identical in ipro_sdio_probe and
 * ipro_sdio_host_probe; the CIS tuple decoder existed only in the latter. All
 * of it prints to the console rather than the log, because these are answers to
 * an interactive shell command - the same choice ipronet_host's diag paths make.
 */
#include <stdio.h>

#include "sdio_probe.h"

void sdio_probe_hexdump(const uint8_t *buf, uint32_t len)
{
    if (buf == NULL) {
        return;
    }
    for (uint32_t i = 0; i < len; i += 16) {
        printf("  %04lX:", (unsigned long)i);
        for (uint32_t j = 0; j < 16 && (i + j) < len; j++) {
            printf(" %02X", buf[i + j]);
        }
        printf("\r\n");
    }
}

void sdio_probe_dump_enum(void)
{
    SDIO_Host_Type *host = sdio_probe_host();
    if (host == NULL) {
        return;
    }
    printf("  rca       = 0x%04X\r\n", host->rca);
    printf("  num_funcs = %u\r\n", host->num_funcs);
    printf("  ocr       = 0x%08lX\r\n", (unsigned long)host->ocr);
    printf("  cis_ptr   = 0x%06lX\r\n", (unsigned long)host->common_cis_ptr);
    for (uint8_t f = 1; f <= host->num_funcs; f++) {
        SDIO_Func_Info_Type *fi = &host->funcs[f];
        printf("  F%u: cis=0x%06lX class=0x%02X manf=0x%04X prod=0x%04X "
               "max_blk=%u\r\n",
               f, (unsigned long)fi->cis_ptr, fi->func_class,
               fi->manf_id, fi->prod_id, fi->max_blk_size);
    }
}

int sdio_probe_dump_cccr(void)
{
    SDIO_Host_Type *host = sdio_probe_host();
    if (host == NULL) {
        return SDIO_PROBE_ERR_STATE;
    }
    printf("CCCR dump (F0, 0x00..0x16):\r\n");
    for (uint32_t a = 0x00; a <= 0x16; a++) {
        uint8_t v = 0;
        SDIO_Status_Type st = sdio_readb(host, SDIO_FUNC_0, a, &v);
        if (st != Status_SDIO_Success) {
            printf("  0x%02lX: ERR %d\r\n", (unsigned long)a, (int)st);
            return SDIO_PROBE_ERR_IO;
        }
        printf("  0x%02lX: 0x%02X\r\n", (unsigned long)a, v);
    }
    return SDIO_PROBE_OK;
}

static int cis_tuple_cb(uint8_t code, uint8_t link, const uint8_t *body,
                        uint8_t body_len, void *arg)
{
    (void)link; (void)arg;

    printf("  tuple 0x%02X len %u", code, body_len);
    switch (code) {
    case SDIO_CISTPL_MANFID:
        if (body_len >= 4) {
            printf("  MANFID manf=0x%04X card=0x%04X",
                   (unsigned)(body[0] | (body[1] << 8)),
                   (unsigned)(body[2] | (body[3] << 8)));
        }
        break;
    case SDIO_CISTPL_FUNCID:
        if (body_len >= 1) {
            printf("  FUNCID class=0x%02X%s", body[0],
                   body[0] == 0x0C ? " (WLAN)" : "");
        }
        break;
    case SDIO_CISTPL_VERS_1:
        printf("  VERS_1 \"");
        /* Body is major/minor then NUL-separated ASCII strings, 0xFF ends. */
        for (uint8_t i = 2; i < body_len && body[i] != 0xFF; i++) {
            printf("%c", body[i] ? body[i] : ' ');
        }
        printf("\"");
        break;
    case SDIO_CISTPL_FUNCE:
        printf("  FUNCE");
        for (uint8_t i = 0; i < body_len; i++) {
            printf(" %02X", body[i]);
        }
        break;
    default:
        for (uint8_t i = 0; i < body_len && i < 16; i++) {
            printf(" %02X", body[i]);
        }
        break;
    }
    printf("\r\n");
    return 0;
}

int sdio_probe_dump_cis(uint8_t func)
{
    SDIO_Host_Type *host = sdio_probe_host();
    if (host == NULL) {
        return SDIO_PROBE_ERR_STATE;
    }
    if (func > SDIO_MAX_FUNCS) {
        return SDIO_PROBE_ERR_ARG;
    }

    uint32_t start = (func == SDIO_FUNC_0) ? host->common_cis_ptr
                                           : host->funcs[func].cis_ptr;
    if (start == 0) {
        printf("sdio: F%u has no CIS pointer (enumerate first)\r\n", func);
        return SDIO_PROBE_ERR_STATE;
    }
    printf("CIS chain for F%u at 0x%06lX:\r\n", func, (unsigned long)start);

    SDIO_Status_Type st = sdio_walk_cis(host, func, start, cis_tuple_cb, NULL);
    if (st != Status_SDIO_Success) {
        printf("sdio: walk_cis fail (%d)\r\n", (int)st);
        return SDIO_PROBE_ERR_IO;
    }
    return SDIO_PROBE_OK;
}

int sdio_probe_dump_func(uint8_t func)
{
    SDIO_Host_Type *host = sdio_probe_host();
    if (host == NULL) {
        return SDIO_PROBE_ERR_STATE;
    }
    if (func == 0 || func > host->num_funcs) {
        printf("sdio: F%u not present (num_funcs=%u)\r\n",
               func, host->num_funcs);
        return SDIO_PROBE_ERR_ARG;
    }
    SDIO_Func_Info_Type *fi = &host->funcs[func];
    printf("F%u info: enabled=%u  cur_blk=%u  max_blk=%u\r\n",
           func, fi->enabled, fi->cur_blk_size, fi->max_blk_size);
    printf("         class=0x%02X manf=0x%04X prod=0x%04X cis=0x%06lX\r\n",
           fi->func_class, fi->manf_id, fi->prod_id,
           (unsigned long)fi->cis_ptr);
    return SDIO_PROBE_OK;
}

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
 * FMAC download protocol, loader side (fmac_dl.h). Takes requests from
 * any number of buses, writes and checks the image in the download window
 * and starts it. No RTOS, HAL or SDK use: the caller supplies the buses
 * and a platform. Built into the FMAC loader; written to move into a ROM.
 */
#ifndef FMAC_DL_LOADER_H
#define FMAC_DL_LOADER_H

#include <stdint.h>

#include "fmac_dl.h"

/* A transport carrying download packets */
typedef struct {
    uint32_t id;    /* FMAC_BUS_* */
    /* Next received packet, 1 if any; the packet stays valid until release */
    int (*next)(uint8_t **pkt, uint32_t *len);
    void (*release)(uint8_t *pkt);
    int (*send)(const void *msg, uint32_t len);
    /* 1 once every queued send has left the device */
    int (*tx_idle)(void);
} fmac_dl_bus_t;

typedef struct {
    uint32_t loader_ver;
    /* Free-running millisecond clock */
    uint32_t (*now_ms)(void);
    /* Moves the transports while an ack drains before a reboot or a jump */
    void (*poll)(void);
    /* Optional: called for each RUN ack and each failed request */
    void (*log)(const fmac_dl_bus_t *bus, const fmac_dl_ack_t *ack);
    /* Do not return */
    void (*reboot)(void);
    void (*jump)(uint32_t entry, uint32_t bus);
} fmac_dl_plat_t;

void fmac_dl_loader_init(const fmac_dl_plat_t *plat);

/* Serves one request from the bus: 1 if a packet was served. A RUN that
 * passes its checks or a REBOOT does not return. */
int fmac_dl_loader_serve(const fmac_dl_bus_t *bus);

#endif

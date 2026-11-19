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

#ifndef IPRO_BR_ND_H
#define IPRO_BR_ND_H
#include <stddef.h>
#include <stdint.h>

/* Return ICMPv6 offset for a valid ND packet, or zero. Does not consume input. */
int br_nd_validate(const uint8_t *packet, size_t length);
/* Ethernet NS only: permits assigned unicast or DAD's unspecified source.
 * Returns ICMPv6 offset; target is at offset+8. This validates bytes, not
 * authorization to proxy a target or the sender's ownership of an address. */
int br_nd_ns_validate(const uint8_t *packet, size_t length);
#endif

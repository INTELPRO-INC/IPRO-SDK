/**
 * @file ip4_napt.h
 * This is a private interface of ip4_napt used from ip4.c
 *
 * @see ip4_napt.c
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * original reassembly code by Adam Dunkels <adam@sics.se>
 *
 */

#ifndef LWIP_HDR_IP4_NAPT_H
#define LWIP_HDR_IP4_NAPT_H

#include "lwip/opt.h"

#if IP_FORWARD
#if IP_NAPT

#include "lwip/def.h"
#include "lwip/pbuf.h"
#include "lwip/ip4_addr.h"
#include "lwip/err.h"
#include "lwip/netif.h"
#include "lwip/prot/ip4.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/err.h"
#include "lwip/ip4.h"


#ifndef NAPT_TMR_INTERVAL
#define NAPT_TMR_INTERVAL 2000
#endif

/**
 * NAPT for a forwarded packet. It checks weather we need NAPT and modify
 * the packet source address and port if needed.
 *
 * @param p the packet to forward (p->payload points to IP header)
 * @param iphdr the IP header of the input packet
 * @param inp the netif on which this packet was received
 * @param outp the netif on which this packet will be sent
 * @return ERR_OK if packet should be sent, or ERR_RTE if it should be dropped
 */
err_t
ip_napt_forward(struct pbuf *p, struct ip_hdr *iphdr, struct netif *inp, struct netif *outp);

/**
 * NAPT for an input packet. It checks weather the destination is on NAPT
 * table and modify the packet destination address and port if needed.
 *
 * @param p the packet to forward (p->payload points to IP header)
 * @param iphdr the IP header of the input packet
 */
void
ip_napt_recv(struct pbuf *p, struct ip_hdr *iphdr);

/* Check dynamic/static NAPT reservations before allocating a local host port.
 * port is in host byte order. Caller must hold the lwIP core lock. */
u8_t
ip_napt_port_in_use(u8_t proto, u16_t port);

/* Internal pbuf metadata, never a wire flag. ip4_input clears it and only a
 * successful inbound translation sets it. Private translation transports can
 * reject packets routed directly to their internal address without NAPT. */
#define IP_NAPT_RECV_FLAG 0x40U

/* Whether any NAPT table is active; caller holds the lwIP core lock. */
u8_t ip_napt_is_active(void);

/* Invalidate mappings before interface loss, address change, or index reuse.
 * Caller holds the lwIP core lock and the old IPv4 address is still installed. */
void ip_napt_netif_changed(const struct netif *netif);

#if LWIP_ICMP
/* Local IPv4 output only, before fragmentation/netif output. Reserve Echo IDs
 * without rewriting the packet. Errors retain caller ownership. Runs even
 * while NAPT is inactive to protect already-in-flight local queries. */
err_t ip_napt_icmp_output(struct pbuf *p, const struct ip_hdr *iphdr);
#endif

#ifdef __cplusplus
}
#endif

#endif /* IP_NAPT */
#endif /* IP_FORWARD */

#endif /* LWIP_HDR_IP4_NAPT_H */

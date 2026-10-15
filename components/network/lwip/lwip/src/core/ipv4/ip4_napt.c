/**
 * @file
 * This is the NAPT implementation on IPv4 layer
 *
 * @see ip4.c
 *
 */
#include "lwip/opt.h"

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

#if LWIP_IPV4 && IP_NAPT

#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "lwip/sys.h"
#include "lwip/ip.h"
#include "lwip/def.h"
#include "lwip/inet_chksum.h"
#include "lwip/netif.h"
#include "lwip/icmp.h"
#include "lwip/udp.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/lwip_napt.h"
#include "lwip/ip4_napt.h"
#include "lwip/timeouts.h"
#include "lwip/stats.h"

#define NO_IDX ((u16_t)-1)
#define NT(x) ((x) == NO_IDX ? NULL : &ip_napt_table[x])

#pragma GCC diagnostic push
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wc90-c99-compat" /* To allow u8_t bit fields */
#endif
struct ip_napt_entry {
  u32_t last;
  u32_t src;   /* net */
  u32_t dest;  /* net */
  u32_t external_addr; /* net; reject replies arriving at another local address */
  u8_t inside_idx, outside_idx; /* netif indices, invalidated before reuse */
  u16_t sport; /* net */
  u16_t dport; /* net */
  u16_t mport; /* net */
  u8_t proto;
  u8_t fin1 : 1;
  u8_t fin2 : 1;
  u8_t finack1 : 1;
  u8_t finack2 : 1;
  u8_t synack : 1;
  u8_t rst : 1;
  u8_t _unused : 2;
  u32_t src_seqno;  /* host */
  u32_t dest_seqno; /* host */
  u16_t next, prev;
};
#pragma GCC diagnostic pop

struct ip_portmap_entry {
  u32_t maddr;
  u32_t daddr;
  u16_t mport;
  u16_t dport;
  u8_t proto;
  u8_t valid;
};

static u16_t napt_list = NO_IDX, napt_list_last = NO_IDX, napt_free = 0;
static u8_t ip_portmap_max = 0;
static uint16_t ip_napt_max = 0;

static struct ip_napt_entry *ip_napt_table = NULL;
static struct ip_portmap_entry *ip_portmap_table = NULL;

#if LWIP_ICMP
/* This ledger is independent of NAPT enable/disable: a local Echo sent before
 * translation starts must retain its ID. No heap, raw-PCB lifetime or timer
 * callback is required; expired reservations are reclaimed on lookup/send. */
struct ip_napt_icmp_host_entry {
  u32_t last;
  u16_t id; /* network byte order */
  u8_t used;
};
static struct ip_napt_icmp_host_entry ip_napt_icmp_hosts[IP_NAPT_ICMP_HOST_MAX];

static struct ip_napt_icmp_host_entry *
ip_napt_icmp_host_find(u16_t id)
{
  unsigned i;
  u32_t now = sys_now();
  struct ip_napt_icmp_host_entry *match = NULL;
  for (i = 0; i < IP_NAPT_ICMP_HOST_MAX; ++i) {
    struct ip_napt_icmp_host_entry *entry = &ip_napt_icmp_hosts[i];
    if (entry->used && (u32_t)(now - entry->last) >= IP_NAPT_ICMP_HOST_TIMEOUT_MS)
      entry->used = 0;
    if (entry->used && entry->id == id)
      match = entry;
  }
  return match;
}
#endif

u8_t
ip_napt_is_active(void)
{
  LWIP_ASSERT_CORE_LOCKED();
  return ip_napt_max != 0;
}

static void ip_napt_gc(uint32_t now, bool force);
static void ip_napt_tmr(void *arg);

#define WRAPPED_AROUND(a, b) ((((a) ^ (b)) & (1UL << 31)) != 0)

#if NAPT_DEBUG
/* Print NAPT table using LWIP_DEBUGF */
#define DPRINTF(m) LWIP_DEBUGF(NAPT_DEBUG, m)
/* #define DPRINTF(m) printf m */
static void
napt_debug_print(void)
{
  int i, next, p;
  u32_t now = sys_now();
#if LWIP_STATS
  u32_t nr_total = STATS_GET(ip_napt.nr_active_tcp) + STATS_GET(ip_napt.nr_active_udp) + STATS_GET(ip_napt.nr_active_icmp);
  DPRINTF(("NAPT table (%"U16_F"+%"U16_F"+%"U16_F"=%"U32_F" / %"U16_F"):\n",
          STATS_GET(ip_napt.nr_active_tcp), STATS_GET(ip_napt.nr_active_udp), STATS_GET(ip_napt.nr_active_icmp), nr_total, ip_napt_max));
  if (nr_total == 0) return;
#endif

  DPRINTF(("+-----------------------+-----------------------+-------+---------+----------+\n"));
  DPRINTF(("| src                   | dest                  | mport | flags   | age      |\n"));
  DPRINTF(("+-----------------------+-----------------------+-------+---------+----------+\n"));
  for (i = napt_list; i != NO_IDX; i = next) {
     struct ip_napt_entry *t = &ip_napt_table[i];
     next = t->next;

     DPRINTF(("| %3"U16_F".%3"U16_F".%3"U16_F".%3"U16_F":%5"U16_F" ",
              ((const u8_t*) (&t->src))[0],
              ((const u8_t*) (&t->src))[1],
              ((const u8_t*) (&t->src))[2],
              ((const u8_t*) (&t->src))[3],
              lwip_ntohs(t->sport)));

     DPRINTF(("| %3"U16_F".%3"U16_F".%3"U16_F".%3"U16_F":%5"U16_F" ",
              ((const u8_t*) (&t->dest))[0],
              ((const u8_t*) (&t->dest))[1],
              ((const u8_t*) (&t->dest))[2],
              ((const u8_t*) (&t->dest))[3],
              lwip_ntohs(t->dport)));

     p = t->proto;
     DPRINTF(("| %5"U16_F" | %c%c%c%c%c%c%c | %8"U32_F" |\n",
              lwip_ntohs(t->mport),
              (p == IP_PROTO_TCP ? 'T' : (p == IP_PROTO_UDP ? 'U' : (p == IP_PROTO_ICMP ? 'I' : '?'))),
              (t->fin1 ? 'f' : '.'),
              (t->fin2 ? 'F' : '.'),
              (t->finack1 ? 'a' : '.'),
              (t->finack2 ? 'A' : '.'),
              (t->synack ? 'S' : '.'),
              (t->rst ? 'R' : '.'),
              now - t->last));

  }
  DPRINTF(("+-----------------------+-----------------------+-------+---------+----------+\n"));
}
#endif /* NAPT_DEBUG */

/**
 * Deallocates the NAPT tables.
 *
 */
static void
ip_napt_deinit(void)
{
  napt_list = NO_IDX;
  napt_list_last = NO_IDX;
  napt_free = 0;
  ip_napt_max = 0;
  ip_portmap_max = 0;
  if (ip_napt_table != NULL)
    mem_free(ip_napt_table);
  ip_napt_table = NULL;
  if (ip_portmap_table != NULL)
    mem_free(ip_portmap_table);
  ip_portmap_table = NULL;
  sys_untimeout(ip_napt_tmr, NULL);
}

/**
 * Allocates and initializes the NAPT tables.
 *
 * @param max_nat max number of enties in the NAPT table (use IP_NAPT_MAX if in doubt)
 * @param max_portmap max number of enties in the NAPT table (use IP_PORTMAP_MAX if in doubt)
 */
static err_t
ip_napt_init(uint16_t max_nat, uint8_t max_portmap)
{
  u16_t i;

  if (max_nat == 0 || max_nat == NO_IDX || max_nat > IP_NAPT_MAX || max_portmap > IP_PORTMAP_MAX)
    return ERR_ARG;
  if (ip_napt_table != NULL)
    return max_nat == ip_napt_max && max_portmap == ip_portmap_max ? ERR_OK : ERR_USE;
  if (ip_portmap_table == NULL) {
    ip_napt_table = (struct ip_napt_entry *) mem_calloc(max_nat, sizeof(*ip_napt_table));
    if (ip_napt_table == NULL)
      return ERR_MEM;
    if (max_portmap != 0) {
      ip_portmap_table = (struct ip_portmap_entry *) mem_calloc(max_portmap, sizeof(*ip_portmap_table));
      if (ip_portmap_table == NULL) {
        mem_free(ip_napt_table);
        ip_napt_table = NULL;
        return ERR_MEM;
      }
    }

    for (i = 0; i < max_nat - 1; i++)
      ip_napt_table[i].next = i + 1;
    ip_napt_table[i].next = NO_IDX;
    ip_napt_max = max_nat;
    ip_portmap_max = max_portmap;

    sys_timeout(NAPT_TMR_INTERVAL, ip_napt_tmr, NULL);
  }
  return ERR_OK;
}

err_t
ip_napt_enable_netif(struct netif *netif, u16_t max_entries, u8_t max_portmaps)
{
  err_t error;
  LWIP_ASSERT_CORE_LOCKED();
  if (netif == NULL)
    return ERR_ARG;
  error = ip_napt_init(max_entries, max_portmaps);
  if (error == ERR_OK)
    netif->napt = 1;
  return error;
}

static void
ip_napt_set_enabled(struct netif *netif, int enable)
{
  struct netif *other;
  LWIP_ASSERT_CORE_LOCKED();
  if (enable) {
    (void)ip_napt_enable_netif(netif, IP_NAPT_MAX, IP_PORTMAP_MAX);
    return;
  }
  if (netif->napt) {
    ip_napt_netif_changed(netif);
    netif->napt = 0;
  }
  for (other = netif_list; other != NULL; other = other->next)
    if (other->napt)
      return;
  ip_napt_deinit();
}

void
ip_napt_enable(u32_t addr, int enable)
{
  struct netif *netif;
  LWIP_ASSERT_CORE_LOCKED();
  for (netif = netif_list; netif; netif = netif->next) {
    if (!ip4_addr_isany_val(*netif_ip4_addr(netif)) && netif_ip4_addr(netif)->addr == addr)
      ip_napt_set_enabled(netif, enable);
  }
}

void
ip_napt_enable_no(u8_t number, int enable)
{
  struct netif *netif;
  LWIP_ASSERT_CORE_LOCKED();
  for (netif = netif_list; netif; netif = netif->next) {
    if (netif->num == number) {
      ip_napt_set_enabled(netif, enable);
      break;
    }
  }
}

/* adjusts checksum in a packet
- chksum points to the chksum in the packet
- optr points to the old data in the packet (before)
- nptr points to the new data in the packet (after)
*/
static void
checksumadjust(u8_t *chksum, u8_t *optr, int olen, u8_t *nptr, int nlen)
{
  s32_t x, before, after;
  x=chksum[0]*256+chksum[1];
  x=~x & 0xFFFFU;
  while (olen)
  {
    before=optr[0]*256+optr[1]; optr+=2;
    x-=before & 0xFFFFU;
    if (x<=0) { x--; x&=0xFFFFU; }
    olen-=2;
  }
  while (nlen)
  {
    after=nptr[0]*256+nptr[1]; nptr+=2;
    x+=after & 0xFFFFU;
    if (x & 0x10000U) { x++; x&=0xFFFFU; }
    nlen-=2;
  }
  x=~x & 0xFFFFU;
  chksum[0]=x/256; chksum[1]=x & 0xFFU;
}


static void
ip_napt_send_rst(u32_t src_be, u16_t sport_be, u32_t dst_be, u16_t dport_be, u32_t seqno_le, u32_t ackno_le)
{
  struct pbuf *p = pbuf_alloc(PBUF_IP, TCP_HLEN, PBUF_RAM);
  struct tcp_hdr *tcphdr;
  struct netif *netif;
  ip_addr_t src, dst;
  if (p == NULL) return;
  tcphdr = (struct tcp_hdr *)p->payload;
  tcphdr->src = sport_be;
  tcphdr->dest = dport_be;
  tcphdr->seqno = lwip_htonl(seqno_le);
  tcphdr->ackno = lwip_htonl(ackno_le);
  TCPH_HDRLEN_FLAGS_SET(tcphdr, 5, (TCP_RST | TCP_ACK));
  tcphdr->wnd = lwip_htons(512);
  tcphdr->urgp = 0;
  tcphdr->chksum = 0;
  ip_addr_set_ip4_u32_val(src, src_be);
  ip_addr_set_ip4_u32_val(dst, dst_be);
  tcphdr->chksum = ip_chksum_pseudo(p, IP_PROTO_TCP, p->tot_len, &src, &dst);
  netif = ip4_route(ip_2_ip4(&dst));
  if (netif != NULL) {
    err_t res = ip4_output_if(p, ip_2_ip4(&src), ip_2_ip4(&dst), ICMP_TTL, 0, IP_PROTO_TCP, netif);
    LWIP_UNUSED_ARG(res); /* might be unused if debugging off */
    LWIP_DEBUGF(NAPT_DEBUG, ("SEND RST to %"X32_F":%"U16_F" from %"X32_F":%"U16_F" seq %"U32_F" ack %"U32_F" res %"S32_F"\n",
        lwip_ntohl(src_be), lwip_ntohs(sport_be), lwip_ntohl(dst_be), lwip_ntohs(dport_be),
        seqno_le, ackno_le, (s32_t)res));
  }
  pbuf_free(p);
}

/* t must be indexed by napt_free */
static void
ip_napt_insert(struct ip_napt_entry *t)
{
  u16_t ti = t - ip_napt_table;
  assert(ti == napt_free);
  napt_free = t->next;
  t->prev = NO_IDX;
  t->next = napt_list;
  if (napt_list != NO_IDX)
    NT(napt_list)->prev = ti;
  napt_list = ti;
  if (napt_list_last == NO_IDX)
    napt_list_last = ti;

#if LWIP_STATS
#if LWIP_TCP
  if (t->proto == IP_PROTO_TCP)
    STATS_INC(ip_napt.nr_active_tcp);
#endif
#if LWIP_UDP
  if (t->proto == IP_PROTO_UDP)
    STATS_INC(ip_napt.nr_active_udp);
#endif
#if LWIP_ICMP
  if (t->proto == IP_PROTO_ICMP)
    STATS_INC(ip_napt.nr_active_icmp);
#endif
  LWIP_DEBUGF(NAPT_DEBUG, ("ip_napt_insert(): TCP=%d, UDP=%d, ICMP=%d\n",
          STATS_GET(ip_napt.nr_active_tcp), STATS_GET(ip_napt.nr_active_udp), STATS_GET(ip_napt.nr_active_icmp)));
#endif /* LWIP_STATS */
}

static void
ip_napt_unlink(struct ip_napt_entry *t)
{
  u16_t ti = t - ip_napt_table;
  if (ti == napt_list)
    napt_list = t->next;
  if (ti == napt_list_last)
    napt_list_last = t->prev;
  if (t->next != NO_IDX)
    NT(t->next)->prev = t->prev;
  if (t->prev != NO_IDX)
    NT(t->prev)->next = t->next;
  t->prev = NO_IDX;
  t->next = napt_free;
  napt_free = ti;

#if LWIP_STATS
#if LWIP_TCP
  if (t->proto == IP_PROTO_TCP)
    STATS_DEC(ip_napt.nr_active_tcp);
#endif
#if LWIP_UDP
  if (t->proto == IP_PROTO_UDP)
    STATS_DEC(ip_napt.nr_active_udp);
#endif
#if LWIP_ICMP
  if (t->proto == IP_PROTO_ICMP)
    STATS_DEC(ip_napt.nr_active_icmp);
#endif
#endif /* LWIP_STATS */

  LWIP_DEBUGF(NAPT_DEBUG, ("ip_napt_free\n"));
#if NAPT_DEBUG
  napt_debug_print();
#endif
}

static void
ip_napt_free(struct ip_napt_entry *t)
{
  ip_napt_unlink(t);
  /* Send RST to both sides to let them know connection is being evicted */
  if (t->proto == IP_PROTO_TCP && t->synack && !(t->fin1 || t->fin2 || t-> rst)) {
    /* Send RST both ways. */
    ip_napt_send_rst(t->dest, t->dport, t->src, t->sport, t->dest_seqno, t->src_seqno);
    ip_napt_send_rst(t->src, t->sport, t->dest, t->dport, t->src_seqno, t->dest_seqno);
  }
}

static u8_t
ip_napt_find_port(u8_t proto, u16_t port)
{
  int i, next;
  for (i = napt_list; i != NO_IDX; i = next) {
    struct ip_napt_entry *t = &ip_napt_table[i];
    next = t->next;
    if (t->proto == proto && t->mport == port)
      return 1;
  }
  return 0;
}

static struct ip_portmap_entry *
ip_portmap_find(u8_t proto, u16_t mport);

/* All port arguments below are in host byte order. Reserve globally because
 * the NAPT table is shared by all interfaces; never borrow an IPv4 host port. */
static u8_t
ip_napt_host_port_in_use(u8_t proto, u16_t port)
{
#if LWIP_TCP
  if (proto == IP_PROTO_TCP) {
    unsigned i;
    struct tcp_pcb *pcb;
    for (i = 0; i < NUM_TCP_PCB_LISTS; ++i)
      for (pcb = *tcp_pcb_lists[i]; pcb; pcb = pcb->next)
        if (pcb->local_port == port && !IP_IS_V6_VAL(pcb->local_ip))
          return 1;
  }
#endif
#if LWIP_UDP
  if (proto == IP_PROTO_UDP) {
    struct udp_pcb *pcb;
    for (pcb = udp_pcbs; pcb; pcb = pcb->next)
      if (pcb->local_port == port && !IP_IS_V6_VAL(pcb->local_ip))
        return 1;
  }
#endif
#if LWIP_ICMP
  if (proto == IP_PROTO_ICMP)
    return ip_napt_icmp_host_find(lwip_htons(port)) != NULL;
#endif
  return 0;
}

u8_t
ip_napt_port_in_use(u8_t proto, u16_t port)
{
  LWIP_ASSERT_CORE_LOCKED();
  return ip_napt_find_port(proto, lwip_htons(port)) ||
         ip_portmap_find(proto, lwip_htons(port)) != NULL;
}

#if LWIP_ICMP
err_t
ip_napt_icmp_output(struct pbuf *p, const struct ip_hdr *iphdr)
{
  struct icmp_echo_hdr echo;
  struct ip_napt_icmp_host_entry *entry;
  u16_t hlen;
  unsigned i;
  LWIP_ASSERT_CORE_LOCKED();
  if (p == NULL || iphdr == NULL || p->payload != iphdr || p->len < IP_HLEN)
    return ERR_VAL;
  hlen = IPH_HL_BYTES(iphdr);
  if (IPH_V(iphdr) != 4 || hlen < IP_HLEN || hlen > p->len ||
      lwip_ntohs(IPH_LEN(iphdr)) != p->tot_len ||
      (IPH_OFFSET(iphdr) & PP_HTONS(IP_OFFMASK | IP_MF | IP_RF)) != 0 ||
      pbuf_copy_partial(p, &echo, sizeof(echo), hlen) != sizeof(echo))
    return ERR_VAL;
  if (IPH_PROTO(iphdr) != IP_PROTO_ICMP || echo.type != ICMP_ECHO)
    return ERR_OK;
  if (echo.code != 0)
    return ERR_VAL;
  /* Just like a late host bind to a NAPT UDP/TCP port, a host must choose
   * another ID if an active translated query already owns it. */
  if (ip_napt_find_port(IP_PROTO_ICMP, echo.id))
    return ERR_USE;
  entry = ip_napt_icmp_host_find(echo.id);
  if (entry == NULL) {
    for (i = 0; i < IP_NAPT_ICMP_HOST_MAX; ++i) {
      if (!ip_napt_icmp_hosts[i].used) {
        entry = &ip_napt_icmp_hosts[i];
        break;
      }
    }
  }
  if (entry == NULL)
    return ERR_MEM; /* Never evict an unexpired local query's reservation. */
  entry->used = 1;
  entry->id = echo.id;
  entry->last = sys_now();
  return ERR_OK;
}
#endif

static u16_t
ip_napt_new_port(u8_t proto, u16_t port)
{
  const u32_t count = IP_NAPT_PORT_RANGE_END - IP_NAPT_PORT_RANGE_START + 1;
  u32_t attempt;
  u16_t candidate = lwip_ntohs(port);
  if (candidate >= IP_NAPT_PORT_RANGE_START && candidate <= IP_NAPT_PORT_RANGE_END &&
      !ip_napt_port_in_use(proto, candidate) && !ip_napt_host_port_in_use(proto, candidate))
    return port;
  candidate = (u16_t)(IP_NAPT_PORT_RANGE_START + LWIP_RAND() % count);
  /* A random start followed by a bounded scan also works with a repeated RNG
   * value. Port exhaustion must not hang the tcpip thread. */
  for (attempt = 0; attempt < count; ++attempt) {
    if (!ip_napt_port_in_use(proto, candidate) && !ip_napt_host_port_in_use(proto, candidate))
      return lwip_htons(candidate);
    candidate = candidate == IP_NAPT_PORT_RANGE_END ? IP_NAPT_PORT_RANGE_START : candidate + 1;
  }
  return 0;
}

/* Outgoing identity includes both peers and both interfaces. Incoming identity
 * also includes the actual external address; global port reservation alone
 * must not permit a reply to a different local interface address. */
static struct ip_napt_entry *
ip_napt_find_out(u8_t proto, u32_t src, u16_t sport, u32_t dest, u16_t dport,
                 const struct netif *inp, const struct netif *outp)
{
  u16_t i;
  for (i = napt_list; i != NO_IDX; i = NT(i)->next) {
    struct ip_napt_entry *t = NT(i);
    if (t->proto == proto && t->src == src && t->sport == sport &&
        t->dest == dest && t->dport == dport &&
        t->inside_idx == netif_get_index(inp) && t->outside_idx == netif_get_index(outp) &&
        t->external_addr == netif_ip4_addr(outp)->addr) {
      t->last = sys_now();
      return t;
    }
  }
  return NULL;
}

static struct ip_napt_entry *
ip_napt_find_in(u8_t proto, u32_t remote, u16_t port, u16_t mapped, u32_t external)
{
  u16_t i;
  for (i = napt_list; i != NO_IDX; i = NT(i)->next) {
    struct ip_napt_entry *t = NT(i);
    if (t->proto == proto && t->dest == remote && t->dport == port &&
        t->mport == mapped && t->external_addr == external) {
      t->last = sys_now();
      return t;
    }
  }
  return NULL;
}

void
ip_napt_netif_changed(const struct netif *netif)
{
  u16_t i, next;
  u8_t index;
  const struct netif *registered;
  LWIP_ASSERT_CORE_LOCKED();
  /* netif_add invokes the address setter before assigning its final unique
   * index. Ignore that provisional identity until the interface is listed. */
  for (registered = netif_list; registered != NULL && registered != netif;
       registered = registered->next) {}
  if (registered == NULL)
    return;
  index = netif_get_index(netif);
  for (i = napt_list; i != NO_IDX; i = next) {
    struct ip_napt_entry *t = NT(i);
    next = t->next;
    /* A dead link must not emit eviction RST packets on a replacement link. */
    if (t->inside_idx == index || t->outside_idx == index)
      ip_napt_unlink(t);
  }
  /* Static mappings for the old external address must be explicitly installed
   * again after a lifecycle change. Keep unrelated entries densely packed. */
  for (i = 0; i < ip_portmap_max && ip_portmap_table[i].valid;) {
    if (ip_portmap_table[i].maddr == netif_ip4_addr(netif)->addr) {
      ip_portmap_remove(ip_portmap_table[i].proto, lwip_ntohs(ip_portmap_table[i].mport));
    } else {
      ++i;
    }
  }
}

/* Keep one external binding per internal endpoint (RFC 6146 section 5).
 * Peer records remain separate for reply filtering and per-session TCP state;
 * a new peer must not change the external ID/port of an existing binding. */
static u16_t
ip_napt_find_binding(u8_t proto, u32_t src, u16_t sport,
                     const struct netif *inp, const struct netif *outp)
{
  u16_t i;
  for (i = napt_list; i != NO_IDX; i = NT(i)->next) {
    const struct ip_napt_entry *t = NT(i);
    if (t->proto == proto && t->src == src && t->sport == sport &&
        t->inside_idx == netif_get_index(inp) && t->outside_idx == netif_get_index(outp) &&
        t->external_addr == netif_ip4_addr(outp)->addr)
      return t->mport;
  }
  return 0;
}

static u16_t
ip_napt_add(u8_t proto, u32_t src, u16_t sport, u32_t dest, u16_t dport, u32_t seqno,
            const struct netif *inp, const struct netif *outp)
{
  struct ip_napt_entry *t = ip_napt_find_out(proto, src, sport, dest, dport, inp, outp);
  if (t) {
    t->last = sys_now();
    /* Move without sending the eviction RST on a live connection. */
    ip_napt_unlink(t);
    ip_napt_insert(t);

    LWIP_DEBUGF(NAPT_DEBUG, ("ip_napt_add\n"));
#if NAPT_DEBUG
    napt_debug_print();
#endif

    return t->mport;
  }
  t = NT(napt_free);
  if (!t) {
    ip_napt_gc(sys_now(), true /* make_room */);
    t = NT(napt_free);
  }
  if (t) {
    u16_t mport = ip_napt_find_binding(proto, src, sport, inp, outp);
    if (mport == 0)
      mport = ip_napt_new_port(proto, sport);
    if (mport == 0)
      return 0;
    t->last = sys_now();
    t->src = src;
    t->dest = dest;
    t->external_addr = netif_ip4_addr(outp)->addr;
    t->inside_idx = netif_get_index(inp);
    t->outside_idx = netif_get_index(outp);
    t->sport = sport;
    t->dport = dport;
    t->mport = mport;
    t->proto = proto;
    t->fin1 = t->fin2 = t->finack1 = t->finack2 = t->synack = t->rst = 0;
    t->src_seqno = ntohl(seqno);
    t->dest_seqno = 0;
    ip_napt_insert(t);

    LWIP_DEBUGF(NAPT_DEBUG, ("ip_napt_add\n"));
#if NAPT_DEBUG
    napt_debug_print();
#endif

    return mport;
  }
  LWIP_DEBUGF(NAPT_DEBUG, ("ip_napt_add() failed to insert\n"));
  return 0;
}

u8_t
ip_portmap_add(u8_t proto, u32_t maddr, u16_t mport, u32_t daddr, u16_t dport)
{
  int i;
  LWIP_ASSERT_CORE_LOCKED();
  /* Registration must obey the same ownership rule as dynamic allocation.
   * An existing static entry may be updated, but not a host or NAPT flow. */
  if (ip_napt_host_port_in_use(proto, mport) || ip_napt_find_port(proto, lwip_htons(mport)))
    return 0;
  mport = PP_HTONS(mport);
  dport = PP_HTONS(dport);

  for (i = 0; i < ip_portmap_max; i++) {
    struct ip_portmap_entry *p = &ip_portmap_table[i];
    if (p->valid && p->proto == proto && p->mport == mport) {
      p->dport = dport;
      p->daddr = daddr;
      p->maddr = maddr;
      return 1;
    } else if (!p->valid) {
      p->maddr = maddr;
      p->daddr = daddr;
      p->mport = mport;
      p->dport = dport;
      p->proto = proto;
      p->valid = 1;
      return 1;
    }
  }
  return 0;
}

static struct ip_portmap_entry *
ip_portmap_find(u8_t proto, u16_t mport)
{
  int i;
  for (i = 0; i < ip_portmap_max; i++) {
    struct ip_portmap_entry *p = &ip_portmap_table[i];
    if (!p->valid)
      return 0;
    if (p->proto == proto && p->mport == mport)
      return p;
  }
  return NULL;
}

static struct ip_portmap_entry *
ip_portmap_find_dest(u8_t proto, u16_t dport, u32_t daddr)
{
  int i;
  for (i = 0; i < ip_portmap_max; i++) {
    struct ip_portmap_entry *p = &ip_portmap_table[i];
    if (!p->valid)
      return 0;
    if (p->proto == proto && p->dport == dport && p->daddr == daddr)
      return p;
  }
  return NULL;
}

u8_t
ip_portmap_get(u8_t proto, u16_t mport, u32_t *maddr, u32_t *daddr, u16_t *dport)
{
  struct ip_portmap_entry *m = ip_portmap_find(proto, PP_HTONS(mport));
  if (!m)
    return 0;
  *maddr = m->maddr;
  *daddr = m->daddr;
  *dport = PP_NTOHS(m->dport);
  return 1;
}

u8_t
ip_portmap_remove(u8_t proto, u16_t mport)
{
  struct ip_portmap_entry *last = &ip_portmap_table[ip_portmap_max - 1];
  struct ip_portmap_entry *m = ip_portmap_find(proto, PP_HTONS(mport));
  if (!m)
    return 0;
  for (; m != last; m++)
    memcpy(m, m + 1, sizeof(*m));
  last->valid = 0;
  return 1;
}

#if LWIP_TCP
static void
ip_napt_modify_port_tcp(struct tcp_hdr *tcphdr, u8_t dest, u16_t newval)
{
  if (dest) {
    checksumadjust((u8_t *)&tcphdr->chksum, (u8_t *)&tcphdr->dest, 2, (u8_t *)&newval, 2);
    tcphdr->dest = newval;
  } else {
    checksumadjust((u8_t *)&tcphdr->chksum, (u8_t *)&tcphdr->src, 2, (u8_t *)&newval, 2);
    tcphdr->src = newval;
  }
}

static void
ip_napt_modify_addr_tcp(struct tcp_hdr *tcphdr, ip4_addr_p_t *oldval, u32_t newval)
{
  checksumadjust((u8_t *)&tcphdr->chksum, (u8_t *)&oldval->addr, 4, (u8_t *)&newval, 4);
}
#endif /* LWIP_TCP */

#if LWIP_UDP
static void
ip_napt_modify_port_udp(struct udp_hdr *udphdr, u8_t dest, u16_t newval)
{
  if (dest) {
    if (udphdr->chksum != 0)
      checksumadjust((u8_t *)&udphdr->chksum, (u8_t *)&udphdr->dest, 2, (u8_t *)&newval, 2);
    udphdr->dest = newval;
  } else {
    if (udphdr->chksum != 0)
      checksumadjust((u8_t *)&udphdr->chksum, (u8_t *)&udphdr->src, 2, (u8_t *)&newval, 2);
    udphdr->src = newval;
  }
}

static void
ip_napt_modify_addr_udp(struct udp_hdr *udphdr, ip4_addr_p_t *oldval, u32_t newval)
{
  if (udphdr->chksum != 0)
    checksumadjust( (u8_t *)&udphdr->chksum, (u8_t *)&oldval->addr, 4, (u8_t *)&newval, 4);
}
#endif /* LWIP_UDP */

static void
ip_napt_modify_addr(struct ip_hdr *iphdr, ip4_addr_p_t *field, u32_t newval)
{
  checksumadjust((u8_t *)&IPH_CHKSUM(iphdr), (u8_t *)&field->addr, 4, (u8_t *)&newval, 4);
  field->addr = newval;
}

/* Validate before any table lookup (which refreshes a mapping), checksum
 * adjustment, or transport-header access. Callers retain the pbuf unchanged.
 * Fragments must be reassembled by ip4_input before reaching this boundary. */
static u8_t
ip_napt_packet_valid(struct pbuf *p, const struct ip_hdr *iphdr)
{
  u16_t hlen, length, transport_len, sum;
  u8_t protocol;
  ip4_addr_t src, dest;
  if (p == NULL || iphdr == NULL || p->payload != iphdr || p->len < IP_HLEN)
    return 0;
  hlen = IPH_HL_BYTES(iphdr);
  length = lwip_ntohs(IPH_LEN(iphdr));
  if (IPH_V(iphdr) != 4 || hlen < IP_HLEN || hlen > p->len ||
      length < hlen || length != p->tot_len ||
      (IPH_OFFSET(iphdr) & PP_HTONS(IP_OFFMASK | IP_MF | IP_RF)) != 0 ||
      inet_chksum(iphdr, hlen) != 0)
    return 0;
  transport_len = length - hlen;
  protocol = IPH_PROTO(iphdr);
  switch (protocol) {
#if LWIP_TCP
    case IP_PROTO_TCP: {
      const struct tcp_hdr *tcp;
      if (transport_len < TCP_HLEN || p->len - hlen < TCP_HLEN)
        return 0;
      tcp = (const struct tcp_hdr *)((const u8_t *)iphdr + hlen);
      if (TCPH_HDRLEN_BYTES(tcp) < TCP_HLEN || TCPH_HDRLEN_BYTES(tcp) > transport_len)
        return 0;
      break;
    }
#endif
#if LWIP_UDP
    case IP_PROTO_UDP: {
      const struct udp_hdr *udp;
      u16_t udp_len;
      if (transport_len < UDP_HLEN || p->len - hlen < UDP_HLEN)
        return 0;
      udp = (const struct udp_hdr *)((const u8_t *)iphdr + hlen);
      udp_len = lwip_ntohs(udp->len);
      if (udp_len < UDP_HLEN || udp_len > transport_len)
        return 0;
      if (udp->chksum == 0) /* IPv4 permits UDP without a checksum. */
        return 1;
      transport_len = udp_len;
      break;
    }
#endif
#if LWIP_ICMP
    case IP_PROTO_ICMP: {
      const struct icmp_echo_hdr *echo;
      if (transport_len < sizeof(*echo) || p->len - hlen < sizeof(*echo))
        return 0;
      echo = (const struct icmp_echo_hdr *)((const u8_t *)iphdr + hlen);
      if (echo->code != 0 || (echo->type != ICMP_ECHO && echo->type != ICMP_ER))
        return 0;
      break;
    }
#endif
    default:
      return 0; /* No translation exists for this protocol. */
  }
  ip4_addr_copy(src, iphdr->src);
  ip4_addr_copy(dest, iphdr->dest);
  /* The IP header is contiguous and remains allocated while hidden. */
  if (pbuf_remove_header(p, hlen) != 0)
    return 0;
  sum = protocol == IP_PROTO_ICMP ? inet_chksum_pbuf(p) :
        inet_chksum_pseudo_partial(p, protocol, transport_len, transport_len, &src, &dest);
  pbuf_add_header_force(p, hlen);
  return sum == 0;
}

void
ip_napt_recv(struct pbuf *p, struct ip_hdr *iphdr)
{
  struct ip_portmap_entry *m;
  struct ip_napt_entry *t;

  if (p != NULL) p->flags &= (u8_t)~IP_NAPT_RECV_FLAG;
  if (ip_napt_max == 0 || !ip_napt_packet_valid(p, iphdr)) return;

#if LWIP_ICMP
  /* NAPT for ICMP Echo Request using identifier */
  if (IPH_PROTO(iphdr) == IP_PROTO_ICMP) {
    struct icmp_echo_hdr *iecho = (struct icmp_echo_hdr *)((u8_t *)p->payload + IPH_HL(iphdr) * 4);
    if (iecho->type == ICMP_ER) {
      t = ip_napt_find_in(IP_PROTO_ICMP, iphdr->src.addr, 0, iecho->id, iphdr->dest.addr);
      if (!t)
        return;
      checksumadjust((u8_t *)&iecho->chksum, (u8_t *)&iecho->id, 2, (u8_t *)&t->sport, 2);
      iecho->id = t->sport;
      /* An Echo Reply with ID/sequence/payload all zero needs the negative
       * zero checksum representation. 0xffff also validates for a folded-zero
       * incremental result with nonzero payload. */
      if (iecho->chksum == 0)
        iecho->chksum = 0xffff;
      ip_napt_modify_addr(iphdr, &iphdr->dest, t->src);
      p->flags |= IP_NAPT_RECV_FLAG;
      return;
    }

    return;
  }
#endif /* LWIP_ICMP */

#if LWIP_TCP
  if (IPH_PROTO(iphdr) == IP_PROTO_TCP) {
    uint32_t seqno, dest_seqno;
    struct tcp_hdr *tcphdr = (struct tcp_hdr *)((u8_t *)p->payload + IPH_HL(iphdr) * 4);


    LWIP_DEBUGF(NAPT_DEBUG, ("ip_napt_recv\n"));
    LWIP_DEBUGF(NAPT_DEBUG, ("src: %"U16_F".%"U16_F".%"U16_F".%"U16_F", dest: %"U16_F".%"U16_F".%"U16_F".%"U16_F", \n",
                             ip4_addr1_16(&iphdr->src), ip4_addr2_16(&iphdr->src),
                             ip4_addr3_16(&iphdr->src), ip4_addr4_16(&iphdr->src),
                             ip4_addr1_16(&iphdr->dest), ip4_addr2_16(&iphdr->dest),
                             ip4_addr3_16(&iphdr->dest), ip4_addr4_16(&iphdr->dest)));

    LWIP_DEBUGF(NAPT_DEBUG, ("sport %u, dport: %u\n",
                             lwip_htons(tcphdr->src),
                             lwip_htons(tcphdr->dest)));

    m = ip_portmap_find(IP_PROTO_TCP, tcphdr->dest);
    if (m && m->maddr == iphdr->dest.addr) {
      /* packet to mapped port: rewrite destination */
      if (m->dport != tcphdr->dest)
        ip_napt_modify_port_tcp(tcphdr, 1, m->dport);
      ip_napt_modify_addr_tcp(tcphdr, &iphdr->dest, m->daddr);
      ip_napt_modify_addr(iphdr, &iphdr->dest, m->daddr);
      p->flags |= IP_NAPT_RECV_FLAG;
      return;
    }
    t = ip_napt_find_in(IP_PROTO_TCP, iphdr->src.addr, tcphdr->src, tcphdr->dest, iphdr->dest.addr);
    if (!t)
      return; /* Unknown TCP session; do nothing */

    if (t->sport != tcphdr->dest)
      ip_napt_modify_port_tcp(tcphdr, 1, t->sport);
    ip_napt_modify_addr_tcp(tcphdr, &iphdr->dest, t->src);
    ip_napt_modify_addr(iphdr, &iphdr->dest, t->src);
      p->flags |= IP_NAPT_RECV_FLAG;

    if ((TCPH_FLAGS(tcphdr) & (TCP_SYN|TCP_ACK)) == (TCP_SYN|TCP_ACK))
      t->synack = 1;
    if ((TCPH_FLAGS(tcphdr) & TCP_FIN))
      t->fin1 = 1;
    if (t->fin2 && (TCPH_FLAGS(tcphdr) & TCP_ACK))
      t->finack2 = 1; /* FIXME: Currently ignoring ACK seq... */
    if (TCPH_FLAGS(tcphdr) & TCP_RST)
      t->rst = 1;
    seqno = ntohl(tcphdr->seqno);
    dest_seqno = t->dest_seqno;
    if (seqno >= dest_seqno || WRAPPED_AROUND(seqno, dest_seqno)) {
      t->dest_seqno = seqno + (p->tot_len - IPH_HL(iphdr) * 4 - TCPH_HDRLEN_BYTES(tcphdr));
    }
    return;
  }
#endif /* LWIP_TCP */

#if LWIP_UDP
  if (IPH_PROTO(iphdr) == IP_PROTO_UDP) {
    struct udp_hdr *udphdr = (struct udp_hdr *)((u8_t *)p->payload + IPH_HL(iphdr) * 4);
    m = ip_portmap_find(IP_PROTO_UDP, udphdr->dest);
    if (m && m->maddr == iphdr->dest.addr) {
      /* packet to mapped port: rewrite destination */
      if (m->dport != udphdr->dest)
        ip_napt_modify_port_udp(udphdr, 1, m->dport);
      ip_napt_modify_addr_udp(udphdr, &iphdr->dest, m->daddr);
      ip_napt_modify_addr(iphdr, &iphdr->dest, m->daddr);
      p->flags |= IP_NAPT_RECV_FLAG;
      return;
    }
    t = ip_napt_find_in(IP_PROTO_UDP, iphdr->src.addr, udphdr->src, udphdr->dest, iphdr->dest.addr);
      if (!t)
        return; /* Unknown session; do nothing */

      if (t->sport != udphdr->dest)
        ip_napt_modify_port_udp(udphdr, 1, t->sport);
      ip_napt_modify_addr_udp(udphdr, &iphdr->dest, t->src);
      ip_napt_modify_addr(iphdr, &iphdr->dest, t->src);
      p->flags |= IP_NAPT_RECV_FLAG;
      return;
  }
#endif /* LWIP_UDP */
}

err_t
ip_napt_forward(struct pbuf *p, struct ip_hdr *iphdr, struct netif *inp, struct netif *outp)
{
  if (!inp->napt)
    return ERR_OK;
  if (ip_napt_max == 0 || !ip_napt_packet_valid(p, iphdr))
    return ERR_RTE;

#if LWIP_ICMP
  /* NAPT for ICMP Echo Request using identifier */
  if (IPH_PROTO(iphdr) == IP_PROTO_ICMP) {
    struct icmp_echo_hdr *iecho = (struct icmp_echo_hdr *)((u8_t *)p->payload + IPH_HL(iphdr) * 4);
    if (iecho->type == ICMP_ECHO) {
      u16_t mapped = ip_napt_add(IP_PROTO_ICMP, iphdr->src.addr, iecho->id,
                                  iphdr->dest.addr, 0, 0, inp, outp);
      if (mapped == 0)
        return ERR_RTE;
      checksumadjust((u8_t *)&iecho->chksum, (u8_t *)&iecho->id, 2, (u8_t *)&mapped, 2);
      iecho->id = mapped;
      if (iecho->chksum == 0)
        iecho->chksum = 0xffff;
      ip_napt_modify_addr(iphdr, &iphdr->src, ip_2_ip4(&outp->ip_addr)->addr);
    }

    return ERR_OK;
  }
#endif

#if LWIP_TCP
  if (IPH_PROTO(iphdr) == IP_PROTO_TCP) {
    struct tcp_hdr *tcphdr = (struct tcp_hdr *)((u8_t *)p->payload + IPH_HL(iphdr) * 4);
    u16_t mport;

    struct ip_portmap_entry *m = ip_portmap_find_dest(IP_PROTO_TCP, tcphdr->src, iphdr->src.addr);
    if (m) {
      /* packet from port-mapped dest addr/port: rewrite source to this node */
      if (m->mport != tcphdr->src)
        ip_napt_modify_port_tcp(tcphdr, 0, m->mport);
      ip_napt_modify_addr_tcp(tcphdr, &iphdr->src, m->maddr);
      ip_napt_modify_addr(iphdr, &iphdr->src, m->maddr);
      return ERR_OK;
    }
    if ((TCPH_FLAGS(tcphdr) & (TCP_SYN|TCP_ACK)) == TCP_SYN &&
        PP_NTOHS(tcphdr->src) >= 1024) {
      /* Register new TCP session to NAPT */
      mport = ip_napt_add(IP_PROTO_TCP, iphdr->src.addr, tcphdr->src,
                          iphdr->dest.addr, tcphdr->dest, tcphdr->seqno, inp, outp);
      if (mport == 0) {
#if LWIP_ICMP
        icmp_dest_unreach(p, ICMP_DUR_PORT);
#endif
        return ERR_RTE; /* routing err if add entry failed */
      }
    } else {
      uint32_t seqno, src_seqno;
      struct ip_napt_entry *t = ip_napt_find_out(IP_PROTO_TCP, iphdr->src.addr, tcphdr->src,
                           iphdr->dest.addr, tcphdr->dest, inp, outp);
      if (!t || t->dest != iphdr->dest.addr || t->dport != tcphdr->dest) {
#if LWIP_ICMP
        icmp_dest_unreach(p, ICMP_DUR_PORT);
#endif
        return ERR_RTE; /* Drop unknown TCP session */
      }
      mport = t->mport;
      if ((TCPH_FLAGS(tcphdr) & TCP_FIN))
        t->fin2 = 1;
      if (t->fin1 && (TCPH_FLAGS(tcphdr) & TCP_ACK))
        t->finack1 = 1; /* FIXME: Currently ignoring ACK seq... */
      if (TCPH_FLAGS(tcphdr) & TCP_RST)
        t->rst = 1;
      seqno = ntohl(tcphdr->seqno);
      src_seqno = t->src_seqno;
      if (seqno >= t->src_seqno || WRAPPED_AROUND(seqno, src_seqno)) {
        t->src_seqno = seqno + (p->tot_len - IPH_HL(iphdr) * 4 - TCPH_HDRLEN_BYTES(tcphdr));
      }
    }

    if (mport != tcphdr->src)
      ip_napt_modify_port_tcp(tcphdr, 0, mport);
    ip_napt_modify_addr_tcp(tcphdr, &iphdr->src, ip_2_ip4(&outp->ip_addr)->addr);
    ip_napt_modify_addr(iphdr, &iphdr->src, ip_2_ip4(&outp->ip_addr)->addr);

    return ERR_OK;
  }
#endif

#if LWIP_UDP
  if (IPH_PROTO(iphdr) == IP_PROTO_UDP) {
    struct udp_hdr *udphdr = (struct udp_hdr *)((u8_t *)p->payload + IPH_HL(iphdr) * 4);
    u16_t mport;

    struct ip_portmap_entry *m = ip_portmap_find_dest(IP_PROTO_UDP, udphdr->src, iphdr->src.addr);
    if (m) {
      /* packet from port-mapped dest addr/port: rewrite source to this node */
      if (m->mport != udphdr->src)
        ip_napt_modify_port_udp(udphdr, 0, m->mport);
      ip_napt_modify_addr_udp(udphdr, &iphdr->src, m->maddr);
      LWIP_DEBUGF(NAPT_DEBUG, ("Modify UDP addr %"X32_F" %"X32_F, iphdr->src.addr, m->maddr));
      ip_napt_modify_addr(iphdr, &iphdr->src, m->maddr);
      return ERR_OK;
    }
    if (PP_NTOHS(udphdr->src) >= 1024) {
      /* Register new UDP session */
      mport = ip_napt_add(IP_PROTO_UDP, iphdr->src.addr, udphdr->src,
                          iphdr->dest.addr, udphdr->dest, 0, inp, outp);
      if (mport == 0)
        return ERR_RTE; /* routing err if add entry failed */
    } else {
      struct ip_napt_entry *t = ip_napt_find_out(IP_PROTO_UDP, iphdr->src.addr, udphdr->src,
                           iphdr->dest.addr, udphdr->dest, inp, outp);
      if (!t || t->dest != iphdr->dest.addr || t->dport != udphdr->dest) {
#if LWIP_ICMP
        icmp_dest_unreach(p, ICMP_DUR_PORT);
#endif
        return ERR_RTE; /* Drop unknown UDP session */
      }
      mport = t->mport;
    }

    if (mport != udphdr->src)
      ip_napt_modify_port_udp(udphdr, 0, mport);
    ip_napt_modify_addr_udp(udphdr, &iphdr->src, ip_2_ip4(&outp->ip_addr)->addr);
    ip_napt_modify_addr(iphdr, &iphdr->src, ip_2_ip4(&outp->ip_addr)->addr);
    return ERR_OK;
  }
#endif

  return ERR_OK;
}

static void
ip_napt_gc(uint32_t now, bool force)
{
  u16_t i, next;
  u32_t age;
  int checked = 0, evicted = 0;
  for (i = napt_list; i != NO_IDX; i = next) {
    struct ip_napt_entry *t = &ip_napt_table[i];
    checked++;
    next = t->next;
    age = now - t->last;
#if LWIP_TCP
    if (t->proto == IP_PROTO_TCP) {
      if (age > IP_NAPT_TIMEOUT_MS_TCP_DISCON) {
        if ((t->finack1 || t->finack2 || !t->synack || t->rst) ||
            age > IP_NAPT_TIMEOUT_MS_TCP) {
          ip_napt_free(t);
          evicted++;
          if (force) break;
        }
      }
      continue;
    }
#endif
#if LWIP_UDP
    if (t->proto == IP_PROTO_UDP) {
      if (age > IP_NAPT_TIMEOUT_MS_UDP) {
        ip_napt_free(t);
        evicted++;
        if (force) break;
      }
      continue;
    }
#endif
#if LWIP_ICMP
    if (t->proto == IP_PROTO_ICMP) {
      if (age > IP_NAPT_TIMEOUT_MS_ICMP) {
        ip_napt_free(t);
        evicted++;
        if (force) break;
      }
      continue;
    }
#endif
  }
  /* Resource pressure must not terminate an unexpired session. A caller
   * asking for room retries only after reclaiming actually expired entries. */
  LWIP_DEBUGF(NAPT_DEBUG, ("ip_napt_gc(%d): chk %d evict %d\n", force, checked, evicted));
}

static void
ip_napt_maint(void)
{
  /* GC uses unsigned elapsed subtraction, which already handles sys_now()
   * wrap. Resetting every last-use timestamp at wrap extends expired leases. */
  if (napt_list != NO_IDX)
    ip_napt_gc(sys_now(), false /* make_room */);
}

static void
ip_napt_tmr(void *arg)
{
  ip_napt_maint();
  sys_timeout(NAPT_TMR_INTERVAL, ip_napt_tmr, arg);
}

#if LWIP_STATS
void
ip_napt_get_stats(struct stats_ip_napt *stats)
{
  *stats = STATS_GET(ip_napt);
}
#endif /* LWIP_STATS */

#endif /* LWIP_IPV4 && IP_NAPT */

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

/* Host test of the production policy, with timeout/PCB environment stubs. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define TCP_QUEUE_OOSEQ 1
#define LWIP_CALLBACK_API 1
#define ESTABLISHED 4
#define TF_ACK_DELAY 1
#define TF_ACK_NOW 2
#define TF_INFR 4
#define TF_CLOSEPEND 8
#define TF_RXCLOSED 16
#define TF_FIN 32
#define TF_NAGLEMEMERR 128
#define TF_RTO 2048
#define SOF_KEEPALIVE 8
#define ip_get_option(p, bit) ((p)->so_options & (bit))
static int locked=1;
#define LWIP_ASSERT_CORE_LOCKED() assert(locked)
struct tcp_pcb {
  struct tcp_pcb *next;
  int state, flags, persist_backoff, snd_queuelen, pollinterval, so_options;
  void *unacked, *unsent, *refused_data, *ooseq, *poll;
  int netconn_idle;
};
static struct tcp_pcb *tcp_active_pcbs, *tcp_tw_pcbs;
static int tcpip_tcp_timer_active, slow_pending, fast_pending, other_pending;
typedef void (*sys_timeout_handler)(void *);
static void tcpip_tcp_slow_timer(void) {}
static void tcpip_tcp_fast_timer(void) {}
static int tcp_lp_netconn_idle(struct tcp_pcb *p) { return p->netconn_idle; }
static void sys_untimeout(sys_timeout_handler h, void *arg) {
  assert(!arg);
  if (h==(sys_timeout_handler)tcpip_tcp_slow_timer) slow_pending=0;
  else if(h==(sys_timeout_handler)tcpip_tcp_fast_timer) fast_pending=0;
  else assert(0);
}
static void tcp_timer_needed(void) {
  if (!tcpip_tcp_timer_active && (tcp_active_pcbs || tcp_tw_pcbs))
    tcpip_tcp_timer_active=slow_pending=fast_pending=1;
}
#include "../../../src/core/tcp_lp_idle.inc"
static struct tcp_pcb idle(void) {
  struct tcp_pcb p={0}; p.state=ESTABLISHED; p.netconn_idle=1; return p;
}
static void arm(void) { slow_pending=fast_pending=tcpip_tcp_timer_active=1; other_pending=7; }
static void must_keep(void) {
  arm(); assert(tcp_timer_lp_recheck()==0);
  assert(slow_pending && fast_pending && tcpip_tcp_timer_active && other_pending==7);
}
int main(void) {
  struct tcp_pcb a=idle(), b=idle(); tcp_active_pcbs=&a;
  must_keep(); /* cold boot default OFF */
  assert(tcp_timer_lp_control(1)==1);
  assert(!slow_pending && !fast_pending && !tcpip_tcp_timer_active && other_pending==7);
  a.next=&b;
#define BUSY(field,value) do { b.field=(value); must_keep(); b=idle(); } while(0)
  BUSY(state,5); BUSY(unacked,&a); BUSY(unsent,&a); BUSY(snd_queuelen,1);
  BUSY(persist_backoff,1); BUSY(refused_data,&a); BUSY(ooseq,&a);
  BUSY(poll,&a); BUSY(pollinterval,1); BUSY(so_options,SOF_KEEPALIVE);
  BUSY(netconn_idle,0);
  int flags[]={TF_ACK_DELAY,TF_ACK_NOW,TF_INFR,TF_CLOSEPEND,TF_RXCLOSED,TF_FIN,TF_NAGLEMEMERR,TF_RTO};
  for(unsigned i=0;i<sizeof(flags)/sizeof(flags[0]);i++) { BUSY(flags,flags[i]); }
  tcp_tw_pcbs=&b; must_keep(); tcp_tw_pcbs=NULL;
  arm(); assert(tcp_timer_lp_recheck()==1); assert(!slow_pending && !fast_pending);
  arm(); tcpip_tcp_timer_active=0; /* RXW1 queued timers with active zero. */
  assert(tcp_timer_lp_recheck()==1 && !slow_pending && !fast_pending);
  /* KA3: write arms, ACK empties queue, recheck cancels both. */
  a.unacked=&b; tcp_timer_needed(); must_keep(); a.unacked=NULL;
  assert(tcp_timer_lp_recheck()==1 && !tcpip_tcp_timer_active);
  /* Late ACK input arms again; final idle recheck cancels again. */
  tcp_timer_needed(); assert(slow_pending && fast_pending);
  assert(tcp_timer_lp_recheck()==1 && !slow_pending && !fast_pending);
  assert(tcp_timer_lp_control(0)==0); assert(slow_pending && fast_pending);
  must_keep();
  assert(tcp_timer_lp_control(-1)==0);
  assert(tcp_timer_lp_control(9)==-1);
  puts("tcp_lp_idle production policy: PASS");
}

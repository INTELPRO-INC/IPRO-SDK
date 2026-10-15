/* Included by timeouts.c: shares its private fast/slow timer state. */
static int tcp_lp_idle_enabled;

/* Only a known idle netconn may release the shared timers. Raw callbacks and
 * every other PCB's protocol/application work retain the existing schedule. */
int
tcp_timer_lp_recheck(void)
{
  struct tcp_pcb *pcb;
  LWIP_ASSERT_CORE_LOCKED();
  if (!tcp_lp_idle_enabled || tcp_tw_pcbs) {
    return 0;
  }
  for (pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next) {
    if (pcb->state != ESTABLISHED || pcb->unacked || pcb->unsent ||
        pcb->snd_queuelen || pcb->persist_backoff || pcb->refused_data ||
        pcb->pollinterval || ip_get_option(pcb, SOF_KEEPALIVE) ||
        (pcb->flags & (TF_ACK_DELAY | TF_ACK_NOW | TF_INFR | TF_CLOSEPEND |
                       TF_RXCLOSED | TF_FIN | TF_NAGLEMEMERR | TF_RTO))) {
      return 0;
    }
#if TCP_QUEUE_OOSEQ
    if (pcb->ooseq) {
      return 0;
    }
#endif
#if LWIP_CALLBACK_API
    if (pcb->poll) {
      return 0;
    }
#endif
    if (!tcp_lp_netconn_idle(pcb)) {
      return 0;
    }
  }
  /* Unconditionally unlink: precise timers may be queued while active == 0. */
  sys_untimeout((sys_timeout_handler)tcpip_tcp_slow_timer, NULL);
  sys_untimeout((sys_timeout_handler)tcpip_tcp_fast_timer, NULL);
  tcpip_tcp_timer_active = 0;
  return 1;
}

/* -1=query, 0=disable and restore scheduling, 1=enable and recheck. */
int
tcp_timer_lp_control(int mode)
{
  LWIP_ASSERT_CORE_LOCKED();
  if (mode == 0 || mode == 1) {
    int was_enabled = tcp_lp_idle_enabled;
    tcp_lp_idle_enabled = mode;
    if (mode) {
      tcp_timer_lp_recheck();
    } else if (was_enabled) {
      tcp_timer_needed();
    }
  } else if (mode != -1) {
    return -1;
  }
  return tcp_lp_idle_enabled;
}

/* Existing setters can create timer work without traffic. Restore the shared
 * schedule before that work becomes dependent on a future fast/slow tick. */
void
tcp_timer_lp_work_changed(void)
{
  LWIP_ASSERT_CORE_LOCKED();
  if (tcp_lp_idle_enabled) {
    tcp_timer_needed();
  }
}

/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef IPRO_WIFI_TCPTRACE_H_
#define IPRO_WIFI_TCPTRACE_H_

#include <stddef.h>
#include <stdint.h>

struct net_pkt;
struct shell;

void ipro_tcptrace_rx(const uint8_t *frame, size_t len);
void ipro_tcptrace_tx(struct net_pkt *pkt);
int ipro_tcptrace_cmd(const struct shell *sh, size_t argc, char **argv);

#endif

"""
Copyright (c) 2026 IntelPro Technology Co., Ltd.
All rights reserved.

This software and associated documentation files (the "Software") are
proprietary and confidential to IntelPro Technology Co., Ltd..

Access to and use of this Software is limited exclusively to authorized 
customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
valid license or non-disclosure agreement.

Unauthorized copying, modification, distribution, or reverse engineering 
of this Software, via any medium, is strictly prohibited without 
prior written permission from IntelPro Technology Co., Ltd..

SPDX-License-Identifier: Proprietary
"""

"""Run the actual Host response-report dispatcher with synthetic HCI input."""
import pathlib
import subprocess
import tempfile
import unittest

HOST = pathlib.Path(__file__).resolve().parents[1] / "upstream/zephyr/subsys/bluetooth/host"


class PawrDispatch(unittest.TestCase):
    def test_fragmented_report_delivers_one_complete_payload(self):
        source = (HOST / "adv.c").read_text()
        start = source.index("void bt_hci_le_per_adv_response_report(")
        end = source.index("\n#endif", start)
        handler = source[start:end]
        code = r'''
#include <assert.h>
#include <stddef.h>
#include "pawr_reassembly.h"
#define LOG_ERR(...) ((void)0)
#define LOG_WRN(...) ((void)0)
#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS_PARTIAL 1
#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS_COMPLETE 0
#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS_RX_FAILED 255
struct net_buf { uint8_t *data; size_t len; };
struct net_buf_simple { uint8_t *data; size_t len; };
struct bt_hci_evt_le_per_adv_response_report { uint8_t adv_handle, subevent, tx_status, num_responses; };
struct bt_hci_evt_le_per_adv_response { int8_t tx_power, rssi; uint8_t cte_type, response_slot, data_status, data_length; };
struct bt_le_per_adv_response_info { uint8_t subevent, tx_status, cte_type, response_slot; int8_t tx_power, rssi; };
struct bt_le_ext_adv;
struct callbacks { void (*pawr_response)(struct bt_le_ext_adv *, struct bt_le_per_adv_response_info *, struct net_buf_simple *); };
struct bt_le_ext_adv { struct bt_pawr_reassembly pawr_reassembly; const struct callbacks *cb; };
static struct bt_le_ext_adv adv;
static uint8_t received[251];
static size_t received_len;
static unsigned calls;
static void cb(struct bt_le_ext_adv *a, struct bt_le_per_adv_response_info *i, struct net_buf_simple *b)
{ (void)a; (void)i; assert(b); calls++; received_len=b->len; memcpy(received,b->data,b->len); }
static const struct callbacks callbacks = { cb };
static struct bt_le_ext_adv *bt_hci_adv_lookup_handle(uint8_t h) { return h == 0 ? &adv : NULL; }
static uint8_t bt_get_df_cte_type(uint8_t c) { return c; }
static void *net_buf_pull_mem(struct net_buf *b, size_t n)
{ assert(n <= b->len); void *p=b->data; b->data+=n; b->len-=n; return p; }
static void net_buf_simple_init_with_data(struct net_buf_simple *b, void *p, size_t n)
{ b->data=p; b->len=n; }
''' + handler + r'''
static void send(uint8_t status, unsigned start, unsigned n) {
    uint8_t raw[265] = {0, 2, 0, 1, 0, 0, 0, 0, status, n};
    for (unsigned i=0;i<n;i++) raw[10+i]=(uint8_t)(start+i);
    struct net_buf b={raw,10+n};
    bt_hci_le_per_adv_response_report(&b);
}
int main(void) {
    adv.cb=&callbacks;
    send(1,0,244);
    assert(calls==0);
    send(0,244,7);
    assert(calls==1 && received_len==251);
    for (unsigned i=0;i<251;i++) assert(received[i]==(uint8_t)i);
    /* Oversize fragments must never leak a tail to the application. */
    send(1,0,244); send(0,0,8); send(0,0,7);
    assert(calls==1);
    bt_pawr_reassembly_reset(&adv.pawr_reassembly);
    send(0,0,24); assert(calls==2 && received_len==24);
    /* Truncated HCI response cannot be followed by a falsely complete tail. */
    send(1,0,244);
    uint8_t raw[10]={0,2,0,1,0,0,0,0,0,7};
    struct net_buf malformed={raw,sizeof(raw)};
    bt_hci_le_per_adv_response_report(&malformed);
    send(0,244,7); assert(calls==2);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as tmp:
            path = pathlib.Path(tmp)
            (path / "test.c").write_text(code)
            subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror",
                            "-fsanitize=address,undefined", "-I", str(HOST),
                            str(path / "test.c"), "-o", str(path / "test")], check=True)
            result = subprocess.run([str(path / "test")], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)

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

"""Execute both actual Controller response-report handlers with a fake HCI sink."""
import pathlib
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2] / "ipro_ble_controller/upstream"


class ResponseBudget(unittest.TestCase):
    def test_all_lengths(self):
        for platform in ("src", "ipro6s"):
            with self.subTest(platform=platform), tempfile.TemporaryDirectory() as tmp:
                source = (ROOT / platform / "ip/ll/ble/src/llm/llm_adv.c").read_text()
                start = source.index("KE_MSG_HANDLER_NO_STATIC(lld_pawr_rsp_report,")
                handler = source[start:source.index("#endif // (BLE_PAWR)", start)]
                code = r'''
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#define LLM_PER_ADV_EN 1
#define KE_MSG_CONSUMED 0
#define HCI_LE_PER_ADV_RSP_REPORT_EVT_SUBCODE 0x28
#define PAWR_MAX_RSP_REPORT_DATA_LEN 244
#define BLE_PAWR_MORE_DATA 1
#define KE_MSG_HANDLER_NO_STATIC(n,t) int run(const t *param)
struct hci_le_set_per_adv_param_cmd { uint8_t adv_hdl; } params;
struct { struct { int state; void *host_params; } act_info[1]; } llm_env;
struct lld_pawr_rsp_report {
 uint8_t act_id, subevt, tx_status, tx_pwr, rssi, cte_type, rsp_slot, data_status, data_len, data[251];
};
struct hci_le_per_adv_rsp_report_evt {
 uint8_t subcode,adv_hdl,subevt,tx_status,num_rsp,tx_pwr,rssi,cte_type,rsp_slot,data_status,data_len,data[251];
} storage;
#define KE_MSG_ALLOC(a,b,c,d) (&storage)
static unsigned received, calls, expected;
static void hci_send_2_host(struct hci_le_per_adv_rsp_report_evt *e) {
 assert(e->data_len + 11u <= 255u);
 assert(e->subevt==3 && e->rsp_slot==2 && e->adv_hdl==7);
 for(unsigned i=0;i<e->data_len;i++) assert(e->data[i]==(uint8_t)(received+i));
 received+=e->data_len; calls++;
 assert(e->data_status==(received<expected?1:0));
}
''' + handler + r'''
int main(void) {
 params.adv_hdl=7; llm_env.act_info[0].state=LLM_PER_ADV_EN;
 llm_env.act_info[0].host_params=&params;
 struct lld_pawr_rsp_report input={.subevt=3,.rsp_slot=2};
 for(unsigned i=0;i<251;i++) input.data[i]=i;
 for(unsigned n=0;n<=251;n++) {
  received=calls=0; expected=n; input.data_len=n;
  assert(run(&input)==0); assert(received==n);
  assert(calls==(n>244?2:1));
 }
 return 0;
}
'''
                path = pathlib.Path(tmp)
                (path / "test.c").write_text(code)
                subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror",
                                "-fsanitize=address,undefined", str(path / "test.c"),
                                "-o", str(path / "test")], check=True)
                result = subprocess.run([str(path / "test")], capture_output=True, text=True)
                self.assertEqual(result.returncode, 0, result.stderr)

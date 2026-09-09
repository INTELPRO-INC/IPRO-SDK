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

"""Execute the Controller's actual response-owner selection before RX parsing."""
import pathlib
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2] / "ipro_ble_controller/upstream"


class ResponseOwner(unittest.TestCase):
    def test_scheduler_skip_preserves_response_owner(self):
        for platform in ("src", "ipro6s"):
            with self.subTest(platform=platform), tempfile.TemporaryDirectory() as tmp:
                source = (ROOT / platform / "ip/ll/ble/src/lld/lld_pawr.c").read_text()
                start = source.index("__STATIC void lld_pawr_pkt_rx(uint8_t act_id)")
                body = source.index("{", start)
                end = source.index("    // Check if a packet has been received", body)
                prefix = source[body + 1:end]
                code = r'''
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
enum { PAWR_EVT_WAIT, PAWR_EVT_ACTIVE, PAWR_RSP_SLOT_ACTIVE, PAWR_NO_RSP_SLOT };
struct lld_pawr_env_tag {
    unsigned pa_subevt_cnt, rsp_subevt, num_subevt, state, rsp_slot_state;
} env, *lld_pawr_env[] = { &env };
static unsigned owner(uint8_t act_id) {
''' + prefix + r'''
    (void)con_ok;
    return pa_subevt_cnt;
}
int main(void) {
    env.num_subevt = 4;
    env.state = PAWR_EVT_WAIT;
    env.rsp_slot_state = PAWR_RSP_SLOT_ACTIVE;
    /* Packet belongs to subevent0. Scheduler can advance by more than one
     * while retrying insertion of the NEXT advertising subevent. */
    for (unsigned response_sub = 0; response_sub < 4; response_sub++) {
        env.rsp_subevt = response_sub;
        for (unsigned next = 0; next < 4; next++) {
            env.pa_subevt_cnt = next;
            assert(owner(0) == response_sub);
        }
    }
    env.rsp_subevt = 3;
    env.pa_subevt_cnt = 0;
    assert(owner(0) == 3);
    /* AUX_CONNECT_RSP still belongs to current advertising subevent. */
    env.state = PAWR_EVT_ACTIVE;
    env.rsp_slot_state = PAWR_NO_RSP_SLOT;
    env.pa_subevt_cnt = 2;
    assert(owner(0) == 2);
    env.state = PAWR_EVT_WAIT;
    assert(owner(0) == 2);
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

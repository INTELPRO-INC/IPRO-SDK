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

"""PAwR buffers must have one owner after driver submission, including rejection."""
import pathlib
import subprocess
import tempfile
import unittest


class BufferOwner(unittest.TestCase):
    def test_transfer_and_reject(self):
        root = pathlib.Path(__file__).resolve().parents[2] / "ipro_ble_controller/upstream"
        for chip in ("src", "ipro6s"):
            with self.subTest(chip=chip), tempfile.TemporaryDirectory() as tmp:
                source = (root / chip / "ip/ll/ble/src/llm/llm_adv.c").read_text()
                start = source.index("status = lld_pawr_subevt_data_set(")
                start = source.index(";", start) + 1
                body = source[start:source.index("} while(false);", start)]
                code = '''#include <assert.h>
#include <stdbool.h>
#define CO_ERROR_NO_ERROR 0
struct { struct { struct { struct { struct {
unsigned new_buf,new_len,curr_buf,curr_len;
} adv_data; bool subevt_data_received; } per_adv; } info; } act_info[1]; } llm_env;
static unsigned freed;
static void ble_util_buf_adv_tx_free(unsigned p) { assert(p==42); freed++; }
static void finish(unsigned status) { unsigned per_act_id=0;
''' + body + '''}
int main(void) {
 for(unsigned status=0;status<2;status++) {
  llm_env.act_info[0].info.per_adv.adv_data.new_buf=42;
  llm_env.act_info[0].info.per_adv.adv_data.new_len=247;
  freed=0; finish(status);
  assert(llm_env.act_info[0].info.per_adv.adv_data.curr_buf==0);
  assert(llm_env.act_info[0].info.per_adv.adv_data.new_buf==0);
  assert(freed==status);
 }
 return 0;
}
'''
                path = pathlib.Path(tmp)
                (path / "test.c").write_text(code)
                subprocess.run(["cc", "-std=c11", "-Wno-unused-function", "-fsanitize=address,undefined", str(path / "test.c"), "-o", str(path / "test")], check=True)
                result = subprocess.run([str(path / "test")], capture_output=True, text=True)
                self.assertEqual(result.returncode, 0, result.stderr)

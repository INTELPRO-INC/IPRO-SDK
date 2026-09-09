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

"""Check actual Controller report chunk selection against HCI wire budgets."""
import pathlib
import re
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2] / "ipro_ble_controller/upstream"


class PeriodicBudget(unittest.TestCase):
    def test_v1_v2_boundaries(self):
        for platform in ("src", "ipro6s"):
            with self.subTest(platform=platform), tempfile.TemporaryDirectory() as tmp:
                source = (ROOT / platform / "ip/ll/ble/src/llm/llm_scan.c").read_text()
                start = source.index("//Per Adv Report:")
                setup = source[start:source.index("// do-while", start)]
                assignment = re.search(r"uint8_t report_data_len = [^;]+;", source[start:])[0]
                code = r'''
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#define BLE_PAWR 1
#define PER_ADV_DATA_MAX_LEN 247
#define HCI_LE_PER_ADV_REPORT_V2_EVT_SUBCODE 0x25
static bool masked;
#define hci_le_evt_mask_check(code) (masked)
static unsigned length(uint8_t rem_data_len) {
''' + setup + assignment + r'''
 return report_data_len;
}
int main(void) {
 for(unsigned v2=0; v2<2; v2++) {
  masked=!v2;
  unsigned limit=v2?244:247;
  for(unsigned n=0;n<=255;n++) {
   unsigned chunk=length(n);
   assert(chunk==(n<limit?n:limit));
   assert(chunk+(v2?11:8)<=255);
  }
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

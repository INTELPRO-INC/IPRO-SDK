#!/usr/bin/env python3
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

import importlib.util
from pathlib import Path
import unittest


TOOL = Path(__file__).with_name("verify_gatt_security_levels.py")
SPEC = importlib.util.spec_from_file_location("verify_gatt_security_levels", TOOL)
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)


class ParseReadLineTest(unittest.TestCase):
    def test_accepts_exact_unsecured_value(self):
        line = ("\x1b[0;32mI\x1b[0m GSL_TEST: READ PASS rc=0 att_err=0x00 "
                "sec=1 values=1 len=4 data=01040202 pass=7 fail=0")
        result = MODULE.parse_read_line(line)
        self.assertIsNotNone(result)
        self.assertTrue(MODULE.read_result_passes(result))
        self.assertEqual(result["pass_count"], 7)

    def test_rejects_wrong_payload(self):
        result = MODULE.parse_read_line(
            "GSL_TEST: READ PASS rc=0 att_err=0x00 sec=1 values=1 len=4 "
            "data=01010202 pass=1 fail=0")
        self.assertFalse(MODULE.read_result_passes(result))

    def test_rejects_encrypted_link_for_permission_gate(self):
        result = MODULE.parse_read_line(
            "GSL_TEST: READ PASS rc=0 att_err=0x00 sec=2 values=1 len=4 "
            "data=01040202 pass=1 fail=0")
        self.assertFalse(MODULE.read_result_passes(result))

    def test_ignores_other_output(self):
        self.assertIsNone(MODULE.parse_read_line("ipro />"))


if __name__ == "__main__":
    unittest.main()

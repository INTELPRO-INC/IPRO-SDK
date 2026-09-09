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


TOOL = Path(__file__).with_name("verify_channel_classification.py")
SPEC = importlib.util.spec_from_file_location("verify_channel_classification", TOOL)
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)


class ChannelClassificationParserTest(unittest.TestCase):
    def test_parses_central_map_and_bits(self):
        result = MODULE.parse_central_line(
            "\x1b[32mI\x1b[0m CH_CLASS_TEST: CENTRAL map=7fffffff1f "
            "expect_bad=7 PASS rc=0 sec=1")
        self.assertIsNotNone(result)
        self.assertFalse(MODULE.channel_enabled(result["map"], 7))
        self.assertTrue(MODULE.channel_enabled(result["map"], 36))

    def test_parses_boundary_channel(self):
        result = MODULE.parse_central_line(
            "CH_CLASS_TEST: CENTRAL map=ffffffff0f expect_bad=36 PASS rc=0 sec=1")
        self.assertFalse(MODULE.channel_enabled(result["map"], 36))
        self.assertTrue(MODULE.channel_enabled(result["map"], 35))

    def test_parses_peripheral_forms(self):
        forced = MODULE.parse_peripheral_line(
            "CH_CLASS_TEST: PERIPHERAL force_bad=36 rc=0 controller_reads=12")
        cleared = MODULE.parse_peripheral_line(
            "CH_CLASS_TEST: PERIPHERAL clear rc=0 controller_reads=13")
        status = MODULE.parse_peripheral_line(
            "CH_CLASS_TEST: PERIPHERAL status controller_reads=14")
        self.assertEqual(forced["channel"], 36)
        self.assertEqual(forced["rc"], 0)
        self.assertEqual(cleared["clear_rc"], 0)
        self.assertEqual(status["reads"], 14)

    def test_ignores_unrelated_output(self):
        self.assertIsNone(MODULE.parse_central_line("state=connected"))
        self.assertIsNone(MODULE.parse_peripheral_line("ipro />"))


if __name__ == "__main__":
    unittest.main()

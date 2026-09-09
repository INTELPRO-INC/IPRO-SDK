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

"""Unit tests for the GATT Security Levels policy bench verifier."""

import unittest

from verify_gatt_security_policy import (
    denial_passes,
    parse_protected_line,
    parse_security_changed_line,
    protected_read_passes,
)


class GattSecurityPolicyParserTests(unittest.TestCase):
    def test_level_one_read_is_rejected_by_authentication_permission(self):
        result = parse_protected_line(
            "I (12) GSL_TEST: PROTECTED rc=0 att_err=0x05 sec=1 len=0 "
            "exact=0 data=0000000000000000"
        )
        self.assertIsNotNone(result)
        self.assertTrue(denial_passes(result))

    def test_level_one_read_with_retained_ltk_requires_encryption(self):
        result = parse_protected_line(
            "GSL_TEST: PROTECTED rc=0 att_err=0x0f sec=1 len=0 "
            "exact=0 data=0000000000000000"
        )
        self.assertTrue(denial_passes(result))

    def test_level_four_read_accepts_only_exact_eight_bytes(self):
        result = parse_protected_line(
            "GSL_TEST: PROTECTED rc=0 att_err=0x00 sec=4 len=8 exact=1 "
            "data=4950524f2d534334"
        )
        self.assertTrue(protected_read_passes(result))

    def test_rejects_level_three_or_wrong_payload(self):
        level_three = parse_protected_line(
            "GSL_TEST: PROTECTED rc=0 att_err=0x00 sec=3 len=8 exact=1 "
            "data=4950524f2d534334"
        )
        wrong_payload = parse_protected_line(
            "GSL_TEST: PROTECTED rc=0 att_err=0x00 sec=4 len=8 exact=1 "
            "data=4950524f2d534335"
        )
        self.assertFalse(protected_read_passes(level_three))
        self.assertFalse(protected_read_passes(wrong_payload))

    def test_parses_successful_security_change(self):
        result = parse_security_changed_line(
            "[ipro-central-smp] security_changed peer=CA:00:10:00:00:00 (public) "
            "level=4 err=0 count=1"
        )
        self.assertEqual(result, {"level": 4, "err": 0, "count": 1})

    def test_ignores_unrelated_output(self):
        self.assertIsNone(parse_protected_line("ipro />"))
        self.assertIsNone(parse_security_changed_line("GSL_TEST: AUTH_CANCEL"))


if __name__ == "__main__":
    unittest.main()

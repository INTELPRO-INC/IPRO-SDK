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

"""Unit tests for the periodic advertising ADI bench verifier."""

import unittest

from verify_periodic_adi import parse_status_line, sequence_transition_ok


class PeriodicAdiParserTests(unittest.TestCase):
    def test_parse_status_line(self):
        line = (
            "\x1b[0;32mI\x1b[0m (10) ADI_TEST: status rc=0 adv=0 adi=0 "
            "seeking=0 sync=1 filter=1 tx_seq=0 rx=4 good=4 bad=0 "
            "same=0 changed=3 last=3 lost=0 err=0"
        )
        self.assertEqual(
            parse_status_line(line),
            {
                "operation": "status", "rc": 0, "adv": 0, "adi": 0,
                "seeking": 0, "sync": 1, "filter": 1, "tx_seq": 0,
                "rx": 4, "good": 4, "bad": 0, "same": 0,
                "changed": 3, "last": 3, "lost": 0, "err": 0,
            },
        )

    def test_ignore_unrelated_line(self):
        self.assertIsNone(parse_status_line("ADI_TEST: RX seq=2 exact=1"))

    def test_exact_filtered_transition(self):
        before = {"rx": 3, "good": 3, "bad": 0, "same": 0,
                  "changed": 2, "last": 2, "sync": 1, "err": 0,
                  "lost": 0}
        after = {"rx": 4, "good": 4, "bad": 0, "same": 0,
                 "changed": 3, "last": 3, "sync": 1, "err": 0,
                 "lost": 0}
        self.assertTrue(sequence_transition_ok(before, after, 3))

    def test_reject_duplicate_or_bad_transition(self):
        before = {"rx": 3, "good": 3, "bad": 0, "same": 0,
                  "changed": 2, "last": 2, "sync": 1, "err": 0,
                  "lost": 0}
        duplicate = {"rx": 5, "good": 5, "bad": 0, "same": 1,
                     "changed": 3, "last": 3, "sync": 1, "err": 0,
                     "lost": 0}
        self.assertFalse(sequence_transition_ok(before, duplicate, 3))


if __name__ == "__main__":
    unittest.main()

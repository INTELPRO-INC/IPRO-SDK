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

import unittest
from verify_past import healthy, parse_status


class PastParser(unittest.TestCase):
    def test_exact_status(self):
        status = parse_status("\x1b[32mI PAST_TEST: status rc=0 transfers=0 syncs=3 "
                              "reports=301 bad=0 lost=2 err=0\x1b[0m")
        self.assertTrue(healthy(status))
        self.assertEqual(status["reports"], 301)

    def test_reject_bad_or_error(self):
        for field in ("rc", "bad", "err"):
            status = dict(rc=0, bad=0, err=0)
            status[field] = -5 if field == "rc" else 1
            self.assertFalse(healthy(status))

    def test_unrelated_and_partial(self):
        self.assertIsNone(parse_status("PAST_TEST: SYNC_PASS"))
        self.assertIsNone(parse_status("PAST_TEST: status rc=0 transfers=0"))
        self.assertFalse(healthy(None))


if __name__ == "__main__":
    unittest.main()

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

"""Keep the experimental PAwR receive path out of reference profiles."""
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[1] / "tools"))
from build_matrix import PROFILES


class PawrProfiles(unittest.TestCase):
    def test_long_reports_have_reassembly_storage(self):
        for name in ("le_pawr_ipro6s", "le_pawr_ipro7ai", "le_pawr_ipro6s_passive_rx_experiment"):
            self.assertGreaterEqual(int(PROFILES[name].settings.get("CONFIG_BT_PER_ADV_SYNC_BUF_SIZE", "0")), 251)

    def test_reference_does_not_select_experiment(self):
        for name in ("le_pawr_ipro6s", "le_pawr_ipro7ai"):
            with self.subTest(profile=name):
                self.assertEqual(PROFILES[name].settings[
                    "CONFIG_IPRO6S_PAWR_PASSIVE_RX_EXPERIMENT"], "n")

    def test_experiment_is_fixture_only(self):
        selected = [name for name, profile in PROFILES.items()
                    if profile.settings.get("CONFIG_IPRO6S_PAWR_PASSIVE_RX_EXPERIMENT") == "y"]
        self.assertEqual(selected, ["le_pawr_ipro6s_passive_rx_experiment"])
        profile = PROFILES[selected[0]]
        self.assertEqual(profile.settings["CONFIG_IPRO_BT_HOST_PAWR_TEST"], "y")
        self.assertEqual(profile.app, "apps/platform/ipro6_btdm_phyrf")


if __name__ == "__main__":
    unittest.main()

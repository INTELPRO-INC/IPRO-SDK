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

"""Execute the production advertising allocator with a fake Controller remove.

This isolates allocation/error semantics; dual-board PAST covers real HCI,
retained bonds, event ordering and actual Controller buffer reclamation.
"""
import pathlib
import subprocess
import tempfile
import unittest


SOURCE = (pathlib.Path(__file__).resolve().parents[1] /
          "upstream/zephyr/subsys/bluetooth/host/adv.c")


def allocator_source():
    source = SOURCE.read_text()
    start = source.index("static struct bt_le_ext_adv *adv_new(")
    end = source.index("\nstatic void adv_delete", start)
    function = source[start:end]
    call = "adv_new(&err)" if "int *err" in function else "adv_new()"
    return function, call


class AdvReuse(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory(prefix="ipro-adv-reuse-")
        path = pathlib.Path(cls.temp.name)
        function, call = allocator_source()
        code = r'''
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
enum { BT_ADV_CREATED, BT_ADV_PARAMS_SET };
struct bt_le_ext_adv { unsigned flags[1]; unsigned handle; };
static struct bt_le_ext_adv adv_pool[2];
static int remove_calls, remove_error;
static bool atomic_test_bit(unsigned *p, unsigned b) { return (*p >> b) & 1U; }
static void atomic_set_bit(unsigned *p, unsigned b) { *p |= 1U << b; }
static int bt_le_ext_adv_delete(struct bt_le_ext_adv *adv) {
    remove_calls++;
    if (remove_error) return remove_error;
    adv->flags[0] = 0;
    return 0;
}
''' + function + r'''
int main(int argc, char **argv) {
    assert(argc == 2);
    int scenario = atoi(argv[1]), err = -ENOMEM;
    struct bt_le_ext_adv *adv;
    if (scenario == 1) {
        adv_pool[0].flags[0] = adv_pool[1].flags[0] = 1U << BT_ADV_CREATED;
    } else if (scenario >= 2 && scenario <= 4) {
        adv_pool[0].flags[0] = 1U << BT_ADV_PARAMS_SET;
        if (scenario >= 3) remove_error = -EIO;
    } else if (scenario == 5) {
        adv_pool[0].flags[0] = 1U << BT_ADV_CREATED;
        adv_pool[1].flags[0] = 1U << BT_ADV_PARAMS_SET;
        adv_pool[1].handle = 1;
    }
    adv = CALL;
    if (scenario == 1) {
        assert(!adv && err == -ENOMEM && remove_calls == 0);
    } else if (scenario == 3 || scenario == 4) {
        assert(!adv && err == -EIO && remove_calls == 1);
        assert(adv_pool[0].flags[0] == (1U << BT_ADV_PARAMS_SET));
        if (scenario == 4) {
            remove_error = 0;
            adv = CALL;
            assert(adv == &adv_pool[0] && remove_calls == 2);
        }
    } else {
        assert(adv == &adv_pool[scenario == 5 ? 1 : 0]);
        assert(adv->handle == (unsigned)(scenario == 5 ? 1 : 0));
        assert(adv->flags[0] == (1U << BT_ADV_CREATED));
        assert(remove_calls == (scenario == 2 || scenario == 5 ? 1 : 0));
    }
    return 0;
}
'''
        (path / "test.c").write_text(code.replace("CALL", call))
        cls.binary = path / "test"
        subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra",
                        "-Wno-sign-compare", "-fsanitize=address,undefined",
                        str(path / "test.c"), "-o", str(cls.binary)], check=True)

    @classmethod
    def tearDownClass(cls):
        cls.temp.cleanup()

    def test_allocator(self):
        for scenario, name in enumerate(("fresh", "full", "retired", "remove_error",
                                         "retry", "second_slot")):
            with self.subTest(name=name):
                result = subprocess.run([str(self.binary), str(scenario)],
                                        capture_output=True, text=True)
                self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()

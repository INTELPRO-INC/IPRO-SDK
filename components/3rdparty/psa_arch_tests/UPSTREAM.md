# Upstream sources of components/3rdparty/psa_arch_tests

The directories `psa_arch_tests/` and `qcbor/` are verbatim copies of
the upstream files listed below; nothing in them is edited. Our code
(Kconfig, CMake, the RISC-V NSPE port) lives beside them, see
`README.md`.

Check or refresh the copy from checkouts at the pinned commits:

```
git clone https://github.com/ARM-software/psa-arch-tests <arch>
git -C <arch> checkout fa0b9efbd9420d4b204db9b65e7d168b47185a98
git -C <arch> submodule update --init val_common
git clone https://github.com/laurencelundblade/QCBOR.git <qcbor>
git -C <qcbor> checkout 42272e466a8472948bf8fca076d113b81b99f0e0
tools/psa/psa_arch_tests_vendor.sh --from <arch> <qcbor>          # compare
tools/psa/psa_arch_tests_vendor.sh --from <arch> <qcbor> --copy   # refresh
```

The script exits 0 and prints `psa_arch_tests_vendor: identical to
...` when every vendored file matches its upstream byte for byte.

## psa-arch-tests

- Repository: https://github.com/ARM-software/psa-arch-tests
- Commit: `fa0b9efbd9420d4b204db9b65e7d168b47185a98`
  (tag `v25.08_API1.9_ADAC_1.0.2`)
- Licence: Apache-2.0, `psa_arch_tests/LICENSE.md`
- Subset, under `psa_arch_tests/`:
  - `LICENSE.md`
  - `api-tests/val/`
  - `api-tests/platform/targets/common/nspe/`
  - `api-tests/platform/drivers/nvmem/`
  - `api-tests/dev_apis/crypto/`
  - `api-tests/dev_apis/storage/`
  - `api-tests/dev_apis/initial_attestation/`
  - `api-tests/tools/scripts/gen_tests_list.py`

## psa-acs-common-val (submodule `val_common` of psa-arch-tests)

`api-tests/val/common/val.h` includes its headers and the NSPE VAL
links its sources (logging, status, regression report).

- Repository: https://github.com/ARM-software/psa-acs-common-val
- Commit: `36ad62ef04ec5cccc65e541547c85e7a621bb364` (the gitlink of
  psa-arch-tests `fa0b9ef`)
- Licence: BSD-3-Clause (SPDX line in every source file and
  `psa_arch_tests/val_common/README.md`; the repository has no
  separate licence file)
- Subset, under `psa_arch_tests/val_common/`: `README.md`, `inc/`,
  `src/`

## QCBOR

The CBOR decoder of the Initial Attestation suite; psa-arch-tests
pins the same commit in `api-tests/tools/cmake/common/CMakeExternal.cmake`.

- Repository: https://github.com/laurencelundblade/QCBOR
- Commit: `42272e466a8472948bf8fca076d113b81b99f0e0`
- Licence: BSD-3-Clause (Qualcomm/CAF files: the slightly modified
  3-clause text in each file header); see "Copyright and License" in
  `qcbor/README.md`
- Subset, under `qcbor/`: `README.md`, `inc/`, `src/`

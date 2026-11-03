# Upstream sources of components/crypto/tfpsa

`tf-psa-crypto/` is a copy of the upstream release files listed below with
one edit, `port/sec_eng_driver.patch`: the ipro_sec SEC_ENG driver
(components/psa/accel) in the generated driver wrappers and
context unions, every block under CONFIG_PSA_SEC_ENG_ACCEL.
`tools/psa/psa_sec_eng_patch.py` writes it. Our code is
`CMakeLists.txt`, `port/` and this file.

- Repository: https://github.com/Mbed-TLS/TF-PSA-Crypto
- Release: `tf-psa-crypto-1.2.0` (2026-07-07), asset
  `tf-psa-crypto-1.2.0.tar.bz2`, sha256
  `4de95b3719874625ed4649bfc824b9271c13fc358ca88d5e227134469ad02007`
  (the release's `tf-psa-crypto-1.2.0-sha256sum.txt`). The git tag
  `v1.2.0` lacks the generated `core/psa_crypto_driver_wrappers.h` and
  `core/psa_crypto_driver_wrappers_no_static.c`; the release asset has them.
- Licence: Apache-2.0 OR GPL-2.0-or-later, `tf-psa-crypto/LICENSE`
- Subset: `include/`, `core/`, `dispatch/`, `drivers/builtin/`,
  `extras/`, `platform/`, `utilities/`, `LICENSE`, `README.md`, `ChangeLog`;
  removed from them: every `CMakeLists.txt` and `.gitignore`,
  `core/crypto-library.make`. Not copied: `drivers/p256-m`,
  `drivers/everest`, `drivers/pqcp`, `framework/`, `tests/`,
  `programs/`, `scripts/`, `docs/`.

Check the copy against the release asset:

```
mkdir x && tar -xjf tf-psa-crypto-1.2.0.tar.bz2 -C x && R=x/tf-psa-crypto-1.2.0
for d in include core dispatch drivers/builtin extras platform utilities; do
    diff -r -x .gitignore -x CMakeLists.txt -x crypto-library.make -x '*.jinja' \
        $R/$d tf-psa-crypto/$d
done
for f in LICENSE README.md ChangeLog; do cmp $R/$f tf-psa-crypto/$f; done
```

The diff then lists only the files of `port/sec_eng_driver.patch`;
`patch -p1 -R -d tf-psa-crypto < port/sec_eng_driver.patch` restores them.

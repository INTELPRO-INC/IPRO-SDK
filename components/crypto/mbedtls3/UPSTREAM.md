# Upstream sources of components/crypto/mbedtls3

`mbedtls/` is a copy of the upstream files listed below with
one edit, `port/sec_eng_driver.patch`: the ipro_sec SEC_ENG driver
(components/psa/accel) in the generated driver wrappers and
context unions, every block under CONFIG_PSA_SEC_ENG_ACCEL.
`tools/psa/psa_sec_eng_patch.py` writes it. Our code is
`CMakeLists.txt`, `port/` and this file.

- Repository: https://github.com/Mbed-TLS/mbedtls
- Tag: `mbedtls-3.6.7`, commit `068ff080b369adfac81509f9b57b2afabaf82dc5`
- Licence: Apache-2.0 OR GPL-2.0-or-later, `mbedtls/LICENSE`
- Subset: `include/` and `library/` (generated files as released),
  `LICENSE`, `README.md`; removed from them: `include/.gitignore`,
  `include/CMakeLists.txt`, `library/.gitignore`,
  `library/CMakeLists.txt`, `library/Makefile`

Check the copy against a checkout of the tag:

```
git clone --depth 1 --branch mbedtls-3.6.7 https://github.com/Mbed-TLS/mbedtls.git <src>
for d in include library; do diff -r -x .gitignore -x CMakeLists.txt -x Makefile <src>/$d mbedtls/$d; done
cmp <src>/LICENSE mbedtls/LICENSE && cmp <src>/README.md mbedtls/README.md
```

The diff then lists only the files of `port/sec_eng_driver.patch`;
`patch -p1 -R -d mbedtls < port/sec_eng_driver.patch` restores them.

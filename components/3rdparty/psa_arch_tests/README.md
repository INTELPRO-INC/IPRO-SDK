# psa_arch_tests

Arm psa-arch-tests `dev_apis` suites (Crypto, Internal Trusted
Storage, Initial Attestation) built for the IPRO7 U-mode NSPE
(`apps/security/ipro7_psa_demo`). The tests call the PSA APIs of
`components/psa/client`, which forwards them over `ecall` to the
M-mode SPE (`components/psa/README.md`). One suite per build; the
component is off unless `CONFIG_COMPONENTS_PSA_ARCH_TESTS_ENABLE=y`.

## Layout

| Path | Content |
|---|---|
| `psa_arch_tests/` | verbatim upstream subset (psa-arch-tests `fa0b9ef`, its `val_common` submodule) |
| `qcbor/` | verbatim QCBOR `42272e4` (CBOR decoder of the attestation suite) |
| `UPSTREAM.md` | commits, subset, licences; `tools/psa/psa_arch_tests_vendor.sh` checks the copy |
| `port/nspe/` | IPRO7 target: config headers and PAL sources |
| `port/crypto_1.0.0_testsuite.db` | Crypto test list: upstream `1.0.0_testsuite.db` with c019 marked `skip` (Tests) |
| `port/crypto_1.1.0_m3_testsuite.db` | Crypto test list of `CONFIG_MBEDTLS_V3`: upstream `1.1.0_testsuite.db` (c001-c063, c066) |
| `port/crypto_1.2_tfpsa_testsuite.db` | Crypto test list of `CONFIG_TF_PSA_CRYPTO`: the 1.1.0 list plus c070, c076, c080 |
| `port/nspe/val_target.h` | `PRINT_TEST` for test_c080, which includes this header |
| `port/tests/` | host test of the driver PAL |
| `Kconfig`, `CMakeLists.txt` | component build |

## Kconfig

| Symbol | Meaning |
|---|---|
| `COMPONENTS_PSA_ARCH_TESTS_ENABLE` | build the suite library (needs `PSA_CLIENT`) |
| `PSA_ARCH_TEST_SUITE_CRYPTO` | Crypto, test list `port/crypto_1.0.0_testsuite.db` (c001-c063 of the 1.0.0 list, c019 skipped) |
| `PSA_ARCH_TEST_SUITE_ITS` | Internal Trusted Storage, `dev_apis/storage/its_testsuite.db` (s001-s010) |
| `PSA_ARCH_TEST_SUITE_ATTEST` | Initial Attestation profile 1, `dev_apis/initial_attestation/testsuite.db` (a001) |
| `PSA_ARCH_TEST_RANGE_START` / `_END` | first / last test number built, 0 = no bound |
| `PSA_ARCH_TEST_VERBOSITY` | `VERBOSITY` of the suite, 1 all .. 5 errors only (lower prints more; 3, the default, prints test progress); ALWAYS lines print at every level |

Defconfigs are full `.config` files: an app defconfig lists all seven
symbols. At configure time `gen_tests_list.py` turns the test list
and range into `test_entry_list.inc` and
`test_entry_fn_declare_list.inc` under `<build>/.../psa_arch_gen`;
configuring fails when the range selects no test.

## Interface for the app

- `int32_t val_entry(void)` (`val_entry.h`): runs every built test,
  prints the report and returns `VAL_STATUS_SUCCESS` (0),
  `VAL_STATUS_TEST_FAILED` when a test failed, or another VAL status
  when the framework itself fails.
- `void pal_nvmem_reset(void)` (`pal_common.h`, declared in
  `port/nspe/pal_config.h`): fills the 1 KB RAM NVMEM with 0xFF. Call
  it before each `val_entry()`: the suite keeps its boot state, last
  test id and counters there, and after the fill a run starts at the
  first test with zeroed counters.
- `ARCH_TEST_SUITE_NAME`: exported string literal, `"CRYPTO"`,
  `"INTERNAL_TRUSTED_STORAGE"` or `"INITIAL_ATTESTATION"`.
- Exported include directories: the generated `psa_arch_gen`,
  `val/common`, `val/nspe`, `val_common/inc`,
  `platform/targets/common/nspe` (and its `crypto/`), `port/nspe`.

Include `val_entry.h` and `pal_common.h` before `FreeRTOS.h`:
`bsp/common/platform_utils/include/misc.h` defines `ERROR` as a macro
and `val_common_log.h` declares an enum constant `ERROR`.

## Build flags

Private compile definitions: `VAL_NSPE_BUILD`,
`NONSECURE_TEST_BUILD`, `PSA_CMAKE_BUILD`, `VERBOSITY=<n>`,
`PLATFORM_PSA_ISOLATION_LEVEL=2`, `STATELESS_ROT=0`,
`STATIC_ASSERT_CHECKS` (skips the LP64 size asserts of
`val_common_log.c`), the suite macro (`CRYPTO`,
`INTERNAL_TRUSTED_STORAGE` or `INITIAL_ATTESTATION`), and for the
attestation suite `PSA_ATTESTATION_PROFILE_1` and
`PLATFORM_HAS_ATTEST_PK`. `CRYPTO_VERSION_BETA3` comes from
`port/nspe/pal_config.h` for every suite.

The library builds with `-Werror` like other SDK components. Vendored
sources only add `-Wno-unused-variable`: `test_c040.c` keeps the
locals of checks that the undefined RSA flags compile out. Port
sources take no warning option.

## Port (`port/nspe`)

| File | Content |
|---|---|
| `pal_config.h` | UART0 0x3000B000, no watchdog (`WATCHDOG_AVAILABLE` undefined, `PLATFORM_WD_*` 0), NVMEM base 0, `CRYPTO_VERSION_BETA3`, the suite's PSA header, `pal_nvmem_reset()`; for Crypto also five PSA Crypto API 1.1 identifiers missing from the mbedtls 2.28 headers (below) |
| `pal_crypto_config.h` | `ARCH_TEST_*` flags of the features the SPE serves (below) |
| `pal_storage_config.h` | `ARCH_TEST_STORAGE_UID_MAX_SIZE` 512 |
| `pal_attestation_config.h` | COSE constants; prototype of `tfm_initial_attest_get_public_key` |
| `pal_driver_intf.c` | console through `putchar()`, watchdog calls succeed, 1 KB RAM NVMEM with bounds checks, `pal_system_reset()` returns `PAL_STATUS_UNSUPPORTED_FUNC`, `pal_terminate_simulation()` returns |
| `pal_crypto_intf.c` | upstream `platform/targets/common/nspe/crypto/pal_crypto_intf.c` with upstream formatting; hash suspend/resume, `KEY_DERIVATION_INPUT_INTEGER`, `KEY_DERIVATION_VERIFY_BYTES`/`_KEY`, `KEY_AGREEMENT` (API 1.1 form) and every `PAKE_*` call return `PSA_ERROR_NOT_SUPPORTED` |
| `pal_attest_pk.c` | `tfm_initial_attest_get_public_key()`: the IAK public key from `psa_platform_get_iak_pub()`; any key not starting with 0x04 gives `PAL_ATTEST_ERR_KEY_FAIL` |

API 1.1 identifiers in `pal_config.h`, defined only when the mbedtls
headers lack them, with the values of the PSA Crypto API 1.1
specification: `PSA_KEY_DERIVATION_INPUT_PASSWORD` (0x0102),
`PSA_KEY_DERIVATION_INPUT_OTHER_SECRET` (0x0103),
`PSA_KEY_DERIVATION_INPUT_COST` (0x0205), `PSA_ALG_IS_PBKDF2_HMAC`
(base 0x08800100) and `PSA_ALG_GET_HASH`. Tests c018, c019 and c041
of the 1.0.0 list name them. The SPE (mbedtls 2.28) knows none of the
three steps: a check that passes one to it gets an error status.

## Crypto feature flags

Defined (`pal_crypto_config.h`): `ARCH_TEST_AES`, `_AES_128`,
`_AES_192`, `_AES_256`, `_RAW`, `_CIPHER`, `_CIPHER_MODE_CTR`,
`_CIPHER_MODE_CBC`, `_CTR_AES`, `_CBC_AES`, `_CBC_AES_NO_PADDING`,
`_CBC_NO_PADDING`, `_CBC_PKCS7`, `_HASH`, `_HMAC`, `_SHA1`,
`_SHA224`, `_SHA256`, `_SHA384`, `_SHA512`, `_HKDF`, `_TLS12_PRF`,
`_CMAC`, `_GMAC`, `_CCM`, `_GCM`, `_TRUNCATED_MAC`, `_ECC`,
`_ECC_CURVE_SECP256R1`, `_ECC_CURVE_SECP384R1`, `_ECDH`, `_ECDSA`,
`_DETERMINISTIC_ECDSA`, `_ECC_ASYMMETRIC_API_SUPPORT`. The SPE also
has P-521 and Curve25519; no test of the 1.0.0 list has a flag for
them.

Also defined: `ARCH_TEST_AEAD_MULTISTAGE_OFF`. The PSA core of mbedtls
2.28.2 has one-shot AEAD only, the client answers every multipart
AEAD call with `PSA_ERROR_NOT_SUPPORTED`, and with the flag c054-c063
(multipart AEAD) build as skipped tests.

The PSA defconfigs set `CONFIG_MBEDTLS_V3`; the 2.28 description
above holds for `defconfig_arch_crypto_m2`. With `CONFIG_MBEDTLS_V3`
the CMake defines `PSA_ARCH_MBEDTLS_V3` and
`PSA_API_MODE_ON=0` (upstream leaves it undefined, which `-Werror=undef`
refuses) and takes `port/crypto_1.1.0_m3_testsuite.db` (the 1.1.0 list:
the 1.0.0 list with c019, plus c066): `pal_crypto_config.h` then leaves
`ARCH_TEST_AEAD_MULTISTAGE_OFF` undefined and adds `ARCH_TEST_PBKDF2`,
`ARCH_TEST_RSA`,
`_PKCS1V15`, `_RSA_1024`, `_RSA_2048`, `_RSA_PKCS1V15_CRYPT`,
`_RSA_PKCS1V15_SIGN`, `_RSA_PKCS1V15_SIGN_RAW`, `_RSA_OAEP`.
`pal_crypto_intf.c` then serves `KEY_DERIVATION_INPUT_INTEGER`
(upstream form). Not in the 1.1.0 list or skipped there, and not
implemented by mbedtls 3.6.7: c064/c065 (hash suspend/resume),
c067/c068 (`verify_bytes`/`verify_key`), c069-c079 (PAKE of the
1.2 API, SPAKE2+), c080 (`psa_key_agreement`).

With `CONFIG_TF_PSA_CRYPTO` (TF-PSA-Crypto 1.2.0) the CMake also
defines `PSA_ARCH_API_1_2`: `pal_crypto_config.h` adds
`ARCH_TEST_JPAKE`, `pal_crypto_intf.c` serves `KEY_AGREEMENT` and
the PAKE calls (upstream form), and the list is
`port/crypto_1.2_tfpsa_testsuite.db`. TF-PSA-Crypto 1.2.0 runs c070
(`set_role`), c076 (`abort`) and c080; it does not pass:

| Test | Reason |
|---|---|
| c064, c065 | `psa_hash_suspend` / `resume` return `PSA_ERROR_NOT_SUPPORTED` |
| c067, c068 | `verify_bytes` / `verify_key` return `PSA_ERROR_NOT_SUPPORTED` |
| c069 | the J-PAKE suite is checked at the first output, not at setup |
| c071-c075, c077 | J-PAKE takes only the ids `client` / `server`; the tests use `user` / `peer`; `set_context` returns `PSA_ERROR_NOT_SUPPORTED` |
| c078 | the test passes `sizeof` of a pointer as the id length |
| c079 | no SPAKE2+ |

A failing PAKE test leaves its operation open in the SPE; after the 8
operation slots are used, later tests get
`PSA_ERROR_INSUFFICIENT_MEMORY`.

Undefined, one line per flag the vendored code reads:

| Flag | Reason |
|---|---|
| `ARCH_TEST_AES_512` | not served by the SPE: AES has no 512-bit key |
| `ARCH_TEST_AES_MMO_ZIGBEE` | not served by the SPE: no AES-MMO hash |
| `ARCH_TEST_ARIA` | not served by the SPE: no ARIA |
| `ARCH_TEST_ASYMMETRIC_ENCRYPTION` | not served by the SPE: no RSA encryption |
| `ARCH_TEST_CCM_MIN_TAG_LEN_8` | defining it drops the 4-byte CCM tag cases; the SPE accepts 4-byte tags |
| `ARCH_TEST_CCM_STAR_NO_TAG` | not served by the SPE: no CCM* without tag |
| `ARCH_TEST_CFB_AES` | not served by the SPE: no CFB mode |
| `ARCH_TEST_CHACHA20` | not served by the SPE: no ChaCha20 |
| `ARCH_TEST_CHACHA20_POLY1305` | not served by the SPE: no ChaCha20-Poly1305 |
| `ARCH_TEST_CIPER_MODE_CFB` | not served by the SPE: no CFB mode (upstream spelling) |
| `ARCH_TEST_CIPHER_MODE_CFB` | not served by the SPE: no CFB mode |
| `ARCH_TEST_DES` | not served by the SPE: no DES |
| `ARCH_TEST_DES_1KEY` | not served by the SPE: no DES |
| `ARCH_TEST_DES_2KEY` | not served by the SPE: no DES |
| `ARCH_TEST_DES_3KEY` | not served by the SPE: no DES |
| `ARCH_TEST_ECC_CURVE_SECP192R1` | not served by the SPE: curve not enabled |
| `ARCH_TEST_ECC_CURVE_SECP224R1` | not served by the SPE: curve not enabled |
| `ARCH_TEST_FFDH` | not served by the SPE: no finite-field DH |
| `ARCH_TEST_FFDHE2048` | not served by the SPE: no finite-field DH |
| `ARCH_TEST_FFDHE3072` | not served by the SPE: no finite-field DH |
| `ARCH_TEST_FFDHE4096` | not served by the SPE: no finite-field DH |
| `ARCH_TEST_HASH_CLONE_UNSUPPORTED` | defining it drops the hash clone cases; the SPE serves `psa_hash_clone` |
| `ARCH_TEST_HASH_RESUME` | not served by the SPE: no `psa_hash_resume` (API 1.1) |
| `ARCH_TEST_HASH_SUSPEND` | not served by the SPE: no `psa_hash_suspend` (API 1.1) |
| `ARCH_TEST_HKDF_EXPAND` | not served by the SPE: no HKDF-Expand-only algorithm in mbedtls 2.28 |
| `ARCH_TEST_HKDF_EXTRACT` | not served by the SPE: no HKDF-Extract-only algorithm in mbedtls 2.28 |
| `ARCH_TEST_JPAKE` | not served by the SPE: no PAKE |
| `ARCH_TEST_MD2` | not served by the SPE: no MD2 |
| `ARCH_TEST_MD4` | not served by the SPE: no MD4 |
| `ARCH_TEST_MD5` | not served by the SPE: no MD5 |
| `ARCH_TEST_PBKDF2` | not served by the SPE: no PBKDF2 (API 1.1) |
| `ARCH_TEST_PKCS1V15` | not served by the SPE: no RSA |
| `ARCH_TEST_RIPEMD160` | not served by the SPE: no RIPEMD-160 |
| `ARCH_TEST_RSA` | not served by the SPE: no RSA |
| `ARCH_TEST_RSA_1024` | not served by the SPE: no RSA |
| `ARCH_TEST_RSA_2048` | not served by the SPE: no RSA |
| `ARCH_TEST_RSA_3072` | not served by the SPE: no RSA |
| `ARCH_TEST_RSA_OAEP` | not served by the SPE: no RSA |
| `ARCH_TEST_RSA_PKCS1V15_CRYPT` | not served by the SPE: no RSA |
| `ARCH_TEST_RSA_PKCS1V15_SIGN` | not served by the SPE: no RSA |
| `ARCH_TEST_RSA_PKCS1V15_SIGN_RAW` | not served by the SPE: no RSA |
| `ARCH_TEST_SHA3_224` | not served by the SPE: no SHA-3 |
| `ARCH_TEST_SHA3_256` | not served by the SPE: no SHA-3 |
| `ARCH_TEST_SHA3_384` | not served by the SPE: no SHA-3 |
| `ARCH_TEST_SHA3_512` | not served by the SPE: no SHA-3 |
| `ARCH_TEST_SHA512_224` | not served by the SPE: no SHA-512/224 |
| `ARCH_TEST_SHA512_256` | not served by the SPE: no SHA-512/256 |
| `ARCH_TEST_SKIP_RSA_2048_GEN_KEY` | RSA only; no RSA in the SPE |
| `ARCH_TEST_SP800_108_COUNTER_CMAC` | not served by the SPE: no SP 800-108 KDF |
| `ARCH_TEST_SP800_108_COUNTER_HMAC` | not served by the SPE: no SP 800-108 KDF |
| `ARCH_TEST_SPAKE2P` | not served by the SPE: no PAKE |
| `ARCH_TEST_SPAKE2P_CMAC` | not served by the SPE: no PAKE |
| `ARCH_TEST_SPAKE2P_HMAC` | not served by the SPE: no PAKE |
| `ARCH_TEST_SPAKE2P_MATTER` | not served by the SPE: no PAKE |
| `ARCH_TEST_TLS12_PSK_TO_MS` | not served by the SPE: its c018 case passes the key as the API 1.1 step `OTHER_SECRET`, which mbedtls 2.28 refuses with `PSA_ERROR_INVALID_ARGUMENT` (evb7, 2026-10-16) |
| `ARCH_TEST_TWISTED_EDWARDS` | not served by the SPE: no EdDSA |
| `ARCH_TEST_XCHACHA20` | not served by the SPE: no XChaCha20 |

## Tests

```
make -C components/3rdparty/psa_arch_tests/port/tests test
tools/psa/psa_arch_tests_vendor.sh --from <psa-arch-tests> <qcbor>
```

The host test prints `test_pal_nvmem: 7/7`: NVMEM reads 0xFF after
`pal_nvmem_reset()`, a 4-byte write at offset 1020 succeeds and at
1021 fails, a 1025-byte read fails, a NULL buffer fails (also with
size 0), an offset past 1024 fails (also with size 0), a size near
`SIZE_MAX` whose sum with the offset wraps fails and leaves the data
unchanged, `pal_system_reset()` is unsupported. The vendor script
prints `psa_arch_tests_vendor: identical to ...` when the vendored
files match upstream.

On the DUT, `tools/psa/psa_arch_run.py` runs one suite image (results:
`components/psa/README.md`, Status). The Crypto suite skips one test:
`port/crypto_1.0.0_testsuite.db` is the upstream 1.0.0 list with
`test_c019, skip`, which `gen_tests_list.py` leaves out of the build.
c019 check 2, `TLS12_PSK_TO_MS_SECP256R1[ECDH]`, passes the ECDH
output as the API 1.1 step `OTHER_SECRET` and expects `PSA_SUCCESS`;
mbedtls 2.28 refuses the step with `PSA_ERROR_INVALID_ARGUMENT`, and
no `ARCH_TEST_*` flag guards the check. On evb7 (2026-10-16) a copy
of c019 without only that vector passed its other 10 checks, the
negative test included.

Library sizes (`size` over the archive members, riscv64-unknown-elf
14.2.1, `-Os`) and the `defconfig_arch_<suite>` image of
`ipro7_psa_demo` (defconfig_ipro7 image: 48954 B text):

| Suite | Tests | text (incl. rodata) | data | bss | image text | image flash |
|---|---|---|---|---|---|---|
| Crypto | 62 | 131910 | 745 | 12517 | 169646 | 173224 B of 256 KB |
| ITS | 10 | 23550 | 416 | 10272 | 70670 | 74104 B |
| Attestation | 1 | 18376 | 21 | 9517 | 65482 | 68512 B |

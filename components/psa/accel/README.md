# components/psa/accel

`ipro_sec`: a PSA transparent accelerator driver that runs part of
the PSA Crypto API of the SPE on the SEC_ENG hardware. One driver
serves both mbedtls 3.6 (`components/crypto/mbedtls3`) and
TF-PSA-Crypto 1.2 (`components/crypto/tfpsa`). It is built only with
`CONFIG_PSA_SEC_ENG_ACCEL` (default n); with the option off no file
of either library tree changes behaviour.

A call the driver does not accelerate returns
`PSA_ERROR_NOT_SUPPORTED` before it touches any state, and the
generated driver wrapper hands the call to the built-in software
driver.

## Scope

On SEC_ENG (each also needs its `PSA_WANT_*` symbol in the library
configuration):

| Area | Accelerated |
|---|---|
| Hash | SHA-1, SHA-224, SHA-256, SHA-384, SHA-512: one-shot and multipart, clone |
| HMAC, HKDF, PBKDF2 | no own code; the built-in code calls the accelerated hash |
| Cipher | AES-128/192/256 ECB and CBC without padding, CTR; one-shot and multipart |
| AEAD | AES-128/192/256 GCM; one-shot and multipart; tag length 4, 8, 12..16; any nonce length >= 1 |
| ECDSA | P-256 randomized sign (any hash algorithm), verify (randomized and deterministic algorithms) |
| ECDH | P-256 raw key agreement |
| Key | P-256 key pair generation, public key export |
| Entropy | TRNG seed of the CTR_DRBG (see Entropy) |

Software fallback (`PSA_ERROR_NOT_SUPPORTED`):

- Hash: MD5, RIPEMD-160, SHA-512/224, SHA-512/256, SHA3.
- Cipher: AES with a key size other than 128, 192, 256 bits; CBC
  with PKCS7 padding, CFB, OFB, XTS, CCM-STAR; ARIA, Camellia,
  ChaCha20, DES.
- AEAD: CCM, ChaCha20-Poly1305; GCM with a tag length other than
  4, 8, 12..16 (the built-in driver then reports the error).
- MAC: CMAC; HMAC itself (it is built-in code over the hash).
- ECC: every curve other than secp256r1; deterministic ECDSA sign;
  compressed public points (verify, ECDH); a verify or sign with
  `hash_length` 0; ECDH with a peer key that is not an uncompressed
  point; key generation outside the default production path.
- RSA, DH, FFDH, J-PAKE, SPAKE2+, EC-JPAKE, key derivation.

The case lists are the `switch` and `if` guards in
`ipro_sec_hash.c`, `ipro_sec_cipher.c`, `ipro_sec_aead.c` and
`ipro_sec_ecc.c`; the driver entry points are declared in
`ipro_sec_driver.h`.

## Layering

```
PSA Crypto core (mbedtls 3.6 / TF-PSA)
  psa_crypto_driver_wrappers (patched)
    ipro_sec driver          components/psa/accel
      HAL v2                 bsp/hal/sec/{inc,src}/hal_sec_*_v2.[ch]
        drv_sec_eng          bsp/drivers/*/std_drv (HAL only)
```

- The driver includes `hal_sec_*_v2.h` and nothing from
  `drv_sec_eng` or `std_drv`. Only the HAL touches the driver
  layer.
- The v1 `hal_sec_*` API is not called and not changed. The driver
  and v1 both program the same engines, so they must not run at
  the same time (see Limits).
- The mbedtls 2.28 hardware accelerator port
  (`components/crypto/mbedtls/port/**`) is not used and not
  edited. The driver is not an `*_ALT` module: TF-PSA-Crypto 1.x
  has no ALT interface.

`ipro_sec_driver_contexts.h` holds the operation contexts. They
are plain data (HAL v2 contexts have no pointers), so the library
copies and clones them freely.

## Build

`CONFIG_PSA_SEC_ENG_ACCEL` (`components/psa/Kconfig`, menu "PSA")
depends on `MBEDTLS_V3 || TF_PSA_CRYPTO` and selects
`SEC_ENG_HAL_V2`. `components/psa/CMakeLists.txt` adds this
directory when it is set. `PSA_SPE` selects `SEC_ENG_HAL_V2` on its
own for the SPM; the option adds only the driver.

`CONFIG_SEC_ENG_HAL_V2_CHUNK` (128 to 4096, multiple of 128,
default 256) sets the size of each HAL scratch buffer; both accel
defconfigs use 512.

| Defconfig (`apps/security/ipro7_psa_spe/`) | Library |
|---|---|
| `defconfig_ipro7_devhuk_accel` | mbedtls 3.6, driver on |
| `defconfig_ipro7_devhuk_tfpsa_accel` | TF-PSA-Crypto 1.2, driver on |

Each is `defconfig_ipro7_devhuk` or `defconfig_ipro7_devhuk_tfpsa`
plus `CONFIG_PSA_SEC_ENG_ACCEL=y` and CHUNK 512.

```
./build_freertos.sh -d defconfig_ipro7_devhuk_accel \
    security/ipro7_psa_spe
```

The SPE configs `mbedtls3_config.h` and `tfpsa_config.h` define
`MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS` (see Buffers and DMA).

## Wrapper patch

The PSA core reaches a driver through generated files. The driver
is added to those files of the vendored trees:

| File | Edit |
|---|---|
| `psa_crypto_driver_wrappers.h` (3.6 `library/`, TF-PSA `core/`) | `#include "ipro_sec_driver.h"`, driver id 16, a try-first call or a `case` per entry point |
| `psa_crypto_driver_wrappers_no_static.c` | `ipro_sec_export_public_key` |
| `include/psa/crypto_driver_contexts_primitives.h` | `ipro_sec_ctx` in the hash and cipher unions |
| `include/psa/crypto_driver_contexts_composites.h` | `ipro_sec_ctx` in the AEAD union |

Every added block sits between `#if defined(CONFIG_PSA_SEC_ENG_ACCEL)`
and `#endif /* CONFIG_PSA_SEC_ENG_ACCEL */`. A one-shot call tries
the driver first and continues with the built-in driver on
`PSA_ERROR_NOT_SUPPORTED`. A multipart setup does the same and
records the driver id in the operation; later calls switch on it.

`tools/psa/psa_sec_eng_patch.py` writes the blocks:

```
python3 tools/psa/psa_sec_eng_patch.py mbedtls3|tfpsa
python3 tools/psa/psa_sec_eng_patch.py mbedtls3|tfpsa --check
```

It anchors on the generated function names and fails when an
anchor is missing or the tree is already patched. `--check` writes
nothing: it prints `patched` or `not patched` and exits 0 or 1.

The result is kept as `port/sec_eng_driver.patch` in each tree
directory (`components/crypto/mbedtls3/port/`,
`components/crypto/tfpsa/port/`). To regenerate it after the
script changes, run it on a tree restored to upstream, then from the
repository root:

```
git diff --relative=components/crypto/mbedtls3/mbedtls/ 59a104765 \
    -- components/crypto/mbedtls3/mbedtls \
    > components/crypto/mbedtls3/port/sec_eng_driver.patch
git diff --relative=components/crypto/tfpsa/tf-psa-crypto/ 59a104765 \
    -- components/crypto/tfpsa/tf-psa-crypto \
    > components/crypto/tfpsa/port/sec_eng_driver.patch
```

`59a104765` is the commit with the upstream copy of each tree.
`patch -p1 -R -d <tree> < port/sec_eng_driver.patch` restores
upstream; `components/crypto/{mbedtls3,tfpsa}/UPSTREAM.md` has the
upstream tag or release and the comparison commands.

## Entropy

`ipro_sec_entropy.c` replaces the v1 entropy ports
(`mbedtls3/port/entropy_hw_poll.c`, `tfpsa/port/entropy_get.c`,
both compiled out when the option is set):

- 3.6: `mbedtls_hardware_poll` (`MBEDTLS_ENTROPY_HARDWARE_ALT`).
- TF-PSA: `mbedtls_platform_get_entropy`
  (`MBEDTLS_PSA_DRIVER_GET_ENTROPY`); it reports 8 bits of entropy
  per output byte and returns `PSA_ERROR_NOT_SUPPORTED` for a
  nonzero `flags`.

Both call `hal_sec_trng_v2_read`. On failure the output is wiped
and the call returns `MBEDTLS_ERR_ENTROPY_SOURCE_FAILED` (3.6) or
`PSA_ERROR_INSUFFICIENT_ENTROPY` (TF-PSA).

## ECC

All P-256 work is `hal_sec_p256_v2_*` on the PKA. Integers are
32 bytes big-endian; a private key buffer is the 32-byte scalar.

- Sign (`ipro_sec_sign_hash`): randomized ECDSA only. The nonce is
  drawn from the TRNG. The HAL multiplies G by the nonce with a
  fixed-sequence Montgomery ladder (D3): 256 steps, each the same
  two complete point additions, the key bit only selects register
  bases. The signature is 64 bytes `r || s`.
- Verify: a public key or a key pair (the public point is then
  computed with the ladder). The HAL verify uses a Shamir sum and
  runs in variable time; its inputs are public. A signature of a
  length other than 64 bytes returns `PSA_ERROR_INVALID_SIGNATURE`.
- ECDH: `PSA_ALG_ECDH`, uncompressed peer point (`0x04 || X || Y`),
  32-byte shared secret. The HAL validates the peer point first.
- Key generation: scalar in `[1, n - 1]` from the TRNG. Export:
  the uncompressed 65-byte point.

Falls back: deterministic ECDSA sign, every other curve, compressed
points, `hash_length` 0.

## Buffers and DMA

`spm_services` copies every iovec of an NSPE call into SPE RAM
(`spm_bounce_in`, `spm_bounce_out`) before a service runs. The
SPE configs therefore define `MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS`
so the PSA core does not copy again, and the driver may see input
and output at the same address.

- Hash and AES read the input in place when it is word-aligned and
  in the cacheable OCRAM alias; otherwise they copy it into the
  HAL scratch (at most CHUNK bytes per job).
- AES writes the output in place only for whole 32-byte cache
  lines at a 32-byte aligned address (HAL `sec_v2_direct_out`);
  otherwise through the scratch. The bounce layout aligns each
  iovec to 8 bytes (`SPM_BOUNCE_ALIGN`), so only `out[0]`, which
  starts the bounce buffer, reliably takes direct output.
- ECB and CBC keep a partial input block in the operation. A
  multipart update that holds such a block and whose output
  overlaps its input would shift output against input; the driver
  returns `PSA_ERROR_INVALID_ARGUMENT` for it. Without a buffered
  block in-place use is correct.
- `finish` of ECB or CBC with a buffered partial block returns
  `PSA_ERROR_INVALID_ARGUMENT`.

## Errors

| HAL v2 return | PSA status |
|---|---|
| `HAL_SEC_V2_OK` | `PSA_SUCCESS` |
| `HAL_SEC_V2_ERR_PARAM` | `PSA_ERROR_INVALID_ARGUMENT` (verify: `PSA_ERROR_INVALID_SIGNATURE`) |
| `HAL_SEC_V2_ERR_VERIFY` | `PSA_ERROR_INVALID_SIGNATURE` |
| `ERR_TIMEOUT`, `ERR_HW`, `ERR_ACCESS` | `PSA_ERROR_HARDWARE_FAILURE` |

An engine that stays busy for `SEC_V2_SPIN` register reads
(about 30 ms at 192 MHz) makes the HAL job return
`HAL_SEC_V2_ERR_TIMEOUT` after `hal_sec_v2_recover()`: SEC_ENG soft
reset, TRNG re-init, and a re-claim of group0 if it was claimed
before. The failed call returns `PSA_ERROR_HARDWARE_FAILURE`; the
next call runs on a recovered engine. Outputs of a failed call are
wiped.

## Tests

- Host KATs, `apps/security/ipro_sec_kat/host`: the driver and HAL
  v2 on a model of the engines, with the mbedtls library as
  oracle: `make -C apps/security/ipro_sec_kat/host test`
  (CHUNK 128 and 4096, ASan and UBSan).
- On evb7: `apps/security/ipro_sec_kat` runs the same driver KATs
  on silicon plus the P-256 and hardware-key checks.
- psa-arch-tests (`components/3rdparty/psa_arch_tests`,
  `tools/psa/psa_arch_run.py`) through the SPE with the driver on:
  the crypto, ITS and attestation suites. Results at 262f3c799:
  crypto 64/64 on 3.6 and 67/67 on TF-PSA, ITS 10/10, attestation
  1/1; the `ipro7_psa_demo` command set 17/17.

## Performance

evb7, IPRO7 at 192 MHz, SPE with the driver off (SW) and on (HW),
time of the PSA call from the NSPE through `psa_call`, in us, best
of 4 to 5 runs, at 262f3c799. The SW columns are the same SPE with
`MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS` on.

| Operation | 3.6 SW | 3.6 HW | x | TF-PSA SW | TF-PSA HW | x |
|---|---|---|---|---|---|---|
| SHA-256, 16 KB | 4359 | 783 | 5.6 | 4791 | 859 | 5.6 |
| AES-128-CBC, 16 KB | 8338 | 1887 | 4.4 | 8178 | 1879 | 4.4 |
| P-256 ECDSA sign | 274437 | 79807 | 3.4 | 391057 | 79917 | 4.9 |

Earlier 3.6 runs (images of 3abd09995 and 25a3d3d96):

| Operation | SW | HW | x |
|---|---|---|---|
| HMAC-SHA-256, 16 KB | 4614 | 1245 | 3.7 |
| AES-128-GCM, 3968 B | 9478 | 629 | 15 |
| P-256 verify | 1326408 | 146503 | 9.1 |
| P-256 export public key + ECDH (two ladders) | 1015479 | 154012 | 6.6 |

Measured with the `psa_perf [runs]` shell command of
`apps/security/ipro7_psa_demo` (`perf_cli.c`): `runs` is 1 to 100,
default 4; it prints the best of `runs` in us from `rdtime` (4 MHz).
Data goes in calls of at most 3968 bytes (the bounce limit less the
header): 16 KB is 5 calls of one multipart operation. Tests:
`sha256 16KB`, `hmac-sha256 16KB`, `aes128-cbc 16KB`,
`aes128-gcm 3968B`, `p256 sign`, `p256 verify`, `p256 ecdh`.

## Limits

- The PKA constant-time argument rests on three assumptions that
  are not verified on silicon: (A1) the time of one PKA MMUL, MADD
  or MSUB does not depend on operand values or register indices;
  (A2) an MMUL result is canonical (below the modulus); (A3) the
  PKA register clear (CREG) clears the register SRAM.
- The PKA functions of `drv_sec_eng` return `void`. A PKA
  operation that never completes is not reported to the HAL, which
  has no PKA timeout of its own. Sign, ECDH and public key results
  are checked for being on the curve (`HAL_SEC_V2_ERR_HW`); a
  verify of a bad result fails. `HAL_SEC_V2_ERR_TIMEOUT` comes
  from the SHA, AES and GHASH jobs and from a failed lock hook.
- v1 `hal_sec_*` and HAL v2 share no lock. The SPE is single
  threaded, so there is no concurrent use; do not call v1 and v2
  from different contexts at the same time. `hal_sec_v2_set_lock_ops`
  installs lock hooks for a threaded user of v2 only.
- Only `out[0]` of an NSPE call reliably takes direct output
  (Buffers and DMA).
- The driver is built into the SPE only; the NSPE has no access to
  SEC_ENG.

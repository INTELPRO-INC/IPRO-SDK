# SEC_ENG HAL

`bsp/hal/sec/` holds both SEC_ENG HALs. v1 (the default of every
app): `inc/hal_sec_{common,hw_common,hash,aes,gmac,trng,dsa,ecdsa,p256}.h`
and `src/hal_sec_{common,hash,aes,gmac,trng,dsa,ecdsa,p256}.c`.
`bsp/hal/CMakeLists.txt` adds `sec/inc` to the HAL include path.

## HAL v2

`hal_sec_<module>_v2`: a second HAL for the SEC_ENG blocks (SHA,
AES, GMAC, PKA, TRNG) beside the v1 `hal_sec_*`. Files:

- `bsp/hal/sec/inc/hal_sec_{common,hash,aes,gmac,ecc,trng}_v2.h`
- `bsp/hal/sec/src/hal_sec_{common,hash,aes,gmac,ecc,trng}_v2.c`
- `bsp/hal/sec/src/hal_sec_v2_priv.h` (register map, job runner, DMA
  helpers; not for users)

`CONFIG_SEC_ENG_HAL_V2` (`bsp/Kconfig`, default n) builds the six
`.c` files (`bsp/hal/CMakeLists.txt`). `CONFIG_SEC_ENG_HAL_V2_CHUNK`
(128 to 4096, multiple of 128, default 256) is the size of each
per-block scratch buffer.

## Why v2 beside v1

The v1 API (`hal_sec_hash.h`, `hal_sec_aes.h`, `hal_sec_gmac.h`,
`hal_sec_trng.h`, `hal_sec_p256.h`) stays the default. With
`SEC_ENG_HAL_V2` off, no v1 file and no non-PSA build changes.

v1 does not fit a PSA driver:

- A handle holds addresses of its own buffers (the link context is
  initialized with them), so it cannot be copied or moved. A PSA
  operation context is copied (hash clone).
- The engine state stays in the engine between calls; two
  operations cannot be interleaved.
- The calls have no timeout or error path for a stuck engine.

v2 contexts are plain data. Each call programs the block from the
context in LINK mode, runs one job per piece of data and saves the
state back into the context. Calls return `HAL_SEC_V2_*` codes:

| Code | Value | Meaning |
|---|---|---|
| `HAL_SEC_V2_OK` | 0 | done |
| `HAL_SEC_V2_ERR_PARAM` | -1 | bad argument, point or scalar |
| `HAL_SEC_V2_ERR_TIMEOUT` | -2 | engine busy too long (after recover), or lock hook failed |
| `HAL_SEC_V2_ERR_HW` | -3 | hardware fault: TRNG, off-curve result |
| `HAL_SEC_V2_ERR_VERIFY` | -4 | signature does not verify |
| `HAL_SEC_V2_ERR_ACCESS` | -5 | group0 claim failed |

Users include only `hal_sec_*_v2.h`. They never call `drv_sec_eng`
(`std_drv`); only the HAL does.

## API

### Common (`hal_sec_common_v2.h`)

| Function | Purpose |
|---|---|
| `hal_sec_v2_init` | SEC_ENG clock on, TRNG on |
| `hal_sec_v2_claim` | claim group0 (ID0) on all six blocks |
| `hal_sec_v2_claimed` | 1 when every block is owned by group0 only |
| `hal_sec_v2_recover` | SEC_ENG soft reset, TRNG init, re-claim if claimed before |
| `hal_sec_v2_set_lock_ops` | per-block lock hooks; none set: every lock succeeds |
| `hal_sec_v2_zeroize` | memset that the compiler keeps |

### Hash (`hal_sec_hash_v2.h`)

SHA-1, SHA-224, SHA-256, SHA-384, SHA-512 (`enum
hal_sec_hash_v2_type`). Context `hal_sec_hash_v2_ctx_t`: state,
partial block, total length.

| Function | Purpose |
|---|---|
| `hal_sec_hash_v2_digest_len` | digest length, 0 for an unknown type |
| `hal_sec_hash_v2_setup` | start an operation |
| `hal_sec_hash_v2_update` | absorb data |
| `hal_sec_hash_v2_finish` | write the digest, wipe the context |
| `hal_sec_hash_v2_abort` | wipe the context |

### AES (`hal_sec_aes_v2.h`)

ECB, CBC, CTR and CTR32 (only the low 32 counter bits advance; GCM
inc32), 128, 192 and 256-bit keys, software key or hardware key.

| Function | Purpose |
|---|---|
| `hal_sec_aes_v2_setkey` | software key, 16, 24 or 32 bytes |
| `hal_sec_aes_v2_setkey_hw` | eFuse hardware key `sel` 0-3, length 16 or 32, key source 0 |
| `hal_sec_aes_v2_set_hw_src` | SE_AES_SBOOT key source 0 or 1 of a hardware key |
| `hal_sec_aes_v2_set_iv` | CBC IV, or CTR / CTR32 counter block |
| `hal_sec_aes_v2_crypt` | `len % 16 == 0`; in place allowed; IV or counter advances |
| `hal_sec_aes_v2_abort` | wipe the context |

### GHASH (`hal_sec_gmac_v2.h`)

The GMAC block for AES-GCM. Context `hal_sec_ghash_v2_ctx_t`.

| Function | Purpose |
|---|---|
| `hal_sec_ghash_v2_setup` | set the hash key H |
| `hal_sec_ghash_v2_update` | absorb data; a partial block is kept |
| `hal_sec_ghash_v2_pad` | zero-pad and absorb a pending partial block |
| `hal_sec_ghash_v2_finish` | pad, write X, wipe the context |
| `hal_sec_ghash_v2_abort` | wipe the context |

### ECC (`hal_sec_ecc_v2.h`)

P-256 only. Integers are 32-byte big-endian, points 64 bytes
`X || Y`.

| Function | Purpose |
|---|---|
| `hal_sec_p256_v2_check_point` | x, y < p and on the curve, else `ERR_PARAM` |
| `hal_sec_p256_v2_check_scalar` | `1 <= d <= n - 1`, else `ERR_PARAM` |
| `hal_sec_p256_v2_pubkey` | Q = dG |
| `hal_sec_p256_v2_keygen` | random d from the TRNG, and Q |
| `hal_sec_p256_v2_sign` | randomized ECDSA, `r || s` |
| `hal_sec_p256_v2_verify` | `OK` or `ERR_VERIFY`; variable time |
| `hal_sec_p256_v2_ecdh` | x(d * peer); peer validated first |

### TRNG (`hal_sec_trng_v2.h`)

| Function | Purpose |
|---|---|
| `hal_sec_trng_v2_init` | TRNG on (IPRO7: ROSC4-7 first) |
| `hal_sec_trng_v2_read` | fill a buffer; a failed block re-arms the TRNG, one retry |
| `hal_sec_trng_v2_enable` | 0: TRNG off; else on and wait for instantiate |
| `hal_sec_trng_v2_generate` | one 256-bit generation, no re-arm; all-zero is `ERR_HW` |
| `hal_sec_trng_v2_reg_read` | read `enum hal_sec_trng_v2_reg`; unknown reads 0 |
| `hal_sec_trng_v2_reg_write` | write it; unknown ignored |

The register accessors and `generate` serve the HUK provisioning
code (`components/psa/spm/huk_prov_hw.c`).

## Engine and DMA model

- A hash, AES or GHASH job is one LINK-mode job: `sec_v2_job` waits
  for idle, writes the link address, sets LINK mode, triggers and
  waits for idle. The link configuration (control word, source,
  destination, IV, key or state) is built in a static per-block
  scratch buffer, `__attribute__((aligned(32)))`, of the
  configuration header plus CHUNK bytes. The engine addresses it
  through the non-cacheable alias (`hal_get_noncacheable_addr`);
  the CPU writes it through the cache and flushes the lines before
  and after the job (`sec_v2_flush`).
- Input the engine may read in place goes straight to the engine:
  4-byte aligned and inside the cacheable OCRAM alias
  `0x11000000-0x11FFFFFF` (`sec_v2_direct`), up to
  `SEC_V2_DIRECT_MAX` (16 KB) per job. Any other input is copied
  into the scratch, at most CHUNK bytes per job.
- AES output goes directly to the caller's buffer only when the
  input is direct and the output is 32-byte aligned and whole
  32-byte lines (`sec_v2_direct_out`), so no other data shares a
  line with the output. Otherwise it passes through the scratch.
- Hash and GHASH keep a partial block in the context and run
  whole blocks only.
- After each job the scratch is zeroized and flushed. Contexts are
  zeroized by `finish` and `abort`.
- A lock hook pair (`hal_sec_v2_set_lock_ops`) is called per block
  around each call; the default has no locking.

## Group0 claim and recovery

`hal_sec_v2_claim` writes the protect register of each block that
reads as unclaimed so that group0 (master ID0) owns it. After the
claim ID1 reads 0xDEADBEEF from the block and cannot trigger or
release it. `hal_sec_v2_claimed` reads `SE_CTRL_PROT_RD` back. A
SEC_ENG or chip reset frees the claim.

A job that stays busy for `SEC_V2_SPIN` register reads (about 30 ms
at 192 MHz) calls `hal_sec_v2_recover` and returns
`HAL_SEC_V2_ERR_TIMEOUT`. `hal_sec_v2_recover` does the SoC SEC_ENG
soft reset, re-initializes the TRNG and, if `hal_sec_v2_claim` was
called before, claims again (`HAL_SEC_V2_ERR_ACCESS` on failure).
The SPE uses the claim with `CONFIG_PSA_TZC_ISOLATION`
(`components/psa/spm/spm_tzc.c`).

## P-256

`hal_sec_ecc_v2.c` drives the PKA register machine through
`drv_sec_eng`. Only 256-bit registers are used.

- Point addition is the complete projective formula for a = -3 of
  Renes, Costello and Batina (2016, algorithm 4) in Montgomery
  form. It is valid for P = Q and for the point at infinity, so the
  ladder needs no special case.
- The scalar multiplication is a Montgomery ladder over all 256
  bits of the scalar. Each bit runs the same two additions; the bit
  only selects the register bases, with arithmetic and without a
  branch. The sequence of PKA operations is the same for every
  scalar.
- The projective result becomes affine through a Fermat inversion
  (`z^(p - 2)`), a square-and-multiply loop whose exponent is a
  public constant. The same loop gives `k^-1` mod n for sign.
- The affine result is checked against the curve equation
  (`y^2 = x^3 - 3x + b`); a result off the curve returns
  `ERR_HW`. The input point of ECDH and the base point are also
  checked.
- Sign draws k from the TRNG in `[1, n - 1]` (up to 64 draws) and
  retries when `r` or `s` is zero (up to 16 times).
- Verify uses a Shamir sum with the table `{G, Q, G + Q}`; it runs
  in variable time on public data.
- At the end of each operation every PKA register used is
  overwritten with zero and cleared (`Sec_Eng_PKA_CREG`), then the
  PKA is reset, because a PKA reset alone keeps the register SRAM.

The code assumes that the time of one PKA MMUL, MADD or MSUB does
not depend on operand values or register indices; this is not
verified on silicon.

## Users

| User | Use |
|---|---|
| `components/psa/accel/` | the ipro_sec PSA driver: hash, AES, GHASH, P-256, TRNG |
| `components/psa/spm/huk.c` | AES hardware-key block for the HUK |
| `components/psa/spm/huk_prov_hw.c` | TRNG registers and generation for HUK provisioning |
| `components/psa/spm/spm_nspe.c` | SHA-256 measurement of the NSPE image |
| `components/psa/spm/spm_tzc.c` | `hal_sec_v2_claim` |

`PSA_SPE` selects `SEC_ENG_HAL_V2`; so does
`PSA_SEC_ENG_ACCEL`. See `components/psa/accel/README.md`.

## Both chips

The code is common to IPRO6 and IPRO7. The SoC differences are
behind `CONFIG_IPRO7` in `hal_sec_common_v2.c` (clock gate and
SEC_ENG reset through the GLB registers on IPRO7, the BSP GLB
calls on IPRO6) and `hal_sec_trng_v2.c` (ROSC configuration, IPRO7
only). Build both chips when changing a v2 file or any `hal_sec_*`
file.

## Tests

`apps/security/ipro_sec_kat` runs the known-answer tests of this
HAL on evb7 and, on a host model of the engines, on the build
machine; see its `README.md`.

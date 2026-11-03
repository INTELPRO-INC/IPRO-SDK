# components/psa

PSA Level 2 reference implementation for IPRO7: an M-mode PSA Root of
Trust (SPE) that isolates a U-mode non-secure application (NSPE) with
the PMP and, on TEE parts, the sPMP, and serves Crypto, Internal
Trusted Storage and Initial Attestation to it through `ecall`. Boot,
OTA and anti-rollback stay in `ipro_boot2`. It has run on one IPRO7
dev part, first with a blank eFuse, then with secure boot provisioned
(PK hash, `sign_mode`, anti-rollback counter) and the development HUK
in eFuse (Status below); the core facts it relies on come from
`ipro7_pmp_probe` (Silicon notes below).

The design rationale (isolation model, memory map, key hierarchy, risks)
is the vault note `2026-10-15-ipro7-psa-l2-design`
(IPRO/Projects/IPRO7/Notes).

SEC_ENG hardware acceleration of PSA Crypto: the `ipro_sec` driver
is `accel/README.md`; the HAL under it is
`bsp/hal/sec/README.md`.

Everything here is behind default-n Kconfig symbols (`menu "PSA"` in
`Kconfig`); builds that do not set them are unchanged.

| Symbol | Meaning |
|---|---|
| `PSA_ENABLE` | set by `PSA_SPE` and `PSA_CLIENT`; gates the shared-file edits |
| `PSA_SPE` | build the SPM, services and mbedtls PSA core (needs `CONFIG_NO_RTOS=y`) |
| `PSA_CLIENT` | build the NSPE client library (needs `FREERTOS_PORT_NSPE`) |
| `FREERTOS_PORT_NSPE` | FreeRTOS U-mode port `portable_ipro7_nspe` instead of `portable_ipro7` |
| `MBEDTLS_PSA_CRYPTO` | compile the mbedtls PSA crypto core |
| `PSA_HUK_IN_RAM`, `PSA_HUK_KAT`, `PSA_DEV_HUK`, `PSA_HUK_KEY_SEL` | HUK source options, see `ipro7_psa_spe/README.md` |
| `PSA_HUK_LOCK` | after SPE init set `tzc_sboot_done` = 0xF and prove the hardware key is the zero key (default y without `PSA_HUK_IN_RAM`) |
| `PSA_HUK_PROVISION` | HUK provisioning image (`defconfig_ipro7_hukprov` only): the SPE draws the HUK from the TRNG, burns key slot 4 and its locks, never launches an NSPE; see `ipro7_psa_spe/README.md` |

## Architecture

```
 flash 0x13000000  SPE  (M-mode)  256 KB   SPM, PMP, ecall, services
 flash 0x13040000  NSPE (U-mode)  <= 256 KB header, text, rodata, data LMA
 OCRAM 0x11010000  boot2 hand-off 1 KB, then SPE data/bss/stack
 OCRAM 0x11020000  NSPE 128 KB    OCRAM2 0x11040000 NSPE 64 KB
 PSRAM 0x1e000000  NSPE 8 MB      UART0 0x3000B000 NSPE console
 DMA0 0x30082000, USB 0x30084000  NSPE with CONFIG_PSA_NSPE_DMA_USB
```

One signed FW image = SPE (padded with 0xFF to 0x40000) followed by the
NSPE image. `ipro_boot2` verifies and maps it as it does any FW image
and jumps to the SPE. The SPE checks the NSPE header, measures the
image, programs the PMP (and on TEE parts the same entries into the
sPMP plus `smpuswitch0`) and enters the NSPE in U-mode. Hardware
interrupts reach the NSPE only as virtual IRQs delivered by the SPE.
Addresses are the values in `include/psa_ipro/memmap.h`.

## Bus-master isolation (`spm/spm_tzc.c`)

`CONFIG_PSA_TZC_ISOLATION` (default n): before the NSPE runs, the SPE
claims SEC_ENG group0 on all six blocks (`hal_sec_v2_claim`; ID1 then
reads 0xDEADBEEF and cannot trigger or release a block; a SEC_ENG
reset in `hal_sec_v2_recover` re-claims), then writes, locks and reads
back TZC_SEC (0x30006000). The CPU is master
ID0 in M and U mode, so TZC_SEC separates bus masters only and PMP
still separates SPE and NSPE. Policy (`spm_tzc_regs[]`):

| Register | Value | Effect for ID1 |
|---|---|---|
| OCRAM / MM2OCRAM R0 | 0x40..0x7F, ID0 only | no 0x11010000-0x1101FFFF (boot2 params, SPE RAM) |
| SF RX | ID0 only | no flash (ID1 sees raw flash, not the XIP map) |
| APB slots 0x10C / 0x110 | 01 (ID0) except the `spm_periph` slots | UART0 only |
| DMA_M_ID | channels 0-7 ID1 | DMA is ID1 |
| TZMID | USB, SDHA, EMAC, M154 ID1; CCI ID1 with `CONFIG_PSA_TZC_CCI_ID1` | |
| DMA_S, TMR_S | ID0 only | no DMA, WDT0, TMR2/3 registers |
| AHB slaves 0x108 | PWR, EMAC, DMA, MM, SDHA ID0; USB both | no PDS, HBN, HBN RAM, MM, EMAC, SDH registers |
| SE_CTRL_0 0xF40 | SHA, AES, TRNG, PKA, CDET, GMAC ID0, CRMD 1 | no SEC_ENG |
| SE_CTRL_1 0xF44 | SF_CR, SF_SEC ID0, CRMD 1 | no SF_CTRL (flash controller, XIP offset) |

The SE and SF slave filters work only with their CRMD bit set; with
CRMD 0 an ID1 master reads both blocks whatever the ID fields say.
The SE fields cover SHA, AES, TRNG, PKA, CDET and GMAC
(0x30080000-0x300805FF); the SE_CTRL page 0x30080F00 has no field:
ID1 reads SE_CTRL_PROT_RD (the ID0 bits, 0x555) and writes the
reserved words 0x30080F04 and 0x30080F08 (0x30080F0C takes no write,
over CCI either).

Locks: region and mask lock bits, TZMID, APB, DMA_M_ID, DMA_S and
TMR_S lock registers, the AHB lock bits, SE_CTRL_2. Then a tamper
write of the reset values must not take. A denied ID1 read returns 0xDEAD2019 (OCRAM), 0xDEADBEEF
(flash, SEC_ENG, SF_CTRL), 0xDEADA9B1 (APB), 0xDEAD201A (DMA
registers) or 0xDEADDEAD (MM) with no DMA error; a denied write is
dropped. RST pin, GLB SW system reset, SW POR reset and a WDT reset
each set every TZC register, the AHB, SE and SF ones too, back to
its reset value and clear the locks; the SPE sets them again on every boot. evb7 runs
2026-10-22: vault
IPRO/Projects/IPRO7/Notes/2026-10-22-ipro7-psa-m5-tzc-survey.md.

## NSPE DMA0 and USB (`CONFIG_PSA_NSPE_DMA_USB`)

Default n; on in `ipro7_psa_spe` `defconfig_ipro7` and
`defconfig_ipro7_devhuk` and in `ipro7_psa_demo` `defconfig_ipro7`.

- SPE (`spm_periph.c`): DMA0 and USB become NSPE PMP windows (PMP
  and sPMP use all 8 entries) with their IRQs `DMA0_ALL_IRQn` and
  `USB_IRQn`. A U-mode `lw`/`sw`/`c.lw`/`c.sw` that faults on a
  register of `spm_reg_grants[]` is done by the SPE (`spm_trap.c`,
  decoder `spm_ldst.h`); a store changes only the bits of the write
  mask:

  | GLB register | Write mask | Used by |
  |---|---|---|
  | 0x30000228 usb_xcvr | 0x00F11770 | `drv_usb_v1.c` |
  | 0x3000022C usb_xcvr_config | 0x77777FFF | `drv_usb_v1.c` |
  | 0x30000110 GPIO_CFGCTL4 (pins 8, 9 = USB DM/DP) | all but func_sel, which a store sets to 10 (analog) | `GLB_GPIO_Init` |
  | 0x30000190 GPIO_CFGCTL35 | 0x00000300 (OE of pins 8, 9) | `GLB_GPIO_Init` |

  The access address must equal mtval. Every other GLB access still
  faults. At each NSPE launch, before the NSPE is measured, the SPE
  ungates the DMA0 clock, disables its 4 channels (channels 4-7 of
  the BSP never complete on evb7), waits for them to stop (else
  `spm_fatal`) and clears their TC and error flags, disables the USB core and
  clears usb_xcvr PU_USB and USB_ENUM (the host sees a detach), so no
  transfer of an earlier NSPE runs on.
- Kconfig: the SPE option needs `CONFIG_PSA_TZC_ISOLATION` (both are
  master ID1, kept to NSPE memory and NSPE peripherals); with it off
  `spm_periph.c` stops the build.
- `SPM_PMP_ENTRIES` is 8 in every build: with the option off the
  extra entries are written off and `smpuswitch0` is 0xFF, so the
  SPE binary differs from 83c098af3 with the same PMP behaviour.
- NSPE: `IPRO_HAL_OS_IRQ_SEAM` is defined for the BSP drivers, so
  `drv_usb_v1.c` and `drv_dma.c` register and enable their IRQs
  through the vIRQ table (`ipro7_psa_demo/nspe_irq_glue.c`).

evb7 2026-10-22, devhuk and product SPE: NSPE `dmaprobe` 140/140 on
channels 0-3 (reads of SPE RAM and its 0x21010400 alias, boot2
params, flash raw 0x0, 0x15000, 0xDA000 and the 0x23000000 alias,
SF_CTRL, SF AES keys and IVs, SF_CTRL_BUF, PDS, HBN, HBN RAM, eFuse,
GLB, TZC, every SEC_ENG sub-block, DMA, EMAC, SDH and MM registers
denied; NSPE OCRAM and UART0 copied; an LLI item in SPE RAM loads as
0xAAAAAAAA, one in NSPE OCRAM loads), `probe grant`. ID1 DMA writes
over CCI readback (`c_dmawr.py`, each target first written over CCI
as a control): HBN RAM, SHA and AES MSA, SF_CTRL, SF AES IV, DMA
channel registers, SPE RAM and boot2 params unchanged, PSRAM
written. A self-linked channel
stops at an NSPE reset and the USB device enumerates again. CDC-ACM
IN vs `ipro7_demo` in alternating pairs: see
`ipro7_psa_demo/README.md`.

## Flash layouts

boot2 maps the FW image (FW-A + 0x1000, after the 4 KB bootheader)
to XIP 0x13000000; with FW-A at 0x14000 a flash address `a` appears
at 0x13000000 + `a` - 0x15000. The PSM alias must stay outside the
NSPE flash PMP entry (XIP 0x13040000 + 256 KB). `its_lfs_mount()`
checks it before the mount (`spm/spm_harden.c`,
`spm_psm_layout_check`): the PSM must be 4 KB aligned, have an XIP
alias outside the SPE and NSPE flash windows (0x13000000 + 512 KB),
miss both FW entries and lie above them; otherwise the SPE logs
`[SPE] PSM layout <addr>+<size> img <off>: <reason>` and ITS stays
down (STORAGE_FAILURE), with nothing written to flash.

`apps/security/ipro7_psa_spe/partition_cfg_1M_psa_ota.toml` (1 MB,
run on silicon):

```
0x000000 Boot2 (0x12000)   0x012000 PT0   0x013000 PT1
0x014000 FW-A 0x60000 (bootheader 4 KB, SPE 256 KB, NSPE <= 124 KB)
0x074000 FW-B 0x50000 (OTA package, xz)
0x0C4000 PSM  0x20000 (ITS; XIP alias 0x130AF000)
0x0E4000 backup 0x8000   0x0EC000 media 0x3000   0x0EF000 factory 0x1000
0x0F0000 unused 0x10000
```

`apps/security/ipro7_psa_spe/partition_cfg_2M_psa_ota.toml` (2 MB,
not run): FW-A 0x14000 1 MB, FW-B 0x114000 768 KB, PSM 0x1D4000
128 KB (XIP alias 0x131BF000), backup 0x1F4000, media 0x1FC000,
factory 0x1FF000.

iprocube `partition_cfg_1M_boot2.toml` (1 MB, no OTA): FW-A 0x14000
792 KB, PSM 0xDA000 128 KB (XIP alias 0x130C5000); the psa-arch-tests
images, whose NSPE exceeds 124 KB, run on it.

`tools/psa/psa_ota_pkg.sh` builds the OTA package and the pending table
for a packed image; `apps/security/ipro7_psa_spe/README.md` has the
install steps and the boot2 console lines.

The PSA product boot2 is `ipro_boot2` `defconfig_ipro7_psa` built by
`tools/psa/psa_build_boot2.sh`: no USB CDC download window, and an
installed package advances the anti-rollback counter to its SVN
(`BOOT2_SVN_AUTOADVANCE=on`). `apps/security/ipro7_psa_spe/README.md`,
PSA boot2, has the reasons and the limit (the counter moves at
install time, so a new image that fails later cannot be rolled back).

## Directory map

| Path | Content |
|---|---|
| `include/psa_ipro/` | SPE/NSPE contract: `memmap.h`, `abi.h` (ecall ids, handles, control block, frame, image header), `ecall.h`, RPC layouts `crypto_rpc.h`, `its_rpc.h`, `attest_rpc.h`, `platform_rpc.h`, `platform_api.h`, `fwu_rpc.h`, `fwu_api.h` |
| `include/psa/` | PSA client API headers: `client.h`, `error.h`, `initial_attestation.h`, `internal_trusted_storage.h` |
| `spm/` | M-mode side: `spm_entry.S`, `spm_trap.c`, `spm_nspe.c`, `spm_pmp.c` (PMP and sPMP), `spm_periph.c` (NSPE peripheral windows, IRQs and register grants), `spm_ldst.h` (word load/store decoder of the grants), `spm_tzc.c` (TZC_SEC bus-master isolation), `spm_harden.c` (launch hardening, PSM layout check), `spm_virq.c`, `spm_range.c`, `spm_copy.c`, `spm_heap.c`, services `svc_crypto*.c`, `svc_its.c`, `svc_attest.c`, `svc_platform.c`, `svc_fwu.c`, `huk.c`, `kdf.c`, `its_store.c`, `its_lfs.c`, `its_mbedtls_adapter.c`, `eat.c`; HUK provisioning image `huk_prov.c`, `huk_prov_hw.c` |
| `client/` | NSPE side: `psa_client.c` (framework calls), `psa_crypto_client*.c`, `psa_its_client.c`, `psa_attest_client.c`, `psa_platform_client.c`, `psa_fwu_client.c` |
| `tests/host/` | host unit tests of the pure-C parts (`make test`, `make test MBEDTLS_V=3`) |

Apps: `apps/security/ipro7_pmp_probe` (DUT probe of PMP and U-mode
facts), `apps/security/ipro7_psa_spe` (SPE image),
`apps/security/ipro7_psa_demo` (NSPE image, shell, client library,
isolation probes). Tools, in `tools/psa/`: `tools/psa/psa_pack.py`,
`tools/psa/psa_nspe_check.py`, `tools/psa/psa_regress.sh`,
`tools/psa/psa_arch_run.py` (one psa-arch-tests run on the DUT),
`tools/psa/psa_ota_pkg.sh` (OTA package),
`tools/psa/psa_sign_images.sh` (signed boot2, FW and OTA images),
`tools/psa/psa_build_boot2.sh` (PSA boot2),
`tools/psa/psa_fwu_push.py` (OTA package to the NSPE over the console),
`tools/psa/psa_huk_prov.py` (host side of the HUK provisioning image).

## ecall ABI

`a7` = function id, `a0..a5` = arguments, `a0` = result
(`include/psa_ipro/abi.h`).

| a7 | Name | Purpose |
|---|---|---|
| 0x00 | `PSA_ECALL_FRAMEWORK_VERSION` | PSA framework version (0x0101) |
| 0x01 | `PSA_ECALL_VERSION` | service version by handle, 0 when unknown |
| 0x02 | `PSA_ECALL_CALL` | `psa_call`: up to 4 in and 4 out vectors |
| 0x03 | `PSA_ECALL_CONNECT` | `psa_connect`: `PSA_ERROR_NOT_SUPPORTED` |
| 0x04 | `PSA_ECALL_CLOSE` | `psa_close`: `PSA_ERROR_NOT_SUPPORTED` |
| 0x10 | `NSPE_ECALL_INIT` | register trap entry and control block |
| 0x11 | `NSPE_ECALL_IRQ_RETURN` | resume a saved frame, clears `virq_mask` |
| 0x12 | `NSPE_ECALL_IRQ_DELIVER` | deliver a pending vIRQ after unmask |
| 0x13 | `NSPE_ECALL_IRQ_ATTACH` | attach a peripheral IRQ of `spm_periph.c` (UART0; DMA0, USB with `CONFIG_PSA_NSPE_DMA_USB`) |
| 0x14 | `NSPE_ECALL_IRQ_DETACH` | detach it |
| 0x15 | `NSPE_ECALL_TIMER_SET` | periodic `VIRQ_TICK` |
| 0x16 | `NSPE_ECALL_YIELD` | yield |
| 0x17 | `NSPE_ECALL_WFI` | idle until a vIRQ |
| 0x18 | `NSPE_ECALL_RESET` | relaunch the NSPE |
| 0x20 | `NSPE_ECALL_LOG` | SPE console line for the NSPE |

Service handles (FF-M 1.1 stateless):

| Handle | Service | RPC layout |
|---|---|---|
| 0x40000100 | PSA Crypto | `crypto_rpc.h` |
| 0x40000101 | Internal Trusted Storage | `its_rpc.h` |
| 0x40000102 | Initial Attestation | `attest_rpc.h` |
| 0x40000103 | Platform (uid, lifecycle, IAK public key, NSPE measurement) | `platform_rpc.h` |
| 0x40000104 | Firmware update staging (OTA package to FW-B, pending table) | `fwu_rpc.h` |

Virtual IRQ ids: `VIRQ_NOTIFY` 1, `VIRQ_FAULT` 2, `VIRQ_YIELD` 3,
`VIRQ_TICK` 7; peripheral vIRQs use the ECLIC ids (bitmap 0..95). The
NSPE image header (`struct psa_nspe_header`, magic `NSPE` =
0x4550534E, 32 bytes) sits at 0x13040000.

## Firmware update staging (`svc_fwu.c`)

The NSPE stages an OTA package for `ipro_boot2` without a flash
window: only the SPE writes FW-B and the partition tables. FW-B, the
table addresses and the active table are read from flash as boot2
reads them (bootheader words 164 and 168, both CRC32s, higher age,
PT0 on a tie); FW-B is `Address[1]` / `maxLen[1]` of the FW entry
(type 0) and must be sector aligned and apart from FW-A and the other
table, else `PSA_ERROR_NOT_SUPPORTED`.

| fn | Vectors | Rule |
|---|---|---|
| `FWU_FN_WRITE` 1 | in [`struct fwu_req` {offset, len}, data] | 1..2048 bytes inside FW-B; offset 0 starts a package, any other offset must equal the bytes written so far (`BAD_STATE`); each 4 KB sector that starts in the chunk is erased first; the bytes are read back (`STORAGE_FAILURE`, staging restarts at 0) |
| `FWU_FN_INSTALL` 2 | in [`struct fwu_req` {0, len}] | len = bytes written; header as boot2 checks it before the signature: magic `IPRO_OTA_Ver1.0`, type `XZ  `, payload length != 0, payload + 512 = len <= FW-B, else `PSA_ERROR_DATA_INVALID`; then the active table with FW activeindex 1, FW age + 1 and table age + 1 goes to the other copy, which is read back as the active one |
| `FWU_FN_QUERY` 3 | out [`struct fwu_status`, 40 B] | state (idle, writing, pending), FW activeindex, active table and age, bytes staged, FW-B size, header SVN in FW-B, eFuse anti-rollback counter, boot2's OTA result of this boot and, after an install, the running SVN |

While FW activeindex is 1, WRITE and INSTALL return `BAD_STATE`
until boot2 has run. The signature, payload SHA-256 and SVN stay
boot2's checks. At init the SPE takes boot2's OTA result word (HBN
RAM 0x20090618, magic `OR`) and clears it; after an install
(result 0) the header SVN in FW-B is the running image's SVN. A
client reset drops the staged count. The NSPE side is the `fwu`
shell command of `apps/security/ipro7_psa_demo` and
`tools/psa/psa_fwu_push.py`.

## Build

```
./build_freertos.sh -d defconfig_ipro7 security/ipro7_pmp_probe
./build_freertos.sh -d defconfig_ipro7 security/ipro7_psa_spe
./build_freertos.sh -d defconfig_ipro7 security/ipro7_psa_demo
python3 tools/psa/psa_pack.py \
    --spe apps/security/ipro7_psa_spe/build_ipro7/build_out/ipro7_psa_spe_IPRO7.bin \
    --nspe apps/security/ipro7_psa_demo/build_ipro7/build_out/ipro7_psa_demo_IPRO7.bin \
    --out apps/security/ipro7_psa_spe/build_ipro7/ipro7_psa_fw.bin
```

The NSPE build fails (and deletes its outputs) when `psa_nspe_check.py`
finds an M-mode CSR access, `mret`/`sret`/`wfi`/`fence.i` or an SPE-only
BSP object in the link; an empty map file, or a disassembly with no
instructions, ends the check with exit 2. `psa_pack.py` applies the
checks of `spm_header_ok` (`spm/spm_nspe.c`) to the NSPE header: magic,
`hdr_size` 32, `image_size` equal to the file size and at most
0x40000, entry at least 32 bytes into the image and inside it, stack
top 16-byte aligned, above 0x11020000 and at most 0x11040000. It pads
the SPE with 0xFF to 0x40000 (`--spe-size` takes only that value, any
other exits 2) and prints size and SHA-256 of both parts and of the
output.

## Tests

Host unit tests (SPM range checks, vIRQ state, PMP/sPMP and peripheral
tables, iovec bounce, SPE stack scan, crypto and ITS services, KDF,
ITS store, EAT, attestation, firmware-update staging, HUK
provisioning, client marshalling, NSPE trap and vIRQ logic):

```
make -C components/psa/tests/host test
```

Tool tests:

```
python3 -m unittest discover -s tools/psa/tests -p 'test_psa_*.py'
python3 tools/tests/test_zmodem.py
```

Regression script (run from the worktree root; clean builds, a few
minutes per image):

```
IPROCUBE_DIR=<iprocube checkout> tools/psa/psa_regress.sh
```

It builds the three PSA apps, runs both test sets, packs the image to
`apps/security/ipro7_psa_spe/build_ipro7/ipro7_psa_fw.bin`, rebuilds the
shared-code images listed in `tools/psa/psa_regress_baseline.txt` and
compares their timestamp-masked SHA-256 (`tools/psa/psa_regress_mask.py`),
and runs the `ipro_boot2` host tests `verify_secure_chain.py` and
`verify_ota_chain.py` against `IPROCUBE_DIR` (a checkout with `libs/`).
Step 1 also builds the SPE HUK provisioning image
(`defconfig_ipro7_hukprov`), the development-HUK SPE
(`defconfig_ipro7_devhuk`) and the `security/ipro7_psa_demo` psa-arch-tests
defconfigs (`defconfig_arch_crypto`, `_its`, `_attest`), step 2 also
runs the psa-arch-tests port host test
(`components/3rdparty/psa_arch_tests/port/tests`), and step 4 packs
the demo with the development-HUK SPE
(`build_ipro7_devhuk/ipro7_psa_fw.bin`) and each psa-arch-tests image
with the SPE to check the 0x40000 NSPE limit.
It prints one PASS/FAIL line per step and image and exits
1 on any FAIL. An image recorded as `PREEXISTING_FAILURE:<text>` must
still fail to build with `<text>` in its log. `PSA_REGRESS_CLEAN=0`
builds incrementally. `PSA_REGRESS_STEPS="2 3"` runs only the listed
steps (1 PSA builds, 2 host tests, 3 tools tests, 4 pack, 5 image
compare, 6 boot2 tests); the last line then names the steps not run,
e.g. `psa_regress: PASS (steps not run: 1 4 5 6)`. Step 4 needs step 1
in the same run; step 6 FAILs when `IPROCUBE_DIR` is unset unless
`PSA_REGRESS_STEPS` leaves it out. `PSA_REGRESS_PYTHON` selects the
interpreter for step 6 (it needs `ecdsa`; `HOME` is redirected there).

The baseline digests hold for the checkout path and toolchain named in
the header of `tools/psa/psa_regress_baseline.txt`: images embed `__FILE__`
and assert strings, so a run from another path or toolchain reports
FAIL for every image by design. To re-baseline, revert the five shared
files listed there to 9bf59a9d3, run `PSA_REGRESS_STEPS=5`, copy the
`got` digests into the baseline, restore the files and re-run.

## Status

### SEC_ENG hardware-key HUK on part #2 (2026-10-19, 2436eb9ce)

Second IPRO7 dev part in the evb7 socket: PK hash, `sign_mode` 1,
AES-128 XIP encryption, HUK in slot 4 and flash key in slot 2, both
read- and write-locked, `keep_dbg_port_closed` set. Signed + encrypted
images of 2436eb9ce (`CONFIG_PSA_HUK_IN_RAM=n`, `KEY_SEL` 1), vault
`psa_evidence/2026-10-19-part2-engine/`: `[SPE] HUK via SEC_ENG
KEY_SEL 1` on every boot, one ITS reformat (the part ran on the
development HUK before), then 20/20 reset boots with `[SB] ok`,
11/11 commands, `lifecycle 0x1000`, probes 5/5, `probe addr` 32/32
(TZC_SEC read denied too), `probe ecall: 15/15`, tokens verified with
the IAK; psa-arch-tests (`1M_boot2` table) ITS 10/10, attestation
1/1, crypto 62/50/0/0/12, each on reset and on re-run. The IAK public
key equals the host derivation from the recorded HUK, so the engine
path keeps the RAM-path keys. JTAG was not tried (no probe on evb7).

### Slot-4 read lock (2026-10-16 18:42)

At 18:42 a user-approved burn set eFuse word 0x7C bit 30 (read lock
of key slot 4) on the evb7 part: lock word 0x00180000 -> 0x40180000,
and the loader then reads slot 4 as zero. Experiment, vault
`psa_evidence/2026-10-16-l2run-fd894ca-rdlock/` (`ipro7_pmp_probe`
c0ca0173a and ec6c3b3c5): every SEC_ENG hardware-key run, `KEY_SEL`
0-3 x `SE_AES_SBOOT` 0/1 x 128- and 256-bit key with
`tzc_sboot_done` 0xf, encrypts under the all-zero key; none gives
AES of the zero block under the burned HUK (any byte order, with or
without slot 5) or under the non-zero, unlocked slot 3. Outcome: the
engine route is not live, so this part has no usable read-protected
HUK and `CONFIG_PSA_HUK_IN_RAM=n` is no option on it. Since 18:42 the
SPE logs `[SPE] HUK not provisioned` and `[SPE] development HUK in use`
and runs on the public development key: the first boot reformatted
ITS (`[SPE] ITS key changed, reformatting`), objects written before
are gone, the IAK changed and the lifecycle claim reads 0x0000. The
wave-1 demo image runs on it (ITS set/get, `psa_attest` OK, the NSPE
eFuse probe faults with mcause 13 / mdcause 6). Closing the HUK item
needs the chip team (which enable routes key slots to SEC_ENG) or a
HUK in a slot 6-11 on a fresh part.

### Strict-review wave 1 (344ab6c1c)

2026-10-16 18:06-18:17, same board and eFuse state, signed images of
344ab6c1c (IAK on deterministic ECDSA, `probe addr` / `probe ecall`),
vault `psa_evidence/2026-10-16-l2run-fd894ca-wave1/`: 20/20 reset
boots with `[SB] ok`, 11/11 commands, probes 5/5; `probe addr`: 29
accesses to SPE flash, SPE RAM, the boot2 hand-off page, their
0x2xxxxxxx aliases, ECLIC, SysTimer, DMA, SF_CTRL, GLB, eFuse and
SEC_ENG denied (mcause 12/13/15, mdcause 6), 3 NSPE controls allowed;
`probe ecall: 15/15`, and `frame` / `sp` reset the NSPE; two
`psa_attest` tokens for one nonce are equal byte for byte and verify
with the `psa_iak` key; psa-arch-tests attestation 1/1 twice. With
counter 1 an SVN 2 package of an older image installs after SVN 3
(`[OTA] sig ok, svn 2`). One flipped byte in the FW-A image body gives
`[SB] E15`; one flipped byte in an OTA payload gives `[OTA] sha err`
and the running image boots on.

### Signed evidence run fd894ca

2026-10-16, head fd894ca (the code of b61622d56 plus
`tools/psa/psa_sign_images.sh`), the board below after its eFuse burn:
`sign_mode` 1, PK hash of the development signing key in key slots
0+1, anti-rollback counter 1, development HUK in slot 4 (not locked),
PSA lifecycle 0x1000 (eFuse lifecycle field 0). eFuse values are from
the 15:09 burn readback (vault
`psa_evidence/svn_packet/20261016-150934-burn-sboot_svn1-after.masked.bin`,
key slot 4 zeroed; the unmasked file holds the development HUK);
this run did not read eFuse. Every image was clean-built at that head
and signed with `tools/psa/psa_sign_images.sh`
(`apps/security/ipro7_psa_spe/README.md`, Signed images); boot2
`defconfig_ipro7` has the baseline masked digest. Logs, the SHA-256
of every flashed image and the procedure: vault
`psa_evidence/2026-10-16-l2run-fd894ca/` (`README.md`,
`images.sha256`, `SHA256SUMS`).

- 1 MB OTA table, demo image: 20/20 reset boots with `[SB] ok` and
  the shell; `psa_ver`, `psa_uid`, `psa_rand 16`, `psa_hash abc`,
  `psa_aes`, `psa_sign`, `psa_its_set 4006 abc`, `psa_its_get 4006`,
  `psa_attest` (462-byte token), `psa_iak`, `psa_stats` OK; probes
  5/5 (`spe_ram` mcause 13 mdcause 6, `efuse` 13 / 6, `sec_eng`
  15 / 6, `csr` 2, `badptr` returned -135), shell alive.
- Each boot prints `Warning: boot2 parameters not available`,
  `[SPE] boot2 params absent, PT1 read from flash` and
  `[SPE] stack min free 6124 B of 8192`.
- OTA with counter 1: the signed SVN 0 package is refused
  (`[OTA] svn 0 < 1`, `[OTA] E6`, `[OTA] rejected`,
  `[BOOT2] OTA FAIL (-2)`, `[OTA] pending cleared`) and the running
  image boots on; the SVN 3 package installs (`[OTA] sig ok, svn 3`,
  `[OTA] OK`), the new image boots, the next reset runs no OTA, and
  the ITS object is still there.
- psa-arch-tests on the `1M_boot2` table, from reset and again with
  `psa_arch_test` in the same boot, same per-test results: crypto 62
  tests, 50 PASSED, 12 SKIPPED, 0 FAILED, 0 SIM ERROR; ITS 10/10;
  attestation 1/1.
- Secure boot negative: the demo FW image without a signature gives
  `[SB] E2`, signed with another key `[SB] E9`, each followed by
  `[BOOT2] FW rejected by secure boot` and no SPE start; the signed
  image written back boots with `[SB] ok`.

### Earlier runs

Silicon runs on 2026-10-16 with commit d3ae1c405, one board: QFN32
IPRO7 dev part, blank eFuse (no secure boot), 1 MB SF1 flash, PSRAM,
partition table `1M_boot2` (FW 0x14000, PSM 0xDA000 128 KB),
development HUK (`CONFIG_PSA_DEV_HUK=y`), lifecycle 0.

- `ipro7_pmp_probe`: exit criteria PASS 21/21 (PMP 8 x 1 KB; sPMP,
  the Ssmpu variant, 8 x 1 KB via `smpuswitch0` = CSR 0x170;
  `mseccfg` 0; TEE = 1).
- Packed SPE||NSPE (`ipro7_psa_fw.bin`): 20/20 reset boots (not
  power cycles) reach the U-mode shell, about 1.4 s each.
- Shell: `psa_ver`, `psa_uid`, `psa_rand`, `psa_hash` (SHA-256 of
  "abc" correct), `psa_aes`, `psa_sign`, `psa_its_set` / `get` / `rm`,
  `psa_iak`, `psa_attest` all OK. The token was 460 bytes: with the
  development HUK the lifecycle claim is 0x0000, one CBOR byte; the
  later runs, with lifecycle 0x1000, give 462 bytes.
- Fault probes 5/5: `spe_ram` mcause 13 mdcause 6, `efuse` 13 / 6,
  `sec_eng` 15 / 6, `csr` 2, `badptr` returned -135; the shell stays
  alive afterwards. `psa_stats`: `stat_emul` 666 (emulated user
  counter reads).
- psa-arch-tests v25.08 suites, commit ce689ad3c (`fix: final-review
  wave`, the code of the 2026-10-16 HEAD; the commit on top changes
  only this Status), same board with the development HUK burned in
  eFuse slot 4 (not locked, lifecycle 0x1000), 1 MB OTA table, one
  image per suite (`tools/psa/psa_arch_run.py`; logs in the vault,
  `psa_evidence/2026-10-16-psa-arch-<suite>.log` and
  `-<suite>-rerun.log`). Each suite ran twice in one boot, from reset
  and again after `psa_arch_test`, with the same per-test results:
  - Crypto, 1.0.0 list with c019 skipped: 62 tests, 50 PASSED,
    12 SKIPPED, 0 FAILED, 0 SIM ERROR. Skipped: c039, c040 (no RSA
    encryption), c054-c063 (no multipart AEAD).
  - Internal Trusted Storage: s001-s010, 10 PASSED, 0 FAILED.
  - Initial Attestation, profile 1: a001 PASSED (16 checks; the
    token verifies with the IAK public key the SPE reports at run
    time, a self-consistency check).
  The runner task kept at least 13352 B of its 16 KB stack free.
  The crypto image (439472 B with its bootheader) is larger than
  FW-A of that table (393216 B), so iprocube refuses the flash
  download; it was written by an iprocube single download at
  0x14000 and runs on into FW-B, through the same boot2 chain.
  The runs of commit 6bfe0c764 on the `1M_boot2` table gave the same
  totals.
- M2 set on ce689ad3c with the plain demo image, 1 MB OTA table:
  20/20 reset boots to the shell; `psa_ver`, `psa_uid`, `psa_rand`,
  `psa_hash abc`, `psa_aes`, `psa_sign`, `psa_its_set` / `get` /
  `rm`, `psa_attest` (462-byte token) OK; probes 5/5 with the causes
  above.
- OTA on the 1 MB table, images of 530d22158 (counter 0, `sign_mode`
  0): an SVN 1 package installs and boots, no second OTA on the next
  boot; an SVN 0 package of an older image installs as well, because
  boot2 checks the SVN only with `sign_mode` set
  (`apps/security/ipro7_psa_spe/README.md`, Partition tables and OTA).

## Known limitations / residual risks

- With `CONFIG_PSA_TZC_CCI_ID1` CCI cannot even set its pinmux (GLB
  is ID0 only): no CCI access to the part while the SPE runs. The
  MM2OCRAM region is set like OCRAM but not checked with an MM
  master (the MM register slave is ID0 only).
- Bus-master denial is measured with DMA0 only: USB, SDHA, EMAC and
  M154 are ID1 by TZMID but not driven as masters in a test. DMA
  writes are checked into SPE RAM, TZC, HBN RAM, SEC_ENG, SF_CTRL and
  DMA registers, not into eFuse, GLB, PDS or HBN registers (no write
  there from a test).
- ID1 reads SE_CTRL_PROT_RD and writes the SE_CTRL reserved words
  0x30080F04 and 0x30080F08 (no TZC field). MAC154 0x3C000000 has no
  slave filter; its master is ID1 too.
- `CONFIG_PSA_NSPE_DMA_USB`: the NSPE sets the USB transceiver
  analog bits of usb_xcvr / usb_xcvr_config and the pull, drive and
  OE of pins 8 and 9.

- Anti-rollback holds only on a part with `sign_mode` set and a PK
  hash in eFuse: without them boot2 installs a package of any SVN.
  The stock boot2 never advances the counter (eFuse word 0x4C); the
  PSA boot2 (`defconfig_ipro7_psa`) advances it to the SVN of each
  package it installs. The counter is checked only on the OTA path,
  not at boot.
- ITS has no rollback or replay protection: an older copy of an
  object file, or of the whole `PSM`, passes its GCM check. There is
  no per-namespace quota; NSPE objects and persistent keys share the
  128 KB `PSM`: 24 objects of 2048 B, or 278 of 3 B (objects up to 256 B
  are inline in the directory). A write of more than 256 B that would
  leave fewer than 4 free blocks returns
  `PSA_ERROR_INSUFFICIENT_STORAGE` (`its_lfs_room`), so a full store
  still mounts, rewrites `/its/meta` and removes objects
  (`tests/host/test_its_lfs.c`, `test_full`).
- A replayed older `/its/meta` with a valid tag rolls the boot count
  back and keeps the objects. `its_ctr` restarts at 1 on each boot,
  so the nonce of a new object then differs from one of the earlier
  boot with that count only in the 4-byte `boot_rand`; equal values
  are a GCM nonce reuse under the ITS key. The bullet above covers
  the integrity side only.
- `/its/meta` missing or empty while `/its` holds any other file is
  taken as tampering: the SPE logs
  `[SPE] ITS meta missing with objects, reformatting` and formats the
  store, so every object is lost; with no other file it is a first
  mount (boot count 1). A meta file that fails its tag logs
  `[SPE] ITS key changed, reformatting` and formats too. Whoever can
  write the `PSM` can thus empty the store.
- littlefs formats the `PSM` when it cannot mount it, and the SPE
  formats a `PSM` it can mount but not read as its ITS (stale data of
  another partition table, a foreign littlefs or one of other
  geometry: `[SPE] ITS store unreadable[ (lfs <rc>)], reformatting`).
  Both empty the store (availability); a flash I/O error or a failed
  format leaves ITS down (`PSA_ERROR_STORAGE_FAILURE`).
- `CONFIG_PSA_DEV_HUK=y` is the default: with a blank HUK (blank slot
  4 or zero hardware key; not a locked or failed engine) the HUK is a
  public development key, so ITS confidentiality and the IAK private
  key are public and the lifecycle reads 0x0000
  (`PSA_LIFECYCLE_UNKNOWN`). In Status the d3ae1c405 run used it, the
  runs from e6da9883e to wave 1 used the HUK burned in slot 4
  (lifecycle 0x1000), and the first part used it again after the
  slot-4 read lock of 2026-10-16 18:42.
- HUK engine path (`CONFIG_PSA_HUK_IN_RAM=n`, SPE default since
  2026-10-19): it needs eFuse `keep_dbg_port_closed`, so the BROM
  leaves `tzc_sboot_done` at 0. The SEC_ENG hardware key (`KEY_SEL` 1
  = slot 4) is live for M-mode code from reset until `huk_lock` at
  the end of SPE init (`CONFIG_PSA_HUK_LOCK`), which sets the field to
  0xF and proves the key is then the zero key; the NSPE never runs
  before it and has no SEC_ENG access (PMP). Key hierarchy, lock
  sequence and the production eFuse requirements
  (`keep_dbg_port_closed` and the separate JTAG-disable bit) are in
  `apps/security/ipro7_psa_spe/README.md`, HUK, KDF and ITS. boot2
  and the SPE before the lock run with the key live and are trusted
  (signed chain). On a part without `keep_dbg_port_closed` the BROM
  sets the field: the HUK is absent (no development HUK), ITS and
  attestation are disabled. Open items (same README): JTAG after the
  lock (design input: JTAG follows `ef_dbg_mode` /
  `ef_dbg_jtag_*_dis`; the BROM comment `ipro_bootrom_exit.c:57-58`
  ties JTAG open to the field; not measured, evb7 has no JTAG probe),
  clearing the field from M-mode, a software or WDT reset restoring
  it to 0 before the OTA reboot, and the bootrom_protect / `R0_LOCK`
  fields of the same register.
- `apps/security/ipro7_pmp_probe` reaches the HUK through SEC_ENG on
  a `keep_dbg_port_closed` part (M-mode): never sign it with a
  production key.
- The SPE 8 KB stack (also the trap stack) is measured only at an
  NSPE launch: `main` first fills its free part with 0xA5A5A5A5
  (`spm_stack_fill`, `spm/spm_entry.S`), and every launch logs
  `[SPE] stack min free <n> B of 8192`, the bytes from the stack base
  up to the first changed word (`spm_stack_min_free`). At boot that
  covers `main` up to the first NSPE entry (service init included, no
  trap or service call); the line after an NSPE reset covers
  everything since `main`. Startup code before `main` is not covered,
  and a used word that equals the pattern counts as free. The NSPE
  4 KB boot / vIRQ trap stack is not measured.
- boot2 hand-off: the SPE takes the partition table from the boot2
  param block (`Active Partition[0], entryCnt 7`, the copy boot2
  used). With an `ipro_boot2` that passes the wrong entries CRC
  (before 78447d48c) `Boot2_Param_Get()` returns NULL: the BSP
  `hal_boot2_init` prints `Warning: boot2 parameters not available`
  and loads the flash copy of higher age, PT1 on a tie, and the SPE
  logs `[SPE] boot2 params absent, PT1 read from flash`.
- UART0 is shared: the SPE log and the NSPE console write the same
  port, and the NSPE, which owns the UART0 registers, can change its
  settings or print lines that look like `[SPE]` lines.
- The PSA defconfigs set `CONFIG_MBEDTLS_V3`: the SPE runs mbedtls 3.6
  (`components/crypto/mbedtls3`), which serves multipart AEAD,
  `OTHER_SECRET`, RSA (PKCS#1 v1.5, OAEP, PSS) and PBKDF2-HMAC
  (`KD_INPUT_INTEGER`, id 64).
  `_tfpsa` defconfigs (`CONFIG_TF_PSA_CRYPTO`) build on TF-PSA-Crypto
  1.2.0 (`components/crypto/tfpsa`): also `psa_key_agreement` (id 65)
  and J-PAKE (ids 66-74, one PAKE operation per slot). Library
  differences reach the code only through `psa_ipro/crypto_lib.h`.
  The `_m2` defconfigs build on mbedtls 2.28.2, which does not serve
  multipart AEAD (one-shot only; the client returns
  `PSA_ERROR_NOT_SUPPORTED`), RSA, `OTHER_SECRET` or PBKDF2.
  `components/3rdparty/psa_arch_tests/README.md` lists every flag. Its algorithm set is the
  PSA_WANT_* list `psa_ipro/crypto_features3.h`
  (`MBEDTLS_PSA_CRYPTO_CONFIG`), shared by the SPE and client configs.
- Key attributes cross the RPC as `struct psa_crypto_attr` (public
  fields only, 28 bytes), packed and unpacked through the PSA setters
  and getters, so SPE and NSPE builds of 2.28 and 3.6 share the layout;
  the SPE unpack clears an id given with a volatile lifetime.
- Output capacities: `psa_client_cap` (`client/psa_client_priv.h`)
  clamps a declared crypto or attestation output capacity to the
  4096-byte bounce limit, and only `psa_its_get` refuses a buffer
  span past the address space top. An oversized declared output
  buffer therefore succeeds where TF-M refuses it. The SPM
  range-checks every byte it writes: an API-conformance edge, not an
  isolation gap.
- On the 1 MB OTA table (`partition_cfg_1M_psa_ota.toml`) the NSPE
  flash PMP entry (R X, XIP 0x13040000 + 256 KB) maps flash up to
  0x94FFF, that is the first 132 KB of FW-B: the NSPE can read the
  staged OTA package, which is plaintext.
- Firmware update staging: any NSPE code can stage a package and set
  it pending. The SPE checks the header format only; boot2 refuses a
  package whose signature, payload SHA-256 or SVN fails and clears
  the pending flag, so a bad package costs one boot. A validly signed
  package of any SVN at or above the eFuse counter installs, an older
  one included, unless the PSA boot2 has already advanced the
  counter past it. The
  running SVN is known only on the boot after an install. An AES-128
  encrypted image hardly compresses: its package nearly fills the
  320 KB FW-B of the 1 MB OTA table.
- U-mode cache maintenance is off: the SPE writes `ccm_suen` = 0, so
  every U-mode CCM operation (CSRs 0x4CB-0x4CF) raises an illegal
  instruction. No NSPE driver needs one: with
  `CONFIG_PSA_NSPE_DMA_USB` the NSPE USB runs on the FIFO path, the
  check allows the CCM CSRs only in the unused DMA-RX branch of
  `USBD_IRQHandler` and rejects them elsewhere.
- HUK derivation (`spm/kdf.c`): block i = AES-128-ECB(HUK, label
  zero-padded to 12 bytes || i, 32-bit big-endian). This is a label
  KDF over a one-block AES PRF, not an SP 800-108 KDF (no HMAC, CMAC
  or KMAC PRF, no [L] or context field). An SP 800-108 CMAC KDF with a
  known-answer test would re-key every device: the ITS objects are
  lost and the IAK and instance id change.
- The IAK signs with deterministic ECDSA (RFC 6979), so its
  signatures do not depend on the TRNG. Everything else random does:
  CTR_DRBG is seeded by `mbedtls_hardware_poll`
  (`components/crypto/mbedtls/port/entropy_poll_alt.c`), which never
  reports a failure; no code reads the TRNG health-test status.
  Generated keys, the boot seed and the ITS nonce bytes depend on it.
- Attestation: the token measures the SPE (by itself) and the NSPE;
  boot2 is not measured. The implementation id is SHA-256 of the
  boot2 PK hash, so it names the signer, not the RoT build. The
  lifecycle claim comes from `ef_sec_lifecycle` only and is not tied
  to `dbg_mode` or the JTAG disable bits.
- Launch hardening (`spm/spm_harden.c`, every NSPE launch, after the
  PMP): `mideleg` = 0 (it reads 0 on evb7 whatever is written),
  `stvec` = an SPE address (no PMP entry, so an S-mode fetch there
  faults), every ECLIC source set to M-mode, and a fatal stop when an
  enabled source is vectored (its handler would run on the NSPE `sp`).
  Log: `[SPE] harden: mideleg 0 stvec <a>, <n> IRQs, <k> on, <m> set
  to M`.
- Not done: `mseccfg` MML/MMWP stay 0, so M-mode is not restricted by
  PMP; the core implements `mseccfg` (RLB writable on evb7, the
  `ipro7_pmp_probe` `mseccfg_rlb` line). The partition table read from
  flash is CRC-protected only; the PSM layout check above limits what
  a forged table can point ITS at.
- Side channels were not evaluated. The surface: table-based software
  AES (`MBEDTLS_AES_ROM_TABLES`) for the HUK PRF, ITS and NSPE keys;
  the SPE emulates `cycle`/`mcycle` reads, so the NSPE has a
  cycle-accurate counter; caches are shared by M and U mode; the NSPE
  can trigger ITS AES-GCM and every crypto key operation at will.

## Silicon notes

QFN32 IPRO7 dev part, `ipro7_pmp_probe` runs 1-3 (2026-10-16); the
details are in `apps/security/ipro7_psa_spe/README.md`.

- `mcfg_info` 0x0001b685: TEE = 1. PMP 8 entries, grain 1 KB, NAPOT.
- The sPMP is Ssmpu: `spmpcfg0..3` 0x1A0-0x1A3 (A | X | W | R, no
  U/S/L bit), `spmpaddr0..7` 0x1B0-0x1B7 (grain 1 KB), switch
  `smpuswitch0` 0x170. Entry i is active only while
  `smpuswitch0[i]` = 1 and cfg.A != 0; with no active entry every
  U-mode access faults, so the SPE mirrors its PMP table into sPMP
  0..5 and writes `smpuswitch0 = 0x3F` when TEE is set.
- U access denied by both units or matching neither: sPMP page fault,
  mcause 12 / 13 / 15, mdcause 6; allowed by sPMP only: PMP access
  fault 1 / 5 / 7. `spm_trap.c` delivers both kinds as `VIRQ_FAULT`.
- U `ecall` -> 8, U `csrr mcycle` -> 2, U `wfi` does not trap,
  `mscratchcsw` swaps only on traps from U.
- U counter reads: the SPE writes `mcounteren` = 7 and never
  `scounteren` (0x106), so U reads of `cycle`, `time` and `instret`
  (and the `h` halves) trap with mcause 2 (`rdcycleh` seen on silicon,
  `apps/security/ipro7_psa_spe/README.md`). No ecall is involved:
  `spm_trap.c` takes the trap on its illegal-instruction path, decodes
  the instruction at `mepc` (`spm_csr.h`), writes `mcycle(h)`, the
  SysTimer `mtime` (4 MHz) or `minstret(h)` into `rd`, steps `mepc`
  by 4 and counts `stat_emul`. The same path serves reads of
  `mcycle(h)`, `mhartid` and `mstatus` (MIE only) and the MIE toggle;
  any other illegal instruction, a counter write included, becomes
  `VIRQ_FAULT`.

## DUT procedure (M2, outline)

Requires an IPRO7 board that runs the secure boot2 chain, with a flash
big enough for the FW partition (SPE 256 KB + NSPE) and, for ITS, a
`PSM` partition of at least 128 KB. Partition table changes and any
eFuse write need the user's explicit yes.

1. Flash `ipro7_pmp_probe_IPRO7.bin` first on a fresh board as the FW
   image through iprocube, like any `ipro7_demo` image. Capture UART0
   at 115200 from reset. The run must end with
   `[PMP] exit_criteria: PASS ...`; see
   `apps/security/ipro7_pmp_probe/README.md`.
2. Flash `ipro7_psa_fw.bin` the same way. Console, in order:
   `[SPE] IPRO7 PSA-RoT v0.1 build ...`, HUK / ITS / attestation status
   lines (`[SPE] HUK via SEC_ENG KEY_SEL 1` and
   `[SPE] HUK locked (sboot_done 0xf)` with the HUK in slot 4 and
   `keep_dbg_port_closed` set; on a part without
   `keep_dbg_port_closed`
   `[SPE] HUK unavailable: sboot_done 0xf, ITS and attestation disabled`,
   no development HUK),
   `[SPE] NSPE v00010000 size <n> entry <pc> sha256 <hex>..`, then
   `[NSPE] ipro7_psa_demo, CPU <n> Hz` and the shell prompt.
3. Shell: `psa_ver`, `psa_hash abc`, `psa_aes`, `psa_sign`,
   `psa_its_set`/`psa_its_get`, `psa_attest`, then the isolation probes
   `probe spe_ram`, `probe efuse`, `probe sec_eng`, `probe csr`,
   `probe badptr`; each must print its documented pass line and leave
   the shell alive (`apps/security/ipro7_psa_demo/README.md`).

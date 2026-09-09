# NMSIS-NN

Nuclei's neural-network kernel library for RISC-V — the RISC-V mirror of ARM's
CMSIS-NN. int8 / int4 / int16 convolution, depthwise convolution, pooling,
fully-connected, softmax, SVDF and LSTM kernels.

| | |
|---|---|
| Upstream | https://github.com/Nuclei-Software/NMSIS |
| Commit | `dc728b988b6562868d311a2e14b5d9f8ebbd26b0` (2026-06-04) |
| Vendored | `NMSIS/NN/{Include,Source}` only |
| License | Apache-2.0 (see `LICENSE`) |

## DSP acceleration is automatic

Upstream `riscv_nn_math_types.h` keys off the compiler-predefined
`__riscv_dsp` / `__riscv_xxldsp{,n1x,n2x,n3x}`. The IPRO7 vendor toolchain
predefines all of them for `-march=rv32imafc_..._xxldspn2x`, so `RISCV_MATH_DSP`
and `NUCLEI_DSP_N1/N2` turn themselves on. **Do not pass `-DRISCV_MATH_DSP`.**

Verified on the built image: the P-extension instructions `kmada`,
`sunpkd820`, `maddr32` and `pkbb16` appear in the linked kernels.

## Why headers live under `Include/nmsis_nn/`

`components/ai` ships its own trimmed `riscv_nn_types.h` in which
`nmsis_nn_conv_params` carries an extra `filter_offset` field that upstream does
not have. `__component_add_include_dirs()` is `INTERFACE`-scoped on the shared
`__component_lib` target, so a flat `Include/` here would place two different
`riscv_nn_types.h` on one global search path — the winner decided by include
order, the loser silently reading struct fields at the wrong offsets.

Consumers therefore write:

```c
#include "nmsis_nn/riscv_nnfunctions.h"
```

A translation unit may include the `components/ai` headers *or* these, never
both. `apps/test/ipro_nmsis_nn_test` keeps its two benchmark kernels in
separate `.c` files for exactly this reason.

## Relationship to components/ai

They are separate worlds and are meant to stay that way:

| | `components/ai` | this component |
|---|---|---|
| data type | uint8 offset-binary (the NPU's native format) | int8 |
| kernels | `riscv_convolve_u8` — plain C, no DSP | `riscv_convolve_s8/s4/s16` — DSP |
| serves | `.blai` NPU path | TFLM |

Note that `components/ai` declares six kernels in
`iproai_riscv_nnfunctions.h` but only ever defined two of them
(`riscv_convolve_u8` and `riscv_convolve_s8_get_buffer_size`); its
`RISCV_MATH_DSP` guard has never been defined by any build, and
`forward_MAXPOOL_tflite_nmsis()` calls an undefined `riscv_max_pool_u8` that
survives only because `-gc-sections` drops the function. Treat that header as
historical, not as an interface to extend.

## NMSIS Core

Not vendored. `bsp/drivers/ipro7_soc/risc-v/Core/Include` already ships NMSIS
Core 1.0.0, and although upstream NN targets Core 1.6.0, both produce
byte-identical codegen for these kernels (verified: same `.text` size, same 138
DSP instructions in `riscv_nn_mat_mult_nt_t_s8`). A second copy would only
create version skew.

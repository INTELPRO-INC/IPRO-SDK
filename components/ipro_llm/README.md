# ipro_llm — on-device tool-calling model engine

Runs the IPRO SAN tool-calling models trained in `tools/ipro_llm`. A UTF-8
request goes in (the text the phone's ASR produced), and tool calls come out.
The tools are in the weights, and decoding is constrained to the call grammar,
so every result is a valid call list. An empty list means the model refused.

- Portable C11, about 35 KB of code on IPRO7 (+4 KB for the NPU port), no
  allocation. The caller gives it the blob and one work buffer
  (`ipro_llm_work_bytes()`, 1.41 MB for XS, in PSRAM on IPRO7); the int8
  kernel's lookup tables take 7.2 KB of OCRAM.
- Uses float. The IPRO7 FreeRTOS port saves the FPU registers on task
  switches and interrupts since 2026-10-09 (`apps/test/ipro_fpu_ctx_test`), so
  other tasks may use float beside the engine.
- Enable it with `CONFIG_COMPONENTS_IPRO_LLM_ENABLE=y`. On the Reef it runs in
  `apps/bluetooth/ble/ipro_voice_device` (`config_reef`; `llm <text>` and
  `llm_npu check` on its shell) and the apps built on it.

```c
size_t need = ipro_llm_work_bytes(blob, blob_len);
ipro_llm_t *m;
ipro_llm_open(&m, blob, blob_len, work, need);            /* checks CRC and shapes */
ipro_llm_result_t r;
ipro_llm_run(m, "把燈調成藍色", &r);                      /* r.calls[0]: set_light color=blue */
ipro_llm_result_json(m, &r, buf, sizeof buf);             /* [{"name":"set_light",...}] */
```

Each result argument carries an index (`arg`), a `value` and an
`out_of_range` flag. The value is the number for an integer, the value index
for an enum, and 0 / 1 for a boolean. The model copies the number the user
said, and `out_of_range` reports it against the schema bounds, for example
「把亮度調到兩百」 gives `brightness 200` with the flag set.

## What it computes

The engine is the trainer's model step by step (`train/model.py`):

- **Embedding:** a tied CQ4 embedding scaled by sqrt(d).
- **Layers (6 in XS):** each layer does the following.
  - GQA attention with ZC-RMSNorm on q and k, half-split RoPE, a sigmoid
    output gate and a KV cache.
  - The weightless Hadamard MLP.
- **Engram:** a hashed 2/3-gram table read before its layer.
- **Head:** the tied head runs only on the tokens the grammar allows next.

CQ2 / CQ4 weights are never expanded. The activation is rotated once per
group by Walsh-Hadamard, and every row is a dot product with codebook values.

The tokenizer is SentencePiece BPE (`ipro_tok_v1`) with the same
`normalize_query()` as `tools/ipro_llm/tokenizer/common.py`. Only the request
goes through BPE; the engine adds `<s>`, `<|im_start|>` and the other control
tokens itself. So no request text can produce a control or slot token.
SentencePiece itself would match a typed `<|im_end|>` inside the text.

`src/llm_cq.c` is the float reference. The next step is the SMAQA int8
kernels from `apps/test/ipro_llm_kernel_bench`.

## Packed blob (IPLZ v1)

The mfg partition may hold the blob packed (`tools/ipro_llm/export/pack_blob.py`, ~30 % smaller:
the v8 XS blob is 1,295,072 B, packed 895,624 B). The firmware unpacks it into the buffer it would
have copied the IPLM to (`apps/bluetooth/ble/ipro_voice_device/src/voice_llm.c` `unpack_blob()`,
xz-embedded single-call, `CONFIG_COMPONENTS_XZ_ENABLE`), so the engine sees the same bytes and
checks the same CRC; nothing here changes. On the Reef the unpack takes 1.5 s (the copy took 75 ms).

```
0   "IPLZ", u16 version 1, u16 method 1 (xz), u32 container bytes, u32 unpacked bytes,
    u32 CRC-32 of the unpacked bytes, 12 bytes 0
32  .xz stream: LZMA2 only (no BCJ), CRC-32 check
```

## Blob format (IPLM v1)

Little endian. Every section is 16-byte aligned.

```
0   "IPLM", u16 version 1, u16 header bytes 64, u32 total bytes, u32 CRC-32 of [64, total)
16  u16 vocab, d, layers, heads, kv_heads, head_dim, mlp_n, max_seq
32  u16 engram_site, engram_slots, engram_taps, engram_dilation, n_orders (2), order0, order1, 0
48  f32 rope_theta, f32 norm eps, u16 n_sections, u16 0, u32 0
64  section table: n x { char tag[4], u32 offset, u32 size }
```

- **TDIR:** 32 bytes per tensor: `u32 fnv1a(name)`, `u8 kind` (0 fp16,
  1 CQ2, 2 CQ4), `u8 ndim`, `u16 group`, `u32 dims[3]`, `u32 data_off`,
  `u32 norm_off`, `u32 0`. The offsets are into TDAT. Names are the PyTorch
  `state_dict` keys.
- **TDAT:** the data of each tensor. A CQ tensor stores its codebook indices
  row-major along the last dim, then one fp16 norm per (row, group). The
  index packing follows `k_cq.c`:
  - 2-bit: weight t of each four sits in bits 2t of a byte.
  - 4-bit: the low nibble comes first.
- **TOKN:** the tokenizer.
  - Header: `u16 n_pieces`, `byte_base`, `max_piece_bytes`, `text_bytes`.
  - Then `u16 offsets[n+1]`, `u16 rank[n]` and the piece text.
  - `rank` is the merge priority, 0 first. It is 0xFFFF for pieces that are
    never a merge result: control, user-defined and byte pieces.
  - The engine builds its piece hash table in work memory at open.
- **GRAM:** the call grammar.
  - Header: `u16 open`, `close`, `im_end`, `bos`, `n_tools`, `n_args`,
    `n_values`, `n_prefix`, `n_suffix`, `user_literal`, then `digits[10]`.
  - Then the prompt prefix and suffix token ids, the tools (8 bytes each),
    the arguments (14 bytes each), the values (6 bytes each) and the
    NUL-terminated name strings.
  - `tools/ipro_llm/export/export_blob.py` has the exact struct layouts.
- **NPUS (optional, `export_blob.py --npu-calib N`):** the static int8 scales
  of the NPU prefill. Header `u16 version 1`, `layers`, `q/k/v/gate rows`,
  `0`, `u32 calibration rows`, `0`; then per layer `f32 s_in, s_out` of
  q/k/v/gate (one fused projection) and `s_in, s_out` of out. Engines that
  do not know it ignore it; without it the accelerator cannot attach.

## Host tests

Build the blob and the PyTorch reference from the same checkpoint:

```bash
tools/ipro_llm/.venv/bin/python tools/ipro_llm/export/export_blob.py --ckpt <best.pt> --data tools/ipro_llm/data/phase2/reef_v4 --out /tmp/m.iplm
```

```bash
tools/ipro_llm/.venv/bin/python tools/ipro_llm/export/make_golden.py --ckpt <best.pt> --data tools/ipro_llm/data/phase2/reef_v4 --out /tmp/golden
```

```bash
sh components/ipro_llm/test_host/run.sh /tmp/m.iplm /tmp/golden
```

On `v4_xs_regen_s0` (2026-10-09):

- **Tokenizer:** 73,256 requests give exactly the same ids as SentencePiece.
- **Logits:** max error 1.1e-5 against the quantised PyTorch model (6e-7
  of the largest logit), with the same argmax on all 64 prompts.
- **Decoding:** 10,948 dev, challenge and test requests give the same
  generated tokens and the same JSON as `decode.greedy_decode`.
- **Sanitizers:** ASan and UBSan report nothing.
- **Speed:** 12 ms per request on an M5.

## Kernels

`ipro_llm_set_kernel()` picks one of two kernels.

- **`IPRO_LLM_KERNEL_INT8` (default):**
  - Each Hadamard-rotated group of the activation is quantised to int8 with
    scale max / 127. The codebook is quantised to int8 as level / top * 127.
  - A row group is then one int32 sum. On IPRO7 that sum is
    `__RV_SMAQA` / `__RV_PKBB16` over 256-entry tables in OCRAM, as in
    `apps/test/ipro_llm_kernel_bench`. Elsewhere it is the same integer
    arithmetic in C, so the host measures the board's accuracy.
- **`IPRO_LLM_KERNEL_FLOAT`:** the reference that matches PyTorch.

`ipro_llm_profile()` reports where the last request spent its time. The
units come from `ipro_llm_clock()`, which is weak and returns 0 by default;
ipro_voice_device returns microseconds.

On `v4_xs_regen_s0_qat`, the host int8 kernel gives the same calls as
PyTorch on 99.95 % of the 10,948 decode rows. Its exact match against the
dataset answers is 95.12 %, against 95.09 % for the reference. `run.sh`
fails if int8 is more than 0.5 point below the reference.

## On the IPRO7AI Reef

The blob is 1,688,007 B. Copying it from flash to PSRAM takes 98 ms, and
open (CRC + setup) takes about 690 ms. The 8 boot requests have 11-17 prompt
tokens and 3-9 generated tokens; the self-test runs all 8 back to back.

| step (2026-10-09) | per request | 8-request self-test |
|---|---|---|
| float reference kernels | 1.25-2.17 s | 13.7 s |
| int8 SMAQA kernels | 0.45-0.80 s | 5.10 s |
| + batched prefill, cached prompt prefix, fast fp16 norms, `-fno-math-errno` | 0.26-0.55 s | 3.39 s |
| + forced tokens without forwards, `llm_expf` | **0.21-0.48 s** | **2.88 s** |

What each step is:

- **Batched prefill:** the prompt goes through in chunks of up to
  `LLM_BATCH` (32) tokens. Each weight row group is expanded once per chunk.
  A prompt token now costs about 20 ms and a decode token about 29 ms.
- **Prefix cache:** `<s><|im_start|>` sits at the same positions in every
  prompt, so its KV is computed once.
- **Forced tokens:** `<tool_call>` is forced at the start, so it rides along
  with the prefill. After `</tool_call>` only `<|im_end|>` is left, so that
  step needs no forward.
- **`llm_expf`:** a 1-4 ulp polynomial `expf`, the same code on every target,
  used by the sigmoid gate, SiLU and softmax.

All steps except `llm_expf` give bit-identical outputs; the host checks this
by diffing every row of every decode (`LLM_DUMP`). `llm_expf` changed 1 of
10,948 int8 decodes, an input that was already wrong, and no float decode.

Tried and dropped: four SMAQA accumulators instead of one (no change on the
board; the loop is limited by the core, not by the SMAQA latency).

Where the time goes now: attention projections 64 %, attention core 15 %,
MLP 11 %, engram 6 %. A batched int8 MAC costs about 2.5 cycles; Phase 0
measured about 1.9 for the bare loop.

## NPU projections (IPRO7, `CONFIG_IPRO_LLM_NPU`)

The attention projections can run on the NPU: prompt chunks (2-32 tokens) and
one-token decode steps. Each layer is two NPU calls, q, k, v and gate fused
(192 -> 576) and out (192 -> 192). Every projection has two models, one built
for 32 tokens (a shorter chunk is padded: the NPU's cost is the weight
stream, `apps/test/ipro_llm_npu_bench`) and one for a single token, which
shares the 32-token model's weights.

```c
ipro_llm_accel_t npu;                          /* port/ipro7/ipro_llm_npu.h */
ipro_llm_npu_init(&npu);
size_t n = ipro_llm_accel_scratch_bytes(m);    /* 0: the blob has no NPUS section */
void *scratch = pvPortMalloc(n);
ipro_llm_accel_attach(m, &npu, scratch, n);    /* builds and loads 24 models */
vPortFree(scratch);
ipro_llm_accel_enable(m, IPRO_LLM_ACCEL_PREFILL);   /* A/B: prompt only (ALL after attach) */
```

The arithmetic is TFLite's int8 convolution with zero points 0
(`src/llm_accel.c` has it in full):

- **Input:** `x8 = sat8(round(x / s_in))`, one static scale per projection
  (NPUS).
- **Weights:** each CQ row dequantised (`llm_cq_row`) and quantised to int8
  with `max |w| / 127`, at attach.
- **Output:** `y8 = sat8(MultiplyByQuantizedMultiplier(acc, mult, shift))`,
  with the multiplier of `s_in * s_w / s_out`; `y = y8 * s_out`.

`tools/ipro_llm/train/npu_sim.py` simulates the same and calibrates the
scales; applied to every position it costs the v7 model under 0.1 point.

How the models are built on the board:

- **Templates:** of an `.inm` only the two 48-byte instruction words depend
  on the geometry. `port/ipro7/llm_npu_templates.h` keeps them per shape at
  32 and 1 tokens, from IPRO AI Studio's converter
  (`tools/ipro_llm/export/npu_templates.py`). Three fields in them carry
  numbers and are filled at load: the output scale (float32 bits in
  `cpu.inst`) and the last output channel's multiplier and shift
  (`npu.inst`).
- **Sections:** bias (zero), weights in 4 x 4 channel blocks (+128) and the
  multiplier / shift section are packed in C. The layout is in
  `tools/ipro_llm/export/npu_inm.py`; `tools/ipro_llm/tests/test_npu_inm.py`
  rebuilds the converter's output for the bench models byte for byte.
- **Memory:** the runtime copies every section to the heap (weights 0.9 MB
  for XS, once: the one-token models point at the chunk models' weights) and
  all models share one 256 KB activation buffer.
- **Other shapes:** a model with a different d needs its templates first:
  `npu_templates.py --shape <cin>,<cout> ...`.

`ipro_llm_npu_check(slot, n, ...)` runs one projection on a pseudo-random
input (n = 1: on the one-token model) and compares every output with the
integer reference computed from the weights and multipliers the runtime
holds.

🔴 The NPU runtime writes a model's buffer addresses into the NPU only when it
loads it (`iproai_npu_initCfg`), so with several models loaded every run used
the last one's weights (the check: 11 of 12 projections wrong). `npu_infer()`
points the NPU at its model's sections (`hal_npu_layer_setup`,
`hal_npu_set_input_buffer`, `hal_npu_set_net_param`) before every run. A
buffer the runtime allocated goes back with `IPROAI_FREE` (its pointer sits
behind a header), not `vPortFree`.

Host check: `ipro_llm_accel_ref()` is the same arithmetic in C as an
accelerator (prompt and decode), and `run.sh` decodes every golden row with
it (`accel`):

| v8 cq_emb2 + NPUS (512 rows), 10,937 rows | same calls as PyTorch | exact match vs answers |
|---|---|---|
| float kernels (the PyTorch reference) | 100 % | 0.9411 |
| int8 kernels (CPU) | 99.95 % | 0.9408 |
| + NPU arithmetic for the projections (prompt and decode) | 99.76 % | 0.9414 |

On the IPRO7AI Reef (`ipro_voice_device` config_reef, 2026-10-09):

- **Attach:** 24 models in 1.27 s on the model task (after BLE is up),
  1,212 KB heap.
- **`llm_npu check`:** all 12 projections, both models, bit-exact against the
  integer reference, so the host numbers above are the board's.

## Speed on the IPRO7AI Reef

The core's FPU does not pipeline: a float multiply-add costs about 10 cycles,
while SMAQA does four int8 products in one. So the work left after the NPU
took the projections was float, and it was changed in two steps:

- **Bit-identical** (every kernel, checked with `LLM_DUMP` on all golden
  rows): `llm_expf` / sigmoid / SiLU inline, the FWHT three butterfly stages
  per pass in registers, the two query heads of a KV head attended together
  (each K / V row read once, four independent sums), the ZC-RMSNorm gain
  (1 + scale) folded at open.
- **The int8 kernel only** (the float kernel stays the PyTorch reference):
  sigmoid, SiLU and the softmax's exp from interpolated tables (built from
  `llm_expf` at open, the same on every target); attention on an int8 KV:
  K rows and transposed V columns in int8 with one scale per row, q . K and
  P . V by SMAQA, the softmax in float. Accuracy is in the table above.

`CONFIG_IPRO_LLM_PROF_CYCLES` (debug) prints the float work's cycle buckets
after every request. One 21+9-token request, in millions of cycles:

| | KV write | q / k norm + RoPE | attend | sigmoid gate | FWHT | SiLU |
|---|---|---|---|---|---|---|
| before | 1.3 | 1.7 | 14.0 | 4.2 | 4.4 | 6.0 |
| after | 2.4 (+ int8 KV) | 1.4 | 4.8 | 1.5 | 1.9 | 2.0 |

(The norms, 3.2 after, only lost an add per element; the engram, 7.6, is unchanged.)

10 English shell requests (12-21 prompt tokens), the model's time:

| | before | after |
|---|---|---|
| CPU only | mean 417 ms | mean 347 ms (213-537) |
| NPU for the projections | mean 249 ms (prompt only) | **mean 166 ms** (101-255, prompt and decode) |

The same call lists either way. What is left: the engram's two 192 x 192
projections on the CPU (about 39 ms of a 21+9-token request), the norms and
the KV quantisation.

// Teach without the converter: write a trained head straight into a template .inm.
//
// A port of IPRO AI Studio's ipro_ai_core/teach_inm.py, checked byte for byte
// against it (test/patch.test.mjs, golden vectors from build_web_assets.py),
// which in turn is checked byte for byte against the native converter.
//
// The head (a 1x1 conv, OC x IC) lives at the tail of four .inm sections:
//   NPU weights   last OC*IC bytes, 4x4 blocks (oc/4, ic/4, oc%4, ic%4), int8 + 128
//   NPU bias      last OC*4 bytes, int32
//   NPU mult      8 bytes of 6-bit shifts, then OC int32 multipliers
//   NPU inst      the head's words repeat the last channel's multiplier and shift
//   DSP inst      the output scale, float32 at bit 12 of the last word

const HEADER_WORDS = 8;
const SECTIONS = ["dsp_inst", "dsp_bias", "dsp_wei", "dsp_mult", "npu_inst", "npu_bias", "npu_wei", "npu_mult"];
const INT32_MIN = -2147483648;
const INT32_MAX = 2147483647;

/** (offset, length) of every section after the 8-word header. */
export function sections(inm) {
  const dv = new DataView(inm.buffer, inm.byteOffset, inm.byteLength);
  const out = {};
  let off = HEADER_WORDS * 4;
  SECTIONS.forEach((name, i) => {
    const n = dv.getInt32(i * 4, true);
    out[name] = [off, n];
    off += n;
  });
  if (off !== inm.byteLength) throw new Error(`.inm header sums to ${off} bytes, the file has ${inm.byteLength}`);
  return out;
}

/** numpy's round: half to even. */
function roundHalfEven(x) {
  const f = Math.floor(x);
  const d = x - f;
  if (d > 0.5) return f + 1;
  if (d < 0.5) return f;
  return f % 2 === 0 ? f : f + 1;
}

/**
 * The fitted head as the template stores it. w is C x n (row-major Float64Array,
 * w[c * n + k]), b has n entries. Same arithmetic as head_quant() in Python.
 */
export function headQuant(w, b, logitRange, sIn, nMax, C) {
  const n = b.length;
  if (n > nMax) throw new Error(`${n} classes; this template's head has ${nMax}`);
  const wf = new Float64Array(nMax * C);          // nMax x C
  for (let k = 0; k < n; k++) for (let c = 0; c < C; c++) wf[k * C + c] = w[c * n + k];
  const bf = new Float64Array(nMax).fill(-logitRange);   // unused classes can never win
  for (let k = 0; k < n; k++) bf[k] = b[k];
  const sW = new Float64Array(nMax);
  for (let k = 0; k < nMax; k++) {
    let m = 0;
    for (let c = 0; c < C; c++) m = Math.max(m, Math.abs(wf[k * C + c]));
    sW[k] = m / 127.0;
  }
  // Unused rows are all zero; give them the used rows' scale so their bias fits int32.
  let used = 0;
  for (let k = 0; k < n; k++) used = Math.max(used, sW[k]);
  for (let k = n; k < nMax; k++) sW[k] = used;
  for (let k = 0; k < nMax; k++) sW[k] = Math.max(sW[k], 1e-8);
  const wq = new Int8Array(nMax * C);
  for (let k = 0; k < nMax; k++) {
    for (let c = 0; c < C; c++) {
      wq[k * C + c] = Math.min(127, Math.max(-127, roundHalfEven(wf[k * C + c] / sW[k])));
    }
  }
  const bq = new Int32Array(nMax);
  for (let k = 0; k < nMax; k++) {
    bq[k] = Math.min(INT32_MAX, Math.max(INT32_MIN, roundHalfEven(bf[k] / (sIn * sW[k]))));
  }
  return { wq, bq, sW, sOut: Math.fround(logitRange / 127.0) };
}

function frexp(x) {
  // x = q * 2^e, q in [0.5, 1); exact (only powers of two are multiplied).
  let e = Math.floor(Math.log2(x)) + 1;
  let q = x / Math.pow(2, e);
  while (q >= 1) { q /= 2; e += 1; }
  while (q < 0.5) { q *= 2; e -= 1; }
  return [q, e];
}

/** TFLite's QuantizeMultiplier: eff = m * 2^(shift - 31), m in [2^30, 2^31). */
export function quantizeMultiplier(eff) {
  if (eff === 0) return [0, 0];
  let [q, shift] = frexp(eff);
  let m = Math.floor(q * 2147483648 + 0.5);
  if (m === 2147483648) { m /= 2; shift += 1; }
  return [m, shift];
}

function putBits(buf, byteOff, bit, width, value, nbytes) {
  let word = 0n;
  for (let i = nbytes - 1; i >= 0; i--) word = (word << 8n) | BigInt(buf[byteOff + i]);
  const mask = ((1n << BigInt(width)) - 1n) << BigInt(bit);
  word = (word & ~mask) | ((BigInt(value) << BigInt(bit)) & mask);
  for (let i = 0; i < nbytes; i++) { buf[byteOff + i] = Number(word & 0xffn); word >>= 8n; }
}

/** A copy of the template .inm (Uint8Array) with this head; wq is OC x IC. */
export function patchInm(template, wq, bq, sIn, sW, sOut, OC, IC) {
  if (OC % 4 || IC % 4) throw new Error(`head ${OC}x${IC}: the NPU packs channels in fours`);
  const out = new Uint8Array(template);
  const sec = sections(template);
  const dv = new DataView(out.buffer);

  let [o, n] = sec.npu_wei;
  let p = o + n - OC * IC;
  for (let ob = 0; ob < OC / 4; ob++) {
    for (let ib = 0; ib < IC / 4; ib++) {
      for (let u = 0; u < 4; u++) {
        for (let v = 0; v < 4; v++) out[p++] = wq[(ob * 4 + u) * IC + ib * 4 + v] + 128;
      }
    }
  }

  [o, n] = sec.npu_bias;
  for (let k = 0; k < OC; k++) dv.setInt32(o + n - OC * 4 + 4 * k, bq[k], true);

  // Each scale went through float32 in the TFLite; the converter works in double.
  const sIn32 = Math.fround(sIn);
  const sOut32 = Math.fround(sOut);
  const ms = Array.from(sW, (sw) => quantizeMultiplier(sIn32 * Math.fround(sw) / sOut32));
  if (ms.some(([, sh]) => sh < -32 || sh >= 32)) throw new Error("a channel's shift does not fit the NPU's 6-bit field");
  [o, n] = sec.npu_mult;
  let packed = 0n;
  ms.forEach(([, sh], c) => { packed |= BigInt(sh & 63) << BigInt(6 * c); });
  for (let i = 0; i < 8; i++) out[o + n - OC * 4 - 8 + i] = Number((packed >> BigInt(8 * i)) & 0xffn);
  ms.forEach(([m], k) => dv.setInt32(o + n - OC * 4 + 4 * k, m, true));

  [o, n] = sec.npu_inst;
  const [mLast, shLast] = ms[ms.length - 1];
  putBits(out, o + n - 40, 13, 31, mLast, 8);
  putBits(out, o + n - 6, 5, 6, shLast & 63, 2);

  [o, n] = sec.dsp_inst;
  const f = new DataView(new ArrayBuffer(4));
  f.setFloat32(0, sOut32, true);
  putBits(out, o + n - 16, 12, 32, f.getUint32(0, true), 8);
  return out;
}

/** mfg + 0x1000: the .inm, then the label block the firmware reads after it. */
export function payload(inm, labels, maxClasses = 8, labelMax = 15) {
  if (labels.length < 1 || labels.length > maxClasses) throw new Error(`${labels.length} labels; the board takes 1..${maxClasses}`);
  const block = new Uint8Array(8 + maxClasses * (labelMax + 1));
  block.set([0x54, 0x43, 0x48, 0x31, 1, labels.length, 0, 0]);   // "TCH1", version 1, count, reserved
  labels.forEach((name, i) => {
    const s = normaliseLabel(name);
    for (let j = 0; j < s.length; j++) block[8 + i * (labelMax + 1) + j] = s.charCodeAt(j);
  });
  const out = new Uint8Array(inm.length + block.length);
  out.set(inm, 0);
  out.set(block, inm.length);
  return out;
}

/** Upper-case ASCII the board's 5x7 font draws, at most 15 characters. */
export function normaliseLabel(text, labelMax = 15) {
  let s = text.toUpperCase().replace(/[^A-Z0-9 _\-.]/g, "").replace(/\s+/g, " ").trim();
  s = s.slice(0, labelMax).trim();
  if (!s) throw new Error(`"${text}" has no characters the board can show (A-Z, 0-9)`);
  return s;
}

// The frozen backbone in the browser: LiteRT.js (WebAssembly, CPU) runs the
// template up to the head's input and returns its 4x4xC int8 feature map.
//
// The runtime comes from vendor/litertjs when serve.py --fetch-runtime put it
// there, otherwise from the jsDelivr CDN (pinned version). The page's import map
// points the bare "@litertjs/wasm-utils" import at the vendored copy.

const VERSION = "2.6.0";
const CDN = `https://cdn.jsdelivr.net/npm/@litertjs/core@${VERSION}`;

let runtime = null;

/** Load LiteRT.js once; `base` is the page's URL of tools/teach_web/. */
export async function loadRuntime(base) {
  if (runtime) return runtime;
  const local = new URL("vendor/litertjs/core/", base).href;
  let lib;
  let wasm;
  try {
    lib = await import(local + "dist/index.js");
    wasm = local + "wasm/";
  } catch {
    lib = await import(`${CDN}/+esm`);
    wasm = `${CDN}/wasm/`;
  }
  await lib.loadLiteRt(wasm);
  runtime = { lib, source: wasm.startsWith("http") && !wasm.startsWith(base) ? "cdn" : "local" };
  return runtime;
}

/** One backbone: `features(rgb)` gives the int8 feature map of a 128x128 RGB crop. */
export class Backbone {
  constructor(meta, model, lib) {
    this.meta = meta;
    this.model = model;
    this.lib = lib;
    [, this.h, this.w, this.c] = meta.feat_shape;
  }

  static async load(base, meta) {
    const { lib } = await loadRuntime(base);
    const model = await lib.loadAndCompile(new URL(`assets/${meta.features}`, base).href, { accelerator: "wasm" });
    return new Backbone(meta, model, lib);
  }

  /**
   * rgb: Uint8Array/Uint8ClampedArray of 128*128*3 (or RGBA with stride 4).
   * The board feeds uint8 pixels the model sees as int8 = pixel - 128, R, G, B.
   */
  async features(rgb, stride = 3) {
    const n = this.meta.input || 128;
    const q = new Int8Array(n * n * 3);
    for (let i = 0, j = 0; i < n * n; i++, j += stride) {
      q[3 * i] = rgb[j] - 128;
      q[3 * i + 1] = rgb[j + 1] - 128;
      q[3 * i + 2] = rgb[j + 2] - 128;
    }
    const input = new this.lib.Tensor(q, [1, n, n, 3]);
    const out = await this.model.run(input);
    input.delete();
    const t = out[0];
    const data = Int8Array.from(t.toTypedArray());
    for (const o of out) o.delete();
    return data;                       // h * w * c, int8
  }

  /** The head's input averaged over the 4x4 positions, dequantised (C floats). */
  async pooled(rgb, stride = 3) {
    const f = await this.features(rgb, stride);
    const { feat_scale: s, feat_zp: zp } = this.meta;
    const C = this.c;
    const pos = f.length / C;
    const out = new Float64Array(C);
    for (let p = 0; p < pos; p++) for (let c = 0; c < C; c++) out[c] += f[p * C + c] - zp;
    for (let c = 0; c < C; c++) out[c] = (out[c] / pos) * s;
    return out;
  }
}

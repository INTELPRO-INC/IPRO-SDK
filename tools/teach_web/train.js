// Training the head: the same softmax regression as IPRO AI Studio's teach.fit(),
// on standardised features, folded back onto raw ones (the board's head sees raw
// features). Pure functions; train_worker.js runs them off the page's thread.

/**
 * x: N x C Float32Array (row-major), y: Int32Array of class indices, n classes.
 * Returns {w: C x n Float64Array (w[c * n + k]), b: Float64Array(n)}.
 */
export function fit(x, y, N, C, n, { l2 = 1e-3, iters = 500, lr = 0.5, onIter } = {}) {
  const mu = new Float64Array(C);
  const sd = new Float64Array(C);
  for (let i = 0; i < N; i++) for (let c = 0; c < C; c++) mu[c] += x[i * C + c];
  for (let c = 0; c < C; c++) mu[c] /= N;
  for (let i = 0; i < N; i++) for (let c = 0; c < C; c++) { const d = x[i * C + c] - mu[c]; sd[c] += d * d; }
  for (let c = 0; c < C; c++) sd[c] = Math.sqrt(sd[c] / N) + 1e-6;
  const z = new Float32Array(N * C);
  for (let i = 0; i < N; i++) for (let c = 0; c < C; c++) z[i * C + c] = (x[i * C + c] - mu[c]) / sd[c];

  const w = new Float64Array(C * n);
  const b = new Float64Array(n);
  const logits = new Float64Array(N * n);
  const gW = new Float64Array(C * n);
  const g = new Float64Array(N * n);
  for (let it = 0; it < iters; it++) {
    logits.fill(0);
    for (let i = 0; i < N; i++) {
      const zi = i * C;
      const li = i * n;
      for (let c = 0; c < C; c++) {
        const v = z[zi + c];
        if (v === 0) continue;
        const wc = c * n;
        for (let k = 0; k < n; k++) logits[li + k] += v * w[wc + k];
      }
      let m = -Infinity;
      for (let k = 0; k < n; k++) { logits[li + k] += b[k]; m = Math.max(m, logits[li + k]); }
      let s = 0;
      for (let k = 0; k < n; k++) { const e = Math.exp(logits[li + k] - m); g[li + k] = e; s += e; }
      for (let k = 0; k < n; k++) g[li + k] = (g[li + k] / s - (y[i] === k ? 1 : 0)) / N;
    }
    gW.fill(0);
    for (let i = 0; i < N; i++) {
      const zi = i * C;
      const li = i * n;
      for (let c = 0; c < C; c++) {
        const v = z[zi + c];
        if (v === 0) continue;
        const wc = c * n;
        for (let k = 0; k < n; k++) gW[wc + k] += v * g[li + k];
      }
    }
    for (let j = 0; j < C * n; j++) w[j] -= lr * (gW[j] + l2 * w[j]);
    for (let k = 0; k < n; k++) {
      let s = 0;
      for (let i = 0; i < N; i++) s += g[i * n + k];
      b[k] -= lr * s;
    }
    if (onIter && it % 25 === 0) onIter(it / iters);
  }
  // z @ w + b == x @ (w / sd) + (b - (mu / sd) @ w)
  const wr = new Float64Array(C * n);
  const br = Float64Array.from(b);
  for (let c = 0; c < C; c++) {
    for (let k = 0; k < n; k++) {
      wr[c * n + k] = w[c * n + k] / sd[c];
      br[k] -= (mu[c] / sd[c]) * w[c * n + k];
    }
  }
  return { w: wr, b: br };
}

/** argmax of x @ w + b for each row; x is M x C. */
export function predict(x, M, C, w, b) {
  const n = b.length;
  const out = new Int32Array(M);
  for (let i = 0; i < M; i++) {
    let best = -Infinity;
    for (let k = 0; k < n; k++) {
      let s = b[k];
      for (let c = 0; c < C; c++) s += x[i * C + c] * w[c * n + k];
      if (s > best) { best = s; out[i] = k; }
    }
  }
  return out;
}

/**
 * The board's output tensor carries one position's logits, which swing wider
 * than their average: size the int8 output scale for those, with margin.
 * maps: array of dequantised feature maps (P x C Float32Array each).
 */
export function logitRange(maps, P, C, w, b) {
  const n = b.length;
  let peak = 0;
  for (const f of maps) {
    for (let p = 0; p < P; p++) {
      for (let k = 0; k < n; k++) {
        let s = b[k];
        for (let c = 0; c < C; c++) s += f[p * C + c] * w[c * n + k];
        peak = Math.max(peak, Math.abs(s));
      }
    }
  }
  return peak * 1.25 || 1.0;
}

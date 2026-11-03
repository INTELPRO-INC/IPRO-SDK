// Runs train.js off the page's thread. One message in, progress and one result out.
import { fit, predict, logitRange } from "./train.js";

self.onmessage = (ev) => {
  const { x, y, N, C, n, test, testY, M, maps, P } = ev.data;
  try {
    const t0 = performance.now();
    const { w, b } = fit(x, y, N, C, n, { onIter: (f) => self.postMessage({ progress: f }) });
    let acc = null;
    const confusion = Array.from({ length: n }, () => new Array(n).fill(0));
    if (M > 0) {
      const pred = predict(test, M, C, w, b);
      let right = 0;
      for (let i = 0; i < M; i++) {
        confusion[testY[i]][pred[i]] += 1;
        if (pred[i] === testY[i]) right++;
      }
      acc = right / M;
    }
    const range = logitRange(maps, P, C, w, b);
    self.postMessage({ done: true, w, b, logitRange: range, acc, confusion, ms: performance.now() - t0 }, [w.buffer, b.buffer]);
  } catch (err) {
    self.postMessage({ error: String(err && err.message ? err.message : err) });
  }
};

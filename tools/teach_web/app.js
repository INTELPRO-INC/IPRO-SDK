// IPRO AI Teach in the browser: record crops from the Reef's own camera, train
// a classifier head on them, and deploy it to the board over the same USB cable.
//
// The same flow as IPRO AI Studio's Teach page, without installing anything:
// the backbone runs in LiteRT.js (backbone.js), the head trains in a worker
// (train.js), the .inm is written without the converter (teach_inm.js), and the
// board is reached over Web Serial (board.js). Chrome or Edge, served from
// localhost (serve.py) or https.

import { Backbone, loadRuntime } from "./backbone.js";
import { headQuant, patchInm, payload, normaliseLabel } from "./teach_inm.js";
import * as board from "./board.js";

const $ = (id) => document.getElementById(id);
const BASE = new URL("./", location.href).href;
const IN = 128;
const REC_FPS = 10;
const THUMBS = 9;

const state = {
  assets: null,
  backbone: null,
  backbones: {},             // name -> Backbone
  templates: {},             // name -> Uint8Array (.inm)
  classes: [],               // {id, name, samples: [{rgba, thumb, feat, drop}]}
  selected: 0,
  stream: null,
  port: null,
  trained: null,             // {backbone, labels, w, b, logitRange, acc, signature}
  recording: false,
  deploying: false,
};
let nextId = 1;

// ---------------------------------------------------------------------------
// Start-up and environment checks
// ---------------------------------------------------------------------------

function notice(html) {
  const n = $("notice");
  n.innerHTML = html;
  n.hidden = !html;
}

async function main() {
  const problems = [];
  if (!window.isSecureContext) {
    problems.push("Open this page from <code>http://localhost</code> (run <code>python3 tools/teach_web/serve.py</code>) or https: the camera and the board are not reachable from a file.");
  }
  if (!board.supported()) {
    problems.push("This browser has no Web Serial, so it cannot reach the board. Use <b>Chrome</b> or <b>Edge</b> on Windows, macOS or Linux.");
  }
  notice(problems.join("<br>"));

  state.assets = await (await fetch("assets/teach_assets.json")).json();
  const sel = $("backbone");
  for (const [name, meta] of Object.entries(state.assets.backbones)) {
    sel.add(new Option(meta.description, name));
  }
  state.backbone = sel.value;
  sel.addEventListener("change", () => {
    state.backbone = sel.value;
    state.trained = null;
    for (const c of state.classes) for (const s of c.samples) queueFeatures(s);
    renderTrain();
  });

  addClass("BACKGROUND");
  addClass("CLASS 1");
  state.selected = 1;
  renderClasses();
  wireUi();

  loadRuntime(BASE).then(
    (rt) => { $("runtime-note").textContent = rt.source === "cdn" ? "LiteRT.js from the CDN." : ""; },
    (err) => notice(`The model runtime did not load (${err.message}). Without internet, run <code>python3 tools/teach_web/serve.py --fetch-runtime</code> once.`),
  );
  await startCamera();
  const p = await board.knownPort();
  if (p) { state.port = p; refreshBoard(); }
  navigator.mediaDevices?.addEventListener("devicechange", onDeviceChange);
  navigator.serial?.addEventListener("connect", (e) => {
    if (board.VID === e.target.getInfo().usbVendorId) { state.port = e.target; refreshBoard(); }
  });
  navigator.serial?.addEventListener("disconnect", (e) => {
    if (state.port === e.target && !state.deploying) { setBoard("Board unplugged", false); }
  });
}

// ---------------------------------------------------------------------------
// Camera
// ---------------------------------------------------------------------------

function stageMsg(text) {
  const m = $("stage-msg");
  m.textContent = text || "";
  m.hidden = !text;
}

async function listCameras() {
  const devs = (await navigator.mediaDevices.enumerateDevices()).filter((d) => d.kind === "videoinput");
  const sel = $("camera-select");
  const keep = sel.value;
  sel.innerHTML = "";
  for (const d of devs) sel.add(new Option(d.label || "Camera", d.deviceId));
  const reef = devs.find((d) => /IPRO7 UVC/i.test(d.label));
  if (devs.some((d) => d.deviceId === keep)) sel.value = keep;
  else if (reef) sel.value = reef.deviceId;
  return { devs, reef };
}

async function startCamera() {
  stopCamera();
  if (!navigator.mediaDevices?.getUserMedia) { stageMsg("This browser cannot open a camera."); return; }
  try {
    if (!$("camera-select").value) {
      // Labels and device ids appear only after the first permission grant.
      const probe = await navigator.mediaDevices.getUserMedia({ video: true });
      probe.getTracks().forEach((t) => t.stop());
    }
    const { devs, reef } = await listCameras();
    if (!devs.length) { stageMsg("No camera found. Plug in the Reef (USB-C) with the Teach firmware."); return; }
    const id = $("camera-select").value;
    state.stream = await navigator.mediaDevices.getUserMedia({
      video: { deviceId: { exact: id }, width: { ideal: 640 }, height: { ideal: 360 } },
    });
    const v = $("video");
    v.srcObject = state.stream;
    await v.play();
    state.stream.getVideoTracks()[0].addEventListener("ended", () => {
      if (!state.deploying) stageMsg("The camera went away. Plug the board back in.");
    });
    stageMsg("");
    const isReef = reef && reef.deviceId === id;
    if (!isReef) {
      notice("This camera is not the Reef. Pick <b>IPRO7 UVC Cam</b> above: the model is trained on the Reef's own view.");
    }
    drawOverlay();
    $("record").disabled = false;
  } catch (err) {
    stageMsg(err.name === "NotAllowedError" ? "Camera permission was denied. Allow it in the address bar and reload."
      : `The camera did not start (${err.message}).`);
  }
}

function stopCamera() {
  if (state.stream) state.stream.getTracks().forEach((t) => t.stop());
  state.stream = null;
  $("video").srcObject = null;
  $("record").disabled = true;
}

let devTimer = null;
function onDeviceChange() {
  // After a deploy the board re-enumerates; also catches a board plugged in late.
  clearTimeout(devTimer);
  devTimer = setTimeout(async () => {
    if (state.deploying) return;
    const { reef } = await listCameras();
    const live = state.stream && state.stream.getVideoTracks()[0]?.readyState === "live";
    if (reef && !live) startCamera();
  }, 600);
}

/** The NPU's square in a w x h stream frame (the firmware's ISP channel 1). */
function box(w, h) {
  const [x, y, s] = state.assets.box_1280x720;
  return [Math.floor((x * w) / 1280), Math.floor((y * h) / 720), Math.floor((s * w) / 1280)];
}

function drawOverlay() {
  const v = $("video");
  const c = $("overlay");
  const r = c.getBoundingClientRect();
  c.width = Math.round(r.width * devicePixelRatio);
  c.height = Math.round(r.height * devicePixelRatio);
  const g = c.getContext("2d");
  g.clearRect(0, 0, c.width, c.height);
  if (!v.videoWidth) return;
  // Where the video sits inside the stage (object-fit: contain).
  const k = Math.min(c.width / v.videoWidth, c.height / v.videoHeight);
  const ox = (c.width - v.videoWidth * k) / 2;
  const oy = (c.height - v.videoHeight * k) / 2;
  const [bx, by, bs] = box(v.videoWidth, v.videoHeight);
  const X = ox + bx * k, Y = oy + by * k, S = bs * k;
  g.fillStyle = "rgba(0,0,0,0.35)";
  g.fillRect(0, 0, c.width, Y);
  g.fillRect(0, Y + S, c.width, c.height - Y - S);
  g.fillRect(0, Y, X, S);
  g.fillRect(X + S, Y, c.width - X - S, S);
  g.strokeStyle = state.recording ? "#e5483b" : "rgba(255,255,255,0.9)";
  g.lineWidth = 2 * devicePixelRatio;
  g.strokeRect(X, Y, S, S);
}

const crop = new OffscreenCanvas(IN, IN);
const cropG = crop.getContext("2d", { willReadFrequently: true });

/** The box out of the current video frame, at the model's input size (RGBA). */
function grabCrop() {
  const v = $("video");
  if (!v.videoWidth) return null;
  const [x, y, s] = box(v.videoWidth, v.videoHeight);
  cropG.imageSmoothingEnabled = true;
  cropG.imageSmoothingQuality = "medium";
  cropG.drawImage(v, x, y, s, s, 0, 0, IN, IN);
  return cropG.getImageData(0, 0, IN, IN).data.slice();
}

async function thumbOf(rgba) {
  const c = new OffscreenCanvas(IN, IN);
  c.getContext("2d").putImageData(new ImageData(new Uint8ClampedArray(rgba), IN, IN), 0, 0);
  const blob = await c.convertToBlob({ type: "image/jpeg", quality: 0.85 });
  return URL.createObjectURL(blob);
}

// ---------------------------------------------------------------------------
// Recording
// ---------------------------------------------------------------------------

let recTimer = null;
function startRecording() {
  if (state.recording || !state.stream || state.deploying) return;
  state.recording = true;
  setStep("collect");
  $("record").classList.add("recording");
  drawOverlay();
  const cls = state.classes[state.selected];
  recTimer = setInterval(async () => {
    const rgba = grabCrop();
    if (!rgba) return;
    const s = { rgba, thumb: await thumbOf(rgba), feat: null };
    cls.samples.push(s);
    queueFeatures(s);
    renderClass(cls);
    renderTrain();
  }, 1000 / REC_FPS);
}

function stopRecording() {
  if (!state.recording) return;
  state.recording = false;
  clearInterval(recTimer);
  $("record").classList.remove("recording");
  drawOverlay();
  state.trained = null;
  renderTrain();
}

// ---------------------------------------------------------------------------
// Features, computed in the background as crops come in
// ---------------------------------------------------------------------------

const featQueue = [];
let featBusy = false;
let featWaiters = [];

function queueFeatures(s) {
  s.feat = null;
  featQueue.push(s);
  pumpFeatures();
}

function featuresPending() {
  return state.classes.reduce((n, c) => n + c.samples.filter((s) => !s.feat).length, 0);
}

async function getBackbone(name) {
  if (!state.backbones[name]) {
    const meta = { ...state.assets.backbones[name], input: state.assets.input };
    state.backbones[name] = Backbone.load(BASE, meta);
  }
  return state.backbones[name];
}

/** Variety the frozen backbone cannot learn by itself: a mirror and two exposure changes (as Studio). */
function augment(rgba) {
  const out = [rgba];
  const m = new Uint8ClampedArray(rgba.length);
  for (let y = 0; y < IN; y++) {
    for (let x = 0; x < IN; x++) {
      const a = (y * IN + x) * 4;
      const b = (y * IN + (IN - 1 - x)) * 4;
      m[a] = rgba[b]; m[a + 1] = rgba[b + 1]; m[a + 2] = rgba[b + 2]; m[a + 3] = 255;
    }
  }
  out.push(m);
  for (let i = 0; i < 2; i++) {
    const gain = 0.75 + Math.random() * 0.5;
    const e = new Uint8ClampedArray(rgba.length);
    for (let j = 0; j < rgba.length; j += 4) {
      e[j] = rgba[j] * gain; e[j + 1] = rgba[j + 1] * gain; e[j + 2] = rgba[j + 2] * gain; e[j + 3] = 255;
    }
    out.push(e);
  }
  return out;
}

async function pumpFeatures() {
  if (featBusy) return;
  featBusy = true;
  try {
    while (featQueue.length) {
      const s = featQueue.shift();
      if (s.dropped || s.feat) continue;
      const name = state.backbone;
      const bb = await getBackbone(name);
      const meta = state.assets.backbones[name];
      const C = meta.channels;
      const pooled = [];
      let map = null;
      for (const [i, img] of augment(s.rgba).entries()) {
        const f = await bb.features(img, 4);
        if (i === 0) map = f;
        const p = new Float32Array(C);
        const P = f.length / C;
        for (let q = 0; q < P; q++) for (let c = 0; c < C; c++) p[c] += f[q * C + c] - meta.feat_zp;
        for (let c = 0; c < C; c++) p[c] = (p[c] / P) * meta.feat_scale;
        pooled.push(p);
      }
      if (name === state.backbone) s.feat = { backbone: name, pooled, map };
      else featQueue.push(s);
    }
  } catch (err) {
    notice(`Computing features failed: ${err.message}`);
  } finally {
    featBusy = false;
    const w = featWaiters;
    featWaiters = [];
    w.forEach((r) => r());
  }
}

function featuresReady() {
  if (!featuresPending() && !featBusy) return Promise.resolve();
  return new Promise((r) => { featWaiters.push(r); pumpFeatures(); });
}

// ---------------------------------------------------------------------------
// Classes
// ---------------------------------------------------------------------------

function addClass(name) {
  if (state.classes.length >= state.assets.max_classes) return;
  state.classes.push({ id: nextId++, name: name || `CLASS ${state.classes.length}`, samples: [] });
  state.selected = state.classes.length - 1;
  state.trained = null;
}

function renderClasses() {
  const list = $("class-list");
  list.innerHTML = "";
  state.classes.forEach((c, i) => {
    const el = document.createElement("div");
    el.className = "class" + (i === state.selected ? " on" : "");
    el.dataset.id = c.id;
    el.innerHTML = `
      <div class="class-top">
        <input value="" maxlength="15" aria-label="Class name">
        <span class="count"></span>
        <button data-act="review">Review</button>
        <button data-act="remove" ${state.classes.length <= 1 ? "disabled" : ""}>Remove</button>
      </div>
      <div class="thumbs"></div>`;
    const input = el.querySelector("input");
    input.value = c.name;
    input.addEventListener("change", () => {
      try { c.name = normaliseLabel(input.value); } catch { /* keep the old name */ }
      input.value = c.name;
      state.trained = null;
      renderTrain();
      renderRecordButton();
    });
    el.addEventListener("click", (e) => {
      const act = e.target.dataset?.act;
      if (act === "remove") {
        state.classes.splice(i, 1);
        state.selected = Math.min(state.selected, state.classes.length - 1);
        state.trained = null;
        renderClasses();
        return;
      }
      if (act === "review") { openReview(c); return; }
      if (state.selected !== i) { state.selected = i; renderClasses(); }
    });
    list.append(el);
    renderClass(c);
  });
  $("add-class").disabled = state.classes.length >= state.assets.max_classes;
  renderRecordButton();
  renderTrain();
}

function renderClass(c) {
  const el = document.querySelector(`.class[data-id="${c.id}"]`);
  if (!el) return;
  el.querySelector(".count").textContent = c.samples.length;
  const th = el.querySelector(".thumbs");
  const last = c.samples.slice(-THUMBS);
  if (!last.length) {
    th.innerHTML = `<span class="empty">${/BACKGROUND/.test(c.name) ? "Nothing in the box: lets the board say so" : "No pictures yet"}</span>`;
    return;
  }
  th.innerHTML = "";
  for (const s of last) {
    const img = document.createElement("img");
    img.src = s.thumb;
    img.alt = "";
    th.append(img);
  }
}

function renderRecordButton() {
  const c = state.classes[state.selected];
  $("record").textContent = c ? `Hold to record ${c.name}` : "Hold to record";
}

function openReview(c) {
  const dlg = $("review");
  $("review-title").textContent = `${c.name}: ${c.samples.length} pictures`;
  const grid = $("review-grid");
  grid.innerHTML = "";
  c.samples.forEach((s) => {
    const img = document.createElement("img");
    img.src = s.thumb;
    img.addEventListener("click", () => img.classList.toggle("drop"));
    img._sample = s;
    grid.append(img);
  });
  dlg.onclose = () => {
    if (dlg.returnValue === "delete") {
      const drop = new Set([...grid.querySelectorAll("img.drop")].map((i) => i._sample));
      c.samples = c.samples.filter((s) => !drop.has(s));
      drop.forEach((s) => { s.dropped = true; URL.revokeObjectURL(s.thumb); });
      state.trained = null;
      renderClasses();
    }
  };
  dlg.showModal();
}

// ---------------------------------------------------------------------------
// Training
// ---------------------------------------------------------------------------

function trainable() {
  const used = state.classes.filter((c) => c.samples.length);
  return used.length >= 2 && used.length === state.classes.length;
}

function renderTrain() {
  $("train").disabled = !trainable() || state.deploying;
  $("deploy").disabled = !state.trained || state.deploying;
  if (!state.trained) {
    const empty = state.classes.filter((c) => !c.samples.length).map((c) => c.name);
    $("train-result").innerHTML = empty.length
      ? `Record ${empty.join(", ")} first.`
      : `${state.classes.reduce((n, c) => n + c.samples.length, 0)} pictures in ${state.classes.length} classes. Ready to train.`;
  }
}

function progress(id, f) {
  const p = $(id);
  p.hidden = f === null;
  if (f !== null) p.querySelector("span").style.width = `${Math.round(f * 100)}%`;
}

async function train() {
  setStep("train");
  $("train").disabled = true;
  const t0 = performance.now();
  $("train-result").textContent = "Reading the pictures…";
  progress("train-progress", 0.05);
  await featuresReady();
  const name = state.backbone;
  const meta = state.assets.backbones[name];
  const C = meta.channels;
  const labels = state.classes.map((c) => c.name);
  const n = labels.length;
  const trX = [], trY = [], teX = [], teY = [], maps = [];
  state.classes.forEach((c, k) => {
    c.samples.forEach((s, i) => {
      if (c.samples.length >= 4 && i % 4 === 3) { teX.push(s.feat.pooled[0]); teY.push(k); }
      else { for (const p of s.feat.pooled) { trX.push(p); trY.push(k); } maps.push(s.feat.map); }
    });
  });
  const step = Math.max(1, Math.floor(maps.length / 64));
  const P = meta.feat_shape[1] * meta.feat_shape[2];
  const deq = maps.filter((_, i) => i % step === 0).map((f) => {
    const out = new Float32Array(f.length);
    for (let i = 0; i < f.length; i++) out[i] = (f[i] - meta.feat_zp) * meta.feat_scale;
    return out;
  });
  const flat = (rows) => { const x = new Float32Array(rows.length * C); rows.forEach((r, i) => x.set(r, i * C)); return x; };
  const worker = new Worker("train_worker.js", { type: "module" });
  const res = await new Promise((resolve, reject) => {
    worker.onmessage = (e) => {
      if (e.data.progress !== undefined) progress("train-progress", 0.1 + 0.9 * e.data.progress);
      else if (e.data.error) reject(new Error(e.data.error));
      else if (e.data.done) resolve(e.data);
    };
    worker.onerror = (e) => reject(new Error(e.message));
    worker.postMessage({
      x: flat(trX), y: Int32Array.from(trY), N: trX.length, C, n,
      test: flat(teX), testY: Int32Array.from(teY), M: teX.length, maps: deq, P,
    });
  }).finally(() => worker.terminate());
  progress("train-progress", null);
  const secs = (performance.now() - t0) / 1000;
  state.trained = { backbone: name, labels, w: res.w, b: res.b, logitRange: res.logitRange, acc: res.acc };
  $("train-result").innerHTML = res.acc === null
    ? `Trained on ${trX.length} pictures in ${secs.toFixed(1)} s. Record 4 or more per class to see a held-out score.`
    : `Trained in ${secs.toFixed(1)} s. <strong>${Math.round(res.acc * 100)}%</strong> of ${teX.length} held-out pictures right` +
      ` (${trX.length} training pictures with variations).` +
      (res.acc < 0.9 ? " Record more, from more angles, for the classes it confuses." : "");
  renderTrain();
  setStep("deploy");
}

// ---------------------------------------------------------------------------
// Board
// ---------------------------------------------------------------------------

function setBoard(text, ok) {
  const el = $("board-state");
  el.textContent = text;
  el.classList.toggle("ok", !!ok);
}

async function refreshBoard() {
  if (!state.port) { setBoard("Board not connected", false); return; }
  try {
    const i = await board.info(state.port);
    if (i.state === "ready") setBoard(`Board runs ${i.labels.join(", ")} (${Math.round(i.modelBytes / 1024)} KB)`, true);
    else if (i.state === "empty") setBoard("Board connected, no model yet", true);
    else if (i.state === "busy") setBoard("Board is loading its model…", true);
    else setBoard("The board did not answer. Is it running the Teach firmware?", false);
  } catch (err) {
    setBoard(`Board: ${err.message}`, false);
  }
}

async function connect() {
  try {
    state.port = await board.choosePort();
    await refreshBoard();
  } catch (err) {
    if (err.name !== "NotFoundError") setBoard(`Board: ${err.message}`, false);
  }
}

function stepMark(k, cls, text) {
  const li = document.querySelector(`#deploy-steps li[data-k="${k}"]`);
  li.className = cls;
  li.querySelector("span")?.remove();
  if (text) { const s = document.createElement("span"); s.textContent = text; li.append(s); }
}

async function template(name) {
  if (!state.templates[name]) {
    const meta = state.assets.backbones[name];
    state.templates[name] = new Uint8Array(await (await fetch(`assets/${meta.inm}`)).arrayBuffer());
  }
  return state.templates[name];
}

async function deploy() {
  const t = state.trained;
  if (!t) return;
  if (!state.port) {
    try { state.port = await board.choosePort(); } catch { return; }
  }
  state.deploying = true;
  renderTrain();
  ["build", "send", "restart"].forEach((k) => stepMark(k, ""));
  $("deploy-result").textContent = "";
  try {
    let t0 = performance.now();
    stepMark("build", "run");
    const meta = state.assets.backbones[t.backbone];
    const q = headQuant(t.w, t.b, t.logitRange, meta.feat_scale, meta.n_max, meta.channels);
    const inm = patchInm(await template(t.backbone), q.wq, q.bq, meta.feat_scale, q.sW, q.sOut, meta.n_max, meta.channels);
    const data = payload(inm, t.labels, state.assets.max_classes, state.assets.label_max);
    stepMark("build", "done", `${((performance.now() - t0) / 1000).toFixed(1)} s`);

    // Close the camera first: the board restarts afterwards, and a capture
    // session left open across that confuses some hosts' camera service.
    stopCamera();
    stageMsg("Writing the model…");
    t0 = performance.now();
    stepMark("send", "run");
    progress("deploy-progress", 0);
    const r = await board.deploy(state.port, data, (f) => progress("deploy-progress", f));
    progress("deploy-progress", null);
    if (!r.ok) {
      stepMark("send", "fail", r.reply);
      $("deploy-result").textContent = `The board refused the model: ${r.reply}. Its old model is unchanged.`;
      return;
    }
    stepMark("send", "done", `${((performance.now() - t0) / 1000).toFixed(1)} s`);

    t0 = performance.now();
    stepMark("restart", "run");
    stageMsg("The board is restarting…");
    const back = await board.waitReady();
    if (!back) {
      stepMark("restart", "fail", "no answer");
      $("deploy-result").textContent = "The board did not come back within 30 s. Unplug and replug it.";
      return;
    }
    state.port = back.port;
    stepMark("restart", "done", `${((performance.now() - t0) / 1000).toFixed(1)} s`);
    setBoard(`Board runs ${back.labels.join(", ")} (${Math.round(back.modelBytes / 1024)} KB)`, true);
    $("deploy-result").innerHTML = "<strong>On the board.</strong> It classifies the box by itself now: the result is drawn beside the box in the picture.";
    setStep("run");
  } catch (err) {
    $("deploy-result").textContent = `Deploy failed: ${err.message}`;
  } finally {
    progress("deploy-progress", null);
    state.deploying = false;
    renderTrain();
    await startCamera();
  }
}

// ---------------------------------------------------------------------------
// Save and open a set of pictures
// ---------------------------------------------------------------------------

async function rgbaToPng(rgba) {
  const c = new OffscreenCanvas(IN, IN);
  c.getContext("2d").putImageData(new ImageData(new Uint8ClampedArray(rgba), IN, IN), 0, 0);
  const blob = await c.convertToBlob({ type: "image/png" });
  return new Promise((r) => { const fr = new FileReader(); fr.onload = () => r(fr.result); fr.readAsDataURL(blob); });
}

async function saveSet() {
  const doc = { format: "ipro-teach", version: 1, backbone: state.backbone, classes: [] };
  for (const c of state.classes) {
    doc.classes.push({ name: c.name, pictures: await Promise.all(c.samples.map((s) => rgbaToPng(s.rgba))) });
  }
  const a = document.createElement("a");
  a.href = URL.createObjectURL(new Blob([JSON.stringify(doc)], { type: "application/json" }));
  a.download = `${state.classes.map((c) => c.name.toLowerCase().replace(/\s+/g, "_")).join("-")}.teach`;
  a.click();
  setTimeout(() => URL.revokeObjectURL(a.href), 5000);
}

async function openSet(file) {
  const doc = JSON.parse(await file.text());
  if (doc.format !== "ipro-teach") throw new Error("not a Teach picture set");
  state.classes = [];
  for (const c of doc.classes.slice(0, state.assets.max_classes)) {
    const cls = { id: nextId++, name: normaliseLabel(c.name), samples: [] };
    for (const url of c.pictures) {
      const bmp = await createImageBitmap(await (await fetch(url)).blob());
      cropG.drawImage(bmp, 0, 0, IN, IN);
      const rgba = cropG.getImageData(0, 0, IN, IN).data.slice();
      const s = { rgba, thumb: await thumbOf(rgba), feat: null };
      cls.samples.push(s);
      queueFeatures(s);
    }
    state.classes.push(cls);
  }
  if (doc.backbone && state.assets.backbones[doc.backbone]) {
    $("backbone").value = doc.backbone;
    state.backbone = doc.backbone;
  }
  state.selected = 0;
  state.trained = null;
  renderClasses();
}

// ---------------------------------------------------------------------------
// Wiring
// ---------------------------------------------------------------------------

function setStep(step) {
  const order = ["collect", "train", "deploy", "run"];
  const at = order.indexOf(step);
  document.querySelectorAll("#steps li").forEach((li) => {
    const i = order.indexOf(li.dataset.step);
    li.classList.toggle("on", i === at);
    li.classList.toggle("done", i < at);
  });
}

function wireUi() {
  const rec = $("record");
  rec.addEventListener("pointerdown", (e) => { rec.setPointerCapture(e.pointerId); startRecording(); });
  rec.addEventListener("pointerup", stopRecording);
  rec.addEventListener("pointercancel", stopRecording);
  window.addEventListener("keydown", (e) => {
    if (e.code === "Space" && !e.repeat && !/INPUT|SELECT|TEXTAREA/.test(document.activeElement?.tagName)) {
      e.preventDefault();
      startRecording();
    }
  });
  window.addEventListener("keyup", (e) => { if (e.code === "Space") stopRecording(); });
  window.addEventListener("blur", stopRecording);
  $("add-class").addEventListener("click", () => { addClass(); renderClasses(); });
  $("train").addEventListener("click", () => train().catch((err) => {
    progress("train-progress", null);
    $("train-result").textContent = `Training failed: ${err.message}`;
    renderTrain();
  }));
  $("connect").addEventListener("click", connect);
  $("deploy").addEventListener("click", deploy);
  $("camera-select").addEventListener("change", startCamera);
  $("mirror").addEventListener("change", (e) => $("stage").classList.toggle("mirror", e.target.checked));
  $("save-set").addEventListener("click", () => saveSet().catch((err) => notice(`Save failed: ${err.message}`)));
  $("open-set").addEventListener("click", () => $("file-open").click());
  $("file-open").addEventListener("change", (e) => {
    const f = e.target.files[0];
    e.target.value = "";
    if (f) openSet(f).catch((err) => notice(`Could not open ${f.name}: ${err.message}`));
  });
  new ResizeObserver(drawOverlay).observe($("stage"));
  $("video").addEventListener("loadedmetadata", drawOverlay);
  setStep("collect");
}

main().catch((err) => notice(`Start-up failed: ${err.message}`));

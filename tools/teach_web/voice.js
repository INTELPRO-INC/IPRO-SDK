// Voice page: enroll people on the Reef and watch who is speaking.
//
// Everything runs on the board (apps/ai/ipro_reef_teach, voice mode); this page only
// sends commands and draws what comes back over Web Serial (voice_link.js).

import * as link from "./voice_link.js";

const $ = (id) => document.getElementById(id);
const WINDOW_S = 60;
const PALETTE = ["#1f7a50", "#2f6fb5", "#b5481a", "#7a4fb0", "#b08a1f", "#1f8a8a", "#b0306a", "#5c6b2e"];

const PASSAGES = {
  zh: "今天的會議先確認上週的進度，再討論下一季的規劃。請每個人依序說明自己負責的項目、目前遇到的問題，"
    + "以及需要哪些協助。如果有不同的看法，也請直接提出來，我們一起找出最合適的做法。最後會整理成會議紀錄，寄給所有參加的人。",
  en: "Today we will start by going over last week's progress, then talk about the plan for next quarter. "
    + "Each of you, please describe the items you own, any problems you have run into, and what help you need. "
    + "If you see it differently, say so, and we will work out the best way forward together.",
};

const state = {
  link: null,
  ready: null,            // { model, dim, n, id, anon, state, src }
  speakers: [],           // [{ idx, count, name }]
  pending: [],
  live: false,
  enrolling: null,        // { name, seconds, t0 }
  segs: [],               // [{ t0, t1, label }]
  anon: new Map(),        // k -> seconds heard
  renamed: new Map(),     // k -> name given on this page
  order: [],              // timeline rows, by first appearance
};

/* ------------------------------------------------------------------ helpers */

function notice(html) {
  const n = $("notice");
  n.innerHTML = html;
  n.hidden = !html;
}

function setBoard(text, ok) {
  const b = $("board-state");
  b.textContent = text;
  b.classList.toggle("ok", !!ok);
}

/** First n space-separated fields, then the rest of the line (names may have spaces). */
function fields(line, n) {
  const out = [];
  let rest = line;
  for (let i = 0; i < n; i++) {
    const sp = rest.indexOf(" ");
    if (sp < 0) { out.push(rest); rest = ""; break; }
    out.push(rest.slice(0, sp));
    rest = rest.slice(sp + 1);
  }
  out.push(rest);
  return out;
}

const anonLabel = (k) => state.renamed.get(k) || `Speaker ${k + 1}`;

function colorOf(label) {
  let i = state.order.indexOf(label);
  if (i < 0) { state.order.push(label); i = state.order.length - 1; }
  return PALETTE[i % PALETTE.length];
}

function send(line) {
  if (!state.link) { notice("Connect the board first."); return; }
  state.link.send(line).catch((err) => notice(`Sending to the board failed: ${err.message}`));
}

/** LIST, starting the answer afresh (a lost SPK END must not leave old rows). */
function requestList() {
  state.pending = [];
  send("LIST");
}

function refreshButtons() {
  const on = !!state.link && state.ready?.model !== "none";
  $("enroll").disabled = !on || state.live || !!state.enrolling;
  $("live").disabled = !on || !!state.enrolling;
  $("live").textContent = state.live ? "Stop listening" : "Start listening";
  $("live").classList.toggle("recording", state.live);
  $("thr-set").disabled = !state.link;
  // boards older than stranger mode leave the field out of READY
  $("stranger").disabled = !state.link || !state.ready?.mode;
}

/* ------------------------------------------------------------------ board lines */

function onReady(line) {
  const [, model, dim, n, id, anon, st, src, mode] = line.split(" ");
  state.ready = { model, dim: +dim, n: +n, id: +id, anon: +anon, state: st, src, mode };
  $("stranger").checked = mode === "stranger";
  if (model === "none") {
    setBoard("Board connected, no voice model", false);
    notice("The board has no voice model. Flash <code>model_b.spkb</code> to its mfg partition (see the app README).");
  } else {
    setBoard(`Board ready · model ${model}`, true);
    notice("");
  }
  $("thr-id").value = state.ready.id.toFixed(2);
  $("thr-anon").value = state.ready.anon.toFixed(2);
  state.live = st === "live";
  refreshButtons();
}

function onSpk(line) {
  if (line.startsWith("SPK END")) {
    state.speakers = state.pending;
    state.pending = [];
    renderSpeakers();
    return;
  }
  const [, idx, count, name] = fields(line, 3);
  state.pending.push({ idx: +idx, count: +count, name });
}

function onEnroll(line) {
  const [, what] = line.split(" ");
  if (what === "START") {
    const [, , seconds, name] = fields(line, 3);
    state.enrolling = { name, seconds: +seconds, t0: performance.now() };
    $("enroll-run").hidden = false;
    $("enroll-result").textContent = `Recording ${name}…`;
    tickClock();
  } else if (what === "OK") {
    const [, , windows, quality, name] = fields(line, 4);
    endEnroll();
    $("enroll-result").innerHTML = `<strong>${escapeHtml(name)}</strong> saved: ${windows} stretches of speech, consistency ${(+quality).toFixed(2)}.`;
    $("enroll-name").value = "";
    requestList();
  } else if (what === "ERR") {
    const [, , reason, name] = fields(line, 3);
    endEnroll();
    const why = {
      too_quiet: "Too little speech was heard. Read a little louder or closer, and keep reading until the time is up.",
      full: "The board's voice list is full. Remove someone first.",
      no_model: "The board has no voice model.",
      mic: "The board's microphone did not start.",
      cancelled: "Recording was cancelled.",
      refused: "The board refused: check the name, or wait for what it is doing to finish.",
    }[reason] || reason;
    $("enroll-result").innerHTML = `Could not save <strong>${escapeHtml(name)}</strong>: ${why}`;
  }
}

function endEnroll() {
  state.enrolling = null;
  $("enroll-run").hidden = true;
  refreshButtons();
}

function onLevel(line) {
  const [, , db, voiced] = line.split(" ");
  const pct = Math.max(0, Math.min(100, ((+db + 70) / 60) * 100));
  const m = $("meter");
  m.style.width = `${pct}%`;
  m.classList.toggle("voiced", voiced === "1");
  $("now").classList.toggle("voiced", state.live && voiced === "1");
}

function onSeg(line) {
  const [, t0, t1, kind, id, score, tail] = fields(line, 6);
  let label = null;
  if (kind === "id") label = tail;
  if (kind === "anon") {
    label = anonLabel(+id);
    // the board's own count: right even if the page joined mid-session
    if (!state.renamed.has(+id)) state.anon.set(+id, Math.max(+tail || 0, (state.anon.get(+id) || 0) + 1));
    renderAnon();
  }
  state.segs.push({ t0: +t0 / 1000, t1: +t1 / 1000, label, k: kind === "anon" ? +id : -1 });
  if (state.segs.length > 4000) state.segs.splice(0, 1000);
  $("now-name").textContent = label || "—";
  $("now-score").textContent = label ? `${(+score).toFixed(2)}` : "";
  $("now").style.setProperty("--who", label ? colorOf(label) : "var(--line)");
  draw();
}

function onLine(line) {
  const tag = line.split(" ", 1)[0];
  switch (tag) {
    case "READY": onReady(line); break;
    case "SPK": onSpk(line); break;
    case "ENROLL": onEnroll(line); break;
    case "LEVEL": onLevel(line); break;
    case "SEG": onSeg(line); break;
    case "LIVE":
      state.live = line === "LIVE ON";
      if (state.live) { state.segs = []; state.anon.clear(); state.renamed.clear(); renderAnon(); }
      refreshButtons();
      break;
    case "DELETE":
    case "NAME":
      if (line.split(" ")[1] !== "OK") notice(`The board said: ${escapeHtml(line)}`);
      requestList();
      break;
    case "THRESH": {
      const [, a, b] = line.split(" ");
      $("thr-id").value = (+a).toFixed(2);
      $("thr-anon").value = (+b).toFixed(2);
      break;
    }
    case "MODE": {
      const [, mode, a, b] = line.split(" ");
      $("stranger").checked = mode === "STRANGER";
      if (state.ready) state.ready.mode = mode.toLowerCase();
      $("thr-id").value = (+a).toFixed(2);
      $("thr-anon").value = (+b).toFixed(2);
      break;
    }
    case "ERR":
    case "FEED":
    case "EMB":
      notice(`The board said: ${escapeHtml(line)}`);
      if (["MODE", "LIVE"].includes(line.split(" ")[2])) send("HELLO");   // put the switches back
      break;
    default:
      break;
  }
}

/* ------------------------------------------------------------------ rendering */

function escapeHtml(s) {
  return s.replace(/[&<>"']/g, (c) => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" }[c]));
}

function renderSpeakers() {
  const list = $("spk-list");
  list.replaceChildren();
  $("spk-count").textContent = state.speakers.length ? `${state.speakers.length}` : "";
  if (!state.speakers.length) {
    const p = document.createElement("p");
    p.className = "hint";
    p.textContent = "Nobody enrolled yet.";
    list.append(p);
    return;
  }
  for (const s of state.speakers) {
    const row = document.createElement("div");
    row.className = "class spk";
    const dot = document.createElement("span");
    dot.className = "dot";
    dot.style.background = colorOf(s.name);
    const name = document.createElement("strong");
    name.textContent = s.name;
    const info = document.createElement("span");
    info.className = "count";
    info.textContent = s.count > 1 ? `${s.count} stretches` : "";
    const del = document.createElement("button");
    del.className = "ghost";
    del.textContent = "Remove";
    del.addEventListener("click", () => {
      if (confirm(`Remove ${s.name}'s voiceprint from the board?`)) send(`DELETE ${s.name}`);
    });
    const top = document.createElement("div");
    top.className = "class-top";
    top.append(dot, name, info, del);
    row.append(top);
    list.append(row);
  }
}

function renderAnon() {
  const box = $("anon-list");
  box.replaceChildren();
  const ks = [...state.anon.keys()].filter((k) => !state.renamed.has(k) && state.anon.get(k) >= 3);
  $("anon").hidden = ks.length === 0;
  for (const k of ks) {
    const chip = document.createElement("button");
    chip.className = "chip";
    chip.style.setProperty("--who", colorOf(anonLabel(k)));
    chip.textContent = `${anonLabel(k)} · ${state.anon.get(k)} s · Name…`;
    chip.addEventListener("click", () => {
      const name = prompt(`Who is ${anonLabel(k)}? Their voiceprint is taken from what the board just heard.`);
      if (name && name.trim() && !/[\r\n]/.test(name)) {
        state.renamed.set(k, name.trim());
        send(`NAME ${k} ${name.trim()}`);
        for (const s of state.segs) if (s.k === k) s.label = name.trim();
        renderAnon();
        draw();
      }
    });
    box.append(chip);
  }
}

function draw() {
  const c = $("timeline");
  const dpr = window.devicePixelRatio || 1;
  const w = c.clientWidth;
  const rows = state.order.length || 1;
  const rowH = 26, top = 6, left = 110, bottom = 22;
  const h = top + rows * rowH + bottom;
  if (c.width !== Math.round(w * dpr) || c.height !== Math.round(h * dpr)) {
    c.width = Math.round(w * dpr);
    c.height = Math.round(h * dpr);
    c.style.height = `${h}px`;
  }
  const g = c.getContext("2d");
  g.setTransform(dpr, 0, 0, dpr, 0, 0);
  g.clearRect(0, 0, w, h);
  const css = getComputedStyle(document.documentElement);
  const ink = css.getPropertyValue("--muted").trim();
  const line = css.getPropertyValue("--line").trim();
  const end = state.segs.length ? state.segs[state.segs.length - 1].t1 : WINDOW_S;
  const start = Math.max(0, end - WINDOW_S);
  const x = (t) => left + ((t - start) / WINDOW_S) * (w - left - 8);
  g.font = "13px -apple-system, 'Segoe UI', 'PingFang TC', 'Microsoft JhengHei', sans-serif";
  g.textBaseline = "middle";
  state.order.forEach((label, i) => {
    const y = top + i * rowH;
    g.fillStyle = ink;
    g.fillText(label.length > 13 ? `${label.slice(0, 12)}…` : label, 6, y + rowH / 2);
    g.fillStyle = line;
    g.fillRect(left, y + rowH - 1, w - left - 8, 1);
  });
  for (const s of state.segs) {
    if (!s.label || s.t1 < start) continue;
    const i = state.order.indexOf(s.label);
    if (i < 0) continue;
    g.fillStyle = colorOf(s.label);
    g.fillRect(x(s.t0), top + i * rowH + 4, Math.max(1, x(s.t1) - x(s.t0) - 1), rowH - 9);
  }
  g.fillStyle = ink;
  for (let t = Math.ceil(start / 10) * 10; t <= end; t += 10) {
    const m = Math.floor(t / 60), sec = String(Math.floor(t % 60)).padStart(2, "0");
    g.fillText(`${m}:${sec}`, x(t) - 12, h - bottom / 2);
  }
}

function tickClock() {
  if (!state.enrolling) return;
  const left = state.enrolling.seconds + 0.25 - (performance.now() - state.enrolling.t0) / 1000;
  if (left < -30) {                       // the answer never came: do not stay stuck
    endEnroll();
    $("enroll-result").textContent = "The board did not answer. Check the list below, and try again if the name is missing.";
    send("HELLO");
    requestList();
    return;
  }
  $("enroll-clock").textContent = left > 0 ? `${Math.ceil(left)} s left` : "Working out the voiceprint…";
  setTimeout(tickClock, 250);
}

/* ------------------------------------------------------------------ connection */

async function attach(port) {
  if (state.link) return;
  const l = new link.VoiceLink(port, onLine, () => {
    state.link = null;
    state.live = false;
    endEnroll();
    setBoard("Board not connected", false);
    refreshButtons();
  });
  try {
    await l.open();
  } catch (err) {
    notice(`Could not open the board's port (${escapeHtml(err.message)}). Close other programs using it and try again.`);
    return;
  }
  state.link = l;
  setBoard("Board connected", true);
  send("");                               // ends a line a previous session left half-sent
  send("HELLO");
  requestList();
  refreshButtons();
}

async function connect() {
  if (!link.supported()) {
    notice("This browser has no Web Serial. Use Chrome or Edge.");
    return;
  }
  try {
    await attach(await link.choosePort());
  } catch (err) {
    if (err.name !== "NotFoundError") notice(`Connect failed: ${escapeHtml(err.message)}`);
  }
}

/* voice.html?demo plays test/voice_demo.log (lines a board sent) at 4x, for
 * checking the page without a board. test/ is not in the release. */
async function demo() {
  const text = await (await fetch("test/voice_demo.log")).text();
  setBoard("Demo: replaying a recording", true);
  state.link = { send: () => Promise.resolve() };
  for (const line of text.split("\n").filter(Boolean)) {
    onLine(line);
    if (line.startsWith("SEG ") || line.startsWith("LEVEL ")) {
      await new Promise((r) => setTimeout(r, line.startsWith("SEG ") ? 0 : 62));
    }
  }
}

function main() {
  const lang = (navigator.language || "en").toLowerCase().startsWith("zh") ? "zh" : "en";
  $("passage").textContent = PASSAGES[lang];
  $("connect").addEventListener("click", connect);
  $("enroll").addEventListener("click", () => {
    const name = $("enroll-name").value.trim();
    if (!name || /[\r\n]/.test(name)) {
      $("enroll-result").textContent = "Enter a name first.";
      return;
    }
    if (state.speakers.some((s) => s.name === name) &&
        !confirm(`${name} is already enrolled. Record again and replace the voiceprint?`)) return;
    send(`ENROLL ${$("enroll-seconds").value} ${name}`);
  });
  $("live").addEventListener("click", () => send(state.live ? "LIVE OFF" : "LIVE ON"));
  $("thr-set").addEventListener("click", () => send(`THRESH ${$("thr-id").value} ${$("thr-anon").value}`));
  $("stranger").addEventListener("change", (e) => send(`MODE ${e.target.checked ? "STRANGER" : "NORMAL"}`));
  window.addEventListener("resize", draw);
  draw();
  renderSpeakers();
  refreshButtons();
  if (new URLSearchParams(location.search).has("demo")) {
    demo().catch((err) => notice(`Demo failed: ${escapeHtml(err.message)}`));
    return;
  }
  if (!link.supported()) {
    notice("This browser has no Web Serial. Use Chrome or Edge.");
    return;
  }
  navigator.serial.addEventListener("connect", (e) => {
    const i = e.target.getInfo?.() || {};
    if (i.usbVendorId === link.VID && i.usbProductId === link.PID) attach(e.target);
  });
  link.knownPort().then((p) => p && attach(p));
}

main();

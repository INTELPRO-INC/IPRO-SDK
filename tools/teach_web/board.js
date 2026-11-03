// The Reef over Web Serial: the Teach firmware's CDC-ACM port (VID 0x3896, PID 0x0705).
//
// Wire format (apps/ai/ipro_reef_teach/image/uvc_teach_link.h), same as IPRO AI Studio's:
//   host -> board   "TDEP" | u32 size | u32 crc32 | u32 0, then the payload
//                   "TINF" | 12 zero bytes
//   board -> host   TDEP OK <size> <crc32>   (written; the board reboots)
//                   TDEP ERR <reason>
//                   TINF <bytes> <classes> <label>,<label>,...
//                   TINF BUSY                (model still loading)
// The board stages the whole payload and checks its CRC before it erases
// anything, so a refused or cut-short deploy leaves the old model in place.

export const VID = 0x3896;
export const PID = 0x0705;
const BAUD = 115200;            // ignored by CDC, required by the API
const CHUNK = 16 * 1024;        // progress granularity

const CRC_TABLE = (() => {
  const t = new Uint32Array(256);
  for (let n = 0; n < 256; n++) {
    let c = n;
    for (let k = 0; k < 8; k++) c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1;
    t[n] = c >>> 0;
  }
  return t;
})();

/** zlib's crc32. */
export function crc32(data) {
  let c = 0xffffffff;
  for (let i = 0; i < data.length; i++) c = CRC_TABLE[(c ^ data[i]) & 0xff] ^ (c >>> 8);
  return (c ^ 0xffffffff) >>> 0;
}

function header(tag, a = 0, b = 0) {
  const h = new Uint8Array(16);
  for (let i = 0; i < 4; i++) h[i] = tag.charCodeAt(i);
  const dv = new DataView(h.buffer);
  dv.setUint32(4, a, true);
  dv.setUint32(8, b, true);
  return h;
}

export const supported = () => typeof navigator !== "undefined" && "serial" in navigator;

const isTeachPort = (p) => {
  const i = p.getInfo();
  return i.usbVendorId === VID && i.usbProductId === PID;
};

/** A port the user already allowed, if the board is plugged in. */
export async function knownPort() {
  if (!supported()) return null;
  const ports = await navigator.serial.getPorts();
  return ports.find(isTeachPort) || null;
}

/** Ask the user to pick the board (needs a click). */
export async function choosePort() {
  return navigator.serial.requestPort({ filters: [{ usbVendorId: VID, usbProductId: PID }] });
}

/** Open, run fn(io), close. io.write(bytes), io.readLine(timeoutMs). */
async function session(port, fn) {
  await port.open({ baudRate: BAUD, bufferSize: 64 * 1024 });
  const writer = port.writable.getWriter();
  const reader = port.readable.getReader();
  let buf = "";
  const dec = new TextDecoder();
  const io = {
    write: (bytes) => writer.write(bytes),
    async readLine(timeoutMs) {
      const deadline = performance.now() + timeoutMs;
      for (;;) {
        const nl = buf.indexOf("\n");
        if (nl >= 0) {
          const line = buf.slice(0, nl).trim();
          buf = buf.slice(nl + 1);
          if (line) return line;
          continue;
        }
        const left = deadline - performance.now();
        if (left <= 0) return null;
        let timer;
        const r = await Promise.race([
          reader.read(),
          new Promise((res) => { timer = setTimeout(() => res({ timeout: true }), left); }),
        ]);
        clearTimeout(timer);
        if (r.timeout) return null;
        if (r.done) return null;
        buf += dec.decode(r.value, { stream: true });
      }
    },
  };
  try {
    return await fn(io);
  } finally {
    try { await reader.cancel(); } catch { /* port gone */ }
    reader.releaseLock();
    writer.releaseLock();
    try { await port.close(); } catch { /* port gone */ }
  }
}

/** What the board runs: {state: ready|busy|empty|silent, modelBytes, labels}. */
export async function info(port, timeoutMs = 3000) {
  return session(port, async (io) => {
    await io.write(header("TINF"));
    const line = await io.readLine(timeoutMs);
    if (!line || !line.startsWith("TINF")) return { state: "silent", modelBytes: 0, labels: [] };
    const parts = line.split(" ");
    if (parts[1] === "BUSY") return { state: "busy", modelBytes: 0, labels: [] };
    const size = parseInt(parts[1], 10);
    if (!Number.isFinite(size)) return { state: "silent", modelBytes: 0, labels: [] };
    if (size === 0) return { state: "empty", modelBytes: 0, labels: [] };
    const labels = parts.slice(3).join(" ").split(",").filter(Boolean);
    return { state: "ready", modelBytes: size, labels };
  });
}

/** Send the payload; resolves once the board has checked and written it. */
export async function deploy(port, payload, onProgress, timeoutMs = 30000) {
  const crc = crc32(payload);
  return session(port, async (io) => {
    const t0 = performance.now();
    await io.write(header("TDEP", payload.length, crc));
    for (let off = 0; off < payload.length; off += CHUNK) {
      await io.write(payload.subarray(off, Math.min(off + CHUNK, payload.length)));
      if (onProgress) onProgress(Math.min(off + CHUNK, payload.length) / payload.length);
    }
    const t1 = performance.now();
    const reply = (await io.readLine(timeoutMs)) || "no reply";
    const t2 = performance.now();
    return { ok: reply.startsWith("TDEP OK"), reply, sendMs: t1 - t0, writeMs: t2 - t1 };
  });
}

/** After a deploy: wait for the board to reboot, re-enumerate and load its model. */
export async function waitReady(timeoutMs = 30000, settleMs = 1500) {
  await new Promise((r) => setTimeout(r, settleMs));      // it detaches ~0.8 s after replying
  const end = performance.now() + timeoutMs;
  while (performance.now() < end) {
    const port = await knownPort();
    if (port) {
      try {
        const got = await info(port, 1500);
        if (got.state === "ready" || got.state === "empty") return { port, ...got };
      } catch { /* still enumerating */ }
    }
    await new Promise((r) => setTimeout(r, 500));
  }
  return null;
}

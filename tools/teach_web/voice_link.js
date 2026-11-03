// The Reef running apps/ai/ipro_reef_teach in voice mode, over Web Serial: its CDC-ACM port
// on the board's USB-C (VID 0x3896, PID 0x0720). Text lines both ways; the
// commands and replies are listed in that app's README.md.

export const VID = 0x3896;
export const PID = 0x0720;

export const supported = () => typeof navigator !== "undefined" && "serial" in navigator;

const isVoicePort = (p) => {
  const i = p.getInfo();
  return i.usbVendorId === VID && i.usbProductId === PID;
};

/** A port the user already allowed, if the board is plugged in. */
export async function knownPort() {
  if (!supported()) return null;
  const ports = await navigator.serial.getPorts();
  return ports.find(isVoicePort) || null;
}

/** Ask the user to pick the board (needs a click). */
export async function choosePort() {
  return navigator.serial.requestPort({ filters: [{ usbVendorId: VID, usbProductId: PID }] });
}

export class VoiceLink {
  constructor(port, onLine, onClose) {
    this.port = port;
    this.onLine = onLine;
    this.onClose = onClose;
    this.writer = null;
    this.reader = null;
    this.encoder = new TextEncoder();
  }

  async open() {
    await this.port.open({ baudRate: 115200, bufferSize: 16 * 1024 });
    // The board only talks while DTR is up, so a closed page never stalls it.
    await this.port.setSignals({ dataTerminalReady: true });
    this.writer = this.port.writable.getWriter();
    this.loop();
  }

  async loop() {
    const decoder = new TextDecoder();
    let buf = "";
    this.reader = this.port.readable.getReader();
    try {
      for (;;) {
        const { value, done } = await this.reader.read();
        if (done) break;
        buf += decoder.decode(value, { stream: true });
        let nl;
        while ((nl = buf.indexOf("\n")) >= 0) {
          const line = buf.slice(0, nl).replace(/\r$/, "");
          buf = buf.slice(nl + 1);
          if (line) this.onLine(line);
        }
      }
    } catch (err) {
      // unplugged or closed
    } finally {
      this.reader.releaseLock();
      this.onClose?.();
    }
  }

  send(line) {
    return this.writer.write(this.encoder.encode(line + "\n"));
  }

  async close() {
    try { await this.reader?.cancel(); } catch (err) { /* already gone */ }
    try { this.writer?.releaseLock(); } catch (err) { /* already gone */ }
    try { await this.port.close(); } catch (err) { /* already gone */ }
  }
}

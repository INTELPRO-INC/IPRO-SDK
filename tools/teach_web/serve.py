#!/usr/bin/env python3
#======================================================================
# Copyright (c) 2026 IntelPro Technology Co., Ltd.
# All rights reserved.
#
# This software and associated documentation files (the "Software") are
# proprietary and confidential to IntelPro Technology Co., Ltd..
#
# Access to and use of this Software is limited exclusively to authorized 
# customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
# valid license or non-disclosure agreement.
#
# Unauthorized copying, modification, distribution, or reverse engineering 
# of this Software, via any medium, is strictly prohibited without 
# prior written permission from IntelPro Technology Co., Ltd..
#
# SPDX-License-Identifier: Proprietary
#======================================================================

"""Serve the IPRO AI Teach web page on this computer and open it in the browser.

The page needs a secure context for the camera and Web Serial, which a page
from http://localhost has and a file opened from disk does not. Nothing leaves
the computer: the server listens on 127.0.0.1 only.

    python3 tools/teach_web/serve.py                  # http://localhost:8780, opens the browser
    python3 tools/teach_web/serve.py --fetch-runtime  # once, for use without internet

The page runs the backbone with LiteRT.js. It loads it from vendor/litertjs
when present, otherwise from the jsDelivr CDN; --fetch-runtime puts a pinned
copy (checked against the npm integrity hash) into vendor/. Python 3.8+,
standard library only.
"""
from __future__ import annotations

import argparse
import base64
import functools
import hashlib
import http.server
import io
import sys
import tarfile
import urllib.request
import webbrowser
from pathlib import Path

HERE = Path(__file__).resolve().parent
VENDOR = HERE / "vendor" / "litertjs"

# @litertjs/core and its one dependency, pinned. The page's import map expects
# exactly these paths (index.html).
RUNTIME = {
    "core": ("https://registry.npmjs.org/@litertjs/core/-/core-2.6.0.tgz",
             "sha512-TH6/5zLq57YzyZH6q+vO8be1SvTH76uDFe+CiL0wWtRNTUDhc2GAWm2YCohhnyt4fPvefSNztg5PKFZGAYL0cw==",
             ["package/package.json", "package/dist/index.js",
              "package/wasm/litert_wasm_internal.js", "package/wasm/litert_wasm_internal.wasm",
              "package/wasm/litert_wasm_compat_internal.js", "package/wasm/litert_wasm_compat_internal.wasm"]),
    "wasm-utils": ("https://registry.npmjs.org/@litertjs/wasm-utils/-/wasm-utils-2.6.0.tgz",
                   "sha512-rxARtl8owf+aPV2TsMiAClYDBaEDemML3PjrzBxIpGCvv6POswR7068Kt0sIkrK5XvKkY2Z+JLcm3a9CHmvuUg==",
                   ["package/package.json", "package/dist/index.js"]),
}


def fetch_runtime() -> None:
    for name, (url, integrity, members) in RUNTIME.items():
        print(f"downloading {url}")
        data = urllib.request.urlopen(url, timeout=120).read()
        algo, want = integrity.split("-", 1)
        got = base64.b64encode(hashlib.new(algo, data).digest()).decode()
        if got != want:
            sys.exit(f"{url}: integrity mismatch, refusing to install it")
        with tarfile.open(fileobj=io.BytesIO(data), mode="r:gz") as tar:
            for m in members:
                dst = VENDOR / name / m[len("package/"):]
                dst.parent.mkdir(parents=True, exist_ok=True)
                dst.write_bytes(tar.extractfile(m).read())
    print(f"runtime installed in {VENDOR}")


class Handler(http.server.SimpleHTTPRequestHandler):
    extensions_map = {**http.server.SimpleHTTPRequestHandler.extensions_map,
                      ".wasm": "application/wasm", ".js": "text/javascript", ".mjs": "text/javascript",
                      ".tflite": "application/octet-stream", ".inm": "application/octet-stream"}

    def end_headers(self):
        self.send_header("Cache-Control", "no-cache")
        super().end_headers()

    def log_message(self, fmt, *args):         # quiet: one line per error only
        if args and str(args[1]).startswith(("4", "5")):
            super().log_message(fmt, *args)


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--port", type=int, default=8780)
    ap.add_argument("--no-browser", action="store_true")
    ap.add_argument("--fetch-runtime", action="store_true", help="download LiteRT.js into vendor/ and exit")
    args = ap.parse_args()
    if args.fetch_runtime:
        fetch_runtime()
        return
    handler = functools.partial(Handler, directory=str(HERE))
    with http.server.ThreadingHTTPServer(("127.0.0.1", args.port), handler) as srv:
        url = f"http://localhost:{args.port}/"
        print(f"IPRO AI Teach: {url}  (Chrome or Edge; Ctrl+C to stop)")
        if not args.no_browser:
            webbrowser.open(url)
        try:
            srv.serve_forever()
        except KeyboardInterrupt:
            pass


if __name__ == "__main__":
    main()

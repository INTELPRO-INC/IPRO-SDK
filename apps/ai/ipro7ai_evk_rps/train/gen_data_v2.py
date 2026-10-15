#!/usr/bin/env python3
"""
Copyright (c) 2026 IntelPro Technology Co., Ltd.
All rights reserved.

This software and associated documentation files (the "Software") are
proprietary and confidential to IntelPro Technology Co., Ltd..

Access to and use of this Software is limited exclusively to authorized 
customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
valid license or non-disclosure agreement.

Unauthorized copying, modification, distribution, or reverse engineering 
of this Software, via any medium, is strictly prohibited without 
prior written permission from IntelPro Technology Co., Ltd..

SPDX-License-Identifier: Proprietary
"""

"""Synthesise rock/paper/scissors/none training crops for the IPRO7AI NPU classifier.

Source hands: the TensorFlow rock_paper_scissors CGI set (300x300, white background).
Backgrounds: tf flower_photos crops + procedural (solid / gradient / noise) so the
model sees the hand on something other than white.

Output: npz with uint8 images (N,128,128,3) RGB and int labels
    0 rock  1 paper  2 scissors  3 none
"""
import glob, os, random, sys
import numpy as np
from PIL import Image, ImageFilter, ImageEnhance

ROOT = os.path.expanduser("~/Work/rps/data")
OUT = os.path.expanduser("~/Work/rps/data/rps128_v2.npz")
SZ = 128
CLASSES = ["rock", "paper", "scissors"]
N_TRAIN_PER_CLASS = 4000
N_TRAIN_ROCK_EXTRA = 3000        # v2: the fist is the class the board misses
N_TRAIN_NONE = 2400              # v2: a less greedy "no hand" class
N_VAL_PER_CLASS = 500
N_VAL_NONE = 400
SEED = 1337

rng = random.Random(SEED)
nrng = np.random.default_rng(SEED)


def load_hand(path):
    """RGBA hand cut out of its white background, cropped to its bbox."""
    im = Image.open(path).convert("RGB")
    a = np.asarray(im)
    mask = (a.min(axis=2) < 225).astype(np.uint8) * 255          # shadows are >225
    ys, xs = np.where(mask > 0)
    if len(xs) < 200:
        return None
    x0, x1, y0, y1 = xs.min(), xs.max() + 1, ys.min(), ys.max() + 1
    rgba = np.dstack([a, mask])[y0:y1, x0:x1]
    hand = Image.fromarray(rgba, "RGBA")
    # feather the edge a little so the composite is not a hard cut-out
    alpha = hand.split()[3].filter(ImageFilter.GaussianBlur(0.8))
    hand.putalpha(alpha)
    return hand


def load_hands(split):
    hands = {}
    for c in CLASSES:
        paths = sorted(glob.glob(os.path.join(ROOT, split, c, "*.png")))
        hs = [h for h in (load_hand(p) for p in paths) if h is not None]
        print(f"  {split}/{c}: {len(hs)} hands", flush=True)
        hands[c] = hs
    return hands


def load_backgrounds():
    paths = sorted(glob.glob(os.path.join(ROOT, "flower_photos", "*", "*.jpg")))
    rng.shuffle(paths)
    bgs = []
    for p in paths[:1400]:
        try:
            im = Image.open(p).convert("RGB")
        except Exception:
            continue
        s = 200 / min(im.size)
        im = im.resize((max(SZ, int(im.width * s)), max(SZ, int(im.height * s))))
        bgs.append(im)
    print(f"  backgrounds: {len(bgs)} photos", flush=True)
    return bgs


def procedural_bg():
    kind = rng.random()
    if kind < 0.45:                                  # flat colour (walls, desks)
        # v2: bias towards the greys / beiges / dark woods of a desk and wall
        base = rng.randint(25, 200)
        c = tuple(int(np.clip(base + nrng.integers(-25, 25), 0, 255)) for _ in range(3))
        im = Image.new("RGB", (SZ, SZ), c)
        if rng.random() < 0.5:                       # a monitor / table edge
            from PIL import ImageDraw
            d = ImageDraw.Draw(im)
            c2 = tuple(int(np.clip(v + rng.randint(-80, 80), 0, 255)) for v in c)
            if rng.random() < 0.5:
                d.rectangle([0, rng.randint(0, SZ), SZ, SZ], fill=c2)
            else:
                d.rectangle([rng.randint(0, SZ), 0, SZ, SZ], fill=c2)
    elif kind < 0.75:                                # gradient
        a = nrng.integers(0, 255, 3).astype(np.float32)
        b = nrng.integers(0, 255, 3).astype(np.float32)
        t = np.linspace(0, 1, SZ, dtype=np.float32)
        g = a[None, None, :] * (1 - t)[:, None, None] + b[None, None, :] * t[:, None, None]
        g = np.repeat(g, SZ, axis=1)
        if rng.random() < 0.5:
            g = g.transpose(1, 0, 2)
        im = Image.fromarray(g.astype(np.uint8))
    else:                                            # low-frequency noise
        n = nrng.integers(0, 255, (8, 8, 3)).astype(np.uint8)
        im = Image.fromarray(n).resize((SZ, SZ), Image.BILINEAR)
    return im


def photo_bg(bgs):
    im = rng.choice(bgs)
    x = rng.randint(0, im.width - SZ)
    y = rng.randint(0, im.height - SZ)
    crop = im.crop((x, y, x + SZ, y + SZ))
    if rng.random() < 0.6:
        crop = crop.filter(ImageFilter.GaussianBlur(rng.uniform(0.5, 3.0)))
    # v2: indoor look - darker and less saturated than a flower photo
    crop = ImageEnhance.Color(crop).enhance(rng.uniform(0.2, 0.8))
    crop = ImageEnhance.Brightness(crop).enhance(rng.uniform(0.4, 0.9))
    return crop


def background(bgs):
    return photo_bg(bgs) if rng.random() < 0.65 else procedural_bg()


def jitter_color(im, lo=0.65, hi=1.35):
    im = ImageEnhance.Brightness(im).enhance(rng.uniform(lo, hi))
    im = ImageEnhance.Contrast(im).enhance(rng.uniform(0.75, 1.25))
    im = ImageEnhance.Color(im).enhance(rng.uniform(0.7, 1.3))
    return im


def composite(hand, bg):
    """Hand pasted onto a 128x128 background, roughly filling the crop."""
    canvas = bg.copy()
    # v2: hand height 60..125 % of the crop - a fist held up to the camera fills
    # the box and is often cut off at the wrist
    target_h = rng.uniform(0.60, 1.25) * SZ
    s = target_h / hand.height
    h = hand.resize((max(8, int(hand.width * s)), max(8, int(hand.height * s))),
                    Image.BILINEAR)
    if rng.random() < 0.5:
        h = h.transpose(Image.FLIP_LEFT_RIGHT)
    h = h.rotate(rng.uniform(-45, 45), resample=Image.BILINEAR, expand=True)
    rgb = jitter_color(h.convert("RGB"))
    h = Image.merge("RGBA", (*rgb.split(), h.split()[3]))
    # place: centred with jitter; the wrist usually comes from the bottom edge
    cx = SZ / 2 + rng.uniform(-0.18, 0.18) * SZ
    cy = SZ / 2 + rng.uniform(-0.10, 0.22) * SZ
    x = int(cx - h.width / 2)
    y = int(cy - h.height / 2)
    canvas.paste(h, (x, y), h)
    return canvas


def finish(im):
    """Whole-image degradations that the camera path will add."""
    im = jitter_color(im, 0.7, 1.25)
    if rng.random() < 0.4:
        im = im.filter(ImageFilter.GaussianBlur(rng.uniform(0.3, 1.5)))
    a = np.asarray(im).astype(np.float32)
    # v2: ISP-ish - gamma, a warm/cool cast, a vignette, sensor noise
    if rng.random() < 0.6:
        g = rng.uniform(0.6, 1.6)
        a = 255.0 * (a / 255.0) ** g
    if rng.random() < 0.5:
        a *= np.array([rng.uniform(0.85, 1.15), 1.0, rng.uniform(0.85, 1.15)], np.float32)
    if rng.random() < 0.4:
        yy, xx = np.mgrid[0:SZ, 0:SZ].astype(np.float32)
        r = np.sqrt((xx - SZ / 2) ** 2 + (yy - SZ / 2) ** 2) / (SZ / 2)
        a *= (1.0 - rng.uniform(0.1, 0.5) * r ** 2)[:, :, None]
    if rng.random() < 0.6:
        a += nrng.normal(0, rng.uniform(2, 10), a.shape)
    return np.clip(a, 0, 255).astype(np.uint8)


def make_split(hands, bgs, n_per_class, n_none, rock_extra=0):
    xs, ys = [], []
    for ci, c in enumerate(CLASSES):
        n = n_per_class + (rock_extra if c == "rock" else 0)
        for _ in range(n):
            xs.append(finish(composite(rng.choice(hands[c]), background(bgs))))
            ys.append(ci)
    for _ in range(n_none):
        xs.append(finish(background(bgs)))
        ys.append(3)
    idx = list(range(len(xs)))
    rng.shuffle(idx)
    return np.stack([xs[i] for i in idx]), np.array([ys[i] for i in idx], dtype=np.int64)


def main():
    print("loading hands", flush=True)
    train_hands = load_hands("rps")
    val_hands = load_hands("rps-test-set")
    bgs = load_backgrounds()
    print("compositing train", flush=True)
    xtr, ytr = make_split(train_hands, bgs, N_TRAIN_PER_CLASS, N_TRAIN_NONE, N_TRAIN_ROCK_EXTRA)
    print("compositing val", flush=True)
    xva, yva = make_split(val_hands, bgs, N_VAL_PER_CLASS, N_VAL_NONE)
    np.savez_compressed(OUT, x_train=xtr, y_train=ytr, x_val=xva, y_val=yva)
    print(f"saved {OUT}: train {xtr.shape} val {xva.shape}")
    # a contact sheet to eyeball
    sheet = Image.new("RGB", (SZ * 8, SZ * 4))
    for i in range(32):
        sheet.paste(Image.fromarray(xtr[i]), ((i % 8) * SZ, (i // 8) * SZ))
    sheet.save(os.path.expanduser("~/Work/rps/data/contact_sheet_v2.png"))
    print("labels of the sheet:", ytr[:32].tolist())


if __name__ == "__main__":
    main()

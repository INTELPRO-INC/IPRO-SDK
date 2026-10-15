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

"""Build the v3 rock/paper/scissors/none set from HaGRID (real people, real
webcams, indoor) plus a slice of the CGI synthetic set.

HaGRID sample: 512x384 frames, normalised [x, y, w, h] hand boxes, one json per
gesture. Class mapping:
    rock     <- fist
    paper    <- palm, stop, stop_inverted
    scissors <- peace, peace_inverted, two_up, two_up_inverted
    none     <- no_gesture hands, every other gesture (call, like, ok, one,
                three, HaGRID's "rock" = the horns sign, ...), and random
                background / face / torso crops that contain no hand.

Crops imitate the board's play box: a square around the hand, side = the
larger box side x U(1.1, 3.0) (hand fills ~33-90 %), centre jitter, clipped
to the frame, resized to 128. Validation split is by user_id so no person is
in both splits.
"""
import glob, hashlib, json, os, random
import numpy as np
from PIL import Image

ROOT = os.path.expanduser("~/Work/rps/data/hagrid/hagrid-sample-30k-384p")
SYN = os.path.expanduser("~/Work/rps/data/rps128_v2.npz")
OUT = os.path.expanduser("~/Work/rps/data/rps128_v3.npz")
SZ = 128
SEED = 2024
rng = random.Random(SEED)
nrng = np.random.default_rng(SEED)

MAP = {
    "fist": 0,
    "palm": 1, "stop": 1, "stop_inverted": 1,
    "peace": 2, "peace_inverted": 2, "two_up": 2, "two_up_inverted": 2,
}
# per-image crop counts by our class (gesture classes get more views)
CROPS = {0: 3, 1: 2, 2: 2, 3: 1}
CAP_TRAIN = {0: 6000, 1: 6500, 2: 6500, 3: 7500}   # cap per class after generation
SYN_TAKE = 5000                                    # CGI images mixed in (train only)
VAL_FRAC = 0.10


def load_anns():
    """{image_id: (folder, [(label, [x,y,w,h]), ...], user_id)} for images present."""
    anns = {}
    for jp in sorted(glob.glob(os.path.join(ROOT, "ann_train_val", "*.json"))):
        gesture = os.path.splitext(os.path.basename(jp))[0]
        folder = os.path.join(ROOT, "hagrid_30k", f"train_val_{gesture}")
        if not os.path.isdir(folder):
            continue
        present = {os.path.splitext(f)[0] for f in os.listdir(folder) if f.endswith(".jpg")}
        d = json.load(open(jp))
        for k, v in d.items():
            if k in present:
                anns[k] = (folder, list(zip(v["labels"], v["bboxes"])), v.get("user_id", k))
    print(f"annotated images present: {len(anns)}", flush=True)
    return anns


def other_hand_inside(x0, y0, side, boxes, this_box, this_cls, W, H):
    """True if a hand of a DIFFERENT class has >20 % of its area in the window -
    that crop would carry the wrong label."""
    for (label, b) in boxes:
        if b is this_box or MAP.get(label, 3) == this_cls:
            continue
        bx, by, bw, bh = b[0] * W, b[1] * H, b[2] * W, b[3] * H
        ix = max(0, min(x0 + side, bx + bw) - max(x0, bx))
        iy = max(0, min(y0 + side, by + bh) - max(y0, by))
        if bw * bh > 0 and ix * iy > 0.2 * bw * bh:
            return True
    return False


def square_crop(im, box, scale, boxes, cls, jitter=0.15):
    """Square crop around a normalised [x,y,w,h] box, clipped inside the image.
    None if a hand of another class would be in the picture."""
    W, H = im.size
    bx, by, bw, bh = box[0] * W, box[1] * H, box[2] * W, box[3] * H
    side = max(bw, bh) * scale
    side = min(side, min(W, H))
    cx = bx + bw / 2 + rng.uniform(-jitter, jitter) * side
    cy = by + bh / 2 + rng.uniform(-jitter, jitter) * side
    x0 = int(np.clip(cx - side / 2, 0, W - side))
    y0 = int(np.clip(cy - side / 2, 0, H - side))
    if other_hand_inside(x0, y0, side, boxes, box, cls, W, H):
        return None
    return im.crop((x0, y0, x0 + int(side), y0 + int(side))).resize((SZ, SZ), Image.BILINEAR)


def iou_free(x0, y0, side, boxes, W, H):
    for (_, b) in boxes:
        bx, by, bw, bh = b[0] * W, b[1] * H, b[2] * W, b[3] * H
        ix = max(0, min(x0 + side, bx + bw) - max(x0, bx))
        iy = max(0, min(y0 + side, by + bh) - max(y0, by))
        if ix * iy > 0.05 * side * side:
            return False
    return True


def background_crop(im, boxes):
    W, H = im.size
    for _ in range(12):
        side = rng.randint(110, 300)
        x0 = rng.randint(0, W - side)
        y0 = rng.randint(0, H - side)
        if iou_free(x0, y0, side, boxes, W, H):
            return im.crop((x0, y0, x0 + side, y0 + side)).resize((SZ, SZ), Image.BILINEAR)
    return None


def main():
    anns = load_anns()
    xs = {0: [], 1: [], 2: [], 3: []}
    vs = {0: [], 1: [], 2: [], 3: []}
    n_img = 0
    for k, (folder, boxes, user) in anns.items():
        im = Image.open(os.path.join(folder, k + ".jpg")).convert("RGB")
        is_val = int(hashlib.md5(user.encode()).hexdigest(), 16) % 100 < VAL_FRAC * 100
        dst = vs if is_val else xs
        for (label, box) in boxes:
            cls = MAP.get(label, 3)
            n = CROPS[cls] if not is_val else 1
            for _ in range(n):
                c = square_crop(im, box, rng.uniform(1.1, 3.0), boxes, cls)
                if c is not None:
                    dst[cls].append(np.asarray(c))
        # background / face / torso: one per third image
        if rng.random() < 0.34:
            bg = background_crop(im, boxes)
            if bg is not None:
                dst[3].append(np.asarray(bg))
        n_img += 1
        if n_img % 5000 == 0:
            print(f"  {n_img} images", flush=True)

    for c in range(4):
        rng.shuffle(xs[c])
        xs[c] = xs[c][:CAP_TRAIN[c]]
        print(f"class {c}: train {len(xs[c])}  val {len(vs[c])}", flush=True)

    x_train = np.stack([a for c in range(4) for a in xs[c]])
    y_train = np.array([c for c in range(4) for _ in xs[c]], dtype=np.int64)
    x_val = np.stack([a for c in range(4) for a in vs[c]])
    y_val = np.array([c for c in range(4) for _ in vs[c]], dtype=np.int64)

    if os.path.exists(SYN):
        s = np.load(SYN)
        idx = nrng.choice(len(s["x_train"]), min(SYN_TAKE, len(s["x_train"])), replace=False)
        x_train = np.concatenate([x_train, s["x_train"][idx]])
        y_train = np.concatenate([y_train, s["y_train"][idx]])
        print(f"mixed in {len(idx)} CGI synthetic images", flush=True)

    p = nrng.permutation(len(x_train))
    x_train, y_train = x_train[p], y_train[p]
    np.savez_compressed(OUT, x_train=x_train, y_train=y_train, x_val=x_val, y_val=y_val)
    print(f"saved {OUT}: train {x_train.shape} {np.bincount(y_train)}  val {x_val.shape} {np.bincount(y_val)}")

    sheet = Image.new("RGB", (SZ * 8, SZ * 4))
    for i in range(32):
        sheet.paste(Image.fromarray(x_train[i]), ((i % 8) * SZ, (i // 8) * SZ))
    sheet.save(os.path.expanduser("~/Work/rps/data/contact_sheet_v3.png"))
    print("sheet labels:", y_train[:32].tolist())


if __name__ == "__main__":
    main()

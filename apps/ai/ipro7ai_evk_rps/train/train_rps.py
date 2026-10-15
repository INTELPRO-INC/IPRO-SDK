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

"""Train the rock/paper/scissors/none classifier for the IPRO7AI NPU and export
a full-integer int8 TFLite.

Architecture rules (from the IPRO7AI NPU operator subset and the CIFAR-10 model
card in ipro_ai_studio/examples):
  - 3x3 convolutions only, stride 1 or 2, SAME/VALID padding, fused ReLU
  - BatchNorm (folds into the conv), 2x2 max-pool
  - NO dense layers, NO global-average-pool, NO depthwise conv: those fall back
    to the CPU and the CIFAR card shows one FC layer eating 62 % of the time.
    The head is a 3x3 stride-2 VALID conv that collapses 4x4 -> 1x1, then a
    1x1 conv to the 4 logits. Softmax/argmax happen on the device CPU.
  - Input: float [0,1] -> int8 scale 1/255 zp -128 after quantisation, the
    same convention as person_pet_detect / big_a, so the device feeds the raw
    ISP frame through the NPU's external-input port.

Outputs (in ~/Work/rps/out):
  rps_float.keras, rps_int8.tflite, numbers.json, samples/<class>.{png,bin,expected.txt}
"""
import json, os, time
import numpy as np
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers

DATA = os.path.expanduser("~/Work/rps/data/rps128.npz")
OUT = os.path.expanduser("~/Work/rps/out")
CLASSES = ["rock", "paper", "scissors", "none"]
SZ = 128
EPOCHS = int(os.environ.get("EPOCHS", "30"))
BATCH = 64
SEED = 1337

tf.random.set_seed(SEED)
np.random.seed(SEED)
os.makedirs(OUT, exist_ok=True)


def build():
    def cbr(x, ch, stride=1, padding="same", name=None):
        x = layers.Conv2D(ch, 3, strides=stride, padding=padding, use_bias=False,
                          name=name)(x)
        x = layers.BatchNormalization()(x)
        return layers.ReLU()(x)

    inp = keras.Input((SZ, SZ, 3), name="image")           # float in [0,1]
    x = cbr(inp, 16, 2)          # 64
    x = cbr(x, 16)
    x = cbr(x, 32, 2)            # 32
    x = cbr(x, 32)
    x = cbr(x, 64, 2)            # 16
    x = cbr(x, 64)
    x = cbr(x, 96, 2)            # 8
    x = cbr(x, 96)
    x = layers.MaxPool2D(2)(x)   # 4
    x = cbr(x, 96, 2, "valid")   # (4-3)//2+1 = 1  -> 1x1x96
    x = layers.Dropout(0.3)(x)
    x = layers.Conv2D(len(CLASSES), 1, name="logits")(x)   # 1x1x4 logits
    return keras.Model(inp, x, name="rps_npu")


def augment(x, y):
    """On-the-fly: flip, brightness, and a random RGB<->BGR swap so the model
    does not care which channel the ISP puts first."""
    x = tf.image.random_flip_left_right(x)
    x = tf.image.random_brightness(x, 0.08)
    x = tf.image.random_contrast(x, 0.85, 1.15)
    swap = tf.random.uniform([]) < 0.35
    x = tf.cond(swap, lambda: tf.reverse(x, axis=[-1]), lambda: x)
    return tf.clip_by_value(x, 0.0, 1.0), y


def main():
    d = np.load(DATA)
    xtr = d["x_train"].astype(np.float32) / 255.0
    ytr = d["y_train"]
    xva = d["x_val"].astype(np.float32) / 255.0
    yva = d["y_val"]
    print("train", xtr.shape, "val", xva.shape, "class counts", np.bincount(ytr))

    ds = (tf.data.Dataset.from_tensor_slices((xtr, ytr)).shuffle(4096, seed=SEED)
          .map(augment, num_parallel_calls=tf.data.AUTOTUNE).batch(BATCH)
          .prefetch(tf.data.AUTOTUNE))
    vds = tf.data.Dataset.from_tensor_slices((xva, yva)).batch(256)

    model = build()
    model.summary()
    params = model.count_params()

    def loss(y, logits):
        return keras.losses.sparse_categorical_crossentropy(y, tf.reshape(logits, [-1, 4]),
                                                            from_logits=True)

    def acc(y, logits):
        return keras.metrics.sparse_categorical_accuracy(y, tf.reshape(logits, [-1, 4]))

    steps = (len(xtr) // BATCH) * EPOCHS
    sched = keras.optimizers.schedules.CosineDecay(2e-3, steps, alpha=0.02)
    model.compile(optimizer=keras.optimizers.Adam(sched), loss=loss, metrics=[acc])
    t0 = time.time()
    hist = model.fit(ds, validation_data=vds, epochs=EPOCHS, verbose=2)
    train_s = time.time() - t0
    float_acc = float(hist.history["val_acc"][-1])
    model.save(os.path.join(OUT, "rps_float.keras"))

    # ---- full-integer int8 TFLite ----
    def rep():
        idx = np.random.default_rng(SEED).choice(len(xtr), 300, replace=False)
        for i in idx:
            yield [xtr[i:i + 1]]

    conv = tf.lite.TFLiteConverter.from_keras_model(model)
    conv.optimizations = [tf.lite.Optimize.DEFAULT]
    conv.representative_dataset = rep
    conv.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
    conv.inference_input_type = tf.int8
    conv.inference_output_type = tf.int8
    tfl = conv.convert()
    tfl_path = os.path.join(OUT, "rps_int8.tflite")
    open(tfl_path, "wb").write(tfl)

    # ---- evaluate the int8 graph ----
    it = tf.lite.Interpreter(model_content=tfl)
    it.allocate_tensors()
    inp = it.get_input_details()[0]
    out = it.get_output_details()[0]
    in_scale, in_zp = inp["quantization"]
    out_scale, out_zp = out["quantization"]
    print("input", inp["shape"].tolist(), inp["dtype"].__name__, in_scale, in_zp)
    print("output", out["shape"].tolist(), out["dtype"].__name__, out_scale, out_zp)

    def run_int8(img_u8):
        q = np.round(img_u8.astype(np.float32) / 255.0 / in_scale + in_zp)
        q = np.clip(q, -128, 127).astype(np.int8)[None]
        it.set_tensor(inp["index"], q)
        it.invoke()
        return it.get_tensor(out["index"]).reshape(-1)

    xva_u8 = d["x_val"]
    correct = 0
    confusion = np.zeros((4, 4), dtype=int)
    for i in range(len(xva_u8)):
        p = int(np.argmax(run_int8(xva_u8[i])))
        confusion[yva[i], p] += 1
        correct += p == yva[i]
    int8_acc = correct / len(xva_u8)
    print(f"float val acc {float_acc:.4f}  int8 val acc {int8_acc:.4f}")
    print("confusion (rows=true rock,paper,scissors,none):\n", confusion)

    # ---- per-class samples for the device: raw int8 bytes + golden output ----
    sd = os.path.join(OUT, "samples")
    os.makedirs(sd, exist_ok=True)
    from PIL import Image
    for ci, c in enumerate(CLASSES):
        for i in range(len(xva_u8)):
            if yva[i] != ci:
                continue
            o = run_int8(xva_u8[i])
            if int(np.argmax(o)) != ci:
                continue
            Image.fromarray(xva_u8[i]).save(os.path.join(sd, f"{c}.png"))
            q = np.round(xva_u8[i].astype(np.float32) / 255.0 / in_scale + in_zp)
            np.clip(q, -128, 127).astype(np.int8).tofile(os.path.join(sd, f"{c}.bin"))
            with open(os.path.join(sd, f"{c}.expected.txt"), "w") as f:
                f.write("int8 logits: " + " ".join(str(int(v)) for v in o) + "\n")
                f.write("dequant: " + " ".join(f"{(int(v) - out_zp) * out_scale:.3f}" for v in o) + "\n")
            break

    numbers = {
        "classes": CLASSES, "input": f"{SZ}x{SZ}x3 int8",
        "input_quant": [float(in_scale), int(in_zp)],
        "output_quant": [float(out_scale), int(out_zp)],
        "output_shape": out["shape"].tolist(),
        "params": int(params), "tflite_bytes": len(tfl), "epochs": EPOCHS,
        "float_val_acc": float_acc, "int8_val_acc": int8_acc,
        "confusion": confusion.tolist(), "train_seconds": round(train_s),
        "tensorflow": tf.__version__, "seed": SEED,
    }
    json.dump(numbers, open(os.path.join(OUT, "numbers.json"), "w"), indent=2)
    print(json.dumps(numbers, indent=2))


if __name__ == "__main__":
    main()

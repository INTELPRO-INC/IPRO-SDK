/**
 * @file yolo_runner.cc
 * @brief YOLOv8 (352x640x3 int8, 6 output heads) through TFLM on the CPU.
 *
 * The point of this app is the other half of a comparison: the exact same
 * .tflite is what ips_aitool compiles into person_pet_detect.inm, which runs
 * on the NPU. Measuring both gives a like-for-like NPU-vs-CPU number instead
 * of comparing two different models.
 *
 * Model shape (parsed from the flatbuffer, not assumed):
 *   106 operators, 235 tensors, 59 CONV_2D
 *   461,120,000 MAC total; largest single layer 32.4 M MAC
 *   inputs  [1,352,640,3] int8
 *   outputs 6 heads at 3 scales - (11x20, 22x40, 44x80) x (4 box, 2 class)
 *
 * No post-processing: decoding boxes would measure our NMS, not the model.
 * The outputs are checksummed instead, which is enough to prove the graph
 * actually ran and to catch a silently-wrong result.
 */

#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include <stdio.h>
#include <string.h>

#include "ipro_cycles.h"
#include "yolo_runner.h"

extern "C" {
extern const unsigned char g_yolov8_model[];
extern const unsigned char g_yolov8_model_end[];
}

namespace {

/* Largest activation is 353*641*3 = 678,819 elements; the arena also holds the
 * interpreter's tensor bookkeeping and the NMSIS-NN scratch. Sized generously
 * and reported back, so the first run tells us what it actually needs rather
 * than us guessing twice. Plain .bss => PSRAM on this build, which the kernel
 * benchmark showed costs under 1% for compute-bound convolution. */
constexpr int kArenaSize = 4 * 1024 * 1024;
alignas(16) uint8_t g_arena[kArenaSize];

tflite::MicroInterpreter *g_interp = nullptr;

}  // namespace

extern "C" bool yolo_init(yolo_info_t *info)
{
    memset(info, 0, sizeof(*info));

    const uint64_t t0 = ipro_cycles64();

    const tflite::Model *model = tflite::GetModel(g_yolov8_model);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        printf("  schema %lu != %d\r\n",
               (unsigned long)model->version(), TFLITE_SCHEMA_VERSION);
        return false;
    }

    /* Exactly the eight builtin ops the flatbuffer uses - CONV_2D x59,
     * STRIDED_SLICE x16, CONCATENATION x13, PAD x7, ADD x4, MAX_POOL_2D x3,
     * QUANTIZE x2, RESIZE_NEAREST_NEIGHBOR x2. AllOpsResolver would link all
     * ~117 kernels for no reason. */
    static tflite::MicroMutableOpResolver<8> resolver;
    if (resolver.AddConv2D()                != kTfLiteOk ||
        resolver.AddStridedSlice()          != kTfLiteOk ||
        resolver.AddConcatenation()         != kTfLiteOk ||
        resolver.AddPad()                   != kTfLiteOk ||
        resolver.AddAdd()                   != kTfLiteOk ||
        resolver.AddMaxPool2D()             != kTfLiteOk ||
        resolver.AddQuantize()              != kTfLiteOk ||
        resolver.AddResizeNearestNeighbor() != kTfLiteOk) {
        printf("  op resolver setup failed\r\n");
        return false;
    }

    static tflite::MicroInterpreter interp(model, resolver, g_arena, kArenaSize);
    if (interp.AllocateTensors() != kTfLiteOk) {
        printf("  AllocateTensors failed with a %d KB arena\r\n", kArenaSize / 1024);
        return false;
    }

    const uint64_t t1 = ipro_cycles64();

    g_interp = &interp;
    info->model_bytes = (uint32_t)(g_yolov8_model_end - g_yolov8_model);
    info->arena_used  = (uint32_t)interp.arena_used_bytes();
    info->arena_size  = kArenaSize;
    info->load_cycles = t1 - t0;
    info->n_inputs    = (uint32_t)interp.inputs_size();
    info->n_outputs   = (uint32_t)interp.outputs_size();

    TfLiteTensor *in = interp.input(0);
    info->in_h = in->dims->size > 1 ? in->dims->data[1] : 0;
    info->in_w = in->dims->size > 2 ? in->dims->data[2] : 0;
    info->in_c = in->dims->size > 3 ? in->dims->data[3] : 0;
    return true;
}

extern "C" bool yolo_infer(yolo_result_t *res)
{
    if (g_interp == nullptr) {
        return false;
    }

    TfLiteTensor *in = g_interp->input(0);
    const int n = in->bytes;

    /* Deterministic pseudo-image. The graph is data-independent, so this costs
     * exactly what a camera frame would. */
    int32_t seed = 20260821;
    for (int i = 0; i < n; i++) {
        seed = seed * 1103515245 + 12345;
        in->data.int8[i] = (int8_t)((seed >> 16) & 0xff);
    }

    const uint64_t t0 = ipro_cycles64();
    const TfLiteStatus st = g_interp->Invoke();
    const uint64_t t1 = ipro_cycles64();

    if (st != kTfLiteOk) {
        return false;
    }

    res->cycles = t1 - t0;
    res->n_heads = 0;
    for (unsigned h = 0; h < g_interp->outputs_size() && h < YOLO_MAX_HEADS; h++) {
        TfLiteTensor *o = g_interp->output(h);
        int32_t sum = 0;
        for (size_t i = 0; i < o->bytes; i++) {
            sum += o->data.int8[i];
        }
        res->head_sum[h]   = sum;
        res->head_bytes[h] = (uint32_t)o->bytes;
        res->n_heads++;
    }
    return true;
}

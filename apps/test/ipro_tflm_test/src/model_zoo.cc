/**
 * @file model_zoo.cc
 * @brief Run several .tflite models through TFLM and time them uniformly.
 *
 * The point is the spread, not any single number: 4,160 to 7,157,888 MAC is a
 * 1720x range, and where a model sits on it decides whether the CPU path is
 * viable at all. Paired with the NPU measurement in apps/test/ipro_npu_yolo
 * (YOLOv8 at 461 M MAC, 102 ms) this is what answers "can my model run on the
 * CPU" without having to port it first.
 *
 * One arena is shared: the models run sequentially and nothing needs to stay
 * resident, so each gets a stack-local interpreter over the same buffer.
 * Sized for person_detect, which needs ~82 KB.
 */

#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include <stdio.h>
#include <string.h>

#include "ipro_cycles.h"
#include "model_zoo.h"

extern "C" {
extern const unsigned char g_m_keyword[],  g_m_keyword_end[];
extern const unsigned char g_m_speech[],   g_m_speech_end[];
extern const unsigned char g_m_hello[],    g_m_hello_end[];
extern const unsigned char g_m_person[],   g_m_person_end[];
}

namespace {

/* person_detect reports ~82 KB used; the rest are far smaller. Plain .bss, so
 * PSRAM on this build - shown to cost <1% for compute-bound convolution. */
constexpr int kArenaSize = 160 * 1024;
alignas(16) uint8_t g_arena[kArenaSize];

/* Each model needs its own op set, so registration is per-entry rather than a
 * shared AllOpsResolver - that would link ~117 kernels for no benefit. */
typedef TfLiteStatus (*register_fn)(tflite::MicroOpResolver **out);

TfLiteStatus reg_keyword(tflite::MicroOpResolver **out)
{
    static tflite::MicroMutableOpResolver<4> r;
    if (r.AddSvdf()            != kTfLiteOk ||
        r.AddFullyConnected()  != kTfLiteOk ||
        r.AddQuantize()        != kTfLiteOk ||
        r.AddSoftmax()         != kTfLiteOk) return kTfLiteError;
    *out = &r;
    return kTfLiteOk;
}

TfLiteStatus reg_speech(tflite::MicroOpResolver **out)
{
    static tflite::MicroMutableOpResolver<4> r;
    if (r.AddReshape()          != kTfLiteOk ||
        r.AddDepthwiseConv2D()  != kTfLiteOk ||
        r.AddFullyConnected()   != kTfLiteOk ||
        r.AddSoftmax()          != kTfLiteOk) return kTfLiteError;
    *out = &r;
    return kTfLiteOk;
}

TfLiteStatus reg_hello(tflite::MicroOpResolver **out)
{
    static tflite::MicroMutableOpResolver<1> r;
    if (r.AddFullyConnected() != kTfLiteOk) return kTfLiteError;
    *out = &r;
    return kTfLiteOk;
}

TfLiteStatus reg_person(tflite::MicroOpResolver **out)
{
    static tflite::MicroMutableOpResolver<5> r;
    if (r.AddAveragePool2D()   != kTfLiteOk ||
        r.AddConv2D()          != kTfLiteOk ||
        r.AddDepthwiseConv2D() != kTfLiteOk ||
        r.AddReshape()         != kTfLiteOk ||
        r.AddSoftmax()         != kTfLiteOk) return kTfLiteError;
    *out = &r;
    return kTfLiteOk;
}

struct entry {
    const char          *name;
    const unsigned char *blob;
    const unsigned char *blob_end;
    register_fn          reg;
    uint64_t             mac;   /* from the flatbuffer's tensor shapes */
};

const entry kZoo[] = {
    { "keyword_scrambled", g_m_keyword, g_m_keyword_end, reg_keyword,       4160ULL },
    { "micro_speech",      g_m_speech,  g_m_speech_end,  reg_speech,      336000ULL },
    { "hello_world",       g_m_hello,   g_m_hello_end,   reg_hello,          288ULL },
    { "person_detect",     g_m_person,  g_m_person_end,  reg_person,     7157888ULL },
};
constexpr int kZooN = sizeof(kZoo) / sizeof(kZoo[0]);

/* Deterministic filler; the graphs are data-independent so this costs what a
 * real input would, and keeps the checksum comparable across builds. */
void fill_input(TfLiteTensor *t)
{
    int32_t seed = 20260821;
    for (size_t i = 0; i < t->bytes; i++) {
        seed = seed * 1103515245 + 12345;
        t->data.int8[i] = (int8_t)((seed >> 16) & 0xff);
    }
}

}  // namespace

extern "C" int zoo_count(void) { return kZooN; }

extern "C" const char *zoo_name(int idx)
{
    return (idx >= 0 && idx < kZooN) ? kZoo[idx].name : nullptr;
}

extern "C" bool zoo_run(int idx, int iters, zoo_result_t *out)
{
    if (idx < 0 || idx >= kZooN) return false;
    const entry &e = kZoo[idx];

    memset(out, 0, sizeof(*out));
    out->name        = e.name;
    out->model_bytes = (uint32_t)(e.blob_end - e.blob);
    out->total_mac   = e.mac;

    const uint64_t t0 = ipro_cycles64();

    const tflite::Model *model = tflite::GetModel(e.blob);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        printf("  %s: schema %lu != %d\r\n", e.name,
               (unsigned long)model->version(), TFLITE_SCHEMA_VERSION);
        return false;
    }

    tflite::MicroOpResolver *resolver = nullptr;
    if (e.reg(&resolver) != kTfLiteOk) {
        printf("  %s: op registration failed\r\n", e.name);
        return false;
    }

    tflite::MicroInterpreter interp(model, *resolver, g_arena, kArenaSize);
    if (interp.AllocateTensors() != kTfLiteOk) {
        printf("  %s: AllocateTensors failed (arena %d KB)\r\n",
               e.name, kArenaSize / 1024);
        return false;
    }
    out->load_cycles = ipro_cycles64() - t0;
    out->arena_used  = (uint32_t)interp.arena_used_bytes();

    uint64_t warm = 0;
    int warm_n = 0;
    for (int i = 0; i < iters; i++) {
        fill_input(interp.input(0));

        const uint64_t a = ipro_cycles64();
        const TfLiteStatus st = interp.Invoke();
        const uint64_t b = ipro_cycles64();

        if (st != kTfLiteOk) {
            printf("  %s: invoke failed at run %d\r\n", e.name, i);
            return false;
        }
        if (i == 0) {
            out->cold_cycles = b - a;
            TfLiteTensor *o = interp.output(0);
            int32_t sum = 0;
            for (size_t k = 0; k < o->bytes; k++) sum += o->data.int8[k];
            out->checksum = sum;
        } else {
            warm += b - a;
            warm_n++;
        }
    }
    out->warm_cycles = warm_n ? (warm / (uint64_t)warm_n) : out->cold_cycles;
    out->ok = true;
    return true;
}

/*
 * models.S - the benchmark model zoo, linked in via .incbin.
 *
 * Four .tflite files spanning 4,160 to 7,157,888 MAC. Keeping the flatbuffers
 * verbatim (rather than as generated C arrays) means the artefact under test
 * is provably the upstream file, and keeps ~330 KB of models out of the diff
 * as source text.
 *
 *   keyword_scrambled  TFLM benchmark model, SVDF x7 + FC x5, int16 in
 *   micro_speech       keyword spotting, DEPTHWISE_CONV + FC + SOFTMAX
 *   hello_world        sine regressor, FC only - interpreter-overhead floor
 *   person_detect      visual wake words, MobileNet-shaped
 */

    .section .rodata.models, "a", @progbits
    .balign 16

#define BLOB(sym, file)          \
    .global sym                 ;\
    .type   sym, @object        ;\
sym:                            ;\
    .incbin file                ;\
    .global sym##_end           ;\
sym##_end:                      ;\
    .size   sym, sym##_end - sym;\
    .balign 16

    BLOB(g_m_keyword,  "keyword_scrambled.tflite")
    BLOB(g_m_speech,   "micro_speech.tflite")
    BLOB(g_m_hello,    "hello_world.tflite")
    BLOB(g_m_person,   "person_detect.tflite")

/*
 * model_inm.S - link person_pet_detect.inm into the image.
 *
 * The .inm that ips_aitool compiles from
 * yolov8_best_headless_6out_full_integer_quant.tflite - the same .tflite that
 * apps/test/ipro_tflm_yolo runs through TFLM on the CPU. Embedding rather than
 * reading from the 'mfg' partition keeps the two apps symmetric (both carry
 * their model) and removes a flashing step from the comparison.
 *
 * 700,088 bytes, of which 696,536 (99.5%) are NPU sections.
 */

    .section .rodata.inm_model, "a", @progbits
    .balign 16

    .global g_inm_model
    .type   g_inm_model, @object
g_inm_model:
    .incbin "person_pet_detect.inm"
    .global g_inm_model_end
g_inm_model_end:
    .size   g_inm_model, g_inm_model_end - g_inm_model

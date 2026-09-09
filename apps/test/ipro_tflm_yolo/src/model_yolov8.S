/*
 * model_yolov8.S - link the YOLOv8 int8 model into the image via .incbin.
 *
 * 779 KB as a generated C array would be a ~4.7 MB .c file. .incbin keeps the
 * .tflite itself in the tree, byte-for-byte, which also means the artefact
 * under test is provably the same file the .inm was compiled from.
 *
 * Source: model_porting_user_guide/model/
 *         yolov8_best_headless_6out_full_integer_quant.tflite
 * The same file ips_aitool compiles into person_pet_detect.inm, which is what
 * makes the NPU-vs-CPU comparison in this app an apples-to-apples one.
 */

    .section .rodata.yolo_model, "a", @progbits
    .balign 16

    .global g_yolov8_model
    .type   g_yolov8_model, @object
g_yolov8_model:
    .incbin "yolov8_person_pet.tflite"
    .global g_yolov8_model_end
g_yolov8_model_end:
    .size   g_yolov8_model, g_yolov8_model_end - g_yolov8_model

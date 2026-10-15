/*
 * model_blob.S - link model/rps_v3.inm into the firmware image
 * (CONFIG_RPS_EMBED_MODEL; CMakeLists.txt assembles this file only then).
 *
 * .incbin rather than a generated C array: a 300 KB model becomes a ~1.8 MB
 * .c file that is slow to compile and pointless to keep in git.
 *
 * The blob lands in .rodata, so it is read over XIP exactly like the mfg
 * copy was - iproai_load_model_from_buffer() keeps pointers into it, and the
 * NPU fetches its weights from flash. Cost: the image grows by the model
 * (~299 KB), which needs the FW partition of partition_cfg_1M_rps_embed.toml,
 * and swapping the model means rebuilding the firmware.
 */

    .section .rodata.rps_model, "a", @progbits
    .balign 16

    .global g_rps_model
    .type   g_rps_model, @object
g_rps_model:
    .incbin "rps_v3.inm"
    .global g_rps_model_end
g_rps_model_end:
    .size   g_rps_model, g_rps_model_end - g_rps_model

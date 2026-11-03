/*
 * model_blob.S - link the .inm model straight into the firmware image.
 *
 * .incbin rather than a generated C array: a 1 MB model becomes a ~6 MB .c
 * file that is slow to compile and pointless to keep in git.
 *
 * Cost, before enabling this: mbv2.inm is 1,032,080 bytes. Added to the
 * ~617 KB firmware that is ~1.65 MB, which only fits the FW partition of
 * partition_cfg_2M_noota.toml (2004 KB); the OTA-capable tables leave at most
 * 1180 KB. Embedding therefore trades OTA away, and the model can no longer be
 * swapped without a full firmware update. Reasonable for a bench setup, bad
 * for anything shipped.
 */

#include <generated/autoconf.h>

#ifdef CONFIG_MEETING_EMBED_MODEL

    .section .rodata.ai_model, "a", @progbits
    .balign 16

    .global g_embedded_model
    .type   g_embedded_model, @object
g_embedded_model:
    .incbin CONFIG_MEETING_EMBED_MODEL_PATH
    .global g_embedded_model_end
g_embedded_model_end:
    .size   g_embedded_model, g_embedded_model_end - g_embedded_model

#endif /* CONFIG_MEETING_EMBED_MODEL */

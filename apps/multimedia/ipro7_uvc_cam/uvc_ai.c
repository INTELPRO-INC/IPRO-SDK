/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

/**
 * @file uvc_ai.c
 * @brief AI/NPU person + pet detection for the IPRO7 UVC camera.
 *
 * Based on apps/platform/ipro7_demo/demo/ai/demo_ai.c (external-buffer
 * inference loop), trimmed to: load model from flash -> run YOLOv8 person/pet
 * inference on ISP channel-1 frames -> publish results for OSD overlay.
 *
 * The model is stored in a dedicated flash partition (default "mfg") so it can
 * be re-flashed independently of the firmware (dynamic model replacement).
 * Layout in the partition: [0x1000 header][model.inm].
 */

#include <generated/autoconf.h>

#include "uvc_ai.h"

#ifdef CONFIG_AI_SUPPORT

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

/* Flash / partition lookup */
#include "hal_flash.h"
#include "drv_sf_ctrl.h"
#include "hal_boot2.h"

/* AI runtime + detection post-processor */
#include <iproai_core.h>
#include <iproai_def.h>
#include "hal_npu.h"
#include "iproai_inst_npu.h"
#include <system_soc.h>   /* L1C_DCACHE_CLEAN_INVALID_RANGE for the external input buffer */

/* Post-processor selection (all produce yolov8_result_t so the OSD path is the
 * same). Priority: GESTURE > BIG_A_SPPF > yolov8 person/pet.
 *   UVC_AI_MODEL_GESTURE    : hand-sign-digit detector, YOLOv8 anchor-free + DFL
 *                             (3 outputs x 74ch, 10 classes). INTERNAL input
 *                             (int8 zp-128 -> write getInputBuffer). Boxes->OSD.
 *   UVC_AI_MODEL_BIG_A_SPPF : SKU-110K single-class (3 outputs x 18ch, v5-anchor).
 *                             EXTERNAL input (NPU_Img_Ext_Addr_Cfg).
 *   else                    : original YOLOv8 person/pet (6 outputs). EXTERNAL. */
#ifndef UVC_AI_MODEL_GESTURE
#define UVC_AI_MODEL_GESTURE 1
#endif
#ifndef UVC_AI_MODEL_BIG_A_SPPF
#define UVC_AI_MODEL_BIG_A_SPPF 0
#endif

#if UVC_AI_MODEL_GESTURE
#include "detect/gesture_dfl_detect.h"
typedef gdfl_config_t uvc_ai_postproc_cfg_t;
#define UVC_AI_PP_INIT    gdfl_init
#define UVC_AI_PP_PROCESS gdfl_process
#define UVC_AI_PP_PRINT   gdfl_print_results
#define UVC_AI_PP_DEINIT  gdfl_deinit
/* DEVICE-VERIFIED (2026-06-03): the .inm header ext_input=1, so the NPU reads
 * input from the EXTERNAL DM2 address (NPU_Img_Ext_Addr_Cfg), int8 zp -128,
 * 4-byte/pixel (pad_c=align(3,4)=4, iproai_inst_cpu.c:831-833). We feed a buffer
 * we convert (u-128) from the ISP ch1 ARGB frame. The earlier "frozen output"
 * was TWO bugs stacked: (1) reading the NPU output from CPU-cached PSRAM without
 * invalidating -> stale previous-frame output (fixed in gesture_dfl_detect.c),
 * and (2) feeding the wrong buffer. With the output-cache invalidate in place,
 * the DM2 external feed is the correct input path (DM0/getInputBuffer is ignored
 * for ext_input=1). */
/* The NPU-clean rebuild (deploy_ipro7/gesture_npuclean.inm, 2026-06-03) is the
 * deployable model: layer 0 is an NPU ext_input CONV that reads the 256x256x3
 * image directly (all-NPU, no CPU stem, no leading PAD), so it is fed exactly
 * like big_a via NPU_Img_Ext_Addr_Cfg -- EXCEPT its input tensor is int8
 * (zp -128, scale 1/255) where big_a is uint8. The one required difference is
 * that every camera byte must be stored as (u - 128), which is exactly XOR 0x80
 * (flip the sign bit): u=0->-128, u=128->0, u=255->127 -- one word-wide op
 * (^ 0x80808080). We convert the ISP ch1 frame VERBATIM (no channel reorder)
 * into a dedicated buffer and point NPU_Img_Ext_Addr_Cfg at it (UVC_AI_EXT_XOR80).
 * Channel order is the raw ISP B,G,R,A: feeding RGB-reordered killed detections,
 * so the .inm expects the device's native order -- do NOT reorder.
 *
 * If you instead flash the uint8-input .inm (gesture_npuclean_UINT8input,
 * compiled by IntelPro's ips_aitool -> zp 0), set UVC_AI_EXT_XOR80 0 to feed the
 * raw ARGB frame with no conversion (exact big_a parity, zero-copy).
 *
 * The DFL decoder reads scale/zp from net_info at runtime, so the same decode
 * handles either model -- only this input flag and the flashed .inm differ. */
#define UVC_AI_INTERNAL_INPUT   0
#define UVC_AI_EXT_CONVERT_INT8 0
#define UVC_AI_EXT_XOR80        1  /* int8 model: feed (u-128) via NPU_Img_Ext_Addr_Cfg */
#elif UVC_AI_MODEL_BIG_A_SPPF
#include "detect/big_a_sppf_detect.h"
typedef big_a_sppf_config_t uvc_ai_postproc_cfg_t;
#define UVC_AI_PP_INIT    big_a_sppf_init
#define UVC_AI_PP_PROCESS big_a_sppf_process
#define UVC_AI_PP_PRINT   big_a_sppf_print_results
#define UVC_AI_PP_DEINIT  big_a_sppf_deinit
#define UVC_AI_INTERNAL_INPUT  0
#define UVC_AI_EXT_CONVERT_INT8 0  /* uint8 input: raw ARGB frame addr is fed directly */
#define UVC_AI_EXT_XOR80       0
#else
#include "detect/yolov8_person_pet_detect.h"
typedef yolov8_config_t uvc_ai_postproc_cfg_t;
#define UVC_AI_PP_INIT    yolov8_person_pet_init
#define UVC_AI_PP_PROCESS yolov8_person_pet_process
#define UVC_AI_PP_PRINT   yolov8_person_pet_print_results
#define UVC_AI_PP_DEINIT  yolov8_person_pet_deinit
#define UVC_AI_INTERNAL_INPUT  0
#define UVC_AI_EXT_CONVERT_INT8 0
#define UVC_AI_EXT_XOR80       0
#endif

/* ===== Model input geometry =====================================================
 * AI_INPUT_640X384 selects what the firmware feeds the NPU:
 *   0 = 256x256 square-crop bring-up (centered 720x720 of the sensor -> 256x256).
 *   1 = 640x384 full-frame letterbox: the whole 1280x720 frame is scaled to
 *       640x360 (clean 2x, no distortion) and centered in a 640x384 buffer with
 *       12-row gray (114) letterbox bars top+bottom -- byte-identical to what
 *       ultralytics imgsz=640 produces. So train with STANDARD ultralytics on
 *       full field-of-view frames and the device pre-processing matches exactly.
 * MUST stay in sync with uvc_isp.c's AI_INPUT_640X384 (OUT_WIDTH_1/OUT_HEIGHT_1).
 * The .inm input tensor MUST equal UVC_AI_IN_W x UVC_AI_IN_H. */
#ifndef AI_INPUT_640X384
#define AI_INPUT_640X384 1
#endif
#if AI_INPUT_640X384
#define UVC_AI_ISP_W 640   /* ch1 scaler output = the un-padded image content   */
#define UVC_AI_ISP_H 360
#define UVC_AI_IN_W  640   /* model input tensor (content + letterbox bars)     */
#define UVC_AI_IN_H  384
#else
#define UVC_AI_ISP_W 256
#define UVC_AI_ISP_H 256
#define UVC_AI_IN_W  256
#define UVC_AI_IN_H  256
#endif
#define UVC_AI_LB_Y      (((UVC_AI_IN_H) - (UVC_AI_ISP_H)) / 2) /* letterbox top rows */
#define UVC_AI_LB_X      (((UVC_AI_IN_W) - (UVC_AI_ISP_W)) / 2)
#define UVC_AI_LB_PAD_I8 ((uint8_t)(114 - 128))                 /* gray 114 -> int8 -14 */

/* ARGB byte order of the ISP ch1 frame (used only by the INTERNAL input path).
 * The int8 XOR ext feed does NOT use these — it feeds the raw ISP order verbatim
 * (reordering to RGB was verified to kill detections). If predictions look wrong
 * on the INTERNAL path, the camera may be B,G,R,A vs R,G,B,A — flip these. */
#define UVC_AI_ARGB_R 2
#define UVC_AI_ARGB_G 1
#define UVC_AI_ARGB_B 0

/* DIAGNOSTIC: when 1, feed a synthetic moving checkerboard instead of the camera
 * to prove whether the NPU actually reads our input buffer. Set 0 for real use. */
#ifndef UVC_AI_TEST_PATTERN
#define UVC_AI_TEST_PATTERN 0
#endif

/* Classification test mode. When 1, the inference task runs the TinyNet CIFAR-10
 * classifier (model.inm in "mfg"): it feeds the ch1 frame into the model input
 * buffer, runs the NPU, argmaxes the 10 int8 logits, and prints the class to
 * serial. The detection post-processor (above) is NOT used. Set 0 to go back to
 * the big_a / yolov8 detection path. */
#ifndef UVC_AI_MODE_CLASSIFY
#define UVC_AI_MODE_CLASSIFY 0
#endif

#if UVC_AI_MODE_CLASSIFY
#define CLS_RES         128                 /* model input (see MODEL_CARD.md) */
#define CLS_NUM_CLASSES 10
#define CLS_OUT_SCALE   0.045254096f        /* output dequant scale            */
#define CLS_OUT_ZP      8                   /* output dequant zero_point       */
/* ARGB byte order of the ISP ch1 frame. If predictions look wrong, the camera
 * may be B,G,R,A vs R,G,B,A — flip these channel indices. */
#define CLS_ARGB_R 2
#define CLS_ARGB_G 1
#define CLS_ARGB_B 0
static const char *CLS_LABELS[CLS_NUM_CLASSES] = {
    "airplane", "automobile", "bird", "cat", "deer",
    "dog", "frog", "horse", "ship", "truck"};
#endif

/* MM clock un-gate for the NPU */
#include "drv_glb.h"

/****************************************/
/*    CONFIGURATION                     */
/****************************************/

/* Partition holding the model blob. The model sits 0x1000 past the partition
 * base (the standard partition header). Re-flash this partition to swap models
 * without rebuilding the firmware. */
#ifdef CONFIG_UVC_AI_MODEL_PARTITION_NAME
#define UVC_AI_MODEL_PARTITION CONFIG_UVC_AI_MODEL_PARTITION_NAME
#else
#define UVC_AI_MODEL_PARTITION "mfg"
#endif

#define PARTITION_HEADER_SIZE 0x1000

#define UVC_AI_TASK_STACK    8192
/* Below the MJPEG encode / UVC stream tasks (priority 10) so the long CPU-bound
 * inference burst (the YOLOv8 stem's transpose/slice ops run on CPU, ~900ms)
 * can be preempted to service the ch0 stream FIFO — stops OUT 0 FIFO_OVERWRITE /
 * VSYNC_CNT_ERROR. Does not reduce latency; the real perf fix is an NPU-friendly
 * stem (no focus/space-to-depth). */
#define UVC_AI_TASK_PRIORITY 8
#define UVC_AI_QUEUE_SIZE    1 /* one frame in flight */

/****************************************/
/*    PRIVATE STATE                     */
/****************************************/

static iproai_model_hdl_t g_model_hdl = NULL;
#if !UVC_AI_MODE_CLASSIFY
static uvc_ai_postproc_cfg_t g_postproc_cfg;
#endif
static yolov8_result_t g_detect_result;
static QueueHandle_t g_ai_buffer_queue = NULL;
static QueueHandle_t g_ai_result_queue = NULL;
static SemaphoreHandle_t g_ai_done_sem = NULL;
static TaskHandle_t g_ai_task_handle = NULL;
static volatile uint8_t g_ai_initialized = 0;
static volatile uint8_t g_ai_task_running = 0;
#if UVC_AI_EXT_CONVERT_INT8
/* External-input frame buffer: w*h*align(3,4)=w*h*4 int8 bytes, filled per frame
 * from the ISP ch1 ARGB frame with a -128 offset (int8 zero_point -128).
 *
 * CRITICAL: for an ext_input=1 model the NPU reads its input from imgBuffer+4
 * (the SDK programs DM2 = imgBuffer+4 at load, iproai_inst_cpu.c:225). `imgBuffer`
 * is a WEAK NULL stub in hal_npu.c whose comment says "user should define in their
 * application" — this app never did, so DM2 pointed at 0x4 (garbage) and the NPU
 * ran on a FIXED bogus input every frame (frozen output). We define it strongly
 * here, allocate it BEFORE model load, and write the live frame to imgBuffer+4. */
#define UVC_AI_EXT_BUF_BYTES (UVC_AI_IN_W * UVC_AI_IN_H * 4)
char *imgBuffer = NULL;            /* strong def overrides hal_npu.c weak stub */
#define UVC_AI_EXT_DATA() ((int8_t *)(imgBuffer + 4))  /* NPU reads from imgBuffer+4 */
/* DEVICE TRACE verdict: in legacy mode (use_npu_accel=0) the image-input layer
 * reads from model->buffer (DM0) at in_layer1_mem*patch_size, NOT the DM2 ext addr.
 * Resolved at runtime from the layer with img_in==1. */
static uint32_t g_img_off = 0;     /* byte offset of the image input patch in model->buffer */
static uint32_t g_img_bytes = 0;   /* size of that patch */
#endif
#if UVC_AI_EXT_XOR80
/* int8 ext_input feed buffer for gesture_npuclean.inm: per frame we convert the
 * ISP ch1 ARGB frame (u) -> int8 (u-128 == XOR 0x80) into this buffer, then point
 * NPU_Img_Ext_Addr_Cfg at it. Dedicated (not in-place) so the NPU sees a stable
 * input decoupled from ISP DMA timing. 4 bytes/pixel (pad_c = align(3,4) = 4),
 * same stride as big_a. PSRAM via pvPortMalloc (NPU/DMA reachable). */
#define UVC_AI_XOR_BUF_BYTES (UVC_AI_IN_W * UVC_AI_IN_H * 4)
static uint8_t *g_xor_buf = NULL;
#endif

/****************************************/
/*    MODEL LOADING (FLASH PARTITION)   */
/****************************************/

static int load_ai_model(void)
{
    uint32_t flash_addr = 0;
    uint32_t part_size = 0;
    uint32_t model_size = 0;
    IPROAI_Status_e status;
    const char *part_name = UVC_AI_MODEL_PARTITION;

    /* Step 1: resolve the partition address from the active partition table. */
    int ret = hal_boot2_partition_addr_active(part_name, &flash_addr, &part_size);
    if (ret != 0) {
        printf("[UVC_AI] ERROR: partition '%s' not found (ret=%d)\r\n", part_name, ret);
        return -1;
    }
    printf("[UVC_AI] partition '%s': base=0x%x size=%u\r\n",
           part_name, (unsigned)flash_addr, (unsigned)part_size);

    /* Model is stored after the 0x1000 partition header. */
    uint32_t model_flash_addr = flash_addr + PARTITION_HEADER_SIZE;

    /* Translate flash physical address to the XIP virtual address. boot2/OTA
     * may offset XIP_BASE to the active FW slot; the model lives outside that
     * slot, so subtract the current image offset rather than assuming 1:1. */
    uint32_t sf_offset = SF_Ctrl_Get_Flash_Image_Offset(0, 0);
    if (model_flash_addr < sf_offset) {
        printf("[UVC_AI] ERROR: phys 0x%x below SF offset 0x%x\r\n",
               (unsigned)model_flash_addr, (unsigned)sf_offset);
        return -1;
    }
    const uint8_t *model_xip_src = (const uint8_t *)(uintptr_t)
        (FLASH_XIP_BASE + (model_flash_addr - sf_offset));

    /* Step 2: read the 32-byte model header straight from XIP. The first 8
     * uint32 words are the sizes of the 8 model parts; total = sum + 32. */
    uint32_t header[8];
    memcpy(header, model_xip_src, sizeof(header));

    uint32_t sum_parts = 0;
    for (int i = 0; i < 8; i++) {
        sum_parts += header[i];
    }
    model_size = sum_parts + sizeof(header);

    printf("[UVC_AI] model size = 0x%x (%u)\r\n", (unsigned)model_size, (unsigned)model_size);

    if (model_size <= sizeof(header) ||
        (model_size + PARTITION_HEADER_SIZE) > part_size) {
        printf("[UVC_AI] ERROR: bad model size %u vs partition %u\r\n",
               (unsigned)model_size, (unsigned)part_size);
        return -1;
    }

    /* Step 3: load the model into the NPU DIRECTLY from XIP flash — no PSRAM
     * copy. iproai_load_model_from_buffer() only memcpy-reads the buffer (it
     * copies each weights/bias/inst section into its own PSRAM NPU/CPU buffer;
     * see iproai_core.c) with no DMA from the source, so the memory-mapped XIP
     * flash address works as the source. Skipping the copy saves `model_size`
     * of peak PSRAM — essential for large models (e.g. the BIG-A object model
     * whose ~4MB resident activation buffer leaves no room for a transient
     * model-size copy). */
    if (g_model_hdl == NULL) {
        g_model_hdl = iproai_create();
        if (g_model_hdl == NULL) {
            printf("[UVC_AI] ERROR: iproai_create failed\r\n");
            return -4;
        }
    }

    printf("[UVC_AI] loading model into NPU (XIP, no copy)...\r\n");
    status = iproai_load_model_from_buffer(g_model_hdl, model_xip_src);

    if (status != IPROAI_STATUS_NO_ERROR) {
        printf("[UVC_AI] ERROR: NPU load failed (status=%d)\r\n", status);
        return -4;
    }

    uint32_t in_w = 0, in_h = 0;
    if (iproai_getInputResolution(g_model_hdl, &in_w, &in_h) == IPROAI_STATUS_NO_ERROR) {
        printf("[UVC_AI] model input resolution %lux%lu\r\n",
               (unsigned long)in_w, (unsigned long)in_h);
    }
    printf("[UVC_AI] model ready\r\n");
    return 0;
}

/****************************************/
/*    INFERENCE TASK                    */
/****************************************/

static void uvc_ai_inference_task(void *arg)
{
    (void)arg;
    uint32_t buffer_addr;
    yolov8_result_t result;
    struct iproai_net_info_t *net;
    IPROAI_Status_e status;
    int ret;

    printf("[UVC_AI] inference task started\r\n");
    g_ai_task_running = 1;

    net = iproai_getNetInfo(g_model_hdl);
    if (net == NULL) {
        printf("[UVC_AI] ERROR: getNetInfo failed\r\n");
        goto task_exit;
    }

#if !UVC_AI_MODE_CLASSIFY
    /* Initialize the detection post-processor once for the session. */
    ret = UVC_AI_PP_INIT(&g_postproc_cfg, net);
    if (ret != 0) {
        printf("[UVC_AI] ERROR: postproc init failed (ret=%d)\r\n", ret);
        goto task_exit;
    }
#else
    (void)net; (void)result; (void)ret;
    printf("[UVC_AI] classification mode (TinyNet CIFAR-10, %dx%d)\r\n", CLS_RES, CLS_RES);
#endif

#if UVC_AI_EXT_CONVERT_INT8
    /* Resolve where the model reads its image input. Dump the first layers + find
     * the img_in==1 layer; its in_layer1_mem*patch_size is the offset into
     * model->buffer (DM0) where the live frame must be written (legacy path). */
    {
        int img_layer = 0, found = 0;
        for (uint32_t i = 0; i < (uint32_t)net->layer_cnt; i++) {
            struct cpu_inst_layer_t *l = &net->layers[i];
            if (i < 8 || l->img_in == 1) {
                printf("[GDFL] layer[%lu] img_in=%d ext=%d in1=%d out=%d in(%dx%dx%d)\r\n",
                       (unsigned long)i, l->img_in, l->ext_input,
                       l->in_layer1_mem, l->out_layer_mem, l->w, l->h, l->c);
            }
            if (!found && l->img_in == 1) { img_layer = (int)i; found = 1; }
        }
        struct cpu_inst_layer_t *li = &net->layers[img_layer];
        g_img_off   = (uint32_t)li->in_layer1_mem * (uint32_t)net->patch_size;
        g_img_bytes = (uint32_t)li->w * (uint32_t)li->h * 4u;  /* pad_c=align(3,4)=4 */
        if (g_img_bytes == 0 || g_img_bytes > UVC_AI_EXT_BUF_BYTES * 2u)
            g_img_bytes = UVC_AI_EXT_BUF_BYTES;
        printf("[GDFL] IMAGE INPUT: layer[%d] img_in=%d in1=%d off=0x%lx bytes=%lu (patch=%lu mode=%d)\r\n",
               img_layer, li->img_in, li->in_layer1_mem, (unsigned long)g_img_off,
               (unsigned long)g_img_bytes, (unsigned long)net->patch_size, net->mode);
    }
#endif

    while (g_ai_task_running) {
        if (xQueueReceive(g_ai_buffer_queue, &buffer_addr, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        /* Mark busy. */
        xSemaphoreTake(g_ai_done_sem, 0);

#if UVC_AI_MODE_CLASSIFY
        /* Convert the ch1 ARGB frame -> int8 RGB into the model input buffer
         * (q = u - 128, matching int8 input scale 1/255 / zp -128), run the NPU,
         * argmax the 10 int8 logits, print the class. */
        {
            const uint8_t *argb = (const uint8_t *)(uintptr_t)buffer_addr;
            int8_t *in = (int8_t *)iproai_getInputBuffer(g_model_hdl);
            if (in != NULL) {
                for (int p = 0; p < CLS_RES * CLS_RES; p++) {
                    in[p * 3 + 0] = (int8_t)((int)argb[p * 4 + CLS_ARGB_R] - 128);
                    in[p * 3 + 1] = (int8_t)((int)argb[p * 4 + CLS_ARGB_G] - 128);
                    in[p * 3 + 2] = (int8_t)((int)argb[p * 4 + CLS_ARGB_B] - 128);
                }
                status = iproai_startCompute(g_model_hdl);
                if (status == IPROAI_STATUS_NO_ERROR) {
                    uint32_t osz = 0;
                    const int8_t *out =
                        (const int8_t *)iproai_getOutputBuffer(g_model_hdl, &osz);
                    if (out != NULL && osz >= CLS_NUM_CLASSES) {
                        int best = 0;
                        for (int i = 1; i < CLS_NUM_CLASSES; i++)
                            if (out[i] > out[best]) best = i;
                        float maxl = CLS_OUT_SCALE * ((int)out[best] - CLS_OUT_ZP);
                        float sum = 0.0f;
                        for (int i = 0; i < CLS_NUM_CLASSES; i++)
                            sum += expf(CLS_OUT_SCALE * ((int)out[i] - CLS_OUT_ZP) - maxl);
                        printf("[CLS] %s (class %d, %.1f%%)\r\n",
                               CLS_LABELS[best], best, 100.0f / sum);
                    } else {
                        printf("[UVC_AI] ERROR: bad output (osz=%lu)\r\n",
                               (unsigned long)osz);
                    }
                } else {
                    printf("[UVC_AI] ERROR: inference failed (status=%d)\r\n", status);
                }
            }
            /* No boxes in classification mode — publish an empty result so the
             * OSD / UVC streaming path keeps running. */
            g_detect_result.num_detections = 0;
            yolov8_result_t *result_ptr = &g_detect_result;
            xQueueSend(g_ai_result_queue, &result_ptr, pdMS_TO_TICKS(1));
        }
#else
        /* Feed the NPU, then run inference. */
#if UVC_AI_INTERNAL_INPUT
        /* Internal input (DM0): the NPU reads its input from model->buffer
         * (== iproai_getInputBuffer), 4 bytes/pixel (pad_c=align(3,4)=4), int8
         * with zero_point -128. Copy the ISP ch1 ARGB frame verbatim with a -128
         * offset on every byte (same 4-byte layout as the source; channels 0..2
         * are RGB, byte 3 is pad). Outputs are read back from the same buffer at
         * net_info offsets after compute (input region is consumed by layer 0). */
        {
            const uint8_t *argb = (const uint8_t *)(uintptr_t)buffer_addr;
            int8_t *in = (int8_t *)iproai_getInputBuffer(g_model_hdl);
            if (in != NULL) {
                const int nbytes = UVC_AI_IN_W * UVC_AI_IN_H * 4;
                /* ISP DMA-writes the frame; drop any stale CPU cache before read. */
                L1C_DCACHE_INVALID_RANGE((uintptr_t)argb, nbytes);
                {
                    static uint32_t s_fc = 0;
                    printf("[GDFL] src@%08lx f%lu argb=%u %u %u %u %u %u %u %u\r\n",
                           (unsigned long)buffer_addr, (unsigned long)(++s_fc),
                           argb[0], argb[1], argb[2], argb[3],
                           argb[4], argb[5], argb[6], argb[7]);
                }
                for (int i = 0; i < nbytes; i++) {
                    in[i] = (int8_t)((int)argb[i] - 128);
                }
                /* Push the input to PSRAM so the NPU's DMA sees it (not stale cache). */
                L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)in, nbytes);
            }
        }
#elif UVC_AI_EXT_CONVERT_INT8
        /* Legacy path (use_npu_accel=0): the image-input layer reads from
         * model->buffer (DM0) at g_img_off (= img_in layer's in_layer1_mem *
         * patch_size), resolved at task start. Write the frame THERE -- not the
         * DM2 ext addr, which this CPU-stem model never reads. 4 bytes/pixel
         * (pad_c=align(3,4)=4), int8 = u - 128. */
        {
            const uint8_t *argb = (const uint8_t *)(uintptr_t)buffer_addr;
            uint8_t *base = iproai_getInputBuffer(g_model_hdl);
            int8_t  *dst  = (int8_t *)(base + g_img_off);
            uint32_t npix = g_img_bytes / 4u;
            /* ISP DMA-writes the source frame to PSRAM; drop stale CPU cache. */
            L1C_DCACHE_INVALID_RANGE((uintptr_t)argb, UVC_AI_EXT_BUF_BYTES);
            {
                static uint32_t s_fc = 0;
                printf("[GDFL] src@%08lx f%lu argb=%u %u %u %u | dst@%p off=0x%lx\r\n",
                       (unsigned long)buffer_addr, (unsigned long)(++s_fc),
                       argb[0], argb[1], argb[2], argb[3], (void *)dst,
                       (unsigned long)g_img_off);
            }
#if UVC_AI_TEST_PATTERN
            /* DIAGNOSTIC: high-contrast checkerboard that MOVES every frame, written
             * to the model->buffer input patch. If the NPU reads it, [GDFL] L0 MUST
             * change + vary frame-to-frame. If still frozen at 221 234.../0.086, this
             * patch is not the input either (check the [GDFL] layer dump). */
            {
                static uint32_t tc = 0;
                tc++;
                for (uint32_t p = 0; p < npix; p++) {
                    uint32_t gx = p % UVC_AI_IN_W, gy = p / UVC_AI_IN_W;
                    int8_t v = (((gx >> 4) + (gy >> 4) + tc) & 1) ? (int8_t)120 : (int8_t)(-120);
                    dst[p * 4 + 0] = v;
                    dst[p * 4 + 1] = v;
                    dst[p * 4 + 2] = v;
                    dst[p * 4 + 3] = 0;
                }
            }
#else
            for (uint32_t p = 0; p < npix; p++) {
                dst[p * 4 + 0] = (int8_t)((int)argb[p * 4 + 0] - 128);
                dst[p * 4 + 1] = (int8_t)((int)argb[p * 4 + 1] - 128);
                dst[p * 4 + 2] = (int8_t)((int)argb[p * 4 + 2] - 128);
                dst[p * 4 + 3] = 0;
            }
#endif
            /* Push to PSRAM so the NPU/CPU stem reads our data, not stale cache. */
            L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)dst, g_img_bytes);
        }
#elif UVC_AI_EXT_XOR80
        /* int8 ext_input feed, IN PLACE. The ISP DMA-wrote the raw ARGB content
         * rows directly into g_xor_buf's letterbox window (the ch1 target addr is
         * g_xor_buf + LB_Y*IN_W*4 -- see uvc_ai_get_ch1_target); the gray bars were
         * pre-filled at init. We just sign-flip (u-128 == XOR 0x80) the content rows
         * in place and point the NPU at the whole buffer. Sharing one buffer for
         * ch1 + model input is what frees the ~0.9 MB needed for 640x384 + VGA.
         *
         * CHANNEL ORDER (verified on device 2026-06-04): fed in the RAW ISP byte
         * order (B,G,R,A). RGB reorder was tried and KILLED detections -> the .inm
         * is trained on the device's native order, so do NOT reorder. */
        {
            (void)buffer_addr;   /* == g_xor_buf + content offset; we use g_xor_buf */
            uint8_t  *base   = g_xor_buf;
            uint32_t  coff   = (uint32_t)UVC_AI_LB_Y * UVC_AI_IN_W * 4u; /* content start */
            uint32_t  cwords = ((uint32_t)UVC_AI_ISP_H * UVC_AI_IN_W * 4u) / 4u;
            uint32_t *p      = (uint32_t *)(base + coff);
            /* ISP DMA wrote raw content (ISP_W==IN_W so rows are contiguous); drop
             * stale CPU cache before the in-place sign flip. */
            L1C_DCACHE_INVALID_RANGE((uintptr_t)p, cwords * 4u);
            for (uint32_t i = 0; i < cwords; i++) {
                p[i] ^= 0x80808080u;   /* per-byte u-128, raw channel order, in place */
            }
            /* Push the whole buffer (gray bars + int8 content) to PSRAM for the NPU. */
            L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)base,
                                           (uint32_t)UVC_AI_IN_W * UVC_AI_IN_H * 4u);
            NPU_Img_Ext_Addr_Cfg((uint32_t)(uintptr_t)base);
        }
#else
        /* External input, uint8 model: point the NPU at the ISP ch1 ARGB frame. */
        NPU_Img_Ext_Addr_Cfg(buffer_addr);
#endif
        status = iproai_startCompute(g_model_hdl);

        if (status == IPROAI_STATUS_NO_ERROR) {
            uint8_t *data_buffer = iproai_getInputBuffer(g_model_hdl);
            if (data_buffer == NULL) {
                printf("[UVC_AI] ERROR: getInputBuffer failed\r\n");
                goto done;
            }

            ret = UVC_AI_PP_PROCESS(&g_postproc_cfg, net, data_buffer, &result);
            if (ret != 0) {
                printf("[UVC_AI] ERROR: postprocess failed (ret=%d)\r\n", ret);
                goto done;
            }

            UVC_AI_PP_PRINT(&result);

            /* Publish a pointer to the latest result (queue depth 1). */
            memcpy(&g_detect_result, &result, sizeof(yolov8_result_t));
            yolov8_result_t *result_ptr = &g_detect_result;
            xQueueSend(g_ai_result_queue, &result_ptr, pdMS_TO_TICKS(1));
        } else {
            printf("[UVC_AI] ERROR: inference failed (status=%d)\r\n", status);
        }

done:
#endif
        xSemaphoreGive(g_ai_done_sem);
    }

#if !UVC_AI_MODE_CLASSIFY
    UVC_AI_PP_DEINIT(&g_postproc_cfg);
#endif

task_exit:
    printf("[UVC_AI] inference task exiting\r\n");
    g_ai_task_handle = NULL;
    vTaskDelete(NULL);
}

/****************************************/
/*    PUBLIC API                        */
/****************************************/

int uvc_ai_init(void)
{
    int ret;

    if (g_ai_initialized) {
        return 0;
    }

    printf("[UVC_AI] initializing...\r\n");

    /* NPU lives behind the MM clock domain. */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);

#if UVC_AI_EXT_CONVERT_INT8
    /* Allocate the external-input frame buffer BEFORE loading the model: the load
     * programs DM2 = imgBuffer+4, so imgBuffer must already point at our buffer.
     * PSRAM via pvPortMalloc (same heap as the ISP ch1 frame -> NPU/DMA reachable).
     * +4 for the SDK's imgBuffer+4 offset, +64 slack for cache-line alignment. */
    imgBuffer = (char *)pvPortMalloc(UVC_AI_EXT_BUF_BYTES + 64);
    if (imgBuffer == NULL) {
        printf("[UVC_AI] ERROR: imgBuffer alloc failed (%d bytes)\r\n",
               (int)UVC_AI_EXT_BUF_BYTES + 64);
        return -6;
    }
    printf("[UVC_AI] imgBuffer @ %p, NPU input @ %p (%d bytes, int8 -128)\r\n",
           (void *)imgBuffer, (void *)UVC_AI_EXT_DATA(), (int)UVC_AI_EXT_BUF_BYTES);
#endif

#if UVC_AI_EXT_XOR80
    /* int8 model-input buffer (lives for the session). PSRAM, NPU/DMA reachable,
     * 4 B/pixel; +64 slack for cache-line alignment. This SAME buffer is also the
     * ISP ch1 target (see uvc_ai_get_ch1_target): the ISP DMA-writes the raw frame
     * content directly into the letterbox window of this buffer, and we sign-flip
     * it in place. Sharing one buffer (vs a separate ch1 frame) frees ~0.9 MB,
     * which is what lets the 640x384 model + VGA preview fit in 8 MB PSRAM. */
    g_xor_buf = (uint8_t *)pvPortMalloc(UVC_AI_XOR_BUF_BYTES + 64);
    if (g_xor_buf == NULL) {
        printf("[UVC_AI] ERROR: int8 feed buffer alloc failed (%d bytes)\r\n",
               (int)UVC_AI_XOR_BUF_BYTES + 64);
        return -6;
    }
    /* Pre-fill the whole buffer with the letterbox gray pad (int8 -14). The ISP
     * overwrites only the content rows each frame; the bars (if any) persist.
     * Clean to PSRAM so the NPU sees valid bars even before the first frame. */
    memset(g_xor_buf, UVC_AI_LB_PAD_I8, UVC_AI_XOR_BUF_BYTES);
    L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)g_xor_buf, UVC_AI_XOR_BUF_BYTES);
    printf("[UVC_AI] int8 feed buffer @ %p (%d bytes), ISP ch1 target @ %p (+%lu)\r\n",
           (void *)g_xor_buf, (int)UVC_AI_XOR_BUF_BYTES,
           (void *)(g_xor_buf + (uint32_t)UVC_AI_LB_Y * UVC_AI_IN_W * 4u),
           (unsigned long)((uint32_t)UVC_AI_LB_Y * UVC_AI_IN_W * 4u));
#endif

    ret = load_ai_model();
    if (ret != 0) {
        printf("[UVC_AI] model load failed (ret=%d); detection disabled\r\n", ret);
        return ret;
    }

    g_ai_buffer_queue = xQueueCreate(UVC_AI_QUEUE_SIZE, sizeof(uint32_t));
    g_ai_result_queue = xQueueCreate(UVC_AI_QUEUE_SIZE, sizeof(uint32_t));
    g_ai_done_sem = xSemaphoreCreateBinary();
    if (!g_ai_buffer_queue || !g_ai_result_queue || !g_ai_done_sem) {
        printf("[UVC_AI] ERROR: queue/sem alloc failed\r\n");
        goto fail;
    }
    xSemaphoreGive(g_ai_done_sem); /* start "done" */

    if (xTaskCreate(uvc_ai_inference_task, "uvc_ai", UVC_AI_TASK_STACK, NULL,
                    UVC_AI_TASK_PRIORITY, &g_ai_task_handle) != pdPASS) {
        printf("[UVC_AI] ERROR: task create failed\r\n");
        goto fail;
    }

    g_ai_initialized = 1;
    printf("[UVC_AI] ready, free heap %u\r\n", (unsigned)xPortGetFreeHeapSize());
    return 0;

fail:
    if (g_ai_done_sem) { vSemaphoreDelete(g_ai_done_sem); g_ai_done_sem = NULL; }
    if (g_ai_buffer_queue) { vQueueDelete(g_ai_buffer_queue); g_ai_buffer_queue = NULL; }
    if (g_ai_result_queue) { vQueueDelete(g_ai_result_queue); g_ai_result_queue = NULL; }
    return -5;
}

int uvc_ai_push_buffer(uint32_t addr)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (!g_ai_initialized || g_ai_buffer_queue == NULL) {
        return -1;
    }

    /* Called from the ISP frame-ready callback (ISR context). */
    if (xQueueSendFromISR(g_ai_buffer_queue, &addr, &xHigherPriorityTaskWoken) != pdTRUE) {
        return -2; /* still busy with the previous frame */
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    return 0;
}

int uvc_ai_get_result(uint32_t *result_addr)
{
    if (!g_ai_initialized || g_ai_result_queue == NULL) {
        return -1;
    }
    xQueueReceive(g_ai_result_queue, result_addr, portMAX_DELAY);
    return 0;
}

uint32_t uvc_ai_get_ch1_target(void)
{
#if UVC_AI_EXT_XOR80
    /* The int8 gesture model shares its input buffer with ch1: the ISP should
     * DMA the raw frame content into the letterbox window of g_xor_buf, and the
     * feed sign-flips it in place. Returns 0 if the buffer isn't allocated yet
     * (caller then allocates its own ch1 frame). */
    if (g_xor_buf == NULL) return 0;
    return (uint32_t)(uintptr_t)(g_xor_buf + (uint32_t)UVC_AI_LB_Y * UVC_AI_IN_W * 4u);
#else
    return 0;   /* big_a/yolov8 feed the raw ISP frame directly; ISP allocs its own */
#endif
}

#else /* !CONFIG_AI_SUPPORT */

int uvc_ai_init(void) { return -1; }
int uvc_ai_push_buffer(uint32_t addr) { (void)addr; return -1; }
int uvc_ai_get_result(uint32_t *result_addr) { (void)result_addr; return -1; }
uint32_t uvc_ai_get_ch1_target(void) { return 0; }

#endif /* CONFIG_AI_SUPPORT */

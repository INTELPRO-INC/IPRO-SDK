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
 * @file detect.c
 * @brief YOLOv8 person/pet detection on the IPRO7 NPU. See detect.h.
 *
 * Model path and NPU feed follow the SDK's camera demos that have run this
 * exact model on hardware (the UVC camera's yolov8 person/pet branch among
 * them):
 *
 *   - the .blai sits in the "mfg" partition at +0x1000 and is loaded straight
 *     from XIP flash (iproai_load_model_from_buffer only memcpy-reads it, so
 *     no PSRAM staging copy of the 700 KB blob is needed);
 *   - the model is uint8 ext_input: the NPU reads the raw ISP channel-1 ARGB
 *     frame through NPU_Img_Ext_Addr_Cfg(), no CPU conversion, no channel
 *     reorder (the .blai was compiled for the ISP's native byte order).
 */

#include "detect.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include "hal_flash.h"
#include "drv_sf_ctrl.h"
#include "hal_boot2.h"
#include "drv_glb.h"

#include <iproai_core.h>
#include <iproai_def.h>
#include "hal_npu.h"
#include "iproai_inst_npu.h"
#include "detect/yolov8_person_pet_detect.h"

#include "ipro_log.h"

#define LOG_TAG "DETECT"

/* The model blob starts one partition header past the partition base. */
#define DETECT_PARTITION       "mfg"
#define PARTITION_HEADER_SIZE  0x1000u

/* uint8 ext_input, 4 bytes per pixel (pad_c = align(3,4) = 4). */
#define DETECT_FRAME_BYTES     ((uint32_t)DETECT_IN_W * DETECT_IN_H * 4u)
#define DETECT_FRAME_ALIGN     32u

/*
 * ABOVE the display task. The NPU pass is ~100 layers, each dispatched by the
 * CPU and then waited for on a semaphore; the CPU share is only ~10 ms per
 * frame but it is needed a hundred times, at short notice. The display task
 * spins in the polling SPI driver for ~60 ms per frame, so if it out-ranked
 * this task every layer dispatch would queue behind a blit and a 100 ms
 * inference would take most of a second.
 */
#define DETECT_TASK_STACK      4096
#define DETECT_TASK_PRIO       (configMAX_PRIORITIES - 3)

/*
 * components/ai programs the NPU's external-input address (DM2) from
 * imgBuffer + 4 while it parses the model header, and cleans that range from
 * the cache after the weights are up. hal_npu.c only provides a weak NULL, so
 * the application has to own it: apps/test/ipro_npu_yolo does the same.
 */
char *imgBuffer = NULL;

/* ---------------------------------------------------------------------------
 * State
 * -------------------------------------------------------------------------*/

static iproai_model_hdl_t   s_hdl;
static yolov8_config_t      s_cfg;
static uint8_t             *s_frame_raw;       /* pvPortMalloc'd block      */
static uint32_t             s_frame;           /* 32-aligned NPU input      */

static QueueHandle_t        s_frame_q;         /* depth 1: one in flight    */
static SemaphoreHandle_t    s_result_mtx;
static TaskHandle_t         s_task;
static volatile bool        s_ready;
static volatile bool        s_busy;
static const char          *s_fail = "";

static detect_result_t      s_result;
static detect_stats_t       s_stats;
static volatile bool        s_print;
static detect_done_cb_t     s_done_cb;
static void                *s_done_arg;

/* Class labels the HUD and the console print. Index = model class id. */
static const char *s_class_names[YOLOV8_PERSON_PET_NUM_CLASSES] = {
    "PERSON", "PET"
};

/* ---------------------------------------------------------------------------
 * Model load
 * -------------------------------------------------------------------------*/

static int detect_load_model(void)
{
    uint32_t part_addr = 0, part_size = 0;
    uint32_t header[8];
    uint32_t model_size = 0;
    IPROAI_Status_e st;
    TickType_t t0;

    if (hal_boot2_partition_addr_active(DETECT_PARTITION, &part_addr,
                                        &part_size) != 0) {
        IPRO_LOGE(LOG_TAG, "partition '%s' not in the flashed table - flash "
                           "partition_cfg_1M_person_lcd.toml with --pt",
                  DETECT_PARTITION);
        s_fail = "NO MFG PARTITION";
        return DETECT_ERR_NOPART;
    }

    /* Physical flash address -> XIP window. boot2/OTA may offset the window
     * to the active FW slot, so subtract the current image offset rather
     * than assuming 1:1. */
    uint32_t blob_phys = part_addr + PARTITION_HEADER_SIZE;
    uint32_t sf_off    = SF_Ctrl_Get_Flash_Image_Offset(0, 0);
    if (blob_phys < sf_off) {
        IPRO_LOGE(LOG_TAG, "model @0x%06X is below the XIP offset 0x%06X",
                  (unsigned)blob_phys, (unsigned)sf_off);
        s_fail = "MODEL BELOW XIP";
        return DETECT_ERR_MODEL;
    }
    const uint8_t *xip = (const uint8_t *)(uintptr_t)
                         (FLASH_XIP_BASE + (blob_phys - sf_off));

    /* 32-byte header: eight u32 section sizes, total = sum + 32. An erased
     * partition reads as 0xFF... which fails the size check below, so a
     * board that was never given a model says so instead of hanging in the
     * NPU loader. */
    memcpy(header, xip, sizeof(header));
    for (int i = 0; i < 8; i++) {
        model_size += header[i];
    }
    model_size += sizeof(header);

    if (model_size <= sizeof(header) ||
        model_size + PARTITION_HEADER_SIZE > part_size) {
        IPRO_LOGE(LOG_TAG, "no valid model in '%s' (size word 0x%08X, slot %u B)"
                           " - flash person_pet_detect with --mfg",
                  DETECT_PARTITION, (unsigned)model_size, (unsigned)part_size);
        s_fail = "NO MODEL IN MFG";
        return DETECT_ERR_MODEL;
    }
    s_stats.model_bytes = model_size;
    IPRO_LOGI(LOG_TAG, "model: %u B at flash 0x%06X (partition '%s' %u KB)",
              (unsigned)model_size, (unsigned)blob_phys, DETECT_PARTITION,
              (unsigned)(part_size / 1024u));

    s_hdl = iproai_create();
    if (!s_hdl) {
        s_fail = "NPU CREATE FAILED";
        return DETECT_ERR_NOMEM;
    }

    IPRO_LOGI(LOG_TAG, "loading into the NPU (about a second)...");
    t0 = xTaskGetTickCount();
    st = iproai_load_model_from_buffer(s_hdl, xip);
    s_stats.load_ms = (uint32_t)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS);
    if (st != IPROAI_STATUS_NO_ERROR) {
        IPRO_LOGE(LOG_TAG, "NPU load failed (status %d)", (int)st);
        s_fail = "NPU LOAD FAILED";
        return DETECT_ERR_MODEL;
    }

    struct iproai_net_info_t *net = iproai_getNetInfo(s_hdl);
    if (!net) {
        s_fail = "NO NET INFO";
        return DETECT_ERR_MODEL;
    }
    IPRO_LOGI(LOG_TAG, "loaded in %u ms: input %dx%dx%d, %d outputs, "
                       "ext_input %d, %d layers",
              (unsigned)s_stats.load_ms, net->w, net->h, net->c,
              net->output_num, net->ext_input, net->layer_cnt);

    /* The ISP scaler, the input buffer and the box decode are all sized for
     * a 640x360 ext_input frame. The model's own header says 640x353x3
     * (board, 2026-09-18): its first layer pads 353 rows, so the NPU reads
     * the top 353 of the 360 rows the ISP writes - the same feed the camera
     * demos validated. Anything wider or taller than the frame, or not
     * ext_input, would read past the buffer or need a CPU feed. */
    if (net->w != DETECT_IN_W || net->h > DETECT_IN_H || !net->ext_input) {
        IPRO_LOGE(LOG_TAG, "model geometry %dx%d ext=%d does not fit the "
                           "640x360 ext_input frame this app feeds",
                  net->w, net->h, net->ext_input);
        s_fail = "WRONG MODEL";
        return DETECT_ERR_MODEL;
    }

    if (yolov8_person_pet_init(&s_cfg, net) != 0) {
        s_fail = "POSTPROC INIT FAILED";
        return DETECT_ERR_MODEL;
    }
    s_cfg.class_names = s_class_names;
    return DETECT_OK;
}

/* ---------------------------------------------------------------------------
 * Result publication
 * -------------------------------------------------------------------------*/

static int16_t clamp_i16(float v, int hi)
{
    if (!(v >= 0.0f)) {            /* also catches NaN */
        return 0;
    }
    if (v > (float)hi) {
        return (int16_t)hi;
    }
    return (int16_t)v;
}

static void publish(const yolov8_result_t *r, uint32_t npu_ms, uint32_t post_ms)
{
    detect_result_t out;
    int n = r->num_detections;

    if (n > DETECT_MAX) {
        n = DETECT_MAX;
    }
    memset(&out, 0, sizeof(out));
    for (int i = 0; i < n; i++) {
        const yolov8_detection_t *d = &r->detections[i];
        detect_box_t *b = &out.box[out.count];
        int16_t x1 = clamp_i16(d->box.x1, DETECT_IN_W - 1);
        int16_t y1 = clamp_i16(d->box.y1, DETECT_IN_H - 1);
        int16_t x2 = clamp_i16(d->box.x2, DETECT_IN_W - 1);
        int16_t y2 = clamp_i16(d->box.y2, DETECT_IN_H - 1);

        if (x2 <= x1 || y2 <= y1) {
            continue;                       /* degenerate, nothing to draw */
        }
        b->x1 = x1; b->y1 = y1; b->x2 = x2; b->y2 = y2;
        b->cls  = (uint8_t)d->class_id;
        b->conf = (uint8_t)clamp_i16(d->confidence * 100.0f, 100);
        if (b->cls == DETECT_CLASS_PERSON) {
            out.persons++;
        } else {
            out.pets++;
        }
        out.count++;
    }
    out.npu_ms  = npu_ms;
    out.post_ms = post_ms;

    xSemaphoreTake(s_result_mtx, portMAX_DELAY);
    out.seq = s_result.seq + 1u;
    s_result = out;
    xSemaphoreGive(s_result_mtx);
}

static void print_result(const detect_result_t *r)
{
    printf("[DETECT] #%lu %u box (%u person, %u pet) npu %lu ms post %lu ms\r\n",
           (unsigned long)r->seq, r->count, r->persons, r->pets,
           (unsigned long)r->npu_ms, (unsigned long)r->post_ms);
    for (int i = 0; i < r->count; i++) {
        const detect_box_t *b = &r->box[i];
        printf("         %-6s %3u%%  [%3d,%3d]-[%3d,%3d]\r\n",
               s_class_names[b->cls < YOLOV8_PERSON_PET_NUM_CLASSES ? b->cls : 0],
               b->conf, b->x1, b->y1, b->x2, b->y2);
    }
}

/* ---------------------------------------------------------------------------
 * Inference task
 * -------------------------------------------------------------------------*/

static void detect_task(void *arg)
{
    struct iproai_net_info_t *net = iproai_getNetInfo(s_hdl);
    static yolov8_result_t raw;          /* 50 x ~32 B: keep it off the stack */
    TickType_t last_done = 0;
    uint32_t addr;

    (void)arg;

    for (;;) {
        if (xQueueReceive(s_frame_q, &addr, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        TickType_t t0 = xTaskGetTickCount();
        NPU_Img_Ext_Addr_Cfg(addr);
        IPROAI_Status_e st = iproai_startCompute(s_hdl);
        TickType_t t1 = xTaskGetTickCount();

        if (st != IPROAI_STATUS_NO_ERROR) {
            s_stats.errors++;
            IPRO_LOGE(LOG_TAG, "NPU compute failed (status %d)", (int)st);
        } else {
            uint8_t *data = iproai_getInputBuffer(s_hdl);
            if (!data || yolov8_process_6out(&s_cfg, net, data, &raw) != 0) {
                s_stats.errors++;
                IPRO_LOGE(LOG_TAG, "box decode failed");
            } else {
                TickType_t t2 = xTaskGetTickCount();
                publish(&raw, (uint32_t)((t1 - t0) * portTICK_PERIOD_MS),
                              (uint32_t)((t2 - t1) * portTICK_PERIOD_MS));
                s_stats.frames++;
                if (last_done) {
                    uint32_t period = (uint32_t)((t2 - last_done) * portTICK_PERIOD_MS);
                    if (period) {
                        /* EMA over ~4 frames, in tenths of a frame per second */
                        uint32_t inst = 10000u / period;
                        s_stats.fps_x10 = s_stats.fps_x10
                                        ? (s_stats.fps_x10 * 3u + inst) / 4u
                                        : inst;
                    }
                }
                last_done = t2;
                if (s_print) {
                    detect_result_t snap;
                    detect_get_result(&snap);
                    print_result(&snap);
                }
            }
        }

        s_busy = false;
        if (s_done_cb) {
            s_done_cb(s_done_arg);          /* camera re-arms channel 1 */
        }
    }
}

/* ---------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------*/

int detect_init(void)
{
    int rc;

    if (s_ready) {
        return DETECT_OK;
    }

    /* The NPU sits in the multimedia clock domain. */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);

    /* Input frame first, model second. The model's activation buffer is a
     * single ~3 MB allocation; with only these two large blocks on a fresh
     * heap there is nothing to fragment yet, and the load-time DM2 setup
     * needs imgBuffer to point at real memory. */
    if (!s_frame_raw) {
        s_frame_raw = pvPortMalloc(DETECT_FRAME_BYTES + DETECT_FRAME_ALIGN + 4u);
        if (!s_frame_raw) {
            IPRO_LOGE(LOG_TAG, "PSRAM alloc %u B for the NPU input failed",
                      (unsigned)DETECT_FRAME_BYTES);
            s_fail = "NO PSRAM FOR INPUT";
            return DETECT_ERR_NOMEM;
        }
        uintptr_t p = ((uintptr_t)s_frame_raw + 4u + DETECT_FRAME_ALIGN - 1u)
                      & ~(uintptr_t)(DETECT_FRAME_ALIGN - 1u);
        s_frame   = (uint32_t)p;
        imgBuffer = (char *)(p - 4u);       /* runtime reads DM2 = imgBuffer + 4 */
        memset((void *)p, 0, DETECT_FRAME_BYTES);
    }

    if (!s_result_mtx) {
        s_result_mtx = xSemaphoreCreateMutex();
    }
    if (!s_frame_q) {
        s_frame_q = xQueueCreate(1, sizeof(uint32_t));
    }
    if (!s_result_mtx || !s_frame_q) {
        s_fail = "NO RAM FOR QUEUES";
        return DETECT_ERR_NOMEM;
    }

    rc = detect_load_model();
    if (rc != DETECT_OK) {
        return rc;
    }
    s_stats.free_heap = (uint32_t)xPortGetFreeHeapSize();
    IPRO_LOGI(LOG_TAG, "PSRAM heap left after the model: %u KB",
              (unsigned)(s_stats.free_heap / 1024u));

    if (xTaskCreate(detect_task, "detect", DETECT_TASK_STACK, NULL,
                    DETECT_TASK_PRIO, &s_task) != pdPASS) {
        s_fail = "TASK CREATE FAILED";
        return DETECT_ERR_NOMEM;
    }

    s_ready = true;
    s_fail  = "";
    return DETECT_OK;
}

bool        detect_is_ready(void)    { return s_ready; }
const char *detect_fail_reason(void) { return s_fail; }
uint32_t    detect_input_addr(void)  { return s_frame; }
bool        detect_busy(void)        { return s_busy; }

int detect_push_frame(uint32_t addr)
{
    BaseType_t woken = pdFALSE;

    if (!s_ready || s_busy) {
        return DETECT_ERR_STATE;
    }
    s_busy = true;
    if (xQueueSendFromISR(s_frame_q, &addr, &woken) != pdTRUE) {
        s_busy = false;
        return DETECT_ERR_STATE;
    }
    portYIELD_FROM_ISR(woken);
    return DETECT_OK;
}

void detect_set_done_cb(detect_done_cb_t cb, void *arg)
{
    s_done_cb  = cb;
    s_done_arg = arg;
}

void detect_get_result(detect_result_t *out)
{
    if (!out) {
        return;
    }
    if (!s_result_mtx) {
        memset(out, 0, sizeof(*out));
        return;
    }
    xSemaphoreTake(s_result_mtx, portMAX_DELAY);
    *out = s_result;
    xSemaphoreGive(s_result_mtx);
}

int detect_set_conf(int pct)
{
    if (pct < 1 || pct > 99) {
        return DETECT_ERR_STATE;
    }
    /* Single float store, read by the task on its next frame. */
    s_cfg.conf_threshold = (float)pct / 100.0f;
    return DETECT_OK;
}

int detect_get_conf(void)
{
    return (int)(s_cfg.conf_threshold * 100.0f + 0.5f);
}

void detect_set_print(bool on) { s_print = on; }
bool detect_get_print(void)    { return s_print; }

void detect_get_stats(detect_stats_t *out)
{
    if (out) {
        *out = s_stats;
    }
}

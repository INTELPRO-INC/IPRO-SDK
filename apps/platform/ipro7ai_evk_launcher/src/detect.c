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
 * @brief YOLOv8 person/pet detection from the SD card. See detect.h.
 *
 * Model path: iproai_load_model_from_file() reads the whole .inm into PSRAM
 * and the runtime copies each section out of it. Feed: uint8 ext_input, the
 * NPU reads the ISP channel-1 ARGB frame in place (the model was compiled
 * for the ISP's native byte order). Everything else is what
 * apps/ai/ipro7ai_evk_person_detect validated on this board.
 */

#include "detect.h"
#include "npu_share.h"

#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include "drv_glb.h"

#include <iproai_core.h>
#include <iproai_def.h>
#include "hal_npu.h"
#include "detect/yolov8_person_pet_detect.h"

#include "evk.h"
#include "ipro_log.h"

#define LOG_TAG "DETECT"

/* uint8 ext_input, 4 bytes per pixel (pad_c = align(3,4) = 4). */
#define DETECT_FRAME_BYTES     ((uint32_t)DETECT_IN_W * DETECT_IN_H * 4u)
#define DETECT_FRAME_ALIGN     32u

/* Same priority as the gesture task: both dispatch ~100 NPU layers from the
 * CPU and must not queue behind the display task's polling SPI blits. */
#define DETECT_TASK_STACK      4096
#define DETECT_TASK_PRIO       (configMAX_PRIORITIES - 3)

#define DETECT_CONF_DEFAULT    25

/* Minimum time between two detections. The NPU is shared with the gesture
 * classifier; back-to-back ~100 ms detections would leave it only the gaps.
 * 5 detections a second is plenty for boxes on a 10 fps preview. */
#define DETECT_PERIOD_DEFAULT_MS 200

/* Defined in gesture.c (one definition per image); the runtime reads DM2
 * from imgBuffer + 4 while it parses a model, so it must point at THIS
 * model's frame while this model loads. */
extern char *imgBuffer;

/* ---------------------------------------------------------------------------
 * State
 * -------------------------------------------------------------------------*/

static iproai_model_hdl_t        s_hdl;
static struct iproai_net_info_t *s_net;
static yolov8_config_t           s_cfg;
static uint8_t                  *s_frame_raw;
static uint32_t                  s_frame;

static QueueHandle_t             s_frame_q;
static SemaphoreHandle_t         s_result_mtx;
static volatile bool             s_ready;
static volatile bool             s_busy;
static const char               *s_fail = "";

static detect_result_t           s_result;
static detect_stats_t            s_stats;
static volatile bool             s_print;
static volatile int              s_conf_pct = DETECT_CONF_DEFAULT;
static volatile uint32_t         s_period_ms = DETECT_PERIOD_DEFAULT_MS;
static detect_done_cb_t          s_done_cb;
static void                     *s_done_arg;

static const char *s_class_names[YOLOV8_PERSON_PET_NUM_CLASSES] = { "PERSON", "PET" };

const char *detect_class_name(int cls)
{
    return (cls >= 0 && cls < YOLOV8_PERSON_PET_NUM_CLASSES) ? s_class_names[cls] : "?";
}

/* ---------------------------------------------------------------------------
 * Model load
 * -------------------------------------------------------------------------*/

static int load_model(void)
{
    IPROAI_Status_e st;
    TickType_t t0;

#if EVK_HAS_SDCARD
    if (!evk_sd_present()) {
        s_fail = "NO SD CARD";
        return DETECT_ERR_NOFILE;
    }
    if (evk_sd_mount() != EVK_SD_OK) {
        s_fail = "SD MOUNT FAILED";
        return DETECT_ERR_NOFILE;
    }
    if (!evk_sd_exists(DETECT_SD_PATH)) {
        s_fail = "NO " DETECT_SD_PATH;
        return DETECT_ERR_NOFILE;
    }
#else
    s_fail = "NO SD SUPPORT";
    return DETECT_ERR_NOFILE;
#endif

    s_hdl = iproai_create();
    if (!s_hdl) {
        s_fail = "NPU CREATE FAILED";
        return DETECT_ERR_NOMEM;
    }
    IPRO_LOGI(LOG_TAG, "loading " DETECT_SD_PATH " into the NPU (about a second)...");
    t0 = xTaskGetTickCount();
    st = iproai_load_model_from_file(s_hdl, DETECT_SD_PATH);
    s_stats.load_ms = (uint32_t)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS);
    if (st != IPROAI_STATUS_NO_ERROR) {
        IPRO_LOGE(LOG_TAG, "NPU load failed (status %d)", (int)st);
        s_fail = "NPU LOAD FAILED";
        goto fail;
    }
    s_net = iproai_getNetInfo(s_hdl);
    if (!s_net) {
        s_fail = "NO NET INFO";
        goto fail;
    }
    IPRO_LOGI(LOG_TAG, "loaded in %u ms: input %dx%dx%d, %d outputs, ext_input %d, %d layers "
                       "(%d NPU), unsigned %d relu_n %d tflite %d mode %d",
              (unsigned)s_stats.load_ms, s_net->w, s_net->h, s_net->c, s_net->output_num,
              s_net->ext_input, s_net->layer_cnt, s_net->npu_layer_cnt, s_net->unsign_input,
              s_net->relu_n, s_net->use_tflite, s_net->mode);

    /* The model's own header says 640x353 (its first layer pads 353 rows),
     * so the NPU reads the top 353 of the 360 rows the ISP writes. */
    if (s_net->w != DETECT_IN_W || s_net->h > DETECT_IN_H || !s_net->ext_input) {
        IPRO_LOGE(LOG_TAG, "model geometry %dx%d ext=%d does not fit the 640x360 "
                           "ext_input frame this app feeds",
                  s_net->w, s_net->h, s_net->ext_input);
        s_fail = "WRONG MODEL";
        goto fail;
    }
    if (yolov8_person_pet_init(&s_cfg, s_net) != 0) {
        s_fail = "POSTPROC INIT FAILED";
        goto fail;
    }
    s_cfg.class_names = s_class_names;
    return DETECT_OK;

fail:
    iproai_free(s_hdl);
    s_hdl = NULL;
    s_net = NULL;
    return DETECT_ERR_MODEL;
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
            continue;
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
        printf("         %-6s %3u%%  [%3d,%3d]-[%3d,%3d]\r\n", detect_class_name(b->cls),
               b->conf, b->x1, b->y1, b->x2, b->y2);
    }
}

/* ---------------------------------------------------------------------------
 * Inference task - the only FPU user after boot
 * -------------------------------------------------------------------------*/

static void detect_task(void *arg)
{
    static yolov8_result_t raw;          /* 50 x ~32 B: keep it off the stack */
    TickType_t last_done = 0;
    uint32_t addr;
    int conf_applied = -1;

    (void)arg;
    for (;;) {
        if (xQueueReceive(s_frame_q, &addr, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        if (conf_applied != s_conf_pct) {
            conf_applied = s_conf_pct;
            s_cfg.conf_threshold = (float)conf_applied / 100.0f;
        }

        TickType_t t0 = xTaskGetTickCount();
        npu_share_begin(s_net, addr);
        TickType_t tw = xTaskGetTickCount();
        IPROAI_Status_e st = iproai_startCompute(s_hdl);
        npu_share_end();
        TickType_t t1 = xTaskGetTickCount();
        s_stats.wait_ms = (uint32_t)((tw - t0) * portTICK_PERIOD_MS);

        if (st != IPROAI_STATUS_NO_ERROR) {
            s_stats.errors++;
            IPRO_LOGE(LOG_TAG, "NPU compute failed (status %d)", (int)st);
        } else {
            uint8_t *data = iproai_getInputBuffer(s_hdl);
            if (!data || yolov8_process_6out(&s_cfg, s_net, data, &raw) != 0) {
                s_stats.errors++;
                IPRO_LOGE(LOG_TAG, "box decode failed");
            } else {
                TickType_t t2 = xTaskGetTickCount();
                publish(&raw, (uint32_t)((t1 - tw) * portTICK_PERIOD_MS),
                              (uint32_t)((t2 - t1) * portTICK_PERIOD_MS));
                s_stats.frames++;
                if (last_done) {
                    uint32_t period = (uint32_t)((t2 - last_done) * portTICK_PERIOD_MS);
                    if (period) {
                        uint32_t inst = 10000u / period;
                        s_stats.fps_x10 = s_stats.fps_x10
                                        ? (s_stats.fps_x10 * 3u + inst) / 4u : inst;
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
        /* Pace: give the gesture classifier the NPU for a while. */
        {
            TickType_t spent = xTaskGetTickCount() - t0;
            TickType_t want  = pdMS_TO_TICKS(s_period_ms);
            if (spent < want) {
                vTaskDelay(want - spent);
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
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);

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
        s_frame = (uint32_t)p;
        memset((void *)p, 0, DETECT_FRAME_BYTES);
    }
    /* The runtime programs DM2 = imgBuffer + 4 while it parses THIS model;
     * gesture_init() pointed it at the gesture frame for its own load. The
     * ext-input address is re-applied per inference by npu_share anyway. */
    imgBuffer = (char *)(uintptr_t)(s_frame - 4u);

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

    rc = load_model();
    if (rc != DETECT_OK) {
        return rc;
    }
    s_stats.free_heap = (uint32_t)xPortGetFreeHeapSize();
    IPRO_LOGI(LOG_TAG, "heap left after the model: %u KB", (unsigned)(s_stats.free_heap / 1024u));

    if (xTaskCreate(detect_task, "detect", DETECT_TASK_STACK, NULL, DETECT_TASK_PRIO,
                    NULL) != pdPASS) {
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
int         detect_get_conf(void)    { return s_conf_pct; }
void        detect_set_print(bool on){ s_print = on; }
bool        detect_get_print(void)   { return s_print; }

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

int detect_set_period_ms(uint32_t ms)
{
    if (ms > 5000) {
        return DETECT_ERR_STATE;
    }
    s_period_ms = ms;
    return DETECT_OK;
}

uint32_t detect_get_period_ms(void)
{
    return s_period_ms;
}

int detect_set_conf(int pct)
{
    if (pct < 1 || pct > 99) {
        return DETECT_ERR_STATE;
    }
    s_conf_pct = pct;                    /* the task folds it into the float config */
    return DETECT_OK;
}

void detect_get_stats(detect_stats_t *out)
{
    if (out) {
        *out = s_stats;
    }
}

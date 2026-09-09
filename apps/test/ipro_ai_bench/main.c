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

/* ipro_ai_bench —— 主流模型的板上效能量測。
 *
 * 與 ipro_npu_opverify 的分工：
 *   opverify  逐算子的**正確性**，全部案例共用一份 32x32x8 輸入。
 *   這一支    整模型的**速度與派工**，每個模型用自己的原生輸入尺寸。
 *
 * 輸入內容刻意用板上 PRNG 填，不燒進 flash —— 卷積的時間與資料無關，
 * 而 320x320x3 的輸入光是存起來就要 300 KB，會把模型擠出分割區。
 *
 * 板上指令：
 *   bench_list          列出內建模型
 *   bench_run <name>    跑單一模型
 *   bench_all           全部跑一遍
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "freertos_app_hooks.h"
#include "shell.h"

#include "iproai_core.h"
#include "iproai_def.h"
#include "iproai_cycle_profile.h"
#include "hal_npu.h"            /* NPU_Img_Ext_Addr_Cfg, imgBuffer */
#include "drv_glb.h"            /* GLB_PER_Clock_UnGate */
#include <system_soc.h>         /* L1C_DCACHE_CLEAN_INVALID_RANGE */
#include "bench_table.h"

#define BENCH_REPS 5          /* 取中位數；首次跑帶著 PSRAM/快取的冷啟成本 */

extern char *imgBuffer;

static int      s_hw_ready;
static uint8_t *s_feed;        /* 4 bytes 表頭 + 最大輸入 */
static uint32_t s_feed_cap;

/* 板上 PRNG：xorshift32。時間與資料無關，但要**確定性**，
 * 才不會兩次量測看到不同的快取行為就以為是雜訊。 */
static uint32_t s_rng = 0x1234567u;
static uint8_t rnd8(void)
{
    s_rng ^= s_rng << 13; s_rng ^= s_rng >> 17; s_rng ^= s_rng << 5;
    return (uint8_t)(s_rng & 0xff);
}

static uint32_t max_input_bytes(void)
{
    uint32_t mx = 0;
    for (size_t i = 0; i < BENCH_MODEL_COUNT; i++) {
        const struct bench_model *m = &g_bench_models[i];
        const uint32_t n = (uint32_t)m->in_w * m->in_h * m->in_c;
        if (n > mx) { mx = n; }
    }
    return mx;
}

static int bench_hw_init(void)
{
    if (s_hw_ready) { return 0; }
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);

    s_feed_cap = max_input_bytes();
    s_feed = pvPortMalloc(4 + s_feed_cap);
    if (s_feed == NULL) {
        printf("[bench] cannot allocate %lu-byte feed buffer\r\n",
               (unsigned long)(4 + s_feed_cap));
        return -1;
    }
    imgBuffer = (char *)s_feed;
    iproai_npu_init();
    s_hw_ready = 1;
    return 0;
}

static int cmp_u64(const void *a, const void *b)
{
    const uint64_t x = *(const uint64_t *)a, y = *(const uint64_t *)b;
    return (x > y) - (x < y);
}

struct bench_result {
    int      ran;
    int      layers, on_npu;
    uint64_t cyc_total, cyc_npu;
    uint32_t dispatches;
};

static void run_model(const struct bench_model *m, struct bench_result *r)
{
    memset(r, 0, sizeof *r);
    const uint32_t in_bytes = (uint32_t)m->in_w * m->in_h * m->in_c;

    /* ext_input 的來源緩衝。內容是 PRNG —— 見檔頭。 */
    s_rng = 0x1234567u ^ (uint32_t)m->inm_off;
    for (uint32_t i = 0; i < in_bytes; i++) { s_feed[4 + i] = rnd8(); }
    L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)(s_feed + 4), in_bytes);

    uint64_t tot[BENCH_REPS], npu[BENCH_REPS];

    for (int k = 0; k < BENCH_REPS; k++) {
        iproai_model_hdl_t hdl = iproai_create();
        if (hdl == NULL) { printf("  %-18s create failed\r\n", m->name); return; }

        if (iproai_load_model_from_buffer(hdl, (uint8_t *)(g_bench_models_blob + m->inm_off))
            != IPROAI_STATUS_NO_ERROR) {
            printf("  %-18s load failed\r\n", m->name);
            iproai_free(hdl);
            return;
        }
        struct iproai_net_info_t *net = iproai_getNetInfo(hdl);

        if (k == 0) {
            for (int i = 0; i < net->layer_cnt; i++) {
                r->layers++;
                r->on_npu += (net->layers[i].NPU_on != 0);
            }
        }
        NPU_Img_Ext_Addr_Cfg((uint32_t)(uintptr_t)(s_feed + 4));

        iproai_cycle_reset();
        if (iproai_startCompute(hdl) != IPROAI_STATUS_NO_ERROR) {
            printf("  %-18s compute failed\r\n", m->name);
            iproai_free(hdl);
            return;
        }
        tot[k] = iproai_cycle_total();
        npu[k] = iproai_cycle_npu();
        r->dispatches = iproai_cycle_npu_dispatches();
        iproai_free(hdl);
    }

    qsort(tot, BENCH_REPS, sizeof(uint64_t), cmp_u64);
    qsort(npu, BENCH_REPS, sizeof(uint64_t), cmp_u64);
    r->cyc_total = tot[BENCH_REPS / 2];
    r->cyc_npu   = npu[BENCH_REPS / 2];
    r->ran = 1;
}

static void print_row(const struct bench_model *m, const struct bench_result *r)
{
    if (!r->ran) { return; }
    const uint64_t cpu = (r->cyc_total > r->cyc_npu) ? r->cyc_total - r->cyc_npu : 0;
    printf("BENCH %-18s %dx%dx%d %5d %5d %10llu %10llu %10llu %4lu\r\n",
           m->name, m->in_w, m->in_h, m->in_c, r->layers, r->on_npu,
           (unsigned long long)r->cyc_total, (unsigned long long)r->cyc_npu,
           (unsigned long long)cpu, (unsigned long)r->dispatches);
}

static int cmd_bench_list(int argc, char *argv[])
{
    (void)argc; (void)argv;
    printf("%-18s %-12s %-20s %10s\r\n", "name", "input", "app_type", "inm bytes");
    for (size_t i = 0; i < BENCH_MODEL_COUNT; i++) {
        const struct bench_model *m = &g_bench_models[i];
        char in[16];
        snprintf(in, sizeof in, "%dx%dx%d", m->in_w, m->in_h, m->in_c);
        printf("%-18s %-12s %-20s %10lu\r\n", m->name, in, m->app_type,
               (unsigned long)m->inm_len);
    }
    return 0;
}

static int cmd_bench_run(int argc, char *argv[])
{
    if (argc < 2) { printf("usage: bench_run <name>\r\n"); return -1; }
    if (bench_hw_init() != 0) { return -1; }
    for (size_t i = 0; i < BENCH_MODEL_COUNT; i++) {
        if (strcmp(argv[1], g_bench_models[i].name) != 0) { continue; }
        struct bench_result r;
        run_model(&g_bench_models[i], &r);
        print_row(&g_bench_models[i], &r);
        return 0;
    }
    printf("no such model: %s\r\n", argv[1]);
    return -1;
}

#ifdef CONFIG_AI_MODEL_FROM_FILE
/* Models past the flash "mfg" slot (843,776 B stock, 1,822,720 B with the
 * no-OTA "bigmodel" layout) live on a card. A 224x224 ImageNet classifier is
 * 2.8-4.3 MB, so that is the only way to run one.
 *
 * The card must already be mounted; this command does not mount it, because
 * the volume name and the SD pinout are board-specific. */
static int cmd_bench_file(int argc, char *argv[])
{
    if (argc < 5) {
        printf("usage: bench_file <path> <w> <h> <c>\r\n");
        printf("  e.g. bench_file sd:/mnv2_224.inm 224 224 3\r\n");
        return -1;
    }
    const int w = atoi(argv[2]), h = atoi(argv[3]), c = atoi(argv[4]);
    if (w <= 0 || h <= 0 || c <= 0) {
        printf("bad input geometry\r\n");
        return -1;
    }
    if (bench_hw_init() != 0) { return -1; }

    const uint32_t need = (uint32_t)w * h * c;
    if (need > s_feed_cap) {
        /* The built-in models sized this buffer; a card model can be larger. */
        vPortFree(s_feed);
        s_feed = pvPortMalloc(4 + need);
        if (s_feed == NULL) {
            printf("cannot grow the feed buffer to %lu bytes\r\n",
                   (unsigned long)(4 + need));
            s_feed_cap = 0;
            return -1;
        }
        s_feed_cap = need;
        imgBuffer = (char *)s_feed;
    }

    s_rng = 0x1234567u;
    for (uint32_t i = 0; i < need; i++) { s_feed[4 + i] = rnd8(); }
    L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)(s_feed + 4), need);

    iproai_model_hdl_t hdl = iproai_create();
    if (hdl == NULL) { printf("create failed\r\n"); return -1; }
    if (iproai_load_model_from_file(hdl, argv[1]) != IPROAI_STATUS_NO_ERROR) {
        iproai_free(hdl);
        return -1;
    }
    struct iproai_net_info_t *net = iproai_getNetInfo(hdl);
    int layers = 0, on_npu = 0;
    for (int i = 0; i < net->layer_cnt; i++) {
        layers++;
        on_npu += (net->layers[i].NPU_on != 0);
    }
    NPU_Img_Ext_Addr_Cfg((uint32_t)(uintptr_t)(s_feed + 4));

    iproai_cycle_reset();
    if (iproai_startCompute(hdl) != IPROAI_STATUS_NO_ERROR) {
        printf("compute failed\r\n");
        iproai_free(hdl);
        return -1;
    }
    const uint64_t tot = iproai_cycle_total(), npu = iproai_cycle_npu();
    printf("BENCH %-18s %dx%dx%d %5d %5d %10llu %10llu %10llu %4lu\r\n",
           argv[1], w, h, c, layers, on_npu,
           (unsigned long long)tot, (unsigned long long)npu,
           (unsigned long long)(tot > npu ? tot - npu : 0),
           (unsigned long)iproai_cycle_npu_dispatches());
    iproai_free(hdl);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_bench_file, bench_file, Benchmark a model read from a card);
#endif /* CONFIG_AI_MODEL_FROM_FILE */

static int cmd_bench_all(int argc, char *argv[])
{
    (void)argc; (void)argv;
    if (bench_hw_init() != 0) { return -1; }
    printf("BENCHHDR name input layers on_npu total_med npu_med cpu_med dispatches\r\n");
    for (size_t i = 0; i < BENCH_MODEL_COUNT; i++) {
        struct bench_result r;
        run_model(&g_bench_models[i], &r);
        print_row(&g_bench_models[i], &r);
    }
    printf("BENCHEND\r\n");
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_bench_list, bench_list, List the built-in models);
SHELL_CMD_EXPORT_ALIAS(cmd_bench_run,  bench_run,  Benchmark one model);
SHELL_CMD_EXPORT_ALIAS(cmd_bench_all,  bench_all,  Benchmark every model);

static void bench_task(void *arg)
{
    (void)arg;
    printf("\r\nipro_ai_bench ready - %u models\r\n", (unsigned)BENCH_MODEL_COUNT);
    printf("commands: bench_list / bench_run <name> / bench_all\r\n");
#ifdef CONFIG_AI_MODEL_FROM_FILE
    printf("          bench_file <path> <w> <h> <c>   (model on a mounted card)\r\n");
#endif
    vTaskDelete(NULL);
}

int main(void)
{
    ipro_platform_init();
    xTaskCreate(bench_task, "bench", 2048, NULL, 5, NULL);
    vTaskStartScheduler();
    while (1) { }
}

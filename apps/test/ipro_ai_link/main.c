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

/* ipro_ai_link —— 讓主機端工具直接驅動板子跑推論。
 *
 * 這支是**交付給客戶**的韌體：燒一次，之後換模型、換測試資料、跑推論、
 * 取回結果全部從 IPRO AI Studio 下指令，不必重燒。
 *
 * 與另外兩支測試 app 的分工：
 *   ipro_npu_opverify   逐算子正確性，模型與 golden 都燒在 flash 裡
 *   ipro_ai_bench       整模型效能，輸入用板上 PRNG 生
 *   這一支              **模型與輸入都從卡片來**，輸出原樣送回主機比對
 *
 * 板上指令：
 *   ai_mount / ai_unmount / ai_ls [dir]
 *   ai_load  sd:/model.inm
 *   ai_input sd:/input.bin
 *   ai_run   [reps]
 *   ai_out   [max_bytes]
 *   ai_info
 *
 * 🔴 路徑一律用原生 FatFS 的 "sd:/"，不是 EVFS 的 "/sdcard/"。
 *    iproai_load_model_from_file() 走的是原生 FatFS，而 SDK 裡其他 SD 消費者
 *    走 EVFS —— 同一張卡兩套命名空間，混用會 f_open 失敗而訊息指不到原因。
 *
 * 輸出框架都是機器可讀的前綴（AIINFO / AIRUN / AIOUT / AIDET），
 * 主機端 ipro_ai_core/device.py 直接剖析。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "freertos_app_hooks.h"
#include "shell.h"

#include "ff.h"                 /* 原生 FatFS —— 與 iproai_load_model_from_file 同一套 */
#include "iproai_core.h"
#include "iproai_def.h"
#include "iproai_cycle_profile.h"
#include "iproai_inst_util.h"   /* BALI_CHN_ALIGN */
#include "hal_npu.h"            /* NPU_Img_Ext_Addr_Cfg, imgBuffer */
#include "drv_glb.h"            /* GLB_PER_Clock_UnGate */
#include <system_soc.h>         /* L1C_DCACHE_CLEAN_INVALID_RANGE */
#include <sys/time.h>

#ifdef CONFIG_CHERRYUSB_DEVICE_CDC
void cdc_link_init(void);          /* src/cdc_link.c */
#endif


/* 🔴 FatFS 建檔時要蓋時間戳：get_fattime() -> time() -> _gettimeofday()。
 * 沒有連結 FreeRTOS-POSIX 的話，newlib 會把 _gettimeofday 解析到 libgloss 的
 * semihosting stub —— 那支會發 `ecall`，在裸機上就是 M-mode ECALL 例外。
 *
 * 症狀非常容易誤判：f_open(FA_CREATE_ALWAYS) 當場掛掉，而例外處理器印的是
 * 「堆疊指標超出範圍、EPC 0x7b」，看起來完全像記憶體毀損。實際上 SD 驅動
 * 一點問題也沒有（SD_disk_read/SD_disk_write 直接測都回 rc=0），
 * 讀取路徑也完全正常 —— 只有「建檔」會走到蓋時間戳這一步。
 *
 * 先前其他要寫檔的 app 也踩過同一件事。
 * ⚠ 這代表 SDK 裡**每一支要寫檔的 app 都得自己補這支**，否則必掛。 */
int _gettimeofday(struct timeval *tv, void *tz)
{
    (void)tz;
    if (tv) { tv->tv_sec = 0; tv->tv_usec = 0; }
    return 0;
}

extern char *imgBuffer;

/* fatfs_sdh_sdcard.c 註冊的硬體 SDH 驅動與匯流排寬度設定。 */
extern void fatfs_sdh_driver_register(void);
extern void hal_sd_bus_4bits_enable(void);

/* ---- 狀態 ---------------------------------------------------------------
 * 模型與輸入都留著，這樣「換輸入重跑」與「換模型重跑」都不必重來一遍。 */
static FATFS              s_fatfs;
static bool               s_mounted;
static bool               s_driver_registered;
static int                s_hw_ready;

static iproai_model_hdl_t s_hdl;            /* 目前載入的模型 */
static char               s_model_path[64];

static uint8_t           *s_feed;           /* 4 bytes 大小表頭 + 輸入 */
static uint32_t           s_feed_cap;       /* 表頭之後的容量 */
static uint32_t           s_feed_len;       /* 實際餵進去的長度 */

/* ---- SD ------------------------------------------------------------------ */

int ai_link_sd_mount(void)
{
    if (s_mounted) { return 0; }
    if (!s_driver_registered) {
        /* 🔴 4-bit 要在註冊驅動之前先要。
         *
         * fatfs_sdh_sdcard.c 的 sdio_bus_width 預設是 1-bit，而 SD_disk_initialize()
         * 只有第一次被呼叫時會把它帶進 SDH_Init()，之後再改就沒有作用。
         * 少了這一行，init log 明明印著「4-line supported[YES]」，卡片卻整場跑在
         * 四分之一的頻寬上 —— 訊息就在那裡，很容易看過去。
         *
         * 量過，別高估它：ai_load 一顆 424 KB 模型，1-bit 與 4-bit 的板端耗時
         * 中位數是 1829 ms 對 1834 ms —— 區間完全重疊，**分不出來**。這條路徑
         * 的時間不在 SD 讀取上（1-bit 理論值也才 ~137 ms），所以別拿 4-bit 當
         * 效能修正來賣。留著是因為它是這張卡（SCR 回報 4-line supported）的
         * 正確設定、不要錢，而且大批連續讀寫時才會開始有感。
         *
         * EVB 的 GPIO28..33 已由 bsp/board/ipro7/ipro7_evb_board_config.h 設成
         * GPIO_FUN_SDC，四條資料線都在，這裡不必再補 pinmux。
         * 卡片若不支援 4-bit，SDH_SDCardInit() 自己會退回 1-bit
         * （bsp/hal/src/hal_sdh.c:1429-1433），所以這是「要求」而不是「假設」。
         *
         * ⚠ 追過一次、排除了：CDC 收檔時 f_open/f_write 間歇性失敗曾被懷疑是
         *   4-bit 的時序問題，把這一行停掉做 A/B 實測 —— **1-bit 一樣失敗**。
         *   所以匯流排寬度不是那個問題的原因，不必再試一次。真正的解法是
         *   cdc_link.c 的兩階段接收（先收進 RAM，USB 安靜後再寫卡）。 */
        hal_sd_bus_4bits_enable();
        fatfs_sdh_driver_register();
        s_driver_registered = true;
    }
    /* opt = 1：立刻掛載。卡片沒插或沒格式化要在這裡失敗，
     * 而不是等到第一次開檔才報一個指不到原因的錯。 */
    const FRESULT fr = f_mount(&s_fatfs, "sd:", 1);
    if (fr != FR_OK) {
        printf("AIERR mount failed fr=%d\r\n", (int)fr);
        return -1;
    }
    s_mounted = true;
    return 0;
}

static int cmd_ai_mount(int argc, char *argv[])
{
    (void)argc; (void)argv;
    if (ai_link_sd_mount() != 0) { return -1; }
    printf("AIMOUNT ok\r\n");
    return 0;
}

/* 修復用:重建卡上的檔案系統。今天這張卡在數十次被中斷的 424 KB 寫入
 * （主機端取消、硬重置）之後,FAT 簇鏈實測已壞 —— 對舊檔 f_open 截斷回
 * FR_INT_ERR(fr=2),對新檔 f_write 配置簇時也會撞到同一片壞區,
 * 而 raw 磁區讀寫(ai_sdtest)始終正常。f_mkfs 是唯一的就地修復。
 * 破壞性操作,所以要求逐字打 ai_mkfs YES 才動手。 */
static int cmd_ai_mkfs(int argc, char *argv[])
{
    if (argc < 2 || strcmp(argv[1], "YES") != 0) {
        printf("AIERR ai_mkfs erases the card. Type: ai_mkfs YES\r\n");
        return -1;
    }
    if (s_mounted) {
        f_mount(NULL, "sd:", 0);
        s_mounted = false;
    }
    /* work 買 heap 的,不佔 BSS:32KB 的靜態緩衝把 BSS 撐過了 heap 區段
     * 的起點,ai_load 的大配置一跑就撞 heap_5 的 assert。 */
    uint8_t *work = (uint8_t *)malloc(32768);
    if (work == NULL) { printf("AIERR mkfs no memory\r\n"); return -1; }
    const MKFS_PARM parm = { FM_FAT32 | FM_SFD, 0, 0, 0, 0 };
    FRESULT fr = f_mkfs("sd:", &parm, work, 32768);
    free(work);
    if (fr != FR_OK) {
        printf("AIERR mkfs fr=%d\r\n", (int)fr);
        return -1;
    }
    printf("AIMKFS ok\r\n");
    return 0;
}

static int cmd_ai_unmount(int argc, char *argv[])
{
    (void)argc; (void)argv;
    if (s_mounted) {
        f_mount(NULL, "sd:", 0);
        s_mounted = false;
    }
    printf("AIMOUNT released\r\n");
    return 0;
}

static int cmd_ai_ls(int argc, char *argv[])
{
    if (ai_link_sd_mount() != 0) { return -1; }
    const char *dir = (argc > 1) ? argv[1] : "sd:/";
    DIR d;
    FRESULT fr = f_opendir(&d, dir);
    if (fr != FR_OK) {
        printf("AIERR opendir %s fr=%d\r\n", dir, (int)fr);
        return -1;
    }
    printf("AILS %s\r\n", dir);
    for (;;) {
        FILINFO fi;
        fr = f_readdir(&d, &fi);
        if (fr != FR_OK || fi.fname[0] == 0) { break; }
        printf("  %-40s %10lu %s\r\n", fi.fname, (unsigned long)fi.fsize,
               (fi.fattrib & AM_DIR) ? "dir" : "");
    }
    f_closedir(&d);
    printf("AILSEND\r\n");
    return 0;
}

/* ---- 從主機寫檔到卡片 ----------------------------------------------------
 *
 * 正式的路徑是 USB（MSC 或 CDC-ACM）—— 這一組走 shell，慢但一定能用，
 * 而且不必把卡片拔出來就能放進一個小模型或一份測試輸入。
 *
 * ⚠ shell 的一行上限是 128 個位元組（SHELL_CONSOLEBUF_SIZE），扣掉指令名
 * 之後每次約 48 個位元組。1 KB 的算子模型十幾次就好；大模型請走 USB。 */
static FIL  s_wfil;
static bool s_wopen;
static uint32_t s_wbytes;

static int hexval(char ch)
{
    if (ch >= '0' && ch <= '9') { return ch - '0'; }
    if (ch >= 'a' && ch <= 'f') { return ch - 'a' + 10; }
    if (ch >= 'A' && ch <= 'F') { return ch - 'A' + 10; }
    return -1;
}

static int cmd_ai_wopen(int argc, char *argv[])
{
    if (argc < 2) { printf("usage: ai_wopen sd:/file.bin\r\n"); return -1; }
    if (ai_link_sd_mount() != 0) { return -1; }
    if (s_wopen) { f_close(&s_wfil); s_wopen = false; }

    const FRESULT fr = f_open(&s_wfil, argv[1], FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK) {
        printf("AIERR wopen %s fr=%d\r\n", argv[1], (int)fr);
        return -1;
    }
    s_wopen = true;
    s_wbytes = 0;
    printf("AIWOPEN %s\r\n", argv[1]);
    return 0;
}

static int cmd_ai_wchunk(int argc, char *argv[])
{
    if (!s_wopen) { printf("AIERR no file open\r\n"); return -1; }
    if (argc < 2) { printf("usage: ai_wchunk <hex>\r\n"); return -1; }

    const char *h = argv[1];
    const size_t n = strlen(h);
    if (n == 0 || (n & 1)) { printf("AIERR odd hex length\r\n"); return -1; }

    uint8_t buf[64];
    if (n / 2 > sizeof(buf)) { printf("AIERR chunk too long\r\n"); return -1; }
    for (size_t i = 0; i < n / 2; i++) {
        const int hi = hexval(h[2 * i]), lo = hexval(h[2 * i + 1]);
        if (hi < 0 || lo < 0) { printf("AIERR bad hex\r\n"); return -1; }
        buf[i] = (uint8_t)((hi << 4) | lo);
    }
    UINT wrote = 0;
    if (f_write(&s_wfil, buf, (UINT)(n / 2), &wrote) != FR_OK || wrote != n / 2) {
        printf("AIERR write failed\r\n");
        return -1;
    }
    s_wbytes += wrote;
    printf("AIWCHUNK %lu\r\n", (unsigned long)s_wbytes);
    return 0;
}

static int cmd_ai_wclose(int argc, char *argv[])
{
    (void)argc; (void)argv;
    if (!s_wopen) { printf("AIERR no file open\r\n"); return -1; }
    f_close(&s_wfil);
    s_wopen = false;
    printf("AIWCLOSE bytes=%lu\r\n", (unsigned long)s_wbytes);
    return 0;
}

/* ---- SD 寫入的隔離診斷 ------------------------------------------------
 *
 * f_open(FA_CREATE_ALWAYS) 會讓板子硬錯誤，但那條路徑上有 FatFS 與驅動兩層。
 * 這一支把驅動單獨拉出來測：讀一個磁區、再把**完全相同**的內容寫回同一個
 * 磁區。內容不變，所以就算寫壞了也不會改到任何資料；而如果它在這裡就掛，
 * 責任就在 SD_disk_write 而不是 FatFS。 */
extern int SD_disk_read(BYTE *buff, LBA_t sector, UINT count);
extern int SD_disk_write(const BYTE *buff, LBA_t sector, UINT count);

static uint8_t s_sect[512] __attribute__((aligned(64)));

static int cmd_ai_sdtest(int argc, char *argv[])
{
    if (ai_link_sd_mount() != 0) { return -1; }
    const uint32_t sector = (argc > 1) ? (uint32_t)strtoul(argv[1], NULL, 0) : 40000u;

    printf("AISD read sector=%lu ...\r\n", (unsigned long)sector);
    const int rr = SD_disk_read(s_sect, (LBA_t)sector, 1);
    printf("AISD read rc=%d first=%02x%02x%02x%02x\r\n", rr,
           s_sect[0], s_sect[1], s_sect[2], s_sect[3]);
    if (rr != 0) { return -1; }

    /* 同樣的內容寫回同一個磁區 —— 資料不會變。 */
    printf("AISD write back ...\r\n");
    const int wr = SD_disk_write(s_sect, (LBA_t)sector, 1);
    printf("AISD write rc=%d\r\n", wr);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ai_sdtest, ai_sdtest, Read a sector and write it straight back);

/* FatFS 的 ff_memalloc() 直接用 libc 的 malloc（components/fs/fatfs/ffsystem.c:17-22），
 * 而 FF_USE_LFN==3 表示長檔名的工作緩衝是動態配置的。這一支先看 malloc
 * 到底回什麼，再看 f_open 建檔在哪一步不見。 */
static int cmd_ai_fstest(int argc, char *argv[])
{
    (void)argc; (void)argv;
    if (ai_link_sd_mount() != 0) { return -1; }

    void *a = malloc(660);
    printf("AIFS malloc(660)=%p\r\n", a);
    if (a != NULL) { memset(a, 0x5a, 660); printf("AIFS malloc write ok\r\n"); free(a); }

    void *b = pvPortMalloc(660);
    printf("AIFS pvPortMalloc(660)=%p heap_free=%u\r\n",
           b, (unsigned)xPortGetFreeHeapSize());
    if (b != NULL) { vPortFree(b); }

    static FIL f;
    printf("AIFS f_open create ...\r\n");
    const FRESULT fr = f_open(&f, "sd:/t.bin", FA_WRITE | FA_CREATE_ALWAYS);
    printf("AIFS f_open fr=%d\r\n", (int)fr);
    if (fr == FR_OK) {
        UINT wrote = 0;
        static uint8_t buf[512];
        memset(buf, 0xa5, sizeof(buf));
        const FRESULT wf = f_write(&f, buf, sizeof(buf), &wrote);
        printf("AIFS f_write fr=%d wrote=%u\r\n", (int)wf, (unsigned)wrote);
        f_close(&f);
        printf("AIFS f_close done\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ai_fstest, ai_fstest, Probe malloc and f_open create);

/* ---- NPU ----------------------------------------------------------------- */

static int hw_init(void)
{
    if (s_hw_ready) { return 0; }
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);
    iproai_npu_init();
    s_hw_ready = 1;
    return 0;
}

/* 讓輸入緩衝至少有 need 個位元組。
 *
 * ⚠ 前面固定留 4 個位元組：韌體從 imgBuffer + 4 讀畫面，前 4 個是大小表頭
 * （iproai_inst_cpu.c:225）。imgBuffer 是 hal_npu.c 裡的弱符號，這裡覆蓋它。
 *
 * 🔴 但「表頭 + 4」不能只是把 malloc 回來的指標加 4 —— **酬載本身必須對齊**。
 * f_read 對整磁區會直接 DMA 進使用者緩衝，目的地沒對齊時 SDH 會靜默地
 * 傳 0 個位元組（實測：同一張卡、同一個檔，iproai_load_model_from_file
 * 讀得到，因為它讀進 IPROAI_MALLOC 的對齊記憶體；讀進 base+4 就回 got=0，
 * 而 FRESULT 仍然是 FR_OK，看起來像檔案是空的）。
 *
 * 所以多配 64 個位元組，把酬載擺在對齊邊界上，表頭放它前面。 */
#define FEED_ALIGN 64u

static uint8_t *s_feed_base;                /* 真正 free 的那一個指標 */

static int feed_reserve(uint32_t need)
{
    if (s_feed != NULL && need <= s_feed_cap) { return 0; }
    if (s_feed_base != NULL) { vPortFree(s_feed_base); s_feed_base = NULL; }

    s_feed_base = pvPortMalloc(need + 2 * FEED_ALIGN);
    if (s_feed_base == NULL) {
        s_feed = NULL;
        s_feed_cap = 0;
        printf("AIERR feed alloc %lu failed\r\n", (unsigned long)(need + 2 * FEED_ALIGN));
        return -1;
    }
    /* 酬載對齊，表頭正好放在它前面的 4 個位元組。 */
    uintptr_t payload = ((uintptr_t)s_feed_base + FEED_ALIGN) & ~(uintptr_t)(FEED_ALIGN - 1);
    s_feed     = (uint8_t *)(payload - 4);
    s_feed_cap = need;
    imgBuffer  = (char *)s_feed;
    return 0;
}

static void drop_model(void)
{
    if (s_hdl != NULL) {
        iproai_free(s_hdl);
        s_hdl = NULL;
    }
    s_model_path[0] = 0;
}

/* ---- 結果回呼 ------------------------------------------------------------
 * 🔴 只有在 CONFIG_AI_LEGACY_YOLO / CONFIG_AI_LEGACY_POSTPROCESS 有開的
 * 建置裡才會被呼叫 —— iproai_inst_process.c 的每一個 app_type 分支都包在
 * 那兩個設定裡，沒開的話一次都不會觸發。 */
static void on_result(struct iproai_result_t *r)
{
    if (r == NULL) { return; }
    switch (r->app_type) {
    case OBJECT_DETECTION:
    case RETINA_FACE:
    case RETINA_PERSON:
        /* prob 是 float（核心裡已經乘過 100，所以是百分比），rect 是 uint32_t。
         * ⚠ 這裡刻意轉成整數再印：IPRO7 的 FreeRTOS port **不存 FPU context**，
         * 推論任務裡多碰一次浮點就多一分風險，而百分比取整就夠用了。 */
        printf("AIDET class=%d prob=%d x=%lu y=%lu w=%lu h=%lu\r\n",
               r->r.objdet.best_class, (int)r->r.objdet.prob,
               (unsigned long)r->r.objdet.rect.x, (unsigned long)r->r.objdet.rect.y,
               (unsigned long)r->r.objdet.rect.w, (unsigned long)r->r.objdet.rect.h);
        break;
    case CLASSIFICATION:
        printf("AICLS class=%d\r\n", r->r.classification.best_class);
        break;
    case FACE_RECONGNITION:
        printf("AIFR class=%d\r\n", r->r.fr.best_class);
        break;
    default:
        printf("AIRES app_type=%d\r\n", r->app_type);
        break;
    }
}

/* 🔴 forward_SSD_tflite() 會用 model->post_info.classification.classes_name
 * 直接 printf 名稱，沒填就是對 NULL 解參考。給一張足夠長的預設表。 */
static const char *s_class_names[64];
static char        s_class_name_buf[64][8];

static void install_default_class_names(void)
{
    for (int i = 0; i < 64; i++) {
        snprintf(s_class_name_buf[i], sizeof(s_class_name_buf[i]), "c%d", i);
        s_class_names[i] = s_class_name_buf[i];
    }
}

/* ---- 指令 ---------------------------------------------------------------- */

static int cmd_ai_load(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: ai_load sd:/model.inm\r\n");
        return -1;
    }
    if (ai_link_sd_mount() != 0 || hw_init() != 0) { return -1; }
    drop_model();

    s_hdl = iproai_create();
    if (s_hdl == NULL) { printf("AIERR create failed\r\n"); return -1; }

    if (iproai_load_model_from_file(s_hdl, argv[1]) != IPROAI_STATUS_NO_ERROR) {
        printf("AIERR load %s failed\r\n", argv[1]);
        drop_model();
        return -1;
    }
    iproai_setResultCB(s_hdl, on_result);
    snprintf(s_model_path, sizeof(s_model_path), "%s", argv[1]);
    printf("AILOAD ok %s\r\n", argv[1]);
    return 0;
}

static int cmd_ai_info(int argc, char *argv[])
{
    (void)argc; (void)argv;
    if (s_hdl == NULL) { printf("AIERR no model loaded\r\n"); return -1; }
    struct iproai_net_info_t *net = iproai_getNetInfo(s_hdl);
    if (net == NULL) { printf("AIERR no net info\r\n"); return -1; }

    int on_npu = 0;
    for (int i = 0; i < net->layer_cnt; i++) {
        on_npu += (net->layers[i].NPU_on != 0);
    }
    const struct cpu_inst_layer_t *lin  = &net->layers[0];
    const struct cpu_inst_layer_t *lout = &net->layers[net->layer_cnt - 1];
    printf("AIINFO path=%s layers=%d npu=%d in=%dx%dx%d out=%dx%dx%d "
           "patch_size=%d patch_count=%d app_type=%d feed=%lu\r\n",
           s_model_path, net->layer_cnt, on_npu,
           lin->w, lin->h, lin->c, lout->out_w, lout->out_h, lout->out_c,
           net->patch_size, net->patch_count, net->app_type,
           (unsigned long)s_feed_len);
    return 0;
}

static int cmd_ai_input(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: ai_input sd:/input.bin\r\n");
        return -1;
    }
    if (ai_link_sd_mount() != 0 || hw_init() != 0) { return -1; }

    FIL f;
    FRESULT fr = f_open(&f, argv[1], FA_READ);
    if (fr != FR_OK) {
        printf("AIERR open %s fr=%d\r\n", argv[1], (int)fr);
        return -1;
    }
    const FSIZE_t size = f_size(&f);
    if (size == 0 || size > 0x400000) {
        printf("AIERR input size %lu out of range\r\n", (unsigned long)size);
        f_close(&f);
        return -1;
    }
    if (feed_reserve((uint32_t)size) != 0) { f_close(&f); return -1; }

    /* f_read 單次傳輸有上限，照 iproai_load_model_from_file 的做法分塊。 */
    UINT done = 0;
    while (done < (UINT)size) {
        UINT want = (UINT)size - done;
        if (want > 0x8000u) { want = 0x8000u; }
        UINT got = 0;
        fr = f_read(&f, s_feed + 4 + done, want, &got);
        if (fr != FR_OK || got == 0) { break; }
        done += got;
    }
    f_close(&f);
    if (done != (UINT)size) {
        printf("AIERR short read %lu of %lu\r\n",
               (unsigned long)done, (unsigned long)size);
        return -1;
    }

    /* 前 4 個位元組是大小表頭；NPU 是 bus master，看不到 CPU 的 D-cache。 */
    s_feed[0] = (uint8_t)(size & 0xff);
    s_feed[1] = (uint8_t)((size >> 8) & 0xff);
    s_feed[2] = (uint8_t)((size >> 16) & 0xff);
    s_feed[3] = (uint8_t)((size >> 24) & 0xff);
    s_feed_len = (uint32_t)size;
    L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)s_feed, 4 + s_feed_len);

    printf("AIINPUT ok %s bytes=%lu\r\n", argv[1], (unsigned long)s_feed_len);
    return 0;
}

static int cmd_ai_run(int argc, char *argv[])
{
    if (s_hdl == NULL) { printf("AIERR no model loaded\r\n"); return -1; }
    if (s_feed == NULL || s_feed_len == 0) {
        printf("AIERR no input loaded\r\n");
        return -1;
    }
    const int reps = (argc > 1) ? atoi(argv[1]) : 1;
    if (reps < 1 || reps > 100) { printf("AIERR reps out of range\r\n"); return -1; }

    for (int k = 0; k < reps; k++) {
        NPU_Img_Ext_Addr_Cfg((uint32_t)(uintptr_t)(s_feed + 4));
        iproai_cycle_reset();
        if (iproai_startCompute(s_hdl) != IPROAI_STATUS_NO_ERROR) {
            printf("AIERR compute failed\r\n");
            return -1;
        }
        printf("AIRUN rep=%d total=%llu npu=%llu dispatches=%lu\r\n",
               k, (unsigned long long)iproai_cycle_total(),
               (unsigned long long)iproai_cycle_npu(),
               (unsigned long)iproai_cycle_npu_dispatches());
    }
    return 0;
}

/* 逐層時間。ai_run 之後才有東西可看 —— 資料只保留**最近一次**推論。
 *
 * NPU 的列會是 first != last:一次 dispatch 涵蓋一整段連續的 NPU 層,
 * 硬體上沒有逐層的邊界可量（見 iproai_cycle_profile.h）。 */
static int cmd_ai_prof(int argc, char *argv[])
{
    (void)argc; (void)argv;
    iproai_cycle_layer_report();
    return 0;
}

static int cmd_ai_out(int argc, char *argv[])
{
    if (s_hdl == NULL) { printf("AIERR no model loaded\r\n"); return -1; }
    struct iproai_net_info_t *net = iproai_getNetInfo(s_hdl);
    if (net == NULL) { printf("AIERR no net info\r\n"); return -1; }

    uint32_t patch = 0;
    const uint8_t *out = iproai_getOutputBuffer(s_hdl, &patch);
    if (out == NULL) { printf("AIERR no output buffer\r\n"); return -1; }

    /* ⚠ 兩個陷阱都在這幾行裡：
     *   1. iproai_getOutputBuffer 回傳的 *size 是 **patch 大小**，不是張量大小。
     *   2. NPU 把通道補齊到 4 的倍數，所以實體間距是 BALI_CHN_ALIGN(out_c)
     *      而不是 out_c —— 主機端要照這個間距解讀。 */
    const struct cpu_inst_layer_t *lout = &net->layers[net->layer_cnt - 1];
    const int pitch = lout->NPU_on ? (int)BALI_CHN_ALIGN(lout->out_c) : lout->out_c;
    uint32_t need = (uint32_t)lout->out_h * (uint32_t)lout->out_w * (uint32_t)pitch;

    uint32_t cap = (argc > 1) ? (uint32_t)strtoul(argv[1], NULL, 0) : need;
    if (cap > need)   { cap = need; }
    if (cap > patch)  { cap = patch; }

    L1C_DCACHE_INVALID_RANGE((uintptr_t)out, cap);

    /* 位元組原樣送出。⚠ 板上與主機模擬都把輸出存成 **XOR 0x80** 的形式，
     * 主機端解碼時要一致（ipro_ai_core/simulate.py 有記載）。 */
    printf("AIOUT bytes=%lu h=%d w=%d c=%d pitch=%d patch=%lu\r\n",
           (unsigned long)cap, lout->out_h, lout->out_w, lout->out_c, pitch,
           (unsigned long)patch);
    static const char hx[] = "0123456789abcdef";
    char line[80];
    int k = 0;
    for (uint32_t i = 0; i < cap; i++) {
        line[k++] = hx[(out[i] >> 4) & 0xf];
        line[k++] = hx[out[i] & 0xf];
        if (k == 64) { line[k] = 0; printf("%s\r\n", line); k = 0; }
    }
    if (k) { line[k] = 0; printf("%s\r\n", line); }
    printf("AIOUTEND\r\n");
    return 0;
}

/* 🔴 板上燒的到底是不是剛編的那一版。
 *
 * 一整晚的除錯裡有一段是在追一個「已經修好」的問題 —— 修好的韌體根本沒
 * 燒進去。工具鏈沒有任何辦法回答這個問題,而它是完全避得掉的浪費。
 *
 * 刻意選編譯時間戳而不是 git hash:開發途中的工作區幾乎永遠是 dirty 的,
 * hash 會指向一份與板上不同的原始碼,比沒有還糟。時間戳問的是「這顆 binary
 * 是哪一次建置出來的」,那正是要問的問題。
 *
 * 標記字串會原封不動留在 .bin 裡,所以主機端不必知道任何建置系統的細節:
 * 在本機的 .bin 裡找同一個標記,和板子回報的比一比就知道。 */
static const char AI_BUILD_ID[] = "IPROAI-BUILD " __DATE__ " " __TIME__;

static int cmd_ai_ver(int argc, char *argv[])
{
    (void)argc; (void)argv;
    printf("AIVER %s\r\n", AI_BUILD_ID);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_ai_ver,     ai_ver,     Report which build is running);
SHELL_CMD_EXPORT_ALIAS(cmd_ai_mount,   ai_mount,   Mount the SD card);
SHELL_CMD_EXPORT_ALIAS(cmd_ai_unmount, ai_unmount, Release the SD card);
SHELL_CMD_EXPORT_ALIAS(cmd_ai_mkfs,    ai_mkfs,    Rebuild the card filesystem [erases card]);
SHELL_CMD_EXPORT_ALIAS(cmd_ai_ls,      ai_ls,      List a directory on the card);
SHELL_CMD_EXPORT_ALIAS(cmd_ai_load,    ai_load,    Load a model from the card);
SHELL_CMD_EXPORT_ALIAS(cmd_ai_input,   ai_input,   Load input data from the card);
SHELL_CMD_EXPORT_ALIAS(cmd_ai_run,     ai_run,     Run inference);
SHELL_CMD_EXPORT_ALIAS(cmd_ai_out,     ai_out,     Dump the output buffer);
SHELL_CMD_EXPORT_ALIAS(cmd_ai_prof,    ai_prof,    Per-layer cycles of the last run);
SHELL_CMD_EXPORT_ALIAS(cmd_ai_info,    ai_info,    Describe the loaded model);
/* 讀回:wopen/wchunk 的鏡像。推上去的檔案到底寫進了什麼,一直沒有
 * 辦法驗證 —— 424,544 個位元組「都寫成功」而內容全錯的那一天,就是
 * 這兩支指令該存在的理由。 */
static FIL  s_rfil;
static bool s_ropen;

static int cmd_ai_ropen(int argc, char *argv[])
{
    if (argc < 2) { printf("usage: ai_ropen sd:/file.bin\r\n"); return -1; }
    if (ai_link_sd_mount() != 0) { return -1; }
    if (s_ropen) { f_close(&s_rfil); s_ropen = false; }
    const FRESULT fr = f_open(&s_rfil, argv[1], FA_READ);
    if (fr != FR_OK) { printf("AIERR ropen %s fr=%d\r\n", argv[1], (int)fr); return -1; }
    s_ropen = true;
    printf("AIROPEN %s bytes=%lu\r\n", argv[1], (unsigned long)f_size(&s_rfil));
    return 0;
}

static int cmd_ai_rchunk(int argc, char *argv[])
{
    if (!s_ropen) { printf("AIERR no file open\r\n"); return -1; }
    uint32_t off = (argc > 1) ? (uint32_t)strtoul(argv[1], NULL, 0) : 0;
    uint32_t len = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 64;
    if (len > 256) { len = 256; }
    uint8_t buf[256]; UINT got = 0;
    FRESULT fr = f_lseek(&s_rfil, off);
    if (fr == FR_OK) { fr = f_read(&s_rfil, buf, len, &got); }
    if (fr != FR_OK) { printf("AIERR rchunk fr=%d\r\n", (int)fr); return -1; }
    printf("AIRCHUNK %lu ", (unsigned long)off);
    for (UINT i = 0; i < got; i++) { printf("%02x", buf[i]); }
    printf("\r\n");
    return 0;
}

static int cmd_ai_rclose(int argc, char *argv[])
{
    (void)argc; (void)argv;
    if (s_ropen) { f_close(&s_rfil); s_ropen = false; }
    printf("AIRCLOSE\r\n");
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_ai_wopen,   ai_wopen,   Open a file on the card for writing);
SHELL_CMD_EXPORT_ALIAS(cmd_ai_ropen,   ai_ropen,   Open a file on the card for reading);
SHELL_CMD_EXPORT_ALIAS(cmd_ai_rchunk,  ai_rchunk,  Read hex bytes [off len]);
SHELL_CMD_EXPORT_ALIAS(cmd_ai_rclose,  ai_rclose,  Close the read file);
SHELL_CMD_EXPORT_ALIAS(cmd_ai_wchunk,  ai_wchunk,  Append hex bytes to the open file);
SHELL_CMD_EXPORT_ALIAS(cmd_ai_wclose,  ai_wclose,  Close the file being written);

static void link_task(void *arg)
{
    (void)arg;
    install_default_class_names();
#ifdef CONFIG_CHERRYUSB_DEVICE_CDC
    /* 🔴 只能呼叫一次。這一段曾被(03:01 的檔案回退事故)複製成兩份 ——
     * 雙重 usbd 初始化本來就不對,掛上 usb_console attach 之後更會把
     * xTaskCreateStatic 打在已執行 task 的同一塊靜態 TCB 上,直接野指標。
     * CDC 現在是複合裝置:ACM0 資料通道 + ACM1 shell console,
     * 一條 USB 線就能操作,不再依賴 probe 的 UART 橋。 */
    cdc_link_init();
#endif
    printf("\r\nipro_ai_link ready\r\n");
    printf("  ai_mount · ai_ls [dir]\r\n");
    printf("  ai_load sd:/model.inm · ai_input sd:/input.bin\r\n");
    printf("  ai_run [reps] · ai_out [max] · ai_info\r\n");
    printf("  ai_wopen/ai_wchunk/ai_wclose  慢（~220 B/s），小檔用\r\n");
#ifdef CONFIG_CHERRYUSB_DEVICE_CDC
    printf("  ai_recv + CDC 資料埠        快，大檔用\r\n");
#endif
    vTaskDelete(NULL);
}

int main(void)
{
    ipro_platform_init();
    xTaskCreate(link_task, "ailink", 2048, NULL, 5, NULL);
    vTaskStartScheduler();
    while (1) { }
}

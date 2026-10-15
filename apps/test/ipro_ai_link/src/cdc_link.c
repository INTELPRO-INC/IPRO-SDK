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

/* CDC-ACM 大量資料路徑 —— 把模型與測試輸入推到卡片上。
 *
 * 為什麼需要這一支：走 shell 的 hex 傳輸實測只有 **220 B/s**（一行 128 個
 * 位元組上限，扣掉指令名每次 48 個）。1.4 KB 的算子模型 6.5 秒還能忍，
 * 8 KB 的輸入就要 35 秒，500 KB 的模型完全不用談。
 * USB 是全速裝置（cherryusb/CMakeLists.txt:19-21 明確擋掉 high speed），
 * 上限約 1 MB/s，實務上 700–900 KB/s —— 快三個數量級。
 *
 * 分工刻意這樣切：
 *   控制走 **UART0 的 shell**（ai_recv 起手、AIRECV 收尾）
 *   資料走 **CDC 的 bulk-OUT**，沒有任何框架
 *
 * 兩個理由。一是 usbd_event_handler 的轉接層只支援一個回呼
 * （usb_console_event.c:12-24），console 一旦也搬到 USB 就會相撞；留在 UART
 * 就完全沒有這個問題。二是 bulk-OUT 本身就有流量控制 —— 裝置沒有掛下一個
 * 讀取時主機會收到 NAK，所以不必自己做協定（tools/ota_usb_send.py:11-12
 * 也是這樣寫的）。
 *
 * 線上格式沿用 SDK 既有範本的慣例（cdc_acm_template.c:655-680）：
 *   "WRIT" | u32 檔案大小 | u32 路徑長度 | 路徑 | 酬載
 * 差別是那份範本寫檔走 EVFS 的 POSIX open()，這裡走原生 FatFS，
 * 與 iproai_load_model_from_file() 同一個命名空間（"sd:/…"）。
 */
#include <generated/autoconf.h>
#include "ipro_vendor_ids.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <task.h>
#include <stream_buffer.h>

#include "usbd_core.h"
#include "usbd_cdc.h"
#include <compiler/common.h>
#include "system_soc.h"
#include "shell.h"
#include "ff.h"
#ifdef CONFIG_DEBUG_CONSOLE_USB_ATTACH
#include "usb_console.h"
#endif

#define CDC_IN_EP  0x81
#define CDC_OUT_EP 0x02
#define CDC_INT_EP 0x83

#define USBD_VID           IPRO_USB_VID
#define USBD_PID           0x0704          /* 與其他 app 區隔 */
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#ifdef CONFIG_DEBUG_CONSOLE_USB_ATTACH
/* 雙 ACM 複合:ACM0 = 資料通道(WRIT 協定),ACM1 = shell console。
 * 一條 USB 線同時給出兩個序列埠 —— DIY 使用者不需要 probe 的 UART 橋。 */
#define USB_CONFIG_SIZE (9 + 2 * CDC_ACM_DESCRIPTOR_LEN)
#define USB_INTF_COUNT  0x04
#define CON_IN_EP  0x84
#define CON_OUT_EP 0x05
#define CON_INT_EP 0x86
#else
#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN)
#define USB_INTF_COUNT  0x02
#endif
#define CDC_MAX_MPS 64                     /* full speed */

#define RX_BUF_BYTES     2048
#define RX_STREAM_BYTES  32768
#define FILE_CHUNK       4096

static const uint8_t cdc_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, USB_INTF_COUNT, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MAX_MPS, 0x02),
#ifdef CONFIG_DEBUG_CONSOLE_USB_ATTACH
    CDC_ACM_DESCRIPTOR_INIT(0x02, CON_INT_EP, CON_OUT_EP, CON_IN_EP, CDC_MAX_MPS, 0x02),
#endif
    USB_LANGID_INIT(USBD_LANGID_STRING),
    /* string1: manufacturer */
    0x10, USB_DESCRIPTOR_TYPE_STRING,
    'I', 0, 'N', 0, 'T', 0, 'E', 0, 'L', 0, 'P', 0, 'R', 0,
    /* string2: product */
    0x18, USB_DESCRIPTOR_TYPE_STRING,
    'I', 0, 'P', 0, 'R', 0, 'O', 0, ' ', 0, 'A', 0, 'I', 0,
    ' ', 0, 'l', 0, 'i', 0, 'n', 0,
    /* string3: serial */
    0x0a, USB_DESCRIPTOR_TYPE_STRING,
    'a', 0, 'i', 0, '0', 0, '1', 0,
    0x00
};

/* DMA 目的地要對齊，而且 CPU 讀之前要 invalidate —— USB 控制器是 bus master。 */
static uint8_t rx_buf[RX_BUF_BYTES] ATTR_EALIGN(32) ATTR_PSRAM_BSS_SECTION;

static StreamBufferHandle_t s_rx_stream;
static volatile int         s_configured;

/* 一次接收的狀態，由 shell 起手、由接收任務收尾。 */
static volatile int      s_recv_active;
static char              s_recv_path[80];
static volatile uint32_t s_recv_want;
static volatile uint32_t s_recv_got;
static volatile int      s_recv_result;      /* 0 = 進行中/成功，負值 = 失敗 */
static volatile int      s_recv_fr;          /* 最後一次 FatFS 錯誤碼（f_open/f_write） */
static FIL               s_pre_fil;          /* shell 情境預先開好的檔 */
static volatile int      s_pre_open;

static struct usbd_interface intf0;
static struct usbd_interface intf1;

/* ---- 端點回呼（在 USB ISR 裡跑，只能塞進 stream buffer） ---------------- */

/* 🔴 掛不掛下一個讀取,就是這裡唯一的流量控制。
 *
 * 一開始我無條件重新掛讀取,以為「bulk-OUT 自己會流控」。**那是錯的**：
 * 裝置一直有讀取掛著,主機就永遠不會被 NAK,於是它以 USB 全速灌進來,
 * 而接收任務只能以 SD 的寫入速度消化 —— 中間的 stream buffer 填滿之後,
 * xStreamBufferSendFromISR() 只會寫進放得下的部分然後**靜默丟掉其餘**。
 * 實測 500 KB 的模型只收到 32,745 個位元組,正好卡在 32 KB 的緩衝上限。
 *
 * 正確的做法是：放不下一個完整封包時就**不要掛讀取**。主機因此被 NAK,
 * USB 硬體替我們擋住;等接收任務消化出空間再由它重新掛上。 */
static volatile int s_rx_paused;
static volatile uint32_t s_rx_dropped;

/* 🔴 掛讀取的**長度**不能永遠是 RX_BUF_BYTES。
 *
 * 控制器只在兩種情形下呼叫這支回呼（drv_usb_v1.c:886-887）：收到短封包
 * （pkt_bytes % ep_mps != 0），或者要求的長度剛好收滿（xfer_len == 0）。
 * 兩者都不成立時走 br = 2，重新掛 DMA 繼續等更多資料。
 *
 * 所以尾巴長度只要是 64（ep_mps，全速 bulk）的**非零倍數**、又填不滿還沒
 * 收完的 2048，回呼就永遠不會觸發，那段資料留在控制器裡，主機那邊看起來
 * 就是傳輸停住。實測：模型 145,136 B 尾巴 1776（1776 % 64 = 48，短封包）
 * 收得完；輸入 3,072 B 尾巴 1024（1024 % 64 = 0）必掛在 got=2048。
 * 而輸入張量幾乎一定是 64 的倍數（3072 = 32*32*3），所以是**輸入必掛**。
 *
 * 解法：一旦知道這次總共要收多少，就照剩下的長度掛讀取，讓 xfer_len == 0
 * 這個條件負責收尾，不必仰賴主機湊出短封包。 */
static volatile uint32_t s_rx_expect;    /* 12 + path_len + size，0 = 還不知道 */
static volatile uint32_t s_rx_seen;      /* 這次已經進到 stream 的總量 */

static uint32_t rx_next_len(void)
{
    if (s_rx_expect == 0 || s_rx_seen >= s_rx_expect) {
        return RX_BUF_BYTES;             /* 表頭還沒解析，或這一輪收完了 */
    }
    const uint32_t left = s_rx_expect - s_rx_seen;
    return (left < RX_BUF_BYTES) ? left : RX_BUF_BYTES;
}

/* 由接收任務在解析完表頭之後補上，當作 ISR 自己判讀失敗時的後備。 */
void cdc_link_set_expected(uint32_t total)
{
    s_rx_expect = total;
}

void usbd_cdc_acm_bulk_out(uint8_t ep, uint32_t nbytes)
{
    (void)ep;
    L1C_DCACHE_INVALID_RANGE((uintptr_t)rx_buf, RX_BUF_BYTES);
    if (nbytes > 0 && s_rx_stream != NULL) {
        /* 這一輪的第一個封包就是表頭，直接在這裡把總長算出來 —— 等接收
         * 任務解析完再設就太晚了，中間那幾次讀取已經用預設長度掛出去。 */
        if (s_rx_expect == 0 && s_rx_seen == 0 && nbytes >= 12 &&
            memcmp(rx_buf, "WRIT", 4) == 0) {
            uint32_t size = 0, path_len = 0;
            memcpy(&size, &rx_buf[4], 4);
            memcpy(&path_len, &rx_buf[8], 4);
            if (path_len > 0 && path_len < sizeof(s_recv_path)) {
                s_rx_expect = 12 + path_len + size;
            }
        }
        BaseType_t woken = pdFALSE;
        const size_t sent = xStreamBufferSendFromISR(s_rx_stream, rx_buf, nbytes, &woken);
        if (sent != nbytes) {
            /* 走到這裡就代表上面那個規則破了 —— 記下來,不要無聲無息。 */
            s_rx_dropped += (uint32_t)(nbytes - sent);
        }
        s_rx_seen += (uint32_t)sent;
        portYIELD_FROM_ISR(woken);
    }
    if (s_rx_stream != NULL &&
        xStreamBufferSpacesAvailable(s_rx_stream) >= RX_BUF_BYTES) {
        usbd_ep_start_read(CDC_OUT_EP, rx_buf, rx_next_len());
    } else {
        s_rx_paused = 1;
    }
}

/* 接收任務消化出空間之後由它重新掛讀取。test-and-clear 放在臨界區裡,
 * 免得和 ISR 同時掛上兩個讀取。 */
static void rx_resume_if_paused(void)
{
    int resume = 0;
    taskENTER_CRITICAL();
    if (s_rx_paused && s_rx_stream != NULL &&
        xStreamBufferSpacesAvailable(s_rx_stream) >= RX_BUF_BYTES) {
        s_rx_paused = 0;
        resume = 1;
    }
    taskEXIT_CRITICAL();
    if (resume) {
        usbd_ep_start_read(CDC_OUT_EP, rx_buf, rx_next_len());
    }
}

void usbd_cdc_acm_bulk_in(uint8_t ep, uint32_t nbytes)
{
    (void)ep; (void)nbytes;
}

static struct usbd_endpoint cdc_out_ep = { .ep_addr = CDC_OUT_EP, .ep_cb = usbd_cdc_acm_bulk_out };
static struct usbd_endpoint cdc_in_ep  = { .ep_addr = CDC_IN_EP,  .ep_cb = usbd_cdc_acm_bulk_in };

/* cherryusb 的 usbd_event_handler 是 __WEAK 的；這裡給強符號。
 * console 留在 UART，所以整個韌體只有這一個 USB 使用者，不需要轉接層。 */
void usbd_event_handler(uint8_t event)
{
    switch (event) {
    case USBD_EVENT_CONFIGURED:
        s_configured = 1;
        usbd_ep_start_read(CDC_OUT_EP, rx_buf, RX_BUF_BYTES);
        break;
    case USBD_EVENT_DISCONNECTED:
    case USBD_EVENT_RESET:
        s_configured = 0;
        break;
    default:
        break;
    }
#ifdef CONFIG_DEBUG_CONSOLE_USB_ATTACH
    usb_console_notify_event(event);   /* console 的第二組 ACM 同步狀態 */
#endif
}

/* ---- 接收任務 ------------------------------------------------------------ */

static void drain(uint32_t n)
{
    static uint8_t junk[512];
    while (n > 0) {
        const size_t want = (n > sizeof(junk)) ? sizeof(junk) : n;
        const size_t got = xStreamBufferReceive(s_rx_stream, junk, want, pdMS_TO_TICKS(2000));
        rx_resume_if_paused();
        if (got == 0) { break; }
        n -= (uint32_t)got;
    }
}

static void recv_task(void *arg)
{
    (void)arg;
    static uint8_t chunk[FILE_CHUNK];

    while (s_recv_active) {
        uint8_t hdr[12];
        if (xStreamBufferReceive(s_rx_stream, hdr, sizeof(hdr), pdMS_TO_TICKS(500)) < sizeof(hdr)) {
            /* 空等也要喚醒端點:暫停若發生在表頭與資料之間,沒有這一下,
             * 任務與主機就互相等對方,誰也動不了。 */
            rx_resume_if_paused();
            continue;
        }
        if (memcmp(hdr, "WRIT", 4) != 0) {
            printf("AIRECV bad magic %02x%02x%02x%02x\r\n", hdr[0], hdr[1], hdr[2], hdr[3]);
            continue;
        }
        uint32_t size = 0, path_len = 0;
        memcpy(&size, &hdr[4], 4);
        memcpy(&path_len, &hdr[8], 4);
        if (path_len == 0 || path_len >= sizeof(s_recv_path)) {
            printf("AIRECV bad path length %lu\r\n", (unsigned long)path_len);
            drain(size);
            continue;
        }
        if (xStreamBufferReceive(s_rx_stream, s_recv_path, path_len,
                                 pdMS_TO_TICKS(2000)) < path_len) {
            printf("AIRECV path timeout\r\n");
            continue;
        }
        s_recv_path[path_len] = 0;
        s_recv_want = size;
        s_recv_got  = 0;
        /* ISR 通常已經算好了；表頭被拆到兩個封包時它算不出來,這裡補上。 */
        cdc_link_set_expected(12 + path_len + size);

        FIL f_local;
        FIL *f_ptr = &f_local;
        FRESULT ofr = FR_OK;
        if (s_pre_open) {
            f_ptr = &s_pre_fil;                  /* shell 已開好,直接用 */
        } else {
            ofr = f_open(&f_local, s_recv_path, FA_WRITE | FA_CREATE_ALWAYS);
        }
        if (ofr != FR_OK) {
            s_recv_fr = (int)ofr;
            printf("AIRECV cannot create %s\r\n", s_recv_path);
            drain(size);
            s_recv_result = -1;
            continue;
        }
        /* 🔴 收與寫**分兩階段**,不要交錯。USB DMA 進行中時,這個任務發起
         * 的 SD I/O 會在磁碟層失敗:f_open 回 FR_INT_ERR、f_write 回
         * FR_DISK_ERR,同樣的操作在 USB 安靜時（shell 情境）從不失敗 ——
         * SDH 與 USB 兩個 bus master 的驅動目前合不來。所以先把整個檔案
         * 收進 RAM（USB 全速、SD 不動）,收完、匯流排安靜之後,再一口氣
         * 寫進卡片。staging 期間 s_recv_got 照樣推進,主機的流量窗與
         * 完成輪詢語意不變;RAM 要不到就退回舊的交錯路徑。 */
        uint8_t *stage = (size > 0 && size <= (8u << 20))
                       ? (uint8_t *)malloc(size) : NULL;
        if (stage != NULL) {
            uint32_t staged = 0;
            while (staged < size) {
                uint32_t want = size - staged;
                if (want > 8192) { want = 8192; }
                const size_t got = xStreamBufferReceive(s_rx_stream, stage + staged,
                                                        want, pdMS_TO_TICKS(5000));
                rx_resume_if_paused();
                if (got == 0) { break; }
                staged += (uint32_t)got;
                s_recv_got = staged;
            }
            if (staged == size) {
                s_recv_got = 0;
                while (s_recv_got < size) {
                    UINT want = size - s_recv_got;
                    if (want > 32768) { want = 32768; }
                    UINT wrote = 0;
                    FRESULT wfr = f_write(f_ptr, stage + s_recv_got, want, &wrote);
                    if (wfr != FR_OK || wrote != want) {
                        printf("AIRECV f_write fr=%d wrote=%u of %u at %lu\r\n",
                               (int)wfr, (unsigned)wrote, (unsigned)want,
                               (unsigned long)s_recv_got);
                        s_recv_fr = (int)wfr;
                        break;
                    }
                    s_recv_got += wrote;
                }
            }
            free(stage);
        } else {
        while (s_recv_got < size) {
            uint32_t want = size - s_recv_got;
            if (want > FILE_CHUNK) { want = FILE_CHUNK; }
            const size_t got = xStreamBufferReceive(s_rx_stream, chunk, want,
                                                    pdMS_TO_TICKS(5000));
            rx_resume_if_paused();
            if (got == 0) { break; }
            UINT wrote = 0;
            FRESULT wfr = f_write(f_ptr, chunk, (UINT)got, &wrote);
            if (wfr != FR_OK || wrote != got) {
                printf("AIRECV f_write fr=%d wrote=%u of %u at %lu\r\n",
                       (int)wfr, (unsigned)wrote, (unsigned)got,
                       (unsigned long)s_recv_got);
                s_recv_fr = (int)wfr;
                break;
            }
            s_recv_got += (uint32_t)got;
            rx_resume_if_paused();
        }
        }
        f_close(f_ptr);
        s_pre_open = 0;

        if (s_recv_got == size) {
            printf("AIRECV done %s bytes=%lu\r\n", s_recv_path, (unsigned long)s_recv_got);
            s_recv_result = 0;
        } else {
            printf("AIRECV short %s %lu of %lu\r\n", s_recv_path,
                   (unsigned long)s_recv_got, (unsigned long)size);
            s_recv_result = -2;
        }
        s_recv_active = 0;      /* 一次一個檔，主機再下一次 ai_recv */
    }
    vTaskDelete(NULL);
}

/* ---- shell ---------------------------------------------------------------- */

extern int ai_link_sd_mount(void);       /* main.c */

void cdc_link_init(void)
{
    usbd_desc_register(cdc_descriptor);
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf1));
    usbd_add_endpoint(&cdc_out_ep);
    usbd_add_endpoint(&cdc_in_ep);
#ifdef CONFIG_DEBUG_CONSOLE_USB_ATTACH
    usb_console_attach();              /* interfaces 2/3 + EP 0x84/0x05/0x86 */
#endif
    usbd_initialize();
}

static int cmd_ai_recv(int argc, char *argv[])
{
    if (ai_link_sd_mount() != 0) { return -1; }
    if (!s_configured) {
        printf("AIERR usb not configured - is the CDC port open on the host?\r\n");
        return -1;
    }
    if (s_recv_active) {
        printf("AIERR receive already in progress\r\n");
        return -1;
    }
    if (s_rx_stream == NULL) {
        s_rx_stream = xStreamBufferCreate(RX_STREAM_BYTES, 1);
        if (s_rx_stream == NULL) {
            printf("AIERR stream buffer alloc failed\r\n");
            return -1;
        }
    }
    /* 🔴 f_open 在**這裡**（shell 任務）做,不在接收任務做。同一個 boot、
     * 同一個檔名,接收任務裡的 f_open 穩定回 FR_INT_ERR,shell 裡永遠成功
     * —— 根因未明（不是堆疊 2048→4096、不是 FS 損壞、不是匯流排忙碌,
     * 全都排除過）,先用實測可靠的情境把路走通。主機把目的路徑當參數
     * 帶進來;沒帶參數就退回舊行為（任務自己開檔）。 */
    s_pre_open = 0;
    if (argc >= 2) {
        FRESULT pfr = f_open(&s_pre_fil, argv[1], FA_WRITE | FA_CREATE_ALWAYS);
        if (pfr != FR_OK) {
            printf("AIERR recv pre-open %s fr=%d\r\n", argv[1], (int)pfr);
            return -1;
        }
        s_pre_open = 1;
    }
    xStreamBufferReset(s_rx_stream);
    s_recv_active = 1;
    s_recv_result = 0;
    s_recv_fr = 0;
    s_recv_got = 0;
    s_recv_want = 0;
    s_rx_dropped = 0;
    s_rx_expect = 0;
    s_rx_seen = 0;
    /* 🔴 端點可能還停在上一輪的暫停:stream 滿了、或 configure 後資料在
     * stream 建立前就到,回呼都會設 s_rx_paused 而不再掛讀取。到這裡 stream
     * 已經清空,不喚醒它的話這一輪一個位元組也收不到 —— 主機看到的是
     * 「寫了 header 之後整條線 NAK 到天荒地老」,只有重開機救得回來。 */
    rx_resume_if_paused();
    /* 比 shell 的執行緒高一階，才不會被主機灌進來的資料餓死。
     * ipro_pio_sdh_test 的 README 記過：把 SD I/O 排在 console TX 之上會把
     * console 整個餓死，所以刻意不要更高。 */
    /* 4096 words:2048 之下 f_open/f_write 在這個任務裡間歇回 FR_INT_ERR,
     * shell 大堆疊任務跑同樣的操作永遠成功 —— 溢位打壞的是 FatFS 的
     * 內部狀態,症狀因此漂移(f_open 失敗、f_write 失敗輪流出現)。 */
    xTaskCreate(recv_task, "ai_recv", 4096, NULL, configMAX_PRIORITIES - 4, NULL);
    printf("AIRECV ready\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ai_recv, ai_recv, Receive one file over the CDC data port);

static int cmd_ai_usb(int argc, char *argv[])
{
    (void)argc; (void)argv;
    /* 每個欄位都是獨立的 key=value —— "got=1424/1424" 這種寫法主機端剖析不了。 */
    printf("AIUSB configured=%d receiving=%d path=%s got=%lu want=%lu dropped=%lu "
           "seen=%lu expect=%lu paused=%d result=%d fr=%d\r\n",
           s_configured, s_recv_active, s_recv_path[0] ? s_recv_path : "-",
           (unsigned long)s_recv_got, (unsigned long)s_recv_want,
           (unsigned long)s_rx_dropped, (unsigned long)s_rx_seen,
           (unsigned long)s_rx_expect, s_rx_paused, s_recv_result, s_recv_fr);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ai_usb, ai_usb, Report the CDC data port state);

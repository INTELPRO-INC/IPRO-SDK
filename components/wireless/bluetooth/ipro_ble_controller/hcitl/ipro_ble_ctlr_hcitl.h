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
 ****************************************************************************************
 * @file ipro_ble_ctlr_hcitl.h
 *
 * @brief 內部 HCI 傳輸層 —— 用共享 ring buffer 實作上游的 `struct rwip_eif_api`。
 *
 * 這是計畫規則 3(「host↔controller 只准走位元組流」)的落點。
 *
 * ## 為什麼這樣就等於「乾淨的界面」
 *
 * 上游把外部介面抽象成四個函式(`read`/`write`/`flow_on`/`flow_off`),
 * `rwip_init()` 拿到它就自動接上標準 H4 HCI:
 *
 *     LL ←→ ip/hci ←→ modules/rwtl/rwtl.c ←→ h4tl.c ←→ rwip_eif_api ← 這裡
 *
 * 也就是說「內部 HCI」與「UART HCI」**對 controller 而言完全相同** ——
 * 差別只在這四個函式底下是 ring buffer 還是 UART。controller 不知道、
 * 也不需要知道 host 在同一顆晶片上。
 *
 * 舊樹的 on-chip 路徑不是這樣:指令走 opcode+打包參數,但 ACL/ISO 接收
 * 直接把 RW 結構指標與 Exchange Memory offset 遞過界,host 端自己讀 EM
 * 再呼叫 `ble_util_buf_rx_free()` —— 緩衝區所有權跨界,controller 無法單獨替換。
 *
 * ## 執行緒模型
 *
 * 兩條單生產者單消費者(SPSC)的 ring:
 *
 *     h2c: host thread 寫 → controller task 讀
 *     c2h: controller task 寫 → host thread 讀
 *
 * IPRO7 是單核心,對齊的 32-bit volatile 存取是原子的,SPSC 的 head/tail
 * 因此不需要鎖,只需要編譯器屏障。
 *
 * ## 🔴 永不丟位元組
 *
 * H4 是位元組流,封包中途掉一個 byte 會讓對面**永久失步**(h4tl 會進
 * `h4tl_out_of_sync()`,但那是為了實體線路的雜訊設計的,不是給我們用的)。
 *
 * 所以 ring 滿的時候**不丟也不阻塞**,改成把這次傳輸記成「待決」,
 * 等 `ipro_ble_ctlr_hcitl_poll()` 有空間時再完成。上游的 h4tl 是狀態機,
 * callback 沒回來就不會發下一筆,所以待決只會有一筆。
 *
 * 這也是為什麼 read 用同一套機制:h4tl 一次要 N 個 byte,不足 N 就等,
 * 而不是回傳部分資料。
 ****************************************************************************************
 */

#ifndef IPRO_BLE_CTLR_HCITL_H_
#define IPRO_BLE_CTLR_HCITL_H_

#include <stdbool.h>
#include <stdint.h>

/* 上游的 rwip_eif_api 定義在 rwip.h。這裡只前置宣告,讓 host 端
 * 可以 include 本檔而不必把整個 controller 的標頭鏈拖進去。 */
struct rwip_eif_api;

/*
 * 尺寸
 ****************************************************************************************
 */

/**
 * ring 大小,必須是 2 的冪(用遮罩取代除法)。
 *
 * 預設值的依據(link1_periph,單連線、周邊、無 ISO):
 *   - h2c:host→controller。最大單筆 = HCI ACL 251 payload + 4 header + 1 H4 type = 256
 *   - c2h:controller→host。事件較小但較密,且 ACL 上行同樣 256,給兩倍緩衝
 *
 * ISO profile uses larger rings.  A single timestamped ISO HCI packet can be
 * close to 256 bytes, and bt54_iso2 exposes two ISO streams plus control
 * events.  The embedded H4 link must absorb short FreeRTOS scheduling latency
 * without pushing back into the controller radio path.
 *
 * 這兩個值影響的是「多少筆可以在途」,不影響正確性 —— 滿了會延遲完成而非丟棄。
 */
/*
 * H2C must hold a full ACL TX window, not just a packet or two.
 *
 * One ACL packet at the 247-byte MTU is 256 bytes on the wire (251 payload
 * + 4 ACL header + 1 H4 type), and the controller advertises 7 TX buffers.
 * At 512 bytes the ring held exactly two, so the host's send path sat in a
 * yield loop for every packet after that and the link carried about one
 * notification per connection event - 13 kB/s on 2M at a 15 ms interval,
 * where the radio itself could do ten times that.
 *
 * 2048 bytes is eight packets: the whole credit window plus one, so the
 * host is never the thing the radio waits for.
 */
#ifndef IPRO_BLE_CTLR_HCITL_H2C_SIZE
#define IPRO_BLE_CTLR_HCITL_H2C_SIZE   2048
#endif

#ifndef IPRO_BLE_CTLR_HCITL_C2H_SIZE
#if defined(CFG_ISOOHCI) || defined(CFG_VOHCI)
#define IPRO_BLE_CTLR_HCITL_C2H_SIZE   4096
#else
#define IPRO_BLE_CTLR_HCITL_C2H_SIZE   1024
#endif
#endif

/*
 * Controller 側 —— 由 arch_main.c 使用
 ****************************************************************************************
 */

/**
 * 初始化。必須在 `rwip_init()` **之前**呼叫 ——
 * `rwip_init()` 內部會呼叫 `rwip_eif_get(0)` 取得下面那個 vtable。
 */
void ipro_ble_ctlr_hcitl_init(void);

/**
 * 取得給上游的 `rwip_eif_api` vtable。
 */
const struct rwip_eif_api *ipro_ble_ctlr_hcitl_eif_get(void);

/**
 * 服務待決的傳輸。**只能從 controller task 呼叫**,而且要在
 * `rwip_process()` 之前 —— 完成 read 會註冊一個 djob,
 * 同一輪的 `rwip_process()` 才處理得到。
 *
 * @return true 表示這次有完成任何傳輸(診斷用)。
 */
bool ipro_ble_ctlr_hcitl_poll(void);

/**
 * @brief True when a further poll would make progress right now.
 *
 * The main loop uses this to decide whether it may block at all. A read the
 * H4 layer has just issued, with its bytes already sitting in h2c, must be
 * served by the next iteration - not by whatever wakes the task next, which
 * in a quiet link is the connection-event interrupt.
 */
bool ipro_ble_ctlr_hcitl_has_work(void);

/**
 * Host HCI activity needs the BLE IP awake as well as the controller task.
 * Returns false for a short interval after host->controller traffic so the
 * main loop uses core-retentive WFI instead of BLE-IP deep sleep.
 */
bool ipro_ble_ctlr_hcitl_deep_sleep_allowed(void);

/** Mark completion of the mandatory HCI controller initialization sequence. */
void ipro_ble_ctlr_hcitl_host_ready(void);

/// False until the host has completed HCI initialization.
extern volatile bool ipro_ble_ctlr_hcitl_host_is_ready;

/// Number of controller-loop sleeps held off by the HCI activity guard.
extern volatile uint32_t ipro_ble_ctlr_hcitl_sleep_guard_cnt;

/*
 * Host 側 —— 由 Zephyr 的 bt_hci_driver 使用(Phase 3)
 ****************************************************************************************
 */

/**
 * 送 H4 位元組給 controller。
 *
 * @param[in] data  H4 位元組流(**含**開頭的封包型別 byte:0x01 CMD / 0x02 ACL)
 * @param[in] len   長度
 * @return 實際接受的位元組數。**小於 len 代表 ring 滿了** ——
 *         呼叫端必須把剩下的留著稍後再送,不可丟棄(見檔頭「永不丟位元組」)。
 *         用 `ipro_ble_ctlr_hcitl_host_write_space()` 先問可以避免部分寫入。
 */
uint32_t ipro_ble_ctlr_hcitl_host_write(const uint8_t *data, uint32_t len);

/// h2c 目前還能收多少位元組。
uint32_t ipro_ble_ctlr_hcitl_host_write_space(void);

/**
 * 從 controller 收 H4 位元組。
 *
 * @param[out] out  輸出緩衝區
 * @param[in]  max  最多讀幾個
 * @return 實際讀到的位元組數(0 表示沒資料)。
 */
uint32_t ipro_ble_ctlr_hcitl_host_read(uint8_t *out, uint32_t max);

/// c2h 目前有多少位元組可讀。
uint32_t ipro_ble_ctlr_hcitl_host_avail(void);

/**
 * 註冊「controller 有東西給 host」的通知。
 *
 * 🔴 callback 在 **controller task** 的上下文執行,不是 host 的。
 * 裡面只能做「叫醒 host thread」這件事(給 semaphore / 送 notify),
 * 不可以在裡面解析 HCI —— 那會把 host 的處理時間算到 controller 頭上,
 * 而 controller 有硬性的無線電時限。
 */
void ipro_ble_ctlr_hcitl_host_set_rx_notify(void (*cb)(void));

/*
 * 診斷計數器
 ****************************************************************************************
 */

/// 累計通過的位元組數。
extern volatile uint32_t ipro_ble_ctlr_hcitl_h2c_bytes;
extern volatile uint32_t ipro_ble_ctlr_hcitl_c2h_bytes;

/**
 * 因為 ring 滿而被延後的傳輸次數。
 *
 * 🔴 這**不是**錯誤計數,是「ring 太小」的指標。持續增加代表對面來不及
 * 消化,應該調大對應的 `IPRO_BLE_CTLR_HCITL_*_SIZE`,而不是當作 bug 追。
 */
extern volatile uint32_t ipro_ble_ctlr_hcitl_h2c_deferred;
extern volatile uint32_t ipro_ble_ctlr_hcitl_c2h_deferred;
extern volatile uint32_t ipro_ble_ctlr_hcitl_gate_now_tick;
extern volatile uint32_t ipro_ble_ctlr_hcitl_gate_awake_until_tick;
extern volatile uint32_t ipro_ble_ctlr_hcitl_gate_reject_not_ready;
extern volatile uint32_t ipro_ble_ctlr_hcitl_gate_reject_guard;
extern volatile uint32_t ipro_ble_ctlr_hcitl_gate_reject_h2c;
extern volatile uint32_t ipro_ble_ctlr_hcitl_gate_reject_c2h;
extern volatile uint32_t ipro_ble_ctlr_hcitl_gate_reject_rx_partial;
extern volatile uint32_t ipro_ble_ctlr_hcitl_gate_reject_tx_pending;
extern volatile uint32_t ipro_ble_ctlr_hcitl_gate_last_h2c_count;
extern volatile uint32_t ipro_ble_ctlr_hcitl_gate_last_c2h_count;

#endif /* IPRO_BLE_CTLR_HCITL_H_ */

# ipro_pio_sdh_test — 雙 host 同時掛載 SD 檔案系統測試

> **v1.0 封版 (2026-07-24)** — 客戶 demo / 評估 / benchmark 工具。雙 host 同掛已實測驗證
> (BOTH HOSTS VERIFIED),benchmark 全 PASS。基準與範例輸出見下。

PIO 軟體 SD host 與硬體 SDH **各自一張卡、同時掛載**的 FS 測試/格式化環境。
raw 區塊級指令已移除,僅保留 FS 測試與格式化。

## 雙掛載架構

| Host | 腳位 | FatFS drive | 掛載點 | 指令 |
|------|------|-------------|--------|------|
| **PIO SDH** | CLK=G22 CMD=G23 D0-D3=G24-27 | `DEV_MMC`("mmc:") | **`/piosd`** | `pio_sdcard ...` |
| **HW SDH** | G28-33(SDC pinmux) | `DEV_SD`("sd:") | **`/sdcard`** | `sdcard` / `format_sd` / `sdh_speed_test` |

兩個 host 各插一張卡,可同時掛載、同時操作(EVFS 多實例 + FatFS 多卷)。
PIO 側經 app-local 的 `fatfs_pio_mmc_port.c` 綁到 DEV_MMC 槽位(共用元件的
DEV_SD PIO port 在本 app 不使用)。

## Pad 預設(本 bench:轉板自帶外部上拉)

`.config`:`CONFIG_HAL_PIO_SDH_PAD_DRIVE=0`(最弱驅動/最緩邊緣)+
`CONFIG_HAL_PIO_SDH_PULL_NONE=y`(內部上拉關)。運行時隨時可調(sticky 跨 init):

```text
pio_sdcard pad <drive 0-3> <pull 0|1> [smt 0|1]
pio_sdcard pad 3 1        # 回 SDK 傳統組態(無外部上拉的接法「必須」pull=1)
```

## 執行緒模型(即時 console 輸出)

`pio_sdcard` 的 CLI handler **只解析參數 + 派工**,重活跑在專屬 worker task,
**不在 shell exec thread 裡執行**。優先級關鍵:

```
shell exec thread     prio 19   (只派工,秒回)
USB console TX task    prio 2    (usb_console.c)
pio worker task        prio 1    (← 低於 console,執行 FS 重活)
sdh_speed_test task    prio 1    (HW 側,同理)
```

worker 低於 console TX,所以即使 worker 正忙於 SD I/O,console 仍能搶進來
**即時**吐每行輸出。(舊版把 batch 跑在 shell thread(prio 19)裡,整段不
yield,console(prio 2)整個過程搶不進來 → 全部輸出卡到最後才噴。)

- CLI 派工後立即返回,soak 進行中你**仍可打其他指令**(會排進 worker 佇列,深度 4)。
- 佇列滿會回 `busy - job queue full`。
- `pad` 例外:即時 GPIO 設定,直接在 shell thread 跑(不進佇列)。

## Build / Flash / Console

```bash
make -C apps/test/ipro_pio_sdh_test
```
- Console/log/shell 走 USB CDC(VID 0x1209 / PID 0x7014);UART 變體 = 關閉 .config 的 CherryUSB 區塊
- 板上需有 `ipro7_uboot_boot2`(app 帶 BOOT_APP_HEADER;stock boot2 開不起來)
- 燒錄:UART `ipro_iot_tool_lite --flash-pin 0x9`(arduino 板)或 USB 1200-touch

## 客戶用法(3 步跑分)

```text
pio_sdcard info                  # 認卡:vendor / 容量 / bus / heap
pio_sdcard format                # 準備 exFAT(⚠ 清空卡)—— 卡已有 FS 可改 `mount`
pio_sdcard benchmark             # 一鍵跑分 + 報告(預設 128MB seq + 128×256KB 小檔)
pio_sdcard benchmark dual        # 雙卡對照:PIO vs HW SDH 並列(需先 `sdcard` 掛 HW 卡)
pio_sdcard benchmark 256 dual    # 參數可任意順序(數字=MB,dual=雙卡模式)
```

**雙卡對照模式**(`benchmark dual`)在一個指令內對 PIO(/piosd)和硬體 SDH(/sdcard)
各跑一次相同套件,並列出對照表 —— 客戶評估「軟體 host vs 硬體 SDH」的一鍵答案:
```
-----------------------------------------------
                          PIO        HW SDH
 Sequential write     7.08 MB/s    7.50 MB/s
 Sequential read      9.87 MB/s   10.50 MB/s
 Small-file write       20 f/s       25 f/s
 Small-file read        33 f/s       40 f/s
 Data integrity         PASS         PASS
 RESULT: PASS
```
前提:兩張卡分別在 PIO + HW SDH 卡座,且都已掛載(`sdcard` + PIO `format`/`mount`)。

benchmark 報告內含:**順序寫/讀 MB/s、小檔 files/s、RAM 用量、資料完整性 PASS/FAIL**。
每階段**每秒吐一行進度**,隨時可用 `pio_sdcard status` 查當前進度(soak 進行中也可查)。

範例輸出(SanDisk SN64G,2026-07-24 實測):
```
============ PIO SD Host Benchmark ============
 Card : SanDisk SN64G   59.4 GB   4-bit SDHC/SDXC
 Host : PIO GPIO22-27 @ 24 MHz DMA,  exFAT /piosd
 Size : 128 MB sequential + 128 x 256 KB small-file
-----------------------------------------------
 [1/2] Sequential 128 MB:
    write           5 /  128 MB    7.13 MB/s
    write          16 /  128 MB    7.01 MB/s
    ... (每秒一行) ...
    read+verify   124 /  128 MB    9.87 MB/s
 [2/2] Small files 128 x 256 KB:
    small write  103 / 128 files
    small read    92 / 128 files
-----------------------------------------------
 Sequential write ..........    7.10 MB/s
 Sequential read ...........    9.83 MB/s
 Small-file write ..........       20 files/s
 Small-file read ...........       33 files/s
 Data integrity ............ PASS (all verified)
 RAM: heap free 8152 KB, peak use 257 KB (64 KB I/O buf + FatFS)
-----------------------------------------------
 RESULT: PASS
===============================================
```

> **RAM** = FreeRTOS heap(64KB I/O 緩衝 + FatFS 結構);static 佔用見 build 的 flash/ocram 數字。

## 進階 / soak 指令

```text
pio_sdcard fspeed [kb=2048]           # 單檔寫讀 + verify
pio_sdcard fstress <w|r|x> <idx> [mb] # 大檔寫 / 驗證讀 / 刪
pio_sdcard fstress_batch <n> [mb=64]  # N 檔 soak + 全位元驗證(每檔即時進度)
pio_sdcard fstress_mix [secs=3600]    # 大小檔混合時間制 soak
pio_sdcard dual [mb=32]               # 雙 host 同時掛載交錯測試(見下)
pio_sdcard pad <drive 0-3> <pull 0|1> [smt]  # pad 調整(sticky)
```

## 雙 host 同時掛載

```text
sdcard                           # HW SDH 掛 /sdcard(另一張卡、另一卡座)
pio_sdcard mount                 # PIO 掛 /piosd(兩者共存)
pio_sdcard dual 32               # 兩張卡交錯寫讀 + 全驗證,各自報 MB/s
```

`dual` 以 64KiB chunk 在兩掛載點**交錯**寫入再讀回逐位元驗證(單執行緒交錯,
FF_FS_REENTRANT=0 下安全),是「雙 host 同時掛載」的端到端證明。

## 基準數據(健康 bench,SanDisk SN64G,2026-07-24 實測)

| | PIO(`benchmark`) | HW SDH(`sdh_speed_test`) |
|---|------------------|--------------------------|
| 順序寫 | 7.0 MB/s | 7.5 MB/s |
| 順序讀 | 9.8 MB/s | 10.5 MB/s |

**HW SDH 略快;PIO ≈ HW 的 94%。** PIO 的價值不是更快,而是**用純軟體 + 幾支空閒 GPIO
達到接近硬體 SDH 的吞吐**,把專用硬體 SDH 卡座讓給另一張卡(如 SDIO WiFi 模組)。

雙 host 同掛實測:`dual` 交錯兩張卡,HW 寫7.29/讀10.20、PIO 寫7.02/讀9.62,BOTH VERIFIED。
讀吞吐抖動 <0.1% 是健康簽名;數字亂跳 = bench 接觸/供電有問題(見下)。

## 硬體紅線(2026-07 三日血淚)

1. **供電**:SD 寫入尖峰電流大 —— VDD/GND 杜邦線並聯 ×2 + 轉板就地 10µF+100nF,
   否則會出現「慢速列舉過/全速死、單塊過/連續死、時好時壞」的 brownout 三聯徵。
   外部供電必須共地。
2. **接點**:breakout 固定 + 線束應變消除;排針焊點是頭號故障源(讀好寫壞的
   方向不對稱 = 卡輸出推得過爛接點、host pad 推不過)。
3. **換卡/動線先斷電**(熱插拔傷 pad)。
4. 物理動作後先跑 `fstest` + `fspeed` 驗收再繼續。

## FS 層地雷(fatfs port,兩個 host 都適用)

- `mount` 的 `fatfs_register rc=14` **不是錯誤**;成功看 mounted 行 + 開檔正常。
- **FS 半毀時先 mkfs/format_sd 再 mount**:eager f_mount 遇 FR_NO_FILESYSTEM 會觸發
  靜默 auto-mkfs(無進度、4KB cluster、極慢)。
- mkfs err=1 = 寫入中途失敗;err=13 = 寫完但讀不回(檢查 bench)。
- `WRITE-FAIL` 訊息 = write() 短寫,底層 I/O 錯誤和 ENOSPC 都會觸發。
- FS 指令**一次一個**(FF_FS_REENTRANT=0);`dual` 內部是安全的單執行緒交錯。

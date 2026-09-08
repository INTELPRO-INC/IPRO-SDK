# ipro_pio_sdh_test — 測試流程 (Test Procedure)

PIO 軟體 SD host(GPIO22-27)的 demo / 評估 / benchmark 測試程序。
硬體 SDH(GPIO28-33)作為同板 A/B 對照,可同時掛載。

---

## 0. 前置條件 (Prerequisites)

### 硬體
| 項目 | 說明 |
|------|------|
| 主板 | IPRO7 EVB / Arduino 板(已載入本測試韌體) |
| SD 轉板接線 | CLK=G22, CMD=G23, D0=G24, D1=G25, D2=G26, D3=G27 + 3.3V + GND |
| **SD 供電** | **板上 3.3V 直供,不需外部電源**(pad 為 drive 0 + pull-none 的已驗證組態) |
| 卡 | SDHC/SDXC microSD(exFAT 相容);準備 2 張可做雙 host 對照 |

### 接線健康度(每次動線後必檢)
- SD 轉板的電源 LED 應亮
- 動線 / 換卡 **一律先斷電**(熱插拔會傷 GPIO pad)

---

## 1. 連線

Console 走 **USB CDC,免 UART 轉接板**。接上 USB 後開啟該埠即可。

```bash
python3 tools/serial_monitor.py /dev/cu.usbmodem*
```

開機應看到 banner + `ipro />` 提示。裝置列舉為 `IPRO PIO SDH Test`(VID 0x1209/PID 0x7014)。

---

## 2. 認卡 (Card identification)

```text
pio_sdcard info
```

**檢查點**:
- Vendor / Product / Capacity 正確(e.g. `SanDisk SN64G  59.4 GB`)
- Bus = `4-bit SDHC/SDXC`
- Heap free 顯示正常(≈ MB 級)

> 認不到卡 → 檢查接線 + 供電(電源 LED)+ 卡是否插到底。

---

## 3. 準備檔案系統

卡已有 exFAT(之前格過)→ 直接掛載:
```text
pio_sdcard mount
```

卡是新的 / 無檔案系統 → 格式化(**會清空卡**):
```text
pio_sdcard format
```

**檢查點**:出現 `mounted at /piosd`。
> `benchmark` 在未掛載時會拒跑並提示 —— 這是防止誤抹卡的安全設計。

---

## 4. 一鍵跑分 (Benchmark) — 主測項

```text
pio_sdcard benchmark          # 預設 128MB 順序 + 128×256KB 小檔
pio_sdcard benchmark 256      # 可指定順序測試大小(MB)
```

測試進行中**每秒吐一行進度**;另開一行可查即時狀態:
```text
pio_sdcard status             # RUNNING [write] 48/128 MB 7.14 MB/s
```

**報告判讀**:
```
 Sequential write ..........    7.00 MB/s     ← 順序寫吞吐
 Sequential read ...........    9.80 MB/s     ← 順序讀吞吐
 Small-file write ..........      NNN files/s ← 小檔(含 open/close/FAT 開銷)
 Small-file read ...........      NNN files/s
 Data integrity ............ PASS (all verified)   ← 必須 PASS
 RAM: heap free NNN KB, peak use NNN KB           ← 動態記憶體成本
 RESULT: PASS
```

**合格標準**(SanDisk SN64G 實測基準,2026-07-24):
| 指標 | 健康值 |
|------|--------|
| 順序寫 | **6.9 - 7.0 MB/s** |
| 順序讀 | **9.80 MB/s** |
| 小檔寫 | ~18 files/s(256KB;含 open/close/FAT 開銷) |
| 小檔讀 | ~33 files/s |
| RAM peak use | ~257 KB(64KB I/O 緩衝 + FatFS) |
| 資料完整性 | **PASS(必須)** |

> ⚠️ **benchmark 前建議先 `format`**:掛到舊的小分割區(非全卡)時,小檔階段會 ENOSPC。
> format 產生全卡 exFAT 就不會撞。`info` 顯示的容量是「卡」的容量,不是「分割區」大小。

---

## 5. 耐久 / 壓力測試 (Soak)

```text
pio_sdcard fstress_batch 20 64    # 20 檔 × 64MB = 1.28GB 寫+讀+全位元驗證
pio_sdcard fstress_mix 3600       # 大小檔混合,時間制(秒),長時間穩定度
```

**檢查點**:
- 每檔 `verify=OK`,最終 `verify 20/20`
- 吞吐**抖動 < 1%**(e.g. 讀 10098–10111 KiB/s)= bench 健康的簽名
- 數字**亂跳** = 供電/接觸問題(見第 8 節)

---

## 6. 雙 host 同時掛載測試 (Dual-mount)

需要兩張卡,一張在 PIO 卡座、一張在 HW SDH 卡座。

```text
sdcard                        # HW SDH 掛載 → /sdcard
pio_sdcard mount              # PIO 掛載 → /piosd(兩者共存)
pio_sdcard dual 32            # 兩張卡交錯寫/讀 + 各自全驗證
```

**檢查點**:`BOTH HOSTS VERIFIED`,兩個 host 各自報吞吐。
> 證明 PIO 與硬體 SDH 可真正同時運作(不同 FatFS drive / 掛載點)。

---

## 7. A/B 對照(PIO vs 硬體 SDH)

同一張卡,分別在兩個卡座測(一次一個 host):
```text
# PIO 卡座
pio_sdcard info ; pio_sdcard mount ; pio_sdcard fspeed 65536

# 換到 HW SDH 卡座
sdcard ; sdh_speed_test           # 128MB 寫/讀/驗證
```

**參考差異**(2026-07-24 實測):PIO 寫 7.0 / 讀 9.8 MB/s vs **HW SDH 寫 7.5 / 讀 10.5 MB/s**
—— HW SDH 略快(PIO ≈ HW 94%)。PIO 的價值是**純軟體 + 空閒 GPIO 達近硬體吞吐**,
把專用硬體 SDH 卡座讓給另一張卡(如 SDIO WiFi 模組)。

---

## 8. 故障排除 (Troubleshooting)

| 症狀 | 最可能原因 | 處置 |
|------|-----------|------|
| 吞吐數字亂跳 | 接觸不良 / 供電路徑偏弱 | 重插卡與接線;確認 pad 為 drive 0 + pull-none(`pad 0 0`) |
| 讀 OK 但寫失敗 | host→card 方向接觸差 | 重點檢查 D0(G24)/CMD(G23)接線與焊點 |
| 慢速列舉過、全速壓測死 | 供電路徑撐不住持續電流 | 縮短並加粗 VDD/GND 接線 |
| 單塊過、連續傳輸死 | 同上供電路徑問題 | 同上 |
| `no filesystem` | 未格式化 / FS 損壞 | `pio_sdcard format`(清空) |
| mkfs err=13 | 寫完但讀不回 | 檢查接線/供電,非軟體問題 |
| 換卡後認不到 | 需重新列舉 | 重打 `pio_sdcard mount`(每指令會重新列舉) |

**黃金準則**:任何物理動作(換卡/碰線/換板)後,先 `pio_sdcard fspeed 4096` 或
`benchmark` 驗收,數字對得上再繼續 —— 別在壞 bench 上追軟體問題。

---

## 附:完整指令表

| 指令 | 用途 |
|------|------|
| `info` | 認卡(vendor/容量/bus/heap) |
| `format` | 格式化 exFAT + 掛載(**清空卡**) |
| `mount` | 掛載既有檔案系統 |
| `benchmark [mb]` | 一鍵跑分 + 報告 |
| `status` | 查進行中測試的即時進度 |
| `fspeed [kb]` | 單檔寫讀 + verify |
| `fstress <w\|r\|x> <idx> [mb]` | 大檔寫 / 驗證讀 / 刪 |
| `fstress_batch <n> [mb]` | N 檔 soak + 全驗證 |
| `fstress_mix [secs]` | 大小檔混合時間制 soak |
| `dual [mb]` | 雙 host 同時掛載交錯測試 |
| `pad <drive> <pull> [smt]` | pad 調整(sticky) |
| `help` | 完整指令清單 |

破壞性指令:`format` / `fstress w` / `dual`(會寫卡)。

# BLE Throughput Test 操作手冊

## 目錄

1. [硬體準備](#1-硬體準備)
2. [軟體環境](#2-軟體環境)
3. [編譯韌體](#3-編譯韌體)
4. [燒錄韌體](#4-燒錄韌體)
5. [串口監控](#5-串口監控)
6. [測試操作流程](#6-測試操作流程)
7. [CLI 命令參考](#7-cli-命令參考)
8. [測試場景範例](#8-測試場景範例)
9. [配置調整](#9-配置調整)
10. [故障排除](#10-故障排除)

---

## 1. 硬體準備

### 所需設備

| 項目 | 數量 | 說明 |
|------|------|------|
| IPRO7 EVB 開發板 | 2 塊 | 一塊作為 Slave (Peripheral)，一塊作為 Master (Central) |
| USB 線 | 2 條 | 用於燒錄韌體與串口通訊 |
| macOS 電腦 | 1 台 | 開發與測試主機 |

### 接線與串口識別

將兩塊開發板透過 USB 連接至電腦後，確認串口裝置名稱：

```bash
ls /dev/cu.usbserial-*
```

輸出範例：
```
/dev/cu.usbserial-2120
/dev/cu.usbserial-2130
```

> **重要：macOS 必須使用 `/dev/cu.*`，不要使用 `/dev/tty.*`**。`tty.*` 會等待 DCD 信號而造成阻塞。

記錄每塊板子對應的串口名稱，後續操作需要用到。本手冊以下列對應關係為例：

| 角色 | 串口 |
|------|------|
| Board 1 (Slave) | `/dev/cu.usbserial-2120` |
| Board 2 (Master) | `/dev/cu.usbserial-2130` |

---

## 2. 軟體環境

### 必備工具

| 工具 | 路徑/來源 | 說明 |
|------|-----------|------|
| RISC-V 交叉編譯器 | `/opt/toolchain/riscv_ipro7/bin/riscv64-unknown-elf-*` | rv32imafc / ilp32f |
| CMake | 系統安裝 (macOS: `brew install cmake`) | 建置系統 |
| Make | 系統自帶 | 建置入口 |
| Python 3 | 系統自帶 | 串口監控工具 |
| ipro_iot_tool_lite | `tools/ipro_iot_tool_lite/ipro_iot_tool_lite` | 韌體燒錄工具 (SDK 內建) |
| serial_monitor.py | `tools/serial_monitor.py` | 串口監控工具 (SDK 內建，無需 pyserial) |

### 驗證環境

```bash
# 確認交叉編譯器
/opt/toolchain/riscv_ipro7/bin/riscv64-unknown-elf-gcc --version

# 確認燒錄工具存在
ls -la tools/ipro_iot_tool_lite/ipro_iot_tool_lite

# 確認串口監控工具
python3 tools/serial_monitor.py --help
```

---

## 3. 編譯韌體

### 3.1 完整編譯

從 SDK 根目錄執行：

```bash
make -C apps/bluetooth/ble/ipro_throughput_test
```

編譯成功後，二進位檔案輸出位置：
```
apps/bluetooth/ble/ipro_throughput_test/build/build_out/ipro_throughput_test_IPRO7.bin
```

### 3.2 清除後重新編譯

```bash
make -C apps/bluetooth/ble/ipro_throughput_test clean
make -C apps/bluetooth/ble/ipro_throughput_test
```

### 3.3 確認編譯結果

編譯最後會顯示記憶體使用量：

```
Flash usage: XXXXX / 1048576 (XX%)
OCRAM usage: XXXXX / 229376 (XX%)
```

典型值：Flash 約 399 KB (38%)，OCRAM 約 82 KB (36%)。

---

## 4. 燒錄韌體

### 4.1 進入下載模式

燒錄前，開發板必須先進入下載模式（Download Mode）。有兩種方式：

**方式一：透過串口命令（推薦，適用於板子已正常運行時）**

```bash
python3 tools/serial_monitor.py /dev/cu.usbserial-2120 --send reboot_dl --timeout 5
```

**方式二：手動操作**

按住板上 BOOT 按鈕，同時按下 RESET 按鈕，先放開 RESET 再放開 BOOT。

### 4.2 執行燒錄

燒錄命令格式：

```bash
tools/ipro_iot_tool_lite/ipro_iot_tool_lite \
    --chipname ipro7 \
    --port /dev/cu.usbserial-XXXX \
    --firmware apps/bluetooth/ble/ipro_throughput_test/build/build_out/ipro_throughput_test_IPRO7.bin \
    --baudrate 921600 \
    --flash-pin 0x02
```

### 4.3 燒錄兩塊板子的完整流程

**Board 1 (Slave)：**

```bash
# 步驟 1: 進入下載模式
python3 tools/serial_monitor.py /dev/cu.usbserial-2120 --send reboot_dl --timeout 5

# 步驟 2: 等待 2 秒讓板子準備好
sleep 2

# 步驟 3: 燒錄
tools/ipro_iot_tool_lite/ipro_iot_tool_lite \
    --chipname ipro7 \
    --port /dev/cu.usbserial-2120 \
    --firmware apps/bluetooth/ble/ipro_throughput_test/build/build_out/ipro_throughput_test_IPRO7.bin \
    --baudrate 921600 \
    --flash-pin 0x02
```

**Board 2 (Master)：**

```bash
# 步驟 1: 進入下載模式
python3 tools/serial_monitor.py /dev/cu.usbserial-2130 --send reboot_dl --timeout 5

# 步驟 2: 等待 2 秒
sleep 2

# 步驟 3: 燒錄
tools/ipro_iot_tool_lite/ipro_iot_tool_lite \
    --chipname ipro7 \
    --port /dev/cu.usbserial-2130 \
    --firmware apps/bluetooth/ble/ipro_throughput_test/build/build_out/ipro_throughput_test_IPRO7.bin \
    --baudrate 921600 \
    --flash-pin 0x02
```

燒錄成功會顯示進度條並完成，耗時約 10 秒。

### 4.4 燒錄後重啟

燒錄完成後板子會自動重啟。若未自動重啟，可手動按下 RESET 按鈕，或使用：

```bash
python3 tools/serial_monitor.py /dev/cu.usbserial-2120 --send reboot --timeout 5
```

---

## 5. 串口監控

### 5.1 開啟串口終端

需要同時開啟兩個終端窗口，分別監控兩塊板子。

**終端 1 — Board 1 (Slave)：**

```bash
python3 tools/serial_monitor.py /dev/cu.usbserial-2120
```

**終端 2 — Board 2 (Master)：**

```bash
python3 tools/serial_monitor.py /dev/cu.usbserial-2130
```

### 5.2 串口監控工具用法

```
serial_monitor.py <port> [options]

選項:
  --baud BAUD        鮑率 (預設: 115200)
  --timeout SEC      超時秒數 (預設: 無限)
  --send CMD         連接後發送命令
  --wait-for PATTERN 等待匹配的正則表達式
  --log FILE         將輸出同時寫入檔案
```

### 5.3 發送命令

在 serial_monitor 中直接輸入文字即可發送命令給開發板。支援的系統命令：

| 命令 | 功能 |
|------|------|
| `reboot` | 正常重啟 |
| `reboot_dl` | 進入下載模式 (燒錄用) |
| `tp help` | 顯示吞吐量測試命令列表 |

---

## 6. 測試操作流程

### 6.1 基本測試流程（逐步說明）

以下操作透過串口監控終端輸入命令完成。

#### Step 1: 啟動 Slave 端

在 **Board 1 (Slave)** 的串口終端輸入：

```
tp slave
```

預期輸出：
```
Starting as slave (peripheral)...
[TP_APP] Starting as slave (peripheral)
[TP_APP] Slave started - advertising as 'IPRO-TP-TEST'
```

板子開始以 `IPRO-TP-TEST` 名稱進行 BLE 廣播。

#### Step 2: 啟動 Master 端

在 **Board 2 (Master)** 的串口終端輸入：

```
tp master
```

預期輸出：
```
Starting as master (central)...
[TP_APP] Starting as master (central)
[TP_APP] Master started - scanning for slave
```

Master 開始掃描，找到 Slave 後自動連接：

```
[TP_APP] Found target: IPRO-TP-TEST RSSI:-XX Addr:XX:XX:XX:XX:XX:XX
[TP_APP] Connected: XX:XX:XX:XX:XX:XX
[TP_APP] Requesting DLE: tx_octets=251, tx_time=2120us
[TP_APP] MTU exchange complete: MTU=247
[TP_APP] Starting service discovery...
[TP_APP] Service discovery complete
[TP_APP] Notifications enabled - ready for test
```

同時 Slave 端也會顯示連線資訊：
```
[TP_APP] Connected: XX:XX:XX:XX:XX:XX
[TP_APP] Notifications enabled by master
[TP_APP] Slave ready for throughput test
```

#### Step 3: 切換 PHY（選用）

預設使用 1M PHY。若要使用 2M PHY 以獲得更高吞吐量，在**任一端**輸入：

```
tp phy 2
```

預期輸出：
```
[TP_APP] PHY update initiated: 2M
[TP_APP] PHY updated: TX=2 RX=2
```

> 兩端都會收到 PHY 更新回呼。

#### Step 4: 調整連線參數（選用）

預設連線間隔為 7.5 ms（最佳吞吐量）。如需調整，在**任一端**輸入：

```
tp conn 6 6
```

參數單位為 1.25 ms，常用值：

| 命令 | 實際間隔 |
|------|----------|
| `tp conn 6 6` | 7.5 ms |
| `tp conn 12 12` | 15 ms |
| `tp conn 24 24` | 30 ms |
| `tp conn 80 80` | 100 ms |

#### Step 5: 啟動吞吐量測試

在 **Slave 端**輸入：

```
tp start 244 10
```

參數說明：
- `244` — 每個封包的資料大小（bytes），範圍 20-244
- `10` — 測試持續時間（秒），0 表示無限

預期輸出（每秒更新一次）：
```
[TP] TX: 512 pkts, 124928 bytes, 999.4 Kbps | RX: 0 pkts, 0 bytes, 0.0 Kbps | Err: 0
[TP] TX: 1024 pkts, 249856 bytes, 999.2 Kbps | RX: 0 pkts, 0 bytes, 0.0 Kbps | Err: 0
...
```

#### Step 6: 查看測試結果

測試結束後自動顯示統計：

```
=== Throughput Test Statistics ===
Duration: 10001 ms
TX: 5118 packets, 1248792 bytes, 998.93 Kbps (124.87 KBps)
RX: 0 packets, 0 bytes, 0.00 Kbps (0.00 KBps)
Errors: 0
Connection: interval=7.50ms, PHY=2M
==================================
```

也可以隨時輸入 `tp stats` 手動查看。

#### Step 7: 停止/斷開

```
tp stop       # 停止測試（保持連線）
tp stop       # 再次輸入斷開連線
```

### 6.2 快速測試流程（命令摘要）

```
[Slave]  tp slave
[Master] tp master
         （等待連線完成...）
[任一端] tp phy 2
[Slave]  tp start 244 10
         （等待 10 秒測試完成...）
[任一端] tp stats
```

---

## 7. CLI 命令參考

所有命令以 `tp` 為前綴。

### 角色設定

| 命令 | 說明 |
|------|------|
| `tp slave` | 啟動為 Slave（Peripheral），開始 BLE 廣播 |
| `tp master` | 啟動為 Master（Central），掃描並自動連接 Slave |
| `tp master XX:XX:XX:XX:XX:XX` | 指定目標地址連接 |

### 測試控制

| 命令 | 說明 |
|------|------|
| `tp start [size] [sec] [interval_ms]` | 啟動吞吐量測試 |
| `tp stop` | 停止測試或斷開連線 |
| `tp stats` | 顯示當前/最後一次測試統計 |
| `tp status` | 顯示連線狀態、角色、PHY 等 |

#### `tp start` 參數詳解

| 參數 | 預設值 | 範圍 | 說明 |
|------|--------|------|------|
| `size` | 244 | 20-244 | 每個封包的資料大小 (bytes) |
| `sec` | 10 | 0-∞ | 測試持續時間 (秒)，0 表示無限 |
| `interval_ms` | 0 | 0-1000 | 封包發送間隔 (ms)，0 為 flood 模式 (最大吞吐量) |

### 連線參數

| 命令 | 說明 |
|------|------|
| `tp conn <min> <max>` | 更新連線間隔 (單位: 1.25ms) |
| `tp phy <1\|2\|3>` | 切換 PHY (1=1M, 2=2M, 3=Coded) |
| `tp interval <ms>` | 運行中更改發送間隔 |

### 資訊

| 命令 | 說明 |
|------|------|
| `tp help` | 顯示命令說明 |

---

## 8. 測試場景範例

### 場景 1: 最大吞吐量測試 (2M PHY)

```
[Slave]  tp slave
[Master] tp master
         # 等待連線...
[任一端] tp phy 2
[Slave]  tp start 244 10
```

預期結果：約 **999 Kbps**

### 場景 2: 1M PHY 基準測試

```
[Slave]  tp slave
[Master] tp master
         # 等待連線...（預設即為 1M PHY）
[Slave]  tp start 244 10
```

預期結果：約 **507 Kbps**

### 場景 3: 不同封包大小比較

```
# 連線完成並切換到 2M PHY 後...

[Slave]  tp start 244 10      # 最大封包
         # 等待結束...
[Slave]  tp start 100 10      # 中等封包
         # 等待結束...
[Slave]  tp start 20 10       # 最小封包
```

預期結果：

| 封包大小 | 預期吞吐量 |
|----------|------------|
| 244 B | ~999 Kbps |
| 100 B | ~610 Kbps |
| 20 B | ~187 Kbps |

### 場景 4: 不同連線間隔比較

```
# 連線完成並切換到 2M PHY 後...

[任一端] tp conn 6 6           # 7.5 ms
[Slave]  tp start 244 10
         # 等待結束...

[任一端] tp conn 12 12         # 15 ms
[Slave]  tp start 244 10
         # 等待結束...

[任一端] tp conn 24 24         # 30 ms
[Slave]  tp start 244 10
```

### 場景 5: 長時間穩定性測試

```
[Slave]  tp slave
[Master] tp master
[任一端] tp phy 2
[Slave]  tp start 244 0       # duration=0 表示無限
         # 持續觀察...隨時用 tp stats 查看
[Slave]  tp stop              # 手動停止
```

---

## 9. 配置調整

### 9.1 `.config` 關鍵配置項

配置檔位置：`apps/bluetooth/ble/ipro_throughput_test/.config`

#### BLE Controller 變體

```ini
CONFIG_BTBLECONTROLLER_LIB="link2"
```

可選值：`link1_periph`、`link1`、`link2`、`link4`、`link8` 等。`link2` 支持 2 個連線。

#### TX Buffer 配置

```ini
CONFIG_BT_L2CAP_TX_BUF_COUNT=10     # Host L2CAP TX buffer 數量
CONFIG_BT_CONN_TX_MAX=10            # Host 連線層 TX 上限
```

> Controller 端的優化（`BLE_NB_TX_DESC_PER_CON=4`）已內建於預編譯的 `link2` library 中。
> 如需重建 library：`components/wireless/bluetooth/build_all_prebuilts.sh link2`

#### PHY 支援

```ini
CONFIG_CFG_LE_2M_PHY=y              # 啟用 2M PHY
CONFIG_CFG_LE_CODED_PHY=y           # 啟用 Coded PHY
CONFIG_CFG_DLE=y                     # 啟用 Data Length Extension
```

#### 連線數量

```ini
CONFIG_CFG_CON=4                     # 最大連線數
CONFIG_CFG_PERIPHERAL=y              # 啟用 Peripheral 角色
CONFIG_CFG_CENTRAL=y                 # 啟用 Central 角色
```

### 9.2 修改配置後

修改 `.config` 後必須重新編譯並燒錄：

```bash
make -C apps/bluetooth/ble/ipro_throughput_test clean
make -C apps/bluetooth/ble/ipro_throughput_test
# 然後燒錄兩塊板子（見第 4 節）
```

---

## 10. 故障排除

### 問題：燒錄工具報 "Resource busy"

**原因：** 串口被其他程序佔用（例如 serial_monitor 仍在運行）。

**解決方法：**
```bash
# 找出佔用串口的程序
lsof /dev/cu.usbserial-2120

# 結束佔用的程序
kill <PID>

# 重新燒錄
```

### 問題：板子無回應 / 串口無輸出

**可能原因與解決方法：**

1. **使用了 `/dev/tty.*`** → 改用 `/dev/cu.*`
2. **板子卡死** → 手動按住 BOOT + RESET 進入下載模式，重新燒錄
3. **串口參數錯誤** → 確認鮑率為 115200

### 問題：Master 掃描不到 Slave

**解決方法：**
1. 確認 Slave 端已執行 `tp slave` 且顯示 "advertising"
2. 確認兩塊板子距離不超過數公尺
3. Slave 端執行 `tp stop` 再執行 `tp slave` 重新廣播

### 問題：連線後立即斷開

**可能原因：**
1. 連線間隔設定太小 → 嘗試較寬鬆的間隔：`tp conn 12 12`
2. Supervision timeout 過短 → 程式預設 1 秒，通常足夠

### 問題：吞吐量明顯低於預期

**排查步驟：**

1. 確認 PHY：`tp status`，確認顯示 `PHY: 2M`
2. 確認連線間隔：`tp status`，確認 `Connection Interval: 7.50 ms`
3. 確認使用 flood 模式：`tp start 244 10`（不帶第三個參數或第三個參數為 0）
4. 確認 DLE 已啟用：連線日誌應顯示 "Requesting DLE: tx_octets=251"
5. 確認 MTU 交換成功：日誌應顯示 "MTU exchange complete: MTU=247"

### 問題：測試中板子當機 (系統凍結)

**解決方法：**
1. 手動進入下載模式（按住 BOOT + RESET）
2. 重新燒錄韌體
3. 若頻繁發生，考慮降低 TX buffer 數量或使用較寬鬆的連線間隔

### 問題：`tp start` 報 "Not ready"

**原因：** 連線尚未完成 GATT 服務發現或 Notification 未啟用。

**解決方法：**
1. `tp status` 查看當前狀態
2. 確認狀態為 `Ready` 後再執行 `tp start`
3. 若 Slave 端提示 "notifications not enabled"，等待 Master 端完成服務發現

---

## 附錄：測試結果參考值

以下為已驗證的測試結果，可作為基準比較：

| PHY | 連線間隔 | 封包大小 | 吞吐量 | 封包數 | 資料量 |
|-----|----------|----------|--------|--------|--------|
| 1M | 7.5 ms | 244 B | 507 Kbps | 2,599 | 634 KB |
| 2M | 7.5 ms | 244 B | 999 Kbps | 5,118 | 1,249 KB |
| 2M | 15 ms | 244 B | 984 Kbps | 5,047 | 1,231 KB |
| 2M | 7.5 ms | 100 B | 610 Kbps | 7,632 | 763 KB |
| 2M | 7.5 ms | 20 B | 187 Kbps | 11,669 | 233 KB |

測試時間均為 10 秒。

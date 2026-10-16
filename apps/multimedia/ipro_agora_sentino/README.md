# ipro_agora_sentino — Sentino AI 語音閘道

在 IPRO7AI Reef EVK 上執行的 AI 語音對話裝置：

1. 手機 App 透過 BLE 把 Wi-Fi 帳密與 MQTT 伺服器設定給裝置。
2. 裝置經插在卡槽的 IPRO6S FIN 連上 Wi-Fi，登入 Sentino 雲端（MQTT）。
3. 雲端配發 Agora RTC 存取權，裝置加入頻道；PDM 麥克風收音、DAC 喇叭播放，與 AI 即時語音對話。

程式模組與資料流見 [ARCHITECTURE.md](ARCHITECTURE.md)。

## 需要準備

| 項目 | 說明 |
|------|------|
| IPRO7AI Reef EVK | 預設設定（`.config`）；JP1 必須插上 |
| IPRO6S FIN 卡 | 插在 Reef 的 SD 卡槽，經 SDIO 提供 Wi-Fi。卡上要執行 IPRO6S 端韌體（`ipro6_if_wifi`，SDIO 設定），不包含於本 SDK，請向 INTELPRO 取得 |
| Sentino 裝置三元組 | 每塊板子一組 pid／uuid／secret，由 Sentino 平台配發 |
| 手機 App | Sentino 提供的 App，以 BLE 為裝置設定 Wi-Fi 與 MQTT 伺服器 |
| Agora AOSL | Agora 的 OS 抽象層，以 git submodule 取自 GitHub（見下節） |

## Agora 元件

`components/3rdparty/agora/` 隨 SDK 提供：

- `sdk/`：Agora RTC SDK 預編譯函式庫（`libagora-rtc-sdk.a`）與標頭檔。INTELPRO 經 Agora 書面同意再散布，使用時依 [Agora 服務條款](https://www.agora.io/en/terms-of-service/)，詳見 `sdk/LICENSE`。Agora 也同意客戶將 AOSL（含 IPRO7 移植）與 RTC SDK 一起用於出貨給終端使用者的裝置。
- `port/ipro7/`：AOSL 的 IPRO7 平台移植，以及 AOSL 授權條款 `port/LICENSE`。建置時以它取代 AOSL 內的 `platform/src/ipro7/`。
- `aosl/`：AOSL 核心，為 git submodule，指向 [AgoraIO-Community/aosl](https://github.com/AgoraIO-Community/aosl) 的 commit `d825607`（2026-02-05）。SDK 的 Agora RTC 函式庫是依這一版建置的，之後的版本改過 API 標頭，未經測試。

取得 AOSL：

```bash
# 以 git clone 取得 SDK 時
git submodule update --init components/3rdparty/agora/aosl

# 從發行壓縮檔解開 SDK 時
git clone https://github.com/AgoraIO-Community/aosl components/3rdparty/agora/aosl
git -C components/3rdparty/agora/aosl checkout d825607
```

缺少 AOSL 時，CMake 會停下並顯示上述步驟；AOSL 不在 `d825607` 時會出現警告。本 app 使用 AOSL（Powered by AOSL）。

## 建置

```bash
make -C apps/multimedia/ipro_agora_sentino              # IPRO7AI Reef（.config）
make -C apps/multimedia/ipro_agora_sentino BOARD=evk    # IPRO7AI EVK（config_evk，AIC8800 Wi-Fi）
make -C apps/multimedia/ipro_agora_sentino clean
```

Reef 的輸出為 `build/build_out/ipro_agora_sentino_IPRO7.bin`，EVK 的輸出在 `build_config_evk/`。EVK 設定可以建置，但未經板上驗證。

## 燒錄

使用 IntelProCube 命令列工具（`ipro_iot_tool.exe`、`ipro_iot_tool_macos` 或 `ipro_iot_tool_linux`），在 IntelProCube 目錄執行：

```bash
ipro_iot_tool_macos --chipname ipro7 --chip-variant IPRO7AI --port /dev/cu.usbserial-XXXX \
    --baudrate 921600 --pt chips/ipro7/partition/partition_cfg_2M.toml \
    --firmware <SDK>/apps/multimedia/ipro_agora_sentino/build/build_out/ipro_agora_sentino_IPRO7.bin
```

工具失敗時也會回傳 0，請確認輸出有 `[All Success]`。

## 第一次開機

主控台在 Reef 的 U6 接頭，115200 baud。

1. **輸入裝置三元組**：`sentino_triple set <pid> <uuid> <secret>`，再執行 `reboot`。
   三元組存在 PSM，重燒韌體也會保留，不放在原始碼或設定檔裡。
   沒有三元組時裝置不會連 MQTT，log 會提示這個指令。每塊板子要用自己的三元組，兩塊板子共用一組會搶同一個 MQTT 身分。
2. **配網**：裝置以 BLE 名稱 `RY` 廣播。App 經 BLE 送出 Wi-Fi 帳密與 MQTT 伺服器位址、埠號，裝置存起來後連網。
   也可以在主控台用 `wifi_set <ssid> <psk>` 設定 Wi-Fi。
3. 連上 Sentino 後，裝置取得 Agora RTC 存取權並加入頻道，就可以與 AI 對話。

## 主控台指令

| 指令 | 說明 |
|------|------|
| `sentino_triple` | 顯示 PSM 裡的三元組（secret 只顯示有沒有設定） |
| `sentino_triple set <pid> <uuid> <secret>` | 存入三元組（pid、uuid 各 ≤ 16 字元，secret ≤ 64），重開機後生效 |
| `sentino_triple clear` | 刪除三元組 |
| `ble_prov_start` | 開始 BLE 配網廣播 |
| `ble_prov_status` | 顯示綁定狀態 |
| `ble_prov_clear` | 清除綁定狀態，重開機後可重新配網 |
| `wifi_set <ssid> [psk]` | 儲存 Wi-Fi 帳密並重新連線 |
| `wifi_status` | 顯示 Wi-Fi 連線狀態 |
| `wifi_scan` | 經 FIN 卡掃描基地台 |
| `wifi_dns [host]` | 顯示 DNS 伺服器，或解析主機名稱 |
| `wifi_dns_set <ip>` | 指定 DNS 伺服器 |
| `wifi_tcp <ip> <port>` | TCP 連線測試 |
| `dac_vol <0-100>` | 設定喇叭音量 |
| `mic_gain <dB>` | 設定 PDM 麥克風數位增益 |
| `mic_dbg [秒數]` | 監看麥克風訊號強度 |
| `echo_gate` | 回音抑制參數調整 |
| `reboot` | 重新開機 |

## 網路與安全

- MQTT 以明文 TCP 連線，伺服器與埠號由 App 配網時設定。
- 登入密碼是以裝置 secret 計算的 HMAC-SHA1 簽章，secret 本身不會在網路上傳送。但簽章使用固定的時間戳，在明文連線上可被截取重送。
- 本 app 不建立 TLS 連線。量產產品請改用 MQTT over TLS，並開啟憑證驗證。
- Wi-Fi 帳密與三元組都不在原始碼或設定檔中。

## 依賴元件

- Agora RTC SDK：`components/3rdparty/agora/sdk/`（預編譯）
- AOSL：`components/3rdparty/agora/aosl/`（git submodule，AgoraIO-Community/aosl commit `d825607`）＋ IPRO7 移植 `components/3rdparty/agora/port/ipro7/`
- Opus：`components/3rdparty/opus/`
- IPRO6S FIN 主機端：`components/network/ipronet_host/`
- FreeRTOS、lwIP、mbedTLS（HMAC 簽章）、Bluetooth LE（Zephyr Host＋IPRO BLE Controller）

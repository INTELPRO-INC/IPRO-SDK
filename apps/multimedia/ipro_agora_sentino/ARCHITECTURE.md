# Agora Sentino AI Gateway -- 模組化架構文件

## 一、架構全景圖

```
┌──────────────────────────────────────────────────────────────┐
│                         main.c                                │
│          platform_init  →  gateway task  →  vTaskStart        │
└──────────────────────────────┬───────────────────────────────┘
                               │
┌──────────────────────────────▼───────────────────────────────┐
│                    app_gateway.c  (Orchestrator)               │
│                                                                │
│   管理所有模組的生命週期、初始化順序、錯誤恢復                  │
│   持有所有模組的 ops 指標，負責模組間接線                       │
│                                                                │
│   Phase 1: BLE 配網（可選，完成後釋放 BLE 記憶體）             │
│   Phase 2: network_ops->init  →  connect  →  wait_ready       │
│   Phase 3: cloud_ops->init    →  wait_connected                │
│   Phase 4: audio_out->init  +  audio_in->init                  │
│   Phase 5: cloud->request_rtc_access  →  rtc_ops->init         │
│   Phase 6: audio_route 接線 (input→RTC, RTC→output)            │
│   Loop:    健康檢查  +  錯誤恢復                               │
│                                                                │
│   ┌──────────┐  ┌──────────┐  ┌──────────┐                    │
│   │app_issue │  │app_audio │  │  SNTP    │  (附屬模組)         │
│   │  .c/h    │  │ _route   │  │          │                     │
│   └────┬─────┘  └────┬─────┘  └──────────┘                    │
│        │              │                                         │
└────────┼──────────────┼────────────────────────────────────────┘
         │              │
    ┌────▼────┐    ┌────▼────────────────────────┐
    │ 透過 ops│    │ 透過 ops callback 接線       │
    │ 指標呼叫│    │ input→on_mic_frame→rtc.send  │
    └────┬────┘    │ rtc.rx→on_rtc_rx→output.feed │
         │         └────┬──────────┬──────────┬───┘
         │              │          │          │
    ┌────▼────┐    ┌────▼───┐ ┌───▼───┐ ┌────▼────┐ ┌──────────┐
    │ cloud   │    │audio_in│ │audio  │ │  rtc    │ │ network  │
    │ _ops_t  │    │_ops_t  │ │_out   │ │ _ops_t  │ │ _ops_t   │
    │         │    │        │ │_ops_t │ │         │ │          │
    │ Sentino │    │  PDM   │ │ DAC   │ │ Agora   │ │ LTE-PPP  │
    │  MQTT   │    │ DMIC   │ │       │ │  RTC    │ │  /EMAC   │
    └─────────┘    └────────┘ └───────┘ └─────────┘ └──────────┘
```

**設計原則**：

- 每個模組透過 `*_ops_t` function pointer struct 暴露介面，實作可抽換
- 模組之間**不直接呼叫**，全部由 gateway 或 audio_route 中介層接線
- Gateway 負責初始化順序和錯誤恢復，模組只管自己的生命週期

---

## 二、模組列表

### 2.1 Network 模組

| 項目 | 說明 |
|------|------|
| **介面** | `network_ops_t` |
| **實作** | `network_wifi_ipro6s.c`（IPRO7AI Reef：插在卡槽的 IPRO6S FIN 經 SDIO 提供 Wi-Fi，預設 `.config`）、`network_wifi_aic.c`（IPRO7AI EVK：AIC8800 SDIO Wi-Fi，`config_evk`）、`network_lte.c`（LTE PPP）、`network_emac.c`（Ethernet DHCP） |
| **職責** | lwIP 協定棧初始化、連線管理、IP 取得、斷線重連 |
| **選擇方式** | 由 `.config` 的 `CONFIG_NETWORK_USE_WIFI_IPRO6S`／`CONFIG_NETWORK_USE_WIFI_AIC`／`CONFIG_NETWORK_USE_LTE` 在 compile time 決定，都未設定時用 Ethernet |

```c
typedef struct network_ops {
    const char *name;                        /* "LTE-PPP" / "EMAC" */
    app_err_t (*init)(void);                 /* 初始化 lwIP */
    app_err_t (*connect)(void);              /* 啟動連線 */
    app_err_t (*disconnect)(void);           /* 斷線 */
    int       (*is_connected)(void);         /* 0/1 */
    app_err_t (*wait_ready)(int timeout_s);  /* 等待 IP 取得 */
    app_err_t (*reconnect)(void);            /* 重連 */
} network_ops_t;
```

### 2.2 Audio Input 模組

| 項目 | 說明 |
|------|------|
| **介面** | `audio_input_ops_t` |
| **實作** | `audio_pdm.c`（PDM DMIC 3SM222） |
| **職責** | 麥克風硬體初始化、DMA 雙緩衝採集、PCM frame 回調 |

```c
typedef void (*audio_frame_cb_t)(const int16_t *pcm, uint32_t num_samples);

typedef struct audio_input_ops {
    const char *name;                              /* "PDM-DMIC" */
    app_err_t (*init)(void);                       /* 初始化硬體 + DMA */
    app_err_t (*start)(void);                      /* 開始採集 */
    app_err_t (*stop)(void);                       /* 停止採集 */
    void      (*set_callback)(audio_frame_cb_t cb);/* 設定 PCM 回調 */
    app_err_t (*set_gain)(int gain_db);            /* 設定數位增益 */
} audio_input_ops_t;
```

### 2.3 Audio Output 模組

| 項目 | 說明 |
|------|------|
| **介面** | `audio_output_ops_t` |
| **實作** | `audio_dac.c`（內建 DAC + GPIO22 喇叭致能） |
| **職責** | DAC 初始化、DMA 播放、音量控制、喇叭靜音管理 |

```c
typedef struct audio_output_ops {
    const char *name;                              /* "DAC" */
    app_err_t (*init)(void);                       /* 初始化 DAC */
    app_err_t (*start)(void);                      /* 開始播放 */
    void      (*stop)(void);                       /* 停止播放 */
    app_err_t (*feed)(const int16_t *pcm, uint32_t num_samples);
    void      (*set_volume)(int vol_0_100);        /* 音量 0~100 */
    int       (*get_volume)(void);                 /* 取得音量 */
} audio_output_ops_t;
```

### 2.4 RTC 模組（即時通訊）

| 項目 | 說明 |
|------|------|
| **介面** | `rtc_ops_t` |
| **實作** | `rtc_agora.c` + `rtc_agora_audio.c` + `rtc_agora_stream.c` |
| **職責** | Agora SDK 初始化、頻道加入/離開/重加入、音頻收發佇列、串流訊息重組、AI 狀態解析 |

```c
typedef void (*rtc_rx_audio_cb_t)(const int16_t *pcm, uint32_t num_samples);

typedef enum {
    RTC_AI_STATE_LISTENING,
    RTC_AI_STATE_THINKING,
    RTC_AI_STATE_SPEAKING,
} rtc_ai_state_t;
typedef void (*rtc_ai_state_cb_t)(rtc_ai_state_t state);

typedef enum {
    RTC_EVENT_CONNECTED,
    RTC_EVENT_DISCONNECTED,
    RTC_EVENT_RECONNECTING,
    RTC_EVENT_ERROR,
} rtc_event_t;
typedef void (*rtc_event_cb_t)(rtc_event_t event, int error_code);

typedef struct rtc_ops {
    const char *name;                              /* "Agora" */
    app_err_t (*init)(const char *app_id, const char *token,
                      const char *channel, uint32_t uid);
    app_err_t (*deinit)(void);
    app_err_t (*leave)(void);
    app_err_t (*rejoin)(const char *app_id, const char *token,
                        const char *channel, uint32_t uid);
    int       (*is_connected)(void);
    app_err_t (*send_audio)(const void *pcm, size_t len);
    void      (*set_rx_callback)(rtc_rx_audio_cb_t cb);
    void      (*set_ai_state_callback)(rtc_ai_state_cb_t cb);
    void      (*set_event_callback)(rtc_event_cb_t cb);
    int       (*is_ai_speaking)(void);
    uint32_t  (*get_remote_user_count)(void);
} rtc_ops_t;
```

**內部拆分**：

| 檔案 | 職責 |
|------|------|
| `rtc_agora.c` | RTC 連線管理（init/join/leave/rejoin）、SDK event handler、ops struct 定義 |
| `rtc_agora_audio.c` | RX/TX FreeRTOS queue + task、Opus 解碼、PCM 編碼 |
| `rtc_agora_stream.c` | 串流訊息重組（base64 + multi-segment）、AI state/transcription JSON 解析 |

### 2.5 Cloud 模組

| 項目 | 說明 |
|------|------|
| **介面** | `cloud_ops_t` |
| **實作** | `cloud_sentino.c`（封裝 `sentino/` 子模組群） |
| **職責** | MQTT 連線、裝置綁定/回報、AI access 取得、issue 分發 |

```c
typedef void (*cloud_msg_handler_t)(const char *code, char *payload);

typedef struct cloud_ops {
    const char *name;                              /* "Sentino-MQTT" */
    app_err_t (*init)(void);                       /* 初始化 + 連線 */
    app_err_t (*deinit)(void);                     /* 斷線 + 清理 */
    int       (*is_connected)(void);
    app_err_t (*wait_connected)(int timeout_s);
    app_err_t (*publish)(const char *code, const char *data_json, int ack);
    void      (*set_issue_handler)(cloud_msg_handler_t handler);
    app_err_t (*request_rtc_access)(void *access_out, size_t access_size);
} cloud_ops_t;
```

`sentino/` 子模組群（**不變**，`cloud_sentino.c` 只是對外封裝）：

| 檔案 | 職責 |
|------|------|
| `app_sentino.c/h` | MQTT client 管理、topic 訂閱/發佈、訊息路由 |
| `sentino_auth.c/h` | 裝置三元組（pid/uuid/secret）、HMAC 簽名 |
| `sentino_protocol.c/h` | Sentino JSON 協議封裝/解析 |
| `sentino_lifecycle.c/h` | 裝置資訊回報、時間同步、重置 |
| `sentino_ai.c/h` | AI agent access 請求、NFC tag 回報 |
| `sentino_property.c/h` | 屬性回報 |
| `sentino_ota.c/h` | OTA 升級（stub） |
| `sentino_cmd.c/h` | Shell 命令（debug 用） |

---

## 三、模組間通訊方式

### 3.1 設計原則

模組之間**禁止直接 `#include` 對方的 header 並呼叫函數**。通訊只有兩種方式：

1. **ops 指標呼叫**（由 gateway 或 issue dispatcher 發起）
2. **callback 回調**（由模組觸發，由 audio_route 或 gateway 接收）

### 3.2 資料流

```
麥克風採集流 (TX)：
  audio_pdm (ISR) ──DMA callback──▶ audio_route.on_mic_frame()
                                     ├─ 靜音偵測（peak < threshold → drop）
                                     ├─ 半雙工閘控（AI speaking → drop）
                                     └─ rtc_ops->send_audio()
                                          └─ rtc_agora TX queue → Agora SDK

遠端音頻流 (RX)：
  Agora SDK ──on_audio_data──▶ rtc_agora RX queue → rtc_agora_audio task
                                  ├─ Opus/PCM 解碼
                                  └─ rtc_rx_audio_cb_t()
                                       └─ audio_route.on_rtc_rx_audio()
                                            ├─ 16kHz → 32kHz 升頻（2x 插值）
                                            └─ audio_output_ops->feed()

AI 狀態流：
  Agora SDK ──on_stream_message──▶ rtc_agora_stream task
                                     ├─ multi-segment 重組
                                     ├─ base64 解碼
                                     ├─ JSON 解析（message.state）
                                     └─ rtc_ai_state_cb_t()
                                          └─ audio_route.on_ai_state_changed()
                                               └─ 更新半雙工閘控狀態

雲端命令流：
  MQTT broker ──subscribe──▶ app_sentino ──issue_handler──▶ app_issue.on_issue()
                                                              ├─ property_set → audio_output_ops->set_volume()
                                                              ├─ agora_join   → rtc_ops->rejoin()
                                                              ├─ reset        → sentino_lifecycle_reset()
                                                              └─ 其他 → log + ack

RTC 連線事件：
  Agora SDK ──on_connection_lost──▶ rtc_agora.c
                                      └─ rtc_event_cb_t(RTC_EVENT_DISCONNECTED)
                                           └─ app_gateway 健康監控迴圈處理
```

### 3.3 callback 註冊時機

所有 callback 在 gateway Phase 6（audio_route 接線）時由 `audio_route_init()` 統一註冊：

```c
app_err_t audio_route_init(const audio_route_cfg_t *cfg) {
    cfg->rtc->set_rx_callback(on_rtc_rx_audio);
    cfg->rtc->set_ai_state_callback(on_ai_state_changed);
    cfg->input->set_callback(on_mic_frame);
    return APP_OK;
}
```

RTC event callback 由 gateway 自行註冊：

```c
s_config.rtc->set_event_callback(on_rtc_event);
```

---

## 四、Gateway 啟動流程

```
main()
  ├─ platform_init()
  ├─ ipro_setup_heap()
  ├─ hal_boot2_init()
  ├─ network_ops->init()          // lwIP 協定棧（scheduler 前）
  ├─ shell_init_with_task()       // Shell（可選）
  ├─ xTaskCreate(gateway_task)
  └─ vTaskStartScheduler()

gateway_task()
  │
  ├─ Phase 1: BLE 配網
  │    ├─ app_ble_prov_init()
  │    ├─ if (!bound) → app_ble_prov_start() → 等待綁定完成
  │    ├─ app_ble_prov_stop() + app_ble_prov_deinit()  // 釋放 BLE 記憶體
  │    └─ 讀取 userId / assetId
  │
  ├─ Phase 2: 網路連線
  │    ├─ network_ops->connect()      // 啟動 LTE modem / DHCP
  │    ├─ （同時初始化 audio HW，利用等待時間）
  │    ├─ network_ops->wait_ready(30) // 等待 IP
  │    └─ SNTP 時間同步
  │
  ├─ Phase 3: 雲端 MQTT
  │    ├─ cloud_ops->init()           // 連 MQTT broker
  │    ├─ cloud_ops->wait_connected(15)
  │    ├─ sentino_lifecycle_info_report()
  │    ├─ sentino_lifecycle_time_sync()
  │    └─ bind report (userId + assetId)
  │
  ├─ Phase 4: Audio 硬體
  │    ├─ audio_output_ops->init()    // DAC 初始化
  │    └─ audio_input_ops->init()     // PDM DMIC 初始化
  │
  ├─ Phase 5: RTC 加入
  │    ├─ cloud_ops->request_rtc_access()  // 向 Sentino 雲端要 Agora credentials
  │    ├─ audio_output_ops->start()        // 啟動 DAC（RTC 前準備好）
  │    ├─ rtc_ops->init(app_id, token, channel, uid)
  │    └─ 等待 channel join callback
  │
  ├─ Phase 6: 接線
  │    ├─ audio_route_init(cfg)       // 註冊所有 callback
  │    ├─ audio_input_ops->start()    // 開始採集 → TX 開始送
  │    └─ app_issue_init(audio_out, rtc)  // 註冊 issue handler
  │
  └─ Phase 7: 健康監控迴圈
       └─ while(1) { 每 5 秒檢查 network/cloud/rtc 狀態 }
```

**Phase 2 與 Phase 4 的交錯**：Audio 硬體初始化（DAC + DMIC）不需要網路，因此在等待 PPP IP 的同時進行，節省啟動時間。

---

## 五、錯誤恢復機制

### 5.1 錯誤碼定義

所有模組統一使用 `app_err_t`：

```c
typedef enum {
    APP_OK          =  0,
    APP_ERR_INIT    = -1,   /* 初始化失敗 */
    APP_ERR_TIMEOUT = -2,   /* 操作逾時 */
    APP_ERR_NETWORK = -3,   /* 網路相關錯誤 */
    APP_ERR_AGORA   = -4,   /* RTC SDK 錯誤 */
    APP_ERR_MQTT    = -5,   /* MQTT 連線/發佈錯誤 */
    APP_ERR_NOMEM   = -6,   /* 記憶體不足 */
    APP_ERR_INVALID = -7,   /* 參數無效 */
} app_err_t;
```

### 5.2 各層恢復策略

| 層級 | 故障 | 恢復方式 | 擁有者 |
|------|------|----------|--------|
| Network | PPP 斷線 | `network_lte.c` 內建 watchdog task，自動 modem re-init + PPP reconnect（每 10 秒檢測） | network 模組 |
| Cloud | MQTT 斷線 | gateway 健康監控偵測到 `cloud->is_connected() == 0` → 呼叫 `cloud->init()` 重連 | gateway |
| RTC | 連線中斷 | Agora SDK 內部自動重連 + `on_reconnecting` callback；若 token 過期 → gateway 重新 `request_rtc_access` + `rtc->rejoin` | gateway + SDK |
| Audio | 硬體異常 | 目前無自動恢復；DAC/DMIC 初始化失敗 → gateway 進入 retry loop | gateway |
| 啟動 | 任一 Phase 失敗 | `goto retry` → 延遲 10 秒 → 從 Phase 2 重新開始（或 reboot） | gateway |

### 5.3 健康監控迴圈

```c
/* gateway_task Phase 7 */
while (1) {
    vTaskDelay(pdMS_TO_TICKS(5000));

    /* Network */
    if (!network->is_connected()) {
        IPRO_LOGW(TAG, "Network down");
        network->reconnect();
    }

    /* Cloud MQTT */
    if (!cloud->is_connected()) {
        IPRO_LOGW(TAG, "Cloud disconnected");
        cloud->init();  /* sentino 模組內部處理重連 */
    }

    /* RTC */
    if (!rtc->is_connected() && rtc_state == MOD_STATE_RUNNING) {
        IPRO_LOGW(TAG, "RTC disconnected");
        /* Agora SDK 自動重連中，不需手動介入 */
        /* 除非收到 token 過期 callback → 重新要 access */
    }
}
```

---

## 六、檔案清單與說明

### 核心框架

| 檔案 | 行數 | 說明 |
|------|------|------|
| `main.c` | ~80 | platform_init + heap + shell + task create |
| `app_module.h` | ~120 | 所有 ops struct + error codes + callback types |
| `app_gateway.c/h` | ~300 | Orchestrator：啟動流程 + 健康監控 + retry |
| `app_audio_route.c/h` | ~120 | 音頻路由：升頻 + 靜音 + 半雙工 + callback 接線 |
| `app_issue.c/h` | ~150 | Sentino issue 分發器（volume/agora_join/reset） |

### 模組實作

| 檔案 | 行數 | 介面 | 說明 |
|------|------|------|------|
| `network_lte.c` | ~150 | `network_ops_t` | LTE PPP：modem init + watchdog + reconnect |
| `network_emac.c` | ~80 | `network_ops_t` | Ethernet DHCP（`#else` 路徑） |
| `audio_pdm.c` | ~320 | `audio_input_ops_t` | PDM DMIC (3SM222) 採集 + shell debug |
| `audio_dac.c` | ~340 | `audio_output_ops_t` | DAC 播放 + DMA + 音量 + shell |
| `rtc_agora.c` | ~400 | `rtc_ops_t` | Agora SDK 連線 + event handlers |
| `rtc_agora_audio.c` | ~200 | (內部) | RX/TX queue + task + Opus decode |
| `rtc_agora_stream.c` | ~250 | (內部) | 串流重組 + AI state 解析 |
| `cloud_sentino.c` | ~80 | `cloud_ops_t` | 封裝 sentino/ 模組群 |

### 子模組（不變）

| 目錄/檔案 | 說明 |
|-----------|------|
| `sentino/app_sentino.c/h` | MQTT client 管理 |
| `sentino/sentino_auth.c/h` | 裝置認證 |
| `sentino/sentino_protocol.c/h` | 協議封裝 |
| `sentino/sentino_lifecycle.c/h` | 生命週期管理 |
| `sentino/sentino_ai.c/h` | AI access 請求 |
| `sentino/sentino_property.c/h` | 屬性回報 |
| `sentino/sentino_ota.c/h` | OTA（stub） |
| `sentino/sentino_cmd.c/h` | Shell 命令 |
| `ble/app_ble_prov.c/h` | BLE 配網 |
| `nfc/app_nfc.c/h` | NFC 讀取（stub） |

### 工具

| 檔案 | 說明 |
|------|------|
| `app_json.h` | 輕量 JSON 字串提取（header-only） |
| `app_log.h` | 日誌封裝（header-only） |

---

## 七、如何新增或替換模組

### 7.1 替換網路模組（例：換成 WiFi）

1. 建立 `network_wifi.c`
2. 實作 `network_ops_t` 所有函數指標：

```c
/* network_wifi.c */
#include "app_module.h"

static app_err_t wifi_init(void)      { /* lwIP + WiFi driver init */ }
static app_err_t wifi_connect(void)   { /* WiFi STA connect */ }
static app_err_t wifi_disconnect(void){ /* WiFi disconnect */ }
static int       wifi_is_connected(void) { /* check netif */ }
static app_err_t wifi_wait_ready(int timeout_s) { /* wait DHCP */ }
static app_err_t wifi_reconnect(void) { /* disconnect + connect */ }

const network_ops_t network_wifi_ops = {
    .name         = "WiFi",
    .init         = wifi_init,
    .connect      = wifi_connect,
    .disconnect   = wifi_disconnect,
    .is_connected = wifi_is_connected,
    .wait_ready   = wifi_wait_ready,
    .reconnect    = wifi_reconnect,
};
```

3. 在 `app_gateway.c` 更換指標：

```c
static const gateway_config_t s_config = {
    .network = &network_wifi_ops,   /* 原本是 &network_lte_ops */
    /* 其餘不變 */
};
```

4. 更新 `CMakeLists.txt`：加入 `network_wifi.c`，移除 `network_lte.c`
5. 更新 `.config`：移除 `CONFIG_LTE_MODEM=y`，加入 WiFi 相關 config

**Gateway、audio_route、cloud、rtc 不需要任何修改。**

### 7.2 替換音頻輸入（例：換成 I2S 麥克風）

1. 建立 `audio_i2s_in.c`
2. 實作 `audio_input_ops_t`：

```c
const audio_input_ops_t audio_i2s_in_ops = {
    .name         = "I2S-MIC",
    .init         = i2s_mic_init,
    .start        = i2s_mic_start,
    .stop         = i2s_mic_stop,
    .set_callback = i2s_mic_set_callback,
    .set_gain     = i2s_mic_set_gain,
};
```

3. 在 `app_gateway.c` 更換：

```c
.audio_in = &audio_i2s_in_ops,  /* 原本是 &audio_pdm_ops */
```

**audio_route 層不需修改**，因為它只透過 `ops->set_callback()` 和 `ops->start()` 操作。

### 7.3 替換音頻輸出（例：換成 I2S DAC）

同理，建立 `audio_i2s_out.c`，實作 `audio_output_ops_t`，更換 `app_gateway.c` 的 `.audio_out` 指標。

**注意**：如果新的 DAC 取樣率不是 32kHz，需要更新 `audio_route_cfg_t.output_sample_rate`，audio_route 的升頻邏輯會據此調整。

### 7.4 替換雲端平台（例：換成自建 HTTP）

1. 建立 `cloud_http.c`，實作 `cloud_ops_t`
2. `request_rtc_access` 改為向自建 server 請求 RTC credentials
3. `set_issue_handler` 改為 HTTP long-polling 或 WebSocket
4. 更換 `app_gateway.c` 的 `.cloud` 指標

### 7.5 替換 RTC（例：換成 WebRTC）

1. 建立 `rtc_webrtc.c`，實作 `rtc_ops_t`
2. 如果不需要 AI state 回調（沒有 stream message），`set_ai_state_callback` 可以是空操作
3. 更換 `app_gateway.c` 的 `.rtc` 指標

### 7.6 新增模組的 checklist

- [ ] 建立 `<module>.c` 並實作對應的 `*_ops_t`
- [ ] 在檔案底部定義 `const *_ops_t <module>_ops = { ... };`
- [ ] 在 `app_gateway.c` 的 `gateway_config_t` 更換指標
- [ ] 更新 `CMakeLists.txt` 的 `target_sources()`
- [ ] 更新 `.config` 的相關 Kconfig
- [ ] `make clean && make` 驗證建置
- [ ] 檢查 linker map 確認 flash/OCRAM 用量

---

## 八、記憶體影響

| 項目 | 大小 | 位置 | 說明 |
|------|------|------|------|
| `network_ops_t` | ~28 bytes | Flash (const) | 6 function pointers + 1 name pointer |
| `audio_input_ops_t` | ~24 bytes | Flash (const) | 5 function pointers + 1 name pointer |
| `audio_output_ops_t` | ~28 bytes | Flash (const) | 6 function pointers + 1 name pointer |
| `rtc_ops_t` | ~52 bytes | Flash (const) | 12 function pointers + 1 name pointer |
| `cloud_ops_t` | ~32 bytes | Flash (const) | 7 function pointers + 1 name pointer |
| `audio_route_cfg_t` | ~28 bytes | Stack/BSS | 3 ops pointers + 3 config values |
| `gateway_config_t` | ~20 bytes | Flash (const) | 5 ops pointers |

**總額外開銷**：~212 bytes Flash + ~28 bytes RAM。相比整個 app 的 400KB+ Flash 和 90KB+ OCRAM，影響可忽略。

拆檔案不會增加程式碼量（同樣的邏輯只是搬家），linker 的 `--gc-sections` 會自動移除未使用的 ops struct。

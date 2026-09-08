# IPRO SDK Logging System

IPRO SDK 的日誌系統提供了完整的日誌記錄功能，支援多種日誌等級、彩色輸出、時間戳記、執行緒 ID 等功能。

## 特性

- **多等級日誌**: ERROR, WARN, INFO, DEBUG, VERBOSE
- **彩色輸出**: 支援終端機彩色輸出，提高可讀性
- **時間戳記**: 自動添加時間戳記
- **執行緒安全**: 在 FreeRTOS 環境下支援執行緒安全
- **模組標籤**: 支援模組標籤，便於追蹤日誌來源
- **Buffer 轉存**: 支援十六進位和 ASCII 格式的 buffer 轉存
- **效能測量**: 提供效能測量宏，方便程式效能分析
- **編譯時最佳化**: 根據日誌等級進行編譯時最佳化

## 使用方法

### 1. 初始化日誌系統

```c
#include "ipro_log.h"

// 使用預設配置初始化
ipro_log_config_t config = IPRO_LOG_DEFAULT_CONFIG();
config.global_level = IPRO_LOG_LEVEL_DEBUG;
ipro_log_init(&config);
```

### 2. 使用日誌宏

```c
#define LOG_TAG "MY_MODULE"

// 不同等級的日誌
IPRO_LOGE(LOG_TAG, "This is an error message: %d", error_code);
IPRO_LOGW(LOG_TAG, "This is a warning message");
IPRO_LOGI(LOG_TAG, "This is an info message");
IPRO_LOGD(LOG_TAG, "This is a debug message: %s", debug_info);
IPRO_LOGV(LOG_TAG, "This is a verbose message");
```

### 3. Buffer 轉存

```c
uint8_t data[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

// 十六進位格式轉存
IPRO_LOG_BUFFER_HEX(LOG_TAG, data, sizeof(data));

// 十六進位 + ASCII 格式轉存
IPRO_LOG_BUFFER_HEXDUMP(LOG_TAG, data, sizeof(data));
```

### 4. 效能測量

```c
IPRO_LOG_PERF_START(LOG_TAG, function_name);
// 執行需要測量的程式碼
some_function();
IPRO_LOG_PERF_END(LOG_TAG, function_name);
```

### 5. 動態日誌等級控制

```c
// 設定新的日誌等級
ipro_log_set_level(IPRO_LOG_LEVEL_INFO);

// 檢查日誌等級是否啟用
if (ipro_log_level_enabled(IPRO_LOG_LEVEL_DEBUG)) {
    // 只有在 DEBUG 等級啟用時才執行
    expensive_debug_operation();
}
```

## 配置選項

### Kconfig 配置

在 `menuconfig` 中可以配置以下選項：

- `CONFIG_IPRO_LOG_ENABLE`: 啟用日誌系統
- `CONFIG_LOG_DEFAULT_LEVEL`: 預設日誌等級
- `CONFIG_LOG_COLORS`: 啟用彩色輸出
- `CONFIG_LOG_TIMESTAMP`: 啟用時間戳記
- `CONFIG_LOG_THREAD_ID`: 啟用執行緒 ID
- `CONFIG_LOG_BUFFER_SIZE`: 日誌緩衝區大小

### 編譯時配置

```c
// 在編譯時定義日誌等級
#define CONFIG_LOG_DEFAULT_LEVEL 4  // DEBUG level

// 啟用/停用特定功能
#define CONFIG_LOG_COLORS 1
#define CONFIG_LOG_TIMESTAMP 1
```

## 輸出格式

預設的日誌輸出格式為：

```
LEVEL (timestamp) [thread_id] TAG: message
```

範例：
```
I (1234) [123456] BLE_DEMO: BLE advertising started successfully
E (1235) [123456] BLE_DEMO: Failed to start advertising (status: 1)
D (1236) [123456] BLE_DEMO: Current state = 2
```

## 最佳實務

1. **使用有意義的模組標籤**：使用簡短但描述性的標籤
2. **適當的日誌等級**：
   - ERROR: 錯誤條件，需要立即關注
   - WARN: 警告條件，可能導致問題
   - INFO: 一般資訊，正常操作流程
   - DEBUG: 除錯資訊，開發時使用
   - VERBOSE: 詳細除錯資訊，非常詳細的追蹤

3. **避免在中斷中使用日誌**：日誌系統可能會阻塞，不適合在中斷服務程式中使用

4. **使用效能測量**：利用 `IPRO_LOG_PERF_*` 宏來測量關鍵函數的執行時間

5. **編譯時最佳化**：在正式版本中使用較低的日誌等級以提高效能

## 與舊系統的遷移

如果您的專案目前使用 `printf`，可以按照以下步驟遷移：

1. 包含 `ipro_log.h`
2. 定義模組標籤 `#define LOG_TAG "MODULE_NAME"`
3. 將 `printf` 替換為對應的日誌宏
4. 在應用程式啟動時初始化日誌系統

範例遷移：
```c
// 舊的程式碼
printf("Device connected, id: %d\n", conn_id);
printf("ERROR: Failed to initialize\n");

// 新的程式碼
IPRO_LOGI(LOG_TAG, "Device connected, id: %d", conn_id);
IPRO_LOGE(LOG_TAG, "Failed to initialize");
```

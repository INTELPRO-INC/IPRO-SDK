# IPRO BLE Remote Control Demo

## 概述

這是一個互動式 BLE 遙控器應用程式，展示如何透過手機 App（如 nRF Connect、LightBlue）與 IPRO7 開發板進行雙向互動。

## 功能特色

### 1. 📱 LED 控制服務 (LED Control Service)
- 手機可遠程控制 LED 開/關
- 支援 LED 亮度調節 (0-100%)
- 支援 LED 閃爍模式設定

### 2. 🔘 按鈕狀態服務 (Button Service)
- 開發板按鈕狀態即時通知
- 長按/短按/雙擊事件通知
- 按鈕按下次數統計

### 3. 🌡️ 環境感測服務 (Environment Service)  
- 模擬溫度數據 (可擴展為真實傳感器)
- 模擬濕度數據
- 週期性自動通知或手動讀取

### 4. 💬 UART 透傳服務 (UART Service)
- 雙向文字通訊
- 支援長訊息分包傳輸
- 類似 Nordic UART Service (NUS)

### 5. 📷 PIR 動態偵測 + 圖片傳輸服務 (PIR Image Service)
- PIR 紅外線感測器動態偵測
- 動態事件即時通知到手機
- 320x480 JPEG 圖片擷取與傳輸
- 分塊傳輸協議 (支援大圖片)

### 6. 🔄 OTA 韌體更新服務 (OTA Service) - NEW
- 無線韌體更新 (Over-The-Air Update)
- **MTU 517 自動協商**: 支援大封包傳輸，效能提升 6 倍
- **動態負載偵測**: 自動適應手機 MTU (504-508 bytes)
- **高速傳輸**: 12.3 KB/s (vs 舊版 2 KB/s)，250KB 韌體 20 秒完成
- 雙分區啟動保護
- CRC32 完整性驗證
- 支援最大 800KB 韌體
- 進度監控與錯誤處理
- 記憶體優化 (4KB 緩衝區動態分配)

**效能規格 (實測):**

| 功能 | MTU | Payload | 傳輸速度 | 備註 |
|-----|-----|---------|---------|------|
| **Image Transfer** | 517 | 506 bytes | **101.4 KB/s** | 56KB/542ms ⚡ |
| **OTA Update** | 517 | 504 bytes | 12.3 KB/s | 250KB/20s |
| Legacy (MTU 247) | 247 | 244 bytes | 2.0 KB/s | 舊版 |

> 💡 **建議**: 手機 APP 在連線後使用 `requestMtu(513)` 以獲得最佳效能  
> ⚡ **圖片傳輸特別快**: 得益於專用 TX 任務和優化的 chunk 間隔

## 服務 UUID

| 服務 | UUID | 說明 |
|------|------|------|
| LED Control | `0xFF10` | LED 控制服務 |
| Button | `0xFF20` | 按鈕狀態服務 |
| Environment | `0xFF30` | 環境感測服務 |
| UART | `0xFF40` | UART 透傳服務 |
| PIR Image | `0xFF50` | PIR 動態偵測 + 圖片傳輸服務 |
| **OTA** | **`0xFF60`** | **OTA 韌體更新服務 (MTU 517 支援)** |
| Image Upload | `0xFF70` | 手機上傳圖片到裝置 |
| Reef Hardware | `0xFF80` | 僅 IPRO7AI Reef 版：LED 顏色、按鍵識別、麥克風音量、提示音、板卡資訊（見「IPRO7AI Reef 板」） |

### 特徵值 UUID

| 特徵值 | UUID | 屬性 | 說明 |
|--------|------|------|------|
| LED State | `0xFF11` | Read/Write | LED 開關狀態 |
| LED Brightness | `0xFF12` | Read/Write | LED 亮度 (0-100) |
| LED Mode | `0xFF13` | Read/Write | LED 模式 (常亮/閃爍) |
| Button State | `0xFF21` | Read/Notify | 按鈕狀態 |
| Button Count | `0xFF22` | Read | 按鈕計數 |
| Temperature | `0xFF31` | Read/Notify | 溫度值 |
| Humidity | `0xFF32` | Read/Notify | 濕度值 |
| UART TX | `0xFF41` | Notify | 裝置發送到手機 |
| UART RX | `0xFF42` | Write | 手機發送到裝置 |
| **PIR Status** | `0xFF51` | Read/Notify | PIR 動態狀態 |
| **Image Control** | `0xFF52` | Write | 圖片控制命令 |
| **Image Data** | `0xFF53` | Notify | 圖片數據塊 |
| **Image Info** | `0xFF54` | Read/Notify | 圖片元資訊 |
| **OTA Control** | `0xFF61` | Write | OTA 控制命令 (START/END/VERIFY/REBOOT) |
| **OTA Data** | `0xFF62` | Write No Response | OTA 韌體數據 (504-byte packets @ MTU 513+) |
| **OTA Status** | `0xFF63` | Read/Notify | OTA 狀態和進度 |

---

## 📱 手機 APP 開發指南 (Mobile App Development Guide)

本節提供 APP 團隊實作手機端應用程式所需的完整技術規格。

### 藍牙連接流程

```
┌─────────────┐     ┌─────────────────┐     ┌─────────────────┐
│  1. 掃描     │ --> │  2. 連接        │ --> │  3. 協商 MTU     │
│  Scan for   │     │  Connect to     │     │  Request        │
│  "IPRO-Rmt" │     │  Device         │     │  MTU 513        │
└─────────────┘     └─────────────────┘     └─────────────────┘
                                                    │
                    ┌─────────────────┐     ┌───────▼───────────┐
                    │  5. 啟用通知     │ <-- │  4. 發現服務      │
                    │  Enable CCCD    │     │  Discover        │
                    │  Notifications  │     │  Services        │
                    └─────────────────┘     └───────────────────┘
                             │
                    ┌────────▼────────┐
                    │  6. 開始互動     │
                    │  Read/Write     │
                    │  Characteristics│
                    └─────────────────┘
```

> 💡 **重要**: OTA 更新前必須先協商 MTU 513+，否則傳輸速度會降低 6 倍

### 設備廣播資訊

| 欄位 | 值 |
|------|-----|
| Device Name | `IPRO-Remote` |
| Appearance | `0x0180` (Generic Remote Control) |
| TX Power Level | 0 dBm |
| Service UUIDs | `0xFF10`, `0xFF20`, `0xFF30`, `0xFF40`, `0xFF50`, `0xFF60` |

---

## 🔌 PIR Image Service 詳細規格 (0xFF50)

### 服務架構

```
PIR Image Service (0xFF50)
├── PIR Status (0xFF51)
│   ├── Properties: Read, Notify
│   └── CCCD: 0x2902
├── Image Control (0xFF52)
│   └── Properties: Write
├── Image Data (0xFF53)
│   ├── Properties: Notify
│   └── CCCD: 0x2902
└── Image Info (0xFF54)
    ├── Properties: Read, Notify
    └── CCCD: 0x2902
```

### PIR Status (0xFF51) - PIR 狀態特徵值

**屬性**: Read, Notify  
**長度**: 9 bytes

#### 數據格式

```c
typedef struct __attribute__((packed)) {
    uint8_t  motion_detected;   // Offset 0: 1=動態偵測, 0=無動態
    uint32_t timestamp;         // Offset 1-4: 時間戳記 (ms, Little Endian)
    uint32_t event_count;       // Offset 5-8: 總事件次數 (Little Endian)
} pir_status_t;                 // Total: 9 bytes
```

#### 範例解析 (Kotlin)

```kotlin
fun parsePirStatus(data: ByteArray): PirStatus {
    val buffer = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN)
    return PirStatus(
        motionDetected = buffer.get() == 1.toByte(),
        timestamp = buffer.int.toLong() and 0xFFFFFFFFL,
        eventCount = buffer.int.toLong() and 0xFFFFFFFFL
    )
}
```

#### 範例解析 (Swift)

```swift
struct PirStatus {
    let motionDetected: Bool
    let timestamp: UInt32
    let eventCount: UInt32
    
    init(data: Data) {
        motionDetected = data[0] == 1
        timestamp = data.subdata(in: 1..<5).withUnsafeBytes { $0.load(as: UInt32.self).littleEndian }
        eventCount = data.subdata(in: 5..<9).withUnsafeBytes { $0.load(as: UInt32.self).littleEndian }
    }
}
```

### Image Control (0xFF52) - 圖片控制特徵值

**屬性**: Write  
**長度**: 1-4 bytes

#### 控制命令

| 命令 | 值 | 長度 | 說明 |
|------|-----|------|------|
| CAPTURE | `0x01` | 1 byte | 觸發拍照 |
| REQUEST_INFO | `0x02` | 1 byte | 請求圖片資訊 |
| REQUEST_CHUNK | `0x03` | 3 bytes | 請求特定數據塊 |
| ABORT | `0x04` | 1 byte | 中止傳輸 |

#### REQUEST_CHUNK 格式

```
Byte 0: 0x03 (Command)
Byte 1: Chunk Index Low
Byte 2: Chunk Index High
```

#### 範例 (Kotlin)

```kotlin
// 拍照
fun captureImage(characteristic: BluetoothGattCharacteristic) {
    characteristic.value = byteArrayOf(0x01)
    gatt.writeCharacteristic(characteristic)
}

// 請求圖片資訊
fun requestImageInfo(characteristic: BluetoothGattCharacteristic) {
    characteristic.value = byteArrayOf(0x02)
    gatt.writeCharacteristic(characteristic)
}

// 請求特定數據塊
fun requestChunk(characteristic: BluetoothGattCharacteristic, index: Int) {
    val data = ByteBuffer.allocate(3).order(ByteOrder.LITTLE_ENDIAN)
        .put(0x03.toByte())
        .putShort(index.toShort())
        .array()
    characteristic.value = data
    gatt.writeCharacteristic(characteristic)
}

// 中止傳輸
fun abortTransfer(characteristic: BluetoothGattCharacteristic) {
    characteristic.value = byteArrayOf(0x04)
    gatt.writeCharacteristic(characteristic)
}
```

### Image Info (0xFF54) - 圖片資訊特徵值

**屬性**: Read, Notify  
**長度**: 10 bytes

#### 數據格式

```c
typedef struct __attribute__((packed)) {
    uint32_t image_size;    // Offset 0-3: 圖片總大小 (bytes, Little Endian)
    uint16_t chunk_count;   // Offset 4-5: 總數據塊數量 (Little Endian)
    uint16_t chunk_size;    // Offset 6-7: 每塊大小 (通常 244 bytes)
    uint8_t  format;        // Offset 8: 格式 (0=JPEG, 1=RAW)
    uint8_t  status;        // Offset 9: 狀態
} img_info_t;               // Total: 10 bytes
```

#### 狀態值

| 值 | 狀態 | 說明 |
|----|------|------|
| 0 | IDLE | 閒置，無圖片 |
| 1 | CAPTURING | 拍照中 |
| 2 | READY | 圖片就緒，可傳輸 |
| 3 | TRANSFERRING | 傳輸中 |
| 4 | ERROR | 錯誤 |

#### 範例解析 (Kotlin)

```kotlin
data class ImageInfo(
    val imageSize: Int,
    val chunkCount: Int,
    val chunkSize: Int,
    val format: ImageFormat,
    val status: ImageStatus
)

enum class ImageFormat { JPEG, RAW }
enum class ImageStatus { IDLE, CAPTURING, READY, TRANSFERRING, ERROR }

fun parseImageInfo(data: ByteArray): ImageInfo {
    val buffer = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN)
    return ImageInfo(
        imageSize = buffer.int,
        chunkCount = buffer.short.toInt() and 0xFFFF,
        chunkSize = buffer.short.toInt() and 0xFFFF,
        format = if (buffer.get() == 0.toByte()) ImageFormat.JPEG else ImageFormat.RAW,
        status = ImageStatus.values()[buffer.get().toInt()]
    )
}
```

### Image Data (0xFF53) - 圖片數據特徵值

**屬性**: Notify  
**長度**: 4 + data bytes (最大 248 bytes)

#### 數據塊格式

```
┌────────────┬────────────┬─────────────────────┐
│ Chunk Index│ Chunk Size │      Image Data     │
│  (2 bytes) │  (2 bytes) │    (≤240 bytes)     │
└────────────┴────────────┴─────────────────────┘
```

```c
typedef struct __attribute__((packed)) {
    uint16_t chunk_index;   // Offset 0-1: 數據塊索引 (0-based)
    uint16_t chunk_size;    // Offset 2-3: 本塊數據大小 (CRITICAL: 必須從此讀取!)
    uint8_t  data[];        // Offset 4+: 圖片數據
} img_chunk_header_t;
```

> ⚠️ **重要說明 (BLE MTU Considerations)**:  
> **IMAGE_CHUNK_SIZE = 240** 確保 BLE notification (4 header + 240 payload = 244 bytes) 
> 符合標準 BLE MTU 限制 (247 - 3 ATT header = 244 bytes available)。
> 
> 每個 chunk 的實際數據大小存儲在 `chunk_size` 欄位 (byte[2-3])：
> - 前面的 chunks: 通常是 240 bytes  
> - 最後一個 chunk: 可能不同 (例如 166 bytes for 886-byte JPEG)  
> 
> **錯誤做法** ❌:  
> ```kotlin
> val PAYLOAD_SIZE = 240  // 硬編碼，會導致數據遺失！
> val payload = ByteArray(PAYLOAD_SIZE)
> System.arraycopy(data, 4, payload, 0, PAYLOAD_SIZE)
> ```
> 
> **正確做法** ✅:  
> ```kotlin
> val chunkSize = ByteBuffer.wrap(data, 2, 2)
>     .order(ByteOrder.LITTLE_ENDIAN).short.toInt()
> val payload = ByteArray(chunkSize)  // 使用 header 中的大小
> System.arraycopy(data, 4, payload, 0, chunkSize)
> ```

#### 範例接收 (Kotlin)

```kotlin
class ImageReceiver {
    private var imageInfo: ImageInfo? = null
    private var receivedChunks = mutableMapOf<Int, ByteArray>()
    
    fun onImageInfoReceived(data: ByteArray) {
        imageInfo = parseImageInfo(data)
        receivedChunks.clear()
    }
    
    fun onChunkReceived(data: ByteArray): Float {
        val buffer = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN)
        val chunkIndex = buffer.short.toInt() and 0xFFFF
        val chunkSize = buffer.short.toInt() and 0xFFFF
        
        val chunkData = ByteArray(chunkSize)
        buffer.get(chunkData)
        receivedChunks[chunkIndex] = chunkData
        
        // 返回進度
        return receivedChunks.size.toFloat() / (imageInfo?.chunkCount ?: 1)
    }
    
    fun isComplete(): Boolean {
        val info = imageInfo ?: return false
        return receivedChunks.size == info.chunkCount
    }
    
    fun assembleImage(): ByteArray? {
        val info = imageInfo ?: return null
        if (!isComplete()) return null
        
        val imageData = ByteArrayOutputStream()
        for (i in 0 until info.chunkCount) {
            receivedChunks[i]?.let { imageData.write(it) }
        }
        return imageData.toByteArray()
    }
}
```

---

## 📷 完整圖片傳輸流程

### 流程圖

```
┌─────────┐                      ┌─────────┐
│  手機   │                      │  設備   │
└────┬────┘                      └────┬────┘
     │                                │
     │  1. Enable Image Data Notify   │
     │ ──────────────────────────────>│
     │                                │
     │  2. Enable Image Info Notify   │
     │ ──────────────────────────────>│
     │                                │
     │  3. Write CAPTURE (0x01)       │
     │ ──────────────────────────────>│
     │                                │
     │  4. Notify: Image Info         │
     │ <──────────────────────────────│ (status=CAPTURING)
     │                                │
     │  5. Notify: Image Info         │
     │ <──────────────────────────────│ (status=READY, size, chunks)
     │                                │
     │  6. Write REQUEST_CHUNK(0)     │
     │ ──────────────────────────────>│
     │                                │
     │  7. Notify: Chunk 0 data       │
     │ <──────────────────────────────│
     │                                │
     │  8. Write REQUEST_CHUNK(1)     │
     │ ──────────────────────────────>│
     │                                │
     │  9. Notify: Chunk 1 data       │
     │ <──────────────────────────────│
     │                                │
     │        ... repeat ...          │
     │                                │
     │  N. All chunks received        │
     │  -> Assemble JPEG              │
     │  -> Display image              │
     ▼                                ▼
```

### 自動傳輸模式 (PIR 觸發)

```
┌─────────┐                      ┌─────────┐
│  手機   │                      │  設備   │
└────┬────┘                      └────┬────┘
     │                                │
     │  1. Enable PIR Status Notify   │
     │ ──────────────────────────────>│
     │                                │
     │  [PIR sensor detects motion]   │
     │                                │
     │  2. Notify: PIR Status         │
     │ <──────────────────────────────│ (motion=1)
     │                                │
     │  [Device auto captures image]  │
     │                                │
     │  3. Notify: Image Info         │
     │ <──────────────────────────────│ (status=READY)
     │                                │
     │  4. App starts chunk requests  │
     │ ──────────────────────────────>│
     ▼                                ▼
```

---

## 🔧 Android 完整範例

### BLE 服務連接

```kotlin
class BleRemoteService : Service() {
    private val PIR_IMAGE_SERVICE_UUID = UUID.fromString("0000FF50-0000-1000-8000-00805F9B34FB")
    private val PIR_STATUS_CHAR_UUID = UUID.fromString("0000FF51-0000-1000-8000-00805F9B34FB")
    private val IMG_CTRL_CHAR_UUID = UUID.fromString("0000FF52-0000-1000-8000-00805F9B34FB")
    private val IMG_DATA_CHAR_UUID = UUID.fromString("0000FF53-0000-1000-8000-00805F9B34FB")
    private val IMG_INFO_CHAR_UUID = UUID.fromString("0000FF54-0000-1000-8000-00805F9B34FB")
    private val CCCD_UUID = UUID.fromString("00002902-0000-1000-8000-00805F9B34FB")
    
    private val imageReceiver = ImageReceiver()
    
    private val gattCallback = object : BluetoothGattCallback() {
        override fun onServicesDiscovered(gatt: BluetoothGatt, status: Int) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                val pirService = gatt.getService(PIR_IMAGE_SERVICE_UUID)
                pirService?.let {
                    // Enable notifications
                    enableNotification(gatt, it.getCharacteristic(PIR_STATUS_CHAR_UUID))
                    enableNotification(gatt, it.getCharacteristic(IMG_DATA_CHAR_UUID))
                    enableNotification(gatt, it.getCharacteristic(IMG_INFO_CHAR_UUID))
                }
            }
        }
        
        override fun onCharacteristicChanged(
            gatt: BluetoothGatt,
            characteristic: BluetoothGattCharacteristic,
            value: ByteArray
        ) {
            when (characteristic.uuid) {
                PIR_STATUS_CHAR_UUID -> {
                    val status = parsePirStatus(value)
                    if (status.motionDetected) {
                        onMotionDetected(status)
                    }
                }
                IMG_INFO_CHAR_UUID -> {
                    imageReceiver.onImageInfoReceived(value)
                    val info = parseImageInfo(value)
                    if (info.status == ImageStatus.READY) {
                        // Start requesting chunks
                        requestNextChunk(gatt, 0)
                    }
                }
                IMG_DATA_CHAR_UUID -> {
                    val progress = imageReceiver.onChunkReceived(value)
                    onTransferProgress(progress)
                    
                    if (imageReceiver.isComplete()) {
                        val jpegData = imageReceiver.assembleImage()
                        jpegData?.let { onImageReceived(it) }
                    } else {
                        // Request next chunk
                        requestNextChunk(gatt, imageReceiver.receivedChunks.size)
                    }
                }
            }
        }
    }
    
    private fun enableNotification(gatt: BluetoothGatt, char: BluetoothGattCharacteristic?) {
        char?.let {
            gatt.setCharacteristicNotification(it, true)
            val descriptor = it.getDescriptor(CCCD_UUID)
            descriptor?.value = BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE
            gatt.writeDescriptor(descriptor)
        }
    }
    
    private fun requestNextChunk(gatt: BluetoothGatt, index: Int) {
        val pirService = gatt.getService(PIR_IMAGE_SERVICE_UUID)
        val ctrlChar = pirService?.getCharacteristic(IMG_CTRL_CHAR_UUID)
        ctrlChar?.let {
            val data = ByteBuffer.allocate(3).order(ByteOrder.LITTLE_ENDIAN)
                .put(0x03.toByte())
                .putShort(index.toShort())
                .array()
            it.value = data
            gatt.writeCharacteristic(it)
        }
    }
    
    fun captureImage(gatt: BluetoothGatt) {
        val pirService = gatt.getService(PIR_IMAGE_SERVICE_UUID)
        val ctrlChar = pirService?.getCharacteristic(IMG_CTRL_CHAR_UUID)
        ctrlChar?.let {
            it.value = byteArrayOf(0x01)
            gatt.writeCharacteristic(it)
        }
    }
}
```

### 顯示 JPEG 圖片

```kotlin
fun onImageReceived(jpegData: ByteArray) {
    val bitmap = BitmapFactory.decodeByteArray(jpegData, 0, jpegData.size)
    runOnUiThread {
        imageView.setImageBitmap(bitmap)
    }
}
```

---

## 🍎 iOS 完整範例 (Swift)

```swift
class BleRemoteManager: NSObject, CBCentralManagerDelegate, CBPeripheralDelegate {
    
    let pirImageServiceUUID = CBUUID(string: "FF50")
    let pirStatusCharUUID = CBUUID(string: "FF51")
    let imgCtrlCharUUID = CBUUID(string: "FF52")
    let imgDataCharUUID = CBUUID(string: "FF53")
    let imgInfoCharUUID = CBUUID(string: "FF54")
    
    var imageReceiver = ImageReceiver()
    var imgCtrlChar: CBCharacteristic?
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        guard service.uuid == pirImageServiceUUID else { return }
        
        for char in service.characteristics ?? [] {
            switch char.uuid {
            case pirStatusCharUUID, imgDataCharUUID, imgInfoCharUUID:
                peripheral.setNotifyValue(true, for: char)
            case imgCtrlCharUUID:
                imgCtrlChar = char
            default:
                break
            }
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        guard let data = characteristic.value else { return }
        
        switch characteristic.uuid {
        case pirStatusCharUUID:
            let status = PirStatus(data: data)
            if status.motionDetected {
                onMotionDetected(status)
            }
            
        case imgInfoCharUUID:
            imageReceiver.onImageInfoReceived(data)
            if let info = imageReceiver.imageInfo, info.status == .ready {
                requestChunk(peripheral, index: 0)
            }
            
        case imgDataCharUUID:
            let progress = imageReceiver.onChunkReceived(data)
            onTransferProgress(progress)
            
            if imageReceiver.isComplete() {
                if let jpegData = imageReceiver.assembleImage() {
                    onImageReceived(jpegData)
                }
            } else {
                requestChunk(peripheral, index: imageReceiver.receivedChunks.count)
            }
            
        default:
            break
        }
    }
    
    func captureImage(_ peripheral: CBPeripheral) {
        guard let char = imgCtrlChar else { return }
        peripheral.writeValue(Data([0x01]), for: char, type: .withResponse)
    }
    
    func requestChunk(_ peripheral: CBPeripheral, index: Int) {
        guard let char = imgCtrlChar else { return }
        var data = Data([0x03])
        data.append(contentsOf: withUnsafeBytes(of: UInt16(index).littleEndian) { Array($0) })
        peripheral.writeValue(data, for: char, type: .withResponse)
    }
}

class ImageReceiver {
    var imageInfo: ImageInfo?
    var receivedChunks = [Int: Data]()
    
    func onImageInfoReceived(_ data: Data) {
        imageInfo = ImageInfo(data: data)
        receivedChunks.removeAll()
    }
    
    func onChunkReceived(_ data: Data) -> Float {
        let chunkIndex = Int(data[0]) | (Int(data[1]) << 8)
        let chunkSize = Int(data[2]) | (Int(data[3]) << 8)
        let chunkData = data.subdata(in: 4..<(4 + chunkSize))
        receivedChunks[chunkIndex] = chunkData
        
        return Float(receivedChunks.count) / Float(imageInfo?.chunkCount ?? 1)
    }
    
    var isComplete: Bool {
        guard let info = imageInfo else { return false }
        return receivedChunks.count == info.chunkCount
    }
    
    func assembleImage() -> Data? {
        guard let info = imageInfo, isComplete else { return nil }
        var imageData = Data()
        for i in 0..<info.chunkCount {
            if let chunk = receivedChunks[i] {
                imageData.append(chunk)
            }
        }
        return imageData
    }
}
```

---

## 🪸 IPRO7AI Reef 板

`defconfig_reef` 把同一套 GATT 服務接到 Reef 的實際硬體上。原有服務的 UUID 與資料格式不變，既有手機 App 照常可用；Reef 才有的功能集中在新的 `0xFF80` 服務。

| 服務 | Reef 上的行為 |
|------|--------------|
| LED `0xFF10` | WS2812 RGB LED：開關、亮度 0-100 % 真的調光、模式 2/3 為慢閃/快閃；顏色由 `0xFF81` 設定（預設白色） |
| Button `0xFF20` | SW1(BOOT)、SW3、SW4、SW5 任一鍵都會送 `0xFF21`；按下、放開、長按（≥1 s）、雙擊（400 ms 內） |
| Environment `0xFF30` | 溫度＝晶片內建溫度感測器（0.01 °C）；板上沒有濕度感測器，濕度固定回報 0 |
| PIR Image `0xFF50` | OV5640 實拍 1280×720 JPEG（Q70，實測 50-100 KB、MTU 247 傳輸約 1-1.5 秒，視畫面而定）；「PIR」改為相機畫面動態偵測，手機開啟 `0xFF51` 通知時啟動，偵測到動態會自動拍照；有插 SD 記憶卡時每張照片另存 `sd:/REMOTE/IMGnnnnn.JPG` |
| OTA `0xFF60` | 經 boot2 的 A/B OTA（下述） |
| Reef `0xFF80` | 見下表 |

| 特徵值 | UUID | 屬性 | 格式 |
|--------|------|------|------|
| LED Colour | `0xFF81` | Read/Write | 3 bytes：R, G, B |
| Key Event | `0xFF82` | Read/Notify | 6 bytes：鍵（0 BOOT, 1 SW3, 2 SW4, 3 SW5）、事件（`btn_event_t`：0 放開, 1 按下, 2 長按, 3 雙擊）、按下次數 u32 LE |
| Sound Level | `0xFF83` | Read/Notify | int8：PDM 麥克風前一秒的音量 dBFS；開啟通知才會開始量測 |
| Tone | `0xFF84` | Write | 5 bytes：頻率 Hz u16 LE（20-12000）、時間 ms u16 LE（1-10000）、音量 % u8；喇叭與耳機同時出聲（找遙控器） |
| Board Info | `0xFF85` | Read | UTF-8 文字：相機型號與解析度、SD 卡、編譯時間 |

本機按鍵行為：短按 BOOT（SW1）會拍一張照片，連線中且已開啟圖片通知時自動傳給手機。

### 相機

- 相機在第一次拍照或啟動動態偵測時開始串流，之後一直保持：DVP 停止後再啟動常常收不到影格（SDK 其他相機 app 也從不停止串流），串流後每次拍照只要約 15-60 ms。串流超過 2 秒沒有新影格會自動重啟。
- 動態偵測：畫面切成 16×9 區塊，每個區塊各自學習背景亮度與平常的雜訊；變化同時超過 16 灰階與該區塊雜訊 3 倍、至少 3 個區塊、連續兩次（200 ms 間隔）才算動態，3 秒沒有變化視為結束。燈光閃爍的區域會自動變得不敏感。啟動後約 4 秒內只學習不回報。
- 感測器 720p 30 fps；增益超過 8 倍仍不夠亮時，AEC 才把影格拉長，最多兩倍（最低 15 fps）。防閃爍濾波設為 60 Hz 市電（`OV5640_MAINS_HZ`，`components/sensor/ov5640.c`）。亮暗快速切換實測不掉幀、不需重啟串流。
- 這顆 FLEX-24 模組的原始畫面是上下鏡像；感測器在 binning 模式翻轉會讓去馬賽克錯一個 Bayer 相位（紅藍或綠紫互換），所以改由擷取 DMA（DVP2AXI flip mode）由下往上寫，畫面方向正確、不耗 CPU。也因此不提供 180° 旋轉。
- 畫質設定（2026-10-07 用螢幕上的校正圖量測）：感測器銳化 0x5302 = 0x28、IPRO7 ISP Cb/Cr 飽和 1.125 倍、拍照 JPEG 品質 70。比原本的銳化 0x18／品質 75 邊緣清晰約 17%，檔案還小約 6%。
- `reef reg`／`reef mm` 可以即時讀寫感測器與 ISP 暫存器、`reef jpegq` 調整拍照品質，調畫質用；開機後恢復預設。

### 編譯與第一次燒錄（boot2 + A/B 分割表 + 韌體）

```bash
make -C apps/bluetooth/ble/ipro_ble_remote DEFCONFIG=defconfig_reef
# -> build_reef/build_out/ipro_ble_remote_IPRO7.bin

# --chip-variant IPRO7AI：Reef（QFN56）的 flash pin。工具失敗時也回傳 0，請看輸出有沒有 [All Success]
ipro_iot_tool_macos --chipname ipro7 --chip-variant IPRO7AI --port /dev/cu.usbmodemXXXX --baudrate 921600 \
    --boot2 <IntelProCube>/chips/ipro7/builtin_imgs/boot2_v0.5.2/ipro_boot2_IPRO7_v0.5.2.bin \
    --pt apps/bluetooth/ble/ipro_ble_remote/partition_cfg_2M_remote_ab.toml \
    --firmware apps/bluetooth/ble/ipro_ble_remote/build_reef/build_out/ipro_ble_remote_IPRO7.bin
```

開機訊息會先出現 `[BOOT2] IPRO7 Boot2 v0.5.2`，然後從 FW A（0x12000）啟動 App。

### BLE OTA

`partition_cfg_2M_remote_ab.toml`：FW A 0x12000（824 KB，執行區）、FW B 0xE0000（512 KB，OTA 套件暫存區）。App 把手機送來的 `.xz.ota` 套件寫進 FW B、驗 CRC32、切換分割區後重開；boot2 驗套件的 SHA256，通過後才抹除 FW A 並解壓寫入，再從 FW A 開機。套件驗證失敗時 boot2 進 USB 救援模式，不會變磚。

```bash
# 產生套件（需要 IntelProCube 命令列工具：IPRO_FLASH_TOOL 指向它，或放在 PATH 裡）
IPRO_FLASH_TOOL=<IntelProCube>/ipro_iot_tool_macos sh apps/bluetooth/ble/ipro_ble_remote/tools/make_reef_ota.sh
# -> build_reef/build_out/ipro_ble_remote_reef.xz.ota（約 345 KB）

# Mac 上用 bleak 推送（python3 -m venv .venv && .venv/bin/pip install bleak）
.venv/bin/python apps/bluetooth/ble/ipro_ble_remote/tools/reef_ota.py build_reef/build_out/ipro_ble_remote_reef.xz.ota
```

手機 App 推送的也必須是 `.xz.ota` 套件，不是 `.bin`。實測（2026-10-06，macOS，MTU 247）：345 KB 套件傳輸 13-17 秒。

⚠️ 不要在 Reef 上用標準 `partition_cfg_2M.toml` 做 OTA：寫進它的 FW B（0x12A000）並切換後，板子完全沒有輸出，只能進下載模式重燒。

### 測試工具

`tools/reef_ble_test.py`（bleak）：`info led env sound tone img` 自動測試；`keys`、`motion` 需要有人按鍵 / 在鏡頭前移動。

### Shell：`reef`

| 命令 | 說明 |
|------|------|
| `reef status` | 板卡資訊、LED、相機、動態偵測、溫度、音量 |
| `reef led <r> <g> <b>` | 設定 LED 顏色並點亮 |
| `reef tone <hz> <ms> <vol%>` | 喇叭 / 耳機提示音 |
| `remote img capture` | 拍一張照片（連線中且開啟圖片通知時會傳給手機） |
| `reef motion on\|off\|map` | 動態偵測開關；`map` 印出最近一次的區塊變化圖 |
| `reef meter on\|off` | 麥克風音量量測 |
| `reef reg <hex addr> [hex val]` | 讀／寫 OV5640 暫存器（調校用） |
| `reef jpegq <1-100>` | 拍照 JPEG 品質（預設 70） |
| `reef mm <hex addr> [hex val]` | 讀／寫多媒體區塊暫存器（0x30040000-0x3004FFFC：ISP、WDR、Gamma 等，調校用） |

### 程式結構

驅動沿用 `apps/platform/ipro7ai_reef_demo/src` 的 `rgb_led.c`、`keys.c`、`audio.c`、`sdcard.c`、`cam.c`（CMake 直接引用，不複製）；`src/board/reef_hw.c` 是唯一操作硬體的工作（BLE host 執行緒只投遞請求，也讓 FPU 只有一個使用者），`src/board/reef_jpeg.c` 做 MJPEG 單張編碼，`src/services/reef_service.c` 是 `0xFF80` 服務，`src/app/remote_reef.c` 把它們接到原有服務上。

## 快速開始

### 編譯

```bash
./build_freertos.sh ipro_ble_remote build
```

### 燒錄並連接

1. 燒錄韌體到 IPRO7 開發板
2. 連接 UART 控制台 (115200 baud)
3. 看到 "BLE Remote Control Ready" 表示啟動成功

### 手機連接

1. 打開 nRF Connect 或 LightBlue App
2. 掃描並連接 "IPRO-Remote"
3. 發現 5 個自訂服務 (包含 PIR Image Service)
4. 開始互動！

## CLI 命令

| 命令 | 說明 | 範例 |
|------|------|------|
| `remote status` | 顯示連接狀態 | `remote status` |
| `remote led <on\|off>` | 控制 LED | `remote led on` |
| `remote led bright <0-100>` | 設定 LED 亮度 | `remote led bright 50` |
| `remote env notify <on\|off>` | 開關環境通知 | `remote env notify on` |
| `remote uart send <msg>` | 發送 UART 訊息 | `remote uart send Hello` |
| `remote pir start [gpio]` | 啟動 PIR 感測器 | `remote pir start 10` |
| `remote pir stop` | 停止 PIR 感測器 | `remote pir stop` |
| `remote pir trigger` | 模擬 PIR 觸發 | `remote pir trigger` |
| `remote pir status` | 查看 PIR 狀態 | `remote pir status` |
| `remote img capture` | 拍攝照片 | `remote img capture` |
| `remote img info` | 查看圖片資訊 | `remote img info` |
| `remote img transfer` | 開始傳輸圖片 | `remote img transfer` |
| `help` | 顯示幫助 | `help` |

## 手機操作指南

### 控制 LED

1. 連接裝置後，找到 **LED Control Service** (0xFF10)
2. 寫入 **LED State** (0xFF11):
   - `0x01` = 開燈
   - `0x00` = 關燈
3. 寫入 **LED Brightness** (0xFF12):
   - `0x00` - `0x64` (0-100%)

### 接收按鈕事件

1. 找到 **Button Service** (0xFF20)
2. 啟用 **Button State** (0xFF21) 的 Notification
3. 按下開發板按鈕，手機會收到：
   - `0x01` = 按下
   - `0x00` = 放開
   - `0x02` = 長按
   - `0x03` = 雙擊

### 讀取環境數據

1. 找到 **Environment Service** (0xFF30)
2. 讀取或訂閱通知：
   - **Temperature** (0xFF31): 格式 `int16_t` (0.01°C 為單位)
   - **Humidity** (0xFF32): 格式 `uint8_t` (0-100%)

### UART 雙向通訊

1. 找到 **UART Service** (0xFF40)
2. 啟用 **UART TX** (0xFF41) 的 Notification
3. 寫入文字到 **UART RX** (0xFF42)
4. 裝置會回應（Echo 模式）或執行命令

### PIR 動態偵測 + 圖片傳輸 (新功能)

1. 找到 **PIR Image Service** (0xFF50)
2. 啟用所有通知 (0xFF51, 0xFF53, 0xFF54)
3. 等待 PIR 觸發或寫入 `0x01` 到 Image Control (0xFF52) 拍照
4. 收到 Image Info 後，依序請求各數據塊
5. 組裝完成後顯示 JPEG 圖片

## 技術架構

```
+------------------+     BLE GATT      +------------------+
|   手機 App       | <--------------> |  IPRO7 開發板     |
|   (Android/iOS)  |                   |                  |
+------------------+                   +------------------+
       |                                      |
       | Write LED State                      | LED GPIO
       | Read Temperature                     | Temp Sensor
       | Enable Notification                  | Button GPIO
       | Send UART Data                       | UART Console
       | Receive PIR Events                   | PIR Sensor GPIO
       | Receive Image Chunks                 | Camera / Simulated
       |                                      |
```

## 檔案結構

```
apps/bluetooth/ble/ipro_ble_remote/
├── README.md                      # 本文件
├── OTA_IMPLEMENTATION_GUIDE.md     # OTA 實作指南 (給 APP Team)
├── CMakeLists.txt                 # 編譯設定
├── Kconfig                        # 配置選項
├── .config                      # 專案配置
├── main.c                         # 主程式入口
├── build/                         # 編譯輸出
├── src/                           # 源代碼目录
│   ├── app/                       # 應用層
│   │   ├── remote_app.c          # 應用邏輯
│   │   └── remote_cli.c          # CLI 命令
│   ├── services/                  # BLE 服務層
│   │   ├── remote_services.c     # 基礎服務 (LED/Button/Env/UART)
│   │   ├── pir_image_service.c   # PIR + 圖片傳輸服務
│   │   └── ota_service.c         # OTA 韌體更新服務
│   └── data/                      # 數據文件
│       ├── pic_array.h           # 圖片數據 1
│       ├── pic_cat_array.h       # 圖片數據 2
│       └── pic_cat_2_array.h     # 圖片數據 3
└── include/                       # 公共標頭文件
    ├── remote_app.h               # 應用標頭
    ├── remote_services.h          # 服務標頭
    ├── pir_image_service.h        # PIR 服務標頭
    └── ota_service.h              # OTA 服務標頭
```

### 項目結構說明

- **src/app/**: 應用程式邏輯層，負責 BLE 事件處理和 CLI 交互
- **src/services/**: BLE GATT 服務實作，各服務獨立模塊化
- **src/data/**: 圖片數據和其他資源文件
- **include/**: 公共 API 標頭文件，供外部引用

## 圖片規格

| 項目 | 規格 |
|------|------|
| 解析度 | 320 × 480 像素 |
| 格式 | Baseline JPEG |
| 色彩 | 灰階 (可擴展為彩色) |
| 預估大小 | 3-10 KB |
| 數據塊大小 | 244 bytes |
| 傳輸時間 | ~2-5 秒 (視 BLE 連線品質) |

## 編譯與部署

### 編譯指令

```bash
# 從 SDK 根目录編譯
cd /path/to/ipro_sdk
./build_freertos.sh ipro_ble_remote build

# 編譯輸出位置
apps/bluetooth/ble/ipro_ble_remote/build/build_out/ipro_ble_remote_IPRO7.elf
```

### 資源使用

| 資源 | 使用量 | 百分比 |
|------|--------|----------|
| **Flash** | 390,472 bytes | 37.24% |
| **OCRAM** | 96 KB | 60.00% |
| **HBNRAM** | 136 bytes | 3.32% |

### 優化成果

本項目已經過 **日誌優化** 和 **編譯警告修復**：

- ✅ **Flash 節省**: 從 423,688 bytes 降至 390,472 bytes，節省 **33,216 bytes (7.8%)**
- ✅ **無編譯警告**: 所有 app 和 ipro_ble_lib 警告已修復
- ✅ **日誌統一**: 使用 `[OTA]` `[BLE]` `[LED]` `[IMG]` `[PIR]` `[UART]` 等統一前綴
- ✅ **代碼清晰**: 移除表情符號，減少凗餘訊息
- ✅ **OTA 效能**: MTU 517 協商，傳輸速度從 2 KB/s 提升至 **12.3 KB/s (6 倍)**
- ✅ **記憶體優化**: 4KB OTA 緩衝區動態分配，傳輸完成後釋放
- ✅ **堆疊優化**: OTA 任務堆疊從 2KB 優化至 1KB

## 擴展建議

1. **真實傳感器**: 連接 I2C/SPI 溫濕度傳感器 (如 SHT30)
2. **PWM LED**: 使用 PWM 實現真正的亮度調節
3. **✅ OTA 更新**: 已實作！支援 MTU 517 無線韌體更新 (請參考 [OTA_IMPLEMENTATION_GUIDE.md](OTA_IMPLEMENTATION_GUIDE.md))
4. **多連接**: 支援多個手機同時連接
5. **真實相機**: 整合 DVP/SPI Camera 模組
6. **彩色圖片**: 擴展 JPEG 編碼器支援 RGB

## 📚 延伸閱讀

- **[OTA Implementation Guide](OTA_IMPLEMENTATION_GUIDE.md)** - APP 團隊 OTA 實作完整指南 (v3.0)
  - **新功能**: MTU 517 協商和動態負載偵測
  - BLE 服務規格 (支援 504-byte packets)
  - 協議流程和效能優化
  - Android/iOS 範例代碼 (含 MTU negotiation)
  - 錯誤處理和除錯指南
  - 效能基準測試 (12.3 KB/s 實測)

## License

Copyright (C) IPRO SDK 2025

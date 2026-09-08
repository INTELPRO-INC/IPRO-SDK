# IPRO BLE Remote - OTA Firmware Update Implementation Guide

**Document Version:** 3.0  
**Date:** December 27, 2025  
**Target Audience:** Mobile APP Development Team (Android & iOS)  
**Latest Updates:** MTU 517 negotiation, dynamic payload sizing, 6x performance improvement

---

## 📋 Table of Contents

1. [Overview](#overview)
2. [Architecture](#architecture)
3. [BLE Service Specification](#ble-service-specification)
4. [MTU Negotiation & Dynamic Payload](#mtu-negotiation--dynamic-payload)
5. [Protocol Details](#protocol-details)
6. [Implementation Flow](#implementation-flow)
7. [Android Implementation](#android-implementation)
8. [iOS Implementation](#ios-implementation)
9. [Error Handling](#error-handling)
10. [Testing Guide](#testing-guide)
11. [Performance Optimization](#performance-optimization)
12. [FAQ](#faq)

---

## 🎯 Overview

The IPRO BLE Remote supports **wireless firmware updates (OTA)** via Bluetooth Low Energy, enabling seamless device upgrades without physical access.

### Key Features

- ✅ **MTU 517 Negotiation**: Automatic negotiation for large packet support
- ✅ **Dynamic Payload Sizing**: Auto-adapts to negotiated MTU (up to 512 bytes per packet)
- ✅ **High Throughput**: 12.3 KB/s (98+ Kbps) real-world transfer speed
- ✅ **Reliable Transfer Protocol**: WRITE_NO_RESPONSE with sequence tracking
- ✅ **Integrity Verification**: CRC32 checksum validation
- ✅ **Dual-Partition Safety**: Automatic rollback on failure
- ✅ **Progress Monitoring**: Real-time update status
- ✅ **Asynchronous Flash**: Non-blocking operations prevent BLE timeout
- ✅ **Maximum Size**: 800 KB firmware support
- ✅ **Memory Efficient**: 4KB flash buffer with dynamic allocation

### System Requirements

| Component | Requirement |
|-----------|-------------|
| **BLE Connection** | Stable connection required |
| **MTU Size** | 247-517 bytes (auto-negotiated) |
| **Recommended MTU** | 513+ bytes for optimal performance |
| **Firmware Format** | `.bin` binary file |
| **File Preparation** | Firmware + embedded CRC32 |
| **Mobile OS** | Android 5.0+ / iOS 10.0+ |
| **OTA Task Stack** | 1KB minimum |

---

## 🏗️ Architecture

### Dual-Partition Boot System

```
┌──────────────────────────────────────────────────────┐
│               IPRO7 Flash Memory Layout               │
├──────────────────────────────────────────────────────┤
│  Partition 0 (Active)      │  800 KB  │ 0x13000000  │
│  Partition 1 (Backup)      │  800 KB  │ 0x130C8000  │
│  Bootloader Metadata       │  4 KB    │ 0x13190000  │
└──────────────────────────────────────────────────────┘
```

**Boot Logic:**
1. Bootloader reads metadata to determine active partition
2. Verifies CRC32 of active partition
3. If verification fails → switch to backup partition
4. If both fail → enter recovery mode

### OTA Update Process

```
┌─────────────┐     ┌──────────────┐     ┌─────────────┐
│  Mobile App │────▶│  BLE GATT    │────▶│  OTA Service│
│             │     │  Service     │     │  (Device)   │
└─────────────┘     └──────────────┘     └─────────────┘
       │                                         │
       │  1. START (size, CRC32)                │
       ├────────────────────────────────────────▶
       │                                         │
       │  2. ACK (ready)                        │
       ◀────────────────────────────────────────┤
       │                                         │
       │  3. DATA packets (0~N-1)               │
       ├────────────────────────────────────────▶
       │     (244-byte chunks)                  │
       │                                         │
       │  4. END                                 │
       ├────────────────────────────────────────▶
       │                                         │
       │  5. VERIFY (async flash complete)      │
       ├────────────────────────────────────────▶
       │                                         │
       │  6. Status notification                │
       │     (CRC32 match result)               │
       ◀────────────────────────────────────────┤
       │                                         │
       │  7. REBOOT                              │
       ├────────────────────────────────────────▶
       │                                         │
       │  Device reboots to new firmware        │
       └─────────────────────────────────────────
```

---

## 📡 BLE Service Specification

### Service UUID: `0xFF60`

The OTA service provides three characteristics:

| Characteristic | UUID | Properties | Size | Description |
|----------------|------|------------|------|-------------|
| **OTA Control** | `0xFF61` | Write | 1-9 bytes | Command channel |
| **OTA Data** | `0xFF62` | Write No Response | 250 bytes | Firmware data transfer |
| **OTA Status** | `0xFF63` | Read, Notify | 13 bytes | Status and progress |

---

### 1. OTA Control (0xFF61)

**Properties:** Write  
**Command Format:**

#### START Command
```
Byte 0:     0x01 (START)
Byte 1-4:   Firmware size (uint32_t, Little Endian)
Byte 5-8:   CRC32 checksum (uint32_t, Little Endian)
```

**Example:**
```
// Firmware: 258,288 bytes, CRC32: 0x4F8608A9
01 F0 F0 03 00 A9 08 86 4F
```

#### DATA Command
```
Byte 0-1:   Sequence number (uint16_t, Little Endian, 0-based)
Byte 2-247: Firmware data (up to 244 bytes)
```

**Example:**
```
// Packet #0: seq=0x0000, 244 bytes data
00 00 [244 bytes of firmware data]

// Packet #1: seq=0x0001, 244 bytes data  
01 00 [244 bytes of firmware data]
```

**Note:** Sequence number auto-increments. No explicit command byte needed for DATA packets.

#### END Command
```
Byte 0:     0x02 (END)
```

Signals completion of data transfer. Device begins queued flash write operations.

#### VERIFY Command
```
Byte 0:     0x03 (VERIFY)
```

Ensures all asynchronous flash writes complete before CRC32 verification.

#### REBOOT Command
```
Byte 0:     0x04 (REBOOT)
```

Reboots device into new firmware (only valid after successful verification).

#### ABORT Command
```
Byte 0:     0x05 (ABORT)
```

Cancels ongoing OTA update.

---

### 2. OTA Data (0xFF62)

**Properties:** Write No Response  
**Max Size:** 250-518 bytes (dynamic based on MTU)

**Packet Format:**
```
┌──────────────┬──────────────┬─────────────────────────┐
│ Sequence (2B)│              │   Firmware Data         │
│  Little      │              │   (244-512 bytes)       │
│  Endian      │              │                         │
└──────────────┴──────────────┴─────────────────────────┘
```

**MTU-Based Optimization:**
- **MTU 247**: Packet size 246 bytes (2-byte seq + 244-byte payload) - Standard BLE
- **MTU 513+**: Packet size 506 bytes (2-byte seq + 504-byte payload) - Optimized (default)
- **MTU 517**: Device negotiates 517 bytes, auto-detects actual payload from phone
- Uses WRITE_NO_RESPONSE for maximum throughput

**Transmission Strategy:**
- Send packets continuously without waiting for ACK
- Use sequence numbers for packet loss detection
- Device auto-detects payload size from first packet
- Retransmit missing packets based on bitmap feedback (future enhancement)

**Performance Impact:**
| MTU | Payload Size | Transfer Speed | Time (250KB) |
|-----|-------------|----------------|--------------|
| 247 | 244 bytes | ~2 KB/s | ~125 seconds |
| 513+ | 504 bytes | ~12.3 KB/s | ~20 seconds |

> 📝 **Note**: The device will automatically adjust to your phone's actual MTU. Most modern phones support MTU 512+.

---

### 3. OTA Status (0xFF63)

**Properties:** Read, Notify  
**Size:** 13 bytes

**Status Format:**
```c
typedef struct __attribute__((packed)) {
    uint8_t  state;              // Byte 0: Current state
    uint32_t total_size;         // Byte 1-4: Total firmware size
    uint32_t received_bytes;     // Byte 5-8: Bytes received so far
    uint32_t calculated_crc32;   // Byte 9-12: Device-calculated CRC32
} ota_status_t;
```

**State Values:**

| Value | State | Description |
|-------|-------|-------------|
| 0 | IDLE | No OTA operation |
| 1 | READY | Ready to receive firmware |
| 2 | RECEIVING | Receiving firmware data |
| 3 | VERIFY | Verifying CRC32 |
| 4 | COMPLETE | OTA successful, ready to reboot |
| 5 | ERROR | OTA failed |

**Example (Little Endian):**
```
// State: RECEIVING (2)
// Total: 258,288 bytes (0x0003F0F0)
// Received: 122,000 bytes (0x0001DC90)
// CRC32: Not yet calculated (0x00000000)
02 F0 F0 03 00 90 DC 01 00 00 00 00 00
```

**Notification Timing:**
- Sent every ~10 packets during RECEIVING state
- Sent immediately on state changes
- Always sent after VERIFY command

---

## 🔄 MTU Negotiation & Dynamic Payload

### MTU Exchange Process

The OTA service supports **MTU 517 negotiation** for optimal transfer performance. The device automatically adapts packet size based on the negotiated MTU.

#### Connection Sequence

```
┌─────────────┐                      ┌─────────────┐
│   Phone     │                      │   Device    │
└──────┬──────┘                      └──────┬──────┘
       │                                    │
       │  1. BLE Connect                    │
       │ ──────────────────────────────────>│
       │                                    │
       │  2. MTU Exchange Request (513)     │
       │ ──────────────────────────────────>│
       │                                    │
       │  3. MTU Exchange Response (517)    │
       │ <──────────────────────────────────│
       │                                    │
       │  4. Effective MTU = min(513, 517)  │
       │     = 513 bytes                    │
       │                                    │
       │  5. Start OTA with 504-byte payload│
       │ ──────────────────────────────────>│
       │                                    │
```

#### MTU Configuration

| MTU Value | Payload Size | Description |
|-----------|-------------|-------------|
| **Default (23)** | N/A | Not supported for OTA |
| **247** | 244 bytes | Standard BLE, low performance |
| **513 (Phone)** | 504 bytes | Typical modern phone MTU |
| **517 (Device)** | 508 bytes | Device maximum MTU |
| **Effective** | 504 bytes | Auto-detected from first packet |

### Auto-Detection Mechanism

The device implements **first-packet payload detection** to handle MTU variations:

1. **START Command**: Phone sends firmware size assuming 508-byte payload
2. **First Packet**: Phone sends actual payload (e.g., 504 bytes due to MTU 513)
3. **Auto-Adjust**: Device detects actual size and recalculates expected packet count
4. **Subsequent Packets**: All packets use the detected payload size

**Log Example:**
```
[OTA] START: 246988 bytes, 487 packets (508 bytes/pkt, MTU=517)
[OTA] Auto-adjusting payload: 508 → 504 bytes (from first packet)
[OTA] Recalculated: 491 packets expected (504 bytes/pkt)
```

### MTU Negotiation Implementation

#### Android (Kotlin)

```kotlin
// Request MTU after connection
private val gattCallback = object : BluetoothGattCallback() {
    override fun onConnectionStateChange(gatt: BluetoothGatt, status: Int, newState: Int) {
        if (newState == BluetoothProfile.STATE_CONNECTED) {
            // Request MTU 513 for optimal performance
            gatt.requestMtu(513)
        }
    }
    
    override fun onMtuChanged(gatt: BluetoothGatt, mtu: Int, status: Int) {
        if (status == BluetoothGatt.GATT_SUCCESS) {
            Log.d(TAG, "MTU changed to: $mtu")
            // Effective payload = MTU - 3 (ATT header) - 2 (sequence number)
            val payloadSize = mtu - 5
            Log.d(TAG, "Using payload size: $payloadSize bytes")
            
            // Discover services after MTU negotiation
            gatt.discoverServices()
        }
    }
}
```

#### iOS (Swift)

```swift
// iOS automatically negotiates MTU
func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
    // Check negotiated MTU
    if let mtu = peripheral.maximumWriteValueLength(for: .withoutResponse) {
        print("Negotiated MTU: \(mtu)")
        let payloadSize = mtu - 2  // Subtract sequence number
        print("Using payload size: \(payloadSize) bytes")
    }
}
```

### Performance Comparison

| Configuration | MTU | Payload | Packets (250KB) | Speed | Time |
|--------------|-----|---------|-----------------|-------|------|
| **Legacy** | 247 | 244 B | 1,049 | 2.0 KB/s | 125s |
| **Optimized** | 513 | 504 B | 508 | 12.3 KB/s | 20s |
| **Improvement** | +106% | +107% | -52% | **+515%** | **-84%** |

> 🚀 **Key Takeaway**: MTU 513+ provides **6x faster** OTA updates!

### Troubleshooting MTU Issues

**Problem**: Transfer speed still slow despite MTU negotiation

**Solutions**:
1. **Verify MTU Request**: Ensure `requestMtu(513)` is called before service discovery
2. **Check Phone Support**: Some older phones max out at MTU 247
3. **Monitor Logs**: Device logs show actual payload size detected
4. **Test with Modern Phone**: Use Android 6.0+ or iOS 11+ for best results

**Problem**: "Packet count mismatch" error

**Solutions**:
1. Device auto-detects payload from first packet - no app changes needed
2. Send packets with consistent size throughout transfer
3. Check that WRITE_NO_RESPONSE is used (not WRITE_WITH_RESPONSE)

---

## 🔄 Protocol Details

### Transfer Protocol Design

The OTA protocol uses a **reliable transfer mechanism** optimized for BLE:

#### Key Design Decisions

1. **WRITE_NO_RESPONSE vs WRITE_WITH_RESPONSE**
   - Original: WRITE_WITH_RESPONSE → 1.3 KB/s (180ms RTT bottleneck)
   - Optimized: WRITE_NO_RESPONSE → 20+ KB/s (no waiting)

2. **Sequence Number Tracking**
   - 16-bit sequence numbers (0-65535)
   - Continuous incrementing
   - Packet loss detection via bitmap (future)

3. **Asynchronous Flash Write**
   - 50-packet queue (12KB buffer)
   - Background FreeRTOS task
   - Prevents BLE link timeout

4. **CRC32 Verification**
   - Flash-based calculation (not real-time)
   - VERIFY command ensures all writes complete
   - Result reported via OTA Status notification

### Packet Loss Handling

**Current Implementation:**
- Best-effort delivery with 99%+ success rate
- Device tracks received packets internally
- Future: Bitmap-based retransmission

**Recommended App Strategy:**
1. Monitor OTA Status notifications
2. Check `received_bytes` matches expected
3. If mismatch after END → retry entire transfer

### Timing Requirements

| Operation | Timing | Notes |
|-----------|--------|-------|
| Packet interval | 4-10 ms | Optimal for throughput |
| Status check | Every 10 packets | Monitor progress |
| VERIFY command | After END + 500ms | Wait for queue drain |
| CRC32 calculation | ~2-3 seconds | Device-side operation |
| Reboot delay | After COMPLETE status | User-initiated |

---

## 📱 Implementation Flow

### Complete OTA Update Sequence

```
1. CONNECT
   └─▶ Establish BLE connection
       └─▶ Discover OTA service (0xFF60)
           └─▶ Enable Status notifications (CCCD)

2. PREPARE
   └─▶ Read firmware .bin file
       └─▶ Calculate CRC32 checksum
           └─▶ Verify file size ≤ 800KB

3. START
   └─▶ Send START command (size + CRC32)
       └─▶ Wait for Status = READY
           └─▶ If not READY within 5s → abort

4. TRANSFER
   └─▶ Split firmware into 244-byte packets
       └─▶ For each packet (seq 0 to N-1):
           ├─▶ Prepend 2-byte sequence number
           ├─▶ Write to OTA Data (0xFF62)
           ├─▶ Wait 4-10ms between packets
           └─▶ Monitor Status notifications
       └─▶ Track progress via received_bytes

5. END
   └─▶ Send END command
       └─▶ Status changes to VERIFY
           └─▶ Wait 500ms for flash queue drain

6. VERIFY
   └─▶ Send VERIFY command
       └─▶ Wait for Status notification
           ├─▶ COMPLETE: CRC32 match ✓
           └─▶ ERROR: CRC32 mismatch ✗

7. REBOOT (if COMPLETE)
   └─▶ Send REBOOT command
       └─▶ Device reboots into new firmware
           └─▶ Reconnect and verify version
```

### State Machine Diagram

```
        ┌─────┐
        │IDLE │
        └──┬──┘
           │ START command
           ▼
        ┌──────┐
        │READY │
        └──┬───┘
           │ DATA packets
           ▼
      ┌──────────┐
      │RECEIVING │◀─── More DATA
      └─────┬────┘
            │ END command
            ▼
       ┌────────┐
       │ VERIFY │
       └────┬───┘
            │ VERIFY command + CRC32 check
            ├──────┬─────────┐
            ▼      ▼         ▼
       ┌────────┐  ┌─────┐  │
       │COMPLETE│  │ERROR│  │
       └────────┘  └─────┘  │
            │                │
            │ REBOOT         │ ABORT
            ▼                ▼
       [Device            ┌─────┐
        Reboot]           │IDLE │
                          └─────┘
```

---

## 📱 Android Implementation

### Step 1: Setup BLE Manager

```kotlin
class OtaManager(
    private val context: Context,
    private val device: BluetoothDevice
) {
    private var bluetoothGatt: BluetoothGatt? = null
    private var otaControlChar: BluetoothGattCharacteristic? = null
    private var otaDataChar: BluetoothGattCharacteristic? = null
    private var otaStatusChar: BluetoothGattCharacteristic? = null
    
    private val OTA_SERVICE_UUID = UUID.fromString("0000FF60-0000-1000-8000-00805F9B34FB")
    private val OTA_CONTROL_UUID = UUID.fromString("0000FF61-0000-1000-8000-00805F9B34FB")
    private val OTA_DATA_UUID = UUID.fromString("0000FF62-0000-1000-8000-00805F9B34FB")
    private val OTA_STATUS_UUID = UUID.fromString("0000FF63-0000-1000-8000-00805F9B34FB")
    private val CCCD_UUID = UUID.fromString("00002902-0000-1000-8000-00805F9B34FB")
    
    companion object {
        const val CMD_START = 0x01.toByte()
        const val CMD_END = 0x02.toByte()
        const val CMD_VERIFY = 0x03.toByte()
        const val CMD_REBOOT = 0x04.toByte()
        const val CMD_ABORT = 0x05.toByte()
    }
    
    interface OtaCallback {
        fun onConnected()
        fun onReady()
        fun onProgress(received: Long, total: Long, percentage: Int)
        fun onComplete(crc32: Long)
        fun onError(error: String)
    }
}
```

### Step 2: Connect and Discover Services

```kotlin
fun connect(callback: OtaCallback) {
    bluetoothGatt = device.connectGatt(context, false, object : BluetoothGattCallback() {
        override fun onConnectionStateChange(gatt: BluetoothGatt, status: Int, newState: Int) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                Log.d(TAG, "Connected to device")
                gatt.discoverServices()
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                callback.onError("Device disconnected")
            }
        }
        
        override fun onServicesDiscovered(gatt: BluetoothGatt, status: Int) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                val otaService = gatt.getService(OTA_SERVICE_UUID)
                if (otaService == null) {
                    callback.onError("OTA service not found")
                    return
                }
                
                otaControlChar = otaService.getCharacteristic(OTA_CONTROL_UUID)
                otaDataChar = otaService.getCharacteristic(OTA_DATA_UUID)
                otaStatusChar = otaService.getCharacteristic(OTA_STATUS_UUID)
                
                // Enable notifications on OTA Status
                enableNotifications(gatt, otaStatusChar!!)
                
                callback.onConnected()
            }
        }
        
        override fun onCharacteristicChanged(
            gatt: BluetoothGatt,
            characteristic: BluetoothGattCharacteristic
        ) {
            if (characteristic.uuid == OTA_STATUS_UUID) {
                handleStatusNotification(characteristic.value, callback)
            }
        }
    })
}

private fun enableNotifications(
    gatt: BluetoothGatt,
    characteristic: BluetoothGattCharacteristic
) {
    gatt.setCharacteristicNotification(characteristic, true)
    val descriptor = characteristic.getDescriptor(CCCD_UUID)
    descriptor.value = BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE
    gatt.writeDescriptor(descriptor)
}
```

### Step 3: Send START Command

```kotlin
fun startOta(firmwareFile: File, callback: OtaCallback) {
    // Read firmware file
    val firmwareData = firmwareFile.readBytes()
    val firmwareSize = firmwareData.size.toLong()
    
    // Calculate CRC32
    val crc = CRC32()
    crc.update(firmwareData)
    val crc32 = crc.value
    
    Log.d(TAG, "Firmware size: $firmwareSize bytes")
    Log.d(TAG, "CRC32: 0x${crc32.toString(16).toUpperCase()}")
    
    // Build START command
    val startCmd = ByteBuffer.allocate(9)
        .order(ByteOrder.LITTLE_ENDIAN)
        .put(CMD_START)
        .putInt(firmwareSize.toInt())
        .putInt(crc32.toInt())
        .array()
    
    // Send START command
    otaControlChar?.value = startCmd
    otaControlChar?.writeType = BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT
    bluetoothGatt?.writeCharacteristic(otaControlChar)
    
    // Store firmware for later transfer
    this.firmwareData = firmwareData
    this.firmwareCrc32 = crc32
}
```

### Step 4: Transfer Firmware Data

```kotlin
private var firmwareData: ByteArray? = null
private var currentSequence = 0
private val CHUNK_SIZE = 244
private val PACKET_DELAY_MS = 5L

fun transferData(callback: OtaCallback) {
    Thread {
        val firmware = firmwareData ?: return@Thread
        val totalPackets = (firmware.size + CHUNK_SIZE - 1) / CHUNK_SIZE
        
        Log.d(TAG, "Starting transfer: $totalPackets packets")
        
        for (seq in 0 until totalPackets) {
            val offset = seq * CHUNK_SIZE
            val remaining = firmware.size - offset
            val chunkSize = minOf(CHUNK_SIZE, remaining)
            
            // Build packet: 2-byte sequence + data
            val packet = ByteBuffer.allocate(2 + chunkSize)
                .order(ByteOrder.LITTLE_ENDIAN)
                .putShort(seq.toShort())
                .put(firmware, offset, chunkSize)
                .array()
            
            // Send packet
            otaDataChar?.value = packet
            otaDataChar?.writeType = BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE
            bluetoothGatt?.writeCharacteristic(otaDataChar)
            
            // Small delay for flow control
            Thread.sleep(PACKET_DELAY_MS)
            
            // Report progress every 10 packets
            if (seq % 10 == 0) {
                val received = (seq + 1) * CHUNK_SIZE
                val percentage = (received * 100 / firmware.size).coerceAtMost(100)
                callback.onProgress(received.toLong(), firmware.size.toLong(), percentage)
            }
        }
        
        Log.d(TAG, "Transfer complete, sending END command")
        sendEndCommand()
    }.start()
}

private fun sendEndCommand() {
    val endCmd = byteArrayOf(CMD_END)
    otaControlChar?.value = endCmd
    otaControlChar?.writeType = BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT
    bluetoothGatt?.writeCharacteristic(otaControlChar)
    
    // Wait for flash queue to drain
    Handler(Looper.getMainLooper()).postDelayed({
        sendVerifyCommand()
    }, 500)
}

private fun sendVerifyCommand() {
    Log.d(TAG, "Sending VERIFY command")
    val verifyCmd = byteArrayOf(CMD_VERIFY)
    otaControlChar?.value = verifyCmd
    otaControlChar?.writeType = BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT
    bluetoothGatt?.writeCharacteristic(otaControlChar)
}
```

### Step 5: Handle Status Notifications

```kotlin
private fun handleStatusNotification(data: ByteArray, callback: OtaCallback) {
    if (data.size < 13) return
    
    val buffer = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN)
    val state = buffer.get().toInt()
    val totalSize = buffer.int.toLong() and 0xFFFFFFFFL
    val receivedBytes = buffer.int.toLong() and 0xFFFFFFFFL
    val calculatedCrc32 = buffer.int.toLong() and 0xFFFFFFFFL
    
    Log.d(TAG, "OTA Status: state=$state, total=$totalSize, received=$receivedBytes, crc32=0x${calculatedCrc32.toString(16)}")
    
    when (state) {
        1 -> callback.onReady() // READY
        2 -> { // RECEIVING
            val percentage = ((receivedBytes * 100) / totalSize).toInt()
            callback.onProgress(receivedBytes, totalSize, percentage)
        }
        3 -> Log.d(TAG, "Verifying CRC32...") // VERIFY
        4 -> { // COMPLETE
            Log.d(TAG, "OTA Complete! CRC32 matched: 0x${calculatedCrc32.toString(16)}")
            callback.onComplete(calculatedCrc32)
        }
        5 -> callback.onError("OTA verification failed") // ERROR
    }
}

fun rebootDevice() {
    Log.d(TAG, "Sending REBOOT command")
    val rebootCmd = byteArrayOf(CMD_REBOOT)
    otaControlChar?.value = rebootCmd
    otaControlChar?.writeType = BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT
    bluetoothGatt?.writeCharacteristic(otaControlChar)
}
```

### Step 6: Complete Usage Example

```kotlin
// In your Activity or Fragment
class OtaActivity : AppCompatActivity() {
    private lateinit var otaManager: OtaManager
    
    fun startFirmwareUpdate(firmwareFile: File) {
        otaManager = OtaManager(this, bluetoothDevice)
        
        otaManager.connect(object : OtaManager.OtaCallback {
            override fun onConnected() {
                Log.d(TAG, "BLE Connected, starting OTA")
                otaManager.startOta(firmwareFile, this)
            }
            
            override fun onReady() {
                Log.d(TAG, "Device ready, starting data transfer")
                otaManager.transferData(this)
            }
            
            override fun onProgress(received: Long, total: Long, percentage: Int) {
                runOnUiThread {
                    progressBar.progress = percentage
                    statusText.text = "$percentage% ($received / $total bytes)"
                }
            }
            
            override fun onComplete(crc32: Long) {
                runOnUiThread {
                    AlertDialog.Builder(this@OtaActivity)
                        .setTitle("Update Complete")
                        .setMessage("Firmware verified! Reboot device?")
                        .setPositiveButton("Reboot") { _, _ ->
                            otaManager.rebootDevice()
                        }
                        .show()
                }
            }
            
            override fun onError(error: String) {
                runOnUiThread {
                    Toast.makeText(this@OtaActivity, "Error: $error", Toast.LENGTH_LONG).show()
                }
            }
        })
    }
}
```

---

## 🍎 iOS Implementation

### Step 1: Setup OTA Manager

```swift
import CoreBluetooth

class OTAManager: NSObject {
    private var centralManager: CBCentralManager!
    private var peripheral: CBPeripheral?
    private var otaControlChar: CBCharacteristic?
    private var otaDataChar: CBCharacteristic?
    private var otaStatusChar: CBCharacteristic?
    
    private let OTA_SERVICE_UUID = CBUUID(string: "FF60")
    private let OTA_CONTROL_UUID = CBUUID(string: "FF61")
    private let OTA_DATA_UUID = CBUUID(string: "FF62")
    private let OTA_STATUS_UUID = CBUUID(string: "FF63")
    
    private enum Command: UInt8 {
        case start = 0x01
        case end = 0x02
        case verify = 0x03
        case reboot = 0x04
        case abort = 0x05
    }
    
    protocol OTADelegate: AnyObject {
        func otaDidConnect()
        func otaDidReady()
        func otaDidProgress(received: UInt32, total: UInt32, percentage: Int)
        func otaDidComplete(crc32: UInt32)
        func otaDidFail(error: String)
    }
    
    weak var delegate: OTADelegate?
    private var firmwareData: Data?
    private var expectedCRC32: UInt32 = 0
}
```

### Step 2: Connect and Discover

```swift
extension OTAManager: CBCentralManagerDelegate, CBPeripheralDelegate {
    
    func connect(to peripheral: CBPeripheral) {
        self.peripheral = peripheral
        self.peripheral?.delegate = self
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == .poweredOn {
            central.connect(peripheral!, options: nil)
        }
    }
    
    func centralManager(_ central: CBCentralManager, 
                       didConnect peripheral: CBPeripheral) {
        print("Connected to device")
        peripheral.discoverServices([OTA_SERVICE_UUID])
    }
    
    func peripheral(_ peripheral: CBPeripheral, 
                   didDiscoverServices error: Error?) {
        guard let services = peripheral.services else { return }
        
        for service in services {
            if service.uuid == OTA_SERVICE_UUID {
                peripheral.discoverCharacteristics([
                    OTA_CONTROL_UUID,
                    OTA_DATA_UUID,
                    OTA_STATUS_UUID
                ], for: service)
            }
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral,
                   didDiscoverCharacteristicsFor service: CBService,
                   error: Error?) {
        guard let characteristics = service.characteristics else { return }
        
        for characteristic in characteristics {
            switch characteristic.uuid {
            case OTA_CONTROL_UUID:
                otaControlChar = characteristic
            case OTA_DATA_UUID:
                otaDataChar = characteristic
            case OTA_STATUS_UUID:
                otaStatusChar = characteristic
                peripheral.setNotifyValue(true, for: characteristic)
            default:
                break
            }
        }
        
        delegate?.otaDidConnect()
    }
}
```

### Step 3: Calculate CRC32

```swift
extension Data {
    func crc32() -> UInt32 {
        var crc: UInt32 = 0xFFFFFFFF
        
        for byte in self {
            var temp = (crc ^ UInt32(byte)) & 0xFF
            for _ in 0..<8 {
                if (temp & 1) == 1 {
                    temp = (temp >> 1) ^ 0xEDB88320
                } else {
                    temp >>= 1
                }
            }
            crc = (crc >> 8) ^ temp
        }
        
        return ~crc
    }
}
```

### Step 4: Start OTA

```swift
func startOTA(firmwareURL: URL) {
    guard let firmware = try? Data(contentsOf: firmwareURL) else {
        delegate?.otaDidFail(error: "Failed to read firmware file")
        return
    }
    
    self.firmwareData = firmware
    self.expectedCRC32 = firmware.crc32()
    
    let size = UInt32(firmware.count)
    
    print("Firmware size: \(size) bytes")
    print("CRC32: 0x\(String(format: "%08X", expectedCRC32))")
    
    // Build START command
    var startCmd = Data()
    startCmd.append(Command.start.rawValue)
    startCmd.append(contentsOf: withUnsafeBytes(of: size.littleEndian) { Data($0) })
    startCmd.append(contentsOf: withUnsafeBytes(of: expectedCRC32.littleEndian) { Data($0) })
    
    peripheral?.writeValue(startCmd, for: otaControlChar!, type: .withResponse)
}
```

### Step 5: Transfer Data

```swift
private let CHUNK_SIZE = 244
private let PACKET_DELAY: TimeInterval = 0.005 // 5ms

func transferFirmware() {
    guard let firmware = firmwareData else { return }
    
    let totalPackets = (firmware.count + CHUNK_SIZE - 1) / CHUNK_SIZE
    print("Starting transfer: \(totalPackets) packets")
    
    DispatchQueue.global(qos: .userInitiated).async { [weak self] in
        guard let self = self else { return }
        
        for seq in 0..<totalPackets {
            let offset = seq * CHUNK_SIZE
            let remaining = firmware.count - offset
            let chunkSize = min(CHUNK_SIZE, remaining)
            
            // Build packet
            var packet = Data()
            packet.append(contentsOf: withUnsafeBytes(of: UInt16(seq).littleEndian) { Data($0) })
            packet.append(firmware.subdata(in: offset..<(offset + chunkSize)))
            
            // Send packet
            self.peripheral?.writeValue(packet, for: self.otaDataChar!, type: .withoutResponse)
            
            // Flow control delay
            Thread.sleep(forTimeInterval: PACKET_DELAY)
            
            // Report progress
            if seq % 10 == 0 {
                let received = UInt32(min((seq + 1) * CHUNK_SIZE, firmware.count))
                let percentage = Int((received * 100) / UInt32(firmware.count))
                
                DispatchQueue.main.async {
                    self.delegate?.otaDidProgress(
                        received: received,
                        total: UInt32(firmware.count),
                        percentage: percentage
                    )
                }
            }
        }
        
        print("Transfer complete, sending END")
        self.sendEndCommand()
    }
}

private func sendEndCommand() {
    let endCmd = Data([Command.end.rawValue])
    peripheral?.writeValue(endCmd, for: otaControlChar!, type: .withResponse)
    
    // Wait for flash queue
    DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) { [weak self] in
        self?.sendVerifyCommand()
    }
}

private func sendVerifyCommand() {
    print("Sending VERIFY command")
    let verifyCmd = Data([Command.verify.rawValue])
    peripheral?.writeValue(verifyCmd, for: otaControlChar!, type: .withResponse)
}
```

### Step 6: Handle Status Updates

```swift
func peripheral(_ peripheral: CBPeripheral,
               didUpdateValueFor characteristic: CBCharacteristic,
               error: Error?) {
    guard characteristic.uuid == OTA_STATUS_UUID,
          let data = characteristic.value,
          data.count >= 13 else { return }
    
    let state = data[0]
    let totalSize = data.subdata(in: 1..<5).withUnsafeBytes { $0.load(as: UInt32.self).littleEndian }
    let receivedBytes = data.subdata(in: 5..<9).withUnsafeBytes { $0.load(as: UInt32.self).littleEndian }
    let calculatedCRC32 = data.subdata(in: 9..<13).withUnsafeBytes { $0.load(as: UInt32.self).littleEndian }
    
    print("OTA Status: state=\(state), received=\(receivedBytes)/\(totalSize), crc32=0x\(String(format: "%08X", calculatedCRC32))")
    
    switch state {
    case 1: // READY
        delegate?.otaDidReady()
        transferFirmware()
        
    case 2: // RECEIVING
        let percentage = Int((receivedBytes * 100) / totalSize)
        delegate?.otaDidProgress(received: receivedBytes, total: totalSize, percentage: percentage)
        
    case 3: // VERIFY
        print("Verifying CRC32...")
        
    case 4: // COMPLETE
        print("OTA Complete! CRC32: 0x\(String(format: "%08X", calculatedCRC32))")
        delegate?.otaDidComplete(crc32: calculatedCRC32)
        
    case 5: // ERROR
        delegate?.otaDidFail(error: "CRC32 verification failed")
        
    default:
        break
    }
}

func rebootDevice() {
    print("Sending REBOOT command")
    let rebootCmd = Data([Command.reboot.rawValue])
    peripheral?.writeValue(rebootCmd, for: otaControlChar!, type: .withResponse)
}
```

### Step 7: Complete Usage Example

```swift
class OTAViewController: UIViewController, OTADelegate {
    private var otaManager: OTAManager!
    
    @IBOutlet weak var progressBar: UIProgressView!
    @IBOutlet weak var statusLabel: UILabel!
    
    func startFirmwareUpdate(firmwareURL: URL, peripheral: CBPeripheral) {
        otaManager = OTAManager()
        otaManager.delegate = self
        
        otaManager.connect(to: peripheral)
    }
    
    // MARK: - OTADelegate
    
    func otaDidConnect() {
        print("Connected, starting OTA")
        guard let firmwareURL = getFirmwareURL() else { return }
        otaManager.startOTA(firmwareURL: firmwareURL)
    }
    
    func otaDidReady() {
        DispatchQueue.main.async {
            self.statusLabel.text = "Transferring firmware..."
        }
    }
    
    func otaDidProgress(received: UInt32, total: UInt32, percentage: Int) {
        DispatchQueue.main.async {
            self.progressBar.progress = Float(percentage) / 100.0
            self.statusLabel.text = "\(percentage)% (\(received) / \(total) bytes)"
        }
    }
    
    func otaDidComplete(crc32: UInt32) {
        DispatchQueue.main.async {
            let alert = UIAlertController(
                title: "Update Complete",
                message: "Firmware verified! Reboot device?",
                preferredStyle: .alert
            )
            
            alert.addAction(UIAlertAction(title: "Reboot", style: .default) { _ in
                self.otaManager.rebootDevice()
            })
            
            alert.addAction(UIAlertAction(title: "Cancel", style: .cancel))
            
            self.present(alert, animated: true)
        }
    }
    
    func otaDidFail(error: String) {
        DispatchQueue.main.async {
            self.statusLabel.text = "Error: \(error)"
        }
    }
}
```

---

## ⚠️ Error Handling

### Common Error Scenarios

| Error | Cause | Solution |
|-------|-------|----------|
| **Service not found** | OTA service (0xFF60) missing | Ensure correct firmware version |
| **Connection timeout** | BLE connection lost | Retry connection, check signal strength |
| **CRC32 mismatch** | Data corruption during transfer | Retry entire OTA process |
| **Flash write timeout** | Device busy | Wait for VERIFY completion before REBOOT |
| **Invalid state** | Command sent in wrong state | Follow state machine strictly |
| **Packet loss** | Poor BLE link quality | Reduce packet rate, improve proximity |

### Error Recovery Strategy

```kotlin
// Android Example
class OtaErrorHandler {
    private var retryCount = 0
    private val MAX_RETRIES = 3
    
    fun handleError(error: OtaError, otaManager: OtaManager) {
        when (error) {
            is CRC32Mismatch -> {
                if (retryCount < MAX_RETRIES) {
                    retryCount++
                    Log.w(TAG, "CRC32 mismatch, retry $retryCount/$MAX_RETRIES")
                    otaManager.abortAndRetry()
                } else {
                    Log.e(TAG, "OTA failed after $MAX_RETRIES attempts")
                    showErrorDialog("Update failed. Please try again later.")
                }
            }
            
            is ConnectionLost -> {
                Log.e(TAG, "BLE connection lost")
                otaManager.reconnectAndResume()
            }
            
            is InvalidState -> {
                Log.e(TAG, "Invalid OTA state")
                otaManager.abortAndReset()
            }
        }
    }
}
```

### Abort and Cleanup

```kotlin
fun abortOta() {
    val abortCmd = byteArrayOf(CMD_ABORT)
    otaControlChar?.value = abortCmd
    otaControlChar?.writeType = BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT
    bluetoothGatt?.writeCharacteristic(otaControlChar)
    
    // Reset state
    firmwareData = null
    currentSequence = 0
}
```

---

## 🧪 Testing Guide

### Test Cases

#### 1. Basic OTA Success Path
```
Input: Valid 100KB firmware
Expected: 
  - Transfer completes in ~5-6 seconds
  - CRC32 matches
  - Device reboots successfully
  - New version reflected after reboot
```

#### 2. Large Firmware (800KB)
```
Input: Maximum size firmware
Expected:
  - Transfer completes in ~40 seconds
  - No memory overflow
  - CRC32 verification successful
```

#### 3. Intentional CRC32 Mismatch
```
Input: Modify 1 byte after calculating CRC32
Expected:
  - Transfer completes
  - VERIFY fails
  - Device reports ERROR state
  - Device remains on old firmware
```

#### 4. Connection Interruption
```
Input: Disconnect BLE during transfer
Expected:
  - Device timeout after 30 seconds
  - State resets to IDLE
  - Partial firmware discarded
```

#### 5. Multiple Consecutive Updates
```
Input: Perform 3 OTA updates back-to-back
Expected:
  - Each update successful
  - No memory leaks
  - Partition switching works correctly
```

### Test Tools

**Android Test App:**
```kotlin
// Use nRF Connect for Mobile
// Or build custom test app with above code

class OtaTestSuite {
    fun testBasicOta() { /* ... */ }
    fun testLargeFirmware() { /* ... */ }
    fun testCrcMismatch() { /* ... */ }
    fun testConnectionLoss() { /* ... */ }
}
```

**iOS Test App:**
```swift
// Use LightBlue Explorer
// Or build custom test app with above code

class OTATests: XCTestCase {
    func testBasicOTA() { /* ... */ }
    func testLargeFirmware() { /* ... */ }
    func testCRCMismatch() { /* ... */ }
}
```

### Performance Benchmarks

**Real-World Results (MTU 513+):**

| Firmware Size | Transfer Time | Throughput | Packets |
|---------------|---------------|------------|---------|
| 100 KB | ~8 seconds | ~12.5 KB/s | 203 |
| 247 KB | ~20 seconds | ~12.3 KB/s | 491 |
| 500 KB | ~40 seconds | ~12.5 KB/s | 992 |
| 800 KB | ~65 seconds | ~12.3 KB/s | 1,587 |

**Legacy Performance (MTU 247):**

| Firmware Size | Transfer Time | Throughput | Packets |
|---------------|---------------|------------|---------|
| 100 KB | ~50 seconds | ~2.0 KB/s | 419 |
| 247 KB | ~125 seconds | ~2.0 KB/s | 1,049 |
| 500 KB | ~250 seconds | ~2.0 KB/s | 2,098 |

**Performance Improvement:**
- Speed: **12.3 KB/s** (MTU 513+) vs 2.0 KB/s (MTU 247) = **6x faster**
- Time: **20 seconds** vs 125 seconds for 247KB = **84% reduction**
- Packets: **491** vs 1,049 for 247KB = **52% fewer packets**

**Measurement:**
```kotlin
val startTime = System.currentTimeMillis()
// ... OTA transfer ...
val endTime = System.currentTimeMillis()
val durationSeconds = (endTime - startTime) / 1000.0
val throughputKBps = firmwareSize / 1024.0 / durationSeconds
Log.d(TAG, "OTA completed in $durationSeconds seconds, throughput: $throughputKBps KB/s")
```

**Device Logs Example:**
```
[OTA] START: 246988 bytes, 491 packets (504 bytes/pkt, MTU=517)
[OTA] Auto-adjusting payload: 508 → 504 bytes (from first packet)
[OTA] Transfer complete: 246988 bytes in 20032 ms (12329 bytes/s)
[OTA] CRC32 OK! Switching partition...
[OTA] Ready to reboot
```

---

## ⚡ Performance Optimization

### Mobile App Optimizations

#### 1. MTU Negotiation (Critical)
```kotlin
// Request MTU 513 for 6x performance boost
private val gattCallback = object : BluetoothGattCallback() {
    override fun onConnectionStateChange(gatt: BluetoothGatt, status: Int, newState: Int) {
        if (newState == BluetoothProfile.STATE_CONNECTED) {
            // MUST request MTU before service discovery
            gatt.requestMtu(513)  // Modern phones support this
        }
    }
    
    override fun onMtuChanged(gatt: BluetoothGatt, mtu: Int, status: Int) {
        if (status == BluetoothGatt.GATT_SUCCESS) {
            Log.d(TAG, "MTU changed to: $mtu")
            // Payload = MTU - 3 (ATT header) - 2 (seq number)
            val payloadSize = mtu - 5
            Log.d(TAG, "Effective payload: $payloadSize bytes")
            gatt.discoverServices()
        }
    }
}
```

#### 2. Packet Rate Tuning
```kotlin
// Adjust based on MTU and connection quality
private val PACKET_DELAY_MS = when {
    mtu >= 513 && signalStrength > -60 -> 4L  // Fast mode
    mtu >= 513 && signalStrength > -70 -> 5L  // Normal mode
    mtu >= 247 && signalStrength > -70 -> 7L  // Slow mode
    else -> 10L                                 // Conservative mode
}

// Use coroutines for non-blocking delays
scope.launch {
    for (packet in packets) {
        writeCharacteristic(otaDataChar, packet)
        delay(PACKET_DELAY_MS)
    }
}
```

#### 3. Background Transfer
```kotlin
// Use foreground service to prevent interruption
class OtaService : Service() {
    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        val notification = createOtaNotification()
        startForeground(NOTIFICATION_ID, notification)
        
        // Start OTA in background
        otaManager.startTransfer()
        
        return START_STICKY
    }
}
```

#### 4. Large Packet Writing
```kotlin
// Use writeCharacteristic with proper size
private fun sendDataPacket(seq: Int, payload: ByteArray) {
    val packet = ByteBuffer.allocate(2 + payload.size)
        .order(ByteOrder.LITTLE_ENDIAN)
        .putShort(seq.toShort())
        .put(payload)
        .array()
    
    otaDataChar.value = packet
    otaDataChar.writeType = BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE
    gatt.writeCharacteristic(otaDataChar)
}
```

### Device-Side Optimizations (Already Implemented)

- ✅ **MTU 517 Negotiation**: Supports large packets (up to 512 bytes payload)
- ✅ **Auto-Detection**: Adapts to actual MTU from first packet
- ✅ **Asynchronous Flash**: 4KB buffer with 8-packet queue prevents BLE blocking
- ✅ **WRITE_NO_RESPONSE**: Maximum throughput, no ACK overhead
- ✅ **Dynamic Payload**: Auto-adjusts to 504/508 bytes based on phone MTU
- ✅ **Connection Interval**: 7.5-15ms for optimal throughput
- ✅ **Memory Efficient**: 4KB flash buffer freed after completion

### Performance Tuning Checklist

**Before Transfer:**
- [ ] Request MTU 513+ immediately after connection
- [ ] Wait for `onMtuChanged` before starting OTA
- [ ] Use foreground service for background stability
- [ ] Verify WRITE_NO_RESPONSE is enabled

**During Transfer:**
- [ ] Monitor signal strength and adjust packet rate
- [ ] Track progress via OTA Status notifications
- [ ] Log actual throughput for debugging
- [ ] Handle BLE disconnections gracefully

**After Transfer:**
- [ ] Measure total time and calculate throughput
- [ ] Verify COMPLETE state before sending REBOOT
- [ ] Log results for performance tracking

### Troubleshooting Slow Transfers

| Symptom | Cause | Solution |
|---------|-------|----------|
| ~2 KB/s throughput | MTU not negotiated | Call `requestMtu(513)` before OTA |
| ~1.3 KB/s throughput | Using WRITE_WITH_RESPONSE | Switch to WRITE_NO_RESPONSE |
| Frequent disconnects | Packet rate too high | Increase PACKET_DELAY_MS to 7-10ms |
| Inconsistent speed | Signal interference | Move phone closer to device |
| Stuck at 50% | Flash queue overflow | Already fixed in v3.0 |

---

## ❓ FAQ

### Q1: What happens if OTA fails mid-transfer?

**A:** The device remains on the old firmware. The bootloader only switches partitions after successful CRC32 verification. Partial transfers are discarded.

### Q2: Can I interrupt and resume an OTA update?

**A:** No, the current protocol requires complete transfer. Interruption requires restarting from scratch.

### Q3: How do I generate the CRC32 checksum?

**A:** Use standard CRC32 algorithms:

```bash
# Linux/Mac
crc32 firmware.bin

# Python
import zlib
with open('firmware.bin', 'rb') as f:
    crc = zlib.crc32(f.read())
    print(f"CRC32: 0x{crc:08X}")
```

### Q4: What if CRC32 mismatches?

**A:** The device reports ERROR state. Firmware is NOT applied. App should retry the entire update.

### Q5: Can I update while device is in use?

**A:** Yes, but avoid during critical operations. Best practice: prompt user before starting.

### Q6: How to verify update success?

**A:** After reboot, reconnect and check firmware version via Device Information Service (0x180A).

### Q7: Why use WRITE_NO_RESPONSE instead of WRITE_WITH_RESPONSE?

**A:** WRITE_WITH_RESPONSE has 180ms RTT, limiting throughput to 1.3 KB/s. WRITE_NO_RESPONSE achieves 12.3 KB/s (9x faster).

### Q8: What's the maximum firmware size?

**A:** 800 KB (0xC8000 bytes). Each partition can hold up to this size.

### Q9: How does dual-partition work?

**A:** Bootloader maintains metadata tracking active partition. OTA writes to inactive partition. After successful verification, bootloader switches active partition on next reboot.

### Q10: What if both partitions are corrupted?

**A:** Device enters recovery mode. Requires physical connection for reflashing.

### Q11: Why is MTU negotiation important?

**A:** MTU 513+ enables 504-byte payloads (vs 244 bytes at MTU 247), providing **6x faster** OTA updates. Always call `requestMtu(513)` after connection.

### Q12: What if my phone doesn't support MTU 513?

**A:** The device auto-detects actual MTU from the first packet. It will work with any MTU, just slower. Most Android 5.0+ and iOS 10.0+ support MTU 512+.

### Q13: How does payload auto-detection work?

**A:** The device expects 508-byte payload by default. When it receives the first packet, it detects the actual size (e.g., 504 bytes) and recalculates the expected packet count. No app changes needed.

### Q14: Why am I getting "packet count mismatch" errors?

**A:** This error is now automatically handled by the auto-detection mechanism (v3.0+). If you still see it, ensure:
- All data packets are the same size (except possibly the last one)
- You're using WRITE_NO_RESPONSE
- MTU negotiation completed before starting OTA

### Q15: What's the optimal packet delay?

**A:** For MTU 513+, use 4-5ms between packets. For MTU 247, use 7-10ms. Adjust based on signal strength.

---

## 📞 Support

For questions or issues:

1. **Check device logs**: Enable verbose logging in firmware
2. **Use BLE sniffer**: Analyze packet-level communication
3. **Test with reference tools**: nRF Connect (Android), LightBlue (iOS)
4. **Contact SDK team**: Provide logs and firmware version

---

## 📚 Related Documents

- [BLE Remote Control README](README.md) - Main application documentation
- [IPRO SDK README](../../README.md) - SDK overview
- [BLE Library Guide](../../docs/ble/guides/IPRO_BLE_LIBRARY_GUIDE.md) - BLE development guide

---

**Document End**  
*For the latest version, check the repository.*

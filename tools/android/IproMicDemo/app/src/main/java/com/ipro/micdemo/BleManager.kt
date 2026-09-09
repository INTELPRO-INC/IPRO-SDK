package com.ipro.micdemo

import android.annotation.SuppressLint
import android.bluetooth.*
import android.bluetooth.le.*
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.os.ParcelUuid
import android.os.SystemClock
import android.util.Log
import androidx.core.content.ContextCompat
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.util.UUID

/**
 * BLE manager for IPRO-MIC NUS communication.
 *
 * Handles scanning, connection, NUS service discovery, command sending,
 * and notification parsing.
 */
@SuppressLint("MissingPermission")
class BleManager(private val context: Context) {

    companion object {
        private const val TAG = "BleManager"
        private const val DEVICE_NAME = "IPRO-MIC"

        // NUS UUIDs
        val NUS_SERVICE_UUID: UUID = UUID.fromString("6E400001-B5A3-F393-E0A9-E50E24DCCA9E")
        val NUS_RX_UUID: UUID = UUID.fromString("6E400002-B5A3-F393-E0A9-E50E24DCCA9E")
        val NUS_TX_UUID: UUID = UUID.fromString("6E400003-B5A3-F393-E0A9-E50E24DCCA9E")
        val CCCD_UUID: UUID = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb")

        // Commands (phone -> device)
        const val CMD_START_REC: Byte = 0x01
        const val CMD_STOP_REC: Byte = 0x02
        const val CMD_GET_AUDIO: Byte = 0x03
        const val CMD_GET_STATUS: Byte = 0x04
        const val CMD_SET_GAIN: Byte = 0x05
        const val CMD_START_STREAM: Byte = 0x06
        const val CMD_STOP_STREAM: Byte = 0x07
        const val CMD_EPD_TEXT: Byte = 0x08
        const val CMD_DAC_PLAY: Byte = 0x09
        const val CMD_DAC_STOP: Byte = 0x0A
        const val CMD_EPD_PAGE: Byte = 0x0B
        const val CMD_GET_CHUNK: Byte = 0x0C
        const val CMD_EPD_IMG: Byte = 0x0D

        const val STATUS_IDLE: Int = 0x00
        const val STATUS_RECORDING: Int = 0x01
        const val STATUS_SENDING: Int = 0x02
        const val STATUS_STREAMING: Int = 0x03
        const val STATUS_RECORDED: Int = 0x04
        const val STATUS_PLAYING: Int = 0x05
        const val STATUS_CHUNK_READY: Int = 0x06

        const val MAX_RECORD_SECONDS: Int = 14400
        const val EPD_TEXT_MAX: Int = 2000

        // Responses (device -> phone)
        const val RSP_STATUS: Int = 0x81
        const val RSP_AUDIO_HDR: Int = 0x82
        const val RSP_AUDIO_DATA: Int = 0x83
        const val RSP_AUDIO_DONE: Int = 0x84
        const val RSP_ERROR: Int = 0x85
        const val RSP_AUDIO_LC3: Int = 0x86
        const val RSP_EPD_PAGE: Int = 0x87

        fun stateName(state: Int) = when (state) {
            STATUS_IDLE -> "Idle"
            STATUS_RECORDING -> "Recording"
            STATUS_SENDING -> "Sending"
            STATUS_STREAMING -> "Streaming"
            STATUS_RECORDED -> "Recorded"
            STATUS_PLAYING -> "Playing"
            STATUS_CHUNK_READY -> "Chunk ready"
            else -> "Unknown($state)"
        }

        /*
         * Keep the connection bring-up conservative by default. The IPRO
         * controller/host baseline must survive service discovery before the
         * app adds MTU, high-priority interval, PHY or DLE pressure.
         */
        private const val REQUEST_MTU_ON_CONNECT = true
        private const val REQUEST_HIGH_PRIORITY_AFTER_MTU = false
    }

    enum class ConnectionState { DISCONNECTED, SCANNING, CONNECTING, CONNECTED }

    /** Mirrors BluetoothDevice.BOND_*, so the UI does not have to know the constants. */
    enum class BondState { NONE, BONDING, BONDED }

    data class DeviceStatus(
        val state: Int = 0,
        val recordedBytes: Int = 0,
        val sampleRate: Int = 16000,
        val gain: Int = 0,
        val epdIndex: Int = 0,
        val epdCount: Int = 0
    ) {
        val canStartRecording: Boolean
            get() = state == STATUS_IDLE || state == STATUS_RECORDED
        val canStopRecording: Boolean
            get() = state == STATUS_RECORDING || state == STATUS_CHUNK_READY
        val canDacPlay: Boolean
            get() = recordedBytes > 0 &&
                (state == STATUS_IDLE || state == STATUS_RECORDED)
    }

    // Public state
    private val _connectionState = MutableStateFlow(ConnectionState.DISCONNECTED)
    val connectionState: StateFlow<ConnectionState> = _connectionState

    private val _bondState = MutableStateFlow(BondState.NONE)
    val bondState: StateFlow<BondState> = _bondState

    /** Address of the device this phone is paired with, or null. */
    private val _pairedAddress = MutableStateFlow<String?>(null)
    val pairedAddress: StateFlow<String?> = _pairedAddress

    private val _statusText = MutableStateFlow("Disconnected")
    val statusText: StateFlow<String> = _statusText

    private val _deviceStatus = MutableStateFlow(DeviceStatus())
    val deviceStatus: StateFlow<DeviceStatus> = _deviceStatus

    private val _audioTransferProgress = MutableStateFlow(0f)
    val audioTransferProgress: StateFlow<Float> = _audioTransferProgress

    private val _audioReady = MutableStateFlow(false)
    val audioReady: StateFlow<Boolean> = _audioReady

    private val _isStreaming = MutableStateFlow(false)
    val isStreaming: StateFlow<Boolean> = _isStreaming

    // Callback for streaming PCM data to AudioPlayer
    var onStreamData: ((ByteArray) -> Unit)? = null

    // LC3 decoder for compressed streaming
    private val lc3Decoder = Lc3Decoder()
    private var lc3Initialized = false

    // Audio receive buffer
    private var audioBuffer = ByteArray(0)

    // --- 2M 批次傳輸序號校正(v1.1 起預設開啟,對應韌體預設 pdm_seq on) ---
    // 封包=[0x83][seq][data]:手機藍牙棧多執行緒派發使通知「到達順序」在
    // 2M 突發下不可信 — 按序號定位寫入使結果與到達順序無關。
    private val _seqDebug = MutableStateFlow(true)
    val seqDebug: StateFlow<Boolean> = _seqDebug
    fun setSeqDebug(on: Boolean) {
        _seqDebug.value = on
        Log.i(TAG, "SeqDebug ${if (on) "ON — 裝置端請先 pdm_seq on" else "off"}")
    }

    private var bulkPkts = 0
    private var bulkStartMs = 0L
    private var seqLast = -1
    private var seqGaps = 0
    private var seqLostPkts = 0
    private val seqGapDetail = StringBuilder()

    // 按序號定位寫入：手機藍牙棧可能用執行緒池派發回呼,到達順序不可信
    // (2M 突發下實測出現相鄰包對調) — 位置=展開序號×chunk,與順序無關。
    private var seqExtLast = -1   // 8-bit 序號展開後的絕對序號
    private var bulkChunk = 0     // 首包 payload 大小(=242 @MTU247)
    private var audioTotalBytes = 0
    private var audioReceivedBytes = 0
    private var audioSampleRate = 16000

    // 原始側錄:每個 bulk notification 原封不動存 [u32 t_ms][u16 len][bytes]
    // → 存成 IPRO_RAW_*.bin。離線比對「到達位元組」vs「重組結果」,
    //   一刀切開 裝置/空中 vs 手機端組裝 的責任歸屬。
    private var rawCap: java.io.ByteArrayOutputStream? = null

    // BLE internals
    private val bluetoothAdapter: BluetoothAdapter? by lazy {
        (context.getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager).adapter
    }
    private var scanner: BluetoothLeScanner? = null
    private var gatt: BluetoothGatt? = null
    private var rxCharacteristic: BluetoothGattCharacteristic? = null
    private var txCharacteristic: BluetoothGattCharacteristic? = null

    // --- Bonding -------------------------------------------------------------
    //
    // The device asks for encryption as soon as it connects (it calls
    // bt_conn_set_security(L2) in its connected callback), which is what makes
    // the reconnect after a power-button sleep free of a pairing prompt: the
    // key is already on both sides.
    //
    // The cost is that bonding now overlaps connection bring-up, and Android
    // does not serialise the two. Requesting an MTU or discovering services
    // while the stack is in the middle of pairing is the classic way to get
    // status 133 and a half-populated service database, and it looks like a
    // device fault rather than a sequencing one. So bring-up waits for the
    // bond, and starts it explicitly rather than waiting to be asked - an
    // explicit createBond() gives a deterministic order instead of racing the
    // peripheral's security request.

    private var pendingBringUp = false

    /** Set by disconnect()/forgetDevice() so a user-requested drop is not re-armed. */
    private var userDisconnect = false

    /**
     * Re-arm the background connection when the device drops the link by
     * itself. The user can switch this off; it is what makes a device that
     * slept on its power button come back with no tap on this end.
     */
    private val _autoReconnect = MutableStateFlow(true)
    val autoReconnect: StateFlow<Boolean> = _autoReconnect
    fun setAutoReconnect(on: Boolean) { _autoReconnect.value = on }

    /** On CHUNK_READY, pull the next 30 s slice (live STT path). */
    private val _autoChunk = MutableStateFlow(true)
    val autoChunk: StateFlow<Boolean> = _autoChunk
    fun setAutoChunk(on: Boolean) { _autoChunk.value = on }

    private val _chunkCount = MutableStateFlow(0)
    val chunkCount: StateFlow<Int> = _chunkCount
    private var audioBusy = false
    private var receivingChunk = false
    private var recordingRequested = false
    private val mainHandler = android.os.Handler(android.os.Looper.getMainLooper())
    private val prefs by lazy {
        context.getSharedPreferences("ipro_mic_ble", Context.MODE_PRIVATE)
    }

    private fun hasPermission(permission: String): Boolean =
        android.os.Build.VERSION.SDK_INT < android.os.Build.VERSION_CODES.S ||
        ContextCompat.checkSelfPermission(context, permission) ==
            android.content.pm.PackageManager.PERMISSION_GRANTED

    private fun bondStateOf(v: Int) = when (v) {
        BluetoothDevice.BOND_BONDED -> BondState.BONDED
        BluetoothDevice.BOND_BONDING -> BondState.BONDING
        else -> BondState.NONE
    }

    private val bondReceiver = object : BroadcastReceiver() {
        override fun onReceive(ctx: Context?, intent: Intent) {
            if (intent.action != BluetoothDevice.ACTION_BOND_STATE_CHANGED) return
            @Suppress("DEPRECATION")
            val device = intent.getParcelableExtra<BluetoothDevice>(BluetoothDevice.EXTRA_DEVICE)
                ?: return

            // Other devices bond too; only ours is interesting. With nothing
            // connected and nothing remembered there is no "ours", and taking
            // a stranger's bond event here would remember the wrong address.
            val ours = gatt?.device?.address ?: _pairedAddress.value ?: return
            if (device.address != ours) return

            val now = intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE,
                                         BluetoothDevice.BOND_NONE)
            val was = intent.getIntExtra(BluetoothDevice.EXTRA_PREVIOUS_BOND_STATE,
                                         BluetoothDevice.BOND_NONE)
            _bondState.value = bondStateOf(now)
            Log.i(TAG, "Bond ${device.address}: ${bondStateOf(was)} -> ${bondStateOf(now)}")

            when (now) {
                BluetoothDevice.BOND_BONDED -> {
                    rememberDevice(device.address)
                    _statusText.value = "Paired"
                    if (pendingBringUp) {
                        pendingBringUp = false
                        mainHandler.removeCallbacks(bondPoll)
                        // The bond is written before the stack has finished
                        // settling; going straight into discovery from the
                        // broadcast is the case that returns a partial
                        // database. A short hop off this callback is enough.
                        mainHandler.postDelayed({ gatt?.let { beginBringUp(it) } }, 400)
                    }
                }
                BluetoothDevice.BOND_NONE -> {
                    if (was == BluetoothDevice.BOND_BONDING) {
                        Log.w(TAG, "Pairing failed or was rejected")
                        _statusText.value = "Pairing failed"
                        pendingBringUp = false
                        gatt?.disconnect()
                    } else {
                        // The bond went away without us asking - the user
                        // removed it in Settings, or the device was reflashed
                        // and its identity address changed.
                        forgetRemembered()
                    }
                }
            }
        }
    }

    init {
        /*
         * RECEIVER_EXPORTED, deliberately. NOT_EXPORTED admits broadcasts
         * from this app and from the system, and the bond broadcast is sent
         * by neither: since Android 12 the Bluetooth stack is a mainline
         * module running as uid `bluetooth` (com.google.android.bluetooth on
         * a Pixel), and a NOT_EXPORTED receiver never sees it. Observed on
         * hardware as the phone reaching BOND_BONDED, logcat showing the
         * broadcast go out to every other listener, and this app sitting on
         * "Pairing..." indefinitely.
         *
         * Exporting is safe here: ACTION_BOND_STATE_CHANGED is a protected
         * broadcast that only the platform may send.
         */
        ContextCompat.registerReceiver(
            context,
            bondReceiver,
            IntentFilter(BluetoothDevice.ACTION_BOND_STATE_CHANGED),
            ContextCompat.RECEIVER_EXPORTED
        )

        /*
         * Trust the platform, not the preference. A bond can disappear while
         * the app is not running - removed in Settings, or the device was
         * reflashed and came up on a different identity address - and a
         * remembered address that is no longer bonded sends reconnectPaired()
         * after something that will only ever pair again.
         */
        val saved = prefs.getString("paired_address", null)
        if (saved != null) {
            val bonded = runCatching {
                bluetoothAdapter?.getRemoteDevice(saved)?.bondState
            }.getOrNull()
            if (bonded == BluetoothDevice.BOND_BONDED) {
                _pairedAddress.value = saved
                _bondState.value = BondState.BONDED
            } else {
                Log.i(TAG, "Remembered $saved is no longer bonded; forgetting it")
                prefs.edit().remove("paired_address").apply()
            }
        }
    }

    /*
     * Belt and braces for the broadcast: poll the bond state while bring-up
     * is waiting on it. If a broadcast is ever missed again - another OEM,
     * another Android release - this notices within half a second instead of
     * leaving the UI on "Pairing..." until the user gives up.
     */
    private val bondPoll = object : Runnable {
        override fun run() {
            val g = gatt ?: return
            if (!pendingBringUp) return
            val state = g.device.bondState
            _bondState.value = bondStateOf(state)
            when (state) {
                BluetoothDevice.BOND_BONDED -> {
                    Log.i(TAG, "Bond observed by polling (broadcast not seen)")
                    pendingBringUp = false
                    rememberDevice(g.device.address)
                    _statusText.value = "Paired"
                    mainHandler.postDelayed({ gatt?.let { beginBringUp(it) } }, 400)
                }
                BluetoothDevice.BOND_BONDING -> mainHandler.postDelayed(this, 500)
                else -> {
                    // NONE while we were waiting: pairing failed or was
                    // dismissed. The broadcast path handles the same case;
                    // this just stops the poll.
                }
            }
        }
    }

    private fun startBondPoll() {
        mainHandler.removeCallbacks(bondPoll)
        mainHandler.postDelayed(bondPoll, 500)
    }

    private fun rememberDevice(address: String) {
        _pairedAddress.value = address
        prefs.edit().putString("paired_address", address).apply()
    }

    private fun forgetRemembered() {
        _pairedAddress.value = null
        prefs.edit().remove("paired_address").apply()
    }

    /**
     * Reconnect to the paired device without scanning.
     *
     * autoConnect = true on purpose. It is slower to establish than a direct
     * connect, and that is exactly the property wanted here: the request stays
     * pending in the Android stack while the device is away, and completes on
     * its own when it starts advertising again. After the device sleeps on its
     * power button there is nothing to press on this end.
     *
     * @return false if no device has been paired yet
     */
    fun reconnectPaired(): Boolean {
        val address = _pairedAddress.value ?: return false
        if (_connectionState.value != ConnectionState.DISCONNECTED) return true
        if (!hasPermission(android.Manifest.permission.BLUETOOTH_CONNECT)) {
            _statusText.value = "Grant the Bluetooth permission, then tap again"
            return true
        }

        val device = try {
            bluetoothAdapter?.getRemoteDevice(address) ?: return false
        } catch (e: IllegalArgumentException) {
            Log.e(TAG, "Stored address $address is not valid", e)
            forgetRemembered()
            return false
        }

        _connectionState.value = ConnectionState.CONNECTING
        _statusText.value = "Waiting for $address..."
        _bondState.value = bondStateOf(device.bondState)
        gatt = device.connectGatt(context, true, gattCallback, BluetoothDevice.TRANSPORT_LE)
        return true
    }

    /**
     * Drop the pairing on this phone.
     *
     * BluetoothDevice.removeBond() has never been public API, and there is no
     * supported replacement - Settings is the only sanctioned route. Reflection
     * is how every app does this; it is wrapped so a future Android that hides
     * it for real degrades to a log line instead of a crash.
     *
     * The device keeps its own copy of the bond either way. Clearing only one
     * side leaves the two disagreeing, which shows up as the phone pairing
     * again on the next connection, so clear the device too with its `bond
     * clear` shell command when you want a genuinely clean slate.
     */
    fun forgetDevice() {
        val address = _pairedAddress.value
        val device = gatt?.device
            ?: address?.let { runCatching { bluetoothAdapter?.getRemoteDevice(it) }.getOrNull() }

        disconnect()
        if (device != null) {
            val removed = runCatching {
                device.javaClass.getMethod("removeBond").invoke(device) as? Boolean
            }.getOrNull()
            if (removed != true) {
                Log.w(TAG, "removeBond() unavailable - remove the pairing in Settings")
                _statusText.value = "Remove the pairing in Android Settings"
            }
        }
        forgetRemembered()
        _bondState.value = BondState.NONE
    }

    // --- Scanning ---

    fun startScan() {
        if (_connectionState.value != ConnectionState.DISCONNECTED) return

        /* First launch: the permission dialog is still up when this runs,
         * because the button requests permissions and starts the scan in the
         * same tap. startScan() threw SecurityException and took the app
         * down. Refuse politely; the user taps again after granting. */
        if (!hasPermission(android.Manifest.permission.BLUETOOTH_SCAN)) {
            _statusText.value = "Grant the Bluetooth permission, then tap again"
            return
        }

        scanner = bluetoothAdapter?.bluetoothLeScanner
        if (scanner == null) {
            _statusText.value = "BLE not available"
            return
        }

        _connectionState.value = ConnectionState.SCANNING
        _statusText.value = "Scanning for $DEVICE_NAME..."

        val filter = ScanFilter.Builder()
            .setDeviceName(DEVICE_NAME)
            .build()
        val settings = ScanSettings.Builder()
            .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY)
            .build()

        scanner?.startScan(listOf(filter), settings, scanCallback)

        // Auto-stop scan after 10s
        android.os.Handler(android.os.Looper.getMainLooper()).postDelayed({
            if (_connectionState.value == ConnectionState.SCANNING) {
                stopScan()
                _statusText.value = "Device not found"
                _connectionState.value = ConnectionState.DISCONNECTED
            }
        }, 10_000)
    }

    private fun stopScan() {
        scanner?.stopScan(scanCallback)
    }

    private val scanCallback = object : ScanCallback() {
        override fun onScanResult(callbackType: Int, result: ScanResult) {
            val device = result.device
            Log.i(TAG, "Found ${device.name ?: "?"} [${device.address}] RSSI=${result.rssi}")
            stopScan()
            connectToDevice(device)
        }

        override fun onScanFailed(errorCode: Int) {
            Log.e(TAG, "Scan failed: $errorCode")
            _statusText.value = "Scan failed (error $errorCode)"
            _connectionState.value = ConnectionState.DISCONNECTED
        }
    }

    // --- Connection ---

    private fun connectToDevice(device: BluetoothDevice) {
        _connectionState.value = ConnectionState.CONNECTING
        _statusText.value = "Connecting to ${device.address}..."
        gatt = device.connectGatt(context, false, gattCallback, BluetoothDevice.TRANSPORT_LE)
    }

    fun disconnect() {
        // Also the way out of a pending reconnectPaired(): autoConnect waits
        // indefinitely, and close() is what withdraws the request.
        if (_connectionState.value == ConnectionState.SCANNING) stopScan()
        pendingBringUp = false
        userDisconnect = true
        gatt?.let {
            it.disconnect()
            it.close()
        }
        gatt = null
        rxCharacteristic = null
        txCharacteristic = null
        _connectionState.value = ConnectionState.DISCONNECTED
        _statusText.value = "Disconnected"
    }

    /** MTU then service discovery - only ever entered on a settled bond. */
    private fun beginBringUp(g: BluetoothGatt) {
        if (REQUEST_MTU_ON_CONNECT) {
            Log.i(TAG, "Requesting MTU...")
            _statusText.value = "Connected, requesting MTU..."
            if (!g.requestMtu(251)) {
                Log.w(TAG, "requestMtu failed to start; discovering services directly")
                discoverServices(g)
            }
        } else {
            Log.i(TAG, "Discovering services...")
            discoverServices(g)
        }
    }

    private fun discoverServices(g: BluetoothGatt) {
        _statusText.value = "Discovering services..."
        if (!g.discoverServices()) {
            Log.e(TAG, "discoverServices failed to start")
            _statusText.value = "Service discovery failed to start"
            g.disconnect()
        }
    }

    private val gattCallback = object : BluetoothGattCallback() {
        override fun onConnectionStateChange(g: BluetoothGatt, status: Int, newState: Int) {
            when (newState) {
                BluetoothProfile.STATE_CONNECTED -> {
                    val bond = g.device.bondState
                    _bondState.value = bondStateOf(bond)
                    when (bond) {
                        BluetoothDevice.BOND_BONDED -> {
                            rememberDevice(g.device.address)
                            beginBringUp(g)
                        }
                        BluetoothDevice.BOND_BONDING -> {
                            Log.i(TAG, "Connected while pairing; waiting for the bond")
                            _statusText.value = "Pairing..."
                            pendingBringUp = true
                            startBondPoll()
                        }
                        else -> {
                            Log.i(TAG, "Connected, not bonded - pairing first")
                            _statusText.value = "Pairing..."
                            pendingBringUp = true
                            startBondPoll()
                            if (!g.device.createBond()) {
                                // Refused usually means pairing is already
                                // running - the peripheral's security request
                                // beat us to it. That pairing still completes
                                // (as a shade notification rather than a
                                // dialog), so wait for it like any other;
                                // bringing up the link underneath it is how
                                // discovery ends up racing SMP.
                                if (g.device.bondState == BluetoothDevice.BOND_BONDING) {
                                    Log.i(TAG, "Pairing already in progress (remote-initiated); waiting")
                                    _bondState.value = BondState.BONDING
                                } else {
                                    Log.w(TAG, "createBond() refused; bringing up unbonded")
                                    pendingBringUp = false
                                    beginBringUp(g)
                                }
                            }
                        }
                    }
                }
                BluetoothProfile.STATE_DISCONNECTED -> {
                    Log.i(TAG, "Disconnected (status=$status)")
                    pendingBringUp = false
                    _connectionState.value = ConnectionState.DISCONNECTED
                    /*
                     * 133 is Android's catch-all GATT error and it is what an
                     * authentication problem surfaces as. Naming it saves the
                     * next person from reading it as a radio fault: the usual
                     * cause is the two sides disagreeing about the bond, which
                     * `Forget` here plus `bond clear` on the device resolves.
                     */
                    _statusText.value = if (status == 133) {
                        "Disconnected (GATT 133 - try Forget, then reconnect)"
                    } else {
                        "Disconnected"
                    }
                    rxCharacteristic = null
                    txCharacteristic = null
                    gatt?.close()
                    gatt = null
                    linkHigh = false
                    afterLinkUpdate = null
                    mainHandler.removeCallbacks(linkUpdateTimeout)
                    phyText = "PHY 1M"; intervalText = ""; _linkInfo.value = ""
                    audioBusy = false
                    receivingChunk = false

                    /*
                     * The device dropped the link on its own - it went to
                     * sleep on its power button, or the radio lost it. Put
                     * the background connection straight back: it costs
                     * nothing while the device is away, and completes on its
                     * own the moment the device advertises again - which it
                     * does directed at this phone for the first 30 s after a
                     * wake. Nothing to tap.
                     *
                     * Not after a disconnect the user asked for, and not
                     * without a bond: an unbonded reconnect would only start
                     * pairing again.
                     */
                    val wasUser = userDisconnect
                    userDisconnect = false
                    if (!wasUser && _autoReconnect.value && _pairedAddress.value != null) {
                        Log.i(TAG, "Device dropped the link; re-arming background reconnect")
                        mainHandler.postDelayed({
                            if (_connectionState.value == ConnectionState.DISCONNECTED) {
                                reconnectPaired()
                            }
                        }, 300)
                    }
                }
            }
        }

        override fun onMtuChanged(g: BluetoothGatt, mtu: Int, status: Int) {
            Log.i(TAG, "MTU changed to $mtu (status=$status)")
            if (REQUEST_HIGH_PRIORITY_AFTER_MTU) {
                g.requestConnectionPriority(BluetoothGatt.CONNECTION_PRIORITY_HIGH)
            }
            discoverServices(g)
        }

        override fun onServicesDiscovered(g: BluetoothGatt, status: Int) {
            if (status != BluetoothGatt.GATT_SUCCESS) {
                Log.e(TAG, "Service discovery failed: $status")
                _statusText.value = "Service discovery failed"
                g.disconnect()
                return
            }

            val service = g.getService(NUS_SERVICE_UUID)
            if (service == null) {
                Log.e(TAG, "NUS service not found")
                _statusText.value = "NUS service not found"
                g.disconnect()
                return
            }

            rxCharacteristic = service.getCharacteristic(NUS_RX_UUID)
            txCharacteristic = service.getCharacteristic(NUS_TX_UUID)

            if (rxCharacteristic == null || txCharacteristic == null) {
                Log.e(TAG, "NUS characteristics not found")
                _statusText.value = "NUS characteristics not found"
                g.disconnect()
                return
            }

            // Enable TX notifications
            if (!g.setCharacteristicNotification(txCharacteristic, true)) {
                Log.e(TAG, "setCharacteristicNotification failed")
                _statusText.value = "Enable notification failed"
                g.disconnect()
                return
            }

            val cccd = txCharacteristic!!.getDescriptor(CCCD_UUID)
            if (cccd != null) {
                if (android.os.Build.VERSION.SDK_INT >= 33) {
                    val rc = g.writeDescriptor(cccd, BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE)
                    if (rc != BluetoothStatusCodes.SUCCESS) {
                        Log.e(TAG, "writeDescriptor failed to start: $rc")
                        _statusText.value = "Enable notification failed ($rc)"
                        g.disconnect()
                    }
                } else {
                    @Suppress("DEPRECATION")
                    cccd.value = BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE
                    @Suppress("DEPRECATION")
                    if (!g.writeDescriptor(cccd)) {
                        Log.e(TAG, "writeDescriptor failed to start")
                        _statusText.value = "Enable notification failed"
                        g.disconnect()
                    }
                }
                return
            }

            markNusReady()
        }

        override fun onDescriptorWrite(
            g: BluetoothGatt,
            descriptor: BluetoothGattDescriptor,
            status: Int
        ) {
            if (descriptor.uuid != CCCD_UUID) return

            Log.i(TAG, "CCCD write complete (status=$status)")
            if (status != BluetoothGatt.GATT_SUCCESS) {
                _statusText.value = "Enable notification failed ($status)"
                g.disconnect()
                return
            }

            markNusReady()
        }

        private fun markNusReady() {
            _connectionState.value = ConnectionState.CONNECTED
            _statusText.value = "Connected to $DEVICE_NAME"
            Log.i(TAG, "NUS service ready")

            /*
             * Ask for 2M now that the link is up and secured. The central
             * drives the PHY update; the device accepts but does not
             * initiate one on its own (its auto-PHY flag is off so the
             * central stays in control). A 251-byte packet takes 2.1 ms on
             * 1M and 1.06 ms on 2M, and the bulk transfer is bounded by how
             * many of them fit in a connection event, so this alone roughly
             * doubles it. The interval is the other half - see
             * setLinkPriority().
             */
            gatt?.setPreferredPhy(
                BluetoothDevice.PHY_LE_2M_MASK,
                BluetoothDevice.PHY_LE_2M_MASK,
                BluetoothDevice.PHY_OPTION_NO_PREFERRED
            )

            // Query initial status, then settle on the idle interval.
            sendCommand(byteArrayOf(CMD_GET_STATUS))
            linkHigh = true      // force the request: a fresh link is BALANCED, not LOW_POWER
            setLinkPriority(high = false)
        }

        override fun onPhyUpdate(g: BluetoothGatt, txPhy: Int, rxPhy: Int, status: Int) {
            Log.i(TAG, "PHY now TX=${phyName(txPhy)} RX=${phyName(rxPhy)} (status=$status)")
            phyText = "PHY ${phyName(txPhy)}"
            _linkInfo.value = "$phyText · $intervalText"
        }

        /*
         * No `override`: onConnectionUpdated is @hide in the SDK stubs, so the
         * compiler cannot see the base method. The JVM dispatches by name and
         * descriptor regardless, so a method with this exact signature is
         * what the framework calls - which is how every app that reports the
         * interval does it. If a release ever changes the signature this
         * silently stops being called; the 1.5 s timeout covers that case.
         */
        @Suppress("unused")
        fun onConnectionUpdated(g: BluetoothGatt, interval: Int, latency: Int,
                                timeout: Int, status: Int) {
            val ms = interval * 1.25
            Log.i(TAG, "Connection updated: interval ${"%.2f".format(ms)} ms, " +
                       "latency $latency, timeout ${timeout * 10} ms (status=$status)")
            intervalText = "${"%.1f".format(ms)} ms" + if (latency > 0) " ×${latency + 1}" else ""
            _linkInfo.value = "$phyText · $intervalText"
            afterLinkUpdate?.let {
                afterLinkUpdate = null
                mainHandler.removeCallbacks(linkUpdateTimeout)
                it()
            }
        }

        // API 33+：框架傳入不可變副本 — 無共享 value 競態(2M 亂序根因)
        override fun onCharacteristicChanged(
            g: BluetoothGatt,
            characteristic: BluetoothGattCharacteristic,
            value: ByteArray
        ) {
            if (characteristic.uuid == NUS_TX_UUID) {
                handleNotification(value)
            }
        }

        // API <33 舊路徑：characteristic.value 是共享物件,高速通知下會被
        // 下一包覆寫(亂序/重複的來源)。33+ 由上面的新 callback 處理,這裡
        // 直接略過避免雙重處理;舊機種立刻複製一份降低競態窗口。
        @Deprecated("Deprecated in Java")
        override fun onCharacteristicChanged(
            g: BluetoothGatt,
            characteristic: BluetoothGattCharacteristic
        ) {
            if (android.os.Build.VERSION.SDK_INT >= 33) return
            if (characteristic.uuid == NUS_TX_UUID) {
                handleNotification(characteristic.value.copyOf())
            }
        }
    }

    // --- Notification parsing ---

    @Synchronized  // 回呼可能跑在多條 binder 執行緒 — 序列化重組,防交錯
    private fun handleNotification(data: ByteArray) {
        if (data.isEmpty()) return
        val rspId = data[0].toInt() and 0xFF

        when (rspId) {
            RSP_STATUS -> parseStatus(data)
            RSP_AUDIO_HDR -> parseAudioHeader(data)
            RSP_AUDIO_DATA -> parseAudioData(data)
            RSP_AUDIO_DONE -> parseAudioDone(data)
            RSP_ERROR -> parseError(data)
            RSP_AUDIO_LC3 -> parseLc3Frame(data)
            RSP_EPD_PAGE -> parseEpdPage(data)
            else -> Log.w(TAG, "Unknown response 0x${rspId.toString(16)}")
        }
    }

    private fun parseStatus(data: ByteArray) {
        if (data.size < 6) return
        val bb = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN)
        bb.get() // skip rsp_id
        val state = bb.get().toInt() and 0xFF
        val recordedBytes = bb.int

        val prev = _deviceStatus.value
        val stateStr = stateName(state)
        _deviceStatus.value = prev.copy(state = state, recordedBytes = recordedBytes)
        _statusText.value = "Device: $stateStr, $recordedBytes bytes"
        Log.i(TAG, "Status: state=$stateStr, bytes=$recordedBytes")

        recordingRequested = LinkPriorityPolicy.recordingRequestedForStatus(
            state, STATUS_RECORDING, STATUS_CHUNK_READY,
            STATUS_IDLE, STATUS_RECORDED, recordingRequested)

        if (state == STATUS_CHUNK_READY && _autoChunk.value && !audioBusy) {
            Log.i(TAG, "CHUNK_READY — auto GET_CHUNK")
            requestChunk()
        }
    }

    private fun parseEpdPage(data: ByteArray) {
        if (data.size < 3) return
        val index = data[1].toInt() and 0xFF
        val count = data[2].toInt() and 0xFF
        _deviceStatus.value = _deviceStatus.value.copy(epdIndex = index, epdCount = count)
        _statusText.value = if (count == 0) "EPD summary cleared"
            else "EPD page ${index + 1}/$count"
    }

    private fun parseAudioHeader(data: ByteArray) {
        if (data.size < 7) return
        val bb = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN)
        bb.get() // skip rsp_id
        val sr = bb.short.toInt() and 0xFFFF
        audioTotalBytes = bb.int
        audioSampleRate = sr
        audioReceivedBytes = 0
        audioBuffer = ByteArray(audioTotalBytes)
        _audioTransferProgress.value = 0f
        _audioReady.value = false
        audioBusy = true
        bulkPkts = 0
        bulkStartMs = SystemClock.elapsedRealtime()
        seqLast = -1
        seqGaps = 0
        seqLostPkts = 0
        seqGapDetail.setLength(0)
        seqExtLast = -1
        bulkChunk = 0
        rawCap = if (_seqDebug.value && BuildConfig.DEBUG)
            java.io.ByteArrayOutputStream(audioTotalBytes + audioTotalBytes / 16) else null
        _statusText.value = "Receiving audio: 0 / $audioTotalBytes bytes"
        Log.i(TAG, "Audio header: $audioTotalBytes bytes, ${audioSampleRate}Hz, rawCap=${rawCap != null}")
    }

    private fun parseAudioData(data: ByteArray) {
        if (data.size < 2) return
        // 原始側錄:任何解析/擺放之前,先原封不動記下到達位元組+時間戳
        rawCap?.let { cap ->
            val t = (SystemClock.elapsedRealtime() - bulkStartMs).toInt()
            cap.write(t and 0xFF); cap.write((t shr 8) and 0xFF)
            cap.write((t shr 16) and 0xFF); cap.write((t shr 24) and 0xFF)
            cap.write(data.size and 0xFF); cap.write((data.size shr 8) and 0xFF)
            cap.write(data, 0, data.size)
        }
        // data[0] = RSP_AUDIO_DATA;
        // seqDebug off: data[1..] = PCM / on: data[1]=seq, data[2..] = PCM
        val payloadOffset = if (_seqDebug.value) 2 else 1
        if (data.size <= payloadOffset) return
        val pcmData = data.copyOfRange(payloadOffset, data.size)

        // If streaming mode, forward PCM directly to audio player
        if (_isStreaming.value) {
            onStreamData?.invoke(pcmData)
            return
        }

        // Bulk transfer mode — 統計 + 缺號偵測 + 按序號定位寫入
        bulkPkts++
        if (_seqDebug.value) {
            val seq = data[1].toInt() and 0xFF
            if (seqLast >= 0) {
                val expect = (seqLast + 1) and 0xFF
                if (seq != expect) {
                    val miss = (seq - expect) and 0xFF
                    seqGaps++
                    seqLostPkts += miss
                    if (seqGaps <= 20) {
                        seqGapDetail.append(
                            "#$bulkPkts: $seqLast->$seq (到達順序異常)\n")
                    }
                }
            }
            seqLast = seq

            // 8-bit 序號展開為絕對序號(容忍 ±127 亂序窗;裝置端每次
            // GET_AUDIO 由 0 起算) → 寫入位置與到達順序解耦。
            val ext: Int = if (seqExtLast < 0) {
                seq
            } else {
                val diff = ((seq - (seqExtLast and 0xFF)) + 256) and 0xFF
                if (diff < 128) seqExtLast + diff else seqExtLast - (256 - diff)
            }
            if (bulkChunk == 0) bulkChunk = pcmData.size
            val off = ext * bulkChunk
            if (ext >= 0 && off < audioTotalBytes) {
                val copyLen = minOf(pcmData.size, audioTotalBytes - off)
                System.arraycopy(pcmData, 0, audioBuffer, off, copyLen)
                audioReceivedBytes += copyLen
            }
            if (ext > seqExtLast) seqExtLast = ext
        } else {
            // 無序號模式:只能照到達順序累加(亂序時會錯位 — 開 Seq 驗證)
            val copyLen = minOf(pcmData.size, audioTotalBytes - audioReceivedBytes)
            if (copyLen > 0) {
                System.arraycopy(pcmData, 0, audioBuffer, audioReceivedBytes, copyLen)
                audioReceivedBytes += copyLen
            }
        }
        val progress = if (audioTotalBytes > 0) audioReceivedBytes.toFloat() / audioTotalBytes else 0f
        _audioTransferProgress.value = progress
        if (audioReceivedBytes % 8192 < pcmData.size) {
            _statusText.value = "Receiving: $audioReceivedBytes / $audioTotalBytes bytes"
        }
    }

    private fun parseAudioDone(data: ByteArray) {
        var totalSent = -1
        if (data.size >= 5) {
            val bb = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN)
            bb.get() // skip rsp_id
            totalSent = bb.int
        }
        // 原始側錄落檔:格式 magic "IRAW1" + N×[u32 t_ms][u16 len][bytes]
        rawCap?.let { cap ->
            try {
                val ts = java.text.SimpleDateFormat("yyyyMMdd_HHmmss", java.util.Locale.US)
                    .format(java.util.Date())
                val dir = android.os.Environment.getExternalStoragePublicDirectory(
                    android.os.Environment.DIRECTORY_DOWNLOADS)
                val f = java.io.File(dir, "IPRO_RAW_$ts.bin")
                java.io.FileOutputStream(f).use { fos ->
                    fos.write("IRAW1".toByteArray())
                    cap.writeTo(fos)
                }
                Log.i(TAG, "Raw capture saved: ${f.absolutePath} (${cap.size() + 5} bytes)")
                _statusText.value = "Raw capture: ${f.name}"
            } catch (e: Exception) {
                Log.e(TAG, "Raw capture save failed: ${e.message}")
            }
            rawCap = null
        }
        _audioTransferProgress.value = 1f
        _audioReady.value = true
        audioBusy = false
        if (receivingChunk) {
            _chunkCount.value = _chunkCount.value + 1
            receivingChunk = false
        }

        // --- 2M 斷音判決 ---
        val elapsedMs = SystemClock.elapsedRealtime() - bulkStartMs
        val kBps = if (elapsedMs > 0) audioReceivedBytes / elapsedMs else 0
        if (LinkPriorityPolicy.shouldDowngradeAfterAudio(recordingRequested)) {
            setLinkPriority(high = false)
        }
        val lostBytes = audioTotalBytes - audioReceivedBytes
        val verdict = when {
            _seqDebug.value && seqGaps == 0 && lostBytes == 0 ->
                "✔ 無丟包無亂序 — 傳輸完整"
            _seqDebug.value && lostBytes == 0 ->
                "✔ 亂序 $seqGaps 筆已按序號校正寫入 — 音檔完整應無斷音" +
                    "(根因:手機藍牙棧多執行緒派發,到達順序不可信)"
            _seqDebug.value ->
                "✘ 真丟包:少收 $lostBytes bytes — 通知過載,降速(ble_pace)可解"
            lostBytes > 0 ->
                "✘ 少收 $lostBytes bytes (開 Seq 驗證可定位+校正)"
            else -> "✔ 位元組數吻合(順序未驗證 — 開 Seq 驗證確認)"
        }
        _statusText.value =
            "收 $audioReceivedBytes/$audioTotalBytes B · $bulkPkts 包 · " +
                "${elapsedMs}ms (${kBps}kB/s)\n$verdict"
        Log.i(TAG, "Audio done: sent=$totalSent recv=$audioReceivedBytes " +
                "pkts=$bulkPkts gaps=$seqGaps lostPkts=$seqLostPkts " +
                "elapsed=${elapsedMs}ms")
        if (seqGapDetail.isNotEmpty()) {
            Log.w(TAG, "Gap detail (前20處):\n$seqGapDetail")
        }
    }

    private fun parseError(data: ByteArray) {
        val code = if (data.size > 1) data[1].toInt() and 0xFF else 0
        val msg = if (data.size > 2) String(data, 2, data.size - 2) else ""
        audioBusy = false
        receivingChunk = false
        _statusText.value = "Error $code: $msg"
        Log.e(TAG, "Device error: code=$code, msg=$msg")
    }

    @Synchronized
    private fun parseLc3Frame(data: ByteArray) {
        if (data.size < 2) return

        // Init decoder on first LC3 frame
        if (!lc3Initialized) {
            lc3Initialized = lc3Decoder.setup(10000, 16000)
            if (!lc3Initialized) {
                Log.e(TAG, "LC3 decoder init failed")
                return
            }
        }

        // Payload may contain 1 or 2 packed LC3 frames (40 or 80 bytes)
        val payload = data.copyOfRange(1, data.size)
        val frameSize = 40  // LC3_OCTETS

        var offset = 0
        while (offset + frameSize <= payload.size) {
            val lc3Data = payload.copyOfRange(offset, offset + frameSize)
            val pcmBytes = lc3Decoder.decodeToPcmBytes(lc3Data)
            if (pcmBytes != null) {
                onStreamData?.invoke(pcmBytes)
            }
            offset += frameSize
        }
    }

    // --- Commands ---

    fun sendCommand(cmd: ByteArray) {
        val rx = rxCharacteristic ?: return
        val g = gatt ?: return
        val ok = if (android.os.Build.VERSION.SDK_INT >= 33) {
            val rc = g.writeCharacteristic(rx, cmd, BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE)
            if (rc != BluetoothStatusCodes.SUCCESS) {
                Log.e(TAG, "writeCharacteristic failed to start: $rc")
            }
            rc == BluetoothStatusCodes.SUCCESS
        } else {
            @Suppress("DEPRECATION")
            rx.value = cmd
            rx.writeType = BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE
            @Suppress("DEPRECATION")
            g.writeCharacteristic(rx)
        }
        if (!ok) {
            Log.e(TAG, "sendCommand failed cmd=0x${(cmd.firstOrNull()?.toInt() ?: 0).and(0xFF).toString(16)}")
        }
    }

    fun startRecording(seconds: Int) {
        val sec = seconds.coerceIn(1, MAX_RECORD_SECONDS)
        recordingRequested = true
        _audioReady.value = false
        _chunkCount.value = 0
        receivingChunk = false
        val payload = ByteArray(3)
        payload[0] = CMD_START_REC
        payload[1] = (sec and 0xFF).toByte()
        payload[2] = ((sec shr 8) and 0xFF).toByte()
        sendCommand(payload)
    }

    fun stopRecording() {
        recordingRequested = false
        sendCommand(byteArrayOf(CMD_STOP_REC))
    }

    private fun phyName(phy: Int) = when (phy) {
        BluetoothDevice.PHY_LE_1M -> "1M"
        BluetoothDevice.PHY_LE_2M -> "2M"
        BluetoothDevice.PHY_LE_CODED -> "Coded"
        else -> "?$phy"
    }

    private val _linkInfo = MutableStateFlow("")
    val linkInfo: StateFlow<String> = _linkInfo
    private var phyText = "PHY 1M"
    private var intervalText = ""

    /*
     * Connection interval: two settings, nothing in between.
     *
     * Idle - connected, nothing moving - runs at LOW_POWER (100-125 ms with
     * peripheral latency 2 on Android), which is what lets the device sit on
     * the link for hours. Moving audio runs at HIGH (11.25-15 ms): bulk
     * throughput is packets-per-event / interval, so this alone is worth
     * ~8x over idle. The switch is made around each transfer rather than
     * left high, because HIGH also multiplies the phone's radio duty.
     *
     * The device does not request intervals itself (its auto-update flag is
     * off) so that the central's choice here is the one that sticks.
     *
     * A parameter update is not instant - the peripheral has to acknowledge
     * an instant several events out - so anything that wants the new
     * interval waits for onConnectionUpdated() before it starts. Bounded by
     * a timer, for phones that quietly ignore the request.
     */
    private var linkHigh = false
    private var afterLinkUpdate: (() -> Unit)? = null
    private val linkUpdateTimeout = Runnable {
        afterLinkUpdate?.let {
            Log.w(TAG, "Connection update not reported in time; proceeding anyway")
            afterLinkUpdate = null
            it()
        }
    }

    private fun setLinkPriority(high: Boolean, then: (() -> Unit)? = null) {
        val g = gatt ?: run { then?.invoke(); return }
        if (high == linkHigh) {
            then?.invoke()
            return
        }
        linkHigh = high
        val ok = g.requestConnectionPriority(
            if (high) BluetoothGatt.CONNECTION_PRIORITY_HIGH
            else BluetoothGatt.CONNECTION_PRIORITY_LOW_POWER
        )
        Log.i(TAG, "Connection priority -> ${if (high) "HIGH" else "LOW_POWER"} ($ok)")
        if (then != null) {
            afterLinkUpdate = then
            mainHandler.removeCallbacks(linkUpdateTimeout)
            /*
             * Four seconds, not one and a half. From LOW_POWER the
             * peripheral is listening once every ~1 s (124 ms x latency 7),
             * and a connection update needs the instant acknowledged several
             * events out, so the switch to HIGH takes 2-4 s to land. With
             * the shorter timeout every transfer started on the slow
             * interval and ran its first second at a fifth of the rate.
             */
            mainHandler.postDelayed(linkUpdateTimeout, 4000)
        }
    }

    fun requestAudio() {
        _audioTransferProgress.value = 0f
        _audioReady.value = false
        receivingChunk = false
        audioBusy = true
        setLinkPriority(high = true) {
            sendCommand(byteArrayOf(CMD_GET_AUDIO))
        }
    }

    fun requestChunk() {
        if (audioBusy) {
            Log.w(TAG, "GET_CHUNK skipped — transfer in progress")
            return
        }
        _audioTransferProgress.value = 0f
        _audioReady.value = false
        receivingChunk = true
        audioBusy = true
        setLinkPriority(high = true) {
            sendCommand(byteArrayOf(CMD_GET_CHUNK))
        }
    }

    fun dacPlay() {
        sendCommand(byteArrayOf(CMD_DAC_PLAY))
    }

    fun dacStop() {
        sendCommand(byteArrayOf(CMD_DAC_STOP))
    }

    fun epdPage(delta: Int) {
        sendCommand(byteArrayOf(CMD_EPD_PAGE, delta.toByte()))
    }

    // Chunked EPD_TEXT: offset u16 LE, total u16 LE, then utf8
    fun sendEpdText(text: String) {
        val bytes = text.toByteArray(Charsets.UTF_8)
        if (bytes.isEmpty()) return
        val total = minOf(bytes.size, EPD_TEXT_MAX)
        var offset = 0
        val maxData = 180
        while (offset < total) {
            val n = minOf(maxData, total - offset)
            val pkt = ByteArray(5 + n)
            pkt[0] = CMD_EPD_TEXT
            pkt[1] = (offset and 0xFF).toByte()
            pkt[2] = ((offset shr 8) and 0xFF).toByte()
            pkt[3] = (total and 0xFF).toByte()
            pkt[4] = ((total shr 8) and 0xFF).toByte()
            System.arraycopy(bytes, offset, pkt, 5, n)
            sendCommand(pkt)
            offset += n
        }
    }

    fun requestStatus() {
        sendCommand(byteArrayOf(CMD_GET_STATUS))
    }

    fun setGain(db: Int) {
        sendCommand(byteArrayOf(CMD_SET_GAIN, db.toByte()))
    }

    fun startStreaming() {
        _isStreaming.value = true
        setLinkPriority(high = true) {
            sendCommand(byteArrayOf(CMD_START_STREAM))
        }
    }

    fun stopStreaming() {
        sendCommand(byteArrayOf(CMD_STOP_STREAM))
        _isStreaming.value = false
        onStreamData = null
        setLinkPriority(high = false)
    }

    // --- Audio data access ---

    fun getAudioData(): ByteArray = audioBuffer.copyOf(audioReceivedBytes)
    fun getAudioSampleRate(): Int = audioSampleRate

    fun cleanup() {
        runCatching { context.unregisterReceiver(bondReceiver) }
        mainHandler.removeCallbacksAndMessages(null)
        stopScan()
        gatt?.disconnect()
        gatt?.close()
        gatt = null
        lc3Decoder.release()
        lc3Initialized = false
    }
}

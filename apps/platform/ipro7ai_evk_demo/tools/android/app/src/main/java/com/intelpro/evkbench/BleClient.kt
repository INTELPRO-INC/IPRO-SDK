package com.intelpro.evkbench

import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothGatt
import android.bluetooth.BluetoothGattCallback
import android.bluetooth.BluetoothGattCharacteristic
import android.bluetooth.BluetoothGattDescriptor
import android.bluetooth.BluetoothProfile
import android.bluetooth.le.ScanCallback
import android.bluetooth.le.ScanFilter
import android.bluetooth.le.ScanResult
import android.bluetooth.le.ScanSettings
import android.content.Context
import android.os.Build
import android.os.Handler
import android.os.Looper
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import java.util.UUID

/**
 * Talks to the board's Nordic UART Service.
 *
 * The firmware bridges that service straight to its shell, so this is a
 * transport and nothing more: anything the serial console accepts goes over
 * unchanged, and commands added to the firmware later need no change here.
 */
class BleClient(private val context: Context) {

    companion object {
        val NUS: UUID = UUID.fromString("6e400001-b5a3-f393-e0a9-e50e24dcca9e")
        val RX:  UUID = UUID.fromString("6e400002-b5a3-f393-e0a9-e50e24dcca9e")
        val TX:  UUID = UUID.fromString("6e400003-b5a3-f393-e0a9-e50e24dcca9e")
        private val CCCD: UUID =
            UUID.fromString("00002902-0000-1000-8000-00805f9b34fb")

        const val DEFAULT_NAME = "IPRO7AI-EVK"
        private const val SCAN_MS = 12_000L
    }

    enum class State { Idle, Scanning, Connecting, Ready, Failed }

    var state by mutableStateOf(State.Idle)
        private set
    var deviceName by mutableStateOf<String?>(null)
        private set
    var lastError by mutableStateOf<String?>(null)
        private set

    /** Everything the board has said, appended as it arrives. */
    var log by mutableStateOf("")
        private set

    private val main = Handler(Looper.getMainLooper())
    private var gatt: BluetoothGatt? = null
    private var rx: BluetoothGattCharacteristic? = null
    private var scanning = false

    private val adapter: BluetoothAdapter?
        get() = (context.getSystemService(Context.BLUETOOTH_SERVICE)
                as? android.bluetooth.BluetoothManager)?.adapter

    fun append(text: String) {
        main.post {
            // Keep the tail rather than growing without bound - a long session
            // of `sd ls` and register dumps adds up.
            val next = log + text
            log = if (next.length > 60_000) next.takeLast(48_000) else next
        }
    }

    fun clearLog() { log = "" }

    @SuppressLint("MissingPermission")
    fun connect(name: String = DEFAULT_NAME) {
        val ad = adapter
        if (ad == null || !ad.isEnabled) {
            fail("Bluetooth is off. Turn it on and try again.")
            return
        }
        if (scanning || state == State.Connecting) return

        state = State.Scanning
        lastError = null
        append("— scanning for \"$name\" —\n")

        val scanner = ad.bluetoothLeScanner ?: run {
            fail("No BLE scanner available on this device.")
            return
        }

        // Filter by name rather than service UUID: the board advertises its
        // LE Audio UUIDs, not the shell service, so a UUID filter finds nothing.
        val filter = ScanFilter.Builder().setDeviceName(name).build()
        val settings = ScanSettings.Builder()
            .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY)
            .build()

        val cb = object : ScanCallback() {
            override fun onScanResult(callbackType: Int, result: ScanResult) {
                stopScan(this)
                open(result.device)
            }
            override fun onScanFailed(errorCode: Int) {
                stopScan(this)
                fail("Scan failed (code $errorCode).")
            }
        }

        scanning = true
        scanner.startScan(listOf(filter), settings, cb)

        main.postDelayed({
            if (scanning) {
                stopScan(cb)
                fail(
                    "No board called \"$name\" is advertising.\n" +
                    "Run `bt init` on the serial console once, and check it is " +
                    "not already connected elsewhere — the firmware allows a " +
                    "single connection."
                )
            }
        }, SCAN_MS)
    }

    @SuppressLint("MissingPermission")
    private fun stopScan(cb: ScanCallback) {
        if (!scanning) return
        scanning = false
        runCatching { adapter?.bluetoothLeScanner?.stopScan(cb) }
    }

    @SuppressLint("MissingPermission")
    private fun open(device: BluetoothDevice) {
        state = State.Connecting
        deviceName = device.name
        append("— connecting to ${device.name ?: device.address} —\n")
        gatt = device.connectGatt(context, false, callback, BluetoothDevice.TRANSPORT_LE)
    }

    @SuppressLint("MissingPermission")
    fun disconnect() {
        gatt?.disconnect()
    }

    @SuppressLint("MissingPermission")
    fun send(command: String) {
        val c = rx
        val g = gatt
        if (c == null || g == null || state != State.Ready) {
            append("not connected\n")
            return
        }
        append("› $command\n")
        val bytes = (command + "\n").toByteArray()

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            g.writeCharacteristic(
                c, bytes,
                BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE
            )
        } else {
            @Suppress("DEPRECATION")
            run {
                c.writeType = BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE
                c.value = bytes
                g.writeCharacteristic(c)
            }
        }
    }

    private fun fail(msg: String) {
        main.post {
            state = State.Failed
            lastError = msg
            append("$msg\n")
        }
    }

    private val callback = object : BluetoothGattCallback() {

        @SuppressLint("MissingPermission")
        override fun onConnectionStateChange(g: BluetoothGatt, status: Int, newState: Int) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                // A larger MTU means fewer notifications for the same output;
                // the board answers with whatever it is given.
                g.requestMtu(247)
            } else {
                main.post {
                    rx = null
                    state = State.Idle
                    append("— disconnected —\n")
                }
                g.close()
                gatt = null
            }
        }

        @SuppressLint("MissingPermission")
        override fun onMtuChanged(g: BluetoothGatt, mtu: Int, status: Int) {
            g.discoverServices()
        }

        @SuppressLint("MissingPermission")
        override fun onServicesDiscovered(g: BluetoothGatt, status: Int) {
            val svc = g.getService(NUS)
            if (svc == null) {
                fail(
                    "Connected, but this device has no shell service.\n" +
                    "The firmware needs bt_shell_init() — check the boot log " +
                    "for \"shell over BLE ready\"."
                )
                g.disconnect()
                return
            }
            rx = svc.getCharacteristic(RX)
            val tx = svc.getCharacteristic(TX)
            if (rx == null || tx == null) {
                fail("Shell service is missing its characteristics.")
                g.disconnect()
                return
            }

            g.setCharacteristicNotification(tx, true)
            // The descriptor write is what actually turns notifications on;
            // setCharacteristicNotification alone only arms the local side.
            tx.getDescriptor(CCCD)?.let { d ->
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                    g.writeDescriptor(d, BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE)
                } else {
                    @Suppress("DEPRECATION")
                    run {
                        d.value = BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE
                        g.writeDescriptor(d)
                    }
                }
            }

            main.post {
                state = State.Ready
                append("— ready —\n")
            }
        }

        @Deprecated("Kept for API < 33")
        @Suppress("DEPRECATION")
        override fun onCharacteristicChanged(
            g: BluetoothGatt, c: BluetoothGattCharacteristic
        ) {
            if (c.uuid == TX) append(String(c.value))
        }

        override fun onCharacteristicChanged(
            g: BluetoothGatt, c: BluetoothGattCharacteristic, value: ByteArray
        ) {
            if (c.uuid == TX) append(String(value))
        }
    }
}

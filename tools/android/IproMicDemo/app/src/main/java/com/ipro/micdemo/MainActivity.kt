package com.ipro.micdemo

import android.Manifest
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.material3.MaterialTheme
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.core.content.ContextCompat
import com.ipro.micdemo.ui.MainScreen

class MainActivity : ComponentActivity() {

    private lateinit var bleManager: BleManager
    private lateinit var audioPlayer: AudioPlayer

    private val requiredPermissions: Array<String>
        get() {
            val base = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                mutableListOf(
                    Manifest.permission.BLUETOOTH_SCAN,
                    Manifest.permission.BLUETOOTH_CONNECT,
                    Manifest.permission.ACCESS_FINE_LOCATION
                )
            } else {
                mutableListOf(
                    Manifest.permission.BLUETOOTH,
                    Manifest.permission.BLUETOOTH_ADMIN,
                    Manifest.permission.ACCESS_FINE_LOCATION
                )
            }
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                base.add(Manifest.permission.POST_NOTIFICATIONS)
            }
            return base.toTypedArray()
        }

    private val permissionLauncher = registerForActivityResult(
        ActivityResultContracts.RequestMultiplePermissions()
    ) { results ->
        val bleOk = results
            .filterKeys { it != Manifest.permission.POST_NOTIFICATIONS }
            .values
            .all { it }
        if (bleOk) {
            bleManager.startScan()
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val app = application as IproApplication
        bleManager = app.bleManager
        audioPlayer = app.audioPlayer

        setContent {
            MaterialTheme {
                val connectionState by bleManager.connectionState.collectAsState()
                LaunchedEffect(connectionState) {
                    when (connectionState) {
                        BleManager.ConnectionState.CONNECTED ->
                            BleForegroundService.start(this@MainActivity)
                        BleManager.ConnectionState.DISCONNECTED ->
                            BleForegroundService.stop(this@MainActivity)
                        else -> Unit
                    }
                }
                MainScreen(
                    bleManager = bleManager,
                    audioPlayer = audioPlayer,
                    onRequestPermissions = { requestPermissionsIfNeeded() }
                )
            }
        }
    }

    private fun requestPermissionsIfNeeded() {
        val missing = requiredPermissions.filter {
            ContextCompat.checkSelfPermission(this, it) != PackageManager.PERMISSION_GRANTED
        }
        if (missing.isNotEmpty()) {
            permissionLauncher.launch(missing.toTypedArray())
        }
    }

    override fun onDestroy() {
        // Do not tear down BLE here — Foreground Service keeps the link.
        // User Disconnect stops the service.
        if (isFinishing) {
            audioPlayer.stop()
        }
        super.onDestroy()
    }
}

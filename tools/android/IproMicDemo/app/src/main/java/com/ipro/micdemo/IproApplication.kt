package com.ipro.micdemo

import android.app.Application

/**
 * Process-wide BLE/audio so the link survives Activity destroy
 * while [BleForegroundService] is running.
 */
class IproApplication : Application() {
    lateinit var bleManager: BleManager
        private set
    lateinit var audioPlayer: AudioPlayer
        private set

    override fun onCreate() {
        super.onCreate()
        bleManager = BleManager(this)
        audioPlayer = AudioPlayer()
    }
}

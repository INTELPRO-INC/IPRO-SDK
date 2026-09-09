package com.ipro.micdemo

import android.app.Notification
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.app.Service
import android.content.Context
import android.content.Intent
import android.content.pm.ServiceInfo
import android.os.Build
import android.os.IBinder
import android.util.Log
import androidx.core.app.NotificationCompat
import androidx.core.content.ContextCompat

/**
 * Keeps the process alive for BLE GATT while the Activity is in the background.
 */
class BleForegroundService : Service() {

    companion object {
        private const val TAG = "BleFgService"
        private const val CHANNEL_ID = "ipro_ble_bg"
        private const val NOTIFICATION_ID = 1001
        const val ACTION_START = "com.ipro.micdemo.action.BLE_FG_START"
        const val ACTION_STOP = "com.ipro.micdemo.action.BLE_FG_STOP"
        const val ACTION_UPDATE = "com.ipro.micdemo.action.BLE_FG_UPDATE"
        const val EXTRA_TEXT = "text"

        fun start(context: Context) {
            val i = Intent(context, BleForegroundService::class.java).apply {
                action = ACTION_START
            }
            ContextCompat.startForegroundService(context, i)
        }

        fun stop(context: Context) {
            context.stopService(Intent(context, BleForegroundService::class.java))
        }

        fun updateNotification(context: Context, text: String) {
            val i = Intent(context, BleForegroundService::class.java).apply {
                action = ACTION_UPDATE
                putExtra(EXTRA_TEXT, text)
            }
            try {
                context.startService(i)
            } catch (e: Exception) {
                Log.w(TAG, "updateNotification: ${e.message}")
            }
        }
    }

    private var started = false

    override fun onBind(intent: Intent?): IBinder? = null

    override fun onCreate() {
        super.onCreate()
        ensureChannel()
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        when (intent?.action) {
            ACTION_STOP -> {
                stopForegroundService()
                stopSelf()
                return START_NOT_STICKY
            }
            ACTION_UPDATE -> {
                val text = intent.getStringExtra(EXTRA_TEXT)
                    ?: getString(R.string.ble_fg_notification_text)
                if (started) {
                    val nm = getSystemService(NOTIFICATION_SERVICE) as NotificationManager
                    nm.notify(NOTIFICATION_ID, buildNotification(text))
                }
                return START_STICKY
            }
            else -> {
                enterForeground(
                    intent?.getStringExtra(EXTRA_TEXT)
                        ?: getString(R.string.ble_fg_notification_text)
                )
            }
        }
        return START_STICKY
    }

    override fun onDestroy() {
        started = false
        super.onDestroy()
    }

    private fun enterForeground(text: String) {
        val notification = buildNotification(text)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.UPSIDE_DOWN_CAKE) {
            startForeground(
                NOTIFICATION_ID,
                notification,
                ServiceInfo.FOREGROUND_SERVICE_TYPE_CONNECTED_DEVICE
            )
        } else {
            startForeground(NOTIFICATION_ID, notification)
        }
        started = true
        Log.i(TAG, "Foreground started")
    }

    private fun stopForegroundService() {
        stopForeground(STOP_FOREGROUND_REMOVE)
        started = false
        Log.i(TAG, "Foreground stopped")
    }

    private fun ensureChannel() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) return
        val nm = getSystemService(NOTIFICATION_SERVICE) as NotificationManager
        if (nm.getNotificationChannel(CHANNEL_ID) != null) return
        val ch = NotificationChannel(
            CHANNEL_ID,
            getString(R.string.ble_fg_channel_name),
            NotificationManager.IMPORTANCE_LOW
        ).apply {
            description = getString(R.string.ble_fg_channel_desc)
            setShowBadge(false)
        }
        nm.createNotificationChannel(ch)
    }

    private fun buildNotification(text: String): Notification {
        val launch = PendingIntent.getActivity(
            this,
            0,
            Intent(this, MainActivity::class.java).apply {
                flags = Intent.FLAG_ACTIVITY_SINGLE_TOP or Intent.FLAG_ACTIVITY_CLEAR_TOP
            },
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
        )
        return NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle(getString(R.string.ble_fg_notification_title))
            .setContentText(text)
            .setSmallIcon(android.R.drawable.stat_sys_data_bluetooth)
            .setContentIntent(launch)
            .setOngoing(true)
            .setOnlyAlertOnce(true)
            .setCategory(NotificationCompat.CATEGORY_SERVICE)
            .setPriority(NotificationCompat.PRIORITY_LOW)
            .build()
    }
}

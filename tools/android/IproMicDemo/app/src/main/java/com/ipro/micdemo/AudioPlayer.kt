package com.ipro.micdemo

import android.content.Context
import android.media.AudioAttributes
import android.media.AudioFormat
import android.media.AudioTrack
import android.os.Environment
import android.util.Log
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import java.io.File
import java.io.FileOutputStream
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

/**
 * Simple AudioTrack wrapper for playing 16-bit mono PCM at a given sample rate.
 */
class AudioPlayer {

    companion object {
        private const val TAG = "AudioPlayer"
    }

    private val _isPlaying = MutableStateFlow(false)
    val isPlaying: StateFlow<Boolean> = _isPlaying

    private val _isStreamPlaying = MutableStateFlow(false)
    val isStreamPlaying: StateFlow<Boolean> = _isStreamPlaying

    private var audioTrack: AudioTrack? = null
    private var streamTrack: AudioTrack? = null

    /** Streaming path software gain (整段峰值未知,維持手動) */
    var softwareGain: Int = 32

    fun play(pcmData: ByteArray, sampleRate: Int) {
        stop()

        if (pcmData.isEmpty()) {
            Log.w(TAG, "No audio data to play")
            return
        }

        // 峰值自動正規化(取代固定 ×32):錄音位準高時固定增益會硬削頂,
        // 削頂失真聽起來就是劈啪破音 — 正規化到 ~80% 滿刻度,永不削頂。
        val amplified = normalizePcm(pcmData)

        val bufferSize = AudioTrack.getMinBufferSize(
            sampleRate,
            AudioFormat.CHANNEL_OUT_MONO,
            AudioFormat.ENCODING_PCM_16BIT
        )

        audioTrack = AudioTrack.Builder()
            .setAudioAttributes(
                AudioAttributes.Builder()
                    .setUsage(AudioAttributes.USAGE_MEDIA)
                    .setContentType(AudioAttributes.CONTENT_TYPE_SPEECH)
                    .build()
            )
            .setAudioFormat(
                AudioFormat.Builder()
                    .setSampleRate(sampleRate)
                    .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                    .setChannelMask(AudioFormat.CHANNEL_OUT_MONO)
                    .build()
            )
            .setBufferSizeInBytes(maxOf(bufferSize, pcmData.size))
            .setTransferMode(AudioTrack.MODE_STATIC)
            .build()

        audioTrack?.let { track ->
            track.write(amplified, 0, amplified.size)
            track.setNotificationMarkerPosition(amplified.size / 2) // 16-bit = 2 bytes/sample
            track.setPlaybackPositionUpdateListener(object :
                AudioTrack.OnPlaybackPositionUpdateListener {
                override fun onMarkerReached(t: AudioTrack) {
                    _isPlaying.value = false
                    Log.i(TAG, "Playback complete")
                }
                override fun onPeriodicNotification(t: AudioTrack) {}
            })

            _isPlaying.value = true
            track.play()

            val durationMs = (amplified.size.toLong() * 1000) / (sampleRate * 2)
            Log.i(TAG, "Playing ${amplified.size} bytes at ${sampleRate}Hz (${durationMs}ms), gain=${softwareGain}x")
        }
    }

    /** 峰值正規化:增益 = min(80%FS/峰值, ×32),整段一致、不削頂 */
    private fun normalizePcm(input: ByteArray): ByteArray {
        var peak = 1
        val scan = java.nio.ByteBuffer.wrap(input).order(java.nio.ByteOrder.LITTLE_ENDIAN)
        while (scan.remaining() >= 2) {
            val v = scan.short.toInt()
            val a = if (v < 0) -v else v
            if (a > peak) peak = a
        }
        val gain = (26214f / peak).coerceAtMost(128f)  // -48dB 源也拉得回來
        Log.i(TAG, "normalizePcm: peak=$peak gain=x%.1f".format(gain))
        if (gain <= 1.05f) return input

        val output = ByteArray(input.size)
        val bb = java.nio.ByteBuffer.wrap(input).order(java.nio.ByteOrder.LITTLE_ENDIAN)
        val out = java.nio.ByteBuffer.wrap(output).order(java.nio.ByteOrder.LITTLE_ENDIAN)
        while (bb.remaining() >= 2) {
            val s = (bb.short.toInt() * gain).toInt().coerceIn(-32768, 32767)
            out.putShort(s.toShort())
        }
        return output
    }

    /** Amplify 16-bit PCM data by a given multiplier, with clipping */
    private fun amplifyPcm(input: ByteArray, gain: Int): ByteArray {
        if (gain <= 1) return input
        val output = ByteArray(input.size)
        val bb = java.nio.ByteBuffer.wrap(input).order(java.nio.ByteOrder.LITTLE_ENDIAN)
        val out = java.nio.ByteBuffer.wrap(output).order(java.nio.ByteOrder.LITTLE_ENDIAN)
        while (bb.remaining() >= 2) {
            val sample = bb.short.toInt() * gain
            val clamped = sample.coerceIn(-32768, 32767)
            out.putShort(clamped.toShort())
        }
        return output
    }

    /** Start streaming playback — AudioTrack in MODE_STREAM */
    fun startStreamPlayback(sampleRate: Int) {
        stopStreamPlayback()

        val bufferSize = AudioTrack.getMinBufferSize(
            sampleRate,
            AudioFormat.CHANNEL_OUT_MONO,
            AudioFormat.ENCODING_PCM_16BIT
        )

        streamTrack = AudioTrack.Builder()
            .setAudioAttributes(
                AudioAttributes.Builder()
                    .setUsage(AudioAttributes.USAGE_MEDIA)
                    .setContentType(AudioAttributes.CONTENT_TYPE_SPEECH)
                    .build()
            )
            .setAudioFormat(
                AudioFormat.Builder()
                    .setSampleRate(sampleRate)
                    .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                    .setChannelMask(AudioFormat.CHANNEL_OUT_MONO)
                    .build()
            )
            .setBufferSizeInBytes(bufferSize * 4)
            .setTransferMode(AudioTrack.MODE_STREAM)
            .build()

        streamTrack?.play()
        _isStreamPlaying.value = true
        Log.i(TAG, "Stream playback started at ${sampleRate}Hz, buf=${bufferSize * 4}")
    }

    /** Write a chunk of PCM data to the streaming AudioTrack */
    fun writeStreamData(pcmData: ByteArray) {
        val track = streamTrack ?: return
        val amplified = amplifyPcm(pcmData, softwareGain)
        track.write(amplified, 0, amplified.size)
    }

    /** Stop streaming playback */
    fun stopStreamPlayback() {
        streamTrack?.let { track ->
            if (track.playState == AudioTrack.PLAYSTATE_PLAYING) {
                track.stop()
            }
            track.release()
        }
        streamTrack = null
        _isStreamPlaying.value = false
    }

    /**
     * Save PCM data as a WAV file in the Downloads folder.
     * Applies software gain before saving.
     * Returns the file path on success, null on failure.
     */
    fun saveAsWav(context: Context, pcmData: ByteArray, sampleRate: Int): String? {
        if (pcmData.isEmpty()) {
            Log.w(TAG, "No audio data to save")
            return null
        }

        val amplified = normalizePcm(pcmData)
        val channels = 1
        val bitsPerSample = 16
        val byteRate = sampleRate * channels * bitsPerSample / 8
        val blockAlign = channels * bitsPerSample / 8
        val dataSize = amplified.size
        val fileSize = 36 + dataSize

        // WAV header (44 bytes)
        val header = ByteBuffer.allocate(44).order(ByteOrder.LITTLE_ENDIAN).apply {
            put("RIFF".toByteArray())           // ChunkID
            putInt(fileSize)                     // ChunkSize
            put("WAVE".toByteArray())           // Format
            put("fmt ".toByteArray())           // Subchunk1ID
            putInt(16)                           // Subchunk1Size (PCM)
            putShort(1)                          // AudioFormat (1 = PCM)
            putShort(channels.toShort())         // NumChannels
            putInt(sampleRate)                   // SampleRate
            putInt(byteRate)                     // ByteRate
            putShort(blockAlign.toShort())       // BlockAlign
            putShort(bitsPerSample.toShort())    // BitsPerSample
            put("data".toByteArray())           // Subchunk2ID
            putInt(dataSize)                     // Subchunk2Size
        }.array()

        return try {
            val timestamp = SimpleDateFormat("yyyyMMdd_HHmmss", Locale.US).format(Date())
            val fileName = "IPRO_MIC_${timestamp}.wav"
            val downloadsDir = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS)
            val file = File(downloadsDir, fileName)

            FileOutputStream(file).use { fos ->
                fos.write(header)
                fos.write(amplified)
            }

            Log.i(TAG, "WAV saved: ${file.absolutePath} (${dataSize + 44} bytes)")
            file.absolutePath
        } catch (e: Exception) {
            Log.e(TAG, "Failed to save WAV: ${e.message}")
            null
        }
    }

    fun stop() {
        audioTrack?.let { track ->
            if (track.playState == AudioTrack.PLAYSTATE_PLAYING) {
                track.stop()
            }
            track.release()
        }
        audioTrack = null
        _isPlaying.value = false
        stopStreamPlayback()
    }
}

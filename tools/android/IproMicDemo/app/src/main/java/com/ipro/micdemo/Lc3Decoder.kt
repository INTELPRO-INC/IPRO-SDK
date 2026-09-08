package com.ipro.micdemo

import android.util.Log
import java.nio.ByteBuffer
import java.nio.ByteOrder

/**
 * JNI wrapper for LC3 decoder.
 * Decodes LC3 compressed audio frames to 16-bit PCM.
 */
class Lc3Decoder {
    private var handle: Long = 0

    /** Initialize decoder. Returns true on success. */
    fun setup(frameDurationUs: Int = 10000, sampleRate: Int = 16000): Boolean {
        handle = nativeInit(frameDurationUs, sampleRate)
        if (handle == 0L) {
            Log.e(TAG, "LC3 decoder init failed")
            return false
        }
        Log.i(TAG, "LC3 decoder initialized")
        return true
    }

    /** Decode LC3 frame to PCM. Returns PCM ByteArray (little-endian int16) or null. */
    fun decodeToPcmBytes(lc3Data: ByteArray): ByteArray? {
        if (handle == 0L) return null
        val shorts = nativeDecode(handle, lc3Data, 0) ?: return null
        // Convert short[] to byte[] (little-endian)
        val bb = ByteBuffer.allocate(shorts.size * 2).order(ByteOrder.LITTLE_ENDIAN)
        for (s in shorts) bb.putShort(s)
        return bb.array()
    }

    /** Release decoder resources. */
    fun release() {
        if (handle != 0L) {
            nativeDestroy(handle)
            handle = 0
        }
    }

    private external fun nativeInit(frameDurationUs: Int, sampleRate: Int): Long
    private external fun nativeDecode(handle: Long, lc3Data: ByteArray, outputSamples: Int): ShortArray?
    private external fun nativeDestroy(handle: Long)

    companion object {
        private const val TAG = "Lc3Decoder"
        init {
            System.loadLibrary("lc3decoder")
        }
    }
}

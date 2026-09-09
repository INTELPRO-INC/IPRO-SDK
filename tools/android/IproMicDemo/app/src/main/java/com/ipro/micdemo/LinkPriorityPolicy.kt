package com.ipro.micdemo

internal object LinkPriorityPolicy {
    fun shouldDowngradeAfterAudio(recordingRequested: Boolean): Boolean =
        !recordingRequested

    fun recordingRequestedForStatus(
        state: Int,
        recording: Int,
        chunkReady: Int,
        idle: Int,
        recorded: Int,
        current: Boolean,
    ): Boolean = when (state) {
        recording, chunkReady -> true
        idle, recorded -> false
        else -> current
    }
}

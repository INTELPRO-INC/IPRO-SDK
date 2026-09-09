package com.ipro.micdemo

import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class LinkPriorityPolicyTest {
    @Test
    fun `continuous recording keeps the link at high priority between chunks`() {
        assertFalse(LinkPriorityPolicy.shouldDowngradeAfterAudio(recordingRequested = true))
    }

    @Test
    fun `idle device returns to low power after audio transfer`() {
        assertTrue(LinkPriorityPolicy.shouldDowngradeAfterAudio(recordingRequested = false))
    }

    @Test
    fun `reconnecting to an active recording restores high priority policy`() {
        assertTrue(
            LinkPriorityPolicy.recordingRequestedForStatus(
                state = 1, recording = 1, chunkReady = 6,
                idle = 0, recorded = 2, current = false,
            )
        )
    }
}

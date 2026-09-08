package com.ipro.micdemo.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.ipro.micdemo.AudioPlayer
import com.ipro.micdemo.BleManager
import com.ipro.micdemo.BleManager.ConnectionState

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MainScreen(
    bleManager: BleManager,
    audioPlayer: AudioPlayer,
    onRequestPermissions: () -> Unit
) {
    val connectionState by bleManager.connectionState.collectAsState()
    val statusText by bleManager.statusText.collectAsState()
    val deviceStatus by bleManager.deviceStatus.collectAsState()
    val transferProgress by bleManager.audioTransferProgress.collectAsState()
    val audioReady by bleManager.audioReady.collectAsState()
    val isPlaying by audioPlayer.isPlaying.collectAsState()
    val isBleStreaming by bleManager.isStreaming.collectAsState()
    val isStreamPlaying by audioPlayer.isStreamPlaying.collectAsState()
    val bondState by bleManager.bondState.collectAsState()
    val pairedAddress by bleManager.pairedAddress.collectAsState()
    val autoReconnect by bleManager.autoReconnect.collectAsState()
    val linkInfo by bleManager.linkInfo.collectAsState()

    var duration by remember { mutableIntStateOf(3) }
    var gainDb by remember { mutableFloatStateOf(0f) }

    val isConnected = connectionState == ConnectionState.CONNECTED

    Scaffold(
        topBar = {
            TopAppBar(
                title = { Text("IPRO Mic Demo") },
                colors = TopAppBarDefaults.topAppBarColors(
                    containerColor = MaterialTheme.colorScheme.primaryContainer
                )
            )
        }
    ) { paddingValues ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(paddingValues)
                .padding(16.dp),
            verticalArrangement = Arrangement.spacedBy(16.dp)
        ) {
            // Connection status
            Card(modifier = Modifier.fillMaxWidth()) {
                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(16.dp),
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.spacedBy(12.dp)
                ) {
                    // Status indicator dot
                    Box(
                        modifier = Modifier
                            .size(12.dp)
                            .background(
                                color = when (connectionState) {
                                    ConnectionState.CONNECTED -> Color(0xFF4CAF50)
                                    ConnectionState.SCANNING,
                                    ConnectionState.CONNECTING -> Color(0xFFFFC107)
                                    ConnectionState.DISCONNECTED -> Color(0xFFF44336)
                                },
                                shape = CircleShape
                            )
                    )
                    Column(modifier = Modifier.weight(1f)) {
                        Text(
                            text = when (connectionState) {
                                ConnectionState.CONNECTED -> "Connected"
                                ConnectionState.SCANNING -> "Scanning..."
                                ConnectionState.CONNECTING -> "Connecting..."
                                ConnectionState.DISCONNECTED -> "Disconnected"
                            },
                            fontWeight = FontWeight.Bold
                        )
                        Text(
                            text = if (isConnected && linkInfo.isNotEmpty()) "$statusText · $linkInfo" else statusText,
                            fontSize = 12.sp,
                            color = MaterialTheme.colorScheme.onSurfaceVariant
                        )
                        Text(
                            text = when (bondState) {
                                BleManager.BondState.BONDED ->
                                    "Paired · ${pairedAddress ?: "?"}"
                                BleManager.BondState.BONDING -> "Pairing..."
                                BleManager.BondState.NONE -> "Not paired"
                            },
                            fontSize = 12.sp,
                            color = when (bondState) {
                                BleManager.BondState.BONDED -> Color(0xFF4CAF50)
                                else -> MaterialTheme.colorScheme.onSurfaceVariant
                            }
                        )
                    }
                    if (connectionState != ConnectionState.DISCONNECTED) {
                        OutlinedButton(onClick = { bleManager.disconnect() }) {
                            Text(if (isConnected) "Disconnect" else "Cancel")
                        }
                    } else {
                        Button(
                            onClick = {
                                onRequestPermissions()
                                /*
                                 * Once paired, go straight to the device.
                                 * reconnectPaired() leaves the request pending
                                 * in the Android stack, so a device that is
                                 * still asleep on its power button is picked up
                                 * the moment it advertises again - which a
                                 * ten-second scan window would usually miss.
                                 */
                                if (!bleManager.reconnectPaired()) {
                                    bleManager.startScan()
                                }
                            },
                            enabled = connectionState == ConnectionState.DISCONNECTED
                        ) {
                            Text(if (pairedAddress != null) "Reconnect" else "Connect")
                        }
                    }
                }
            }

            if (pairedAddress != null) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    OutlinedButton(
                        onClick = {
                            onRequestPermissions()
                            bleManager.startScan()
                        },
                        enabled = connectionState == ConnectionState.DISCONNECTED,
                        modifier = Modifier.weight(1f)
                    ) {
                        Text("Scan instead")
                    }
                    OutlinedButton(
                        onClick = { bleManager.forgetDevice() },
                        modifier = Modifier.weight(1f)
                    ) {
                        Text("Forget")
                    }
                }
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Switch(
                        checked = autoReconnect,
                        onCheckedChange = { bleManager.setAutoReconnect(it) }
                    )
                    Spacer(Modifier.width(8.dp))
                    Column {
                        Text("Auto-reconnect", fontSize = 14.sp)
                        Text(
                            "Re-arm the connection whenever the device drops it, " +
                            "so it comes back by itself after sleeping",
                            fontSize = 11.sp,
                            color = MaterialTheme.colorScheme.onSurfaceVariant
                        )
                    }
                }
                Text(
                    text = "Forget clears the pairing on this phone only. " +
                           "Run `bond clear` on the device too, or it will keep " +
                           "a key this phone no longer has.",
                    fontSize = 11.sp,
                    color = MaterialTheme.colorScheme.onSurfaceVariant
                )
            }

            // Recording controls
            Card(modifier = Modifier.fillMaxWidth()) {
                Column(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(16.dp),
                    verticalArrangement = Arrangement.spacedBy(12.dp)
                ) {
                    Text("Recording", fontWeight = FontWeight.Bold, fontSize = 16.sp)

                    // Duration selector
                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        Text("Duration: ${duration}s", modifier = Modifier.width(100.dp))
                        Slider(
                            value = duration.toFloat(),
                            onValueChange = { duration = it.toInt() },
                            valueRange = 1f..30f,
                            steps = 28,
                            modifier = Modifier.weight(1f),
                            enabled = isConnected
                        )
                    }

                    // Gain slider
                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        Text("Gain: ${gainDb.toInt()} dB", modifier = Modifier.width(100.dp))
                        Slider(
                            value = gainDb,
                            onValueChange = {
                                gainDb = it
                                if (isConnected) bleManager.setGain(it.toInt())
                            },
                            valueRange = -18f..18f,
                            steps = 11,
                            modifier = Modifier.weight(1f),
                            enabled = isConnected
                        )
                    }

                    // Record / Stop buttons
                    Row(
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        Button(
                            onClick = { bleManager.startRecording(duration) },
                            enabled = isConnected && deviceStatus.state == 0,
                            modifier = Modifier.weight(1f),
                            colors = ButtonDefaults.buttonColors(
                                containerColor = Color(0xFFD32F2F)
                            )
                        ) {
                            Text("Record")
                        }
                        OutlinedButton(
                            onClick = { bleManager.stopRecording() },
                            enabled = isConnected && deviceStatus.state == 1,
                            modifier = Modifier.weight(1f)
                        ) {
                            Text("Stop")
                        }
                        Button(
                            onClick = { bleManager.requestStatus() },
                            enabled = isConnected,
                            modifier = Modifier.weight(1f)
                        ) {
                            Text("Status")
                        }
                    }
                }
            }

            // Audio transfer and playback
            Card(modifier = Modifier.fillMaxWidth()) {
                Column(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(16.dp),
                    verticalArrangement = Arrangement.spacedBy(12.dp)
                ) {
                    Text("Audio", fontWeight = FontWeight.Bold, fontSize = 16.sp)

                    // 2M 斷音驗證：對應裝置端 shell `pdm_seq on`
                    val seqDebug by bleManager.seqDebug.collectAsState()
                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.SpaceBetween,
                        modifier = Modifier.fillMaxWidth()
                    ) {
                        Column(modifier = Modifier.weight(1f)) {
                            Text("序號校正（2M 亂序修復）", fontSize = 14.sp)
                            Text(
                                "預設開啟，對應韌體序號模式（pdm_seq）",
                                fontSize = 11.sp,
                                color = MaterialTheme.colorScheme.onSurfaceVariant
                            )
                        }
                        Switch(
                            checked = seqDebug,
                            onCheckedChange = { bleManager.setSeqDebug(it) }
                        )
                    }

                    // Transfer progress
                    if (transferProgress > 0f && transferProgress < 1f) {
                        LinearProgressIndicator(
                            progress = transferProgress,
                            modifier = Modifier.fillMaxWidth()
                        )
                        Text(
                            text = "${(transferProgress * 100).toInt()}%",
                            fontSize = 12.sp,
                            color = MaterialTheme.colorScheme.onSurfaceVariant
                        )
                    }

                    Row(
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        Button(
                            onClick = { bleManager.requestAudio() },
                            enabled = isConnected,
                            modifier = Modifier.weight(1f)
                        ) {
                            Text("Get Audio")
                        }
                        Button(
                            onClick = {
                                if (isPlaying) {
                                    audioPlayer.stop()
                                } else {
                                    audioPlayer.play(
                                        bleManager.getAudioData(),
                                        bleManager.getAudioSampleRate()
                                    )
                                }
                            },
                            enabled = audioReady || isPlaying,
                            modifier = Modifier.weight(1f),
                            colors = ButtonDefaults.buttonColors(
                                containerColor = if (isPlaying)
                                    MaterialTheme.colorScheme.error
                                else
                                    MaterialTheme.colorScheme.primary
                            )
                        ) {
                            Text(if (isPlaying) "Stop Play" else "Play")
                        }
                    }

                    // Save WAV button
                    val context = androidx.compose.ui.platform.LocalContext.current
                    var saveResult by remember { mutableStateOf<String?>(null) }
                    Button(
                        onClick = {
                            val path = audioPlayer.saveAsWav(
                                context,
                                bleManager.getAudioData(),
                                bleManager.getAudioSampleRate()
                            )
                            saveResult = if (path != null) "Saved: ${path.substringAfterLast('/')}" else "Save failed"
                        },
                        enabled = audioReady && !isPlaying,
                        modifier = Modifier.fillMaxWidth()
                    ) {
                        Text("Save as WAV")
                    }
                    if (saveResult != null) {
                        Text(
                            text = saveResult!!,
                            fontSize = 12.sp,
                            color = MaterialTheme.colorScheme.onSurfaceVariant
                        )
                    }
                }
            }

            // Live streaming
            Card(modifier = Modifier.fillMaxWidth()) {
                Column(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(16.dp),
                    verticalArrangement = Arrangement.spacedBy(12.dp)
                ) {
                    Text("Live Stream", fontWeight = FontWeight.Bold, fontSize = 16.sp)
                    if (isBleStreaming) {
                        Text(
                            "Streaming...",
                            color = Color(0xFF4CAF50),
                            fontSize = 14.sp
                        )
                    }
                    Button(
                        onClick = {
                            if (isBleStreaming) {
                                bleManager.stopStreaming()
                                audioPlayer.stopStreamPlayback()
                            } else {
                                // Wire up stream data callback
                                bleManager.onStreamData = { pcm ->
                                    audioPlayer.writeStreamData(pcm)
                                }
                                audioPlayer.startStreamPlayback(16000)
                                bleManager.startStreaming()
                            }
                        },
                        enabled = isConnected && !isPlaying &&
                                  deviceStatus.state != 1, // not recording
                        modifier = Modifier.fillMaxWidth(),
                        colors = ButtonDefaults.buttonColors(
                            containerColor = if (isBleStreaming)
                                MaterialTheme.colorScheme.error
                            else
                                Color(0xFF4CAF50)
                        )
                    ) {
                        Text(if (isBleStreaming) "Stop Stream" else "Stream")
                    }
                }
            }

            // Device info
            if (isConnected) {
                Card(modifier = Modifier.fillMaxWidth()) {
                    Column(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(16.dp),
                        verticalArrangement = Arrangement.spacedBy(4.dp)
                    ) {
                        Text("Device Info", fontWeight = FontWeight.Bold, fontSize = 16.sp)
                        val stateStr = when (deviceStatus.state) {
                            0 -> "Idle"
                            1 -> "Recording"
                            2 -> "Sending"
                            3 -> "Streaming"
                            else -> "Unknown"
                        }
                        Text("State: $stateStr", fontSize = 14.sp)
                        Text("Recorded: ${deviceStatus.recordedBytes} bytes", fontSize = 14.sp)
                        Text(
                            "Duration: ${"%.1f".format(
                                deviceStatus.recordedBytes.toFloat() /
                                    (deviceStatus.sampleRate * 2)
                            )}s",
                            fontSize = 14.sp
                        )
                    }
                }
            }
        }
    }
}

package com.intelpro.evkbench

import android.Manifest
import android.os.Build
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.compose.setContent
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent { MaterialTheme(colorScheme = benchColors()) { BenchScreen() } }
    }
}

@Composable
private fun benchColors(): ColorScheme {
    val dark = androidx.compose.foundation.isSystemInDarkTheme()
    // Signal amber against blue-biased instrument greys, rather than the stock
    // Material purple - it reads as test equipment and keeps green/red free to
    // mean pass and fail.
    return if (dark) darkColorScheme(
        primary = Color(0xFFE8871A),
        onPrimary = Color(0xFF1A1006),
        surface = Color(0xFF181C22),
        onSurface = Color(0xFFE4E8EE),
        surfaceVariant = Color(0xFF13171C),
        onSurfaceVariant = Color(0xFF98A1AD),
        background = Color(0xFF101317),
        onBackground = Color(0xFFE4E8EE),
        outline = Color(0xFF2A3138),
    ) else lightColorScheme(
        primary = Color(0xFFC26A10),
        onPrimary = Color(0xFFFFFFFF),
        surface = Color(0xFFFFFFFF),
        onSurface = Color(0xFF171A1F),
        surfaceVariant = Color(0xFFF6F7F9),
        onSurfaceVariant = Color(0xFF5B636F),
        background = Color(0xFFEEF0F3),
        onBackground = Color(0xFF171A1F),
        outline = Color(0xFFD6DAE1),
    )
}

private val blePerms: Array<String>
    get() = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S)
        arrayOf(Manifest.permission.BLUETOOTH_SCAN, Manifest.permission.BLUETOOTH_CONNECT)
    else
        arrayOf(Manifest.permission.ACCESS_FINE_LOCATION)

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun BenchScreen() {
    val ctx = LocalContext.current
    val ble = remember { BleClient(ctx) }
    var pendingConnect by remember { mutableStateOf(false) }

    val ask = rememberLauncherForActivityResult(
        ActivityResultContracts.RequestMultiplePermissions()
    ) { granted ->
        if (granted.values.all { it }) {
            if (pendingConnect) ble.connect()
        } else {
            ble.append(
                "Bluetooth permission was declined. Nearby-devices access is " +
                "what lets the app find the board; grant it in Settings and " +
                "press Connect again.\n"
            )
        }
        pendingConnect = false
    }

    val ready = ble.state == BleClient.State.Ready
    val busy = ble.state == BleClient.State.Scanning ||
               ble.state == BleClient.State.Connecting

    Scaffold(
        topBar = {
            TopAppBar(
                title = {
                    Column {
                        Text("Bench Console", fontWeight = FontWeight.SemiBold, fontSize = 17.sp)
                        Text(
                            ble.deviceName ?: "IPRO7AI EVK",
                            fontFamily = FontFamily.Monospace, fontSize = 11.sp,
                            color = MaterialTheme.colorScheme.onSurfaceVariant,
                            maxLines = 1, overflow = TextOverflow.Ellipsis
                        )
                    }
                },
                actions = {
                    StateChip(ble.state)
                    Spacer(Modifier.width(8.dp))
                    if (ready) {
                        TextButton(onClick = { ble.disconnect() }) { Text("Disconnect") }
                    } else {
                        Button(
                            onClick = { pendingConnect = true; ask.launch(blePerms) },
                            enabled = !busy
                        ) { Text("Connect") }
                    }
                    Spacer(Modifier.width(6.dp))
                }
            )
        }
    ) { pad ->
        Column(
            Modifier.padding(pad).fillMaxSize().verticalScroll(rememberScrollState())
                .padding(horizontal = 14.dp).padding(bottom = 20.dp),
            verticalArrangement = Arrangement.spacedBy(12.dp)
        ) {
            Spacer(Modifier.height(2.dp))
            Console(ble, ready)
            Panels(ble, ready)
        }
    }
}

@Composable
private fun StateChip(state: BleClient.State) {
    val (label, tone) = when (state) {
        BleClient.State.Idle       -> "idle" to MaterialTheme.colorScheme.onSurfaceVariant
        BleClient.State.Scanning   -> "scanning" to MaterialTheme.colorScheme.primary
        BleClient.State.Connecting -> "connecting" to MaterialTheme.colorScheme.primary
        BleClient.State.Ready      -> "connected" to Color(0xFF3E9E6B)
        BleClient.State.Failed     -> "failed" to MaterialTheme.colorScheme.error
    }
    Surface(shape = RoundedCornerShape(100), color = tone.copy(alpha = 0.13f)) {
        Text(
            label, color = tone,
            fontFamily = FontFamily.Monospace, fontSize = 12.sp,
            modifier = Modifier.padding(horizontal = 10.dp, vertical = 5.dp)
        )
    }
}

@Composable
private fun Card(title: String, tag: String, content: @Composable ColumnScope.() -> Unit) {
    Surface(
        shape = RoundedCornerShape(10.dp),
        color = MaterialTheme.colorScheme.surface,
        border = androidx.compose.foundation.BorderStroke(1.dp, MaterialTheme.colorScheme.outline),
        modifier = Modifier.fillMaxWidth()
    ) {
        Column {
            Row(
                Modifier.fillMaxWidth()
                    .background(MaterialTheme.colorScheme.surfaceVariant)
                    .padding(horizontal = 14.dp, vertical = 10.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(title, fontWeight = FontWeight.SemiBold, fontSize = 14.sp)
                Spacer(Modifier.weight(1f))
                Text(
                    tag.uppercase(), fontFamily = FontFamily.Monospace, fontSize = 10.sp,
                    color = MaterialTheme.colorScheme.onSurfaceVariant
                )
            }
            Column(
                Modifier.padding(14.dp),
                verticalArrangement = Arrangement.spacedBy(9.dp),
                content = content
            )
        }
    }
}

@Composable
private fun Cmd(ble: BleClient, enabled: Boolean, label: String, cmd: String) {
    OutlinedButton(onClick = { ble.send(cmd) }, enabled = enabled) {
        Text(label, fontSize = 13.sp, maxLines = 1)
    }
}

@Composable
private fun Note(text: String) {
    Text(
        text, fontSize = 12.5.sp, lineHeight = 18.sp,
        color = MaterialTheme.colorScheme.onSurfaceVariant
    )
}

@OptIn(ExperimentalLayoutApi::class)
@Composable
private fun Panels(ble: BleClient, on: Boolean) {

    Card("Link", "bt") {
        FlowRow(horizontalArrangement = Arrangement.spacedBy(7.dp)) {
            Cmd(ble, on, "Status", "bt status")
            Cmd(ble, on, "RF cal", "bt rf")
            Cmd(ble, on, "Audio", "bt audio")
            Cmd(ble, on, "Advertise", "bt adv on")
        }
        Note(
            "Check RF cal first whenever anything streams badly — roscal must " +
            "read 3. Anything else means the receiver is uncalibrated, and the " +
            "packet loss that follows imitates every other fault."
        )
    }

    var clip by remember { mutableStateOf("3") }
    Card("Video player", "vid") {
        OutlinedTextField(
            value = clip, onValueChange = { clip = it.filter(Char::isDigit).take(1) },
            label = { Text("clip") }, singleLine = true, enabled = on,
            modifier = Modifier.width(110.dp)
        )
        FlowRow(horizontalArrangement = Arrangement.spacedBy(7.dp)) {
            Button(onClick = { ble.send("vid play $clip") }, enabled = on) { Text("Play") }
            Cmd(ble, on, "Stop", "vid stop")
            Cmd(ble, on, "Status", "vid status")
            Cmd(ble, on, "Loop on", "vid loop on")
            Cmd(ble, on, "Memory", "vid mem")
        }
        Note(
            "Play the largest clip first. The payload buffer is claimed once " +
            "and never resized, so whichever clip loads first fixes the " +
            "reservation for the session — start small and every larger clip fails."
        )
    }

    var hz by remember { mutableStateOf("1000") }
    var gain by remember { mutableFloatStateOf(0f) }
    Card("Speaker", "spk") {
        Row(verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.spacedBy(9.dp)) {
            OutlinedTextField(
                value = hz, onValueChange = { hz = it.filter(Char::isDigit).take(5) },
                label = { Text("Hz") }, singleLine = true, enabled = on,
                modifier = Modifier.width(130.dp)
            )
            Button(onClick = { ble.send("spk tone $hz 2 30") }, enabled = on) { Text("Tone") }
        }
        Text("gain ${gain.toInt()} dB", fontFamily = FontFamily.Monospace, fontSize = 12.sp)
        Slider(
            value = gain, onValueChange = { gain = it },
            onValueChangeFinished = { ble.send("spk gain ${gain.toInt()}") },
            valueRange = -60f..24f, enabled = on
        )
        FlowRow(horizontalArrangement = Arrangement.spacedBy(7.dp)) {
            Cmd(ble, on, "Status", "spk status")
            Cmd(ble, on, "Stop", "spk stop")
            Cmd(ble, on, "Amp on", "spk on")
            Cmd(ble, on, "Amp off", "spk off")
        }
        Note(
            "A clean tone proves the converter, amplifier and analogue path. " +
            "Clean tone with rough streamed audio puts the fault upstream."
        )
    }

    Card("Microphone", "mic") {
        FlowRow(horizontalArrangement = Arrangement.spacedBy(7.dp)) {
            Cmd(ble, on, "Record 3 s", "mic rec 3")
            Cmd(ble, on, "Play back", "mic play")
            Cmd(ble, on, "Status", "mic status")
            Cmd(ble, on, "Probe pins", "mic probe")
            Cmd(ble, on, "Pad owners", "mic pads")
        }
        Note(
            "Pad ownership is worth reading: the AON bits sit across two " +
            "registers, and checking only the first reports the pins as free " +
            "while the input buffer is still off."
        )
    }

    Card("Camera", "cam") {
        FlowRow(horizontalArrangement = Arrangement.spacedBy(7.dp)) {
            Cmd(ble, on, "Probe", "cam probe")
            Cmd(ble, on, "Start", "cam start")
            Cmd(ble, on, "Stop", "cam stop")
            Cmd(ble, on, "Pixels", "cam pixels")
        }
        Note(
            "Zero frames and zero errors together means no pixel data arrives " +
            "at all. With SCCB answering, look at the DVP side — MCLK shares " +
            "GPIO41 with the boot strap, so a jumper left on J6 silences the " +
            "sensor clock while I²C keeps working."
        )
    }

    Card("Storage and power", "sd · adc") {
        FlowRow(horizontalArrangement = Arrangement.spacedBy(7.dp)) {
            Cmd(ble, on, "Mount", "sd mount")
            Cmd(ble, on, "Status", "sd status")
            Cmd(ble, on, "List", "sd ls sd:/")
            Cmd(ble, on, "Read test", "sd bench sd:/3.ipv")
            Cmd(ble, on, "Supply", "vbat")
            Cmd(ble, on, "Panel bars", "lcd bars")
        }
        Note(
            "The read test reads a file straight through with nothing else " +
            "running. It separates a card that cannot sustain the rate from " +
            "one that only fails while the panel is also being driven."
        )
    }
}

@Composable
private fun Console(ble: BleClient, on: Boolean) {
    var line by remember { mutableStateOf("") }
    val scroll = rememberScrollState()

    LaunchedEffect(ble.log) { scroll.animateScrollTo(scroll.maxValue) }

    Card("Console", "shell") {
        Surface(
            color = MaterialTheme.colorScheme.surfaceVariant,
            shape = RoundedCornerShape(7.dp),
            modifier = Modifier.fillMaxWidth().height(280.dp)
        ) {
            Text(
                ble.log.ifEmpty {
                    "Run `bt init` on the board once so it advertises, then " +
                    "press Connect.\n"
                },
                fontFamily = FontFamily.Monospace, fontSize = 11.5.sp, lineHeight = 16.sp,
                modifier = Modifier.verticalScroll(scroll).padding(11.dp)
            )
        }
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp),
            verticalAlignment = Alignment.CenterVertically) {
            OutlinedTextField(
                value = line, onValueChange = { line = it },
                placeholder = { Text("any shell command") },
                singleLine = true, enabled = on, modifier = Modifier.weight(1f),
                keyboardOptions = KeyboardOptions(imeAction = ImeAction.Send),
                keyboardActions = KeyboardActions(onSend = {
                    if (line.isNotBlank()) { ble.send(line.trim()); line = "" }
                })
            )
            Button(
                onClick = { if (line.isNotBlank()) { ble.send(line.trim()); line = "" } },
                enabled = on
            ) { Text("Send") }
        }
        TextButton(onClick = { ble.clearLog() }) { Text("Clear") }
    }
}

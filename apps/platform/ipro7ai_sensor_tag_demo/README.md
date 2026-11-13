# ipro7ai_sensor_tag_demo — the sensors of the IPRO7AI Sensor Tag board

A small application for the IPRO7AI Sensor Tag board: it reads the board's sensors and
prints them over the board's own USB port, twenty times a second, as one line of
`name:value` pairs. IPRO Workbench shows that line as a live dashboard; any serial
terminal, spreadsheet or script reads it as well.

| On the board | Part | Pins |
|---|---|---|
| 6-axis IMU | ISM330BX | SPI by GPIO: CLK 31, MISO 32, MOSI 33, CS 34 |
| Fuel gauge | CW2015 | I2C0: SDA 29, SCL 30 |
| Button | SW9 | GPIO 10, pressed = low |
| LEDs | D2 red, D4 green | GPIO 26, GPIO 27, on = low |
| Buzzer | passive, 1 kHz | GPIO 2 (PWM0 channel 2) |
| Console | USB CDC | the board's USB connector |

No probe is needed: the console, the shell and the firmware download all use the
board's USB connector.

## 1. Build

```bash
make -C apps/platform/ipro7ai_sensor_tag_demo
```

You should see the memory table and
`build/build_out/ipro7ai_sensor_tag_demo_IPRO7.bin` (about 100 KB).

In IPRO Workbench: select the application `ipro7ai_sensor_tag_demo` and press Build.

## 2. Flash over USB

The board's boot2 has a USB downloader. A firmware with the SDK shell (this one,
and the firmware the board ships with) enters it with the shell command
`reboot_usb`.

```bash
ls /dev/cu.usbmodem*                      # find the board's port, e.g. /dev/cu.usbmodem11201
python3 tools/serial_monitor.py /dev/cu.usbmodem11201 --send reboot_usb --timeout 3
python3 tools/ipro_dl.py flash apps/platform/ipro7ai_sensor_tag_demo/build/build_out/ipro7ai_sensor_tag_demo_IPRO7.bin \
    --wrap --addr 0x12000 --boot --wait-rescue 20
```

The first command ends with `Device not configured` and `TIMEOUT after 3s`: the
board has left, and `ls /dev/cu.usbmodem*` now shows the downloader's port
(`/dev/cu.usbmodem000000011`) in place of the board's. The second prints
`rescue handshake OK`, erases, writes, prints `verify OK`, and the board starts
again on its first port. If the port does not come back by itself, unplug the
USB cable and plug it in again.

If the board prints nothing at all (a firmware without the USB console), unplug
it, start the `flash` command with `--wait-rescue 60`, and plug it in: boot2
listens for the downloader for a moment at every power-on.

## 3. See the sensors

### In IPRO Workbench

1. IPRO: Select Port, and pick the board's port (it is listed as a USB serial
   port, not as a probe).
2. IPRO: Open Monitor. You should see lines such as

   ```
   ax:0.012 ay:-0.004 az:1.003 gx:0.3 gy:-0.1 gz:0.0 temp:27.4 batt:87 vbat:4.012 btn:0
   ```

3. IPRO: Sensor Dashboard. You should see three groups: Acceleration and Rotation,
   each with three values and a chart, and Board with the temperature, the battery
   and the button. Tilt the board and the Tilt figure follows it; press the button
   and the Button lamp turns on (the red LED on the board lights as well).
4. The buttons above the groups send commands to the board: Beep sounds the
   buzzer, the LED buttons switch the green LED.

`dashboard.yaml` in this folder says how each value is shown (group, unit, range,
decimals, chart, tilt, buttons). Change it and press Read dashboard.yaml Again.

IPRO: Serial Plotter draws the same lines as one chart.

### In a terminal

```bash
python3 tools/serial_monitor.py /dev/cu.usbmodem11201
```

## Shell commands

| Command | What it does |
|---|---|
| `stream on` / `stream off` | starts or stops the sensor lines |
| `rate <1..100>` | lines a second (20 at power-on) |
| `led red on|off`, `led green on|off` | an LED (the red one also follows the button) |
| `beep [ms]` | a 1 kHz tone, 200 ms unless told otherwise |

## What each value is

| Name | Unit | From |
|---|---|---|
| `ax` `ay` `az` | g | ISM330BX accelerometer, ±4 g |
| `gx` `gy` `gz` | degrees per second | ISM330BX gyroscope, ±500 dps |
| `temp` | °C | the IMU's own temperature sensor |
| `batt` | % | CW2015 state of charge |
| `vbat` | V | CW2015 cell voltage |
| `btn` | 0 or 1 | the button |

A value is left out of the line when its sensor did not answer, so a board
without a battery still shows the IMU.

## Files

| File | What is in it |
|---|---|
| `main.c` | the sensor task, the line format, the shell commands |
| `tag_board.c`, `tag_board.h` | the pins, the LEDs, the button, the buzzer |
| `drivers/` | ISM330BX, the GPIO SPI bus it uses, CW2015 |
| `dashboard.yaml` | the layout of the Sensor Dashboard |

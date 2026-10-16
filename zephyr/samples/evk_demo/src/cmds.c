/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Shell front end: "evk lcd ...", "evk mic ...", "evk spk ...", "evk cam ...".
 */
#include "demo.h"
#include <stdlib.h>

static uint16_t parse_color(const char *s)
{
	static const struct { const char *n; uint16_t c; } tab[] = {
		{"black", C_BLACK}, {"white", C_WHITE}, {"red", C_RED}, {"green", C_GREEN},
		{"blue", C_BLUE}, {"yellow", C_YELLOW}, {"cyan", C_CYAN}, {"magenta", C_MAGENTA},
	};
	for (size_t i = 0; i < ARRAY_SIZE(tab); i++) {
		if (strcmp(s, tab[i].n) == 0) {
			return tab[i].c;
		}
	}
	return (uint16_t)strtoul(s, NULL, 0);
}

static int cmd_lcd_fill(const struct shell *sh, size_t argc, char **argv)
{
	int r = lcd_fill(argc > 1 ? parse_color(argv[1]) : C_BLACK);

	shell_print(sh, "fill: %d", r);
	return r;
}

static int cmd_lcd_bars(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc); ARG_UNUSED(argv);
	int r = lcd_bars();

	shell_print(sh, "bars (white yellow cyan green magenta red blue black): %d", r);
	return r;
}

static int cmd_lcd_checker(const struct shell *sh, size_t argc, char **argv)
{
	int r = lcd_checker(argc > 1 ? (uint16_t)strtoul(argv[1], NULL, 0) : 16);

	shell_print(sh, "checker: %d", r);
	return r;
}

static int cmd_lcd_gradient(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc); ARG_UNUSED(argv);
	int r = lcd_gradient();

	shell_print(sh, "gradient (red across, green down, blue inverse): %d", r);
	return r;
}

static int cmd_lcd_bl(const struct shell *sh, size_t argc, char **argv)
{
	bool on = argc < 2 || strcmp(argv[1], "off") != 0;
	int r = lcd_backlight(on);

	shell_print(sh, "backlight %s: %d", on ? "on" : "off", r);
	return r;
}

static int cmd_lcd_info(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc); ARG_UNUSED(argv);
	shell_print(sh, "panel %ux%u RGB565, ST7789P3 over SPI0 (SCLK15 MOSI16 CS10 DC17 RST14), backlight GPIO12",
		    lcd_width(), lcd_height());
	return 0;
}

static int cmd_mic_level(const struct shell *sh, size_t argc, char **argv)
{
	return audio_mic_level(sh, argc > 1 ? strtoul(argv[1], NULL, 0) : 5, true);
}

static int cmd_mic_rec(const struct shell *sh, size_t argc, char **argv)
{
	return audio_mic_record(sh, argc > 1 ? strtoul(argv[1], NULL, 0) : 3);
}

static int cmd_mic_play(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc); ARG_UNUSED(argv);
	return audio_play_recording(sh);
}

static int cmd_mic_dump(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc); ARG_UNUSED(argv);
	return audio_mic_dump(sh);
}

static int cmd_spk_gain(const struct shell *sh, size_t argc, char **argv)
{
	int shift = argc > 1 ? (int)strtoul(argv[1], NULL, 0) : 2;
	int r = audio_set_gain_shift(shift);

	shell_print(sh, "playback gain x%d: %d", 1 << shift, r);
	return r;
}

static int cmd_mic_loop(const struct shell *sh, size_t argc, char **argv)
{
	return audio_loopback(sh, argc > 1 ? strtoul(argv[1], NULL, 0) : 10);
}

static int cmd_spk_tone(const struct shell *sh, size_t argc, char **argv)
{
	uint32_t hz = argc > 1 ? strtoul(argv[1], NULL, 0) : 1000;
	uint32_t ms = argc > 2 ? strtoul(argv[2], NULL, 0) : 1000;

	return audio_tone(sh, hz, ms);
}

#ifdef CONFIG_EVK_DEMO_CAMERA
static int cmd_cam_start(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc); ARG_UNUSED(argv);
	return cam_start(sh);
}
static int cmd_cam_stop(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc); ARG_UNUSED(argv);
	return cam_stop(sh);
}
static int cmd_cam_snap(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc); ARG_UNUSED(argv);
	return cam_snap(sh);
}
static int cmd_cam_fps(const struct shell *sh, size_t argc, char **argv)
{
	return cam_set_fps(sh, argc > 1 ? strtoul(argv[1], NULL, 0) : 15);
}
static int cmd_cam_mirror(const struct shell *sh, size_t argc, char **argv)
{
	return cam_set_mirror(sh, argc < 2 || strcmp(argv[1], "off") != 0);
}
static int cmd_cam_pixels(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc); ARG_UNUSED(argv);
	return cam_pixels(sh);
}
static int cmd_cam_status(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc); ARG_UNUSED(argv);
	shell_print(sh, "camera %s", cam_running() ? "running" : "stopped");
	return 0;
}
SHELL_STATIC_SUBCMD_SET_CREATE(sub_cam,
	SHELL_CMD(start, NULL, "Live preview on the LCD", cmd_cam_start),
	SHELL_CMD(stop, NULL, "Stop the preview", cmd_cam_stop),
	SHELL_CMD(snap, NULL, "Start and report after 1.5 s", cmd_cam_snap),
	SHELL_CMD(status, NULL, "Running or not", cmd_cam_status),
	SHELL_CMD(pixels, NULL, "Dump pixels of the newest ISP frame", cmd_cam_pixels),
	SHELL_CMD_ARG(fps, NULL, "fps <1..30> - ISP output rate (next start)", cmd_cam_fps, 1, 1),
	SHELL_CMD_ARG(mirror, NULL, "mirror on|off - horizontal flip (default on)", cmd_cam_mirror, 1, 1),
	SHELL_SUBCMD_SET_END);
#endif

SHELL_STATIC_SUBCMD_SET_CREATE(sub_lcd,
	SHELL_CMD(info, NULL, "Panel info", cmd_lcd_info),
	SHELL_CMD_ARG(fill, NULL, "fill [color|0xRGB565]", cmd_lcd_fill, 1, 1),
	SHELL_CMD(bars, NULL, "Colour bars", cmd_lcd_bars),
	SHELL_CMD_ARG(checker, NULL, "checker [cell px]", cmd_lcd_checker, 1, 1),
	SHELL_CMD(gradient, NULL, "RGB gradient", cmd_lcd_gradient),
	SHELL_CMD_ARG(bl, NULL, "bl on|off", cmd_lcd_bl, 1, 1),
	SHELL_SUBCMD_SET_END);

SHELL_STATIC_SUBCMD_SET_CREATE(sub_mic,
	SHELL_CMD_ARG(level, NULL, "level [seconds] - RMS meter (also on the LCD)", cmd_mic_level, 1, 1),
	SHELL_CMD_ARG(rec, NULL, "rec [seconds<=20] - record to PSRAM", cmd_mic_rec, 1, 1),
	SHELL_CMD(play, NULL, "Play the recording on the speaker", cmd_mic_play),
	SHELL_CMD(dump, NULL, "Print samples around block boundaries", cmd_mic_dump),
	SHELL_CMD_ARG(loop, NULL, "loop [seconds] - live mic -> speaker", cmd_mic_loop, 1, 1),
	SHELL_SUBCMD_SET_END);

SHELL_STATIC_SUBCMD_SET_CREATE(sub_spk,
	SHELL_CMD_ARG(tone, NULL, "tone [hz] [ms]", cmd_spk_tone, 1, 2),
	SHELL_CMD_ARG(gain, NULL, "gain <shift 0..5> - playback gain 2^shift", cmd_spk_gain, 1, 1),
	SHELL_SUBCMD_SET_END);

SHELL_STATIC_SUBCMD_SET_CREATE(sub_evk,
	SHELL_CMD(lcd, &sub_lcd, "LCD patterns", NULL),
	SHELL_CMD(mic, &sub_mic, "PDM microphone", NULL),
	SHELL_CMD(spk, &sub_spk, "Speaker (GPDAC)", NULL),
#ifdef CONFIG_EVK_DEMO_CAMERA
	SHELL_CMD(cam, &sub_cam, "Camera", NULL),
#endif
	SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(evk, &sub_evk, "IPRO7AI EVK demo", NULL);

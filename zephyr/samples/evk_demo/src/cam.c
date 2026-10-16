/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * SC101IOT DVP camera -> ISP (DVP_AS, scaler channel 1, ARGB) -> PSRAM
 * ping-pong -> RGB565 -> LCD, the pipeline of apps/platform/ipro7ai_evk_demo
 * cam.c on Zephyr. The board init already gave the sensor MCLK and a reset
 * pulse; start() repeats the reset with the MM blocks held in reset (the
 * SDK's fix for the ISP colour order), probes the sensor over SCCB, loads
 * its register table and runs the ISP into two PSRAM frames.
 *
 * The ISP firmware's AE/AWB (components/isp_firmware img_proc, FreeRTOS
 * tasks) is not built here: exposure and white balance stay at the
 * sensor's power-on values.
 */
#include "demo.h"
#include <ipro_zephyr/ipro_hal_glue.h>
#include <string.h>

#include "ipro7.h"
#include "drv_glb.h"
#include "drv_mm_misc.h"
#include "hal_isp.h"
#include "camera.h"
#include "sensor.h"
#include "sccb.h"

#define PREVIEW_W 320
#define PREVIEW_H 180              /* 16:9 letterboxed on the 320x240 landscape panel */
#define PREVIEW_Y ((240 - PREVIEW_H) / 2)
#define FRAME_BYTES (PREVIEW_W * PREVIEW_H * 4u)
#define CONVERGE_FRAMES 6
static uint8_t fps = 15; /* ISP channel-1 frame-rate control, source 30 fps */
static bool mirror = true; /* horizontal flip while repacking (the module faces the user) */


/* ISP channel-1 ping-pong output, ARGB, in PSRAM. */
__attribute__((section(".psram_bss"))) __aligned(32) static uint32_t frames[2][PREVIEW_W * PREVIEW_H];
/* ARGB (PSRAM) -> wire-order RGB565 strips in OCRAM, one DMA write per strip. */
#define STRIP_ROWS 12
static uint16_t strip[2][PREVIEW_W * STRIP_ROWS] __aligned(32);
static int64_t t_start;

static const rt_camera_desc *desc;
static volatile bool running;
static volatile uint32_t latest;
static volatile uint32_t last_frame; /* newest ISP frame address, for "cam pixels" */
static volatile uint32_t converge;
static struct { uint32_t frames, drawn, dropped, fifo_err, vsync_err, hsync_err, blit_err; int last_err; } st;
static K_SEM_DEFINE(frame_sem, 0, 1);
static K_THREAD_STACK_DEFINE(preview_stack, 2048);
static struct k_thread preview_thread;
static k_tid_t preview_tid;

void ipro7ai_evk_cam_mclk_pinmux(void);
void ipro7ai_evk_cam_mclk_clock(void);
void ipro7ai_evk_cam_reset(int assert);
void ipro7ai_evk_cam_dvp_pinmux(void);

/* The ISP firmware would register its 3A statistics hook here (not built). */
int ipro_isp_stats_ready_config(void *cb, void *usrdata)
{
	ARG_UNUSED(cb); ARG_UNUSED(usrdata);
	return 0;
}

static void isp_cb(isp_out_frame_info_t info)
{
	switch (info.event) {
	case ISP_HAL_INT_FIFO_OVERWRITE: st.fifo_err++; return;
	case ISP_HAL_INT_VSYNC_CNT_ERROR: st.vsync_err++; return;
	case ISP_HAL_INT_HSYNC_CNT_ERROR: st.hsync_err++; return;
	case ISP_HAL_INT_NORMAL: break;
	default: return;
	}
	if (info.out_port != ISP_CHANNEL_1) {
		return;
	}
	st.frames++;
	if (converge) {
		converge--;
		return;
	}
	if (latest) {
		st.dropped++;
	}
	latest = info.out_addr;
	last_frame = info.out_addr;
	k_sem_give(&frame_sem);
}

static inline uint16_t pack_be(uint32_t p)
{
	/* ISP ch1 ARGB: byte 0 = R, 1 = G, 2 = B (after the MM reset); RGB565 big-endian */
	uint16_t c = RGB565(p & 0xFF, (p >> 8) & 0xFF, (p >> 16) & 0xFF);

	return (uint16_t)((c >> 8) | (c << 8));
}

static void blit_frame(uint32_t addr)
{
	const uint32_t *px = (const uint32_t *)(uintptr_t)addr;
	bool be = lcd_big_endian();
	int which = 0;

	ipro_hal_cache_invalidate(addr, FRAME_BYTES);
	for (uint16_t y0 = 0; y0 < PREVIEW_H; y0 += STRIP_ROWS) {
		uint16_t rows = MIN(STRIP_ROWS, PREVIEW_H - y0);
		uint16_t *dst = strip[which];

		for (uint16_t y = 0; y < rows; y++) {
			const uint32_t *src = px + (uint32_t)(y0 + y) * PREVIEW_W;
			uint16_t *d = dst + (uint32_t)y * PREVIEW_W;

			if (mirror) {
				for (uint16_t x = 0; x < PREVIEW_W; x++) {
					uint16_t c = pack_be(src[x]);

					d[PREVIEW_W - 1 - x] = be ? c : (uint16_t)((c >> 8) | (c << 8));
				}
			} else {
				for (uint16_t x = 0; x < PREVIEW_W; x++) {
					uint16_t c = pack_be(src[x]);

					d[x] = be ? c : (uint16_t)((c >> 8) | (c << 8));
				}
			}
		}
		int r = lcd_write_raw(0, PREVIEW_Y + y0, PREVIEW_W, rows, dst, y0 + rows >= PREVIEW_H);

		if (r != 0) {
			st.blit_err++;
			st.last_err = r;
		}
		which ^= 1;
	}
	st.drawn++;
}

static void preview_fn(void *a, void *b, void *c)
{
	ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);
	while (running) {
		if (k_sem_take(&frame_sem, K_MSEC(500)) != 0) {
			continue;
		}
		uint32_t addr = latest;

		latest = 0;
		if (addr && running) {
			blit_frame(addr);
		}
	}
}

static void mm_reset(void)
{
	MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP_AS, 1);
	MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP_AS, 0);
	MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP2AXI_C, 1);
	MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP2AXI_C, 0);
	MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_ISP, 1);
	MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_ISP, 0);
}

int cam_init(void)
{
	return 0;
}

bool cam_running(void) { return running; }

static int isp_up(const struct shell *sh)
{
	isp_hal_cfg_t cfg = {0};

	cfg.isp_src = ISP_HAL_SRC_DVP_AS;
	cfg.in_size.width = desc->width;
	cfg.in_size.height = desc->height;
	cfg.src_cfg.dvp_input.hsync = ISP_HAL_DVP_ACTIVE_HIGH;
	cfg.src_cfg.dvp_input.vsync = ISP_HAL_DVP_ACTIVE_HIGH;
	cfg.src_cfg.dvp_input.order = ISP_HAL_DATA_BYTE_LOWER;
	cfg.src_cfg.dvp_input.pclk = desc->pclk;
	cfg.src_cfg.dvp_input.totalHPixels = desc->total_width;
	cfg.glb_crop.width = desc->width;
	cfg.glb_crop.height = desc->height;
	cfg.out_ch_0.enable = 0;
	cfg.out_ch_1.enable = 1;
	cfg.out_ch_1.scaler_cfg.in.width = desc->width;
	cfg.out_ch_1.scaler_cfg.in.height = desc->height;
	cfg.out_ch_1.scaler_cfg.out.width = PREVIEW_W;
	cfg.out_ch_1.scaler_cfg.out.height = PREVIEW_H;
	cfg.out_ch_1.format = ISP_HAL_OUT_FMT1_ARGB;
	cfg.out_ch_1.chn_common.fr_ctrl.src_fps = 30;
	cfg.out_ch_1.chn_common.fr_ctrl.dst_fps = fps;
	cfg.out_ch_1.chn_common.addr[0] = (uint32_t)(uintptr_t)frames[0];
	cfg.out_ch_1.chn_common.addr[1] = (uint32_t)(uintptr_t)frames[1];
	cfg.out_ch_1.chn_common.pp_num = 2;
	cfg.out_ch_1.chn_common.out_crop.width = PREVIEW_W;
	cfg.out_ch_1.chn_common.out_crop.height = PREVIEW_H;
	cfg.out_callback = isp_cb;

	int r = isp_init(&cfg);

	if (r != 0) {
		shell_error(sh, "isp_init: %d", r);
		return -EIO;
	}
	for (int i = 0; i < 2; i++) {
		ipro_hal_cache_clean((uint32_t)(uintptr_t)frames[i], FRAME_BYTES);
		ipro_hal_cache_invalidate((uint32_t)(uintptr_t)frames[i], FRAME_BYTES);
	}
	/* PSRAM write/read command arbitration for the DVP2AXI-C master. */
	MM_MISC_Set_IPROAIPRO_Rcmd_Mode(MM_MISC_IPROAIPRO_MODE_PROACTIVE);
	MM_MISC_Set_IPROAIPRO_Rcmd_MasterEn(0, 0, 1);
	MM_MISC_Set_IPROAIPRO_Wcmd_Mode(MM_MISC_IPROAIPRO_MODE_PROACTIVE);
	MM_MISC_Set_IPROAIPRO_Wcmd_MasterEn(0, 0, 1);
	return isp_start() == 0 ? 0 : -EIO;
}

int cam_start(const struct shell *sh)
{
	if (running) {
		shell_print(sh, "already running");
		return 0;
	}
	memset(&st, 0, sizeof(st));
	latest = 0;
	t_start = k_uptime_get();
	converge = CONVERGE_FRAMES;

	/* Sensor in reset while the MM blocks are reset, MCLK on, release. */
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);
	ipro7ai_evk_cam_reset(1);
	mm_reset();
	ipro7ai_evk_cam_mclk_pinmux();
	ipro7ai_evk_cam_dvp_pinmux();
	ipro7ai_evk_cam_mclk_clock();
	k_msleep(2);
	ipro7ai_evk_cam_reset(0);
	k_msleep(20);

	if (SCCB_Init() != 0) {
		shell_error(sh, "i2c0 not ready");
		return -ENODEV;
	}
	desc = camera_module_init();
	if (desc == NULL) {
		shell_error(sh, "no sensor answered on SCCB (J6 pin 3-7 wired? FPC seated?)");
		return -ENODEV;
	}
	shell_print(sh, "sensor %s @0x%02x, %ux%u, pclk %u", desc->name, desc->addr, desc->width,
		    desc->height, desc->pclk);
	camera_module_start(desc); /* register table (720p, DVP) */

	running = true;
	shell_print(sh, "lcd clear: %d", lcd_fill(C_BLACK));
	preview_tid = k_thread_create(&preview_thread, preview_stack,
				      K_THREAD_STACK_SIZEOF(preview_stack), preview_fn, NULL, NULL,
				      NULL, K_LOWEST_APPLICATION_THREAD_PRIO, 0, K_NO_WAIT);
	k_thread_name_set(preview_tid, "cam_preview");
	int r = isp_up(sh);

	if (r != 0) {
		running = false;
		return r;
	}
	shell_print(sh, "preview %ux%u at %u fps on the LCD (ISP ch1 ARGB -> RGB565)", PREVIEW_W,
		    PREVIEW_H, fps);
	return 0;
}

int cam_stop(const struct shell *sh)
{
	if (!running) {
		shell_print(sh, "not running");
		return 0;
	}
	running = false;
	k_sem_give(&frame_sem);
	k_thread_join(preview_tid, K_MSEC(1000));
	(void)isp_deinit();
	ipro7ai_evk_cam_reset(1); /* hold the sensor in reset */
	{
		int64_t ms = MAX(k_uptime_get() - t_start, 1);

		shell_print(sh, "stopped after %lld ms: %u frames (%u.%u fps), %u drawn (%u.%u fps), "
			    "%u dropped, errors fifo %u vsync %u hsync %u, blit errors %u (last %d)",
			    ms, st.frames, (uint32_t)(st.frames * 1000 / ms),
			    (uint32_t)(st.frames * 10000 / ms % 10), st.drawn,
			    (uint32_t)(st.drawn * 1000 / ms), (uint32_t)(st.drawn * 10000 / ms % 10),
			    st.dropped, st.fifo_err, st.vsync_err, st.hsync_err, st.blit_err,
			    st.last_err);
	}
	return 0;
}

int cam_snap(const struct shell *sh)
{
	int r = running ? 0 : cam_start(sh);

	if (r != 0) {
		return r;
	}
	k_msleep(1500);
	shell_print(sh, "%u frames so far, %u drawn, errors fifo %u vsync %u hsync %u", st.frames,
		    st.drawn, st.fifo_err, st.vsync_err, st.hsync_err);
	return 0;
}

int cam_set_fps(const struct shell *sh, uint32_t n)
{
	if (n < 1 || n > 30) {
		return -EINVAL;
	}
	fps = (uint8_t)n;
	shell_print(sh, "fps %u (takes effect at the next start)", fps);
	return 0;
}

int cam_set_mirror(const struct shell *sh, bool on)
{
	mirror = on;
	shell_print(sh, "mirror %s", on ? "on" : "off");
	return 0;
}

/* A few ARGB pixels from the newest ISP frame (centre row), as the SDK's "cam pixels". */
int cam_pixels(const struct shell *sh)
{
	uint32_t addr = last_frame;

	if (addr == 0) {
		shell_error(sh, "no frame yet");
		return -ENODATA;
	}
	const uint32_t *px = (const uint32_t *)(uintptr_t)addr;
	uint32_t sum = 0;

	ipro_hal_cache_invalidate(addr, FRAME_BYTES);
	shell_print(sh, "frame @%08x, row %d from x=156:", addr, PREVIEW_H / 2);
	for (int i = 0; i < 8; i++) {
		shell_fprintf(sh, SHELL_NORMAL, " %08x", px[(PREVIEW_H / 2) * PREVIEW_W + 156 + i]);
	}
	for (uint32_t i = 0; i < PREVIEW_W * PREVIEW_H; i += 97) {
		sum += px[i] & 0x00FFFFFF;
	}
	shell_print(sh, "\nsampled sum of RGB bytes: %u (0 = all black)", sum);
	return 0;
}

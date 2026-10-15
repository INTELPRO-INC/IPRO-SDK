/**
 * @file reef_hw.c
 * @brief IPRO7AI Reef hardware behind the BLE remote services (see reef_hw.h)
 *
 * The drivers are the reef demo's (apps/platform/ipro7ai_reef_demo/src:
 * rgb_led, keys, audio, cam, sdcard), linked from that directory rather than
 * copied. This file adds the remote's behaviour on top: request queue, LED
 * animation, JPEG capture, motion detection, temperature and level meters.
 *
 * Copyright (c) 2026 INTELPRO
 */

#include "reef_hw.h"
#include "reef_jpeg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "board_config.h"
#include "rgb_led.h"
/* by path: the Zephyr host's subsys/bluetooth/host/keys.h is on the include path too */
#include REEF_DEMO_KEYS_H
#include "audio.h"
#include "cam.h"
#include "sdcard.h"
#include "ff.h"
#include "hal_adc.h"
#include "drv_adc.h"
#include <system_soc.h>
#include "ipro_log.h"

#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#define LOG_TAG                 "REEF_HW"

#define HW_TASK_STACK_WORDS     2048
#define HW_TASK_PRIORITY        4       /* below the BLE host threads (6) */
#define HW_QUEUE_LEN            8
#define HW_TICK_MS              50u

#define LED_MAX_LEVEL           96u     /* WS2812 at 255 is blinding */
#define LED_SLOW_PERIOD_MS      1000u
#define LED_FAST_PERIOD_MS      250u

#define TEMP_PERIOD_MS          2000u
#define TSEN_DEFAULT_OFFSET     1310u   /* ipro7_demo tsen_demo.c, when eFuse has none */
#define TSEN_WAIT_LOOPS         200000u

#define CAP_SETTLE_FRAMES       10u     /* AE/AWB after a cold start */
#define CAM_STALL_MS            2000u   /* streaming but no new frame: restart */
#define CAP_TIMEOUT_MS          3000u
#define MM_TUNE_FIRST           0x30040000u     /* MM_MISC */
#define MM_TUNE_LAST            0x3004FFFCu     /* end of GAMMA */
#define CAP_JPEG_QUALITY        70u     /* 720p q70: ~50-100 KB, about 1-1.5 s over BLE */
#define CAP_BUF_SIZE            (192u * 1024u)
#define SD_DIR                  "sd:/REMOTE"
#ifndef CAM_SCALE_DIV
#define CAM_SCALE_DIV           4u      /* cam.c's default */
#endif

#define MOTION_PERIOD_MS        200u
#define MOTION_HOLD_MS          3000u
#define MOTION_GRID_X           16u
#define MOTION_GRID_Y           9u
#define MOTION_BLOCK_DELTA      16      /* luma levels, after removing global change */
#define MOTION_NOISE_MULT       3       /* and this many times the block's own noise */
#define MOTION_BLOCKS_MIN       3u
#define MOTION_LEARN_CHECKS     15u     /* 3 s of background before reporting */

/* One-off jobs go through the queue; state the phone sets (LED, motion,
 * meter) is a "wanted" value the worker picks up on its next tick, so a full
 * queue can never lose a setting. */
typedef enum {
    REQ_CAPTURE = 0,
    REQ_TONE,
    REQ_SENSOR_REG,     /* `reef reg`: read, or write when .write */
} req_type_t;

typedef struct {
    uint8_t type;
    union {
        bool on;
        struct { uint16_t hz; uint16_t ms; uint8_t vol; } tone;
        struct { uint16_t addr; uint8_t val; bool write; } reg;
    } u;
} req_t;

typedef struct {
    bool led_on;
    uint8_t led_bright;
    uint8_t led_mode;
    uint8_t rgb[3];
    bool motion;
    bool meter;
} wanted_t;

static struct {
    reef_hw_callbacks_t cb;
    QueueHandle_t q;
    TaskHandle_t task;
    /* LED */
    bool led_on;
    uint8_t led_bright;
    uint8_t led_mode;
    uint8_t rgb[3];
    uint8_t shown[3];
    bool shown_valid;
    /* camera */
    bool cam_ok;
    bool streaming;
    uint8_t jpeg_q;                 /* capture quality, `reef jpegq` */
    uint8_t jpeg_q_live;            /* quality the encoder was created with */
    uint32_t cam_restarts;
    uint32_t last_frames;
    uint32_t last_frame_ms;
    bool motion_on;
    bool motion_active;
    uint32_t motion_last_ms;
    uint32_t motion_next_ms;
    /* per block, Q4: slow background luma and its usual deviation (lights
     * that flicker against the rolling shutter learn a high noise and stop
     * counting as motion) */
    int16_t bg_q4[MOTION_GRID_X * MOTION_GRID_Y];
    int16_t noise_q4[MOTION_GRID_X * MOTION_GRID_Y];
    uint32_t learn;
    uint32_t motion_peak;           /* most changed blocks in one check, since `reef status` */
    uint32_t motion_hits;           /* consecutive checks over the threshold */
    uint8_t grid_delta[MOTION_GRID_X * MOTION_GRID_Y];  /* last check, for `reef motion map` */
    uint8_t *cap_buf;
    uint32_t captures;
    /* SD */
    bool sd_ok;
    uint32_t sd_next;
    /* sensors */
    volatile int16_t temp_centi;
    uint16_t tsen_offset;
    uint32_t temp_next_ms;
    volatile int8_t sound_dbfs;
    bool meter_on;
    bool meter_running;
    char info[112];
    volatile wanted_t want;         /* written by any task under a critical section */
} s = {
    .led_bright = 100,
    .rgb = { 255, 255, 255 },
    .temp_centi = REEF_HW_TEMP_INVALID,
    .sound_dbfs = REEF_HW_SOUND_INVALID,
    .want = { .led_bright = 100, .rgb = { 255, 255, 255 } },
};

static uint32_t now_ms(void)
{
    return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

static int post(const req_t *r)
{
    if (s.q == NULL || xQueueSend(s.q, r, 0) != pdTRUE) {
        IPRO_LOGW(LOG_TAG, "request %u dropped (worker busy)", r->type);
        return -1;
    }
    return 0;
}

/* ---------------------------------------------------------------------------
 * LED
 * -------------------------------------------------------------------------*/

static void led_update(uint32_t t)
{
    uint8_t want[3] = { 0, 0, 0 };
    bool lit = s.led_on && s.led_bright > 0u;

    if (lit && (s.led_mode == 2u || s.led_mode == 3u)) {        /* blink slow / fast */
        uint32_t period = (s.led_mode == 2u) ? LED_SLOW_PERIOD_MS : LED_FAST_PERIOD_MS;
        lit = (t % period) < (period / 2u);
    }
    if (lit) {
        for (int i = 0; i < 3; i++) {
            want[i] = (uint8_t)(((uint32_t)s.rgb[i] * s.led_bright * LED_MAX_LEVEL) / (255u * 100u));
        }
    }
    if (s.shown_valid && memcmp(want, s.shown, sizeof(want)) == 0) {
        return;
    }
    if (rgb_led_set(want[0], want[1], want[2]) == 0) {
        memcpy(s.shown, want, sizeof(want));
        s.shown_valid = true;
    }
}

/* ---------------------------------------------------------------------------
 * Temperature: TSEN_Get_Temp() in integer arithmetic (the driver's version
 * is float, and this task must not be the second FPU user)
 * -------------------------------------------------------------------------*/

static void tsen_init(void)
{
    adc_device_t adc0 = {
        .id                   = 0,
        .caps                 = COMMON_CAPS_INT_RX,
        .continuous_conv_mode = 0,
        .callback             = NULL,
        .clk_div              = ADC_CLOCK_DIV_32,
        .vref                 = ADC_VREF_3V2,
        .differential_mode    = false,
        .data_width           = ADC_DATA_WIDTH_16B_WITH_256_AVERAGE,
        .fifo_threshold       = ADC_FIFO_THRESHOLD_1BYTE,
        .gain                 = ADC_GAIN_1,
        .rx_dma               = -1,
    };
    uint8_t pos_list[1] = { ADC_CHANNEL_TSEN_P };
    uint8_t neg_list[1] = { ADC_CHANNEL_GND };
    adc_channel_cfg_t ch_cfg = { .pos_channel = pos_list, .neg_channel = neg_list, .num = 1 };
    uint16_t offset = 0;

    hal_adc_setConfig(&adc0);
    hal_adc_init(0);
    hal_adc_ioctl(0, ADC_CTRL_CHANNEL_CONFIG, &ch_cfg);
    hal_adc_ioctl(0, ADC_CTRL_TSEN_ON, NULL);
    hal_adc_trim_tsen(&offset);
    s.tsen_offset = offset ? offset : TSEN_DEFAULT_OFFSET;
}

/* One phase: 16-bit result of a 256-sample average, single-ended. */
static int32_t tsen_phase(bool high)
{
    uint32_t loops = 0;

    ADC_FIFO_Clear();
    if (high) {
        ADC_SET_TSVBE_HIGH();
    } else {
        ADC_SET_TSVBE_LOW();
    }
    ADC_Start();
    while (ADC_Get_FIFO_Count() == 0u) {
        if (++loops > TSEN_WAIT_LOOPS) {
            return -1;
        }
    }
    return (int32_t)(ADC_Read_FIFO() & 0xFFFFu);
}

static void tsen_read(void)
{
    int32_t v0 = tsen_phase(false);
    int32_t v1 = tsen_phase(true);
    int32_t d;

    if (v0 < 0 || v1 < 0) {
        return;
    }
    d = (v1 > v0 ? v1 - v0 : v0 - v1) - (int32_t)s.tsen_offset;
    if (d > 20000 || d < -20000) {          /* nonsense reading; also keeps d * 100000 in range */
        return;
    }
    /* (|v1 - v0| - offset) / 7.753 C, in 0.01 C */
    s.temp_centi = (int16_t)((d * 100000) / 7753);
}

/* ---------------------------------------------------------------------------
 * Microphone level and tones (reef demo audio.c jobs)
 * -------------------------------------------------------------------------*/

/* round(32768 * 10^(dB / 20)) for dB = -96 .. 0 */
static const uint16_t s_db_amp[97] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 3, 3, 3, 4, 4, 5,
    5, 6, 7, 7, 8, 9, 10, 12, 13, 15,
    16, 18, 21, 23, 26, 29, 33, 37, 41, 46,
    52, 58, 65, 73, 82, 92, 104, 116, 130, 146,
    164, 184, 207, 232, 260, 292, 328, 368, 413, 463,
    519, 583, 654, 734, 823, 924, 1036, 1163, 1305, 1464,
    1642, 1843, 2068, 2320, 2603, 2920, 3277, 3677, 4125, 4629,
    5193, 5827, 6538, 7336, 8231, 9235, 10362, 11627, 13045, 14637,
    16423, 18427, 20675, 23198, 26029, 29205, 32768,
};

/* 20 log10(amp / 32768) rounded down, in integer arithmetic (audio.c's own
 * conversion is float, and this task is not that FPU user) */
static int8_t amp_to_dbfs(uint32_t amp)
{
    int lo = 0, hi = 96;

    if (amp < s_db_amp[0]) {
        return -96;
    }
    while (lo < hi) {
        int mid = (lo + hi + 1) / 2;
        if (amp >= s_db_amp[mid]) {
            lo = mid;
        } else {
            hi = mid - 1;
        }
    }
    return (int8_t)(lo - 96);
}

static void meter_service(void)
{
    if (s.meter_running && !audio_busy()) {
        s.sound_dbfs = amp_to_dbfs(audio_last_rms_amp());
        s.meter_running = false;
        if (s.meter_on && s.cb.sound) {
            s.cb.sound(s.sound_dbfs);
        }
    }
    if (s.meter_on && !s.meter_running && !audio_busy()) {
        s.meter_running = (audio_record(AUDIO_IN_PDM, 1) == 0);
    }
}

static void do_tone(uint16_t hz, uint16_t ms, uint8_t vol)
{
    if (audio_busy()) {
        audio_stop();
        for (int i = 0; i < 20 && audio_busy(); i++) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        s.meter_running = false;
    }
    if (audio_tone(hz, ms, vol, AUDIO_ROUTE_SPK) != 0) {
        IPRO_LOGW(LOG_TAG, "tone %u Hz refused", hz);
    }
}

/* ---------------------------------------------------------------------------
 * SD card
 * -------------------------------------------------------------------------*/

static void sd_init(void)
{
    DIR dir;
    FILINFO fi;
    unsigned n;

    if (!sdcard_present() || sdcard_mount() != SD_OK) {
        IPRO_LOGI(LOG_TAG, "no SD memory card - captures stay in RAM only");
        return;
    }
    (void)f_mkdir(SD_DIR);
    s.sd_next = 1;
    if (f_opendir(&dir, SD_DIR) == FR_OK) {
        while (f_readdir(&dir, &fi) == FR_OK && fi.fname[0] != '\0') {
            if (sscanf(fi.fname, "IMG%5u.JPG", &n) == 1 && n >= s.sd_next) {
                s.sd_next = n + 1u;
            }
        }
        (void)f_closedir(&dir);
    }
    s.sd_ok = true;
    IPRO_LOGI(LOG_TAG, "SD card mounted, next capture " SD_DIR "/IMG%05lu.JPG",
              (unsigned long)s.sd_next);
}

static void sd_save(const uint8_t *jpg, uint32_t len)
{
    char path[32];
    FIL f;
    UINT wr = 0;
    FRESULT fr;

    if (!s.sd_ok) {
        return;
    }
    snprintf(path, sizeof(path), SD_DIR "/IMG%05lu.JPG", (unsigned long)s.sd_next);
    fr = f_open(&f, path, FA_CREATE_NEW | FA_WRITE);
    if (fr == FR_OK) {
        fr = f_write(&f, jpg, len, &wr);
        (void)f_close(&f);
    }
    if (fr != FR_OK || wr != len) {
        IPRO_LOGW(LOG_TAG, "%s: write failed (FRESULT %d), SD saving off", path, (int)fr);
        s.sd_ok = false;
        return;
    }
    s.sd_next++;
    IPRO_LOGI(LOG_TAG, "saved %s (%lu bytes)", path, (unsigned long)len);
}

/* ---------------------------------------------------------------------------
 * Camera: stream on demand, JPEG capture, motion
 * -------------------------------------------------------------------------*/

static int stream_on(void)
{
    if (s.streaming && cam_is_running()) {
        return 0;
    }
    if (cam_start() != CAM_OK) {
        IPRO_LOGE(LOG_TAG, "camera start failed");
        return -1;
    }
    s.streaming = true;
    s.learn = 0;
    s.last_frames = 0;
    s.last_frame_ms = now_ms();
    reef_jpeg_deinit();         /* cam_start resets the MM blocks */
    return 0;
}

static void stream_off(void);

/* A restarted DVP stream sometimes delivers no frames; a second start does. */
static int stream_restart(const char *why)
{
    s.cam_restarts++;
    IPRO_LOGW(LOG_TAG, "camera: %s, restarting (#%lu)", why, (unsigned long)s.cam_restarts);
    stream_off();
    return stream_on();
}

static void stream_off(void)
{
    if (!s.streaming) {
        return;
    }
    reef_jpeg_deinit();
    cam_stop();
    cam_park();
    s.streaming = false;
}

/* Newest frame once at least @p frames frames exist; 0 on timeout. */
static uint32_t wait_frame(uint32_t frames)
{
    uint32_t t0 = now_ms();
    uint32_t n = 0;
    uint32_t addr;

    for (;;) {
        cam_service();
        addr = cam_latest_frame(&n);
        if (addr != 0u && n >= frames) {
            return addr;
        }
        if (now_ms() - t0 > CAP_TIMEOUT_MS) {
            return 0;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static int32_t capture_jpeg(void)
{
    uint16_t w, h;
    uint32_t frames = 0, addr;
    int32_t len;

    if (s.cap_buf == NULL) {
        s.cap_buf = pvPortMalloc(CAP_BUF_SIZE);
        if (s.cap_buf == NULL) {
            return -1;
        }
    }
    if (stream_on() != 0) {
        return -1;
    }
    (void)cam_latest_frame(&frames);
    addr = wait_frame(frames < CAP_SETTLE_FRAMES ? CAP_SETTLE_FRAMES : frames + 1u);
    if (addr == 0u && stream_restart("no frames for a capture") == 0) {
        addr = wait_frame(CAP_SETTLE_FRAMES);
    }
    if (addr == 0u) {
        IPRO_LOGE(LOG_TAG, "camera: still no frames");
        return -1;
    }
    cam_out_size(&w, &h);
    if (s.jpeg_q_live != s.jpeg_q) {
        reef_jpeg_deinit();             /* quality is fixed when the encoder is made */
        s.jpeg_q_live = s.jpeg_q;
    }
    if (addr == 0u || reef_jpeg_init(w, h, s.jpeg_q) != 0) {
        return -1;
    }
    len = reef_jpeg_encode(addr, s.cap_buf, CAP_BUF_SIZE);
    if (len < 4 || s.cap_buf[0] != 0xFFu || s.cap_buf[1] != 0xD8u) {
        IPRO_LOGE(LOG_TAG, "encode failed (%ld)", (long)len);
        return -1;
    }
    return len;
}

static void do_capture(void)
{
    uint32_t t0 = now_ms();
    int32_t len = s.cam_ok ? capture_jpeg() : -1;

    /* The stream stays up: a DVP restart after a stop often delivers no
     * frames (the SDK's own camera apps never stop theirs either). */
    if (len > 0) {
        s.captures++;
        IPRO_LOGI(LOG_TAG, "capture #%lu: %ld bytes in %lu ms", (unsigned long)s.captures,
                  (long)len, (unsigned long)(now_ms() - t0));
        sd_save(s.cap_buf, (uint32_t)len);
    }
    if (s.cb.capture_done) {
        s.cb.capture_done(len > 0 ? s.cap_buf : NULL, len > 0 ? (uint32_t)len : 0u);
    }
}

/* Mean luma of a 5 x 5 sample lattice in each grid block (YUYV: Y at even bytes). */
static void motion_grid(uint32_t addr, uint16_t w, uint16_t h, uint8_t *grid)
{
    const uint8_t *f = (const uint8_t *)(uintptr_t)addr;
    const uint32_t bw = w / MOTION_GRID_X, bh = h / MOTION_GRID_Y;

    L1C_DCACHE_INVALID_RANGE((uintptr_t)addr, (uint32_t)w * h * 2u);
    for (uint32_t gy = 0; gy < MOTION_GRID_Y; gy++) {
        for (uint32_t gx = 0; gx < MOTION_GRID_X; gx++) {
            uint32_t sum = 0;

            for (uint32_t j = 0; j < 5u; j++) {
                uint32_t y = gy * bh + (bh * (2u * j + 1u)) / 10u;
                for (uint32_t i = 0; i < 5u; i++) {
                    uint32_t x = (gx * bw + (bw * (2u * i + 1u)) / 10u) & ~1u;
                    sum += f[(y * w + x) * 2u];
                }
            }
            grid[gy * MOTION_GRID_X + gx] = (uint8_t)(sum / 25u);
        }
    }
}

/* Blocks that differ from their background by more than both the fixed
 * delta and a multiple of their own noise; updates the model as it goes. */
static uint32_t motion_changed_blocks(const uint8_t *cur)
{
    const uint32_t n = MOTION_GRID_X * MOTION_GRID_Y;
    int32_t mean_d = 0;
    uint32_t changed = 0;

    if (s.learn == 0u) {
        for (uint32_t i = 0; i < n; i++) {
            s.bg_q4[i] = (int16_t)(cur[i] << 4);
            s.noise_q4[i] = 0;
        }
    }
    for (uint32_t i = 0; i < n; i++) {
        mean_d += ((int32_t)cur[i] << 4) - s.bg_q4[i];
    }
    mean_d /= (int32_t)n;                      /* exposure change, not motion */

    for (uint32_t i = 0; i < n; i++) {
        int32_t d = ((int32_t)cur[i] << 4) - s.bg_q4[i] - mean_d;
        int32_t ad = d < 0 ? -d : d;
        int32_t limit = s.noise_q4[i] * MOTION_NOISE_MULT;

        if (limit < (MOTION_BLOCK_DELTA << 4)) {
            limit = MOTION_BLOCK_DELTA << 4;
        }
        s.grid_delta[i] = (uint8_t)((ad >> 4) > 255 ? 255 : (ad >> 4));
        if (ad > limit && s.learn >= MOTION_LEARN_CHECKS) {
            changed++;
        } else {
            /* only quiet blocks teach the model, so a person standing still
             * is not learned away within one check; fast while learning so a
             * flickering lamp is known to be noisy before reporting starts */
            int shift = (s.learn < MOTION_LEARN_CHECKS) ? 2 : 4;

            s.noise_q4[i] = (int16_t)(s.noise_q4[i] + ((ad - s.noise_q4[i]) >> shift));
        }
        s.bg_q4[i] = (int16_t)(s.bg_q4[i] + ((d + mean_d) >> 3));
    }
    if (s.learn < MOTION_LEARN_CHECKS) {
        s.learn++;
    }
    return changed;
}

static void motion_service(uint32_t t)
{
    uint8_t grid[MOTION_GRID_X * MOTION_GRID_Y];
    uint16_t w, h;
    uint32_t addr, changed;

    if (!s.motion_on || (int32_t)(t - s.motion_next_ms) < 0) {
        return;
    }
    s.motion_next_ms = t + MOTION_PERIOD_MS;
    if (!s.streaming) {
        return;
    }
    {
        uint32_t frames = 0;

        addr = cam_latest_frame(&frames);
        if (frames != s.last_frames) {
            s.last_frames = frames;
            s.last_frame_ms = t;
        } else if (t - s.last_frame_ms > CAM_STALL_MS) {
            (void)stream_restart("stream stalled");
            return;
        }
    }
    if (addr == 0u) {
        return;
    }
    cam_out_size(&w, &h);
    motion_grid(addr, w, h, grid);
    changed = motion_changed_blocks(grid);
    if (changed > s.motion_peak) {
        s.motion_peak = changed;
    }
    /* two checks in a row (200 ms apart), so one noisy frame is not motion */
    s.motion_hits = (changed >= MOTION_BLOCKS_MIN) ? s.motion_hits + 1u : 0u;

    if (s.motion_hits >= 2u) {
        s.motion_last_ms = t;
        if (!s.motion_active) {
            s.motion_active = true;
            IPRO_LOGI(LOG_TAG, "motion (%lu blocks)", (unsigned long)changed);
            if (s.cb.motion) {
                s.cb.motion(true);
            }
        }
    } else if (s.motion_active && t - s.motion_last_ms > MOTION_HOLD_MS) {
        s.motion_active = false;
        if (s.cb.motion) {
            s.cb.motion(false);
        }
    }
}

static void set_motion(bool on)
{
    if (on == s.motion_on) {
        return;
    }
    if (on && (!s.cam_ok || stream_on() != 0)) {
        IPRO_LOGW(LOG_TAG, "motion detection needs the camera");
        s.want.motion = false;                  /* do not retry every tick */
        return;
    }
    s.motion_on = on;
    s.motion_next_ms = now_ms() + 1000u;         /* let AE settle first */
    s.learn = 0;                                 /* new background */
    if (!on) {
        if (s.motion_active && s.cb.motion) {
            s.cb.motion(false);
        }
        s.motion_active = false;
    }
    IPRO_LOGI(LOG_TAG, "motion detection %s", on ? "on" : "off");
}

/* ---------------------------------------------------------------------------
 * Worker
 * -------------------------------------------------------------------------*/

/* Sensor descriptors are named "__rti_name_<chip>" by the camera registry. */
static const char *sensor_name(void)
{
    const char *n = cam_sensor_name();
    static const char prefix[] = "__rti_name_";

    if (n == NULL) {
        return "none";
    }
    return strncmp(n, prefix, sizeof(prefix) - 1u) == 0 ? n + sizeof(prefix) - 1u : n;
}

static void build_info(void)
{
    uint16_t w = 0, h = 0;

    cam_sensor_size(&w, &h);
    snprintf(s.info, sizeof(s.info), "IPRO7AI Reef; camera %s %ux%u; SD %s; build %s %s",
             s.cam_ok ? sensor_name() : "none", (unsigned)(w / CAM_SCALE_DIV),
             (unsigned)(h / CAM_SCALE_DIV), s.sd_ok ? "mounted" : "none", __DATE__, __TIME__);
}

/* Pick up what other tasks asked for since the last tick. */
static void apply_wanted(void)
{
    wanted_t w;

    taskENTER_CRITICAL();
    memcpy(&w, (const void *)&s.want, sizeof(w));
    taskEXIT_CRITICAL();
    s.led_on = w.led_on;
    s.led_bright = w.led_bright;
    s.led_mode = w.led_mode;
    memcpy(s.rgb, w.rgb, sizeof(s.rgb));
    s.meter_on = w.meter;
    if (w.motion != s.motion_on) {
        set_motion(w.motion);
    }
}

static void handle(const req_t *r)
{
    switch (r->type) {
    case REQ_CAPTURE:
        do_capture();
        break;
    case REQ_TONE:
        do_tone(r->u.tone.hz, r->u.tone.ms, r->u.tone.vol);
        break;
    case REQ_SENSOR_REG: {
        int v = r->u.reg.val;

        if (!s.cam_ok) {
            printf("reg: no camera\r\n");
        } else if (r->u.reg.write ? cam_reg_write(r->u.reg.addr, v) : cam_reg_read(r->u.reg.addr, &v)) {
            printf("reg 0x%04x: SCCB error\r\n", r->u.reg.addr);
        } else {
            printf("reg 0x%04x %s 0x%02x\r\n", r->u.reg.addr, r->u.reg.write ? "<-" : "=", v & 0xff);
        }
        break;
    }
    default:
        break;
    }
}

static void hw_task(void *arg)
{
    req_t r;

    (void)arg;
    /* Slow bring-up here, so BLE is not held up by it */
    cam_park();
    s.cam_ok = (cam_probe() == CAM_OK);
    cam_park();
    IPRO_LOGI(LOG_TAG, "camera: %s", s.cam_ok ? sensor_name() : "not found");
    sd_init();
    build_info();

    for (;;) {
        if (xQueueReceive(s.q, &r, pdMS_TO_TICKS(HW_TICK_MS)) == pdTRUE) {
            handle(&r);
        }
        uint32_t t = now_ms();

        apply_wanted();
        cam_service();
        led_update(t);
        motion_service(t);
        meter_service();
        if ((int32_t)(t - s.temp_next_ms) >= 0) {
            s.temp_next_ms = t + TEMP_PERIOD_MS;
            tsen_read();
        }
    }
}

static void key_cb(key_id_t key, bool pressed, void *ctx)
{
    (void)ctx;
    if (s.cb.key) {
        s.cb.key((uint8_t)key, pressed);
    }
}

int reef_hw_init(const reef_hw_callbacks_t *cb)
{
    if (s.task != NULL) {
        return 0;
    }
    if (cb) {
        s.cb = *cb;
    }
    s.jpeg_q = CAP_JPEG_QUALITY;
    s.q = xQueueCreate(HW_QUEUE_LEN, sizeof(req_t));
    if (s.q == NULL) {
        return -1;
    }
    if (rgb_led_init() != 0) {
        IPRO_LOGE(LOG_TAG, "RGB LED init failed");
    }
    if (keys_init() == 0) {
        keys_set_callback(key_cb, NULL);
    }
    if (audio_init() != 0) {
        IPRO_LOGE(LOG_TAG, "audio init failed");
    }
    tsen_init();
    tsen_read();
    snprintf(s.info, sizeof(s.info), "IPRO7AI Reef; starting");
    if (xTaskCreate(hw_task, "reef_hw", HW_TASK_STACK_WORDS, NULL, HW_TASK_PRIORITY,
                    &s.task) != pdPASS) {
        IPRO_LOGE(LOG_TAG, "worker task create failed");
        return -1;
    }
    return 0;
}

void reef_hw_led(bool on, uint8_t brightness, uint8_t mode)
{
    taskENTER_CRITICAL();
    s.want.led_on = on;
    s.want.led_bright = brightness > 100u ? 100u : brightness;
    s.want.led_mode = mode;
    taskEXIT_CRITICAL();
}

void reef_hw_led_color(uint8_t red, uint8_t green, uint8_t blue)
{
    taskENTER_CRITICAL();
    s.want.rgb[0] = red;
    s.want.rgb[1] = green;
    s.want.rgb[2] = blue;
    taskEXIT_CRITICAL();
}

int reef_hw_capture(void)
{
    req_t r = { .type = REQ_CAPTURE };

    if (!s.cam_ok) {
        return -1;
    }
    return post(&r);
}

void reef_hw_motion(bool on)
{
    s.want.motion = on;
}

int reef_hw_tone(uint16_t freq_hz, uint16_t ms, uint8_t volume_pct)
{
    req_t r = { .type = REQ_TONE };

    if (freq_hz < 20u || freq_hz > 12000u || ms == 0u || ms > 10000u || volume_pct > 100u) {
        return -1;
    }
    r.u.tone.hz = freq_hz;
    r.u.tone.ms = ms;
    r.u.tone.vol = volume_pct;
    return post(&r);
}

void reef_hw_sound_meter(bool on)
{
    s.want.meter = on;
}

int16_t reef_hw_temperature_centi(void) { return s.temp_centi; }
int8_t  reef_hw_sound_dbfs(void)        { return s.sound_dbfs; }
bool    reef_hw_camera_ok(void)         { return s.cam_ok; }
bool    reef_hw_sd_ok(void)             { return s.sd_ok; }
const char *reef_hw_info(void)          { return s.info; }

/* ---------------------------------------------------------------------------
 * Shell: `reef`
 * -------------------------------------------------------------------------*/
#ifdef CONFIG_SHELL
static void reef_status(void)
{
    int16_t t = s.temp_centi;

    printf("%s\r\n", s.info);
    printf("LED %s %u%% mode %u colour %u,%u,%u; camera %s%s; motion %s%s; captures %lu, "
           "restarts %lu\r\n",
           s.led_on ? "on" : "off", s.led_bright, s.led_mode, s.rgb[0], s.rgb[1], s.rgb[2],
           s.cam_ok ? "ok" : "none", s.streaming ? " (streaming)" : "",
           s.motion_on ? "on" : "off", s.motion_active ? " (active)" : "",
           (unsigned long)s.captures, (unsigned long)s.cam_restarts);
    if (t == REEF_HW_TEMP_INVALID) {
        printf("chip temperature n/a; ");
    } else {
        printf("chip temperature %d.%02u C; ", t / 100, (unsigned)((t < 0 ? -t : t) % 100));
    }
    printf("mic %d dBFS (meter %s); motion peak %lu blocks since last status\r\n",
           s.sound_dbfs, s.meter_on ? "on" : "off", (unsigned long)s.motion_peak);
    s.motion_peak = 0;
    if (s.streaming) {
        cam_stats_t st;
        uint16_t w = 0, h = 0;

        cam_get_stats(&st);
        cam_out_size(&w, &h);
        printf("camera %ux%u, %lu.%lu fps, %lu frames, errors fifo %lu vsync %lu hsync %lu, "
               "luma %lu, JPEG q%u\r\n", w, h, (unsigned long)(st.fps_x10 / 10u),
               (unsigned long)(st.fps_x10 % 10u), (unsigned long)st.frames,
               (unsigned long)st.fifo_err, (unsigned long)st.vsync_err, (unsigned long)st.hsync_err,
               (unsigned long)st.luma_mean, s.jpeg_q);
    }
}

static int cmd_reef(int argc, char **argv)
{
    if (argc < 2 || strcmp(argv[1], "status") == 0) {
        reef_status();
        printf("usage: reef status | led <r> <g> <b> | tone <hz> <ms> <vol%%> | "
               "motion on|off|map | meter on|off | reg <hex addr> [hex val] | mm <hex addr> [hex val] | "
               "jpegq <1-100>  (photo: remote img capture)\r\n");
    } else if (strcmp(argv[1], "led") == 0 && argc >= 5) {
        reef_hw_led_color((uint8_t)atoi(argv[2]), (uint8_t)atoi(argv[3]), (uint8_t)atoi(argv[4]));
        reef_hw_led(true, s.led_bright ? s.led_bright : 100u, s.led_mode);
    } else if (strcmp(argv[1], "tone") == 0 && argc >= 5) {
        if (reef_hw_tone((uint16_t)atoi(argv[2]), (uint16_t)atoi(argv[3]), (uint8_t)atoi(argv[4])) != 0) {
            printf("tone: 20-12000 Hz, 1-10000 ms, 0-100 %%\r\n");
        }
    } else if (strcmp(argv[1], "snap") == 0) {
        /* a capture belongs to the image service, which owns its state */
        printf("use `remote img capture`\r\n");
    } else if (strcmp(argv[1], "motion") == 0 && argc >= 3 && strcmp(argv[2], "map") == 0) {
        /* last check: '.' quiet, '+' below threshold, '#' counted as motion */
        for (uint32_t y = 0; y < MOTION_GRID_Y; y++) {
            for (uint32_t x = 0; x < MOTION_GRID_X; x++) {
                uint8_t d = s.grid_delta[y * MOTION_GRID_X + x];
                putchar(d > MOTION_BLOCK_DELTA ? '#' : (d > MOTION_BLOCK_DELTA / 2 ? '+' : '.'));
            }
            printf("\r\n");
        }
    } else if (strcmp(argv[1], "motion") == 0 && argc >= 3) {
        reef_hw_motion(strcmp(argv[2], "on") == 0);
    } else if (strcmp(argv[1], "reg") == 0 && argc >= 3) {
        req_t r = { .type = REQ_SENSOR_REG };

        r.u.reg.addr = (uint16_t)strtoul(argv[2], NULL, 16);
        r.u.reg.write = (argc >= 4);
        r.u.reg.val = r.u.reg.write ? (uint8_t)strtoul(argv[3], NULL, 16) : 0u;
        if (post(&r) != 0) {    /* the worker owns the sensor's SCCB bus */
            printf("busy, try again\r\n");
        }
    } else if (strcmp(argv[1], "mm") == 0 && argc >= 3) {
        /* 32-bit access to the multimedia block (MM_MISC .. GAMMA) for ISP
         * tuning; anything else is refused. */
        uint32_t addr = (uint32_t)strtoul(argv[2], NULL, 16);

        if (addr < MM_TUNE_FIRST || addr > MM_TUNE_LAST || (addr & 3u) != 0u) {
            printf("mm: 0x%08lx is outside 0x%08lx-0x%08lx or unaligned\r\n", (unsigned long)addr,
                   (unsigned long)MM_TUNE_FIRST, (unsigned long)MM_TUNE_LAST);
            return -1;
        }
        if (argc >= 4) {
            *(volatile uint32_t *)(uintptr_t)addr = (uint32_t)strtoul(argv[3], NULL, 16);
        }
        printf("mm 0x%08lx = 0x%08lx\r\n", (unsigned long)addr,
               (unsigned long)*(volatile uint32_t *)(uintptr_t)addr);
    } else if (strcmp(argv[1], "jpegq") == 0 && argc >= 3) {
        int q = atoi(argv[2]);

        if (q < 1 || q > 100) {
            printf("jpegq: 1-100\r\n");
            return -1;
        }
        s.jpeg_q = (uint8_t)q;          /* the worker re-creates the encoder on the next capture */
    } else if (strcmp(argv[1], "meter") == 0 && argc >= 3) {
        reef_hw_sound_meter(strcmp(argv[2], "on") == 0);
    } else {
        printf("reef: unknown command\r\n");
        return -1;
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_reef, reef, IPRO7AI Reef board - reef status for usage);
#endif /* CONFIG_SHELL */

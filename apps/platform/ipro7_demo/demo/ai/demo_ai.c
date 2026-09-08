/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "shell.h"
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <event_groups.h>
/* Filesystem - EVFS POSIX API */
#include "evfs.h"
#include "evfs/fatfs_fs.h"
#include "evfs_posix.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <fatfs.h>
#include <fatfs_diskio_register.h>
#include <diskio.h>
#include <hal_sdh.h>
#include <system_soc.h>
#include "drv_glb.h"
#include "hal_npu.h"

#define FATFS_SD_MOUNT_POINT "/sdcard"
static int g_sdcard_mounted = 0;
/**
 * Initialize SDH GPIO pinmux for SD card
 * GPIO28-33 = SDC function, AF mode, drive=2, smt=1
 */
static void sdh_pinmux_init(void)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_UP,
        .drive = 2,
        .smtCtrl = 1,
        .gpioFun = GPIO_FUN_SDC,
    };
    uint8_t pins[] = {
        GLB_GPIO_PIN_28, GLB_GPIO_PIN_29, GLB_GPIO_PIN_30,
        GLB_GPIO_PIN_31, GLB_GPIO_PIN_32, GLB_GPIO_PIN_33,
    };
    for (int i = 0; i < (int)(sizeof(pins) / sizeof(pins[0])); i++) {
        cfg.gpioPin = pins[i];
        /* DAT0 (GPIO30) needs no pull-up */
        cfg.pullType = (pins[i] == GLB_GPIO_PIN_30) ?
                       GPIO_PULL_NONE : GPIO_PULL_UP;
        GLB_GPIO_Init(&cfg);
    }

    /* SDH clock: PLL 96MHz, div=1 → 48MHz */
    GLB_Set_SDH_CLK(1, GLB_SDH_CLK_PLL_96M, 1);
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SDH);
}

/**
 * Mount SD card with EVFS/FatFS support
 * This function can be called multiple times safely - it only mounts once
 * @return 0 on success, -1 on failure
 */
static int mount_sdcard(void)
{
    if (g_sdcard_mounted) {
        return 0;
    }

    printf("Mounting SD card...\r\n");
    sdh_pinmux_init();
    hal_sd_bus_4bits_enable();
    fatfs_sdh_driver_register();
    fatfs_register();

#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_USE_FATFS)
    /* Register FatFS with EVFS */
    int evfs_ret = evfs_register_fatfs("sdcard", DEV_SD, false);
    if (evfs_ret != EVFS_OK) {
        printf("EVFS FatFS registration failed: %d\r\n", evfs_ret);
        /* Continue anyway - basic fatfs may still work */
    } else {
        /* Mount at /sdcard mount point */
        evfs_ret = evfs_posix_mount(FATFS_SD_MOUNT_POINT, "sdcard");
        if (evfs_ret != 0) {
            printf("EVFS mount failed: %d\r\n", evfs_ret);
        } else {
            printf("FatFS mounted at %s via EVFS\r\n", FATFS_SD_MOUNT_POINT);
        }
    }
#endif

    g_sdcard_mounted = 1;
    printf("SD card mounted\r\n");
    return 0;
}

#ifdef CONFIG_AI_SUPPORT
#include <iproai_core.h>
#include <iproai_def.h>
#include "iproai_inst_postprocess.h"
#include "detect/yolov8_person_pet_detect.h"
#include "audio/speaker_recog.h"
#include "audio/fbank.h"     /* AI_MODEL_SPEAKER_RECOG: Kaldi fbank front-end */
#include "audio/gallery.h"   /* AI_MODEL_SPEAKER_RECOG: cosine gallery        */

// Global model handle - only available when AI is enabled
static iproai_model_hdl_t g_model_hdl = NULL;

// Global YOLOv8 configuration
static yolov8_config_t g_yolov8_config;

// Global Speaker Recognition configuration
static speaker_recog_config_t g_speaker_config;
static bool g_speaker_recog_initialized = false;

// 當前模型索引（用於後處理）
static int g_current_model_index = -1;

// 已載入的模型索引（-1 = 尚未載入）
static int g_loaded_model_index = -1;

// Audio 辨識模式
typedef enum {
    AUDIO_MODE_RECOGNIZE = 0,
    AUDIO_MODE_REGISTER,
} audio_mode_e;
static audio_mode_e g_audio_mode = AUDIO_MODE_RECOGNIZE;
static char g_register_speaker_name[SPEAKER_RECOG_MAX_NAME_LEN];
static speaker_recog_result_t speaker_result;

/* ===== AI_MODEL_SPEAKER_RECOG (res2dnet end-to-end: fbank + gallery) ===== */
/* int8 input quant — matches input_int8.csv (verified bit-exact). NOTE: unlike
 * AUDIO_RECOG this uses the TRUE zero_point (-18), NOT a -128-shifted one;
 * res2dnet feature range would saturate to -128 under the legacy shift. */
#define SPK_IN_SCALE    0.11175476f
#define SPK_IN_ZP      (-18)
/* output dequant fallback; the live path reads scale/zp from net_info. */
#define SPK_OUT_SCALE   0.032567f
#define SPK_OUT_ZP     (-4)
/* Debug: 1 = verbose dumps (fbank int8 first/last 20, net structure, raw NPU
 * output, embedding) + write fbank_dump.txt / npu_out.{bin,txt} to SD.
 * Default 0 for production (frequent SD writes can corrupt the FS on reset). */
#define SPK_DUMP_OUTPUT 0
static FBankContext *g_spk_fbank = NULL;
static Gallery       g_spk_gallery;
static bool          g_spk_gallery_inited = false;

// ========== Audio Recognition Constants ==========
#define AUDIO_RECOG_EMB_SIZE    SPEAKER_RECOG_DEFAULT_EMB_SIZE  // 48

// MFCC 前處理開關：
//   1 = 啟用（輸入為 raw PCM，需要 MFCC 轉換）
//   0 = 停用（輸入已經是模型特徵格式 [128][20][87]）
#define AUDIO_ENABLE_MFCC_PREPROCESS  1

// NPU 模型輸出維度（用於調試）
#define AUDIO_RECOG_C   SPEAKER_RECOG_DEFAULT_C     // 128
#define AUDIO_RECOG_T   SPEAKER_RECOG_DEFAULT_T     // 20
#define AUDIO_RECOG_F   SPEAKER_RECOG_DEFAULT_F     // 87

// Class names for YOLO detection models
static const char* person_pet_classes[] = {
    "person", "pet"  // YOLOv8 person+pet: 0=person, 1=pet
};

static const char* person_classes[] = {
    "person"
};

static const char* pet_classes[] = {
    "pet"
};

static const char* face_classes[] = {
    "face"
};

// Global image buffer for external input (img_b.bin)
char *imgBuffer = NULL;

// Global buffers for temporary layer verification (tmp_layer_b.bin files)
char *tmpLayerBuffer[3] = {NULL, NULL, NULL};

// Global buffers for output layer verification (output_layer_b.bin files)
char *outputLayerBuffer[6] = {NULL, NULL, NULL, NULL, NULL, NULL};

// Global flag to control inference task
static int g_iproai_task_started = 0;

// ========== External Buffer Support ==========
#define EXT_BUFFER_QUEUE_SIZE  5              // Queue 最多存放 5 個 buffer 地址
#define RESULT_QUEUE_SIZE      5              // Queue 最多存放 5 個 result 地址
#define EXT_BUFFER_EVENT_STOP  (1 << 0)       // 停止事件標誌

static QueueHandle_t g_ext_buffer_queue = NULL;        // External buffer 地址 queue
static EventGroupHandle_t g_ext_event_group = NULL;    // 事件組用於同步
static int g_ext_task_started = 0;                     // External 推理任務狀態

#ifdef CONFIG_MM_SUPPORT
// for save result in queue and call API to get the result
static QueueHandle_t g_result_queue = NULL;        // result buffer 地址 queue
static yolov8_result_t g_det_result;
#endif

#ifdef CONFIG_EVA_BIN_MODE
// External function from iproai_inst_process.c for tmp layer verification
extern uint8_t **iproai_tmp_layer_bin_init(uint8_t* buffer[3]);

// External function from iproai_inst_process.c for output layer verification
extern uint8_t **iproai_output_layer_bin_init(uint32_t buffer);
#endif

// AI 模型類型定義
typedef enum {
    AI_MODEL_PERSON_DETECT = 0,           // 人形檢測
    AI_MODEL_PERSON_PET_DETECT = 1,       // 人形+寵物檢測
    AI_MODEL_PET_DETECT = 2,              // 寵物檢測
    AI_MODEL_FACE_DETECT = 3,             // 人臉檢測
    AI_MODEL_AUDIO_RECOG = 4,             // 語者識別 (MFCC/legacy, audio_recog.inm)
    AI_MODEL_SPEAKER_RECOG = 5,           // 語者識別 (res2dnet end-to-end, fbank + gallery)
    // 可以繼續添加更多模型...
    AI_MODEL_MAX
} ai_model_type_e;

// 語者識別的類別名稱（用於顯示）
static const char* audio_recog_classes[] = {
    "Speaker_1", "Speaker_2", "Speaker_3", "Speaker_4"
};

// 模型配置結構
typedef struct {
    const char *model_path;               // 模型檔案路徑
    const char *img_path;                 // 輸入圖像路徑
    const char *description;              // 描述
    uint32_t model_layer_num;             // 模型層數
    const char **class_names;             // 類別名稱陣列
    int class_count;                      // 類別數量
    uint32_t input_width;                 // 輸入寬度
    uint32_t input_height;                // 輸入高度
} ai_model_config_t;

// 模型配置表
static const ai_model_config_t g_model_configs[] = {
    [AI_MODEL_PERSON_DETECT] = {
        .model_path = "/sdcard/ai_model/person_detect.inm",
        .img_path = "/sdcard/ai_model/person_img.bin",
        .description = "YOLOv8 Person Detection",
        .model_layer_num = 4,
        .class_names = person_classes,
        .class_count = 1,
        .input_width = 640,
        .input_height = 360
    },
    [AI_MODEL_PERSON_PET_DETECT] = {
        .model_path = "/sdcard/ai_model/person_pet_detect.inm",
        .img_path = "/sdcard/ai_model/person_pet_img.bin",
        .description = "YOLOv8 Person+Pet Detection",
        .model_layer_num = 6,  // YOLOv8 with 6 separate outputs (3 box + 3 class) - same as ipro85_linux
        .class_names = person_pet_classes,
        .class_count = 2,
        .input_width = 640,
        .input_height = 360
    },
    [AI_MODEL_PET_DETECT] = {
        .model_path = "/sdcard/ai_model/pet_detect.inm",
        .img_path = "/sdcard/ai_model/pet_img.bin",
        .description = "YOLOv8 Pet Detection",
        .model_layer_num = 4,
        .class_names = pet_classes,
        .class_count = 1,
        .input_width = 640,
        .input_height = 360
    },
    [AI_MODEL_FACE_DETECT] = {
        .model_path = "/sdcard/ai_model/face_detect.inm",
        .img_path = "/sdcard/ai_model/face_img.bin",
        .description = "YOLOv8 Face Detection",
        .model_layer_num = 4,
        .class_names = face_classes,
        .class_count = 1,
        .input_width = 640,
        .input_height = 360
    },
    [AI_MODEL_AUDIO_RECOG] = {
        .model_path = "/sdcard/ai_model/audio_recog.inm",
        .img_path = "/sdcard/ai_model/audio_input.bin",   // PCM 音頻特徵輸入
        .description = "Speaker Recognition (Audio)",
        .model_layer_num = 1,   
        .class_names = audio_recog_classes,
        .class_count = 4,
        .input_width = 0,     // 音頻不使用 width/height
        .input_height = 0
    },
    [AI_MODEL_SPEAKER_RECOG] = {
        .model_path = "/sdcard/ai_model/res2dnet.inm",
        .img_path = "/sdcard/ai_model/audio_input.bin",   // raw int16 PCM, 16kHz mono
        .description = "Speaker ID (res2dnet/fbank)",
        .model_layer_num = 1,
        .class_names = NULL,
        .class_count = 0,
        .input_width = 0,
        .input_height = 0
    },
};
#endif

/**
 * AI test command - placeholder for AI functionality
 */
static int cmd_ai_test(int argc, char **argv)
{
    printf("AI Test Command\r\n");
    printf("AI functionality placeholder\r\n");
    // Add your AI test logic here
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ai_test, ai_test, AI Test);

/**
 * AI inference command - placeholder for running AI inference
 */
static int cmd_ai_inference(int argc, char **argv)
{
    printf("AI Inference Command\r\n");
    printf("Running AI inference...\r\n");
    // Add your AI inference logic here
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ai_inference, ai_inference, Run AI Inference);

/**
 * AI status command - display AI subsystem status
 */
static int cmd_ai_status(int argc, char **argv)
{
    printf("AI Status:\r\n");
    printf("  AI Support: Enabled\r\n");
    printf("  Memory Available: %zu bytes\r\n", xPortGetFreeHeapSize());
    // Add more status information as needed
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ai_status, ai_status, Show AI Status);

/**
 * AI SD card load command - load AI model from SD card
 */
static int cmd_ai_sd_load(int argc, char **argv)
{
    int fd;
    uint8_t model_header[16];
    int ret;
    int file_size;

    printf("AI SD Load Command\r\n");

    // Mount SD card with EVFS support
    mount_sdcard();

    // Open the model file
    printf("Opening /sdcard/ai_model/model.inm...\r\n");
    fd = open("/sdcard/ai_model/model.inm", O_RDONLY);
    if (fd < 0) {
        printf("Error: Failed to open model file (fd=%d)\r\n", fd);
        printf("Please ensure /sdcard/ai_model/model.inm exists\r\n");
        return -1;
    }

    // Get file size
    file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    printf("Model file size: %d bytes\r\n", file_size);

    if (file_size <= 0) {
        printf("Error: Invalid file size\r\n");
        close(fd);
        return -1;
    }

    // Read first 16 bytes for display
    ret = read(fd, model_header, 16);
    if (ret < 0) {
        printf("Error: Failed to read model file (ret=%d)\r\n", ret);
        close(fd);
        return -1;
    }

    printf("First 16 bytes of model.inm:\r\n");
    printf("Hex: ");
    for (int i = 0; i < ret && i < 16; i++) {
        printf("%02X ", model_header[i]);
    }
    printf("\r\n");

    printf("ASCII: ");
    for (int i = 0; i < ret && i < 16; i++) {
        if (model_header[i] >= 32 && model_header[i] <= 126) {
            printf("%c", model_header[i]);
        } else {
            printf(".");
        }
    }
    printf("\r\n");

    // Reset file pointer to beginning
    lseek(fd, 0, SEEK_SET);

#ifdef CONFIG_AI_SUPPORT
    // Variables only used when AI is enabled
    uint8_t *model_buffer = NULL;
    int bytes_read;
    int img_fd;
    int img_size;

    // ===== Load img_b.bin FIRST =====
    printf("\r\n=== Loading Input Image (img_b.bin) ===\r\n");
    printf("Opening /sdcard/ai_model/img_b.bin...\r\n");
    img_fd = open("/sdcard/ai_model/img_b.bin", O_RDONLY);
    if (img_fd < 0) {
        printf("Error: Failed to open img_b.bin (fd=%d)\r\n", img_fd);
        printf("Please ensure /sdcard/ai_model/img_b.bin exists\r\n");
        close(fd);
        return -1;
    }

    // Get image file size
    img_size = lseek(img_fd, 0, SEEK_END);
    lseek(img_fd, 0, SEEK_SET);
    printf("Image file size: %d bytes\r\n", img_size);

    if (img_size <= 0) {
        printf("Error: Invalid image file size\r\n");
        close(img_fd);
        close(fd);
        return -1;
    }

    // Free old imgBuffer if it exists
    if (imgBuffer != NULL) {
        printf("Freeing old imgBuffer...\r\n");
        vPortFree(imgBuffer);
        imgBuffer = NULL;
    }

    // Allocate memory for image buffer
    printf("Allocating %d bytes for image buffer...\r\n", img_size);
    imgBuffer = (char *)pvPortMalloc(img_size);
    if (imgBuffer == NULL) {
        printf("Error: Failed to allocate memory for image (%d bytes)\r\n", img_size);
        printf("Available heap: %zu bytes\r\n", xPortGetFreeHeapSize());
        close(img_fd);
        close(fd);
        return -1;
    }
    printf("Image buffer allocated successfully at 0x%p\r\n", imgBuffer);

    // Read image file into buffer
    printf("Reading image file into memory...\r\n");
    bytes_read = read(img_fd, imgBuffer, img_size);
    if (bytes_read != img_size) {
        printf("Error: Failed to read complete image file (read %d/%d bytes)\r\n", bytes_read, img_size);
        vPortFree(imgBuffer);
        imgBuffer = NULL;
        close(img_fd);
        close(fd);
        return -1;
    }
    printf("Successfully loaded image into memory\r\n");
    printf("imgBuffer address: 0x%p (will use 0x%p for NPU)\r\n", imgBuffer, (void*)((uint32_t)imgBuffer + 4));
    close(img_fd);

    // ===== Now load the model =====
    // Allocate memory for the entire model
    printf("\r\n=== Memory Allocation Debug ===\r\n");
    printf("Available heap before allocation: %zu bytes\r\n", xPortGetFreeHeapSize());
    printf("Allocating %d bytes for model buffer...\r\n", file_size);
    model_buffer = (uint8_t *)pvPortMalloc(file_size);
    if (model_buffer == NULL) {
        printf("Error: Failed to allocate memory for model (%d bytes)\r\n", file_size);
        printf("Available heap: %zu bytes\r\n", xPortGetFreeHeapSize());
        close(fd);
        return -1;
    }
    printf("Model buffer allocated successfully at 0x%p\r\n", model_buffer);
    printf("Available heap after model buffer allocation: %zu bytes\r\n", xPortGetFreeHeapSize());

    // Read entire file into buffer
    printf("Reading model file into memory...\r\n");
    bytes_read = read(fd, model_buffer, file_size);
    if (bytes_read != file_size) {
        printf("Error: Failed to read complete file (read %d/%d bytes)\r\n", bytes_read, file_size);
        vPortFree(model_buffer);
        close(fd);
        return -1;
    }
    printf("Successfully loaded model into memory\r\n");

    // Close the file
    close(fd);

    // Create IPROAI model handle if not already created
    if (g_model_hdl == NULL) {
        printf("Creating IPROAI model handle...\r\n");
        g_model_hdl = iproai_create();
        if (g_model_hdl == NULL) {
            printf("Error: Failed to create IPROAI model handle\r\n");
            vPortFree(model_buffer);
            return -1;
        }
        printf("IPROAI model handle created\r\n");
    }

    // Load model from buffer into NPU
    printf("\r\n=== Loading Model into NPU ===\r\n");
    printf("Available heap before iproai_load_model_from_buffer: %zu bytes\r\n", xPortGetFreeHeapSize());
    printf("Loading model into NPU...\r\n");
    IPROAI_Status_e status = iproai_load_model_from_buffer(g_model_hdl, model_buffer);

    // Free the model buffer after loading
    printf("\r\n=== Freeing temporary model buffer ===\r\n");
    printf("Heap before freeing model buffer: %zu bytes\r\n", xPortGetFreeHeapSize());
    vPortFree(model_buffer);
    printf("Heap after freeing model buffer: %zu bytes\r\n", xPortGetFreeHeapSize());

    if (status == IPROAI_STATUS_NO_ERROR) {
        printf("SUCCESS: Model loaded into NPU successfully!\r\n");

        // Get model input resolution
        uint32_t width = 0, height = 0;
        if (iproai_getInputResolution(g_model_hdl, &width, &height) == IPROAI_STATUS_NO_ERROR) {
            printf("Model input resolution: %lux%lu\r\n", width, height);
        }
    } else {
        printf("ERROR: Failed to load model into NPU (status=%d)\r\n", status);
        const char* error_msg[] = {
            "NO_ERROR", "OUT_OF_MEMORY", "INCOMPLETE", "BAD_DATA",
            "OP_FAILED", "UNAVAILABLE_DEVICE", "INVALID_INPUT"
        };
        if (status < 7) {
            printf("Error type: %s\r\n", error_msg[status]);
        }
    }
#else
    printf("AI support not enabled in configuration\r\n");
    close(fd);
#endif
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ai_sd_load, ai_sd_load, Load AI model from SD card);

#ifdef CONFIG_AI_SUPPORT
/**
 * AI inference task - runs in a separate thread
 * Based on iproai_inference_task from iproai_process.c
 */
static void iproai_inference_task(void *arg)
{
    iproai_model_hdl_t iproai_hdl = (iproai_model_hdl_t) arg;
    uint8_t* img_buf;
    IPROAI_Status_e status = IPROAI_STATUS_NO_ERROR;

    printf("[Inference Task] Started\r\n");
    printf("[Inference Task] DEBUG: g_iproai_task_started=%d\r\n", g_iproai_task_started);

    while (g_iproai_task_started) {
        printf("[Inference Task] DEBUG: Loop iteration start\r\n");
        // Get input buffer from IPROAI
        img_buf = iproai_getInputBuffer(iproai_hdl);

        if (img_buf == NULL) {
            printf("[Inference Task] ERROR: img_buf unavailable\r\n");
            goto exit;
        }
        printf("[Inference Task] DEBUG: Got input buffer at 0x%p\r\n", img_buf);

        // Prepare Input - copy from imgBuffer (loaded from img_b.bin)
        printf("[Inference Task] DEBUG: Checking imgBuffer (0x%p)\r\n", imgBuffer);
        if (imgBuffer != NULL) {
            // imgBuffer format: first 4 bytes = size, then actual image data
            uint32_t img_size = *((uint32_t*)imgBuffer);
            uint8_t* img_data = (uint8_t*)imgBuffer + 4;

            printf("[Inference Task] DEBUG: Copying %lu bytes from imgBuffer to input buffer\r\n", (unsigned long)img_size);
            memcpy(img_buf, img_data, img_size);

            // Flush cache to ensure data is visible to NPU
            L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)img_buf, img_size);
            printf("[Inference Task] DEBUG: Image data copied and cache flushed\r\n");
        } else {
            printf("[Inference Task] ERROR: imgBuffer is NULL! Load img_b.bin first.\r\n");
            goto exit;
        }

        // Start Inference
        printf("[Inference Task] DEBUG: About to call iproai_startCompute\r\n");
#ifdef CONFIG_EVA_BIN_MODE
        //while(status == IPROAI_STATUS_NO_ERROR){
        	status = iproai_startCompute(iproai_hdl);
        //}
#else
		status = iproai_startCompute(iproai_hdl);
#endif
        printf("[Inference Task] DEBUG: iproai_startCompute returned status=%d (0=SUCCESS)\r\n", status);
        if (status == IPROAI_STATUS_NO_ERROR) {
            printf("[Inference Task] SUCCESS: Inference completed successfully!\r\n");

            // 執行後處理（根據模型類型）
            if (g_current_model_index == AI_MODEL_PERSON_PET_DETECT) {
                printf("[Inference Task] Running YOLOv8 Person+Pet postprocessing...\r\n");

                // 獲取網路資訊
                struct iproai_net_info_t *net = iproai_getNetInfo(iproai_hdl);
                if (net == NULL) {
                    printf("[Inference Task] ERROR: Failed to get network info\r\n");
                    goto exit;
                }

                // 初始化 YOLOv8 Person+Pet 檢測器
                int ret = yolov8_person_pet_init(&g_yolov8_config, net);
                if (ret != 0) {
                    printf("[Inference Task] ERROR: Failed to initialize YOLOv8 Person+Pet\r\n");
                    goto exit;
                }

                // 獲取輸出緩衝區
                uint8_t *data_buffer = iproai_getInputBuffer(iproai_hdl);
                if (data_buffer == NULL) {
                    printf("[Inference Task] ERROR: Failed to get data buffer\r\n");
                    yolov8_person_pet_deinit(&g_yolov8_config);
                    goto exit;
                }

                // 執行後處理
                yolov8_result_t result;

                printf("[Inference Task] DEBUG: Model has %d output layers\r\n", net->output_num);
                printf("[Inference Task] Running YOLOv8 postprocessing...\r\n");

                ret = yolov8_person_pet_process(&g_yolov8_config, net, data_buffer, &result);
                printf("[Inference Task] DEBUG: Postprocessing returned %d\r\n", ret);

                if (ret != 0) {
                    printf("[Inference Task] ERROR: YOLOv8 Person+Pet postprocessing failed\r\n");
                    yolov8_person_pet_deinit(&g_yolov8_config);
                    goto exit;
                }

                // 列印結果
                yolov8_person_pet_print_results(&result);

                // 清理資源
                yolov8_person_pet_deinit(&g_yolov8_config);
            }
            // ========== 語者識別後處理 ==========
            else if (g_current_model_index == AI_MODEL_AUDIO_RECOG) {
                printf("[Inference Task] Running Speaker Recognition postprocessing...\r\n");

                // 自動初始化並載入權重（如果還沒初始化）
                if (!g_speaker_recog_initialized) {
                    printf("[Inference Task] Auto-initializing speaker recognition...\r\n");
                    if (speaker_recog_init(&g_speaker_config, NULL, NULL) != 0) {
                        printf("[Inference Task] ERROR: Auto-init failed\r\n");
                        goto exit;
                    }
                    g_speaker_recog_initialized = true;
                }

                // 獲取網路資訊
                struct iproai_net_info_t *net = iproai_getNetInfo(iproai_hdl);
                if (net == NULL) {
                    printf("[Inference Task] ERROR: Failed to get network info\r\n");
                    goto exit;
                }

                // 設定網路資訊（更新量化參數）
                speaker_recog_set_net(&g_speaker_config, net);

                // 獲取 data buffer
                uint8_t *data_buffer = iproai_getInputBuffer(iproai_hdl);
                if (data_buffer == NULL) {
                    printf("[Inference Task] ERROR: Failed to get data buffer\r\n");
                    goto exit;
                }

                // 取得輸出層資訊
                if (net->output_num < 1) {
                    printf("[Inference Task] ERROR: No output layer\r\n");
                    goto exit;
                }

                uint32_t layer_idx = net->output_layer[0];
                uint32_t output_offset = net->layers[layer_idx].out_layer_mem * net->patch_size;
                uint32_t out_h = net->layers[layer_idx].out_h;
                uint32_t out_w = net->layers[layer_idx].out_w;
                uint32_t out_c = net->layers[layer_idx].out_c;
                uint32_t output_size = out_h * out_w * out_c;
                uint8_t *output_buffer = data_buffer + output_offset;

                printf("[Inference Task] Output: layer[%lu], size=%lu bytes\r\n",
                       (unsigned long)layer_idx, (unsigned long)output_size);

                // 執行語者識別後處理
                //speaker_recog_result_t result;
                int ret = speaker_recog_process(&g_speaker_config, output_buffer, output_size, &speaker_result);
                if (ret != 0) {
                    printf("[Inference Task] ERROR: Speaker recognition processing failed\r\n");
                    goto exit;
                }

                // 列印結果
                speaker_recog_print_result(&speaker_result);
            }
        } else {
            printf("[Inference Task] ERROR: Inference failed with status %d\r\n", status);
            if (status == IPROAI_STATUS_UNAVAILABLE_DEVICE) {
                printf("[Inference Task] NPU device unavailable\r\n");
            }
        }

        // For single-shot inference, break after one iteration
        break;
    }

exit:
    printf("[Inference Task] Exiting...\r\n");
    g_iproai_task_started = 0;
    vTaskDelete(NULL);
}

/**
 * Start inference in a separate thread
 * Based on iproai_start_inference from iproai_process.c
 */
static void iproai_start_inference(iproai_model_hdl_t hdl)
{
    if (g_iproai_task_started == 0) {
        g_iproai_task_started = 1;
        xTaskCreate(iproai_inference_task, "iproai_inference", 4096, hdl, 10, NULL);
    } else {
        printf("[Inference] Inference task already running\r\n");
    }
}

/**
 * AI inference task for external buffer - 連續從 queue 讀取 external buffer 進行推理
 * @param arg: iproai_model_hdl_t 模型句柄
 */
static void iproai_inference_task_ext(void *arg)
{
    iproai_model_hdl_t iproai_hdl = (iproai_model_hdl_t) arg;
    uint32_t ext_buffer_addr = 0;
    IPROAI_Status_e status = IPROAI_STATUS_NO_ERROR;
    EventBits_t event_bits;
    int frame_count = 0;

    printf("[Ext Inference Task] Started - waiting for external buffers\r\n");

    while (g_ext_task_started) {
        // 等待 queue 中的 buffer 地址（阻塞等待，最多 1 秒）
        if (xQueueReceive(g_ext_buffer_queue, &ext_buffer_addr, pdMS_TO_TICKS(1000)) == pdTRUE) {
            printf("\r\n[Ext Inference Task] Frame #%d - Received buffer address: 0x%08lx\r\n",
                   ++frame_count, (unsigned long)ext_buffer_addr);

            // 配置 NPU external image address
            NPU_Img_Ext_Addr_Cfg(ext_buffer_addr);
            printf("[Ext Inference Task] NPU external address configured\r\n");

            // Start Inference
#ifdef CONFIG_EVA_BIN_MODE
            while(status == IPROAI_STATUS_NO_ERROR){
                status = iproai_startCompute(iproai_hdl);
            }
#else
            status = iproai_startCompute(iproai_hdl);
#endif

            if (status == IPROAI_STATUS_NO_ERROR) {
                printf("[Ext Inference Task] SUCCESS: Inference completed for frame #%d\r\n", frame_count);

                // 執行後處理（根據模型類型）
                if (g_current_model_index == AI_MODEL_PERSON_PET_DETECT) {
                    printf("[Ext Inference Task] Running YOLOv8 Person+Pet postprocessing...\r\n");

                    // 獲取網路資訊
                    struct iproai_net_info_t *net = iproai_getNetInfo(iproai_hdl);
                    if (net == NULL) {
                        printf("[Ext Inference Task] ERROR: Failed to get network info\r\n");
                        continue;
                    }

                    // 初始化 YOLOv8 Person+Pet 檢測器（每次都重新初始化以確保正確）
                    int ret = yolov8_person_pet_init(&g_yolov8_config, net);
                    if (ret != 0) {
                        printf("[Ext Inference Task] ERROR: Failed to initialize YOLOv8 Person+Pet\r\n");
                        continue;
                    }

                    // 獲取輸出緩衝區
                    uint8_t *data_buffer = iproai_getInputBuffer(iproai_hdl);
                    if (data_buffer == NULL) {
                        printf("[Ext Inference Task] ERROR: Failed to get data buffer\r\n");
                        yolov8_person_pet_deinit(&g_yolov8_config);
                        continue;
                    }

                    // 執行後處理
                    yolov8_result_t result;
                    ret = yolov8_person_pet_process(&g_yolov8_config, net, data_buffer, &result);

                    if (ret != 0) {
                        printf("[Ext Inference Task] ERROR: YOLOv8 Person+Pet postprocessing failed\r\n");
                        yolov8_person_pet_deinit(&g_yolov8_config);
                        continue;
                    }

                    // 列印結果
                    yolov8_person_pet_print_results(&result);

#ifdef CONFIG_MM_SUPPORT // tricky call function
                    memcpy(&g_det_result, &result, sizeof(yolov8_result_t));
                    yolov8_result_t *result_ptr = &g_det_result;
                    xQueueSend(g_result_queue, &result_ptr, pdMS_TO_TICKS(1));
#endif
                    // 清理資源
                    yolov8_person_pet_deinit(&g_yolov8_config);
                }
                else if (g_current_model_index == AI_MODEL_AUDIO_RECOG) {
                    printf("[Ext Inference Task] Running Speaker Recognition postprocessing...\r\n");

                    // 自動初始化並載入權重（如果還沒初始化）
                    if (!g_speaker_recog_initialized) {
                        printf("[Ext Inference Task] Auto-initializing speaker recognition...\r\n");
                        if (speaker_recog_init(&g_speaker_config, NULL, NULL) != 0) {
                            printf("[Ext Inference Task] ERROR: Auto-init failed\r\n");
                            continue;
                        }
                        g_speaker_recog_initialized = true;
                    }

                    // 獲取網路資訊
                    struct iproai_net_info_t *net = iproai_getNetInfo(iproai_hdl);
                    if (net == NULL) {
                        printf("[Ext Inference Task] ERROR: Failed to get network info\r\n");
                        continue;
                    }

                    // 設定網路資訊（更新量化參數）
                    speaker_recog_set_net(&g_speaker_config, net);

                    // 獲取 data buffer
                    uint8_t *data_buffer = iproai_getInputBuffer(iproai_hdl);
                    if (data_buffer == NULL) {
                        printf("[Ext Inference Task] ERROR: Failed to get data buffer\r\n");
                        continue;
                    }

                    // 取得輸出層資訊
                    if (net->output_num < 1) {
                        printf("[Ext Inference Task] ERROR: No output layer\r\n");
                        continue;
                    }

                    uint32_t layer_idx = net->output_layer[0];
                    uint32_t output_offset = net->layers[layer_idx].out_layer_mem * net->patch_size;
                    uint32_t out_h = net->layers[layer_idx].out_h;
                    uint32_t out_w = net->layers[layer_idx].out_w;
                    uint32_t out_c = net->layers[layer_idx].out_c;
                    uint32_t output_size = out_h * out_w * out_c;
                    uint8_t *output_buffer = data_buffer + output_offset;

                    printf("[Ext Inference Task] Output: layer[%lu], size=%lu bytes\r\n",
                           (unsigned long)layer_idx, (unsigned long)output_size);

                    // 執行語者識別後處理
                    //speaker_recog_result_t result;
                    int ret = speaker_recog_process(&g_speaker_config, output_buffer, output_size, &speaker_result);
                    if (ret != 0) {
                        printf("[Ext Inference Task] ERROR: Speaker recognition processing failed\r\n");
                        continue;
                    }

                    // 根據模式執行動作
                    if (g_audio_mode == AUDIO_MODE_REGISTER) {
                        printf("[Ext Inference Task] Registering speaker: %s\r\n", g_register_speaker_name);
                        int reg_ret = speaker_recog_register(&g_speaker_config, g_register_speaker_name);
                        if (reg_ret >= 0) {
                            printf("[Ext Inference Task] Speaker '%s' registered at slot %d\r\n", g_register_speaker_name, reg_ret);
                        } else {
                            printf("[Ext Inference Task] ERROR: Registration failed (ret=%d)\r\n", reg_ret);
                        }
                    } else {
                        // 列印辨識結果
                        speaker_recog_print_result(&speaker_result);
                    }
                }
                else if (g_current_model_index == AI_MODEL_SPEAKER_RECOG) {
                    printf("[Ext Inference Task] Running Speaker ID (res2dnet) postprocessing...\r\n");

                    struct iproai_net_info_t *net = iproai_getNetInfo(iproai_hdl);
                    uint8_t *data_buffer = iproai_getInputBuffer(iproai_hdl);
                    if (net == NULL || data_buffer == NULL || net->output_num < 1) {
                        printf("[Ext Inference Task] ERROR: bad net/buffer for SpkID\r\n");
                        continue;
                    }

#if SPK_DUMP_OUTPUT
                    /* model output structure — to locate / understand the 256-D embedding */
                    printf("[SpkID] net: output_num=%d patch_size=%d  input(w=%d h=%d c=%d)\r\n",
                           net->output_num, net->patch_size, net->w, net->h, net->c);
                    for (int k = 0; k < net->output_num && k < 8; k++) {
                        uint32_t lk = net->output_layer[k];
                        printf("[SpkID]   out[%d]: layer=%lu h=%d w=%d c=%d (=%d vals) mem=%d scale=%.6f zp=%d\r\n",
                               k, (unsigned long)lk,
                               net->layers[lk].out_h, net->layers[lk].out_w, net->layers[lk].out_c,
                               net->layers[lk].out_h * net->layers[lk].out_w * net->layers[lk].out_c,
                               net->layers[lk].out_layer_mem,
                               net->layers[lk].output_scale, net->layers[lk].tf_output_offset);
                    }
#endif

                    uint32_t li  = net->output_layer[0];
                    uint32_t off = (uint32_t)net->layers[li].out_layer_mem * net->patch_size;
                    uint32_t osz = net->layers[li].out_h * net->layers[li].out_w * net->layers[li].out_c;
                    /* NPU output is uint8 (offset-binary), same as speaker_recog's dequantize_uint8 */
                    uint8_t *out = (uint8_t *)(data_buffer + off);

                    float out_scale = net->layers[li].output_scale;
                    int   out_zp    = net->layers[li].tf_output_offset;
                    if (!(out_scale > 0.0f)) { out_scale = SPK_OUT_SCALE; out_zp = SPK_OUT_ZP; }
#if SPK_DUMP_OUTPUT
                    printf("[SpkID] output: %lu values, dequant scale=%.6f zp=%d\r\n",
                           (unsigned long)osz, out_scale, out_zp);
#endif
                    if (osz < (uint32_t)EMBEDDING_DIM) {
                        printf("[SpkID] ERROR: output %lu < %d (check res2dnet.inm output layer)\r\n",
                               (unsigned long)osz, EMBEDDING_DIM);
                        continue;
                    }

                    /* dequantize (uint8) + L2-normalise -> embedding[256] */
                    float emb[EMBEDDING_DIM];
                    float norm_sq = 0.0f;
                    for (int i = 0; i < EMBEDDING_DIM; i++) {
                        emb[i] = ((int)out[i] - out_zp) * out_scale;
                        norm_sq += emb[i] * emb[i];
                    }
                    float norm = sqrtf(norm_sq);
                    if (norm > 1e-8f)
                        for (int i = 0; i < EMBEDDING_DIM; i++) emb[i] /= norm;

#if SPK_DUMP_OUTPUT
                    printf("[SpkID] raw out bytes[0..31] hex: ");
                    for (int i = 0; i < 32; i++) printf("%02x ", (unsigned)out[i]);
                    printf("\r\n[SpkID] out uint8[0..15]: ");
                    for (int i = 0; i < 16; i++) printf("%d ", (int)out[i]);
                    printf("\r\n[SpkID] embedding[%d] (uint8 dequant + L2-norm; compare cosine vs golden):\r\n",
                           EMBEDDING_DIM);
                    for (int i = 0; i < EMBEDDING_DIM; i++)
                        printf("%9.6f%s", emb[i], ((i % 8) == 7) ? "\r\n" : " ");

                    /* dump NPU output (256 uint8) to SD: .bin (raw) + .txt (one hex/line) */
                    {
                        int fd = open("/sdcard/ai_model/npu_out.bin",
                                      O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fd >= 0) { write(fd, out, EMBEDDING_DIM); close(fd); }
                        char *t = (char *)pvPortMalloc(EMBEDDING_DIM * 4 + 16);
                        if (t) {
                            int o = 0;
                            for (int i = 0; i < EMBEDDING_DIM; i++)
                                o += snprintf(t + o, EMBEDDING_DIM * 4 + 16 - o, "%02x\n", out[i]);
                            int fd2 = open("/sdcard/ai_model/npu_out.txt",
                                           O_WRONLY | O_CREAT | O_TRUNC, 0644);
                            if (fd2 >= 0) { write(fd2, t, o); close(fd2); }
                            vPortFree(t);
                        }
                        printf("[SpkID] NPU output -> /sdcard/ai_model/npu_out.{bin,txt} (%d uint8)\r\n",
                               EMBEDDING_DIM);
                    }
					/* dump NPU output (256 uint8) to SD*/
#endif

                    if (!g_spk_gallery_inited) {
                        gallery_init(&g_spk_gallery, GALLERY_DEFAULT_THRESHOLD);
                        g_spk_gallery_inited = true;
                    }

                    if (g_audio_mode == AUDIO_MODE_REGISTER) {
                        /* multi-clip enroll: call 'register <name>' once per clip;
                         * same name accumulates + averages, not overwrites. */
                        int nclip = gallery_enroll(&g_spk_gallery, g_register_speaker_name, emb);
                        if (nclip > 0)
                            printf("[SpkID] Enrolled '%s' (%d clip%s averaged; %d speakers total)\r\n",
                                   g_register_speaker_name, nclip, (nclip > 1 ? "s" : ""),
                                   g_spk_gallery.n);
                        else
                            printf("[SpkID] ERROR: enroll failed (gallery full?)\r\n");
                    } else {
                        float score = 0.0f;
                        int idx = gallery_identify(&g_spk_gallery, emb, &score);
                        if (idx >= 0)
                            printf("[SpkID] ACCEPT  speaker=%s  score=%.4f  (thr=%.2f)\r\n",
                                   g_spk_gallery.names[idx], score, g_spk_gallery.threshold);
                        else
                            printf("[SpkID] REJECT  unknown       score=%.4f  (thr=%.2f)\r\n",
                                   score, g_spk_gallery.threshold);
                    }
                }
            } else {
                printf("[Ext Inference Task] ERROR: Inference failed with status %d\r\n", status);
                if (status == IPROAI_STATUS_UNAVAILABLE_DEVICE) {
                    printf("[Ext Inference Task] NPU device unavailable\r\n");
                }
            }
        }

        // 檢查是否有停止事件
        event_bits = xEventGroupGetBits(g_ext_event_group);
        if (event_bits & EXT_BUFFER_EVENT_STOP) {
            printf("[Ext Inference Task] Stop event received\r\n");
            break;
        }
    }

    printf("[Ext Inference Task] Exiting... Total frames processed: %d\r\n", frame_count);
    g_ext_task_started = 0;
    vTaskDelete(NULL);
}

/**
 * Start external buffer inference in a separate thread
 */
static void iproai_start_inference_ext(iproai_model_hdl_t hdl)
{
    if (g_ext_task_started == 0) {
        g_ext_task_started = 1;
        xTaskCreate(iproai_inference_task_ext, "iproai_inf_ext", 8192, hdl, 10, NULL);
        printf("[Ext Inference] External buffer inference task started\r\n");
    } else {
        printf("[Ext Inference] External inference task already running\r\n");
    }
}

/**
 * Load temporary layer binary files from SD card for NPU verification
 * Looks for tmp_layer_b.bin or tmp_layer_b0.bin -> buffer[0]
 * Looks for tmp_layer_b1.bin -> buffer[1]
 * Looks for tmp_layer_b2.bin -> buffer[2]
 * Returns the number of buffers successfully loaded
 */
static int load_tmp_layer_bins(void)
{
    int loaded_count = 0;

    printf("\n=== Loading Temporary Layer Verification Files ===\n");

    // Try to load buffer[0]: first try tmp_layer_b.bin, then tmp_layer_b0.bin
    const char* buffer0_files[] = {
        "/sdcard/ai_model/tmp_layer_b.bin",
        "/sdcard/ai_model/tmp_layer_b0.bin"
    };

    for (int i = 0; i < 2; i++) {
        printf("Trying to open %s...\r\n", buffer0_files[i]);
        int fd = open(buffer0_files[i], O_RDONLY);
        if (fd >= 0) {
            // Get file size
            int file_size = lseek(fd, 0, SEEK_END);
            lseek(fd, 0, SEEK_SET);

            if (file_size > 0) {
                // Free old buffer if it exists
                if (tmpLayerBuffer[0] != NULL) {
                    vPortFree(tmpLayerBuffer[0]);
                    tmpLayerBuffer[0] = NULL;
                }

                // Allocate memory
                tmpLayerBuffer[0] = (char *)pvPortMalloc(file_size);
                if (tmpLayerBuffer[0] != NULL) {
                    // Read file
                    int bytes_read = read(fd, tmpLayerBuffer[0], file_size);
                    if (bytes_read == file_size) {
                        printf("Loaded %s (%d bytes) -> buffer[0]\r\n", buffer0_files[i], file_size);
                        loaded_count++;
                    } else {
                        printf("Error: Failed to read complete file (read %d/%d bytes)\r\n", bytes_read, file_size);
                        vPortFree(tmpLayerBuffer[0]);
                        tmpLayerBuffer[0] = NULL;
                    }
                } else {
                    printf("Error: Failed to allocate memory for %s\r\n", buffer0_files[i]);
                }
            }
            close(fd);
            break; // Successfully processed buffer[0], don't try the second file
        }
    }

    // Try to load buffer[1]: tmp_layer_b1.bin
    printf("Trying to open /sdcard/ai_model/tmp_layer_b1.bin...\r\n");
    int fd1 = open("/sdcard/ai_model/tmp_layer_b1.bin", O_RDONLY);
    if (fd1 >= 0) {
        int file_size = lseek(fd1, 0, SEEK_END);
        lseek(fd1, 0, SEEK_SET);

        if (file_size > 0) {
            if (tmpLayerBuffer[1] != NULL) {
                vPortFree(tmpLayerBuffer[1]);
                tmpLayerBuffer[1] = NULL;
            }

            tmpLayerBuffer[1] = (char *)pvPortMalloc(file_size);
            if (tmpLayerBuffer[1] != NULL) {
                int bytes_read = read(fd1, tmpLayerBuffer[1], file_size);
                if (bytes_read == file_size) {
                    printf("Loaded tmp_layer_b1.bin (%d bytes) -> buffer[1]\r\n", file_size);
                    loaded_count++;
                } else {
                    printf("Error: Failed to read tmp_layer_b1.bin\r\n");
                    vPortFree(tmpLayerBuffer[1]);
                    tmpLayerBuffer[1] = NULL;
                }
            } else {
                printf("Error: Failed to allocate memory for tmp_layer_b1.bin\r\n");
            }
        }
        close(fd1);
    }

    // Try to load buffer[2]: tmp_layer_b2.bin
    printf("Trying to open /sdcard/ai_model/tmp_layer_b2.bin...\r\n");
    int fd2 = open("/sdcard/ai_model/tmp_layer_b2.bin", O_RDONLY);
    if (fd2 >= 0) {
        int file_size = lseek(fd2, 0, SEEK_END);
        lseek(fd2, 0, SEEK_SET);

        if (file_size > 0) {
            if (tmpLayerBuffer[2] != NULL) {
                vPortFree(tmpLayerBuffer[2]);
                tmpLayerBuffer[2] = NULL;
            }

            tmpLayerBuffer[2] = (char *)pvPortMalloc(file_size);
            if (tmpLayerBuffer[2] != NULL) {
                int bytes_read = read(fd2, tmpLayerBuffer[2], file_size);
                if (bytes_read == file_size) {
                    printf("Loaded tmp_layer_b2.bin (%d bytes) -> buffer[2]\r\n", file_size);
                    loaded_count++;
                } else {
                    printf("Error: Failed to read tmp_layer_b2.bin\r\n");
                    vPortFree(tmpLayerBuffer[2]);
                    tmpLayerBuffer[2] = NULL;
                }
            } else {
                printf("Error: Failed to allocate memory for tmp_layer_b2.bin\r\n");
            }
        }
        close(fd2);
    }

    if (loaded_count > 0) {
        printf("Successfully loaded %d temporary layer verification file(s)\r\n", loaded_count);
    } else {
        printf("No temporary layer verification files found (this is optional)\r\n");
    }

    return loaded_count;
}

/**
 * Load output layer binary files from SD card for NPU verification
 * Looks for output_layer_b.bin or output_layer_b0.bin -> buffer[0]
 * Looks for output_layer_b1.bin -> buffer[1]
 * Looks for output_layer_b2.bin -> buffer[2]
 * Looks for output_layer_b3.bin -> buffer[3]
 * Looks for output_layer_b4.bin -> buffer[4]
 * Looks for output_layer_b5.bin -> buffer[5]
 * Returns the number of buffers successfully loaded
 */
static int load_output_layer_bins(void)
{
    int loaded_count = 0;

    printf("\n=== Loading Output Layer Verification Files ===\n");

    // Try to load buffer[0]: first try output_layer_b.bin, then output_layer_b0.bin
    const char* buffer0_files[] = {
        "/sdcard/ai_model/output_layer_b.bin",
        "/sdcard/ai_model/output_layer_b0.bin"
    };

    for (int i = 0; i < 2; i++) {
        printf("Trying to open %s...\r\n", buffer0_files[i]);
        int fd = open(buffer0_files[i], O_RDONLY);
        printf("  fd = %d\r\n", fd);

        if (fd >= 0) {
            // Get file size
            int file_size = lseek(fd, 0, SEEK_END);
            lseek(fd, 0, SEEK_SET);
            printf("  file_size = %d\r\n", file_size);

            if (file_size > 0) {
                // Free old buffer if it exists
                if (outputLayerBuffer[0] != NULL) {
                    vPortFree(outputLayerBuffer[0]);
                    outputLayerBuffer[0] = NULL;
                }

                // Allocate memory
                outputLayerBuffer[0] = (char *)pvPortMalloc(file_size);
                if (outputLayerBuffer[0] != NULL) {
                    // Read file
                    int bytes_read = read(fd, outputLayerBuffer[0], file_size);
                    if (bytes_read == file_size) {
                        printf("Loaded %s (%d bytes) -> buffer[0]\r\n", buffer0_files[i], file_size);
                        loaded_count++;
                        close(fd);
                        break; // Successfully loaded, don't try the second file
                    } else {
                        printf("Error: Failed to read complete file (read %d/%d bytes)\r\n", bytes_read, file_size);
                        vPortFree(outputLayerBuffer[0]);
                        outputLayerBuffer[0] = NULL;
                    }
                } else {
                    printf("Error: Failed to allocate memory for %s\r\n", buffer0_files[i]);
                }
            } else {
                printf("  Invalid file size, trying next option...\r\n");
            }
            close(fd);
        } else {
            printf("  File not found (fd < 0), trying next option...\r\n");
        }
    }


    // Load buffers[1] through [5]: output_layer_b1.bin through output_layer_b5.bin
    for (int idx = 1; idx <= 5; idx++) {
        char filename[64];
        snprintf(filename, sizeof(filename), "/sdcard/ai_model/output_layer_b%d.bin", idx);

        printf("Trying to open %s...\r\n", filename);
        int fd = open(filename, O_RDONLY);
        if (fd >= 0) {
            int file_size = lseek(fd, 0, SEEK_END);
            lseek(fd, 0, SEEK_SET);

            if (file_size > 0) {
                if (outputLayerBuffer[idx] != NULL) {
                    vPortFree(outputLayerBuffer[idx]);
                    outputLayerBuffer[idx] = NULL;
                }

                outputLayerBuffer[idx] = (char *)pvPortMalloc(file_size);
                if (outputLayerBuffer[idx] != NULL) {
                    int bytes_read = read(fd, outputLayerBuffer[idx], file_size);
                    if (bytes_read == file_size) {
                        printf("Loaded output_layer_b%d.bin (%d bytes) -> buffer[%d]\r\n", idx, file_size, idx);
                        loaded_count++;
                    } else {
                        printf("Error: Failed to read output_layer_b%d.bin\r\n", idx);
                        vPortFree(outputLayerBuffer[idx]);
                        outputLayerBuffer[idx] = NULL;
                    }
                } else {
                    printf("Error: Failed to allocate memory for output_layer_b%d.bin\r\n", idx);
                }
            }
            close(fd);
        }
    }

    if (loaded_count > 0) {
        printf("Successfully loaded %d output layer verification file(s)\r\n", loaded_count);
    } else {
        printf("No output layer verification files found (this is optional)\r\n");
    }

    return loaded_count;
}
#endif


/**
 * AI SD Run command - Load model and run inference
 * Combines ai_sd_load flow with iproai_start_inference
 */
static int cmd_ai_sd_run(int argc, char **argv)
{
#ifdef CONFIG_AI_SUPPORT
    int fd;
    int file_size;
    uint8_t *model_buffer = NULL;
    int bytes_read;
    int img_fd;
    int img_size;

    printf("=== AI SD Run Command ===\r\n");
    printf("This command will:\r\n");
    printf("  1. Load model from SD card\r\n");
    printf("  2. Load input image (img_b.bin)\r\n");
    printf("  3. Run inference\r\n\r\n");

    // Mount SD card with EVFS support
    mount_sdcard();

    // ===== Step 1: Load img_b.bin =====
    printf("\r\n=== Step 1: Loading Input Image (img_b.bin) ===\r\n");
    printf("Opening /sdcard/ai_model/img_b.bin...\r\n");
    img_fd = open("/sdcard/ai_model/img_b.bin", O_RDONLY);
    if (img_fd < 0) {
        printf("Error: Failed to open img_b.bin (fd=%d)\r\n", img_fd);
        return -1;
    }

    img_size = lseek(img_fd, 0, SEEK_END);
    lseek(img_fd, 0, SEEK_SET);
    printf("Image file size: %d bytes\r\n", img_size);

    if (img_size <= 0) {
        printf("Error: Invalid image file size\r\n");
        close(img_fd);
        return -1;
    }

    // Free old imgBuffer if it exists
    if (imgBuffer != NULL) {
        printf("Freeing old imgBuffer...\r\n");
        vPortFree(imgBuffer);
        imgBuffer = NULL;
    }

    // Allocate memory for image buffer
    printf("Allocating %d bytes for image buffer...\r\n", img_size);
    imgBuffer = (char *)pvPortMalloc(img_size);
    if (imgBuffer == NULL) {
        printf("Error: Failed to allocate memory for image\r\n");
        close(img_fd);
        return -1;
    }

    // Read image file
    bytes_read = read(img_fd, imgBuffer, img_size);
    if (bytes_read != img_size) {
        printf("Error: Failed to read image file\r\n");
        vPortFree(imgBuffer);
        imgBuffer = NULL;
        close(img_fd);
        return -1;
    }
    printf("Image loaded successfully\r\n");
    close(img_fd);

    // ===== Step 1.5: Load Temporary Layer Verification Files (Optional) =====
    int tmp_layer_count = load_tmp_layer_bins();
    if (tmp_layer_count > 0) {
        printf("\r\n=== Initializing Temporary Layer Verification ===\r\n");
        printf("Calling iproai_tmp_layer_bin_init with %d buffer(s)...\r\n", tmp_layer_count);
#ifdef CONFIG_EVA_BIN_MODE
        // Cast tmpLayerBuffer to uint8_t* array as expected by iproai_tmp_layer_bin_init
        iproai_tmp_layer_bin_init((uint8_t**)tmpLayerBuffer);
#else
		printf("Verification mode need to define CONFIG_EVA_BIN_MODE\r\n");
		return -1;
#endif
        printf("Temporary layer verification initialized\r\n");
    }

    // ===== Step 1.6: Load Output Layer Verification Files (Optional) =====
    int output_layer_count = load_output_layer_bins();
    if (output_layer_count > 0) {
        printf("\r\n=== Initializing Output Layer Verification ===\r\n");
        printf("Calling iproai_output_layer_bin_init with %d buffer(s)...\r\n", output_layer_count);
#ifdef CONFIG_EVA_BIN_MODE
		// Cast outputLayerBuffer to uint32_t as expected by iproai_output_layer_bin_init
        iproai_output_layer_bin_init((uint32_t)(uintptr_t)outputLayerBuffer);
#else
		printf("Verification mode need to define CONFIG_EVA_BIN_MODE\r\n");
		return -1;
#endif
        printf("Output layer verification initialized\r\n");
    }

    // ===== Step 2: Load Model =====
    printf("\r\n=== Step 2: Loading Model (model.inm) ===\r\n");
    printf("Opening /sdcard/ai_model/model.inm...\r\n");
    fd = open("/sdcard/ai_model/model.inm", O_RDONLY);
    if (fd < 0) {
        printf("Error: Failed to open model file (fd=%d)\r\n", fd);
        return -1;
    }

    file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    printf("Model file size: %d bytes\r\n", file_size);

    if (file_size <= 0) {
        printf("Error: Invalid file size\r\n");
        close(fd);
        return -1;
    }

    // Allocate memory for model
    printf("Allocating %d bytes for model buffer...\r\n", file_size);
    model_buffer = (uint8_t *)pvPortMalloc(file_size);
    if (model_buffer == NULL) {
        printf("Error: Failed to allocate memory for model\r\n");
        close(fd);
        return -1;
    }

    // Read model file
    bytes_read = read(fd, model_buffer, file_size);
    if (bytes_read != file_size) {
        printf("Error: Failed to read model file\r\n");
        vPortFree(model_buffer);
        close(fd);
        return -1;
    }
    close(fd);
    printf("Model loaded into memory\r\n");

    // Free previous model if exists
    if (g_model_hdl != NULL) {
        printf("Freeing previous model memory...\r\n");
        iproai_free(g_model_hdl);
        g_model_hdl = NULL;
        printf("Free heap after model free: %zu bytes\r\n", xPortGetFreeHeapSize());
    }

    // Create new IPROAI model handle
    printf("Creating IPROAI model handle...\r\n");
    g_model_hdl = iproai_create();
    if (g_model_hdl == NULL) {
        printf("Error: Failed to create IPROAI model handle\r\n");
        vPortFree(model_buffer);
        return -1;
    }

    // Load model into NPU
    printf("Loading model into NPU...\r\n");
    IPROAI_Status_e status = iproai_load_model_from_buffer(g_model_hdl, model_buffer);

    // Free the temporary model buffer
    vPortFree(model_buffer);
    printf("Temporary model buffer freed\r\n");

    if (status != IPROAI_STATUS_NO_ERROR) {
        printf("ERROR: Failed to load model (status=%d)\r\n", status);
        return -1;
    }
    printf("Model loaded successfully\r\n");

    // Get model info
    uint32_t width = 0, height = 0;
    if (iproai_getInputResolution(g_model_hdl, &width, &height) == IPROAI_STATUS_NO_ERROR) {
        printf("Model input resolution: %lux%lu\r\n", width, height);
    }

    // ===== Step 3: Run Inference =====
    printf("\r\n=== Step 3: Starting Inference ===\r\n");
    printf("Available heap: %zu bytes\r\n", xPortGetFreeHeapSize());

    iproai_start_inference(g_model_hdl);

    // Wait a bit for the inference task to start and print its initial messages
    /*vTaskDelay(pdMS_TO_TICKS(100));

    printf("\r\n=== AI SD Run Complete ===\r\n");
    printf("Inference task has been started in background\r\n");
    printf("Monitor the output above for inference results\r\n");
    printf("Note: Inference is running in a separate thread\r\n");*/

#else
    printf("AI support not enabled in configuration\r\n");
#endif
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ai_sd_run, ai_sd_run, Load model and run inference);

/**
 * AI Run Demo command - Load and run different AI models based on index
 * Usage: ai_run_demo <index>
 *   0: Person Detection
 *   1: Person + Pet Detection
 *   2: Pet Detection
 *   3: Face Detection
 */
static int cmd_ai_run_demo(int argc, char **argv)
{
#ifdef CONFIG_AI_SUPPORT
    int model_index;
    int fd;
    int file_size;
    uint8_t *model_buffer = NULL;
    int bytes_read;
    int img_fd;
    int img_size;
    const ai_model_config_t *config;

    // 檢查參數
    if (argc < 2) {
        printf("Usage: ai_run_demo <index>\r\n");
        printf("Available models:\r\n");
        for (int i = 0; i < AI_MODEL_MAX; i++) {
            printf("  %d) %s\r\n", i, g_model_configs[i].description);
        }
        return -1;
    }

    // 解析模型索引
    model_index = atoi(argv[1]);
    if (model_index < 0 || model_index >= AI_MODEL_MAX) {
        printf("Error: Invalid model index %d\r\n", model_index);
        printf("Valid range: 0 - %d\r\n", AI_MODEL_MAX - 1);
        return -1;
    }

    config = &g_model_configs[model_index];

    printf("=== AI Run Demo - Model %d ===\r\n", model_index);
    printf("Model: %s\r\n", config->description);
    printf("Model path: %s\r\n", config->model_path);
    printf("Image path: %s\r\n", config->img_path);
    printf("Model layers: %lu\r\n\r\n", (unsigned long)config->model_layer_num);

    // ===== Step 1: 掛載 SD 卡 =====
    printf("Step 1: Mounting SD card...\r\n");
    mount_sdcard();

    // ===== Step 2: 載入輸入圖像 =====
    printf("\r\nStep 2: Loading input image (%s)...\r\n", config->img_path);
    img_fd = open(config->img_path, O_RDONLY);
    if (img_fd < 0) {
        printf("Warning: Failed to open image file (fd=%d)\r\n", img_fd);
        printf("Trying default image: /sdcard/ai_model/img_b.bin\r\n");

        // 嘗試使用預設圖像
        img_fd = open("/sdcard/ai_model/img_b.bin", O_RDONLY);
        if (img_fd < 0) {
            printf("Error: No valid input image found\r\n");
            return -1;
        }
    }

    img_size = lseek(img_fd, 0, SEEK_END);
    lseek(img_fd, 0, SEEK_SET);
    printf("Image file size: %d bytes\r\n", img_size);

    if (img_size <= 0) {
        printf("Error: Invalid image file size\r\n");
        close(img_fd);
        return -1;
    }

    // 釋放舊的圖像緩衝區
    if (imgBuffer != NULL) {
        printf("Freeing old imgBuffer...\r\n");
        vPortFree(imgBuffer);
        imgBuffer = NULL;
    }

    // 分配圖像緩衝區
    printf("Allocating %d bytes for image buffer...\r\n", img_size);
    imgBuffer = (char *)pvPortMalloc(img_size);
    if (imgBuffer == NULL) {
        printf("Error: Failed to allocate memory for image\r\n");
        close(img_fd);
        return -1;
    }

    // 讀取圖像檔案
    bytes_read = read(img_fd, imgBuffer, img_size);
    if (bytes_read != img_size) {
        printf("Error: Failed to read image file\r\n");
        vPortFree(imgBuffer);
        imgBuffer = NULL;
        close(img_fd);
        return -1;
    }
    printf("Input file loaded successfully\r\n");
    close(img_fd);

    // ===== 語者識別特殊處理：PCM -> Features =====
#if AUDIO_ENABLE_MFCC_PREPROCESS
    if (model_index == AI_MODEL_AUDIO_RECOG) {
        printf("\r\n[Audio] Preprocessing raw PCM to MFCC features...\r\n");

        // 確保音頻前處理模組已初始化
        if (speaker_recog_preprocess_init(NULL) != 0) {
            printf("[Audio] ERROR: Failed to initialize audio preprocessing\r\n");
            vPortFree(imgBuffer);
            imgBuffer = NULL;
            return -1;
        }

        // PCM 數據直接從 imgBuffer 讀取（無 header）
        int16_t *pcm_data = (int16_t *)imgBuffer;
        int pcm_samples = img_size / sizeof(int16_t);

        printf("[Audio] PCM samples: %d (%.3f sec @ 16kHz)\r\n",
               pcm_samples, (float)pcm_samples / 16000.0f);

        // 分配特徵緩衝區 [num_frames][n_mels]
        int feature_size = speaker_recog_preprocess_get_output_size();
        float *feature_buffer = (float *)pvPortMalloc(feature_size);
        if (feature_buffer == NULL) {
            printf("[Audio] ERROR: Failed to allocate feature buffer (%d bytes)\r\n", feature_size);
            vPortFree(imgBuffer);
            imgBuffer = NULL;
            return -1;
        }

        // PCM -> MFCC 特徵
        int num_frames = speaker_recog_preprocess_pcm(pcm_data, pcm_samples, feature_buffer, feature_size);
        if (num_frames <= 0) {
            printf("[Audio] ERROR: Audio preprocessing failed\r\n");
            vPortFree(feature_buffer);
            vPortFree(imgBuffer);
            imgBuffer = NULL;
            return -1;
        }

        // 釋放 PCM buffer，使用 feature buffer 作為輸入
        vPortFree(imgBuffer);
        imgBuffer = (char *)feature_buffer;
        img_size = feature_size;

        printf("[Audio] Feature extraction complete. %d frames, output size: %d bytes\r\n",
               num_frames, feature_size);
    }
#else
    // MFCC 前處理停用：輸入已經是模型特徵格式
    if (model_index == AI_MODEL_AUDIO_RECOG) {
        printf("\r\n[Audio] Using pre-processed features (MFCC disabled)\r\n");
        printf("[Audio] Input size: %d bytes\r\n", img_size);
    }
#endif

    // ===== Step 3: 載入 AI 模型 =====
    printf("\r\nStep 3: Loading AI model (%s)...\r\n", config->model_path);
    fd = open(config->model_path, O_RDONLY);
    if (fd < 0) {
        printf("Error: Failed to open model file (fd=%d)\r\n", fd);
        printf("Make sure %s exists on SD card\r\n", config->model_path);
        return -1;
    }

    file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    printf("Model file size: %d bytes\r\n", file_size);

    if (file_size <= 0) {
        printf("Error: Invalid model file size\r\n");
        close(fd);
        return -1;
    }

    // 分配模型緩衝區
    printf("Allocating %d bytes for model buffer...\r\n", file_size);
    printf("Free heap before allocation: %zu bytes\r\n", xPortGetFreeHeapSize());

    model_buffer = (uint8_t *)pvPortMalloc(file_size);
    if (model_buffer == NULL) {
        printf("Error: Failed to allocate memory for model\r\n");
        close(fd);
        return -1;
    }

    // 讀取模型檔案
    bytes_read = read(fd, model_buffer, file_size);
    if (bytes_read != file_size) {
        printf("Error: Failed to read model file\r\n");
        vPortFree(model_buffer);
        close(fd);
        return -1;
    }
    close(fd);
    printf("Model loaded into memory\r\n");

    // 等待之前的推理任務完成
    if (g_iproai_task_started) {
        printf("Waiting for previous inference task to finish...\r\n");
        int wait_timeout = 50;  // 5 秒超時
        while (g_iproai_task_started && wait_timeout > 0) {
            vTaskDelay(pdMS_TO_TICKS(100));
            wait_timeout--;
        }
        if (g_iproai_task_started) {
            printf("WARNING: Previous task did not finish in time\r\n");
        } else {
            printf("Previous inference task finished\r\n");
        }
    }

    // 如果已有舊模型，先釋放
    if (g_model_hdl != NULL) {
        printf("Freeing previous model memory...\r\n");
        iproai_free(g_model_hdl);
        g_model_hdl = NULL;
        printf("Free heap after model free: %zu bytes\r\n", xPortGetFreeHeapSize());
    }

    // 創建新的 IPROAI 模型句柄
    printf("Creating IPROAI model handle...\r\n");
    g_model_hdl = iproai_create();
    if (g_model_hdl == NULL) {
        printf("Error: Failed to create IPROAI model handle\r\n");
        vPortFree(model_buffer);
        return -1;
    }

    // 載入模型到 NPU
    printf("Loading model into NPU...\r\n");
    IPROAI_Status_e status = iproai_load_model_from_buffer(g_model_hdl, model_buffer);

    // 釋放臨時模型緩衝區
    vPortFree(model_buffer);
    printf("Temporary model buffer freed\r\n");
    printf("Free heap after loading: %zu bytes\r\n", xPortGetFreeHeapSize());

    if (status != IPROAI_STATUS_NO_ERROR) {
        printf("ERROR: Failed to load model (status=%d)\r\n", status);
        return -1;
    }
    printf("Model loaded successfully into NPU\r\n");

    // 獲取模型資訊
    uint32_t width = 0, height = 0;
    if (iproai_getInputResolution(g_model_hdl, &width, &height) == IPROAI_STATUS_NO_ERROR) {
        printf("Model input resolution: %lux%lu\r\n", width, height);
    }

    // ===== Step 4: 啟動推理 =====
    printf("\r\nStep 4: Starting inference...\r\n");
    printf("Available heap: %zu bytes\r\n", xPortGetFreeHeapSize());

    // 設定當前模型索引（用於後處理）
    g_current_model_index = model_index;

    iproai_start_inference(g_model_hdl);

    printf("\r\n=== AI Run Demo Complete ===\r\n");
    printf("Model %d (%s) is now running\r\n", model_index, config->description);
    printf("Monitor the output for inference results\r\n");

#else
    printf("AI support not enabled in configuration\r\n");
#endif
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ai_run_demo, ai_run_demo, Run AI demo);

/**
 * AI Run External - 從 external buffer queue 連續推理
 * Usage:
 *   ai_run_ext <model_index>				- 推理模型
 *   ai_run_ext stop                        - 停止推理
 */
static int cmd_ai_run_ext(int argc, char **argv)
{
#ifdef CONFIG_AI_SUPPORT
    // ===== 處理停止命令 =====
    if (argc >= 2 && strcmp(argv[1], "stop") == 0) {
        if (g_ext_event_group == NULL || g_ext_task_started == 0) {
            printf("No external inference task is running\r\n");
            return -1;
        }

        printf("Stopping external buffer inference...\r\n");
        xEventGroupSetBits(g_ext_event_group, EXT_BUFFER_EVENT_STOP);

        // 等待任務結束（最多 3 秒）
        int timeout = 30;  // 3 秒（每次 100ms）
        while (g_ext_task_started && timeout > 0) {
            vTaskDelay(pdMS_TO_TICKS(100));
            timeout--;
        }

        if (g_ext_task_started == 0) {
            printf("External inference task stopped successfully\r\n");
            xEventGroupClearBits(g_ext_event_group, EXT_BUFFER_EVENT_STOP);
        } else {
            printf("WARNING: Task did not stop within timeout\r\n");
        }
        return -1;
    }

    int model_index;
    int fd;
    int file_size;
    uint8_t *model_buffer = NULL;
    int bytes_read;
    int img_fd;
    int img_size;
    const ai_model_config_t *config;
    uint32_t ext_addr;

    // 檢查參數
    if (argc < 2) {
        printf("=== AI Run External - Usage ===\r\n\r\n");
        printf("Description:\r\n");
        printf("  Run AI inference using external buffer address from NPU register.\r\n");
        printf("  Automatically reads address from NPU APU_DM2_ADDR register.\r\n\r\n");
        printf("Usage:\r\n");
        printf("  ai_run_ext <model_index>                  - Run inference (recognize mode)\r\n");
        printf("  ai_run_ext <model_index> register <name>  - Run inference (register speaker)\r\n");
        printf("  ai_run_ext stop                           - Stop running inference task\r\n\r\n");
        printf("Available models:\r\n");
        for (int i = 0; i < AI_MODEL_MAX; i++) {
            printf("  [%d] %s\r\n", i, g_model_configs[i].description);
        }
        printf("\r\nExamples:\r\n");
        printf("  ai_run_ext 4              - Run speaker recognition\r\n");
        printf("  ai_run_ext 4 register Alice - Register speaker named Alice\r\n");
        printf("  ai_run_ext stop           - Stop inference\r\n\r\n");
        printf("Note: Model is loaded only once; subsequent calls reuse the loaded model.\r\n");
        return -1;
    }

    // 解析模型索引
    model_index = atoi(argv[1]);
    if (model_index < 0 || model_index >= AI_MODEL_MAX) {
        printf("Error: Invalid model index %d\r\n", model_index);
        printf("Valid range: 0 - %d\r\n", AI_MODEL_MAX - 1);
        return -1;
    }

    config = &g_model_configs[model_index];
    const char *input_path = NULL;
    printf("=== AI Run External ===\r\n");
    printf("Model: %s\r\n", config->description);
    printf("Model path: %s\r\n", config->model_path);
    printf("Image path: %s\r\n\r\n", config->img_path);

    // ===== Audio Model: 模式選擇（從參數）=====
    if (model_index == AI_MODEL_AUDIO_RECOG || model_index == AI_MODEL_SPEAKER_RECOG) {
        if (argc >= 4 && strcmp(argv[2], "register") == 0) {
            g_audio_mode = AUDIO_MODE_REGISTER;
            strncpy(g_register_speaker_name, argv[3], SPEAKER_RECOG_MAX_NAME_LEN - 1);
            g_register_speaker_name[SPEAKER_RECOG_MAX_NAME_LEN - 1] = '\0';
            printf("Mode: Register  Speaker: %s\r\n", g_register_speaker_name);
        } else {
            g_audio_mode = AUDIO_MODE_RECOGNIZE;
            printf("Mode: Recognize\r\n");
        }
        // register
        if (argc >= 5 && strcmp(argv[4], "img_path") == 0) {
            input_path = argv[5];
            printf("Dynamic Image path: %s\r\n\r\n", input_path);
        } else {
            // non register moode
            if (argc >= 4 && strcmp(argv[2], "img_path") == 0) {
                input_path = argv[3];
                printf("Dynamic Image path: %s\r\n\r\n", input_path);
            } else {
                input_path = config->img_path;
            }
        }

    }

    // ===== Step 1: 掛載 SD 卡 =====
    printf("Step 1: Mounting SD card...\r\n");
    mount_sdcard();

    // ===== Step 2: 載入輸入圖像到 imgBuffer =====
    if(input_path == NULL) {
        input_path = config->img_path;
    }
    printf("\r\nStep 2: Loading input image (%s)...\r\n", input_path);
    img_fd = open(input_path, O_RDONLY);
    if (img_fd < 0) {
        // 嘗試使用預設圖像
        img_fd = open(config->img_path, O_RDONLY);
        printf("\r\nStep 2: Trying default image: (%s)...\r\n", config->img_path);
        if (img_fd < 0) {
            printf("Error: No valid input image found\r\n");
            return -1;
        }
    }

    img_size = lseek(img_fd, 0, SEEK_END);
    lseek(img_fd, 0, SEEK_SET);
    printf("Image file size: %d bytes\r\n", img_size);

    if (img_size <= 0) {
        printf("Error: Invalid image file size\r\n");
        close(img_fd);
        return -1;
    }

    // 釋放舊的圖像緩衝區
    if (imgBuffer != NULL) {
        printf("Freeing old imgBuffer...\r\n");
        vPortFree(imgBuffer);
        imgBuffer = NULL;
    }

    // 分配圖像緩衝區
    printf("Allocating %d bytes for image buffer...\r\n", img_size);
    imgBuffer = (char *)pvPortMalloc(img_size);
    if (imgBuffer == NULL) {
        printf("Error: Failed to allocate memory for image\r\n");
        close(img_fd);
        return -1;
    }

    // 讀取圖像檔案到 imgBuffer
    bytes_read = read(img_fd, imgBuffer, img_size);
    if (bytes_read != img_size) {
        printf("Error: Failed to read image file\r\n");
        vPortFree(imgBuffer);
        imgBuffer = NULL;
        close(img_fd);
        return -1;
    }
    printf("Input file loaded successfully to imgBuffer\r\n");
    close(img_fd);

    // ===== 語者識別特殊處理：PCM -> Features =====
    // SPEAKER_RECOG(5): Kaldi fbank front-end — res2dnet expects float[300][80].
    if (model_index == AI_MODEL_SPEAKER_RECOG) {
        if (g_spk_fbank == NULL) {
            g_spk_fbank = (FBankContext *)pvPortMalloc(sizeof(FBankContext));
            if (g_spk_fbank == NULL) {
                printf("[SpkID] ERROR: fbank ctx alloc failed (%u bytes)\r\n",
                       (unsigned)sizeof(FBankContext));
                vPortFree(imgBuffer); imgBuffer = NULL; return -1;
            }
            fbank_init(g_spk_fbank);
        }
        int16_t *pcm_data = (int16_t *)imgBuffer;
        int pcm_samples = img_size / (int)sizeof(int16_t);
#if SPK_DUMP_OUTPUT
        printf("[SpkID] PCM samples: %d (%.3f sec @ 16kHz)\r\n",
               pcm_samples, (float)pcm_samples / 16000.0f);
#endif

        int feat_bytes = FBANK_SEQ_LEN * FBANK_N_MELS * (int)sizeof(float);
        float (*feat)[FBANK_N_MELS] =
            (float (*)[FBANK_N_MELS])pvPortMalloc(feat_bytes);
        if (feat == NULL) {
            printf("[SpkID] ERROR: feature buffer alloc failed (%d bytes)\r\n", feat_bytes);
            vPortFree(imgBuffer); imgBuffer = NULL; return -1;
        }
        int vf = fbank_compute(g_spk_fbank, pcm_data, pcm_samples, feat);
        (void)vf;
#if SPK_DUMP_OUTPUT
        printf("[SpkID] fbank: %d valid frames -> [%d][%d]\r\n",
               vf, FBANK_SEQ_LEN, FBANK_N_MELS);
#endif

        /* float features stay in imgBuffer; quantised to int8 after model load */
        vPortFree(imgBuffer);
        imgBuffer = (char *)feat;
        img_size  = feat_bytes;
    }
    else
#if AUDIO_ENABLE_MFCC_PREPROCESS
    if (model_index == AI_MODEL_AUDIO_RECOG) {
        printf("\r\n[Audio] Preprocessing raw PCM to MFCC features...\r\n");

        // 確保音頻前處理模組已初始化
        if (speaker_recog_preprocess_init(NULL) != 0) {
            printf("[Audio] ERROR: Failed to initialize audio preprocessing\r\n");
            vPortFree(imgBuffer);
            imgBuffer = NULL;
            return -1;
        }

        // PCM 數據直接從 imgBuffer 讀取（無 header）
        int16_t *pcm_data = (int16_t *)imgBuffer;
        int pcm_samples = img_size / sizeof(int16_t);

        printf("[Audio] PCM samples: %d (%.3f sec @ 16kHz)\r\n",
               pcm_samples, (float)pcm_samples / 16000.0f);

        // 分配特徵緩衝區 [num_frames][n_mels]
        int feature_size = speaker_recog_preprocess_get_output_size();
        float *feature_buffer = (float *)pvPortMalloc(feature_size);
        if (feature_buffer == NULL) {
            printf("[Audio] ERROR: Failed to allocate feature buffer (%d bytes)\r\n", feature_size);
            vPortFree(imgBuffer);
            imgBuffer = NULL;
            return -1;
        }

        // PCM -> MFCC 特徵
        int num_frames = speaker_recog_preprocess_pcm(pcm_data, pcm_samples, feature_buffer, feature_size);
        if (num_frames <= 0) {
            printf("[Audio] ERROR: Audio preprocessing failed\r\n");
            vPortFree(feature_buffer);
            vPortFree(imgBuffer);
            imgBuffer = NULL;
            return -1;
        }

        // 釋放 PCM buffer，使用 feature buffer 作為輸入
        vPortFree(imgBuffer);
        imgBuffer = (char *)feature_buffer;
        img_size = feature_size;

        printf("[Audio] Feature extraction complete. %d frames, output size: %d bytes\r\n",
               num_frames, feature_size);

        // 列印前 3 frames 的前 10 個 mel bin 數值
        int n_mels = feature_size / (num_frames * sizeof(float));
        printf("[Audio] MFCC output (float, first 3 frames x 10 bins):\r\n");
        for (int f = 0; f < 3 && f < num_frames; f++) {
            printf("  frame[%d]: ", f);
            for (int m = 0; m < 10 && m < n_mels; m++) {
                printf("%6.3f ", feature_buffer[f * n_mels + m]);
            }
            printf("\r\n");
        }

        // float features 暫存於 imgBuffer，待模型載入後量化為 int8 再設 NPU 地址
    }
    else
#endif
    if (model_index == AI_MODEL_AUDIO_RECOG) {
        // MFCC 前處理停用：輸入已經是模型特徵格式
        printf("\r\n[Audio] Using pre-processed features (MFCC disabled)\r\n");
        printf("[Audio] Input size: %d bytes\r\n", img_size);

        // === Debug: 顯示輸入數值 (int8 格式) ===
        int8_t *input_i8 = (int8_t *)imgBuffer + 4;
        int num_bytes = img_size;
        printf("\r\n[Audio Debug] Input verification:\r\n");
        printf("  Total bytes (int8): %d (expected: %d = 80*345*4)\r\n", num_bytes, 80*345*4);
        printf("  Input buffer address: 0x%08lx\r\n", (unsigned long)imgBuffer + 4);

        // 顯示前 20 個 int8 數值
        printf("\r\n  First 20 input values (int8):\r\n  ");
        for (int i = 0; i < 20 && i < num_bytes; i++) {
            printf("%4d ", input_i8[i]);
            if ((i + 1) % 10 == 0) printf("\r\n  ");
        }
        printf("\r\n");

        // 音頻模型：直接使用輸入（無 4-byte header）
        ext_addr = (uint32_t)imgBuffer + 4;
        printf("\r\nConfiguring NPU external address (audio features)...\r\n");
        NPU_Img_Ext_Addr_Cfg(ext_addr);
        printf("NPU external address configured: 0x%08lx\r\n\r\n", (unsigned long)ext_addr);
    }
    else {
        // 圖像模型：img.bin 格式有 4-byte header
        uint32_t img_data_size = *((uint32_t*)imgBuffer);
        uint8_t* img_data_addr = (uint8_t*)imgBuffer + 4;

        printf("Image file format:\r\n");
        printf("  Header (4 bytes): size = %lu bytes\r\n", (unsigned long)img_data_size);
        printf("  imgBuffer base address: 0x%08lx\r\n", (unsigned long)imgBuffer);
        printf("  Actual image data starts at: 0x%08lx (imgBuffer + 4)\r\n", (unsigned long)img_data_addr);

        // 使用 imgBuffer + 4 作為 external address（跳過前4字節）
        ext_addr = (uint32_t)img_data_addr;
        printf("\r\nConfiguring NPU external address (skipping 4-byte header)...\r\n");
        NPU_Img_Ext_Addr_Cfg(ext_addr);
        printf("NPU external address configured: 0x%08lx\r\n\r\n", (unsigned long)ext_addr);
    }

    // ===== Step 3: 創建 Queue 和 Event Group =====
    if (g_ext_buffer_queue == NULL) {
        g_ext_buffer_queue = xQueueCreate(EXT_BUFFER_QUEUE_SIZE, sizeof(uint32_t));
        if (g_ext_buffer_queue == NULL) {
            printf("ERROR: Failed to create external buffer queue\r\n");
            return -1;
        }
        printf("External buffer queue created (size: %d)\r\n", EXT_BUFFER_QUEUE_SIZE);
    }

#ifdef CONFIG_MM_SUPPORT
    if (g_result_queue == NULL) {
        g_result_queue = xQueueCreate(RESULT_QUEUE_SIZE, sizeof(uint32_t));
        if (g_result_queue == NULL) {
            printf("ERROR: Failed to create result queue\r\n");
            return -1;
        }
        printf("Result queue created (size: %d)\r\n", RESULT_QUEUE_SIZE);
    }
#endif

    if (g_ext_event_group == NULL) {
        g_ext_event_group = xEventGroupCreate();
        if (g_ext_event_group == NULL) {
            printf("ERROR: Failed to create event group\r\n");
            return -1;
        }
        printf("Event group created\r\n");
        xEventGroupClearBits(g_ext_event_group, EXT_BUFFER_EVENT_STOP);
    }

    // ===== Step 4: 載入模型（如果已載入相同模型則跳過）=====
    bool need_model_load = (g_loaded_model_index != model_index || g_model_hdl == NULL);

    if (need_model_load) {
        printf("\r\nStep 4: Loading model (%s)...\r\n", config->model_path);
        fd = open(config->model_path, O_RDONLY);
        if (fd < 0) {
            printf("Error: Failed to open model file (fd=%d)\r\n", fd);
            return -1;
        }

        file_size = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);
        printf("Model file size: %d bytes\r\n", file_size);

        if (file_size <= 0) {
            printf("Error: Invalid model file size\r\n");
            close(fd);
            return -1;
        }

        model_buffer = (uint8_t *)pvPortMalloc(file_size);
        if (model_buffer == NULL) {
            printf("Error: Failed to allocate %d bytes for model\r\n", file_size);
            close(fd);
            return -1;
        }
        printf("Model buffer allocated: %d bytes\r\n", file_size);
        printf("Free heap after allocation: %zu bytes\r\n", xPortGetFreeHeapSize());

        bytes_read = read(fd, model_buffer, file_size);
        if (bytes_read != file_size) {
            printf("Error: Failed to read model file\r\n");
            vPortFree(model_buffer);
            close(fd);
            return -1;
        }
        close(fd);
        printf("Model loaded into memory\r\n");
    } else {
        printf("\r\nStep 4: Model %d already loaded, skipping reload\r\n", model_index);
        model_buffer = NULL;
    }

    // 停止之前的 ext 推理任務（如果有）
    if (g_ext_task_started && g_ext_event_group != NULL) {
        printf("Stopping previous ext inference task...\r\n");
        xEventGroupSetBits(g_ext_event_group, EXT_BUFFER_EVENT_STOP);
        int stop_timeout = 30;  // 3 秒超時
        while (g_ext_task_started && stop_timeout > 0) {
            vTaskDelay(pdMS_TO_TICKS(100));
            stop_timeout--;
        }
        if (g_ext_task_started == 0) {
            printf("Previous ext inference task stopped\r\n");
            xEventGroupClearBits(g_ext_event_group, EXT_BUFFER_EVENT_STOP);
        } else {
            printf("WARNING: Ext task did not stop in time\r\n");
        }
    }

    // 等待之前的普通推理任務完成（如果有）
    if (g_iproai_task_started) {
        printf("Waiting for previous inference task to finish...\r\n");
        int wait_timeout = 50;  // 5 秒超時
        while (g_iproai_task_started && wait_timeout > 0) {
            vTaskDelay(pdMS_TO_TICKS(100));
            wait_timeout--;
        }
        if (g_iproai_task_started) {
            printf("WARNING: Previous task did not finish in time\r\n");
        } else {
            printf("Previous inference task finished\r\n");
        }
    }

    if (need_model_load) {
        // 如果已有舊模型，先釋放
        if (g_model_hdl != NULL) {
            printf("Freeing previous model memory...\r\n");
            iproai_free(g_model_hdl);
            g_model_hdl = NULL;
            g_loaded_model_index = -1;
            printf("Free heap after model free: %zu bytes\r\n", xPortGetFreeHeapSize());
        }

        // 創建新的 IPROAI 模型句柄
        printf("Creating IPROAI model handle...\r\n");
        g_model_hdl = iproai_create();
        if (g_model_hdl == NULL) {
            printf("Error: Failed to create IPROAI model handle\r\n");
            vPortFree(model_buffer);
            return -1;
        }

        // 載入模型到 NPU
        printf("Loading model into NPU...\r\n");
        IPROAI_Status_e status = iproai_load_model_from_buffer(g_model_hdl, model_buffer);

        // 釋放臨時模型緩衝區
        vPortFree(model_buffer);
        printf("Temporary model buffer freed\r\n");
        printf("Free heap after loading: %zu bytes\r\n", xPortGetFreeHeapSize());

        if (status != IPROAI_STATUS_NO_ERROR) {
            printf("ERROR: Failed to load model (status=%d)\r\n", status);
            return -1;
        }
        printf("Model loaded successfully into NPU\r\n");
        g_loaded_model_index = model_index;
    } else {
        printf("Reusing existing model handle\r\n");
    }

    // 獲取模型資訊
    uint32_t width = 0, height = 0;
    if (iproai_getInputResolution(g_model_hdl, &width, &height) == IPROAI_STATUS_NO_ERROR) {
        printf("Model input resolution: %lux%lu\r\n", width, height);
    }

#if AUDIO_ENABLE_MFCC_PREPROCESS
    // ===== MFCC float32 → int8 量化（Option B：模型載入後讀取 scale/zero_point） =====
    if (model_index == AI_MODEL_AUDIO_RECOG && imgBuffer != NULL) {
        // TFLite quant: float = 0.07030236721038818 * (q - (-3))
        const float q_scale = 0.07030236721038818f;
        const int32_t q_zero_point = -3-128;
        printf("[Audio] Input quant params (hardcoded): scale=%.10f, zero_point=%ld\r\n",
               q_scale, (long)q_zero_point);

        int num_floats = img_size / sizeof(float);
        float *float_buf = (float *)imgBuffer;

        printf("[Audio] Float MFCC (first 10): ");
        for (int qi = 0; qi < 10 && qi < num_floats; qi++)
            printf("%.3f ", float_buf[qi]);
        printf("\r\n");

        // 分配 NPU input buffer：每個值佔 4 bytes (int8 值放低位 byte，其餘補 0)
        int npu_buf_size = num_floats * (int)sizeof(uint32_t);
        uint32_t *npu_buf = (uint32_t *)pvPortMalloc(npu_buf_size);
        if (npu_buf == NULL) {
            printf("ERROR: Failed to allocate NPU input buffer (%d bytes)\r\n", npu_buf_size);
            vPortFree(imgBuffer);
            imgBuffer = NULL;
            return -1;
        }

        // 量化: int8 = clamp(round(float / scale) + zero_point, -128, 127)
        // 每個 int8 值存為 4-byte word：低位 byte 放 uint8 表示，高位 3 bytes 補 0
        printf("[Audio] Quantization detail (first 20 values):\r\n");
        printf("  idx |   float   |  /scale   |  +zp(raw) | rounded | clamped |     hex\r\n");
        for (int i = 0; i < num_floats; i++) {
            float divided = float_buf[i] / q_scale;
            float qf = divided + (float)q_zero_point;
            int32_t qi = (int32_t)(qf >= 0.0f ? qf + 0.5f : qf - 0.5f);
            if (qi < -128) qi = -128;
            if (qi > 127)  qi = 127;
            int8_t i8val = (int8_t)qi;
            npu_buf[i] = (uint32_t)(uint8_t)i8val;  // 低位 byte 放值，其餘補 0
            if (i < 20) {
                printf("  %3d | %9.4f | %9.4f | %9.4f | %7d | %7d | %08lx\r\n",
                       i, float_buf[i], divided, qf,
                       (int)(qf >= 0.0f ? qf + 0.5f : qf - 0.5f),
                       (int)i8val, (unsigned long)npu_buf[i]);
            }
        }
        printf("[Audio] NPU input (first 20, hex): ");
        for (int i = 0; i < 20 && i < num_floats; i++)
            printf("%08lx ", (unsigned long)npu_buf[i]);
        printf("\r\n");

        // 替換 imgBuffer：釋放 float 緩衝區，改用 NPU input buffer
        vPortFree(imgBuffer);
        imgBuffer = (char *)npu_buf;
        img_size = npu_buf_size;

        // 設定 NPU external address（需 cache flush）
        L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)imgBuffer, img_size);
        ext_addr = (uint32_t)imgBuffer;
        NPU_Img_Ext_Addr_Cfg(ext_addr);
        printf("[Audio] NPU ext addr: 0x%08lx, size=%d (%d values x 4 bytes)\r\n",
               (unsigned long)ext_addr, img_size, num_floats);
    }
#endif

    // ===== SPEAKER_RECOG(5): fbank float32 → CONTIGUOUS uint8 (offset-binary), via ext-addr =====
    // res2dnet wants CONTIGUOUS uint8 (1 byte/value, byte = int8 + 128). fbank output is
    // fed straight to the NPU in memory (no file → no 4-byte size header like img_b.bin).
    if (model_index == AI_MODEL_SPEAKER_RECOG && imgBuffer != NULL) {
        const float   q_scale = SPK_IN_SCALE;
        const int32_t q_zp    = SPK_IN_ZP;
        int    num_vals  = img_size / (int)sizeof(float);   /* 300*80 = 24000 */
        float *float_buf = (float *)imgBuffer;

        uint8_t *u8 = (uint8_t *)pvPortMalloc(num_vals);    /* 1 byte per value */
        if (u8 == NULL) {
            printf("[SpkID] ERROR: input buffer alloc failed (%d bytes)\r\n", num_vals);
            vPortFree(imgBuffer); imgBuffer = NULL; return -1;
        }
        for (int i = 0; i < num_vals; i++) {
            float qf = float_buf[i] / q_scale + (float)q_zp;
            int32_t qi = (int32_t)(qf >= 0.0f ? qf + 0.5f : qf - 0.5f);
            if (qi < -128) qi = -128;
            if (qi >  127) qi =  127;
            u8[i] = (uint8_t)(qi + 128);                    /* offset-binary */
        }
#if SPK_DUMP_OUTPUT
        printf("[SpkID] quant scale=%.8f zp=%d\r\n", q_scale, (int)q_zp);
        printf("[SpkID]   signed int8 first 20 (cmp input_int8.csv): ");
        for (int i = 0; i < 20 && i < num_vals; i++) printf("%d ", (int)u8[i] - 128);
        printf("\r\n[SpkID]   NPU bytes  first 20 (hex): ");
        for (int i = 0; i < 20 && i < num_vals; i++) printf("%02x ", u8[i]);
        {
            int start = (num_vals > 20) ? (num_vals - 20) : 0;
            printf("\r\n[SpkID]   signed int8 last  20: ");
            for (int i = start; i < num_vals; i++) printf("%d ", (int)u8[i] - 128);
            printf("\r\n[SpkID]   NPU bytes  last  20 (hex): ");
            for (int i = start; i < num_vals; i++) printf("%02x ", u8[i]);
        }
        printf("\r\n");

        /* optional: dump all fbank int8 (signed, one/line) to SD for offline diff */
        {
            int   cap = num_vals * 6 + 16;
            char *txt = (char *)pvPortMalloc(cap);
            if (txt) {
                int off = 0;
                for (int i = 0; i < num_vals; i++)
                    off += snprintf(txt + off, cap - off, "%d\n", (int)u8[i] - 128);
                int fd = open("/sdcard/ai_model/fbank_dump.txt",
                              O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd >= 0) { write(fd, txt, off); close(fd);
                    printf("[SpkID] fbank int8 -> /sdcard/ai_model/fbank_dump.txt (%d vals)\r\n",
                           num_vals);
                }
                vPortFree(txt);
            }
        }
#endif

        vPortFree(imgBuffer);
        imgBuffer = (char *)u8;
        img_size  = num_vals;

        L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)imgBuffer, img_size);
        ext_addr = (uint32_t)imgBuffer;
        NPU_Img_Ext_Addr_Cfg(ext_addr);
    }

    // ===== Step 5: 啟動 External Buffer 推理 =====
    printf("\r\nStep 5: Starting external buffer inference...\r\n");
    printf("Available heap: %zu bytes\r\n", xPortGetFreeHeapSize());

    // 設定當前模型索引（用於後處理）
    g_current_model_index = model_index;

    // 啟動 external buffer 推理任務
    iproai_start_inference_ext(g_model_hdl);

    printf("\r\n=== AI Run External Started ===\r\n");
    printf("Model %d (%s) is now running\r\n", model_index, config->description);

    // ===== 推送測試圖片地址到 queue（5次，模擬5張圖片） =====
    printf("\r\n=== Pushing Test Images to Queue ===\r\n");
    printf("Pushing same image address 5 times (simulating 5 frames)...\r\n");

    int push_count = 0;
    for (int i = 0; i < 1; i++) {
        if (xQueueSend(g_ext_buffer_queue, &ext_addr, pdMS_TO_TICKS(100)) == pdTRUE) {
            push_count++;
            printf("[%d/5] Pushed address: 0x%08lx\r\n", push_count, (unsigned long)ext_addr);
            vTaskDelay(pdMS_TO_TICKS(10));  // 短暫延遲避免推送過快
        } else {
            printf("[%d/5] ERROR: Failed to push address (queue full?)\r\n", i+1);
            break;
        }
    }

    printf("\r\nTotal %d frames pushed to queue\r\n", push_count);
    printf("Inference task will process all frames until queue is empty\r\n");
    printf("External modules can push more addresses using ai_ext_push_buffer()\r\n");
    printf("Use 'ai_run_ext stop' to stop inference task\r\n");

#else
    printf("AI support not enabled in configuration\r\n");
#endif
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ai_run_ext, ai_run_ext, Run AI with external buffer queue);

/* spk_list — list speakers registered in the AI_MODEL_SPEAKER_RECOG gallery */
static int cmd_spk_list(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (!g_spk_gallery_inited) {
        gallery_init(&g_spk_gallery, GALLERY_DEFAULT_THRESHOLD);
        g_spk_gallery_inited = true;
    }
    printf("=== Speaker Gallery ===\r\n");
    printf("Speakers: %d/%d, threshold=%.2f\r\n",
           g_spk_gallery.n, GALLERY_MAX_SPEAKERS, g_spk_gallery.threshold);
    for (int i = 0; i < g_spk_gallery.n; i++)
        printf("  [%d] %-16s (%d clip%s averaged)\r\n",
               i, g_spk_gallery.names[i], g_spk_gallery.count[i],
               (g_spk_gallery.count[i] > 1 ? "s" : ""));
    if (g_spk_gallery.n == 0)
        printf("  (empty - register with: ai_run_ext 5 register <name> img_path <clip>)\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spk_list, spk_list, List registered speakers);

/* spk_clear — clear the AI_MODEL_SPEAKER_RECOG gallery */
static int cmd_spk_clear(int argc, char **argv)
{
    (void)argc; (void)argv;
    gallery_init(&g_spk_gallery, GALLERY_DEFAULT_THRESHOLD);
    g_spk_gallery_inited = true;
    printf("[SpkID] gallery cleared\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spk_clear, spk_clear, Clear the speaker gallery);


/**
 * Initialize AI demo subsystem
 * This function is called from main.c during initialization
 */
void demo_ai_init(void)
{
    printf("AI Demo Initialized\r\n");
    // Add any AI initialization logic here
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);

    // 初始化音頻前處理模組 (使用 speaker_recog 組件)
    if (speaker_recog_preprocess_init(NULL) != 0) {
        printf("[AI Demo] Warning: Audio preprocess init failed\r\n");
    }
}

/**
 * Public API: Push an external buffer address to the AI inference queue
 */
int ai_ext_push_buffer(uint32_t addr)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (g_ext_buffer_queue == NULL) {
        printf("[ERROR] External buffer queue not created. Start ai_run_ext first.\r\n");
        return -1;  // Queue not created
    }

    if (!g_ext_task_started) {
        printf("[ERROR] External inference task not started.\r\n");
        return -1;
    }

    // BaseType_t ret = xQueueSend(g_ext_buffer_queue, &addr, pdMS_TO_TICKS(100));
    BaseType_t ret = xQueueSendFromISR(g_ext_buffer_queue, &addr, &xHigherPriorityTaskWoken);
    if (ret != pdTRUE) {
        printf("[ERROR] Failed to push buffer 0x%08lx: Queue full or timeout\r\n", addr);
        return -2;  // Queue full or timeout
    }

    printf("[INFO] Buffer pushed: 0x%08lx\r\n", addr);
    return 0;  // Success
}

/**
 * Public API: Check if external inference task is running
 */
int ai_ext_is_running(void)
{
    return g_ext_task_started;
}

/**
 * Public API: Get available space in external buffer queue
 */
int ai_ext_get_queue_space(void)
{
    if (g_ext_buffer_queue == NULL) {
        return -1;  // Queue not created
    }

    UBaseType_t available = uxQueueSpacesAvailable(g_ext_buffer_queue);
    return (int)available;
}

#ifdef CONFIG_MM_SUPPORT // tricky call function
int ai_get_result(uint32_t *result_addr)
{
    if (g_result_queue == NULL) {
        printf("[ERROR] Result queue not created. Start ai_run_ext first.\r\n");
        return -1;  // Queue not created
    }

    if (!g_ext_task_started) {
        printf("[ERROR] External inference task not started.\r\n");
        return -1;
    }

    // task blocking
    xQueueReceive(g_result_queue, result_addr, portMAX_DELAY);

    return 0;
}
#endif

// ========== Speaker Recognition Shell Commands ==========
#ifdef CONFIG_AI_SUPPORT

/**
 * Initialize speaker recognition module
 * Usage: ai_audio_load [path]
 * Default path: /sdcard/ai_model/audio_weights.bin
 */
static int cmd_ai_audio_load(int argc, char **argv)
{
    const char *weights_path = NULL;

    if (argc >= 2) {
        weights_path = argv[1];
    }

    printf("=== Initialize Speaker Recognition ===\r\n");

    // 檢查是否已初始化
    if (g_speaker_recog_initialized) {
        printf("Already initialized. Use 'ai_audio_unload' to deinit first.\r\n");
        return 0;
    }

    // 掛載 SD 卡
    printf("Mounting SD card...\r\n");
    mount_sdcard();

    printf("Free heap before: %zu bytes\r\n", xPortGetFreeHeapSize());

    // 初始化 speaker recognition（會自動載入權重）
    int ret = speaker_recog_init(&g_speaker_config, NULL, weights_path);
    if (ret != 0) {
        printf("ERROR: Failed to initialize speaker recognition\r\n");
        return -1;
    }

    g_speaker_recog_initialized = true;

    printf("Free heap after: %zu bytes\r\n", xPortGetFreeHeapSize());
    printf("\r\nReady! Run: ai_run_demo 4 or ai_run_ext 4\r\n");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ai_audio_load, ai_audio_load, Initialize speaker recognition);

/**
 * Deinitialize speaker recognition module
 */
static int cmd_ai_audio_unload(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (!g_speaker_recog_initialized) {
        printf("Not initialized.\r\n");
        return 0;
    }

    speaker_recog_deinit(&g_speaker_config);
    g_speaker_recog_initialized = false;

    printf("Free heap: %zu bytes\r\n", xPortGetFreeHeapSize());

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ai_audio_unload, ai_audio_unload, Deinitialize speaker recognition);

/**
 * Register current embedding as a new speaker
 * Usage: ai_audio_register [speaker_name]
 * Note: Run ai_run_demo 4 first to get embedding
 */
static int cmd_ai_audio_register(int argc, char **argv)
{
    const char *name = "Unknown";

    if (argc >= 2) {
        name = argv[1];
    }

    printf("=== Register Speaker ===\r\n");

    if (!g_speaker_recog_initialized) {
        printf("ERROR: Speaker recognition not initialized.\r\n");
        printf("Run 'ai_audio_load' first.\r\n");
        return -1;
    }

    if (!g_speaker_config.has_current_embedding) {
        printf("ERROR: No embedding available.\r\n");
        printf("Run 'ai_run_demo 4' first to extract speaker embedding.\r\n");
        return -1;
    }

    int id = speaker_recog_register(&g_speaker_config, name);
    if (id >= 0) {
        printf("Speaker registered successfully with ID: %d\r\n", id);
    }

    return (id >= 0) ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ai_audio_register, ai_audio_register, Register speaker from current embedding);

/**
 * Clear all registered speakers
 * Usage: ai_audio_clear
 */
static int cmd_ai_audio_clear(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("=== Clear Registered Speakers ===\r\n");

    if (!g_speaker_recog_initialized) {
        printf("Not initialized.\r\n");
        return 0;
    }

    speaker_recog_clear(&g_speaker_config);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ai_audio_clear, ai_audio_clear, Clear all registered speakers);

/**
 * Show speaker recognition status
 * Usage: ai_audio_status
 */
static int cmd_ai_audio_status(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("=== Speaker Recognition Status ===\r\n");
    printf("Initialized: %s\r\n", g_speaker_recog_initialized ? "Yes" : "No");

    if (g_speaker_recog_initialized) {
        printf("Weights loaded: %s\r\n", g_speaker_config.weights_loaded ? "Yes" : "No");
        printf("Similarity threshold: %.2f\r\n", g_speaker_config.similarity_threshold);
        speaker_recog_print_speakers(&g_speaker_config);
    }

    printf("\r\nUsage:\r\n");
    printf("  1. ai_audio_load [path]     - Initialize and load weights\r\n");
    printf("  2. ai_run_demo 4            - Run inference on audio_input.bin\r\n");
    printf("  3. ai_audio_register [name] - Register current speaker\r\n");
    printf("  4. ai_audio_clear           - Clear all registered speakers\r\n");
    printf("  5. ai_audio_unload          - Deinitialize and free memory\r\n");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ai_audio_status, ai_audio_status, Show speaker recognition status);


#define MAX_ARGC 16
#define LOG_FILE_PATH "/sdcard/run_script_result.txt"
#define SR_ERROR(fmt, ...)  printf("[SpeakerRecog] ERROR: " fmt, ##__VA_ARGS__)

static int append_to_log(const char *buf, size_t len)
{
    int fd = open(LOG_FILE_PATH, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd < 0) {
        SR_ERROR("Cannot open log file %s\r\n", LOG_FILE_PATH);
        return -1;
    }

    write(fd, buf, len);
    close(fd);
    return 0;
}

static int parse_line_to_argv(char *line, char **argv)
{
    int argc = 0;
    char *p = line;

    while (*p && argc < MAX_ARGC) {
        // 跳過空格
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') break;

        if (*p == '"') { // 引號參數
            p++;
            argv[argc++] = p;
            while (*p && *p != '"') p++;
            if (*p) *p++ = '\0';
        } else { // 普通參數
            argv[argc++] = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p++ = '\0';
        }
    }

    return argc;
}


static int run_script_file(const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        SR_ERROR("Failed to open script: %s\r\n", path);
        return -1;
    }

    int file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    if (file_size <= 0 || file_size > 8192) {
        SR_ERROR("Invalid script size: %d\r\n", file_size);
        close(fd);
        return -1;
    }

    char *buf = pvPortMalloc(file_size + 1);
    if (!buf) {
        SR_ERROR("Malloc failed\r\n");
        close(fd);
        return -1;
    }

    read(fd, buf, file_size);
    buf[file_size] = '\0';
    close(fd);

    char log_line[256];
    snprintf(log_line, sizeof(log_line), "=== Running script: %s ===\n", path);
    printf("%s", log_line);
    append_to_log(log_line, strlen(log_line));

    char *saveptr_line = NULL;
    char *line = strtok_r(buf, "\r\n", &saveptr_line);
    int line_no = 0;
    char *argv[MAX_ARGC];
    int total = 0, pass = 0, fail = 0;

    while (line != NULL) {
        line_no++;

        if (strlen(line) == 0) {
            line = strtok_r(NULL, "\r\n", &saveptr_line);
            continue;
        }

        snprintf(log_line, sizeof(log_line), "[%02d] %s\n", line_no, line);
        printf("%s", log_line);
        append_to_log(log_line, strlen(log_line));

        int argc = parse_line_to_argv(line, argv);
        if (argc == 0) {
            line = strtok_r(NULL, "\t\r\n", &saveptr_line);
            continue;
        }

        if (strcmp(argv[0], "ai_run_ext") == 0) {
            const char *expected_id = NULL;
            if (argc >= 4 && strcmp(argv[2], "img_path") == 0) {
                const char *img_path = argv[3];
                const char *base = strrchr(img_path, '/');
                if (base) base++; else base = img_path;

                static char tmp_id[32] = {0};
                const char *underscore = strchr(base, '_');
                if (underscore) {
                    strncpy(tmp_id, base, underscore - base);
                    tmp_id[underscore - base] = '\0';
                } else {
                    strncpy(tmp_id, base, sizeof(tmp_id)-1);
                }
                expected_id = tmp_id;
            }

            int ret = cmd_ai_run_ext(argc, argv);
            snprintf(log_line, sizeof(log_line), "cmd_ai_run_ext returned: %d\n", ret);
            printf("%s", log_line);

            vTaskDelay(pdMS_TO_TICKS(1000));
            total++;

            if (expected_id) {
                if (strcmp(speaker_result.speaker_name, expected_id) == 0) {
                    snprintf(log_line, sizeof(log_line),
                             "[%02d] PASS: Recognized %s (expected %s)\n",
                             line_no, speaker_result.speaker_name, expected_id);
                    printf("%s", log_line);
                    append_to_log(log_line, strlen(log_line));
                    pass++;
                } else {
                    speaker_result.speaker_name = "NULL";
                    snprintf(log_line, sizeof(log_line),
                             "[%02d] FAIL: Recognized %s (expected %s)\n",
                             line_no, speaker_result.speaker_name, expected_id);
                    printf("%s", log_line);
                    append_to_log(log_line, strlen(log_line));
                    fail++;
                }
            }

            // 等待 inference 完成
            int wait_ms = 0;
            const int max_wait_ms = 1000;
            while (g_ext_task_started && wait_ms < max_wait_ms) {
                vTaskDelay(pdMS_TO_TICKS(10));
                wait_ms += 10;
            }

            if (g_ext_task_started) {
                snprintf(log_line, sizeof(log_line),
                         "WARNING: timeout waiting inference after %d ms\n", wait_ms);
                printf("%s", log_line);
                g_ext_task_started = 0;
            } else {
                snprintf(log_line, sizeof(log_line),
                         "Inference finished after %d ms\n", wait_ms);
                printf("%s", log_line);
                speaker_result.speaker_name = NULL;
            }
        } else {
            snprintf(log_line, sizeof(log_line), "Unknown command: %s\n", argv[0]);
            printf("%s", log_line);
            append_to_log(log_line, strlen(log_line));
        }

        vTaskDelay(pdMS_TO_TICKS(10));
        line = strtok_r(NULL, "\t\r\n", &saveptr_line);
    }

    snprintf(log_line, sizeof(log_line),
             "=== Script done ===\nTotal: %d, PASS: %d, FAIL: %d\n\n",
             total, pass, fail);
    printf("%s", log_line);
    append_to_log(log_line, strlen(log_line));

    vPortFree(buf);
    return 0;
}
static int cmd_run_script(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: run_script <file>\r\n");
        return -1;
    }

    return run_script_file(argv[1]);
}
SHELL_CMD_EXPORT_ALIAS(cmd_run_script, run_script, Run script file);

#endif // CONFIG_AI_SUPPORT

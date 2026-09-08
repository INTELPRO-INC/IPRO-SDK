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

/*
 * hw_sdcard_cmd.c - hardware-SDH (GPIO28-33) A/B reference commands,
 * extracted from ipro7_demo peripheral_demo.c.
 *
 *   sdcard          SDH pinmux + 4-bit + FatFS register + mount at /sdcard
 *   format_sd       fatfs_format over the HW host (exFAT, 64 KB clusters,
 *                   progress callback) - also serves as a HW write soak
 *   sdh_speed_test  128 MB file write + 128 MB read + full memcmp verify
 *
 * A/B use: the card sits in exactly ONE socket at a time. Whichever host's
 * mount command runs last (PIO `pio_sdcard mount` vs HW `sdcard`) owns the
 * DEV_SD drive slot and the /sdcard mount point. Moving the card + host is
 * the fault-isolation step: HW writes OK + PIO writes dead => host-side
 * problem, both dead => card problem.
 */
#include "drv_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "shell.h"
#include <FreeRTOS.h>
#include <task.h>

#include <fatfs.h>
#include <fatfs_diskio_register.h>
#include <diskio.h>
#include <hal_sdh.h>
#include "drv_glb.h"

#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_USE_FATFS)
#include "evfs.h"
#include "evfs/fatfs_fs.h"
#include "evfs_posix.h"
#endif

#define FATFS_SD_MOUNT_POINT "/sdcard"

static void mkfs_report(int progress)
{
    printf("format progress %d\r\n", progress);
}

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
        cfg.pullType = (pins[i] == GLB_GPIO_PIN_30) ?
                       GPIO_PULL_NONE : GPIO_PULL_UP;
        GLB_GPIO_Init(&cfg);
    }

    GLB_Set_SDH_CLK(1, GLB_SDH_CLK_PLL_96M, 1);
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SDH);
}

int cmd_sdcard_test(int argc, char **argv)
{
    extern void f_set_mkfs_cb(void (*cb)(int));
    sdh_pinmux_init();
    hal_sd_bus_4bits_enable();
    f_set_mkfs_cb(mkfs_report);
    fatfs_sdh_driver_register();
    int ret = fatfs_register();

#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_USE_FATFS)
    /* fatfs_register returns aos_register_fs result, not 0 on success.
     * Always try EVFS registration since SD card init completed. */
    (void)ret;

    int evfs_ret = evfs_register_fatfs("sdcard", DEV_SD, false);
    if (evfs_ret != EVFS_OK) {
        printf("EVFS FatFS registration failed: %d\r\n", evfs_ret);
    } else {
        evfs_ret = evfs_posix_mount(FATFS_SD_MOUNT_POINT, "sdcard");
        if (evfs_ret != 0) {
            printf("EVFS mount failed: %d\r\n", evfs_ret);
        } else {
            printf("FatFS mounted at %s\r\n", FATFS_SD_MOUNT_POINT);
        }
    }
#else
    (void)ret;
#endif

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_sdcard_test, sdcard, HW SDH mount at /sdcard);

int cmd_format_sdcard_test(int argc, char **argv)
{
    fatfs_format(DEV_SD, mkfs_report);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_format_sdcard_test, format_sd, format card (exFAT) via current host);

#define FATFS_TEST_FILE_NAME     "/sdcard/test_file.txt"
#define SDU_DATA_CHECK 1
#define TEST_BUFF_SIZE (128 * 1024)

static const char test_data[] =
    "I've been reading books of old \r\n\
    The legends and the myths \r\n\
    Achilles and his gold \r\n\
    Hercules and his gifts \r\n\
    Spiderman's control \r\n\
    And Batman with his fists\r\n\
    And clearly I don't see myself upon that list\r\n\
    But she said, where'd you wanna go?\r\n\
    How much you wanna risk?\r\n\
    I'm not looking for somebody\r\n\
    With some superhuman gifts\r\n\
    Some superhero\r\n\
    Some fairytale bliss\r\n\
    Just something I can turn to\r\n\
    Somebody I can kiss\r\n\
    I want something just like this\r\n\r\n";

static void sdh_speed_test_proc(void *arg)
{
    unsigned int time_node, i;
    int fd_file;
    int ret;
    uint8_t *rw_buffer = NULL;
    void *rw_mem = NULL;
    int test_cnt = 1024;

#if SDU_DATA_CHECK
    uint8_t *check_buffer = NULL;
    void *check_mem = NULL;
#endif

    rw_mem = pvPortMalloc(TEST_BUFF_SIZE + 8);
    if (rw_mem == NULL) {
        printf("fatfs test no memory\r\n");
        goto out;
    }
    /* Must be 8 bytes aligned */
    rw_buffer = rw_mem + (8 - (uintptr_t)rw_mem % 8);

#if SDU_DATA_CHECK
    check_mem = pvPortMalloc(TEST_BUFF_SIZE + 8);
    if (check_mem == NULL) {
        printf("fatfs test no memory\r\n");
        goto out;
    }
    /* Must be 8 bytes aligned */
    check_buffer = check_mem + (8 - (uintptr_t)check_mem % 8);
#endif

    for (int round = 0; round < 1; round++) {
        /* full test data to buff */
        for (uint32_t size = 0; size < (TEST_BUFF_SIZE - sizeof(test_data)); size += sizeof(test_data)) {
            memcpy(&rw_buffer[size], test_data, sizeof(test_data));
        }
#if SDU_DATA_CHECK
        memcpy(check_buffer, rw_buffer, TEST_BUFF_SIZE);
#endif
        L1C_DCACHE_CLEAN_RANGE((uintptr_t)rw_buffer, TEST_BUFF_SIZE);

        /* write test */
        printf("\r\n******************** [%d]be about to write test... **********************\r\n", round);
        /* O_CREAT added over the ipro7_demo original: on a fresh/formatted
         * card the test file does not exist yet and a bare O_TRUNC open
         * fails with "Fail to open or create files". */
        fd_file = open(FATFS_TEST_FILE_NAME, O_CREAT | O_TRUNC | O_RDWR);
        if (fd_file >= 0) {
            int n = 0;
            time_node = (uint32_t)xTaskGetTickCount();
            for (n = 0; n < test_cnt; n++) {
                ret = write(fd_file, rw_buffer, TEST_BUFF_SIZE);
                if (ret != TEST_BUFF_SIZE) {
                    printf("ret = %d\r\n", ret);
                    goto out;
                }
            }
            ret = close(fd_file);
            time_node = (uint32_t)xTaskGetTickCount() - time_node;

            if (ret >= 0) {
                printf("Write Test Succeed! \r\n");
                printf("Single data size:%d Byte, Write the number:%d, Total size:%d KB\r\n", TEST_BUFF_SIZE, n, TEST_BUFF_SIZE * n >> 10);
                printf("Time:%dms, Write Speed:%d KB/s \r\n", time_node, ((TEST_BUFF_SIZE * n) >> 10) * 1000 / time_node);
            } else {
                printf("Fail to write files(%d) num:%d\n", ret, n);
                goto out;
            }
        } else {
            printf("Fail to open or create files.\r\n");
            goto out;
        }

        memset(rw_buffer, 0xFF, TEST_BUFF_SIZE);

        L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)rw_buffer, TEST_BUFF_SIZE);
        /* read test */
        printf("\r\n******************** be about to read test... **********************\r\n");
        fd_file = open(FATFS_TEST_FILE_NAME, O_RDONLY);
        if (fd_file > 0) {
            time_node = (uint32_t)xTaskGetTickCount();

            for (i = 0; i < (unsigned int)test_cnt; i++) {
                ret = read(fd_file, rw_buffer, TEST_BUFF_SIZE);
                if (ret != TEST_BUFF_SIZE) {
                    break;
                }
            }
            ret = close(fd_file);
            time_node = (uint32_t)xTaskGetTickCount() - time_node;

            if (ret >= 0) {
                printf("Read Test Succeed! \r\n");
                printf("Single data size:%dByte, Read the number:%d, Total size:%d KB\r\n", TEST_BUFF_SIZE, i, TEST_BUFF_SIZE * i >> 10);
                printf("Time:%dms, Read Speed:%d KB/s \r\n", time_node, ((TEST_BUFF_SIZE * i) >> 10) * 1000 / time_node);
            } else {
                printf("Fail to read file: (%d), num:%d\n", ret, i);
                goto out;
            }
        } else {
            printf("Fail to open files.\r\n");
            goto out;
        }

        /* check data */
#if SDU_DATA_CHECK
        printf("\r\n******************** be about to check test... **********************\r\n");
        if (memcmp(rw_buffer, check_buffer, TEST_BUFF_SIZE)) {
            printf("Check Test Error! \r\n");
        } else {
            printf("Check Test Succeed! \r\n");
            printf("All Data Is Good! \r\n");
        }
#endif
    }
out:
    if (rw_mem != NULL) {
        vPortFree(rw_mem);
    }
#if SDU_DATA_CHECK
    if (check_mem != NULL) {
        vPortFree(check_mem);
    }
#endif
    printf("fatfs test end\r\n");
    vTaskDelete(NULL);
}

static int cmd_sdh_speed_test(int argc, char **argv)
{
    /* Priority 1 - below the USB console TX task (2) so the write/read
     * progress streams live instead of appearing only when the test ends. */
    xTaskCreate(sdh_speed_test_proc, "sdh_speed_test", 1024, (void *)0, 1, NULL);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_sdh_speed_test, sdh_speed_test, 128MB file write/read + verify);

/* ==================================================================== */
/*  Card-detect (CD) auto-mount - HW SDH CD switch wired to GPIO34.      */
/*  Insert a card -> auto `sdcard` mount; remove -> unmount /sdcard.     */
/*  Polarity is socket-specific: verify with `cd`, set with `cd pol`,    */
/*  then enable with `cd auto 1`. Default OFF so a wrong polarity never   */
/*  triggers a mount with no card present.                               */
/* ==================================================================== */
#include "task.h"

#define CD_GPIO GLB_GPIO_PIN_34

static volatile bool    s_cd_auto;
static volatile uint8_t s_cd_present_lvl;   /* GPIO level meaning "card present"; 0 = active-low */
static volatile bool    s_cd_mounted;
static StackType_t      s_cd_stack[512];
static StaticTask_t     s_cd_tcb;
static bool             s_cd_started;

static void cd_gpio_init(void)
{
    GLB_GPIO_Cfg_Type c = {
        .gpioPin = CD_GPIO, .gpioFun = GPIO_FUN_GPIO, .gpioMode = GPIO_MODE_INPUT,
        .pullType = GPIO_PULL_UP, .drive = 0, .smtCtrl = 1,
    };
    GLB_GPIO_Init(&c);
}

static bool cd_present(void)
{
    return ((GLB_GPIO_Read(CD_GPIO) & 1u) == s_cd_present_lvl);
}

static void cd_task(void *pv)
{
    (void)pv;
    bool last = cd_present();
    int stable = 4;                          /* start settled, no spurious edge */
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(200));
        if (!s_cd_auto) { last = cd_present(); stable = 4; continue; }
        bool now = cd_present();
        if (now != last) { last = now; stable = 0; continue; }   /* debounce */
        if (stable < 4) {
            if (++stable == 4) {                                  /* 800 ms stable = real edge */
                if (now && !s_cd_mounted) {
                    printf("[CD] card inserted -> mounting /sdcard\n");
                    cmd_sdcard_test(0, NULL);
                    s_cd_mounted = true;
                } else if (!now && s_cd_mounted) {
                    printf("[CD] card removed -> /sdcard offline\n");
                    fatfs_unregister();
                    s_cd_mounted = false;
                }
            }
        }
    }
}

static void cd_ensure(void)
{
    if (s_cd_started) return;
    cd_gpio_init();
    xTaskCreateStatic(cd_task, "cd", sizeof(s_cd_stack) / sizeof(StackType_t),
                      NULL, 3, s_cd_stack, &s_cd_tcb);
    s_cd_started = true;
}

int cmd_cd(int argc, char **argv)
{
    cd_ensure();
    if (argc >= 2 && strcmp(argv[1], "auto") == 0) {
        s_cd_auto = (argc > 2) ? (atoi(argv[2]) != 0) : true;
        printf("[CD] auto-mount %s (present = GPIO34 level %u)\n",
               s_cd_auto ? "ON" : "OFF", s_cd_present_lvl);
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "pol") == 0) {
        s_cd_present_lvl = (argc > 2 && atoi(argv[2])) ? 1u : 0u;
        printf("[CD] card-present level set to %u\n", s_cd_present_lvl);
        return 0;
    }
    printf("[CD] GPIO34 raw=%lu -> %s   (present-level=%u, auto=%s)\n",
           (unsigned long)(GLB_GPIO_Read(CD_GPIO) & 1u),
           cd_present() ? "PRESENT" : "absent", s_cd_present_lvl, s_cd_auto ? "ON" : "OFF");
    printf("     verify: insert/remove a card and watch `raw`; then `sdcd pol <0|1>`\n");
    printf("     enable: `sdcd auto 1`  (insert -> auto sdcard mount, remove -> unmount)\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cd, sdcd, HW SDH card-detect (GPIO34) auto-mount);

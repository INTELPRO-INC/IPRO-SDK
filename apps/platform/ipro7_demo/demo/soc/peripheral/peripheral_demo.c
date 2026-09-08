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

#include "drv_common.h"
#include "drv_hbn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Filesystem - EVFS POSIX API */
#include "evfs.h"
#include "evfs/fatfs_fs.h"
#include "evfs_posix.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "shell.h"
#include <FreeRTOS.h>
#include <hal_dma.h>
#include <hal_mtimer.h>
#include <hal_timer.h>
#include <hal_uart.h>
#include <hal_wdt.h>
#include <hal_pwm.h>
#include <task.h>

#include <fatfs.h>
#include <fatfs_diskio_register.h>
#include <diskio.h>
#include <hal_sdh.h>
#include "drv_glb.h"

#define TIMER0_ID TIMER0_INDEX
#define FATFS_SD_MOUNT_POINT "/sdcard"

static int gWDTFeedStart = 0;

static void timer_irq_hdl(int timer_id, int timer_ch, uint32_t event)
{
    printf("Timer %d:%d event %lu\r\n", timer_id, timer_ch, event);
}

static int cmd_timer_test(int argc, char **argv)
{
    hal_timer_config_t tmr0 = {
        .cnt_mode = TIMER_CNT_PRELOAD,
        .trigger = TIMER_PRELOAD_TRIGGER_COMP2,
        .reload = 0,
        .timeout1 = 10000,
        .timeout2 = 20000,
        .timeout3 = 30000,
    };

    hal_timer_config_t tmr1 = {
        .cnt_mode = TIMER_CNT_PRELOAD,
        .trigger = TIMER_PRELOAD_TRIGGER_COMP1,
        .reload = 0,
        .timeout1 = 1000,
        .timeout2 = 2000,
        .timeout3 = 3000,
    };

    timer_init(TIMER0_ID, TIMER_CH0_INDEX, &tmr0);
    timer_init(TIMER0_ID, TIMER_CH1_INDEX, &tmr1);
	timer_setCallback(TIMER0_ID, TIMER_CH0_INDEX, timer_irq_hdl);
	timer_setCallback(TIMER0_ID, TIMER_CH1_INDEX, timer_irq_hdl);
    timer_start(TIMER0_ID, TIMER_CH0_INDEX);
    timer_start(TIMER0_ID, TIMER_CH1_INDEX);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_timer_test, timer_test, Timer Test);

static void tick_proc(void *arg)
{
    uint32_t last_soctick;
    uint64_t last_cputick;
    uint64_t last_rtctick;
    uint32_t curr_soctick;
    uint64_t curr_cputick;
    uint64_t curr_rtctick;
    uint32_t timel, timeh;

    hal_timer_config_t tmr0 = {
        .cnt_mode = TIMER_CNT_FREERUN,
        .trigger = TIMER_PRELOAD_TRIGGER_NONE,
        .reload = 0,
        .timeout1 = 0,
        .timeout2 = 0,
        .timeout3 = 0,
    };

	timer_init(TIMER0_ID, TIMER_CH0_INDEX, &tmr0);
	timer_start(TIMER0_ID, TIMER_CH0_INDEX);

    last_soctick = timer_get_counter(TIMER0_ID, TIMER_CH0_INDEX);
    last_cputick = mtimer_get_time_us();
    HBN_Get_RTC_Timer_Val(&timel, &timeh);
    last_rtctick = ((uint64_t)timeh << 32) | timel;
    arch_delay_ms(1000);

    while(1) {
        curr_soctick =  timer_get_counter(TIMER0_ID, TIMER_CH0_INDEX);
        curr_cputick = mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val(&timel, &timeh);
        curr_rtctick = ((uint64_t)timeh << 32) | timel;
        printf("soc timer tick: %lu, mtime tick: %llu, rtos: %lu, rtc: %llu (cur: %llu)\r\n", curr_soctick - last_soctick, curr_cputick - last_cputick, (uint32_t)xTaskGetTickCount(), curr_rtctick - last_rtctick, curr_rtctick);
        arch_delay_ms(1000);
        last_soctick = curr_soctick;
        last_cputick = curr_cputick;
        last_rtctick = curr_rtctick;
    }
    vTaskDelete(NULL);
}

static int cmd_tick_test(int argc, char **argv)
{
    xTaskCreate(tick_proc, "tick", 1024, NULL, 10, NULL);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_tick_test, tick_test, Timer Test);

static int cmd_wdt_test(int argc, char **argv)
{
    printf("Init WDT for 10 seconds\r\n");
    wdt_init(0, 10000);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_wdt_test, wdt_test, WDT Test);

static void proc_wdt_feed(void *arg)
{
    while(gWDTFeedStart) {
        printf("Feed WDT\r\n");
        wdt_feed();
        vTaskDelay(1000);
    }
    vTaskDelete(NULL);
}

static int cmd_wdt_feed_test(int argc, char **argv)
{
    if (gWDTFeedStart) {
        gWDTFeedStart = 0;
        printf("Stop WDT Feeder\r\n");
    } else {
        printf("Start WDT Feeder\r\n");
        gWDTFeedStart = 1;
        xTaskCreate(proc_wdt_feed, "wdt_feeder", 1024, NULL, 10, NULL);
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_wdt_feed_test, wdt_feed_test, WDT Feed Test);

#ifdef CONFIG_AON_WDT_ENABLE
#include <hal_aon_wdt.h>
#include "hbn_reg.h"
#include "compiler/common.h"

static int cmd_aon_wdt_test(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: aon_wdt <start [mask_bit]|stop|feed|status|hang|dump>\r\n");
        return 0;
    }

    if (strcmp(argv[1], "start") == 0) {
        uint8_t mask_bit = AON_WDT_TIMEOUT_16S;
        if (argc >= 3)
            mask_bit = (uint8_t)atoi(argv[2]);
        uint32_t timeout_ms = ((1UL << (mask_bit + 1)) * 1000) / 32768;
        printf("AON WDT start: mask_bit=%d, timeout=%lums\r\n", mask_bit, (unsigned long)timeout_ms);
        aon_wdt_init(mask_bit);
        uint32_t wdt_val = readl(HBN_BASE + HBN_WDT_OFFSET);
        printf("  HBN_WDT=0x%08lx (en=%lu, mask=%lu, st=%lu)\r\n",
               (unsigned long)wdt_val,
               (unsigned long)((wdt_val >> 8) & 1),
               (unsigned long)((wdt_val >> 12) & 0x3F),
               (unsigned long)((wdt_val >> 11) & 1));
    } else if (strcmp(argv[1], "stop") == 0) {
        printf("AON WDT disabled\r\n");
        aon_wdt_disable();
    } else if (strcmp(argv[1], "feed") == 0) {
        printf("AON WDT fed\r\n");
        aon_wdt_feed();
    } else if (strcmp(argv[1], "status") == 0) {
        printf("AON WDT reset status: %d\r\n", aon_wdt_get_reset_status());
    } else if (strcmp(argv[1], "dump") == 0) {
        uint32_t wdt_val = readl(HBN_BASE + HBN_WDT_OFFSET);
        uint32_t glb_val = readl(HBN_BASE + 0x30);
        printf("HBN_WDT(0x110)=0x%08lx\r\n", (unsigned long)wdt_val);
        printf("  en=%lu, kick=w1p, clr=w1p, st=%lu, mask=%lu, key=w\r\n",
               (unsigned long)((wdt_val >> 8) & 1),
               (unsigned long)((wdt_val >> 11) & 1),
               (unsigned long)((wdt_val >> 12) & 0x3F));
        printf("HBN_GLB(0x30)=0x%08lx\r\n", (unsigned long)glb_val);
        printf("  en_hbn_watchdog_timeout=%lu\r\n", (unsigned long)((glb_val >> 14) & 1));
        printf("  en_hbn_wdt_reset_as_swrst=%lu\r\n", (unsigned long)((glb_val >> 22) & 1));
    } else if (strcmp(argv[1], "hang") == 0) {
        printf("Hanging... WDT should reset\r\n");
        __disable_irq();
        while (1) {}
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_aon_wdt_test, aon_wdt, AON Watchdog Timer Test);
#endif

static char uart_dma_buffer[32];

void uart1_irq_callback(void *dev, void *args, uint32_t size, uint32_t event)
{
}

void dma0_irq_callback(void *dev, void *args, uint32_t size, uint32_t event)
{
}

void dma1_irq_callback(void *dev, void *args, uint32_t size, uint32_t event)
{
    printf("dma1: %s\r\n", uart_dma_buffer);
}


static int cmd_uart_dma_test(int argc, char **argv)
{
    dma_device_t uart_dma_dev;

    uart_init(UART1_INDEX, COMMON_CAPS_DMA_TX | COMMON_CAPS_DMA_RX);

    uart_dma_dev.id = DMA0_ID;
    uart_dma_dev.ch = DMA0_CH0_INDEX;
    uart_dma_dev.direction = DMA_MEMORY_TO_PERIPH;
    uart_dma_dev.transfer_mode = DMA_LLI_ONCE_MODE;
    uart_dma_dev.src_req = DMA_REQUEST_NONE;
    uart_dma_dev.dst_req = DMA_REQUEST_UART1_TX;
    uart_dma_dev.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    uart_dma_dev.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    uart_dma_dev.src_burst_size = DMA_BURST_INCR1;
    uart_dma_dev.dst_burst_size = DMA_BURST_INCR1;
    uart_dma_dev.src_width = DMA_TRANSFER_WIDTH_8BIT;
    uart_dma_dev.dst_width = DMA_TRANSFER_WIDTH_8BIT;
    dma_setConfig(&uart_dma_dev);
    dma_init(DMA0_ID, uart_dma_dev.ch);
    dma_ioctl(DMA0_ID, DMA0_CH0_INDEX, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(DMA0_ID, DMA0_CH0_INDEX, DMA_CTRL_SET_CALLBACK, dma0_irq_callback);

    uart_dma_dev.id = DMA0_ID;
    uart_dma_dev.ch = DMA0_CH1_INDEX;
    uart_dma_dev.direction = DMA_PERIPH_TO_MEMORY;
    uart_dma_dev.transfer_mode = DMA_LLI_ONCE_MODE;
    uart_dma_dev.src_req = DMA_REQUEST_UART1_RX;
    uart_dma_dev.dst_req = DMA_REQUEST_NONE;
    uart_dma_dev.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    uart_dma_dev.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    uart_dma_dev.src_burst_size = DMA_BURST_INCR1;
    uart_dma_dev.dst_burst_size = DMA_BURST_INCR1;
    uart_dma_dev.src_width = DMA_TRANSFER_WIDTH_8BIT;
    uart_dma_dev.dst_width = DMA_TRANSFER_WIDTH_8BIT;
    dma_setConfig(&uart_dma_dev);
    dma_init(DMA0_ID, uart_dma_dev.ch);
    dma_ioctl(DMA0_ID, DMA0_CH1_INDEX, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(DMA0_ID, DMA0_CH1_INDEX, DMA_CTRL_SET_CALLBACK, dma1_irq_callback);

    uart_ioctl(UART1_INDEX, UART_CTRL_ATTACH_TX_DMA, (void *)DMA0_CH0_INDEX);
    uart_ioctl(UART1_INDEX, UART_CTRL_ATTACH_RX_DMA, (void *)DMA0_CH1_INDEX);

    dma_ioctl(DMA0_ID, DMA0_CH1_INDEX, DMA_CTRL_CHANNEL_START, NULL);

    uart_write(UART1_INDEX, "test", 4);
    uart_write(UART1_INDEX, "test", 4);
    uart_write(UART1_INDEX, "test", 4);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_uart_dma_test, uart_dma, uart_dma);

static int cmd_uart_dma_read_test(int argc, char **argv)
{
    uart_read(UART1_INDEX, uart_dma_buffer, 8);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_uart_dma_read_test, uart_dma_read, uart_dma_read);

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

    /* Register FatFS with EVFS */
    int evfs_ret = evfs_register_fatfs("sdcard", DEV_SD, false);
    if (evfs_ret != EVFS_OK) {
        printf("EVFS FatFS registration failed: %d\r\n", evfs_ret);
    } else {
        /* Mount at /sdcard mount point */
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
SHELL_CMD_EXPORT_ALIAS(cmd_sdcard_test, sdcard, sdcard);

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
    uint8_t *check_buffer= NULL;
    void *check_mem = NULL;
#endif

    rw_mem = pvPortMalloc(TEST_BUFF_SIZE + 8);
    if(rw_mem == NULL){
        printf("fatfs test no memory\r\n");
        goto out;
    }
    /* Must be 8 bytes aligned */
    rw_buffer = rw_mem + (8 - (uintptr_t)rw_mem % 8);

#if SDU_DATA_CHECK
    check_mem = pvPortMalloc(TEST_BUFF_SIZE + 8);
    if(check_mem == NULL){
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
        fd_file = open(FATFS_TEST_FILE_NAME, O_TRUNC | O_RDWR);
        if (fd_file >= 0) {
            int i = 0;
            time_node = (uint32_t)xTaskGetTickCount();
            /*write into file*/
            for (i = 0; i < test_cnt; i++) {
                ret = write(fd_file, rw_buffer, TEST_BUFF_SIZE);
    			///printf("i:%d, ret = %d\r\n", i, ret);
                if (ret != TEST_BUFF_SIZE) {
                    printf("ret = %d\r\n", ret);
                    goto out;
                }
            }
            /* close file */
            ret= close(fd_file);
            /* get time */
            time_node = (uint32_t)xTaskGetTickCount() - time_node;

            if (ret >= 0) {
                printf("Write Test Succeed! \r\n");
                printf("Single data size:%d Byte, Write the number:%d, Total size:%d KB\r\n", TEST_BUFF_SIZE, i, TEST_BUFF_SIZE * i >> 10);
                printf("Time:%dms, Write Speed:%d KB/s \r\n", time_node, ((TEST_BUFF_SIZE * i) >> 10) * 1000 / time_node);
            } else {
                printf("Fail to write files(%d) num:%d\n", ret, i);
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

            for (i = 0; i < test_cnt; i++) {
                ret = read(fd_file, rw_buffer, TEST_BUFF_SIZE);
                if (ret != TEST_BUFF_SIZE) {
                    break;
                }
            }
            /* close file */
            ret= close(fd_file);
            /* get time */
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

        //memset(rw_buffer, 0xFF, TEST_BUFF_SIZE);

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
    if(rw_mem != NULL){
        vPortFree(rw_mem);
    }
#if SDU_DATA_CHECK
    if(check_mem != NULL){
        vPortFree(check_mem);
    }
#endif
    printf("fatfs test end\r\n");
    vTaskDelete(NULL);
}

static int cmd_sdh_speed_test(int argc, char **argv)
{
    xTaskCreate(sdh_speed_test_proc, "sdh_speed_test", 1024, (void *)0, 10, NULL);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_sdh_speed_test, sdh_speed_test, sdh_speed_test);

int cmd_format_sdcard_test(int argc, char **argv)
{
    fatfs_format(DEV_SD, mkfs_report);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_format_sdcard_test, format_sd, format card);

#include <hal_boot2.h>
#include <hal_flash.h>

int cmd_ota_test(int argc, char **argv)
{
    PtTable_Entry_Config ptEntryBackup, ptEntry;
    uint32_t imgLen = 0;
    hal_boot2_get_active_entries_byname((uint8_t *)"backup", &ptEntryBackup);
    hal_boot2_get_active_entries(0, &ptEntry);

    printf("copy from %lx to %lx\r\n", ptEntryBackup.Address[0], ptEntry.Address[0]);

    imgLen = ptEntryBackup.maxLen[0];
    void *ptBuf = malloc(imgLen);
    printf("start read from %lx, len %lx\r\n", ptEntryBackup.Address[0], imgLen);
    flash_read(ptEntryBackup.Address[0], ptBuf, imgLen);
    printf("erase %lx, len %lx\r\n", ptEntry.Address[1], ptEntry.maxLen[1]);
    flash_erase(ptEntry.Address[1], ptEntry.maxLen[1]);
    printf("start write to %lx, len %lx\r\n", ptEntry.Address[1], imgLen);
    flash_write(ptEntry.Address[1], ptBuf, imgLen);
    printf("switch active\r\n");
    hal_boot2_update_ptable(&ptEntry);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ota_test, ota_test, format card);

/**
 * Activate OTA: switch FW activeIndex to 1 and reboot.
 *
 * Before calling this, write the OTA package (.xz.ota) to FW-B partition
 * (Address[1]) using the flash tool:
 *   ipro_iot_tool --single --firmware <file.xz.ota> --addr 0x4E000
 *
 * Boot2 will detect activeIndex=1, decompress OTA from FW-B,
 * write to FW-A, reset activeIndex to 0, and boot updated FW-A.
 */
int cmd_ota_activate(int argc, char **argv)
{
    PtTable_Entry_Config ptEntry;

    hal_boot2_get_active_entries(0, &ptEntry);
    printf("[OTA] FW addr[0]=0x%lX addr[1]=0x%lX active=%d\r\n",
           (unsigned long)ptEntry.Address[0],
           (unsigned long)ptEntry.Address[1],
           ptEntry.activeIndex);

    if (ptEntry.Address[1] == 0 || ptEntry.maxLen[1] == 0) {
        printf("[OTA] ERROR: no FW-B partition (Address[1]=0)\r\n");
        return -1;
    }

    printf("[OTA] switching activeIndex to 1, rebooting...\r\n");
    hal_boot2_update_ptable(&ptEntry);

    /* Small delay to flush UART */
    extern void mtimer_delay_ms(uint32_t ms);
    mtimer_delay_ms(100);

    /* Reboot */
    extern void hal_sys_reset(void);
    hal_sys_reset();

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ota_activate, ota_activate, Activate OTA update and reboot);


void demo_peripheral_init(void)
{
    extern void sflash_test_init(void);
    extern void spi_test_init(void);
    extern void i2s_demo_init(void);
    extern void pwm_test_init(void);
    extern void romapi_test_init(void);

    sflash_test_init();
    spi_test_init();
    pwm_test_init();
    romapi_test_init();
//    i2s_demo_init();
}


/**
 * @file main.c
 * @brief IPRO LE Audio Headset - Main Entry Point
 * 
 * This application implements a LE Audio Unicast Server (headset/earbud)
 * that receives audio from a phone and decodes it using LC3 codec.
 * 
 * Architecture:
 *   Phone (Central) --> BLE ISO --> LC3 Decode --> DAC --> Speaker
 * 
 * Copyright (c) 2026 INTELPRO
 */

#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "hal_sys.h"
#include "hal_boot2.h"
#include "hal_gpio.h"
#include "platform_common.h"
#include "ipro_log.h"
#include "freertos_app_hooks.h"

#ifdef CONFIG_SHELL
#include "shell.h"
#include <zephyr/bluetooth/bluetooth.h>
#endif

// Application modules (Zephyr blestack)
#include "ble_init.h"

#ifndef CONFIG_IPRO_LE_AUDIO_CCI_ANALYSIS
#define CONFIG_IPRO_LE_AUDIO_CCI_ANALYSIS 0
#endif

#define IPRO_GLB_PARM_ADDR              0x30000090UL
#define IPRO_GLB_PARM_CCI_TRANSPORT_MSK ((1UL << 16) | (1UL << 19))

/* CCI is an APP-selected diagnostic transport, not controller policy.  The
 * board boot path already owns the GPIO0/1/2/11 mux; this RMW only enables the
 * two GLB transport bits and preserves the flash-routing fields in the same
 * register. */
static void cci_analysis_transport_enable(void)
{
#if CONFIG_IPRO_LE_AUDIO_CCI_ANALYSIS
    volatile uint32_t * const glb_parm =
        (volatile uint32_t *)IPRO_GLB_PARM_ADDR;

    *glb_parm = *glb_parm | IPRO_GLB_PARM_CCI_TRANSPORT_MSK;
#endif
}
#if !defined(CONFIG_IPRO_LE_AUDIO_NEW_HOST_SMOKE)
#include "app_audio.h"
#endif
#ifdef CONFIG_AUDIO_OUTPUT_UAC
#include "usb_dc.h"
#include "app_uac.h"
#endif

#define TAG "MAIN"

/* Debug GPIO pin for hardware debugging (toggle to detect freeze location) */
#define DEBUG_GPIO_PIN 28

/* Define to test UAC-only without BLE/LE Audio */
#define UAC_ONLY_TEST 0

/*
 * BLE and Audio Initialization (Zephyr blestack)
 */
static void subsystems_init(void)
{
    IPRO_LOGI(TAG, "Initializing subsystems...");

    /* Initialize debug GPIO (pin 28) for hardware watchdog */
    gpio_set_mode(DEBUG_GPIO_PIN, GPIO_OUTPUT_PP_MODE);
    gpio_write(DEBUG_GPIO_PIN, 0);  /* Start LOW */
    IPRO_LOGI(TAG, "[GPIO] Debug pin %d initialized", DEBUG_GPIO_PIN);

#if defined(CONFIG_BSP_USING_DAC) || defined(CONFIG_AUDIO_OUTPUT_UAC)
    // Initialize Audio subsystem (LC3 decoder + DAC/UAC output)
    app_audio_init();
    #ifdef CONFIG_AUDIO_OUTPUT_UAC
    IPRO_LOGI(TAG, "Audio subsystem initialized (UAC output)");
    #else
    IPRO_LOGI(TAG, "Audio subsystem initialized (DAC output)");
    #endif
#else
    IPRO_LOGI(TAG, "Audio subsystem disabled (no output configured)");
#endif

#if UAC_ONLY_TEST
    IPRO_LOGI(TAG, "UAC-only test mode - skipping BLE init");
#else
    {
        int ret;
        // Initialize BLE stack (controller + Zephyr host)
        ret = ble_stack_init();
        if (ret != 0) {
            IPRO_LOGE(TAG, "BLE init failed: %d", ret);
            return;
        }

        // Wait for BLE stack to become ready
        uint32_t timeout = 50; // 5 seconds
        while (!ble_stack_is_ready() && timeout > 0) {
            vTaskDelay(pdMS_TO_TICKS(100));
            timeout--;
        }

        if (!ble_stack_is_ready()) {
            IPRO_LOGE(TAG, "BLE stack ready timeout!");
            return;
        }
    }
#endif

    IPRO_LOGI(TAG, "Subsystems initialized successfully");
}

/*
 * Main Task
 */
static void main_task(void *arg)
{
    IPRO_LOGI(TAG, "IPRO LE Audio Headset v2.0 (Zephyr blestack)");
    IPRO_LOGI(TAG, "Codec: LC3 24kHz/10ms/48kbps (Mono)");
    IPRO_LOGI(TAG, "Mode:  Unicast Server (BAP + VCP + CAP)");

    // Initialize all subsystems
    subsystems_init();

#if defined(CONFIG_BSP_USING_DAC) || defined(CONFIG_AUDIO_OUTPUT_UAC)
    static uint32_t last_rx = 0;
#endif

    // Main loop - monitor audio streaming
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(5000));  // Report every 5 seconds

        /* Toggle GPIO to show main loop is still running */
        static uint8_t gpio_state = 0;
        gpio_state = !gpio_state;
        gpio_write(DEBUG_GPIO_PIN, gpio_state);

#if defined(CONFIG_BSP_USING_DAC) || defined(CONFIG_AUDIO_OUTPUT_UAC)
#if UAC_ONLY_TEST
        {
            app_audio_stats_t stats;
            app_audio_get_stats(&stats);
            if (stats.frames_decoded > 0) {
                IPRO_LOGD(TAG, "AUDIO: dec=%lu", (unsigned long)stats.frames_decoded);
            }
        }
#else
        if (ble_stack_is_ready()) {
            app_audio_stats_t stats;
            app_audio_get_stats(&stats);

            if (stats.frames_received > 0) {
                uint32_t rx_delta = stats.frames_received - last_rx;
                last_rx = stats.frames_received;

                IPRO_LOGD(TAG, "STREAM: rx=%lu (+%lu) dec=%lu lost=%lu",
                          (unsigned long)stats.frames_received, (unsigned long)rx_delta,
                          (unsigned long)stats.frames_decoded, (unsigned long)stats.frames_lost);
            }
        }
#endif
#endif

#ifdef CONFIG_AUDIO_OUTPUT_UAC
        /* USB status check */
        bool uac_ready = app_uac_is_ready();
        if (!uac_ready) {
            IPRO_LOGD(TAG, "Waiting for USB host...");
        }
#endif
    }
}

/*
 * Shell Commands (Simplified for Phase 1)
 */
#ifdef CONFIG_SHELL

static void bond_print_cb(const struct bt_bond_info *info, void *data)
{
    uint32_t *count = data;
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(&info->addr, addr, sizeof(addr));
    (*count)++;
    printf("  bond[%lu]: %s\r\n", (unsigned long)(*count), addr);
}

static uint32_t print_bonds(const char *tag)
{
    uint32_t count = 0;

    printf("%s bonds:\r\n", tag);
    bt_foreach_bond(BT_ID_DEFAULT, bond_print_cb, &count);
    printf("  total=%lu\r\n", (unsigned long)count);
    return count;
}

static int cmd_status(int argc, char **argv)
{
    printf("=== LE Audio Headset Status ===\r\n");
    printf("BLE Ready: %s\r\n", ble_stack_is_ready() ? "Yes" : "No");

#if defined(CONFIG_BSP_USING_DAC) || defined(CONFIG_AUDIO_OUTPUT_UAC)
    app_audio_stats_t stats;
    app_audio_get_stats(&stats);
    printf("Audio Stats:\r\n");
    printf("  Frames Received: %lu\r\n", (unsigned long)stats.frames_received);
    printf("  Frames Decoded:  %lu\r\n", (unsigned long)stats.frames_decoded);
    printf("  Frames Lost:     %lu\r\n", (unsigned long)stats.frames_lost);
    printf("  Decode Errors:   %lu\r\n", (unsigned long)stats.decode_errors);
    if (stats.frames_received > 0) {
        uint32_t loss_rate = (stats.frames_lost * 100) / stats.frames_received;
        printf("  Loss Rate:       %lu%%\r\n", (unsigned long)loss_rate);
    }

    const app_audio_config_t* cfg = app_audio_get_config();
    printf("Audio Config:\r\n");
    printf("  Sample Rate:     %lu Hz\r\n", (unsigned long)cfg->sample_rate);
    printf("  Frame Duration:  %lu us\r\n", (unsigned long)cfg->frame_duration_us);
    printf("  Octets/Frame:    %u bytes\r\n", cfg->octets_per_frame);
#ifdef CONFIG_AUDIO_OUTPUT_UAC
    printf("  Output:          UAC (USB Audio)\r\n");
#else
    printf("  Output:          DAC (Speaker)\r\n");
#endif
#else
    printf("Audio output disabled\r\n");
#endif

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_status, leaudio_status, Show LE Audio headset status);

static int cmd_unpair(int argc, char **argv)
{
    int rc;

    (void)argc;
    (void)argv;

    print_bonds("before");
    rc = bt_unpair(BT_ID_DEFAULT, NULL);
    printf("bt_unpair(all) rc=%d\r\n", rc);
    print_bonds("after");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_unpair, leaudio_unpair, Clear all LE Audio BLE bonds);

#if defined(CONFIG_BSP_USING_DAC) || defined(CONFIG_AUDIO_OUTPUT_UAC)
static int cmd_stats(int argc, char **argv)
{
    app_audio_stats_t stats;
    app_audio_get_stats(&stats);

    printf("rx=%lu dec=%lu lost=%lu err=%lu\r\n",
           (unsigned long)stats.frames_received,
           (unsigned long)stats.frames_decoded,
           (unsigned long)stats.frames_lost,
           (unsigned long)stats.decode_errors);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_stats, leaudio_stats, Show audio statistics);
#endif

#endif /* CONFIG_SHELL */

/*
 * Main Entry Point
 */
int main(void)
{
    // Platform initialization
    platform_init(0);
    cci_analysis_transport_enable();
    ipro_setup_heap();
    hal_boot2_init();
    
    // Initialize logging system
    ipro_log_init(NULL);
    
    IPRO_LOGI(TAG, "IPRO LE Audio Headset Starting...");
    
#ifdef CONFIG_SHELL
    shell_init_with_task(0); // UART0
#endif

#if UAC_ONLY_TEST
    IPRO_LOGI(TAG, "UAC-only test - skipping BLE controller init");
#endif

    // Create main task - subsystems initialized in task
    // Reduced from 8 KB to 4 KB to save RAM
    static StackType_t main_stack[1024];
    static StaticTask_t main_tcb;
    xTaskCreateStatic(main_task, "main", 1024, NULL, 10, main_stack, &main_tcb);
    
    // Start FreeRTOS scheduler
    vTaskStartScheduler();
    
    return 0;
}

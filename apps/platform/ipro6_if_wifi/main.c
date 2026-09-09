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

/* Standard C library headers - must come first for type definitions */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* Project configuration */
#include <generated/autoconf.h>

/* WiFi - must be included early to avoid macro redefinition issues */
#ifdef CONFIG_WIFI6
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"
#include "ipro_fw_api.h"
#include "wl_board_cfg.h"

/* The WiFi MAC hardware ISR, provided by the MAC stack (macsw intc). On IPRO6
 * board_init() does NOT install it (unlike ipro6le_soc, whose board_init
 * registers it), so the app must wire it to WIFI_IRQn itself -- see
 * hal_wifi_start_firmware_task(). */
extern void interrupt0_handler(void);
#endif

#ifdef CONFIG_COMPONENTS_IPERF_ENABLE
#include "iperf.h"
#endif

/* Network */
#ifdef CONFIG_COMPONENTS_LWIP_ENABLE
#include <lwip/tcpip.h>
#endif

/* Shell */
#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#if defined(CONFIG_DEBUG_CONSOLE_USB)
#include "usb_console.h"     /* mirror printf/log + shell over USB CDC */
#endif

/* FreeRTOS */
#include <FreeRTOS.h>
#include <task.h>

/* Filesystem */
#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_POSIX)
#include "evfs.h"
#include "evfs_posix.h"
#endif

/* Event Loop */
#ifdef CONFIG_IPRO_EVENT
#include "ipro_event.h"
/* Declare WIFI_EVENT base (defined in platform_iotsdk.c) */
#ifdef CONFIG_WIFI6
IPRO_EVENT_DECLARE_BASE(WIFI_EVENT);
#endif
#endif

/* HAL headers */
#include "hal_boot2.h"
#include "hal_sys.h"
#include "hal_timer.h"
#include "hal_uart.h"
#include "hal_sec_common.h"

/* USB support */
#ifdef CONFIG_CHERRYUSB
#include "usbd_core.h"
#endif

/* Platform common */
#include "platform_common.h"

/* Flash config struct for brom param test */
#include "drv_sflash.h"
#include "drv_glb.h"
#include "drv_gpio.h"
#include "drv_glb_gpio.h"
#include "ipro6_sdio3.h"
#if defined(CONFIG_COMPONENTS_SDIOWIFI_ENABLE)
#include <trcver_sdio.h>   /* ipro_sdu_init / handshake / read_cb_register / send_raw */
#include "sdio_port_ipro6.h"
#endif
#if defined(CONFIG_COMPONENTS_USBWIFI_ENABLE)
#include <usbwifi.h>       /* usbwifi_start / g_usbwifi */
#endif

/* Low power demo — PDS/HBN/DTIM `lp` command + app flow */
#ifdef CONFIG_DEMO_LOWPOWER
#include "lp_demo.h"
#include "drv_hbn.h"   /* HBN_BASE, HBN_RSV0_OFFSET */
#endif

/*
 * Bootrom flash config test - read from brom_param region in OCRAM.
 *
 * Bootrom writes flash config at 0x1104FE80 (cacheable) before jumping to FW.
 * We read from non-cached alias 0x2104FE80 to avoid cache coherency issues.
 *
 * Layout at 0x1104FE80:
 *   +0x00  "BPRM" magic (4 bytes)
 *   +0x04  total length  (4 bytes, reserved)
 *   +0x08  flash_cfg_len (4 bytes, = 4 + sizeof(SPI_Flash_Cfg_Type))
 *   +0x0C  "FCFG" magic  (4 bytes)
 *   +0x10  SPI_Flash_Cfg_Type (84 bytes)
 */
#define BROM_PARAM_NC_ADDR      0x2104FE80
#define BROM_PARAM_MAGIC        0x4D525042  /* "BPRM" */
#define BROM_FLASH_CFG_MAGIC    0x47464346  /* "FCFG" */

static void test_brom_flash_cfg(void)
{
    volatile uint8_t *base = (volatile uint8_t *)BROM_PARAM_NC_ADDR;
    uint32_t val;

    printf("\r\n=== Bootrom Flash Config Test (@ 0x%08X) ===\r\n",
           BROM_PARAM_NC_ADDR);

    /* raw hex dump first 16 bytes (header) */
    printf("Header hex:");
    for (int i = 0; i < 16; i++)
        printf(" %02X", base[i]);
    printf("\r\n");

    /* check BPRM magic */
    memcpy(&val, (const void *)base, 4);
    printf("BPRM magic: 0x%08X %s\r\n", (unsigned)val,
           val == BROM_PARAM_MAGIC ? "[OK]" : "[FAIL]");
    if (val != BROM_PARAM_MAGIC)
        return;

    /* total length (reserved) */
    memcpy(&val, (const void *)(base + 4), 4);
    printf("Total len : %u\r\n", (unsigned)val);

    /* flash cfg length */
    memcpy(&val, (const void *)(base + 8), 4);
    printf("FCfg len  : %u (expect %u)\r\n", (unsigned)val,
           (unsigned)(4 + sizeof(SPI_Flash_Cfg_Type)));

    /* FCFG magic */
    memcpy(&val, (const void *)(base + 12), 4);
    printf("FCFG magic: 0x%08X %s\r\n", (unsigned)val,
           val == BROM_FLASH_CFG_MAGIC ? "[OK]" : "[FAIL]");
    if (val != BROM_FLASH_CFG_MAGIC)
        return;

    /* parse flash config */
    const volatile uint8_t *p = base + 16;
    SPI_Flash_Cfg_Type cfg;
    memcpy(&cfg, (const void *)p, sizeof(cfg));

    printf("--- SPI Flash Config (%u bytes) ---\r\n",
           (unsigned)sizeof(SPI_Flash_Cfg_Type));
    printf("  ioMode         = 0x%02X\r\n", cfg.ioMode);
    printf("  clkDelay       = 0x%02X\r\n", cfg.clkDelay);
    printf("  clkInvert      = 0x%02X\r\n", cfg.clkInvert);
    printf("  jedecIdCmd     = 0x%02X\r\n", cfg.jedecIdCmd);
    printf("  sectorSize     = %u KB\r\n", cfg.sectorSize);
    printf("  mid            = 0x%02X\r\n", cfg.mid);
    printf("  pageSize       = %u\r\n", cfg.pageSize);
    printf("  chipEraseCmd   = 0x%02X\r\n", cfg.chipEraseCmd);
    printf("  sectorEraseCmd = 0x%02X\r\n", cfg.sectorEraseCmd);
    printf("  writeEnableCmd = 0x%02X\r\n", cfg.writeEnableCmd);
    printf("  pageProgramCmd = 0x%02X\r\n", cfg.pageProgramCmd);
    printf("  fastReadCmd    = 0x%02X\r\n", cfg.fastReadCmd);
    printf("  fastReadQioCmd = 0x%02X\r\n", cfg.fastReadQioCmd);

    /* full raw hex dump */
    printf("Raw cfg hex:\r\n");
    for (unsigned i = 0; i < sizeof(SPI_Flash_Cfg_Type); i++) {
        printf("%02X ", p[i]);
        if ((i + 1) % 16 == 0)
            printf("\r\n");
    }
    if (sizeof(SPI_Flash_Cfg_Type) % 16)
        printf("\r\n");

    printf("=== End Flash Config Test ===\r\n\r\n");
}

#ifdef CONFIG_SHELL
static void cmd_brom_param(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    test_brom_flash_cfg();
}
SHELL_CMD_EXPORT_ALIAS(cmd_brom_param, brom_param,
                       dump bootrom flash config from OCRAM);
#endif

extern uint32_t __heap_start, __heap_size;
extern uint32_t __heap_psram_start, __heap_psram_size;
#ifdef CONFIG_FPGA_RAM_AS_HEAP
extern uint32_t __heap_flash_start, __heap_flash_size;
#endif

static HeapRegion_t xHeapRegions[] = {
    { (uint8_t *)&__heap_start, (size_t)&__heap_size }, /* TCM heap */
#ifdef CONFIG_FPGA_RAM_AS_HEAP
    { (uint8_t *)&__heap_flash_start, (size_t)&__heap_flash_size }, /* FPGA RAM heap */
#endif
    { NULL, 0 }, /* Terminates the array. */
    { NULL, 0 }  /* Terminates the array. */
};

void __attribute__((weak)) vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    puts("Stack Overflow checked\r\n");
    if (pcTaskName) {
        printf("Stack name %s\r\n", pcTaskName);
    }
    while (1) {
        /*empty here*/
    }
}

void __attribute__((weak)) vApplicationMallocFailedHook(void)
{
    printf("Memory Allocate Failed. Current left size is %d bytes\r\n",
           xPortGetFreeHeapSize());
    while (1) {
        /*empty here*/
    }
}

/* FreeRTOS runtime stats: the weak definitions in freertos_app_hooks.c are used.
 * This app used to carry its own copy that returned `csrr mcycle` unshifted -
 * the low 32 bits only, so it wrapped every 17.9 s at 240 MHz and made every ps
 * percentage meaningless - alongside a vConfigureTimerForRunTimeStats whose
 * counter was never read. Deleted rather than fixed in place, so the corrected
 * version has one home instead of two.
 */

void __attribute__((weak)) vApplicationIdleHook(void)
{
#ifdef CONFIG_DEMO_LOWPOWER
    /* Enters PDS when `lp auto on` was issued; no-op otherwise. */
    lp_demo_idle();
#else
    /*empty*/
#endif
}

void __attribute__((weak)) vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, configSTACK_DEPTH_TYPE *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    /* [PERF 07-24] 1024 -> 512 words (4KB -> 2KB); ps high-water: idle task
     * uses <80 words. Freed 2KB funds hot-code-to-RAM. */
    #define APP_IDLE_TASK_STACK_WORDS 512
    static StackType_t uxIdleTaskStack[APP_IDLE_TASK_STACK_WORDS];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    the size is specified in words, not bytes. */
    *pulIdleTaskStackSize = APP_IDLE_TASK_STACK_WORDS;
}

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void __attribute__((weak)) vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    /* [PERF 07-24] app-local override: configTIMER_TASK_STACK_DEPTH is 4096
     * words (16KB) but ps high-water shows the timer task using <100 words.
     * 1024 words (4KB) keeps 10x headroom; freed 12KB funds hot-code-to-RAM. */
    /* [mem-32K] 1024 -> 512 words. Measured high-water on this app is 372 B (93
     * words) across boot + associate + a 30 M uplink and a 20 M downlink, which
     * independently matches the <100 words above. 512 words leaves 5.5x headroom.
     *
     * The obvious worry is a callback that only runs on a rare path, so the
     * timer-service callbacks were enumerated FROM THE LINKED ELF rather than
     * assumed. Only two reach this stack in this image:
     *   - sdio_tx_timer_cb (the 450 ms TX watchdog) calls notify_sdu_worker()
     *     and nothing else; the actual recovery runs on the SMID worker task.
     *     Worth stating because the opposite was assumed when this cut was
     *     proposed - "wait for a WDT to fire" is not the test it sounds like.
     *   - heartbeat_timer_cb -> rnms_send_simple_async_msg_timeout(): the RNM
     *     PING. This is the deepest one, and it runs continuously once the host
     *     is up, so the 372 B above already includes it.
     * The deeper candidates are all ABSENT from the linked image (checked by
     * symbol): do_tcp_timer_trigger and net_iperf_tcp_client_run (the on-device
     * RivieraWaves iperf, not built - CFG_IPERF is undefined here),
     * platform_add_schedule_event (would hand macsw an arbitrary handler to run
     * on this stack; zero callers), and the ipro_event delayed/periodic
     * callbacks. Re-check that list before enabling any of them.
     * configCHECK_FOR_STACK_OVERFLOW is 2, so a mistake here traps rather than
     * corrupting silently. */
    #define APP_TMR_TASK_STACK_WORDS 512
    static StackType_t uxTimerTaskStack[APP_TMR_TASK_STACK_WORDS];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    the size is specified in words, not bytes. */
    *pulTimerTaskStackSize = APP_TMR_TASK_STACK_WORDS;
}

void user_vAssertCalled(const char *const pcFileName, unsigned long ulLine) __attribute__((weak, alias("vAssertCalled")));
void __attribute__((weak)) vAssertCalled(const char *const pcFileName, unsigned long ulLine)
{
    printf("Assert raised by task %s, called by %p\r\n", pcTaskGetName(xTaskGetCurrentTaskHandle()), __builtin_return_address(0));
    taskDISABLE_INTERRUPTS();
    while (1)
        ;
}

#ifdef CONFIG_WIFI6
#define WIFI_STACK_SIZE     (1536)
#define TASK_PRIORITY_FW    (30)

TaskHandle_t wifi_task_handler = NULL;

int hal_wifi_start_firmware_task(void)
{
    /* Install the MAC HW ISR on WIFI_IRQn (wifi_to_cpu, IRQ_NUM_BASE+54) before
     * the firmware task runs. Interrupt_Handler_Register writes the ECLIC
     * vector_table[] slot AND enables the line; a bare CPU_Interrupt_Enable
     * would leave vector_table[WIFI_IRQn]=0, so the first MAC interrupt during
     * mm_start dispatches through a NULL vector (JALMNXTI -> 0) and wedges
     * bring-up with a wild-jump exception. */
    Interrupt_Handler_Register(WIFI_IRQn, interrupt0_handler);
    xTaskCreate(wifi_main, (char *)"fw", WIFI_STACK_SIZE, NULL, TASK_PRIORITY_FW, &wifi_task_handler);
    return 0;
}

static wifi_conf_t conf = {
    .country_code = "CN",
};

static bool wifi_ssid_conf = false;
static char conf_ap_ssid[33] = {0};
static char conf_ap_psk[66] = {0};

int connect_wifi(void)
{
    int ret;
    char *ssid;
    char *key;
    char *bssid = NULL;
    char *akm_str = NULL;
    uint16_t freq[2] = {0};
    uint8_t use_dhcp = 1;

    if (wifi_ssid_conf) {
        if (*conf_ap_ssid != '\0')
            ssid = conf_ap_ssid;
        else
            return -1;
        if (*conf_ap_psk != '\0')
            key = conf_ap_psk;
        else
            key = NULL;
    } else {
        return -1;
    }

    ret = wifi_mgmr_sta_autoconnect_enable();
    if (ret < 0) {
        printf("wifi_mgmr_sta_autoconnect_enable failed\r\n");
    }

    ret = wifi_sta_connect(ssid, key, bssid, akm_str, 1, freq[0], freq[1], use_dhcp);
    if (ret < 0) {
        printf("wifi_sta_connect failed\r\n");
    }

    return 0;
}

/* Start the WiFi firmware task once. Both the `stack_wifi` command and the
 * boot-time auto-start come through here, so whichever runs second is a no-op.
 * Returns true if this call is the one that started it. */
static bool if_wifi_stack_start(void)
{
    static uint8_t stack_wifi_init = 0;

    if (1 == stack_wifi_init) {
        return false;
    }
    stack_wifi_init = 1;

    hal_wifi_start_firmware_task();
    return true;
}

#ifdef CONFIG_SHELL
static void stack_wifi(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (!if_wifi_stack_start()) {
        puts("Wi-Fi Stack Started already!!!\r\n");
    }
}
SHELL_CMD_EXPORT(stack_wifi, start wifi stack);

/* mac_burn <aa:bb:cc:dd:ee:ff> <magic> — one-time factory-MAC burn into efuse
 * MAC slot 0 (EF_DATA 0x14/0x18, parity in high word [21:16]).
 *
 * Safety rails:
 *   - efuse programming is IRREVERSIBLE: the write only runs when <magic> ==
 *     MAC_BURN_MAGIC (0xEFAC); anything else just prints status.
 *   - slot 0 ONLY: if slot 0 is already written the command is ignored (slots
 *     1/2 borrow ef_sw_usage_2/3 and key_slot_11 — leave them alone).
 *   - multicast / all-zero / all-ff addresses are rejected.
 *
 * After a successful burn, platform_get_mac() picks the efuse MAC up on the
 * next boot (stack_wifi reads it once at init) — reboot to take effect. */
#define MAC_BURN_MAGIC  0xEFACu
static void cmd_mac_burn(int argc, char **argv)
{
    /* extern (not #include): drv_ef_cfg.h drags std_drv types that clash in
     * this TU — same pattern as platform_iotsdk.c's efuse externs. */
    extern uint8_t EF_Ctrl_Is_MAC_Address_Slot_Empty(uint8_t slot, uint8_t reload);
    extern int EF_Ctrl_Write_MAC_Address_Opt(uint8_t slot, uint8_t mac[6], uint8_t program);
    extern int EF_Ctrl_Read_MAC_Address_Opt(uint8_t slot, uint8_t mac[6], uint8_t reload);

    uint8_t mac[6], wr[6], rb[6] = { 0 };
    uint8_t slot0_empty = EF_Ctrl_Is_MAC_Address_Slot_Empty(0, 1);
    unsigned v[6];
    int i;

    if (argc < 3) {
        printf("usage: mac_burn <aa:bb:cc:dd:ee:ff> <magic>\r\n");
        printf("  IRREVERSIBLE efuse write to MAC slot 0; magic gates the burn\r\n");
        printf("  slot 0: %s\r\n", slot0_empty ? "EMPTY (burnable)" : "WRITTEN (command will be ignored)");
        if (!slot0_empty && EF_Ctrl_Read_MAC_Address_Opt(0, rb, 1) == 0) {
            printf("  slot 0 mac: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                   rb[0], rb[1], rb[2], rb[3], rb[4], rb[5]);
        }
        return;
    }

    if (sscanf(argv[1], "%2x:%2x:%2x:%2x:%2x:%2x",
               &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]) != 6) {
        printf("[mac_burn] bad MAC format (want aa:bb:cc:dd:ee:ff)\r\n");
        return;
    }
    for (i = 0; i < 6; i++) {
        mac[i] = (uint8_t)v[i];
    }

    if (strtoul(argv[2], NULL, 0) != MAC_BURN_MAGIC) {
        printf("[mac_burn] magic mismatch — nothing written\r\n");
        return;
    }
    if (mac[0] & 0x01) {
        printf("[mac_burn] multicast address rejected\r\n");
        return;
    }
    for (i = 0; i < 6 && mac[i] == 0x00; i++) { }
    if (i == 6) {
        printf("[mac_burn] all-zero address rejected\r\n");
        return;
    }
    for (i = 0; i < 6 && mac[i] == 0xff; i++) { }
    if (i == 6) {
        printf("[mac_burn] all-ff address rejected\r\n");
        return;
    }
    if (!slot0_empty) {
        printf("[mac_burn] slot 0 already written — command ignored\r\n");
        return;
    }

    /* EF_Ctrl_Write_MAC_Address_Opt reverses its input in place — hand it a
     * copy. program=1 burns the fuses immediately. */
    memcpy(wr, mac, 6);
    __disable_irq();
    EF_Ctrl_Write_MAC_Address_Opt(0, wr, 1);
    __enable_irq();

    /* Trust the read-back (parity-checked), not the driver return code. */
    if (EF_Ctrl_Read_MAC_Address_Opt(0, rb, 1) == 0 && memcmp(rb, mac, 6) == 0) {
        printf("[mac_burn] slot 0 burned OK: %02x:%02x:%02x:%02x:%02x:%02x — reboot to apply\r\n",
               rb[0], rb[1], rb[2], rb[3], rb[4], rb[5]);
    } else {
        printf("[mac_burn] VERIFY FAILED: wrote %02x:%02x:%02x:%02x:%02x:%02x, read back %02x:%02x:%02x:%02x:%02x:%02x\r\n",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
               rb[0], rb[1], rb[2], rb[3], rb[4], rb[5]);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_mac_burn, mac_burn, burn factory MAC to efuse slot0 - needs magic);

#if defined(CONFIG_COMPONENTS_SDIOWIFI_ENABLE)
extern int hostrouter_init(void);
#endif

/* Start whichever transport bridge is compiled in, once. Neither
 * hostrouter_init() nor usbwifi_start() is idempotent, so the guard lives here
 * rather than in the callers: the shell command and the boot-time auto-start
 * share this entry and the second one is a no-op. Returns true if this call is
 * the one that started it. */
static bool if_wifi_bridge_start(void)
{
    static uint8_t bridge_started = 0;

    if (1 == bridge_started) {
        return false;
    }
    bridge_started = 1;

#if defined(CONFIG_COMPONENTS_SDIOWIFI_ENABLE)
    hostrouter_init();
#elif defined(CONFIG_COMPONENTS_USBWIFI_ENABLE)
    usbwifi_start(&g_usbwifi);
#endif
    return true;
}

#if defined(CONFIG_COMPONENTS_SDIOWIFI_ENABLE)
/* P3: start the SMID SDIO-WiFi device bridge (queue/ADMA transport). The RPi
 * (ipronet) host then enumerates the device and binds iproeth0. Run `stack_wifi`
 * first so the WiFi MAC is available for the scratch handshake. */
static void sdiowifi(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (!if_wifi_bridge_start()) {
        puts("sdio-wifi bridge started already\r\n");
    }
}
SHELL_CMD_EXPORT(sdiowifi, start sdio-wifi device bridge);

/* Downlink-stall debug: expose the device<->host SDIO counters. upld_* is the
 * device->host upload path (WiFi RX -> host, i.e. iperf UDP-RX); tx_pool_drops
 * is the host->device path; pm26 is the host->device credit throttle. `sdio_diag r`
 * zeroes the cumulative counters for a clean before/after around a load test. */
static void cmd_sdio_diag(int argc, char **argv)
{
    extern volatile uint32_t sdio_upld_attempt;
    extern volatile uint32_t sdio_upld_fail;
    extern volatile uint32_t sdio_wifi_tx_pool_drops;
    extern volatile uint32_t pm26_throttle_engaged_cnt;
    extern volatile uint32_t pm26_throttle_released_cnt;
    extern volatile uint32_t pm26_throttle_active;

    if (argc > 1 && argv[1][0] == 'r') {
        sdio_upld_attempt = 0;
        sdio_upld_fail = 0;
        sdio_wifi_tx_pool_drops = 0;
        pm26_throttle_engaged_cnt = 0;
        pm26_throttle_released_cnt = 0;
        printf("[sdio_diag] cumulative counters reset\r\n");
        return;
    }
    printf("--- sdio_diag ---\r\n");
    printf("upld_attempt=%lu upld_fail=%lu (dev->host WiFi-RX upload)\r\n",
           (unsigned long)sdio_upld_attempt, (unsigned long)sdio_upld_fail);
    printf("tx_pool_drops=%lu (host->dev, empty wifi-tx pbuf)\r\n",
           (unsigned long)sdio_wifi_tx_pool_drops);
    printf("pm26 engaged=%lu released=%lu active=%lu\r\n",
           (unsigned long)pm26_throttle_engaged_cnt,
           (unsigned long)pm26_throttle_released_cnt,
           (unsigned long)pm26_throttle_active);
}
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_diag, sdio_diag, sdio dev<->host up/down counters);

/* Full transport stats: SMID upld/dnld queue depth + IRQ-event histogram +
 * upld copy pool + tx_buf credit state. Sample DURING a load test: upld_wait
 * pinned at 8 = host drains slower than the device produces; ~0 = the device
 * side is the producer-limited stage. */
static void cmd_sdio_stats(int argc, char **argv)
{
    extern int hostrouter_stats_display(void);

    (void)argc; (void)argv;
    hostrouter_stats_display();
}
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_stats, sdio_stats, sdio transport queue/pool stats);

#if defined(CONFIG_SMID_RAW_TX_TEST)
/* smid_flood N LEN: continuous raw device->host upld stream to measure the SMID
 * transport ceiling (no WiFi in the path). Packs [2B len][body]... + 0x0000
 * sentinel aggregates like the D-2 exerciser in ipro6_demo; pair with the host
 * ipronet smid_perf_rx hook (pause its RX poll thread first via smid_rx_pause).
 * Requires the sdiowifi bridge up (`sdiowifi`) so the SMID transport is live. */
#include "sdio_port_ipro6.h"
#define SMID_FLOOD_AGG_SIZE 6144   /* == SDIO_DNLD_AGG_BUF_SIZE (upld aggregate) */
static void cmd_smid_flood(int argc, char **argv)
{
    int n   = (argc > 1) ? atoi(argv[1]) : 5000;
    int len = (argc > 2) ? atoi(argv[2]) : 2048;
    static uint8_t agg[SMID_FLOOD_AGG_SIZE];
    uint32_t off = 0, aligned;
    int k = 0, f = 0, uplds = 0;

    if (len < 1 || len > 2048) {
        len = 2048;
    }
    while (off + 2 + (uint32_t)len + 2 <= sizeof(agg)) {
        agg[off]     = len & 0xff;
        agg[off + 1] = (len >> 8) & 0xff;
        off += 2 + len;
        k++;
    }
    agg[off] = 0; agg[off + 1] = 0; off += 2;
    aligned = (off + 511u) & ~511u;   /* host SMID block size = 512 */
    if (aligned > sizeof(agg)) {
        aligned = sizeof(agg);
    }
    while (off < aligned) {
        agg[off++] = 0;
    }

    while (f < n) {
        int tries = 0;
        while (ipro_sdio_send_raw(agg, (uint16_t)off) != 0) {
            vTaskDelay(1);
            if (++tries > 10000) {
                printf("[smid_flood] aborted at %d frames (host not draining)\r\n", f);
                return;
            }
        }
        f += k;
        uplds++;
    }
    printf("[smid_flood] queued %d frames x %dB (%d/agg, %luB/upld) in %d upld\r\n",
           f, len, k, (unsigned long)aligned, uplds);
}
SHELL_CMD_EXPORT_ALIAS(cmd_smid_flood, smid_flood, smid raw dev->host flood N LEN);
#endif /* CONFIG_SMID_RAW_TX_TEST */

#endif

#if defined(CONFIG_COMPONENTS_USBWIFI_ENABLE)
/* Start the USB-WiFi device bridge (CherryUSB CDC transport). The RPi
 * (ipronet, CONFIG_IPRO_INTF=USB) enumerates VID/PID 0x349B:0x616F and binds
 * iproeth0. Run `stack_wifi` first so the WiFi MAC is available. On IPRO6 the
 * CherryUSB device port (usb_dc_init) handles USB clock/PHY/IRQ bring-up, so no
 * board-side clock init is needed here (unlike the IPRO6LE bl_usb_clock_init). */
static void usbwifi(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (!if_wifi_bridge_start()) {
        puts("usb-wifi bridge started already\r\n");
    }
}
SHELL_CMD_EXPORT(usbwifi, start usb-wifi device bridge);

#endif

static void _cli_init(void)
{
#if defined(CONFIG_WIFI6)
#ifdef CONFIG_USE_MACSW
    extern int wifi6_cli_init(void);
    wifi6_cli_init();
#else
    extern int bl_wifi6_cli_init(void);
    bl_wifi6_cli_init();
#endif
#endif
#if defined(CONFIG_COMPONENTS_IPERF_ENABLE)
    /* Also anchors iperf_cli.o so its SHELL_CMD_EXPORT survives archive GC */
    iperf_cli_register();
#endif

#if defined(CONFIG_MM_SUPPORT)
    #pragma message "CONFIG_MM_SUPPORT defined"
        extern void demo_mjdec_init(void);
        extern void demo_mjpeg_init(void);

        demo_mjdec_init();
        demo_mjpeg_init();
#endif
}
#endif /* CONFIG_SHELL */

#if defined(CONFIG_IPRO_EVENT) && defined(CONFIG_WIFI6)
/* WiFi event handler using ipro_event system */
static void wifi_event_handler(const ipro_event_t *event, void *user_data)
{
    (void)user_data;

    if (event->event_base != WIFI_EVENT) {
        return;
    }

    int32_t code = event->event_id;

    /* Handle WiFi events based on event ID (same as CODE_WIFI_ON_* defines) */
    switch (code) {
        case CODE_WIFI_ON_INIT_DONE:
            printf("[APP] [EVT] CODE_WIFI_ON_INIT_DONE\r\n");
            wifi_mgmr_init(&conf);
            break;
        case CODE_WIFI_ON_MGMR_DONE:
            printf("[APP] [EVT] CODE_WIFI_ON_MGMR_DONE\r\n");
            /* wifi_mgmr_init() defaults disable_autoreconnect to 1, i.e. the
             * STA never re-associates on its own. For a product whose whole
             * job is to be a network interface for a host that is exactly
             * wrong: bench 2026-07-25, one LAB02 deauth (reason_code 1 under
             * over-offer) left iproeth0 carrier-down until someone typed
             * wifi_sta_connect by hand. Declare the intent here, per app, and
             * leave the SDK default alone for products that want manual
             * control. The host can still turn it off over RNM. */
            wifi_mgmr_sta_autoconnect_enable();
            connect_wifi();
#ifdef CONFIG_IF_WIFI_AUTO_START
            /* The WiFi MAC is up now, which is the precondition the transport
             * bridge needs for its scratch handshake -- start it from here
             * rather than from a fixed delay after the stack. A host that
             * enumerates before this point gets no CMD5 answer (enum 207). */
            if_wifi_bridge_start();
#endif
            break;
        case CODE_WIFI_ON_SCAN_DONE:
            printf("[APP] [EVT] CODE_WIFI_ON_SCAN_DONE\r\n");
            wifi_mgmr_sta_scanlist();
            break;
        case CODE_WIFI_ON_CONNECTED:
            printf("[APP] [EVT] CODE_WIFI_ON_CONNECTED\r\n");
            break;
        case CODE_WIFI_ON_GOT_IP:
            printf("[APP] [EVT] CODE_WIFI_ON_GOT_IP\r\n");
            printf("[SYS] Memory left is %d Bytes\r\n", xPortGetFreeHeapSize());
            break;
        case CODE_WIFI_ON_DISCONNECT:
            printf("[APP] [EVT] CODE_WIFI_ON_DISCONNECT\r\n");
            break;
        case CODE_WIFI_ON_AP_STARTED:
            printf("[APP] [EVT] CODE_WIFI_ON_AP_STARTED\r\n");
            break;
        case CODE_WIFI_ON_AP_STOPPED:
            printf("[APP] [EVT] CODE_WIFI_ON_AP_STOPPED\r\n");
            break;
        case CODE_WIFI_ON_EXIT_PS:
            printf("[APP] [EVT] CODE_WIFI_ON_EXIT_PS\r\n");
            break;
        default:
            printf("[APP] [EVT] Unknown code %ld\r\n", (long)code);
            break;
    }
}
#endif /* CONFIG_IPRO_EVENT && CONFIG_WIFI6 */
#endif /* CONFIG_WIFI6 */

static void main_loop_proc(void *arg)
{
    (void)arg;

#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_POSIX)
    evfs_posix_init();
#endif

#ifdef CONFIG_IPRO_EVENT
    /* Initialize event loop */
    int ret = ipro_event_loop_create_default();
    if (ret == IPRO_OK) {
        printf("[EVENT] Event loop initialized\r\n");
#ifdef CONFIG_WIFI6
        /* Register WiFi event handler */
        ret = ipro_event_handler_register(WIFI_EVENT, IPRO_EVENT_ANY_ID,
                                          wifi_event_handler, NULL);
        if (ret == IPRO_OK) {
            printf("[EVENT] WiFi event handler registered\r\n");
        } else {
            printf("[EVENT] Failed to register WiFi handler: %d\r\n", ret);
        }
#endif
    } else {
        printf("[EVENT] Failed to create event loop: %d\r\n", ret);
    }
#endif

#ifdef CONFIG_SHELL
    puts("Init Shell (RT-Thread FinSH based)\r\n");
    shell_init_with_task(UART0_INDEX);
#if defined(CONFIG_DEBUG_CONSOLE_USB)
    /* Bring up the USB CDC console (mirrors UART; needs scheduler + WIFIPLL up). */
    usb_console_init();
#endif
#ifdef CONFIG_WIFI6
    _cli_init();
#endif
#endif

#ifdef CONFIG_IF_WIFI_AUTO_START
    /* Last, deliberately: the WiFi event handler above must already be
     * registered or CODE_WIFI_ON_MGMR_DONE can fire before anyone is listening
     * and the bridge would never start. The shell task is created before this
     * too, so the console stays usable while the firmware task runs. */
    printf("[APP] auto-start: WiFi stack, bridge follows on MGMR_DONE\r\n");
    if_wifi_stack_start();
#endif

    /* Keep task alive - shell runs in its own task */
    for (;;) {
        vTaskDelay(portMAX_DELAY);
    }
}

void setup_heap(void)
{
    printf("Heap Info start \r\n");

    for (int i = 0; i < 4; i++) {
        if (xHeapRegions[i].pucStartAddress == NULL)
            break;
        printf("Heap Info: %zd KB @ [%p ~ %p]\r\n",
               xHeapRegions[i].xSizeInBytes >> 10,
               xHeapRegions[i].pucStartAddress,
               xHeapRegions[i].pucStartAddress + xHeapRegions[i].xSizeInBytes);
    }
    vPortDefineHeapRegions(xHeapRegions);
}

/* [PERF 07-24] 1024 -> 512 words; ps high-water: main_loop uses <140 words. */
static StackType_t main_loop_proc_stack[384];
static StaticTask_t main_loop_proc_task;

/* P2's inline smid_p2_bringup() (clock/reset/pinmux + ipro6_sdio3_init +
 * keep-ready poll) has moved into the sdiowifi transport glue (ipro_sdu_init /
 * ipro_sdio_handshake in trcver_sdio_ipro6.c + sdio_port_ipro6.c). Start it at
 * runtime via the `sdiowifi` shell command. */

int main(void)
{
#ifdef CONFIG_DEMO_LOWPOWER
    /* Clear HBN_RSV0 immediately so a hardware reset always cold-boots. Without
     * this, if the staged LP firmware ever crashes, reset would fast-wake back
     * into it -> crash loop. */
    *(volatile uint32_t *)(HBN_BASE + HBN_RSV0_OFFSET) = 0;
#endif

    platform_init(0);

    /* WRAM/EM split (em_sel) is programmed at board_init() entry — see
     * board_wram_em_sel_init() in bsp/board/ipro6/board.c. HW-probed
     * 2026-07-10: GLB_SRAM_CFG3 IS writable there (the earlier "not writable
     * before main()" note was a misdiagnosis). */

    /* UART0 on GPIO28(TX)/GPIO29(RX) @ 115200 8N1. Pads are muxed in
     * board_pin_mux_init(); UART0_CONFIG defaults to 2 Mbaud, so override the
     * baudrate before bringing the peripheral up. */
    uart_set_baudrate(UART0_INDEX, 115200);
    uart_init(UART0_INDEX, 0);

    setup_heap();

    /* SEC engine bring-up (TRNG today). TRNG feeds wpa_supplicant
     * os_get_random (WPA2 SNonce, SAE commit scalar); without it every
     * secure connect dies in the handshake with "Failed to get random
     * data". */
    hal_sec_common_init();

#ifdef CONFIG_DEMO_LOWPOWER
    /* Stage the lp_fw image + bring up the hal_lp framework before the scheduler
     * so the `lp` command and idle auto-PDS are ready. */
    lp_demo_app_init();
#endif

#ifdef FPGA
    hal_boot2_init();
#endif

    /* test: dump bootrom flash config passed via OCRAM */
    test_brom_flash_cfg();

    /* P3: the SMID SDIO-WiFi device bridge now runs under the scheduler, started
     * by the `sdiowifi` shell command (hostrouter_init -> sdiowifi_mgmr_start).
     * The old inline smid_p2_bringup() blocked here forever (pre-scheduler demo). */

#if defined(CONFIG_WIFI6) || defined(CONFIG_BLUETOOTH_LE_ENABLE)
    //hal_board_ctl(BRD_CTL_RF_RESET_DEFAULT, 1);
#endif

    printf("[OS] Starting main_loop task...\r\n");

    xTaskCreateStatic(main_loop_proc, (char *)"main_loop", sizeof(main_loop_proc_stack) / 4, NULL, 10, main_loop_proc_stack, &main_loop_proc_task);

#ifdef CONFIG_COMPONENTS_LWIP_ENABLE
    puts("[OS] Starting TCP/IP Stack...\r\n");
    tcpip_init(NULL, NULL);
#endif

    printf("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();
}

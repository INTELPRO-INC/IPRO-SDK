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

/* mac_burn <aa:bb:cc:dd:ee:ff> <magic> -- one-time factory-MAC burn into efuse
 * MAC slot 0 (EF_DATA 0x14/0x18, parity in high word [21:16]). Ported from
 * ipro6_if_wifi/main.c; it lives in the plain CONFIG_SHELL block (not the
 * CONFIG_WIFI6 one) because the efuse is chip-level -- the No-WiFi LP build
 * needs to burn a MAC too.
 *
 * Safety rails:
 *   - efuse programming is IRREVERSIBLE: the write only runs when <magic> ==
 *     MAC_BURN_MAGIC (0xEFAC); anything else just prints status.
 *   - slot 0 ONLY: if slot 0 is already written the command is ignored (slots
 *     1/2 borrow ef_sw_usage_2/3 and key_slot_11 -- leave them alone).
 *   - multicast / all-zero / all-ff addresses are rejected.
 *
 * After a successful burn, platform_get_mac() picks the efuse MAC up on the
 * next boot (stack_wifi reads it once at init) -- reboot to take effect. */
#define MAC_BURN_MAGIC  0xEFACu
static void cmd_mac_burn(int argc, char **argv)
{
    /* extern (not #include): drv_ef_cfg.h drags std_drv types that clash in
     * this TU -- same pattern as platform_iotsdk.c's efuse externs. */
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
        printf("[mac_burn] magic mismatch -- nothing written\r\n");
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
        printf("[mac_burn] slot 0 already written -- command ignored\r\n");
        return;
    }

    /* EF_Ctrl_Write_MAC_Address_Opt reverses its input in place -- hand it a
     * copy. program=1 burns the fuses immediately. */
    memcpy(wr, mac, 6);
    __disable_irq();
    EF_Ctrl_Write_MAC_Address_Opt(0, wr, 1);
    __enable_irq();

    /* Trust the read-back (parity-checked), not the driver return code. */
    if (EF_Ctrl_Read_MAC_Address_Opt(0, rb, 1) == 0 && memcmp(rb, mac, 6) == 0) {
        printf("[mac_burn] slot 0 burned OK: %02x:%02x:%02x:%02x:%02x:%02x -- reboot to apply\r\n",
               rb[0], rb[1], rb[2], rb[3], rb[4], rb[5]);
    } else {
        printf("[mac_burn] VERIFY FAILED: wrote %02x:%02x:%02x:%02x:%02x:%02x, read back %02x:%02x:%02x:%02x:%02x:%02x\r\n",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
               rb[0], rb[1], rb[2], rb[3], rb[4], rb[5]);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_mac_burn, mac_burn, burn factory MAC to efuse slot0 - needs magic);
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
 * the low 32 bits only, so it wrapped in seconds and made every ps percentage
 * meaningless - alongside a vConfigureTimerForRunTimeStats whose counter was
 * never read. Deleted rather than fixed in place, so the corrected version has
 * one home instead of two. */

void __attribute__((weak)) vApplicationIdleHook(void)
{
#ifdef CONFIG_DEMO_LOWPOWER
    /* Credit tickless oversleep to the kernel clock first: this hook is the
     * one idle-task context where the scheduler is live (xTaskCatchUpTicks
     * asserts inside the tickless sleep hook itself). */
    lp_tickless_tick_catchup();
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
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
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
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void user_vAssertCalled(const char *const pcFileName, unsigned long ulLine) __attribute__((weak, alias("vAssertCalled")));
void __attribute__((weak)) vAssertCalled(const char *const pcFileName, unsigned long ulLine)
{
    /* Park the caller in a retained trace word FIRST: an assert raised while
     * the UART is de-inited (e.g. inside the PDS entry/wake window) prints
     * into the void, and this loop is unreachable to the shell. Readable
     * post-mortem via JTAG or `lp md 2104ffd0`. */
    *(volatile uint32_t *)0x2104FFD0u = (uint32_t)__builtin_return_address(0);
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

#ifdef CONFIG_SHELL
static void stack_wifi(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    /*wifi fw stack and thread stuff*/
    static uint8_t stack_wifi_init = 0;

    if (1 == stack_wifi_init) {
        puts("Wi-Fi Stack Started already!!!\r\n");
        return;
    }
    stack_wifi_init = 1;

    hal_wifi_start_firmware_task();
}
SHELL_CMD_EXPORT(stack_wifi, start wifi stack);

/* --- Diagnostic: MAC HW MIB TX-response counters + RX-SIFS timing ----------
 * Localizes the TX->RX turnaround fault seen on-air (DUT floods RTS/BAR though
 * the AP answers with CTS/BlockAck). The MAC HW updates `machw_mib` (section
 * MACHWMIB) live. The HW increments rts_success only when it actually RECEIVES
 * the CTS after our RTS; rts_failure when it does not. So:
 *   rts_fail >> rts_ok  => HW never saw the CTS  => TX->RX RX-after-TX fault
 *                          (below the MAC SW; SW can't "ignore" a CTS the HW
 *                           counted, because success is counted in HW).
 *   rts_ok high, ack_fail high => RTS/CTS ok but data not ACKed (other path).
 * Read as raw u32 words at the stable pre-NX_MAC_HE struct offsets to avoid a
 * macsw internal-header dependency in the app. Run AFTER stack_wifi (needs the
 * modem clocked to read MDM_RXCTRL5). */
extern volatile uint32_t machw_mib[];
static void txmib(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    volatile uint32_t *m = machw_mib;
    uint32_t tx_mpdu = 0, failed = 0, retry = 0, rts_ok = 0, rts_fail = 0, ack_fail = 0, rx_mpdu = 0;
    for (int t = 0; t < 8; t++) {
        tx_mpdu  += m[12 + t];  /* rw_qos_utransmitted_mpdu_count[8] @0x30 */
        failed   += m[28 + t];  /* dot11_qos_failed_count[8]         @0x70 */
        retry    += m[36 + t];  /* dot11_qos_retry_count[8]          @0x90 */
        rts_ok   += m[44 + t];  /* dot11_qos_rts_success_count[8]    @0xB0 */
        rts_fail += m[52 + t];  /* dot11_qos_rts_failure_count[8]    @0xD0 */
        ack_fail += m[60 + t];  /* rw_qos_ack_failure_count[8]       @0xF0 */
        rx_mpdu  += m[68 + t];  /* rw_qos_ureceived_mpdu_count[8]    @0x110 */
    }
    uint32_t fcs     = m[1];    /* dot11_fcs_error_count @0x4 */
    uint32_t phyer   = m[2];    /* rw_rx_phy_error_count @0x8 */
    uint32_t rxctrl5 = *(volatile uint32_t *)0x24C010D4;  /* MDM_RXCTRL5 */
    uint32_t sifs    = rxctrl5 & 0x1FFF;                  /* RXSIFSDELAY[12:0], reset 0x780 */

    printf("=== MAC HW MIB ===\r\n");
    printf("TX  mpdu=%lu rts_ok=%lu rts_fail=%lu ack_fail=%lu retry=%lu failed=%lu\r\n",
           (unsigned long)tx_mpdu, (unsigned long)rts_ok, (unsigned long)rts_fail,
           (unsigned long)ack_fail, (unsigned long)retry, (unsigned long)failed);
    printf("RX  mpdu=%lu fcs_err=%lu phy_err=%lu\r\n",
           (unsigned long)rx_mpdu, (unsigned long)fcs, (unsigned long)phyer);
    printf("RXSIFSDELAY=0x%lx (reset 0x780) RXCTRL5=0x%08lx\r\n",
           (unsigned long)sifs, (unsigned long)rxctrl5);

    /* Reliable RAM-global discriminator: txl_ba_push() runs from the RX ISR
     * when the HW *receives* a BlockAck. call>0 => HW gets the SIFS-spaced BA
     * (fault is SW-side processing); call~0 while AP is sending BAs on-air
     * => HW never receives the post-TX response (TX->RX turnaround fault).
     * ih_drain_* = RX-ISR frame drain activity (is the RX ISR even firing?). */
    extern uint32_t ba_push_call_cnt, ba_push_reset_cnt, ba_push_ac_bad_cnt;
    extern uint32_t ba_push_null_cnt, ba_push_ok_cnt, ba_push_decode_ok_cnt;
    printf("ba_push call=%lu reset=%lu ac_bad=%lu null=%lu ok=%lu decode_ok=%lu\r\n",
           (unsigned long)ba_push_call_cnt, (unsigned long)ba_push_reset_cnt,
           (unsigned long)ba_push_ac_bad_cnt, (unsigned long)ba_push_null_cnt,
           (unsigned long)ba_push_ok_cnt, (unsigned long)ba_push_decode_ok_cnt);

    /* Leg-2 per-MPDU delivery ratio + cfm latency + no-BAR diagnostics. */
    extern void macsw_tx_kpi_print(void);
    macsw_tx_kpi_print();
}
SHELL_CMD_EXPORT(txmib, MAC HW MIB TX-response + SIFS);

/* Leg-2 feed-bottleneck A/B: WiFi task (cfm/push/credit consumer, default prio 27)
 * sits below producer tcpip(28), RX(28), TX-ctrl(29) -> priority inversion stalls
 * cfm + queue_tx handoff -> TX feed-rate bound. This toggles the existing (never-
 * called) rtos_wifi_task_prio_high() (prio 30) for a live A/B. */
static void wprio(int argc, char **argv)
{
    extern void rtos_wifi_task_prio_high(void);
    extern void rtos_wifi_task_prio_default(void);
    if (argc >= 2 && argv[1][0] == 'h') {
        rtos_wifi_task_prio_high();
        printf("WiFi task prio -> HIGH (30)\r\n");
    } else if (argc >= 2 && argv[1][0] == 'd') {
        rtos_wifi_task_prio_default();
        printf("WiFi task prio -> DEFAULT (27)\r\n");
    } else {
        printf("usage: wprio <high|default>\r\n");
    }
}
SHELL_CMD_EXPORT(wprio, set WiFi task priority high/default for feed A/B);

/* Leg-2 feed profiling: dump fhost_tx_start per-call timing + the fhost_tx_schedule
 * sc-stage decomposition (deq/qw/ind/null). ipro6le_demo has these CLIs; ipro6_demo
 * did not, so the sc-305us breakdown was unreachable here. */
extern void fhost_tx_trace_print(void);
extern void fhost_tx_trace_reset(void);
static void cmd_tx_trace(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    fhost_tx_trace_print();
}
SHELL_CMD_EXPORT_ALIAS(cmd_tx_trace, tx_trace, fhost_tx per-call + sc-stage timing);

static void cmd_tx_trace_reset(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    fhost_tx_trace_reset();
}
SHELL_CMD_EXPORT_ALIAS(cmd_tx_trace_reset, tx_trace_reset, reset tx_trace counters);

/* WiFi-task-side push cost split (prep/push/agg) — localizes the sc "ind" 220us. */
extern void macif_tx_evt_prof_print(void);
extern void macif_tx_evt_prof_reset(void);
static void cmd_macif_prof(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    macif_tx_evt_prof_print();
}
SHELL_CMD_EXPORT_ALIAS(cmd_macif_prof, macif_prof, WiFi-task tx push cost split);

static void cmd_macif_prof_reset(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    macif_tx_evt_prof_reset();
}
SHELL_CMD_EXPORT_ALIAS(cmd_macif_prof_reset, macif_prof_reset, reset macif prof);

extern void txu_cntrl_push_prof_print(void);
extern void txu_cntrl_push_prof_reset(void);
static void cmd_txu_prof(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    txu_cntrl_push_prof_print();
}
SHELL_CMD_EXPORT_ALIAS(cmd_txu_prof, txu_prof, txu_cntrl_push 3-stage split);

static void cmd_txu_prof_reset(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    txu_cntrl_push_prof_reset();
}
SHELL_CMD_EXPORT_ALIAS(cmd_txu_prof_reset, txu_prof_reset, reset txu push prof);

extern int GLB_Set_MCU_System_CLK_Div(uint8_t mcuClkDiv, uint8_t mcuPBclkDiv);
extern int GLB_Get_MCU_System_CLK_Div(uint8_t *mcuClkDiv, uint8_t *mcuPBclkDiv);
static void cmd_clkdiv(int argc, char **argv)
{
    uint8_t h = 0, b = 0;
    if (argc >= 2)
    {
        uint8_t hd = (uint8_t)atoi(argv[1]);
        uint8_t bd = (argc >= 3) ? (uint8_t)atoi(argv[2]) : hd;
        GLB_Set_MCU_System_CLK_Div(hd, bd);
    }
    GLB_Get_MCU_System_CLK_Div(&h, &b);
    printf("clkdiv hclk_div=%u bclk_div=%u (hclk~=%u MHz)\r\n",
           h, b, (unsigned)(240 / (h + 1)));
}
SHELL_CMD_EXPORT_ALIAS(cmd_clkdiv, clkdiv, set CPU hclk/bclk divider for clock-down A/B);

#if defined(CONFIG_COMPONENTS_SDIOWIFI_ENABLE)
extern int hostrouter_init(void);
/* P3: start the SMID SDIO-WiFi device bridge (queue/ADMA transport). The RPi
 * (ipronet) host then enumerates the device and binds iproeth0. Run `stack_wifi`
 * first so the WiFi MAC is available for the scratch handshake. */
static void sdiowifi(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    hostrouter_init();
}
SHELL_CMD_EXPORT(sdiowifi, start sdio-wifi device bridge);

/* ---- P3 data-plane bring-up test (no WiFi) -------------------------------
 * Validates the SMID queue data plane DUT<->RPi using raw test frames, so the
 * SDIO transport can be verified before the WiFi stack is ready:
 *   smid_test [echo]  bring the transport up; register an RX callback that
 *                     counts + verifies a ramp pattern (byte[i]==(byte[0]+i)).
 *                     With "echo", each received frame is sent straight back.
 *   smid_send N LEN   send N device->host frames of LEN bytes (ramp), for the
 *                     host-read direction test.
 *   smid_stat         print RX/TX counters.
 * Host side: a minimal kernel module CMD53-writes/reads func1 (see RPi notes).
 */
extern void trcver_sdio_stats_display(void);
static volatile uint32_t g_smid_test_rx_frames;
static volatile uint32_t g_smid_test_rx_bytes;
static volatile uint32_t g_smid_test_pat_err;
static volatile uint32_t g_smid_test_tx_frames;
static volatile uint32_t g_smid_test_tx_fail;
static uint8_t  g_smid_test_up;
static uint8_t  g_smid_test_echo;

static void smid_test_rx_cb(void *cb_arg, const void *data_ptr, uint16_t data_len)
{
    const uint8_t *p = (const uint8_t *)data_ptr;

    (void)cb_arg;
    g_smid_test_rx_frames++;
    g_smid_test_rx_bytes += data_len;
    /* DIAG: hexdump the first frame only (1 print — printing every frame stalls
     * the worker on the 115200 console and drains the dnld queue). host frame f:
     * byte i = f+i, so frame 0 should read 00 01 02 ... */
    if (g_smid_test_rx_frames == 1) {
        printf("[smid_rx] f1 len=%u buf=%p: %02x %02x %02x %02x %02x %02x %02x %02x\r\n",
               data_len, data_ptr,
               p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
    }
    for (uint16_t i = 0; i < data_len; i++) {
        if (p[i] != (uint8_t)(p[0] + i)) {
            g_smid_test_pat_err++;
            break;
        }
    }
    if (g_smid_test_echo) {
        if (ipro_sdio_send_raw(data_ptr, data_len) == 0) {
            g_smid_test_tx_frames++;
        } else {
            g_smid_test_tx_fail++;
        }
    }
}

static void smid_test(int argc, char **argv)
{
    g_smid_test_echo = (argc > 1 && argv[1][0] == 'e') ? 1 : 0;
    if (!g_smid_test_up) {
        ipro_sdu_init();
        ipro_sdio_handshake();
        ipro_sdio_read_cb_register(NULL, smid_test_rx_cb, NULL);
        g_smid_test_up = 1;
    }
    printf("[smid_test] transport up, echo=%d. Host: CMD53 write/read func1.\r\n",
           g_smid_test_echo);
}
SHELL_CMD_EXPORT(smid_test, smid data-plane test [echo]);

static void smid_send(int argc, char **argv)
{
    int n   = (argc > 1) ? atoi(argv[1]) : 1;
    int len = (argc > 2) ? atoi(argv[2]) : 256;
    static uint8_t agg[SDIO_DNLD_AGG_BUF_SIZE];   /* 6144 upld aggregate */
    int f = 0, uplds = 0;

    if (!g_smid_test_up) {
        printf("[smid_send] run smid_test first\r\n");
        return;
    }
    if (len < 1 || len > 2048) {
        len = 256;
    }
    /* D-3 upld aggregation: pack [2B len][ramp]... + a 0x0000 sentinel into one
     * buffer and send the whole pack as ONE upld (read-count = total); the host
     * reads the block in one sdio_readsb and de-aggregates. The upld queue is 8
     * deep, so a sequential queue-then-read test sends <= 8 aggregates. */
    while (f < n) {
        uint32_t off = 0;

        while (f < n && off + 2 + (uint32_t)len + 2 <= sizeof(agg)) {
            agg[off]     = len & 0xff;
            agg[off + 1] = (len >> 8) & 0xff;
            for (int i = 0; i < len; i++) {
                agg[off + 2 + i] = (uint8_t)(f + i);   /* ramp seeded by frame index */
            }
            off += 2 + len;
            f++;
        }
        agg[off] = 0; agg[off + 1] = 0; off += 2;      /* 0x0000 sentinel */
        /* Block-align the read-count so the host's sdio_readsb is a single
         * block-mode read: a non-block-multiple read-count splits into a
         * block + a byte transfer, which corrupts the data (-84 CRC). The host
         * de-aggregator stops at the sentinel, so the zero padding is ignored. */
        {
            uint32_t aligned = (off + 511u) & ~511u;   /* 512-byte block-align (host SMID block size) */
            if (aligned > sizeof(agg)) {
                aligned = sizeof(agg);
            }
            while (off < aligned) {
                agg[off++] = 0;
            }
        }
        if (ipro_sdio_send_raw(agg, (uint16_t)off) != 0) {
            g_smid_test_tx_fail++;
            break;
        }
        uplds++;
    }
    g_smid_test_tx_frames += f;
    printf("[smid_send] queued %d frames x %dB in %d upld\r\n", f, len, uplds);
}
SHELL_CMD_EXPORT(smid_send, smid device->host send N LEN);

/* smid_flood N LEN: continuous device->host upld stream for the sustained
 * upld-speed test. Re-sends one aggregate template, retrying on a full upld
 * pool/queue (the worker frees buffers as the host reads). With the pre-
 * allocated upld pool (ipro_sdio_send_raw no longer malloc-per-aggregate) this
 * no longer exhausts the heap. */
static void smid_flood(int argc, char **argv)
{
    int n   = (argc > 1) ? atoi(argv[1]) : 5000;
    int len = (argc > 2) ? atoi(argv[2]) : 2048;
    static uint8_t agg[SDIO_DNLD_AGG_BUF_SIZE];
    uint32_t off = 0, aligned;
    int k = 0, f = 0, uplds = 0;

    if (!g_smid_test_up) {
        printf("[smid_flood] run smid_test first\r\n");
        return;
    }
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
    aligned = (off + 511u) & ~511u;   /* 512-byte block-align (host SMID block size) */
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
                g_smid_test_tx_fail++;
                printf("[smid_flood] aborted at %d frames (host not draining)\r\n", f);
                g_smid_test_tx_frames += f;
                return;
            }
        }
        f += k;
        uplds++;
    }
    g_smid_test_tx_frames += f;
    printf("[smid_flood] queued %d frames x %dB (%d/agg) in %d upld\r\n", f, len, k, uplds);
}
SHELL_CMD_EXPORT(smid_flood, smid device->host continuous stream N LEN);

static void smid_stat(int argc, char **argv)
{
    (void)argc; (void)argv;
    printf("[smid_test] rx_frames=%lu rx_bytes=%lu pat_err=%lu tx_frames=%lu tx_fail=%lu\r\n",
           (unsigned long)g_smid_test_rx_frames, (unsigned long)g_smid_test_rx_bytes,
           (unsigned long)g_smid_test_pat_err,   (unsigned long)g_smid_test_tx_frames,
           (unsigned long)g_smid_test_tx_fail);
    trcver_sdio_stats_display();
}
SHELL_CMD_EXPORT(smid_stat, smid data-plane test stats);
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


#if defined(CONFIG_DEMO_LCD)
    #pragma message "CONFIG_DEMO_LCD defined"
        extern void demo_lcd_init(void);
        demo_lcd_init();
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
            connect_wifi();
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
#ifdef CONFIG_DEMO_LOWPOWER
            /* An armed tickless standby can never see the host wake command
             * on a dead association — kick it to disarm (no-op if idle). */
            lp_tickless_on_disconnect();
#endif
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

static StackType_t main_loop_proc_stack[1024];
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

    /* WRAM/EM split (em_sel) is programmed in board_init() — see
     * board_wram_em_sel_init(). */

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

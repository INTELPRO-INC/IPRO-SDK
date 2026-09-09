/**
 * @file main.c
 * @brief IPRO6 BTDM Demo Application Entry Point
 *
 * This is the main entry point for the IPRO6 BTDM (Bluetooth Dual Mode) demo.
 * Currently implements BLE Controller only (HCI interface).
 *
 * Copyright (c) 2024 IPRO
 */

/* Standard C library headers */
#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* Project configuration */
#include <generated/autoconf.h>
#if defined(CONFIG_IPRO_LOG_ENABLE)
#include "ipro_log.h"
#endif

/* FreeRTOS */
#include <FreeRTOS.h>
#include <task.h>

/* HAL headers */
#include "hal_sys.h"
#include "hal_timer.h"
#include "hal_uart.h"
#include "drv_clock.h"

/* Platform common */
#include "platform_common.h"

/* Shell */
#ifdef CONFIG_SHELL
#include "shell.h"
#endif

/* BTDM SDK */
#ifdef CONFIG_BTDM_ENABLE
#include "btdm_sdk.h"
#include "ipro_ble_ctlr_hcitl.h"
#include "ipro_ble_ctlr_ipro6s_diag.h"
#endif

#if (defined(CONFIG_BTDM_ENABLE) || defined(CONFIG_IPRO6_CLASSIC_BRINGUP)) && \
    defined(CONFIG_PHY_CLI) && !defined(CONFIG_FPGA)
/* IPRO6S shares its calibrated RF front-end between WLAN and BZ. This app
 * does not start Wi-Fi, so no other owner performs cold RF initialization. */
extern void cmd_init_do(uint32_t xtal_freq_hz, uint8_t capcode);
extern void rf_bz_enter(void);
extern void bz_phy_reset(void);
#endif

#if defined(CONFIG_IPRO6_CLASSIC_BRINGUP)
#include "btble_lib_api.h"
#include <bluetooth.h>
#include <conn.h>
#endif

#if defined(CONFIG_BTDM_HOST_BRINGUP)
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#if defined(CONFIG_BT_SETTINGS)
#include <zephyr/settings/settings.h>
#endif
#if defined(CONFIG_BT_BREDR)
#if defined(CONFIG_BT_A2DP)
#include <zephyr/bluetooth/classic/a2dp.h>
#include <zephyr/bluetooth/classic/a2dp_codec_sbc.h>
#if defined(CONFIG_BT_A2DP_SBC_CODEC)
#include <zephyr/bluetooth/sbc.h>
#endif
#endif
#if defined(CONFIG_BT_AVRCP)
#include <zephyr/bluetooth/classic/avrcp.h>
#include "ipro6_avrcp_decode.h"
#include "ipro_log.h"
#endif
#if defined(CONFIG_BT_HFP_HF)
#include <zephyr/bluetooth/classic/hfp_hf.h>
#endif
#include <zephyr/bluetooth/classic/classic.h>
#include <zephyr/bluetooth/classic/l2cap_br.h>
#include <zephyr/bluetooth/classic/rfcomm.h>
#include <zephyr/bluetooth/classic/sdp.h>
#include <zephyr/bluetooth/l2cap.h>
#include <zephyr/net_buf.h>
#endif
#include "hci_driver_ipro_ble_ctlr.h"
#if defined(CONFIG_BT_GATT_SECURITY_LEVELS)
#include "ipro_bt_host/gatt_security_levels.h"
#endif
#if defined(CONFIG_BTDM_VOICE_OVER_HCI)
#include "ipro6_sco_i2s_tx.h"
#endif
#if defined(CONFIG_BT_A2DP_SBC_CODEC) && defined(CONFIG_BTDM_VOICE_OVER_HCI)
#define IPRO6_A2DP_PCM_OUTPUT 1
#endif
extern volatile uint32_t ipro_bt_host_hci_error_count;
extern volatile uint16_t ipro_bt_host_hci_error_opcode;
extern volatile uint8_t ipro_bt_host_hci_error_status;
extern volatile uint32_t ipro_bt_host_acl_rx_pb_count[4];
extern volatile uint32_t ipro_bt_host_acl_rx_pb_reject_count;
extern volatile uint32_t ipro_bt_host_h4_acl_rx_count;
extern volatile uint32_t ipro_bt_host_h4_acl_rx_bytes;
extern volatile uint32_t ipro_bt_host_h4_acl_rx_oversize_count;
extern volatile uint16_t ipro_bt_host_h4_acl_rx_last_len;
extern volatile uint16_t ipro_bt_host_h4_acl_rx_max_len;
#if defined(CONFIG_BTDM_BT_ENABLE)
extern volatile uint32_t dbg_ssp_stage;
extern volatile uint32_t dbg_ssp_lc_state;
extern volatile uint32_t dbg_ssp_sec_con;
extern volatile uint32_t dbg_ssp_dh_start_status;
extern volatile uint32_t dbg_ssp_dh_complete_count;
extern volatile uint32_t dbg_ssp_dh_complete_status;
extern volatile uint32_t dbg_ssp_priv_hash;
extern volatile uint32_t dbg_ssp_remote_pub_x_hash;
extern volatile uint32_t dbg_ssp_remote_pub_y_hash;
extern volatile uint32_t dbg_ssp_dh_hash;
extern volatile uint32_t dbg_ssp_remote_check_hash;
extern volatile uint32_t dbg_ssp_expected_check_hash;
extern volatile uint32_t dbg_ssp_check_equal;
#endif
#if defined(CONFIG_BTDM_BT_ENABLE)
extern volatile uint32_t dbg_bt_acl_rx_count;
extern volatile uint32_t dbg_bt_acl_rx_hash_first;
extern volatile uint32_t dbg_bt_acl_rx_hash_second;
extern volatile uint16_t dbg_bt_acl_rx_buf;
extern volatile uint16_t dbg_bt_acl_rx_len;
extern volatile uint32_t dbg_hci_acl_c2h_count;
extern volatile uint32_t dbg_hci_acl_c2h_hash;
extern volatile uint16_t dbg_hci_acl_c2h_buf;
extern volatile uint16_t dbg_hci_acl_c2h_len;
#endif
#endif

/* Heap region definitions */
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

/* FreeRTOS hook functions */
void __attribute__((weak)) vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    puts("Stack Overflow checked\r\n");
    if (pcTaskName) {
        printf("Stack name %s\r\n", pcTaskName);
    }
    while (1) {
        /* empty here */
    }
}

void __attribute__((weak)) vApplicationMallocFailedHook(void)
{
    printf("Memory Allocate Failed. Current left size is %d bytes\r\n",
           xPortGetFreeHeapSize());
    while (1) {
        /* empty here */
    }
}

/* FreeRTOS runtime stats - uses RISC-V mcycle CSR */
static volatile unsigned long ulRunTimeCounter = 0;

void vConfigureTimerForRunTimeStats(void)
{
    ulRunTimeCounter = 0;
}

unsigned long ulGetRunTimeCounterValue(void)
{
    unsigned long cycles;
    __asm__ volatile("csrr %0, mcycle" : "=r"(cycles));
    return cycles;
}

void __attribute__((weak)) vApplicationIdleHook(void)
{
    /* empty */
}

void __attribute__((weak)) vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                                          StackType_t **ppxIdleTaskStackBuffer,
                                                          configSTACK_DEPTH_TYPE *pulIdleTaskStackSize)
{
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void __attribute__((weak)) vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                                           StackType_t **ppxTimerTaskStackBuffer,
                                                           uint32_t *pulTimerTaskStackSize)
{
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void user_vAssertCalled(const char *const pcFileName, unsigned long ulLine) __attribute__((weak, alias("vAssertCalled")));
void __attribute__((weak)) vAssertCalled(const char *const pcFileName, unsigned long ulLine)
{
    printf("Assert raised by task %s, called by %p\r\n",
           pcTaskGetName(xTaskGetCurrentTaskHandle()),
           __builtin_return_address(0));
    taskDISABLE_INTERRUPTS();
    while (1);
}

#if defined(CONFIG_BTDM_H4_SELFTEST)
static void ipro6s_h4_selftest_run(void);
#endif

#if defined(CONFIG_BTDM_HOST_BRINGUP)
static void ipro6s_host_bringup_start(void);
#endif

/* Main loop task */
static void main_loop_proc(void *arg)
{
    (void)arg;

#if defined(CONFIG_BTDM_VOICE_OVER_HCI)
    {
        int i2s_rc = ipro6_sco_i2s_init();
        printf("[BTDM_SCO_I2S] init=%d\r\n", i2s_rc);
    }
#endif

#if defined(CONFIG_BTDM_H4_SELFTEST)
    ipro6s_h4_selftest_run();
#endif
#if defined(CONFIG_BTDM_HOST_BRINGUP)
    ipro6s_host_bringup_start();
#endif
#if defined(CONFIG_IPRO6_CLASSIC_BRINGUP)
    extern void ipro6s_classic_bringup_start(void);
    ipro6s_classic_bringup_start();
#endif

#ifdef CONFIG_SHELL
    extern int board_get_debug_uart_index(void);
    puts("Init Shell (RT-Thread FinSH based)\r\n");
    shell_init_with_task(board_get_debug_uart_index());

#ifdef CONFIG_BTDM_ENABLE
    /* Register BTDM CLI commands */
    extern void btdm_cli_init(void);
    btdm_cli_init();
#endif

#ifdef CONFIG_FPGA
    /* Register PHY/RF CLI commands */
    extern void phyrf_cli_init(void);
    phyrf_cli_init();
#endif
#endif

    /* Keep task alive */
    for (;;) {
        vTaskDelay(portMAX_DELAY);
    }
}

void setup_heap(void)
{
    printf("Heap Info start\r\n");

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

#define IPRO6S_BRINGUP_MAGIC   0x36534254u /* "TBS6" in little endian */
#define IPRO6S_BRINGUP_VERSION 8u

enum ipro6s_h4_selftest_state {
    IPRO6S_H4_SELFTEST_DISABLED = 0,
    IPRO6S_H4_SELFTEST_RUNNING = 1,
    IPRO6S_H4_SELFTEST_PASS = 2,
    IPRO6S_H4_SELFTEST_FAIL = 3,
};

enum ipro6s_host_bringup_state {
    IPRO6S_HOST_DISABLED = 0,
    IPRO6S_HOST_RUNNING = 1,
    IPRO6S_HOST_ADV = 2,
    IPRO6S_HOST_FAIL = 3,
    IPRO6S_HOST_CONNECTED = 4,
    IPRO6S_HOST_ADV_RESTART = 5,
};

/* Read this structure through S-CCI after GPIO10..13 are connected.  Keep the
 * legacy boot_stage symbol as well so older bench snippets continue to work.
 * TCM is uncached on IPRO6S, so each volatile store is immediately visible to
 * the CCI slave. */
struct ipro6s_bringup_status {
    uint32_t magic;
    uint32_t version;
    uint32_t stage;
    int32_t controller_init_rc;
    uint32_t h4_selftest_state;
    int32_t h4_selftest_rc;
    uint32_t h4_response_len;
    uint32_t h4_h2c_bytes;
    uint32_t h4_c2h_bytes;
    uint8_t h4_response[16];
    uint32_t host_state;
    int32_t host_enable_rc;
    int32_t host_adv_rc;
    uint32_t host_connect_count;
    uint32_t host_disconnect_count;
    uint32_t host_last_disconnect_reason;
    int32_t host_adv_restart_rc;
    uint32_t host_adv_restart_attempts;
    int32_t host_br_connectable_rc;
    int32_t host_br_discoverable_rc;
    uint32_t host_br_connect_count;
    uint32_t host_br_disconnect_count;
    int32_t host_br_echo_register_rc;
    uint32_t host_br_echo_request_count;
    uint32_t host_br_echo_response_count;
    uint32_t host_br_echo_error_count;
    uint32_t host_br_echo_last_length;
    int32_t host_br_echo_last_error;
    uint32_t host_br_echo_rx_pattern_error_count;
    uint32_t host_br_echo_copy_error_count;
    uint32_t host_br_echo_last_sequence;
    uint32_t host_br_echo_last_rx_hash;
    uint32_t host_br_echo_last_copy_hash;
    uint32_t host_br_echo_last_bad_index;
    uint32_t host_br_echo_last_bad_expected_actual;
    uint32_t host_security_changed_count;
    uint32_t host_security_level;
    uint32_t host_security_error;
    uint32_t host_pairing_complete_count;
    uint32_t host_pairing_failed_count;
    uint32_t host_pairing_bonded;
    uint32_t host_pairing_last_error;
    int32_t host_settings_load_rc;
    uint32_t host_le_bond_count;
    uint32_t host_br_bond_count;
};

volatile struct ipro6s_bringup_status ipro6s_bringup_status
    __attribute__((used)) = {
        .magic = IPRO6S_BRINGUP_MAGIC,
        .version = IPRO6S_BRINGUP_VERSION,
        .stage = 0,
        .controller_init_rc = INT32_MIN,
        .h4_selftest_state = IPRO6S_H4_SELFTEST_DISABLED,
        .h4_selftest_rc = INT32_MIN,
        .host_state = IPRO6S_HOST_DISABLED,
        .host_enable_rc = INT32_MIN,
        .host_adv_rc = INT32_MIN,
        .host_br_connectable_rc = INT32_MIN,
        .host_br_discoverable_rc = INT32_MIN,
        .host_br_echo_register_rc = INT32_MIN,
        .host_br_echo_last_error = INT32_MIN,
        .host_settings_load_rc = INT32_MIN,
    };
volatile uint32_t boot_stage __attribute__((used)) = 0;

#if defined(CONFIG_BTDM_HOST_BRINGUP) && defined(CONFIG_BT_BREDR)
static volatile uint32_t s_ipro6s_rfcomm_connect_count;
static volatile uint32_t s_ipro6s_rfcomm_disconnect_count;
static volatile uint32_t s_ipro6s_rfcomm_rx_count;
static volatile uint32_t s_ipro6s_rfcomm_tx_count;
static volatile uint32_t s_ipro6s_rfcomm_error_count;
static volatile uint32_t s_ipro6s_rfcomm_last_rx_hash;
static volatile uint32_t s_ipro6s_rfcomm_last_tx_hash;
#if defined(CONFIG_BT_A2DP_SINK)
static volatile uint32_t s_ipro6s_a2dp_connect_count;
static volatile uint32_t s_ipro6s_a2dp_disconnect_count;
static volatile uint32_t s_ipro6s_a2dp_config_count;
static volatile uint32_t s_ipro6s_a2dp_establish_count;
static volatile uint32_t s_ipro6s_a2dp_start_count;
static volatile uint32_t s_ipro6s_a2dp_suspend_count;
static volatile uint32_t s_ipro6s_a2dp_release_count;
static volatile uint32_t s_ipro6s_a2dp_media_packet_count;
static volatile uint32_t s_ipro6s_a2dp_media_byte_count;
static volatile uint32_t s_ipro6s_a2dp_media_hash = 2166136261u;
static volatile uint32_t s_ipro6s_a2dp_media_seq_error_count;
static volatile bool s_ipro6s_a2dp_media_sequence_valid;
static volatile uint32_t s_ipro6s_a2dp_stream_packet_count;
static volatile uint32_t s_ipro6s_a2dp_stream_byte_count;
static volatile uint32_t s_ipro6s_a2dp_stream_hash = 2166136261u;
static volatile uint32_t s_ipro6s_a2dp_stream_seq_error_count;
static volatile uint32_t s_ipro6s_a2dp_content_valid_count;
static volatile uint32_t s_ipro6s_a2dp_content_error_count;
static volatile uint32_t s_ipro6s_a2dp_content_error_bytes;
static volatile uint32_t s_ipro6s_a2dp_content_unchecked_count;
static volatile uint16_t s_ipro6s_a2dp_first_bad_seq;
static volatile uint16_t s_ipro6s_a2dp_first_bad_index;
static volatile uint16_t s_ipro6s_a2dp_first_bad_expected_actual;
static volatile uint16_t s_ipro6s_a2dp_last_seq;
static volatile uint16_t s_ipro6s_a2dp_last_len;
static volatile uint32_t s_ipro6s_a2dp_last_timestamp;
static volatile int32_t s_ipro6s_a2dp_register_cb_rc = INT32_MIN;
static volatile int32_t s_ipro6s_a2dp_register_ep_rc = INT32_MIN;
static volatile int32_t s_ipro6s_a2dp_register_sdp_rc = INT32_MIN;
#if defined(IPRO6_A2DP_PCM_OUTPUT)
static struct sbc_decoder s_ipro6s_a2dp_sbc_decoder;
static uint8_t s_ipro6s_a2dp_pcm[512] __attribute__((aligned(4)));
static volatile uint32_t s_ipro6s_a2dp_sample_rate;
static volatile uint32_t s_ipro6s_a2dp_decode_frames;
static volatile uint32_t s_ipro6s_a2dp_decode_bytes;
static volatile uint32_t s_ipro6s_a2dp_decode_errors;
static volatile uint32_t s_ipro6s_a2dp_decode_drops;
static volatile uint32_t s_ipro6s_a2dp_decode_hash = 2166136261u;
static volatile int32_t s_ipro6s_a2dp_decoder_rc = INT32_MIN;
static volatile int32_t s_ipro6s_a2dp_i2s_config_rc = INT32_MIN;
static volatile uint8_t s_ipro6s_a2dp_channels;
static volatile uint8_t s_ipro6s_a2dp_sbc_ready;
#endif
#endif
#if defined(CONFIG_BT_AVRCP)
static volatile int32_t s_ipro6s_avrcp_ct_register_rc = INT32_MIN;
static volatile int32_t s_ipro6s_avrcp_tg_register_rc = INT32_MIN;
static volatile uint32_t s_ipro6s_avrcp_ct_connect_count;
static volatile uint32_t s_ipro6s_avrcp_ct_disconnect_count;
static volatile uint32_t s_ipro6s_avrcp_tg_connect_count;
static volatile uint32_t s_ipro6s_avrcp_tg_disconnect_count;
static volatile uint32_t s_ipro6s_avrcp_unit_info_count;
static volatile uint32_t s_ipro6s_avrcp_subunit_info_count;
static volatile uint32_t s_ipro6s_avrcp_passthrough_count;
static volatile uint32_t s_ipro6s_avrcp_response_error_count;
#endif
#if defined(CONFIG_BT_HFP_HF)
static volatile int32_t s_ipro6s_hfp_hf_register_rc = INT32_MIN;
static volatile uint32_t s_ipro6s_hfp_hf_connect_count;
static volatile uint32_t s_ipro6s_hfp_hf_disconnect_count;
static volatile uint32_t s_ipro6s_hfp_sco_connect_count;
static volatile uint32_t s_ipro6s_hfp_sco_disconnect_count;
static volatile uint32_t s_ipro6s_hfp_sco_last_reason;
static volatile uint32_t s_ipro6s_hfp_service_value;
#if defined(CONFIG_BTDM_VOICE_OVER_HCI)
extern volatile uint32_t dbg_rwbt_audio0_isr_count;
extern volatile uint32_t dbg_rwbt_audio0_last_irq_stat;
extern volatile uint32_t dbg_ld_sco_audio_isr_count;
extern volatile uint32_t dbg_ld_sco_audio_vohci_count;
extern volatile uint32_t dbg_ld_sco_audio_rx_old_count;
extern volatile uint32_t dbg_ld_sco_audio_rx_zero_count;
extern volatile uint32_t dbg_ld_sco_audio_last_rx_ptr;
extern volatile uint32_t dbg_ld_sco_audio_last_data_path;
extern volatile uint32_t dbg_ld_sco_audio_rx_status_count[4];
extern volatile uint32_t dbg_ld_sco_audio_alt_status_count[4];
extern volatile uint32_t dbg_ld_sco_audio_last_mute;
extern volatile uint32_t dbg_ld_sco_audio_last_tog_bw;
extern volatile uint32_t dbg_lc_sync_rx_ind_count;
extern volatile uint32_t dbg_lc_sync_rx_hci_count;
extern volatile uint32_t dbg_lc_sync_rx_drop_count;
static volatile int32_t s_ipro6s_sco_data_register_rc = INT32_MIN;
static volatile uint32_t s_ipro6s_sco_data_rx_count;
static volatile uint32_t s_ipro6s_sco_data_rx_bytes;
static volatile uint32_t s_ipro6s_sco_data_valid_count;
static volatile uint32_t s_ipro6s_sco_data_nonzero_count;
static volatile uint32_t s_ipro6s_sco_data_checked_count;
static volatile uint32_t s_ipro6s_sco_data_content_error_count;
static volatile uint32_t s_ipro6s_sco_data_status_error_count;
static volatile uint32_t s_ipro6s_sco_data_echo_error_count;
static volatile uint32_t s_ipro6s_sco_data_rx_hash = 2166136261u;
static volatile uint16_t s_ipro6s_sco_data_last_handle;
static volatile uint8_t s_ipro6s_sco_data_last_len;
static volatile uint8_t s_ipro6s_sco_data_last_status;
#endif
#endif
#endif /* CONFIG_BTDM_HOST_BRINGUP && CONFIG_BT_BREDR */

#if defined(CONFIG_BTDM_HOST_LE_ADV)
#define IPRO6S_GATT_LED_SERVICE_UUID BT_UUID_DECLARE_16(0xFF10)
#define IPRO6S_GATT_LED_STATE_UUID   BT_UUID_DECLARE_16(0xFF11)
#define IPRO6S_GATT_LED_BRIGHT_UUID  BT_UUID_DECLARE_16(0xFF12)
#define IPRO6S_GATT_UART_SERVICE_UUID BT_UUID_DECLARE_16(0xFF40)
#define IPRO6S_GATT_UART_RX_UUID      BT_UUID_DECLARE_16(0xFF42)

static uint8_t s_ipro6s_gatt_led_state;
static uint8_t s_ipro6s_gatt_led_brightness;
static volatile uint32_t s_ipro6s_gatt_read_count;
static volatile uint32_t s_ipro6s_gatt_write_count;
static volatile uint32_t s_ipro6s_gatt_uart_rx_count;
static volatile uint32_t s_ipro6s_gatt_uart_rx_bytes;
static volatile uint32_t s_ipro6s_gatt_uart_rx_hash = 2166136261u;
static volatile uint32_t s_ipro6s_gatt_uart_content_error_count;

static uint32_t ipro6s_gatt_hash(const uint8_t *data, uint16_t len)
{
    uint32_t hash = 2166136261u;

    for (uint16_t i = 0U; i < len; ++i) {
        hash = (hash ^ data[i]) * 16777619u;
    }
    return hash;
}

static ssize_t ipro6s_gatt_u8_read(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr,
                                   void *buf, uint16_t len, uint16_t offset)
{
    const uint8_t *value = attr->user_data;

    s_ipro6s_gatt_read_count++;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
                             sizeof(*value));
}

static ssize_t ipro6s_gatt_u8_write(struct bt_conn *conn,
                                    const struct bt_gatt_attr *attr,
                                    const void *buf, uint16_t len,
                                    uint16_t offset, uint8_t flags)
{
    uint8_t *value = attr->user_data;

    (void)conn;
    (void)flags;
    if (offset != 0U) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
    if (len != sizeof(*value)) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }
    *value = ((const uint8_t *)buf)[0];
    s_ipro6s_gatt_write_count++;
    return len;
}

static ssize_t ipro6s_gatt_uart_rx_write(struct bt_conn *conn,
                                         const struct bt_gatt_attr *attr,
                                         const void *buf, uint16_t len,
                                         uint16_t offset, uint8_t flags)
{
    static const uint8_t expected[] = "ipro-central-gatt-ping";
    const uint8_t *data = buf;

    (void)conn;
    (void)attr;
    (void)flags;
    if (offset != 0U) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
    s_ipro6s_gatt_uart_rx_count++;
    s_ipro6s_gatt_uart_rx_bytes += len;
    s_ipro6s_gatt_uart_rx_hash = ipro6s_gatt_hash(data, len);
    if (len != sizeof(expected) - 1U ||
        memcmp(data, expected, sizeof(expected) - 1U) != 0) {
        s_ipro6s_gatt_uart_content_error_count++;
    }
    return len;
}

BT_GATT_SERVICE_DEFINE(ipro6s_led_svc,
    BT_GATT_PRIMARY_SERVICE(IPRO6S_GATT_LED_SERVICE_UUID),
    BT_GATT_CHARACTERISTIC(IPRO6S_GATT_LED_STATE_UUID,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ipro6s_gatt_u8_read, ipro6s_gatt_u8_write,
                           &s_ipro6s_gatt_led_state),
    BT_GATT_CHARACTERISTIC(IPRO6S_GATT_LED_BRIGHT_UUID,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ipro6s_gatt_u8_read, ipro6s_gatt_u8_write,
                           &s_ipro6s_gatt_led_brightness));

BT_GATT_SERVICE_DEFINE(ipro6s_uart_svc,
    BT_GATT_PRIMARY_SERVICE(IPRO6S_GATT_UART_SERVICE_UUID),
    BT_GATT_CHARACTERISTIC(IPRO6S_GATT_UART_RX_UUID,
                           BT_GATT_CHRC_WRITE |
                           BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE,
                           NULL, ipro6s_gatt_uart_rx_write, NULL));
#endif

void ipro6s_bringup_status_print(void)
{
#if defined(CONFIG_BTDM_ENABLE) && defined(CONFIG_BTDM_HOST_BRINGUP)
    printf("[IPRO6S_BRINGUP] stage=%lu ctlr_rc=%ld host=%lu "
           "enable_rc=%ld adv_rc=%ld h2c=%lu c2h=%lu "
           "conn=%lu disc=%lu reason=0x%02lx restart_rc=%ld retry=%lu "
           "br_rc=%ld/%ld br_conn/disc=%lu/%lu "
           "echo_reg=%ld req/rsp/err=%lu/%lu/%lu len=%lu last=%ld "
           "hci_err=%lu/0x%04x/0x%02x\r\n",
           (unsigned long)ipro6s_bringup_status.stage,
           (long)ipro6s_bringup_status.controller_init_rc,
           (unsigned long)ipro6s_bringup_status.host_state,
           (long)ipro6s_bringup_status.host_enable_rc,
           (long)ipro6s_bringup_status.host_adv_rc,
           (unsigned long)ipro_ble_ctlr_hcitl_h2c_bytes,
           (unsigned long)ipro_ble_ctlr_hcitl_c2h_bytes,
           (unsigned long)ipro6s_bringup_status.host_connect_count,
           (unsigned long)ipro6s_bringup_status.host_disconnect_count,
           (unsigned long)ipro6s_bringup_status.host_last_disconnect_reason,
           (long)ipro6s_bringup_status.host_adv_restart_rc,
           (unsigned long)ipro6s_bringup_status.host_adv_restart_attempts,
           (long)ipro6s_bringup_status.host_br_connectable_rc,
           (long)ipro6s_bringup_status.host_br_discoverable_rc,
           (unsigned long)ipro6s_bringup_status.host_br_connect_count,
           (unsigned long)ipro6s_bringup_status.host_br_disconnect_count,
           (long)ipro6s_bringup_status.host_br_echo_register_rc,
           (unsigned long)ipro6s_bringup_status.host_br_echo_request_count,
           (unsigned long)ipro6s_bringup_status.host_br_echo_response_count,
           (unsigned long)ipro6s_bringup_status.host_br_echo_error_count,
           (unsigned long)ipro6s_bringup_status.host_br_echo_last_length,
           (long)ipro6s_bringup_status.host_br_echo_last_error,
           (unsigned long)ipro_bt_host_hci_error_count,
           (unsigned int)ipro_bt_host_hci_error_opcode,
           (unsigned int)ipro_bt_host_hci_error_status);

    printf("[BTDM_ECHO_DIAG] rx_bad/copy_bad=%lu/%lu seq=%lu "
           "hash=%08lx/%08lx bad_idx=%lu exp/actual=%02lx/%02lx\r\n",
           (unsigned long)ipro6s_bringup_status.host_br_echo_rx_pattern_error_count,
           (unsigned long)ipro6s_bringup_status.host_br_echo_copy_error_count,
           (unsigned long)ipro6s_bringup_status.host_br_echo_last_sequence,
           (unsigned long)ipro6s_bringup_status.host_br_echo_last_rx_hash,
           (unsigned long)ipro6s_bringup_status.host_br_echo_last_copy_hash,
           (unsigned long)ipro6s_bringup_status.host_br_echo_last_bad_index,
           (unsigned long)((ipro6s_bringup_status.host_br_echo_last_bad_expected_actual >> 8) & 0xffu),
           (unsigned long)(ipro6s_bringup_status.host_br_echo_last_bad_expected_actual & 0xffu));

    printf("[BTDM_HOST_ACL] pb0/pb1/pb2/pb3=%lu/%lu/%lu/%lu reject=%lu\r\n",
           (unsigned long)ipro_bt_host_acl_rx_pb_count[0],
           (unsigned long)ipro_bt_host_acl_rx_pb_count[1],
           (unsigned long)ipro_bt_host_acl_rx_pb_count[2],
           (unsigned long)ipro_bt_host_acl_rx_pb_count[3],
           (unsigned long)ipro_bt_host_acl_rx_pb_reject_count);
    printf("[BTDM_HOST_H4] acl_pkt/bytes=%lu/%lu oversize=%lu last/max=%u/%u\r\n",
           (unsigned long)ipro_bt_host_h4_acl_rx_count,
           (unsigned long)ipro_bt_host_h4_acl_rx_bytes,
           (unsigned long)ipro_bt_host_h4_acl_rx_oversize_count,
           ipro_bt_host_h4_acl_rx_last_len,
           ipro_bt_host_h4_acl_rx_max_len);

#if defined(CONFIG_BT_BREDR)
    printf("[BTDM_RFCOMM] conn/disc=%lu/%lu rx/tx/err=%lu/%lu/%lu "
           "hash=%08lx/%08lx\r\n",
           (unsigned long)s_ipro6s_rfcomm_connect_count,
           (unsigned long)s_ipro6s_rfcomm_disconnect_count,
           (unsigned long)s_ipro6s_rfcomm_rx_count,
           (unsigned long)s_ipro6s_rfcomm_tx_count,
           (unsigned long)s_ipro6s_rfcomm_error_count,
           (unsigned long)s_ipro6s_rfcomm_last_rx_hash,
           (unsigned long)s_ipro6s_rfcomm_last_tx_hash);
#if defined(CONFIG_BT_A2DP_SINK)
    printf("[BTDM_A2DP] reg=%ld/%ld/%ld conn/disc=%lu/%lu "
           "cfg/est/start/susp/rel=%lu/%lu/%lu/%lu/%lu "
           "media_pkt/bytes/seqerr=%lu/%lu/%lu last=%u/%u/%lu hash=%08lx\r\n",
           (long)s_ipro6s_a2dp_register_cb_rc,
           (long)s_ipro6s_a2dp_register_ep_rc,
           (long)s_ipro6s_a2dp_register_sdp_rc,
           (unsigned long)s_ipro6s_a2dp_connect_count,
           (unsigned long)s_ipro6s_a2dp_disconnect_count,
           (unsigned long)s_ipro6s_a2dp_config_count,
           (unsigned long)s_ipro6s_a2dp_establish_count,
           (unsigned long)s_ipro6s_a2dp_start_count,
           (unsigned long)s_ipro6s_a2dp_suspend_count,
           (unsigned long)s_ipro6s_a2dp_release_count,
           (unsigned long)s_ipro6s_a2dp_media_packet_count,
           (unsigned long)s_ipro6s_a2dp_media_byte_count,
           (unsigned long)s_ipro6s_a2dp_media_seq_error_count,
           s_ipro6s_a2dp_last_seq,
           s_ipro6s_a2dp_last_len,
           (unsigned long)s_ipro6s_a2dp_last_timestamp,
           (unsigned long)s_ipro6s_a2dp_media_hash);
    printf("[BTDM_A2DP_STREAM] pkt/bytes/seqerr=%lu/%lu/%lu hash=%08lx "
           "content_valid/err_pkt/err_bytes/unchecked=%lu/%lu/%lu/%lu "
           "first=%u/%u/%02x/%02x\r\n",
           (unsigned long)s_ipro6s_a2dp_stream_packet_count,
           (unsigned long)s_ipro6s_a2dp_stream_byte_count,
           (unsigned long)s_ipro6s_a2dp_stream_seq_error_count,
           (unsigned long)s_ipro6s_a2dp_stream_hash,
           (unsigned long)s_ipro6s_a2dp_content_valid_count,
           (unsigned long)s_ipro6s_a2dp_content_error_count,
           (unsigned long)s_ipro6s_a2dp_content_error_bytes,
           (unsigned long)s_ipro6s_a2dp_content_unchecked_count,
           s_ipro6s_a2dp_first_bad_seq,
           s_ipro6s_a2dp_first_bad_index,
           (unsigned int)(s_ipro6s_a2dp_first_bad_expected_actual >> 8),
           (unsigned int)(s_ipro6s_a2dp_first_bad_expected_actual & 0xffu));
#if defined(IPRO6_A2DP_PCM_OUTPUT)
    printf("[BTDM_A2DP_PCM] ready=%u rate/ch=%lu/%u setup/i2s=%ld/%ld "
           "frame/bytes/error/drop=%lu/%lu/%lu/%lu hash=%08lx\r\n",
           s_ipro6s_a2dp_sbc_ready,
           (unsigned long)s_ipro6s_a2dp_sample_rate,
           s_ipro6s_a2dp_channels,
           (long)s_ipro6s_a2dp_decoder_rc,
           (long)s_ipro6s_a2dp_i2s_config_rc,
           (unsigned long)s_ipro6s_a2dp_decode_frames,
           (unsigned long)s_ipro6s_a2dp_decode_bytes,
           (unsigned long)s_ipro6s_a2dp_decode_errors,
           (unsigned long)s_ipro6s_a2dp_decode_drops,
           (unsigned long)s_ipro6s_a2dp_decode_hash);
#endif
#endif
#if defined(CONFIG_BT_AVRCP)
    printf("[BTDM_AVRCP] reg_ct/tg=%ld/%ld ct_conn/disc=%lu/%lu "
           "tg_conn/disc=%lu/%lu unit/subunit/pass/err=%lu/%lu/%lu/%lu\r\n",
           (long)s_ipro6s_avrcp_ct_register_rc,
           (long)s_ipro6s_avrcp_tg_register_rc,
           (unsigned long)s_ipro6s_avrcp_ct_connect_count,
           (unsigned long)s_ipro6s_avrcp_ct_disconnect_count,
           (unsigned long)s_ipro6s_avrcp_tg_connect_count,
           (unsigned long)s_ipro6s_avrcp_tg_disconnect_count,
           (unsigned long)s_ipro6s_avrcp_unit_info_count,
           (unsigned long)s_ipro6s_avrcp_subunit_info_count,
           (unsigned long)s_ipro6s_avrcp_passthrough_count,
           (unsigned long)s_ipro6s_avrcp_response_error_count);
#endif
#if defined(CONFIG_BT_HFP_HF)
    printf("[BTDM_HFP_HF] reg=%ld slc_conn/disc=%lu/%lu "
           "sco_conn/disc/reason=%lu/%lu/0x%02lx service=%lu\r\n",
           (long)s_ipro6s_hfp_hf_register_rc,
           (unsigned long)s_ipro6s_hfp_hf_connect_count,
           (unsigned long)s_ipro6s_hfp_hf_disconnect_count,
           (unsigned long)s_ipro6s_hfp_sco_connect_count,
           (unsigned long)s_ipro6s_hfp_sco_disconnect_count,
           (unsigned long)s_ipro6s_hfp_sco_last_reason,
           (unsigned long)s_ipro6s_hfp_service_value);
#if defined(CONFIG_BTDM_VOICE_OVER_HCI)
    printf("[BTDM_SCO_DATA] reg=%ld rx/bytes/valid/checked=%lu/%lu/%lu/%lu "
           "nonzero=%lu "
           "content/status/echo_err=%lu/%lu/%lu last=%03x/%u/%u "
           "hash=%08lx h4_rx/tx=%lu/%lu h4_bytes=%lu/%lu no_cb=%lu\r\n",
           (long)s_ipro6s_sco_data_register_rc,
           (unsigned long)s_ipro6s_sco_data_rx_count,
           (unsigned long)s_ipro6s_sco_data_rx_bytes,
           (unsigned long)s_ipro6s_sco_data_valid_count,
           (unsigned long)s_ipro6s_sco_data_checked_count,
           (unsigned long)s_ipro6s_sco_data_nonzero_count,
           (unsigned long)s_ipro6s_sco_data_content_error_count,
           (unsigned long)s_ipro6s_sco_data_status_error_count,
           (unsigned long)s_ipro6s_sco_data_echo_error_count,
           s_ipro6s_sco_data_last_handle,
           s_ipro6s_sco_data_last_len,
           s_ipro6s_sco_data_last_status,
           (unsigned long)s_ipro6s_sco_data_rx_hash,
           (unsigned long)ipro_bt_host_h4_sco_rx_count,
           (unsigned long)ipro_bt_host_h4_sco_tx_count,
           (unsigned long)ipro_bt_host_h4_sco_rx_bytes,
           (unsigned long)ipro_bt_host_h4_sco_tx_bytes,
           (unsigned long)ipro_bt_host_h4_sco_rx_no_callback_count);
    printf("[BTDM_SCO_PIPE] irq/isr/vohci=%lu/%lu/%lu "
           "rx_old/zero=%lu/%lu lc_ind/hci/drop=%lu/%lu/%lu "
           "path=%lu ptr=%04lx irqstat=%08lx\r\n",
           (unsigned long)dbg_rwbt_audio0_isr_count,
           (unsigned long)dbg_ld_sco_audio_isr_count,
           (unsigned long)dbg_ld_sco_audio_vohci_count,
           (unsigned long)dbg_ld_sco_audio_rx_old_count,
           (unsigned long)dbg_ld_sco_audio_rx_zero_count,
           (unsigned long)dbg_lc_sync_rx_ind_count,
           (unsigned long)dbg_lc_sync_rx_hci_count,
           (unsigned long)dbg_lc_sync_rx_drop_count,
           (unsigned long)dbg_ld_sco_audio_last_data_path,
           (unsigned long)dbg_ld_sco_audio_last_rx_ptr,
           (unsigned long)dbg_rwbt_audio0_last_irq_stat);
    printf("[BTDM_SCO_STATUS] selected=%lu/%lu/%lu/%lu "
           "alternate=%lu/%lu/%lu/%lu mute=%08lx tog/bw=%08lx\r\n",
           (unsigned long)dbg_ld_sco_audio_rx_status_count[0],
           (unsigned long)dbg_ld_sco_audio_rx_status_count[1],
           (unsigned long)dbg_ld_sco_audio_rx_status_count[2],
           (unsigned long)dbg_ld_sco_audio_rx_status_count[3],
           (unsigned long)dbg_ld_sco_audio_alt_status_count[0],
           (unsigned long)dbg_ld_sco_audio_alt_status_count[1],
           (unsigned long)dbg_ld_sco_audio_alt_status_count[2],
           (unsigned long)dbg_ld_sco_audio_alt_status_count[3],
           (unsigned long)dbg_ld_sco_audio_last_mute,
           (unsigned long)dbg_ld_sco_audio_last_tog_bw);
#endif
#endif
#endif

    printf("[BTDM_SECURITY] changed=%lu level=%lu err=%lu "
           "pair_ok/fail=%lu/%lu bonded=%lu pair_err=%lu "
           "settings=%ld le/br_bonds=%lu/%lu\r\n",
           (unsigned long)ipro6s_bringup_status.host_security_changed_count,
           (unsigned long)ipro6s_bringup_status.host_security_level,
           (unsigned long)ipro6s_bringup_status.host_security_error,
           (unsigned long)ipro6s_bringup_status.host_pairing_complete_count,
           (unsigned long)ipro6s_bringup_status.host_pairing_failed_count,
           (unsigned long)ipro6s_bringup_status.host_pairing_bonded,
           (unsigned long)ipro6s_bringup_status.host_pairing_last_error,
           (long)ipro6s_bringup_status.host_settings_load_rc,
           (unsigned long)ipro6s_bringup_status.host_le_bond_count,
           (unsigned long)ipro6s_bringup_status.host_br_bond_count);

#if defined(CONFIG_BTDM_HOST_LE_ADV)
    printf("[BTDM_GATT] state/brightness=%u/%u read/write=%lu/%lu "
           "uart_rx/bytes/error=%lu/%lu/%lu hash=%08lx\r\n",
           s_ipro6s_gatt_led_state,
           s_ipro6s_gatt_led_brightness,
           (unsigned long)s_ipro6s_gatt_read_count,
           (unsigned long)s_ipro6s_gatt_write_count,
           (unsigned long)s_ipro6s_gatt_uart_rx_count,
           (unsigned long)s_ipro6s_gatt_uart_rx_bytes,
           (unsigned long)s_ipro6s_gatt_uart_content_error_count,
           (unsigned long)s_ipro6s_gatt_uart_rx_hash);
#endif

#if defined(CONFIG_BTDM_BT_ENABLE)
    printf("[BTDM_SSP_DIAG] stage/state=%lu/%lu sc=%lu start=%lu "
           "done/status=%lu/%lu priv=%08lx pub=%08lx/%08lx dh=%08lx "
           "check=%08lx/%08lx equal=%lu\r\n",
           (unsigned long)dbg_ssp_stage,
           (unsigned long)dbg_ssp_lc_state,
           (unsigned long)dbg_ssp_sec_con,
           (unsigned long)dbg_ssp_dh_start_status,
           (unsigned long)dbg_ssp_dh_complete_count,
           (unsigned long)dbg_ssp_dh_complete_status,
           (unsigned long)dbg_ssp_priv_hash,
           (unsigned long)dbg_ssp_remote_pub_x_hash,
           (unsigned long)dbg_ssp_remote_pub_y_hash,
           (unsigned long)dbg_ssp_dh_hash,
           (unsigned long)dbg_ssp_remote_check_hash,
           (unsigned long)dbg_ssp_expected_check_hash,
           (unsigned long)dbg_ssp_check_equal);
#endif

#if defined(CONFIG_BTDM_BLE_ENABLE)
    if (ipro6s_con_diag.magic == IPRO6S_CON_DIAG_MAGIC) {
        printf("[IPRO6S_CON] start=%lu eof=%lu cancel=%lu skip=%lu "
               "rx=%lu/%lu/%lu/%lu nocrc=%lu max=%lu "
               "freeze=%lu reason=0x%02lx clock/next/sync/crc=%lu/%lu/%lu/%lu "
               "int/to=%lu/%lu evt/inc=%u/%u info/state=%04x/%u\r\n",
               (unsigned long)ipro6s_con_diag.event_start_count,
               (unsigned long)ipro6s_con_diag.event_eof_count,
               (unsigned long)ipro6s_con_diag.event_cancel_count,
               (unsigned long)ipro6s_con_diag.event_skip_count,
               (unsigned long)ipro6s_con_diag.rx_desc_count,
               (unsigned long)ipro6s_con_diag.rx_crc_ok_count,
               (unsigned long)ipro6s_con_diag.rx_crc_err_count,
               (unsigned long)ipro6s_con_diag.rx_sync_err_count,
               (unsigned long)ipro6s_con_diag.consecutive_no_crc,
               (unsigned long)ipro6s_con_diag.max_consecutive_no_crc,
               (unsigned long)ipro6s_con_diag.freeze_count,
               (unsigned long)ipro6s_con_diag.freeze_reason,
               (unsigned long)ipro6s_con_diag.freeze_clock,
               (unsigned long)ipro6s_con_diag.freeze_next_ts,
               (unsigned long)ipro6s_con_diag.freeze_last_sync_ts,
               (unsigned long)ipro6s_con_diag.freeze_last_crc_ok_ts,
               (unsigned long)ipro6s_con_diag.freeze_interval_hs,
               (unsigned long)ipro6s_con_diag.freeze_timeout_hs,
               ipro6s_con_diag.freeze_event_counter,
               ipro6s_con_diag.freeze_event_increment,
               ipro6s_con_diag.freeze_link_info,
               ipro6s_con_diag.freeze_state);
        printf("[IPRO6S_CON_CHMAP] map=%08lx:%02x count=%u index_oor=%u\r\n",
               (unsigned long)ipro6s_con_diag.channel_map_lo,
               ipro6s_con_diag.channel_map_hi,
               ipro6s_con_diag.channel_count,
               ipro6s_con_diag.channel_index_oor_count);
        for (uint32_t channel = 0;
             channel < IPRO6S_CON_DIAG_CHANNEL_COUNT; channel++) {
            if (ipro6s_con_diag.channel_total[channel] != 0u) {
                printf("[IPRO6S_CON_CH%02lu] total/ok/crc/sync=%u/%u/%u/%u\r\n",
                       (unsigned long)channel,
                       ipro6s_con_diag.channel_total[channel],
                       ipro6s_con_diag.channel_ok[channel],
                       ipro6s_con_diag.channel_crc_err[channel],
                       ipro6s_con_diag.channel_sync_err[channel]);
            }
        }
        for (uint32_t bank = 0; bank < 2; bank++) {
            const volatile struct ipro6s_con_diag_sample *samples =
                bank == 0 ? ipro6s_con_diag.live : ipro6s_con_diag.frozen;
            const char label = bank == 0 ? 'L' : 'F';

            for (uint32_t i = 0; i < IPRO6S_CON_DIAG_SAMPLE_COUNT; i++) {
                const volatile struct ipro6s_con_diag_sample *s = &samples[i];
                uint32_t agc = s->agc_status;
                printf("[IPRO6S_CON_%c%lu] evt=%u hw=%u flags=%04x "
                       "start/sched/eof/rx/crc=%lu/%lu/%lu/%lu/%lu "
                       "sdm/agc=%08lx/%08lx gain=%lu/%lu/%lu/%lu "
                       "stat/phce/ch=%04x/%04x/%04x idx/phy/hop=%u/%u/%u\r\n",
                       label, (unsigned long)i, s->event_counter,
                       s->hw_event_counter, s->flags,
                       (unsigned long)s->start_clock,
                       (unsigned long)s->scheduled_hs,
                       (unsigned long)s->eof_timestamp,
                       (unsigned long)s->rx_sync_hs,
                       (unsigned long)s->last_crc_ok_ts,
                       (unsigned long)s->rf_sdm_hw, (unsigned long)agc,
                       (unsigned long)((agc >> 21) & 0x0fu),
                       (unsigned long)((agc >> 16) & 0x1fu),
                       (unsigned long)((agc >> 8) & 0xffu),
                       (unsigned long)(agc & 0x3fu),
                       s->rxstat, s->rxphce, s->rxchass,
                       s->used_channel_index, s->physical_channel,
                       s->hop_channel);
            }
        }
    }
#endif
#else
    printf("[IPRO6S_BRINGUP] stage=%lu ctlr_rc=%ld h4=%lu/%ld "
           "rsp_len=%lu rsp=%02x%02x%02x%02x%02x%02x%02x "
           "h2c/c2h=%lu/%lu host=%lu enable_rc=%ld\r\n",
           (unsigned long)ipro6s_bringup_status.stage,
           (long)ipro6s_bringup_status.controller_init_rc,
           (unsigned long)ipro6s_bringup_status.h4_selftest_state,
           (long)ipro6s_bringup_status.h4_selftest_rc,
           (unsigned long)ipro6s_bringup_status.h4_response_len,
           ipro6s_bringup_status.h4_response[0],
           ipro6s_bringup_status.h4_response[1],
           ipro6s_bringup_status.h4_response[2],
           ipro6s_bringup_status.h4_response[3],
           ipro6s_bringup_status.h4_response[4],
           ipro6s_bringup_status.h4_response[5],
           ipro6s_bringup_status.h4_response[6],
           (unsigned long)ipro6s_bringup_status.h4_h2c_bytes,
           (unsigned long)ipro6s_bringup_status.h4_c2h_bytes,
           (unsigned long)ipro6s_bringup_status.host_state,
           (long)ipro6s_bringup_status.host_enable_rc);
#endif
}

static void bringup_stage_set(uint32_t stage)
{
    boot_stage = stage;
    ipro6s_bringup_status.stage = stage;
}

#if defined(CONFIG_BTDM_H4_SELFTEST)
static void ipro6s_h4_selftest_run(void)
{
    static const uint8_t hci_reset[] = { 0x01, 0x03, 0x0c, 0x00 };
    TickType_t deadline;
    uint32_t written;

    ipro6s_bringup_status.h4_selftest_state = IPRO6S_H4_SELFTEST_RUNNING;
    ipro6s_bringup_status.h4_selftest_rc = -1;
    ipro6s_bringup_status.h4_response_len = 0;

    /* Let the higher-priority controller task enter its receive loop first. */
    vTaskDelay(pdMS_TO_TICKS(20));
    if (ipro_ble_ctlr_hcitl_host_write_space() < sizeof(hci_reset)) {
        ipro6s_bringup_status.h4_selftest_rc = -2;
        goto done;
    }

    written = ipro_ble_ctlr_hcitl_host_write(hci_reset, sizeof(hci_reset));
    if (written != sizeof(hci_reset)) {
        ipro6s_bringup_status.h4_selftest_rc = -3;
        goto done;
    }

    deadline = xTaskGetTickCount() + pdMS_TO_TICKS(2000);
    while ((int32_t)(xTaskGetTickCount() - deadline) < 0) {
        uint32_t len = ipro6s_bringup_status.h4_response_len;

        if (len < sizeof(ipro6s_bringup_status.h4_response)) {
            len += ipro_ble_ctlr_hcitl_host_read(
                (uint8_t *)&ipro6s_bringup_status.h4_response[len],
                sizeof(ipro6s_bringup_status.h4_response) - len);
            ipro6s_bringup_status.h4_response_len = len;
        }

        /* Num_HCI_Command_Packets is 5 in the official v13 configuration;
         * it is a controller flow-control credit, not a fixed value of 1.
         * Validate framing, opcode and status instead of that credit byte. */
        if (len >= 7) {
            uint32_t last = len - 7;
            for (uint32_t offset = 0; offset <= last; offset++) {
                const volatile uint8_t *evt =
                    &ipro6s_bringup_status.h4_response[offset];
                if (evt[0] == 0x04 && evt[1] == 0x0e && evt[2] >= 4 &&
                    evt[4] == 0x03 && evt[5] == 0x0c && evt[6] == 0x00) {
                    ipro6s_bringup_status.h4_selftest_rc = 0;
                    ipro6s_bringup_status.h4_selftest_state =
                        IPRO6S_H4_SELFTEST_PASS;
                    goto done;
                }
            }
        }
        vTaskDelay(1);
    }

    ipro6s_bringup_status.h4_selftest_rc = -4;

done:
    ipro6s_bringup_status.h4_h2c_bytes =
        ipro_ble_ctlr_hcitl_h2c_bytes;
    ipro6s_bringup_status.h4_c2h_bytes =
        ipro_ble_ctlr_hcitl_c2h_bytes;
    if (ipro6s_bringup_status.h4_selftest_state != IPRO6S_H4_SELFTEST_PASS) {
        ipro6s_bringup_status.h4_selftest_state = IPRO6S_H4_SELFTEST_FAIL;
    }
}
#endif

#if defined(CONFIG_BTDM_HOST_BRINGUP)
#if defined(CONFIG_BT_BREDR)
/* The BR signaling MTU permits at most 43 bytes of ECHO data.  Two small
 * buffers are sufficient because ownership transfers to the host TX path and
 * the PTS test sends requests serially.  This deliberately avoids a large
 * diagnostic trace allocation. */
#define IPRO6S_BR_ECHO_DATA_MAX 43u
#define IPRO6S_BR_ECHO_TEST_DATA_LEN 16u
#define IPRO6S_RFCOMM_TEST_MTU 64u
NET_BUF_POOL_FIXED_DEFINE(s_ipro6s_br_echo_pool, 2,
                          BT_L2CAP_SDU_BUF_SIZE(IPRO6S_BR_ECHO_DATA_MAX),
                          CONFIG_BT_CONN_TX_USER_DATA_SIZE, NULL);

static uint32_t ipro6s_echo_hash(const uint8_t *data, size_t len)
{
    uint32_t hash = 2166136261u;

    while (len-- != 0u) {
        hash = (hash ^ *data++) * 16777619u;
    }
    return hash;
}

static void ipro6s_br_echo_request(struct bt_conn *conn, uint8_t identifier,
                                   struct net_buf *request)
{
    static const uint8_t suffix[] = "IPRO6S-L2CAP";
    uint8_t expected[IPRO6S_BR_ECHO_TEST_DATA_LEN];
    struct net_buf *response;
    uint32_t expected_sequence;
    uint32_t bad_index = UINT32_MAX;
    int err;

    ipro6s_bringup_status.host_br_echo_request_count++;
    ipro6s_bringup_status.host_br_echo_last_length = request->len;
    expected_sequence = 0u;
    if (request->len >= sizeof(expected_sequence)) {
        memcpy(&expected_sequence, request->data, sizeof(expected_sequence));
        memcpy(expected, request->data, sizeof(expected_sequence));
    } else {
        memset(expected, 0xff, sizeof(expected_sequence));
    }
    memcpy(expected + sizeof(expected_sequence), suffix, sizeof(suffix) - 1u);
    ipro6s_bringup_status.host_br_echo_last_sequence = expected_sequence;
    ipro6s_bringup_status.host_br_echo_last_rx_hash =
        ipro6s_echo_hash(request->data, request->len);
    if (request->len != sizeof(expected)) {
        bad_index = (request->len < sizeof(expected)) ? request->len : sizeof(expected);
    } else {
        for (uint32_t i = sizeof(expected_sequence); i < sizeof(expected); i++) {
            if (request->data[i] != expected[i]) {
                bad_index = i;
                break;
            }
        }
    }
    if (bad_index != UINT32_MAX) {
        uint8_t expected_byte = bad_index < sizeof(expected) ? expected[bad_index] : 0xffu;
        uint8_t actual_byte = bad_index < request->len ? request->data[bad_index] : 0xffu;

        ipro6s_bringup_status.host_br_echo_rx_pattern_error_count++;
        ipro6s_bringup_status.host_br_echo_last_bad_index = bad_index;
        ipro6s_bringup_status.host_br_echo_last_bad_expected_actual =
            ((uint32_t)expected_byte << 8) | actual_byte;
        if (ipro6s_bringup_status.host_br_echo_rx_pattern_error_count == 1u) {
            printf("[BTDM_ECHO_RX_BAD] seq=%lu index=%lu expected=%02x actual=%02x hash=%08lx\r\n",
                   (unsigned long)expected_sequence, (unsigned long)bad_index,
                   expected_byte, actual_byte,
                   (unsigned long)ipro6s_bringup_status.host_br_echo_last_rx_hash);
#if defined(CONFIG_BTDM_BT_ENABLE)
            printf("[BTDM_ECHO_BOUNDARY] lc=%lu buf/len=%04x/%u hash=%08lx/%08lx "
                   "hci=%lu buf/len=%04x/%u hash=%08lx host=%08lx\r\n",
                   (unsigned long)dbg_bt_acl_rx_count,
                   dbg_bt_acl_rx_buf, dbg_bt_acl_rx_len,
                   (unsigned long)dbg_bt_acl_rx_hash_first,
                   (unsigned long)dbg_bt_acl_rx_hash_second,
                   (unsigned long)dbg_hci_acl_c2h_count,
                   dbg_hci_acl_c2h_buf, dbg_hci_acl_c2h_len,
                   (unsigned long)dbg_hci_acl_c2h_hash,
                   (unsigned long)ipro6s_bringup_status.host_br_echo_last_rx_hash);
#endif
        }
    }
    if (request->len > IPRO6S_BR_ECHO_DATA_MAX) {
        ipro6s_bringup_status.host_br_echo_error_count++;
        ipro6s_bringup_status.host_br_echo_last_error = -EMSGSIZE;
        return;
    }

    response = net_buf_alloc(&s_ipro6s_br_echo_pool, K_NO_WAIT);
    if (response == NULL) {
        ipro6s_bringup_status.host_br_echo_error_count++;
        ipro6s_bringup_status.host_br_echo_last_error = -ENOMEM;
        return;
    }

    net_buf_reserve(response, BT_L2CAP_BR_ECHO_RSP_RESERVE);
    net_buf_add_mem(response, request->data, request->len);
    ipro6s_bringup_status.host_br_echo_last_copy_hash =
        ipro6s_echo_hash(response->data, response->len);
    if (ipro6s_bringup_status.host_br_echo_last_copy_hash !=
        ipro6s_bringup_status.host_br_echo_last_rx_hash) {
        ipro6s_bringup_status.host_br_echo_copy_error_count++;
        if (ipro6s_bringup_status.host_br_echo_copy_error_count == 1u) {
            printf("[BTDM_ECHO_COPY_BAD] seq=%lu rx/copy=%08lx/%08lx\r\n",
                   (unsigned long)expected_sequence,
                   (unsigned long)ipro6s_bringup_status.host_br_echo_last_rx_hash,
                   (unsigned long)ipro6s_bringup_status.host_br_echo_last_copy_hash);
        }
    }
    err = bt_l2cap_br_echo_rsp(conn, identifier, response);
    if (err != 0) {
        net_buf_unref(response);
        ipro6s_bringup_status.host_br_echo_error_count++;
        ipro6s_bringup_status.host_br_echo_last_error = err;
        return;
    }

    ipro6s_bringup_status.host_br_echo_response_count++;
    ipro6s_bringup_status.host_br_echo_last_error = 0;
}

static struct bt_l2cap_br_echo_cb s_ipro6s_br_echo_callbacks = {
    .req = ipro6s_br_echo_request,
};

NET_BUF_POOL_FIXED_DEFINE(s_ipro6s_rfcomm_pool, 2,
                          BT_RFCOMM_BUF_SIZE(IPRO6S_RFCOMM_TEST_MTU),
                          CONFIG_BT_CONN_TX_USER_DATA_SIZE, NULL);

static struct bt_sdp_attribute s_ipro6s_spp_attrs[] = {
    BT_SDP_NEW_SERVICE,
    BT_SDP_LIST(
        BT_SDP_ATTR_SVCLASS_ID_LIST,
        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 3),
        BT_SDP_DATA_ELEM_LIST(
            {
                BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                BT_SDP_ARRAY_16(BT_SDP_SERIAL_PORT_SVCLASS)
            },
        )
    ),
    BT_SDP_LIST(
        BT_SDP_ATTR_PROTO_DESC_LIST,
        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 12),
        BT_SDP_DATA_ELEM_LIST(
            {
                BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 3),
                BT_SDP_DATA_ELEM_LIST(
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                        BT_SDP_ARRAY_16(BT_SDP_PROTO_L2CAP)
                    },
                )
            },
            {
                BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 5),
                BT_SDP_DATA_ELEM_LIST(
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                        BT_SDP_ARRAY_16(BT_SDP_PROTO_RFCOMM)
                    },
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UINT8),
                        BT_SDP_ARRAY_8(BT_RFCOMM_CHAN_SPP)
                    },
                )
            },
        )
    ),
    BT_SDP_LIST(
        BT_SDP_ATTR_PROFILE_DESC_LIST,
        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 8),
        BT_SDP_DATA_ELEM_LIST(
            {
                BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
                BT_SDP_DATA_ELEM_LIST(
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                        BT_SDP_ARRAY_16(BT_SDP_SERIAL_PORT_SVCLASS)
                    },
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UINT16),
                        BT_SDP_ARRAY_16(0x0102)
                    },
                )
            },
        )
    ),
    BT_SDP_SERVICE_NAME("IPRO6S Host SPP"),
};

static struct bt_sdp_record s_ipro6s_spp_record =
    BT_SDP_RECORD(s_ipro6s_spp_attrs);

static void ipro6s_rfcomm_connected(struct bt_rfcomm_dlc *dlc)
{
    (void)dlc;
    s_ipro6s_rfcomm_connect_count++;
}

static void ipro6s_rfcomm_disconnected(struct bt_rfcomm_dlc *dlc)
{
    (void)dlc;
    s_ipro6s_rfcomm_disconnect_count++;
}

static void ipro6s_rfcomm_recv(struct bt_rfcomm_dlc *dlc, struct net_buf *buf)
{
    struct net_buf *rsp;
    int err;

    s_ipro6s_rfcomm_rx_count++;
    s_ipro6s_rfcomm_last_rx_hash = ipro6s_echo_hash(buf->data, buf->len);
    if (buf->len > IPRO6S_RFCOMM_TEST_MTU) {
        s_ipro6s_rfcomm_error_count++;
        return;
    }
    rsp = bt_rfcomm_create_pdu(&s_ipro6s_rfcomm_pool);
    if (rsp == NULL) {
        s_ipro6s_rfcomm_error_count++;
        return;
    }
    net_buf_add_mem(rsp, buf->data, buf->len);
    s_ipro6s_rfcomm_last_tx_hash = ipro6s_echo_hash(rsp->data, rsp->len);
    if (s_ipro6s_rfcomm_last_tx_hash != s_ipro6s_rfcomm_last_rx_hash) {
        s_ipro6s_rfcomm_error_count++;
        net_buf_unref(rsp);
        return;
    }
    err = bt_rfcomm_dlc_send(dlc, rsp);
    if (err < 0) {
        s_ipro6s_rfcomm_error_count++;
        net_buf_unref(rsp);
        return;
    }
    s_ipro6s_rfcomm_tx_count++;
}

static struct bt_rfcomm_dlc_ops s_ipro6s_rfcomm_ops = {
    .recv = ipro6s_rfcomm_recv,
    .connected = ipro6s_rfcomm_connected,
    .disconnected = ipro6s_rfcomm_disconnected,
};

static struct bt_rfcomm_dlc s_ipro6s_rfcomm_dlc = {
    .ops = &s_ipro6s_rfcomm_ops,
    .mtu = IPRO6S_RFCOMM_TEST_MTU,
};

static int ipro6s_rfcomm_accept(struct bt_conn *conn,
                               struct bt_rfcomm_server *server,
                               struct bt_rfcomm_dlc **dlc)
{
    (void)conn;
    (void)server;
    if (s_ipro6s_rfcomm_dlc.session != NULL) {
        return -ENOMEM;
    }
    *dlc = &s_ipro6s_rfcomm_dlc;
    return 0;
}

static struct bt_rfcomm_server s_ipro6s_rfcomm_server = {
    .channel = BT_RFCOMM_CHAN_SPP,
    .accept = ipro6s_rfcomm_accept,
};

#if defined(CONFIG_BT_A2DP_SINK)
BT_A2DP_SBC_SINK_EP_DEFAULT(s_ipro6s_a2dp_sink_ep);
static struct bt_a2dp_stream s_ipro6s_a2dp_stream;

static struct bt_sdp_attribute s_ipro6s_a2dp_sink_attrs[] = {
    BT_SDP_NEW_SERVICE,
    BT_SDP_LIST(
        BT_SDP_ATTR_SVCLASS_ID_LIST,
        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 3),
        BT_SDP_DATA_ELEM_LIST(
            {
                BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                BT_SDP_ARRAY_16(BT_SDP_AUDIO_SINK_SVCLASS)
            },
        )
    ),
    BT_SDP_LIST(
        BT_SDP_ATTR_PROTO_DESC_LIST,
        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 16),
        BT_SDP_DATA_ELEM_LIST(
            {
                BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
                BT_SDP_DATA_ELEM_LIST(
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                        BT_SDP_ARRAY_16(BT_SDP_PROTO_L2CAP)
                    },
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UINT16),
                        BT_SDP_ARRAY_16(BT_UUID_AVDTP_VAL)
                    },
                )
            },
            {
                BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
                BT_SDP_DATA_ELEM_LIST(
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                        BT_SDP_ARRAY_16(BT_UUID_AVDTP_VAL)
                    },
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UINT16),
                        BT_SDP_ARRAY_16(0x0103)
                    },
                )
            },
        )
    ),
    BT_SDP_LIST(
        BT_SDP_ATTR_PROFILE_DESC_LIST,
        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 8),
        BT_SDP_DATA_ELEM_LIST(
            {
                BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
                BT_SDP_DATA_ELEM_LIST(
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                        BT_SDP_ARRAY_16(BT_SDP_ADVANCED_AUDIO_SVCLASS)
                    },
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UINT16),
                        BT_SDP_ARRAY_16(0x0104)
                    },
                )
            },
        )
    ),
    BT_SDP_SERVICE_NAME("IPRO6S A2DP Sink"),
    BT_SDP_SUPPORTED_FEATURES(0x0001),
};

static struct bt_sdp_record s_ipro6s_a2dp_sink_record =
    BT_SDP_RECORD(s_ipro6s_a2dp_sink_attrs);

static void ipro6s_a2dp_stream_configured(struct bt_a2dp_stream *stream)
{
    s_ipro6s_a2dp_config_count++;
#if defined(IPRO6_A2DP_PCM_OUTPUT)
    struct bt_a2dp_codec_sbc_params *sbc;

    s_ipro6s_a2dp_sbc_ready = 0u;
    if (stream == NULL || stream->codec_config.len != BT_A2DP_SBC_IE_LENGTH) {
        s_ipro6s_a2dp_decoder_rc = -1;
        return;
    }
    sbc = (struct bt_a2dp_codec_sbc_params *)stream->codec_config.codec_ie;
    s_ipro6s_a2dp_sample_rate = bt_a2dp_sbc_get_sampling_frequency(sbc);
    s_ipro6s_a2dp_channels = bt_a2dp_sbc_get_channel_num(sbc);
    s_ipro6s_a2dp_decoder_rc = sbc_setup_decoder(&s_ipro6s_a2dp_sbc_decoder);
    s_ipro6s_a2dp_i2s_config_rc =
        ipro6_sco_i2s_configure(s_ipro6s_a2dp_sample_rate,
                                s_ipro6s_a2dp_channels);
    if (s_ipro6s_a2dp_decoder_rc == 0 &&
        s_ipro6s_a2dp_i2s_config_rc == 0) {
        s_ipro6s_a2dp_sbc_ready = 1u;
    }
#else
    (void)stream;
#endif
}

static void ipro6s_a2dp_stream_established(struct bt_a2dp_stream *stream)
{
    (void)stream;
    s_ipro6s_a2dp_establish_count++;
}

static void ipro6s_a2dp_stream_released(struct bt_a2dp_stream *stream)
{
    (void)stream;
    s_ipro6s_a2dp_release_count++;
#if defined(IPRO6_A2DP_PCM_OUTPUT)
    ipro6_sco_i2s_set_a2dp_active(false);
#endif
}

static void ipro6s_a2dp_stream_started(struct bt_a2dp_stream *stream)
{
    (void)stream;
    s_ipro6s_a2dp_start_count++;
    /* RTP sequence numbers belong to one media stream.  A new Start may use
     * a new initial sequence number, so establish the continuity anchor from
     * its first packet and enforce +1 for every following packet. */
    s_ipro6s_a2dp_media_sequence_valid = false;
    s_ipro6s_a2dp_stream_packet_count = 0;
    s_ipro6s_a2dp_stream_byte_count = 0;
    s_ipro6s_a2dp_stream_hash = 2166136261u;
    s_ipro6s_a2dp_stream_seq_error_count = 0;
    s_ipro6s_a2dp_content_valid_count = 0;
    s_ipro6s_a2dp_content_error_count = 0;
    s_ipro6s_a2dp_content_error_bytes = 0;
    s_ipro6s_a2dp_content_unchecked_count = 0;
    s_ipro6s_a2dp_first_bad_seq = 0;
    s_ipro6s_a2dp_first_bad_index = 0;
    s_ipro6s_a2dp_first_bad_expected_actual = 0;
#if defined(IPRO6_A2DP_PCM_OUTPUT)
    s_ipro6s_a2dp_decode_frames = 0u;
    s_ipro6s_a2dp_decode_bytes = 0u;
    s_ipro6s_a2dp_decode_errors = 0u;
    s_ipro6s_a2dp_decode_drops = 0u;
    s_ipro6s_a2dp_decode_hash = 2166136261u;
    s_ipro6s_a2dp_decoder_rc = sbc_setup_decoder(&s_ipro6s_a2dp_sbc_decoder);
    s_ipro6s_a2dp_sbc_ready =
        (s_ipro6s_a2dp_decoder_rc == 0 &&
         s_ipro6s_a2dp_i2s_config_rc == 0) ? 1u : 0u;
    ipro6_sco_i2s_set_a2dp_active(s_ipro6s_a2dp_sbc_ready != 0u);
#endif
}

static void ipro6s_a2dp_stream_suspended(struct bt_a2dp_stream *stream)
{
    (void)stream;
    s_ipro6s_a2dp_suspend_count++;
#if defined(IPRO6_A2DP_PCM_OUTPUT)
    ipro6_sco_i2s_set_a2dp_active(false);
#endif
}

static void ipro6s_a2dp_stream_recv(struct bt_a2dp_stream *stream,
                                    struct net_buf *buf, uint16_t seq_num,
                                    uint32_t timestamp)
{
    uint32_t hash = s_ipro6s_a2dp_media_hash;
    uint32_t stream_hash = s_ipro6s_a2dp_stream_hash;
    bool content_bad = false;

    (void)stream;
    if (s_ipro6s_a2dp_media_sequence_valid &&
        seq_num != (uint16_t)(s_ipro6s_a2dp_last_seq + 1u)) {
        s_ipro6s_a2dp_media_seq_error_count++;
        s_ipro6s_a2dp_stream_seq_error_count++;
    }
    for (uint16_t i = 0; i < buf->len; i++) {
        hash = (hash ^ buf->data[i]) * 16777619u;
        stream_hash = (stream_hash ^ buf->data[i]) * 16777619u;
        /* pts_usb_hci.py sends one SBC media-header byte followed by a
         * deterministic 60-byte payload.  Check bytes here, at the final
         * Host callback boundary, so a sender-side count cannot hide an
         * HCI/L2CAP data corruption. */
        if (buf->len != 61u) {
            continue;
        }
        uint8_t expected = (i == 0u)
            ? 0x01u
            : (uint8_t)(((uint32_t)seq_num * 29u +
                         (uint32_t)(i - 1u) * 17u + 0x53u) & 0xffu);
        if (buf->data[i] != expected) {
            if (s_ipro6s_a2dp_content_error_count == 0u && !content_bad) {
                s_ipro6s_a2dp_first_bad_seq = seq_num;
                s_ipro6s_a2dp_first_bad_index = i;
                s_ipro6s_a2dp_first_bad_expected_actual =
                    ((uint16_t)expected << 8) | buf->data[i];
            }
            s_ipro6s_a2dp_content_error_bytes++;
            content_bad = true;
        }
    }
    if (buf->len != 61u) {
        /* Real SBC sources negotiate their own media payload size. Validate
         * the A2DP SBC payload header and the first complete frame sync word;
         * this catches truncation, zero-filled data and codec-boundary errors
         * without assuming that Android emits the PTS test pattern. */
        uint16_t bad_index = 0u;
        uint16_t expected_actual = 0u;
        bool any_nonzero = false;

        if (buf->len < 2u) {
            content_bad = true;
            expected_actual = 0x0200u | (uint16_t)buf->len;
        } else {
            uint8_t media_header = buf->data[0];
            uint8_t frame_count = BT_A2DP_SBC_MEDIA_HDR_NUM_FRAMES_GET(media_header);
            uint8_t fragment_flags = media_header & 0xe0u;

            if ((media_header & 0x10u) != 0u ||
                (frame_count == 0u && fragment_flags == 0u)) {
                content_bad = true;
                expected_actual = media_header;
            } else if (fragment_flags == 0u && buf->data[1] != 0x9cu) {
                content_bad = true;
                bad_index = 1u;
                expected_actual = 0x9c00u | buf->data[1];
            }
            for (uint16_t i = 1u; i < buf->len; i++) {
                any_nonzero = any_nonzero || (buf->data[i] != 0u);
            }
            if (!any_nonzero) {
                content_bad = true;
                bad_index = 1u;
                expected_actual = 0x0100u;
            }
        }
        if (content_bad) {
            s_ipro6s_a2dp_content_error_bytes++;
            if (s_ipro6s_a2dp_content_error_count == 0u) {
                s_ipro6s_a2dp_first_bad_seq = seq_num;
                s_ipro6s_a2dp_first_bad_index = bad_index;
                s_ipro6s_a2dp_first_bad_expected_actual = expected_actual;
            }
        }
    }
    if (content_bad) {
        s_ipro6s_a2dp_content_error_count++;
    } else {
        s_ipro6s_a2dp_content_valid_count++;
    }
    s_ipro6s_a2dp_media_hash = hash;
    s_ipro6s_a2dp_stream_hash = stream_hash;
    s_ipro6s_a2dp_media_packet_count++;
    s_ipro6s_a2dp_media_byte_count += buf->len;
    s_ipro6s_a2dp_stream_packet_count++;
    s_ipro6s_a2dp_stream_byte_count += buf->len;
    s_ipro6s_a2dp_last_seq = seq_num;
    s_ipro6s_a2dp_media_sequence_valid = true;
    s_ipro6s_a2dp_last_len = buf->len;
    s_ipro6s_a2dp_last_timestamp = timestamp;
#if defined(IPRO6_A2DP_PCM_OUTPUT)
    if (s_ipro6s_a2dp_sbc_ready != 0u && buf->len > 1u) {
        uint8_t media_header = buf->data[0];
        uint8_t frame_count = BT_A2DP_SBC_MEDIA_HDR_NUM_FRAMES_GET(media_header);
        const void *input = &buf->data[1];
        uint32_t input_size = buf->len - 1u;

        if (BT_A2DP_SBC_MEDIA_HDR_F_GET(media_header) != 0u ||
            frame_count == 0u) {
            s_ipro6s_a2dp_decode_errors++;
        } else {
            for (uint8_t frame = 0u; frame < frame_count; frame++) {
                uint32_t output_size = sizeof(s_ipro6s_a2dp_pcm);
                int rc = sbc_decode(&s_ipro6s_a2dp_sbc_decoder,
                                    &input, &input_size,
                                    s_ipro6s_a2dp_pcm, &output_size);

                if (rc != 0 || output_size == 0u ||
                    (output_size & 1u) != 0u) {
                    s_ipro6s_a2dp_decode_errors++;
                    break;
                }
                for (uint32_t i = 0u; i < output_size; i++) {
                    s_ipro6s_a2dp_decode_hash =
                        (s_ipro6s_a2dp_decode_hash ^ s_ipro6s_a2dp_pcm[i]) *
                        16777619u;
                }
                s_ipro6s_a2dp_decode_frames++;
                s_ipro6s_a2dp_decode_bytes += output_size;
                if (!ipro6_sco_i2s_write(s_ipro6s_a2dp_pcm, output_size)) {
                    s_ipro6s_a2dp_decode_drops++;
                }
            }
        }
    }
#endif
}

static struct bt_a2dp_stream_ops s_ipro6s_a2dp_stream_ops = {
    .configured = ipro6s_a2dp_stream_configured,
    .established = ipro6s_a2dp_stream_established,
    .released = ipro6s_a2dp_stream_released,
    .started = ipro6s_a2dp_stream_started,
    .suspended = ipro6s_a2dp_stream_suspended,
    .recv = ipro6s_a2dp_stream_recv,
};

static void ipro6s_a2dp_connected(struct bt_a2dp *a2dp, int err)
{
    (void)a2dp;
    if (err == 0) {
        s_ipro6s_a2dp_connect_count++;
    }
}

static void ipro6s_a2dp_disconnected(struct bt_a2dp *a2dp)
{
    (void)a2dp;
    s_ipro6s_a2dp_disconnect_count++;
#if defined(IPRO6_A2DP_PCM_OUTPUT)
    ipro6_sco_i2s_set_a2dp_active(false);
#endif
}

static int ipro6s_a2dp_config_req(struct bt_a2dp *a2dp,
                                  struct bt_a2dp_ep *ep,
                                  struct bt_a2dp_codec_cfg *codec_cfg,
                                  struct bt_a2dp_stream **stream,
                                  uint8_t *rsp_err_code)
{
    (void)a2dp;
    (void)ep;
    (void)codec_cfg;
    bt_a2dp_stream_cb_register(&s_ipro6s_a2dp_stream,
                               &s_ipro6s_a2dp_stream_ops);
    *stream = &s_ipro6s_a2dp_stream;
    *rsp_err_code = BT_AVDTP_SUCCESS;
    return 0;
}

static int ipro6s_a2dp_reconfig_req(struct bt_a2dp_stream *stream,
                                    struct bt_a2dp_codec_cfg *codec_cfg,
                                    uint8_t *rsp_err_code)
{
    (void)stream;
    (void)codec_cfg;
    *rsp_err_code = BT_AVDTP_SUCCESS;
    return 0;
}

static int ipro6s_a2dp_ctrl_req(struct bt_a2dp_stream *stream,
                                uint8_t *rsp_err_code)
{
    (void)stream;
    *rsp_err_code = BT_AVDTP_SUCCESS;
    return 0;
}

static int ipro6s_a2dp_get_config_req(struct bt_a2dp_stream *stream,
                                      uint8_t *rsp_err_code)
{
    return ipro6s_a2dp_ctrl_req(stream, rsp_err_code);
}

static struct bt_a2dp_cb s_ipro6s_a2dp_callbacks = {
    .connected = ipro6s_a2dp_connected,
    .disconnected = ipro6s_a2dp_disconnected,
    .config_req = ipro6s_a2dp_config_req,
    .reconfig_req = ipro6s_a2dp_reconfig_req,
    .establish_req = ipro6s_a2dp_ctrl_req,
    .release_req = ipro6s_a2dp_ctrl_req,
    .start_req = ipro6s_a2dp_ctrl_req,
    .suspend_req = ipro6s_a2dp_ctrl_req,
    .abort_req = ipro6s_a2dp_ctrl_req,
    .get_config_req = ipro6s_a2dp_get_config_req,
};
#endif

#if defined(CONFIG_BT_AVRCP)
static struct bt_avrcp_ct *s_avrcp_control;
#if defined(CONFIG_SHELL)
static atomic_t s_avrcp_key_pending;
static uint8_t s_avrcp_key;
static uint8_t s_avrcp_key_tid = 2;

static void ipro6s_avrcp_key_work(struct k_work *work)
{
    int press = -ENOTCONN;
    int release = -ENOTCONN;
    (void)work;

    /* AVCTP serializes its TX queue with a semaphore; these calls must run
     * in normal task context with the scheduler enabled. */
    if (s_avrcp_control != NULL) {
        uint8_t tid = s_avrcp_key_tid;
        /* 0/1 are metadata; 14 is reserved for playback notification. */
        s_avrcp_key_tid = tid >= 12u ? 2u : tid + 2u;
        press = bt_avrcp_ct_passthrough(s_avrcp_control, tid, s_avrcp_key,
                                       BT_AVRCP_BUTTON_PRESSED, NULL, 0);
        if (press == 0) {
            release = bt_avrcp_ct_passthrough(s_avrcp_control, (tid + 1u) & 15u,
                                             s_avrcp_key, BT_AVRCP_BUTTON_RELEASED,
                                             NULL, 0);
        }
    }
    IPRO_LOGI("AVRCP", "key=%u send press/release=%d/%d", s_avrcp_key, press, release);
    atomic_clear(&s_avrcp_key_pending);
}
K_WORK_DEFINE(s_avrcp_key_work, ipro6s_avrcp_key_work);

static void cmd_ipro6s_avrcp(int argc, char **argv)
{
    uint8_t op;
    if (argc != 2 || (strcmp(argv[1], "play") != 0 && strcmp(argv[1], "pause") != 0)) {
        IPRO_LOGI("AVRCP", "usage: avrcp play|pause");
        return;
    }
    op = strcmp(argv[1], "play") == 0 ? BT_AVRCP_OPID_PLAY : BT_AVRCP_OPID_PAUSE;
    if (!atomic_cas(&s_avrcp_key_pending, 0, 1)) {
        IPRO_LOGW("AVRCP", "key command busy");
        return;
    }
    s_avrcp_key = op;
    int err = k_work_submit(&s_avrcp_key_work);
    if (err < 0) {
        atomic_clear(&s_avrcp_key_pending);
        IPRO_LOGW("AVRCP", "key queue rc=%d", err);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_avrcp, avrcp, AVRCP phone play/pause);
#endif

static void ipro6s_avrcp_passthrough_rsp(struct bt_avrcp_ct *ct, uint8_t tid,
                                       bt_avrcp_rsp_t result,
                                       const struct bt_avrcp_passthrough_rsp *rsp)
{
    (void)ct;
    IPRO_LOGI("AVRCP", "key response tid=%u result=%u op=%u state=%u", tid,
              result, rsp != NULL ? (unsigned)BT_AVRCP_PASSTHROUGH_GET_OPID(rsp) : 255u,
              rsp != NULL ? (unsigned)BT_AVRCP_PASSTHROUGH_GET_STATE(rsp) : 255u);
}

static void ipro6s_avrcp_notify_changed(struct bt_avrcp_ct *ct, uint8_t event,
                                      struct bt_avrcp_event_data *data)
{
    if (event != BT_AVRCP_EVT_PLAYBACK_STATUS_CHANGED || data == NULL) {
        return;
    }
    IPRO_LOGI("AVRCP", "notify CHANGED event=%u state=%u", event, data->play_status);
    /* AVRCP consumes a registration after CHANGED. The Host clears the old
     * callback before dispatch, permitting a fresh subscription here. */
    int err = bt_avrcp_ct_register_notification(ct, 14, event, 0,
                                               ipro6s_avrcp_notify_changed);
    IPRO_LOGI("AVRCP", "notify rearm rc=%d", err);
}

static void ipro6s_avrcp_notification(struct bt_avrcp_ct *ct, uint8_t tid,
                                    uint8_t status, uint8_t event,
                                    struct bt_avrcp_event_data *data)
{
    (void)ct;
    if (status == BT_AVRCP_STATUS_SUCCESS &&
        event == BT_AVRCP_EVT_PLAYBACK_STATUS_CHANGED && data != NULL) {
        IPRO_LOGI("AVRCP", "notify INTERIM tid=%u event=%u state=%u",
                  tid, event, data->play_status);
    } else {
        IPRO_LOGW("AVRCP", "notify rejected tid=%u event=%u status=%u", tid, event, status);
    }
}

static void ipro6s_avrcp_get_element_attrs(struct bt_avrcp_ct *ct, uint8_t tid,
                                         uint8_t status, struct net_buf *buf)
{
    (void)ct;
    if (buf == NULL || !ipro6_avrcp_validate_attrs(buf->data, buf->len)) {
        IPRO_LOGW("AVRCP", "metadata tid=%u status=%u invalid/rejected len=%u",
                  tid, status, buf != NULL ? (unsigned)buf->len : 0u);
        return;
    }
    const uint8_t *p = buf->data + 1;
    IPRO_LOGI("AVRCP", "metadata tid=%u status=%u attrs=%u", tid, status, buf->data[0]);
    for (unsigned i = 0; i < buf->data[0]; ++i) {
        unsigned charset = ((unsigned)p[4] << 8) | p[5];
        unsigned len = ((unsigned)p[6] << 8) | p[7];
        /* Limit UART cost without confusing display truncation with wire length. */
        IPRO_LOGI("AVRCP", "attr=%lu charset=%u len=%u text=%.*s",
                  (unsigned long)ipro6_avrcp_be32(p), charset, len,
                  (int)(len > 80 ? 80 : len), (const char *)(p + 8));
        p += 8 + len;
    }
}

static void ipro6s_avrcp_get_play_status(struct bt_avrcp_ct *ct, uint8_t tid,
                                       uint8_t status, struct net_buf *buf)
{
    struct ipro6_avrcp_play_status play;
    struct net_buf *request;
    int err;

    if (buf != NULL && ipro6_avrcp_decode_play(buf->data, buf->len, &play)) {
        IPRO_LOGI("AVRCP", "play tid=%u status=%u state=%u length_ms=%lu position_ms=%lu",
                  tid, status, play.state, (unsigned long)play.length_ms,
                  (unsigned long)play.position_ms);
    } else {
        IPRO_LOGW("AVRCP", "play tid=%u status=%u invalid/rejected len=%u",
                  tid, status, buf != NULL ? (unsigned)buf->len : 0u);
    }

    request = bt_avrcp_create_vendor_pdu(NULL);
    if (request == NULL) {
        IPRO_LOGW("AVRCP", "metadata request: no buffer");
        return;
    }
    /* Current element identifier=0, num_attrs=0 requests all attributes. */
    net_buf_add_mem(request, (const uint8_t[9]){0}, 9);
    err = bt_avrcp_ct_get_element_attrs(ct, (tid + 1u) & 15u, request);
    if (err != 0) {
        net_buf_unref(request);
    }
    IPRO_LOGI("AVRCP", "metadata request rc=%d", err);
}

static void ipro6s_avrcp_ct_connected(struct bt_conn *conn,
                                      struct bt_avrcp_ct *ct)
{
    (void)conn;
    s_ipro6s_avrcp_ct_connect_count++;
    s_avrcp_control = ct;
    /* Start from the Host callback, where this CT instance is live. */
    int err = bt_avrcp_ct_get_play_status(ct, 0);
    IPRO_LOGI("AVRCP", "play request rc=%d", err);
    err = bt_avrcp_ct_register_notification(ct, 14,
        BT_AVRCP_EVT_PLAYBACK_STATUS_CHANGED, 0, ipro6s_avrcp_notify_changed);
    IPRO_LOGI("AVRCP", "notify register rc=%d", err);
}

static void ipro6s_avrcp_ct_disconnected(struct bt_avrcp_ct *ct)
{
    if (s_avrcp_control == ct) {
        s_avrcp_control = NULL;
    }
    s_ipro6s_avrcp_ct_disconnect_count++;
}

static const struct bt_avrcp_ct_cb s_ipro6s_avrcp_ct_callbacks = {
    .connected = ipro6s_avrcp_ct_connected,
    .disconnected = ipro6s_avrcp_ct_disconnected,
    .get_play_status = ipro6s_avrcp_get_play_status,
    .get_element_attrs = ipro6s_avrcp_get_element_attrs,
    .passthrough_rsp = ipro6s_avrcp_passthrough_rsp,
    .notification = ipro6s_avrcp_notification,
};

static void ipro6s_avrcp_tg_connected(struct bt_conn *conn,
                                      struct bt_avrcp_tg *tg)
{
    (void)conn;
    (void)tg;
    s_ipro6s_avrcp_tg_connect_count++;
}

static void ipro6s_avrcp_tg_disconnected(struct bt_avrcp_tg *tg)
{
    (void)tg;
    s_ipro6s_avrcp_tg_disconnect_count++;
}

static void ipro6s_avrcp_unit_info_req(struct bt_avrcp_tg *tg, uint8_t tid)
{
    struct bt_avrcp_unit_info_rsp rsp = {
        .unit_type = BT_AVRCP_SUBUNIT_TYPE_PANEL,
        .company_id = BT_AVRCP_COMPANY_ID_BLUETOOTH_SIG,
    };

    s_ipro6s_avrcp_unit_info_count++;
    if (bt_avrcp_tg_send_unit_info_rsp(tg, tid, &rsp) != 0) {
        s_ipro6s_avrcp_response_error_count++;
    }
}

static void ipro6s_avrcp_subunit_info_req(struct bt_avrcp_tg *tg, uint8_t tid)
{
    s_ipro6s_avrcp_subunit_info_count++;
    if (bt_avrcp_tg_send_subunit_info_rsp(tg, tid) != 0) {
        s_ipro6s_avrcp_response_error_count++;
    }
}

static void ipro6s_avrcp_passthrough_req(struct bt_avrcp_tg *tg, uint8_t tid,
                                         struct net_buf *buf)
{
    struct net_buf *rsp;
    const struct bt_avrcp_passthrough_cmd *cmd;
    size_t payload_len;
    int err;

    s_ipro6s_avrcp_passthrough_count++;
    if (buf == NULL || buf->len < sizeof(*cmd)) {
        s_ipro6s_avrcp_response_error_count++;
        return;
    }

    cmd = (const struct bt_avrcp_passthrough_cmd *)buf->data;
    payload_len = sizeof(*cmd) + cmd->data_len;
    if (payload_len > buf->len) {
        s_ipro6s_avrcp_response_error_count++;
        return;
    }

    rsp = bt_avrcp_create_pdu(NULL);
    if (rsp == NULL || net_buf_tailroom(rsp) < payload_len) {
        if (rsp != NULL) {
            net_buf_unref(rsp);
        }
        s_ipro6s_avrcp_response_error_count++;
        return;
    }
    net_buf_add_mem(rsp, buf->data, payload_len);
    err = bt_avrcp_tg_send_passthrough_rsp(tg, tid, BT_AVRCP_RSP_ACCEPTED,
                                            rsp);
    if (err != 0) {
        net_buf_unref(rsp);
        s_ipro6s_avrcp_response_error_count++;
    }
}

static const struct bt_avrcp_tg_cb s_ipro6s_avrcp_tg_callbacks = {
    .connected = ipro6s_avrcp_tg_connected,
    .disconnected = ipro6s_avrcp_tg_disconnected,
    .unit_info_req = ipro6s_avrcp_unit_info_req,
    .subunit_info_req = ipro6s_avrcp_subunit_info_req,
    .passthrough_req = ipro6s_avrcp_passthrough_req,
};
#endif

#if defined(CONFIG_BT_HFP_HF)
static void ipro6s_hfp_hf_connected(struct bt_conn *conn,
                                     struct bt_hfp_hf *hf)
{
    (void)conn;
    (void)hf;
    s_ipro6s_hfp_hf_connect_count++;
}

static void ipro6s_hfp_hf_disconnected(struct bt_hfp_hf *hf)
{
    (void)hf;
    s_ipro6s_hfp_hf_disconnect_count++;
}

static void ipro6s_hfp_hf_sco_connected(struct bt_hfp_hf *hf,
                                         struct bt_conn *sco_conn)
{
    (void)hf;
    (void)sco_conn;
    s_ipro6s_hfp_sco_connect_count++;
#if defined(CONFIG_BTDM_VOICE_OVER_HCI)
    ipro6_sco_i2s_set_sco_active(true);
#endif
}

static void ipro6s_hfp_hf_sco_disconnected(struct bt_conn *sco_conn,
                                            uint8_t reason)
{
    (void)sco_conn;
    s_ipro6s_hfp_sco_disconnect_count++;
    s_ipro6s_hfp_sco_last_reason = reason;
#if defined(CONFIG_BTDM_VOICE_OVER_HCI)
    ipro6_sco_i2s_set_sco_active(false);
#endif
}

static void ipro6s_hfp_hf_service(struct bt_hfp_hf *hf, uint32_t value)
{
    (void)hf;
    s_ipro6s_hfp_service_value = value;
}

#if defined(CONFIG_BTDM_VOICE_OVER_HCI)
static void ipro6s_sco_data_received(uint16_t handle, uint8_t packet_status,
                                    const uint8_t *payload, uint8_t len,
                                    void *user_data)
{
    uint32_t hash = s_ipro6s_sco_data_rx_hash;
    bool content_bad = false;
    bool nonzero = false;

    (void)user_data;
    s_ipro6s_sco_data_rx_count++;
    s_ipro6s_sco_data_rx_bytes += len;
    s_ipro6s_sco_data_last_handle = handle;
    s_ipro6s_sco_data_last_len = len;
    s_ipro6s_sco_data_last_status = packet_status;
    if (packet_status != 0u) {
        s_ipro6s_sco_data_status_error_count++;
    } else if (len > 0u) {
        s_ipro6s_sco_data_valid_count++;
    }
    for (uint8_t i = 0; i < len; i++) {
        nonzero = nonzero || payload[i] != 0u;
        hash = (hash ^ payload[i]) * 16777619u;
    }
    if (nonzero) {
        s_ipro6s_sco_data_nonzero_count++;
    }
    s_ipro6s_sco_data_rx_hash = hash;

    if (packet_status == 0u && len > 0u) {
        (void)ipro6_sco_i2s_write(payload, len);
    }

    /* The raw-HCI verifier prefixes deterministic payloads with "IP" and a
     * sequence byte. Other HFP peers remain valid and are counted/hashed,
     * but are not misreported as pattern failures. */
    if (len >= 3u && payload[0] == 0x49u && payload[1] == 0x50u) {
        uint8_t sequence = payload[2];

        s_ipro6s_sco_data_checked_count++;
        for (uint8_t i = 3u; i < len; i++) {
            uint8_t expected = (uint8_t)(((uint32_t)sequence * 29u +
                                          (uint32_t)(i - 3u) * 17u +
                                          0x53u) & 0xffu);
            if (payload[i] != expected) {
                content_bad = true;
            }
        }
        if (content_bad) {
            s_ipro6s_sco_data_content_error_count++;
        }
    }

    /* Exact echo proves both directions of the internal H4 SCO path without
     * inventing an audio codec policy inside the Host component. */
    if (packet_status == 0u &&
        ipro_bt_hci_sco_send(handle, 0u, payload, len) != 0) {
        s_ipro6s_sco_data_echo_error_count++;
    }
}
#endif

static struct bt_hfp_hf_cb s_ipro6s_hfp_hf_callbacks = {
    .connected = ipro6s_hfp_hf_connected,
    .disconnected = ipro6s_hfp_hf_disconnected,
    .sco_connected = ipro6s_hfp_hf_sco_connected,
    .sco_disconnected = ipro6s_hfp_hf_sco_disconnected,
    .service = ipro6s_hfp_hf_service,
};
#endif
#endif

#if defined(CONFIG_BTDM_HOST_LE_ADV)
static volatile uint8_t s_ipro6s_le_adv_requested =
    IS_ENABLED(CONFIG_BTDM_HOST_LE_ADV_DEFAULT_ON) ? 1u : 0u;

static const struct bt_data s_ipro6s_ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),
    /* Keep the target name in the primary ADV_IND.  Requiring an active
     * scanner to receive a SCAN_RSP would make a peripheral RX-path failure
     * indistinguishable from a missing advertising transmission. */
    BT_DATA(BT_DATA_NAME_COMPLETE, "IPRO6S-BLE", sizeof("IPRO6S-BLE") - 1),
};

static const struct bt_data s_ipro6s_sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, "IPRO6S-BLE", sizeof("IPRO6S-BLE") - 1),
};

static int ipro6s_adv_start(void)
{
    const struct bt_le_adv_param param = {
        .id = BT_ID_DEFAULT,
        .options = BT_LE_ADV_OPT_CONN,
        .interval_min = 160,
        .interval_max = 320,
    };

    return bt_le_adv_start(&param, s_ipro6s_ad,
                           sizeof(s_ipro6s_ad) / sizeof(s_ipro6s_ad[0]),
                           s_ipro6s_sd,
                           sizeof(s_ipro6s_sd) / sizeof(s_ipro6s_sd[0]));
}

static int ipro6s_le_adv_apply(bool enable)
{
    int err = enable ? ipro6s_adv_start() : bt_le_adv_stop();

    return (err == -EALREADY) ? 0 : err;
}

int ipro6s_host_set_le_advertising(bool enable)
{
    s_ipro6s_le_adv_requested = enable ? 1u : 0u;

#if defined(CONFIG_BTDM_SINGLE_BR_LINK_RADIO_POLICY)
    if (enable && ipro6s_bringup_status.host_br_connect_count !=
                      ipro6s_bringup_status.host_br_disconnect_count) {
        /* Remember the APP request, but keep the proven one-link arbitration
         * policy until the active Classic ACL is released. */
        return 0;
    }
#endif
    return ipro6s_le_adv_apply(enable);
}

static TaskHandle_t s_ipro6s_adv_restart_task;

static void ipro6s_adv_restart_proc(void *arg)
{
    int err = -1;

    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(200));
    for (uint32_t attempt = 1; attempt <= 50; attempt++) {
        ipro6s_bringup_status.host_adv_restart_attempts = attempt;
        (void)bt_le_adv_stop();
        vTaskDelay(pdMS_TO_TICKS(20));
        err = ipro6s_adv_start();
        ipro6s_bringup_status.host_adv_restart_rc = err;
        if (err == 0) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    ipro6s_bringup_status.host_adv_restart_rc = err;
    ipro6s_bringup_status.host_state =
        (err == 0) ? IPRO6S_HOST_ADV : IPRO6S_HOST_FAIL;
    s_ipro6s_adv_restart_task = NULL;
    vTaskDelete(NULL);
}
#endif

#if defined(CONFIG_BT_BREDR)
static TaskHandle_t s_ipro6s_br_scan_task;
static volatile uint8_t s_ipro6s_br_discoverable_requested;
static volatile uint8_t s_ipro6s_br_link_active;

static void ipro6s_br_scan_policy_proc(void *arg)
{
    bool enable;

    (void)arg;
    /* Connection callbacks execute on the Bluetooth RX work context. A
     * synchronous HCI command from that callback deadlocks command-complete
     * processing, so apply the scan policy from a separate APP task. */
    enable = s_ipro6s_br_discoverable_requested != 0u;
#if defined(CONFIG_BTDM_SINGLE_BR_LINK_RADIO_POLICY) && \
    defined(CONFIG_BTDM_HOST_LE_ADV)
    if (s_ipro6s_br_link_active != 0u) {
        /* IPRO6S A0 currently corrupts the shared crypto/scheduler state when
         * LE advertising overlaps Classic AES-CCM. Stop the real Host
         * advertiser before pairing starts; retain the APP's requested state
         * so it can be restored after disconnect. */
        ipro6s_bringup_status.host_adv_rc = ipro6s_le_adv_apply(false);
    }
#endif
    ipro6s_bringup_status.host_br_discoverable_rc =
        bt_br_set_discoverable(enable, false);
#if defined(CONFIG_BTDM_SINGLE_BR_LINK_RADIO_POLICY) && \
    defined(CONFIG_BTDM_HOST_LE_ADV)
    if ((s_ipro6s_br_link_active == 0u) &&
        (s_ipro6s_le_adv_requested != 0u)) {
        ipro6s_bringup_status.host_adv_rc = ipro6s_le_adv_apply(true);
    }
#endif
    s_ipro6s_br_scan_task = NULL;
    vTaskDelete(NULL);
}

static void ipro6s_br_scan_policy_request(bool discoverable)
{
    s_ipro6s_br_discoverable_requested = discoverable ? 1u : 0u;
    if (s_ipro6s_br_scan_task == NULL) {
        if (xTaskCreate(ipro6s_br_scan_policy_proc, "br_scan", 1024, NULL, 8,
                        &s_ipro6s_br_scan_task) != pdPASS) {
            ipro6s_bringup_status.host_br_discoverable_rc = -ENOMEM;
        }
    }
}
#endif

static void ipro6s_connected(struct bt_conn *conn, uint8_t err)
{
    if (err == 0) {
        ipro6s_bringup_status.host_connect_count++;
#if defined(CONFIG_BT_BREDR)
        if (bt_conn_is_type(conn, BT_CONN_TYPE_BR)) {
            ipro6s_bringup_status.host_br_connect_count++;
            /* This APP is configured for one BR/EDR ACL.  Keep Page Scan
             * enabled through the connectable policy, but stop Inquiry Scan
             * once that only link is occupied.  Besides being the correct
             * one-link product policy, this separates ACL traffic from the
             * IPRO6S Inquiry Response/ACL scheduler overlap under test. */
            s_ipro6s_br_link_active = 1u;
            ipro6s_br_scan_policy_request(false);
        }
#endif
        ipro6s_bringup_status.host_state = IPRO6S_HOST_CONNECTED;
    }
}

static void ipro6s_disconnected(struct bt_conn *conn, uint8_t reason)
{
    ipro6s_bringup_status.host_disconnect_count++;
    ipro6s_bringup_status.host_last_disconnect_reason = reason;
#if defined(CONFIG_BT_BREDR)
    if (bt_conn_is_type(conn, BT_CONN_TYPE_BR)) {
        ipro6s_bringup_status.host_br_disconnect_count++;
        s_ipro6s_br_link_active = 0u;
        ipro6s_br_scan_policy_request(true);
        ipro6s_bringup_status.host_state = IPRO6S_HOST_ADV;
        return;
    }
#endif
#if defined(CONFIG_BTDM_HOST_LE_ADV)
    ipro6s_bringup_status.host_state = IPRO6S_HOST_ADV_RESTART;
    if (s_ipro6s_adv_restart_task == NULL) {
        if (xTaskCreate(ipro6s_adv_restart_proc, "adv_retry", 1024, NULL, 8,
                        &s_ipro6s_adv_restart_task) != pdPASS) {
            ipro6s_bringup_status.host_adv_restart_rc = -1;
            ipro6s_bringup_status.host_state = IPRO6S_HOST_FAIL;
        }
    }
#else
    ipro6s_bringup_status.host_state = IPRO6S_HOST_ADV;
#endif
}

static void ipro6s_security_changed(struct bt_conn *conn,
                                    bt_security_t level,
                                    enum bt_security_err err)
{
    (void)conn;
    ipro6s_bringup_status.host_security_changed_count++;
    ipro6s_bringup_status.host_security_level = (uint32_t)level;
    ipro6s_bringup_status.host_security_error = (uint32_t)err;
}

static struct bt_conn_cb s_ipro6s_conn_callbacks = {
    .connected = ipro6s_connected,
    .disconnected = ipro6s_disconnected,
    .security_changed = ipro6s_security_changed,
};

#if defined(CONFIG_BT_SETTINGS)
static void ipro6s_refresh_bond_counts(void);
#endif

static void ipro6s_pairing_complete(struct bt_conn *conn, bool bonded)
{
    (void)conn;
    ipro6s_bringup_status.host_pairing_complete_count++;
    ipro6s_bringup_status.host_pairing_bonded = bonded ? 1u : 0u;
    ipro6s_bringup_status.host_pairing_last_error = 0u;
#if defined(CONFIG_BT_SETTINGS)
    if (bonded) {
        ipro6s_refresh_bond_counts();
    }
#endif
}

static void ipro6s_pairing_failed(struct bt_conn *conn,
                                  enum bt_security_err reason)
{
    (void)conn;
    ipro6s_bringup_status.host_pairing_failed_count++;
    ipro6s_bringup_status.host_pairing_last_error = (uint32_t)reason;
}

static struct bt_conn_auth_info_cb s_ipro6s_auth_info_callbacks = {
    .pairing_complete = ipro6s_pairing_complete,
    .pairing_failed = ipro6s_pairing_failed,
};

#if defined(CONFIG_IPRO_BT_HOST_GATT_SECURITY_POLICY_TEST)
static void ipro6s_policy_passkey_display(struct bt_conn *conn,
                                          unsigned int passkey)
{
    (void)conn;
    IPRO_LOGI("GSL_TEST", "PASSKEY_DISPLAY passkey=%06u", passkey);
}

static void ipro6s_policy_passkey_confirm(struct bt_conn *conn,
                                          unsigned int passkey)
{
    int rc = bt_conn_auth_passkey_confirm(conn);

    IPRO_LOGI("GSL_TEST", "PASSKEY_CONFIRM passkey=%06u rc=%d", passkey, rc);
}

static void ipro6s_policy_auth_cancel(struct bt_conn *conn)
{
    (void)conn;
    IPRO_LOGI("GSL_TEST", "AUTH_CANCEL");
}

static struct bt_conn_auth_cb s_ipro6s_policy_auth_callbacks = {
    .passkey_display = ipro6s_policy_passkey_display,
    .passkey_confirm = ipro6s_policy_passkey_confirm,
    .cancel = ipro6s_policy_auth_cancel,
};
#endif

#if defined(CONFIG_BT_SETTINGS)
static void ipro6s_le_bond_count_cb(const struct bt_bond_info *info,
                                    void *user_data)
{
    uint32_t *count = user_data;

    (void)info;
    (*count)++;
}

#if defined(CONFIG_BT_BREDR)
static void ipro6s_br_bond_count_cb(const struct bt_br_bond_info *info,
                                    void *user_data)
{
    uint32_t *count = user_data;

    (void)info;
    (*count)++;
}
#endif

static void ipro6s_refresh_bond_counts(void)
{
    uint32_t le_bonds = 0u;
    uint32_t br_bonds = 0u;

    bt_foreach_bond(BT_ID_DEFAULT, ipro6s_le_bond_count_cb, &le_bonds);
#if defined(CONFIG_BT_BREDR)
    bt_br_foreach_bond(ipro6s_br_bond_count_cb, &br_bonds);
#endif
    ipro6s_bringup_status.host_le_bond_count = le_bonds;
    ipro6s_bringup_status.host_br_bond_count = br_bonds;
}

int ipro6s_host_clear_bonds(void)
{
    int le_err = 0;
    int br_err = 0;

#if defined(CONFIG_BT_SMP)
    le_err = bt_unpair(BT_ID_DEFAULT, NULL);
#endif
#if defined(CONFIG_BT_BREDR)
    br_err = bt_br_unpair(NULL);
#endif
    ipro6s_refresh_bond_counts();

    return (le_err != 0) ? le_err : br_err;
}
#endif

#if defined(CONFIG_BT_BREDR)
int ipro6s_host_set_br_discoverable(bool enable)
{
    int err;

    if (!enable) {
        err = bt_br_set_discoverable(false, false);
        return (err == -EALREADY) ? 0 : err;
    }

    /* Keep this operation idempotent. Tearing down an active Inquiry Scan only
     * to recreate it can race the IPRO6S scheduler; the Host cache already
     * returns -EALREADY when the requested state is installed. */
    err = bt_br_set_connectable(true, NULL);
    if (err != 0 && err != -EALREADY) {
        return err;
    }
    err = bt_br_set_discoverable(true, false);
    return (err == -EALREADY) ? 0 : err;
}
#endif

static void ipro6s_host_ready(int err)
{
    ipro6s_bringup_status.host_enable_rc = err;
    if (err != 0) {
        ipro6s_bringup_status.host_state = IPRO6S_HOST_FAIL;
        return;
    }

#if defined(CONFIG_IPRO_BT_HOST_GATT_SECURITY_TEST) && \
    defined(CONFIG_BT_GATT_SECURITY_LEVELS)
    {
        static const struct ipro_bt_gatt_security_requirement late_requirement = {
            .mode = 1U,
            .level = 1U,
        };
        int late_rc = ipro_bt_gatt_security_levels_set(&late_requirement, 1U);

        IPRO_LOGI("GSL_TEST", "POST_ENABLE_SET %s rc=%d expected=%d",
                  (late_rc == -EBUSY) ? "PASS" : "FAIL", late_rc, -EBUSY);
    }
#endif

    ipro_ble_ctlr_hcitl_host_ready();
    bt_conn_cb_register(&s_ipro6s_conn_callbacks);
    bt_set_bondable(IS_ENABLED(CONFIG_BT_BONDABLE));
#if defined(CONFIG_IPRO_BT_HOST_GATT_SECURITY_POLICY_TEST)
    err = bt_conn_auth_cb_register(&s_ipro6s_policy_auth_callbacks);
    IPRO_LOGI("GSL_TEST", "AUTH_CB_REGISTER rc=%d", err);
    if (err != 0) {
        ipro6s_bringup_status.host_pairing_last_error = (uint32_t)(-err);
    }
#endif
    err = bt_conn_auth_info_cb_register(&s_ipro6s_auth_info_callbacks);
    if (err != 0) {
        ipro6s_bringup_status.host_pairing_last_error = (uint32_t)(-err);
    }
#if defined(CONFIG_BT_SETTINGS)
    err = settings_load_subtree("bt");
    ipro6s_bringup_status.host_settings_load_rc = err;
    if (err != 0) {
        printf("[BTDM_SETTINGS] load bt failed rc=%d\r\n", err);
        ipro6s_bringup_status.host_state = IPRO6S_HOST_FAIL;
        return;
    }
    ipro6s_refresh_bond_counts();
    printf("[BTDM_SETTINGS] loaded le/br bonds=%lu/%lu\r\n",
           (unsigned long)ipro6s_bringup_status.host_le_bond_count,
           (unsigned long)ipro6s_bringup_status.host_br_bond_count);
#else
    ipro6s_bringup_status.host_settings_load_rc = -ENOTSUP;
#endif
#if defined(CONFIG_BT_BREDR)
    int sdp_err;
    int rfcomm_err;

    ipro6s_bringup_status.host_br_echo_register_rc =
        bt_l2cap_br_echo_cb_register(&s_ipro6s_br_echo_callbacks);
    rfcomm_err = bt_rfcomm_server_register(&s_ipro6s_rfcomm_server);
    sdp_err = bt_sdp_register_service(&s_ipro6s_spp_record);
    printf("[BTDM_RFCOMM] register channel=%u rc=%d sdp_rc=%d\r\n",
           s_ipro6s_rfcomm_server.channel, rfcomm_err, sdp_err);
#if defined(CONFIG_BT_A2DP_SINK)
    s_ipro6s_a2dp_register_cb_rc =
        bt_a2dp_register_cb(&s_ipro6s_a2dp_callbacks);
    s_ipro6s_a2dp_register_ep_rc =
        bt_a2dp_register_ep(&s_ipro6s_a2dp_sink_ep,
                            BT_AVDTP_AUDIO, BT_AVDTP_SINK);
    s_ipro6s_a2dp_register_sdp_rc =
        bt_sdp_register_service(&s_ipro6s_a2dp_sink_record);
    printf("[BTDM_A2DP] sink register cb/ep/sdp=%ld/%ld/%ld seid=%u\r\n",
           (long)s_ipro6s_a2dp_register_cb_rc,
           (long)s_ipro6s_a2dp_register_ep_rc,
           (long)s_ipro6s_a2dp_register_sdp_rc,
           s_ipro6s_a2dp_sink_ep.sep.sep_info.id);
#endif
#if defined(CONFIG_BT_AVRCP)
    s_ipro6s_avrcp_ct_register_rc =
        bt_avrcp_ct_register_cb(&s_ipro6s_avrcp_ct_callbacks);
    s_ipro6s_avrcp_tg_register_rc =
        bt_avrcp_tg_register_cb(&s_ipro6s_avrcp_tg_callbacks);
    printf("[BTDM_AVRCP] register ct/tg=%ld/%ld\r\n",
           (long)s_ipro6s_avrcp_ct_register_rc,
           (long)s_ipro6s_avrcp_tg_register_rc);
#endif
#if defined(CONFIG_BT_HFP_HF)
    #if defined(CONFIG_BTDM_VOICE_OVER_HCI)
    s_ipro6s_sco_data_register_rc =
        ipro_bt_hci_sco_rx_register(ipro6s_sco_data_received, NULL);
    #endif
    s_ipro6s_hfp_hf_register_rc =
        bt_hfp_hf_register(&s_ipro6s_hfp_hf_callbacks);
    printf("[BTDM_HFP_HF] register=%ld sco_data=%ld\r\n",
           (long)s_ipro6s_hfp_hf_register_rc,
#if defined(CONFIG_BTDM_VOICE_OVER_HCI)
           (long)s_ipro6s_sco_data_register_rc);
#else
           (long)-ENOTSUP);
#endif
#endif
#endif
#if defined(CONFIG_BTDM_HOST_LE_ADV) && \
    defined(CONFIG_BTDM_HOST_LE_ADV_DEFAULT_ON)
    err = ipro6s_adv_start();
    ipro6s_bringup_status.host_adv_rc = err;
    ipro6s_bringup_status.host_state =
        (err == 0) ? IPRO6S_HOST_ADV : IPRO6S_HOST_FAIL;
#else
    ipro6s_bringup_status.host_adv_rc = 0;
    ipro6s_bringup_status.host_state = IPRO6S_HOST_ADV;
    printf("[BTDM_HOST] LE advertising disabled by APP policy\r\n");
#endif
#if defined(CONFIG_BT_BREDR)
    /* Apply BR/EDR Inquiry/Page Scan after LE advertising.  Both protocols
     * share the IPRO6S scheduler, so the last policy programmed at boot must
     * be the dual-mode discoverable state rather than LE-only advertising. */
    ipro6s_bringup_status.host_br_connectable_rc = 0;
    ipro6s_bringup_status.host_br_discoverable_rc =
        ipro6s_host_set_br_discoverable(true);
    printf("[BTDM_HOST] BR/EDR discoverable=%ld name=%s cod=0x%06x\r\n",
           (long)ipro6s_bringup_status.host_br_discoverable_rc,
           CONFIG_BT_DEVICE_NAME, (unsigned int)CONFIG_BT_COD);
#endif
}

static void ipro6s_host_bringup_start(void)
{
    int err;

    ipro6s_bringup_status.host_state = IPRO6S_HOST_RUNNING;
    ipro6s_bringup_status.host_adv_restart_rc = INT32_MIN;
#if defined(CONFIG_IPRO_BT_HOST_GATT_SECURITY_TEST) && \
    defined(CONFIG_BT_GATT_SECURITY_LEVELS)
    {
#if defined(CONFIG_IPRO_BT_HOST_GATT_SECURITY_POLICY_TEST)
        static const struct ipro_bt_gatt_security_requirement requirements[] = {
            {.mode = 1U, .level = 4U},
        };
#else
        static const struct ipro_bt_gatt_security_requirement requirements[] = {
            {.mode = 1U, .level = 4U},
            {.mode = 2U, .level = 2U},
        };
#endif

        err = ipro_bt_gatt_security_levels_set(
            requirements, sizeof(requirements) / sizeof(requirements[0]));
#if defined(CONFIG_IPRO_BT_HOST_GATT_SECURITY_POLICY_TEST)
        IPRO_LOGI("GSL_TEST", "PRE_ENABLE_SET %s rc=%d data=0104",
                  (err == 0) ? "PASS" : "FAIL", err);
#else
        IPRO_LOGI("GSL_TEST", "PRE_ENABLE_SET %s rc=%d data=01040202",
                  (err == 0) ? "PASS" : "FAIL", err);
#endif
        if (err != 0) {
            ipro6s_bringup_status.host_enable_rc = err;
            ipro6s_bringup_status.host_state = IPRO6S_HOST_FAIL;
            return;
        }
    }
#endif
    err = hci_driver_ipro_ble_ctlr_init();
    if (err == 0) {
        err = bt_enable(ipro6s_host_ready);
    }
    if (err != 0) {
        ipro6s_bringup_status.host_enable_rc = err;
        ipro6s_bringup_status.host_state = IPRO6S_HOST_FAIL;
    }
}
#endif

#if defined(CONFIG_BTDM_CCI_DIAG)
static void ipro6s_scci_pinmux_init(void)
{
    /* IPRO6S fixed S-CCI pads: GPIO10=CLK, 11=CS, 12=DI, 13=DO.
     * GPIO_CFG[N] starts at 0x300008c4 and the bench-proven S-CCI pad value is
     * 0x00400f03.  Keep this in the bring-up APP instead of changing the BSP or
     * controller library defaults because customers may use these GPIOs. */
    static const uintptr_t gpio_cfg[] = {
        0x300008ecu, 0x300008f0u, 0x300008f4u, 0x300008f8u,
    };

    for (size_t i = 0; i < sizeof(gpio_cfg) / sizeof(gpio_cfg[0]); i++) {
        *(volatile uint32_t *)gpio_cfg[i] = 0x00400f03u;
    }
}
#endif

int main(void)
{
    bringup_stage_set(1);
#if defined(CONFIG_BTDM_CCI_DIAG)
    /* Establish the diagnostic path before platform_init().  If platform
     * bring-up itself stalls, CCI can still retrieve the stage-1 breadcrumb.
     * Re-apply it afterwards in case generic board setup touched the pads. */
    ipro6s_scci_pinmux_init();
#endif
    platform_init(0);
    bringup_stage_set(2);

#if defined(CONFIG_BTDM_CCI_DIAG)
    ipro6s_scci_pinmux_init();
#endif
    bringup_stage_set(3);

    /* Disable stdout buffering to ensure printf output is immediate */
    setvbuf(stdout, NULL, _IONBF, 0);

    puts("\r\n=== STDIO OK ===\r\n");
    printf("[BTDM_CLK] sys_cfg0=%08lx sys_cfg1=%08lx glb_bz_2=%08lx "
           "glb_bz_3=%08lx wlp1_2=%08lx wlp1_3=%08lx "
           "ble_clk_sel=16 hclk_mhz=%lu bclk_mhz=%lu bclk_ratio_x100=%lu\r\n",
           (unsigned long)*(volatile uint32_t *)0x30000090u,
           (unsigned long)*(volatile uint32_t *)0x30000094u,
           (unsigned long)*(volatile uint32_t *)0x200003e0u,
           (unsigned long)*(volatile uint32_t *)0x300003e0u,
           (unsigned long)*(volatile uint32_t *)0x20001400u,
           (unsigned long)*(volatile uint32_t *)0x30001400u,
           (unsigned long)(Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_CLK) /
                           1000000u),
           (unsigned long)(Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_PBCLK) /
                           1000000u),
           (unsigned long)(Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_PBCLK) /
                           160000u));

    setup_heap();
#if defined(CONFIG_IPRO_LOG_ENABLE)
    /* This bring-up entry point owns platform initialization instead of
     * calling ipro_platform_init(), including the structured logger. */
    ipro_log_init(NULL);
#endif
    bringup_stage_set(4);

    printf("[BTDM] IPRO6 BTDM Demo Starting...\r\n");

#if defined(CONFIG_BTDM_ENABLE) || defined(CONFIG_IPRO6_CLASSIC_BRINGUP)
#if defined(CONFIG_PHY_CLI) && !defined(CONFIG_FPGA)
    /* Initialize the common ASIC RF before rwip_init()/HCI Reset, then retain
     * ownership for BZ because this bring-up app has no WLAN traffic. These
     * are the same board defaults used by the proven `phy_init` CLI. */
    printf("[BTDM] Initializing IPRO6S RF for BZ...\r\n");
    cmd_init_do(40000000U, 32U);
    rf_bz_enter();
    bz_phy_reset();
    printf("[BTDM] IPRO6S RF ready, BZ owns shared front-end\r\n");
#endif

#ifdef CONFIG_BTDM_ENABLE
    /* Initialize BTDM controller */
    printf("[BTDM] Initializing BTDM Controller...\r\n");
    bringup_stage_set(5);
    int ret = btdm_init(NULL);
    ipro6s_bringup_status.controller_init_rc = ret;
    if (ret != 0) {
        printf("[BTDM] BTDM init failed: %d\r\n", ret);
    } else {
        printf("[BTDM] BTDM Controller initialized\r\n");
        printf("[BTDM_PWR_BASE] deep=%08lx/%08lx/%08lx cgen=%08lx "
               "wlp1=%08lx\r\n",
               (unsigned long)*(volatile uint32_t *)0x28000030u,
               (unsigned long)*(volatile uint32_t *)0x28000430u,
               (unsigned long)*(volatile uint32_t *)0x28000830u,
               (unsigned long)*(volatile uint32_t *)0x30000580u,
               (unsigned long)*(volatile uint32_t *)0x30001400u);
        printf("[BTDM_SRAM_BASE] %08lx/%08lx/%08lx/%08lx "
               "%08lx/%08lx/%08lx/%08lx\r\n",
               (unsigned long)*(volatile uint32_t *)0x30000600u,
               (unsigned long)*(volatile uint32_t *)0x30000604u,
               (unsigned long)*(volatile uint32_t *)0x30000608u,
               (unsigned long)*(volatile uint32_t *)0x3000060cu,
               (unsigned long)*(volatile uint32_t *)0x30000610u,
               (unsigned long)*(volatile uint32_t *)0x30000614u,
               (unsigned long)*(volatile uint32_t *)0x30000618u,
               (unsigned long)*(volatile uint32_t *)0x3000061cu);
        printf("[BTDM_PDS_BASE] %08lx/%08lx/%08lx/%08lx\r\n",
               (unsigned long)*(volatile uint32_t *)0x3008e000u,
               (unsigned long)*(volatile uint32_t *)0x3008e010u,
               (unsigned long)*(volatile uint32_t *)0x3008e014u,
               (unsigned long)*(volatile uint32_t *)0x3008e018u);
    }
    bringup_stage_set(6);
#endif

#if defined(CONFIG_IPRO6_CLASSIC_BRINGUP)
    printf("[BTDM] Initializing isolated BR/EDR + BLE controller...\r\n");
    bringup_stage_set(5);
    btble_controller_init(configMAX_PRIORITIES - 1);
    ipro6s_bringup_status.controller_init_rc = 0;
    bringup_stage_set(6);
#endif
#endif

    printf("[OS] Starting main_loop task...\r\n");

    xTaskCreateStatic(main_loop_proc, (char *)"main_loop",
                      sizeof(main_loop_proc_stack) / 4, NULL, 10,
                      main_loop_proc_stack, &main_loop_proc_task);
    bringup_stage_set(7);

    printf("[OS] Starting OS Scheduler...\r\n");
    bringup_stage_set(8);
    vTaskStartScheduler();

    /* Should never reach here */
    return 0;
}

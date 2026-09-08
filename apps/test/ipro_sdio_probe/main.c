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
 * SDIO host stack smoke test (Phase 0 hal_sdio verification).
 *
 * Targets the AIC8800D80 module (Wi-Fi 6 / BT5.0 SoC) connected over SDIO via
 * the IPRO7 SDH controller. The probe stops short of firmware download (Phase 1
 * work); its job is to prove that the SDIO host stack can:
 *
 *   1. Power-cycle the AIC module via GPIO control
 *   2. Drive CMD0/8/5/3/7 enumeration
 *   3. Read CCCR + per-function CIS over CMD52
 *   4. Switch to 4-bit / 25 MHz, set block size, enable F1
 *   5. Apply D80-specific bring-up quirks (FN0[0xF2]=0x7F, etc.)
 *
 * Steps 1-4 are generic and come from components/network/sdio_host_probe along
 * with the whole sdio_* shell table. What stays here is step 5: the D80 quirks,
 * hung off the component's on_post_enable callback so `sdio_probe` applies them
 * in the right place, plus an `aic_quirks` command to re-apply them alone.
 *
 * Pin assignment (per docs/CODEMAPS/aic8800d80_hw_bringup.md):
 *   GPIO28..33 = SDH bus (CMD/CLK/DAT0..3)            [GPIO_FUN_SDC]
 *   GPIO16     = AIC POWER_EN (active high)           [PROPOSED]
 *   GPIO17     = AIC RESET#   (active low)            [PROPOSED]
 *   GPIO18     = AIC WAKE     (host -> AIC, optional) [PROPOSED]
 *
 * The aux pin assignment must be confirmed against the production board before
 * final integration - the macros below isolate that risk, and `sdio_pins`
 * overrides them at runtime.
 *
 * Shell: the generic sdio_* table (see the component's README) plus:
 *   aic_quirks                      re-apply the D80 bring-up registers
 */

#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

#include "shell.h"
#include "platform_common.h"
#include "hal_boot2.h"
#include "hal_uart.h"
#include "freertos_app_hooks.h"

#include "drv_glb.h"
#include "sdio_probe.h"

/* AIC POWER/RESET pin assignment - proposed; HW must confirm. */
#define AIC_PIN_POWER_EN        GLB_GPIO_PIN_16
#define AIC_PIN_RESET_N         GLB_GPIO_PIN_17
#define AIC_PIN_WAKE            GLB_GPIO_PIN_18

/* AIC8800D80 SDIO configuration (from docs/CODEMAPS/aic8800d80_porting.md). */
#define AIC_FUNC_DATA           SDIO_FUNC_1
#define AIC_FUNC_BLOCK_SIZE     512
#define AIC_QUIRK_FN0_F2        0xF2U
#define AIC_QUIRK_FN0_F2_VAL    0x7FU
#define AIC_REG_BYTEMODE_ENABLE 0x07U
#define AIC_REG_INTR_CONFIG     0x00U
#define AIC_FN0_INTR_EN         0x04U

/*
 * D80 bring-up quirks (porting.md 1.2). The AIC chip needs:
 *   - FN0 register 0xF2 = 0x7F  (purpose undocumented, copy verbatim)
 *   - F1  register 0x07 = 0x01  (force block-only mode, V3 layout)
 *   - FN0 register 0x04 = 0x07  (master IRQ enable, including bit 2)
 *   - F1  register 0x00 = 0x07  (per-func IRQ enable: read/write/misc)
 *
 * Run as the component's on_post_enable hook: after IO Enable of F1, before the
 * sequence reports success. Failures are reported but not fatal - a quirk the
 * silicon rejects is itself the finding, and the rest of the dump is still
 * worth seeing.
 */
static int aic_apply_quirks(SDIO_Host_Type *host, void *arg)
{
    (void)arg;

    SDIO_Status_Type st;

    st = sdio_writeb(host, SDIO_FUNC_0, AIC_QUIRK_FN0_F2, AIC_QUIRK_FN0_F2_VAL);
    printf("  FN0[0xF2]=0x7F (D80 quirk) -> %d\r\n", (int)st);
    st = sdio_writeb(host, AIC_FUNC_DATA, AIC_REG_BYTEMODE_ENABLE, 0x01);
    printf("  F1[0x07]=0x01  (block-only) -> %d\r\n", (int)st);
    st = sdio_writeb(host, SDIO_FUNC_0, AIC_FN0_INTR_EN, 0x07);
    printf("  FN0[0x04]=0x07 (master irq) -> %d\r\n", (int)st);
    st = sdio_writeb(host, AIC_FUNC_DATA, AIC_REG_INTR_CONFIG, 0x07);
    printf("  F1[0x00]=0x07  (per-func irq) -> %d\r\n", (int)st);
    return 0;
}

static int cmd_aic_quirks(int argc, char **argv)
{
    (void)argc; (void)argv;

    SDIO_Host_Type *host = sdio_probe_host();
    if (host == NULL) {
        printf("run sdio_init + sdio_enum first\r\n");
        return -1;
    }
    return aic_apply_quirks(host, NULL);
}

SHELL_CMD_EXPORT_ALIAS(cmd_aic_quirks, aic_quirks, "Re-apply the D80 bring-up registers");

/* ------------------------------------------------------------------ */
/* Main                                                                */
/* ------------------------------------------------------------------ */

static void main_task(void *arg)
{
    (void)arg;

    sdio_probe_config_t cfg;
    sdio_probe_config_default(&cfg);
    cfg.pin_power_en = AIC_PIN_POWER_EN;
    cfg.pin_reset_n  = AIC_PIN_RESET_N;
    cfg.pin_wake     = AIC_PIN_WAKE;
    cfg.data_func    = AIC_FUNC_DATA;
    cfg.block_size   = AIC_FUNC_BLOCK_SIZE;

    const sdio_probe_callbacks_t cb = { .on_post_enable = aic_apply_quirks };
    sdio_probe_shell_bind(&cfg, &cb);

    printf("\r\n=========================================\r\n");
    printf("  ipro_sdio_probe - AIC8800D80 bring-up\r\n");
    printf("=========================================\r\n");
    printf("Try: sdio_probe   (full sequence, D80 quirks included)\r\n");
    printf("     sdio_init / sdio_pwr / sdio_reset / sdio_enum / ...\r\n");
    printf("     aic_quirks   (re-apply the D80 registers alone)\r\n");
    printf("=========================================\r\n");

    shell_init_with_task(UART0_INDEX);
    vTaskDelete(NULL);
}

int main(void)
{
    ipro_setup_heap();
    platform_init(0);
    hal_boot2_init();

    xTaskCreate(main_task, "main", 4096, NULL,
                configMAX_PRIORITIES - 1, NULL);
    vTaskStartScheduler();

    while (1) ;
    return 0;
}

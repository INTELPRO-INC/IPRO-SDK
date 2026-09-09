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

#include "usbwifi.h"
#include "utils.h"

#if defined(IPRO6LE) || defined(IPRO6)
#if defined(IPRO6LE) || defined(IPRO6)
#include "drv_l1c.h"
#else
#include <bl616_l1c.h>
#endif
#endif

static void in_cb(void *arg, size_t len)
{
    usbwifi_dc_t *dc = arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(dc->tx_sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void out_cb(void *arg, size_t len)
{
    usbwifi_dc_t *dc = arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    dc->len_read = len;
    xSemaphoreGiveFromISR(dc->rx_sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

int usbwifi_dc_init(usbwifi_dc_t *dc, struct usbwifi_cdc_env_tag *cdc_env)
{
    dc->cdc_env = cdc_env;

    L1C_DCache_Clean_Invalid_By_Addr((uintptr_t)dc->txrx_buffer, sizeof(dc->txrx_buffer));
    dc->tx_buf = (void *)MAWOC(dc->txrx_buffer[0]);
    dc->rx_buf = (void *)MAWOC(dc->txrx_buffer[1]);
    dc->rx_mutex = xSemaphoreCreateMutexStatic(&dc->rx_mutex_buffer);
    dc->rx_sem = xSemaphoreCreateBinaryStatic(&dc->rx_sem_buffer);
    dc->tx_mutex = xSemaphoreCreateMutexStatic(&dc->tx_mutex_buffer);
    dc->tx_sem = xSemaphoreCreateBinaryStatic(&dc->tx_sem_buffer);

    usbwifi_register_cdc_cb(dc->cdc_env, in_cb, dc, out_cb, dc);
    return 0;
}

int usbwifi_dc_write(usbwifi_dc_t *dc, const void *data, size_t len, unsigned timeout_ms)
{
    int ret = 0;

    if (!(dc && data && len)) {
        return -1;
    }
    if (len > USBWIFI_DATA_CHANNEL_BUF_LEN) {
        return -1;
    }

    if (xSemaphoreTake(dc->tx_mutex, pdMS_TO_TICKS(timeout_ms)) != pdPASS) {
        return 1;
    }
    memcpy(dc->tx_buf, data, len);
    usb_cdc_bulk_in(dc->cdc_env, dc->tx_buf, len);
    if (xSemaphoreTake(dc->tx_sem, pdMS_TO_TICKS(timeout_ms))) {
        ret = 1;
    }
    xSemaphoreGive(dc->tx_mutex);

    return ret;
}

int usbwifi_dc_read(usbwifi_dc_t *dc, void *data, size_t len, size_t *len_read, unsigned timeout_ms)
{
    int ret = 0;

    if (!(dc && data && len && len_read)) {
        return -1;
    }

    if (xSemaphoreTake(dc->rx_mutex, pdMS_TO_TICKS(timeout_ms)) != pdPASS) {
        return 1;
    }
    usb_cdc_bulk_out(dc->cdc_env, dc->rx_buf, USBWIFI_DATA_CHANNEL_BUF_LEN);
    if (xSemaphoreTake(dc->rx_sem, pdMS_TO_TICKS(timeout_ms)) != pdPASS) {
        ret = 1;
        goto exit;
    }
    if (dc->len_read > len) {
        ret = 2;
        goto exit;
    }
    memcpy(data, dc->rx_buf, dc->len_read);
    *len_read = dc->len_read;
exit:
    xSemaphoreGive(dc->rx_mutex);
    return ret;
}

static void test_task(void *parm)
{
    usbwifi_dc_t *dc = parm;
    char buf[512];
    size_t len_read = 0;
    int ret;

    while (1) {
        ret = usbwifi_dc_read(dc, buf, 512, &len_read, 100);
        if (ret == 0) {
            ret = usbwifi_dc_write(dc, buf, len_read, 100);
        }
    }
}

void usbwifi_dc_test(usbwifi_dc_t *dc)
{
    xTaskCreate(test_task, "t_usbwifi_dc", 1024, dc, 20, NULL);
}

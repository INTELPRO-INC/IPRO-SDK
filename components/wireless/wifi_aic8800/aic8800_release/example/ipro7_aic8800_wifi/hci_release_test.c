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

/* Release integration smoke test, application side only; not in driver .a. */
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "shell.h"
#include "aic_hci_mailbox.h"
#include "aic_hci_transport.h"
struct response { int status; uint16_t len; uint8_t data[260]; };
static QueueHandle_t responses;
static TaskHandle_t smoke_task;
void aic_release_smoke_register(void) {} /* Pull shell command from app archive. */
static void receive(void *ctx,uint8_t type,const uint8_t *p,uint16_t n)
{
    struct response r;
    (void)ctx;
    if (type!=AIC_HCI_EVENT || n>sizeof(r.data) || (p[0]!=0x0e && p[0]!=0x0f)) return;
    r.status=0; r.len=n; memcpy(r.data,p,n);
    if (xQueueSend(responses,&r,0)!=pdTRUE) puts("[release-hci] response overflow");
}
static void failed(void *ctx,int status)
{
    struct response r={0}; (void)ctx; r.status=status;
    (void)xQueueSend(responses,&r,0);
}
static void submitted(void *ctx,uint32_t token,int status)
{ (void)token; if (status) failed(ctx,status); }
static void smoke(void *arg)
{
    static const uint16_t ops[]={0x0c03,0x1001,0x1003,0x1005,0x1009};
    struct aic_hci_callbacks cb={receive,submitted,failed};
    struct response r;
    struct aic_hci_stats stats;
    unsigned int i;
    int ret=-1, attached=0;
    (void)arg;
    responses=xQueueCreate(4,sizeof(r));
    if (!responses || aic_btsdio_init()) goto done;
    ret=aic_hci_transport_attach(&cb,NULL,AIC_HCI_FLOW_HOST);
    if (ret) goto done;
    attached=1;
    for (i=0;i<sizeof(ops)/sizeof(ops[0]);i++) {
        uint8_t cmd[]={ops[i]&255,ops[i]>>8,0};
        ret=aic_hci_transport_send(AIC_HCI_COMMAND,cmd,3,i,3000);
        if (ret) break;
        if (xQueueReceive(responses,&r,pdMS_TO_TICKS(4000))!=pdTRUE) { ret=-3; break; }
        if (r.status) { ret=r.status; break; }
        if (r.len<6 || r.data[0]!=0x0e || r.data[3]!=cmd[0] || r.data[4]!=cmd[1] || r.data[5]) { ret=-1; break; }
        printf("[release-hci] opcode=0x%04x status=0x00 len=%u\r\n",ops[i],r.len);
        if (ops[i]==0x1003 && r.len>=14) {
            unsigned int j;
            printf("[release-hci] features:");
            for (j=6;j<14;j++) printf(" %02x",r.data[j]);
            puts("");
            if (r.data[10]&0x20) { ret=-1; break; }
        }
    }
done:
    if (attached) {
        aic_hci_transport_stats(&stats);
        printf("[release-hci] tx=%lu rx=%lu queued=%u fault=%u\r\n",(unsigned long)stats.tx_packets,(unsigned long)stats.rx_packets,stats.queued,stats.faulted);
        if (aic_hci_transport_detach()) ret=-1;
    }
    /* No more callbacks after detach. */
    if (responses) { vQueueDelete(responses); responses=NULL; }
    printf("[release-hci] %s ret=%d (transport only, not A2DP)\r\n",ret ? "FAIL" : "PASS",ret);
    smoke_task=NULL;
    vTaskDelete(NULL);
}
static int hci_info(int argc,char **argv)
{
    (void)argc; (void)argv;
    if (smoke_task) return -1;
    return xTaskCreate(smoke,"hci_info",6144,NULL,2,&smoke_task)==pdPASS ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(hci_info,hci_info,"After aic_start: release HCI transport smoke test (BT Reset)");

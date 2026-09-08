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
#include "sccb.h"
#include "camera.h"

extern int cam_desc_start;
extern int cam_desc_end;

const rt_camera_desc* camera_module_init(void)
{
    const rt_camera_desc *desc;
    uint8_t camera_address = 0;
    int camera_found = 0;

    for (desc = (rt_camera_desc *)&cam_desc_start;
            desc != (rt_camera_desc *)&cam_desc_end;
            ++desc) {
        printf("[camera_module_init] Try matching the camera module [%p, %s, 0x%02x]\r\n", desc, desc->name, desc->addr);
        camera_address = SCCB_Scan(desc->addr, desc->addr);
        if ((desc->addr == camera_address) && (desc->probe() == 0)) {
            printf("[camera_module_init] Resetting camera module [%p, %s, 0x%02x]\r\n", desc, desc->name, desc->addr);
            desc->reset();
            camera_found = 1;
            break;
        }
    }
    if (!camera_found) {
        printf("[camera_module_init] No supported camera found\r\n");
        return NULL;
    }
    return desc;
}

void camera_module_start(const rt_camera_desc *desc)
{
    if (desc->setFMT)
        desc->setFMT();
    else
        desc->reset();
}


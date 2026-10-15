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

#ifndef __RTSP_METHOD_H__
#define __RTSP_METHOD_H__


#include "list.h"


/* SDP information. */
struct sdp_info {
    struct list_head entry;          /* Entry of sdp list. */
    char type;                  /* Ascii code, type of sdp info. */
    char *value;                /* String, value of sdp info. */
};


void handle_method_options(struct rtsp_sess *sessp);
void handle_method_describe(struct rtsp_sess *sessp);
void handle_method_setup(struct rtsp_sess *sessp);
void handle_method_play(struct rtsp_sess *sessp);
void handle_method_pause(struct rtsp_sess *sessp);
void handle_method_get_parameter(struct rtsp_sess *sessp);
void handle_method_set_parameter(struct rtsp_sess *sessp);
void handle_method_teardown(struct rtsp_sess *sessp);

int handle_rtsp_req(struct rtsp_sess *sessp);



#endif /* __RTSP_METHOD_H__ */

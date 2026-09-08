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

#ifndef __UTILS_TIME_H__
#define __UTILS_TIME_H__
typedef struct {
    unsigned char ntp_hour;
    unsigned char ntp_minute;
    unsigned char ntp_second;
    unsigned char ntp_week_day;
    unsigned char ntp_date;
    unsigned char ntp_month;
    unsigned char leap_days;
    unsigned char leap_year_ind;
    unsigned int ntp_year;
    unsigned int days_since_epoch;
    unsigned int day_of_year; 
} utils_time_date_t;

int utils_time_date_from_epoch(unsigned int epoch, utils_time_date_t *date);

#endif

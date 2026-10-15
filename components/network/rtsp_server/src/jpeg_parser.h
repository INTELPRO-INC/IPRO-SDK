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

#ifndef __PARSER_H__
#define __PARSER_H__


enum
{
    START_MARKER = 0xFF,
    SOI_MARKER   = 0xD8,
    JFIF_MARKER  = 0xE0,
    CMT_MARKER   = 0xFE,
    DQT_MARKER   = 0xDB,
    SOF_MARKER   = 0xC0,
    DHT_MARKER   = 0xC4,
    SOS_MARKER   = 0xDA,
    EOI_MARKER   = 0xD9,
    DRI_MARKER   = 0xDD
};
 
typedef struct
{
    unsigned char id;
    unsigned char samp;
    unsigned char qt;
} CompInfo;





/* Forward decl — struct jpegframe_parser defined in rtp.h.
 * Explicit prototype for the strict-warning build (no implicit
 * function declarations). */
struct jpegframe_parser;
int parse_jpeg_req(unsigned char* data, unsigned int size,
                   struct jpegframe_parser* jpeg_parser);

#endif /* __PARSER_H__ */

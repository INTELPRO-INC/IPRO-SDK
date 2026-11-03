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

/**
 * @file ipro_ai_devinfo.h
 * @brief Replies of the device protocol's V2.0 queries (docs/edge_ai/06 §5),
 *        built once for the three Teach links.
 *
 * Each function writes one reply line - the command word, then key=value
 * tokens - without the newline, and returns its length as snprintf does.
 * The word is the link's own ("VER" on the text tools, "TVER" on the image
 * one). Values never contain spaces.
 */
#ifndef IPRO_AI_DEVINFO_H
#define IPRO_AI_DEVINFO_H

#include <stdint.h>

#include "ipro_ai.h"
#include "ipro_ai_store.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IPRO_AI_PLATFORM_VERSION "2.0"
#define IPRO_AI_PROTO_VERSION    2u
#define IPRO_AI_FW_VERSION       "2.0.0"

/** "<word> platform=2.0 proto=2 abi=1 store=1 tool=<tool> fw=<fw> ver=2.0.0 chip=IPRO7AI" */
int ipro_ai_devinfo_ver(char *buf, uint32_t size, const char *word, const char *tool, const char *fw);

/** "<word> kind=<tool> room=<bytes> banks=<n> modes=<list|-> feat=<list>" */
int ipro_ai_devinfo_caps(char *buf, uint32_t size, const char *word, const char *tool, uint32_t room,
                         uint32_t banks, const char *modes, const char *feat);

/**
 * "<word> state=<committed|v11|none> bytes= crc= seq= bank= fmt= [why=] [name=]
 *  [in=WxHxC/pitch/bytes/<ob|u8> out=WxHxC/pitch/bytes scale=]"
 * found: what the store reported at boot; m: the loaded model (NULL if none);
 * in_scale: the input scale from the payload's metadata (0: not reported).
 */
int ipro_ai_devinfo_model(char *buf, uint32_t size, const char *word, const ipro_ai_blob_t *found,
                          const ipro_ai_model_t *m, const char *name, float in_scale);

/**
 * Run m up to n times (1..1000; it stops after 2 s, since callers hold their
 * app's lock) on input (NULL: what is in its input buffer) and format
 * "<word> n=<runs made> avg_us= min_us= max_us= [pre_us=] heap_free=
 * heap_min= lock_waits=". pre_us is left out when UINT32_MAX. "<word> ERR
 * failed" when a run fails, "<word> ERR size" for n out of range.
 */
int ipro_ai_devinfo_bench(char *buf, uint32_t size, const char *word, ipro_ai_model_t *m, uint32_t n,
                          const void *input, uint32_t pre_us);

/** The store's reason for an empty slot as a protocol token
 *  (empty, install_interrupted, crc, too_big, no_xip, no_slot, other). */
const char *ipro_ai_devinfo_why(const char *store_why);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_AI_DEVINFO_H */

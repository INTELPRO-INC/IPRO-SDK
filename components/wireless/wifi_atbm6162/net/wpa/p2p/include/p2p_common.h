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


#ifndef P2P_COMMMON_H
#define P2P_COMMMON_H
#include "atbm_hal.h"

#include "stdlib.h"
#include "p2p_defs.h"
#include "p2p_debug.h"
#include "wpabuf.h"

struct os_reltime {
	os_time_t sec;
	os_time_t usec;
};

#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

#define os_snprintf snprintf
#define os_strlen strlen
#define os_memmove memmove
#define os_strncmp strncmp
#define os_strdup(s) atbm_strdup(s)
#define os_realloc realloc


typedef enum { ParseOK = 0, ParseUnknown = 1, ParseFailed = -1 } ParseRes;

static inline int os_reltime_before(struct os_reltime *a,
				    struct os_reltime *b)
{
	return 0;
}


static inline int os_reltime_expired(struct os_reltime *now,
				     struct os_reltime *ts,
				     os_time_t timeout_secs)
{
	return 0;
}


static inline void *os_calloc(atbm_size_t nmemb, atbm_size_t size)
{
	if (size && nmemb > (~(atbm_size_t) 0) / size)
		return NULL;
	return (void *)atbm_kzalloc(nmemb * size, GFP_KERNEL);
}

static inline int os_snprintf_error(atbm_size_t size, int res)
{
	return res < 0 || (unsigned int) res >= size;
}



const char * wpa_ssid_txt(const atbm_uint8 *ssid, atbm_size_t ssid_len);

atbm_size_t os_strlcpy(char *dest, const char *src, atbm_size_t siz);
void * atbm_memdup(const void *src, atbm_size_t len);
void *os_realloc_array(void *ptr, atbm_size_t nmemb, atbm_size_t size);

struct wpabuf * ieee802_11_vendor_ie_concat(const atbm_uint8 *ies, atbm_size_t ies_len,
					    atbm_uint32 oui_type);
int is_ctrl_char(char c);
int supp_rates_11b_only(struct atbmwifi_ieee802_11_elems *elems);
int freq_range_list_includes(const struct wpa_freq_range_list *list,
			     unsigned int freq);
atbm_size_t utf8_escape(const char *inp, atbm_size_t in_size,
		   char *outp, atbm_size_t out_size);



#endif


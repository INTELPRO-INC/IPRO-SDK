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
#include <stdlib.h>
#include <string.h>
#include "rfparam_rftlv.h"
#include "rfparam_adapter.h"
#include "softcrc.h"
#include "wl_api.h"

#include "drv_aon.h"
#include "drv_mfg_media.h"
#include "drv_hbn.h"

__unused static uint8_t g_rfparam_buf[RFPARAM_WL_API_MEM_SIZE] = {0};
static struct wl_cfg_t *g_rfparam_cfg = NULL;
static uint32_t g_tlv_base_addr;

void rfparam_array_printf(char* str, void * buf, uint32_t buf_len, int type)
{
    uint32_t i;
    uint8_t *a=NULL;
    int8_t *b=NULL;
    uint16_t *c=NULL;
    int16_t *d=NULL;

    rfparam_printf("%s[%ld]: ", str, buf_len);

    switch (type) {
    case TYPE_UINT8:
        a = (uint8_t *)buf;
        for (i = 0; i < buf_len; i++) {
            printf("%d,", (int)a[i]);
        }
        break;
    case TYPE_INT8:
        b = (int8_t *)buf;
        for (i = 0; i < buf_len; i++) {
            printf("%d,", (int)b[i]);
        }
        break;
    case TYPE_UINT16:
        c = (uint16_t *)buf;
        for (i = 0; i < buf_len; i++) {
            printf("%d,", (int)c[i]);
        }
        break;
    case TYPE_INT16:
        d = (int16_t *)buf;
        for (i = 0; i < buf_len; i++) {
            printf("%d,", (int)d[i]);
        }
        break;
    default:
        break;
    }
    printf("\r\n");
}

int32_t rfparam_get_wlan_pwroffset_with_option(uint32_t base_addr,int8_t pwr_offset[14],uint8_t reload)
{

    uint8_t pwrmode[RFTLV_MAXLEN_PWR_MODE+1]={0};
    int8_t pwroffset_tmp[RFTLV_MAXLEN_PWR_OFFSET]={0};

    /* pwr_offset_mode */
    if (rfparam_tlv_get(base_addr,RFTLV_TYPE_PWR_MODE, RFTLV_MAXLEN_PWR_MODE, pwrmode) > 0) {
        rfparam_printf("pwr_mode is %s\r\n", pwrmode);
    }else{
    	rfparam_printf("pwr_mode null\r\n");
    	return RFPARAM_ERR_PWR_MODE_NULL;
    }

    for (int i = 0; i < RFTLV_MAXLEN_PWR_MODE; i++) {
        switch (pwrmode[i]) {
        	/* read pwr offset from efuse */
			case 'B':
			case 'b':
			{
			    if(0 == mfg_media_read_hp_poweroffset_with_lock(pwroffset_tmp,reload)){
				    rfparam_array_printf((char *)"efuse wlan pwr_offset",pwroffset_tmp,sizeof(pwroffset_tmp),TYPE_INT8);
					if('B' == pwrmode[i]){
						memcpy(pwr_offset,pwroffset_tmp,sizeof(pwroffset_tmp));
						return RFPARAM_SUSS;
					}else if('b' == pwrmode[i]){
						for(int i = 0; i < sizeof(pwroffset_tmp); i++){
							pwr_offset[i] += pwroffset_tmp[i];
						}
					}
				}else{
					rfparam_printf("no pwr_offset in efuse\r\n");
				}

			}
			break;

			/* read pwr offset from rftlv */
            case 'F':
            case 'f':
            {
				if (rfparam_tlv_get(base_addr,RFTLV_TYPE_PWR_OFFSET, RFTLV_MAXLEN_PWR_OFFSET, pwroffset_tmp) > 0) {
				    for(int i = 0; i < 14; i++){
			            pwroffset_tmp[i] -= 16;
			        }
					rfparam_array_printf((char *)"tlv wlan pwr_offset",pwroffset_tmp,sizeof(pwroffset_tmp),TYPE_INT8);
					if('F' == pwrmode[i]){
						memcpy(pwr_offset,pwroffset_tmp,sizeof(pwroffset_tmp));
						return RFPARAM_SUSS;
					}else if('f' == pwrmode[i]){
						for(int i = 0; i < sizeof(pwroffset_tmp); i++){
							pwr_offset[i] += pwroffset_tmp[i];
						}
					}
				}else{
					rfparam_printf("tlv pwr_offset null\r\n");
				}
            }
            break;

            default:
            {
                rfparam_printf("not support mode\r\n");
            }
        }
    }

    return RFPARAM_SUSS;

}

int32_t rfparam_get_wlan_pwroffset_lp_with_option(uint32_t base_addr,int8_t pwr_offset[14],uint8_t reload)
{

    uint8_t pwrmode[RFTLV_MAXLEN_PWR_MODE+1]={0};
    int8_t pwroffset_tmp[RFTLV_MAXLEN_PWR_OFFSET]={0};

    /* pwr_offset_mode */
    if (rfparam_tlv_get(base_addr,RFTLV_TYPE_PWR_MODE, RFTLV_MAXLEN_PWR_MODE, pwrmode) > 0) {
    }else{
    	return RFPARAM_ERR_PWR_MODE_NULL;
    }

    for (int i = 0; i < RFTLV_MAXLEN_PWR_MODE; i++) {
        switch (pwrmode[i]) {
        	/* read pwr offset from efuse */
			case 'B':
			case 'b':
			{
			    if(0 == mfg_media_read_lp_poweroffset_with_lock(pwroffset_tmp,reload)){
				    rfparam_array_printf((char *)"efuse wlan lp pwr_offset",pwroffset_tmp,sizeof(pwroffset_tmp),TYPE_INT8);
					if('B' == pwrmode[i]){
						memcpy(pwr_offset,pwroffset_tmp,sizeof(pwroffset_tmp));
						return RFPARAM_SUSS;
					}else if('b' == pwrmode[i]){
						for(int i = 0; i < sizeof(pwroffset_tmp); i++){
							pwr_offset[i] += pwroffset_tmp[i];
						}
					}
				}else{
					rfparam_printf("no lp pwr_offset in efuse\r\n");
				}

			}
			break;

			/* read pwr offset from rftlv */
            case 'F':
            case 'f':
            {
				if (rfparam_tlv_get(base_addr,RFTLV_TYPE_PWR_OFFSET_LP, RFTLV_MAXLEN_PWR_OFFSET_LP, pwroffset_tmp) > 0) {
				    for(int i = 0; i < 14; i++){
			            pwroffset_tmp[i] -= 16;
			        }
					rfparam_array_printf((char *)"tlv wlan lp pwr_offset",pwroffset_tmp,sizeof(pwroffset_tmp),TYPE_INT8);
					if('F' == pwrmode[i]){
						memcpy(pwr_offset,pwroffset_tmp,sizeof(pwroffset_tmp));
						return RFPARAM_SUSS;
					}else if('f' == pwrmode[i]){
						for(int i = 0; i < sizeof(pwroffset_tmp); i++){
							pwr_offset[i] += pwroffset_tmp[i];
						}
					}
				}else{
					rfparam_printf("tlv lp pwr_offset null\r\n");
				}
            }
            break;

            default:
            {
                rfparam_printf("not support mode\r\n");
            }
        }
    }

    return RFPARAM_SUSS;

}

int32_t rfparam_get_bz_pwroffset_with_option(uint32_t base_addr,int8_t pwr_offset[5],uint8_t reload)
{

    uint8_t pwrmode[RFTLV_MAXLEN_PWR_MODE+1] = {0};
    int8_t pwroffset_tmp[RFTLV_MAXLEN_BLE_PWR_OFFSET] = {0};

    /* pwr_offset_mode */
    if (rfparam_tlv_get(base_addr,RFTLV_TYPE_PWR_MODE, RFTLV_MAXLEN_PWR_MODE, pwrmode) > 0) {
    }else{
    	return RFPARAM_ERR_PWR_MODE_NULL;
    }

    for (int i = 0; i < RFTLV_MAXLEN_PWR_MODE; i++) {
        switch (pwrmode[i]) {
        	/* read pwr offset from efuse */
			case 'B':
			case 'b':
			{
			    if(0 == mfg_media_read_bz_poweroffset_with_lock(pwroffset_tmp,reload)){

				    rfparam_array_printf((char *)"efuse bz pwr_offset",pwroffset_tmp,sizeof(pwroffset_tmp),TYPE_INT8);
					if('B' == pwrmode[i]){
					    memcpy(pwr_offset,pwroffset_tmp,sizeof(pwroffset_tmp));
						return RFPARAM_SUSS;
					}else if('b' == pwrmode[i]){
						for(int i = 0; i < RFTLV_MAXLEN_BLE_PWR_OFFSET; i++){
							pwr_offset[i] += pwroffset_tmp[i];
						}
					}
				}else{
					rfparam_printf("no bz pwr_offset in efuse\r\n");
				}

			}
			break;

			/* read pwr offset from rftlv */
            case 'F':
            case 'f':
            {
				if (rfparam_tlv_get(base_addr,RFTLV_TYPE_PWR_OFFSET_BLE, RFTLV_MAXLEN_BLE_PWR_OFFSET, pwroffset_tmp) > 0) {
				    for(int i = 0; i < RFTLV_MAXLEN_BLE_PWR_OFFSET; i++){
			            pwroffset_tmp[i] -= 16;
			        }
					rfparam_array_printf((char *)"tlv bz pwr_offset",pwroffset_tmp,sizeof(pwroffset_tmp),TYPE_INT8);
					if('F' == pwrmode[i]){
						memcpy(pwr_offset,pwroffset_tmp,sizeof(pwroffset_tmp));
						return RFPARAM_SUSS;
					}else if('f' == pwrmode[i]){
						for(int i = 0; i < RFTLV_MAXLEN_BLE_PWR_OFFSET; i++){
							pwr_offset[i] += pwroffset_tmp[i];
						}
					}
				}else{
					rfparam_printf("tlv pwr_offset null\r\n");
				}
            }
            break;

            default:
            {
                rfparam_printf("not support mode\r\n");
            }
        }
    }

    return RFPARAM_SUSS;

}

int32_t rfparam_get_cap_code_with_option(uint32_t base_addr, uint8_t *capcode_in, uint8_t *capcode_out, uint8_t reload)
{
    uint8_t capmode[RFTLV_MAXLEN_XTAL_MODE+1]={0};
    __unused uint8_t capcode[RFTLV_MAXLEN_XTAL/4]={0};
    uint8_t tmp_buf[32];

    if (rfparam_tlv_get(base_addr,RFTLV_TYPE_XTAL_MODE, RFTLV_MAXLEN_XTAL_MODE, capmode) > 0) {
        rfparam_printf("capcode mode is %s\r\n",capmode);
    }else{
    	rfparam_printf("capcode mode null\r\n");
    	return RFPARAM_ERR_CAP_CODE_MODE_NULL;
    }

    for (int i = 0; i < RFTLV_MAXLEN_XTAL_MODE; i++) {
        switch (capmode[i]) {
            case 'F':
            {
                if (rfparam_tlv_get(base_addr,RFTLV_TYPE_XTAL, RFTLV_MAXLEN_XTAL, tmp_buf) > 0) {
                    *capcode_in = tmp_buf[0];
                    *capcode_out = tmp_buf[4];
                    rfparam_printf("tlv capcode_in %d,capcode_out %d\r\n",*capcode_in,*capcode_out);
                    return RFPARAM_SUSS;
                }else{
                	rfparam_printf("tlv capcode null\r\n");
                }
            }
            break;
            case 'M':
            {
                if(0 == mfg_media_read_xtal_capcode_with_lock(capcode_in, reload)){
                    *capcode_out = *capcode_in;
                    rfparam_printf("efuse capcode_in %d,capcode_out %d\r\n",*capcode_in,*capcode_out);
                    return RFPARAM_SUSS;
                }else{
                    rfparam_printf("no capcode in efuse\r\n");
                }
            }
            break;
            default:
            {
                rfparam_printf("not support mode\r\n");
            }
        }
    }

    return RFPARAM_SUSS;
}

void rfparam_get_capcode(uint8_t* capcode_in, uint8_t* capcode_out)
{
    *capcode_in = *capcode_out = AON_Get_Xtal_CapCode();
}

void rfparam_set_capcode(uint8_t capcode_in, uint8_t capcode_out)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AON_BASE + AON_XTAL_CFG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AON_XTAL_CAPCODE_IN_AON, capcode_in);
    tmpVal = reg_set_bits(tmpVal, AON_XTAL_CAPCODE_OUT_AON, capcode_out);
    writel(tmpVal, AON_BASE + AON_XTAL_CFG_OFFSET);

    //arch_delay_us(100);

    //return SUCCESS;

    //AON_Set_Xtal_CapCode(capcode_in, capcode_out);
}

/* Explicit RF TLV base override (see rfparam_adapter.h) */
static uint32_t g_tlv_base_override;

void rfparam_tlv_base_set(uint32_t addr)
{
    g_tlv_base_override = addr;
}

int8_t rfparam_load(struct wl_param_t *param)
{

    uint8_t tmp_buf[32]={0};

    if(g_tlv_base_override){
        g_tlv_base_addr = g_tlv_base_override;
    }else if(((uint32_t)rfparam_load & RFPARAM_TLV_BASE_ADDR_MASK) == (RFPARAM_TLV_BASE_ADDR_RAM_CACHE & RFPARAM_TLV_BASE_ADDR_MASK)){
        g_tlv_base_addr = RFPARAM_TLV_BASE_ADDR_RAM_CACHE;
    }else if(((uint32_t)rfparam_load & RFPARAM_TLV_BASE_ADDR_MASK) == (RFPARAM_TLV_BASE_ADDR_RAM_NO_CACHE & RFPARAM_TLV_BASE_ADDR_MASK)){
        g_tlv_base_addr = RFPARAM_TLV_BASE_ADDR_RAM_NO_CACHE;
    }else if(((uint32_t)rfparam_load & RFPARAM_TLV_BASE_ADDR_MASK) == (RFPARAM_TLV_BASE_ADDR_XIP_FLASH & RFPARAM_TLV_BASE_ADDR_MASK)){
        g_tlv_base_addr = RFPARAM_TLV_BASE_ADDR_XIP_FLASH;
    }else{
        return RFPARAM_ERR_TLV_BASE_ADDR;
    }

    if(param == NULL){
        return RFPARAM_ERR_PARAM_CHECK;
    }

    /* 1. get all rf params */
    /* pwr_offset_mode */

    if (RFPARAM_SUSS == rfparam_get_wlan_pwroffset_with_option(g_tlv_base_addr, param->pwrcal.channel_pwrcomp_wlan,1)){
        rfparam_array_printf((char *)"wlan pwr_offset",(void *)param->pwrcal.channel_pwrcomp_wlan,sizeof(param->pwrcal.channel_pwrcomp_wlan),TYPE_INT8);
    }else{
        rfparam_printf("wlan pwr_offset err\r\n");
        return RFPARAM_ERR_PWR_OFFSET_NULL;
    }

    if (RFPARAM_SUSS == rfparam_get_wlan_pwroffset_lp_with_option(g_tlv_base_addr, param->pwrcal.channel_lp_pwrcomp_wlan,1)){
        rfparam_array_printf((char *)"wlan lp pwr_offset",(void *)param->pwrcal.channel_lp_pwrcomp_wlan,sizeof(param->pwrcal.channel_lp_pwrcomp_wlan),TYPE_INT8);
    }else{
        rfparam_printf("wlan lp pwr_offset err\r\n");
        return RFPARAM_ERR_PWR_OFFSET_NULL;
    }

    if (RFPARAM_SUSS == rfparam_get_bz_pwroffset_with_option(g_tlv_base_addr, param->pwrcal.channel_pwrcomp_bz,1)){
        rfparam_array_printf((char *)"bz pwr_offset",(void *)param->pwrcal.channel_pwrcomp_bz,sizeof(param->pwrcal.channel_pwrcomp_bz),TYPE_INT8);
    }else{
        rfparam_printf("bz pwr_offset err\r\n");
        return RFPARAM_ERR_PWR_OFFSET_NULL;
    }

    /* pwr table */
    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11B, RFTLV_MAXLEN_PWR_TABLE_11B, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11b,tmp_buf,sizeof(param->pwrtarget.pwr_11b));

        rfparam_array_printf((char *)"pwr_11b",(void *)param->pwrtarget.pwr_11b,sizeof(param->pwrtarget.pwr_11b),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11b null\r\n");
    	return RFPARAM_ERR_PWR_11B_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11G, RFTLV_MAXLEN_PWR_TABLE_11G, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11g,tmp_buf,sizeof(param->pwrtarget.pwr_11g));
        rfparam_array_printf((char *)"pwr_11g",(void *)param->pwrtarget.pwr_11g,sizeof(param->pwrtarget.pwr_11g),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11g null\r\n");
    	return RFPARAM_ERR_PWR_11G_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11N, RFTLV_MAXLEN_PWR_TABLE_11N, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11n_ht20,tmp_buf,sizeof(param->pwrtarget.pwr_11n_ht20));
        rfparam_array_printf((char *)"pwr_11n_ht20",(void *)param->pwrtarget.pwr_11n_ht20,sizeof(param->pwrtarget.pwr_11n_ht20),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11n_ht20 null\r\n");
    	return RFPARAM_ERR_PWR_11N_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11N_HT40, RFTLV_MAXLEN_PWR_TABLE_11N_HT40, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11n_ht40,tmp_buf,sizeof(param->pwrtarget.pwr_11n_ht40));
        rfparam_array_printf((char *)"pwr_11n_ht40",(void *)param->pwrtarget.pwr_11n_ht40,sizeof(param->pwrtarget.pwr_11n_ht40),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11n_ht40 null\r\n");
    	return RFPARAM_ERR_PWR_11N_HT40_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AC_VHT20, RFTLV_MAXLEN_PWR_TABLE_11AC_VHT20, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ac_vht20,tmp_buf,sizeof(param->pwrtarget.pwr_11ac_vht20));
        rfparam_array_printf((char *)"pwr_11ac_vht20",(void *)param->pwrtarget.pwr_11ac_vht20,sizeof(param->pwrtarget.pwr_11ac_vht20),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11ac_vht20 null\r\n");
    	return RFPARAM_ERR_PWR_11AC_VHT20_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AC_VHT40, RFTLV_MAXLEN_PWR_TABLE_11AC_VHT40, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ac_vht40,tmp_buf,sizeof(param->pwrtarget.pwr_11ac_vht40));
        rfparam_array_printf((char *)"pwr_11ac_vht40",(void *)param->pwrtarget.pwr_11ac_vht40,sizeof(param->pwrtarget.pwr_11ac_vht40),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11ac_vht40 null\r\n");
    	return RFPARAM_ERR_PWR_11AC_VHT40_NULL;
    }
#if 0
    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AC_VHT80, RFTLV_MAXLEN_PWR_TABLE_11AC_VHT80, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ac_vht80,tmp_buf,sizeof(param->pwrtarget.pwr_11ac_vht80));
        rfparam_array_printf((char *)"pwr_11ac_vht80",(void *)param->pwrtarget.pwr_11ac_vht80,sizeof(param->pwrtarget.pwr_11ac_vht80),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11ac_vht80 null\r\n");
    	return RFPARAM_ERR_PWR_11AC_VHT80_NULL;
    }
#endif
    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AX_HE20, RFTLV_MAXLEN_PWR_TABLE_11AX_HE20, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ax_he20,tmp_buf,sizeof(param->pwrtarget.pwr_11ax_he20));
        rfparam_array_printf((char *)"pwr_11ax_he20",(void *)param->pwrtarget.pwr_11ax_he20,sizeof(param->pwrtarget.pwr_11ax_he20),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11ax_he20 null\r\n");
    	return RFPARAM_ERR_PWR_11AX_HE20_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AX_HE40, RFTLV_MAXLEN_PWR_TABLE_11AX_HE40, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ax_he40,tmp_buf,sizeof(param->pwrtarget.pwr_11ax_he40));
        rfparam_array_printf((char *)"pwr_11ax_he40",(void *)param->pwrtarget.pwr_11ax_he40,sizeof(param->pwrtarget.pwr_11ax_he40),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11ax_he40 null\r\n");
    	return RFPARAM_ERR_PWR_11AX_HE40_NULL;
    }
#if 0
    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AX_HE80, RFTLV_MAXLEN_PWR_TABLE_11AX_HE80, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ax_he80,tmp_buf,sizeof(param->pwrtarget.pwr_11ax_he80));
        rfparam_array_printf((char *)"pwr_11ax_he80",(void *)param->pwrtarget.pwr_11ax_he80,sizeof(param->pwrtarget.pwr_11ax_he80),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11ax_he80 null\r\n");
    	return RFPARAM_ERR_PWR_11AX_HE80_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AX_HE160, RFTLV_MAXLEN_PWR_TABLE_11AX_HE160, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ax_he160,tmp_buf,sizeof(param->pwrtarget.pwr_11ax_he160));
        rfparam_array_printf((char *)"pwr_11ax_he160",(void *)param->pwrtarget.pwr_11ax_he160,sizeof(param->pwrtarget.pwr_11ax_he160),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11ax_he160 null\r\n");
    	return RFPARAM_ERR_PWR_11AX_HE160_NULL;
    }
#endif
    /* capcode */
    if(RFPARAM_SUSS == rfparam_get_cap_code_with_option(g_tlv_base_addr,&param->xtalcapcode_in,&param->xtalcapcode_out,1)){
        rfparam_printf("capcode_in %d,capcode_out %d\r\n",param->xtalcapcode_in,param->xtalcapcode_out);
    }else{
        rfparam_printf("capcode err\r\n");
    }

    /*tcal*/
    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_EN_TCAL, RFTLV_MAXLEN_EN_TCAL, tmp_buf) > 0) {
		param->tcal.en_tcal=tmp_buf[0];
		rfparam_printf("tcal.en_tcal = %d\r\n", (int)param->tcal.en_tcal);
    }else{
    	rfparam_printf("tcal.en_tcal null\r\n");
    	return RFPARAM_ERR_TCAL_EN_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_LINEAR_OR_FOLLOW, RFTLV_MAXLEN_LINEAR_OR_FOLLOW, tmp_buf) > 0) {
        param->tcal.linear_or_follow=tmp_buf[0];
        rfparam_printf("tcal.linear_or_follow = %d\r\n", (int)param->tcal.linear_or_follow);
    }else{
    	rfparam_printf("tcal.linear_or_follow null\r\n");
    	return RFPARAM_ERR_TCAL_LINEAR_OR_FOLLOW_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_TCHANNELS, RFTLV_MAXLEN_TCHANNELS, tmp_buf) > 0) {
        for (int i = 0; i < RFTLV_MAXLEN_TCHANNELS/2; i++){
            param->tcal.Tchannels[i]= tmp_buf[i*2] | (tmp_buf[i*2+1] << 8);
        }
        rfparam_array_printf((char *)"tcal.Tchannels",(void *)param->tcal.Tchannels,sizeof(param->tcal.Tchannels)/2,TYPE_UINT16);
    }else{
    	rfparam_printf("tcal.Tchannels null\r\n");
    	return RFPARAM_ERR_TCAL_CHANNELS_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_TCHANNEL_OS, RFTLV_MAXLEN_TCHANNEL_OS, tmp_buf) > 0) {
        for (int i = 0; i < RFTLV_MAXLEN_TCHANNEL_OS/2; i++){
            param->tcal.Tchannel_os[i]=tmp_buf[i*2] | (tmp_buf[i*2+1]<<8);
        }
        rfparam_array_printf((char *)"tcal.Tchannel_os",(void *)param->tcal.Tchannel_os,sizeof(param->tcal.Tchannel_os)/2,TYPE_INT16);
    }else{
    	rfparam_printf("tcal.Tchannel_os null\r\n");
    	return RFPARAM_ERR_TCAL_CHANNEL_OS_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_TCHANNEL_OS_LOW, RFTLV_MAXLEN_TCHANNEL_OS_LOW, tmp_buf) > 0) {
        for (int i = 0; i < RFTLV_MAXLEN_TCHANNEL_OS_LOW/2; i++){
            param->tcal.Tchannel_os_low[i]=tmp_buf[i*2] | (tmp_buf[i*2+1] << 8);
        }
        rfparam_array_printf((char *)"tcal.Tchannel_os_low",(void *)param->tcal.Tchannel_os_low,sizeof(param->tcal.Tchannel_os_low)/2,TYPE_INT16);
    }else{
    	rfparam_printf("tcal.Tchannel_os_low null\r\n");
    	return RFPARAM_ERR_TCAL_CHANNEL_OS_LOW_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_TROOM_OS, RFTLV_MAXLEN_TROOM_OS, tmp_buf) > 0) {
        param->tcal.Troom_os=(tmp_buf[0] | (tmp_buf[1]<<8))-256;
		rfparam_printf("tcal.Troom_os = %d\r\n", (int)param->tcal.Troom_os);
    }else{
    	rfparam_printf("tcal.Troom_os null\r\n");
    	return RFPARAM_ERR_TCAL_ROOM_OS_NULL;
    }

    /*ble*/
    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_BLE, RFTLV_MAXLEN_PWR_TABLE_BLE, tmp_buf) > 0) {
        param->pwrtarget.pwr_ble = (int8_t)(tmp_buf[0] | (tmp_buf[1] << 8) | (tmp_buf[2] << 16) | (tmp_buf[3] << 24));
        if((uint8_t)param->pwrtarget.pwr_ble>128){
        	param->pwrtarget.pwr_ble-=256;
        }

        rfparam_printf("pwr_ble = %d \r\n", (int)param->pwrtarget.pwr_ble);

    }else{
        rfparam_printf("pwr_ble null\r\n");
        return RFPARAM_ERR_PWR_BLE_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_BT, RFTLV_MAXLEN_PWR_TABLE_BT, tmp_buf) > 0) {
        for(uint32_t i = 0; i < 3; i++){
            param->pwrtarget.pwr_bt[i] = (int8_t)(tmp_buf[i * 4] | (tmp_buf[i * 4 + 1] << 8) | (tmp_buf[i * 4 + 2] << 16) | (tmp_buf[i * 4 + 3] << 24));
            if((uint8_t)param->pwrtarget.pwr_bt[i]>128){
                param->pwrtarget.pwr_bt[i]-=256;
            }
        }

        rfparam_array_printf((char *)"pwr_bt",(void *)param->pwrtarget.pwr_bt,sizeof(param->pwrtarget.pwr_bt),TYPE_INT8);

    }else{
        rfparam_printf("pwr_bt null\r\n");
        return RFPARAM_ERR_PWR_BT_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_ZIGBEE, RFTLV_MAXLEN_PWR_TBALE_ZIGBEE, tmp_buf) > 0) {
        param->pwrtarget.pwr_zigbee = (int8_t)(tmp_buf[0] | (tmp_buf[1] << 8) | (tmp_buf[2] << 16) | (tmp_buf[3] << 24));
        if((uint8_t)param->pwrtarget.pwr_zigbee>128){
        	param->pwrtarget.pwr_zigbee-=256;
        }
        rfparam_printf("pwr_zigbee = %d \r\n", (int)param->pwrtarget.pwr_zigbee);

    }else{
        rfparam_printf("pwr_zigbee null\r\n");
        return RFPARAM_ERR_PWR_ZIGBEE_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_COUNTRY_CODE, RFTLV_MAXLEN_COUNTRY_CODE, tmp_buf) > 0) {
        param->country_code = (int8_t)(tmp_buf[0] | (tmp_buf[1] << 8));

        rfparam_printf("country_code = %d \r\n", (int)param->country_code);

    }else{
        rfparam_printf("country_code null\r\n");
        return RFPARAM_ERR_COUNTRY_CODE_NULL;
    }

    /* tcap_cal */
    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_EN_TCAPCAL, RFTLV_MAXLEN_EN_TCAPCAL, tmp_buf) > 0) {
        param->tcap.en_tcap = tmp_buf[0];
        rfparam_printf("en_tcap = %d \r\n", (int)param->tcap.en_tcap);

    }else{
        rfparam_printf("en_tcap Null\r\n");
        //return RFPARAM_ERR_EN_TCAP;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_TCAP_TSEN, RFTLV_MAXLEN_TCAP_TSEN, tmp_buf) > 0) {
        for(int i = 0; i < RFTLV_MAXLEN_TCAP_TSEN; i++){
            param->tcap.tcap_tsen[i] = tmp_buf[i];
        }
        rfparam_array_printf((char *)"tcap_tsen", param->tcap.tcap_tsen,RFTLV_MAXLEN_TCAP_TSEN,TYPE_INT8);

    }else{
        rfparam_printf("tcal_tsen Null\r\n");
        //return RFPARAM_ERR_TCAP_TSEN;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_TCAP_CAPCODE, RFTLV_MAXLEN_TCAP_CAPCODE, tmp_buf) > 0) {
        for(int i = 0; i < RFTLV_MAXLEN_TCAP_CAPCODE; i++){
            param->tcap.tcap_cap[i] = tmp_buf[i];
        }
        rfparam_array_printf((char *)"tcap_cap", param->tcap.tcap_cap,RFTLV_MAXLEN_TCAP_CAPCODE,TYPE_INT8);

    }else{
        rfparam_printf("tcap_cap Null\r\n");
        //return RFPARAM_ERR_TCAP_CAP;
    }

    for(int i = 0; i < NUM_WLAN_CHANNELS; i++){
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_POWER_LIMIT_2G_EXT_CH1+i, RFTLV_MAXLEN_PWR_LIMIT_EXT, tmp_buf) > 0) {
            param->pwrlim[i].en = 1; 
            param->pwrlim[i].b_dsss = tmp_buf[0];
            param->pwrlim[i].b_cck  = tmp_buf[1];
            param->pwrlim[i].g      = tmp_buf[2];
            param->pwrlim[i].n20    = tmp_buf[3];
            param->pwrlim[i].ax20   = tmp_buf[5];
            param->pwrlim[i].n40    = tmp_buf[6];
            param->pwrlim[i].ax40   = tmp_buf[8];

            printf((char *)"rfparam>>pwr_limit channel %d:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",i+1,tmp_buf[0],
                                        tmp_buf[1],tmp_buf[2],tmp_buf[3],
                                        tmp_buf[4],tmp_buf[5],tmp_buf[6],
                                        tmp_buf[7],tmp_buf[8],tmp_buf[9],
                                        tmp_buf[10],tmp_buf[11]);

        }else{
            param->pwrlim[i].en = 0;
            memset(&param->pwrlim[i],0,sizeof(param->pwrlim[i]));
        }
    }

    return RFPARAM_SUSS;

}

#ifdef BL_PSM_RFPARAM

static void parse_string(char* str, void* arr, int max_len, const char* type) {
    char* tok = strtok(str, ",");
    int i = 0;
    while (tok != NULL && i < max_len) {
        if (strcmp(type, "int8_t") == 0) {
            ((int8_t*) arr)[i] = (int8_t) atoi(tok);
        } else if (strcmp(type, "uint8_t") == 0) {
            ((uint8_t*) arr)[i] = (uint8_t) atoi(tok);
        } else if (strcmp(type, "uint16_t") == 0) {
            ((uint16_t*) arr)[i] = (uint16_t) atoi(tok);
        } else if (strcmp(type, "int16_t") == 0) {
            ((int16_t*) arr)[i] = (int16_t) atoi(tok);
        }
        i++;
        tok = strtok(NULL, ",");
    }
    if (i == 1 && str[strlen(str) - 1] != ',') {
        if (strcmp(type, "int8_t") == 0) {
            ((int8_t*) arr)[0] = (int8_t) atoi(str);
        } else if (strcmp(type, "uint8_t") == 0) {
            ((uint8_t*) arr)[0] = (uint8_t) atoi(str);
        } else if (strcmp(type, "uint16_t") == 0) {
            ((uint16_t*) arr)[0] = (uint16_t) atoi(str);
        } else if (strcmp(type, "int16_t") == 0) {
            ((int16_t*) arr)[0] = (int16_t) atoi(str);
        }
    }
}

int8_t rfparam_load_psm(struct wl_param_t *param)
{
    extern char *ef_get_env(const char *key);

    char *ef_env = NULL;

    if(param == NULL){
        return RFPARAM_ERR_PARAM_CHECK;
    }

    /* 1. get all rf params */
    ef_env = ef_get_env("wlan_pwr_offset");
    if(NULL != ef_env){
        parse_string(ef_env,param->pwrcal.channel_pwrcomp_wlan,sizeof(param->pwrcal.channel_pwrcomp_wlan),"int8_t");
        rfparam_array_printf((char *)"psm wlan_pwr_offset",(void *)param->pwrcal.channel_pwrcomp_wlan,sizeof(param->pwrcal.channel_pwrcomp_wlan),TYPE_INT8);
    }else{
        rfparam_printf("psm wlan_pwr_offset NULL\r\n");
    }    

    ef_env = ef_get_env("wlan_lp_pwr_offset");
    if(NULL != ef_env){
        parse_string(ef_env, param->pwrcal.channel_lp_pwrcomp_wlan, sizeof(param->pwrcal.channel_lp_pwrcomp_wlan),"int8_t");
        rfparam_array_printf((char *)"psm wlan_lp_pwr_offset",(void *)param->pwrcal.channel_lp_pwrcomp_wlan,sizeof(param->pwrcal.channel_lp_pwrcomp_wlan),TYPE_INT8);
    }else{
        rfparam_printf("psm wlan_lp_pwr_offset NULL\r\n");
    }

    ef_env = ef_get_env("bz_pwr_offset");
    if(NULL != ef_env){
        parse_string(ef_env, param->pwrcal.channel_pwrcomp_bz, sizeof(param->pwrcal.channel_pwrcomp_bz),"int8_t");
        rfparam_array_printf((char *)"psm bz_pwr_offset",(void *)param->pwrcal.channel_pwrcomp_bz,sizeof(param->pwrcal.channel_pwrcomp_bz),TYPE_INT8);
    }else{
        rfparam_printf("psm bz_pwr_offset NULL\r\n");
    }

    ef_env = ef_get_env("pwr_11b");
    if(NULL != ef_env){
        parse_string(ef_env, param->pwrtarget.pwr_11b, sizeof(param->pwrtarget.pwr_11b),"int8_t");
        rfparam_array_printf((char *)"psm pwr_11b",(void *)param->pwrtarget.pwr_11b,sizeof(param->pwrtarget.pwr_11b),TYPE_INT8);
    }else{
        rfparam_printf("psm pwr_11b NULL\r\n");
    }

    ef_env = ef_get_env("pwr_11g");
    if(NULL != ef_env){
        parse_string(ef_env, param->pwrtarget.pwr_11g, sizeof(param->pwrtarget.pwr_11g),"int8_t");
        rfparam_array_printf((char *)"psm pwr_11g",(void *)param->pwrtarget.pwr_11g,sizeof(param->pwrtarget.pwr_11g),TYPE_INT8);
    }else{
        rfparam_printf("psm pwr_11g NULL\r\n");
    }

    ef_env = ef_get_env("pwr_11n_ht20");
    if(NULL != ef_env){
        parse_string(ef_env, param->pwrtarget.pwr_11n_ht20, sizeof(param->pwrtarget.pwr_11n_ht20),"int8_t");
        rfparam_array_printf((char *)"psm pwr_11n_ht20",(void *)param->pwrtarget.pwr_11n_ht20,sizeof(param->pwrtarget.pwr_11n_ht20),TYPE_INT8);
    }else{
        rfparam_printf("psm pwr_11n_ht20 NULL\r\n");
    }

    ef_env = ef_get_env("pwr_11n_ht40");
    if(NULL != ef_env){
        parse_string(ef_env, param->pwrtarget.pwr_11n_ht40, sizeof(param->pwrtarget.pwr_11n_ht40),"int8_t");
        rfparam_array_printf((char *)"psm pwr_11n_ht40",(void *)param->pwrtarget.pwr_11n_ht40,sizeof(param->pwrtarget.pwr_11n_ht40),TYPE_INT8);
    }else{
        rfparam_printf("psm pwr_11n_ht40 NULL\r\n");
    }

    ef_env = ef_get_env("pwr_11ac_vht20");
    if(NULL != ef_env){
        parse_string(ef_env, param->pwrtarget.pwr_11ac_vht20, sizeof(param->pwrtarget.pwr_11ac_vht20),"int8_t");
        rfparam_array_printf((char *)"psm pwr_11ac_vht20",(void *)param->pwrtarget.pwr_11ac_vht20,sizeof(param->pwrtarget.pwr_11ac_vht20),TYPE_INT8);
    }else{
        rfparam_printf("psm pwr_11ac_vht20 NULL\r\n");
    }

    ef_env = ef_get_env("pwr_11ac_vht40");
    if(NULL != ef_env){
        parse_string(ef_env, param->pwrtarget.pwr_11ac_vht40, sizeof(param->pwrtarget.pwr_11ac_vht40),"int8_t");
        rfparam_array_printf((char *)"psm pwr_11ac_vht40",(void *)param->pwrtarget.pwr_11ac_vht40,sizeof(param->pwrtarget.pwr_11ac_vht40),TYPE_INT8);
    }else{
        rfparam_printf("psm pwr_11ac_vht40 NULL\r\n");
    }

    ef_env = ef_get_env("pwr_11ax_he20");
    if(NULL != ef_env){
        parse_string(ef_env, param->pwrtarget.pwr_11ax_he20, sizeof(param->pwrtarget.pwr_11ax_he20),"int8_t");
        rfparam_array_printf((char *)"psm pwr_11ax_he20",(void *)param->pwrtarget.pwr_11ax_he20,sizeof(param->pwrtarget.pwr_11ax_he20),TYPE_INT8);
    }else{
        rfparam_printf("psm pwr_11ax_he20 NULL\r\n");
    }

    ef_env = ef_get_env("pwr_11ax_he40");
    if(NULL != ef_env){
        parse_string(ef_env, param->pwrtarget.pwr_11ax_he40, sizeof(param->pwrtarget.pwr_11ax_he40),"int8_t");
        rfparam_array_printf((char *)"psm pwr_11ax_he40",(void *)param->pwrtarget.pwr_11ax_he40,sizeof(param->pwrtarget.pwr_11ax_he40),TYPE_INT8);
    }else{
        rfparam_printf("psm pwr_11ax_he40 NULL\r\n");
    }

    ef_env = ef_get_env("capcode_in");
    if(NULL != ef_env){
        parse_string(ef_env, &param->xtalcapcode_in, sizeof(param->xtalcapcode_in),"uint8_t");
        rfparam_printf("psm capcode_in %d\r\n",param->xtalcapcode_in);
    }else{
        rfparam_printf("psm capcode_in NULL\r\n");
    }

    ef_env = ef_get_env("capcode_out");
    if(NULL != ef_env){
        parse_string(ef_env, &param->xtalcapcode_out, sizeof(param->xtalcapcode_out),"uint8_t");
        rfparam_printf("psm capcode_out %d\r\n",param->xtalcapcode_out);
    }else{
        rfparam_printf("psm capcode_out NULL\r\n");
    }

    ef_env = ef_get_env("tcal.en_tcal");
    if(NULL != ef_env){
        parse_string(ef_env, &param->tcal.en_tcal, sizeof(param->tcal.en_tcal),"uint8_t");
        rfparam_printf("psm tcal.en_tcal %d\r\n",param->tcal.en_tcal);
    }else{
        rfparam_printf("psm tcal.en_tcal NULL\r\n");
    }

    ef_env = ef_get_env("tcal.linear_or_follow");
    if(NULL != ef_env){
        parse_string(ef_env, &param->tcal.linear_or_follow, sizeof(param->tcal.linear_or_follow),"uint8_t");
        rfparam_printf("psm tcal.linear_or_follow %d\r\n",param->tcal.linear_or_follow);
    }else{
        rfparam_printf("psm tcal.linear_or_follow NULL\r\n");
    }

    ef_env = ef_get_env("tcal.Tchannels");
    if(NULL != ef_env){
        parse_string(ef_env, param->tcal.Tchannels, sizeof(param->tcal.Tchannels)/2,"uint16_t");
        rfparam_array_printf((char *)"psm tcal.Tchannels",(void *)param->tcal.Tchannels,sizeof(param->tcal.Tchannels)/2,TYPE_UINT16);
    }else{
        rfparam_printf("psm tcal.Tchannels NULL\r\n");
    }

    ef_env = ef_get_env("tcal.Tchannel_os");
    if(NULL != ef_env){
        parse_string(ef_env, param->tcal.Tchannel_os, sizeof(param->tcal.Tchannel_os)/2,"int16_t");
        rfparam_array_printf((char *)"psm tcal.Tchannel_os",(void *)param->tcal.Tchannel_os,sizeof(param->tcal.Tchannel_os)/2,TYPE_UINT16);
    }else{
        rfparam_printf("psm tcal.Tchannel_os NULL\r\n");
    }

    ef_env = ef_get_env("tcal.Tchannel_os_low");
    if(NULL != ef_env){
        parse_string(ef_env, param->tcal.Tchannel_os_low, sizeof(param->tcal.Tchannel_os_low)/2,"int16_t");
        rfparam_array_printf((char *)"psm tcal.Tchannel_os_low",(void *)param->tcal.Tchannel_os_low,sizeof(param->tcal.Tchannel_os_low)/2,TYPE_UINT16);
    }else{
        rfparam_printf("psm tcal.Tchannel_os_low NULL\r\n");
    }

    ef_env = ef_get_env("tcal.Troom_os");
    if(NULL != ef_env){
        parse_string(ef_env, &param->tcal.Troom_os, sizeof(param->tcal.Troom_os)/2,"int16_t");
        rfparam_printf("psm tcal.Troom_os = %d\r\n", (int)param->tcal.Troom_os);
    }else{
        rfparam_printf("psm tcal.Troom_os NULL\r\n");
    }

    ef_env = ef_get_env("pwr_ble");
    if(NULL != ef_env){
        parse_string(ef_env, &param->pwrtarget.pwr_ble, sizeof(param->pwrtarget.pwr_ble)/2,"int8_t");
        rfparam_printf("psm pwr_ble = %d \r\n", (int)param->pwrtarget.pwr_ble);
    }else{
        rfparam_printf("psm pwr_ble NULL\r\n");
    }

    ef_env = ef_get_env("pwr_bt");
    if(NULL != ef_env){
        parse_string(ef_env, param->pwrtarget.pwr_bt, sizeof(param->pwrtarget.pwr_bt),"int8_t");
        rfparam_array_printf((char *)"psm pwr_bt",(void *)param->pwrtarget.pwr_bt,sizeof(param->pwrtarget.pwr_bt),TYPE_INT8);
    }else{
        rfparam_printf("psm pwr_bt NULL\r\n");
    }

    ef_env = ef_get_env("pwr_zigbee");
    if(NULL != ef_env){
        parse_string(ef_env, &param->pwrtarget.pwr_zigbee, sizeof(param->pwrtarget.pwr_zigbee),"int8_t");
        rfparam_printf("psm pwr_zigbee = %d \r\n", (int)param->pwrtarget.pwr_zigbee);
    }else{
        rfparam_printf("psm pwr_zigbee NULL\r\n");
    }

    ef_env = ef_get_env("country_code");
    if(NULL != ef_env){
        parse_string(ef_env, &param->country_code, sizeof(param->country_code)/2,"uint16_t");
        rfparam_printf("psm country_code = %d \r\n", (int)param->country_code);
    }else{
        rfparam_printf("psm country_code NULL\r\n");
    }

    return RFPARAM_SUSS;

}
#endif

struct wl_cfg_t * rfparam_cfg_get()
{
    return g_rfparam_cfg;
}

uint32_t rfparam_tlv_base_addr_get()
{
    return g_tlv_base_addr;
}

int32_t rfparam_init(uint32_t base_addr, void *rf_para, uint32_t apply_flag)
{

    extern void vprint(const char* fmt,va_list arg);

    int32_t ret;
    uint32_t xtal_value;
	Efuse_Common_Trim_Type trim;
#if defined(WL_API_RMEM_EN) && WL_API_RMEM_EN
    /* WL_API_RMEM_ADDR comes from wl_api.h (managed HBN-RAM WLCAL region).
     * A stale local 0x20010600 define here used to bind wl_cfg to a
     * non-writable address. */
    g_rfparam_cfg = wl_cfg_get((uint8_t*)WL_API_RMEM_ADDR);
#else
    g_rfparam_cfg = wl_cfg_get();
#endif
    g_rfparam_cfg->log_level = WL_LOG_LEVEL_TRACE;
    g_rfparam_cfg->log_printf = vprint;
    g_rfparam_cfg->capcode_get = rfparam_get_capcode;
    g_rfparam_cfg->capcode_set = rfparam_set_capcode;
    g_rfparam_cfg->param_load = rfparam_load;
    g_rfparam_cfg->en_full_cal = 1;
    g_rfparam_cfg->en_param_load = 1;
    g_rfparam_cfg->mode = WL_API_MODE_ALL;
    HBN_Get_Xtal_Value(&xtal_value);
    g_rfparam_cfg->param.xtalfreq_hz = xtal_value;
    rfparam_printf("xtal value %d\r\n",(int)xtal_value);

    EF_Ctrl_Read_Common_Trim("dcdc_trim", &trim, 1);
    if (trim.empty) {
        rfparam_printf("dcdc_trim empty\r\n");
        rfparam_printf("dcdc_trim use default value 0x80\r\n"); // request from xueliang
        g_rfparam_cfg->param.ef.dcdc_vout_trim_aon = 0x80;
    } else {
        if (trim.en == 1 && trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value, trim.len)) {
            g_rfparam_cfg->param.ef.dcdc_vout_trim_aon = trim.value;
            rfparam_printf("dcdc_trim value %d\r\n",(int)trim.value);
        }else {
            rfparam_printf("dcdc_trim param error\r\n");
            rfparam_printf("dcdc_trim use default value 0x80\r\n"); // request from xueliang
            g_rfparam_cfg->param.ef.dcdc_vout_trim_aon = 0x80;
        }
    }

    EF_Ctrl_Read_Common_Trim("icx", &trim, 1);
    if (trim.empty) {
        rfparam_printf("icx empty\r\n");
        rfparam_printf("icx use default value 0x80\r\n"); // request from xueliang
        g_rfparam_cfg->param.ef.icx_code = 0x80;
    } else {
        if (trim.en == 1 && trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value, trim.len)) {
            g_rfparam_cfg->param.ef.icx_code = trim.value;
            rfparam_printf("icx value %d\r\n",(int)trim.value);
        }else {
            rfparam_printf("icx param error\r\n");
            rfparam_printf("icx use default value 0x80\r\n"); // request from xueliang
            g_rfparam_cfg->param.ef.icx_code = 0x80;
        }
    }

    EF_Ctrl_Read_Common_Trim("iptat", &trim, 1);
    if (trim.empty) {
        rfparam_printf("iptat empty\r\n");
        rfparam_printf("iptat use default value 0x80\r\n"); // request from xueliang
        g_rfparam_cfg->param.ef.iptat_code = 0x80;
    } else {
        if (trim.en == 1 && trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value, trim.len)) {
            g_rfparam_cfg->param.ef.iptat_code = trim.value;
            rfparam_printf("iptat value %d\r\n",(int)trim.value);
        }else {
            rfparam_printf("iptat param error\r\n");
            rfparam_printf("iptat use default value 0x80\r\n"); // request from xueliang
            g_rfparam_cfg->param.ef.iptat_code = 0x80;
        }
    }

    EF_Ctrl_Read_Common_Trim("tmp_mp2", &trim, 1);
    if (trim.en == 1 && trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value, trim.len)) {
        g_rfparam_cfg->param.ef.Temperature_MP = trim.value;
        rfparam_printf("tmp_mp2 value %d\r\n",(int)trim.value);
    }else{
        EF_Ctrl_Read_Common_Trim("tmp_mp1", &trim, 1);
        if (trim.en == 1 && trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value, trim.len)) {
            g_rfparam_cfg->param.ef.Temperature_MP = trim.value;
            rfparam_printf("tmp_mp1 value %d\r\n",(int)trim.value);
        }else{
            EF_Ctrl_Read_Common_Trim("tmp_mp0", &trim, 1);
            if (trim.en == 1 && trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value, trim.len)) {
                g_rfparam_cfg->param.ef.Temperature_MP = trim.value;
                rfparam_printf("tmp_mp0 value %d\r\n",(int)trim.value);
            }else{
                g_rfparam_cfg->param.ef.Temperature_MP = 35; // request from xueliang
                rfparam_printf("tmp_mp use default value 35\r\n");
            }
        }
    }

    ret = wl_init();

    return ret;

}


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

#ifndef __IPROAI_INST_UTIL_H__
#define __IPROAI_INST_UTIL_H__

#ifndef IPROAI_MALLOC
#define IPROAI_MALLOC(sz) iproai_mem_alloc(sz, __func__, __LINE__)
#endif
#ifndef IPROAI_FREE
#define IPROAI_FREE(addr) iproai_mem_free(addr, __func__, __LINE__)

/* Set to 1 to print every pointer just before it is released. The teardown
 * path is normally exercised only once per boot, so a double free or a stale
 * interior pointer shows up as an assert with no hint of which field caused
 * it; the last line printed names the culprit.
 *
 * Turn it on with CONFIG_AI_FREE_TRACE in the app's .config. An app-level
 * add_compile_definitions() does NOT reach this component - apps are processed
 * after components, so the define never lands on the ai target. */
#ifndef IPROAI_FREE_TRACE
#ifdef CONFIG_AI_FREE_TRACE
#define IPROAI_FREE_TRACE 1
#else
#define IPROAI_FREE_TRACE 0
#endif
#endif

#if IPROAI_FREE_TRACE
#include <stdio.h>
#define IPROAI_FREE_T(label, addr) \
    do { printf("[free] %-24s %p\r\n", (label), (void *)(addr)); \
         IPROAI_FREE(addr); } while (0)
#else
#define IPROAI_FREE_T(label, addr) IPROAI_FREE(addr)
#endif
#endif
#ifndef BALI_CHN_ALIGN
#define BALI_CHN_ALIGN(sz)  (((sz + 3) >> 2) << 2)
#endif

void repack_valid_data(iproai_model_hdl_t hdl);
void* iproai_mem_alloc(size_t size, const char *func, const int line);
void iproai_mem_free(void* addr, const char *func, const int line);
int unsigned_to_signed(int in, int bit);
float fixed_to_float(fixed_point_t input, int FIXED_POINT_FRACTIONAL_BITS);
fixed_point_t fixed32_to_fixed8(int32_t input, int FIXED_POINT_FRACTIONAL_BITS);
fixed_point_t fixed8_to_fixed8(fixed_point_t input, int FIXED_POINT_FRACTIONAL_BITS);
int32_t fixed32_to_fixed32(int32_t input, int FIXED_POINT_FRACTIONAL_BITS);
fixed_point_t float_to_fixed(float input, int FIXED_POINT_FRACTIONAL_BITS);
int32_t fixed8_to_fixed32(fixed_point_t input, int FIXED_POINT_FRACTIONAL_BITS);
int fetch_info(uint8_t* inst, int base, int len);
int8_t* MEM_PTR(int mux, fixed_point_t* DATA_buf, int PATCH_SIZE);
fixed_point_t* MEM_PTR_ADDR(int mux, int addr, fixed_point_t* DATA_buf, int patch_size);
void set_output_wh(struct iproai_net_info_t* net, int layer_cnt);
void set_output_wh_later(struct iproai_net_info_t* net, int layer_cnt);
void set_npu_output_wh(struct iproai_net_info_t* net, int layer_cnt);
void check_merge_layer(struct iproai_net_info_t* net, int layer_cnt);
bool check_combomax_layer(struct cpu_inst_layer_t* l);
bool check_upsample_layer(struct cpu_inst_layer_t* l);
bool check_conv_layer(struct cpu_inst_layer_t *l);
bool check_weight_layer(struct cpu_inst_layer_t *l);
bool check_MultiInput_layer(struct cpu_inst_layer_t *l);
bool check_route_layer(struct cpu_inst_layer_t *l);
bool check_combine_layer(struct cpu_inst_layer_t *l);

#endif // __IPROAI_INST_UTIL_H__

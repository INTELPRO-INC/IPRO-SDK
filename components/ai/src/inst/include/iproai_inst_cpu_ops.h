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

#ifndef __IPROAI_INST_CPU_OPS_H__
#define __IPROAI_INST_CPU_OPS_H__

#include "iproai_inst_cpu_activation.h"
#include "iproai_inst_obj_det_ops.h"

struct BoxCornerEncoding {
    float ymin;
    float xmin;
    float ymax;
    float xmax;
};

struct CenterSizeEncoding {
    float y;
    float x;
    float h;
    float w;
};

struct IPROAI_data_t {
    int image_width, image_height;
    char* name_list;
};

void unpack_channel_data(int mux, fixed_point_t* DATA_buf, int patch_size, int databuf_size, int pack_c, int l_c);
fixed_point_t MEM_RW(int write, int addr, fixed_point_t in, int mux, fixed_point_t* DATA_buf, int patch_size);
float MEM_RW_F32(int write, int addr, float in, int mux, fixed_point_t* DATA_buf, int patch_size);
void fetch_data(struct cpu_inst_layer_t* l, fixed_point_t* data_in, int data_index, int SRAM_in, int win, int hin, fixed_point_t padding, fixed_point_t* DATA_buf, int patch_size);
void fetch_data_even(struct cpu_inst_layer_t* l, fixed_point_t* data_in, int data_index, int SRAM_in, int win, int hin, fixed_point_t padding, fixed_point_t* DATA_buf, int patch_size);
int32_t PE_CONV(fixed_point_t* data_in, fixed_point_t* weights, int size, int unsgn);
int32_t PE_LEAKY(int32_t pass_data);
int32_t PE_RELU(int32_t pass_data);
int32_t PE_RELU6(int32_t pass_data, int fshift);
int32_t PE_RELUN(int32_t pass_data, int fshift, int n);
fixed_point_t PE_MISH(int32_t pass_data, int fin, int fout);

void forward_CONVOLUTIONAL(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_CONVOLUTIONAL_tflite_nmsis(struct iproai_net_info_t *net, struct cpu_inst_layer_t *l, fixed_point_t* DATA_buf, int l_current);
void forward_DEPTHWISE_CONVOLUTIONAL_tflite_nmsis(struct iproai_net_info_t *net, struct cpu_inst_layer_t *l, fixed_point_t* DATA_buf, int l_current);
void forward_CONVOLUTIONAL_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_CONV_MAX_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_CONV_MAX(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_MAXPOOL(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_MAXPOOL_tflite_cmsis(struct iproai_net_info_t *net, struct cpu_inst_layer_t *l, fixed_point_t* DATA_buf, int l_current);
void forward_MAXPOOL_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_AVGPOOL_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_ROUTE_CONV(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_UPSAMPLE(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_UP_BILINEAR_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_ARGMAX_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_ROUTE_UPSAMPLE(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_ROUTE(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_ROUTE_MAX(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_ROUTE_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_ROUTE_W_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_ROUTE_MAX_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_SHORTCUT(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_SHORTCUT_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
#ifdef CONFIG_AI_LEGACY_YOLO
void forward_YOLO(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_YOLO_lm(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
#endif
void forward_RESHAPE_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_PRE_TRANSCONV_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_TRANSPOSE_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_TRANSPOSELK_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_TRANSPOSELK_V2_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_PAD_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_MEAN_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_LEAKY_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_MUL_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_LOGISTIC_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_LOG_SOFTMAX_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_SOFTMAX_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_MATMUL_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_MATMUL(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_SSD_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current, struct IPROAI_data_t *det_data, const char** name_list, iproai_inference_cb result_cb);
void forward_DEQUANTIZE_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_QUANTIZE_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_STRIDESLICE_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_DEPTH_TO_SPACE_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_MIN_MAX2_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current, int take_max);
void forward_GATHER_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_LUT_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);
void forward_MINIMUM_tflite(struct iproai_net_info_t* net, struct cpu_inst_layer_t* l, fixed_point_t* DATA_buf, int l_current);


#endif // __IPROAI_INST_CPU_OPS_H__

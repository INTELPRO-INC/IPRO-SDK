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

#ifndef __IPROAI_INST_OBJ_DET_OPS_H__
#define __IPROAI_INST_OBJ_DET_OPS_H__

typedef struct {
    float x, y, w, h;
} box;

typedef struct {
    box bbox;
    int classes;
    float* prob;
    float* mask;
    float objectness;
    int sort_class;
    float* uc; // Gaussian_YOLOv3 - tx,ty,tw,th uncertainty
    int points; // bit-0 - center, bit-1 - top-left-corner, bit-2 - bottom-right-corner
} detection;

typedef struct {
    detection det;
    // The most probable class id: the best class index in this->prob.
    // Is filled temporary when processing results, otherwise not initialized
    int best_class;
} detection_with_class;

typedef struct {
    float x, y, w, h;
    float lx1, ly1, lx2, ly2, lx3, ly3, lx4, ly4, lx5, ly5;
} lm_box;

typedef struct {
    lm_box bbox;
    int classes;
    float* prob;
    float* mask;
    float objectness;
    int sort_class;
    float* uc; // Gaussian_YOLOv3 - tx,ty,tw,th uncertainty
    int points; // bit-0 - center, bit-1 - top-left-corner, bit-2 - bottom-right-corner
} lm_detection;

typedef struct {
    lm_detection det;
    // The most probable class id: the best class index in this->prob.
    // Is filled temporary when processing results, otherwise not initialized
    int best_class;
} lm_detection_with_class;

int iproai_entry_index(struct cpu_inst_layer_t* l, int location, int entry);
void iproai_activate_array(float* x, const int n, const ACTIVATION a);
int lm_entry_index(struct cpu_inst_layer_t* l, int batch, int location, int entry);
void iproai_scal_add_cpu(int N, float ALPHA, float BETA, float* X, int INCX);

#endif //__IPROAI_INST_OBJ_DET_OPS_H__

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

#ifndef __IPROAI_INST_POSTPROCESS_H__
#define __IPROAI_INST_POSTPROCESS_H__

typedef struct {
    int w;
    int h;
    int c;
    float* data;
} image;

void image_classification(struct iproai_net_info_t* net, const char** imgnet_names, fixed_point_t* DATA_buf, iproai_inference_cb result_cb, int num);
void KWS_classification(struct iproai_net_info_t* net, const char** imgnet_names, fixed_point_t* DATA_buf, iproai_inference_cb result_cb, int num);
void get_output_data(struct iproai_net_info_t* net, fixed_point_t* DATA_buf, int layer, int size);
void face_recongnition(struct iproai_net_info_t* net, const char** names, int names_len, const float** feature_list, int feature_len, fixed_point_t* DATA_buf, iproai_inference_cb result_cb);
void yolo_obj_det(struct iproai_net_info_t *net, const char** names, int image_width, int image_height, iproai_inference_cb result_cb);
void yolo_face_landmark(struct iproai_net_info_t* net, const char** names, int image_width, int image_height, iproai_inference_cb result_cb);
void load_postprocess_classification(iproai_model_hdl_t hdl, const char** classes_name, int classes_num);
void load_postprocess_face_recongnition(iproai_model_hdl_t hdl, const char** names, int names_num, const float** features_list, int features_num);
image similarTransform(image src, image dst);
image iproai_make_image(int w, int h, int c);
void retina_face(struct iproai_net_info_t* net, const char** imgnet_names, fixed_point_t* DATA_buf, int image_width, int image_height, iproai_inference_cb result_cb);
void retina_person(struct iproai_net_info_t* net, const char** imgnet_names, fixed_point_t* DATA_buf, int image_width, int image_height, iproai_inference_cb result_cb);
void retina(struct iproai_net_info_t* net, const char** imgnet_names, int class_num, fixed_point_t* DATA_buf, int image_width, int image_height, iproai_inference_cb result_cb);
void face_recognition_evaluation(struct iproai_net_info_t* net, const char** face_IDs, fixed_point_t* DATA_buf, int image_width, int image_height, iproai_inference_cb result_cb);
void get_segmentation_mask(struct iproai_net_info_t* net, fixed_point_t* DATA_buf, int layer, uint32_t out_image_width, uint32_t out_image_height, iproai_inference_cb result_cb);
void get_sr_result(struct iproai_net_info_t* net, fixed_point_t* DATA_buf, int layer, uint32_t out_image_width, uint32_t out_image_height, iproai_inference_cb result_cb);



#endif /* __IPROAI_INST_POSTPROCESS_H__ */


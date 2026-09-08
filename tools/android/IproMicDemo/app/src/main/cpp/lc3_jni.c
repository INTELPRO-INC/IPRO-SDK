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
 * @file lc3_jni.c
 * @brief JNI bridge for LC3 decoder
 *
 * Provides init/decode/destroy functions for use from Kotlin via JNI.
 * The decoder handle is passed as a long (pointer cast).
 */

#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <android/log.h>
#include "lc3.h"

#define TAG "LC3_JNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

typedef struct {
    lc3_decoder_t decoder;
    void *mem;
    int frame_samples;
} lc3_handle_t;

JNIEXPORT jlong JNICALL
Java_com_ipro_micdemo_Lc3Decoder_nativeInit(
    JNIEnv *env, jobject thiz, jint frame_duration_us, jint sample_rate)
{
    unsigned dec_size = lc3_decoder_size(frame_duration_us, sample_rate);
    if (dec_size == 0) {
        LOGE("Invalid params: %d us, %d Hz", frame_duration_us, sample_rate);
        return 0;
    }

    lc3_handle_t *h = calloc(1, sizeof(lc3_handle_t));
    if (!h) return 0;

    h->mem = malloc(dec_size);
    if (!h->mem) {
        free(h);
        return 0;
    }

    h->decoder = lc3_setup_decoder(frame_duration_us, sample_rate, 0, h->mem);
    if (!h->decoder) {
        LOGE("Decoder setup failed");
        free(h->mem);
        free(h);
        return 0;
    }

    h->frame_samples = lc3_frame_samples(frame_duration_us, sample_rate);
    LOGI("Decoder init: %d us, %d Hz, %d samples/frame, mem=%u bytes",
         frame_duration_us, sample_rate, h->frame_samples, dec_size);

    return (jlong)(uintptr_t)h;
}

JNIEXPORT jshortArray JNICALL
Java_com_ipro_micdemo_Lc3Decoder_nativeDecode(
    JNIEnv *env, jobject thiz, jlong handle,
    jbyteArray lc3_data, jint output_samples)
{
    lc3_handle_t *h = (lc3_handle_t *)(uintptr_t)handle;
    if (!h || !h->decoder) return NULL;

    int in_len = (*env)->GetArrayLength(env, lc3_data);
    jbyte *in_buf = (*env)->GetByteArrayElements(env, lc3_data, NULL);

    int n_samples = (output_samples > 0) ? output_samples : h->frame_samples;
    int16_t *pcm = malloc(n_samples * sizeof(int16_t));
    if (!pcm) {
        (*env)->ReleaseByteArrayElements(env, lc3_data, in_buf, JNI_ABORT);
        return NULL;
    }

    int ret = lc3_decode(h->decoder, in_buf, in_len,
                         LC3_PCM_FORMAT_S16, pcm, 1);
    (*env)->ReleaseByteArrayElements(env, lc3_data, in_buf, JNI_ABORT);

    if (ret < 0) {
        free(pcm);
        return NULL;
    }

    jshortArray result = (*env)->NewShortArray(env, n_samples);
    if (!result) {
        free(pcm);
        return NULL;
    }
    (*env)->SetShortArrayRegion(env, result, 0, n_samples, pcm);
    free(pcm);

    return result;
}

JNIEXPORT void JNICALL
Java_com_ipro_micdemo_Lc3Decoder_nativeDestroy(
    JNIEnv *env, jobject thiz, jlong handle)
{
    lc3_handle_t *h = (lc3_handle_t *)(uintptr_t)handle;
    if (!h) return;
    free(h->mem);
    free(h);
    LOGI("Decoder destroyed");
}

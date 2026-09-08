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
 * @file speaker_recog.h
 * @brief Speaker Recognition API Header
 *
 * Provides speaker recognition (speaker verification/identification) functionality.
 * Processes NPU output through pooling and segment layers to generate speaker embeddings.
 *
 * Architecture:
 *   NPU Output [C][T][F] -> Pooling -> Seg1 (Linear+ReLU+BN) -> Seg2 (Linear) -> Embedding
 *
 * Usage:
 *   1. speaker_recog_init() - Initialize with network info, loads weights from SD card
 *   2. speaker_recog_process() - Process NPU output to get speaker embedding
 *   3. speaker_recog_register() - Register current embedding as a speaker
 *   4. speaker_recog_identify() - Identify speaker from current embedding
 *   5. speaker_recog_deinit() - Cleanup resources
 */

#ifndef __SPEAKER_RECOG_H__
#define __SPEAKER_RECOG_H__

#include <stdint.h>
#include <stdbool.h>
#include "iproai_core.h"
#include "iproai_def_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========== Configuration Constants ========== */

#define SPEAKER_RECOG_MAX_SPEAKERS      20      // Maximum registered speakers
#define SPEAKER_RECOG_MAX_NAME_LEN      32      // Maximum speaker name length

/* Default model dimensions (can be overridden in config) */
#define SPEAKER_RECOG_DEFAULT_C         128     // Channels
#define SPEAKER_RECOG_DEFAULT_T         20      // Time steps
#define SPEAKER_RECOG_DEFAULT_F         75      // Frequency bins
#define SPEAKER_RECOG_DEFAULT_EMB_SIZE  48      // Embedding dimension
#define SPEAKER_RECOG_DEFAULT_POOL_DIM  5120    // Pooling dimension = 2*C*T

/* Default thresholds */
#define SPEAKER_RECOG_DEFAULT_SIMILARITY_THRESHOLD  0.5f

/* Default weights path */
#define SPEAKER_RECOG_DEFAULT_WEIGHTS_PATH  "/sdcard/ai_model/audio_weights.bin"

/* ========== Data Structures ========== */

/**
 * @brief Single speaker information
 */
typedef struct {
    float* embedding;                           // Speaker embedding [emb_size]
    char name[SPEAKER_RECOG_MAX_NAME_LEN];      // Speaker name
    bool valid;                                 // Whether this slot is valid
} speaker_info_t;

/**
 * @brief Speaker recognition result
 */
typedef struct {
    int speaker_id;                             // Identified speaker ID (-1 if unknown)
    const char* speaker_name;                   // Speaker name (NULL if unknown)
    float similarity;                           // Similarity score with best match
    float* embedding;                           // Current embedding (points to internal buffer)
    uint32_t process_time_ms;                   // Processing time in milliseconds
} speaker_recog_result_t;

/**
 * @brief Weights structure for segment layers
 */
typedef struct {
    float* W1;              // First linear layer weights [emb_size][pool_dim]
    float* b1;              // First linear layer bias [emb_size]
    float* running_mean;    // BatchNorm running mean [emb_size]
    float* running_var;     // BatchNorm running variance [emb_size]
    float* W2;              // Second linear layer weights [emb_size][emb_size]
    float* b2;              // Second linear layer bias [emb_size]
    uint8_t* raw_buffer;    // Raw weight buffer (for deallocation)
    uint32_t buffer_size;   // Size of raw buffer
} speaker_recog_weights_t;

/**
 * @brief Working buffers for processing
 */
typedef struct {
    float* dequantized;     // Dequantized NPU output [C*T*F]
    float* transposed;
    float* pooled;          // Pooled features [pool_dim]
    float* seg1_out;        // Segment 1 output [emb_size]
    float* embedding;       // Final embedding [emb_size]
} speaker_recog_buffers_t;

/**
 * @brief Speaker database
 */
typedef struct {
    speaker_info_t speakers[SPEAKER_RECOG_MAX_SPEAKERS];
    int count;
} speaker_database_t;

/**
 * @brief Speaker recognition configuration and state
 */
typedef struct {
    /* Model dimensions */
    uint32_t input_c;           // Input channels (default: 128)
    uint32_t input_t;           // Input time steps (default: 20)
    uint32_t input_f;           // Input frequency bins (default: 75)
    uint32_t emb_size;          // Embedding dimension (default: 48)
    uint32_t pool_dim;          // Pooling dimension (default: 5120 = 2*C*T)

    /* Thresholds */
    float similarity_threshold; // Threshold for speaker identification

    /* Network information (from IPROAI) */
    struct iproai_net_info_t* net;

    /* Quantization parameters */
    float output_scale;
    int32_t output_zero_point;

    /* Weights */
    speaker_recog_weights_t weights;

    /* Working buffers */
    speaker_recog_buffers_t buffers;

    /* Speaker database */
    speaker_database_t database;

    /* State flags */
    bool initialized;
    bool weights_loaded;
    bool has_current_embedding;

} speaker_recog_config_t;

/* ========== Preprocessing Structures ========== */

/* MFCC preprocessing defaults (from reference MFCC.cpp) */
#define MFCC_DEFAULT_SAMPLE_RATE    16000
#define MFCC_DEFAULT_FRAME_LEN_MS   25      /* 400 samples */
#define MFCC_DEFAULT_HOP_LEN_MS     10      /* 160 samples */
#define MFCC_DEFAULT_NUM_MELS       80      /* NUM_FBANK_BINS */
#define MFCC_DEFAULT_MEL_LOW_FREQ   20.0f   /* MEL_LOW_FREQ */
#define MFCC_DEFAULT_MEL_HIGH_FREQ  8000.0f /* MEL_HIGH_FREQ */
#define MFCC_DEFAULT_NUM_FRAMES     298     /* Maximum frames */

/**
 * @brief Audio preprocessing configuration
 */
typedef struct {
    uint32_t sample_rate;       // Sample rate (default: 16000)
    uint32_t frame_length_ms;   // Frame length in ms (default: 25)
    uint32_t hop_length_ms;     // Hop length in ms (default: 10)
    uint32_t n_fft;             // FFT size (auto-calculated, power of 2)
    uint32_t n_mels;            // Number of Mel bins (default: 80)
    float fmin;                 // Minimum frequency (default: 20)
    float fmax;                 // Maximum frequency (default: 8000)
} speaker_recog_preprocess_config_t;

/* ========== Core API Functions ========== */

/**
 * @brief Initialize speaker recognition module
 *
 * Allocates buffers and loads weights from SD card.
 *
 * @param config Pointer to configuration structure (will be initialized)
 * @param net Network information from IPROAI (can be NULL, will be set later)
 * @param weights_path Path to weights file. NULL = default SD card path,
 *                     "" = skip file loading (caller will provide weights via
 *                     speaker_recog_load_weights_from_buffer()).
 * @return 0 on success, negative on failure
 */
int speaker_recog_init(speaker_recog_config_t* config,
                       struct iproai_net_info_t* net,
                       const char* weights_path);

/**
 * @brief Load weights directly from a memory buffer (zero-copy).
 *
 * Use when weights are embedded in flash via .incbin instead of stored on
 * SD card. The buffer must remain valid for the lifetime of the config —
 * speaker_recog does not copy or own it (deinit will not free it).
 *
 * The buffer layout is identical to the on-disk format read by
 * speaker_recog_init() with a non-empty path: W1, b1, running_mean,
 * running_var, W2, b2 — all float32, little-endian, 4-byte aligned.
 *
 * @param config Configuration structure (already initialized via
 *               speaker_recog_init() with weights_path = "").
 * @param buf    Pointer to weights buffer (e.g. .rodata symbol).
 * @param len    Buffer length in bytes; must be >= the layout size derived
 *               from emb_size and pool_dim.
 * @return 0 on success, negative on failure.
 */
int speaker_recog_load_weights_from_buffer(speaker_recog_config_t* config,
                                           const uint8_t* buf,
                                           uint32_t len);

/**
 * @brief Set network information (if not provided in init)
 *
 * @param config Configuration structure
 * @param net Network information from IPROAI
 * @return 0 on success, negative on failure
 */
int speaker_recog_set_net(speaker_recog_config_t* config, struct iproai_net_info_t* net);

/**
 * @brief Process NPU output to generate speaker embedding
 *
 * Performs: dequantization -> pooling -> segment layers -> embedding
 *
 * @param config Configuration structure
 * @param npu_output Raw NPU output buffer (int8)
 * @param output_size Size of NPU output in bytes
 * @param result Output result structure
 * @return 0 on success, negative on failure
 */
int speaker_recog_process(speaker_recog_config_t* config,
                          const uint8_t* npu_output,
                          uint32_t output_size,
                          speaker_recog_result_t* result);

/**
 * @brief Cleanup speaker recognition module
 *
 * Frees all allocated resources.
 *
 * @param config Configuration structure
 */
void speaker_recog_deinit(speaker_recog_config_t* config);

/* ========== Speaker Management Functions ========== */

/**
 * @brief Register current embedding as a new speaker
 *
 * Must call speaker_recog_process() first to generate current embedding.
 *
 * @param config Configuration structure
 * @param name Speaker name (will be copied)
 * @return Speaker ID on success (0-3), negative on failure
 */
int speaker_recog_register(speaker_recog_config_t* config, const char* name);

/**
 * @brief Unregister a speaker by ID
 *
 * @param config Configuration structure
 * @param speaker_id Speaker ID to remove
 * @return 0 on success, negative on failure
 */
int speaker_recog_unregister(speaker_recog_config_t* config, int speaker_id);

/**
 * @brief Clear all registered speakers
 *
 * @param config Configuration structure
 */
void speaker_recog_clear(speaker_recog_config_t* config);

/**
 * @brief Get number of registered speakers
 *
 * @param config Configuration structure
 * @return Number of registered speakers
 */
int speaker_recog_get_count(const speaker_recog_config_t* config);

/**
 * @brief Get speaker info by ID
 *
 * @param config Configuration structure
 * @param speaker_id Speaker ID
 * @return Pointer to speaker info, or NULL if invalid
 */
const speaker_info_t* speaker_recog_get_speaker(const speaker_recog_config_t* config, int speaker_id);

/* ========== Utility Functions ========== */

/**
 * @brief Print recognition result
 *
 * @param result Result structure to print
 */
void speaker_recog_print_result(const speaker_recog_result_t* result);

/**
 * @brief Print all registered speakers
 *
 * @param config Configuration structure
 */
void speaker_recog_print_speakers(const speaker_recog_config_t* config);

/**
 * @brief Calculate cosine similarity between two embeddings
 *
 * @param emb1 First embedding
 * @param emb2 Second embedding
 * @param size Embedding size
 * @return Cosine similarity (-1 to 1)
 */
float speaker_recog_cosine_similarity(const float* emb1, const float* emb2, int size);
float cosine_int8(const int8_t* a, const int8_t* b, int size);
/* ========== Preprocessing Functions ========== */

/**
 * @brief Initialize audio preprocessing module
 *
 * @param config Preprocessing configuration (NULL for defaults)
 * @return 0 on success, negative on failure
 */
int speaker_recog_preprocess_init(const speaker_recog_preprocess_config_t* config);

/**
 * @brief Deinitialize audio preprocessing module
 */
void speaker_recog_preprocess_deinit(void);

/**
 * @brief Convert PCM audio to model input features
 *
 * @param pcm_data Input PCM data (int16, mono)
 * @param pcm_samples Number of PCM samples
 * @param output Output feature buffer
 * @param output_size Output buffer size in bytes
 * @return 0 on success, negative on failure
 */
int speaker_recog_preprocess_pcm(const int16_t* pcm_data,
                                  int pcm_samples,
                                  float* output,
                                  int output_size);

/**
 * @brief Load PCM from file and convert to features
 *
 * @param pcm_path Path to PCM file
 * @param output Output feature buffer
 * @param output_size Output buffer size in bytes
 * @return 0 on success, negative on failure
 */
int speaker_recog_preprocess_file(const char* pcm_path,
                                   float* output,
                                   int output_size);

/**
 * @brief Get required PCM samples for preprocessing
 *
 * @return Required number of PCM samples
 */
int speaker_recog_preprocess_get_required_samples(void);

/**
 * @brief Get output feature size in bytes
 *
 * @return Output size in bytes
 */
int speaker_recog_preprocess_get_output_size(void);

#ifdef __cplusplus
}
#endif

#endif /* __SPEAKER_RECOG_H__ */

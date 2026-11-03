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
 * @file ipro_ai.h
 * @brief IPRO Edge AI runtime: open .inm models, describe their tensors, run them
 *        on the NPU one at a time.
 *
 * The IPRO7AI NPU has one set of base-address registers and one external-input
 * register, and components/ai writes them only when a model is loaded. With two
 * models loaded, a plain iproai_startCompute() would run the last-loaded
 * model's weights. This runtime keeps one lock for the NPU and, before every
 * run, points the NPU back at the model being run, so any number of models can
 * stay open.
 *
 * Threads: call ipro_ai_init() once before tasks share the runtime. After
 * that, open, invoke and close may be called from any task; they are
 * serialised on the NPU lock, and open holds it for the whole load (tens to
 * hundreds of ms), so other models wait meanwhile. One model handle is not
 * re-entrant: its input buffer and output slot belong to the handle, so calls
 * on one handle from two tasks are the caller's to serialise, and a handle is
 * closed only when no other call on it can be in progress (as with any
 * FreeRTOS object). The lock only protects users of this API: code that still
 * drives components/ai directly (iproai_load_model_*, iproai_startCompute) in
 * the same image must hold ipro_ai_npu_lock() around it.
 *
 * Memory: the runtime copies every section of the .inm into the heap (PSRAM
 * when the app has it), so the .inm may be in XIP flash and need not outlive
 * ipro_ai_model_open(). Input buffers: the runtime's own (cfg.input_bytes) or
 * the caller's (cfg.input / the invoke argument), see ipro_ai_model_cfg_t.
 */
#ifndef IPRO_AI_H
#define IPRO_AI_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Bumped when a struct below changes layout or a call changes meaning. */
#define IPRO_AI_ABI_VERSION  1u

/** Input buffers the runtime allocates start on this boundary. */
#define IPRO_AI_INPUT_ALIGN  32u

/** components/ai addresses an external input as imgBuffer + 4, so a buffer
 *  needs this many addressable bytes before it. The runtime writes the input
 *  size there for code that reads it; the NPU does not. */
#define IPRO_AI_INPUT_HEADER 4u

/** Largest input buffer the runtime allocates (cfg.input_bytes). */
#define IPRO_AI_MAX_INPUT_BYTES (2u * 1024u * 1024u)

/** Errors are negative; 0 is success. */
typedef enum {
    IPRO_AI_OK        = 0,
    IPRO_AI_E_ARG     = -1, /**< bad argument */
    IPRO_AI_E_STATE   = -2, /**< not initialised, or models still open */
    IPRO_AI_E_NOMEM   = -3, /**< heap exhausted */
    IPRO_AI_E_FORMAT  = -4, /**< not an .inm, or it does not fit the bytes given */
    IPRO_AI_E_LOAD    = -5, /**< components/ai refused or mis-decoded the model */
    IPRO_AI_E_RUN     = -6, /**< inference failed */
    IPRO_AI_E_TIMEOUT = -7, /**< the NPU did not finish within the timeout; it was reset and may be used again */
    IPRO_AI_E_FAULT   = -8, /**< the NPU did not come back after a reset: every run is refused (fail closed)
                                 until ipro_ai_npu_clear_fault(); the platform's policy is to restart */
} ipro_ai_err_t;

/** Where the NPU is, as the runtime last left it (ipro_ai_get_npu_health()). A run goes
 *  IDLE -> CONFIGURING (this model's registers) -> RUNNING -> COMPLETED -> IDLE; one that
 *  times out goes RUNNING -> TIMEOUT -> RECOVERING (block reset, registers put back) and then
 *  IDLE, or FAULTED when the reset did not bring it back. Nothing outside the NPU lock ever
 *  sees CONFIGURING, RUNNING or RECOVERING: the lock is released only in IDLE or FAULTED. */
typedef enum {
    IPRO_AI_NPU_IDLE        = 0,
    IPRO_AI_NPU_CONFIGURING = 1,
    IPRO_AI_NPU_RUNNING     = 2,
    IPRO_AI_NPU_COMPLETED   = 3,
    IPRO_AI_NPU_TIMEOUT     = 4,
    IPRO_AI_NPU_RECOVERING  = 5,
    IPRO_AI_NPU_FAULTED     = 6,
} ipro_ai_npu_state_t;

/** NPU health since boot, for every client of the NPU (the runtime, and code that drives
 *  components/ai directly: they share one completion path). */
typedef struct {
    uint8_t  state;                /**< ipro_ai_npu_state_t */
    uint8_t  faulted;              /**< 1: runs are refused until ipro_ai_npu_clear_fault() */
    uint16_t consecutive_timeouts; /**< timeouts since the last segment that finished */
    uint32_t timeout_ms;           /**< default limit for one NPU segment (CONFIG_AI_NPU_TIMEOUT_MS) */
    uint32_t completed;            /**< NPU segments that signalled done */
    uint32_t timeouts;
    uint32_t recoveries;           /**< block resets after a timeout */
    uint32_t refused;              /**< runs refused while faulted */
} ipro_ai_npu_health_t;

/** How a tensor's bytes hold its values. */
typedef enum {
    IPRO_AI_ENC_OFFSET_BINARY = 0, /**< byte = int8 value + 128, i.e. int8 ^ 0x80 */
    IPRO_AI_ENC_UNSIGNED      = 1, /**< byte = uint8 value */
} ipro_ai_enc_t;

typedef enum {
    IPRO_AI_INPUT  = 0,
    IPRO_AI_OUTPUT = 1,
} ipro_ai_dir_t;

typedef struct ipro_ai_model ipro_ai_model_t;

typedef struct {
    const void *inm;         /**< the .inm (XIP flash or RAM) */
    uint32_t    max_bytes;   /**< bytes readable at inm; the model's own size must fit */
    /** External-input models need an input buffer from the moment they load.
     *  Either the runtime allocates one of input_bytes (aligned, header before
     *  it; see ipro_ai_input_buffer()) or the caller passes its own in input,
     *  which must leave IPRO_AI_INPUT_HEADER bytes before it. While it loads,
     *  components/ai cleans the data cache over w x h x (channels rounded up
     *  to 4) bytes from the input - four times a packed one-channel input - so
     *  a caller's buffer must be that large; the runtime's own buffer is
     *  loaded through a scratch area of that size instead. Ignored for models
     *  whose input lives in the work buffer. */
    uint32_t    input_bytes;
    void       *input;
    const char *name;        /**< kept by pointer, for logs and stats */
} ipro_ai_model_cfg_t;

/** One tensor as the NPU leaves it in memory. */
typedef struct {
    uint16_t w, h, c;        /**< logical shape, HWC */
    uint16_t c_pitch;        /**< bytes from one position to the next */
    uint32_t bytes;          /**< w * h * c_pitch */
    uint8_t  encoding;       /**< ipro_ai_enc_t */
    /** real = (byte - zero_point) * scale. 0 when the .inm does not carry the
     *  scale (inputs): take it from the model's metadata. */
    float    scale;
    int32_t  zero_point;
} ipro_ai_tensor_info_t;

typedef struct {
    const char *name;
    uint32_t inm_bytes;      /**< size from the section table, header included */
    uint16_t format_version; /**< DSP_HEADER version field */
    uint16_t layers;
    uint16_t npu_layers;     /**< layers the NPU runs; the rest run on the CPU */
    uint8_t  ext_input;      /**< 1: the input is read from an external buffer */
    uint8_t  outputs;
    uint32_t load_us;
} ipro_ai_model_info_t;

typedef struct {
    uint32_t invokes;
    uint32_t failures;
    uint32_t last_us;        /**< NPU/CPU inference only, input preparation excluded */
    uint32_t max_us;
    uint64_t total_us;
} ipro_ai_stats_t;

typedef struct {
    uint32_t abi_version;
    uint32_t open_models;
    uint32_t invokes;
    uint32_t lock_waits;     /**< runs that found the NPU busy and waited */
} ipro_ai_runtime_info_t;

/** What the section table and DSP_HEADER of an .inm say, without loading it. */
typedef struct {
    uint32_t bytes;          /**< 32-byte section table + sections */
    uint32_t sections[8];    /**< cpu inst, bias, weights, mult/shift; npu inst, bias, weights, mult/shift */
    uint16_t version;
    uint16_t layers;
    uint32_t patch_size;
    uint8_t  ext_input;
    uint8_t  unsign_input;
} ipro_ai_inm_header_t;

/** Idempotent, but not safe against a concurrent first call: call it once at start-up. */
int ipro_ai_init(void);
/** IPRO_AI_E_STATE while a model is open. The NPU lock outlives it. */
int ipro_ai_deinit(void);

/** Parse the section table and DSP_HEADER of an .inm of at most max_bytes. */
int ipro_ai_inm_peek(const void *inm, uint32_t max_bytes, ipro_ai_inm_header_t *hdr);

int  ipro_ai_model_open(const ipro_ai_model_cfg_t *cfg, ipro_ai_model_t **out);
void ipro_ai_model_close(ipro_ai_model_t *m);

int ipro_ai_model_get_info(const ipro_ai_model_t *m, ipro_ai_model_info_t *info);
/** Only index 0 in each direction today (the last layer for outputs);
 *  models with several outputs (detection) report info.outputs > 1 but only
 *  the last layer can be read yet. */
int ipro_ai_tensor_get_info(const ipro_ai_model_t *m, ipro_ai_dir_t dir, uint32_t index,
                            ipro_ai_tensor_info_t *info);

/** Where to write the input: the runtime's buffer (cfg.input_bytes) or
 *  cfg.input for external-input models, the input slot of the work buffer
 *  otherwise; NULL if none. The runtime cleans the cache over it before the
 *  NPU reads it, except over a caller's buffer. */
uint8_t *ipro_ai_input_buffer(ipro_ai_model_t *m);

/**
 * Run the model once.
 *
 * @param input  external-input models: the input to read (NULL: the model's
 *               own buffer); IPRO_AI_INPUT_HEADER bytes before it must exist.
 *               Ignored otherwise. The runtime cleans the data cache over its
 *               own buffer before the NPU reads it; for a caller's buffer
 *               (here or cfg.input) the caller makes the bytes visible: clean
 *               after CPU writes, nothing after a DMA such as the camera's.
 * @param output set to the output tensor in the model's work buffer, cache
 *               invalidated over ipro_ai_tensor_info_t.bytes; valid until the
 *               next run of this model.
 */
int ipro_ai_invoke(ipro_ai_model_t *m, const void *input, const uint8_t **output);

/**
 * ipro_ai_invoke() with a limit on how long the NPU may take: every wait for the NPU to
 * finish a segment of layers ends after timeout_ms (0: the default, CONFIG_AI_NPU_TIMEOUT_MS).
 * The whole run is then bounded by (NPU segments x timeout_ms) plus its CPU layers.
 *
 * @return IPRO_AI_E_TIMEOUT when the NPU did not finish: the NPU was stopped, reset and its
 *         registers put back before the lock was released, *output is not set, and the next
 *         run may proceed. IPRO_AI_E_FAULT when a second timeout in a row showed the reset did
 *         not help, and for every run after that until ipro_ai_npu_clear_fault().
 */
int ipro_ai_invoke_timeout(ipro_ai_model_t *m, const void *input, const uint8_t **output,
                           uint32_t timeout_ms);

int ipro_ai_model_get_stats(const ipro_ai_model_t *m, ipro_ai_stats_t *stats);
int ipro_ai_get_runtime_info(ipro_ai_runtime_info_t *info);

/** NPU state and timeout counters (works before ipro_ai_init(): it reads components/ai). */
int ipro_ai_get_npu_health(ipro_ai_npu_health_t *health);
/** Leave FAULTED: reset the NPU block again and accept runs. For tests and for an app that
 *  restarts its pipeline; the supported recovery from a fault is a restart of the device. */
int ipro_ai_npu_clear_fault(void);

/** The NPU lock, for code in the same image that still calls components/ai
 *  directly. Not re-entrant: never call ipro_ai_* while holding it. */
void ipro_ai_npu_lock(void);
void ipro_ai_npu_unlock(void);

const char *ipro_ai_strerror(int err);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_AI_H */

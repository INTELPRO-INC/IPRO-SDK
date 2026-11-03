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
 * @file ipro_ai_source.h
 * @brief Data sources for on-device inference: one lifecycle, source-specific frames.
 *
 * A source is a named entry with an ops table. The framework owns the
 * registry, the lifecycle (closed -> open -> running), frame indices and
 * timestamps, and stall detection; a source only moves data.
 *
 *   kind     what read() fills
 *   SAMPLES  int16 frames, channels interleaved, at a fixed rate
 *            (simulators, the PDM microphone, an I2C/SPI sensor, UART data)
 *   IMAGE    ipro_ai_frame_t descriptors: the image stays where the
 *            producer put it (the camera's ISP buffer), read() hands out its
 *            address - no copy through a generic layout
 *
 * Two helpers build common sources without writing an ops table:
 *
 *   ipro_ai_source_ring_*     push: a producer (an ISR, a UART callback,
 *                             host PCM, application data) calls _push, the
 *                             consumer reads; single producer, single
 *                             consumer, no lock
 *   ipro_ai_source_sampled_*  polled: a task calls the application's
 *                             sample() at a fixed rate (I2C/SPI registers)
 *                             and pushes into a ring
 *
 * Threads: register and unregister at start-up or from one task; each open
 * source has one owner task (the one that reads it). Different sources may
 * be open at the same time.
 */
#ifndef IPRO_AI_SOURCE_H
#define IPRO_AI_SOURCE_H

#include <stdbool.h>
#include <stdint.h>

#include "ipro_ai.h"                   /* IPRO_AI_OK, IPRO_AI_E_* */

#ifdef __cplusplus
extern "C" {
#endif

#define IPRO_AI_SOURCE_MAX       16u
#define IPRO_AI_SOURCE_NAME_MAX  15u

typedef enum { IPRO_AI_SRC_SAMPLES = 0, IPRO_AI_SRC_IMAGE } ipro_ai_src_kind_t;

/* capability bits */
#define IPRO_AI_SRC_CAP_SIMULATED  0x01u  /* synthetic data */
#define IPRO_AI_SRC_CAP_RATE       0x02u  /* cfg.rate is honoured */
#define IPRO_AI_SRC_CAP_CHANNELS   0x04u  /* cfg.channels is honoured */
#define IPRO_AI_SRC_CAP_PUSH       0x08u  /* fed by a producer; may stall */
#define IPRO_AI_SRC_CAP_TIMESTAMP  0x10u  /* the source stamps frames itself */

typedef enum {
    IPRO_AI_SRC_CLOSED = 0,
    IPRO_AI_SRC_OPEN,
    IPRO_AI_SRC_RUNNING,
} ipro_ai_src_state_t;

typedef struct {
    ipro_ai_src_kind_t kind;
    uint32_t caps;                     /* IPRO_AI_SRC_CAP_* */
    uint32_t min_rate, max_rate;       /* SAMPLES: Hz per channel; IMAGE: frames per second */
    uint32_t max_channels;
} ipro_ai_source_caps_t;

typedef struct {
    ipro_ai_src_kind_t kind;
    uint32_t rate;                     /* as opened */
    uint32_t channels;                 /* SAMPLES */
    uint32_t frame_bytes;              /* bytes read() writes per frame */
    float    lsb_per_unit;             /* SAMPLES: int16 counts per physical unit */
    const char *unit;                  /* "g", "dps", "fs" (full scale), ... */
    uint32_t width, height, format;    /* IMAGE: ipro_ai_pixfmt_t (ipro_ai_preproc.h) */
} ipro_ai_source_format_t;

typedef struct {
    uint32_t rate;                     /* 0: the source's default */
    uint32_t channels;                 /* 0: all */
    const void *opts;                  /* source-specific, documented by the source */
} ipro_ai_source_cfg_t;

/** IMAGE sources: one frame, by address. Valid until the next read or stop. */
typedef struct {
    const uint8_t *data;
    uint32_t width, height, stride, format;
} ipro_ai_frame_t;

typedef struct {
    uint64_t t_us;                     /* time of the first frame returned */
    uint64_t index;                    /* its index since start, earlier losses counted */
    uint32_t lost;                     /* frames lost since the previous read, which come
                                        * after the frames returned now (a full push ring
                                        * drops the newest; the next index skips them) */
    bool     stalled;                  /* no data for longer than the stall limit */
} ipro_ai_source_meta_t;

typedef struct {
    ipro_ai_src_state_t state;
    uint64_t frames;                   /* delivered since start */
    uint64_t lost;                     /* reported lost since start */
    uint32_t reads, empty_reads, stalls;
} ipro_ai_source_stats_t;

/**
 * Ops a source implements. read() returns frames written (0 when none
 * arrived within timeout_ms, which it may round up to a scheduler tick) or a
 * negative IPRO_AI_E_*; *lost is set to frames dropped since the previous
 * read (counted after the ones returned) and *t_us to their capture time when
 * the source has CAP_TIMESTAMP. Optional entries
 * may be NULL: start, stop (no-ops), format (from caps and cfg).
 */
typedef struct {
    int     (*open)(void *ctx, const ipro_ai_source_cfg_t *cfg);
    int     (*start)(void *ctx);
    int32_t (*read)(void *ctx, void *dst, uint32_t max_frames, uint32_t timeout_ms,
                    uint32_t *lost, uint64_t *t_us);
    int     (*stop)(void *ctx);
    void    (*close)(void *ctx);
    void    (*caps)(void *ctx, ipro_ai_source_caps_t *caps);
    int     (*format)(void *ctx, ipro_ai_source_format_t *fmt);
} ipro_ai_source_ops_t;

typedef struct ipro_ai_source ipro_ai_source_t;

/* ---------------------------------------------------------------- registry */

/** Add a source. IPRO_AI_E_ARG for a bad or taken name, E_NOMEM when full. */
int ipro_ai_source_register(const char *name, const ipro_ai_source_ops_t *ops, void *ctx);
/** IPRO_AI_E_STATE while it is open. */
int ipro_ai_source_unregister(const char *name);
uint32_t ipro_ai_source_count(void);
/** Slot order (registration order until one is removed); NULL past the end. */
ipro_ai_source_t *ipro_ai_source_at(uint32_t i);
ipro_ai_source_t *ipro_ai_source_find(const char *name);
const char *ipro_ai_source_name(const ipro_ai_source_t *s);
void *ipro_ai_source_ctx(const ipro_ai_source_t *s);

/* ---------------------------------------------------------------- lifecycle */

int  ipro_ai_source_get_caps(ipro_ai_source_t *s, ipro_ai_source_caps_t *caps);
/** The format as opened; before open, the source's defaults. */
int  ipro_ai_source_get_format(ipro_ai_source_t *s, ipro_ai_source_format_t *fmt);
int  ipro_ai_source_open(ipro_ai_source_t *s, const ipro_ai_source_cfg_t *cfg);
int  ipro_ai_source_start(ipro_ai_source_t *s);
/**
 * Up to max_frames frames into dst (SAMPLES: int16 x channels each;
 * IMAGE: ipro_ai_frame_t each). Returns the count, 0 on timeout, or
 * IPRO_AI_E_STATE when not running. meta may be NULL.
 */
int32_t ipro_ai_source_read(ipro_ai_source_t *s, void *dst, uint32_t max_frames,
                            uint32_t timeout_ms, ipro_ai_source_meta_t *meta);
int  ipro_ai_source_stop(ipro_ai_source_t *s);
/** Stops first when running; harmless when closed. */
void ipro_ai_source_close(ipro_ai_source_t *s);
ipro_ai_src_state_t ipro_ai_source_state(const ipro_ai_source_t *s);
int  ipro_ai_source_get_stats(const ipro_ai_source_t *s, ipro_ai_source_stats_t *st);
/** No data for this long while running counts a stall (default 500 ms). */
void ipro_ai_source_set_stall_ms(ipro_ai_source_t *s, uint32_t ms);

/* ---------------------------------------------------------------- push ring */

/** A SAMPLES source fed by ipro_ai_source_ring_push. The caller provides the
 *  storage (capacity frames x channels int16; keep it alive while registered). */
typedef struct {
    int16_t *buf;
    uint32_t capacity;                 /* frames, a power of two */
    uint32_t channels;
    uint32_t rate;                     /* nominal, for timestamps and the format */
    float    lsb_per_unit;
    const char *unit;
    /* the rest is the ring's own state: head, lost_in and t_push are the
     * producer's, tail and lost_out the consumer's */
    volatile uint32_t head, tail;      /* frames written / read, free-running */
    volatile uint32_t lost_in;         /* frames refused while full, ever */
    uint32_t          lost_out;        /* of those, reported by read */
    volatile uint32_t t_push;          /* low 32 bits of the time of the newest push, us */
    volatile bool     running;
} ipro_ai_source_ring_t;

/** Register ring as a source called name (rate, channels, buf, capacity set). */
int ipro_ai_source_ring_register(const char *name, ipro_ai_source_ring_t *ring);
/**
 * Producer side; safe from an ISR. Takes up to `frames` frames, returns how
 * many fit; the rest count as lost. Ignored (0) while the source is not
 * running.
 */
uint32_t ipro_ai_source_ring_push(ipro_ai_source_ring_t *ring, const int16_t *frames, uint32_t n);

/* ---------------------------------------------------------------- sampled */

/** Read one frame (channels int16) from the device; 0 on success. */
typedef int (*ipro_ai_sample_fn)(void *user, int16_t *frame);

typedef struct {
    ipro_ai_source_ring_t ring;        /* buf, capacity, channels, rate, lsb_per_unit, unit */
    ipro_ai_sample_fn sample;
    void    *user;
    uint32_t errors;                   /* sample() failures (counted as lost) */
    void    *task;                     /* the port's sampling task */
} ipro_ai_source_sampled_t;

/** Register a polled source; its task runs only while the source is running.
 *  rate up to the scheduler tick rate (1 kHz on the IPRO7 FreeRTOS port). */
int ipro_ai_source_sampled_register(const char *name, ipro_ai_source_sampled_t *s);
/** One sampling step (what the task does each period). For tests and for
 *  applications that already own a timer. */
void ipro_ai_source_sampled_step(ipro_ai_source_sampled_t *s);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_AI_SOURCE_H */

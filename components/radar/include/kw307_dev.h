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

/*
 * KW307 device layer for IPRO7 - binds the codec to UART1 and a pair of pads.
 *
 * UART0 is the console, so the radar gets UART1. On IPRO7 any pad can carry a
 * UART signal: the pad's function is set to GPIO_FUN_UART and the signal slot
 * (gpio % 12) is pointed at UART1_TXD / UART1_RXD.
 *
 * >>> LEVELS: the KW307 is 3.3 V logic and explicitly NOT 5 V tolerant. The
 * pads chosen here must sit in a bank whose VDDIO is 3.3 V. On the IPRO7AI EVK
 * the VDDIO_1 bank (GPIO0-9, GPIO36-41) is jumpered to 1.8 V for the camera -
 * driving it from the module's 3.3 V TX exceeds the pad rating. See README. <<<
 */
#ifndef __KW307_DEV_H__
#define __KW307_DEV_H__

#include <stdbool.h>
#include <stdint.h>

#include "kw307_proto.h"

#define KW307_DEV_BAUD          115200
#define KW307_DEV_ACK_TIMEOUT   300     /* ms; the module answers in a frame or two */

typedef void (*kw307_frame_cb_t)(const kw307_frame_t *frame, void *arg);

typedef struct {
    uint32_t frames;            /* CMD 0xF0 frames decoded                    */
    uint32_t missed;            /* gaps counted from the frame index          */
    uint32_t crc_errors;
    uint32_t dropped_bytes;
    uint32_t short_frames;      /* CMD 0xF0 with a length other than 27       */
    uint32_t rx_overruns;       /* ring full: the task did not drain in time  */
    uint32_t other_frames;      /* ACKs and read replies                      */
} kw307_stats_t;

/* Bring UART1 up on the given pads. gpo_pin < 0 leaves the presence pin unused.
 * Returns 0 on success. */
int  kw307_dev_init(int tx_pin, int rx_pin, int gpo_pin);
void kw307_dev_set_frame_cb(kw307_frame_cb_t cb, void *arg);

/* Drain the RX ring, decode, and dispatch. Call from a task, not an ISR. */
void kw307_dev_poll(void);

/* Blocking command helpers. Return 0 on ACK OK, -1 on timeout, or the positive
 * ACK status byte when the module rejects the command. */
int kw307_dev_write_cmd(uint8_t cmd, const uint8_t *data, uint8_t len);
int kw307_dev_read_cmd(uint8_t cmd, uint8_t *out, uint8_t out_max, uint8_t *out_len);
int kw307_dev_set_param(uint8_t pid, uint16_t value);

int kw307_dev_set_power_mode(bool normal);
int kw307_dev_set_gain(uint8_t index);
int kw307_dev_get_gain(uint8_t *index);
int kw307_dev_set_detect_range(uint16_t min_cm, uint16_t max_cm);
int kw307_dev_set_detect_fov(uint16_t fov_deg);
int kw307_dev_set_output_mode(uint8_t mode);

/* Raw presence pin, or -1 when no GPO pad was configured. */
int  kw307_dev_gpo_level(void);

const kw307_stats_t *kw307_dev_stats(void);
void kw307_dev_stats_reset(void);
int  kw307_dev_tx_pin(void);
int  kw307_dev_rx_pin(void);

#endif /* __KW307_DEV_H__ */

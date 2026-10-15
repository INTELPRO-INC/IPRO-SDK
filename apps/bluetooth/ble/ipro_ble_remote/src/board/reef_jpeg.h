/**
 * @file reef_jpeg.h
 * @brief One-shot JPEG encode of a camera frame with the IPRO7 MJPEG block
 *
 * The camera streams YUYV into two PSRAM ping-pong buffers (reef demo cam.c);
 * a capture points the MJPEG encoder at the newest one and runs a single SW
 * frame-mode encode, as the other IPRO7 camera pipelines do.
 * The encoder writes header and tail itself, so the result is a complete
 * JPEG file.
 *
 * Task context only (it blocks on the encoder's completion interrupt).
 * Copyright (c) 2026 INTELPRO
 */

#ifndef REEF_JPEG_H
#define REEF_JPEG_H

#include <stdint.h>

/** @brief Set the encoder up for @p width x @p height YUYV at @p quality (1..100). */
int reef_jpeg_init(uint16_t width, uint16_t height, uint8_t quality);

/** @brief Release the encoder and its output buffer. */
void reef_jpeg_deinit(void);

/**
 * @brief Encode the YUYV frame at @p yuv_addr into @p out.
 *
 * @return JPEG length in bytes, or a negative error (-1 not initialised,
 *         -2 encoder timeout, -3 no frame from the encoder, -4 too big).
 */
int32_t reef_jpeg_encode(uint32_t yuv_addr, uint8_t *out, uint32_t out_max);

#endif /* REEF_JPEG_H */

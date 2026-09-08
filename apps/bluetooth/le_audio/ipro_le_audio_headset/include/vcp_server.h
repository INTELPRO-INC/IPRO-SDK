/**
 * @file vcp_server.h
 * @brief VCP Volume Renderer API
 *
 * Copyright (C) IPRO SDK 2026
 */

#ifndef VCP_SERVER_H
#define VCP_SERVER_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initialize VCP Volume Renderer
 *
 * Registers VCP service and enables remote volume control.
 *
 * @return 0 on success, negative error code on failure
 */
int vcp_server_init(void);

/**
 * @brief Get current volume level
 *
 * @return Volume level (0-255)
 */
uint8_t vcp_server_get_volume(void);

/**
 * @brief Get current mute state
 *
 * @return true if muted, false otherwise
 */
bool vcp_server_is_muted(void);

#endif /* VCP_SERVER_H */

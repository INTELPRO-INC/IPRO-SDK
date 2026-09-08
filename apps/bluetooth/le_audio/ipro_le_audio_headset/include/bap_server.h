/**
 * @file bap_server.h
 * @brief BAP Unicast Server API
 *
 * Copyright (C) IPRO SDK 2026
 */

#ifndef BAP_SERVER_H
#define BAP_SERVER_H

/**
 * @brief Initialize BAP Unicast Server
 *
 * Registers BAP callbacks with ASCS and prepares for audio streaming.
 *
 * @return 0 on success, negative error code on failure
 */
int bap_server_init(void);

#endif /* BAP_SERVER_H */

/**
 * @file cap_acceptor.h
 * @brief CAP Acceptor API
 *
 * Copyright (C) IPRO SDK 2026
 */

#ifndef CAP_ACCEPTOR_H
#define CAP_ACCEPTOR_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initialize CAP Acceptor
 *
 * Registers Common Audio Service with CSIP Set Member.
 *
 * @return 0 on success, negative error code on failure
 */
int cap_acceptor_init(void);

/**
 * @brief Generate the current CAP/CSIP RSI advertising payload.
 *
 * @param rsi 6-byte output buffer
 * @return true if CSIP set member is registered and RSI was generated
 */
bool cap_acceptor_generate_rsi(uint8_t rsi[6]);

#endif /* CAP_ACCEPTOR_H */

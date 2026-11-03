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
 * @file ipro_ai_store.h
 * @brief Model slots in flash: find the model to load, install a new one so
 *        that a power cut never leaves half a model to load.
 *
 * A slot is a range of a partition (the Teach tools' mfg slots). It holds one
 * bank, or two when it is large enough to keep the old model while the new
 * one is written (A/B). Each bank ends with a 4 KB record sector:
 *
 *   payload ...                    | record (32 B at the start of the last sector)
 *   bank start (V1.1 offset)       | magic, version, kind, seq, bytes, crc32,
 *                                  | format, header crc, commit word
 *
 * Install: erase the record sector, write the record with the commit word
 * still erased (pending), erase / write / read back the payload, then program
 * the commit word. NOR flash programs 1 -> 0 only, so the commit word can be
 * written last without erasing the record. A cut at any point leaves either
 * the previous model (A/B, or before the old payload was touched), the new
 * one, or a bank that reports no model - never a payload that is partly old
 * and partly new.
 *
 * A bank with no record at all is a V1.1 board's slot: its payload is
 * accepted when the slot's legacy check recognises it, and reported as
 * IPRO_AI_SLOT_LEGACY (no CRC to check it against).
 *
 * Read the payload through the XIP pointer only after a reboot that follows
 * an install (the apps reboot after every deploy): the flash cache may still
 * hold the old bytes in the same boot.
 */
#ifndef IPRO_AI_STORE_H
#define IPRO_AI_STORE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IPRO_AI_STORE_SECTOR       4096u
#define IPRO_AI_STORE_RECORD_MAGIC 0x524D5049u   /* "IPMR" */
#define IPRO_AI_STORE_COMMIT       0x544D4F43u   /* "COMT", programmed last */
#define IPRO_AI_STORE_VERSION      1u

/** What a slot holds; recorded so that a slot is never loaded as the wrong kind. */
typedef enum {
    IPRO_AI_KIND_IMAGE  = 1,   /* .inm + TCH1 labels */
    IPRO_AI_KIND_VOICE  = 2,   /* SPKB */
    IPRO_AI_KIND_SENSOR = 3,   /* .inm + SNS1 */
} ipro_ai_kind_t;

typedef enum {
    IPRO_AI_SLOT_NONE      = 0, /**< nothing loadable */
    IPRO_AI_SLOT_LEGACY    = 1, /**< a V1.1 payload, no record: not CRC-checked */
    IPRO_AI_SLOT_COMMITTED = 2, /**< a committed record whose payload CRC matches */
} ipro_ai_slot_state_t;

typedef struct {
    const char *partition;     /**< e.g. "mfg" */
    uint32_t    offset;        /**< from the partition start, sector aligned */
    uint32_t    size;          /**< 0: to the partition end */
    uint8_t     banks;         /**< 1, or 2 for A/B (each bank half the slot, sector aligned) */
    uint16_t    kind;          /**< ipro_ai_kind_t */
    /** Size of the V1.1 payload at the start of bank A, 0 if it is not one. room = bytes readable. */
    uint32_t  (*legacy_size)(const uint8_t *blob, uint32_t room);
} ipro_ai_slot_t;

typedef struct {
    const uint8_t       *data;   /**< XIP pointer to the payload, NULL when state is NONE */
    uint32_t             bytes;
    uint32_t             crc;    /**< recorded CRC32 (0 for LEGACY) */
    uint32_t             seq;
    uint32_t             format;
    uint8_t              bank;   /**< 0 = A, 1 = B */
    uint8_t              state;  /**< ipro_ai_slot_state_t */
    const char          *why;    /**< for NONE: why nothing is loadable */
} ipro_ai_blob_t;

/** Largest payload one bank takes (bank minus its record sector); 0 if the slot is unusable. */
uint32_t ipro_ai_store_room(const ipro_ai_slot_t *slot);

/** Find what to load: the newest committed bank whose CRC matches, else a V1.1 payload. */
int ipro_ai_store_open(const ipro_ai_slot_t *slot, ipro_ai_blob_t *out);

/**
 * Install bytes of payload. With two banks it goes to the bank not in use, so
 * the current model stays loadable until the new one is committed. Returns 0,
 * or a negative value with *why set to one of "args", "slot", "size",
 * "erase", "write", "verify", "commit".
 */
int ipro_ai_store_install(const ipro_ai_slot_t *slot, const uint8_t *data, uint32_t bytes,
                          uint32_t format, const char **why);

/**
 * Make the slot empty. Every bank's first payload sector is erased before the
 * records, the older bank's first, so a cut leaves the model being removed or
 * nothing: never the older model back, never a payload that would pass for a
 * V1.1 one. Returns 0, or a negative value with *why "slot" or "erase".
 */
int ipro_ai_store_remove(const ipro_ai_slot_t *slot, const char **why);

/**
 * Two banks: give up the newest committed model, so that the previous one
 * (committed, CRC checked now) is what loads after the next reboot. The
 * newest bank's record sector is erased, then its payload's first sector. *bank and *seq (may be NULL) report
 * the model that will load. Refused, with nothing written, as "slot",
 * "single_bank", "no_model", "no_previous" or "previous_damaged"; "erase"
 * when the flash failed.
 */
int ipro_ai_store_rollback(const ipro_ai_slot_t *slot, uint8_t *bank, uint32_t *seq, const char **why);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_AI_STORE_H */

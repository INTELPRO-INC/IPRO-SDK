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

#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Bounded wire codec. Callers own all buffers; inputs and output must not
 * overlap. No allocation, network, clock or OpenThread calls occur here. */
#define BR_DNS64_MAX_PACKET 1232U
enum br_dns64_action { BR_DNS64_INVALID = -1, BR_DNS64_PASS = 0, BR_DNS64_QUERY_A = 1, BR_DNS64_FILTERED = 2 };
#define BR_DNS64_MAX_EXCLUSIONS 8U
struct br_dns64_prefix { uint8_t address[16]; uint8_t length; };
#define BR_PREF64_MAX_PREFIXES 8U
/* RFC 7050 response codec. Validates the complete ipv4only.arpa AAAA reply,
 * collects distinct unambiguous prefixes in wire order, and returns a cache
 * lifetime (including SOA negative caching). False leaves count/ttl zero. */
bool br_dns64_discover(const uint8_t *response, size_t length,
    struct br_dns64_prefix prefixes[BR_PREF64_MAX_PREFIXES], size_t *count, uint32_t *ttl);
typedef bool (*br_dns64_embed)(void *context, const uint8_t ip4[4], uint8_t ip6[16]);

/* AAAA response classification. PASS leaves the original response untouched.
 * QUERY_A writes a copy of the original query with only QTYPE changed to A.
 * FILTERED writes an AAAA response containing the remaining native addresses.
 * ttl_limit is the negative SOA lifetime (600 seconds when absent). */
enum br_dns64_action br_dns64_prepare(const uint8_t *query, size_t query_length,
    const uint8_t *response, size_t response_length, uint8_t *a_query, size_t capacity,
    size_t *length, uint32_t *ttl_limit);

/* Adds up to eight site ranges to the default ::ffff:0:0/96 exclusion. Prefix
 * lengths 0..128 are supported; bits outside the prefix are ignored. Policy
 * arrays must remain immutable for the call. DO+CD bypasses DNS64 rewriting. */
enum br_dns64_action br_dns64_prepare_with_policy(const uint8_t *query, size_t query_length,
    const uint8_t *response, size_t response_length, const struct br_dns64_prefix *exclusions,
    size_t exclusion_count, uint8_t *output, size_t capacity, size_t *length, uint32_t *ttl_limit);

/* Assemble the A-phase result as the original AAAA question, expanding names
 * so relocated RDATA never retains stale compression pointers. False means no
 * complete response fits/is valid; partial output must never be transmitted. */
bool br_dns64_finish(const uint8_t *a_query, size_t query_length,
    const uint8_t *response, size_t response_length, uint32_t ttl_limit,
    br_dns64_embed embed, void *context, uint8_t *output, size_t capacity, size_t *length);

/* PTR wire operations. The caller must match the decoded IPv6 address against
 * site NAT64 prefixes and extract IPv4 before creating the translated query.
 * These functions alone do not select prefixes or send/complete a query. */
bool br_dns64_ptr_address(const uint8_t *query, size_t query_length, uint8_t ip6[16]);
/* Recognizes a question independently of trailing records, so malformed or
 * unsupported site reverse queries can fail closed before upstream TX. */
bool br_dns64_ptr_question_address(const uint8_t *query, size_t query_length, unsigned question, uint8_t ip6[16]);
bool br_dns64_ptr_query(const uint8_t *query, size_t query_length, const uint8_t ip4[4],
    uint8_t *output, size_t capacity, size_t *length);
bool br_dns64_ptr_finish(const uint8_t *original, size_t original_length,
    const uint8_t *translated, size_t translated_length, const uint8_t *response, size_t response_length,
    uint8_t *output, size_t capacity, size_t *length);

/* Normalize DNAME chains before alias continuation or DNS64 assembly.
 * 1 writes missing CNAMEs (or YXDOMAIN for an overlong substitution), 0 leaves
 * the response unchanged, -1 rejects malformed/conflicting/unbounded chains.
 * DO+CD and unsupported signed transactions are not rewritten. */
int br_dns64_dname_expand(const uint8_t *query, size_t query_length,
    const uint8_t *response, size_t response_length, uint8_t *output, size_t capacity, size_t *length);

/* Follow a positive, incomplete CNAME chain with the same question type.
 * 1 writes the next query, 0 needs no continuation, -1 rejects malformed or
 * unsupported continuation. A terminal SOA is a negative result, not a retry. */
int br_dns64_alias_query(const uint8_t *query, size_t query_length,
    const uint8_t *response, size_t response_length, uint8_t *output, size_t capacity, size_t *length);
/* Join a retained partial response to its matched continuation. Prior Answer
 * TTLs age by elapsed seconds; final Authority/Additional come from the new
 * response. Restores the first question/ID and clears AA/AD. */
bool br_dns64_alias_merge(const uint8_t *original, size_t original_length,
    const uint8_t *partial, size_t partial_length, const uint8_t *query, size_t query_length,
    const uint8_t *response, size_t response_length, uint32_t elapsed,
    uint8_t *output, size_t capacity, size_t *length);

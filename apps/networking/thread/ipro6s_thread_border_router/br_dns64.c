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

#include "br_dns64.h"
#include <string.h>

/* Expand compressed names instead of adjusting pointer offsets after A grows
 * from four to sixteen bytes. No recursion or attacker-sized stack arrays. */
struct wire { const uint8_t *data; size_t length; };
struct writer { uint8_t *data; size_t length, capacity; };
struct record { uint8_t name[255]; size_t name_length, data, end; uint16_t type, klass; uint32_t ttl; };
struct packet {
    uint8_t name[255]; size_t name_length, question_end;
    uint16_t type, klass, counts[3], rcode;
    bool do_bit, unsupported;
};
static uint16_t u16(const uint8_t *p) { return (uint16_t)((p[0] << 8) | p[1]); }
static uint32_t u32(const uint8_t *p) { return ((uint32_t)u16(p) << 16) | u16(p + 2); }
static void set16(uint8_t *p, unsigned n) { p[0] = n >> 8; p[1] = n; }
static void set32(uint8_t *p, uint32_t n) { set16(p, n >> 16); set16(p + 2, n); }
static bool name_read(struct wire w, size_t *offset, uint8_t name[255], size_t *length)
{
    size_t pos = *offset, used = 0, steps = 0;
    bool jumped = false;
    while (pos < w.length && ++steps <= w.length) {
        uint8_t n = w.data[pos++];
        if ((n & 0xc0) == 0xc0) {
            if (pos >= w.length) return false;
            size_t target = ((size_t)(n & 63) << 8) | w.data[pos++];
            if (target < 12 || target >= pos - 2) return false;
            if (!jumped) *offset = pos;
            jumped = true; pos = target; continue;
        }
        if (n > 63 || used + 1 + n > 255 || pos + n > w.length) return false;
        name[used++] = n;
        memcpy(name + used, w.data + pos, n); used += n; pos += n;
        if (!n) {
            if (!jumped) *offset = pos;
            *length = used; return true;
        }
    }
    return false;
}
static bool name_equal(const uint8_t *a, size_t an, const uint8_t *b, size_t bn)
{
    if (an != bn) return false;
    for (size_t i = 0; i < an; ++i) {
        uint8_t x = a[i], y = b[i];
        if (x >= 'A' && x <= 'Z') x += 'a' - 'A';
        if (y >= 'A' && y <= 'Z') y += 'a' - 'A';
        if (x != y) return false;
    }
    return true;
}
static bool name_below(const uint8_t *name, size_t n, const uint8_t *zone, size_t z)
{
    for (size_t pos = 0; pos < n; pos += name[pos] + 1U) {
        if (name_equal(name + pos, n - pos, zone, z)) return true;
        if (!name[pos]) break;
    }
    return false;
}
static bool bytes(struct writer *out, const void *data, size_t length)
{
    if (length > out->capacity - out->length) return false;
    if (out->data) memcpy(out->data + out->length, data, length);
    out->length += length; return true;
}
static bool record_read(struct wire w, size_t *pos, struct record *rr)
{
    if (!name_read(w, pos, rr->name, &rr->name_length) || *pos + 10 > w.length) return false;
    rr->type = u16(w.data + *pos); rr->klass = u16(w.data + *pos + 2); rr->ttl = u32(w.data + *pos + 4);
    rr->data = *pos + 10; rr->end = rr->data + u16(w.data + *pos + 8);
    if (rr->end > w.length) return false;
    *pos = rr->end; return true;
}
static bool rdata_write(struct wire w, const struct record *rr, struct writer *out)
{
    size_t pos = rr->data, prefix = 0;
    unsigned names = 0;
    int tail = 0; /* -1: arbitrary non-name tail, otherwise exact byte count. */
    switch (rr->type) {
    case 1: return rr->end - pos == 4 && bytes(out, w.data + pos, 4);
    case 28: return rr->end - pos == 16 && bytes(out, w.data + pos, 16);
    case 39: { /* RFC 6672: DNAME's target MUST NOT use compression. */
        size_t at = pos;
        while (at < rr->end) {
            uint8_t n = w.data[at++];
            if (n > 63 || at + n > rr->end || at + n - pos > 255) return false;
            at += n;
            if (!n) return at == rr->end && bytes(out, w.data + pos, at - pos);
        }
        return false;
    }
    case 2: case 3: case 4: case 5: case 7: case 8: case 9: case 12: case 23:
        names = 1; break;
    case 6: names = 2; tail = 20; break;
    case 14: case 17: names = 2; break;
    case 15: case 18: case 21: case 36: prefix = 2; names = 1; break;
    case 26: prefix = 2; names = 2; break;
    case 33: prefix = 6; names = 1; break;
    case 24: case 46: prefix = 18; names = 1; tail = -1; break;
    case 30: case 47: names = 1; tail = -1; break;
    case 35: /* NAPTR: order, preference, three character strings, replacement. */
        prefix = 4;
        for (unsigned i = 0; i < 3; ++i) {
            if (pos + prefix >= rr->end) return false;
            prefix += 1U + w.data[pos + prefix];
        }
        names = 1; break;
    case 38: case 249: case 250:
        /* Obsolete A6 and transaction-authentication records are not rewritten. */
        return false;
    default:
        /* Unknown RR types cannot use DNS compression (RFC 3597). */
        return bytes(out, w.data + pos, rr->end - pos);
    }
    if (prefix > rr->end - pos || !bytes(out, w.data + pos, prefix)) return false;
    pos += prefix;
    for (unsigned i = 0; i < names; ++i) {
        uint8_t name[255]; size_t length;
        if (!name_read(w, &pos, name, &length) || pos > rr->end || !bytes(out, name, length)) return false;
    }
    return (tail < 0 || rr->end - pos == (size_t)tail) && bytes(out, w.data + pos, rr->end - pos);
}
static bool packet_read(struct wire w, struct packet *packet)
{
    memset(packet, 0, sizeof(*packet));
    if (!w.data || w.length < 12 || w.length > BR_DNS64_MAX_PACKET || u16(w.data + 4) != 1) return false;
    size_t pos = 12;
    if (!name_read(w, &pos, packet->name, &packet->name_length) || pos + 4 > w.length) return false;
    packet->type = u16(w.data + pos); packet->klass = u16(w.data + pos + 2);
    pos += 4; packet->question_end = pos; packet->rcode = w.data[3] & 15;
    bool opt = false;
    unsigned total = 0;
    for (unsigned section = 0; section < 3; ++section) {
        packet->counts[section] = u16(w.data + 6 + 2 * section);
        total += packet->counts[section];
        if (total > 128) return false;
        for (unsigned i = 0; i < packet->counts[section]; ++i) {
            struct record rr;
            if (!record_read(w, &pos, &rr)) return false;
            if (rr.type == 24 || rr.type == 38 || rr.type == 249 || rr.type == 250) {
                packet->unsupported = true; continue;
            }
            struct writer check = {NULL, 0, 65535};
            if (!rdata_write(w, &rr, &check)) return false;
            if (rr.type == 41) {
                if (opt || section != 2 || rr.name_length != 1) return false;
                opt = true; packet->do_bit = (rr.ttl & 0x8000) != 0;
                packet->rcode |= (rr.ttl >> 24) << 4;
                if (rr.ttl & 0x00ff0000) packet->unsupported = true; /* EDNS version */
                size_t at = rr.data;
                while (at < rr.end) {
                    if (rr.end - at < 4 || u16(w.data + at + 2) > rr.end - at - 4) return false;
                    at += 4U + u16(w.data + at + 2);
                }
            }
        }
    }
    return pos == w.length;
}
static bool response_for(struct wire q, const struct packet *qp, struct wire r, const struct packet *rp)
{
    return !(q.data[2] & 0xf8) && (r.data[2] & 0xfa) == 0x80 &&
        u16(q.data) == u16(r.data) && qp->type == rp->type && qp->klass == rp->klass &&
        name_equal(qp->name, qp->name_length, rp->name, rp->name_length);
}
struct alias_target { uint8_t name[255]; size_t length; uint32_t ttl; bool synthetic; };
/* 0: terminal, 1: alias, 2: DNAME substitution exceeds 255 bytes, -1: invalid.
 * Packet validation has already checked uncompressed DNAME RDATA. */
static int next_alias(struct wire w, const struct packet *p, const uint8_t *name, size_t length,
    struct alias_target *next)
{
    size_t pos = p->question_end, dname_at = 0, dname_length = 0, owner_length = 0;
    uint32_t dname_ttl = 0; bool address = false, owner_dname = false;
    memset(next, 0, sizeof(*next));
    for (unsigned i = 0; i < p->counts[0]; ++i) {
        struct record rr;
        if (!record_read(w, &pos, &rr)) return -1;
        if (rr.klass != 1) continue;
        if (rr.type == 39 && rr.name_length < length && name_below(name, length, rr.name, rr.name_length)) {
            size_t n = rr.end - rr.data;
            /* Two ancestor DNAMEs violate the no-descendants rule; conflicting
             * records at the same owner are not a usable singleton RRset. */
            if (owner_length && (owner_length != rr.name_length ||
                !name_equal(w.data + dname_at, dname_length, w.data + rr.data, n))) return -1;
            if (!owner_length || rr.ttl < dname_ttl) dname_ttl = rr.ttl;
            owner_length = rr.name_length; dname_at = rr.data; dname_length = n;
        }
        if (!name_equal(name, length, rr.name, rr.name_length)) continue;
        if (rr.type == 39) owner_dname = true;
        if (rr.type == 1 || rr.type == 28 || rr.type == p->type) address = true;
        if (rr.type == 5) {
            uint8_t target[255]; size_t n, at = rr.data;
            if (!name_read(w, &at, target, &n) || at != rr.end ||
                (next->length && !name_equal(next->name, next->length, target, n))) return -1;
            memcpy(next->name, target, n); next->length = n;
        }
    }
    if (owner_dname && next->length) return -1;
    if (owner_length) {
        size_t lead = length - owner_length;
        if (lead + dname_length > 255) return next->length || address ? -1 : 2;
        if (next->length) {
            if (next->length != lead + dname_length || !name_equal(next->name, lead, name, lead) ||
                !name_equal(next->name + lead, dname_length, w.data + dname_at, dname_length)) return -1;
        } else {
            memcpy(next->name, name, lead); memcpy(next->name + lead, w.data + dname_at, dname_length);
            next->length = lead + dname_length; next->synthetic = true; next->ttl = dname_ttl;
        }
    }
    if (!next->length) return 0;
    return address || name_equal(next->name, next->length, name, length) ? -1 : 1;
}
/* A writer permits missing CNAME synthesis; NULL requires the explicit chain.
 * Return 1 for name overflow so callers can preserve the DNAME in YXDOMAIN. */
static int walk_aliases(struct wire w, const struct packet *p, uint8_t name[255], size_t *length,
    struct writer *synthetic, unsigned *added)
{
    memcpy(name, p->name, p->name_length); *length = p->name_length;
    for (unsigned step = 0; step < 16; ++step) {
        struct alias_target next;
        int result = next_alias(w, p, name, *length, &next);
        if (result != 1) return result == 2 ? 1 : result;
        if (next.synthetic) {
            if (!synthetic) return -1;
            uint8_t fixed[10] = {0,5,0,1}; set32(fixed + 4, next.ttl); set16(fixed + 8, next.length);
            if (!bytes(synthetic, name, *length) || !bytes(synthetic, fixed, 10) ||
                !bytes(synthetic, next.name, next.length)) return -1;
            if (added) ++*added;
        }
        memcpy(name, next.name, next.length); *length = next.length;
    }
    return -1;
}
static bool terminal_name(struct wire w, const struct packet *p, uint8_t name[255], size_t *length)
{
    int result = walk_aliases(w, p, name, length, NULL, NULL);
    return !result || (result == 1 && p->rcode == 6);
}

bool br_dns64_discover(const uint8_t *response, size_t length,
    struct br_dns64_prefix prefixes[BR_PREF64_MAX_PREFIXES], size_t *count, uint32_t *ttl)
{
    static const uint8_t wkn[] = {8,'i','p','v','4','o','n','l','y',4,'a','r','p','a',0};
    static const uint8_t widths[] = {32,40,48,56,64,96};
    struct wire w = {response, length}; struct packet p;
    size_t found = 0; uint32_t lifetime = UINT32_MAX, negative = UINT32_MAX;
    if (!count || !ttl) return false;
    *count = 0; *ttl = 0;
    if (!prefixes || !packet_read(w, &p) || p.unsupported ||
        (response[2] & 0xfa) != 0x80 || (p.rcode != 0 && p.rcode != 3) ||
        p.type != 28 || p.klass != 1 || !name_equal(p.name, p.name_length, wkn, sizeof(wkn))) return false;
    uint8_t terminal[255]; size_t terminal_length;
    if (!terminal_name(w, &p, terminal, &terminal_length)) return false;
    size_t pos = p.question_end;
    for (unsigned section = 0; section < 3; ++section) {
        for (unsigned i = 0; i < p.counts[section]; ++i) {
            struct record rr;
            if (!record_read(w, &pos, &rr)) return false;
            /* A conservative bound also ages any aliases in this Answer. */
            if (section == 0 && rr.ttl < lifetime) lifetime = rr.ttl;
            if (section == 1 && rr.type == 6 && rr.klass == 1 &&
                name_below(terminal, terminal_length, rr.name, rr.name_length)) {
                uint32_t value = u32(w.data + rr.end - 4);
                if (rr.ttl < negative) negative = rr.ttl;
                if (value < negative) negative = value;
            }
            if (section || p.rcode || rr.type != 28 || rr.klass != 1 ||
                !name_equal(rr.name, rr.name_length, terminal, terminal_length)) continue;
            const uint8_t *ip6 = w.data + rr.data;
            unsigned matches = 0; struct br_dns64_prefix candidate = {{0},0};
            for (unsigned n = 0; n < sizeof(widths); ++n) {
                uint8_t v4[4]; unsigned at = widths[n] / 8;
                if (widths[n] != 96 && ip6[8]) continue;
                for (unsigned byte = 0; byte < 4; ++byte) {
                    if (at == 8) ++at;
                    v4[byte] = ip6[at++];
                }
                if (v4[0] != 192 || v4[1] || v4[2] || (v4[3] != 170 && v4[3] != 171)) continue;
                /* An additional octet-aligned occurrence makes this WKA
                 * ambiguous. The other WKA record can still disambiguate. */
                unsigned copies = 0;
                for (unsigned byte = 0; byte <= 12; ++byte)
                    if (!memcmp(ip6 + byte, v4, 4)) ++copies;
                unsigned own_copy = (widths[n] == 32 || widths[n] >= 64) ? 1 : 0;
                if (copies > own_copy) { matches = 2; break; }
                ++matches; candidate.length = widths[n];
                memset(candidate.address, 0, sizeof(candidate.address));
                memcpy(candidate.address, ip6, widths[n] / 8);
            }
            if (matches != 1 || candidate.address[0] == 0xff ||
                (candidate.address[0] == 0xfe && (candidate.address[1] & 0xc0) == 0x80)) continue;
            uint8_t nonzero = 0;
            for (unsigned byte = 0; byte < 16; ++byte) nonzero |= candidate.address[byte];
            if (!nonzero) continue;
            size_t index = 0;
            while (index < found && (prefixes[index].length != candidate.length ||
                   memcmp(prefixes[index].address, candidate.address, 16))) ++index;
            if (index == found) {
                if (found == BR_PREF64_MAX_PREFIXES) return false;
                prefixes[found++] = candidate;
            }
        }
    }
    *count = found;
    *ttl = found ? lifetime : (negative != UINT32_MAX ? negative : 60);
    if (!found && lifetime < *ttl) *ttl = lifetime;
    return true;
}
static bool excluded(const uint8_t address[16], const struct br_dns64_prefix *prefixes, size_t count)
{
    static const uint8_t mapped[12] = {0,0,0,0,0,0,0,0,0,0,255,255};
    if (!memcmp(address, mapped, sizeof(mapped))) return true;
    for (size_t i = 0; i < count; ++i) {
        unsigned whole_bytes = prefixes[i].length / 8, bits = prefixes[i].length % 8;
        if (!memcmp(address, prefixes[i].address, whole_bytes) &&
            (!bits || !((address[whole_bytes] ^ prefixes[i].address[whole_bytes]) & (0xffU << (8 - bits))))) return true;
    }
    return false;
}
static bool changed_aaaa_rrset(struct wire wire, const struct packet *packet, const struct record *signature,
    const struct br_dns64_prefix *prefixes, size_t count)
{
    size_t pos = packet->question_end;
    for (unsigned i = 0; i < packet->counts[0]; ++i) {
        struct record rr;
        if (!record_read(wire, &pos, &rr)) return false;
        if (rr.type == 28 && rr.klass == 1 && name_equal(rr.name, rr.name_length, signature->name, signature->name_length) &&
            excluded(wire.data + rr.data, prefixes, count)) return true;
    }
    return false;
}
static bool write_response(const struct packet *qp, uint8_t cd, struct wire r, const struct packet *rp,
    const uint8_t *terminal, size_t terminal_length, uint32_t ttl_limit, br_dns64_embed embed, void *context,
    const struct br_dns64_prefix *exclusions, size_t exclusion_count, bool filter,
    uint8_t *output, size_t capacity, size_t *length);

enum br_dns64_action br_dns64_prepare_with_policy(const uint8_t *query, size_t query_length,
    const uint8_t *response, size_t response_length, const struct br_dns64_prefix *exclusions,
    size_t exclusion_count, uint8_t *a_query, size_t capacity,
    size_t *length, uint32_t *ttl_limit)
{
    struct wire q = {query, query_length}, r = {response, response_length};
    struct packet qp, rp;
    if (!length || !ttl_limit) return BR_DNS64_INVALID;
    *length = 0;
    if (exclusion_count > BR_DNS64_MAX_EXCLUSIONS || (exclusion_count && !exclusions)) return BR_DNS64_INVALID;
    for (size_t i = 0; i < exclusion_count; ++i) if (exclusions[i].length > 128) return BR_DNS64_INVALID;
    if (!packet_read(q, &qp) || !packet_read(r, &rp) || !response_for(q, &qp, r, &rp)) return BR_DNS64_INVALID;
    if (qp.type != 28 || qp.klass != 1 || qp.unsupported || rp.unsupported ||
        qp.counts[0] || qp.counts[1] || (qp.do_bit && (q.data[3] & 0x10)) || rp.rcode == 3 || rp.rcode == 6)
        return BR_DNS64_PASS;
    uint8_t terminal[255]; size_t terminal_length;
    if (!terminal_name(r, &rp, terminal, &terminal_length)) return BR_DNS64_INVALID;
    uint32_t limit = UINT32_MAX; bool soa = false;
    bool native = false, filtered = false;
    size_t pos = rp.question_end;
    for (unsigned section = 0; section < 3; ++section) {
        for (unsigned i = 0; i < rp.counts[section]; ++i) {
            struct record rr;
            if (!record_read(r, &pos, &rr)) return BR_DNS64_INVALID;
            if (section == 0 && !rp.rcode && rr.type == 28 && rr.klass == 1) {
                if (excluded(r.data + rr.data, exclusions, exclusion_count)) filtered = true;
                else if (section == 0 && name_equal(terminal, terminal_length, rr.name, rr.name_length)) native = true;
            }
            if (section == 1 && rr.type == 6 && rr.klass == 1 && name_below(terminal, terminal_length, rr.name, rr.name_length)) {
                uint32_t minimum = u32(r.data + rr.end - 4);
                if (rr.ttl < limit) limit = rr.ttl;
                if (minimum < limit) limit = minimum;
                soa = true;
            }
        }
    }
    if (native) {
        if (!filtered) return BR_DNS64_PASS;
        return write_response(&qp, q.data[3], r, &rp, terminal, terminal_length, 0, NULL, NULL,
                              exclusions, exclusion_count, true, a_query, capacity, length)
                   ? BR_DNS64_FILTERED : BR_DNS64_INVALID;
    }
    if (!a_query || capacity < query_length) return BR_DNS64_INVALID;
    memcpy(a_query, query, query_length); set16(a_query + qp.question_end - 4, 1);
    *length = query_length; *ttl_limit = soa ? limit : 600;
    return BR_DNS64_QUERY_A;
}
enum br_dns64_action br_dns64_prepare(const uint8_t *query, size_t query_length,
    const uint8_t *response, size_t response_length, uint8_t *output, size_t capacity,
    size_t *length, uint32_t *ttl_limit)
{
    return br_dns64_prepare_with_policy(query, query_length, response, response_length,
                                       NULL, 0, output, capacity, length, ttl_limit);
}
bool br_dns64_finish(const uint8_t *a_query, size_t query_length,
    const uint8_t *response, size_t response_length, uint32_t ttl_limit,
    br_dns64_embed embed, void *context, uint8_t *output, size_t capacity, size_t *length)
{
    struct wire q = {a_query, query_length}, r = {response, response_length};
    struct packet qp, rp;
    if (!length) return false;
    *length = 0;
    if (!output || !embed || !packet_read(q, &qp) || !packet_read(r, &rp) ||
        !response_for(q, &qp, r, &rp) || qp.type != 1 || qp.klass != 1 || qp.unsupported || rp.unsupported) return false;
    uint8_t terminal[255]; size_t terminal_length;
    if (!terminal_name(r, &rp, terminal, &terminal_length)) return false;
    return write_response(&qp, q.data[3], r, &rp, terminal, terminal_length, ttl_limit, embed, context,
                          NULL, 0, false, output, capacity, length);
}
static bool write_response(const struct packet *qp, uint8_t cd, struct wire r, const struct packet *rp,
    const uint8_t *terminal, size_t terminal_length, uint32_t ttl_limit, br_dns64_embed embed, void *context,
    const struct br_dns64_prefix *exclusions, size_t exclusion_count, bool filter,
    uint8_t *output, size_t capacity, size_t *length)
{
    if (!output) return false;
    struct writer out = {output, 0, capacity < BR_DNS64_MAX_PACKET ? capacity : BR_DNS64_MAX_PACKET};
    uint8_t header[12]; memcpy(header, r.data, sizeof(header));
    header[2] &= (uint8_t)~4U; header[3] &= (uint8_t)~0x20U; /* modified RRsets are not authoritative/validated */
    header[3] = (header[3] & (uint8_t)~0x10U) | (cd & 0x10);
    if (!bytes(&out, header, sizeof(header)) || !bytes(&out, qp->name, qp->name_length)) return false;
    const uint8_t question[] = {0,28,0,1};
    if (!bytes(&out, question, sizeof(question))) return false;
    size_t pos = rp->question_end;
    for (unsigned section = 0; section < 3; ++section) {
        unsigned kept = 0;
        for (unsigned i = 0; i < rp->counts[section]; ++i) {
            struct record rr;
            if (!record_read(r, &pos, &rr)) return false;
            if (filter && section == 0 && rr.klass == 1 &&
                ((rr.type == 28 && excluded(r.data + rr.data, exclusions, exclusion_count)) ||
                 (rr.type == 46 && u16(r.data + rr.data) == 28 &&
                  changed_aaaa_rrset(r, rp, &rr, exclusions, exclusion_count)))) continue;
            /* A filtered AAAA RRset no longer matches its original signature.
             * Omit its answer-section RRSIGs while preserving other RDATA. */
            if (!bytes(&out, rr.name, rr.name_length)) return false;
            ++kept;
            bool synthesize = !filter && section == 0 && rr.type == 1 && rr.klass == 1;
            if (synthesize && (rp->rcode || !name_equal(terminal, terminal_length, rr.name, rr.name_length))) return false;
            uint8_t fixed[10]; set16(fixed, synthesize ? 28 : rr.type); set16(fixed + 2, rr.klass);
            set32(fixed + 4, synthesize && rr.ttl > ttl_limit ? ttl_limit : rr.ttl); set16(fixed + 8, 0);
            size_t rdlength_at = out.length + 8;
            if (!bytes(&out, fixed, sizeof(fixed))) return false;
            size_t begin = out.length;
            if (synthesize) {
                uint8_t ip6[16];
                if (!embed(context, r.data + rr.data, ip6) || !bytes(&out, ip6, sizeof(ip6))) return false;
            } else if (!rdata_write(r, &rr, &out)) return false;
            set16(out.data + rdlength_at, out.length - begin);
        }
        set16(out.data + 6 + section * 2, kept);
    }
    *length = out.length; return true;
}

static bool ptr_packet(struct wire query, struct packet *packet)
{
    return packet_read(query, packet) && !(query.data[2] & 0xf8) &&
           packet->type == 12 && packet->klass == 1 && !packet->counts[0] && !packet->counts[1];
}

bool br_dns64_ptr_address(const uint8_t *query, size_t query_length, uint8_t ip6[16])
{
    return query && query_length >= 12 && u16(query + 4) == 1 &&
           br_dns64_ptr_question_address(query, query_length, 0, ip6);
}

bool br_dns64_ptr_question_address(const uint8_t *query, size_t query_length, unsigned question, uint8_t ip6[16])
{
    struct wire q = {query, query_length};
    static const uint8_t suffix[] = {3,'i','p','6',4,'a','r','p','a',0};
    if (!query || !ip6 || query_length < 12 || query_length > BR_DNS64_MAX_PACKET || (query[2] & 0xf8) ||
        !u16(query + 4) || u16(query + 4) > 8 || question >= u16(query + 4)) return false;
    size_t pos = 12, name_length; uint8_t name[255];
    for (unsigned i = 0; i <= question; ++i) {
        if (!name_read(q, &pos, name, &name_length) || pos + 4 > query_length) return false;
        pos += 4;
    }
    if (u16(query + pos - 4) != 12 || u16(query + pos - 2) != 1 || name_length != 74 ||
        !name_equal(name + 64, 10, suffix, sizeof(suffix))) return false;
    uint8_t address[16] = {0};
    for (unsigned i = 0; i < 32; ++i) {
        unsigned c = name[2 * i + 1], digit;
        if (name[2 * i] != 1) return false;
        if (c >= '0' && c <= '9') digit = c - '0';
        else if (c >= 'a' && c <= 'f') digit = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') digit = c - 'A' + 10;
        else return false;
        address[15 - i / 2] |= digit << ((i % 2) * 4);
    }
    memcpy(ip6, address, sizeof(address)); return true;
}

/* Copy a record with expanded owner/RDATA, preserving its type, class and TTL. */
static bool copy_record(struct wire wire, const struct record *rr, struct writer *out)
{
    uint8_t fixed[10]; set16(fixed, rr->type); set16(fixed + 2, rr->klass);
    set32(fixed + 4, rr->ttl); set16(fixed + 8, 0);
    if (!bytes(out, rr->name, rr->name_length) || !bytes(out, fixed, sizeof(fixed))) return false;
    size_t begin = out->length;
    if (!rdata_write(wire, rr, out)) return false;
    set16(out->data + begin - 2, out->length - begin); return true;
}

int br_dns64_dname_expand(const uint8_t *query, size_t query_length,
    const uint8_t *response, size_t response_length, uint8_t *output, size_t capacity, size_t *length)
{
    struct wire q = {query, query_length}, r = {response, response_length};
    struct packet qp, rp; uint8_t terminal[255]; size_t terminal_length;
    if (!length) return -1;
    *length = 0;
    if (!packet_read(q, &qp) || !packet_read(r, &rp) || !response_for(q, &qp, r, &rp)) return -1;
    if (qp.unsupported || rp.unsupported || qp.klass != 1 || qp.counts[0] || qp.counts[1] ||
        (qp.type != 1 && qp.type != 28 && qp.type != 12) ||
        (rp.rcode && rp.rcode != 3 && rp.rcode != 6) || (qp.do_bit && (query[3] & 0x10))) return 0;
    struct writer check = {NULL, 0, 65535}; unsigned added = 0;
    int result = walk_aliases(r, &rp, terminal, &terminal_length, &check, &added);
    if (result < 0) return -1;
    if (!added && !result) return 0;
    if (!output || (unsigned)rp.counts[0] + rp.counts[1] + rp.counts[2] + added > 128) return -1;
    struct writer out = {output, 0, capacity < BR_DNS64_MAX_PACKET ? capacity : BR_DNS64_MAX_PACKET};
    uint8_t header[12], type[4]; memcpy(header, response, 12);
    header[2] = (header[2] & (uint8_t)~5U) | (query[2] & 1);
    header[3] = (header[3] & (uint8_t)~0x30U) | (query[3] & 0x10);
    if (result == 1) header[3] = (header[3] & 0xf0U) | 6; /* YXDOMAIN */
    set16(header + 6, rp.counts[0] + added); set16(type, qp.type); set16(type + 2, qp.klass);
    if (!bytes(&out, header, 12) || !bytes(&out, qp.name, qp.name_length) || !bytes(&out, type, 4)) return -1;
    size_t pos = rp.question_end;
    for (unsigned i = 0; i < rp.counts[0]; ++i) {
        struct record rr;
        if (!record_read(r, &pos, &rr) || !copy_record(r, &rr, &out)) return -1;
    }
    if (walk_aliases(r, &rp, terminal, &terminal_length, &out, NULL) != result) return -1;
    for (unsigned section = 1; section < 3; ++section) for (unsigned i = 0; i < rp.counts[section]; ++i) {
        struct record rr;
        if (!record_read(r, &pos, &rr) || !copy_record(r, &rr, &out)) return -1;
    }
    *length = out.length; return 1;
}

int br_dns64_alias_query(const uint8_t *query, size_t query_length,
    const uint8_t *response, size_t response_length, uint8_t *output, size_t capacity, size_t *length)
{
    struct wire q = {query, query_length}, r = {response, response_length};
    struct packet qp, rp; uint8_t terminal[255]; size_t terminal_length;
    if (!length) return -1;
    *length = 0;
    if (!packet_read(q, &qp) || !packet_read(r, &rp) || !response_for(q, &qp, r, &rp)) return -1;
    if (qp.unsupported || rp.unsupported || qp.klass != 1 || qp.counts[0] || qp.counts[1] ||
        (qp.type != 1 && qp.type != 28 && qp.type != 12) || rp.rcode ||
        (qp.do_bit && (query[3] & 0x10))) return 0;
    if (!terminal_name(r, &rp, terminal, &terminal_length)) return -1;
    if (name_equal(qp.name, qp.name_length, terminal, terminal_length)) return 0;
    size_t pos = rp.question_end;
    for (unsigned section = 0; section < 3; ++section) for (unsigned i = 0; i < rp.counts[section]; ++i) {
        struct record rr;
        if (!record_read(r, &pos, &rr)) return -1;
        if (rr.klass != 1) continue;
        if ((section == 0 && rr.type == qp.type && name_equal(terminal, terminal_length, rr.name, rr.name_length)) ||
            (section == 1 && rr.type == 6 && name_below(terminal, terminal_length, rr.name, rr.name_length))) return 0;
    }
    if (!output) return -1;
    struct writer out = {output, 0, capacity < BR_DNS64_MAX_PACKET ? capacity : BR_DNS64_MAX_PACKET};
    uint8_t type[4]; set16(type, qp.type); set16(type + 2, qp.klass);
    if (!bytes(&out, query, 12) || !bytes(&out, terminal, terminal_length) || !bytes(&out, type, 4)) return -1;
    pos = qp.question_end;
    for (unsigned i = 0; i < qp.counts[2]; ++i) {
        struct record rr;
        if (!record_read(q, &pos, &rr) || rr.type != 41 || !copy_record(q, &rr, &out)) return -1;
    }
    *length = out.length; return 1;
}

bool br_dns64_alias_merge(const uint8_t *original, size_t original_length,
    const uint8_t *partial, size_t partial_length, const uint8_t *query, size_t query_length,
    const uint8_t *response, size_t response_length, uint32_t elapsed,
    uint8_t *output, size_t capacity, size_t *length)
{
    struct wire o = {original, original_length}, p = {partial, partial_length};
    struct wire q = {query, query_length}, r = {response, response_length};
    struct packet op, pp, qp, rp; uint8_t terminal[255]; size_t terminal_length;
    if (!length) return false;
    *length = 0;
    if (!output || !packet_read(o, &op) || !packet_read(p, &pp) || !packet_read(q, &qp) || !packet_read(r, &rp) ||
        !response_for(o, &op, p, &pp) || !response_for(q, &qp, r, &rp) ||
        op.unsupported || pp.unsupported || qp.unsupported || rp.unsupported || pp.rcode ||
        op.type != qp.type || op.klass != qp.klass ||
        !terminal_name(p, &pp, terminal, &terminal_length) ||
        name_equal(op.name, op.name_length, terminal, terminal_length) ||
        !name_equal(qp.name, qp.name_length, terminal, terminal_length) ||
        (unsigned)pp.counts[0] + rp.counts[0] + rp.counts[1] + rp.counts[2] > 128) return false;
    struct writer out = {output, 0, capacity < BR_DNS64_MAX_PACKET ? capacity : BR_DNS64_MAX_PACKET};
    uint8_t header[12], type[4]; memcpy(header, response, 12); memcpy(header, original, 2);
    header[2] = (header[2] & (uint8_t)~5U) | (original[2] & 1);
    header[3] = (header[3] & (uint8_t)~0x30U) | (original[3] & 0x10);
    set16(header + 6, pp.counts[0] + rp.counts[0]); set16(type, op.type); set16(type + 2, op.klass);
    if (!bytes(&out, header, 12) || !bytes(&out, op.name, op.name_length) || !bytes(&out, type, 4)) return false;
    size_t pos = pp.question_end;
    for (unsigned i = 0; i < pp.counts[0]; ++i) {
        struct record rr;
        if (!record_read(p, &pos, &rr)) return false;
        rr.ttl = rr.ttl > elapsed ? rr.ttl - elapsed : 0;
        if (!copy_record(p, &rr, &out)) return false;
    }
    pos = rp.question_end;
    for (unsigned section = 0; section < 3; ++section) for (unsigned i = 0; i < rp.counts[section]; ++i) {
        struct record rr;
        if (!record_read(r, &pos, &rr) || !copy_record(r, &rr, &out)) return false;
    }
    /* Rewalk the accumulated chain: cross-response cycles/conflicts are just
     * as invalid as aliases that contradict one another in a single packet. */
    struct wire merged = {output, out.length};
    if (!packet_read(merged, &rp) || !terminal_name(merged, &rp, terminal, &terminal_length)) return false;
    *length = out.length; return true;
}

bool br_dns64_ptr_query(const uint8_t *query, size_t query_length, const uint8_t ip4[4],
    uint8_t *output, size_t capacity, size_t *length)
{
    struct wire q = {query, query_length}; struct packet qp; uint8_t ip6[16];
    if (!length) return false;
    *length = 0;
    if (!output || !ip4 || !br_dns64_ptr_address(query, query_length, ip6) || !ptr_packet(q, &qp) ||
        qp.unsupported || (qp.do_bit && (query[3] & 0x10))) return false;
    struct writer out = {output, 0, capacity < BR_DNS64_MAX_PACKET ? capacity : BR_DNS64_MAX_PACKET};
    if (!bytes(&out, query, 12)) return false;
    for (int i = 3; i >= 0; --i) {
        uint8_t label[4]; unsigned n = 1, value = ip4[i];
        if (value >= 100) label[n++] = '0' + value / 100;
        if (value >= 10) label[n++] = '0' + (value / 10) % 10;
        label[n++] = '0' + value % 10; label[0] = n - 1;
        if (!bytes(&out, label, n)) return false;
    }
    static const uint8_t tail[] = {7,'i','n','-','a','d','d','r',4,'a','r','p','a',0,0,12,0,1};
    if (!bytes(&out, tail, sizeof(tail))) return false;
    size_t pos = qp.question_end;
    for (unsigned i = 0; i < qp.counts[2]; ++i) {
        struct record rr;
        /* Only EDNS metadata is supported here. Never leak the site reverse
         * name through an arbitrary additional record in a rewritten query. */
        if (!record_read(q, &pos, &rr) || rr.type != 41 || !copy_record(q, &rr, &out)) return false;
    }
    *length = out.length; return true;
}

bool br_dns64_ptr_finish(const uint8_t *original, size_t original_length,
    const uint8_t *translated, size_t translated_length, const uint8_t *response, size_t response_length,
    uint8_t *output, size_t capacity, size_t *length)
{
    struct wire oq = {original, original_length}, q = {translated, translated_length}, r = {response, response_length};
    struct packet op, qp, rp; uint8_t ip6[16], terminal[255]; size_t terminal_length;
    if (!length) return false;
    *length = 0;
    if (!output || !br_dns64_ptr_address(original, original_length, ip6) || !ptr_packet(oq, &op) ||
        !ptr_packet(q, &qp) || !packet_read(r, &rp) || !response_for(q, &qp, r, &rp) ||
        op.unsupported || qp.unsupported || rp.unsupported || (op.do_bit && (original[3] & 0x10)) ||
        !terminal_name(r, &rp, terminal, &terminal_length)) return false;
    uint32_t ttl = 600; bool found = false; size_t pos = rp.question_end;
    for (unsigned i = 0; i < rp.counts[0]; ++i) {
        struct record rr;
        if (!record_read(r, &pos, &rr)) return false;
        if (!rp.rcode && rr.type == 12 && rr.klass == 1 && name_equal(rr.name, rr.name_length, terminal, terminal_length)) {
            found = true; if (rr.ttl < ttl) ttl = rr.ttl;
        }
    }
    /* Do not invent a target for an incomplete positive alias response. */
    if (!rp.rcode && rp.counts[0] && !found) return false;
    struct writer out = {output, 0, capacity < BR_DNS64_MAX_PACKET ? capacity : BR_DNS64_MAX_PACKET};
    uint8_t header[12]; memcpy(header, response, sizeof(header));
    memcpy(header, original, 2); header[2] = (header[2] & (uint8_t)~5U) | (original[2] & 1);
    header[3] &= (uint8_t)~0x20U;
    header[3] = (header[3] & (uint8_t)~0x10U) | (original[3] & 0x10);
    set16(header + 6, rp.counts[0] + (found ? 1U : 0));
    static const uint8_t question[] = {0,12,0,1};
    if (!bytes(&out, header, sizeof(header)) || !bytes(&out, op.name, op.name_length) ||
        !bytes(&out, question, sizeof(question))) return false;
    if (found) {
        uint8_t fixed[10] = {0,5,0,1}; set32(fixed + 4, ttl); set16(fixed + 8, qp.name_length);
        if (!bytes(&out, op.name, op.name_length) || !bytes(&out, fixed, sizeof(fixed)) ||
            !bytes(&out, qp.name, qp.name_length)) return false;
    }
    pos = rp.question_end;
    for (unsigned section = 0; section < 3; ++section) {
        for (unsigned i = 0; i < rp.counts[section]; ++i) {
            struct record rr;
            if (!record_read(r, &pos, &rr)) return false;
            if (!copy_record(r, &rr, &out)) return false;
        }
    }
    *length = out.length; return true;
}

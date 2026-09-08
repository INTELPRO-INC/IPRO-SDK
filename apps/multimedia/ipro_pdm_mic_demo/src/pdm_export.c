/**
 * @file pdm_export.c
 * @brief Dump the PDM recording over the console as base64, for SNR analysis
 *        on a host.
 *
 * Transport-agnostic on purpose: everything goes through printf, so the same
 * command works over the debug UART today and over USB CDC-ACM the moment
 * CONFIG_DEBUG_CONSOLE_USB is turned on — no code change either way.
 *
 * Why base64 over the console rather than a live audio stream: SNR is a
 * measurement of the capture chain's own noise, so the data must be exactly
 * what the AUADC produced. A dump is bit-exact. An isochronous USB stream
 * would fold its own buffering and any dropped packets into the number.
 *
 * The console TX path is polled and blocking (UART_SendData), so bytes are
 * never dropped; at 921600 baud a 5 s 16 kHz recording (160 KB raw, 213 KB
 * encoded) takes about 2.4 s. A checksum is emitted so the host can prove
 * the transfer was clean rather than assume it.
 *
 * Shell: pdm_export [start_sample] [count]
 * Host:  tools/pdm_snr.py
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "shell.h"

/* From main.c */
extern const int16_t *pdm_get_buffer(void);
extern uint32_t pdm_get_recorded_bytes(void);
extern bool pdm_is_recording(void);
extern bool pdm_is_streaming(void);
extern int  pdm_get_gain(void);
extern int  pdm_get_rec_gain(void);

#define EXPORT_RATE        16000
#define BYTES_PER_LINE     48        /* -> exactly 64 base64 chars, no padding */

static const char b64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* FNV-1a. Not cryptographic — just has to catch a truncated or corrupted
 * transfer, and unlike a plain additive sum it also catches reordering. */
static uint32_t checksum32(const uint8_t *p, uint32_t n)
{
    uint32_t h = 2166136261u;

    for (uint32_t i = 0; i < n; i++) {
        h ^= p[i];
        h *= 16777619u;
    }
    return h;
}

/* Encode up to BYTES_PER_LINE bytes into dst; returns chars written. */
static uint32_t b64_line(const uint8_t *src, uint32_t n, char *dst)
{
    uint32_t o = 0;

    for (uint32_t i = 0; i < n; i += 3) {
        uint32_t rem = n - i;
        uint32_t v = (uint32_t)src[i] << 16;

        if (rem > 1) v |= (uint32_t)src[i + 1] << 8;
        if (rem > 2) v |= src[i + 2];

        dst[o++] = b64[(v >> 18) & 0x3F];
        dst[o++] = b64[(v >> 12) & 0x3F];
        dst[o++] = (rem > 1) ? b64[(v >> 6) & 0x3F] : '=';
        dst[o++] = (rem > 2) ? b64[v & 0x3F] : '=';
    }
    dst[o] = '\0';
    return o;
}

static int cmd_pdm_export(int argc, char **argv)
{
    if (pdm_is_recording() || pdm_is_streaming()) {
        printf("pdm_export: capture busy — stop pdm_rec/pdm_stream first\r\n");
        return 0;
    }

    const int16_t *buf = pdm_get_buffer();
    uint32_t total_samples = pdm_get_recorded_bytes() / sizeof(int16_t);

    if (buf == NULL || total_samples == 0) {
        printf("pdm_export: nothing recorded — run pdm_rec first\r\n");
        return 0;
    }

    uint32_t start = (argc > 1) ? (uint32_t)strtoul(argv[1], NULL, 0) : 0;
    uint32_t count = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0)
                                : total_samples;

    if (start >= total_samples) {
        printf("pdm_export: start %lu beyond %lu samples\r\n",
               (unsigned long)start, (unsigned long)total_samples);
        return 0;
    }
    if (start + count > total_samples) {
        count = total_samples - start;
    }

    const uint8_t *p = (const uint8_t *)(buf + start);
    uint32_t bytes = count * (uint32_t)sizeof(int16_t);
    char line[BYTES_PER_LINE * 4 / 3 + 4];

    /* Header carries everything the host needs to rebuild a WAV, so the
     * script never has to assume a rate or a gain setting. */
    printf("PDMEXPORT BEGIN rate=%d ch=1 bits=16 samples=%lu bytes=%lu "
           "pdm_gain=%d sum=0x%08lx\r\n",
           EXPORT_RATE, (unsigned long)count, (unsigned long)bytes,
           pdm_get_rec_gain(), (unsigned long)checksum32(p, bytes));

    for (uint32_t off = 0; off < bytes; off += BYTES_PER_LINE) {
        uint32_t n = bytes - off;
        if (n > BYTES_PER_LINE) {
            n = BYTES_PER_LINE;
        }
        b64_line(p + off, n, line);
        printf("%s\r\n", line);
    }

    printf("PDMEXPORT END\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_export, pdm_export,
                       Base64-dump the recording: pdm_export [start] [count]);

/* ===================================================================
 * pdm_hpf — AUADC high-pass corner
 *
 * The capture on this board sits under a low-frequency hump that carries
 * ~95% of the energy but almost none of the voice: measured on a silent
 * 5 s take, full-band rms was -41.4 dBFS while the 300-3400 Hz band was
 * only -60.9 dBFS. That LF costs ~13 dB of headroom for nothing.
 *
 * The AUADC has a 1st-order high pass in front of the FIFO:
 *   H(z) = (1 - z^-1) / (1 - (1 - 2^-k1 - 2^-k2) z^-1)
 * k1 and k2 place the single pole together (k2 is a fine trim, not a
 * second order). Corner ~= fs * (2^-k1 + 2^-k2) / 2pi.
 *
 * Note the true sample rate is 2.032 MHz / 128 = 15875 Hz, not the 16000
 * the app nominally asks for, so corners are computed against that.
 * =================================================================== */

#include <hal_auadc.h>

#define AUADC_TRUE_RATE  15875.0f   /* 2.032 MHz / 128 */

static int s_hpf_k1 = 8;            /* hardware default */
static int s_hpf_k2 = 0;            /* disabled */

static float hpf_corner(int k1, int k2)
{
    float a = (k1 >= 3 && k1 <= 13) ? (1.0f / (float)(1u << k1)) : 0.0f;
    float b = (k2 >= 3 && k2 <= 13) ? (1.0f / (float)(1u << k2)) : 0.0f;

    return AUADC_TRUE_RATE * (a + b) / 6.2831853f;
}

static int cmd_pdm_hpf(int argc, char **argv)
{
    if (argc < 2) {
        printf("pdm_hpf: k1=%d k2=%d -> corner %.1f Hz\r\n",
               s_hpf_k1, s_hpf_k2, (double)hpf_corner(s_hpf_k1, s_hpf_k2));
        printf("Usage: pdm_hpf <k1> [k2]   (3-13, or 0 to disable that term)\r\n");
        printf("  k1=8  %5.1f Hz  hardware default — leaves LF rumble in\r\n",
               (double)hpf_corner(8, 0));
        printf("  k1=6  %5.1f Hz\r\n", (double)hpf_corner(6, 0));
        printf("  k1=5  %5.1f Hz  suppresses most of the measured hump\r\n",
               (double)hpf_corner(5, 0));
        printf("  k1=4  %5.1f Hz  also trims low male fundamentals\r\n",
               (double)hpf_corner(4, 0));
        printf("  Applies immediately; re-run pdm_rec to hear/measure it.\r\n");
        return 0;
    }

    int k1 = atoi(argv[1]);
    int k2 = (argc > 2) ? atoi(argv[2]) : 0;

    if (k1 != 0 && (k1 < 3 || k1 > 13)) {
        printf("pdm_hpf: k1 must be 0 (off) or 3..13\r\n");
        return 0;
    }
    if (k2 != 0 && (k2 < 3 || k2 > 13)) {
        printf("pdm_hpf: k2 must be 0 (off) or 3..13\r\n");
        return 0;
    }

    s_hpf_k1 = k1;
    s_hpf_k2 = k2;
    hal_auadc_set_high_pass_filter(k1, k2);

    printf("pdm_hpf: k1=%d k2=%d -> corner %.1f Hz\r\n",
           k1, k2, (double)hpf_corner(k1, k2));
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_hpf, pdm_hpf, Set AUADC high-pass corner: pdm_hpf <k1> [k2]);

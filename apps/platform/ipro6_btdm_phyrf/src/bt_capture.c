/**
 * @file bt_capture.c
 * @brief RF sample-capture CLI (bt_cap)
 *
 * The RF block can stream receive samples straight into the WiFi RAM: a
 * source mux picks which point of the receive chain is recorded, an address
 * window says where in the RAM to put it, and a done flag says when the
 * window has been filled.  Source 2 is the 16 MHz stream that feeds the
 * BT/BLE modem, so a capture taken there is exactly what the demodulator
 * sees -- which is what we want to replay off-chip.
 *
 * The RAM-side address the capture hardware uses is NOT the CPU address
 * divided by four: the capture port is bank-interleaved, so consecutive
 * capture addresses land in different banks.  `bt_cap fill` + `bt_cap diff`
 * exist to measure that mapping on the bench instead of assuming it.
 *
 * Copyright (c) 2024 IPRO
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <generated/autoconf.h>

#if defined(CONFIG_BTDM_ENABLE) && defined(CONFIG_SHELL)

#include "shell.h"

/* RF register block */
#define RF_BASE                 0x20001000UL
#define RFIF_DFE_CTRL0          (RF_BASE + 0x220) /* [10:9] rx_test_sel      */
#define RF_SRAM_CTRL0           (RF_BASE + 0x23C) /* swap/link/ext           */
#define RF_SRAM_CTRL1           (RF_BASE + 0x240) /* en/done/done_cnt        */
#define RF_SRAM_CTRL2           (RF_BASE + 0x244) /* addr_start/addr_end     */
#define RF_SRAM_CTRL3           (RF_BASE + 0x248) /* status                  */
#define DFE_CTRL_2              (RF_BASE + 0x608) /* [31] rx_adc_iq_swap     */

/* Capture destination.  The BTDM build leaves the low 128 KB of the WiFi RAM
 * empty (the exchange memory is carved off the top 32 KB), so recording into
 * the bottom of it clobbers nothing. */
#define WRAM_BASE               0x21050000UL
#define WRAM_SAFE_WORDS         (128 * 1024 / 4)  /* everything below the EM */
/* Buffer 0 of the capture hardware is wired to the four 16 KB banks that sit
 * above the three 32 KB ones, so a capture appears 96 KB into the RAM and the
 * second half of its 64 KB reach overlaps the exchange memory.  Keep captures
 * to the first 32 KB of that reach. */
#define CAP_SAFE_SAMPLES        (32 * 1024 / 4)
#define FILL_MAGIC              0xA5000000UL

#define REG32(a)                (*(volatile uint32_t *)(a))

static uint32_t cap_words = 0;   /* window length of the last arm */

static void cap_usage(void)
{
    printf("Usage:\r\n");
    printf("  bt_cap fill [nwords]         - stamp the RAM with an index pattern\r\n");
    printf("  bt_cap arm <nsamp> [src] [trig]\r\n");
    printf("                               - src  0=raw ADC 80M 1=wifi40 2=BT/BLE 16M 3=wifi80\r\n");
    printf("                                 trig 0=immediate 1=on receive enable\r\n");
    printf("  bt_cap stat                  - control/status registers\r\n");
    printf("  bt_cap stop                  - disable the capture\r\n");
    printf("  bt_cap diff [nwords]         - words that no longer match the pattern\r\n");
    printf("  bt_cap dump <word> <n>       - hex dump, 8 words per line\r\n");
}

static void cap_fill(uint32_t n)
{
    volatile uint32_t *p = (volatile uint32_t *)WRAM_BASE;
    uint32_t i;

    if (n == 0 || n > WRAM_SAFE_WORDS) {
        n = WRAM_SAFE_WORDS;
    }
    for (i = 0; i < n; i++) {
        p[i] = FILL_MAGIC | i;
    }
    printf("filled %lu words at 0x%08lX with 0x%08lX|index\r\n",
           (unsigned long)n, (unsigned long)WRAM_BASE, (unsigned long)FILL_MAGIC);
}

static void cap_arm(uint32_t nsamp, uint32_t src, uint32_t trig)
{
    uint32_t ctrl0;

    if (nsamp == 0 || nsamp > 0xFFFF) {
        printf("nsamp must be 1..65535\r\n");
        return;
    }
    if (nsamp > CAP_SAFE_SAMPLES) {
        printf("WARNING: %lu samples reaches into the exchange memory; "
               "keep it under %lu\r\n",
               (unsigned long)nsamp, (unsigned long)CAP_SAFE_SAMPLES);
    }
    cap_words = nsamp;

    /* Pick the recording point. */
    REG32(RFIF_DFE_CTRL0) = (REG32(RFIF_DFE_CTRL0) & ~(3UL << 9)) | ((src & 3UL) << 9);

    /* Address window, in capture-side word addresses. */
    REG32(RF_SRAM_CTRL2) = (0UL << 16) | ((nsamp - 1) & 0xFFFFUL);

    /* Clear any latched trigger, then set the run mode.  Buffer 0, no delay. */
    ctrl0 = (trig ? (3UL << 16) : 0UL)      /* link_mode: 3 = external source */
          | (1UL << 21);                    /* ext_src 1 = receive enable      */
    REG32(RF_SRAM_CTRL0) = ctrl0 | (1UL << 19);   /* ext_clr */
    REG32(RF_SRAM_CTRL0) = ctrl0;

    /* Clear the done flag, then enable. */
    REG32(RF_SRAM_CTRL1) = (1UL << 3);
    REG32(RF_SRAM_CTRL1) = (1UL << 1);

    printf("armed: %lu samples, src=%lu (%s), trig=%s\r\n",
           (unsigned long)nsamp, (unsigned long)src,
           src == 2 ? "BT/BLE 16M" : src == 0 ? "raw ADC 80M" : "wifi",
           trig ? "receive enable" : "immediate");
}

static void cap_stat(void)
{
    uint32_t c0 = REG32(RF_SRAM_CTRL0);
    uint32_t c1 = REG32(RF_SRAM_CTRL1);
    uint32_t c2 = REG32(RF_SRAM_CTRL2);
    uint32_t c3 = REG32(RF_SRAM_CTRL3);
    uint32_t df = REG32(RFIF_DFE_CTRL0);

    printf("RFIF_DFE_CTRL0 @0x%08lX = 0x%08lX  rx_test_sel=%lu\r\n",
           (unsigned long)RFIF_DFE_CTRL0, (unsigned long)df,
           (unsigned long)((df >> 9) & 3));
    /* ext_src only means anything when link_mode is 3; otherwise the
     * capture free-runs and the bit is left over from the last arm. */
    printf("RF_SRAM_CTRL0  @0x%08lX = 0x%08lX  swap=%lu link_mode=%lu ext_src=%lu\r\n",
           (unsigned long)RF_SRAM_CTRL0, (unsigned long)c0,
           (unsigned long)((c0 >> 18) & 1), (unsigned long)((c0 >> 16) & 3),
           (unsigned long)((c0 >> 21) & 3));
    printf("RF_SRAM_CTRL1  @0x%08lX = 0x%08lX  en=%lu done=%lu done_cnt=%lu\r\n",
           (unsigned long)RF_SRAM_CTRL1, (unsigned long)c1,
           (unsigned long)((c1 >> 1) & 1), (unsigned long)(c1 & 1),
           (unsigned long)(c1 >> 16));
    printf("RF_SRAM_CTRL2  @0x%08lX = 0x%08lX  start=%lu end=%lu\r\n",
           (unsigned long)RF_SRAM_CTRL2, (unsigned long)c2,
           (unsigned long)(c2 >> 16), (unsigned long)(c2 & 0xFFFF));
    printf("RF_SRAM_CTRL3  @0x%08lX = 0x%08lX\r\n",
           (unsigned long)RF_SRAM_CTRL3, (unsigned long)c3);
    printf("DFE_CTRL_2     @0x%08lX = 0x%08lX  rx_adc_iq_swap=%lu\r\n",
           (unsigned long)DFE_CTRL_2, (unsigned long)REG32(DFE_CTRL_2),
           (unsigned long)((REG32(DFE_CTRL_2) >> 31) & 1));
}

static void cap_diff(uint32_t n)
{
    volatile uint32_t *p = (volatile uint32_t *)WRAM_BASE;
    uint32_t i, shown = 0, changed = 0;
    uint32_t first = 0xFFFFFFFFUL, last = 0;

    if (n == 0 || n > WRAM_SAFE_WORDS) {
        n = WRAM_SAFE_WORDS;
    }
    for (i = 0; i < n; i++) {
        if (p[i] == (FILL_MAGIC | i)) {
            continue;
        }
        changed++;
        if (first == 0xFFFFFFFFUL) {
            first = i;
        }
        last = i;
        if (shown < 32) {
            printf("  word %5lu (0x%08lX) = 0x%08lX\r\n", (unsigned long)i,
                   (unsigned long)(WRAM_BASE + i * 4), (unsigned long)p[i]);
            shown++;
        }
    }
    if (changed == 0) {
        printf("no word changed in the first %lu\r\n", (unsigned long)n);
    } else {
        printf("%lu of %lu words changed, span %lu..%lu\r\n",
               (unsigned long)changed, (unsigned long)n,
               (unsigned long)first, (unsigned long)last);
    }
}

static void cap_dump(uint32_t off, uint32_t n)
{
    volatile uint32_t *p = (volatile uint32_t *)WRAM_BASE;
    uint32_t i;

    if (off >= WRAM_SAFE_WORDS) {
        printf("offset past the safe window (%lu words)\r\n",
               (unsigned long)WRAM_SAFE_WORDS);
        return;
    }
    if (n == 0 || off + n > WRAM_SAFE_WORDS) {
        n = WRAM_SAFE_WORDS - off;
    }
    for (i = 0; i < n; i++) {
        if ((i & 7) == 0) {
            printf("%05lu:", (unsigned long)(off + i));
        }
        printf(" %08lX", (unsigned long)p[off + i]);
        if ((i & 7) == 7) {
            printf("\r\n");
        }
    }
    if (n & 7) {
        printf("\r\n");
    }
}

static void cmd_bt_cap(int argc, char **argv)
{
    if (argc < 2) {
        cap_usage();
        return;
    }

    if (strcmp(argv[1], "fill") == 0) {
        cap_fill(argc > 2 ? (uint32_t)strtoul(argv[2], NULL, 0) : 0);
    } else if (strcmp(argv[1], "arm") == 0) {
        if (argc < 3) {
            cap_usage();
            return;
        }
        cap_arm((uint32_t)strtoul(argv[2], NULL, 0),
                argc > 3 ? (uint32_t)strtoul(argv[3], NULL, 0) : 2,
                argc > 4 ? (uint32_t)strtoul(argv[4], NULL, 0) : 0);
    } else if (strcmp(argv[1], "stat") == 0) {
        cap_stat();
    } else if (strcmp(argv[1], "stop") == 0) {
        REG32(RF_SRAM_CTRL1) = 0;
        printf("capture disabled\r\n");
    } else if (strcmp(argv[1], "diff") == 0) {
        cap_diff(argc > 2 ? (uint32_t)strtoul(argv[2], NULL, 0) : cap_words * 2);
    } else if (strcmp(argv[1], "dump") == 0) {
        if (argc < 4) {
            cap_usage();
            return;
        }
        cap_dump((uint32_t)strtoul(argv[2], NULL, 0),
                 (uint32_t)strtoul(argv[3], NULL, 0));
    } else {
        cap_usage();
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_bt_cap, bt_cap, RF sample capture into WiFi RAM);

/* ------------------------------------------------------------------------
 * bt_cs -- find and dump the classic control structure in exchange memory
 *
 * The correlator is handed its access code by the hardware, assembled from
 * the device address and its BCH parity as they sit in the control
 * structure.  If either is not what the test expects, everything else can
 * be perfect and no packet will ever be accepted.  Rather than depend on
 * the exchange-memory layout constants, this searches for the test device
 * address (30:03:C0:0C:F0:0F) and dumps the structure around it.
 * ------------------------------------------------------------------------ */
#define EM_BASE                 0x28010000UL
#define EM_WORDS                (32 * 1024 / 4)
#define TEST_BDADDR_LO          0xC00CF00FUL   /* addr[3..0] as one word   */

static void cmd_bt_cs(int argc, char **argv)
{
    volatile uint32_t *em = (volatile uint32_t *)EM_BASE;
    uint32_t i, found = 0;

    (void)argc; (void)argv;
    for (i = 0; i < EM_WORDS; i++) {
        if (em[i] != TEST_BDADDR_LO) {
            continue;
        }
        found++;
        /* The address sits at offset 8 of the structure. */
        uint32_t cs = (i >= 2) ? (i - 2) : 0;
        printf("control structure at 0x%08lX (address word 0x%08lX)\r\n",
               (unsigned long)(EM_BASE + cs * 4), (unsigned long)(EM_BASE + i * 4));
        uint32_t k;
        for (k = 0; k < 16; k++) {
            if ((k & 3) == 0) {
                printf("  +0x%02lX:", (unsigned long)(k * 4));
            }
            printf(" %08lX", (unsigned long)em[cs + k]);
            if ((k & 3) == 3) {
                printf("\r\n");
            }
        }
        printf("  bdaddr  %04lX %04lX %04lX\r\n",
               (unsigned long)(em[cs + 2] & 0xFFFF), (unsigned long)(em[cs + 2] >> 16),
               (unsigned long)(em[cs + 3] & 0xFFFF));
        printf("  bch0/1  %04lX %04lX   rxmaxbuf_bch2 %04lX\r\n",
               (unsigned long)(em[cs + 3] >> 16), (unsigned long)(em[cs + 4] & 0xFFFF),
               (unsigned long)(em[cs + 4] >> 16));
        if (found >= 4) {
            break;
        }
    }
    if (!found) {
        printf("test device address not present anywhere in exchange memory\r\n");
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_bt_cs, bt_cs, Dump the classic control structure);

/* ------------------------------------------------------------------------
 * bt_poll -- accumulate the sticky state of the debug bus
 *
 * Reading the debug bus once from the console catches a random instant of a
 * receive slot.  This polls it flat out for a while and reports which bits
 * were ever high and which were ever low, which is what actually answers
 * "does this pulse at all".
 * ------------------------------------------------------------------------ */
#define PHY_TESTBUS_SEL         0x2000385CUL
#define PHY_TESTBUS_READ        0x20003858UL

static void cmd_bt_poll(int argc, char **argv)
{
    uint32_t ms = argc > 1 ? (uint32_t)strtoul(argv[1], NULL, 0) : 200;
    uint32_t any = 0, all = 0xFFFFFFFFUL, n = 0, prev = 0, edges = 0;
    uint32_t loops = ms * 2000;   /* rough: the read dominates the loop */

    printf("polling testbus (sel=%lu) for about %lu ms\r\n",
           (unsigned long)(REG32(PHY_TESTBUS_SEL) & 0xF), (unsigned long)ms);
    while (n < loops) {
        uint32_t v = REG32(PHY_TESTBUS_READ);
        any |= v;
        all &= v;
        if (n && v != prev) {
            edges++;
        }
        prev = v;
        n++;
    }
    printf("  samples %lu  changes %lu\r\n", (unsigned long)n, (unsigned long)edges);
    printf("  ever high 0x%08lX\r\n", (unsigned long)any);
    printf("  ever low  0x%08lX\r\n", (unsigned long)~all);
}
SHELL_CMD_EXPORT_ALIAS(cmd_bt_poll, bt_poll, Sticky sample of the PHY debug bus);

/* ------------------------------------------------------------------------
 * bt_diag -- the classic core's own debug bytes, read straight off the bus
 *
 * The core carries four selectable debug bytes.  They need no pads and no
 * capture buffer: the selection register picks four of about eighty
 * internal buses and the status register hands all four back over the
 * bus.  The one that matters here is selection 36, which reports the
 * sync pulse together with the number of mismatching bits in the 64-bit
 * access code -- that is what separates a real packet from a marginal
 * correlation, and no counter can tell us that.
 *
 * Because a receive slot is a few hundred microseconds and a console read
 * is not, this samples in a tight loop and reports the sticky picture,
 * the same way bt_poll does for the PHY bus.
 *
 * Useful selections (lower bank; add 64 for the upper bank):
 *     1  frame control state        18  bit stream in and out
 *    22  header ready / header ok / payload ready / payload ok
 *    24  the two completed-packet counters
 *    25  received length, low bits  26  transmit and receive packet type
 *    27  all eight error flags      28  arqn / seqn / flow
 *    29  forward error correction counters and corrections
 *    33  frequency select           34  frequency table pointer
 *    35  window open and size       36  sync pulse + access code errors
 *    37  radio data lines           49  which coding stages are enabled
 *
 * The default four answer the question this was written for: was the
 * thing it synchronised on a real packet, which check rejected it, did
 * the header even complete, and what type did it decode to.
 * ------------------------------------------------------------------------ */
#define BT_DIAGCNTL             0x28000450UL
#define BT_DIAGSTAT             0x28000454UL

static void cmd_bt_diag(int argc, char **argv)
{
    uint32_t d0 = argc > 1 ? (uint32_t)strtoul(argv[1], NULL, 0) : 36;
    uint32_t d1 = argc > 2 ? (uint32_t)strtoul(argv[2], NULL, 0) : 27;
    uint32_t d2 = argc > 3 ? (uint32_t)strtoul(argv[3], NULL, 0) : 22;
    uint32_t d3 = argc > 4 ? (uint32_t)strtoul(argv[4], NULL, 0) : 26;
    uint32_t ms = argc > 5 ? (uint32_t)strtoul(argv[5], NULL, 0) : 200;
    uint32_t cntl, n = 0, loops = ms * 2000;
    uint32_t any = 0, all = 0xFFFFFFFFUL, edges = 0, prev = 0;
    uint32_t hits = 0, err_min = 0x7F, err_max = 0, err_last = 0;

    cntl = (1UL << 31) | ((d3 & 0x7F) << 24)
         | (1UL << 23) | ((d2 & 0x7F) << 16)
         | (1UL << 15) | ((d1 & 0x7F) <<  8)
         | (1UL <<  7) |  (d0 & 0x7F);
    REG32(BT_DIAGCNTL) = cntl;
    printf("DIAGCNTL @0x%08lX = 0x%08lX (readback 0x%08lX)  bytes %lu/%lu/%lu/%lu\r\n",
           (unsigned long)BT_DIAGCNTL, (unsigned long)cntl,
           (unsigned long)REG32(BT_DIAGCNTL),
           (unsigned long)d0, (unsigned long)d1,
           (unsigned long)d2, (unsigned long)d3);

    while (n < loops) {
        uint32_t v = REG32(BT_DIAGSTAT);
        any |= v;
        all &= v;
        if (n && v != prev) {
            edges++;
        }
        prev = v;
        /* byte 0 selection 36: bit 7 is the sync pulse, bits 6..0 the
         * number of access code bits that did not match */
        if (d0 == 36 && (v & 0x80)) {
            uint32_t e = v & 0x7F;
            hits++;
            err_last = e;
            if (e < err_min) {
                err_min = e;
            }
            if (e > err_max) {
                err_max = e;
            }
        }
        n++;
    }
    printf("  samples %lu  changes %lu\r\n", (unsigned long)n, (unsigned long)edges);
    printf("  ever high 0x%08lX\r\n", (unsigned long)any);
    printf("  ever low  0x%08lX\r\n", (unsigned long)~all);
    if (d1 == 27) {
        static const char *flag[8] = {
            "seqn", "fec", "guard", "mic", "crc", "hec", "ltaddr", "sync"
        };
        uint32_t seen = (any >> 8) & 0xFF, i;

        printf("  errors seen:");
        if (!seen) {
            printf(" none");
        }
        for (i = 0; i < 8; i++) {
            if (seen & (1UL << i)) {
                printf(" %s", flag[i]);
            }
        }
        printf("\r\n");
    }
    if (d2 == 22) {
        /* "ok" is a level that stays as the last reception left it, so it
         * says nothing unless the matching "ready" pulse was seen in this
         * window.  Reporting it unconditionally reads as "header accepted"
         * on a silent channel, which is how time gets wasted. */
        uint32_t seen = (any >> 16) & 0xFF;

        if (seen & 0x10) {
            printf("  header completed, accepted: %s\r\n",
                   (seen & 0x08) ? "yes" : "no");
        } else {
            printf("  no header completed in this window\r\n");
        }
        if (seen & 0x04) {
            printf("  payload completed, accepted: %s\r\n",
                   (seen & 0x02) ? "yes" : "no");
        } else {
            printf("  no payload completed in this window\r\n");
        }
    }
    if (d3 == 26) {
        /* This byte is two 4-bit values, not flags, so the sticky picture
         * is only the union of the bits that ever moved; the last sample
         * is the one that can be read as a type. */
        printf("  receive type: last 0x%lX, bits ever set across samples 0x%lX"
               "  (4 = DH1; only meaningful once a header has completed)\r\n",
               (unsigned long)((prev >> 24) & 0x0F),
               (unsigned long)((any >> 24) & 0x0F));
    }
    if (d0 == 36) {
        if (hits) {
            printf("  sync seen %lu times; access code errors min %lu max %lu last %lu\r\n",
                   (unsigned long)hits, (unsigned long)err_min,
                   (unsigned long)err_max, (unsigned long)err_last);
        } else {
            printf("  sync never seen while sampling\r\n");
        }
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_bt_diag, bt_diag, Sample the classic core debug bytes);

/* ------------------------------------------------------------------------
 * bz_diag -- the modem's own debug bus
 *
 * The modem has a diagnostic mux of its own, brought out on the PHY debug
 * bus at selection 14.  It reaches the front end input and output, the
 * interference filter, the DC canceller and the whole frequency estimator
 * chain, which is the part no register can report.  This picks the modem
 * selection and points the PHY bus at it; read it with bt_poll.
 * ------------------------------------------------------------------------ */
#define BZ_BT_DIAGCNTL          0x20003A18UL

static void cmd_bz_diag(int argc, char **argv)
{
    uint32_t sel = argc > 1 ? (uint32_t)strtoul(argv[1], NULL, 0) : 0;

    REG32(BZ_BT_DIAGCNTL) = sel & 0xFF;
    REG32(PHY_TESTBUS_SEL) = (REG32(PHY_TESTBUS_SEL) & ~0xFUL) | 14UL;
    printf("modem diag select @0x%08lX = 0x%02lX (readback 0x%08lX)\r\n",
           (unsigned long)BZ_BT_DIAGCNTL, (unsigned long)(sel & 0xFF),
           (unsigned long)REG32(BZ_BT_DIAGCNTL));
    printf("PHY bus select = %lu, read at 0x%08lX (0x%08lX now); use bt_poll\r\n",
           (unsigned long)(REG32(PHY_TESTBUS_SEL) & 0xF),
           (unsigned long)PHY_TESTBUS_READ, (unsigned long)REG32(PHY_TESTBUS_READ));
}
SHELL_CMD_EXPORT_ALIAS(cmd_bz_diag, bz_diag, Point the PHY debug bus at the modem);

#endif /* CONFIG_BTDM_ENABLE && CONFIG_SHELL */

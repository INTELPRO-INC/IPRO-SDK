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
 * pio_sdcard_cmd.c - shell command for the PIO-based 4-bit SD host.
 *
 * A/B counterpart to the hardware-SDH `sdcard` command. PIO-SDH runs on
 * GPIO22-27 (CLK/CMD/D0-D3); the hardware SDH stays on GPIO28-33.
 *
 * Subcommands:
 *   pio_sdcard init                 full SD enumeration; print CID / CSD / capacity
 *   pio_sdcard read <lba> [n] [khz] enumerate + read n blocks (CMD17/18), hexdump head
 *   pio_sdcard reread <lba>         re-read one block without re-enumerating
 *   pio_sdcard write <lba> [n]      pattern-write n blocks + readback verify (DESTRUCTIVE)
 *   pio_sdcard mkfs                 format the card (exFAT) through this host + mount (DESTRUCTIVE)
 *   pio_sdcard mount                mount the card's FAT fs at /sdcard (then ls/cat)
 *   pio_sdcard fstest               write + verify /sdcard/pio_sdh_ab.txt (A/B artifact)
 *   pio_sdcard fspeed [kb]          fatfs file write/read throughput
 *   pio_sdcard speedtest [lba] [kb] raw sequential-read throughput
 *   pio_sdcard bus <1|4>            switch DATA bus width (ACMD6) at runtime
 *   pio_sdcard gap <0-30>           inter-nibble CLK-low cycles (SI settle knob)
 *   pio_sdcard dma <0|1> [tx rx]    DMA data path on/off + channel select
 *   pio_sdcard sm <0|1> / drive <0-3>  SM select / CLK pad drive (SI probes)
 *   pio_sdcard rstress|rloop|wdiag|status|recover  read/write stress + card-state probes
 *   pio_sdcard clktest|cmdtest|dbg    bring-up probes
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "shell.h"
#include "hal_pio_sdh.h"
#include "hal_mtimer.h"
#include "FreeRTOS.h"
#include "task.h"
#include <fatfs.h>
#include <diskio.h>
#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_USE_FATFS)
#include "evfs.h"
#include "evfs/fatfs_fs.h"
#include "evfs_posix.h"
#endif

static uint8_t s_use_dma = 1;     /* `pio_sdcard dma 0|1 [txch] [rxch]` */
static uint8_t s_dma_tx_ch = 2;   /* DMA0 ch0/1 are the i2s/padc demos' habit */
static uint8_t s_dma_rx_ch = 3;
static uint8_t s_sm = 0;          /* `pio_sdcard sm 0|1` (SM1 = CCI-proven DREQs) */

enum { MAXBLK = 8 };
static uint8_t s_blk[MAXBLK * 512];

#if FF_FS_MKFS_PROGRESS_REPORT
/* f_mkfs progress callback: pct is 0..100. Printed at each FatFS milestone
 * (5/10/.../90/100) so a long format over the PIO datapath is observable. */
static void pio_sdcard_mkfs_progress(int pct)
{
    printf("pio_sdh: mkfs %d%%\n", pct);
}
#endif

static void pio_sdcard_default_cfg(hal_pio_sdh_cfg_t *c)
{
    c->pin_clk = 22;
    c->pin_cmd = 23;
    c->pin_d0  = 24;      /* D1=25, D2=26, D3=27 */
    c->sm_cmd  = s_sm;
    c->sm_dat  = 1;
    c->use_dma = s_use_dma;
    c->dma_tx_ch = s_dma_tx_ch;
    c->dma_rx_ch = s_dma_rx_ch;
    c->pio_clk_div = 0;
    c->pio_clk_sel = 0;
}

static int cmd_pio_sdcard(int argc, char **argv)
{
    hal_pio_sdh_cfg_t cfg;
    pio_sdcard_default_cfg(&cfg);

    if (argc < 2) {
        printf("usage: pio_sdcard <init|read|rstress|rloop|write|wdiag|status|reread\n"
               "                   |mkfs|mount|fstest|fspeed|speedtest|bus|gap|wgap|wauto\n"
               "                   |dma|sm|drive|recover|clktest|cmdtest|dbg>\n");
        return 0;
    }

    if (strcmp(argv[1], "clktest") == 0) {
        hal_pio_sdh_hw_setup(&cfg);
        printf("pio_sdh: SM enabled; CLK free-running on GPIO%u @~400kHz.\n", cfg.pin_clk);
        printf("         CMD=GPIO%u D0-D3=GPIO%u-%u. Probe with a logic analyzer.\n",
               cfg.pin_cmd, cfg.pin_d0, cfg.pin_d0 + 3);
        return 0;
    }

    if (strcmp(argv[1], "cmdtest") == 0) {
        uint32_t r[2] = { 0, 0 };
        int rc;
        hal_pio_sdh_hw_setup(&cfg);
        hal_pio_sdh_clock_burst(96);                        /* card power-up clocks */
        hal_pio_sdh_send_cmd(0, 0x00000000, 0, r);          /* CMD0 GO_IDLE (no resp) */
        rc = hal_pio_sdh_send_cmd(8, 0x000001AA, 48, r);    /* CMD8 SEND_IF_COND (R7) */
        if (rc == 0) {
            printf("pio_sdh: CMD8 R7 raw = 0x%08lx 0x%08lx\n",
                   (unsigned long)r[0], (unsigned long)r[1]);
            printf("         (expect the 0xAA check pattern echoed in the low byte)\n");
        } else {
            printf("pio_sdh: CMD8 timeout - no response. Check card power/wiring on GPIO22-27.\n");
            printf("         FLEVEL=0x%08lx (SM0 tx[3:0] rx[7:4])\n",
                   (unsigned long)hal_pio_sdh_reg_read(0x0C));
        }
        return 0;
    }

    if (strcmp(argv[1], "dbg") == 0) {
        static const struct { const char *n; uint32_t off; } regs[] = {
            { "CTRL",          0x00 }, { "FSTAT",         0x04 },
            { "FLEVEL",        0x0C }, { "SM0_CLKDIV",    0xC8 },
            { "SM0_EXECCTRL",  0xCC }, { "SM0_SHIFTCTRL", 0xD0 },
            { "SM0_ADDR",      0xD4 }, { "SM0_PINCTRL",   0xDC },
            { "DMA_CONFIG",    0x158 },
        };
        for (unsigned i = 0; i < sizeof(regs) / sizeof(regs[0]); i++) {
            printf("%-13s = 0x%08lx\n", regs[i].n,
                   (unsigned long)hal_pio_sdh_reg_read(regs[i].off));
        }
        printf("SM0 PC trace:");
        for (int i = 0; i < 24; i++) {
            printf(" %lu", (unsigned long)(hal_pio_sdh_reg_read(0xD4) & 0x1F));
        }
        printf("\n");
        return 0;
    }

    if (strcmp(argv[1], "init") == 0) {
        hal_pio_sdh_card_t card;
        int rc = hal_pio_sdh_init(&cfg, &card);
        if (rc) {
            printf("pio_sdh: init FAILED rc=%d (-2 ACMD41, -3 CID, -4 RCA, -5 CSD, -6 SEL, -7 ACMD6)\n", rc);
            return 0;
        }
        printf("pio_sdh: card ready, bus=%u-bit %s\n", card.bus_width,
               card.access_mode ? "SDHC/SDXC (sector)" : "SDSC (byte)");
        printf("  capacity: %lu blocks (%lu MB)\n",
               (unsigned long)card.block_count,
               (unsigned long)(card.block_count / 2048u));
        printf("  CID:");
        for (int i = 0; i < 16; i++) printf(" %02x", card.cid[i]);
        printf("\n  CSD:");
        for (int i = 0; i < 16; i++) printf(" %02x", card.csd[i]);
        printf("\n  (A/B: compare CID/capacity with the HW 'sdcard' command)\n");
        return 0;
    }

    if (strcmp(argv[1], "read") == 0) {
        uint8_t *blk = s_blk;
        hal_pio_sdh_card_t card;
        uint32_t lba = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 0;
        uint32_t cnt = (argc > 3) ? (uint32_t)strtoul(argv[3], NULL, 0) : 1;
        if (cnt < 1) cnt = 1;
        if (cnt > MAXBLK) cnt = MAXBLK;
        if (hal_pio_sdh_init(&cfg, &card)) {
            printf("pio_sdh: init failed\n");
            return 0;
        }
        if (argc > 4) {                       /* optional CLK override in kHz  */
            uint32_t khz = (uint32_t)strtoul(argv[4], NULL, 0);
            hal_pio_sdh_set_clock(khz * 1000u);
            printf("pio_sdh: CLK set to %lu kHz\n", (unsigned long)khz);
        }
        int rc = hal_pio_sdh_read_blocks(blk, lba, cnt);
        if (rc) {
            printf("pio_sdh: read LBA %lu x%lu FAILED rc=%d (-1 cmd/timeout, -2 CRC16)\n",
                   (unsigned long)lba, (unsigned long)cnt, rc);
            return 0;
        }
        printf("pio_sdh: read LBA %lu x%lu OK (%s, all CRC16 verified). Block 0 head:\n",
               (unsigned long)lba, (unsigned long)cnt, cnt > 1 ? "CMD18" : "CMD17");
        for (int i = 0; i < 64; i++) {
            printf(" %02x", blk[i]);
            if ((i & 15) == 15) printf("\n");
        }
        printf("  last block tail:");
        for (int i = 0; i < 16; i++) printf(" %02x", blk[(cnt - 1) * 512 + 496 + i]);
        printf("\n  (A/B: compare with HW 'sdcard' block %lu)\n", (unsigned long)lba);
        return 0;
    }

#if defined(CONFIG_DEMO_PIO_SDH_TEST)
    /* DBG: raw multi-block read stress - heap-alloc cnt*512, read <iters> times
     * off the already-init'd card, tally the rc. Isolates the CMD18 read datapath
     * from FatFS/fspeed. usage: pio_sdcard rstress <lba> [cnt=128] [iters=20] */
    if (strcmp(argv[1], "rstress") == 0) {
        uint32_t lba = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 0;
        uint32_t cnt = (argc > 3) ? (uint32_t)strtoul(argv[3], NULL, 0) : 128;
        uint32_t iters = (argc > 4) ? (uint32_t)strtoul(argv[4], NULL, 0) : 20;
        hal_pio_sdh_card_t card;
        if (hal_pio_sdh_init(&cfg, &card)) {
            printf("pio_sdh: rstress init failed\n");
            return 0;
        }
        uint8_t *buf = pvPortMalloc(cnt * 512u);
        if (!buf) {
            printf("pio_sdh: rstress no mem (%lu B)\n", (unsigned long)(cnt * 512u));
            return 0;
        }
        uint32_t ok = 0, e1 = 0, e2 = 0, eo = 0, firstfail = 0xFFFFFFFFu;
        for (uint32_t i = 0; i < iters; i++) {
            int rc = hal_pio_sdh_read_blocks(buf, lba, cnt);
            if (rc == 0) ok++;
            else {
                if (firstfail == 0xFFFFFFFFu) firstfail = i;
                if (rc == -1) e1++; else if (rc == -2) e2++; else eo++;
            }
        }
        printf("pio_sdh: rstress lba=%lu cnt=%lu x%lu: ok=%lu timeout(-1)=%lu crc(-2)=%lu other=%lu firstfail=%ld cmd23=%u\n",
               (unsigned long)lba, (unsigned long)cnt, (unsigned long)iters,
               (unsigned long)ok, (unsigned long)e1, (unsigned long)e2, (unsigned long)eo,
               (firstfail == 0xFFFFFFFFu) ? -1L : (long)firstfail,
               hal_pio_sdh_dbg_last_read_cmd23());
        vPortFree(buf);
        return 0;
    }

    /* DBG: read stress on the ALREADY-init'd card (NO re-init), so bus width /
     * CLK drive / gap set after `init` survive. Reports the rc tally AND read
     * throughput. Flow: init -> bus/drive/gap/recover -> rloop. This is the
     * "stress the raw datapath" tool: with `recover 0`, a setting is only stable
     * if ok == iters. usage: pio_sdcard rloop <lba> [cnt=128] [iters=50] */
    if (strcmp(argv[1], "rloop") == 0) {
        uint32_t lba = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 0;
        uint32_t cnt = (argc > 3) ? (uint32_t)strtoul(argv[3], NULL, 0) : 128;
        uint32_t iters = (argc > 4) ? (uint32_t)strtoul(argv[4], NULL, 0) : 50;
        if (cnt > 256u) cnt = 256u;
        uint8_t *buf = pvPortMalloc(cnt * 512u);
        if (!buf) {
            printf("pio_sdh: rloop no mem (%lu B)\n", (unsigned long)(cnt * 512u));
            return 0;
        }
        uint32_t ok = 0, e1 = 0, e2 = 0, eo = 0, firstfail = 0xFFFFFFFFu;
        uint32_t t0 = xTaskGetTickCount();
        for (uint32_t i = 0; i < iters; i++) {
            int rc = hal_pio_sdh_read_blocks(buf, lba, cnt);
            if (rc == 0) ok++;
            else {
                if (firstfail == 0xFFFFFFFFu) firstfail = i;
                if (rc == -1) e1++; else if (rc == -2) e2++; else eo++;
            }
        }
        uint32_t ms = (uint32_t)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS);
        if (!ms) ms = 1;
        uint32_t kib = ok * cnt / 2u;                  /* only successful reads  */
        printf("pio_sdh: rloop lba=%lu cnt=%lu x%lu: ok=%lu timeout(-1)=%lu crc(-2)=%lu other=%lu firstfail=%ld | %lu KiB in %lu ms = %lu KiB/s\n",
               (unsigned long)lba, (unsigned long)cnt, (unsigned long)iters,
               (unsigned long)ok, (unsigned long)e1, (unsigned long)e2, (unsigned long)eo,
               (firstfail == 0xFFFFFFFFu) ? -1L : (long)firstfail,
               (unsigned long)kib, (unsigned long)ms,
               (unsigned long)(ms ? kib * 1000u / ms : 0u));
        vPortFree(buf);
        return 0;
    }

    /* DBG: write-path wedge diagnostic. For each iter: CMD13 card state BEFORE
     * -> write cnt blocks (current wgap/wauto) -> CMD13 state AFTER (+ raw R1
     * error flags) -> read back with the RELIABLE multi-block read (CMD23+CMD18)
     * and byte-compare. Separates write-data-corruption from flaky single-block
     * verify reads, and shows whether a write strands the card off TRAN (state
     * 4). Stops at the first anomaly. pat: 0=LCG(SSO-ish,default) 1=zeros(benign,
     * no transitions) 2=00FF-alt(SSO-worst, all-4-DAT rise every byte).
     * usage: pio_sdcard wdiag <lba> [cnt=8] [iters=1] [pat=0] */
    if (strcmp(argv[1], "wdiag") == 0) {
        uint32_t lba = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 3000000;
        uint32_t cnt = (argc > 3) ? (uint32_t)strtoul(argv[3], NULL, 0) : 8;
        uint32_t iters = (argc > 4) ? (uint32_t)strtoul(argv[4], NULL, 0) : 1;
        uint32_t pat = (argc > 5) ? (uint32_t)strtoul(argv[5], NULL, 0) : 0;
        hal_pio_sdh_card_t card;
        if (cnt > 256u) cnt = 256u;
        if (hal_pio_sdh_init(&cfg, &card)) {
            printf("pio_sdh: wdiag init failed\n");
            return 0;
        }
        uint8_t *wbuf = pvPortMalloc(cnt * 512u);
        uint8_t *rbuf = pvPortMalloc(cnt * 512u);
        if (!wbuf || !rbuf) {
            printf("pio_sdh: wdiag no mem (%lu B x2)\n", (unsigned long)(cnt * 512u));
            if (wbuf) vPortFree(wbuf);
            if (rbuf) vPortFree(rbuf);
            return 0;
        }
        for (uint32_t b = 0; b < cnt; b++)
            for (uint32_t i = 0; i < 512u; i++) {
                uint32_t o = b * 512u + i;
                wbuf[o] = (pat == 1u) ? 0x00u
                        : (pat == 2u) ? ((i & 1u) ? 0xFFu : 0x00u)
                        : (uint8_t)((lba + b) * 31u + i * 7u + 3u);
            }
        uint32_t kib = cnt / 2u;                        /* 512B blocks -> KiB   */
        for (uint32_t it = 0; it < iters; it++) {
            uint32_t st0 = 0, s0 = 9, st1 = 0, s1 = 9;
            int rc0 = hal_pio_sdh_card_status(&st0, &s0);
            TickType_t tw = xTaskGetTickCount();
            int wrc = hal_pio_sdh_write_blocks(wbuf, lba, cnt);
            uint32_t wms = (uint32_t)((xTaskGetTickCount() - tw) * portTICK_PERIOD_MS);
            int rc1 = hal_pio_sdh_card_status(&st1, &s1);
            for (uint32_t i = 0; i < cnt * 512u; i++) rbuf[i] = 0;
            TickType_t tr = xTaskGetTickCount();
            int rrc = hal_pio_sdh_read_blocks(rbuf, lba, cnt);
            uint32_t rms = (uint32_t)((xTaskGetTickCount() - tr) * portTICK_PERIOD_MS);
            uint32_t st2 = 0, s2 = 9;
            int rc2 = hal_pio_sdh_card_status(&st2, &s2);   /* state AFTER read - no init */
            int bad = -1;
            if (!rrc)
                for (uint32_t i = 0; i < cnt * 512u; i++)
                    if (rbuf[i] != wbuf[i]) { bad = (int)i; break; }
            printf("pio_sdh: wdiag it=%lu lba=%lu cnt=%lu pat=%lu: pre[st%lu] wr=%d postwr[st%lu r1=%08lx] rd=%d postrd[rc=%d st%lu r1=%08lx] data=%s@%d rcmd23=%u wKiB/s=%lu rKiB/s=%lu\n",
                   (unsigned long)it, (unsigned long)lba, (unsigned long)cnt,
                   (unsigned long)pat, (unsigned long)s0, wrc,
                   (unsigned long)s1, (unsigned long)st1, rrc,
                   rc2, (unsigned long)s2, (unsigned long)st2,
                   (bad < 0) ? "OK" : "MISMATCH", bad,
                   hal_pio_sdh_dbg_last_read_cmd23(),
                   (unsigned long)(wms ? kib * 1000u / wms : 0u),
                   (unsigned long)(rms ? kib * 1000u / rms : 0u));
            (void)rc0; (void)rc1;
            if (wrc || rrc || bad >= 0) break;          /* stop at first anomaly */
        }
        vPortFree(wbuf);
        vPortFree(rbuf);
        return 0;
    }

#endif /* CONFIG_DEMO_PIO_SDH_TEST */

    /* DBG: CMD13 SEND_STATUS - print card CURRENT_STATE + raw R1 (wedge probe). */
    if (strcmp(argv[1], "status") == 0) {
        static const char *nm[] = {"idle", "ready", "ident", "stby",
                                   "tran", "data", "rcv", "prg", "dis"};
        hal_pio_sdh_card_t card;
        if (hal_pio_sdh_init(&cfg, &card)) {
            printf("pio_sdh: status init failed\n");
            return 0;
        }
        uint32_t st = 0, state = 9;
        int rc = hal_pio_sdh_card_status(&st, &state);
        printf("pio_sdh: CMD13 rc=%d state=%lu(%s) r1=%08lx\n",
               rc, (unsigned long)state, (state < 9u) ? nm[state] : "?",
               (unsigned long)st);
        return 0;
    }

    /* Format the card as a fresh filesystem through THIS host. f_mkfs
     * (exFAT) writes the FAT/bitmap over the PIO datapath with visible
     * progress, then mounts + posix-mounts at /sdcard. DESTRUCTIVE - wipes
     * the card. Use this before `mount`/`fspeed` when the card has no valid
     * FS (a raw `mount` would otherwise trigger a *silent* auto-mkfs deep
     * inside fatfs_register that gives no progress and no error code).
     * usage: pio_sdcard mkfs */
    if (strcmp(argv[1], "mkfs") == 0) {
        fatfs_pio_sdh_driver_register(&cfg);
        printf("pio_sdh: mkfs start (exFAT, DESTRUCTIVE) over PIO host...\n");
#if FF_FS_MKFS_PROGRESS_REPORT
        int err = fatfs_format(DEV_SD, pio_sdcard_mkfs_progress);
#else
        int err = fatfs_format(DEV_SD, NULL);
#endif
        if (err != FR_OK) {
            printf("pio_sdh: mkfs FAILED (err=%d)\n", err);
            return 0;
        }
        printf("pio_sdh: mkfs OK\n");
#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_USE_FATFS)
        int evfs_ret = evfs_register_fatfs("sdcard", DEV_SD, false);
        if (evfs_ret != EVFS_OK && evfs_ret != EVFS_ERR_BAD_ARG) {
            printf("pio_sdh: EVFS FatFS registration failed: %d\n", evfs_ret);
            return 0;
        }
        evfs_ret = evfs_posix_mount("/sdcard", "sdcard");
        printf("pio_sdh: %s at /sdcard (evfs_ret=%d)\n",
               (evfs_ret == 0 || evfs_ret == -16) ? "mounted" : "mount FAILED",
               evfs_ret);
#endif
        return 0;
    }

    /* Mount the card's FAT filesystem through THIS host (P6). Registers on
     * the same DEV_SD slot / "/sdcard" mount point as the HW `sdcard`
     * command - whichever runs last owns the drive (one socket at a time on
     * the A/B bench). Then use the stock `ls` / `cat` shell commands. NOTE:
     * fatfs_register does an eager f_mount and, on FR_NO_FILESYSTEM, a silent
     * exFAT auto-mkfs - if the card is unformatted, run `mkfs` first. */
    if (strcmp(argv[1], "mount") == 0) {
        fatfs_pio_sdh_driver_register(&cfg);
        printf("pio_sdh: mount: fatfs_register (eager f_mount)...\n");
        int ret = fatfs_register();
        printf("pio_sdh: mount: fatfs_register rc=%d\n", ret);
#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_USE_FATFS)
        int evfs_ret = evfs_register_fatfs("sdcard", DEV_SD, false);
        if (evfs_ret != EVFS_OK && evfs_ret != EVFS_ERR_BAD_ARG) {
            printf("pio_sdh: EVFS FatFS registration failed: %d\n", evfs_ret);
            return 0;
        }
        evfs_ret = evfs_posix_mount("/sdcard", "sdcard");
        if (evfs_ret != 0 && evfs_ret != -16) {
            printf("pio_sdh: EVFS mount failed: %d\n", evfs_ret);
        } else {
            printf("pio_sdh: FatFS mounted at /sdcard (PIO host, GPIO22-27)\n");
        }
#else
        printf("pio_sdh: fatfs_register rc=%d (EVFS off, no posix mount)\n", ret);
#endif
        return 0;
    }

    /* E2E filesystem check: write a marker file, read it back, compare.
     * The file doubles as the cross-host A/B artifact (`cat` it after
     * mounting the same card on the HW SDH). Needs a prior `mount`. */
    if (strcmp(argv[1], "fstest") == 0) {
        static const char *path = "/sdcard/pio_sdh_ab.txt";
        char msg[96];
        char rd[96];
        snprintf(msg, sizeof(msg),
                 "written by PIO-SDH (GPIO22-27) fw build " __DATE__ " " __TIME__ "\n");
        int fd = open(path, O_CREAT | O_TRUNC | O_RDWR);
        if (fd < 0) {
            printf("pio_sdh: fstest open-for-write failed (%d) - mounted?\n", fd);
            return 0;
        }
        int wn = write(fd, msg, strlen(msg));
        close(fd);
        fd = open(path, O_RDONLY);
        if (fd < 0) {
            printf("pio_sdh: fstest re-open failed (%d)\n", fd);
            return 0;
        }
        int rn = read(fd, rd, sizeof(rd) - 1);
        close(fd);
        if (wn != (int)strlen(msg) || rn != wn || memcmp(msg, rd, (size_t)rn) != 0) {
            printf("pio_sdh: fstest FAILED (wrote %d read %d)\n", wn, rn);
            return 0;
        }
        rd[rn] = '\0';
        printf("pio_sdh: fstest OK - %s contains:\n  %s", path, rd);
        printf("  (A/B: move card to HW SDH, run `sdcard` then `cat %s`)\n", path);
        return 0;
    }

    /* FatFS-level file read/write throughput (needs a prior `mount`).
     * usage: pio_sdcard fspeed [kb]  (default 2048 = 2 MiB, 64 KiB chunks) */
    if (strcmp(argv[1], "fspeed") == 0) {
        static const char *path = "/sdcard/pio_speed.bin";
        enum { CHUNK = 64 * 1024 };
        uint32_t kb = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 2048;
        uint32_t chunks = (kb * 1024u + CHUNK - 1u) / CHUNK;
        if (!chunks) chunks = 1;
        uint8_t *buf = pvPortMalloc(CHUNK);
        if (!buf) {
            printf("pio_sdh: fspeed no memory\n");
            return 0;
        }
        for (uint32_t i = 0; i < CHUNK; i++) buf[i] = (uint8_t)(i * 7u + 3u);

        int fd = open(path, O_CREAT | O_TRUNC | O_RDWR);
        if (fd < 0) {
            printf("pio_sdh: fspeed open failed (%d) - mounted?\n", fd);
            vPortFree(buf);
            return 0;
        }
        hal_pio_sdh_wprof_get(NULL, NULL, NULL, NULL, NULL, NULL); /* clear */
        TickType_t t0 = xTaskGetTickCount();
        uint32_t done = 0;
        for (uint32_t i = 0; i < chunks; i++) {
            if (write(fd, buf, CHUNK) != CHUNK) break;
            done++;
        }
        close(fd);
        uint32_t wms = (uint32_t)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS);
        if (!wms) wms = 1;
        if (done != chunks) {
            printf("pio_sdh: fspeed write FAILED at chunk %lu\n", (unsigned long)done);
            vPortFree(buf);
            return 0;
        }
        uint32_t we, wf, wt, wc, wbc, wnb;
        hal_pio_sdh_wprof_get(&we, &wf, &wt, &wc, &wbc, &wnb);
        if (wnb) {
            printf("pio_sdh: write prof/block (%lu blk): expand %lu + feed %lu"
                   " + tokenbusy %lu ns = %lu ns (tb: cell %lu ns, busy %lu clk)\n",
                   (unsigned long)wnb, (unsigned long)we, (unsigned long)wf,
                   (unsigned long)wt, (unsigned long)(we + wf + wt),
                   (unsigned long)wc, (unsigned long)wbc);
        }

        fd = open(path, O_RDONLY);
        if (fd < 0) {
            printf("pio_sdh: fspeed re-open failed (%d)\n", fd);
            vPortFree(buf);
            return 0;
        }
        hal_pio_sdh_prof_get(NULL, NULL, NULL, NULL, NULL);   /* clear counters */
        t0 = xTaskGetTickCount();
        done = 0;
        for (uint32_t i = 0; i < chunks; i++) {
            if (read(fd, buf, CHUNK) != CHUNK) break;
            done++;
        }
        close(fd);
        uint32_t rms = (uint32_t)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS);
        if (!rms) rms = 1;
        if (done != chunks) {
            printf("pio_sdh: fspeed read FAILED at chunk %lu\n", (unsigned long)done);
            vPortFree(buf);
            return 0;
        }
        int bad = 0;                       /* verify the last chunk read back */
        for (uint32_t i = 0; i < CHUNK; i++) {
            if (buf[i] != (uint8_t)(i * 7u + 3u)) { bad++; break; }
        }
        uint32_t kib = chunks * (CHUNK / 1024u);
        printf("pio_sdh: fspeed %lu KiB file: write %lu KiB/s (%lu ms), read %lu KiB/s (%lu ms), data %s (%s)\n",
               (unsigned long)kib,
               (unsigned long)(kib * 1000u / wms), (unsigned long)wms,
               (unsigned long)(kib * 1000u / rms), (unsigned long)rms,
               bad ? "MISMATCH" : "verified", s_use_dma ? "DMA read" : "CPU read");
        uint32_t sc, ar, wa, fi, nb;
        hal_pio_sdh_prof_get(&sc, &ar, &wa, &fi, &nb);
        if (nb) {
            printf("pio_sdh: read prof/block (%lu blk): scan %lu + arm %lu + wait %lu"
                   " + finalize %lu ns = %lu ns\n",
                   (unsigned long)nb, (unsigned long)sc, (unsigned long)ar,
                   (unsigned long)wa, (unsigned long)fi,
                   (unsigned long)(sc + ar + wa + fi));
        }
        vPortFree(buf);
        return 0;
    }

#if defined(CONFIG_DEMO_PIO_SDH_TEST)
    /* Large-file FS stress: one file per call (host loops the index) so each
     * invocation stays bounded/monitorable. Each file gets a per-(idx,chunk)
     * seeded ramp pattern, so read-back verifies EVERY byte and catches a wrong
     * file, a misplaced chunk, or a bit error (unlike fspeed's last-chunk-only
     * check). usage: pio_sdcard fstress <w|r|x> <idx> [mb=1024]
     *   w = create+write, r = read+full-verify, x = delete. */
    if (strcmp(argv[1], "fstress") == 0) {
        enum { CHUNK = 64 * 1024 };
        if (argc < 4) {
            printf("pio_sdh: usage: fstress <w|r|x> <idx> [mb=1024]\n");
            return 0;
        }
        char mode = argv[2][0];
        uint32_t idx = (uint32_t)strtoul(argv[3], NULL, 0);
        uint32_t mb  = (argc > 4) ? (uint32_t)strtoul(argv[4], NULL, 0) : 1024u;
        char path[40];
        snprintf(path, sizeof(path), "/sdcard/pio_str_%lu.bin", (unsigned long)idx);

        if (mode == 'x') {
            int rc = unlink(path);
            printf("pio_sdh: fstress x idx=%lu rc=%d\n", (unsigned long)idx, rc);
            return 0;
        }
        uint32_t chunks = mb * (1024u * 1024u / CHUNK);   /* mb MB / 64 KiB */
        if (!chunks) chunks = 1;
        uint8_t *buf = pvPortMalloc(CHUNK);
        if (!buf) {
            printf("pio_sdh: fstress no memory\n");
            return 0;
        }
        if (mode == 'w') {
            int fd = open(path, O_CREAT | O_TRUNC | O_RDWR);
            if (fd < 0) {
                printf("pio_sdh: fstress w idx=%lu open failed (%d)\n",
                       (unsigned long)idx, fd);
                vPortFree(buf);
                return 0;
            }
            uint64_t io_us = 0;              /* pure write I/O time (fill excluded) */
            uint32_t done = 0;
            for (uint32_t c = 0; c < chunks; c++) {
                uint32_t seed = idx * 0x9E3779B1u + c * 0x85EBCA77u;
                for (uint32_t i = 0; i < CHUNK; i++) buf[i] = (uint8_t)(seed + i);
                uint64_t t = mtimer_get_time_us();
                int r = write(fd, buf, CHUNK);
                io_us += mtimer_get_time_us() - t;
                if (r != (int)CHUNK) break;
                done++;
            }
            uint64_t tc = mtimer_get_time_us();
            close(fd);                          /* time the final flush too */
            io_us += mtimer_get_time_us() - tc;
            if (!io_us) io_us = 1;
            uint32_t kib = done * (CHUNK / 1024u);
            if (done != chunks)
                printf("pio_sdh: fstress w idx=%lu FAILED at chunk %lu/%lu (%lu KiB, ENOSPC?)\n",
                       (unsigned long)idx, (unsigned long)done, (unsigned long)chunks,
                       (unsigned long)kib);
            else
                printf("pio_sdh: fstress w idx=%lu %lu KiB in %lu ms = %lu KiB/s\n",
                       (unsigned long)idx, (unsigned long)kib,
                       (unsigned long)(io_us / 1000u),
                       (unsigned long)((uint64_t)kib * 1000000ULL / io_us));
        } else {   /* 'r' : pure-read throughput (timed) + full byte verify (untimed) */
            int fd = open(path, O_RDONLY);
            if (fd < 0) {
                printf("pio_sdh: fstress r idx=%lu open failed (%d)\n",
                       (unsigned long)idx, fd);
                vPortFree(buf);
                return 0;
            }
            uint64_t io_us = 0;              /* pure read I/O time (verify excluded) */
            uint32_t done = 0;
            long bad = -1;
            for (uint32_t c = 0; c < chunks; c++) {
                uint64_t t = mtimer_get_time_us();
                int r = read(fd, buf, CHUNK);
                io_us += mtimer_get_time_us() - t;
                if (r != (int)CHUNK) break;
                done++;
                uint32_t seed = idx * 0x9E3779B1u + c * 0x85EBCA77u;
                for (uint32_t i = 0; i < CHUNK; i++)   /* verify - NOT timed */
                    if (buf[i] != (uint8_t)(seed + i)) { bad = (long)(c * CHUNK + i); break; }
                if (bad >= 0) break;
            }
            close(fd);
            if (!io_us) io_us = 1;
            uint32_t kib = done * (CHUNK / 1024u);
            printf("pio_sdh: fstress r idx=%lu %lu KiB in %lu ms = %lu KiB/s verify=%s@%ld\n",
                   (unsigned long)idx, (unsigned long)kib,
                   (unsigned long)(io_us / 1000u),
                   (unsigned long)((uint64_t)kib * 1000000ULL / io_us),
                   (bad < 0 && done == chunks) ? "OK" : "BAD", bad);
        }
        vPortFree(buf);
        return 0;
    }

    /* Batched large/small-file FS stress: write N files, read+verify all N,
     * delete all N - ALL on the DUT (no per-file host round-trip). Each file
     * gets a per-(idx,chunk) seeded ramp so read-back verifies every byte.
     * Prints per-file KiB/s + a final aggregate. usage:
     *   pio_sdcard fstress_batch <nfiles> [mb=64] */
    if (strcmp(argv[1], "fstress_batch") == 0) {
        enum { CHUNK = 64 * 1024 };
        if (argc < 3) {
            printf("pio_sdh: usage: fstress_batch <nfiles> [mb=64]\n");
            return 0;
        }
        uint32_t nfiles = (uint32_t)strtoul(argv[2], NULL, 0);
        uint32_t mb     = (argc > 3) ? (uint32_t)strtoul(argv[3], NULL, 0) : 64u;
        uint32_t chunks = mb * (1024u * 1024u / CHUNK);
        if (!chunks) chunks = 1;
        uint32_t kib = chunks * (CHUNK / 1024u);
        uint8_t *buf = pvPortMalloc(CHUNK);
        if (!buf) { printf("pio_sdh: fstress_batch no memory\n"); return 0; }
        char path[40];
        uint32_t wmin = 0xFFFFFFFFu, wmax = 0, rmin = 0xFFFFFFFFu, rmax = 0;
        uint64_t wsum = 0, rsum = 0;
        uint32_t wok = 0, rok = 0, vok = 0;
        int failed = 0;
        printf("pio_sdh: fstress_batch %lu files x %lu MB: WRITE...\n",
               (unsigned long)nfiles, (unsigned long)mb);
        for (uint32_t f = 0; f < nfiles && !failed; f++) {
            snprintf(path, sizeof(path), "/sdcard/pio_str_%lu.bin", (unsigned long)f);
            int fd = open(path, O_CREAT | O_TRUNC | O_RDWR);
            if (fd < 0) { printf("pio_sdh: batch W f=%lu open %d\n", (unsigned long)f, fd); failed = 1; break; }
            uint64_t io_us = 0;             /* pure write I/O time (fill excluded) */
            for (uint32_t c = 0; c < chunks; c++) {
                uint32_t seed = f * 0x9E3779B1u + c * 0x85EBCA77u;
                for (uint32_t i = 0; i < CHUNK; i++) buf[i] = (uint8_t)(seed + i);
                uint64_t t = mtimer_get_time_us();
                int r = write(fd, buf, CHUNK);
                io_us += mtimer_get_time_us() - t;
                if (r != (int)CHUNK) { failed = 1; break; }
            }
            uint64_t tc = mtimer_get_time_us();
            close(fd);
            io_us += mtimer_get_time_us() - tc;
            if (!io_us) io_us = 1;
            if (failed) { printf("pio_sdh: batch W f=%lu WRITE-FAIL (ENOSPC?)\n", (unsigned long)f); break; }
            uint32_t kbs = (uint32_t)((uint64_t)kib * 1000000ULL / io_us);
            wsum += kbs; wok++;
            if (kbs < wmin) wmin = kbs;
            if (kbs > wmax) wmax = kbs;
            printf("pio_sdh: batch W f=%lu %lu KiB/s\n", (unsigned long)f, (unsigned long)kbs);
        }
        if (!failed) {
            printf("pio_sdh: fstress_batch READ+VERIFY...\n");
            for (uint32_t f = 0; f < nfiles; f++) {
                snprintf(path, sizeof(path), "/sdcard/pio_str_%lu.bin", (unsigned long)f);
                int fd = open(path, O_RDONLY);
                if (fd < 0) { printf("pio_sdh: batch R f=%lu open %d\n", (unsigned long)f, fd); continue; }
                uint64_t io_us = 0;        /* pure read I/O time (verify excluded) */
                long bad = -1;
                uint32_t got = 0;
                for (uint32_t c = 0; c < chunks; c++) {
                    uint64_t t = mtimer_get_time_us();
                    int r = read(fd, buf, CHUNK);
                    io_us += mtimer_get_time_us() - t;
                    if (r != (int)CHUNK) break;
                    got++;
                    uint32_t seed = f * 0x9E3779B1u + c * 0x85EBCA77u;
                    for (uint32_t i = 0; i < CHUNK; i++)   /* verify - NOT timed */
                        if (buf[i] != (uint8_t)(seed + i)) { bad = (long)(c * CHUNK + i); break; }
                    if (bad >= 0) break;
                }
                close(fd);
                if (!io_us) io_us = 1;
                int good = (bad < 0 && got == chunks);
                uint32_t kbs = (uint32_t)((uint64_t)got * (CHUNK / 1024u) * 1000000ULL / io_us);
                rsum += kbs; rok++;
                if (kbs < rmin) rmin = kbs;
                if (kbs > rmax) rmax = kbs;
                if (good) vok++;
                printf("pio_sdh: batch R f=%lu %lu KiB/s verify=%s@%ld\n",
                       (unsigned long)f, (unsigned long)kbs, good ? "OK" : "BAD", bad);
            }
        }
        for (uint32_t f = 0; f < nfiles; f++) {
            snprintf(path, sizeof(path), "/sdcard/pio_str_%lu.bin", (unsigned long)f);
            unlink(path);
        }
        vPortFree(buf);
        printf("pio_sdh: fstress_batch DONE: W %lu/%lu ok [min %lu mean %lu max %lu], "
               "R %lu/%lu ok [min %lu mean %lu max %lu], verify %lu/%lu, %lu MB each\n",
               (unsigned long)wok, (unsigned long)nfiles,
               (unsigned long)(wok ? wmin : 0), (unsigned long)(wok ? wsum / wok : 0), (unsigned long)wmax,
               (unsigned long)rok, (unsigned long)nfiles,
               (unsigned long)(rok ? rmin : 0), (unsigned long)(rok ? rsum / rok : 0), (unsigned long)rmax,
               (unsigned long)vok, (unsigned long)nfiles, (unsigned long)mb);
        return 0;
    }

    /* Mixed large+small FS soak, time-bounded, on-device autonomous. Each cycle
     * writes -> reads+verifies -> deletes one large file (64 MB) then a set of
     * small files (4 MB / 1 MB / 256 KB / 64 KB), interleaved. Per-(file,chunk)
     * seeded ramp catches a wrong/stale/short read. Throughput is pure I/O
     * (mtimer us; fill+verify excluded), split into LARGE vs SMALL classes.
     * usage: pio_sdcard fstress_mix [seconds=3600] */
    if (strcmp(argv[1], "fstress_mix") == 0) {
        enum { CHUNK = 64 * 1024 };
        uint32_t secs = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 3600u;
        static const uint32_t mix[] = { 1024u, 64u, 16u, 4u, 1u }; /* chunks: 64M,4M,1M,256K,64K */
        uint8_t *buf = pvPortMalloc(CHUNK);
        if (!buf) { printf("pio_sdh: fstress_mix no memory\n"); return 0; }
        uint32_t wmin[2] = { 0xFFFFFFFFu, 0xFFFFFFFFu }, wmax[2] = { 0, 0 };
        uint32_t rmin[2] = { 0xFFFFFFFFu, 0xFFFFFFFFu }, rmax[2] = { 0, 0 };
        uint64_t wsum[2] = { 0, 0 }, rsum[2] = { 0, 0 };
        uint32_t wn[2] = { 0, 0 }, rn[2] = { 0, 0 }, vok[2] = { 0, 0 }, vbad = 0;
        uint32_t fid = 0, cyc = 0;
        int failed = 0;
        char path[40];
        uint64_t t0 = mtimer_get_time_us();
        uint64_t budget = (uint64_t)secs * 1000000ULL;
        printf("pio_sdh: fstress_mix START secs=%lu (cycle=large 64MB + small 4M/1M/256K/64K)\n",
               (unsigned long)secs);
        while (!failed && (mtimer_get_time_us() - t0) < budget) {
            for (uint32_t m = 0; m < sizeof(mix) / sizeof(mix[0]) && !failed; m++) {
                uint32_t chunks = mix[m];
                uint32_t cls = (chunks >= 1024u) ? 0u : 1u;   /* 0=large 1=small */
                uint32_t kib = chunks * (CHUNK / 1024u);
                snprintf(path, sizeof(path), "/sdcard/pio_mix_%lu.bin", (unsigned long)(fid & 7u));
                int fd = open(path, O_CREAT | O_TRUNC | O_RDWR);
                if (fd < 0) { printf("pio_sdh: mix W open %d\n", fd); failed = 1; break; }
                uint64_t wio = 0;
                for (uint32_t c = 0; c < chunks; c++) {
                    uint32_t seed = fid * 0x9E3779B1u + c * 0x85EBCA77u;
                    for (uint32_t i = 0; i < CHUNK; i++) buf[i] = (uint8_t)(seed + i);
                    uint64_t t = mtimer_get_time_us();
                    int r = write(fd, buf, CHUNK);
                    wio += mtimer_get_time_us() - t;
                    if (r != (int)CHUNK) { failed = 1; break; }
                }
                uint64_t tc = mtimer_get_time_us();
                close(fd);
                wio += mtimer_get_time_us() - tc;
                if (failed) { printf("pio_sdh: mix WRITE-FAIL fid=%lu\n", (unsigned long)fid); break; }
                if (!wio) wio = 1;
                uint32_t wkbs = (uint32_t)((uint64_t)kib * 1000000ULL / wio);
                wsum[cls] += wkbs; wn[cls]++;
                if (wkbs < wmin[cls]) wmin[cls] = wkbs;
                if (wkbs > wmax[cls]) wmax[cls] = wkbs;
                fd = open(path, O_RDONLY);
                if (fd < 0) { printf("pio_sdh: mix R open %d\n", fd); failed = 1; break; }
                uint64_t rio = 0; long bad = -1; uint32_t got = 0;
                for (uint32_t c = 0; c < chunks; c++) {
                    uint64_t t = mtimer_get_time_us();
                    int r = read(fd, buf, CHUNK);
                    rio += mtimer_get_time_us() - t;
                    if (r != (int)CHUNK) break;
                    got++;
                    uint32_t seed = fid * 0x9E3779B1u + c * 0x85EBCA77u;
                    for (uint32_t i = 0; i < CHUNK; i++)     /* verify - NOT timed */
                        if (buf[i] != (uint8_t)(seed + i)) { bad = (long)(c * CHUNK + i); break; }
                    if (bad >= 0) break;
                }
                close(fd);
                if (!rio) rio = 1;
                int good = (bad < 0 && got == chunks);
                uint32_t rkbs = (uint32_t)((uint64_t)got * (CHUNK / 1024u) * 1000000ULL / rio);
                rsum[cls] += rkbs; rn[cls]++;
                if (rkbs < rmin[cls]) rmin[cls] = rkbs;
                if (rkbs > rmax[cls]) rmax[cls] = rkbs;
                if (good) { vok[cls]++; }
                else { vbad++; printf("pio_sdh: mix VERIFY-BAD fid=%lu %luKiB @%ld\n",
                                      (unsigned long)fid, (unsigned long)kib, bad); }
                unlink(path);
                fid++;
            }
            cyc++;
            if ((cyc % 8u) == 0u) {
                uint32_t el = (uint32_t)((mtimer_get_time_us() - t0) / 1000000ULL);
                printf("pio_sdh: mix cyc=%lu t=%lus L[W%lu R%lu v%lu] S[W%lu R%lu v%lu] bad=%lu\n",
                       (unsigned long)cyc, (unsigned long)el,
                       (unsigned long)(wn[0] ? wsum[0] / wn[0] : 0), (unsigned long)(rn[0] ? rsum[0] / rn[0] : 0), (unsigned long)vok[0],
                       (unsigned long)(wn[1] ? wsum[1] / wn[1] : 0), (unsigned long)(rn[1] ? rsum[1] / rn[1] : 0), (unsigned long)vok[1],
                       (unsigned long)vbad);
            }
        }
        for (uint32_t k = 0; k < 8u; k++) {
            snprintf(path, sizeof(path), "/sdcard/pio_mix_%lu.bin", (unsigned long)k);
            unlink(path);
        }
        vPortFree(buf);
        uint32_t el = (uint32_t)((mtimer_get_time_us() - t0) / 1000000ULL);
        printf("pio_sdh: fstress_mix DONE cyc=%lu elapsed=%lus\n", (unsigned long)cyc, (unsigned long)el);
        printf("pio_sdh:   LARGE 64MB : W[min %lu mean %lu max %lu] R[min %lu mean %lu max %lu] n=%lu verify %lu/%lu\n",
               (unsigned long)(wn[0] ? wmin[0] : 0), (unsigned long)(wn[0] ? wsum[0] / wn[0] : 0), (unsigned long)wmax[0],
               (unsigned long)(rn[0] ? rmin[0] : 0), (unsigned long)(rn[0] ? rsum[0] / rn[0] : 0), (unsigned long)rmax[0],
               (unsigned long)wn[0], (unsigned long)vok[0], (unsigned long)rn[0]);
        printf("pio_sdh:   SMALL 4M-64K: W[min %lu mean %lu max %lu] R[min %lu mean %lu max %lu] n=%lu verify %lu/%lu\n",
               (unsigned long)(wn[1] ? wmin[1] : 0), (unsigned long)(wn[1] ? wsum[1] / wn[1] : 0), (unsigned long)wmax[1],
               (unsigned long)(rn[1] ? rmin[1] : 0), (unsigned long)(rn[1] ? rsum[1] / rn[1] : 0), (unsigned long)rmax[1],
               (unsigned long)wn[1], (unsigned long)vok[1], (unsigned long)rn[1]);
        printf("pio_sdh:   verify FAIL total=%lu\n", (unsigned long)vbad);
        return 0;
    }

    /* Write-and-verify (P5): fill blocks with a deterministic SSO-stress
     * pattern, write (CMD24/25), read back (CMD17/18) and memcmp.
     * usage: pio_sdcard write <lba> [cnt]   -- lba REQUIRED (destructive!) */
    if (strcmp(argv[1], "write") == 0) {
        static uint8_t vrf[512];
        hal_pio_sdh_card_t card;
        if (argc < 3) {
            printf("pio_sdh: write needs an explicit <lba> (destructive!)\n");
            return 0;
        }
        uint32_t lba = (uint32_t)strtoul(argv[2], NULL, 0);
        uint32_t cnt = (argc > 3) ? (uint32_t)strtoul(argv[3], NULL, 0) : 1;
        if (cnt < 1) cnt = 1;
        if (cnt > MAXBLK) cnt = MAXBLK;
        if (hal_pio_sdh_init(&cfg, &card)) {
            printf("pio_sdh: init failed\n");
            return 0;
        }
        /* Pattern per block: counter bytes, 00/FF runs (max SSO), FAT-like
         * "xx 07 00 00" entries (the read path's worst case), LCG noise. */
        for (uint32_t b = 0; b < cnt; b++) {
            uint8_t *p = s_blk + b * 512u;
            uint32_t seed = (lba + b) * 2654435761u + 1u;
            for (int i = 0; i < 128; i++) p[i] = (uint8_t)(i + b);
            for (int i = 128; i < 256; i++) p[i] = (i & 1) ? 0xFF : 0x00;
            for (int i = 256; i < 384; i += 4) {
                p[i] = (uint8_t)(0x80 + (i >> 2)); p[i + 1] = 0x07;
                p[i + 2] = 0x00; p[i + 3] = 0x00;
            }
            for (int i = 384; i < 512; i++) {
                seed = seed * 1103515245u + 12345u;
                p[i] = (uint8_t)(seed >> 16);
            }
        }
        int rc = hal_pio_sdh_write_blocks(s_blk, lba, cnt);
        if (rc) {
            printf("pio_sdh: write LBA %lu x%lu FAILED rc=%d (-1 cmd/timeout, -3 crc-reject, -4 wr-err)\n",
                   (unsigned long)lba, (unsigned long)cnt, rc);
            return 0;
        }
        int bad = 0;
        for (uint32_t b = 0; b < cnt; b++) {
            int rrc = hal_pio_sdh_read_blocks(vrf, lba + b, 1);
            if (rrc) {                     /* transient? one retry, and say so */
                rrc = hal_pio_sdh_read_blocks(vrf, lba + b, 1);
                printf("pio_sdh: verify read LBA %lu retry %s\n",
                       (unsigned long)(lba + b), rrc ? "FAILED" : "passed");
            }
            if (rrc) {
                bad++;
                continue;
            }
            const uint8_t *w = s_blk + b * 512u;
            int diff = -1;
            for (int i = 0; i < 512; i++) {
                if (vrf[i] != w[i]) { diff = i; break; }
            }
            if (diff >= 0) {
                printf("pio_sdh: verify FAILED LBA %lu @%d: wr %02x %02x %02x %02x rd %02x %02x %02x %02x\n",
                       (unsigned long)(lba + b), diff,
                       w[diff], w[diff + 1], w[diff + 2], w[diff + 3],
                       vrf[diff], vrf[diff + 1], vrf[diff + 2], vrf[diff + 3]);
                bad++;
            }
        }
        printf("pio_sdh: write LBA %lu x%lu %s (%s, readback %s)\n",
               (unsigned long)lba, (unsigned long)cnt,
               bad ? "VERIFY-FAIL" : "OK",
               cnt > 1 ? "CMD25" : "CMD24", bad ? "mismatch" : "verified");
        return 0;
    }

#endif /* CONFIG_DEMO_PIO_SDH_TEST */

    /* Re-read one block WITHOUT re-enumerating (card must already be init'd by a
     * prior `init`/`read` this boot). Short, repeatable data phase for LA capture
     * of the CMD17 4-bit DATA burst. */
    if (strcmp(argv[1], "reread") == 0) {
        static uint8_t rblk[512];
        uint32_t lba = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 3;
        int rc = hal_pio_sdh_read_blocks(rblk, lba, 1);
        printf("pio_sdh: reread LBA %lu rc=%d (tail %02x %02x %02x %02x)\n",
               (unsigned long)lba, rc, rblk[508], rblk[509], rblk[510], rblk[511]);
        return 0;
    }

    /* Toggle the DMA data path / channels (applies to the NEXT init/read). */
    if (strcmp(argv[1], "dma") == 0) {
        s_use_dma = (argc > 2) ? (uint8_t)(atoi(argv[2]) != 0) : 1;
        if (argc > 3) s_dma_tx_ch = (uint8_t)atoi(argv[3]);
        if (argc > 4) s_dma_rx_ch = (uint8_t)atoi(argv[4]);
        printf("pio_sdh: DMA data path %s tx_ch=%u rx_ch=%u (next init/read)\n",
               s_use_dma ? "ON" : "OFF", s_dma_tx_ch, s_dma_rx_ch);
        return 0;
    }

    /* Select the PIO state machine (DBG: SM1 has the CCI-proven DREQ wiring). */
    if (strcmp(argv[1], "sm") == 0) {
        s_sm = (argc > 2) ? (uint8_t)(atoi(argv[2]) & 1) : 0;
        printf("pio_sdh: using SM%u (next init/read)\n", s_sm);
        return 0;
    }

    /* SI probe: inter-nibble CLK-low gap in SM cycles (applies to the next
     * data phase, no re-init needed). */
    if (strcmp(argv[1], "gap") == 0) {
        uint8_t g = (argc > 2) ? (uint8_t)atoi(argv[2]) : 0;  /* no-arg -> boot default 0 */
        if (hal_pio_sdh_set_dat_gap(g)) {
            printf("pio_sdh: gap out of range (0..30)\n");
            return 0;
        }
        printf("pio_sdh: DATA inter-nibble gap = %u cycles\n", g);
        return 0;
    }

    /* DBG: enable/disable the aborted-transfer stop recovery (stress raw path). */
    if (strcmp(argv[1], "recover") == 0) {
        uint8_t on = (argc > 2) ? (uint8_t)atoi(argv[2]) : 1;
        hal_pio_sdh_dbg_set_recover(on);
        printf("pio_sdh: abort recovery %s\n", on ? "ON" : "OFF (raw stress)");
        return 0;
    }

    /* CMD6 SWITCH_FUNC -> High-Speed mode (run after `init`). Prints the 512-bit
     * switch status head + the group-1 function nibble (1 = HS granted). */
    if (strcmp(argv[1], "hs") == 0) {
        uint8_t st[64];
        int rc = hal_pio_sdh_switch_hs(st);
        printf("pio_sdh: CMD6 HS switch rc=%d (grp1 sel=%u, 1=HS)\n", rc, st[16] & 0x0Fu);
        printf("  status[0..19]:");
        for (int i = 0; i < 20; i++) printf(" %02x", st[i]);
        printf("\n");
        return 0;
    }

    /* DBG: force read sample edge (0 = DS low-phase, 1 = HS rising-latch). */
    if (strcmp(argv[1], "rising") == 0) {
        uint8_t on = (argc > 2) ? (uint8_t)atoi(argv[2]) : 1;
        hal_pio_sdh_dbg_set_rising(on);
        printf("pio_sdh: read sample = %s\n", on ? "rising latch (HS)" : "low phase (DS)");
        return 0;
    }

    /* DBG (R3 probe): route 4-bit DMA reads through the self-clocked read loop. */
    if (strcmp(argv[1], "rauto") == 0) {
        uint8_t on = (argc > 2) ? (uint8_t)atoi(argv[2]) : 1;
        hal_pio_sdh_dbg_set_rauto(on);
        printf("pio_sdh: read path = %s\n",
               on ? "self-clocked (R3 probe)" : "TX-paced (default)");
        return 0;
    }

    /* Read-loop SM cycles/nibble -> SD CLK: 4 = 24 MHz (default, solid),
     * 3 = 32 MHz (+33%, 1 settle cycle), 2 = 48 MHz (~2x wire, SI-marginal on
     * jumper wiring). Mostly matters for raw reads - the FS path is finalize/
     * scan-bound. usage: pio_sdcard rcyc <2|3|4> */
    if (strcmp(argv[1], "rcyc") == 0) {
        uint8_t cyc = (argc > 2) ? (uint8_t)atoi(argv[2]) : 4;
        hal_pio_sdh_dbg_set_read2c(cyc);
        printf("pio_sdh: read loop = %u cyc/nibble (~%u MHz CLK)%s\n",
               (cyc == 2u || cyc == 3u) ? cyc : 4u,
               cyc == 2u ? 48u : (cyc == 3u ? 32u : 24u),
               cyc == 2u ? " EXPERIMENTAL/SI-marginal" : (cyc == 3u ? " experimental" : " default"));
        return 0;
    }

    /* Hardware `jmp pin` start-bit hunt (rising 4-cyc read), default on. Replaces
     * the CPU per-nibble scan (~12 us/block) with an SM self-clocked hunt
     * (+17-24% read). `rhunt 0` restores the CPU scan. usage:
     * pio_sdcard rhunt <0|1> */
    if (strcmp(argv[1], "rhunt") == 0) {
        uint8_t on = (argc > 2) ? (uint8_t)atoi(argv[2]) : 1;
        hal_pio_sdh_dbg_set_rhunt(on);
        printf("pio_sdh: read start-hunt = %s\n",
               on ? "hardware jmp-pin (default)" : "CPU scan (fallback)");
        return 0;
    }

    /* Write-direction inter-nibble gap sweep (host-pad SSO). */
    if (strcmp(argv[1], "wgap") == 0) {
        uint8_t g = (argc > 2) ? (uint8_t)atoi(argv[2]) : 0;
        if (hal_pio_sdh_set_wr_gap(g)) {
            printf("pio_sdh: wgap out of range (0..30)\n");
            return 0;
        }
        printf("pio_sdh: write inter-nibble gap = %u cycles\n", g);
        return 0;
    }

    /* Select the SM-autonomous write path (W3): feed+token+busy on the SM so
     * the CPU expands the next block during the whole SD phase. With the packed
     * expand + wgap 0 default the write is wire-bound (~6.4 MB/s). Arg: 0=off
     * (default DMA-fed CPU token/busy), 1=on. */
    if (strcmp(argv[1], "wauto") == 0) {
        uint8_t on = (argc > 2) ? (uint8_t)atoi(argv[2]) : 1u;
        uint32_t st = 0;
        hal_pio_sdh_set_wauto(on, &st);
        printf("pio_sdh: write path = %s (last CRC status 0x%lx, 0x2=accepted)\n",
               on ? "SM-autonomous (W3)" : "DMA-fed CPU token/busy",
               (unsigned long)st);
        return 0;
    }

#if defined(CONFIG_DEMO_PIO_SDH_TEST)
    /* Sequential-read throughput: init once, stream CMD18 in 8-block chunks.
     * usage: pio_sdcard speedtest [lba] [kb] */
    if (strcmp(argv[1], "speedtest") == 0) {
        hal_pio_sdh_card_t card;
        uint32_t lba = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 0;
        uint32_t kb  = (argc > 3) ? (uint32_t)strtoul(argv[3], NULL, 0) : 512;
        uint32_t chunks = (kb * 2u) / MAXBLK;              /* 2 blocks per KB   */
        if (!chunks) chunks = 1;
        if (hal_pio_sdh_init(&cfg, &card)) {
            printf("pio_sdh: init failed\n");
            return 0;
        }
        hal_pio_sdh_prof_get(NULL, NULL, NULL, NULL, NULL);   /* clear counters */
        TickType_t t0 = xTaskGetTickCount();
        for (uint32_t i = 0; i < chunks; i++) {
            int rc = hal_pio_sdh_read_blocks(s_blk, lba + i * MAXBLK, MAXBLK);
            if (rc) {
                printf("pio_sdh: speedtest FAILED rc=%d at LBA %lu\n",
                       rc, (unsigned long)(lba + i * MAXBLK));
                return 0;
            }
        }
        uint32_t ms = (uint32_t)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS);
        if (!ms) ms = 1;
        uint32_t kib = chunks * MAXBLK / 2u;
        printf("pio_sdh: speedtest %lu KiB in %lu ms = %lu KiB/s (%s)\n",
               (unsigned long)kib, (unsigned long)ms,
               (unsigned long)(kib * 1000u / ms), s_use_dma ? "DMA" : "CPU");
        /* Per-block phase breakdown (DMA path only): attribute the fixed
         * software cost that dominates once the wire is fast. */
        uint32_t sc, ar, wa, fi, nb;
        hal_pio_sdh_prof_get(&sc, &ar, &wa, &fi, &nb);
        if (nb) {
            printf("pio_sdh: prof/block (%lu blk): scan %lu + arm %lu + wait %lu"
                   " + finalize %lu ns = %lu ns\n",
                   (unsigned long)nb, (unsigned long)sc, (unsigned long)ar,
                   (unsigned long)wa, (unsigned long)fi,
                   (unsigned long)(sc + ar + wa + fi));
        }
        return 0;
    }

#endif /* CONFIG_DEMO_PIO_SDH_TEST */

    /* Runtime CLK pad drive-strength override (0..3, default pad cfg is 3).
     * SI experiment for the dense-data nibble drop: sweep drive then `reread 3`
     * without reflashing. Needs a prior `init`/`read` this boot. */
    if (strcmp(argv[1], "drive") == 0) {
        uint8_t d = (argc > 2) ? (uint8_t)atoi(argv[2]) : 0;
        if (hal_pio_sdh_set_clk_drive(d)) {
            printf("pio_sdh: drive: run 'pio_sdcard init' first\n");
            return 0;
        }
        printf("pio_sdh: CLK pad drive set to %u\n", d);
        return 0;
    }

    /* Switch DATA bus width at runtime (SSO probe: 1-bit = only DAT0 toggles,
     * no 4-line simultaneous switching). Needs a prior `init`/`read` this boot;
     * follow with `reread 3` to compare against the 4-bit failure. */
    if (strcmp(argv[1], "bus") == 0) {
        uint8_t w = (argc > 2) ? (uint8_t)atoi(argv[2]) : 4;
        int rc = hal_pio_sdh_set_bus_width(w);
        if (rc) {
            printf("pio_sdh: bus width switch FAILED rc=%d (init'd? width 1|4?)\n", rc);
            return 0;
        }
        printf("pio_sdh: bus width set to %u-bit\n", w);
        return 0;
    }

    printf("pio_sdh: unknown subcommand '%s'\n", argv[1]);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pio_sdcard, pio_sdcard, PIO SD host bring-up test);

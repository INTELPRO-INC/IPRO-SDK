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
 * @file llm_host.c
 * @brief Host checks of the engine against the PyTorch reference
 *        (tools/ipro_llm/export/make_golden.py). See run.sh.
 *
 *   llm_host <blob> tok    <tok.tsv>             prompt ids identical, every row
 *   llm_host <blob> logits <logits.bin>          full-vocab logits after the prompt
 *   llm_host <blob> decode <decode.tsv> [int8|accel]   generated ids and JSON
 *   llm_host <blob> run    "<request>"  [int8|accel]   one request, JSON and timing
 *
 * The float kernel (default) must match the PyTorch reference: exit status 0
 * only if every row matches (logits: within tolerance and the same argmax).
 * The int8 kernel is the board's arithmetic and is not bit-exact; decode then
 * passes if its exact match against the dataset answers is within
 * INT8_MAX_DROP of the reference's. accel adds the NPU prefill's arithmetic
 * (int8 kernel for decode, the reference accelerator for the prompt's
 * projections) and is judged the same way; a blob without the NPUS section
 * skips it.
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ipro_llm.h"
#include "../src/llm_internal.h"

#define LINE_MAX_BYTES 8192
#define INT8_MAX_DROP  0.005

uint32_t ipro_llm_clock(void)       /* microseconds, for the profile */
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)((uint64_t)ts.tv_sec * 1000000u + (uint64_t)ts.tv_nsec / 1000u);
}

static void *read_file(const char *path, size_t *len)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        perror(path);
        exit(2);
    }
    fseek(f, 0, SEEK_END);
    const long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    void *buf = malloc((size_t)n + 1);
    if (!buf || fread(buf, 1, (size_t)n, f) != (size_t)n) {
        fprintf(stderr, "%s: read failed\n", path);
        exit(2);
    }
    fclose(f);
    ((char *)buf)[n] = '\0';
    *len = (size_t)n;
    return buf;
}

static void unescape(char *s)
{
    char *o = s;
    for (; *s; s++) {
        if (*s == '\\' && s[1]) {
            s++;
            *o++ = *s == 't' ? '\t' : *s == 'n' ? '\n' : *s == 'r' ? '\r' : *s;
        } else {
            *o++ = *s;
        }
    }
    *o = '\0';
}

static int parse_ids(char *s, uint16_t *ids, int max)
{
    int n = 0;
    for (char *tok = strtok(s, " "); tok && n < max; tok = strtok(NULL, " ")) {
        ids[n++] = (uint16_t)strtoul(tok, NULL, 10);
    }
    return n;
}

static double now_s(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

static int check_tok(ipro_llm_t *m, const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        perror(path);
        return 2;
    }
    static char line[LINE_MAX_BYTES];
    long rows = 0, bad = 0;
    while (fgets(line, sizeof line, f)) {
        line[strcspn(line, "\n")] = '\0';
        char *tab = strchr(line, '\t');
        if (!tab) {
            continue;
        }
        *tab = '\0';
        unescape(line);
        uint16_t want[IPRO_LLM_MAX_PROMPT * 2], got[IPRO_LLM_MAX_PROMPT * 2];
        const int nw = parse_ids(tab + 1, want, IPRO_LLM_MAX_PROMPT * 2);
        const int ng = llm_encode_prompt(m, line, got, IPRO_LLM_MAX_PROMPT * 2);
        rows++;
        if (ng != nw || memcmp(want, got, (size_t)nw * sizeof *want) != 0) {
            if (bad++ < 8) {
                printf("MISMATCH %s\n  want", line);
                for (int i = 0; i < nw; i++) {
                    printf(" %u", want[i]);
                }
                printf("\n  got ");
                for (int i = 0; i < ng; i++) {
                    printf(" %u", got[i]);
                }
                printf(" (%d)\n", ng);
            }
        }
    }
    fclose(f);
    printf("tok: %ld rows, %ld mismatches\n", rows, bad);
    return bad ? 1 : 0;
}

static int check_logits(ipro_llm_t *m, const char *path)
{
    size_t len;
    uint8_t *b = read_file(path, &len), *p = b;
    const uint32_t n = llm_rd32(p), vocab = llm_rd32(p + 4);
    p += 8;
    float *ref = malloc(vocab * sizeof *ref), *got = malloc(vocab * sizeof *got);
    double worst = 0.0, worst_rel = 0.0;
    int bad_argmax = 0;
    for (uint32_t r = 0; r < n; r++) {
        const uint32_t k = llm_rd32(p);
        p += 4;
        uint16_t ids[IPRO_LLM_MAX_PROMPT];
        for (uint32_t i = 0; i < k && i < IPRO_LLM_MAX_PROMPT; i++) {
            ids[i] = llm_rd16(p + 2 * i);
        }
        for (uint32_t i = 0; i < k; i += LLM_BATCH) {
            llm_forward(m, ids + i, (int)i, (int)(k - i < LLM_BATCH ? k - i : LLM_BATCH));
        }
        p += 2 * k;
        memcpy(ref, p, vocab * sizeof *ref);
        p += vocab * sizeof *ref;
        llm_logits_all(m, got);
        uint32_t am_ref = 0, am_got = 0;
        double span = 0.0;
        for (uint32_t t = 0; t < vocab; t++) {
            const double e = fabs((double)got[t] - ref[t]);
            worst = e > worst ? e : worst;
            span = fabs(ref[t]) > span ? fabs(ref[t]) : span;
            am_ref = ref[t] > ref[am_ref] ? t : am_ref;
            am_got = got[t] > got[am_got] ? t : am_got;
        }
        double row_worst = 0.0;
        for (uint32_t t = 0; t < vocab; t++) {
            const double e = fabs((double)got[t] - ref[t]) / span;
            row_worst = e > row_worst ? e : row_worst;
        }
        worst_rel = row_worst > worst_rel ? row_worst : worst_rel;
        bad_argmax += am_ref != am_got;
    }
    printf("logits: %u prompts, max |err| %.3g, max err / max|logit| %.3g, argmax differs %d\n",
           n, worst, worst_rel, bad_argmax);
    free(b);
    free(ref);
    free(got);
    return worst_rel < 1e-4 && bad_argmax == 0 ? 0 : 1;
}

static int check_decode(ipro_llm_t *m, const char *path, const char *mode, bool approx)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        perror(path);
        return 2;
    }
    static char line[LINE_MAX_BYTES];
    long rows = 0, bad_ids = 0, bad_json = 0, prompt_tok = 0, gen_tok = 0;
    long answered = 0, ref_right = 0, eng_right = 0;
    double t = 0.0;
    ipro_llm_prof_t sum = { 0 };
    /* LLM_DUMP=<file>: every row's generated ids, to diff two engine builds */
    const char *dump_path = getenv("LLM_DUMP");
    FILE *dump = dump_path ? fopen(dump_path, "w") : NULL;
    while (fgets(line, sizeof line, f)) {
        line[strcspn(line, "\n")] = '\0';
        char *t1 = strchr(line, '\t'), *t2 = t1 ? strchr(t1 + 1, '\t') : NULL;
        if (!t2) {
            continue;
        }
        char *t3 = strchr(t2 + 1, '\t');      /* optional: the dataset answer */
        *t1 = *t2 = '\0';
        if (t3) {
            *t3 = '\0';
        }
        unescape(line);
        uint16_t want[IPRO_LLM_MAX_NEW + 1];
        const int nw = parse_ids(t1 + 1, want, IPRO_LLM_MAX_NEW + 1);
        ipro_llm_result_t res;
        const double t0 = now_s();
        const int rc = ipro_llm_run(m, line, &res);
        t += now_s() - t0;
        char json[1024] = "ERROR";
        if (rc == IPRO_LLM_OK) {
            ipro_llm_result_json(m, &res, json, sizeof json);
        }
        rows++;
        if (dump) {
            for (int i = 0; i < res.n_gen; i++) {
                fprintf(dump, i ? " %u" : "%u", res.gen[i]);
            }
            fprintf(dump, "\n");
        }
        prompt_tok += res.n_prompt;
        gen_tok += res.n_gen;
        const ipro_llm_prof_t *pf = ipro_llm_profile(m);
        sum.embed += pf->embed, sum.engram += pf->engram, sum.attn_proj += pf->attn_proj;
        sum.attn_core += pf->attn_core, sum.mlp += pf->mlp, sum.head += pf->head;
        sum.prefill += pf->prefill, sum.decode += pf->decode;
        sum.accel_runs = (uint16_t)(sum.accel_runs + (pf->accel_runs ? 1 : 0));
        sum.accel_fails = (uint16_t)(sum.accel_fails + (pf->accel_fails ? 1 : 0));
        const bool ids_ok = res.n_gen == nw && memcmp(want, res.gen, (size_t)nw * sizeof *want) == 0;
        const bool json_ok = strcmp(json, t2 + 1) == 0;
        bad_ids += !ids_ok;
        bad_json += !json_ok;
        if (t3) {
            answered++;
            ref_right += strcmp(t2 + 1, t3 + 1) == 0;
            eng_right += strcmp(json, t3 + 1) == 0;
        }
        if ((!ids_ok || !json_ok) && bad_ids + bad_json <= (approx ? 4 : 8)) {
            printf("MISMATCH %s\n  want %s\n  got  %s (rc %d)\n", line, t2 + 1, json, rc);
        }
    }
    fclose(f);
    if (dump) {
        fclose(dump);
    }
    const double nr = (double)(rows ? rows : 1);
    printf("decode (%s): %ld rows, ids differ %ld, json differ %ld (%.2f %% same calls); "
           "%.2f ms per request (%.1f prompt + %.1f generated tokens)\n",
           mode, rows, bad_ids, bad_json, 100.0 * (1.0 - (double)bad_json / nr),
           1e3 * t / nr, (double)prompt_tok / nr, (double)gen_tok / nr);
    const double total = (double)sum.prefill + (double)sum.decode;
    if (total > 0) {
        printf("  time: embed %.1f%%  engram %.1f%%  attn proj %.1f%%  attn core %.1f%%  "
               "mlp %.1f%%  head %.1f%%\n", 100.0 * sum.embed / total, 100.0 * sum.engram / total,
               100.0 * sum.attn_proj / total, 100.0 * sum.attn_core / total,
               100.0 * sum.mlp / total, 100.0 * sum.head / total);
    }
    if (!strcmp(mode, "accel")) {
        printf("  accelerator: projections of %u rows, %u rows fell back to the CPU\n",
               sum.accel_runs, sum.accel_fails);
    }
    if (!answered) {
        return bad_ids || bad_json ? 1 : 0;
    }
    const double ref = (double)ref_right / (double)answered, eng = (double)eng_right / (double)answered;
    printf("  exact match vs the dataset answers: reference %.4f, engine %.4f (%+.4f)\n",
           ref, eng, eng - ref);
    if (approx) {
        return eng >= ref - INT8_MAX_DROP ? 0 : 1;
    }
    return bad_ids || bad_json ? 1 : 0;
}

int main(int argc, char **argv)
{
    if (argc != 4 && argc != 5) {
        fprintf(stderr, "usage: %s <blob> tok|logits|decode|run <file or request> [float|int8|accel]\n",
                argv[0]);
        return 2;
    }
    const char *mode = argc == 5 ? argv[4] : "float";
    const bool accel = !strcmp(mode, "accel");
    const bool int8 = accel || !strcmp(mode, "int8");
    size_t len;
    void *blob = read_file(argv[1], &len);
    const size_t need = ipro_llm_work_bytes(blob, len);
    void *work = malloc(need);
    ipro_llm_t *m;
    const int rc = ipro_llm_open(&m, blob, len, work, need);
    if (rc) {
        fprintf(stderr, "ipro_llm_open: %d\n", rc);
        return 2;
    }
    ipro_llm_set_kernel(m, int8 ? IPRO_LLM_KERNEL_INT8 : IPRO_LLM_KERNEL_FLOAT);
    static ipro_llm_accel_t ref;
    if (accel) {
        const size_t scratch_len = ipro_llm_accel_scratch_bytes(m);
        if (!scratch_len) {
            printf("accel: the blob has no NPUS section (export_blob.py --npu-calib), skipped\n");
            return 0;
        }
        const size_t ref_len = ipro_llm_accel_ref_bytes(m);
        void *ref_mem = malloc(ref_len), *scratch = malloc(scratch_len);
        int err = ipro_llm_accel_ref(&ref, ref_mem, ref_len, m);
        err = err ? err : ipro_llm_accel_attach(m, &ref, scratch, scratch_len);
        free(scratch);
        if (err) {
            fprintf(stderr, "accel: %d\n", err);
            return 2;
        }
    }
    if (!strcmp(argv[2], "tok")) {
        return check_tok(m, argv[3]);
    }
    if (!strcmp(argv[2], "logits")) {
        return check_logits(m, argv[3]);
    }
    if (!strcmp(argv[2], "decode")) {
        return check_decode(m, argv[3], mode, int8);
    }
    if (!strcmp(argv[2], "run")) {
        ipro_llm_result_t res;
        const double t0 = now_s();
        const int r = ipro_llm_run(m, argv[3], &res);
        char json[1024] = "";
        ipro_llm_result_json(m, &res, json, sizeof json);
        const ipro_llm_prof_t *pf = ipro_llm_profile(m);
        printf("rc %d  %s  (%u prompt, %u generated tokens, %.2f ms; work %zu bytes; accelerator %u runs, %u back)\n",
               r, json, res.n_prompt, res.n_gen, 1e3 * (now_s() - t0), need, pf->accel_runs, pf->accel_fails);
        return r ? 1 : 0;
    }
    fprintf(stderr, "unknown command %s\n", argv[2]);
    return 2;
}
